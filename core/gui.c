#include "stdlib.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "conf.h"
#include "camera.h"
#include "ubasic.h"
#include "font.h"
#include "lang.h"
#include "gui.h"
#include "gui_lang.h"
#include "draw_palette.h"
#include "gui_menu.h"
#include "mbox_popup.h"
#include "gui_read.h"
#include "gui_fselect.h"
#include "batt_grid.h"
#include "gui_osd.h"
#include "script.h"
#include "motion_detector.h"
#include "gui_sdm_config.h"
#include "raw.h"
#include "math.h"
 
extern char gridShortName[],normal_script[];
extern char *kaplogbuf,*sdmversion;
extern void SetCameraPosition(unsigned int); 
extern int tl_target_shot_count,movie_status,bls,gVideoStopTime,syncStart,gSv,Hdr3dTv96;
extern unsigned short gMenuPressed,gridFlag;
extern unsigned int burstCount;
extern int gZoomPoint,gFocalPoint;
extern long FastLapseStart,FastLapseEnd;
long temp1,temp2;
static unsigned char prevOrientation;
short int* pAFbuf=NULL;
extern void _MoveISLensToRightLimitPosition();
extern void _MoveISLensToLeftLimitPosition();
extern void _MoveISLensToTopLimitPosition();
extern void _MoveISLensToBottomLimitPosition();
extern void _MoveISLensToCentralPosition(); 
extern OSD_pos consoleOrigin; 
extern int _EngDrvRead(int gpio_reg);
extern unsigned char gStartHour,gEndHour,gStartMinutes,gEndMinutes,gRunNow,welcomeDone,exitWelcome,gMovieSyncAutostart,gSDM_will_use_flash;
const char* my_version_number = "1.86";
extern char EasyMode[];
extern const int ovr[66];
extern int gShotsRemaining,gui_menu_redraw,gTv96min,gTv96max,gTv,hdrDark96,hdrLight96,hdrND;
extern unsigned int gFPS;
extern unsigned char gAutoIsoDisabled,gEnableBigZoom,movSynchMode,gVideoReady,g8_16_forced,gHdrPhase,gTooBright,gTooDark,ZoomIn,gPrevStateVideoSynch;
extern onHP onHalfPress;
extern char cfg_name[100];
extern char cfg_set_name[100];
char* MX3 = "57";
char* FINGALO = "128";
char* ALLBEST = "16";
char *CS = "788";
#if defined (CAMERA_s95)
extern volatile short cdsgain;
#endif
extern int sw_delay,kbd_blocked;
extern int current_focus,pw;
extern int rf_focus;              						
static int digi_mode;
static long deltaTuning;
extern time_t gScriptStart;
extern int gNearPoint,gFarPoint,gStereoShift;
extern int PrevZoom,nTxzoom;
extern volatile enum _VideoMode VideoMode;
unsigned int calib,gSlitBytes;
int fpd,movSynchErr;
int sfo=0;                      								
int subject_far;                								
int gCurrentFocus;
static unsigned char farflag;           							
static int camshift,driftTime,LowestCurCnt=9999;
int LowestSynchCnt=9999,pulseCount;
int playback = 0; 										
int gDebug;
int gFrameCount,gFramesRecorded;
int gScriptMovie=0; 
int gSDMShootMode=STEREO;
unsigned char gScriptFolder=2,mini_started=0,startup_script=0;
unsigned char displayedMode,bootOrientation;    
int gRotationDistance=100;     
#if CAM_MULTIPART
int cp = 1;       										
#endif
//forward declarations  --------------------------------------------

int afl = 0;                                      					
int ael = 0;
const char* tl_modes[]={
                        "Single shot mode",
                        "Tv Continuous alternate",
                        "Tv timer ",
                        "Cont equal-step  ",
                        "Timer equal step ",
                        "Cont auto stack  ",
                        "Timer auto stack ",
                        "Continuous digi  ",
                        "Timer digiscope  ",
                        "Cont equal Tv    ",
                        "Cont auto Tv     ",
                        "Cont digi Tv     ",
                        "Timer equal Tv   ",
                        "Timer auto Tv    ",
                        "Timer digi Tv    ",
                       };

long myav;
static void gui_draw_osd();
extern int myhyp,previous_focus,inf_focus;
unsigned char syncQ,syncDone,calibDone,qual1,qual2,qual3;
unsigned long period1,period2,gPrevExposures;
unsigned char scriptExists = 1;
//Menu procs
//-------------------------------------------------------------------
static void gui_read_stacks(int arg); 
static void gui_read_xml(int arg); 
static void gui_draw_fselect(int arg);
static void gui_show_build_info(int arg);
static void gui_draw_osd_le(int arg);
static void gui_load_script(int arg);
static void load_easy_mode_script(int arg);
static void gui_draw_load_lang(int arg);
static void gui_menuproc_reset(int arg);
static void gui_grid_lines_load(int arg);
static const char* gui_font_enum(int change, int arg);
static const char* gui_raw_nr_enum(int change, int arg);
void gui_draw_read(int arg);
static void gui_draw_read_selected(const char *fn);
static void gui_draw_read_last(int arg);
static const char* gui_zoom_value_enum(int change, int arg);
static const char* gui_alt_power_enum(int change, int arg);
static const char* gui_video_synch_enum(int change, int arg);
static const char* gui_video_mode_enum(int change, int arg);
static const char* gui_video_bitrate_enum(int change, int arg);
static const char* gui_tv_enum(int change, int arg);
static const char* gui_nd_filter_state_enum(int change, int arg);
static const char* gui_av_enum(int change, int arg);
static const char* gui_bracket_type_enum(int change, int arg);
static const char* gui_tv_bracket_values_enum(int change, int arg);
static const char* gui_script_param_set_enum(int change, int arg);
static const char* gui_script_name_enum(int change, int arg);
static const char* gui_camera_orientation_enum(int change, int arg);
#if !(CAM_HAS_ZOOM_LEVER)
static const char* gui_range_enum(int change, int arg);
static const char* gui_dist_step_enum(int change, int arg);
static const char* gui_synch_step_enum(int change, int arg);
static const char* gui_digi_step_enum(int change, int arg);
static void cb_step_25();
#endif
static const char* gui_synch_zoom_enum(int change, int arg);
static const char* gui_focus_enum(int change, int arg);
static const char* gui_camera_position_enum(int change, int arg);
static const char* gui_stereo_playback_enum(int change, int arg);
static const char* gui_dist_mode_enum(int change, int arg);
static const char* gui_header_enum(int change, int arg); 
#if !defined(CAMERA_m3)
static const char* gui_yaw_enum(int change, int arg);
#endif
static const char* xml_save_enum(int change, int arg);
static const char* gui_synch_pulse_enum(int change, int arg);
static const char* slave_flash_enum(int change, int arg);
#if defined(PROPCASE_FLASH_EXP_COMP)
static const char* gui_flash_exp_comp_modes_enum(int change, int arg);
#endif
 
const char* gui_ana_button_enum(int change, int arg);
#ifdef CAM_HOTSHOE_OVERRIDE
static const char* gui_hotshoe_enum(int change, int arg);
#endif
#if CAM_MULTIPART
static void gui_menuproc_swap_partitions(int arg);
#endif

//Menu callbacks
//-------------------------------------------------------------------

int g360Images,gViewCircle;

void cb_slitcam_menu_change();

static void cb_volts();
static void cb_battery_menu_change(unsigned int item);
static void cb_stereo_menu_change();
static void cb_synch_menu_change();
static void cb_grid();
static void cb_bracketing_menu_change();
static void cb_digi_menu_change();
#if OPT_SUNSET
static void cb_experimental_menu_change();
#endif
static void cb_script_menu_change();
static void cb_flash_menu_change();
static void cb_serial_menu_change();
static void cb_video_menu_change();
void rinit();

//Menu definition
//-------------------------------------------------------------------

#if !(CAM_HAS_ZOOM_LEVER)
int digi_data_step;
static int delay_step = 10;
#endif

