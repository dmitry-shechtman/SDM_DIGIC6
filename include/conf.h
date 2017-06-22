#ifndef CONF_H
#define CONF_H

#include "gui.h"
#include "script.h"

typedef struct 
{
    unsigned short  x, y;
} OSD_pos;

#define CONF_DEF_PTR        1
#define CONF_VALUE          2
#define CONF_DEF_VALUE      2
#define CONF_EMPTY          3
#define CONF_VALUE_PTR      4
#define CONF_INT_PTR        5
#define CONF_CHAR_PTR       6
#define CONF_OSD_POS_PTR    7
#define CONF_STR_LEN        100

typedef struct 
{
 int isNumb, isStr, isPInt, isPos;
 int numb;
 char* str;
 int* pInt;
 OSD_pos pos;
} tConfigVal;

extern int conf_getValue(unsigned short id, tConfigVal* configVal);
extern int conf_setValue(unsigned short id, tConfigVal configVal);

typedef struct 
{
    int save_raw;
    int raw_in_dir;
    int raw_prefix;
    int raw_ext;
    int raw_save_first_only; //for series shooting
    int raw_nr;
    int show_osd;
    int script_shoot_delay;
    int ubasic_vars[SCRIPT_NUM_PARAMS];
    int ubasic_vars2[SCRIPT_NUM_PARAMS];
    int ubasic_vars3[SCRIPT_NUM_PARAMS];
    char easy_script_file[28];
    char script_file[50];
    char user_script_file[50];
    int show_dof;
    int batt_volts_max;
    int batt_volts_min;
    int batt_step_25;
    int batt_perc_show;
    int batt_volts_show;
    int batt_icon_show;
    int show_clock;

    int zoom_value;

    int show_state;
    int show_values;
    int show_overexp;
 
    OSD_pos dof_pos;
    OSD_pos batt_icon_pos;
    OSD_pos batt_txt_pos;
    OSD_pos mode_state_pos;
    OSD_pos values_pos;
    OSD_pos zoom_pos;
    OSD_pos increment_pos;
    OSD_pos clock_pos;
 
    color osd_color;
    color batt_icon_color;
    color menu_color;
    color reader_color;
    color grid_color;

    int font_cp;
    char menu_rbf_file[100];

    char lang_file[100];

    char reader_file[100];
    int  reader_pos;
    int  reader_autoscroll;
    char reader_rbf_file[100];
    int  reader_codepage;
    int  reader_wrap_by_words;

    int flashlight;
    int splash_show;

    int use_zoom_mf;
    long alt_mode_button; //for S-series
    int alt_prevent_shutdown;

    int show_grid_lines;
    char grid_lines_file[100];
    int grid_force_color;

    int video_mode;
    int video_quality;
    int video_bitrate;  
    int tv_override;
    int ns_enable_memdump;
    int script_startup;			// remote autostart
    int remote_enable;			// remote enable
    int main_flash;                 // enables preflash but disables secondary
    int synch_enable;
    int synch_delay_enable;
    int synch_flash_delay;
    int synch_coarse_delay;
    int synch_delay_range;
    int rangefinder_near;
    int stack_step_size;
    int rangefinder_far;
    int av_override_value;
    int tv_bracket_value;	      
    int subj_dist_bracket_value;
    int bracket_type;
    int dist_step_size;
// ************************************
    int cam_spacing;
    int deviation_factor;
    int stereo_spacing;
    int stereo_deviation;
    int minimum_dist_factor;
    int save_csv_file;
    int save_xml_file;
    int csv_extension;
    int save_same_dir;
    int user_range;
    int user_range_set;
    int distance_setting; // Mr.Bubble
    int focus_mode;
    int show_stereo;
    int stereo_display_mode;
    int layout_mode;
    int camera_position;
    int camera_orientation;
    int camera_rotation;
    OSD_pos stereo_osd_pos;
    OSD_pos synch_pos;
    int dof_dist_from_lens;
    int manual_far;
    int flash_exp_comp;
    int flash_override_value;
    int nd_filter_state;
    int unlock_optical_zoom_for_video;
    int ricoh_ca1_mode;
    int outline_mode;
    int stereo_mode;
    int save_viewport;
    int edge_overlay_thresh;  // name change
    int mute_on_zoom; 
    int expert_menu_enable;
    int computed_distance;
    int osd_page_index;
    int custom_timer_synch;
    int synch_fine_delay;
    int tv_override_enable;
    int add_synch_delays;
    int remote_zoom_enable;
    int zoom_timeout;
    int disable_deviation_guideline;
    int nd_constant_exposure;
    int heading1;
    int heading2;
    int heading3;
    int heading4;
    int heading5;
    int heading6;
    int heading7;
    int heading8;
    int heading9;
    int heading10;
    int heading11;
    int heading12;
    int heading13;
    int heading14;
    int heading15;
    int heading16;
    int heading17;
    int heading18;
    int heading19;
    int extend_lens;
    int disable_overrides;
    OSD_pos overrides_pos;
    int digi_obj_diam;
    int digi_scope_fl;
    int digi_ep_fl;
    int digi_zoom_setting;
    int digi_factor;
    int digi_ref_dist;
    int save_digi;
    int save_auto;
    int dist_mode;
    int digi_step;
    int compact;
    int cs;        // console scrolling
    int sdm_console;
    int num_lines;
    int line_length;
    int edge_overlay_enable;
    color edge_overlay_color;
    int edge_overlay_play;
    int edge_overlay_lock;
    int edge_overlay_zoom;
    int sunrise;
    int sunrise_tv96;
    int sunrise_repeats;
    int sunrise_shots;
    int srb;            // sunrise burst mode
    int sunrise_kf_1;
    int sunrise_kf_2;
    int sunrise_kf_3;
    int sunrise_tv96_2;
    int sunrise_tv96_3;
    int script_param_set;
    int script_param_save;
    int edge_and_osd;
    int bw;                // serial comms '0' bit width
    int md;                // tweak factor for accurate msec delay
    int invert_playback;
    int enable_yaw_guideline;
    int yaw;
    int dc;                   // double-click mode for focus cacelling
    int use_af_led;           // for serial comms
    int zf_size;			// large font zoom value
    int lockout_time;         // lockout time for pulses in FAST mode
    int slave_flash;
    int focus_pos;
    int zoom_point;
    int fpd;
    int bright_screen;
    int raw_strip_mode;
    int strip_width;
    int strip_images;
    int strip_offset;
    int eye_spacing;
    int wait_forever;      // SDM_NG synch option
    int on_release;
    int no_focus;
    int blank_jpg;
    int burst_shutdown;
    int click_time;
    int script_set;  
    int ptp_reboot;
    OSD_pos frames_pos;
    int nearGuideX;
    int farGuideX;
    int video_frame_count;
    int PulseOption;
    int platformid;
    int FastLapseDelay;
    int FastLapseFineTune;
    int CountForOneMsec;
    int burst_frame_count;
    int user_1;
    int user_2;
    int user_3;
    int user_4;
    int user_5;
    int user_6;
    int user_7;
    int user_8;
    int flag_1;
    int flag_2;
    int flag_3;
    int flag_4;
    int flag_5;
    int flag_6;
    int flag_7;
    int flag_8;
    int fastlapse;
    int MovieFineTune;
    int zf_big_permanent;
    int zf_and_depth;
    int bracket_intershot_delay;
    int precision_synch_flash;
    int video_synch_device;
    int hotshoe_override;
    int hp_usb_pw;
    int focus_override_method;
    int easy_pos;
    int user_pos;
    int sleep;
    int low_light;
    int iso_override;
    int near_for_infinity;
    int far_for_near;
    int serial_number;
    int SsTenMsecCount;
    int anaglyph_button;
    int left_flash_dim;
    int auto_rotation;
    int alt_files_shortcut;
} Conf;

extern Conf conf;
#define ALT_PREVENT_SHUTDOWN_NO         0
#define ALT_PREVENT_SHUTDOWN_ALT        1
#define ALT_PREVENT_SHUTDOWN_ALT_SCRIPT 2
#define ALT_PREVENT_SHUTDOWN_ALWAYS     3

#define SHOOTING_PROGRESS_NONE          0
#define SHOOTING_PROGRESS_STARTED       1
#define SHOOTING_PROGRESS_PROCESSING    2
#define SHOOTING_PROGRESS_DONE          3

extern int state_kbd_script_run;
extern int state_shooting_progress;
extern int state_expos_recalculated;
extern int debug_propcase_show;
extern int debug_propcase_page;
extern int debug_vals_show; 
extern void script_autostart();				// remote autostart
extern void conf_save();
extern void conf_restore();
extern void conf_load_defaults();
extern int shutter_int;
extern void conf_update_prevent_shutdown(void);
extern  int gFrameCount,gFramesRecorded;
extern unsigned char gStopVideoCapture; 
#endif
