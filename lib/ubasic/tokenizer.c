/**
 * Copyright (c) 2006, Adam Dunkels
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#ifdef TEST
#include <string.h>
#include <ctype.h>
#endif

#include "tokenizer.h"
#include "stdlib.h"
#include "conf.h"
static char const *ptr, *nextptr;
#define MAX_NUMLEN 6

struct keyword_token {
  const char *keyword;
  int token;
};

static ubasic_token current_token = TOKENIZER_ERROR;
static int current_line = 0,fdScriptLog=-1;
extern int gSpecial_token;

/*WARNING due to tokenizer limitation longest match must be first */

static const struct keyword_token keywords[] = {
  {"<>",                             TOKENIZER_NE},
  {"<=",                             TOKENIZER_LE},
  {">=",                             TOKENIZER_GE},
  {"<",                              TOKENIZER_LT},
  {">",                              TOKENIZER_GT},
  {"@title",                         TOKENIZER_REM},
  {"@param",                         TOKENIZER_REM},
  {"@default",                       TOKENIZER_REM},
  {"@values",                        TOKENIZER_REM},
  {"@range",                         TOKENIZER_REM},
  {"//",                             TOKENIZER_REM},
  {"#",                              TOKENIZER_COMMENT_BLOCK},
#if defined(OPT_ALARIK)
  {"select",                         TOKENIZER_SELECT},
  {"case_else",                      TOKENIZER_CASE_ELSE},
  {"case",                           TOKENIZER_CASE},
  {"end_select",                     TOKENIZER_END_SELECT},
#endif
  {"aeb_canon_off",                  TOKENIZER_AEB_OFF},
  {"aeb_canon_on",                   TOKENIZER_AEB_ON},
  {"af_assist_off",			 TOKENIZER_AFA_OFF},
  {"af_led_off",                     TOKENIZER_AFLOFF},
  {"af_led_on",                      TOKENIZER_AFLO},
  {"altitude",                       TOKENIZER_ALTITUDE},
  {"and",                            TOKENIZER_LAND},
  {"aperture",                       TOKENIZER_AP},
  {"auto_focus_first",               TOKENIZER_AF_FIRST},
  {"auto_focus_bracketing",          TOKENIZER_AFB},
  {"auto_focus_all",                 TOKENIZER_AF_ALL},
  {"auto_focus",                     TOKENIZER_AF},
  {"autofocus",                      TOKENIZER_AF},
  {"av_value",			       TOKENIZER_PRINT_AV},

  {"beep",                           TOKENIZER_BEEP}, 
  {"bigzoom",                        TOKENIZER_BIGZOOM},
  {"big_zoom_display_pos",           TOKENIZER_BZDP},
  {"big_zoom_display_off",           TOKENIZER_DISABLE_BIG_ZOOM},
  {"big_zoom_display_on",                TOKENIZER_ENABLE_BIG_ZOOM},
  {"blink_af_led_for",               TOKENIZER_AFLF},
 
  {"burst_sequence",                 TOKENIZER_BURSTSEQ},
 
  {"cam_has_nd_filter",              TOKENIZER_CAM_HAS_ND},
  {"cam_has_av_mode",                TOKENIZER_CAM_AV_MODE},
  {"cam_has_tv_mode",                TOKENIZER_CAM_TV_MODE},
  {"canon_timer_active",             TOKENIZER_TIME_DELAY_ACTIVE}, 
  {"capture_mode",                   TOKENIZER_CAPTURE_MODE},
  {"change_ev_by",                   TOKENIZER_CEV},
  {"change_fps",                     TOKENIZER_CFPS},
  {"click",                          TOKENIZER_CLICK},
  {"cls",                            TOKENIZER_CLS},
  {"console_position",               TOKENIZER_CONSOLE_POSITION},
  {"console_redraw",                 TOKENIZER_CONSOLE_REDRAW},
  {"continuous_shooting",            TOKENIZER_CS},
  {"cs_off",                         TOKENIZER_CS_OFF},
  {"cs_on",                          TOKENIZER_CS},
  {"cs",                             TOKENIZER_CS},
  {"cover_lens",                     TOKENIZER_COVER_LENS},
  {"custom_timer",                   TOKENIZER_CT},
  {"data_received",                  TOKENIZER_DR},
  {"date",                           TOKENIZER_DATE},
  {"debug_led_off",                  TOKENIZER_BLUE_OFF},
  {"debug_led_on",                   TOKENIZER_BLUE_ON},
  {"disable_big_zoom",               TOKENIZER_DISABLE_BIG_ZOOM},
  {"disable_focus_override",         TOKENIZER_DFO},
  {"disable_logging",                TOKENIZER_DISABLE_LOGGING}, 
  {"disable_video_out",              TOKENIZER_DISABLE_VIDEO_OUT},
  {"do",                             TOKENIZER_DO},

  {"each_photo_alternating",         TOKENIZER_BA},
  {"each_photo_darker",              TOKENIZER_BD},
  {"each_photo_lighter",             TOKENIZER_BL},
  {"elapsed_time",                   TOKENIZER_ELAPSED_TIME},
  {"else",                           TOKENIZER_ELSE},
  {"enable_big_zoom",                TOKENIZER_ENABLE_BIG_ZOOM},
  {"enable_focus_override",          TOKENIZER_EFO},
  {"enable_video_out",               TOKENIZER_ENABLE_VIDEO_OUT},
  {"enable_logging",                 TOKENIZER_ENABLE_LOGGING},
  {"end_time",                       TOKENIZER_FT},
  {"end_hour",                       TOKENIZER_EH},  
  {"End_hour",                       TOKENIZER_EH},
  {"end_minutes",                    TOKENIZER_EM},
  {"End_minutes",                    TOKENIZER_EM},
  {"endif",                          TOKENIZER_ENDIF},
  {"end",                            TOKENIZER_END},
  {"ev_correction",			 TOKENIZER_EV_CORRECTION},
  {"exit_alt",                       TOKENIZER_EXIT_ALT},

  {"finish_time",                    TOKENIZER_FT},
  {"flash_off",                      TOKENIZER_FLASH_OFF},
  {"flash_on",                       TOKENIZER_FLASH_ON},
  {"focus_at_nearpoint",	       TOKENIZER_FOCUS_AT_NP},
  {"focus_out",                      TOKENIZER_FOCUS_OUT},
  {"focus_in",                       TOKENIZER_FOCUS_IN},
  {"focus_mode",                     TOKENIZER_FOCUS_MODE},
  {"for",                            TOKENIZER_FOR},

  {"get_autostart",                  TOKENIZER_SCRIPT_AUTOSTART},
  {"get_brightness",                 TOKENIZER_GET_BRIGHTNESS},
  {"get_capture_mode",               TOKENIZER_GET_SHOOTING_MODE}, // synonym,shooting mode on dial
  {"get_config_value",               TOKENIZER_GET_CONFIG_VALUE},
  {"get_day_seconds",                TOKENIZER_GET_DAY_SECONDS},
  {"get_drive_mode",                 TOKENIZER_GET_DRIVE_MODE},
  {"get_equivalent_focal_length",    TOKENIZER_GET_EFL},
  {"get_efl",                        TOKENIZER_GET_EFL},
  {"get_ev_correction",              TOKENIZER_GET_EV_CORRECTION},
  {"get_exposure_count",             TOKENIZER_GET_EXP_COUNT},
  {"get_exp_count",                  TOKENIZER_GET_EXP_COUNT},
  {"get_farpoint",                   TOKENIZER_GET_FARPOINT},
  {"get_flash_mode",                 TOKENIZER_GET_FLASH_MODE}, //fudgey
  {"get_fl",                         TOKENIZER_GET_FL},
  {"get_focal_length",               TOKENIZER_GET_FL},
  {"get_focused_distance",           TOKENIZER_GET_FOCUS},
  {"get_focus",                      TOKENIZER_GET_FOCUS},
  {"get_fov",                        TOKENIZER_GET_FOV},
  {"get_free_disk_space",            TOKENIZER_GET_FREE_DISK_SPACE},
  {"get_iso",                        TOKENIZER_GET_ISO},
  {"get_lens_mecha_position",		 TOKENIZER_GET_LENS_MECHA_POSITION},
  {"get_mode",		             TOKENIZER_GET_MODE},  //Returns 0 in recordmode, 1 in playmode , 2 in video
  {"get_movie_status",        	 TOKENIZER_GET_MOVIE_STATUS},
  {"get_nearpoint",                  TOKENIZER_GET_NEARPOINT},
  {"get_orientation",        	       TOKENIZER_GET_ORIENTATION},
  {"get_override_tv96",        	 TOKENIZER_GET_OVTV96},
  {"get_prop",                       TOKENIZER_GET_PROP},
  {"get_quality",                    TOKENIZER_GET_QUALITY},
  {"get_real_focal_length_for_step", TOKENIZER_GET_RFL_FOR_STEP}, 
  {"get_real_iso",                   TOKENIZER_GET_REAL_ISO},
  {"get_resolution",                 TOKENIZER_GET_RESOLUTION},
  {"get_sdm_display_mode",           TOKENIZER_SDMDM}, 
  {"get_shoot_fast_param",           TOKENIZER_GET_SHOOT_FAST_PARAM},
  {"get_shooting_mode",              TOKENIZER_GET_SHOOTING_MODE},  // for compatability
  {"get_shooting",                   TOKENIZER_GET_SHOOTING},  
  {"get_tick_count",                 TOKENIZER_GET_TICK_COUNT},  
  {"get_av96",                       TOKENIZER_GET_AV96},
  {"get_min_av96",                   TOKENIZER_GET_MIN_AV96},
  {"get_bv96",                       TOKENIZER_GET_BV96},
  {"get_sv96",	                   TOKENIZER_GET_SV96},
  {"get_tv96",                       TOKENIZER_GET_TV96},
  {"get_usb_power",                  TOKENIZER_GET_USB_POWER},
  {"get_usb_pulse_width",            TOKENIZER_GET_USB_POWER},
  {"get_video_button",               TOKENIZER_GET_VIDEO_BUTTON}, 
  {"get_zoom_steps",                 TOKENIZER_GET_ZOOM_STEPS},
  {"get_current_zoom_position",      TOKENIZER_GET_ZOOM},
  {"get_zoom",                       TOKENIZER_GET_ZOOM},
  {"gosub",                          TOKENIZER_GOSUB},
  {"goto",                           TOKENIZER_GOTO},

  {"hdr_bracket_1/3_ev_steps",       TOKENIZER_SSB},
  {"hours",                          TOKENIZER_GET_HOURS},

  {"if",                             TOKENIZER_IF},
  {"image_number",                   TOKENIZER_IMAGE_NUMBER}, 
  {"interval_shots",                 TOKENIZER_ISHOTS}, 
  {"is_capture_mode_valid",          TOKENIZER_IS_CAPTURE_MODE_VALID},
  {"is_continuous_af_on",		 TOKENIZER_IS_CAF_ON},
  {"is_key",                         TOKENIZER_IS_KEY},
  {"is_pressed",                     TOKENIZER_IS_PRESSED},
  {"is_servo_af_on",		       TOKENIZER_IS_SAF_ON},
  {"is_this_right_camera",           TOKENIZER_CAMPOS},
  {"is_on",					 TOKENIZER_IS_ON},
  {"is_off",			       TOKENIZER_IS_OFF},
  {"is_usb_high",				 TOKENIZER_IS_USB_HIGH},
  
 
  {"let",                            TOKENIZER_LET},
  {"line_1",                         TOKENIZER_LINE_ONE},
  {"line_2",                         TOKENIZER_LINE_TWO},
  {"line_3",                         TOKENIZER_LINE_THREE},
  {"line",                           TOKENIZER_LINE},
  {"load_grid",                      TOKENIZER_LG},
  {"lock_auto_exposure",             TOKENIZER_AE_LOCK},
  {"lock_ae",                        TOKENIZER_AE_LOCK},
  {"lock_autofocus",                 TOKENIZER_LA},
  {"lock_focus_at_infinity",         TOKENIZER_LFAI},
  {"lock_focus_at",                  TOKENIZER_LFA},
  {"log_brightness",                 TOKENIZER_LOG_BV},
  {"log",                            TOKENIZER_LOG},

 
  {"manual_focus_on",                TOKENIZER_MF_ON},
  {"manual_focus_off",               TOKENIZER_MF_OFF},
  {"md_detect_motion",               TOKENIZER_MD_DETECT_MOTION},
  {"md_get_cell_diff",               TOKENIZER_MD_GET_CELL_DIFF},
  {"message",				 TOKENIZER_MESSAGE},
  {"meter_brightness",               TOKENIZER_MB},
  {"mf_on",                          TOKENIZER_MF_ON},
  {"mf_off",                         TOKENIZER_MF_OFF},
  {"minutes",                        TOKENIZER_GET_MINUTES},

  {"nd_filter_out",                  TOKENIZER_NDFO},
  {"nd_filter_off",                  TOKENIZER_NDFF},
  {"nd_filter_in",                   TOKENIZER_NDFI},
  {"next",                           TOKENIZER_NEXT},
  {"not",                            TOKENIZER_LNOT},
  {"number_of_images_to_capture_is", TOKENIZER_SET_BE},

  {"or",                         TOKENIZER_LOR},

  {"playback_mode",              TOKENIZER_PLAYBACK},
  {"playsound",                  TOKENIZER_PLAY_SOUND},
  {"precision_sample_usb_at_msec",TOKENIZER_PSUSBAT},
  {"precision_sample_usb_off",   TOKENIZER_PSUSBOFF}, 
  {"prepare_for_shot_at",        TOKENIZER_PFSAT},  
  {"press",                      TOKENIZER_PRESS},
  {"print_screen",               TOKENIZER_PRINT_SCREEN},  
  {"print",                      TOKENIZER_PRNT},

  {"readYUV",                    TOKENIZER_GET_LUMINANCE},
  {"record_mode",                TOKENIZER_RECORD},
  {"release",                    TOKENIZER_RELEASE},
  {"remove",                     TOKENIZER_REMOVE},
  {"rem",                        TOKENIZER_REM},
  {"restore_focus",              TOKENIZER_RESTORE_FOCUS},
  {"restore_zoom",               TOKENIZER_RESTORE_ZOOM},
  {"return",                     TOKENIZER_RETURN},

  {"save_focus",                 TOKENIZER_SAVE_FOCUS},
  {"save_stack",                 TOKENIZER_SS},
  {"save_zoom",                  TOKENIZER_SAVE_ZOOM},
  {"scroll_console",             TOKENIZER_SCROLL},
  {"sdm_2D_mode",                TOKENIZER_SDM2D},
  {"sdm_3D_SPORTS_mode",         TOKENIZER_SDM3DSPORTS},
  {"sdm_3D_mode",                TOKENIZER_SDM3D},
  {"sdm_auto_hdr_mode",          TOKENIZER_SDMAUTOHDR},
  {"sdm_burst_mode",             TOKENIZER_SDMBM},
  {"sdm_3d_event_mode",           TOKENIZER_SDMTM},
  {"sdm_console_off",            TOKENIZER_SDMCF},
  {"sdm_console_on",             TOKENIZER_SDMCN},
  {"sdm_console_lines",          TOKENIZER_SDMCL},
  {"sdm_console_line_length",    TOKENIZER_SDMCLL},
  {"sdm_reset",                  TOKENIZER_SDM_RESET},
  {"sdm_user_hdr_mode",          TOKENIZER_SDMUHDR},
  {"seconds",                    TOKENIZER_GET_SECONDS},
  {"send_data",                  TOKENIZER_SD},
  {"set_aflock",                 TOKENIZER_SET_AFLOCK},
  {"set_af_assist",              TOKENIZER_SET_AF_ASSIST},
  {"set_autostart",              TOKENIZER_SET_SCRIPT_AUTOSTART},
  {"set_av96_direct",            TOKENIZER_SET_AV96_DIRECT},
  {"set_av_rel",                 TOKENIZER_SET_AV_REL}, // for helicopter script
  {"set_av",                     TOKENIZER_SET_AV},
  {"set_canon_jpg_raw_state",		TOKENIZER_SCJR_STATE},
  {"set_config_value",           TOKENIZER_SET_CONFIG_VALUE},
  {"set_focus_step_to",          TOKENIZER_SET_FS},
  {"set_focus_to",               TOKENIZER_SET_FOCUS},
  {"set_focus_range",  		   TOKENIZER_SET_FOCUS_RANGE},
  {"set_focus",                  TOKENIZER_SET_FOCUS},
  {"set_hdr_tv96",	         TOKENIZER_SET_HDR_TV96},
  {"set_iso",                    TOKENIZER_SET_ISO},
  {"set_led",                    TOKENIZER_SET_LED},
  {"set_max_iso_to",             TOKENIZER_FAST_ISO_MAX},
  {"set_nd_filter",              TOKENIZER_SET_ND_FILTER},
  {"set_prop",                   TOKENIZER_SET_PROP},
  {"set_quality",                TOKENIZER_SET_QUALITY},
  {"set_raw",                    TOKENIZER_SET_RAW},
  {"set_remote_timing",          TOKENIZER_SET_REMOTE_TIMING},  
  {"set_resolution",             TOKENIZER_SET_RESOLUTION},
  {"set_script_speed",           TOKENIZER_SET_MAX_SPT},
  {"set_shoot_fast_param",       TOKENIZER_SET_SHOOT_FAST_PARAM},
  {"set_sv96",		               TOKENIZER_SET_SV96},
  {"set_tv96_direct",            TOKENIZER_SET_TV96_DIRECT},
  {"set_tv_rel",                 TOKENIZER_SET_USER_TV_BY_ID_REL}, // camRemote support
  {"set_tv",                     TOKENIZER_SET_TV},
#if defined(OPT_ALARIK)
  {"set_console_layout",         TOKENIZER_SET_CONSOLE_LAYOUT},
  {"set_console_autoredraw",     TOKENIZER_SET_CONSOLE_AUTOREDRAW},
  {"console_redraw",             TOKENIZER_CONSOLE_REDRAW},
  {"set_user_av_by_id",          TOKENIZER_SET_USER_AV_BY_ID},
  {"set_user_tv_by_id",          TOKENIZER_SET_USER_TV_BY_ID},
#endif
  {"set_zoom_rel",               TOKENIZER_SET_ZOOM_REL},
  {"set_zoom_to_focal_length",   TOKENIZER_SET_ZOOM_FL},     // legacy scripts
  {"set_zoom_to_equiv_focal_length",   TOKENIZER_SET_ZOOM_FL},
  {"set_zoom_to_real_focal_length",TOKENIZER_SET_ZOOM_RFL},
  {"set_zoom_to_step",           TOKENIZER_SET_ZOOM},
  {"set_zoom_to",                TOKENIZER_SET_ZOOM},
  {"set_zoom",                   TOKENIZER_SET_ZOOM},
  {"shoot_fast_at",              TOKENIZER_FSON},
  {"shoot_movie_for",            TOKENIZER_SHOOT_MOVIE},
  {"shoot_when_camera_steady",   TOKENIZER_SWS},
  {"shoot_when_no_movement_for", TOKENIZER_SWNM},
  {"shot_count",                 TOKENIZER_SHOT_COUNT},
  {"shoot",                      TOKENIZER_SHOOT},
  {"shutdown",                   TOKENIZER_SHUT_DOWN},
  {"shut_down",                  TOKENIZER_SHUT_DOWN},
  {"shutter_speed",              TOKENIZER_SSPD},
  {"sleep_for_minutes",          TOKENIZER_SLEEP_M}, 
  {"sleep_for_msecs",            TOKENIZER_SLEEP}, 
  {"sleep_for_seconds",          TOKENIZER_SLEEP_S}, 
  {"sleep_for",                  TOKENIZER_SLEEP}, 
  {"sleep_until",                TOKENIZER_SLEEP_UNTIL},
  {"sleep",                      TOKENIZER_SLEEP},
  {"start_movie",			   TOKENIZER_SM},
  {"start_video",			   TOKENIZER_SM},
  {"stop_movie",			   TOKENIZER_SM},
  {"stop_video",			   TOKENIZER_SM},
  {"start_clock",                TOKENIZER_START_CLOCK},
  {"switch_start_movie",         TOKENIZER_SV},
  {"start_time",                 TOKENIZER_SLEEP_UNTIL},
  {"start_hour",                 TOKENIZER_SH},  
  {"Start_hour",                 TOKENIZER_SH},
  {"start_minutes",              TOKENIZER_SMIN},
  {"Start_minutes",              TOKENIZER_SMIN},
  {"stitch_mode_supported",      TOKENIZER_SMS},
  {"switch_start_video",         TOKENIZER_SV},
  {"switch_stop_movie",          TOKENIZER_STPV},
  {"switch_stop_video",          TOKENIZER_STPV},
  {"step",                       TOKENIZER_STEP},
  {"sync_on",                    TOKENIZER_SYNC_ON},
  {"synch_on",                   TOKENIZER_SYNC_ON},
  {"sync_off",                   TOKENIZER_SYNC_OFF},
  {"synch_off",                  TOKENIZER_SYNC_OFF},
  {"sync_ticks",			   TOKENIZER_SYNC_TICKS},

  {"then",                       TOKENIZER_THEN},
  {"time_lapse",                 TOKENIZER_TL},
  {"time",                       TOKENIZER_TIME},
  {"to",                         TOKENIZER_TO},
  {"tune_unit_pulse",            TOKENIZER_TUP},
  {"turn_lcd_power_off",         TOKENIZER_TLPF},
  {"turn_lcd_power_on",          TOKENIZER_TLPO},
  {"Tv_bracket_1/3_ev_steps",    TOKENIZER_SSB},
  {"tv_value",			   TOKENIZER_PRINT_TV}, 

  {"unit_burst",                 TOKENIZER_UB},
  {"unit_pulse",                 TOKENIZER_UP},  
  {"unlock_auto_exposure",       TOKENIZER_AE_UNLOCK},
  {"unlock_ae",                  TOKENIZER_AE_UNLOCK},
  {"unlock_focus",               TOKENIZER_UF},
  {"unit_burst",                 TOKENIZER_UB},
  {"unit_pulse",                 TOKENIZER_UP},
  {"until",                      TOKENIZER_UNTIL},
  {"usb_pulse_action",           TOKENIZER_USBPA},
  {"use_af_led",                 TOKENIZER_UAFL},
  {"use_debug_led",              TOKENIZER_UPL},


  {"valid_modes",                TOKENIZER_VALID_MODES},
  {"video_zoom_in_out",          TOKENIZER_VZOOM_IN_OUT},

  {"wait_click",                 TOKENIZER_WAIT_CLICK},
  {"wait_for_switch_press",      TOKENIZER_WFSP},
  {"wait_for_usb",		   TOKENIZER_WFSP},
  {"wait_until_done",            TOKENIZER_WUD},
  {"warning",                    TOKENIZER_WARNING},
  {"wend",                       TOKENIZER_WEND},
  {"wheel_left",                 TOKENIZER_WHEEL_LEFT},
  {"wheel_right",                TOKENIZER_WHEEL_RIGHT},
  {"while",                      TOKENIZER_WHILE},
  {NULL,                         TOKENIZER_ERROR},
};