static CMenuItem misc_submenu_items[] = {
    {0,LANG_MENU_OSD_SAVE_XML,            MENUITEM_ENUM,(int*)xml_save_enum},
#if CAM_USE_ZOOM_FOR_MF
    {0,LANG_MENU_MISC_ZOOM_FOR_MF,        MENUITEM_BOOL,    &conf.use_zoom_mf },
#endif
    {0,LANG_MENU_MISC_CLICK_TIME,         MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.click_time, MENU_MINMAX(1,5)},
    {0,LANG_MENU_MISC_DISABLE_LCD_OFF,    MENUITEM_ENUM,    (int*)gui_alt_power_enum },
    {0,LANG_MENU_MISC_LOCKOUT_TIME,       MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.lockout_time, MENU_MINMAX(0, 255)},
 #ifdef CAM_HOTSHOE_OVERRIDE
    {0,LANG_MENU_HOTSHOE,                 MENUITEM_ENUM,    (int*)gui_hotshoe_enum },
#endif
    {0,LANG_MENU_ALT_MODE_SHORTCUT,       MENUITEM_BOOL,&conf.alt_files_shortcut},
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu misc_submenu = {0,0, LANG_MENU_MISC_TITLE, NULL, misc_submenu_items };

static CMenuItem debug_submenu_items[] = {
    {0,LANG_MENU_DEBUG_SHOW_PROPCASES,    MENUITEM_BOOL,          &debug_propcase_show },
    {0,LANG_MENU_DEBUG_PROPCASE_PAGE,     MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,   &debug_propcase_page, MENU_MINMAX(0, 128) },    
    {0,LANG_MENU_DEBUG_SHOW_MISC_VALS,    MENUITEM_BOOL,          &debug_vals_show },
    #if CAM_MULTIPART
    {0,LANG_MENU_DEBUG_SWAP_PART,         MENUITEM_PROC, 	    	(int*)gui_menuproc_swap_partitions },
    #endif
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu debug_submenu = {1,0, LANG_MENU_DEBUG_TITLE, NULL, debug_submenu_items };

static int voltage_step;

static CMenuItem battery_submenu_items[] = {
#if !(CAM_HAS_ZOOM_LEVER)
    {0,LANG_MENU_BATT_VOLT_MAX,           MENUITEM_INT|MENUITEM_ARG_ADDR_INC,     &conf.batt_volts_max,   (int)&voltage_step },
    {0,LANG_MENU_BATT_VOLT_MIN,           MENUITEM_INT|MENUITEM_ARG_ADDR_INC,     &conf.batt_volts_min,   (int)&voltage_step },
    {0,LANG_MENU_BATT_STEP_25,            MENUITEM_BOOL|MENUITEM_ARG_CALLBACK,    &conf.batt_step_25,     (int)cb_step_25 },
#else
    {0,LANG_MENU_BATT_VOLT_MAX,           MENUITEM_INT,     &conf.batt_volts_max },
    {0,LANG_MENU_BATT_VOLT_MIN,           MENUITEM_INT,     &conf.batt_volts_min },
#endif
    {0,(int)"",                           MENUITEM_SEPARATOR },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu battery_submenu = {2,0, LANG_MENU_BATT_TITLE, cb_battery_menu_change, battery_submenu_items };

static CMenuItem grid_submenu_items[] = {
    {0,LANG_MENU_SHOW_GRID,               MENUITEM_BOOL,		&conf.show_grid_lines },
    {0,LANG_MENU_GRID_LOAD,               MENUITEM_PROC,		(int*)gui_grid_lines_load },
#if !defined(CAMERA_m3)&& !defined(CAMERA_sx280hs) && !defined(CAMERA_g7x)
    {0,LANG_MENU_ENABLE_YAW_GUIDELINE,    MENUITEM_ENUM,          (int*)gui_yaw_enum},
    {0,LANG_MENU_YAW_DEGREES,             MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.yaw, MENU_MINMAX(1,30)},
#endif
    {0,LANG_MENU_GRID_CURRENT,            MENUITEM_SEPARATOR },
    {0,(int)grid_title,                   MENUITEM_TEXT },
    {0,(int)"",                           MENUITEM_SEPARATOR },
    {0,LANG_MENU_GRID_FORCE_COLOR,        MENUITEM_BOOL,          &conf.grid_force_color },
    {0,LANG_MENU_GRID_COLOR_LINE,         MENUITEM_COLOR_FG,      (int*)&conf.grid_color },
    {0,LANG_MENU_GRID_COLOR_FILL,         MENUITEM_COLOR_BG,      (int*)&conf.grid_color },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu grid_submenu = {4,0, LANG_MENU_GRID_TITLE,cb_grid, grid_submenu_items };

static int distance_step = 1;

static CMenuItem synch_submenu_items[] = {
    {0,LANG_MENU_SYNCH_STATUS,              MENUITEM_BOOL, &conf.synch_enable},
#if !(CAM_HAS_ZOOM_LEVER)
    {0,LANG_MENU_SYNC_STEP_SIZE ,           MENUITEM_ENUM,(int*)gui_synch_step_enum },
    {0,LANG_MENU_SYNCH_DELAY_VALUE,         MENUITEM_INT|MENUITEM_ARG_ADDR_INC,  &conf.synch_fine_delay,(int)&delay_step},
    {0,LANG_MENU_SYNCH_DELAY_COARSE_VALUE,  MENUITEM_INT|MENUITEM_ARG_ADDR_INC,  &conf.synch_coarse_delay,(int)&delay_step},
#else
    {0,LANG_MENU_SYNCH_DELAY_VALUE,         MENUITEM_INT,  &conf.synch_fine_delay},
    {0,LANG_MENU_SYNCH_DELAY_COARSE_VALUE,  MENUITEM_INT,  &conf.synch_coarse_delay},
#endif
    {0,LANG_MENU_ADD_SYNCH_DELAYS,          MENUITEM_BOOL, &conf.add_synch_delays}, 
    {0,LANG_MENU_TRIGGER,                  MENUITEM_ENUM,(int*)gui_synch_pulse_enum },    
    {0,LANG_MENU_BACK,                      MENUITEM_UP },   
    {0}
};
static CMenu synch_submenu = {5,0,LANG_MENU_SYNCH_TITLE,cb_synch_menu_change , synch_submenu_items };


static CMenuItem stereo_play_submenu_items[] = {
#if !defined(CAMERA_ixus100)&& !defined(CAMERA_ixus105)  
    {0,LANG_MENU_OSD_INVERT_PLAY,          MENUITEM_BOOL,&conf.invert_playback},
#endif  
#if !defined(CAMERA_ixus100)&& !defined(CAMERA_ixus70)
    {0,LANG_MENU_OSD_STEREO_PLAYBACK,      MENUITEM_ENUM,(int*)gui_stereo_playback_enum },
    {0,LANG_MENU_OSD_ANAGLYPH_BUTTON,      MENUITEM_ENUM,(int*)gui_ana_button_enum },
#endif 
    {0,LANG_MENU_BACK,                      MENUITEM_UP },   
    {0}
};
static CMenu stereo_play_submenu = {5,0,LANG_MENU_PLAYBACK_SUBMEN_TITLE,NULL, stereo_play_submenu_items };

static CMenuItem stereo_calc_submenu_items[] = {
    {0,LANG_MENU_OSD_CAM_SPACING,          MENUITEM_INT,  &conf.stereo_spacing},
    {0,LANG_MENU_NEAR_DISTANCE,            MENUITEM_INT,&conf.rangefinder_near},
    {0,LANG_MENU_FAR_DISTANCE,             MENUITEM_INT,&conf.rangefinder_far},   
    {0,LANG_MENU_OSD_DEV_FACTOR,           MENUITEM_INT|MENUITEM_F_MINMAX,  &conf.stereo_deviation,MENU_MINMAX(10,60)},    
    {0,LANG_MENU_BACK,                     MENUITEM_UP },   
    {0}
};
static CMenu stereo_calc_submenu = {5,0,LANG_MENU_CALC_SUBMEN_TITLE,NULL, stereo_calc_submenu_items };

static CMenuItem visual_submenu_items[] = {
    {0,LANG_MENU_VIS_LANG,                MENUITEM_PROC,      (int*)gui_draw_load_lang },
    {0,LANG_MENU_VIS_OSD_FONT,            MENUITEM_ENUM,      (int*)gui_font_enum },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu visual_submenu = {6,0, LANG_MENU_VIS_TITLE, NULL, visual_submenu_items };

static CMenuItem osd_submenu_items[] = {
    {0,LANG_MENU_COMPACT,                 MENUITEM_ENUM,(int*)gui_header_enum },
    {0,LANG_MENU_OSD_SHOW,                MENUITEM_BOOL,        &conf.show_osd },
    {0,LANG_MENU_OSD_SHOW_STATES,         MENUITEM_BOOL,        &conf.show_state },
    {0,LANG_MENU_OSD_SHOW_MISC_VALUES,    MENUITEM_BOOL,      &conf.show_values },
#if !defined(CAMERA_m3)
    {0,LANG_MENU_OSD_ZOOM_VALUE,          MENUITEM_ENUM,      (int*)gui_zoom_value_enum },
    {0,LANG_MENU_OSD_ZOOM_FONT_SIZE,      MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,&conf.zf_size,MENU_MINMAX(0,16)},
    {0,LANG_MENU_OSD_ZOOM_PERMANENT,      MENUITEM_BOOL,        &conf.zf_big_permanent },
    {0,LANG_MENU_OSD_ZOOM_AND_DEPTH,       MENUITEM_BOOL,        &conf.zf_and_depth },
#endif
    {0,LANG_MENU_OSD_SHOW_CLOCK,          MENUITEM_BOOL,      &conf.show_clock },
    {0,LANG_MENU_OSD_LAYOUT_EDITOR,       MENUITEM_PROC,      (int*)gui_draw_osd_le },
    {0,LANG_MENU_OSD_BATT_PARAMS,         MENUITEM_SUBMENU,   (int*)&battery_submenu },
    {0,LANG_MENU_OSD_GRID_PARAMS,         MENUITEM_SUBMENU,   (int*)&grid_submenu },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu osd_submenu = {9,0, LANG_MENU_OSD_TITLE,NULL, osd_submenu_items };

#if !(CAM_HAS_ZOOM_LEVER)
static int dist_bracket_step = 10;
#endif

static CMenuItem override_submenu_items[] = 
{
    {0,LANG_MENU_RAW_SHUTTER_OVERRIDE ,   MENUITEM_BOOL,     &conf.tv_override_enable}, 
    {0,LANG_MENU_SHUTTER_VALUE,           MENUITEM_ENUM,    (int*)gui_tv_enum},
#if CAM_HAS_IRIS_DIAPHRAGM
    {0,LANG_MENU_RAW_AV_OVERRIDE,         MENUITEM_ENUM,    (int*)gui_av_enum },
#endif 
    {0,LANG_MENU_ISO_OVERRIDE,            MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.iso_override, MENU_MINMAX(0,10000)},
#if defined(CAM_HAS_ND_FILTER) && !defined(CAMERA_m3)
    {0,LANG_MENU_OVERRIDE_ND_FILTER,      MENUITEM_ENUM,    (int*)gui_nd_filter_state_enum },
#if !defined(CAM_HAS_IRIS_DIAPHRAGM)
    {0,LANG_MENU_ND_CONSTANT_EXPOSURE,    MENUITEM_BOOL,    &conf.nd_constant_exposure},
#endif
#endif 
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu override_submenu = {11,0, LANG_MENU_OVERRIDES,cb_bracketing_menu_change, override_submenu_items };


static CMenuItem bracket_submenu_items[] = 
{
    {0,LANG_MENU_DIST_MODE,               MENUITEM_ENUM,    (int*)gui_dist_mode_enum },
#if !defined(CAMERA_s100)
    {0,LANG_MENU_DIGI_SAVE,               MENUITEM_BOOL,    &conf.save_auto},
#endif
    {0,LANG_MENU_TV_BRACKET_VALUE,        MENUITEM_ENUM,    (int*)gui_tv_bracket_values_enum },
    {0,LANG_MENU_BRACKET_TYPE,            MENUITEM_ENUM,    (int*)gui_bracket_type_enum },
    {0,LANG_MENU_BRACKET_INTERSHOT_DELAY, MENUITEM_INT|MENUITEM_F_MINMAX,&conf.bracket_intershot_delay,MENU_MINMAX(0,50)},
    {0,LANG_MENU_BURST_SHUTDOWN,          MENUITEM_BOOL,    &conf.burst_shutdown},
    {0,LANG_MENU_TIMER_SYNCH ,            MENUITEM_BOOL,     &conf.custom_timer_synch},
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu bracket_submenu = {21,0, LANG_MENU_BRACKET,cb_bracketing_menu_change, bracket_submenu_items };

static CMenuItem fastlapse_submenu_items[] = 
{
    {0,(int)"Use USB switch !",           MENUITEM_SEPARATOR },
    {0,(int)"",                           MENUITEM_SEPARATOR },
    {0,LANG_MENU_FASTLAPSE_ENABLE,        MENUITEM_BOOL,    &conf.fastlapse},
    {0,LANG_MENU_FAST_LAPSE_DELAY,        MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.FastLapseDelay, MENU_MINMAX(0,65000) },
    {0,LANG_MENU_BURST_FRAME_COUNT,       MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.burst_frame_count,MENU_MINMAX(2,5000) },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu fastlapse_submenu = {22,0, LANG_MENU_OSD_FASTLAPSE,cb_bracketing_menu_change, fastlapse_submenu_items };

static CMenuItem slitcam_submenu_items[] = 
{
    {0,LANG_MENU_RAW_STRIP_MODE,          MENUITEM_BOOL,      &conf.raw_strip_mode },
    {0,LANG_MENU_RAW_STRIP_WIDTH,         MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.strip_width, MENU_MINMAX(1,1000)}, 
    {0,LANG_MENU_RAW_STRIP_OFFSET,        MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.strip_offset, MENU_MINMAX(0,100)},
    {0,LANG_MENU_RAW_STRIP_IMAGES,        MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.strip_images, MENU_MINMAX(1,65000)},
    {0,LANG_MENU_RAW_STRIP_VIEWING,       MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.eye_spacing, MENU_MINMAX(10,10000)},
    {0,LANG_MENU_BACK,                    MENUITEM_UP },    
    {0}
};
static CMenu slitcam_submenu = {19,0, LANG_MENU_SLITCAM_TITLE,cb_slitcam_menu_change, slitcam_submenu_items };


static CMenuItem digiscope_submenu_items[] = {
    {0,LANG_MENU_DIGI_OD,                 MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.digi_obj_diam, MENU_MINMAX(40,150)},
    {0,LANG_MENU_DIGI_FL,                 MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.digi_scope_fl, MENU_MINMAX(100,800)},
    {0,LANG_MENU_DIGI_EP_FL ,             MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.digi_ep_fl, MENU_MINMAX(3,100)},
    {0,LANG_MENU_DIGI_ZOOM_SETTING,       MENUITEM_INT|MENUITEM_F_UNSIGNED,&conf.digi_zoom_setting},
    {0,LANG_MENU_DIGI_REF_DIST,           MENUITEM_INT,  &conf.digi_ref_dist},   
    {0,LANG_MENU_BACK,                    MENUITEM_UP },    
    {0}
};
static CMenu digiscope_submenu = {7,0, LANG_MENU_DIGI_TITLE,cb_digi_menu_change, digiscope_submenu_items };

static CMenuItem creative_submenu_items[] = 
{
  {0,LANG_MENU_OVERRIDES,              	MENUITEM_SUBMENU,   (int*)&override_submenu},
  {0,LANG_MENU_BRACKET,                  	MENUITEM_SUBMENU,   (int*)&bracket_submenu}, 
  {0,LANG_MENU_FASTLAPSE,          		MENUITEM_SUBMENU,   (int*)&fastlapse_submenu}, 
  {0,LANG_MENU_MAIN_SLITCAM,              MENUITEM_SUBMENU,   (int*)&slitcam_submenu },
  {0,LANG_MENU_MAIN_DIGISCOPE,            MENUITEM_SUBMENU,   (int*)&digiscope_submenu },
  {0,LANG_MENU_BACK,                    	MENUITEM_UP },
  {0}
};
static CMenu creative_submenu = {23,0, LANG_MENU_CREATIVE,cb_bracketing_menu_change, creative_submenu_items };


static CMenuItem video_submenu_items[] = {
    {0,LANG_MENU_VIDEO_SYNCH,             MENUITEM_ENUM,    (int*)gui_video_synch_enum},
    {0,LANG_MENU_MOVIE_FINE_TUNE,         MENUITEM_INT,      &conf.MovieFineTune},
#if CAM_CHDK_HAS_EXT_VIDEO_MENU
    {0,LANG_MENU_RAW_VIDEO_MODE,          MENUITEM_ENUM,    (int*)gui_video_mode_enum}, 
    {0,LANG_MENU_RAW_VIDEO_BITRATE,       MENUITEM_ENUM,    (int*)gui_video_bitrate_enum}, 
    {0,LANG_MENU_RAW_VIDEO_QUALITY,       MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.video_quality, MENU_MINMAX(1, 99)},
#endif
    {0,LANG_MENU_VIDEO_FRAME_COUNT,       MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX,  &conf.video_frame_count, MENU_MINMAX(0,60000)},
#if CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO 
    {0,LANG_MENU_OPTICAL_ZOOM_IN_VIDEO,   MENUITEM_BOOL,      &conf.unlock_optical_zoom_for_video},                                                  
#endif 
#if CAM_CAN_MUTE_MICROPHONE 
    {0,LANG_MENU_MUTE_ON_ZOOM,            MENUITEM_BOOL,  &conf.mute_on_zoom}, 
#endif 
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu video_submenu = {12,0, LANG_MENU_VIDEO, cb_video_menu_change, video_submenu_items };

static CMenuItem raw_submenu_items[] = {
    {0,LANG_MENU_RAW_SAVE,                MENUITEM_BOOL,      &conf.save_raw },
    {0,LANG_MENU_RAW_BLANK_JPG,           MENUITEM_BOOL,      &conf.blank_jpg },
    {0,LANG_MENU_RAW_NOISE_REDUCTION,     MENUITEM_ENUM,      (int*)gui_raw_nr_enum },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
 
static CMenu raw_submenu = {13,0, LANG_MENU_RAW, NULL, raw_submenu_items };
 

static CMenuItem image_video_submenu_items[] = {

    {0,LANG_MENU_VIDEO,                    MENUITEM_SUBMENU,   (int*)&video_submenu },
    {0,LANG_MENU_RAW,                      MENUITEM_SUBMENU,   (int*)&raw_submenu },
    {0,LANG_MENU_BACK,                     MENUITEM_UP },
    {0}
};
static CMenu image_video_submenu = {14,0, LANG_MENU_IMAGE_VIDEO, NULL, image_video_submenu_items };

static int synch_pulse;
static CMenuItem stereo_submenu_items[] = { 
#if !defined(CAMERA_m3)
    {0,LANG_MENU_ZOOM_ENABLE,              MENUITEM_ENUM,(int*)gui_synch_zoom_enum },
    {0,LANG_MENU_ZOOM_TIMEOUT,             MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.zoom_timeout,MENU_MINMAX(2,10)},
    {0,LANG_MENU_EXTEND_LENS,              MENUITEM_BOOL, &conf.extend_lens},
#endif
    {0,LANG_MENU_DOUBLE_CLICK,             MENUITEM_BOOL, &conf.dc},
    {0,LANG_MENU_OSD_CAM_POSITION,         MENUITEM_ENUM,(int*)gui_camera_position_enum }, 
#if !defined(CAMERA_tx1)
    {0,LANG_MENU_OSD_CAM_ORIENTATION,      MENUITEM_ENUM,(int*)gui_camera_orientation_enum },
#endif
    {0,LANG_MENU_PLAYBACK,                 MENUITEM_SUBMENU,(int*)&stereo_play_submenu },    
#if CAM_SYNCH
    {0,LANG_MENU_OSD_SYNCH,                MENUITEM_SUBMENU,(int*)&synch_submenu },
#endif    
    {0,LANG_MENU_CALC,                     MENUITEM_SUBMENU,(int*)&stereo_calc_submenu },     
#if !defined (CAMERA_ixus700) 
#if !(CAM_HAS_ZOOM_LEVER)
    {0,LANG_MENU_OSD_VALUE_STEP_SIZE,       MENUITEM_ENUM,(int*)gui_range_enum },
    {0,LANG_MENU_OSD_DISTANCE,             MENUITEM_INT|MENUITEM_ARG_ADDR_INC,  &conf.distance_setting,(int)&distance_step},
#endif
#endif
    {0,LANG_MENU_BACK,                     MENUITEM_UP },
    {0}
};

static CMenu stereo_submenu = {15,0,LANG_MENU_STEREO_TITLE, cb_stereo_menu_change, stereo_submenu_items };

static CMenuItem flash_submenu_items[] = 
{
    {0,LANG_MENU_LEFT_FLASH,               MENUITEM_BOOL, &conf.left_flash_dim},
#if defined(CAM_PRECISION_SYNCH)    
    {0,LANG_MENU_PRECISION_SYNCH_FLASH,     MENUITEM_BOOL, &conf.precision_synch_flash}, 
#endif 
    {0,LANG_MENU_MISC_SLAVE_FLASH,        MENUITEM_ENUM,(int*)slave_flash_enum},  
#if defined(PROPCASE_FLASH_EXP_COMP)    
    {0,LANG_MENU_EXPOSURE_COMP,           MENUITEM_ENUM,(int*)gui_flash_exp_comp_modes_enum}, 
#endif
    {0,LANG_MENU_BRIGHT_SCREEN,           MENUITEM_BOOL, &conf.bright_screen },
    {0,LANG_MENU_BACK,                     MENUITEM_UP },
    {0}
};

static CMenu flash_submenu = {15,0,LANG_MENU_FLASH, cb_flash_menu_change, flash_submenu_items };
static CMenuItem script_submenu_items_bottom[] = 
{
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenuItem script2_submenu_items_top[] = 
{
    {0,LANG_STR_SELECT_SCRIPT_FILE,      MENUITEM_ENUM,         (int*)gui_script_name_enum},
    {0,LANG_MENU_PARAM_SET,              MENUITEM_ENUM,         (int*)gui_script_param_set_enum},
    {0,(int)paramdesc[0],                MENUITEM_SEPARATOR },                      
    {0,(int)paramdesc[1],                MENUITEM_SEPARATOR },
    {0,(int)paramdesc[2],                MENUITEM_SEPARATOR },
    {0,(int)paramdesc[3],                MENUITEM_SEPARATOR },
    {0,(int)paramdesc[4],                MENUITEM_SEPARATOR },
    {0,(int)paramdesc[5],                MENUITEM_SEPARATOR },
    {0,(int)paramdesc[6],                MENUITEM_SEPARATOR },

};

static CMenuItem script2_submenu_items[sizeof(script2_submenu_items_top)/sizeof(script2_submenu_items_top[0])+SCRIPT_NUM_PARAMS+
                               sizeof(script_submenu_items_bottom)/sizeof(script_submenu_items_bottom[0])];
CMenu script2_submenu = {21,0, LANG_MENU_SCRIPT2_SUB_TITLE,cb_script_menu_change, script2_submenu_items };
static int TOP2Size = sizeof(script2_submenu_items_top)/sizeof(script2_submenu_items_top[0]);
static CMenuItem script3_submenu_items_top[] = 
{
    {0,LANG_MENU_SCRIPT_LOAD,            MENUITEM_PROC,                      (int*)gui_load_script},
    {0,LANG_MENU_SCRIPT_AUTOSTART,	     MENUITEM_BOOL,                       &conf.script_startup },
    {0,(int)script_title,                MENUITEM_SEPARATOR },
};

static CMenuItem script3_submenu_items[sizeof(script3_submenu_items_top)/sizeof(script3_submenu_items_top[0])+SCRIPT_NUM_PARAMS+
                               sizeof(script_submenu_items_bottom)/sizeof(script_submenu_items_bottom[0])];
CMenu script3_submenu = {21,0, LANG_MENU_SCRIPT3_TITLE,cb_script_menu_change, script3_submenu_items };
static CMenuItem reader_submenu_items[] = {
    {0,LANG_MENU_READ_OPEN_NEW,           MENUITEM_PROC,    (int*)gui_draw_read},
    {0,LANG_MENU_READ_OPEN_LAST,          MENUITEM_PROC,    (int*)gui_draw_read_last },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },    
    {0}
};
static CMenu reader_submenu = {17,0, LANG_MENU_READ_TITLE,NULL, reader_submenu_items };

static CMenuItem sc_submenu_items[] = 
{
#if CAM_REMOTE
    {0,LANG_MENU_SCRIPT_REMOTE_ENABLE,   MENUITEM_BOOL, &conf.remote_enable},
#endif
    {0,LANG_MENU_USE_AF_LED,              MENUITEM_BOOL, &conf.use_af_led},
    {0,LANG_MENU_BIT_WIDTH,               MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.bw, MENU_MINMAX(2, 300) },
    {0,LANG_MENU_MSEC,                    MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.md, MENU_MINMAX(7,100) },
    {0,LANG_MENU_FAST_FINE_TUNE,          MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.FastLapseFineTune, MENU_MINMAX(0,1000) },
    {0,LANG_MENU_FAST_ONE_MSEC_COUNT,     MENUITEM_INT|MENUITEM_F_UNSIGNED|MENUITEM_F_MINMAX, &conf.CountForOneMsec, MENU_MINMAX(100,500) },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },    
    {0}
};
static CMenu sc_submenu = {18,0, LANG_MENU_SC_TITLE,cb_serial_menu_change, sc_submenu_items };

static CMenuItem root_menu_items[] = {
    {0,LANG_MENU_SCRIPT2_SUB_TITLE,           MENUITEM_SUBMENU,   (int*)&script2_submenu },
    {0,LANG_MENU_SCRIPT3_TITLE,           MENUITEM_SUBMENU,   (int*)&script3_submenu },
    {0,LANG_MENU_CREATIVE,                MENUITEM_SUBMENU,   (int*)&creative_submenu},
    {0,LANG_MENU_OSD_STEREO_PARAM,        MENUITEM_SUBMENU,   (int*)&stereo_submenu}, 
    {0,LANG_MENU_FLASH,                   MENUITEM_SUBMENU,   (int*)&flash_submenu},    
    {0,LANG_MENU_IMAGE_VIDEO ,            MENUITEM_SUBMENU,   (int*)&image_video_submenu},
    {0,LANG_MENU_MAIN_SERIAL_COM,         MENUITEM_SUBMENU,   (int*)&sc_submenu },
    {0,LANG_MENU_MISC_TEXT_READER,        MENUITEM_SUBMENU,   (int*)&reader_submenu },
    {0,LANG_MENU_MISC_FILE_BROWSER,       MENUITEM_PROC,      (int*)gui_draw_fselect },
    {0,LANG_MENU_MAIN_OSD_PARAM,          MENUITEM_SUBMENU,   (int*)&osd_submenu },
    {0,LANG_MENU_MAIN_VISUAL_PARAM,       MENUITEM_SUBMENU,   (int*)&visual_submenu },
    {0,LANG_MENU_MAIN_MISC,               MENUITEM_SUBMENU,   (int*)&misc_submenu },
    {0,LANG_MENU_MAIN_DEBUG,              MENUITEM_SUBMENU,   (int*)&debug_submenu },
    {0,LANG_MENU_MAIN_RESET_OPTIONS,      MENUITEM_PROC,      (int*)gui_menuproc_reset },
    {0,LANG_MENU_BACK,                    MENUITEM_UP },
    {0}
};
static CMenu root_menu = {20,0, LANG_MENU_MAIN_TITLE, NULL, root_menu_items };


static CMenuItem user_submenu_items[] = 
{
  {0,LANG_MENU_MAIN_SCRIPT_PARAM,         MENUITEM_PROC,   (int*)&load_easy_mode_script}, 
  {0,LANG_MENU_MAIN_TITLE,                MENUITEM_SUBMENU,      (int*)&root_menu},
  {0}
};

static CMenu user_submenu = {20,0, LANG_MENU_BASIC_TITLE, NULL, user_submenu_items };

//-------------------------------------------------------------------
void cb_script_menu_change()
{
 if(conf.script_startup)auto_started = 1; 
}
//-------------------------------------------------------------------
void cb_serial_menu_change()
{
 if(conf.burst_frame_count)conf.dist_mode=0;
}

//-------------------------------------------------------------------
void cb_flash_menu_change()
{
 static int slave;
 if(conf.slave_flash){slave=1;conf.bright_screen=0; }
 else if(!conf.slave_flash)
  {
   slave=0;
   shooting_set_prop(PROPCASE_FLASH_ADJUST_MODE,slave); 
  } 
 if(conf.bright_screen)conf.slave_flash=0;
}

//-------------------------------------------------------------------

void cb_slitcam_menu_change()
{
 if(conf.strip_offset)
  {
 int r;
 int zp=gZoomPoint;     
 r = (720 * CAM_JPEG_HEIGHT)/(get_fov(zp)*conf.strip_images);  
 if(r>(CAM_JPEG_HEIGHT/2))                 
  {
   r=CAM_JPEG_HEIGHT/2;
   conf.strip_images=1440/get_fov(zp);     
   play_sound(6);
  }  
 conf.strip_width=r; 

unsigned int x,Den1,Den2,w;
w = 3*get_sensor_width()/16; 
Den1=w*w/10000;                              
x = gFocalPoint; 
Den2=x*x/10000;                              
x = sqrt(Den1+Den2);                         
                                             
 gRotationDistance=(conf.eye_spacing/2*x*100)/w;
 }
}
//-------------------------------------------------------------------

void cb_grid()
{
#if !defined(CAMERA_m3)&& !defined(CAMERA_SX280hs) && !defined(CAMERA_g7x)
 if(conf.enable_yaw_guideline) conf.show_grid_lines=0;
 else if (conf.show_grid_lines)conf.enable_yaw_guideline=0;
#endif
}

#if !(CAM_HAS_ZOOM_LEVER)
void cb_step_25() {
    voltage_step = (conf.batt_step_25)?25:1;
}
#endif

void cb_battery_menu_change(unsigned int item) {
    switch (item) {
        case 0: 
            if (conf.batt_volts_max<conf.batt_volts_min+25) {
                conf.batt_volts_min = conf.batt_volts_max-25;
            }
            break;
        case 1: 
            if (conf.batt_volts_min>conf.batt_volts_max-25) {
                conf.batt_volts_max = conf.batt_volts_min+25;
            }
            break;
        default:
            break;
    }
}

void cb_bracketing_menu_change()
{
 if(conf.fastlapse)conf.save_auto = 0; 
if(conf.subj_dist_bracket_value<1)conf.subj_dist_bracket_value=1;
if(conf.dist_mode==2)conf.bracket_type=3; 

  if(conf.dist_mode)
   {
#if OPT_SUNSET
    conf.sunrise=0;
#endif
    conf.tv_bracket_value=0;
    conf.burst_frame_count=0;					
   }
  else if(conf.tv_bracket_value)
   {
#if OPT_SUNSET
    conf.sunrise=0;
#endif
    conf.dist_mode=0;
   }
#if CAM_HAS_ND_FILTER
  if(!conf.tv_override_enable && !conf.av_override_value && !conf.user_range_set && !conf.nd_filter_state)conf.disable_overrides = 0;
#else
  if(!conf.tv_override_enable && !conf.av_override_value && !conf.user_range_set)conf.disable_overrides = 0;
#endif
}

#if OPT_SUNSET
 void cb_experimental_menu_change()
 {
  if(conf.sunrise)
  {
    conf.dist_mode=0;
    conf.tv_bracket_value=0;
  }
  if(conf.sunrise_kf_1>conf.sunrise_shots) conf.sunrise_kf_1 = conf.sunrise_shots;
  if(conf.sunrise_kf_2<conf.sunrise_kf_1) conf.sunrise_kf_2 = conf.sunrise_kf_1;
  if(conf.sunrise_kf_3<conf.sunrise_kf_2) conf.sunrise_kf_3 = conf.sunrise_kf_2;
 }
#endif
void cb_digi_menu_change()
{
 if(conf.digi_zoom_setting<0) conf.digi_zoom_setting = 0;
 if(conf.digi_zoom_setting>=zoom_points) conf.digi_zoom_setting = zoom_points-1;
}
void cb_synch_menu_change()
{
if(conf.synch_enable < 0 || conf.synch_enable > 1) conf.synch_enable = 0;
if(conf.synch_enable) conf.remote_enable = 1;
if(conf.synch_delay_enable < 0 || conf.synch_delay_enable > 1) conf.synch_delay_enable = 0;
if(conf.synch_fine_delay < 0) conf.synch_fine_delay = 0;
if(conf.synch_fine_delay>999) conf.synch_fine_delay = 999;
}
void cb_video_menu_change()
{
 if(!conf.video_frame_count)gFrameCount=0;
 else gFrameCount=conf.video_frame_count;
}
void cb_stereo_menu_change()
{ 
if(conf.stereo_spacing < 1) conf.stereo_spacing = 1;
if(conf.stereo_deviation <10) conf.stereo_deviation = 10;
if(conf.stereo_deviation >100) conf.stereo_deviation = 100;
if(conf.minimum_dist_factor <15) conf.minimum_dist_factor = 15;
if(conf.distance_setting < 10) conf.distance_setting = 10;
if(conf.distance_setting > 65535) conf.distance_setting = 65535;
if(conf.invert_playback) conf.stereo_mode=0;
if(conf.stereo_mode) conf.invert_playback=0;
#if defined(CAMERA_ixus870)
if(conf.remote_zoom_enable==2)conf.remote_zoom_enable=1;  
#endif
 if(!synch_pulse)conf.wait_forever=0;
 if(synch_pulse==1){conf.wait_forever=1;conf.on_release=0;};
 if(synch_pulse==2){conf.wait_forever=1;conf.on_release=1;};
}

static void gui_enum_value_change(int *value, int change, unsigned num_items) 
{ 
 *value+=change; 
 if (*value<0) 
  *value = num_items-1; 
 else if (*value>=num_items) 
  *value = 0; 
}
 
static const char* gui_change_simple_enum(int* value, int change, const char** items, unsigned num_items) 
{ 
 gui_enum_value_change(value, change, num_items); 
 return items[*value]; 
} 

unsigned char paramSetChanged;
const char* gui_script_param_set_enum(int change, int arg) 
{
 static const char *null = "     ";
 if(scriptExists)
  {
    static const char* modes[]={ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24","25"};
    int i;

    if (change != 0)                                                         
     {
      paramSetChanged=1;
      for(i=0;i<7;i++)paramdesc[i][0]='\0';                                  
        conf.script_param_set += change;                                     
        if (conf.script_param_set < 1)                                      conf.script_param_set = (sizeof(modes)/sizeof(modes[0]))-1;
        else if (conf.script_param_set >= (sizeof(modes)/sizeof(modes[0]))) conf.script_param_set=1;
        load_params_values(conf.script_file, 1, 0);				    
        gui_update_script_submenu();
     }
     else paramSetChanged = 0;
    return modes[conf.script_param_set];
  }
 else return null ;
}

const char* gui_script_name_enum(int change, int arg) 
{
 int i;
 FILE *fd = NULL;
 FILE *j = NULL;
 char fn[24];
 static char modes[26][2]; 
 for(i=0;i<26;i++)
 {
  modes[i][0]=i+'A'; 
  modes[i][1]=0;
 }

    if (change != 0)                                                         
     {
      conf.script_set += change;
      if (conf.script_set<0)conf.script_set=0;
      else if (conf.script_set>=(sizeof(modes)/sizeof(modes[0])))conf.script_set=0;
      i=0;
      do
      {
       if(conf.script_set>25)conf.script_set=0;                       
       sprintf(fn,"A/SDM/SCRIPTS2/%s_.txt",modes[conf.script_set++]);
       fd=(FILE *)fopen(fn,"r");
       j=fd;
       fclose(fd);
       i++;
      }
      while((j==NULL)&&(i<=26));
      if(fd!=NULL)
      {
       play_sound(5);
       conf.script_set--;
       scriptExists = 1;
       for (i=0; i<SCRIPT_NUM_PARAMS; ++i) 
       {
        script_params[i][0]=0;
       }
       conf.script_param_set=1;
       script_load(fn,0); 
       load_params_values(fn, 1, 0);
       gui_update_script_submenu();
       gui_menu_redraw=1;
       gui_menu_draw_initial();
       gui_menu_draw();
      }
 
      else 
      {
       play_sound(6);                     
       for(i=0;i<7;i++)
        {
         paramdesc[i][0]= '\0';             
        }
       scriptExists = 0;
      }
     }
    return modes[conf.script_set];
}
const char* video_bitrate_enum()
{
  return gui_video_bitrate_enum(0,0);
}
const char* tv_enum()
{
  return gui_tv_enum(0,0);
}

const char* tv_bracket_values_enum()
{
  return gui_tv_bracket_values_enum(0,0);
}

const char* focus_enum()
{
 return gui_focus_enum(0,0);
}

#if !defined(CAMERA_m3)
const char* gui_yaw_enum(int change, int arg) 
{
 static const char* decades[]={"Off","Vertical","Diagonal"};
 return gui_change_simple_enum(&conf.enable_yaw_guideline,change,decades,sizeof(decades)/sizeof(decades[0]));
}
#endif

const char* xml_save_enum(int change, int arg) 
{
 static const char* decades[]={"No","Month","Images"};
 return gui_change_simple_enum(&conf.save_xml_file,change,decades,sizeof(decades)/sizeof(decades[0]));
}
const char* gui_header_enum(int change, int arg) 
{
 static const char* decades[]={"Standard","Compact","None"};
 return gui_change_simple_enum(&conf.compact,change,decades,sizeof(decades)/sizeof(decades[0]));
}
#if !(CAM_HAS_ZOOM_LEVER)
const char* gui_digi_step_enum(int change, int arg) 
{
    static const char* decades[]={"1","10","100","1000"};

    conf.digi_step+=change;
    if (conf.digi_step<0)
        conf.digi_step=(sizeof(decades)/sizeof(decades[0]))-1;
    else if (conf.digi_step>=(sizeof(decades)/sizeof(decades[0])))
        conf.digi_step=0;
    digi_data_step = atoi(decades[conf.digi_step]); 
    return decades[conf.digi_step];
}
#endif
const char* gui_synch_zoom_enum(int change, int arg) 
{
 static const char* decades[]={"Off","On"};
 return gui_change_simple_enum(&conf.remote_zoom_enable,change,decades,sizeof(decades)/sizeof(decades[0]));
}

const char* slave_flash_enum(int change, int arg) 
{
 static const char* decades[]={"Off","Dim","Medium","Bright"};
 return gui_change_simple_enum(&conf.slave_flash,change,decades,sizeof(decades)/sizeof(decades[0]));
}

#if defined(PROPCASE_FLASH_EXP_COMP)
const char* gui_flash_exp_comp_modes_enum(int change, int arg)
{
    static const char* modes[] = { "-3", "-2.6", "-2.3", "-2", "-1.6", "-1.3", "-1", "-2/3", "-1/3", "0", "+1/3", "+2/3", "+1", "+1.3", "+1.6", "+2", "+2.3", "+2.6", "+3" };
	return gui_change_simple_enum(&conf.flash_exp_comp,change,modes,sizeof(modes)/sizeof(modes[0]));
}
#endif
 
const char* gui_synch_pulse_enum(int change, int arg) 
{
 static const char* decades[]={"High","Low"};
 return gui_change_simple_enum(&conf.on_release,change,decades,sizeof(decades)/sizeof(decades[0]));
}
const char* gui_stereo_playback_enum(int change, int arg) 
{
 static const char* decades[]={"Off","Mono","Colour"};
 return gui_change_simple_enum(&conf.stereo_mode,change,decades,sizeof(decades)/sizeof(decades[0]));
}
const char* gui_ana_button_enum(int change, int arg) 
{
 static const char* decades[]={"-","UP","DOWN","-","-","SET","HALF"};
 return gui_change_simple_enum(&conf.anaglyph_button,change,decades,sizeof(decades)/sizeof(decades[0]));
} 
#if !(CAM_HAS_ZOOM_LEVER)
const char* gui_synch_step_enum(int change, int arg) 
{
    static const char* decades[]={"1","10","100","1000","10000"};
    conf.synch_delay_range+=change;
    if (conf.synch_delay_range<0)
        conf.synch_delay_range=(sizeof(decades)/sizeof(decades[0]))-1;
    else if (conf.synch_delay_range>=(sizeof(decades)/sizeof(decades[0])))
        conf.synch_delay_range=0;
    delay_step = atoi(decades[conf.synch_delay_range]); 
    return decades[conf.synch_delay_range];
}
#endif

const char* gui_camera_orientation_enum(int change, int arg) 
{
 static const char* extensions[]={ "H","L", "I","R"};
 return gui_change_simple_enum(&conf.camera_orientation,change,extensions,sizeof(extensions)/sizeof(extensions[0]));
}

const char* gui_camera_position_enum (int change, int arg) 
{
 static const char* extensions[]={ "L", "R"};
 return gui_change_simple_enum(&conf.camera_position,change,extensions,sizeof(extensions)/sizeof(extensions[0]));
}


const char* gui_focus_enum(int change, int arg) 
{  
 static const char* extensions[]={ "INF", "HYP", "MAN","RF "};
 return gui_change_simple_enum(&conf.focus_mode,change,extensions,sizeof(extensions)/sizeof(extensions[0]));
}

#if !(CAM_HAS_ZOOM_LEVER)
const char* gui_range_enum(int change, int arg) 
{
    static const char* decades[]={"1","10","100","1000","10000"};

    conf.user_range+=change;
    if (conf.user_range<0)
        conf.user_range=(sizeof(decades)/sizeof(decades[0]))-1;
    else if (conf.user_range>=(sizeof(decades)/sizeof(decades[0])))
        conf.user_range=0;
    distance_step = atoi(decades[conf.user_range]);
    return decades[conf.user_range];
}


const char* gui_dist_step_enum(int change, int arg) 
{
    static const char* decades[]={"1","10","100","1000","10000"};

    conf.dist_step_size+=change;
    if (conf.dist_step_size<0)
        conf.dist_step_size=(sizeof(decades)/sizeof(decades[0]))-1;
    else if (conf.dist_step_size>=(sizeof(decades)/sizeof(decades[0])))
        conf.dist_step_size=0;
    dist_bracket_step = atoi(decades[conf.dist_step_size]);
    return decades[conf.dist_step_size];
}
#endif

const char* gui_dist_mode_enum(int change, int arg) 
{
 static const char* decades[]={"Off","Manual","Auto"};
 return gui_change_simple_enum(&conf.dist_mode,change,decades,sizeof(decades)/sizeof(decades[0]));
}


static int gui_osd_need_restore = 0;    

void gui_set_need_restore()
{
    gui_osd_need_restore = 1;
}

const char* gui_font_enum(int change, int arg) 
{
    static const char* fonts[]={ "Win1250", "Win1251", "Win1252", "Win1254", "Win1257"};

    conf.font_cp+=change;
    if (conf.font_cp<0)
        conf.font_cp=(sizeof(fonts)/sizeof(fonts[0]))-1;
    else if (conf.font_cp>=(sizeof(fonts)/sizeof(fonts[0])))
        conf.font_cp=0;

    if (change != 0) {
        font_set(conf.font_cp);
        gui_menu_init(NULL);
    }
    return fonts[conf.font_cp];
}

const char* gui_raw_nr_enum(int change, int arg) 
{
 static const char* modes[]={ "Auto", "Off", "On"};
 return gui_change_simple_enum(&conf.raw_nr,change,modes,sizeof(modes)/sizeof(modes[0]));
}
 
const char* gui_zoom_value_enum(int change, int arg) 
{
 static const char* modes[]={ "X", "FL", "EFL", "FOV","STEP" };
 return gui_change_simple_enum(&conf.zoom_value,change,modes,sizeof(modes)/sizeof(modes[0]));
}
const char* gui_alt_power_enum(int change, int arg) 
{
    static const char* modes[]={ "Never", "Alt", "Script","Always" };

    conf.alt_prevent_shutdown+=change;
    if (conf.alt_prevent_shutdown<0)
        conf.alt_prevent_shutdown=(sizeof(modes)/sizeof(modes[0]))-1;
    else if (conf.alt_prevent_shutdown>=(sizeof(modes)/sizeof(modes[0])))
        conf.alt_prevent_shutdown=0;
    conf_update_prevent_shutdown(); 
    return modes[conf.alt_prevent_shutdown];
}

#ifdef CAM_HOTSHOE_OVERRIDE
const char* gui_hotshoe_enum(int change, int arg) 
{
    static const char* modes[]={ "Off", "Empty", "In Use"};
    conf.hotshoe_override+=change;
    if (conf.hotshoe_override<0)
        conf.hotshoe_override=(sizeof(modes)/sizeof(modes[0]))-1;
    else if (conf.hotshoe_override>=(sizeof(modes)/sizeof(modes[0])))
        conf.hotshoe_override=0;
    return modes[conf.hotshoe_override];
}
#endif

const char* gui_video_mode_enum(int change, int arg) 
{
 static const char* modes[]={ "Bitrate", "Quality"};
 return gui_change_simple_enum(&conf.video_mode,change,modes,sizeof(modes)/sizeof(modes[0]));
}

const char* gui_video_synch_enum(int change, int arg) 
{
 static const char* modes[]={ "None","AF led","Switch+"};
 return gui_change_simple_enum(&conf.video_synch_device,change,modes,sizeof(modes)/sizeof(modes[0]));
}

const char* gui_video_bitrate_enum(int change, int arg) 
{
    static const char* modes[]={ "0.25x", "0.5x","0.75x", "1x", "1.25x", "1.5x", "1.75x", "2x", "2.5x", "3x"};

    conf.video_bitrate+=change;
    if (conf.video_bitrate<0)
        conf.video_bitrate=0;
    else if (conf.video_bitrate>=(sizeof(modes)/sizeof(modes[0])))
        conf.video_bitrate=sizeof(modes)/sizeof(modes[0])-1;

    shooting_video_bitrate_change(conf.video_bitrate);
    return modes[conf.video_bitrate];
}

const char* gui_nd_filter_state_enum(int change, int arg) 
{
 static const char* modes[]={ "Off", "In", "Out" };
 return gui_change_simple_enum(&conf.nd_filter_state,change,modes,sizeof(modes)/sizeof(modes[0]));
}

const char* gui_tv_enum(int change, int arg) 
{
 static const char* modes[]={ "65\"","50\"", "40\"", "30\"", "25\"","20\"","15\"","13\"","10\"","8\"","6\"","5\"","4\"","3.2\"","2.5\"","2\"","1.6\"","1.3\"","1\"","Null","0.8","0.6","0.5","0.4","0.3","1/4","1/5","1/6","1/8","1/10","1/13","1/15","1/20","1/25","1/30","1/40","1/50","1/60","1/80", "1/100","1/125","1/160","1/200","1/250","1/320","1/400","1/500","1/640","1/800", "1/1000","1/1250","1/1600","1/2000", "1/2500","1/3200","1/4000", "1/5000", "1/6400", "1/8000", "1/10000", "1/12800", "1/16000", "1/20000", "1/26000","1/32000","1/40000"};
 return gui_change_simple_enum(&conf.tv_override,change,modes,sizeof(modes)/sizeof(modes[0]));
}
const char* gui_av_enum(int change, int arg) 
{

    static char buf[8];
    short prop_id;
    conf.av_override_value+=change;

    if (conf.av_override_value<0) conf.av_override_value=shooting_get_aperture_sizes_table_size()+6;
    else if (conf.av_override_value>shooting_get_aperture_sizes_table_size()+6) conf.av_override_value=0;
    if (conf.av_override_value == 0)  return "Off";
    else
    {
      prop_id=shooting_get_aperture_from_av96(shooting_get_av96_override_value());	
	sprintf(buf, "%d.%02d", (int)prop_id/100, (int)prop_id%100 );
	return buf; 
    }

}
const char* gui_tv_bracket_values_enum(int change, int arg) 
{
  static const char* modes[]={ "Off", "1/3 EV","2/3 EV", "1 EV", "1 1/3Ev", "1 2/3Ev", "2 Ev"};
  return gui_change_simple_enum(&conf.tv_bracket_value,change,modes,sizeof(modes)/sizeof(modes[0]));
}
const char* gui_bracket_type_enum(int change, int arg) 
{
  static const char* modes[]={"lighter","darker","alternate","3 times","5 times","7 times","9 times"};
  return gui_change_simple_enum(&conf.bracket_type,change,modes,sizeof(modes)/sizeof(modes[0]));
}


void gui_update_script_submenu() 
{
    register int p=0, i;
   
  if(gScriptFolder==1)
   {
    for (i=0; i<sizeof(script2_submenu_items_top)/sizeof(script2_submenu_items_top[0]); ++p, ++i) 
    {
        script2_submenu_items[p]=script2_submenu_items_top[i];
    }
    for (i=0; i<SCRIPT_NUM_PARAMS; ++i) 
    {
        if (script_params[i][0]) 
        {
            script2_submenu_items[p].text=(int)script_params[i]; 		
            script2_submenu_items[p].type=MENUITEM_INT;
            script2_submenu_items[p].value=&conf.ubasic_vars2[i];
            ++p;
        }
    }
    for (i=0; i<sizeof(script_submenu_items_bottom)/sizeof(script_submenu_items_bottom[0]); ++p, ++i) 
    {
        script2_submenu_items[p]=script_submenu_items_bottom[i];
    }
   }
  
  else if(gScriptFolder==2)
   {  
    for (i=0; i<sizeof(script3_submenu_items_top)/sizeof(script3_submenu_items_top[0]); ++p, ++i) 
    {
        script3_submenu_items[p]=script3_submenu_items_top[i];
    }
    for (i=0; i<SCRIPT_NUM_PARAMS; ++i) 
    {
        if (script_params[i][0])  
        {
            script3_submenu_items[p].text=(int)script_params[i];
            script3_submenu_items[p].type=MENUITEM_INT;
            script3_submenu_items[p].value=&conf.ubasic_vars3[i];
            ++p;
        }
    }
    for (i=0; i<sizeof(script_submenu_items_bottom)/sizeof(script_submenu_items_bottom[0]); ++p, ++i) 
    {
        script3_submenu_items[p]=script_submenu_items_bottom[i];
    }
  }    
}


#if CAM_MULTIPART
static void gui_menuproc_swap_partitions(int arg)
{
 if (get_part_count()<2) gui_mbox_init(LANG_ERROR, LANG_ONLY_ONE_PARTITION, MBOX_BTN_OK|MBOX_TEXT_CENTER, NULL);
 else {swap_partitions();gui_mbox_init(LANG_INFORMATION,LANG_SWAPPED, MBOX_BTN_OK|MBOX_TEXT_CENTER, NULL);}
}
#endif

volatile enum Gui_Mode gui_mode;
static volatile int gui_restore;
static volatile int gui_in_redraw;
static char osd_buf[32];

static Conf old_conf;

void gui_init()
{
    consoleOrigin.x=30;
    consoleOrigin.y=136;
    gStartHour=gEndHour=gStartMinutes=gEndMinutes=0;
    conf.focus_override_method=99;   
#if defined(CAM_SD_OVER_IN_AFL)
    conf.focus_override_method=2;
#elif defined(CAM_SD_OVER_IN_AF)
    conf.focus_override_method=1; 
#elif defined(CAM_SD_OVER_IN_MF)
    conf.focus_override_method=3;  
#endif 

#if defined(LOW_LIGHT)
    conf.low_light=61 ;
#elif defined(MODE_SCN_LOWLIGHT)
    conf.low_light=64 ;
#else
    conf.low_light=0 ;
#endif
    
    if(conf.video_frame_count)
     gFrameCount=conf.video_frame_count;
    gui_mode = GUI_MODE_NONE;
    gui_restore = 0;
    gui_in_redraw = 0;
    conf.sleep=0;
    bls=1;
    gui_lang_init();
    prevOrientation=conf.camera_orientation;
    draw_init();  
  gSlitBytes = ((CAM_RAW_ROWS/2)*conf.strip_offset)/100;
  gSlitBytes = (CAM_RAW_ROWS/2)-gSlitBytes-(conf.strip_width/2);
  gSlitBytes = (gSlitBytes*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL)/8; 
 
    voltage_step = (conf.batt_step_25)?25:1;
    if(conf.batt_volts_max<=1500) conf.batt_volts_max = get_vbatt_max();
    if(conf.batt_volts_min<=1500) conf.batt_volts_min = get_vbatt_min();
    digi_mode = 0;
    conf.invert_playback=0;
    conf.stereo_mode=0;
    disable_shutdown();
    conf.user_1=conf.user_2=conf.user_3=conf.user_4=0;
    myav=shooting_get_min_real_aperture();
    gCurrentFocus=lens_get_focus_pos_from_lens();
    conf.near_for_infinity = nearForInfinity();
#if defined(CAMERA_m3)	
	movie_status=1;
#endif
}

enum Gui_Mode gui_get_mode() 
{
    return gui_mode;
}

void gui_set_mode(enum Gui_Mode mode) 
{
    gui_mode = mode;
}

void gui_force_restore() 
{
    gui_restore = gui_in_redraw;
}
 

void draw_mini_scripts()
{
 if(!state_kbd_script_run)
 {
 char filename[30],*buf,*ptr;
 int xpos,ypos,rcnt,fd;
 unsigned char i,j,size=1 ;
 static unsigned char hp=0;
 struct stat st;
 xpos=screen_width>>1;
 ypos=screen_height>>1;
 
 if(!kbd_is_key_pressed(KEY_SHOOT_HALF))
 {
  if(hp){hp=0;draw_restore();}
 for(i=1;i<5;i++)
 {
  sprintf(filename,"A/SDM/SCRIPTSM/MINI_%1d.TXT",i);
  if(stat(filename, &st)==0)
   {
    buf=(char*)umalloc(st.st_size+1);
   if(buf)
   {
    fd = open(filename, O_RDONLY, 0777);
    if(fd>=0)
    {
     rcnt = read(fd, buf, st.st_size);                                  
     buf[rcnt] = 0;
     close(fd);
     j=0;     
     ptr=buf;
     while (ptr[0]) 
     {
       while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
      if ((ptr[0]=='@')&& (strncmp("@title", ptr, 6)==0)) 
      {
       ptr+=6;
       while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
       while (j<(sizeof(filename)-1) && ptr[j] && ptr[j]!='\r' && ptr[j]!='\n') 
       {
        filename[j]=ptr[j];
        ++j;
       }
       filename[j]=0;
       break;
      }
     }
    }
    ufree(buf);
   }
 }
  else sprintf(filename,"NULL");
  
    switch (i-1)
    {
     case 0:
      xpos=(screen_width>>1)-(FONT_WIDTH*size*(strlen(filename)/2));
      ypos=(screen_height>>1)-40;
     break;
     case 1:
      xpos=(screen_width>>1)+40;
      ypos=(screen_height>>1)-((FONT_HEIGHT*size)/2);      
     break;
     case 2:
      xpos=(screen_width>>1)-(FONT_WIDTH*size*(strlen(filename)/2));
      ypos=(screen_height>>1)+40-(FONT_HEIGHT*size);    
     break;
     case 3:
      xpos=(screen_width>>1)-40-(FONT_WIDTH*size*strlen(filename));
      ypos=(screen_height>>1)-((FONT_HEIGHT*size)/2);       
     break;   
    }     
    draw_string(xpos,ypos,filename,(!gPlayRecMode)?MAKE_COLOR(COLOR_BG,COLOR_RED_P):MAKE_COLOR(COLOR_BG,COLOR_RED),size);      
  }
 }
 
 else
 {
  if(!hp){hp=1;draw_restore();}
  for(i=5;i<9;i++)
 {
  sprintf(filename,"A/SDM/SCRIPTSM/MINI_%1d.TXT",i);
  if(stat(filename, &st)==0)
   {
    buf=umalloc(st.st_size+1);
   if(buf)
   {
    fd = open(filename, O_RDONLY, 0777);
    if(fd>=0)
    {
     rcnt = read(fd, buf, st.st_size);                                  
     buf[rcnt] = 0;
     close(fd);
     j=0;     
     ptr=buf;
     while (ptr[0]) 
     {
       while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
      if ((ptr[0]=='@')&& (strncmp("@title", ptr, 6)==0)) 
      {
       ptr+=6;
       while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
       while (j<(sizeof(filename)-1) && ptr[j] && ptr[j]!='\r' && ptr[j]!='\n') 
       {
        filename[j]=ptr[j];
        ++j;
       }
       filename[j]=0;
        break;      
      }
     }
    }
    ufree(buf);
   }
   }
   else sprintf(filename,"NULL");
    switch (i-5)
    {
     case 0:
      xpos=(screen_width>>1)-(FONT_WIDTH*size*(strlen(filename)/2));
      ypos=(screen_height>>1)-40;
     break;
     case 1:
      xpos=(screen_width>>1)+40;
      ypos=(screen_height>>1)-((FONT_HEIGHT*size)/2);      
     break;
     case 2:
      xpos=(screen_width>>1)-(FONT_WIDTH*size*(strlen(filename)/2));
      ypos=(screen_height>>1)+40-(FONT_HEIGHT*size);    
     break;
     case 3:
      xpos=(screen_width>>1)-40-(FONT_WIDTH*size*strlen(filename));
      ypos=(screen_height>>1)-((FONT_HEIGHT*size)/2);      
     break;   
    }     
    draw_string(xpos,ypos,filename,MAKE_COLOR(COLOR_BG,COLOR_RED),size);     
 }
 }
  sprintf(filename,"%04d",LowestCurCnt);  // period count when usb pressed
  xpos=(screen_width>>1)-(FONT_WIDTH*2);
  ypos=(screen_height>>1)-(FONT_HEIGHT/2); 
  draw_string(xpos,ypos,filename,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);
}
}

static int gui_mode_need_redraw = 0;
static int flag_gui_enforce_redraw = 0;
void gui_redraw()
{
    enum Gui_Mode gui_mode_old;
    static int show_script_console=0;
   int rec = ((mode_get()&MODE_MASK) == MODE_REC);
   static int count;
 
    if (gui_osd_need_restore)
    {
        draw_restore();
        gui_osd_need_restore = 0;
    }
 
    if (gui_mode_need_redraw)
    {
        gui_mode_need_redraw = 0;
    }

   if(!gAutoIsoDisabled && !kbd_is_key_pressed(KEY_SHOOT_HALF) && !kbd_is_key_pressed(KEY_SHOOT_FULL)) onHalfPress.ISO=shooting_get_prop(PROPCASE_ISO_MODE); 
    gui_in_redraw = 1;
    gui_mode_old = gui_mode;

    switch (gui_mode) { 
    
        case GUI_MODE_MENU:
            if(conf.camera_orientation!=prevOrientation)
            {
             prevOrientation=conf.camera_orientation;
             vid_bitmap_refresh();  
            }
            gui_menu_draw();
           if((conf.camera_orientation==1)||(conf.camera_orientation==3))         
             draw_txt_string(35, screen_width/FONT_HEIGHT-1, "<CNF>", MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P));
            else
             draw_txt_string(20, screen_width/FONT_HEIGHT-1, "<CNF>", MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P));
            break;

        case GUI_MODE_ALT:

            gui_draw_osd();

          if (!state_kbd_script_run)
          {
           if((conf.camera_orientation==1)||(conf.camera_orientation==3))
            {
             draw_txt_string(0, screen_width/FONT_HEIGHT-1, (gScriptFolder==1)?script_title2:script_title, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,COLOR_WHITE));
             draw_txt_string(20, screen_width/FONT_HEIGHT-1, "<ALT>", MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P));
		}
           else
            {            
             draw_txt_string(0, screen_height/FONT_HEIGHT-1, (gScriptFolder==1)?script_title2:script_title, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,COLOR_WHITE));
             draw_txt_string(20, screen_height/FONT_HEIGHT-1, "<ALT>", MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P));
 		}
          }
                if (state_kbd_script_run) show_script_console=5;

                if (show_script_console) 
                  {
                    --show_script_console;
			           md_draw_grid();
                    script_console_draw();
                   }

              if ((mode_get()&MODE_MASK) == MODE_PLAY)
               {
                 draw_txt_string(27, 14,(!playback && conf.remote_enable)? "Image   ":(!playback && !conf.remote_enable)?"Image PC":(playback && conf.remote_enable)?"Movie   ":"Movie PC", MAKE_COLOR(COLOR_GREY_P,COLOR_WHITE_P));
               }
               draw_mini_scripts();
            break;

        case GUI_MODE_NONE:
            gui_draw_osd();
            break;

        case GUI_MODE_PALETTE:
            gui_palette_draw();
            break;

        case GUI_MODE_MBOX:
            gui_mbox_draw();
            break;

        case GUI_MODE_FSELECT:
            if(inGridSelection())
             gui_draw_osd();
            else
             gui_fselect_draw();
            break;

        case GUI_MODE_READ:
            gui_read_draw();
            break;

        case GUI_MODE_OSD:
            gui_osd_draw();
            break;
 
       default:
            break;
    }
    
    gui_in_redraw = 0;
    
   if ((gui_mode_old != gui_mode && (gui_mode_old != GUI_MODE_NONE && gui_mode_old != GUI_MODE_ALT) && (gui_mode != GUI_MODE_MBOX && gui_mode != GUI_MODE_MPOPUP)) || gui_restore)

       {
        gui_restore = 0;
       }  
}

static inline void conf_store_old_settings() 
{
    old_conf=conf;
}

inline int conf_save_new_settings_if_changed() 
{
    if (memcmp(&old_conf, &conf, sizeof(Conf)) != 0) 
    {
        conf_save();
        conf_store_old_settings(); 
        return 1;
    }
    return 0;
}

void user_scripts_menu()
{
 kbd_blocked = 1;
 gui_kbd_enter(); 
 gui_menu_init(&script3_submenu);
 gui_mode = GUI_MODE_MENU; 
 gui_load_script(0);
}
 
void welcome_menu()
{
 if(conf.splash_show && !welcomeDone && ((mode_get() & MODE_MASK) == MODE_PLAY))
 {
 int fd;
  getLanguageCode(); 
  fd = open("A/SDM/TEXTS/welcome.txt", O_RDONLY, 0777);
  if(fd>=0)
  {
   close(fd);
   kbd_blocked = 1;
   gui_kbd_enter(); 
   gui_draw_read_selected("A/SDM/TEXTS/welcome.txt");
  }
 else
 {
  play_sound(6);
 } 
  welcomeDone=1;
 }
}
 
void easy_mode_menu()
{
 kbd_blocked = 1;
 gui_kbd_enter(); 
 load_easy_mode_script(1); 
}
void gui_kbd_process()
{
 int clicked_key;
static  long focus_inc;
static  long focus_pos;
static int pressed = 0,cfar,cnear,prev=0;
static long num,denom;
static float realav;
static unsigned char lvShow=0;
static long startTime,endTime,wdStart;
static unsigned short keyMenuDuration;
int m,vmode,NextFocus;
int realfl = gFocalPoint/1000;
realav=(float)(myav)/100;
 
  if(gMenuPressed && !kbd_is_key_pressed(KEY_MENU))
   {
     keyMenuDuration=gMenuPressed;
     gMenuPressed=0;
        switch (gui_mode)
       {
            case GUI_MODE_ALT:
                if(keyMenuDuration>24)
                {
                 draw_restore();
                 gui_menu_init(&user_submenu);
                 gui_mode = GUI_MODE_MENU;
                }
                else
                {
                 kbd_blocked = 0;
                 gui_kbd_leave();
                }
                kbd_key_release_all();          
                break;
            case GUI_MODE_MENU:  
                if(!gridFlag)
                {               
                 conf_save_new_settings_if_changed();  
                 gui_mode = GUI_MODE_ALT;
                 draw_restore();
                }
                else gridFlag = 0;
                break;
            case GUI_MODE_PALETTE:
            case GUI_MODE_OSD:
                draw_restore();                                
                gui_mode = GUI_MODE_MENU;
                break;
            case GUI_MODE_FSELECT:
                gui_fselect_kbd_process();
                break;
           
           
           
           
            case GUI_MODE_READ:
                if(!conf.splash_show)
                 {
                  gui_read_kbd_process();
                  draw_restore();
                  gui_mode = GUI_MODE_MENU;
                 }
                 else
                 {
                 conf.reader_pos=0;
              conf.splash_show=0;
#if !defined(CAMERA_m3)
              conf.extend_lens=1;
#endif
              kbd_blocked=0;
              kbd_key_release_all();
              gui_mode = GUI_MODE_NONE;
              gui_mbox_init((int)"Camera position",(int)"Right camera ?",MBOX_DEF_BTN2|MBOX_TEXT_CENTER|MBOX_BTN_YES_NO,SetCameraPosition);            
                 }
                break;
           
            default:
                break;
        }
        return;
    } 
    
   switch (gui_mode) 
   {
     case GUI_MODE_ALT:

      get_property_case(PROPCASE_SHOOTING_MODE, &vmode, 4);
      vmode=shooting_mode_canon2chdk(vmode);  
      volatile int* counter;
      unsigned char abortSync;
      counter= PERIOD_COUNT;
           
	if(!gSetPressed && get_usb_bit()&& gPlayRecMode && !state_kbd_script_run) 
      {
        draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_BLACK,COLOR_BLACK));
        do{}while(get_usb_bit());       
        LowestCurCnt = *(counter) & 0xffff;   
        play_sound(4); 
        endTime=shooting_get_tick_count();        
      }
    
 #if !defined(VIDEO_STD)
	  else if((gSetPressed && (get_movie_status()==4)&& !MODE_IS_SYNCH_VIDEO(vmode))\
	  || ((gSetPressed||((gMovieSyncAutostart==1)&&((shooting_get_tick_count()-syncStart)>4000))) && gPlayRecMode && (shooting_get_tick_count()>(endTime+2000))&& !state_kbd_script_run && MODE_IS_SYNCH_VIDEO(vmode) && (conf.camera_position||(conf.video_synch_device==2)))) 
#else
   else if((gSetPressed||((gMovieSyncAutostart==1)&&((shooting_get_tick_count()-syncStart)>4000))) && gPlayRecMode && (shooting_get_tick_count()>(endTime+2000))&& !state_kbd_script_run /*&& MODE_IS_SYNCH_VIDEO(vmode) && (conf.camera_position||(conf.video_synch_device==2))*/) 
#endif
      {
       short int* afbuf=NULL;
       static  int cnt,scnt,usbHi;
       static  int curCnt,driftCounts;
       static  unsigned char driftDone;
	 if(gMovieSyncAutostart==1)gMovieSyncAutostart=99;
       vid_bitmap_refresh();
       int period_ref=STD_PERIOD/2;
       
        int std_period = STD_PERIOD;									
        int synch_fps;
        int sync_speed=SYNCH_SPEED;
#if CAM_PROPSET == 4
 synch_fps=shooting_get_prop(PROPCASE_VIDEO_FRAMERATE);
 if(synch_fps==0)synch_fps=30;
 else if(synch_fps==1)synch_fps=24; 
 else if(synch_fps==2)synch_fps=240;
 else synch_fps=120; 
#elif CAM_PROPSET == 5
 synch_fps=shooting_get_prop(PROPCASE_VIDEO_FRAMERATE);
 if(synch_fps==0)synch_fps=30;
 else if(synch_fps==1)synch_fps=25;
 else if(synch_fps==2)synch_fps=24;
 else if(synch_fps==3)synch_fps=240;
 else if(synch_fps==4)synch_fps=120;  
#elif CAM_PROPSET == 6
 synch_fps=shooting_get_prop(PROPCASE_VIDEO_FRAMERATE);
 if(synch_fps==0)synch_fps=30;
 else if(synch_fps==1)synch_fps=25; 
 else if(synch_fps==3)synch_fps=240;
 else if(synch_fps==4)synch_fps=120;
 else synch_fps=60; 
#elif CAM_PROPSET == 7
 synch_fps=shooting_get_prop(PROPCASE_VIDEO_FRAMERATE);
 if(synch_fps==0)synch_fps=30;
  else if(synch_fps==1)synch_fps=25;
 else if(synch_fps==3)synch_fps=24;
 else if(synch_fps==7)synch_fps=60; 
 else if(synch_fps==8)synch_fps=50;  
#else
synch_fps=SYNCH_FPS;	
#endif        startTime=shooting_get_tick_count();
        long deltaPeriod;
        unsigned int cycles,konst;
        char charbuf[6];
        konst = synch_fps * (std_period+1); 								
        period1 = 1000000/synch_fps;             							
        period2 = ((std_period+1+sync_speed)*1000000)/konst;					
        deltaPeriod = period2-period1;									
         
         if(conf.video_synch_device==2)pulseCount=20;
         else pulseCount=150;
         wdStart=startTime;
         play_sound(4);
         msleep(300);
         play_sound(4);
         LowestSynchCnt = 9999;
         afbuf = (short int*)malloc(sizeof(short int)*pulseCount);
         pAFbuf=afbuf;
         lvShow = 1;
       
  while(lvShow)
   {
      if(conf.video_synch_device==2)
      {
       draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_BLUE,COLOR_BLUE));
       abortSync=0;
         do{
           if((shooting_get_tick_count()-wdStart)> 12000)
           {
            abortSync=1;
            break;
           }
          }
         while(!get_usb_bit());                  
      if(!abortSync)
       {      
        msleep(500);
        do{LowestCurCnt = *(counter) & 0xffff;}while(get_usb_bit());       
        draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_BLACK,COLOR_BLACK));
        play_sound(4);
        endTime=shooting_get_tick_count();
        
          temp1=LowestCurCnt;
 
    driftCounts = LowestCurCnt-period_ref;
    if(LowestCurCnt<period_ref)driftCounts+=(std_period+1);
    driftTime = (driftCounts*period1)/std_period; 						
    driftTime = (((driftTime/deltaPeriod+conf.MovieFineTune)*period2))/1000;		
    if(driftTime<2000)
     {
      driftTime=((driftCounts+std_period+1)*period1)/std_period; 				
      driftTime = (((driftTime/deltaPeriod+conf.MovieFineTune)*period2))/1000;
     } 
     else if(driftTime>12000 )                              
     {
      sync_speed=6;
      period2 = ((std_period+1+sync_speed)*1000000)/konst;					
      deltaPeriod = period2-period1;
      driftTime = (driftCounts*period1)/std_period; 						
      driftTime = (((driftTime/deltaPeriod+conf.MovieFineTune)*period2))/1000;		
     }										
    if(driftTime && (driftTime<60000))
     {						
      long dStart=shooting_get_tick_count();
      while(shooting_get_tick_count()==dStart){};						
      dStart=shooting_get_tick_count();
      *(volatile int*)(0xC0F06014) = (std_period)+sync_speed;  				
      *(volatile int*)(0xC0F06000) = 1;
      while (*(volatile int*)(0xC0F06000)) {};							
      rw();
      while((shooting_get_tick_count()-dStart)<driftTime){msleep(10);}			
      *(volatile int*)(0xC0F06014) = std_period; 						
      *(volatile int*)(0xC0F06000) = 1;
     }
     play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     msleep(200);
     play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_RED,COLOR_RED));
        wdStart=shooting_get_tick_count();
        do{
           if((shooting_get_tick_count()-wdStart)> 8000)
           {
            wdStart=shooting_get_tick_count();
            rw();
           }
          }
         while(!get_usb_bit());      
        msleep(500);
        do{LowestCurCnt = *(counter) & 0xffff;}while(get_usb_bit());       
        } 
        abortSync=0;
        play_sound(4);
        draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_TRANSPARENT,COLOR_TRANSPARENT));    
        endTime=shooting_get_tick_count();  
        lvShow=0;
        gSetPressed=0;
       gMenuPressed=keyMenuDuration=1; 
       return;   
      }
 
 else   
 {
   
 if(cnt<pulseCount)
  {
   if(get_usb_bit()&& !usbHi) 
    {
     usbHi = 1;
     ubasic_set_led(9,0,0);
    }
   else if(!get_usb_bit()&& usbHi) 
    {
     usbHi = 0;
     ubasic_set_led(9,1,100);
     curCnt = *(counter) & 0xffff;
     if(curCnt<LowestCurCnt)LowestCurCnt=curCnt;
     cnt++ ;
    } 
  }
 else
  { 
   if(!driftDone)
   {
    temp1=LowestCurCnt;
    play_sound(4);
    ubasic_set_led(9,1,100);
    msleep(300);
    ubasic_set_led(9,0,0);
    driftCounts = LowestCurCnt-period_ref;
    if(LowestCurCnt<period_ref)driftCounts+=(std_period+1);
    driftTime = (driftCounts*period1)/std_period; 						
    driftTime = (((driftTime/deltaPeriod+conf.MovieFineTune)*period2))/1000;		
    if(driftTime<2000)
     {
      driftTime=((driftCounts+std_period+1)*period1)/std_period; 				
      driftTime = (((driftTime/deltaPeriod+conf.MovieFineTune)*period2))/1000;
     } 										
    if(driftTime && (driftTime<60000))
     {						
      long dStart=shooting_get_tick_count();
      while(shooting_get_tick_count()==dStart){};						
      dStart=shooting_get_tick_count();
      *(volatile int*)(0xC0F06014) = (std_period)+sync_speed; 				
      *(volatile int*)(0xC0F06000) = 1;
      while (*(volatile int*)(0xC0F06000)) {};							
      while((shooting_get_tick_count()-dStart)<driftTime){msleep(10);}			
      *(volatile int*)(0xC0F06014) = std_period; 						
      *(volatile int*)(0xC0F06000) = 1;
     }
     play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     msleep(200);
     play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     driftDone=1;
     usbHi=0;
     cnt=0;
     LowestCurCnt=9999;
    }  
  else if (driftDone && (driftTime<60000))													
   {
    while(scnt<pulseCount)
    {
   if(get_usb_bit()&& !usbHi)	 
    {
     usbHi = 1;
     ubasic_set_led(9,0,0);
    }
   else if(!get_usb_bit()&& usbHi) 
    {
     usbHi = 0;
     curCnt = *(counter) & 0xffff;
     if(curCnt<LowestSynchCnt)LowestSynchCnt=curCnt;
     afbuf[scnt]= curCnt;
     scnt++;
     ubasic_set_led(9,1,100);
    } 
   } 
  }
  
} 

  if(driftDone && (driftTime<60000) && (scnt>=pulseCount))
  {
   temp2=LowestSynchCnt;
   driftDone = 0;
   driftTime = 0;
   cnt=usbHi=curCnt=driftCounts=0;
   scnt = 0;
   play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
   if(conf.camera_position||(conf.video_synch_device==2))
    {
     driftCounts = LowestSynchCnt-period_ref;
     int tmp = 1+std_period;
     long tmp2 = driftCounts*period2;								
     deltaTuning = tmp2/tmp/deltaPeriod;
     movSynchErr=abs(LowestSynchCnt-period_ref)*1000/std_period;
     if(movSynchErr<10)syncQ=4;
     else if(movSynchErr<=20)syncQ=3;
     else if(movSynchErr<=30)syncQ=2;
     else syncQ=1;
     syncDone = 1;
     qual1=std_period/100;
     qual2=2*std_period/100;
     qual3=3*std_period/100;
    } 

    lvShow = 0;   
    PrevZoom = -1; 
    play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     msleep(200);
    play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
     msleep(200);
    play_sound(4);
     ubasic_set_led(9,1,100);
     msleep(300);
     ubasic_set_led(9,0,0);
    endTime=shooting_get_tick_count();
   } 

  if(((shooting_get_tick_count()-startTime)> 70000) ||(driftTime>=60000))						
   {
    play_sound(6);
    lvShow = 0;
    driftDone = 0;
    driftTime = 0;
    endTime=shooting_get_tick_count();
    cnt=usbHi=curCnt=driftCounts=0;
    scnt = 0;
    ubasic_set_led(9,0,0);    
   }
   } 
  } 
  return;
 } 
  
    
