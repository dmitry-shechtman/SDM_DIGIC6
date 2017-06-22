#include "platform.h"
#include "keyboard.h"
#include "conf.h"
#include "font.h"
#include "camera.h"
#include "raw.h"
#include "draw_palette.h"
#include "gui_osd.h"
#include "batt_grid.h"
#include "core.h"
#include "stdlib.h"
#include "script.h"

//-------------------------------------------------------------------

#define CONF_FILE  "A/SDM/SDM.CFG"
#define CONF_INFO(id, param, type, def, func) { id, sizeof( param ), &param , type, {def}, func }

//----------------- SDM Version number ------------------------------
#define CONF_MAGICK_VALUE (0x14024453)
char *sdmversion = "2.20";
//-------------------------------------------------------------------

typedef struct 
{
    unsigned short      id;
    unsigned short      size;
    void                *var;
    int                 type;
    union {
           void         *ptr;
           int             i;
           color          cl;
          };
    void           (*func)();
} ConfInfo;

//-------------------------------------------------------------------
Conf conf;

int state_shooting_progress = SHOOTING_PROGRESS_NONE;
int state_expos_recalculated;
int auto_started;
int shot_histogram_enabled;
int debug_propcase_show;
int debug_propcase_page;
int debug_vals_show;
extern unsigned char gScriptFolder;
//-------------------------------------------------------------------
static int def_ubasic_vars[SCRIPT_NUM_PARAMS] = {0};
static int def_ubasic_vars2[SCRIPT_NUM_PARAMS] = {0};
static int def_ubasic_vars3[SCRIPT_NUM_PARAMS] = {0};
static int def_batt_volts_max, def_batt_volts_min;
static OSD_pos def_dof_pos, def_batt_icon_pos, def_batt_txt_pos,def_zoom_pos,def_increment_pos, 
               def_mode_state_pos, def_values_pos, def_clock_pos, def_stereo_pos, def_synch_pos,def_overrides_pos,def_frame_pos;

// ********************************************************
static int def_cam_spacing;
static int def_deviation_factor;
static int def_min_distance_factor;
static int def_decade_factor;
static int def_distance_setting;
static int def_manual_far;
// ********************************************************
static void conf_change_script_file();
static void conf_change_font_cp();
static void conf_change_menu_rbf_file();
static void conf_change_grid_file();

static void conf_change_video_bitrate();

void ubasic_camera_set_raw(int mode)
{
    conf.save_raw = mode;
}

void ubasic_camera_set_nr(int mode)
{
	//"Auto", "Off", "On"
    conf.raw_nr = mode;
}

void ubasic_camera_set_tv_override(int mode)
{
    conf.tv_override = mode;
}

int ubasic_camera_get_tv_override()
{
 return conf.tv_override;
}


int ubasic_camera_script_autostart()
{
 //1 = Autostarted	
 return auto_started;
}

void ubasic_camera_set_script_autostart(int state)
{
 //1 = Autostarted	
 auto_started = state;
}

int ubasic_camera_get_nr()
{
  //"Auto", "Off", "On"
    return conf.raw_nr;
}