//---------------------------------------------------------------------------
static int
singlechar(void)
{
  if(*ptr == '\n') {
    return TOKENIZER_CR;
  } else if(*ptr == ',') {
    return TOKENIZER_COMMA;
  } else if(*ptr == ';') {
    return TOKENIZER_SEMICOLON;
  } else if(*ptr == '+') {
    return TOKENIZER_PLUS;
  } else if(*ptr == '-') {
    return TOKENIZER_MINUS;
  } else if(*ptr == '&') {
    return TOKENIZER_AND;
  } else if(*ptr == '|') {
    return TOKENIZER_OR;
  } else if(*ptr == '^') {
    return TOKENIZER_XOR;
  } else if(*ptr == '*') {
    return TOKENIZER_ASTR;
  } else if((*ptr == '/')&&(*(ptr+1) != '/')) {
    return TOKENIZER_SLASH;
  } else if(*ptr == '%') {
    return TOKENIZER_MOD;
  } else if(*ptr == '(') {
    return TOKENIZER_LEFTPAREN;
  } else if(*ptr == ')') {
    return TOKENIZER_RIGHTPAREN;
  } else if(*ptr == '=') {
    return TOKENIZER_EQ;
  } else if(*ptr == '.') {
    return TOKENIZER_PERIOD;
  } else if(*ptr == ':') {
    return TOKENIZER_COLON;
  }
  return 0;
}
//---------------------------------------------------------------------------
// return error, number,cr,string, rem or variable
// 'ptr' points to current location in script
// --------------------------------------------------------------------------
static int
get_next_token(void)
{
  struct keyword_token const *kt;
  int i;

  DEBUG_PRINTF("get_next_token(): '%s'\n", ptr);

  //eat all whitespace
  while(*ptr == ' ' || *ptr == '\t' || *ptr == '\r') ptr++;

  if(*ptr == 0) {
    return TOKENIZER_ENDOFINPUT;
  }
  
 //UnknStatement should have size, otherwise hanging-up in ubasic.c possible for some cases 
  nextptr = ptr + 1; 

  if(isdigit(*ptr)) {
    for(i = 0; i < (MAX_NUMLEN+1); ++i) {
      if(!isdigit(ptr[i])) {
	if(i > 0) {
	  nextptr = ptr + i;
	  return TOKENIZER_NUMBER;
	} else {
	  DEBUG_PRINTF("get_next_token: error due to too short number\n");
	  return TOKENIZER_ERROR;
	}
      }
      if(!isdigit(ptr[i])) {
	DEBUG_PRINTF("get_next_token: error due to malformed number\n");
	return TOKENIZER_ERROR;
      }
    }
    DEBUG_PRINTF("get_next_token: error due to too long number\n");
    return TOKENIZER_ERROR;
  } else if((*ptr == ':') && !gSpecial_token) {
    // label
    nextptr = ptr;
    do {
      ++nextptr;
    } while(*nextptr != ' ' && *nextptr != '\r' && *nextptr != '\n' && *nextptr != '\t');
    return TOKENIZER_LABEL;
  } else if((i=singlechar()) != 0) {
    if (i == TOKENIZER_CR){
      // move to next line, and skip all following empty lines as well
      while (singlechar() == TOKENIZER_CR) 
      {
        current_line++;
        ptr++;
        // eat all whitespace
        while(*ptr == ' ' || *ptr == '\t' || *ptr == '\r') ptr++;
      };
      ptr--;
    if(conf.outline_mode==777) 
     {
      fdScriptLog=open("A/ubasic_log.txt",O_WRONLY|O_CREAT|O_TRUNC, 0777);
      if(fdScriptLog>=0)
      {
      char linebuf[8];
      sprintf(linebuf,"%5d\n",current_line-1);
      write(fdScriptLog,linebuf, strlen(linebuf));
      close(fdScriptLog); 
      }      
     }    
      //a CR. I hope that doesn't break anything.
    }
    nextptr = ptr + 1;
    return i;
  } else if(*ptr == '"') {
    nextptr = ptr;
    do {
      ++nextptr;
    } while(*nextptr != '"');
    ++nextptr;
    return TOKENIZER_STRING;
  } else {
    for(kt = keywords; kt->keyword != NULL; ++kt) {
      if(strncmp(ptr, kt->keyword, strlen(kt->keyword)) == 0) {
	nextptr = ptr + strlen(kt->keyword);

       if (kt->token == TOKENIZER_REM)                                              // rem, @title,@param,@default
        {
           while(*nextptr != 0 && *nextptr != '\r' && *nextptr != '\n') ++nextptr;  // get following value
        }
	return kt->token;
      }
    }
  }

  if((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')) {
    nextptr = ptr + 1;

    return TOKENIZER_VARIABLE;
  }

  return TOKENIZER_ERROR;
}
//---------------------------------------------------------------------------
void
tokenizer_init(const char *program)
{
  ptr = program;
  current_line = 1;
  current_token = get_next_token();
  while (current_token==TOKENIZER_CR && !tokenizer_finished()) tokenizer_next();
}
//---------------------------------------------------------------------------
int
tokenizer_token(void)
{
  return current_token;
}
//---------------------------------------------------------------------------
void
tokenizer_next(void)
{

  if(tokenizer_finished()) 
  {
    return;
  }

  DEBUG_PRINTF("tokenizer_next: %p\n", nextptr);
  ptr = nextptr;
  while((*ptr == ' ') || (*ptr == '\t')) 
   {
    ++ptr;
   }
  current_token = get_next_token();
  DEBUG_PRINTF("tokenizer_next: '%s' %d\n", ptr, current_token);
  return;
}
//---------------------------------------------------------------------------
int
tokenizer_num(void)
{
  return atoi(ptr);
}
//---------------------------------------------------------------------------
int tokenizer_stringlookahead() 
{                                                 // return 1 (true) if next 'defining' token is string not integer
  char * saveptr = (char *)ptr;;
  char * savenextptr = (char *)nextptr;
  int token = current_token;
  int si = -1;
  
  while (si == -1) 
  {
     if (token == TOKENIZER_CR || token == TOKENIZER_ENDOFINPUT)
	    si = 0;
	 else if (token == TOKENIZER_NUMBER || token == TOKENIZER_VARIABLE)
	    si = 0;                                 // number or numeric var
	 else if (token == TOKENIZER_STRING)
	    si = 1;
       else if (token >= TOKENIZER_ELAPSED_TIME)
	    si = 1;                                // numeric function

     token = get_next_token();   
  }
  ptr = saveptr;
  nextptr = savenextptr;
  return si; 
}
//---------------------------------------------------------------------------

void
tokenizer_string(char *dest, int len)
{
  char *string_end;
  int string_len;
  
  if(tokenizer_token() != TOKENIZER_STRING) {
    return;
  }
  string_end = strchr(ptr + 1, '"');
  if(string_end == NULL) {
    return;
  }
  string_len = string_end - ptr - 1;
  if(len < string_len) {
    string_len = len;
  }
  memcpy(dest, ptr + 1, string_len);
  dest[string_len] = 0;
}
//---------------------------------------------------------------------------
void
tokenizer_label(char *dest, int len)
{
  char *string_end;
  int string_len;
  
  if(tokenizer_token() != TOKENIZER_LABEL) {
    return;
  }
  //allow string \r, \n, tabulation and space to end labels
  string_end = strpbrk(ptr + 1, " \t\r\n");

  if(string_end == NULL) {
    return;
  }
  string_len = string_end - ptr - 1;
  if(len < string_len) {
    string_len = len;
  }
  memcpy(dest, ptr + 1, string_len);
  dest[string_len] = 0;
}
//---------------------------------------------------------------------------
void
tokenizer_error_print(void)
{
  DEBUG_PRINTF("tokenizer_error_print: '%s'\n", ptr);
}
//---------------------------------------------------------------------------
int
tokenizer_finished(void)
{
  return *ptr == 0 || current_token == TOKENIZER_ENDOFINPUT;
}
//---------------------------------------------------------------------------
int
tokenizer_variable_num(void)
{
  return *ptr - (*ptr>='a'?'a':('A'-26));
}
//---------------------------------------------------------------------------
int tokenizer_line_number(void)
{
  return current_line;
}