else 
{
            static int sd = 0; 
            unsigned char ac_key;            
           if(kbd_is_key_pressed(KEY_SET)) sd++;
           if(kbd_is_key_pressed(KEY_SET)&&(sd==50)) play_sound(4);
          if(!kbd_is_key_pressed(KEY_SET)&& sd &&(!(shooting_get_drive_mode() && (conf.dist_mode || conf.tv_bracket_value))))      
          {
              if(!(conf.dist_mode && shooting_get_drive_mode())&&(sd>50))                                  
              {
                if((mode_get()&MODE_MASK) == MODE_PLAY)conf.remote_enable=!conf.remote_enable;              
              }
              sd=0;          
          }
          ac_key = kbd_get_autoclicked_key();
          if((ac_key == conf.alt_mode_button)&&!conf.alt_files_shortcut)      
          {
           if(displayedMode&&((displayedMode<5)||(displayedMode==27)||(displayedMode>7&&displayedMode<13)||(displayedMode>15&&displayedMode<23)||(displayedMode>30&&displayedMode<34)))     
           {
            gui_read_stacks(1);
           }
           else if(conf.save_xml_file)  
           {
            gui_read_xml(1);
           }
           else gui_draw_read(1);  
          }
          ac_key=get_vkey(ac_key);    
          if((ac_key==KEY_UP)||(ac_key==KEY_RIGHT)||(ac_key==KEY_DOWN)||(ac_key==KEY_LEFT))
           {
            unsigned char scriptNum=0;
            char scriptName[30],offset=0;
            if(kbd_is_key_pressed(KEY_SHOOT_HALF))offset=4;
            if(ac_key==KEY_UP)scriptNum=1+offset;
             if(ac_key==KEY_RIGHT)scriptNum=2+offset;
            if(ac_key==KEY_DOWN)scriptNum=3+offset;
            if(ac_key==KEY_LEFT)scriptNum=4+offset;            
            if(scriptNum)
            {
             struct stat st;
             sprintf(scriptName,"A/SDM/SCRIPTSM/MINI_%1d.TXT",scriptNum);            
             if (stat(scriptName,&st) == 0) 
             {            
             gScriptFolder=2;
             strcpy(normal_script,conf.user_script_file); 
             mini_started=1; 
             script_load(scriptName,0);
             play_sound(4);
             draw_restore();
             script_start();
             }
             else play_sound(6);            
            }  
           }           
         }
          break;  

    	case GUI_MODE_MENU:
            gui_menu_kbd_process();
            break;
            
      case GUI_MODE_PALETTE:
            gui_palette_kbd_process();
            break;
 
    	case GUI_MODE_MBOX:
            gui_mbox_kbd_process();
            break;

    	case GUI_MODE_FSELECT:
            gui_fselect_kbd_process();
            break;

    	case GUI_MODE_READ:
            gui_read_kbd_process();
            break;

    	case GUI_MODE_OSD:
            gui_osd_kbd_process();
            break;
 
        default:
            break;
    }
}