static const ConfInfo conf_info[] = {
    CONF_INFO(140, conf.heading1,               CONF_DEF_VALUE, i:0, NULL), // Zebra and Histogram
    CONF_INFO( 50, conf.serial_number,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(141, conf.heading2,               CONF_DEF_VALUE, i:0, NULL), // Advanced Menu\Video
    CONF_INFO( 222, conf.video_synch_device,    CONF_DEF_VALUE, i:2, NULL),
    CONF_INFO(218, conf.MovieFineTune,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 97, conf.video_mode,             CONF_DEF_VALUE, i:0, NULL), 
    CONF_INFO( 96, conf.video_bitrate,          CONF_DEF_VALUE, i:3, conf_change_video_bitrate),
    CONF_INFO( 98, conf.video_quality,          CONF_DEF_VALUE, i:84,NULL), 
    CONF_INFO(211, conf.video_frame_count,      CONF_DEF_VALUE, i:0,NULL), 
    CONF_INFO(122, conf.unlock_optical_zoom_for_video,  CONF_DEF_VALUE, i:0, NULL), 
    CONF_INFO(129, conf.mute_on_zoom,           CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(142, conf.heading3,               CONF_DEF_VALUE, i:0, NULL), // Raw
    CONF_INFO(  2, conf.save_raw,               CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(188, conf.blank_jpg,              CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 70, conf.raw_nr,                 CONF_DEF_VALUE, i:NOISE_REDUCTION_AUTO_CANON, NULL),
    CONF_INFO( 60, conf.raw_save_first_only,    CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(143, conf.heading4,               CONF_DEF_VALUE, i:0, NULL),  // Bracketing and Overrides
    CONF_INFO(134, conf.tv_override_enable,     CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 94, conf.tv_override,		      CONF_DEF_VALUE, i:19, NULL),
    CONF_INFO(108, conf.av_override_value,	   CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(228, conf.iso_override,           CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 81, conf.user_range_set,         CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(121, conf.nd_filter_state,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(139, conf.nd_constant_exposure,   CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(112, conf.dist_step_size,		CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(110, conf.subj_dist_bracket_value,CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(165, conf.save_auto,              CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(167, conf.dist_mode,              CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(109, conf.tv_bracket_value,	      CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(111, conf.bracket_type,		CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(223, conf.bracket_intershot_delay,CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(85, conf.burst_shutdown,		CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(133, conf.custom_timer_synch,     CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(156, conf.disable_overrides,      CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(144, conf.heading5,               CONF_DEF_VALUE, i:0, NULL), // Stereo Parameters Synch
    CONF_INFO(101, conf.synch_enable,           CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(217, conf.fastlapse,        	CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(102, conf.synch_delay_enable,     CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(104, conf.synch_delay_range,      CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(120, conf.synch_fine_delay,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(114, conf.synch_coarse_delay,CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(135, conf.add_synch_delays,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(145, conf.heading6,                CONF_DEF_VALUE, i:0, NULL), // Stereo Parameters General
    CONF_INFO(168, conf.compact,                 CONF_DEF_VALUE, i:0, NULL),
#if !defined(CAMERA_TX1)
    CONF_INFO(136, conf.remote_zoom_enable,      CONF_DEF_VALUE, i:0, NULL),
#else
    CONF_INFO(136, conf.remote_zoom_enable,      CONF_DEF_VALUE, i:1, NULL),
#endif
    CONF_INFO(137, conf.zoom_timeout,            CONF_DEF_VALUE, i:5, NULL),
    CONF_INFO(155, conf.extend_lens,             CONF_DEF_VALUE, i:0, NULL), // to make sure welcome will be displayed on first boot.
    CONF_INFO(125, conf.outline_mode,            CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(138, conf.disable_deviation_guideline,CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(195, conf.dc,                      CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(126, conf.stereo_mode,             CONF_DEF_VALUE, i:2, NULL),
    CONF_INFO( 27, conf.left_flash_dim,          CONF_DEF_VALUE,  i:0, NULL),// turn off flash
    CONF_INFO(229, conf.anaglyph_button,         CONF_DEF_VALUE, i:ANA_BUTTON, NULL),
    CONF_INFO(190, conf.edge_and_osd,            CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(128, conf.edge_overlay_thresh,     CONF_DEF_VALUE, i:40, NULL),  // name change
    CONF_INFO(174, conf.edge_overlay_color,      CONF_DEF_VALUE, cl:EDGE_OVERLAY_COLOUR, NULL),
    CONF_INFO(175, conf.edge_overlay_play,       CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(176, conf.edge_overlay_lock,       CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(177, conf.edge_overlay_zoom,       CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 87, conf.camera_position,        CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 88, conf.camera_orientation,     CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 89, conf.camera_rotation,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 194,conf.invert_playback,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 75, conf.stereo_spacing,         CONF_VALUE_PTR,   ptr:&def_cam_spacing, NULL),
    CONF_INFO( 76, conf.stereo_deviation,       CONF_VALUE_PTR,   ptr:&def_deviation_factor, NULL),
    CONF_INFO( 77, conf.minimum_dist_factor,    CONF_VALUE_PTR,   ptr:&def_min_distance_factor, NULL),
    CONF_INFO( 82, conf.distance_setting,       CONF_VALUE_PTR,   ptr:&def_distance_setting, NULL),
    CONF_INFO( 84, conf.focus_mode,             CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 79, conf.save_xml_file,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 83, conf.save_same_dir,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(146, conf.heading7,               CONF_DEF_VALUE, i:0, NULL), // Scripting
    CONF_INFO(  3, conf.script_shoot_delay,     CONF_DEF_VALUE, i:2, NULL),
    CONF_INFO( 99, conf.script_startup,         CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(80,  conf.heading19,              CONF_DEF_VALUE, i:0, NULL),  // Stereo Slit Camera
    CONF_INFO(107, conf.raw_strip_mode,         CONF_DEF_VALUE, i:0, NULL),  // for slit camera
    CONF_INFO(118, conf.strip_images,           CONF_DEF_VALUE, i:1, NULL),  // for slit camera
    CONF_INFO(173, conf.strip_width,            CONF_DEF_VALUE, i:0, NULL),  // for slit camera
    CONF_INFO(78,  conf.strip_offset,           CONF_DEF_VALUE, i:50, NULL), // for slit camera
    CONF_INFO(163, conf.eye_spacing,            CONF_DEF_VALUE, i:70, NULL),// for slit camera
    CONF_INFO(73,  conf.heading18,              CONF_DEF_VALUE, i:0, NULL),  // Experimental Sunrise script
    CONF_INFO(178, conf.sunrise,                CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(179, conf.sunrise_tv96,           CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(180, conf.sunrise_repeats,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(181, conf.sunrise_shots,          CONF_DEF_VALUE, i:4,NULL),
    CONF_INFO(183, conf.sunrise_kf_1,           CONF_DEF_VALUE, i:4,NULL),
    CONF_INFO(184, conf.sunrise_kf_2,           CONF_DEF_VALUE, i:4,NULL),
    CONF_INFO(185, conf.sunrise_kf_3,           CONF_DEF_VALUE, i:4,NULL),
    CONF_INFO(186, conf.sunrise_tv96_2,         CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(187, conf.sunrise_tv96_3,         CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(182, conf.srb,                    CONF_DEF_VALUE, i:0, NULL), 

    CONF_INFO(158, conf.heading16,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/Digiscoping
    CONF_INFO(159, conf.digi_obj_diam,		CONF_DEF_VALUE, i:62, NULL),
    CONF_INFO(160, conf.digi_scope_fl,		CONF_DEF_VALUE, i:352, NULL),
    CONF_INFO(161, conf.digi_ep_fl,		      CONF_DEF_VALUE, i:25, NULL),
    CONF_INFO(162, conf.digi_zoom_setting,      CONF_DEF_VALUE, i:4, NULL),
    CONF_INFO(164, conf.digi_ref_dist,          CONF_DEF_VALUE, i:500, NULL),
    CONF_INFO(166, conf.digi_step,              CONF_DEF_VALUE, i:2, NULL),  // index to step-size table for data entry
    CONF_INFO(191, conf.heading17,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/Serial Comms
    CONF_INFO(100, conf.remote_enable,          CONF_DEF_VALUE, i:1, NULL),  // moved from Scripting
    CONF_INFO(198, conf.use_af_led,			CONF_DEF_VALUE, i:1, NULL),  // use af led for serial comms
    CONF_INFO(192, conf.bw,                     CONF_DEF_VALUE, i:10, NULL), // unit bit width 
    CONF_INFO(193, conf.md,                     CONF_DEF_VALUE, i:22, NULL), // tweak factor for creating msec delays
    CONF_INFO(221, conf.precision_synch_flash,  CONF_DEF_VALUE, i:0, NULL),  // can use flash AND precision synch (if better than normal synch)
    CONF_INFO(212, conf.PulseOption,            CONF_DEF_VALUE, i:0, NULL),  // for SDM_GZ1, output pulse options
    CONF_INFO(214, conf.FastLapseDelay,         CONF_DEF_VALUE, i:0, NULL),  // for fast semi-synchronised continuous mode
    CONF_INFO(215, conf.FastLapseFineTune,      CONF_DEF_VALUE, i:29, NULL),
    CONF_INFO(216, conf.CountForOneMsec,        CONF_DEF_VALUE, i:100, NULL),
    CONF_INFO( 49, conf.SsTenMsecCount,         CONF_DEF_VALUE, i:9999, NULL),
    CONF_INFO(986, conf.burst_frame_count,      CONF_DEF_VALUE, i:0,NULL),	// **** '986' used to conform with SDM GZ1
    CONF_INFO(147, conf.heading8,               CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/OSD Parameters
    CONF_INFO( 14, conf.show_state,             CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 15, conf.show_values,            CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 58, conf.zoom_value,             CONF_DEF_VALUE, i:2, NULL),
    CONF_INFO(199, conf.zf_size,                CONF_DEF_VALUE, i:3, NULL),
    CONF_INFO(219, conf.zf_big_permanent,       CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(220, conf.zf_and_depth,           CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 41, conf.show_clock,             CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(148, conf.heading9,               CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/OSD Parameters/Battery Parameters
    CONF_INFO(  8, conf.batt_volts_max,         CONF_VALUE_PTR,   ptr:&def_batt_volts_max, NULL),
    CONF_INFO(  9, conf.batt_volts_min,         CONF_VALUE_PTR,   ptr:&def_batt_volts_min, NULL),
    CONF_INFO( 10, conf.batt_step_25,           CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(149, conf.heading10,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/OSD Parameters/Grid Lines
    CONF_INFO( 68, conf.show_grid_lines,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 196,conf.enable_yaw_guideline,   CONF_DEF_VALUE, i:0, NULL),  // vertical or diagonal line to show angular rotation for panoramas
    CONF_INFO( 197,conf.yaw,                    CONF_DEF_VALUE, i:3, NULL),  // yaw angle shown in degrees
    CONF_INFO( 69, conf.grid_lines_file,        CONF_CHAR_PTR,   ptr:"", conf_change_grid_file),
    CONF_INFO( 71, conf.grid_force_color,       CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 72, conf.grid_color,             CONF_DEF_VALUE, cl:MAKE_COLOR(COLOR_BG, COLOR_FG), NULL),
    CONF_INFO(150, conf.heading11,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/visual Settings
    CONF_INFO( 65, conf.font_cp,                CONF_DEF_VALUE, i:FONT_CP_WIN_1252, conf_change_font_cp),
   // CONF_INFO(  7, conf.shortcut_lang,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 28, conf.osd_color,              CONF_DEF_VALUE, cl:MAKE_COLOR(COLOR_BG, COLOR_FG), NULL),
    CONF_INFO( 29, conf.batt_icon_color,        CONF_DEF_VALUE, cl:COLOR_WHITE, NULL),
    CONF_INFO( 30, conf.menu_color,             CONF_DEF_VALUE, cl:MAKE_COLOR(COLOR_BG, COLOR_FG), NULL),
    CONF_INFO( 31, conf.reader_color,           CONF_DEF_VALUE, cl:MAKE_COLOR(COLOR_GREY, COLOR_WHITE), NULL),
    CONF_INFO( 46, conf.reader_codepage,        CONF_DEF_VALUE, i:FONT_CP_WIN, NULL),
    CONF_INFO(151, conf.heading12,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/Miscellaneous/Text File Reader
    CONF_INFO( 61, conf.reader_wrap_by_words,   CONF_DEF_VALUE, i:1, NULL),
 // CONF_INFO( 43, conf.reader_autoscroll,      CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 47, conf.splash_show,            CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 59, conf.use_zoom_mf,            CONF_DEF_VALUE, i:0, NULL),
#if defined(CAM_HAS_DISP_BUTTON)
    CONF_INFO( 63, conf.alt_mode_button,        CONF_DEF_VALUE, i:KEY_DISPLAY,NULL),
#else 

 #if !defined(CAMERA_g1x)
    CONF_INFO( 63, conf.alt_mode_button,        CONF_DEF_VALUE, i:KEY_PLAYBACK,NULL),
 #else
    CONF_INFO( 63, conf.alt_mode_button,        CONF_DEF_VALUE, i:KEY_PRINT,NULL),
 #endif 
#endif                                             
    CONF_INFO( 95, conf.click_time,             CONF_DEF_VALUE, i:3, NULL),
    CONF_INFO( 67, conf.alt_prevent_shutdown,   CONF_DEF_VALUE, i:ALT_PREVENT_SHUTDOWN_ALT,conf_update_prevent_shutdown),
    CONF_INFO( 48, conf.auto_rotation,		CONF_DEF_VALUE, i:0,NULL),
    CONF_INFO(202, conf.lockout_time,           CONF_DEF_VALUE, i:0,NULL),    
    CONF_INFO(21,  conf.hotshoe_override,       CONF_DEF_VALUE, i:0,NULL),
    CONF_INFO( 55, conf.alt_files_shortcut,     CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(119, conf.flash_exp_comp,         CONF_DEF_VALUE, i:9,NULL),
    CONF_INFO(203, conf.slave_flash,            CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(207, conf.bright_screen,          CONF_DEF_VALUE, i:0, NULL),
  //CONF_INFO(124, conf.disable_user_shortcut,  CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(152, conf.heading13,              CONF_DEF_VALUE, i:0, NULL),  // Advanced Menu/Debug Parameters
    CONF_INFO( 34, conf.ns_enable_memdump,      CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(153, conf.heading14,              CONF_DEF_VALUE, i:0, NULL),  // Layout positions
    CONF_INFO( 22, conf.dof_pos,                CONF_OSD_POS_PTR,   ptr:&def_dof_pos, NULL),
    CONF_INFO( 23, conf.batt_icon_pos,          CONF_OSD_POS_PTR,   ptr:&def_batt_icon_pos, NULL),
    CONF_INFO( 24, conf.batt_txt_pos,           CONF_OSD_POS_PTR,   ptr:&def_batt_txt_pos , NULL),
    CONF_INFO( 25, conf.mode_state_pos,         CONF_OSD_POS_PTR,   ptr:&def_mode_state_pos , NULL),
    CONF_INFO( 26, conf.values_pos,             CONF_OSD_POS_PTR,   ptr:&def_values_pos , NULL),
    CONF_INFO( 200, conf.zoom_pos,              CONF_OSD_POS_PTR,   ptr:&def_zoom_pos , NULL),
    CONF_INFO( 201, conf.increment_pos,         CONF_OSD_POS_PTR,   ptr:&def_increment_pos , NULL),
    CONF_INFO( 39, conf.reader_pos,             CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(115, conf.stereo_osd_pos,         CONF_OSD_POS_PTR,   ptr:&def_stereo_pos, NULL),
    CONF_INFO(74,  conf.frames_pos,             CONF_OSD_POS_PTR,   ptr:&def_frame_pos , NULL),
    CONF_INFO(116, conf.synch_pos,              CONF_OSD_POS_PTR,   ptr:&def_synch_pos, NULL),
    CONF_INFO( 42, conf.clock_pos,              CONF_OSD_POS_PTR,   ptr:&def_clock_pos , NULL),
    CONF_INFO(157, conf.overrides_pos,          CONF_OSD_POS_PTR,   ptr:&def_overrides_pos , NULL),
    CONF_INFO(209, conf.nearGuideX,             CONF_DEF_VALUE, i:160, NULL),
    CONF_INFO(210, conf.farGuideX,              CONF_DEF_VALUE, i:180, NULL),
    CONF_INFO(154, conf.heading15,              CONF_DEF_VALUE, i:0, NULL), // System parameters (restricted access)
#if defined(CAMERA_sx280hs)
    CONF_INFO(213, conf.platformid,             CONF_DEF_VALUE, i:12895, NULL),
#elif defined(CAMERA_ixus160_elph160)
    CONF_INFO(213, conf.platformid,             CONF_DEF_VALUE, i:12970, NULL),
#elif defined(CAMERA_m3)
    CONF_INFO(213, conf.platformid,             CONF_DEF_VALUE, i:12953, NULL),
#else
    CONF_INFO(213, conf.platformid,             CONF_DEF_VALUE, i:PLATFORMID, NULL),
#endif
    CONF_INFO(  1, conf.show_osd,               CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(227, conf.easy_script_file,       CONF_CHAR_PTR,   ptr:"A/SDM/SCRIPTS/3d_Mode.txt", NULL),
    CONF_INFO(  5, conf.ubasic_vars,            CONF_INT_PTR,   ptr:&def_ubasic_vars, NULL),
    CONF_INFO(  6, conf.script_file,            CONF_CHAR_PTR,   ptr:"", NULL),
    CONF_INFO(189, conf.script_param_set,       CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(226, conf.ubasic_vars2,           CONF_INT_PTR,   ptr:&def_ubasic_vars2, NULL),
    CONF_INFO(225, conf.ubasic_vars3,           CONF_INT_PTR,   ptr:&def_ubasic_vars3, NULL),
    CONF_INFO( 11, conf.batt_perc_show,         CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 12, conf.batt_volts_show,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 13, conf.batt_icon_show,         CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO( 51, conf.rangefinder_near,       CONF_DEF_VALUE, i:1500, NULL),
    CONF_INFO( 52, conf.rangefinder_far,        CONF_DEF_VALUE,  i:5000, NULL),
    CONF_INFO( 44, conf.stack_step_size,        CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 53, conf.near_for_infinity,      CONF_DEF_VALUE, i:1500, NULL),
    CONF_INFO( 54, conf.far_for_near,           CONF_DEF_VALUE,  i:5000, NULL),
    CONF_INFO( 38, conf.reader_file,            CONF_CHAR_PTR,   ptr:"", /*conf_change_script_file*/NULL),
    CONF_INFO( 45, conf.reader_rbf_file,        CONF_CHAR_PTR,   ptr:"", NULL),
    CONF_INFO( 64, conf.lang_file,              CONF_CHAR_PTR,   ptr:"", NULL),
    CONF_INFO( 66, conf.menu_rbf_file,          CONF_CHAR_PTR,   ptr:"", conf_change_menu_rbf_file),
    CONF_INFO( 86, conf.layout_mode,            CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 103, conf.synch_flash_delay,     CONF_DEF_VALUE, i:100, NULL),
    CONF_INFO( 105, conf.ptp_reboot,            CONF_DEF_VALUE, i:0, NULL),// 
    CONF_INFO( 106, conf.no_focus,              CONF_DEF_VALUE, i:0, NULL),// ******* SDM NG uses for fast non-synch shooting
    CONF_INFO(117, conf.on_release,             CONF_DEF_VALUE, i:1, NULL),// ******* SDM NG uses for synch shooting
    CONF_INFO( 20, conf.focus_override_method,  CONF_DEF_VALUE, i:2, NULL), // can focus-override in MF mode
    CONF_INFO(127, conf.save_viewport,          CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(131, conf.wait_forever,           CONF_DEF_VALUE, i:0, NULL),// ******* SDM NG uses for synch shooting
    CONF_INFO(132, conf.osd_page_index,         CONF_DEF_VALUE, i:2, NULL),
    CONF_INFO(123, conf.ricoh_ca1_mode,         CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(169, conf.cs,                     CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(170, conf.sdm_console,            CONF_DEF_VALUE, i:1, NULL),
    CONF_INFO(171, conf.num_lines,              CONF_DEF_VALUE, i:3, NULL),
    CONF_INFO(172, conf.line_length,            CONF_DEF_VALUE, i:25, NULL),
    CONF_INFO(204, conf.focus_pos,              CONF_DEF_VALUE, i:2000, NULL), //used for saving focus position, required by ubasic 'restore_focus'
    CONF_INFO(205, conf.zoom_point,             CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(206, conf.fpd,                    CONF_DEF_VALUE, i:50, NULL), // added to the saved focused distance by uBasic command 'restore_focus'
    CONF_INFO(208, conf.script_set,             CONF_DEF_VALUE, i:0, NULL), // ******* SDM NG uses new values from that number
    CONF_INFO( 17, conf.sleep,                  CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO( 16, conf.low_light,              CONF_DEF_VALUE, i:0, NULL),
  
    CONF_INFO(300, conf.user_1,                 CONF_DEF_VALUE, i:0, NULL), // User can set these with uBasic and will be saved in XML
    CONF_INFO(301, conf.user_2,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(302, conf.user_3,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(303, conf.user_4,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(308, conf.user_5,                 CONF_DEF_VALUE, i:0, NULL), // User can set these with uBasic and will be saved in XML
    CONF_INFO(309, conf.user_6,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(310, conf.user_7,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(311, conf.user_8,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(304, conf.flag_1,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(305, conf.flag_2,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(306, conf.flag_3,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(307, conf.flag_4,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(312, conf.flag_5,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(313, conf.flag_6,                 CONF_DEF_VALUE, i:0, NULL),// reserved for uBasic sync ticks
    CONF_INFO(314, conf.flag_7,                 CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(315, conf.flag_8,                 CONF_DEF_VALUE, i:0, NULL),// reserved for shutter opening time
    CONF_INFO( 18, conf.easy_pos,               CONF_DEF_VALUE, i:4, NULL),
    CONF_INFO( 19, conf.user_pos,               CONF_DEF_VALUE, i:0, NULL),
    CONF_INFO(  4, conf.hp_usb_pw,              CONF_DEF_VALUE, i:0, NULL),// pulse width measured using HP timer
    CONF_INFO(224, conf.user_script_file,       CONF_CHAR_PTR,   ptr:"", conf_change_script_file),
};


#define CONF_NUM (sizeof(conf_info)/sizeof(conf_info[0]))

static void conf_change_font_cp() 
{
    font_init();
    font_set(conf.font_cp);
}

static void conf_change_script_file() 
{
    gScriptFolder=2;
    script_load(conf.user_script_file,0);
}

static void conf_change_menu_rbf_file() 
{
 msleep(200);
}

static void conf_change_grid_file() 
{
    grid_lines_load(conf.grid_lines_file);
}

static void conf_change_video_bitrate() 
{
    shooting_video_bitrate_change(conf.video_bitrate);
}

 /* 
 CHDK CS 986 Reyalp	
 Update the prevent display off/prevent shutdown based on current state 
 doesn't really belong in conf but not clear where else it should go 
 */ 
 void conf_update_prevent_shutdown(void) 
{ 
 if(conf.alt_prevent_shutdown == ALT_PREVENT_SHUTDOWN_ALWAYS  
 || (conf.alt_prevent_shutdown == ALT_PREVENT_SHUTDOWN_ALT && gui_get_mode() != GUI_MODE_NONE) 
 || (conf.alt_prevent_shutdown == ALT_PREVENT_SHUTDOWN_ALT_SCRIPT && state_kbd_script_run)) 
 { 
  disable_shutdown(); 
 } 
 else 
 { 
  enable_shutdown(); 
 } 
} 
//-------------------------------------------------------------------
static void conf_init_defaults() 
{
    // init some defaults values
    def_batt_volts_max = get_vbatt_max();
    def_batt_volts_min = get_vbatt_min();
 
    def_frame_pos.x=240;
    def_frame_pos.y=190; 
    def_mode_state_pos.x=34;
    def_mode_state_pos.y=70; 
    def_values_pos.x=240;
    def_values_pos.y=100;    
    def_zoom_pos.x=170;
    def_zoom_pos.y=92;
    def_increment_pos.x=50;
    def_increment_pos.y=200;
    def_batt_icon_pos.x = 60;
    def_batt_icon_pos.y = 31;
    def_batt_txt_pos.x=94;
    def_batt_txt_pos.y=30; 
    def_clock_pos.y=258;    
    def_clock_pos.y=30;
    def_stereo_pos.x = 35;
    def_stereo_pos.y = 30;
    def_synch_pos.x = 110;
    def_synch_pos.y = 50;       
    def_overrides_pos.x = 0;
    def_overrides_pos.y = 70;  
    
    def_cam_spacing = 76;
    def_deviation_factor = 30;
    def_min_distance_factor = 15;
    def_distance_setting = 3000;  
    def_manual_far = 65000;
}
//-------------------------------------------------------------------
//			called by Reset option in Advanced Menu
//			and conf_restore()on booting
//-------------------------------------------------------------------
void conf_load_defaults() 
{
    register int i;

    for (i=0; i<CONF_NUM; ++i) 
    {
        switch (conf_info[i].type) 
        {
            case CONF_DEF_VALUE:
                memcpy(conf_info[i].var, &(conf_info[i].i), conf_info[i].size);
                break;
            default:
                memcpy(conf_info[i].var, conf_info[i].ptr, conf_info[i].size);
                break;
        }

        if (conf_info[i].func) 
        {
          conf_info[i].func();
        }
    } 
}

//-------------------------------------------------------------------
//			Called when entering or leaving ALT mode
//-------------------------------------------------------------------
void conf_save() 
{
    static const long t=CONF_MAGICK_VALUE;
    register int i;
    int fd;
    static char buf[sizeof(t)+CONF_NUM*(sizeof(conf_info[0].id)+sizeof(conf_info[0].size))+sizeof(conf)];
    char *p=buf;

    static char fn[17];
    if(((int)CAM_BITMAP_PALETTE)!=1)
    sprintf(fn,"A/SDM/SDM%d.CFG",(int)CAM_BITMAP_PALETTE);
    else sprintf(fn,"A/SDM/SDM.CFG");
    fd = open(fn, O_WRONLY|O_CREAT, 0777);
 
    if (fd>=0)
    {
        memcpy(p, &t, sizeof(t));
        p+=sizeof(t);
        for (i=0; i<CONF_NUM; ++i) 
        {
            memcpy(p, &(conf_info[i].id), sizeof(conf_info[i].id));
            p+=sizeof(conf_info[i].id);
            memcpy(p, &(conf_info[i].size), sizeof(conf_info[i].size));
            p+=sizeof(conf_info[i].size);
            memcpy(p, conf_info[i].var, conf_info[i].size);
            p+=conf_info[i].size;
        }

        write(fd, buf, p-buf);
        close(fd);
    }
}

//-------------------------------------------------------------------
//				Called by spytask() on booting
//  Defaults initialised and loaded and overwritten by cfg contents
//  if the file exists.
//-------------------------------------------------------------------
void conf_restore() 
{
    int fd, rcnt, i;
    long t;
    unsigned short id, size;
    void *ptr;
    conf_init_defaults();
    conf_load_defaults();
    static char fn[17];
    if(((int)CAM_BITMAP_PALETTE)!=1)
    sprintf(fn,"A/SDM/SDM%d.CFG",(int)CAM_BITMAP_PALETTE);
    else sprintf(fn,"A/SDM/SDM.CFG");
    fd = open(fn, O_RDONLY, 0777);
    if (fd>=0)
     {
        // read magick value
        rcnt = read(fd, &t, sizeof(t));
     
          if (rcnt==sizeof(t))
            {
            while (1) 
               {
                rcnt = read(fd, &id, sizeof(id));
                if (rcnt!=sizeof(id)) break;

                rcnt = read(fd, &size, sizeof(size));
                if (rcnt!=sizeof(size)) break;

                for (i=0; i<CONF_NUM; ++i) 
                    {
                    if (conf_info[i].id==id && conf_info[i].size==size) 
                       {
                        ptr=umalloc(size);
                        if (ptr) 
                         {
                            rcnt = read(fd, ptr, size);
                            if (rcnt == size) 
                              {
                               memcpy(conf_info[i].var, ptr, size);

                               if (conf_info[i].func) 
                                {
                                   conf_info[i].func();
                                }
                              }
                            ufree(ptr);
                         }
                        break;
                       }
                    }
                if (i == CONF_NUM) 
                 { //unknown id, just skip data
                  lseek(fd, size, SEEK_CUR);
                 }

              }// _while
           } // _rcnt
	close(fd);
    }// fd>=0 
}

//-------------------------------------------------------------------
int conf_getValue(unsigned short id, tConfigVal* configVal) {
    unsigned short i;
    int ret = CONF_EMPTY;
    OSD_pos* pos;
    
    if( id==0 ) {
        configVal->numb = 0;
// for all entries in conf_info[]
// Set configVal->numb to id of first item and return type as CONF_VALUE
        for( i=0; i<CONF_NUM; ++i ) if( configVal->numb<conf_info[i].id ) configVal->numb = conf_info[i].id;
        ret = CONF_VALUE;
    } else { 
                                                               // for each entry in conf_info[]
        for( i=0; i<CONF_NUM; ++i ) {
            if( conf_info[i].id==id ) {                                      // if matching id found
                switch( conf_info[i].type ) {
                    case CONF_DEF_VALUE:                                     // for CONF_DEF_VALUE and CONF_VALUE_PTR
                    case CONF_VALUE_PTR:
                        switch( conf_info[i].size ) {
                            case sizeof(int):                                // if integer
                                configVal->numb = *(int*)conf_info[i].var;   // configVal->numb = integer value
                                ret = CONF_VALUE;
                            break;
                            case sizeof(short):                              // if short
                                configVal->numb = *(short*)conf_info[i].var; // configVal->numb = short value
                                ret = CONF_VALUE;
                            break;
                            case sizeof(char):                               // if char
                                configVal->numb = *(char*)conf_info[i].var;  // configVal->numb = byte value
                                ret = CONF_VALUE;
                            break;
                        }
                    break;
                    case CONF_INT_PTR:                                       // if pointer to integer array
                        configVal->numb = conf_info[i].size/sizeof(int);     // return number of integer elements
                        configVal->pInt = (int*)conf_info[i].var;            // configVal->pInt = pointer to integer
                        ret = CONF_INT_PTR;
                    break;
                    case CONF_CHAR_PTR:                                      // if pointer to integer array
                        configVal->str = conf_info[i].var;                   // return character pointer
                        ret = CONF_CHAR_PTR;
                    break;
                    case CONF_OSD_POS_PTR:                                   // if pointer to pos structure
                        pos = (OSD_pos*)conf_info[i].var;
                        configVal->pos.x = pos->x;                           // return 'x'
                        configVal->pos.y = pos->y;                           // and 'y' values
                        ret = CONF_OSD_POS_PTR;
                    break;
                }
                break;
            }
        }
    }
    return ret;
}

//-------------------------------------------------------------------
int conf_setValue(unsigned short id, tConfigVal configVal) {
    unsigned short i;
    int ret = CONF_EMPTY, len, len2;
    OSD_pos* pos;
    for( i=0; i<CONF_NUM; ++i ) {
        if( conf_info[i].id==id ) {
            switch( conf_info[i].type ) {
                case CONF_DEF_VALUE:
                case CONF_VALUE_PTR:
                    if( configVal.isNumb ) {
                        switch( conf_info[i].size ) {
                            case sizeof(int):
                                *(int*)conf_info[i].var = (int)configVal.numb;
                                ret = CONF_VALUE;
                            break;
                            case sizeof(short):
                                *(short*)conf_info[i].var = (short)configVal.numb;
                                ret = CONF_VALUE;
                            break;
                            case sizeof(char):
                                *(char*)conf_info[i].var = (char)configVal.numb;
                                ret = CONF_VALUE;
                            break;
                        }
                    }
                break;
                case CONF_INT_PTR:
                    if( configVal.isPInt ) {
                        len = conf_info[i].size;
                        len2 = configVal.numb*sizeof(int);
                        if( len2<len ) len = len2;
                        memcpy(conf_info[i].var, configVal.pInt, len);
                        ret = CONF_INT_PTR;
                    }
                break;
                case CONF_CHAR_PTR:
                    if( configVal.isStr ) {
                        len = strlen(configVal.str);
                        if( len>0 && len<CONF_STR_LEN) {
                            strncpy(conf_info[i].var, configVal.str ,len+1);
                        }
                        ret = CONF_CHAR_PTR;
                    }
                break;
                case CONF_OSD_POS_PTR:
                    if( configVal.isPos ) {
                        pos = (OSD_pos*)conf_info[i].var;
                        pos->x = configVal.pos.x;
                        pos->y = configVal.pos.y;
                        ret = CONF_OSD_POS_PTR;
                    }
                break;
            }
            break;
        }
    }
    if( ret!=CONF_EMPTY ) {
        if (conf_info[i].func) {
            conf_info[i].func();
        }
        conf_save();
    }
    return ret;
}