void gui_kbd_enter()
{
    conf_store_old_settings();
    playback = !playback;
    movSynchMode = !movSynchMode;
    gui_mode = GUI_MODE_ALT;
    conf_update_prevent_shutdown();
    vid_turn_off_updates();
}

void gui_kbd_leave()
{
    conf_save_new_settings_if_changed();
    ubasic_error = 0;
    if(get_sd_status() && ((conf.dist_mode!=3)&& !tl.running)) enable_shutdown(); 
    if(sfo)                                                                      
    {
     conf.user_range_set=1;
     sfo=0;
    }
    vid_turn_on_updates();
    gui_mode = GUI_MODE_NONE;
    draw_restore();
    conf_update_prevent_shutdown();
}

void other_kbd_process()
{
 int key;

#if CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO
#if CAM_HAS_ZOOM_LEVER
   key=KEY_ZOOM_OUT;
#else
   key=KEY_DOWN;
#endif
 if (conf.unlock_optical_zoom_for_video && is_video_recording() &&  kbd_is_key_clicked(key))
  {
   short x;
   get_property_case(PROPCASE_DIGITAL_ZOOM_STATE, &x, sizeof(x));
   if (x) 
    {
     get_property_case(PROPCASE_DIGITAL_ZOOM_POSITION, &x, sizeof(x));
#if defined(CAM_USE_OPTICAL_MAX_ZOOM_STATUS)
     if (x==0) zoom_status=ZOOM_OPTICAL_MAX; //ERR99: No zoom back from digital to optical zoom possible if set to medium
#else
     if (x==0) zoom_status=ZOOM_OPTICAL_MEDIUM;
#endif
    }
  }
#endif
}
extern long physw_status[3];
extern long GetPropertyCase(long opt_id, void *buf, long bufsize);


void gui_draw_osd() 
{
    int anyTempValue=0;
    unsigned int m, n = 0, mode_photo, mode_video,mplay;
    coord x;
    static int flashlight = 0,pressed = 0,usb_on = 0,header_on = 0,nd_on=0; 
    static int slave_on=0;
    static int afl_on=0,ael_on=0, sun_on=0;
    static int afc = 0;                       
    static int fm = 0;                        
    static int shoot_mode=0;
    static unsigned char prevDisplayedMode=0;
    unsigned char driveMode,needRestore;
    driveMode=shooting_get_drive_mode();
    needRestore=0;
    m = mode_get();
    mode_photo = (m&MODE_MASK) == MODE_PLAY || 
                 !((m&MODE_SHOOTING_MASK)==MODE_VIDEO_STD || (m&MODE_SHOOTING_MASK)==MODE_VIDEO_SPEED || (m&MODE_SHOOTING_MASK)==MODE_VIDEO_COMPACT ||
                   (m&MODE_SHOOTING_MASK)==MODE_VIDEO_MY_COLORS || (m&MODE_SHOOTING_MASK)==MODE_VIDEO_COLOR_ACCENT || (m&MODE_SHOOTING_MASK)==MODE_STITCH);
    mplay = (m&MODE_MASK)==MODE_PLAY;
    mode_video = MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK);
    if((mode_video && !is_video_recording()) || gScriptMovie)
     {
      if(conf.video_frame_count)
       VideoMode=VIDEO_BURST;
      else VideoMode = VIDEO_NULL;
     }
    fpd = shooting_get_lens_to_focal_plane_width();
    int flashStatus = shooting_get_prop(PROPCASE_FLASH_MODE);  
#if 1 
 
     gSDMShootMode=0;

     if((displayedMode<34) && (driveMode == 3))gHdrPhase=0; 
 
    if((driveMode==1) && conf.raw_strip_mode && conf.save_raw)gSDMShootMode=12;          
     else if((driveMode==1) &&(conf.dist_mode ||conf.tv_bracket_value)&& !conf.fastlapse)gSDMShootMode=1; 
     else if((driveMode== 3)&& !conf.custom_timer_synch)gSDMShootMode=2;
     else if(!driveMode&&conf.dist_mode)gSDMShootMode=3; 
     else if((driveMode==1)&& !conf.tv_bracket_value && !conf.dist_mode && conf.burst_frame_count && !conf.fastlapse && (conf.bracket_type>2))gSDMShootMode=4; 
     else if((driveMode==1)&&!conf.tv_bracket_value && !conf.dist_mode && conf.fastlapse && conf.burst_frame_count&& (conf.bracket_type<=2))gSDMShootMode=5; 
     else if((driveMode==1)&& conf.fastlapse && conf.burst_frame_count)gSDMShootMode=8;
     else if((driveMode== 3)&& conf.custom_timer_synch) gSDMShootMode=6;  
     else if(!driveMode&&!conf.dist_mode&&conf.synch_enable&& conf.remote_enable)gSDMShootMode=7; 
     else if(!conf.fastlapse && !conf.burst_frame_count && !conf.dist_mode && !conf.tv_bracket_value && (!conf.tv_override_enable||(conf.tv_override_enable&&(conf.user_1==80979))))  
     {
      if(!driveMode)gSDMShootMode=9;
      else if(driveMode==1)gSDMShootMode=10;
      else if(driveMode==3)gSDMShootMode=11;
     }
     else if(driveMode==3)gSDMShootMode=11;
     displayedMode=0; 
     if(!conf.dist_mode && (gSDMShootMode==C_FAST)&&(conf.bracket_type>=3)&&(conf.bracket_type<=6)&& (conf.subj_dist_bracket_value==1))
      {
       if(conf.bracket_type>6)conf.bracket_type=6;      
       displayedMode=conf.bracket_type-2;          
      }
     else if(!conf.dist_mode && (gSDMShootMode==C_FAST)&&(conf.bracket_type>=3)&&(conf.bracket_type<=6) && (conf.subj_dist_bracket_value==2))
      {
       if(conf.bracket_type>6)conf.bracket_type=6; 
       displayedMode=19+conf.bracket_type-3; 
      }

     else if((conf.dist_mode==2) && (gSDMShootMode==C_FAST)&&shooting_can_focus()&& !conf.tv_bracket_value && (conf.FastLapseDelay!=65535))displayedMode=8;    
     else if((gSDMShootMode==9)&& (conf.user_1==80979))displayedMode=27;
     else if((gSDMShootMode==8)&& !conf.dist_mode && (conf.tv_bracket_value==3) && (conf.bracket_type==2))
      {
       displayedMode=((conf.burst_frame_count-1)/2)+8; 
      }
     else if(gSDMShootMode==4)displayedMode=5;
     else if((gSDMShootMode==2)&& !gHdrPhase &&(conf.bracket_type<3)&& conf.tv_bracket_value && !conf.fastlapse)
      {
       displayedMode=16+conf.bracket_type; 
      }
     else if((gSDMShootMode==6)&& !gHdrPhase &&(conf.bracket_type<3)&& conf.tv_bracket_value && !conf.fastlapse)
      {
       displayedMode=31+conf.bracket_type; 
      }
     else if((gSDMShootMode==5)&& (conf.FastLapseDelay<=10))displayedMode=13;
     else if(tl.running && (tl.focus_mode==1) && (tl.bracket_type==1) && (tl.sun==2))displayedMode=15;
     else if((gSDMShootMode==5)&& (conf.FastLapseDelay==65535))displayedMode=15;
     else if((gSDMShootMode==5)&& (conf.FastLapseDelay>10))displayedMode=14;
     else if((gSDMShootMode==7)&&driveMode!=3)displayedMode=6;
     else if((gSDMShootMode==3)&&(conf.user_1!=80979)&&driveMode!=3)displayedMode=7;
     else if(gSDMShootMode==9)displayedMode=23;  
     else if(gSDMShootMode==10)displayedMode=24;
     else if(gSDMShootMode==11)displayedMode=25; 
     else if(gSDMShootMode==12)     
      {
       if(!conf.strip_offset)displayedMode=29; 
       else displayedMode=30; 
      }   
     else if((gSDMShootMode==7)&&driveMode==3)displayedMode=25;
     else if((gSDMShootMode==3)&&driveMode==3)displayedMode=25;  
     else if((gSDMShootMode==2)&&conf.tv_override_enable){conf.tv_override_enable=0;displayedMode=25;}     
     if(displayedMode && (displayedMode<5)&&(prevDisplayedMode>4))hdrDark96=hdrLight96=0;  
       prevDisplayedMode=displayedMode;    


if((conf.dist_mode) && shooting_can_focus() && !digi_mode &&((((driveMode==1)||(driveMode==3))&&(conf.bracket_type>2))|| (driveMode== 3)))
    {
     digi_mode = 1;
     conf.synch_enable=1;
     previous_focus = conf.digi_ref_dist;
     inf_focus = 0;
     conf.user_range_set=0;                                      
    }
 

static unsigned char wasDisplayed;
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
if(state_kbd_script_run || MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK))
#else
if(state_kbd_script_run)
#endif
{
 if(gVideoReady && !wasDisplayed)
  {
   wasDisplayed=1;
   draw_filled_rect(((screen_width>>1)-18), (screen_height>>1)-18, ((screen_width>>1)+18),(screen_height>>1)+18, MAKE_COLOR(COLOR_RED,COLOR_RED));
  }
  
 else if(!gVideoReady && wasDisplayed)
  {
   wasDisplayed=0;
   needRestore=1;
  }
  
}

int vmode;
get_property_case(PROPCASE_SHOOTING_MODE, &vmode, 4);
vmode=shooting_mode_canon2chdk(vmode);
 
static unsigned char trees;
if(nTxzoom)                            
{
 if(ZoomIn )
  {
   if(shooting_get_zoom()!=(zoom_points-1))
    sprintf(osd_buf," %c ",(char)30);
   else
    sprintf(osd_buf,"%c%c%c",(char)30,(char)30,(char)30);
  }
 else  
  { 
   if(shooting_get_zoom())
    sprintf(osd_buf,"%c%c%c",(char)30,(char)30,(char)30);
   else
    sprintf(osd_buf," %c ",(char)30);
  }
  draw_string(screen_width/2-10,40,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
  if(!trees)trees=1;
}
else if(!nTxzoom && trees)
{
 trees=0;
 needRestore=1;
}

if((gZoomPoint==(zoom_points-1))&&displayedMode&&(displayedMode<=4) && (conf.user_1 != 80976)) 
{
 if(gHdrPhase>0)
 {
  sprintf(osd_buf,"%4d",hdrDark96);
  draw_string(124,40,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
 }
 if(gHdrPhase>1)
 {
  sprintf(osd_buf,"%4d",hdrLight96);
  draw_string(164,40,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
  int tmp = abs(hdrLight96-hdrDark96);
  sprintf(osd_buf,"%4d",tmp);
  draw_string(204,40,osd_buf,(tmp>1632)?MAKE_COLOR(COLOR_RED,COLOR_WHITE):MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
 }
}

#if defined(CAMERA_a495)|| defined(CAMERA_a490)
  if (!((canon_menu_active==(int)&canon_menu_active-4)))  return; 
#elif !defined(CAMERA_ixus40) && !defined(CAMERA_g7x) && !defined(CAMERA_sx280hs)
  if (!((canon_menu_active==(int)&canon_menu_active-4) && (canon_shoot_menu_active==0)))  return; 
#endif
  if((driveMode==3)&& !tl.running && conf.bracket_type>2)conf.bracket_type = 2;  
 if ((kbd_is_key_pressed(KEY_SHOOT_HALF)||get_soft_half_press())&&(gui_mode!=GUI_MODE_ALT)&& (mode_photo || (m&MODE_SHOOTING_MASK)==MODE_STITCH)) 
   {
      myav=shooting_get_real_av();                                                                         
      myhyp=shooting_get_hyperfocal_distance();
              if(kbd_is_key_pressed(KEY_SET))
              {
                if (!pressed) 
                 {
               #if CAM_HAS_ND_FILTER
                  if(conf.tv_override_enable || conf.av_override_value || conf.user_range_set || conf.nd_filter_state)
               #else
                  if(conf.tv_override_enable ||conf.av_override_value || conf.user_range_set)
               #endif
                  conf.disable_overrides = !conf.disable_overrides;
                  if(!conf.disable_overrides && conf.user_range_set && (conf.focus_mode==2))conf.distance_setting = current_focus;
                  if (!conf.show_osd) 
                   {
                    draw_restore();
                   }
                   pressed = 1;
                 }
               } 

            else 
               {
                pressed = 0;
               }

    } 

      if(((m&MODE_MASK)==MODE_REC) && conf.show_osd)
 {


    int myfl=gFocalPoint;                            
    int zp=gZoomPoint;
    int synch_x = conf.synch_pos.x;
    if((conf.camera_orientation==1)||(conf.camera_orientation==3))synch_x -= 120;                                   
 

if((conf.compact!=2)&&(recreview_hold==0)&& !inGridSelection()) 
 {
  header_on = 1;
 
  if(!conf.compact) 
  {
   if(mode_video || is_video_recording())
   {
    int av = shooting_get_real_aperture();
    sprintf(osd_buf,"Tv %5s Av %2d.%02d ",shooting_get_camera_tv_string(),av/100,av%100);  
    if(conf.zoom_value==1)sprintf(osd_buf+strlen(osd_buf),"%2d.%d",myfl/1000,(myfl%1000)/100);
    else if (!conf.zoom_value)sprintf(osd_buf+strlen(osd_buf),"%4d",zp);
    else if(conf.zoom_value==3)sprintf(osd_buf+strlen(osd_buf)," %2d%c",get_fov(zp),(char)31);
    else sprintf(osd_buf+strlen(osd_buf),"%4d",get_effective_focal_length(zp)/1000);
    sprintf(osd_buf+strlen(osd_buf),"%*s%05d",30-5-strlen(osd_buf),"",(conf.video_frame_count)?gFrameCount:gFramesRecorded); 
   }
   else if(!tl.running && !mplay) 
    {
     if(!displayedMode)										
      {
       sprintf(osd_buf,"%s%c%c ","Canon normal shooting mode ",(char)3,(char)4); 
      }
      else if(!((conf.flag_4==1234)&& shooting_can_focus() && (displayedMode==23)))  
      {
       if(displayedMode<=4)
         sprintf(osd_buf,"HDR %1d %s Lighter",displayedMode*2+1,(conf.user_1==80976)?"Easy":"Auto");          
       else if((displayedMode>18)&&(displayedMode<23))
         sprintf(osd_buf,"HDR %1d  User set   ",3+ (2*(displayedMode-19)));  
       else if((displayedMode>15)&&(displayedMode<19))                       
         sprintf(osd_buf,"HDR Timer %-10s ",(displayedMode==16)?"Lighter":(displayedMode==17)?"Darker":"Alternate"); 
       else if((displayedMode>30)&&(displayedMode<34))                       
         sprintf(osd_buf,"3D HDR %-10s ",(displayedMode==31)?"Lighter":(displayedMode==32)?"Darker":"Alternate");
       else if (displayedMode==5)
         sprintf(osd_buf,"BURST for %3d shots ",conf.burst_frame_count);
       else if(displayedMode==6)  
		sprintf(osd_buf,"STEREO ");
       else if (displayedMode==7)
         sprintf(osd_buf,"2D FAST with switch ");
       else if((displayedMode>=9)&& (displayedMode<=12))
        sprintf(osd_buf,"3d HDR %1d Alternating ",(displayedMode-8)*2+1);
       else if(displayedMode==13)
        sprintf(osd_buf,"3d SPORTS - %2d shots",conf.burst_frame_count);
       else if(displayedMode==14)
        sprintf(osd_buf,"3dTLAPSE %3d shots ",conf.burst_frame_count);
       else if (displayedMode==23)
          sprintf(osd_buf,"%-21s","Canon Single Shot");
       else if (displayedMode==24)
        sprintf(osd_buf,"%-21s","Canon Continuous");
       else if (displayedMode==25)
        sprintf(osd_buf,"%-21s","Canon Custom Timer");  
       else if (displayedMode==27)
        sprintf(osd_buf,"DIGISCOPE  %5d mm  ",(lens_get_focus_pos_from_lens()==-1)?MAX_DIST:lens_get_focus_pos_from_lens()); 
       else if (displayedMode==29)
        sprintf(osd_buf,"%-21s","Streak image");  
       else if (displayedMode==30)
       {
        sprintf(osd_buf,"%-21s","Stereo Strip images");
        int offset = 120-(conf.strip_offset*120/100);  
        int widthPixels = ((conf.strip_width/2)*240)/CAM_JPEG_HEIGHT;
        int bsw=vid_get_bitmap_screen_width();
        bsw = (bsw==360);
        draw_line((bsw)?50:60, offset-widthPixels,(bsw)?310:420, offset-widthPixels, MAKE_COLOR(COLOR_RED,COLOR_RED));
        draw_line((bsw)?50:60, offset+widthPixels,(bsw)?310:420 , offset+widthPixels, MAKE_COLOR(COLOR_RED,COLOR_RED));
        offset = 120+(conf.strip_offset*120/100);  
        draw_line((bsw)?50:60, offset-widthPixels,(bsw)?310:420, offset-widthPixels, MAKE_COLOR(COLOR_BLUE,COLOR_BLUE));
        draw_line((bsw)?50:60, offset+widthPixels,(bsw)?310:420 , offset+widthPixels, MAKE_COLOR(COLOR_BLUE,COLOR_BLUE));
       }  
#if !defined(CAMERA_m3)      
     if ((displayedMode!=6)&&(displayedMode!=15))			
       sprintf(osd_buf+strlen(osd_buf),"%*s",26-strlen(osd_buf),"Zoom:");
     else if(displayedMode==6)
       sprintf(osd_buf+strlen(osd_buf),"Zoom:");
       if(displayedMode!=15)							
        {
         if(conf.zoom_value==1)sprintf(osd_buf+strlen(osd_buf),"%2d.%d",myfl/1000,(myfl%1000)/100);
         else if (!conf.zoom_value)sprintf(osd_buf+strlen(osd_buf),"%4d",zp);
         else if(conf.zoom_value==3)sprintf(osd_buf+strlen(osd_buf)," %2d%c",((conf.camera_orientation==1)||(conf.camera_orientation==3))?(int)(((30*get_fov(zp)/4)+5)/10):get_fov(zp),(char)31);
         else sprintf(osd_buf+strlen(osd_buf),"%4d",get_effective_focal_length(zp)/1000);
        }  
#endif  
      }  
   }     
  else if(tl.running)
   {
    struct tm *ttm = localtime(&gScriptStart);
    sprintf(osd_buf,"Timelapse started %02d:%02d   ",ttm->tm_hour,ttm->tm_min);          
    if(conf.dist_mode && shooting_can_focus()) sprintf(osd_buf+strlen(osd_buf),"%5d",lens_get_focus_pos_from_lens());
    else if(conf.save_xml_file)sprintf(osd_buf+strlen(osd_buf),"XML");
    sprintf(osd_buf+strlen(osd_buf),"%*s",30-strlen(osd_buf),"");    
    draw_string(synch_x-6*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
   }  
 
if(!((conf.flag_4==1234)&& shooting_can_focus() && (displayedMode==23)))  
{
    if(((!(conf.dist_mode && shooting_can_focus()) || (displayedMode==7)|| (displayedMode==15)|| (displayedMode==24)|| (displayedMode==25)|| (displayedMode==26)) && !tl.running)||mode_video||is_video_recording()) 
     {
      if((displayedMode==6)&& !conf.bright_screen && !(mode_video || is_video_recording())) 
       {
         char buf[5];
#if CAM_CURTAIN
            if((shooting_will_use_flash()||gSDM_will_use_flash|| (flashStatus==1)|| conf.slave_flash)&& !shooting_get_prop(PROPCASE_CURTAIN)) 
#else
            if((shooting_will_use_flash()||gSDM_will_use_flash|| (flashStatus==1)|| conf.slave_flash) )
#endif
		    { 
 	
          if(!conf.camera_position && !conf.slave_flash && (shooting_will_use_flash()||gSDM_will_use_flash|| (flashStatus==1)))
          {
           if(!conf.precision_synch_flash)
            sprintf(osd_buf+strlen(osd_buf),(conf.left_flash_dim)?" M ":" A ");
           else
            sprintf(osd_buf+strlen(osd_buf),(conf.left_flash_dim)?" MP":" AP");           
          }
          
          else if(conf.slave_flash)
           {   
            if(!conf.precision_synch_flash)           
             sprintf(osd_buf+strlen(osd_buf),"%s%1d", " S",conf.slave_flash); 
            else
             sprintf(osd_buf+strlen(osd_buf),"%s%1d %s"," S",conf.slave_flash,"P");             
           }          
          
 	
         else if(conf.camera_position &&conf.synch_delay_enable&& (shooting_will_use_flash()||gSDM_will_use_flash|| (flashStatus==1))&& !conf.bright_screen && !conf.slave_flash)
          sprintf(osd_buf+strlen(osd_buf),(conf.precision_synch_flash)?" P":" F");
#if defined(PROPCASE_FLASH_EXP_COMP) 
          anyTempValue=strlen(osd_buf); 
          strcpy(buf,"    ");          
          if((conf.flash_exp_comp!=9)&&!(kbd_is_key_pressed(KEY_SHOOT_HALF)) && !conf.slave_flash)
          {
           get_flash_ev_comp(buf,conf.flash_exp_comp);
           if(conf.camera_position)
            sprintf(osd_buf+anyTempValue," %4s",buf);   
           else
             sprintf(osd_buf+anyTempValue,"    ");          
          }
#endif          
      }    
       if (!(kbd_is_key_pressed(KEY_SHOOT_HALF)))
       {
        if((conf.synch_fine_delay||conf.synch_coarse_delay)&&conf.add_synch_delays)  
          {
           n = conf.synch_fine_delay+(conf.synch_coarse_delay*1000);
#if !defined(PROPCASE_FLASH_EXP_COMP) 
          sprintf(osd_buf+strlen(osd_buf),"     %d.%04d",n/10000,n%10000);
#else       
        if(shooting_will_use_flash()||gSDM_will_use_flash)
        {
          if((conf.flash_exp_comp!=9) && !conf.slave_flash)  
          sprintf(osd_buf+strlen(osd_buf),"%*s%d.%04d",30-strlen(osd_buf)-strlen(buf)-6," ",n/10000,n%10000);
          else if((conf.flash_exp_comp==9) && !conf.slave_flash)
          sprintf(osd_buf+strlen(osd_buf),"%*s%d.%04d",30-strlen(osd_buf)-6," ",n/10000,n%10000);
          else if(conf.slave_flash)   
          {
           if(conf.precision_synch_flash)
             sprintf(osd_buf+strlen(osd_buf),"%*s%d.%04d",30-strlen(osd_buf)-6," ",n/10000,n%10000);
           else sprintf(osd_buf+strlen(osd_buf),"     %d.%04d",n/10000,n%10000);
           sprintf(osd_buf+strlen(osd_buf),"%*s%d.%04d",30-strlen(osd_buf)-6," ",n/10000,n%10000);
          }
        }
        else
          sprintf(osd_buf+strlen(osd_buf),"%*s%d.%04d",30-strlen(osd_buf)-6," ",n/10000,n%10000);        
#endif
          }
        else sprintf(osd_buf+strlen(osd_buf),"%*s",30-strlen(osd_buf)," ");   
        
       }
       
 #if CAM_CURTAIN
          else  if((shooting_will_use_flash()|| (flashStatus==1))&&conf.camera_position &&conf.synch_delay_enable&& !shooting_get_prop(PROPCASE_CURTAIN)) 
#else
         else   if((shooting_will_use_flash()|| (flashStatus==1))&&conf.camera_position&&conf.synch_delay_enable)
#endif     
            {
               n = conf.synch_flash_delay;
#if !defined(PROPCASE_FLASH_EXP_COMP)                
               sprintf(osd_buf+strlen(osd_buf),"     %d.%04d",n/10000,n%10000);
#else
          if((conf.flash_exp_comp!=9) && !conf.slave_flash)                                      
           sprintf(osd_buf+anyTempValue,"%-4s  %d.%04d",buf,n/10000,n%10000);
          else                                                                
           sprintf(osd_buf+anyTempValue,"%*s%d.%04d",30-anyTempValue-6," ",n/10000,n%10000);
#endif
            }
            else sprintf(osd_buf+strlen(osd_buf),"%*s",30-strlen(osd_buf)," ");
      }       
 	
     else if(conf.bright_screen && !conf.slave_flash)
      {
       if(!conf.precision_synch_flash) 
        sprintf(osd_buf+strlen(osd_buf),"%s",((conf.camera_orientation==1)||(conf.camera_orientation==3))?"B ":" B ");
       else
        sprintf(osd_buf+strlen(osd_buf),"%s",((conf.camera_orientation==1)||(conf.camera_orientation==3))?"BP":" BP");
       sprintf(osd_buf+strlen(osd_buf),"%*s",30-strlen(osd_buf)," ");
      }
         draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
     } 
     
   }  
      else if((conf.flag_4==1234)&& shooting_can_focus()&& state_kbd_script_run && (displayedMode==23)) 
       {
        int fpfl=lens_get_focus_pos_from_lens();
        if(fpfl==-1)
        {
         if(MAX_DIST>99999)
          draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y,"NEAR/FAR FOCUS Stack  INFINITY",(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
         else 
         {
          sprintf(osd_buf,"NEAR/FAR FOCUS Stack  %5d mm",MAX_DIST);
          draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);        
         }
        }
        else
        {
         sprintf(osd_buf,"NEAR/FAR FOCUS Stack  %5d mm",fpfl);
         draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
        }
       }       
 
   osd_buf[0] = '\0';                        
   
   if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))           
   {
    if((conf.user_1!=80973)&&(mode_video|| is_video_recording()||((gui_mode==GUI_MODE_ALT)&& mode_video &&(conf.video_synch_device==2)))) 
    {
#if !defined(VIDEO_STD)
     if(conf.camera_position && ((!is_video_recording()&&MODE_IS_SYNCH_VIDEO(vmode))||(is_video_recording()&&!MODE_IS_SYNCH_VIDEO(vmode)))&& !kbd_blocked &&(LowestCurCnt!=9999))  // right camera on sync completion
#else		
     if(conf.camera_position && !is_video_recording()&& !kbd_blocked &&(LowestCurCnt!=9999))  // right camera on sync completion
#endif
      {
       int tmp1,tmp2;
       tmp2 = LowestCurCnt+(STD_PERIOD/100);
       if(tmp2>STD_PERIOD)tmp2=tmp2%STD_PERIOD;
       tmp1 = LowestCurCnt-(STD_PERIOD/100);
       if(tmp1<0)tmp1=STD_PERIOD+tmp1;     // because tmp1 is negative   
       sprintf(osd_buf,"ISO %4d %4d/%4d->%4d",shooting_get_iso_market(),LowestCurCnt,tmp1,tmp2);
      } 
#if !defined(VIDEO_STD)
    else if(!conf.camera_position && ((!is_video_recording()&&MODE_IS_SYNCH_VIDEO(vmode))||(is_video_recording()&&!MODE_IS_SYNCH_VIDEO(vmode)))&& !kbd_blocked &&(LowestCurCnt!=9999))  // right camera on sync completion
#else		
    else if(!conf.camera_position && !is_video_recording()&& !kbd_blocked &&(LowestCurCnt!=9999))  // left camera on sync completion
#endif          
      {
       sprintf(osd_buf,"ISO %4d %4d/%4d",shooting_get_iso_market(),LowestCurCnt,STD_PERIOD);
      }
      
      else if((LowestCurCnt!=9999)&&kbd_blocked)
      {
        sprintf(osd_buf,"ISO %4d %4d",shooting_get_iso_market(),LowestCurCnt);     
      }
         
      else sprintf(osd_buf,"ISO %4d",shooting_get_iso_market());   
     sprintf(osd_buf+strlen(osd_buf),"%*s%s",30-4-strlen(osd_buf),"",(conf.video_synch_device==1)?"OPTO":(conf.video_synch_device==2)?"USB+":(conf.video_synch_device==3)?"USB ":"    ");   
    }
    
    else if(tl.running) 
     {
      sprintf(osd_buf,"%-30s","Full-press shutter to stop.");
      draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);
     }
    else if((conf.user_1!=80973)&&displayedMode && !tl.running && !((gui_mode==GUI_MODE_ALT)&& mode_video &&(conf.video_synch_device==2)))  
    {
     if(gui_mode!=GUI_MODE_ALT)
     {
     if(!((conf.flag_4==1234)&& shooting_can_focus()&& state_kbd_script_run && (displayedMode==23))) 
     {
     int t;
       if(displayedMode<=4)                                         
        {
       if((!hdrDark96&&!hdrLight96)||!gHdrPhase)
        sprintf(osd_buf,"%-30s","Take spotmeter reading");
       else if(hdrDark96&&!hdrLight96)
        sprintf(osd_buf,"%-30s","First spotmeter reading OK !");
       else if(hdrDark96&&hdrLight96&&(gHdrPhase==2))
        sprintf(osd_buf,"%-30s","Zoom - Half press for Tv range");
        }
  else 
  {
      int intValue;
      unsigned char spaces;
      char normTv[12],overTv[12];
      
     if((conf.user_1==80975) ||(conf.user_1==80977))                                       
     {
      intValue=(int)(shooting_get_shutter_speed_from_tv96(onHalfPress.Tv96)*100000);   
      if(intValue<=50000)                                   
       {
        sprintf(normTv,"1/%d",100000/intValue);
        spaces=7-strlen(normTv);
        if(spaces)sprintf(normTv+strlen(normTv),"%*s",spaces," ");
       }
      else if((intValue>50000) && (intValue<95000))        
       {
        sprintf(normTv,"0.%1d    ",intValue/10000);
       }
      else if((intValue>=95000) && (intValue < 110000))    
       sprintf(normTv,"1      ");
      else sprintf(normTv,"%2d.%1d",intValue/100000,((intValue+5000)%100000)/10000); 
 
      intValue=shooting_get_prop(PROPCASE_CAMERA_TV);
      intValue=(int)(shooting_get_shutter_speed_from_tv96(intValue)*100000);
      
      if(intValue<=50000) 
       {
        sprintf(overTv,"1/%d",100000/intValue);
        spaces=7-strlen(overTv);
        if(spaces)sprintf(overTv+strlen(overTv),"%*s",spaces," ");
       }
      else if((intValue>50000) && (intValue<95000))        
       {
        sprintf(overTv,"0.%1d    ",intValue/10000);
       }       
      else if((intValue>=95000) && (intValue < 110000))    
       sprintf(overTv,"1      ");      
      else sprintf(overTv,"%2d.%1d",intValue/100000,((intValue+5000)%100000)/10000);         
      sprintf(osd_buf," ISO %4d  Tv %s %s ",shooting_get_iso_market(),normTv,overTv);
     }     
     else if((displayedMode>22)&&(displayedMode<26))   
     {
      sprintf(osd_buf,"%-30s","MENU fast/slow for EasyMode");
     }     
     else if((displayedMode>15)&&(displayedMode<19))   
     {
      if(conf.tv_bracket_value)
       {
        if(!(conf.tv_bracket_value%3)) sprintf(osd_buf,"Creative Exposure step %1d EV   ",conf.tv_bracket_value/3);
        else sprintf(osd_buf,"Creative Exposure step %1d/3 EV ",conf.tv_bracket_value);
       }
      else sprintf(osd_buf,"Set EV step in Creative Menu  ");
     }
      else if((displayedMode>30)&&(displayedMode<34))    
     {
       intValue=(int)(shooting_get_shutter_speed_from_tv96(Hdr3dTv96)*100000);   
      if(intValue<=50000)                                   
       {
        sprintf(normTv,"1/%d",100000/intValue);
        spaces=7-strlen(normTv);
        if(spaces)sprintf(normTv+strlen(normTv),"%*s",spaces," ");
       }
      else if((intValue>50000) && (intValue<95000))        
       {
        sprintf(normTv,"0.%1d    ",intValue/10000);
       }
      else if((intValue>=95000) && (intValue < 110000))    
       sprintf(normTv,"1      ");
      else sprintf(normTv,"%2d.%1d",intValue/100000,((intValue+5000)%100000)/10000); 
     
      sprintf(osd_buf,"Starting Tv %s ",normTv);
      if(conf.tv_bracket_value)
       {
        if(!(conf.tv_bracket_value%3)) sprintf(osd_buf+strlen(osd_buf),"   %1d EV   ",conf.tv_bracket_value/3);
        else sprintf(osd_buf+strlen(osd_buf),"   %1d/3 EV ",conf.tv_bracket_value);
       }
      else sprintf(osd_buf+strlen(osd_buf),"No EV step");
      
     }
     else if(displayedMode==5)
      {
       if(shooting_in_progress())sprintf(osd_buf,"Switch/HP->start/stop Shot %2d ",burstCount);
       else 
        {
		 if(FastLapseEnd>FastLapseStart)
		 {
          int tmp = ((burstCount-1)*100000)/(FastLapseEnd-FastLapseStart);
          sprintf(osd_buf,"Switch/HP->start/stop FPS %1d.%02d",tmp/100,tmp%100);
		 }
        }
      }
     else if(displayedMode==6)  
      {
#if !defined(CAMERA_m3) 		  
       sprintf(osd_buf,"Zoom:%3s Focus:%3s %3s %3s %3s",(conf.remote_zoom_enable)?" ON":"OFF",(conf.dc)?" ON":"OFF",(conf.on_release)?" \x0A\x0B":" \x09\x09",(conf.save_raw)?"DNG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");
#else
       sprintf(osd_buf,"%8s Focus:%3s %3s %3s %3s","",(conf.dc)?" ON":"OFF",(conf.on_release)?" \x0A\x0B":" \x09\x09",(conf.save_raw)?"DNG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");		   
#endif
      }
     else if(displayedMode==7) 
      {
       if(conf.user_1 !=80972)
        sprintf(osd_buf,"%19s%3s %3s %3s","",(conf.save_raw)?"DNG":"",(conf.save_auto)?"LOG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");
       else
        sprintf(osd_buf,"%19s%3s %3s %3s","",(conf.save_raw)?"DNG":"",(conf.save_auto)?"LOG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");       
     }
 
     else if(displayedMode==8) 
      {
       if(gFarPoint>99999)
        sprintf(osd_buf,"Near %5d Far %5s Shift %2d ",gNearPoint,"INFIN",gStereoShift);
       else
        sprintf(osd_buf,"Near %5d Far %5d Shift %2d ",gNearPoint,gFarPoint,gStereoShift); 
      }      
     else if((displayedMode>=9)&& (displayedMode<=12))
      {
       t = shooting_get_prop(PROPCASE_CAMERA_TV);
       sprintf(osd_buf,"Exposures %6s to %6s sec",get_tv_string_for_tv96(t+((displayedMode-8)*96)),get_tv_string_for_tv96(t-((displayedMode-8)*96)));
      }
 
     else if(displayedMode==13)
      {
	 gFPS= ((burstCount-1)*100000)/(FastLapseEnd-FastLapseStart);  // moved here from spytask() because sometimes incorrect.
       if(conf.FastLapseDelay)
	   {
		sprintf(osd_buf,"Target FPS    %1d.%01d : FPS %1d.%02d ",10/conf.FastLapseDelay,1000/conf.FastLapseDelay%100,gFPS/100,gFPS%100);
	   }
       else sprintf(osd_buf,"%-20s: FPS %1d.%02d","Infinite", gFPS/100,gFPS%100);
      }
     else if(displayedMode==14)
      {
       sprintf(osd_buf,"%2d.%01d sec interval  %3s %3s %3s",conf.FastLapseDelay/10,conf.FastLapseDelay%10,(conf.save_raw)?"DNG":"",(conf.save_auto)?"LOG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");
      }
 
  else if((displayedMode==29)||(displayedMode==30))   
     {
      if(displayedMode==30)
       {
        cb_slitcam_menu_change(); 
        sprintf(osd_buf,"Vertical,%3d strip rotate %4d",conf.strip_width,gRotationDistance);
       }
      else
       sprintf(osd_buf,"Vertical %4d width %3d strips",conf.strip_width,conf.strip_images);
     } 
     else
      sprintf(osd_buf,"%18s %3s %3s %3s","",(conf.save_raw)?"DNG":"",(conf.save_auto)?"LOG":"",(conf.save_xml_file && !conf.outline_mode)?"XML":"");
     }   
    } 
   } 
   else if(!state_kbd_script_run) 
   {
    if(!conf.alt_files_shortcut)  
    {
    if(displayedMode && ((displayedMode<5)||(displayedMode>7&&displayedMode<13)||(displayedMode>15&&displayedMode<23)||(displayedMode>30&&displayedMode<34)))     
     {
      sprintf(osd_buf,"%-30s","DISP/PLAY --> STACKS folder");
     }
    else if(conf.save_xml_file)  
     {
      sprintf(osd_buf,"%-30s","DISP/PLAY --> XML folder");
     }
     else sprintf(osd_buf,"%-30s","DISP/PLAY --> TEXTS folder");  
     }
     else sprintf(osd_buf,"%-30s","DISP/PLAY --> USER SCRIPTS"); 
     draw_string(synch_x-6*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);     
   }  
   else  
    {
     if(displayedMode==27)
     {
      int ra = shooting_get_real_aperture();  
      int ep = get_focal_length(lens_get_zoom_point())/ra;  
      int mag = ((conf.digi_scope_fl*10)+5)/conf.digi_ep_fl; 
      int xp = ((conf.digi_obj_diam*100)+50)/mag;    
      int efl = mag*get_effective_focal_length(lens_get_zoom_point()); 
      sprintf(osd_buf,"M %2d EFL %5d XP %1d.%1d EP %2d.%1d ",mag/10,efl/10000,xp/10,xp%10,ep/10,ep%10);  
      draw_string(synch_x-6*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
     }
    }
   }  
    
    else if(!state_kbd_script_run &&(gui_mode!=GUI_MODE_ALT))   
     {
      if(!((conf.flag_4==1234)&& state_kbd_script_run && shooting_can_focus() && (displayedMode==23)))
       sprintf(osd_buf,"%-30s","MENU fast/slow for EasyMode");
     } 
    else if(shooting_can_focus()&& (gui_mode==GUI_MODE_ALT)&& state_kbd_script_run && (conf.user_1==80973))
     {
      if(lens_get_focus_pos_from_lens()==-1)sprintf(osd_buf,"Focus INFINITY  Mecha pos %4d",GetMechaPos());
      else sprintf(osd_buf,"Focus %7dmm Mecha pos %4d",lens_get_focus_pos_from_lens(),GetMechaPos());
      draw_string(synch_x-6*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
     }    
 
      int xpos=synch_x-6*FONT_WIDTH;
       if((gui_mode!=GUI_MODE_ALT)&&(((gHdrPhase==3)&&(displayedMode<=4)&&(conf.user_1!=80973))||(conf.user_1==80976))) 
        {
         if(gTooBright && (conf.user_1!=80976)) 
         {
          sprintf(osd_buf,"Use ND%-2d or low ISO ",hdrND);
          draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BLACK,COLOR_RED),1);
          xpos+=strlen(osd_buf)*FONT_WIDTH;
         }
         else
         {
          sprintf(osd_buf,"Exposures ");
          draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1); 
          xpos+=10*FONT_WIDTH;
          sprintf(osd_buf,"%6s",get_tv_string_for_tv96(gTv96max));
          draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,(gTooBright)?MAKE_COLOR(COLOR_BLACK,COLOR_RED):MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1); 
          xpos+=6*FONT_WIDTH;
          draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT," to ",MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1);
          xpos+=4*FONT_WIDTH;
         }
         sprintf(osd_buf,"%6s",get_tv_string_for_tv96(gTv96min));
         draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,(gTooDark)?MAKE_COLOR(COLOR_BLACK,COLOR_RED):MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1);     
         xpos+=6*FONT_WIDTH;
         draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT," sec",MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1);
        }
    else if((gui_mode!=GUI_MODE_ALT)&&(displayedMode>18)&&(displayedMode<23)&&(conf.user_1!=80973))   
     {
       int xpos = synch_x-6*FONT_WIDTH;  
       sprintf(osd_buf,"%6s",get_tv_string_for_tv96(gTv96max));
       draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,(gTooBright)?MAKE_COLOR(COLOR_BG,COLOR_RED):MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
       xpos+=6*FONT_WIDTH;
       draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT," to ",MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);
       xpos+=4*FONT_WIDTH;
       sprintf(osd_buf,"%6s",get_tv_string_for_tv96(gTv96min));
       draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,(gTooDark)?MAKE_COLOR(COLOR_BG,COLOR_RED):MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);     
       xpos+=6*FONT_WIDTH;
       draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT," sec ",MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);
       xpos+=5*FONT_WIDTH;
       if(!(conf.tv_bracket_value%3))sprintf(osd_buf,"at %1d EV  ",conf.tv_bracket_value/3);
       else sprintf(osd_buf,"at %1d/3 EV",conf.tv_bracket_value);
       draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
     }  
               
       else if(!state_kbd_script_run)
         draw_string(synch_x-6*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
         
      if((conf.flag_4==1234)&& state_kbd_script_run && shooting_can_focus() && (displayedMode==23))
      {
       if(gFarPoint>99999)
        sprintf(osd_buf,"Near %5d Far %5s Shift %2d ",gNearPoint,"INFIN",gStereoShift);
       else
        sprintf(osd_buf,"Near %5d Far %5d Shift %2d ",gNearPoint,gFarPoint,gStereoShift); 
      draw_string(xpos,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1); 
     }       
  } 
 } 
 
  else if(!tl.running && conf.compact) 
    {
     synch_x+=14*FONT_WIDTH;
     if(conf.zoom_value==1)sprintf(osd_buf,"%2d.%d",myfl/1000,(myfl%1000)/100);
     else if (!conf.zoom_value)sprintf(osd_buf,"%4d",zp);
     else if(conf.zoom_value==3)sprintf(osd_buf," %2d%c",get_fov(zp),(char)31);
     else sprintf(osd_buf,"%4d",get_effective_focal_length(zp)/1000);
     draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?0:synch_x-6*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
 
     if(displayedMode==6)  
      {        
        if (kbd_is_key_pressed(KEY_SHOOT_HALF) || get_soft_half_press())            
         n = conf.synch_coarse_delay * 1000 + conf.synch_flash_delay;
        else n = conf.synch_fine_delay;
        if(conf.synch_delay_enable)
         {
         sprintf(osd_buf," %d.%04d",n/10000,n%10000);

         }
        else
         {
          sprintf(osd_buf,"%7s","");
         }

#if CAM_CURTAIN
            if(conf.camera_position && (shooting_will_use_flash()|| (flashStatus==1)||conf.slave_flash)&& !shooting_get_prop(PROPCASE_CURTAIN) ) 
#else
            if(conf.camera_position && (shooting_will_use_flash()|| (flashStatus==1)||conf.slave_flash))
#endif
		{     
          if(conf.slave_flash)
             {
              sprintf(osd_buf+strlen(osd_buf)," S%1d ",conf.slave_flash);             
             }
 	
            else if(conf.bright_screen && (flashStatus==1))
             {
              sprintf(osd_buf+strlen(osd_buf),"%s",((conf.camera_orientation==1)||(conf.camera_orientation==3))?"B ":" B  ");
             }
            else if(!conf.camera_position && !conf.bright_screen && (shooting_will_use_flash()|| (flashStatus==1)))
              sprintf(osd_buf+strlen(osd_buf)," D  ");            

            else if(conf.camera_position && (shooting_will_use_flash()|| (flashStatus==1))&& !conf.bright_screen  && !conf.slave_flash)
              sprintf(osd_buf+strlen(osd_buf)," F  ");

	
           else if(!conf.camera_position && conf.bright_screen)
            {
           sprintf(osd_buf+strlen(osd_buf),"%s",((conf.camera_orientation==1)||(conf.camera_orientation==3))?"B  ":" B   ");
            }

           else 
            {
             sprintf(osd_buf+strlen(osd_buf),((conf.camera_orientation==1)||(conf.camera_orientation==3))?"   ":"    ");
            } 
         } 
       else                             
        {
          sprintf(osd_buf,"%11s","");
        }       
         draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?4*FONT_WIDTH:synch_x-FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);      
         } 
    } 
    
  } 
  
 else  

  {
   if(header_on ) 
     {
      header_on = 0;
      needRestore=1;
     }
   }
   
      int ovr_y = conf.overrides_pos.y;
      int ovr_x = conf.overrides_pos.x;

      static int usb_cnt,pw;
 
   if((!shooting_in_progress()||conf.nd_filter_state)&&!inGridSelection())  
    {
      if(get_usb_power(1))                                                          
       {
        usb_on = 1;
        draw_string(ovr_x,ovr_y,"USB ",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;
       }

       else if(!get_usb_power(1) && usb_on)                                         
       {
        if(!pw)pw=get_usb_power(0);                                                 
        if(++usb_cnt<20)                                                               
         {
          sprintf(osd_buf,"%-4d",pw*10);
          draw_string(ovr_x,ovr_y,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
          ovr_y += FONT_HEIGHT;
         }
        else                                                                        
         {
          pw=0;
          usb_cnt=0;
          usb_on = 0;
          needRestore=1;
         }
       }

	   if(conf.save_xml_file)
	   {
		draw_string(ovr_x,ovr_y,"XML",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;    
	   }

      if(afl)
       {
        afl_on=1;
        draw_string(ovr_x,ovr_y,"AFL",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;
       }
      else if(!afl && afl_on)
       {
        afl_on=0;
        needRestore=1;
       }

      if(ael)
       {
        ael_on=1;
        draw_string(ovr_x,ovr_y,"AEL",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;
       }
      else if(!ael && ael_on)
       {
        ael_on=0;
        needRestore=1;
       }
 
#if OPT_SUNSET
      if(conf.sunrise && (shooting_get_drive_mode()==1))
       {
        sun_on=1;
        draw_string(ovr_x,ovr_y,"SUN",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;
       }
      else if(!(conf.sunrise && (shooting_get_drive_mode()==1)) && sun_on)
       {
        sun_on=0;
        draw_string(ovr_x,ovr_y,"   ",MAKE_COLOR(COLOR_TRANSPARENT,COLOR_TRANSPARENT),1);
       }
#endif

      if(conf.disable_overrides)
      {
        draw_string(ovr_x,ovr_y,"OFF",(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        ovr_y += FONT_HEIGHT;
      }
      else if(!(tl.running && (tl.shoot_mode==3)))                                
      { 
        if(conf.raw_nr==2)                                                                                
        {
         draw_string(ovr_x,ovr_y,"NR ", MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
         ovr_y += FONT_HEIGHT;
        }
        if(CAM_PROPSET<4)
        {
         unsigned char ismode=shooting_get_prop(PROPCASE_IS_MODE);
         if(ismode!=3)
          draw_string(ovr_x,ovr_y,(!ismode)?"ISc":(ismode==1)?"ISs":"ISp", MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
        }
        else if(CAM_PROPSET==4)
        {
         unsigned char ismode=shooting_get_prop(PROPCASE_IS_MODE);
         if(ismode!=4)
          draw_string(ovr_x,ovr_y,(!ismode)?"ISc":"ISs", MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);       
         }
       else if(CAM_PROPSET==6)
        {
         unsigned char ismode=shooting_get_prop(PROPCASE_IS_MODE);
         if(ismode!=2)
          draw_string(ovr_x,ovr_y,(!ismode)?"ISc":"ISs", MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);       
         }
        else if(CAM_PROPSET==5)
         {
          unsigned char ismode=shooting_get_prop(PROPCASE_IS_MODE);
         if(ismode!=4)
          draw_string(ovr_x,ovr_y,(!ismode)?"ISc":(ismode==2)?"ISs":"ISp", MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
         }         
        
        if(conf.tv_override_enable)
        {
         draw_string(ovr_x,ovr_y,"Tv ",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
         ovr_y += FONT_HEIGHT;
        }
       
        if(conf.av_override_value)
        {
         draw_string(ovr_x,ovr_y,"Av ",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
         ovr_y += FONT_HEIGHT;
        }
        
        if(conf.iso_override)
        {
         draw_string(ovr_x,ovr_y,"ISO",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
         ovr_y += FONT_HEIGHT;
        }        
 
        #if CAM_HAS_ND_FILTER
      
        if(conf.nd_filter_state)                                                                             
          {
           nd_on = 1;
           draw_string(ovr_x,ovr_y,(mss_putoff.nd)?"IN":"OUT",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
          }

       else if(!conf.nd_filter_state && nd_on)                                                              
         {
          nd_on = 0;                                                                                          
          ovr_y += FONT_HEIGHT;
          needRestore=1;
         }
       #endif
      } 
          if(((driveMode==1)||(driveMode==3)) && !(tl.running && (tl.shoot_mode==3)))   
        {
           if(conf.tv_bracket_value && !conf.dist_mode)                         
           {
            draw_string(ovr_x,ovr_y,"TvB",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
            ovr_y += FONT_HEIGHT;
            if(!conf.fastlapse)draw_string(ovr_x,ovr_y,(driveMode==1)?"CONT":"TIME",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
           }
           if(displayedMode==26 ) 
           {
            draw_string(ovr_x,ovr_y,"DB ",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
            ovr_y += FONT_HEIGHT;
            draw_string(ovr_x,ovr_y, "CONT",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
  }

          if(displayedMode==8)
           {
            draw_string(ovr_x,ovr_y,"DBA",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
            ovr_y += FONT_HEIGHT;
            draw_string(ovr_x,ovr_y, "CONT" ,(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
           }
         }
         
            if(displayedMode==27)   
            {
             draw_string(ovr_x,ovr_y,"DIG",(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
            }
           if ((m&MODE_MASK) == MODE_REC &&(recreview_hold==0)) gui_batt_draw_osd();
       }  
    }
    if (conf.show_grid_lines || conf.enable_yaw_guideline) 
      {
       gui_grid_draw_osd(1);
       if(inGridSelection())
       {
       if(strlen(gridShortName)==1)
        draw_string((screen_width>>1)-4,(screen_height>>1)-40,gridShortName,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
       else
        draw_string((screen_width>>1)-((strlen(gridShortName)*FONT_WIDTH)/2),(screen_height>>1)-40,gridShortName,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
       }
      }
 if(!inGridSelection())
 {
	if(((m&MODE_MASK)==MODE_REC)&&kbd_is_key_pressed(KEY_MENU)&&kbd_is_key_pressed(KEY_SHOOT_HALF)&&(gui_mode==GUI_MODE_NONE))
	{
	 unsigned char xpos,ypos,size = 3;
	 int r;
	  sprintf(osd_buf, "SDM %s",sdmversion);  
	  xpos=(screen_width-(FONT_WIDTH*size*(strlen(sdmversion)+4)))>>1;
      ypos=(screen_height>>1)-(FONT_HEIGHT*size);
      draw_string(xpos,ypos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),size);  
      ypos += FONT_HEIGHT*size;
	  get_parameter_data(1,&r,4);
	  sprintf(osd_buf, "%8d",r); 
      draw_string(xpos,ypos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),size); 	  
	}	
    if (debug_vals_show)     
     {
      sprintf(osd_buf, "Memory %8d",get_memory_info());
      draw_txt_string(28, 9, osd_buf,conf.osd_color);

	sprintf(osd_buf, "0:%8x ",physw_status[0]);
	draw_txt_string(28, 10, osd_buf,conf.osd_color);

      sprintf(osd_buf, "1:%8x ",physw_status[1]);
	draw_txt_string(28, 11, osd_buf,conf.osd_color);

      sprintf(osd_buf, "2:%8x ",physw_status[2]);
	draw_txt_string(28, 12, osd_buf,conf.osd_color);

     }
 if(((m&MODE_MASK)==MODE_REC) && conf.show_osd)
  { 
      if (((m&MODE_MASK) == MODE_REC) &&(recreview_hold==0)) 
       {  
//-------------------------------- show state ------------------------
        if (conf.show_state) 
           {
            gui_osd_draw_state();
           }
//------------------------------- show misc values -------------------
        if (conf.show_values) 
           {
            gui_osd_draw_values();
           }
//---------------------------------   clock --------------------------  
  
        if (conf.show_clock) 
           {
            gui_osd_draw_clock();
           }
           
         if((conf.zf_size && !state_kbd_script_run) || (state_kbd_script_run && gEnableBigZoom))
          {
           gui_osd_draw_big_zoom();
          }
       } 

//---------------------------------- Debug propcase -------------------
    if (debug_propcase_show)
     {
	static char sbuf[100];
	int r,i, p;

	  for (i=0;i<10;i++)
         {
	    r = 0;
	    p = debug_propcase_page*10+i;
	    get_property_case(p, &r, 4);
	    sprintf(sbuf, "%3d: %d              ", p, r);sbuf[20]=0;
	    draw_string(64,16+16*i,sbuf, conf.osd_color,1);
	   }
      }
      
   } 
//---------------------------------- uBasic error ---------------------
    if (ubasic_error)
     {
	const char *msg;
        if (ubasic_error >= UBASIC_E_ENDMARK) 
          {
            msg = ubasic_errstrings[UBASIC_E_UNKNOWN_ERROR];
          } 
        else 
         {
	    msg = ubasic_errstrings[ubasic_error];
	   }
	   sprintf(osd_buf, "uBASIC:%d %s ", ubasic_linenumber(), msg);
	   draw_txt_string(1, 1, osd_buf, MAKE_COLOR(COLOR_RED, COLOR_YELLOW));
      
      if(conf.outline_mode==666)
      {  
       char error_buf[80];
       int fd=-1;
       struct tm *ttm;
       long t=time(NULL);
       ttm = localtime(&t);
       fd=open("A/ubasic_errors.txt",O_WRONLY|O_CREAT|O_APPEND, 0777);
      if(fd>=0)
      {
       sprintf(error_buf,"%02d:%02d:%02d uBASIC script %s: line %d %s \n\n",ttm->tm_hour,ttm->tm_min,ttm->tm_sec, script_title,ubasic_linenumber()-1, msg);
       write(fd,error_buf, strlen(error_buf));
       close(fd);
      }
       ubasic_error = UBASIC_E_NONE;
       script_start();
      }
      }
     }  
      if(needRestore)vid_bitmap_refresh();
//----------------------------------------------------------------------------
#else
 int row,col;
 int x,y,i;
 static int cnt=0,j=0;
 static int cp=0;
 char buf[32];
 unsigned char hue=0;
 unsigned short bgd;
int m = 2;
 x = (screen_width-256)/2;
 y = (screen_height-128)/2;
         
if(cnt<80)
{
 for(row=0;row<8;row++)
  {
   for(col=0;col<32;col++)
    {
     bgd=hue;
     bgd<<= 8;
     draw_char(x+col*FONT_WIDTH,y+row*FONT_HEIGHT,' ',bgd,1);
     hue++;
    }
  }
 }

 else if(cnt<160)
{
 for(row=0;row<8;row++)
  {
   for(col=0;col<32;col++)
    {
     draw_char(x+col*FONT_WIDTH,y+row*FONT_HEIGHT,(char)hue,(row<4)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
     hue++;
    }
  } 
 }

 else if(cnt==160)draw_clear();

 else if(cnt<240)
{
 x = (screen_width-(2*m*FONT_WIDTH))/2;
 y = (screen_height-(2*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)1,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)2,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),m);
}
else if(cnt==240)draw_clear();

 else if(cnt<320)
{
 x = (screen_width-(2*m*FONT_WIDTH))/2;
 y = (screen_height-(2*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)3,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)4,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
}
else if(cnt==320)draw_clear();

 else if(cnt<400)
{
 x = (screen_width-(2*m*FONT_WIDTH))/2;
 y = (screen_height-(2*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)5,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)6,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
}
else if(cnt==400)draw_clear();
 else if(cnt<480)
{
 x = (screen_width-(2*m*FONT_WIDTH))/2;
 y = (screen_height-(2*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)7,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)8,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
}
else if(cnt==480)draw_clear();
 else if(cnt<560)
{
 x = (screen_width-(3*m*FONT_WIDTH))/2;
 y = (screen_height-(3*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)9,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)10,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*2*FONT_WIDTH,y,(char)11,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
}
else if(cnt==560)draw_clear();
 else if(cnt<640)
{
 x = (screen_width-(3*m*FONT_WIDTH))/2;
 y = (screen_height-(3*m*FONT_WIDTH))/2;
 draw_char(x,y,(char)12,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*FONT_WIDTH,y,(char)13,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
 draw_char(x+m*2*FONT_WIDTH,y,(char)14,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE),m);
}

 else vid_bitmap_refresh();
 cnt++;
 if(cnt>=720)cnt=0;
#endif
//----------------------------------------------------------------------------   
}
static void gui_menuproc_reset_selected(unsigned int btn) 
{
    if (btn==MBOX_BTN_YES)
	{
	 tConfigVal configVal = {0,0,0,0};	
     conf_load_defaults();
	 configVal.numb = 0;
     configVal.isNumb = 1;
     conf_setValue(47, configVal);  
	}
}

void gui_menuproc_reset(int arg)
{
    gui_mbox_init(LANG_MSG_RESET_OPTIONS_TITLE, 
                  LANG_MSG_RESET_OPTIONS_TEXT,
                  MBOX_FUNC_RESTORE|MBOX_TEXT_CENTER|MBOX_BTN_YES_NO|MBOX_DEF_BTN2, gui_menuproc_reset_selected);
}
 
void gui_draw_fselect(int arg) 
{
 gui_fselect_init(LANG_STR_FILE_BROWSER, "A", NULL);
}
static void gui_load_script_selected(const char *fn) 
{
 if (fn) 
 {                                               
  script_load(fn,1);                          
  conf_save_new_settings_if_changed();
  if(gRunNow)
  {
   gRunNow=0; 
   gui_mode = GUI_MODE_ALT;
   draw_restore();
   script_start();
  }
  if(strncmp(fn,"A/SDM/SCRIPTS/",14)==0)	
  {
   play_sound(4);
   draw_restore();
   script_start();
  }
 }
}
static void load_easy_mode_script(int arg) 
{
 if(read_easyModes_dir()>1)
  {
   gScriptFolder=0;
   gui_fselect_init(LANG_STR_SELECT_SCRIPT_FILE, "A/SDM/SCRIPTS", gui_load_script_selected);
  }
  else play_sound(6);
}
static void gui_load_script(int arg) 
{
    DIR   *d;
    char  *path;
    path="A/SDM/SCRIPTS3";
    d=opendir(path);
    if (d) 
     {
        closedir(d);
     } 
    else 
     {
      path="A";
     }
      gui_fselect_init(LANG_STR_SELECT_SCRIPT_FILE, path, gui_load_script_selected);
}

static void gui_grid_lines_load_selected(const char *fn) {
    if (fn)
        grid_lines_load(fn);
}
void gui_grid_lines_load(int arg) {
    DIR   *d;
    char  *path="A/SDM/GRIDS";

    
    d=opendir(path);
    if (d) {
        closedir(d);
    } else {
        path="A";
    }

    gui_fselect_init(LANG_STR_SELECT_GRID_FILE, path, gui_grid_lines_load_selected);
}

void gui_draw_osd_le(int arg) {
    gui_mode = GUI_MODE_OSD;
    gui_osd_init();
}
 
static void gui_draw_lang_selected(const char *fn) 
{
    if (fn) 
    {
        strcpy(conf.lang_file, fn);
        lang_load_from_file(conf.lang_file);
        gui_menu_init(NULL);
    }
}

void gui_draw_load_lang(int arg) 
{
    DIR   *d;
    char  *path="A/SDM/LANG";

    
    d=opendir(path);
    if (d) 
    {
        closedir(d);
    } else 
    {
        path="A";
    }

    gui_fselect_init(LANG_STR_SELECT_LANG_FILE, path, gui_draw_lang_selected);
}

static void gui_draw_read_selected(const char *fn) 
{
    if (fn) 
    {
        gui_mode = GUI_MODE_READ;
        gui_read_init(fn);
    }
}
 
void gui_draw_read(int arg) {
    DIR   *d;
    char  *path="A/SDM/TEXTS";
    d=opendir(path);
    if (d) 
    {
        closedir(d);
    } else 
    {
        path="A";
    }
    gui_fselect_init(LANG_STR_SELECT_TEXT_FILE, path, gui_draw_read_selected);
}

void gui_read_stacks(int arg) 
{
    DIR   *d;
    char  *path="A/SDM/STACKS";
    d=opendir(path);
    if (d) 
    {
     closedir(d);
    } 
    gui_fselect_init(LANG_STR_SELECT_TEXT_FILE, path, gui_draw_read_selected);
}

void gui_read_xml(int arg) 
{
    DIR   *d;
    char  *path="A/SDM/XML";
    d=opendir(path);
    if (d) 
    {
     closedir(d);
    } 
    gui_fselect_init(LANG_STR_SELECT_TEXT_FILE, path, gui_draw_read_selected);
}

static void gui_draw_read_last(int arg) 
{
    int fd;
    
    fd = open(conf.reader_file, O_RDONLY, 0777);
    if (fd >= 0) 
    {
        close(fd);
        gui_draw_read_selected(conf.reader_file);
    } else 
    {
        gui_draw_read(arg);
    }
}
 

