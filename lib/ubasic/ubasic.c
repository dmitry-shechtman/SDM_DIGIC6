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
#include "../../include/ubasic.h"
#include "../../include/platform.h"
#include "../../include/script.h"
#include <string.h>
#include <fcntl.h>
#include <io.h>
#else
#include "../../include/ubasic.h"
#include "../../include/platform.h"
#include "../../include/script.h"
#endif
#include "../../core/gui_osd.h"
#include "tokenizer.h"
#include "../../include/keyboard.h"
#include "../../core/kbd.h"
#include "../../include/conf.h"
#include "../../include/gui.h"
#include "../../core/batt_grid.h"
#include "math.h"
#include "camera_functions.h"
#include "stdlib.h"
 
#if defined(OPT_ALARIK)
#include "console.h"
#endif
#ifdef CAM_HAS_GPS
extern void _GPS_UpdateData();
#endif
 
extern void _SleepTask(long msec);
char *logbuf;
extern char *kaplogbuf; 
unsigned char gExAlt,gConsoleScroll;
extern void _PTM_SetCurrentItem(int id,int state);
extern onHP onHalfPress;
extern const int ovr[];
extern OSD_pos consoleOrigin;
extern int inf_focus,gNumOfBvReadings,gBv,gUsbHigh,fpd;
extern int  gNearPoint,gZoomPoint,gFocalPoint,gCurrentFocus;
extern int nd_to_be_in,fastShoot,fastRetVarNum;
extern  long get_batt_average();
extern float myhyp,coc,deviation,my_max_deviation,max_distance;
extern long myav;
extern int shift,afl,ael;
extern Conf conf;
extern long sensor_width;
extern unsigned int screen_width,screen_height;
extern int sertx,sd_repeat;
extern char pcode;
extern char lobyte;
extern char hibyte;
extern int rx_ok,gCurrentNDFilterState,gCurrentNDFilterConstantExposure;
extern int up,ub;
extern int block_script,sertx;
extern int gPTPsession;
extern unsigned char gRawMode,displayedMode,gHPtimerCBfunc,bigZoomXPos,bigZoomYPos;
int get_luminance,put_off_ev96,checkLensCap,avBv;
static const double log_2=0.6931471805599;      //natural logarithm of 2
char buf[128]={'\0'}; 
int gFlashDisable=0;                                  
int gSpecial_token;                               
int gConsoleLine= -1;
int gVideoDisabled;                               
int af_all;                                      
int st_tv_exp=0;                                 
int st_tv_ev=0;                                  
static int current_display_mode,gStartTime,elapsedMode;
int bls = 1; 
int speed = 1;
int yield = 0;
int has_refocused=0;
int gFastTv,gMaxIso96=603;
short gAp96;
unsigned char gEnableBigZoom=0,gStartHour,gEndHour,gStartMinutes,gEndMinutes,gLineCommand,gEVfromScript;
#define MAX_SPEED 5
#include "stdlib.h" 

#ifdef DEBUG
#include <stdio.h>
#endif

static unsigned char block_comment;
static const char nullstring[] = "\0"; 
#define MAX_BUFFERLEN    4000
static char stringbuffer[MAX_BUFFERLEN];
static int  freebufptr = 0;

static char const *program_ptr;
#define MAX_STRINGLEN 80                        
static char string[MAX_STRINGLEN];

#define MAX_GOSUB_STACK_DEPTH 10
static short gosub_stack[MAX_GOSUB_STACK_DEPTH];
static int gosub_stack_ptr;

#define MAX_IF_STACK_DEPTH 4
static short if_stack[MAX_IF_STACK_DEPTH];
static int if_stack_ptr;

static int select_stack_ptr;
#if defined(OPT_ALARIK)
struct select_state 
{
  int select_value;
  short case_run;
};

#define MAX_SELECT_STACK_DEPTH 4
static struct select_state select_stack[MAX_SELECT_STACK_DEPTH];
#endif

struct for_state {
  short line_after_for;
  short for_variable;
  int to;
  int step;
};

#define MAX_WHILE_STACK_DEPTH 10
static short while_stack[MAX_WHILE_STACK_DEPTH];
static int while_stack_ptr;

#define MAX_DO_STACK_DEPTH 4
static short do_stack[MAX_DO_STACK_DEPTH];
static int do_stack_ptr;

#define MAX_FOR_STACK_DEPTH 4
static struct for_state for_stack[MAX_FOR_STACK_DEPTH];
static int for_stack_ptr;

#define MAX_VARNUM 52
static int variables[MAX_VARNUM];

static int  print_screen_p;             //print_screen predicate: 0-off 1-on.
static int  print_screen_d = -1;        //print_screen file descriptor.
int fdLog = -1;                         
int fdKapLog = -1;
static const char print_screen_file[] ="A/SDM/PR_SCREEN.TXT";
char log_file[30] ={'\0'};
#define TL_PARAMS_NAME "A/SDM/PARAMS/T_%1d.txt"
#define TL_PARAM_NUMS 20                

 int ended;

static int expr(void);
static void line_statement(void);
static void endif_statement(void);
static void statement(void);
static int relation(void);
static char sdmString[40]={0};
static char systemString[40]={0};
static long elapsedAccumulated;

#define USBD_MASK  0x4

char *get_systemString_address()
{
 return systemString;
}
int ubasic_error;
const char *ubasic_errstrings[UBASIC_E_ENDMARK] =
{
    "No err",
    "Parse err",
    "Unk stmt",
    "Unk key",
    "Unk label",
    "gosub: Stack ful",
    "bad return",
    "if: Stack ful",
    "bad endif",
    "select: Stack ful",
    "bad end_select",
    "for: Stack ful",
    "bad next",
    "do: Stack ful",
    "bad until",
    "while: Stack ful",
    "bad wend",
    "Unk err" 
};
 
//---------------------------------------------------------------------------
int
ubasic_linenumber()
{
  return tokenizer_line_number();
}
//---------------------------------------------------------------------------
void
ubasic_init(const char *program)
{
  program_ptr = program;
  for_stack_ptr = gosub_stack_ptr = while_stack_ptr = do_stack_ptr = if_stack_ptr = select_stack_ptr = 0;
  tokenizer_init(program);
  ended = 0;
  print_screen_p = 0;
  if (print_screen_d >= 0) {
    close(print_screen_d);
    print_screen_d = -1;
  }
  ubasic_error = UBASIC_E_NONE;
}
/*---------------------------------------------------------------------------*/
static int ubasic_get_key_arg() 
{
  int k;
  tokenizer_string(string, sizeof(string));
  tokenizer_next();
  k = keyid_by_name(string);
  if (k <= 0)
    ubasic_error = UBASIC_E_UNK_KEY;
  return k;
}
//---------------------------------------------------------------------------
static void
accept(int token)
{

  if(token != tokenizer_token()) 
  {
    DEBUG_PRINTF("Token not what was expected (expected %d, got %d)\n",token, tokenizer_token());
    tokenizer_error_print();
     tokenizer_next();
     ended = 1;
     ubasic_error = UBASIC_E_PARSE;
     return;
  }
  DEBUG_PRINTF("Expected %d, got it\n", token);
  tokenizer_next();
}
//---------------------------------------------------------------------------
static void
accept_cr()
{

    while(tokenizer_token() != TOKENIZER_CR &&
        tokenizer_token() != TOKENIZER_ENDOFINPUT)
      tokenizer_next();
    accept(TOKENIZER_CR);
}
//---------------------------------------------------------------------------
static int
varfactor(void)
{
  int r;
  DEBUG_PRINTF("varfactor: obtaining %d from variable %d\n", variables[tokenizer_variable_num()], tokenizer_variable_num());
  r = ubasic_get_variable(tokenizer_variable_num());      
  accept(TOKENIZER_VARIABLE);
  return r;
}
//---------------------------------------------------------------------------
 char* sfactor()                                                               
 { 
  int i,j,y,t,intValue;
  unsigned long tt;
  static struct tm *ttm;
  char c[1];
   switch(tokenizer_token()) 
    {
     case TOKENIZER_ELAPSED_TIME:
     accept(TOKENIZER_ELAPSED_TIME);
     if(shooting_get_day_seconds()<gStartTime)                                 
      {
       elapsedAccumulated+= 86400-gStartTime+shooting_get_day_seconds();       
       gStartTime=shooting_get_day_seconds();
      }  
     else elapsedAccumulated = shooting_get_day_seconds()- gStartTime;      
     t = elapsedAccumulated;
     if(!elapsedMode)
     sprintf(sdmString,"%02d:%02d:%02d",t/3600,(t%3600)/60,(t%3600)%60);       
     else sprintf(sdmString,"%03d:%02d:%02d:%02d",t/86400,(t%86400)/3600,((t%86400)%3600)/60,((t%86400)%3600)%60);  
     break;

     case TOKENIZER_TIME:
     accept(TOKENIZER_TIME);
     get_time(&sdmString[0]);
      break;

     case TOKENIZER_DATE:
     accept(TOKENIZER_DATE);
     intValue=0;
     if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)      
       intValue= expr();  
      if(intValue<=6)get_date(&sdmString[0],intValue);
      else get_date(&sdmString[0],0);
 
      break;

      case TOKENIZER_VALID_MODES:      
 
      accept(TOKENIZER_VALID_MODES); 
       sdmString[18]='\0';
       sdmString[0]='>';
       sdmString[17]='<';
       for(i=0;i<16;i++)                            
        {
         t=0;y=1;                                   
                                                    
         for(j=1;j<5;j++)                           
          {
           if(shooting_mode_chdk2canon(i*4+j) != -1)
           t=t+y;                                   
           y=y*2;                                   
          }
           sprintf(c,"%x",t);
           sdmString[16-i]=*c;
        }                                      
      break;

     case TOKENIZER_PRINT_TV:
      accept(TOKENIZER_PRINT_TV);
      get_tv(&sdmString[0],onHalfPress.Tv96);     
	break;
 
      case TOKENIZER_PRINT_AV:
       accept(TOKENIZER_PRINT_AV);
       get_av(&sdmString[0],onHalfPress.Av96);
	break;
      
      case TOKENIZER_EV_CORRECTION:
      accept(TOKENIZER_EV_CORRECTION);
      get_ev_correction(&sdmString[0],(short int)shooting_get_prop(PROPCASE_EV_CORRECTION_1));
      break;
      
      case TOKENIZER_MESSAGE: 
      accept(TOKENIZER_MESSAGE);      
      return systemString;
      break;
      
      case  TOKENIZER_ALTITUDE:
      accept(TOKENIZER_ALTITUDE);
#if defined(CAM_HAS_GPS)
       tGPS gps;
       _GPS_UpdateData();
       get_property_case(PROPCASE_GPS, &gps, sizeof(gps));
       double height = 0.0;
       height = gps.height[0]/(gps.height[1]*1.0);
       sprintf(sdmString,"%d/%d",gps.height[0],gps.height[1]);
#else
      sprintf(sdmString,"GPS not supported");
#endif
      break;

	  case TOKENIZER_BIGZOOM:
      accept(TOKENIZER_BIGZOOM);  
	  sprintf(sdmString,"{}");
	  break;
      
   default:	  
     sprintf(sdmString,""); 
    }
   return sdmString;
}
//---------------------------------------------------------------------------

static int
factor(void)                                              
{
  int r = 0; 
  int zp; 
  int to; 
  tConfigVal configVal;
  static struct tm *ttm;
  DEBUG_PRINTF("factor: token %d\n", tokenizer_token());
  switch(tokenizer_token()) 
  {
                                                        
  case TOKENIZER_NUMBER:
    r = tokenizer_num();
    DEBUG_PRINTF("factor: number %d\n", r);
    accept(TOKENIZER_NUMBER);
    break;
  case TOKENIZER_LEFTPAREN:
    accept(TOKENIZER_LEFTPAREN);
    r = relation();
    accept(TOKENIZER_RIGHTPAREN);
    break;
  case TOKENIZER_PLUS:
    accept(TOKENIZER_PLUS);
    r = factor();
    break;
  case TOKENIZER_MINUS:
    accept(TOKENIZER_MINUS);
    r = - factor();
    break;
  case TOKENIZER_LNOT:
    accept(TOKENIZER_LNOT);
    r = ! relation();
    break;

 case TOKENIZER_GET_DAY_SECONDS:
    accept(TOKENIZER_GET_DAY_SECONDS);
    r = shooting_get_day_seconds();     
    break;
 case TOKENIZER_GET_TICK_COUNT:
    accept(TOKENIZER_GET_TICK_COUNT);
    r = shooting_get_tick_count();     
    break;

 case TOKENIZER_GET_MODE:                     
    accept(TOKENIZER_GET_MODE);
    int m=mode_get()&MODE_SHOOTING_MASK;
    int mode_video=MODE_IS_VIDEO(m);
    if ((mode_get()&MODE_MASK) != MODE_PLAY) r = 0;
    if ((mode_get()&MODE_MASK) == MODE_PLAY) r = 1;
    if (((mode_get()&MODE_MASK) != MODE_PLAY) && mode_video) r = 2;
    break;

 case TOKENIZER_IS_PRESSED:
    accept(TOKENIZER_IS_PRESSED);
    tokenizer_string(string, sizeof(string));
    tokenizer_next();
    r = ubasic_camera_is_pressed(string);
    break;

 case TOKENIZER_IS_KEY:
    accept(TOKENIZER_IS_KEY);
    tokenizer_string(string, sizeof(string));
    tokenizer_next();
    r = ubasic_camera_is_clicked(string);     
    break;

 case TOKENIZER_SCRIPT_AUTOSTART:
    accept(TOKENIZER_SCRIPT_AUTOSTART);
    r = ubasic_camera_script_autostart();     
    break;
 case TOKENIZER_GET_USB_POWER:
    accept(TOKENIZER_GET_USB_POWER);
    r = getUsbDuration();
    break;

 case TOKENIZER_DR:                           
    accept(TOKENIZER_DR);
    r = rx_ok;                               
    rx_ok=0;                                 
    break;

  case TOKENIZER_GET_FREE_DISK_SPACE:
    accept(TOKENIZER_GET_FREE_DISK_SPACE);
    r = GetFreeCardSpaceKb();
    break;
 
  case TOKENIZER_GET_VIDEO_BUTTON:
    accept(TOKENIZER_GET_VIDEO_BUTTON);
    #if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
    r = 1;
    #else
    r = 0;
    #endif
    break;

  case TOKENIZER_GET_PROP:
    accept(TOKENIZER_GET_PROP);
    int var = expr();
    r = shooting_get_prop(var);
    break;

 case TOKENIZER_GET_FLASH_MODE:
    accept(TOKENIZER_GET_FLASH_MODE);
    r = shooting_get_prop(PROPCASE_FLASH_MODE);
    break;

 case TOKENIZER_GET_SHOOTING:
    accept(TOKENIZER_GET_SHOOTING);
    r = shooting_get_prop_int(PROPCASE_SHOOTING);
    break;

 case TOKENIZER_GET_SHOOTING_MODE:
    accept(TOKENIZER_GET_SHOOTING_MODE);
    int vmode;
    get_property_case(PROPCASE_SHOOTING_MODE, &vmode, 4);
    r = shooting_mode_canon2chdk(vmode);
    break;

 case TOKENIZER_GET_RESOLUTION: 
 	accept(TOKENIZER_GET_RESOLUTION); 
 	r = shooting_get_prop(PROPCASE_RESOLUTION); 
 	break;
 case TOKENIZER_GET_QUALITY: 
 	accept(TOKENIZER_GET_QUALITY); 
 	r = shooting_get_prop(PROPCASE_QUALITY); 
      break;

  case TOKENIZER_GET_ZOOM_STEPS:
    accept(TOKENIZER_GET_ZOOM_STEPS);
    r = zoom_points;
    break;

  case TOKENIZER_GET_ZOOM:
    accept(TOKENIZER_GET_ZOOM);
    r = shooting_get_zoom();
    break;

case TOKENIZER_GET_FOV:
      accept(TOKENIZER_GET_FOV);
      zp=gZoomPoint;
      r = get_fov(zp);     
      break;
 
case TOKENIZER_GET_EXP_COUNT:
    accept(TOKENIZER_GET_EXP_COUNT);
    r = get_exposure_counter();
    break;

case TOKENIZER_IMAGE_NUMBER:
     accept(TOKENIZER_IMAGE_NUMBER);
     r = lastObjectNumber()+1;
     break;

case TOKENIZER_IS_CAPTURE_MODE_VALID: {
     accept(TOKENIZER_IS_CAPTURE_MODE_VALID);
     int modenum = expr();
     if (shooting_mode_chdk2canon(modenum) == -1)
        r = 0;
     else
        r = 1;
     break;
  }

case TOKENIZER_GET_CONFIG_VALUE:
    accept(TOKENIZER_GET_CONFIG_VALUE);
    int var1 = expr();
    int var2 = expr();
    if( conf_getValue(var1, &configVal) == CONF_VALUE) r = configVal.numb; 
    else r = var2;
    break;

  case TOKENIZER_GET_TV96:
    accept(TOKENIZER_GET_TV96);
    r = shooting_get_tv96();
    break;

  case TOKENIZER_GET_AV96:
    accept(TOKENIZER_GET_AV96);
    r = shooting_get_av96();
    break;  
    
  case TOKENIZER_GET_MIN_AV96:
    accept(TOKENIZER_GET_MIN_AV96);
    r = shooting_get_prop(PROPCASE_MIN_AV);
    break; 

  case TOKENIZER_GET_BV96:
    accept(TOKENIZER_GET_BV96);
    r = shooting_get_bv96();
    break;   

  case TOKENIZER_GET_SV96:
    accept(TOKENIZER_GET_SV96);
    r = shooting_get_prop(PROPCASE_SV);
    break;   

  case TOKENIZER_FOCUS_MODE:
    accept(TOKENIZER_FOCUS_MODE);
    r = shooting_get_prop(PROPCASE_FOCUS_MODE);
    break;
 
  case TOKENIZER_GET_HOURS:
    accept(TOKENIZER_GET_HOURS);
    ttm = (struct tm *) get_localtime();
    r = ttm->tm_hour;
    break;

  case TOKENIZER_GET_MINUTES:
    accept(TOKENIZER_GET_MINUTES);
    ttm = (struct tm *)get_localtime();
    r = ttm->tm_min;
    break;

  case TOKENIZER_GET_SECONDS:
    accept(TOKENIZER_GET_SECONDS);
    ttm = (struct tm *)get_localtime();
    r = ttm->tm_sec;
    break;

  case TOKENIZER_GET_BRIGHTNESS:
    accept(TOKENIZER_GET_BRIGHTNESS);
    r = gBv;
    break;

  case TOKENIZER_GET_REAL_ISO:
    accept(TOKENIZER_GET_REAL_ISO);
    r = (int)shooting_get_iso_real();
    break;
    
  case TOKENIZER_GET_ISO:
    accept(TOKENIZER_GET_ISO);
    r = shooting_get_prop(PROPCASE_ISO_MODE);
    break;    

  case TOKENIZER_GET_MOVIE_STATUS:
    accept(TOKENIZER_GET_MOVIE_STATUS);
    r = get_movie_status();
    break;
  case TOKENIZER_GET_LENS_MECHA_POSITION:
    accept(TOKENIZER_GET_LENS_MECHA_POSITION);
    r = GetMechaPos();
    break;

  case TOKENIZER_GET_OVTV96:
    accept(TOKENIZER_GET_OVTV96);
    if (conf.tv_override >=0 && conf.tv_override <=52 && conf.tv_override != 19) 
    return ovr[conf.tv_override];
    else return -1;
    break;

  case TOKENIZER_SET_REMOTE_TIMING:          
    accept(TOKENIZER_SET_REMOTE_TIMING);
    to= expr();
    if(to>500)to=500;
    if (to>0) r = start_usb_HPtimer(to*1000);
    else r = stop_usb_HPtimer();
    break;
  
   case TOKENIZER_PSUSBAT:                   
    accept(TOKENIZER_PSUSBAT);
    to= expr();
    if(to>500)to=500;
    if (to>0) r=start_usb_HPtimer(to*1000);
    else r= start_usb_HPtimer(1000);
    break;  
    
    case TOKENIZER_PSUSBOFF:
    accept(TOKENIZER_PSUSBOFF);
    r=stop_usb_HPtimer();
    break;  
    
  case TOKENIZER_GET_DRIVE_MODE:
    accept(TOKENIZER_GET_DRIVE_MODE);
    r = shooting_get_drive_mode();
    break;

  case TOKENIZER_SDMDM:
    accept(TOKENIZER_SDMDM);
    r = displayedMode;
    break;

  case TOKENIZER_CAM_HAS_ND:
    accept(TOKENIZER_CAM_HAS_ND);
#if !defined(CAM_HAS_ND_FILTER)
    r=0;
#else
 #if !defined(CAM_HAS_IRIS_DIAPHRAGM)
    r = 1;
 #else
    r = 2;
 #endif
#endif
    break; 

  case TOKENIZER_CAM_TV_MODE:
    accept(TOKENIZER_CAM_TV_MODE);
#if defined(CAM_HAS_USER_TV_MODES)
  return 1;
#else
  return 0;
#endif 
  break;

  case TOKENIZER_CAM_AV_MODE:
    accept(TOKENIZER_CAM_AV_MODE);
#if defined(CAM_HAS_IRIS_DIAPHRAGM)
  return 1;
#else
  return 0;
#endif 
  break;
 
  case TOKENIZER_LFA:
 	  accept(TOKENIZER_LFA);
        to = expr();
       if(!shooting_can_focus())
        {
         if(!enable_focus_override())
          r = 0;
        }
        else
         {
          if(to<CAMERA_MIN_DIST)to=CAMERA_MIN_DIST; 
          if(to>CAMERA_MAX_DIST)to=CAMERA_MAX_DIST;          
          lens_set_focus_pos(to+fpd);
          r = 1;
         }
  break;
 
  case TOKENIZER_LFAI:
        accept(TOKENIZER_LFAI);
        if(!shooting_can_focus())
        {
         if(!enable_focus_override())
          r = 0;
        }
        else
         {
          lens_set_focus_pos(CAMERA_MAX_DIST);
          r = 1;
         }
  break;
 
  case  TOKENIZER_EFO:
        accept(TOKENIZER_EFO);
        r = enable_focus_override();
  break;
   
  case  TOKENIZER_DFO:
        accept(TOKENIZER_DFO);
        disable_focus_override();
        r = 1;
  break; 

  case TOKENIZER_GET_FL:
       accept(TOKENIZER_GET_FL);
       r=gFocalPoint/100;           
  break;

  case TOKENIZER_GET_EFL:
       accept(TOKENIZER_GET_EFL);
       r=get_effective_focal_length(gZoomPoint)/1000;
  break;
  
  case TOKENIZER_GET_RFL_FOR_STEP:
       accept(TOKENIZER_GET_RFL_FOR_STEP);
       to = expr();      
       r=get_focal_length(to);
  break;
 
  case  TOKENIZER_SET_ZOOM_RFL:
   accept(TOKENIZER_SET_ZOOM_RFL);
   to = expr();
   r = get_step_for_real_fl(to);
  break;
 
  case TOKENIZER_GET_NEARPOINT:
   accept(TOKENIZER_GET_NEARPOINT);
   r = onHalfPress.NearPoint;
   break;
   
  case TOKENIZER_GET_FARPOINT:
   accept(TOKENIZER_GET_FARPOINT);
   r = onHalfPress.FarPoint;
   break;
 
 case TOKENIZER_SHOT_COUNT:
   accept(TOKENIZER_SHOT_COUNT);
   get_parameter_data(1,&r,4);
   break; 
   
  case TOKENIZER_SMS:
   accept(TOKENIZER_SMS);
   r = shooting_mode_chdk2canon(MODE_SCN_STITCH);
   if(r==-1)
    {
     r = shooting_mode_chdk2canon(MODE_STITCH);
    }
   if(r==-1)
    r=0;
   else r = 1;
   break; 

 case TOKENIZER_TIME_DELAY_ACTIVE:
  accept(TOKENIZER_TIME_DELAY_ACTIVE);
  if(CAM_PROPSET==1)
   {
    if((shooting_get_prop(PROPCASE_DRIVE_MODE)==2)&&(shooting_get_prop(219)!=2))
     r=1;
    else r=0;
   }
  else
   {
   #if defined(PROPCASE_TIMER_MODE)
    if(shooting_get_prop(PROPCASE_TIMER_MODE)&&(shooting_get_prop(PROPCASE_TIMER_MODE)<3))
     r=1;
     else r=0;
   #else
    r=0;
   #endif
   }
  break;

  case TOKENIZER_GET_EV_CORRECTION:
    accept(TOKENIZER_GET_EV_CORRECTION);
    r = shooting_get_prop(PROPCASE_EV_CORRECTION_1);
    break;

  case TOKENIZER_GET_ORIENTATION:
    accept(TOKENIZER_GET_ORIENTATION);
    r = shooting_get_prop(PROPCASE_ORIENTATION_SENSOR);
    break;

  case TOKENIZER_IS_CAF_ON:
    accept(TOKENIZER_IS_CAF_ON);
#if defined(PROPCASE_CONTINUOUS_AF)
    r = shooting_get_prop(PROPCASE_CONTINUOUS_AF);
#else
    r = 0;
#endif
    break;

  case TOKENIZER_IS_SAF_ON:
    accept(TOKENIZER_IS_SAF_ON);
#if defined(PROPCASE_SERVO_AF)
    r = shooting_get_prop(PROPCASE_SERVO_AF);
#else
    r = 0;
#endif
    break;
  case TOKENIZER_USBPA:
    accept(TOKENIZER_USBPA);
    to = expr();
    
    if(!to)  
    {
    to = getUsbDuration();
    if(to)
     {
      if(to<5)r=1;
      else if(to<8)r=2;
      else if(to<11)r=3;
      else if(to<14)r=4;
      else if(to<17)r=5;
      else if(to<20)r=6;
      else r = 0;
     }
     else
      r = 0;
     }
     else if (to==1)  
     {
      to = getUsbDuration();
      if(to)
      {
      if(to<2)r=1;
      else if(to<4)r=2;
      else if(to<6)r=3;
      else if(to<8)r=4;
      else if(to<10)r=5;
      else if(to<12)r=6;
      else if(to<14)r=7;
      else if(to<16)r=8;
      else if(to<18)r=9;
      else if(to<20)r=10;
      else if(to<22)r=11;
      else if(to<42)r=12;
      else r = 13; 
      }
      else r = 0;    
     }
    break;  
   
  case TOKENIZER_IS_USB_HIGH:
    accept(TOKENIZER_IS_USB_HIGH);
     r = get_usb_power(1);
    break; 

  case TOKENIZER_GET_FOCUS:
    accept(TOKENIZER_GET_FOCUS);
    r = lens_get_focus_pos_from_lens();  
    break;  

  case TOKENIZER_AFA_OFF:
   accept(TOKENIZER_AFA_OFF);
#if defined(PROPCASE_AF_ASSIST_BEAM)
  shooting_set_prop(PROPCASE_AF_ASSIST_BEAM,0);
  r = (shooting_get_prop(PROPCASE_AF_ASSIST_BEAM)==0);
#else
 r=1;
#endif
    break; 

    case TOKENIZER_SYNC_TICKS:
    accept(TOKENIZER_SYNC_TICKS);
    var = shooting_get_day_seconds();
   do{_SleepTask(10);}
   while(shooting_get_day_seconds()==var);
   r = shooting_get_tick_count();
   conf.flag_6=r;
   break;
   
 case TOKENIZER_CAMPOS:
   accept(TOKENIZER_CAMPOS);
  r = (conf.camera_position)?1:0;
  break;

  case TOKENIZER_GET_SHOOT_FAST_PARAM:
   accept(TOKENIZER_GET_SHOOT_FAST_PARAM);
   var=expr();
   if((var<0) || (var>3))var=0;
   switch (var) 
   {
    case 0:
    r=fast_params.exp_comp;
    break;
    case 1:
    r=fast_params.pref_ap;
    break;
    case 2:
    r=fast_params.min_ap;
    break;
    case 3:
    r=fast_params.ev_step;
    break;
   }
   break;
  
 case TOKENIZER_CT:                 // Custom timer
   accept(TOKENIZER_CT);
#if defined(UI_CT_PROP)
   var=expr();
  if(var==0)_PTM_SetCurrentItem(UI_CT_PROP,0); //timer off
  else _PTM_SetCurrentItem(UI_CT_PROP,4);      // timer on
  r = 1;
#else
 play_sound(6);	
  r = 0;
#endif
   break;
   
  default:
    r = varfactor();                 
    break;
  }
  return r;
}
//---------------------------------------------------------------------------
static int
term(void)                              
{
  int f1, f2;
  int op;

  f1 = factor();                         
  op = tokenizer_token();                
 if((gSpecial_token)&& ((op == TOKENIZER_SLASH)||(op == TOKENIZER_PERIOD)||(op == TOKENIZER_COLON))) op= TOKENIZER_COMMA;
  DEBUG_PRINTF("term: token %d\n", op);
  while(op == TOKENIZER_ASTR ||
	op == TOKENIZER_SLASH ||
	op == TOKENIZER_LT ||
	op == TOKENIZER_GT ||
	op == TOKENIZER_GE ||
	op == TOKENIZER_LE ||
	op == TOKENIZER_NE ||
	op == TOKENIZER_EQ ||
	op == TOKENIZER_XOR || 
        op == TOKENIZER_OR ||
	op == TOKENIZER_MOD) 
    {
    tokenizer_next();                             
    f2 = factor();                                
    DEBUG_PRINTF("term: %d %d %d\n", f1, op, f2);
    switch(op)                                    
     {
    case TOKENIZER_ASTR:
      f1 = f1 * f2;
      break;
    case TOKENIZER_SLASH:
      f1 = f1 / f2;
      break;
    case TOKENIZER_MOD:
      f1 = f1 % f2;
      break;
    case TOKENIZER_LT:
      f1 = f1 < f2;
      break;
    case TOKENIZER_GT:
      f1 = f1 > f2;
      break;
    case TOKENIZER_EQ:
      f1 = f1 == f2;
      break;
    case TOKENIZER_NE:
      f1 = f1 != f2;
      break;
    case TOKENIZER_LE:
      f1 = f1 <= f2;
      break;
    case TOKENIZER_GE:
      f1 = f1 >= f2;
      break;
    case TOKENIZER_OR:
      f1 = f1 | f2;
      break;
    case TOKENIZER_XOR:
      f1 = f1 ^ f2;
      break;
    }
    op = tokenizer_token();               
  }                                       
  DEBUG_PRINTF("term: %d\n", f1);
  return f1;                             
}
//---------------------------------------------------------------------------
static int
expr(void)                                         
{
  int t1, t2;
  int op;
  
  t1 = term();                                     
  op = tokenizer_token();                          
  DEBUG_PRINTF("expr: token %d\n", op);
  while(op == TOKENIZER_PLUS ||                   
	op == TOKENIZER_MINUS ||
	op == TOKENIZER_AND ||
        op == TOKENIZER_LOR ||
	op == TOKENIZER_XOR) 
  {                                               
    tokenizer_next();                             
    t2 = term();                                   
    DEBUG_PRINTF("expr: %d %d %d\n", t1, op, t2);
    switch(op)                                    
   {
    case TOKENIZER_PLUS:
      t1 = t1 + t2;
      break;
    case TOKENIZER_MINUS:
      t1 = t1 - t2;
      break;
    case TOKENIZER_AND:
      t1 = t1 & t2;
      break;
    case TOKENIZER_LOR:
      t1 = t1 || t2;
      break;
    }
    op = tokenizer_token();                      
  }
  DEBUG_PRINTF("expr: %d\n", t1);
  return t1;                                     
}
//---------------------------------------------------------------------------

static int
relation(void)
{
  int r1, r2;
  int op;
  
  r1 = expr();
  op = tokenizer_token();
  DEBUG_PRINTF("relation: token %d\n", op);
  while(op == TOKENIZER_LAND) {
    tokenizer_next();
    r2 = expr();
    DEBUG_PRINTF("relation: %d %d %d\n", r1, op, r2);
    switch(op) {
    case TOKENIZER_LAND:
      r1 = r1 && r2;
      break;
    }
    op = tokenizer_token();
  }
  return r1;
}

#if 0
//---------------------------------------------------------------------------
static void
jump_linenum(int linenum)
{
  tokenizer_init(program_ptr);
  while(tokenizer_num() != linenum) {
    do {
      do {
	tokenizer_next();
      } while(tokenizer_token() != TOKENIZER_CR &&
	      tokenizer_token() != TOKENIZER_ENDOFINPUT);
      if(tokenizer_token() == TOKENIZER_CR) {
	tokenizer_next();
      }
    } while(tokenizer_token() != TOKENIZER_NUMBER);
    DEBUG_PRINTF("jump_linenum: Found line %d\n", tokenizer_num());
  }
}
#endif

//---------------------------------------------------------------------------
static void
jump_line(int linenum)
{
  tokenizer_init(program_ptr);
  while(tokenizer_line_number() != linenum) {
    tokenizer_next();
  }
  /*swallow the CR that would be read next */
  if (linenum >1) accept(TOKENIZER_CR);  //Dave Mitchell 26/6/2009

}
//---------------------------------------------------------------------------
//TODO: error handling?
int
jump_label(char * label)
{
  char currLabel[MAX_STRINGLEN];
  tokenizer_init(program_ptr);
  currLabel[0] = 0;
  while(tokenizer_token() != TOKENIZER_ENDOFINPUT) {
    tokenizer_next();
    if (tokenizer_token() == TOKENIZER_LABEL) {
      tokenizer_label(currLabel, sizeof(currLabel));
      tokenizer_next();
      if(strcmp(label, currLabel) == 0) {
        accept(TOKENIZER_CR);
        DEBUG_PRINTF("jump_linenum: Found line %d\n", tokenizer_line_number());
        break;
      }
    }
  }
  if (tokenizer_token() == TOKENIZER_ENDOFINPUT) 
  {
   if (state_kbd_script_run == 1) 
    { 
     DEBUG_PRINTF("Label %s not found", label);
     ubasic_error = UBASIC_E_UNK_LABEL;
    }
    return 0;
  } 
  else 
  {
      return 1;
  }
}
//---------------------------------------------------------------------------
static void
goto_statement(void)
{
  accept(TOKENIZER_GOTO);
  if(tokenizer_token() == TOKENIZER_STRING) {
    tokenizer_string(string, sizeof(string));
    tokenizer_next();
    jump_label(string);
  } else {
    DEBUG_PRINTF("ubasic.c: goto_statement(): no label specified\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNK_LABEL;
  }
}
//---------------------------------------------------------------------------
static void
print_screen_statement(void)
{
  int val;
  accept(TOKENIZER_PRINT_SCREEN);
  val = expr();
  accept(TOKENIZER_CR);
  if (val && print_screen_d<0) 
  {
   print_screen_d = open(print_screen_file, O_WRONLY|O_CREAT|O_TRUNC, 0777);
  }
  print_screen_p = val;
}

//---------------------------------------------------------------------------
static void disable_logging_statement(void)
{
  accept(TOKENIZER_DISABLE_LOGGING);
  if(fdLog>=0)fdLog=-1;
  if(fdKapLog>=0)fdKapLog=-1; 
  accept(TOKENIZER_CR);
}
//---------------------------------------------------------------------------
static void enable_logging_statement(void)
{
  accept(TOKENIZER_ENABLE_LOGGING);
  int to=0; //shutup compiler
  char timebuf[30];  
  if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
   to = expr(); 
  
  if(!to)
  {
   if (fdLog<0) 
   {
    sprintf(log_file,"A/SDM/LOGS/LOGFILE_%04d.txt",get_target_file_num()+1);  
    fdLog = open(log_file, O_WRONLY|O_CREAT|O_TRUNC, 0777);
    fdKapLog = fdLog;    
   }
  }

  else if(to == 1)
  {
    int autofocus=1;
    if(conf.user_1==70001)autofocus=0; 
    sprintf(log_file,"A/SDM/LOGS/KAP_%04d.txt",get_target_file_num()+1); 
    fdKapLog = open(log_file, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0777);
    if(fdKapLog>=0)
    {
    sprintf(kaplogbuf,"\n\n--------------------------\nSDM CanonAssistant KAP log\n--------------------------\n\n");
    get_date(&timebuf[0],1);         
    sprintf(kaplogbuf+strlen(kaplogbuf),"%s\n\n",timebuf); 
    if(!autofocus)
                  sprintf(kaplogbuf+strlen(kaplogbuf),"Focus-override in %s\n\n",(conf.focus_override_method==1)?"autofocus mode":(conf.focus_override_method==2)?"autofocus-lock mode":"manual-focus mode");   
    else
      sprintf(kaplogbuf+strlen(kaplogbuf),"Camera autofocused each shot (except bracketed or BURST)\n\n");     
    close(fdKapLog);
    }
  }
 accept(TOKENIZER_CR);
}
//---------------------------------------------------------------------------
#if !defined(OPT_ALARIK)
static void
print_statement(void)
{
  static char buf[128]={'\0'};
  logbuf=&buf[0];
  buf[0]=0;
  
 if((!gConsoleScroll&&gLineCommand)||gConsoleScroll||(gConsoleLine==99)) 
 {
  gLineCommand=0; 
  do 
  {
    DEBUG_PRINTF("Print loop\n");
    if(tokenizer_token() == TOKENIZER_STRING) 
    {
     tokenizer_string(string, sizeof(string));
     sprintf(buf+strlen(buf),"%s", string); 
     tokenizer_next();
    }  

   else if(tokenizer_token() == TOKENIZER_COMMA) 
    {
     strcat(buf, " ");
     tokenizer_next();
    } 
   else if(tokenizer_token() == TOKENIZER_SEMICOLON) 
    {
     tokenizer_next();
    } 

   else if(tokenizer_token() >= TOKENIZER_ELAPSED_TIME)  
    {
     sprintf(buf+strlen(buf),"%s",sfactor());
    }
   else 
    {
     sprintf(buf+strlen(buf), "%d", expr());
    }
  } while(tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ENDOFINPUT && tokenizer_token() != TOKENIZER_ELSE);

  if(gConsoleLine != 99)script_console_add_line(buf);               
 
  if ((print_screen_p && print_screen_d>=0) || ((gConsoleLine==99)&& (fdLog>=0))) 
  {
   int bl = strlen(buf);
    buf[bl]='\n';
    if (print_screen_p && print_screen_d>=0)write(print_screen_d, buf, bl+1);
    else
     {
 #if defined(CAM_DRYOS)                                        
      open(log_file, O_WRONLY|O_CREAT|O_APPEND, 0777);		
 #endif     
      write(fdLog, buf, bl+1);
 #if defined(CAM_DRYOS)     
      close(fdLog);									                  
 #endif
     }
  }
  }
  DEBUG_PRINTF("End of print\n");
  accept_cr();
}
#endif

#ifdef OPT_ALARIK
static void
print_statement(void)
{
  static char buf[128];

  buf[0]=0;
  accept(TOKENIZER_PRNT);
  do {
    DEBUG_PRINTF("Print loop\n");
    if(tokenizer_token() == TOKENIZER_STRING) {
      tokenizer_string(string, sizeof(string));
      sprintf(buf+strlen(buf), "%s", string);
      tokenizer_next();
    } else if(tokenizer_token() == TOKENIZER_COMMA) {
      strcat(buf, " ");
      tokenizer_next();
    } else if(tokenizer_token() == TOKENIZER_SEMICOLON) {
      tokenizer_next();
    } else {
      sprintf(buf+strlen(buf), "%d", expr());
    }
  } while(tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ENDOFINPUT && tokenizer_token() != TOKENIZER_ELSE);
  script_console_add_line(buf);
  DEBUG_PRINTF("End of print\n");
  accept_cr();
}
#endif
//---------------------------------------------------------------------------
static void
prnt_statement(void)
{
 accept(TOKENIZER_PRNT);
 gConsoleLine=-1;  
 print_statement();
}

//---------------------------------------------------------------------------
static void
ip_statement(void)
{
  static char buf[128];
  buf[0]=0;

  do 
  {
    tokenizer_string(string, sizeof(string));
    sprintf(buf+strlen(buf), "%-25s", string);
    tokenizer_next();
  } while(tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ENDOFINPUT && tokenizer_token() != TOKENIZER_ELSE);

  script_console_add_line(buf);
  if (print_screen_p && print_screen_d>=0) 
   {
    int bl = strlen(buf);
    buf[bl]='\n';
    write(print_screen_d, buf, bl+1);
   }
  accept_cr();
}
//---------------------------------------------------------------------------
static void
if_statement(void)
{
  int r, else_cntr,endif_cntr,f_nt,f_sl;
  
  accept(TOKENIZER_IF);
  DEBUG_PRINTF("if_statement: get_relation\n");
  r = relation();
  DEBUG_PRINTF("if_statement: relation %d\n", r);
  accept(TOKENIZER_THEN);
  if (ended) {
    return;
  }

  if (tokenizer_token() == TOKENIZER_CR) {
    
    if(if_stack_ptr < MAX_IF_STACK_DEPTH) {
      if_stack[if_stack_ptr] = r;
      if_stack_ptr++;
    } else {
      DEBUG_PRINTF("if_statement: IF-stack depth exceeded\n");
      ended = 1;
      ubasic_error = UBASIC_E_IF_STACK_EXHAUSTED;
      return;
    }
    DEBUG_PRINTF("if_statement: stack_ptr %d\n", if_stack_ptr);
    accept(TOKENIZER_CR);
    if(r) {
      DEBUG_PRINTF("if_statement: result true\n");
      return;
    }else {
      DEBUG_PRINTF("if_statement: result false\n");

      else_cntr=endif_cntr=0; 
      f_nt=f_sl=0; 

      while(((tokenizer_token() != TOKENIZER_ELSE &&  tokenizer_token() != TOKENIZER_ENDIF)
           || else_cntr || endif_cntr) && tokenizer_token() != TOKENIZER_ENDOFINPUT){
        f_nt=0;
        
        if( tokenizer_token() == TOKENIZER_IF) {
          else_cntr+=1;
          endif_cntr+=1;
          f_sl=0;
          DEBUG_PRINTF("IF: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
        }
        if( tokenizer_token() == TOKENIZER_THEN) {
          f_nt=1;
          tokenizer_next();
          DEBUG_PRINTF("THEN: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
          if (tokenizer_token() != TOKENIZER_CR) { f_sl=1; }
          DEBUG_PRINTF("THEN_SL: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
        }
        if(tokenizer_token() == TOKENIZER_ELSE) {
          else_cntr--;
          DEBUG_PRINTF("ELSE: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
          if (else_cntr<0) { 
            DEBUG_PRINTF("ubasic.c: if_statement(): else without if-statement\n");
            ended = 1;
            ubasic_error = UBASIC_E_UNMATCHED_IF;
            return;
          }
        }
        if(!f_sl && (tokenizer_token() == TOKENIZER_ENDIF)) {
          endif_cntr--;
          if (endif_cntr != else_cntr) else_cntr--;
          DEBUG_PRINTF("ENDIF: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
        } else {
          if (f_sl && (tokenizer_token() == TOKENIZER_CR))  {
            f_sl=0;
            endif_cntr--;
            if (endif_cntr != else_cntr) else_cntr--;
            DEBUG_PRINTF("ENDIF_SL: line %d, token %d, else %d, end %d\n", tokenizer_line_number(),tokenizer_token(),else_cntr,endif_cntr);
          }else{
            if (tokenizer_token()==TOKENIZER_ENDIF){
              DEBUG_PRINTF("ubasic.c: if_statement(): endif in singleline if-statement\n");
              ended = 1;
              ubasic_error = UBASIC_E_PARSE;
              return;
            }
          }
        }
        if (!f_nt) { tokenizer_next(); }
      }
      if(tokenizer_token() == TOKENIZER_ELSE) { return; }
    }
    endif_statement();
  }else {
  
    if(r) {
      statement();
    } else {
      do {
        tokenizer_next();
      } while(tokenizer_token() != TOKENIZER_ELSE &&
        tokenizer_token() != TOKENIZER_CR &&
        tokenizer_token() != TOKENIZER_ENDOFINPUT);
      if(tokenizer_token() == TOKENIZER_ELSE) {
        accept(TOKENIZER_ELSE);
        statement();
      } else {
        accept(TOKENIZER_CR);
      }
    }
  }
}
//---------------------------------------------------------------------------
static void
else_statement(void)
{
  int r=0, endif_cntr, f_nt;
  
  accept(TOKENIZER_ELSE);
  if(if_stack_ptr > 0) {
    r = if_stack[if_stack_ptr-1];
  }
  else{
    DEBUG_PRINTF("ubasic.c: else_statement(): else without if-statement\n");
    ended = 1;
    ubasic_error = UBASIC_E_PARSE;
    return;
  }
  DEBUG_PRINTF("else_statement: relation %d\n", r);
  
  if (tokenizer_token() == TOKENIZER_CR) {
	  accept(TOKENIZER_CR);
	  if(!r) {
      DEBUG_PRINTF("else_statement: result true\n");
      return;
    } else {
      DEBUG_PRINTF("else_statement: result false\n");
      endif_cntr=0;
      while(((tokenizer_token() != TOKENIZER_ENDIF )
           || endif_cntr) && tokenizer_token() != TOKENIZER_ENDOFINPUT){
        f_nt=0;
        if( tokenizer_token() == TOKENIZER_IF) {
          endif_cntr+=1;
        }
        if( tokenizer_token() == TOKENIZER_THEN) {
          tokenizer_next();
          
          if (tokenizer_token() == TOKENIZER_CR) {
            f_nt=1;
          } else { 
            endif_cntr--;
            while(tokenizer_token() != TOKENIZER_ENDIF && tokenizer_token() != TOKENIZER_CR
                 && tokenizer_token() != TOKENIZER_ENDOFINPUT){
              tokenizer_next();
            }
            if (tokenizer_token()==TOKENIZER_ENDIF){
              DEBUG_PRINTF("ubasic.c: else_statement(): endif in singleline if-statement\n");
              ended = 1;
              ubasic_error = UBASIC_E_PARSE;
              return;
            }
          }
        }
        if( tokenizer_token() == TOKENIZER_ENDIF)  { endif_cntr--; }
        if (!f_nt) { tokenizer_next(); }
      }
    }
    endif_statement();
  }

  else{
    DEBUG_PRINTF("ubasic.c: else_statement(): CR after ELSE expected\n");
    ended = 1;
    ubasic_error = UBASIC_E_PARSE;
  }
}
//---------------------------------------------------------------------------
static void
endif_statement(void)
{
  if(if_stack_ptr > 0) 
  {
    accept(TOKENIZER_ENDIF);
    accept(TOKENIZER_CR);
    if_stack_ptr--;
  } 
  else 
  {
    DEBUG_PRINTF("ubasic.c: endif_statement(): endif without if-statement\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_IF;
  }
 }
//---------------------------------------------------------------------------
static void
let_statement(void)
{
 
  int var;

  var = tokenizer_variable_num();

  accept(TOKENIZER_VARIABLE);
  accept(TOKENIZER_EQ);
  ubasic_set_variable(var, expr());
  DEBUG_PRINTF("let_statement: assign %d to %d\n", variables[var], var);
  accept_cr();
}
//---------------------------------------------------------------------------
static void
rem_statement(void)
{
  accept(TOKENIZER_REM);
  DEBUG_PRINTF("rem_statement\n");
  accept(TOKENIZER_CR);
}
//---------------------------------------------------------------------------
static void
bc_statement()
{
  accept(TOKENIZER_COMMENT_BLOCK);
  block_comment = !block_comment;
  accept_cr();
}

//---------------------------------------------------------------------------
static void
cls_statement(void)
{
  accept(TOKENIZER_CLS);
  script_console_clear();
  DEBUG_PRINTF("cls_statement\n");
  accept(TOKENIZER_CR);
}
//---------------------------------------------------------------------------
static void
gosub_statement(void)
{
  accept(TOKENIZER_GOSUB);
  if(tokenizer_token() == TOKENIZER_STRING) {
    tokenizer_string(string, sizeof(string));
    do {
    tokenizer_next();
    } while(tokenizer_token() != TOKENIZER_CR);
    accept(TOKENIZER_CR);
    if(gosub_stack_ptr < MAX_GOSUB_STACK_DEPTH) {

      gosub_stack[gosub_stack_ptr] = tokenizer_line_number();
      gosub_stack_ptr++;
      jump_label(string);
    } else {
      DEBUG_PRINTF("gosub_statement: gosub stack exhausted\n");
      ended = 1;
      ubasic_error = UBASIC_E_GOSUB_STACK_EXHAUSTED;
    }
  } else {
    DEBUG_PRINTF("ubasic.c: goto_statement(): no label specified\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNK_LABEL;
  }
}
//---------------------------------------------------------------------------
static void
return_statement(void)
{
  accept(TOKENIZER_RETURN);
  if(gosub_stack_ptr > 0) {
    gosub_stack_ptr--;
    jump_line(gosub_stack[gosub_stack_ptr]);
  } else {
    DEBUG_PRINTF("return_statement: non-matching return\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_RETURN;
  }
}
//---------------------------------------------------------------------------
static void
next_statement(void)
{
  int var, value;
  
  accept(TOKENIZER_NEXT);
  var = tokenizer_variable_num();
  accept(TOKENIZER_VARIABLE);
  if(for_stack_ptr > 0 &&
     var == for_stack[for_stack_ptr - 1].for_variable) {
    value = ubasic_get_variable(var) + for_stack[for_stack_ptr - 1].step;
    ubasic_set_variable(var, value);

    if(((for_stack[for_stack_ptr - 1].step > 0) && (value <= for_stack[for_stack_ptr - 1].to)) ||
       ((for_stack[for_stack_ptr - 1].step < 0) && (value >= for_stack[for_stack_ptr - 1].to)))
        jump_line(for_stack[for_stack_ptr - 1].line_after_for); 
    else {
      for_stack_ptr--;
      accept(TOKENIZER_CR);
    }
  } else {
    DEBUG_PRINTF("next_statement: non-matching next (expected %d, found %d)\n", for_stack[for_stack_ptr - 1].for_variable, var);
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_NEXT;
  }

}
//---------------------------------------------------------------------------
static void
for_statement(void)
{
  int for_variable, to, step;
  
  accept(TOKENIZER_FOR);
  for_variable = tokenizer_variable_num();
  accept(TOKENIZER_VARIABLE);
  accept(TOKENIZER_EQ);
  ubasic_set_variable(for_variable, expr());
  accept(TOKENIZER_TO);
  to = expr();                    
  step = 1;
  if (tokenizer_token() != TOKENIZER_CR) {
	  accept(TOKENIZER_STEP);
	  step = expr();         
  }
  accept(TOKENIZER_CR);

  if(for_stack_ptr < MAX_FOR_STACK_DEPTH) {
    for_stack[for_stack_ptr].line_after_for = tokenizer_line_number();
    for_stack[for_stack_ptr].for_variable = for_variable;
    for_stack[for_stack_ptr].to = to;
    for_stack[for_stack_ptr].step = step;
    DEBUG_PRINTF("for_statement: new for, var %d to %d\n",
		 for_stack[for_stack_ptr].for_variable,
		 for_stack[for_stack_ptr].to);
		 
    for_stack_ptr++;
  } else {
    DEBUG_PRINTF("for_statement: for stack depth exceeded\n");
    ended = 1;
    ubasic_error = UBASIC_E_FOR_STACK_EXHAUSTED;
 }
}
//---------------------------------------------------------------------------
static void
do_statement(void)
{
  accept(TOKENIZER_DO);
  accept(TOKENIZER_CR);
  if(do_stack_ptr < MAX_DO_STACK_DEPTH) {
     do_stack[do_stack_ptr] = tokenizer_line_number();
     do_stack_ptr++;
  } else {
    DEBUG_PRINTF("do_statement: do stack depth exceeded\n");
    ended = 1;
    ubasic_error = UBASIC_E_DO_STACK_EXHAUSTED;
  }
}
//---------------------------------------------------------------------------
static void
until_statement(void)
{
  int r;
  
  accept(TOKENIZER_UNTIL);
  r = relation();
  if(do_stack_ptr > 0) {
    if(!r) {
      jump_line(do_stack[do_stack_ptr-1]);
    } else {
      do_stack_ptr--;
  	  accept_cr();
    }
  } else {
    DEBUG_PRINTF("until_statement: unmatched until\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_UNTIL;
  }
}
//---------------------------------------------------------------------------
static void
while_statement(void)
{
  int r, while_cntr;
  
  accept(TOKENIZER_WHILE);
  if(while_stack_ptr < MAX_WHILE_STACK_DEPTH) {
    if ((while_stack_ptr == 0)||((while_stack_ptr > 0) && (while_stack[while_stack_ptr-1] != tokenizer_line_number()))){
      while_stack[while_stack_ptr] = tokenizer_line_number();
      while_stack_ptr++;
    }
  } else {
    DEBUG_PRINTF("while_statement: while stack depth exceeded\n");
    ended = 1;
    ubasic_error = UBASIC_E_WHILE_STACK_EXHAUSTED;
    return;
  }

  r = relation();
  if(while_stack_ptr > 0) {
    if(!r) {
    	while_cntr=0;
      while((tokenizer_token() != TOKENIZER_WEND  || while_cntr ) && 
	      tokenizer_token() != TOKENIZER_ENDOFINPUT){   
	      if (tokenizer_token() == TOKENIZER_WHILE) while_cntr+=1;
	      if (tokenizer_token() == TOKENIZER_WEND) while_cntr-=1;           
	      tokenizer_next();
	    }  
      while_stack_ptr--;
    
      accept(TOKENIZER_WEND);
      accept(TOKENIZER_CR);  
    } else {
  	  accept_cr();        
    }
  } else {
    DEBUG_PRINTF("while_statement: unmatched wend\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_WEND;
  }
}
//---------------------------------------------------------------------------
static void
wend_statement(void)
{
  accept(TOKENIZER_WEND);
  if(while_stack_ptr > 0) {
    jump_line(while_stack[while_stack_ptr-1]);
  } else {
    DEBUG_PRINTF("wend_statement: unmatched wend\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_WEND;
  }
}
//---------------------------------------------------------------------------
static void
end_statement(void)
{
  accept(TOKENIZER_END);
  ended = 1;
}
//---------------------------------------------------------------------------
static void
click_statement(void)
{
  accept(TOKENIZER_CLICK);
  tokenizer_string(string, sizeof(string));
  ubasic_camera_click(string);
  tokenizer_next();
  DEBUG_PRINTF("End of click\n");
  accept_cr();
}
//---------------------------------------------------------------------------
static void
press_statement(void)
{
  accept(TOKENIZER_PRESS);
  tokenizer_string(string, sizeof(string));
  ubasic_camera_press(string);
  tokenizer_next();
  DEBUG_PRINTF("End of press\n");
  accept_cr();
}
//---------------------------------------------------------------------------
static void
release_statement(void)
{
  accept(TOKENIZER_RELEASE);
  tokenizer_string(string, sizeof(string));
  ubasic_camera_release(string);
  tokenizer_next();
  DEBUG_PRINTF("End of release\n");
  accept_cr();
}
//---------------------------------------------------------------------------
static void
sleep_statement(void)
{
  int val;
  accept(TOKENIZER_SLEEP);
  val = expr();
  ubasic_camera_sleep(val);
  DEBUG_PRINTF("End of sleep\n");
  accept_cr();
}
//---------------------------------------------------------------------------
static void sleep_s_statement(void)
{
  int val;
  accept(TOKENIZER_SLEEP_S);
  val = expr();
  ubasic_camera_sleep(val*1000);
  accept_cr();
}

static void sleep_m_statement(void)
{
  int val;
  accept(TOKENIZER_SLEEP_M);
  val = expr();
  ubasic_camera_sleep(val*1000*60);
  accept_cr();
}

//---------------------------------------------------------------------------

static void
shoot_statement(void)
{
  accept(TOKENIZER_SHOOT);
  ubasic_camera_shoot();
  DEBUG_PRINTF("End of shoot\n");
  accept_cr();
}

//---------------------------------------------------------------------------

static void set_av96_direct_statement()
{
    int to;
    accept(TOKENIZER_SET_AV96_DIRECT);
    to = expr();
    shooting_set_av96_direct((short int)to, shooting_in_progress()?SET_NOW:SET_LATER);
    accept_cr();
}

static void set_tv96_direct_statement()
{
    int to;
    accept(TOKENIZER_SET_TV96_DIRECT);
    to = expr();
    shooting_set_tv96_direct((short int)to, shooting_in_progress()?SET_NOW:SET_LATER);
    accept_cr();
}

static void set_tv_statement()
{
    int to;
    accept(TOKENIZER_SET_TV);
    to = expr();
    shooting_set_tv(to);
    accept_cr();
}

static void set_user_tv_by_id_rel_statement()
{
    int to;
    accept(TOKENIZER_SET_USER_TV_BY_ID_REL);
    to = expr();
    shooting_set_user_tv_by_id_rel(to);
    accept_cr();
}

 static void set_focus_statement()
{
      accept(TOKENIZER_SET_FOCUS);
      int sd = expr();
      if(!shooting_can_focus())
       {
        if(!enable_focus_override())
        return;
       }
       if(sd<CAMERA_MIN_DIST)sd=CAMERA_MIN_DIST;
       if(sd>(CAMERA_MAX_DIST-fpd))sd=CAMERA_MAX_DIST-fpd;
        lens_set_focus_pos(sd+fpd);
       gCurrentFocus=sd;
      accept_cr();
}
 
 
static void set_led_statement()
{
    int to, to1, to2;
    accept(TOKENIZER_SET_LED);
    to = expr();
    to1 = expr();
	to2 = 200;
	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
		to2 = expr();
    }
	ubasic_set_led(to, to1, to2);
    accept_cr();
}

static void set_av_statement()
{
    int to;
    accept(TOKENIZER_SET_AV);
    to = expr();
    shooting_set_av(to);
    accept_cr();
}

static void set_zoom_fl_statement()  
{
 int i,efl,to;
 accept(TOKENIZER_SET_ZOOM_FL);
 to = expr()*1000;
 for(i=0;i<zoom_points;i++)         
 {
  efl=get_effective_focal_length(i); //effective focal length at this zoom step
  if(efl>=to)
   break;
  else if(i!=(zoom_points-1))i++;
 }
 shooting_set_zoom(i);
 set_prev_zoom(i);		
 accept_cr();
}

static void set_zoom_rfl_statement()
{
 int to;
 accept(TOKENIZER_SET_ZOOM_RFL);
 to = expr();
 to = get_step_for_real_fl(to);
 if(to>=0)
  {
   shooting_set_zoom(to);
   set_prev_zoom(to);		
  }
 accept_cr();
}

static void set_zoom_statement()
{
    int to,gfp;
    accept(TOKENIZER_SET_ZOOM);
    to = expr();
    gfp = get_focal_length(to);
 if(displayedMode==6)		
 {
  if((conf.camera_orientation==1)||(conf.camera_orientation==3))
  conf.near_for_infinity = ((conf.stereo_spacing*gfp*conf.stereo_deviation* 4)/(get_sensor_width()*3));
 else
  conf.near_for_infinity = ((conf.stereo_spacing*gfp*conf.stereo_deviation)/get_sensor_width());
 long fact2;
 long fact1 = (conf.stereo_spacing*gfp)/1000;
 if((conf.camera_orientation==1)||(conf.camera_orientation==3))
  fact2 = ((get_sensor_width()*conf.rangefinder_near*3)/(4*conf.stereo_deviation))/1000;
 else
  fact2 = ((get_sensor_width()*conf.rangefinder_near)/conf.stereo_deviation)/1000;
 int far = (int)((fact1*conf.rangefinder_near)/(fact1-fact2)); 
 if((far>MAX_DIST) ||(far <0))far = MAX_DIST;
 conf.far_for_near =  far;   
  }      
  #if !defined(CAM_SZ) 
    shooting_set_zoom(to);
#else
   ubasic_safe_set_zoom(to);
#endif
 set_prev_zoom(to);		
    accept_cr();
}

static void get_zoom_statement()
{
    int var;
    accept(TOKENIZER_GET_ZOOM);
    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    ubasic_set_variable(var, shooting_get_zoom());
    accept_cr();
}

static void get_focus_statement()
{
    int var;
    accept(TOKENIZER_GET_FOCUS);
    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    ubasic_set_variable(var,lens_get_focus_pos()-fpd);
    accept_cr();
}
 
static void set_zoom_rel_statement()
{
    int to;
    accept(TOKENIZER_SET_ZOOM_REL);
    to = expr();
    shooting_set_zoom_rel(to);
    accept_cr();
}
 
static void set_prop_statement()
{
    int to, to1;
    accept(TOKENIZER_SET_PROP);
    to = expr();
    to1 = expr();
	shooting_set_prop(to, to1);
    accept_cr();
}

static void set_iso_statement()
{
    int to;
    accept(TOKENIZER_SET_ISO);
    to = expr();
    shooting_set_iso(to);
    accept_cr();
}

static void set_raw_statement()
{
    int to;
    accept(TOKENIZER_SET_RAW);
    to = expr();
    ubasic_camera_set_raw(to);
    accept_cr();
}

static void set_nd_filter_statement() 
{ 
    int to; 
    accept(TOKENIZER_SET_ND_FILTER); 
    to = expr(); 
    shooting_set_nd_filter_state(to, shooting_in_progress()?SET_NOW:SET_LATER); 
    accept_cr(); 
} 

static void ndfi_statement()                      
{ 
    accept(TOKENIZER_NDFI);
    shooting_set_nd_filter_state(1, shooting_in_progress()?SET_NOW:SET_LATER); 
    accept_cr(); 
} 

static void ndfo_statement()                     
{ 
    accept(TOKENIZER_NDFO); 
    shooting_set_nd_filter_state(2, shooting_in_progress()?SET_NOW:SET_LATER); 
    accept_cr(); 
} 

static void ndff_statement()                    
{ 
    accept(TOKENIZER_NDFF); 
    shooting_set_nd_filter_state(0, shooting_in_progress()?SET_NOW:SET_LATER); 
    accept_cr(); 
}
 
static void set_autostart_statement()
{
    int to;
    accept(TOKENIZER_SET_SCRIPT_AUTOSTART);
    to = expr();
    ubasic_camera_set_script_autostart(to);
    accept_cr();
}
static void exit_alt_statement()
{
    accept(TOKENIZER_EXIT_ALT);
    gExAlt=1;
    accept_cr();
}
 
static void wait_click_statement()
{
    int timeout=0;
    accept(TOKENIZER_WAIT_CLICK);
    if (tokenizer_token() != TOKENIZER_CR &&
        tokenizer_token() != TOKENIZER_ELSE ) {
        timeout = expr();
    }
    ubasic_camera_wait_click(timeout);
    accept_cr();
}

static void is_key_statement(void)
{
    int var;
    accept(TOKENIZER_IS_KEY);
    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    tokenizer_string(string, sizeof(string));
    tokenizer_next();
    ubasic_set_variable(var, ubasic_camera_is_clicked(string));
    DEBUG_PRINTF("End of is_key\n");
    accept_cr();
}

static void wheel_left_statement(void){
  accept(TOKENIZER_WHEEL_LEFT);
  JogDial_CCW();
  accept_cr();
}

static void wheel_right_statement(void){
  accept(TOKENIZER_WHEEL_RIGHT);
  JogDial_CW();
  accept_cr();
}

static void sdm_line_statement()
{
 accept (TOKENIZER_LINE); 
   int var =expr();
   gConsoleLine=var;
   if(tokenizer_token() == TOKENIZER_COMMA)tokenizer_next();
  if(!gConsoleScroll) gLineCommand=1;
 print_statement();
}
 

static void scroll_statement()
{
 accept(TOKENIZER_SCROLL);
 gConsoleScroll=expr();
 accept_cr();
}

static void is_off_statement()
{
 accept(TOKENIZER_IS_OFF);
 #if defined(CAM_HAS_IS)
 if(CAM_PROPSET<4)
  shooting_set_prop(PROPCASE_IS_MODE,3);
 else
  shooting_set_prop(PROPCASE_IS_MODE,4);
 #endif
 accept_cr();
}

static void is_on_statement()
{
 accept(TOKENIZER_IS_ON);
 #if defined(CAM_HAS_IS)
  shooting_set_prop(PROPCASE_IS_MODE,0);
 #endif
 accept_cr();
}

static void line_one_statement()
{
 accept (TOKENIZER_LINE_ONE);
 if(!gConsoleScroll)gConsoleLine=1;
 gLineCommand=1;
 print_statement();
}

static void line_two_statement()
{
 accept (TOKENIZER_LINE_TWO);
 if(!gConsoleScroll)gConsoleLine=2;
 gLineCommand=1;
 print_statement();
}

static void line_three_statement()
{
 accept (TOKENIZER_LINE_THREE);
 if(!gConsoleScroll)gConsoleLine=3;
 gLineCommand=1;
 print_statement();
}

static void log_statement()
{
 accept (TOKENIZER_LOG);
 gConsoleLine=99;
 print_statement();
}

static void get_luminance_statement()
{
 int ret_var_num,var;
 accept (TOKENIZER_GET_LUMINANCE);
 get_luminance=1;
 accept_cr();
 md_init_motion_detector(15,15,1,2000,3000,255,1,0,1,8,8,8,8,0,1,10);
}

static void sws_statement()
{
 int var,ret_var_num;
 accept(TOKENIZER_SWS);
 var =expr();
 if(var<1)var=1;
 tokenizer_next();
 ret_var_num = tokenizer_variable_num();
 accept_cr();
 md_init_motion_detector(1,1,1,10000,350,var,1,ret_var_num,0,0,0,0,0,16,30,800);
}

static void swnm_statement()
{
 int comp_interval,ret_var_num,sensitivity,timeout;
 accept(TOKENIZER_SWNM);
 comp_interval =expr();
 if(comp_interval<1)comp_interval=1;
 comp_interval*= 500;                     
 tokenizer_next();                        
 ret_var_num = tokenizer_variable_num();  
 sensitivity=128;
 timeout=10000;

 
 accept(TOKENIZER_VARIABLE);              
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) { } 

 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
  {
   tokenizer_next();
   sensitivity=expr();
   if(sensitivity<1)sensitivity=1;
  }

 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
  {
   tokenizer_next();
   timeout=expr();
   if(timeout<comp_interval+1) timeout=comp_interval+1;
   timeout*=1000;                           
  }

  accept_cr();
  md_init_motion_detector(1,1,1,timeout,comp_interval,sensitivity,1,ret_var_num,0,0,0,0,0,16,30,800);
}

static void sspd_statement()                      
{
 int var1,var2;
 short var3;
 gSpecial_token=1;
 var2=0;
 accept(TOKENIZER_SSPD);                           
 var1=expr();                                      
 if ((tokenizer_token() == TOKENIZER_COMMA)||(tokenizer_token() == TOKENIZER_SLASH)) 
 {
    tokenizer_next();                              
    var2 = expr(); 
 }
  if(var2==0)var3=(short)(96.0*log(1.0/var1)/log_2); 
  else var3=(short)(96.0*log(var2)/log_2);           
  if(var3<-576)var3=-576;                            
  if(var3>1440) var3=1440;                           
  shooting_set_tv96_direct(var3,SET_LATER);
  gSpecial_token=0;
  accept_cr();
}

static void su_statement()               
{
 int hr,min;
 gSpecial_token=1;
 accept(TOKENIZER_SLEEP_UNTIL);
 hr = expr();
 if((tokenizer_token() == TOKENIZER_COMMA)||(tokenizer_token() == TOKENIZER_COLON))
  {
   tokenizer_next();
   min = expr();
  }
 else min = 0;
 if(hr<0)hr=0;
 if(hr>23) hr=23;
 if(min<0)min=0;
 if(min>59)min=59;
 st_time.hr=hr;
 st_time.min=min;
 gSpecial_token=0;
 accept_cr();
 ubasic_su(hr,min);
}

static void ft_statement()                 
{
 int hr,min;
 gSpecial_token=1;
 accept(TOKENIZER_FT);
 hr = expr();
 if((tokenizer_token() == TOKENIZER_COMMA)||(tokenizer_token() == TOKENIZER_COLON))
  {
   tokenizer_next();
   min = expr();
  }
 else min = 0;
 if(hr<1)hr=1;     
 if(hr>23) hr=23;
 if(min<0)min=0;
 if(min>59)min=59;
 ft_time.hr=hr;
 ft_time.min=min;
 gSpecial_token=0;
 accept_cr();
}

static void ap_statement()                      
{
 int var1,var2;
 double var3;
 gSpecial_token=1;
 var2=0;
 accept(TOKENIZER_AP);                              
 var1=expr();                                      
 if ((tokenizer_token() == TOKENIZER_COMMA)||(tokenizer_token() == TOKENIZER_PERIOD)) 
 {
    tokenizer_next();                              
    var2 = expr(); 
 }
#if CAM_HAS_IRIS_DIAPHRAGM 
if ((mode_get()&MODE_MASK) != MODE_PLAY)
{
 if(var2>0)var3=(double)(var1+var2/10.0);
 else var3=var1;
 var3= (short)(96.0*(2*log(var3)/log_2));
 shooting_set_av96_direct(var3,SET_LATER);
 gAp96=var3;
 }
#endif
 gSpecial_token=0;
 accept_cr();
}

static void change_ev_statement()
{
 accept(TOKENIZER_CEV);
  int to = expr();
  if(to<-6)to=-6;
  if(to>6)to=6;
  to*=32;
  shooting_set_prop(PROPCASE_EV_CORRECTION_1, to);
  shooting_set_prop(PROPCASE_EV_CORRECTION_2, to);
 
  accept_cr();
}

static void md_get_cell_diff_statement()
{
    int var, col, row;
    accept(TOKENIZER_MD_GET_CELL_DIFF);

		col=expr();tokenizer_next();

		row=expr();tokenizer_next();

    var = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);

    ubasic_set_variable(var, md_get_cell_diff(col,row));
    accept_cr();
}

static void md_detect_motion_statement()
{

 int columns;
 int rows;
 int pixel_measure_mode;
 int detection_timeout;
 int measure_interval;
 int threshold;
 int draw_grid=0;
 int clipping_region_mode=0;
 int clipping_region_row1=0;
 int clipping_region_column1=0;
 int clipping_region_row2=0;
 int clipping_region_column2=0;
 int ret_var_num;
 int parameters=0;
 int pixels_step=1;
 int msecs_before_trigger=0;

    accept(TOKENIZER_MD_DETECT_MOTION);

		columns=expr();tokenizer_next();

		rows=expr();tokenizer_next();

		pixel_measure_mode=expr();tokenizer_next();

		detection_timeout=expr();tokenizer_next();

		measure_interval=expr();tokenizer_next();

		threshold=expr();tokenizer_next();

		draw_grid=expr();tokenizer_next();

    ret_var_num = tokenizer_variable_num();

    accept(TOKENIZER_VARIABLE);

    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) {
			//eat COMMA
		}
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) {
				tokenizer_next();
        clipping_region_mode = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        clipping_region_column1 = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        clipping_region_row1 = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        clipping_region_column2 = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        clipping_region_row2 = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        parameters = expr();
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        pixels_step = expr();
    }

    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) {
				tokenizer_next();
        msecs_before_trigger = expr();
    }
		
    accept_cr();

	md_init_motion_detector(
			columns, rows, pixel_measure_mode, detection_timeout, 
			measure_interval, threshold, draw_grid, ret_var_num,
			clipping_region_mode,
			clipping_region_column1, clipping_region_row1,
			clipping_region_column2, clipping_region_row2,
			parameters, pixels_step, msecs_before_trigger
	);
}

static void so_statement()               
{
 accept(TOKENIZER_SYNC_ON);
 conf.synch_enable=1;
 accept_cr();
}

static void sf_statement()               
{
 accept(TOKENIZER_SYNC_OFF);
 conf.synch_enable=0;
 accept_cr();
}

static void sd_statement()              
{
 int var;
 sd_repeat=0;                           
 accept(TOKENIZER_SD);                 
 var=expr();                           
 if(var<-128)var=-128;
 if(var>255)var=255;
 if(var<0)var+=256;
 pcode = (signed char)var;
 tokenizer_next();                    
 var=expr();                          
 if(var<-128)var=-128;
 if(var>255)var=255;
 if(var<0)var+=256;
 lobyte = (signed char)var;

 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
  {
   tokenizer_next();                     
   var=expr();
   if(var<-128)var=-128;
   if(var>255)var=255;
   if(var<0)var+=256;
   hibyte = (signed char)var;
   sd_repeat=1;                          
  }
                                
 accept_cr();                              
   kbd_key_press(KEY_SHOOT_HALF); 
   sertx=1; 
   block_script=1;                       
}

static void ub_statement()                 
{
 accept(TOKENIZER_UB);                                   
 accept_cr();
 kbd_key_press(KEY_SHOOT_HALF); 
 ub=1;  
 block_script=1;                          
}

static void up_statement()                 
{
 accept(TOKENIZER_UP);
 up=1;                                     
 accept_cr();
 ubasic_camera_click("shoot_half");        
}

static void blue_off_statement()
{
    accept(TOKENIZER_BLUE_OFF);
    debug_led(0);
   DEBUG_PRINTF("End of Blue off\n");
    accept_cr();
}

static void blue_on_statement()
{
    accept(TOKENIZER_BLUE_ON);
    debug_led(1);
   DEBUG_PRINTF("End of Blue on\n");
    accept_cr();
}

static void afloff_statement()
{
    accept(TOKENIZER_AFLOFF);
    ubasic_set_led(9,0,0);
    accept_cr();
}

static void aflo_statement()
{
    accept(TOKENIZER_AFLO);
    ubasic_set_led(9,1,100);
    accept_cr();
}

static void aflf_statement()
{
    int duration;
    accept(TOKENIZER_AFLF);
    duration = expr();
    if(duration<1)duration=1;
    ubasic_baflf(duration);
    accept_cr();
}

static void af_statement()
{
    accept(TOKENIZER_AF);
    ubasic_af();
    accept_cr();
}

	static void play_sound_statement() 
 	{ 
 	 int to; 
 	 accept(TOKENIZER_PLAY_SOUND); 
 	 to = expr(); 
 	 play_sound(to); 
 	 accept_cr(); 
 	} 

	static void beep_statement() 
 	{ 
 	 accept(TOKENIZER_BEEP);  
 	 play_sound(4); 
 	 accept_cr(); 
 	} 
 
 static void set_resolution_statement() 
 	{ 
 	 int to; 
 	 accept(TOKENIZER_SET_RESOLUTION); 
 	 to = expr(); 
 	 shooting_set_prop(PROPCASE_RESOLUTION, to); 
 	 accept_cr(); 
 	} 
 		 
 	static void set_quality_statement() 
 	{ 
 	 int to; 
 	 accept(TOKENIZER_SET_QUALITY); 
 	 to = expr(); 
 	 shooting_set_prop(PROPCASE_QUALITY, to); 
 	 accept_cr(); 
 	} 

 	static void sdmcn_statement() 
 	{ 
 	 accept(TOKENIZER_SDMCN);
       conf.sdm_console=1;
 	 accept_cr(); 
 	} 

 	static void sdmcf_statement() 
 	{ 
 	 accept(TOKENIZER_SDMCF);
       conf.sdm_console=0;
 	 accept_cr(); 
 	} 

 	static void sdmcl_statement() 
 	{ 
 	 int to; 
 	 accept(TOKENIZER_SDMCL); 
 	 to = expr(); 
       if(to<1)to = 1;
       if(to>15)to=15;
       conf.num_lines=to;
 	 accept_cr(); 
 	} 

 	static void sdmcll_statement() 
 	{ 
 	 int to; 
 	 accept(TOKENIZER_SDMCLL); 
 	 to = expr();
       if(to<1)to=1;
       if(to>45)to=45; 
       conf.line_length=to;
 	 accept_cr(); 
 	} 
 
 	static void set_tlpf_statement(void)
 	{
 	  accept(TOKENIZER_TLPF);
        bls=0;
	  TurnOffDisplay();
 	  accept_cr();
 	}

 	static void set_tlpo_statement(void)
 	{
 	  accept(TOKENIZER_TLPO);
        bls=1;
	  TurnOnDisplay();
        gui_set_need_restore();
 	  accept_cr();
 	}
 	static void set_aflock_statement(void)
 	{
 	  int val;
 	  accept(TOKENIZER_SET_AFLOCK);
 	  val = expr();
 	  if (val>0)
        {
         afl=1;
         DoAFLock();
        }  
 	  else 
        {
         afl=0;
         UnlockAF();
        }        
 	  accept_cr();
 	}
 
 	static void lock_ae_statement(void)
 	{
 	  accept(TOKENIZER_AE_LOCK);
        ael=1;
        DoAELock();
        accept_cr();
      }
  
  	static void unlock_ae_statement(void)
        {
 	   accept(TOKENIZER_AE_UNLOCK);
         ael=0;
         UnlockAE();
         accept_cr();
        }        
 
 	static void la_statement(void)
 	{
 	  accept(TOKENIZER_LA);
        afl=1;
 	  DoAFLock();  
 	  accept_cr();
 	}
 	static void uf_statement(void)
 	{
 	 accept(TOKENIZER_UF);
       ubasic_ulf();
 	 accept_cr();
 	}

 
    static void shutdown_statement(void)
     {
     accept(TOKENIZER_SHUT_DOWN);
     power_down();
     accept_cr();
     }

     static void set_bracketing_exposures_statement()
     {
 	int to; 
      accept(TOKENIZER_SET_BE);
      to = expr();
      if(to<3 || to>19) to = 3;
      if((to%2) != 1) to-= 1;
      to = ((to+1)/2)+1;
      conf.bracket_type = to; 
      accept_cr();
     }

     static void shoot_movie_statement()
     {
 	int to; 
      accept(TOKENIZER_SHOOT_MOVIE);
      to = expr();
      if(to<10)to=10;
      accept_cr();
      gFramesRecorded=gFrameCount=0;
      ubasic_sm(to);
     }
 
     static void record_statement()
     {
      accept(TOKENIZER_RECORD);
      accept_cr();
      recordMode();
     }

     static void playback_statement()
     {
      accept(TOKENIZER_PLAYBACK);
      accept_cr();
      playMode();
     }

    static void set_focus_step_statement()
     {
 	int to; 
      accept(TOKENIZER_SET_FS);
      to = expr();
      if(to<1 || to>5000) to = 1;
      conf.subj_dist_bracket_value = to; 
      accept_cr();
     }
 
     static void save_stack_statement()
     {
 	int to; 
      accept(TOKENIZER_SS);
      to = expr();
      if(to<0) to =0;
      if(to>1) to =1;
      conf.save_auto = to;  
      accept_cr();
     }

    static void afb_statement()
     {
      accept(TOKENIZER_AFB);
      conf.tv_bracket_value = 0;
      conf.dist_mode = 2;
      conf.bracket_type=3;
      inf_focus = 0;
      if(!shooting_can_focus())shooting_set_mf(); 
      accept_cr();
     }

     static void ssb_statement()
     {
      int to;
      accept(TOKENIZER_SSB);
      to = expr();
      if(to<1) to =1;
      if(to>12) to =12;
      conf.dist_mode = 0;
      conf.tv_bracket_value = to;
      accept_cr();
     }

     static void bl_statement()
     {
      accept(TOKENIZER_BL);
      conf.bracket_type = 0; 
      conf.dist_mode = 0;
   
      accept_cr();
     }

     static void bd_statement()
     {
      accept(TOKENIZER_BD);
      conf.bracket_type = 1; 
      conf.dist_mode = 0;
  
      accept_cr();

     }

     static void ba_statement()
     {
      accept(TOKENIZER_BA);
       conf.bracket_type = 2;
       conf.dist_mode = 0;
  
      accept_cr();
     }

     static void wud_statement()
     {
      accept(TOKENIZER_WUD);
      ubasic_bracketing_done();
      accept_cr();
     }

     static void wfsp_statement()
     {
      int var =0;
      accept(TOKENIZER_WFSP);
      if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
       var = expr();
      ubasic_wfsp(var);
      accept_cr();
     }

     static void tup_statement()
     {
      int to;
      accept(TOKENIZER_TUP);
      to=expr();
      if(to<7)to=7;
      if(to>35)to=35;
      conf.md=to;
      accept_cr();
     }

     static void uafl_statement()
     {
      accept(TOKENIZER_UAFL);
      conf.use_af_led=1;
      accept_cr();
     }

     static void upl_statement()
     {
      accept(TOKENIZER_UPL);
      conf.use_af_led=0;
      accept_cr();
     }
 
     static void save_focus_statement()
     {
      int num;
      accept(TOKENIZER_SAVE_FOCUS);
      conf.fpd=shooting_get_lens_to_focal_plane_width();
      if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)      
       conf.focus_pos = expr();                                                          
      else 
       conf.focus_pos=lens_get_focus_pos_from_lens();                                   
      accept_cr();
     }

     static void save_zoom_statement()
     {
      accept(TOKENIZER_SAVE_ZOOM);
      conf.zoom_point=gZoomPoint;
      accept_cr();
     }

     static void restore_focus_statement()
     {
      accept(TOKENIZER_RESTORE_FOCUS);    
      ubasic_shooting_set_focus(conf.focus_pos+conf.fpd,SET_NOW);                                            
      has_refocused=1;
      if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)      
      {
       int var = tokenizer_variable_num();
       accept(TOKENIZER_VARIABLE);    
       ubasic_set_variable(var,conf.focus_pos);
      }
      accept_cr();
     }

     static void restore_zoom_statement()
     {
      accept(TOKENIZER_RESTORE_ZOOM);
      lens_set_zoom_point(conf.zoom_point);
      accept_cr();
     }
 
     static void af_first_statement()
     {
      accept(TOKENIZER_AF_FIRST);
      af_all=0;
      accept_cr();
     }

     static void af_all_statement()
     {
      accept(TOKENIZER_AF_ALL);
      af_all=1;
      accept_cr();
     }

     static void sm_statement()
     {
      accept(TOKENIZER_SM);
      ubasic_smov();
      accept_cr();
     }

     static void sv_statement()
     {
      accept(TOKENIZER_SV);
      ubasic_ssmov();
      accept_cr();
     }

     static void stpv_statement()
     {
      accept(TOKENIZER_STPV);
      ubasic_ssmov();
      accept_cr();
     }

     static void ishots_statement() 
     {
      accept(TOKENIZER_ISHOTS);
      int var1,var2,var3,var4;
      var1=var2=var4=0;
      var3=5;
  	   if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {      
       var1=expr(); 
       tokenizer_next();       
  	    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
       {
        var2=expr(); 
        tokenizer_next();
        if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
         var3=expr();
       }
      } 
      if(var2==0) 
      {
       var4=1;    
       var2=1;
      }
   if(shooting_get_drive_mode()!=3)
    {
     if(CDM==99)
     {
      int dummy;
#if defined(UI_CS_PROP)
      _PTM_SetCurrentItem(UI_CS_PROP,0);
#endif
     }  
    }
    else
    {
     shooting_set_mode_chdk(1); 
     msleep(1000);
     shooting_set_mode_chdk(2); 
     if(shooting_get_prop(PROPCASE_DRIVE_MODE)==1) 
     ubasic_cont_shoot(0);
    }     
      init_tl(0, var1, 0, var3, var2, var4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);   
      accept_cr();  
   }
  
     static void tl_statement()
     {
      char fn[64];            
      int fd=-1,rcnt;              
	struct stat st;         
	char *buf=NULL;         
      int paramset;           
      register const char *ptr; 
      int defaults[TL_PARAM_NUMS]; 
      int first_delay_m=0;    
      int first_delay_s=10;   
      int shoot_interval_m=0; 
      int shoot_interval_s=5; 
      int num_exposures=3;    
      int endless_mode=0;     
      int shoot_mode=0;       
      int tv_exposures=3;     
      int focus_exposures=3;  
      int bracket_type=0;     
      int exposure_mode=2;    
      int ev_steps=3;         
      int focus_mode=0;       
      int focus_step=50;      
      int focus_ref=500;      
      int blank_screen=0;     
      int shutdown=0;         
      int save_stack=0;       
      int ev96steps=0;        
      int ev96repeats=0;      
      char osdbuf[32];
      accept(TOKENIZER_TL);

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE ) 
       first_delay_m=expr();                                                      

      if (tokenizer_token() == TOKENIZER_CR)                                      
      {
       sprintf(fn,TL_PARAMS_NAME,first_delay_m);                                  
       if(stat(fn,&st)==0)                                                        
         {
          int i=0;                                                                
          fd=open(fn,O_RDONLY, 0777);                                             
          if(fd>=0)                                                               
           {
            buf=umalloc(st.st_size+1);                                            
            if(buf)
            {                          
	       rcnt=read(fd, buf, st.st_size);                                      
	       buf[rcnt] = 0;                                                       
	       close(fd);                                                           
            }

            ptr=buf;                                                              

            while (ptr[0])                                                        
	       {
               while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;                         
              if (ptr[0]=='@')
	           {
               if ((strncmp("@desc", ptr, 5)==0)||(strncmp("@param", ptr, 6)==0)) 
                 {
                  while (ptr[0] && ptr[0]!='\n') ++ptr;                           
                  ++ptr;
                 }
                else if(strncmp("@default",ptr,8)==0)                             
                 {
                  ptr+=8;                                                         
                  while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;                      
                  ptr+=2;                                                         
                  defaults[i]=strtol(ptr, NULL, 0);                               
                  i++;                                                            
                  while (ptr[0] && ptr[0]!='\n') ++ptr;                           
                  ++ptr;
                 }  
                }
               while (ptr[0] && ptr[0]!='\n') ++ptr;  
               if (ptr[0]) ++ptr;            
            } 
               init_tl(defaults[0],defaults[1],defaults[2],defaults[3],defaults[4],defaults[5],defaults[6],defaults[7],defaults[8],defaults[9],defaults[10],defaults[11],defaults[12],defaults[13],defaults[14],defaults[15],defaults[16],defaults[17],defaults[18],defaults[19]);
             } 
           }
           
         else                                                                     
           {
            play_sound(4);
            accept_cr();
           }
           if(buf) ufree(buf);
           accept_cr();
        } 
    else
    {
      tokenizer_next();
	 first_delay_s=expr();
       tokenizer_next();

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 shoot_interval_m=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 shoot_interval_s=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 num_exposures=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 endless_mode=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 shoot_mode=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 tv_exposures=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 focus_exposures=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 bracket_type=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 exposure_mode=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 ev_steps=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 focus_mode=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 focus_step=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 focus_ref=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 blank_screen=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 shutdown=expr();
       tokenizer_next();
      }
     
	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 save_stack=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 ev96steps=expr();
       tokenizer_next();
      }

	if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE )
      {
	 ev96repeats=expr();
      }
    
      accept_cr();

      init_tl(first_delay_m,first_delay_s,shoot_interval_m,shoot_interval_s,num_exposures,endless_mode,shoot_mode,tv_exposures,focus_exposures,bracket_type,exposure_mode,ev_steps,focus_mode,focus_step,focus_ref,blank_screen,shutdown,save_stack,ev96steps,ev96repeats);
 
     } 
   }

static void set_max_spt_statement() 
{
   int s;
   accept(TOKENIZER_SET_MAX_SPT);
   s = expr();
   if (s > 0 && s <= MAX_SPEED) speed = s;
   accept_cr();
}

static void set_av_rel_statement()
{
    int to;
    accept(TOKENIZER_SET_AV_REL);
    to = expr();
    shooting_set_av_rel(to);
    accept_cr();
}

 static void console_redraw_statement(void)
{
 accept(TOKENIZER_CONSOLE_REDRAW);
 console_redraw();
 accept_cr();
}

static void set_fast_param_statement()
{
 accept(TOKENIZER_SET_SHOOT_FAST_PARAM);
 int var1,var2;
 var1=expr();
 var2=expr();
 if((var1<0)||(var1>3))var1=0;
 switch (var1) 
  {
    case 0:
    fast_params.exp_comp=var2;
    gEVfromScript=1;
    break;
    case 1:
    fast_params.pref_ap=var2;
    break;
    case 2:
    fast_params.min_ap=var2;
    break;
    case 3:
    fast_params.ev_step=var2;
    break;
   }
  accept_cr();
}

static void shoot_fast_statement()                  
{
 int var1,var2;
 short var3;
 var2=0;
 gSpecial_token=1;
 fastRetVarNum = -1;
 accept(TOKENIZER_FSON);
 var1=expr();                                      
 if (tokenizer_token() == TOKENIZER_SLASH)	   
 {
  tokenizer_next();                                
  var2 = expr();                                   
 }
  if(var2==0)var3=(short)(96.0*log(1.0/var1)/log_2); 
  else var3=(short)(96.0*log(var2)/log_2);           
  if(var3<-576)var3=-576;                            
  if(var3>1152) var3=1152;                           
  gFastTv=var3;
  avBv=0;                                            
  if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
   {                     
    tokenizer_next();
    fastRetVarNum = tokenizer_variable_num();             
    accept(TOKENIZER_VARIABLE); 
    }
    if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
     {                     
      tokenizer_next();
      var2 = expr();  
      avBv= -1;                                         
     }
  ubasic_fastShoot(0);
  gSpecial_token=0;
  accept_cr();
}

 
static void prepare_for_shooting_fast_statement()               
{
 int var1,var2;
 short var3;
 var2=0;
 gSpecial_token=1;
 accept(TOKENIZER_PFSAT);
 var1=expr(); 
 if (tokenizer_token() == TOKENIZER_SLASH)	   
 {
  tokenizer_next();                                
  var2 = expr();                                   
 }
  if(var2==0)var3=(short)(96.0*log(1.0/var1)/log_2); 
  else var3=(short)(96.0*log(var2)/log_2);           
  if(var3<-576)var3=-576;                            
  if(var3>1440) var3=1440;                           
  gFastTv=var3;
  avBv=0;
  if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE) 
   {                     
    tokenizer_next();
    var2 = expr();  
    avBv= -1;                                         
   }
  ubasic_fastKAP();
  gSpecial_token=0;
  accept_cr();
} 
 
static void fast_iso_statement()
{
 accept(TOKENIZER_FAST_ISO_MAX);
 short  var1=400;
 var1=expr();
 gMaxIso96=shooting_get_sv96_from_iso(shooting_iso_market_to_real(var1));  
 accept_cr();
}

static void cover_lens_statement()
{
 accept(TOKENIZER_COVER_LENS);
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
  checkLensCap = expr();
 ubasic_cover_lens();
 accept_cr();
}

static void mb_statement(void)
{
 accept(TOKENIZER_MB);
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
  gNumOfBvReadings = expr();
 if(gNumOfBvReadings<10)
  gNumOfBvReadings = 10;
 else if(gNumOfBvReadings>100)
  gNumOfBvReadings = 100;
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
  {
   int to = expr();                                                           
   fill_bv_buffer();                                                          
  }
  else                                                                        
  {                                                            
   sortBv();                                                                  
   averageBv(50,80);                                                          
  }
 accept_cr();
}

static void bzdp_statement()
{
 accept(TOKENIZER_BZDP);
 int var =expr();
 if((var>0)&&(var<screen_width))bigZoomXPos=var;
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
 {
  if (tokenizer_token() == TOKENIZER_COMMA) tokenizer_next();
  var =expr();
  if((var>0)&&(var<screen_height))bigZoomYPos=var;
 }
  accept_cr();
}


static void log_bv_statement(void)
{
 accept(TOKENIZER_LOG_BV);
 printBv();
 accept_cr();
}

static void vzoom_in_out_statement(void)
{
 accept(TOKENIZER_VZOOM_IN_OUT);
 ubasic_vzoom_in_out();
 accept_cr();
}

static void ebz_statement(void)
{
 accept(TOKENIZER_ENABLE_BIG_ZOOM);
 gEnableBigZoom=1;
 accept_cr();
}

static void dbz_statement(void)
{
 accept(TOKENIZER_DISABLE_BIG_ZOOM);
 gEnableBigZoom=0;
 bigZoomYPos=0;
 accept_cr();
}

static void sh_statement(void)
{
 accept(TOKENIZER_SH);
 int  var=expr();
 if(var<0)var=0;
 if(var>23)var=23;
 gStartHour = var+100;  
 accept_cr();
}
 
static void eh_statement(void)
{
 accept(TOKENIZER_EH);
 int  var=expr();
 if(var<0)var=0;
 if(var>23)var=23;
 gEndHour = var+100;   
 accept_cr();
}

static void smin_statement(void)
{
 accept(TOKENIZER_SMIN);
 int  var=expr();
 if(var<0)var=0;
 if(var>59)var=59;
 gStartMinutes = var+100;  
 accept_cr();
}
 
static void em_statement(void)
{
 accept(TOKENIZER_EM);
 int  var=expr();
 if(var<0)var=0;
 if(var>59)var=59;
 gEndMinutes = var+100;  
 accept_cr();
}
    
static void flash_off_statement(void)
{
 accept(TOKENIZER_FLASH_OFF);
 gFlashDisable = 1;
 shooting_set_prop(PROPCASE_FLASH_MODE,2);
 #if defined(PROPCASE_AF_ASSIST_BEAM)
  shooting_set_prop(PROPCASE_AF_ASSIST_BEAM,0);
 #endif
 accept_cr();
}

static void flash_on_statement(void)
{
 accept(TOKENIZER_FLASH_ON);
 gFlashDisable = 0;
 shooting_set_prop(PROPCASE_FLASH_MODE,1);
 accept_cr();
}

static void start_clock_statement(void)
{
 accept(TOKENIZER_START_CLOCK);
 gStartTime = shooting_get_day_seconds();
 elapsedAccumulated=0;
 if (tokenizer_token() != TOKENIZER_CR && tokenizer_token() != TOKENIZER_ELSE)
  {
   int to = expr();                                                           
   elapsedMode=1;                                                             
  }
 else elapsedMode=0;                                                          
 accept_cr();
}

static void disable_video_out_statement(void)
{
 accept(TOKENIZER_DISABLE_VIDEO_OUT);
 gVideoDisabled = 1;
 accept_cr();
}

static void enable_video_out_statement(void)
{
 accept(TOKENIZER_ENABLE_VIDEO_OUT);
 gVideoDisabled = 0;
 accept_cr();
}

static void capture_mode_statement()
{
 accept(TOKENIZER_CAPTURE_MODE);
 int  var=expr();
 shooting_set_mode_chdk(var);
 accept_cr();
}

static void remove_statement(void)
{
 accept(TOKENIZER_REMOVE);
 if(tokenizer_token() == TOKENIZER_STRING) 
  {
   tokenizer_string(string, sizeof(string));
   remove(string);
  }
 accept_cr();
}

static void get_config_value_statement()
{
 int var, var1, var2;
 tConfigVal configVal;
 accept(TOKENIZER_GET_CONFIG_VALUE);
 var = expr();
 var1 = expr();
 var2 = tokenizer_variable_num();
 accept(TOKENIZER_VARIABLE);
 if( conf_getValue(var, &configVal) == CONF_VALUE ) 
  {
   ubasic_set_variable(var2, configVal.numb);
  } 
 else 
  {
   ubasic_set_variable(var2, var1);
  }
 accept_cr();
}

static void set_config_value_statement()
{
 int id, value;
 tConfigVal configVal = {0,0,0,0};
    
 accept(TOKENIZER_SET_CONFIG_VALUE);
 id = expr();
 value = expr();
 if( conf_getValue(id, &configVal) == CONF_DEF_VALUE ) 
  {
   configVal.numb = value;
   configVal.isNumb = 1;
   conf_setValue(id, configVal);
  }
 accept_cr();
}

static void set_sv96_statement()
{
 int to;
 accept(TOKENIZER_SET_SV96);
 to = expr();
 shooting_set_sv96((short int)to, shooting_in_progress()?SET_NOW:SET_LATER);
 accept_cr();
}

 //----------------------------------------------------------------------
 
static void mf_off_statement()
{
 accept(TOKENIZER_MF_OFF);
 if(shooting_get_prop(PROPCASE_FOCUS_MODE)==1)UnlockMF();
 accept_cr();
}

static void mf_on_statement()
{
 accept(TOKENIZER_MF_ON);
 if(shooting_get_prop(PROPCASE_FOCUS_MODE)!=1)DoMFLock();
 accept_cr();
}

 //----------------------------------------------------------------------
 
 static void aeb_on_statement()
{
 static unsigned char aeb_state = 0;
 accept(TOKENIZER_AEB_ON);
  if(CDM==99)
  {
#if defined(UI_AEB_PROP)
      int dialMode;
      get_property_case(PROPCASE_SHOOTING_MODE, &dialMode, 4);
      dialMode=shooting_mode_canon2chdk(dialMode);
      if((dialMode==MODE_P)||(dialMode==MODE_TV)||(dialMode==MODE_AV))
       {       
         _PTM_SetCurrentItem(UI_AEB_PROP,1);
       }
      else play_sound(6);
#endif
  }
 accept_cr();
}

 static void aeb_off_statement()
{
 static unsigned char aeb_state = 0;
 accept(TOKENIZER_AEB_OFF);
  if(CDM==99)
  {
#if defined(UI_AEB_PROP)
      int dialMode;
      get_property_case(PROPCASE_SHOOTING_MODE, &dialMode, 4);
      dialMode=shooting_mode_canon2chdk(dialMode);
      if((dialMode==MODE_P)||(dialMode==MODE_TV)||(dialMode==MODE_AV))
       {  
         _PTM_SetCurrentItem(UI_AEB_PROP,0);
       }
      else play_sound(6);
#endif
  }
 accept_cr();
}

static void cs()
{
  if(shooting_get_drive_mode()!=3)
 {
  if(CDM==99)
  {
#if defined(UI_CS_PROP)
      int dialMode;
      get_property_case(PROPCASE_SHOOTING_MODE, &dialMode, 4);
      dialMode=shooting_mode_canon2chdk(dialMode);
      if(dialMode==MODE_AUTO)shooting_set_mode_chdk(MODE_P);     
#if !defined(CAMERA_ixus160_elph_160) && !defined(CAMERA_g7x)
      _PTM_SetCurrentItem(UI_CS_PROP,1);
#else
      _PTM_SetCurrentItem(UI_CS_PROP,2);
#endif
#endif
  }
  else
  {
   shooting_set_mode_chdk(1); 
   msleep(1000);
   shooting_set_mode_chdk(MODE_P); 
   if(shooting_get_prop(PROPCASE_DRIVE_MODE)!=1) 
    ubasic_cont_shoot(1); 
  }
 }
}
 
static void cs_statement()
{
 accept(TOKENIZER_CS);
 cs();
 accept_cr();
}

static void cs_off_statement()
{
 accept(TOKENIZER_CS_OFF);
 if(shooting_get_drive_mode()!=3)
 {
  if(CDM==99)
  {
  int dummy;
#if defined(UI_CS_PROP)
  _PTM_SetCurrentItem(UI_CS_PROP,0);
#endif
  }  
  
  else
  {
   shooting_set_mode_chdk(1); 
   msleep(1000);
   shooting_set_mode_chdk(2); 
   if(shooting_get_prop(PROPCASE_DRIVE_MODE)==1) 
   ubasic_cont_shoot(0);
  }
 }
 accept_cr();
}

static void focus_out_statement()
{
 accept(TOKENIZER_FOCUS_OUT);
 shooting_focus_out();
 accept_cr();
}

static void focus_in_statement()
{
 accept(TOKENIZER_FOCUS_IN);
 shooting_focus_in();
 accept_cr();
}

static void focus_at_np_statement()			
{
 accept(TOKENIZER_FOCUS_AT_NP);
 ubasic_shooting_set_focus(gNearPoint, SET_NOW);
 accept_cr();
}
 
static void lg_statement()
{
 int to;
 char buf[20];
 accept(TOKENIZER_LG);
 to = expr();
 sprintf(buf,"A/SDM/GRIDS/%d.grd",to);
 grid_lines_load(buf);
 accept_cr();
}
 
static void set_focus_range_statement()
{
 accept(TOKENIZER_SET_FOCUS_RANGE);
 ubasic_set_focus_range();
 accept_cr();
}

static void change_fps_statement()
{
 int to;
 accept(TOKENIZER_CFPS);
 to = expr();
 fps(to);
 accept_cr();
}
 
static void console_position_statement(void)
{
  accept(TOKENIZER_CONSOLE_POSITION);
  consoleOrigin.x = expr();
  consoleOrigin.y = expr();
  accept_cr();  
}

static void shdrtv96_statement()
{
 int to;
 accept(TOKENIZER_SET_HDR_TV96);
 to = expr();
 if(to<-576)to=-576;
 if(to>1056)to=1056;
 shooting_set_prop(PROPCASE_CAMERA_TV,to);
 #if (CAM_PROPSET>3)
 if(CAM_PROPSET>3) shooting_set_prop(PROPCASE_TV2,to); 
 #endif
 accept_cr();
}
 
static void burstseq_statement()
{
  accept(TOKENIZER_BURSTSEQ);
  conf.burst_frame_count = expr();
  ubasic_burst();
  accept_cr();  
}

static void sdmbm_statement()
{
 accept(TOKENIZER_SDMBM);
 conf.show_grid_lines=0;
 conf.remote_zoom_enable=0;
 conf.show_osd=1;
 conf.save_xml_file=0;
 conf.save_raw=0;
 conf.blank_jpg=0;
 conf.save_auto=0;
 conf.tv_bracket_value=0;
 conf.dist_mode=0;
 conf.compact=0;
 conf.bracket_type=3;
 conf.raw_strip_mode=0;
 conf.bracket_intershot_delay=0;
 conf.fastlapse=0;
 conf.FastLapseDelay=3;
 conf.lockout_time=2;
 conf.burst_frame_count=10;
 conf.raw_nr=0;
 cs();
 gFlashDisable = 1;
 #if defined(CAM_HAS_IS)
 if(CAM_PROPSET<4)
  shooting_set_prop(PROPCASE_IS_MODE,3);
 else
  shooting_set_prop(PROPCASE_IS_MODE,4);
 #endif
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr();  
}

static void sdm2dm_statement()
{
 accept(TOKENIZER_SDM2D);
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0;
 conf.show_grid_lines=0;
 conf.remote_zoom_enable=0;
 conf.show_osd=1;
 conf.dist_mode=1;      
 conf.save_xml_file=1;
 conf.compact=0;
 conf.lockout_time=2;
 cs_off_statement();
 gFlashDisable = 1;
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr(); 
}
 
static void sdm3dm_statement()
{
 accept(TOKENIZER_SDM3D);
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0;
 conf.dc=1;
 conf.on_release=1;
 conf.remote_enable=1;
 conf.synch_enable=1;
 conf.remote_zoom_enable=1;
 conf.enable_yaw_guideline=0;
 conf.save_xml_file=1;
 conf.show_osd=1;
 conf.compact=0;
 conf.lockout_time=2;
 grid_lines_load("A/SDM/GRIDS/1.grd");
 conf.show_grid_lines=1;
 cs_off_statement();
 gFlashDisable = 1;
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr();  
}

static void sdmtm_statement()
{
 accept(TOKENIZER_SDMTM);
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0;
 conf.dc=1;
 conf.on_release=0;
 conf.remote_enable=1;
 conf.synch_enable=1;
 conf.remote_zoom_enable=1;
 conf.enable_yaw_guideline=0;
 conf.save_xml_file=1;
 conf.show_osd=1;
 conf.compact=0;
 conf.lockout_time=2;
 grid_lines_load("A/SDM/GRIDS/1.grd");
 conf.show_grid_lines=1;
 cs_off_statement();
 gFlashDisable = 1;
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr();  
}

static void sdm3dsports_statement()
{
 accept(TOKENIZER_SDM3DSPORTS);
 int v = expr();
 if(v<2)v=2;
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0; 
 conf.burst_frame_count=v;
 conf.dc=1;
 conf.on_release=1;
 conf.dist_mode=0;
 conf.remote_enable=1;
 conf.synch_enable=0; 
 conf.remote_zoom_enable=1;
 conf.enable_yaw_guideline=0;
 conf.save_xml_file=0;
 conf.raw_strip_mode=0;
 conf.save_raw=0;
 conf.show_osd=1;
 conf.compact=0;
 conf.tv_bracket_value=0;
 conf.lockout_time=2;
 conf.show_grid_lines=0;
 conf.fastlapse=1;
 conf.bracket_type=2;  
 conf.FastLapseDelay=6;
 cs();
 gFlashDisable = 1;
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr();  
}

void sdmuserhdr()
{
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0; 
 conf.show_grid_lines=0;
 conf.show_osd=1;
 conf.remote_zoom_enable=0;
 conf.save_xml_file=0;
 conf.save_auto=0;
 conf.raw_strip_mode=0;
 conf.dist_mode=0;
 conf.subj_dist_bracket_value=2;
 conf.tv_override_enable=1;
 conf.bracket_type=3;
 conf.fastlapse=0;
 conf.FastLapseDelay=0;
 conf.save_raw=0;
 conf.compact=0;
 conf.blank_jpg=0;
 conf.bracket_intershot_delay=0; 
 conf.raw_nr=0;
 conf.tv_bracket_value=6;
 
 cs();
 gFlashDisable = 1;
 #if defined(CAM_HAS_IS)
 if(CAM_PROPSET<4)
  shooting_set_prop(PROPCASE_IS_MODE,3);
 else
  shooting_set_prop(PROPCASE_IS_MODE,4);
 #endif
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
}
static void sdmuserhdr_statement()
{
 accept(TOKENIZER_SDMUHDR);
 sdmuserhdr();
 accept_cr(); 
}

static void sdmautohdr_statement()
{
 accept(TOKENIZER_SDMAUTOHDR);
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0; 
 conf.show_grid_lines=0;
 conf.show_osd=1;
 conf.remote_zoom_enable=0;
 conf.save_xml_file=0;
 conf.raw_strip_mode=0;
 conf.save_raw=0;
 conf.dist_mode=0;
 conf.fastlapse=0;
 conf.compact=0;
 conf.blank_jpg=0;
 conf.save_auto=0;
 conf.bracket_type=3;
 conf.tv_bracket_value=6;
 conf.bracket_intershot_delay=0; 
 cs();
 gFlashDisable = 1;
 #if defined(CAM_HAS_USER_TV_MODES)
  shooting_set_mode_chdk(3);         
 #endif
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))UnlockMF();
 ubasic_ulf();
 accept_cr();  
}

static void warning_statement()
{
 accept(TOKENIZER_WARNING);
 play_sound(6);
 accept_cr();  
}

static void sdm_reset_statement()
{
 accept(TOKENIZER_SDM_RESET);
 conf.user_1=conf.user_2=conf.user_3=conf.user_4=0; 
 conf.fastlapse  = 0 ;
 conf.burst_frame_count  =  0;
 conf.tv_bracket_value  = 0 ;
 conf.tv_override_enable  = 0 ;  
 conf.dist_mode  = 0 ;
 conf.synch_enable  = 0 ;  
 conf.show_grid_lines  = 0 ;  
 conf.remote_zoom_enable  = 0 ;
 conf.show_osd  = 1 ;
 conf.save_xml_file  = 0 ;
 conf.save_raw  = 0 ;
 conf.blank_jpg  = 0  ;
 conf.save_auto  = 0 ;
 conf.compact  = 0 ;
 conf.bracket_intershot_delay  = 0 ;
 accept_cr();  
}  

static void af_assist_statement()
{
 int to;
 accept(TOKENIZER_SET_AF_ASSIST);
#if defined(PROPCASE_AF_ASSIST_BEAM)
 to = expr();
 if(!to||(to==1))shooting_set_prop(PROPCASE_AF_ASSIST_BEAM,to);
#endif
 accept_cr();
}   

#if defined(OPT_ALARIK)

static void set_uav_by_id_statement()
{
 int to;
 accept(TOKENIZER_SET_USER_AV_BY_ID);
 to = expr();
 shooting_set_user_av_by_id(to);
 accept_cr();
}

static void set_utv_by_id_statement()
{
 int to;
 accept(TOKENIZER_SET_USER_TV_BY_ID);
 to = expr();
 shooting_set_user_tv_by_id(to);
 accept_cr();
}

static void set_console_layout(void)
{
  int x1,y1,x2,y2;
  accept(TOKENIZER_SET_CONSOLE_LAYOUT);
  x1 = expr();
  y1 = expr();
  x2 = expr();
  y2 = expr();
  console_set_layout(x1,y1,x2,y2);
  accept_cr();  
}

/*---------------------------------------------------------------------------*/
static void set_console_autoredraw(void)
{
  accept(TOKENIZER_SET_CONSOLE_AUTOREDRAW);
  console_set_autoredraw(expr());
  accept_cr();  
}
 
/*---------------------------------------------------------------------------*/
static void dec_select_stack(void)
{
  if(select_stack_ptr > 0) 
  {
      select_stack_ptr--;
  } else 
  {
    DEBUG_PRINTF("select_statement: SELECT-Stack fail\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_END_SELECT;
  }
}
 
/*---------------------------------------------------------------------------*/
static void end_select_statement(void)
{
  if(select_stack_ptr > 0) 
  {
    accept(TOKENIZER_END_SELECT);
    accept(TOKENIZER_CR);
    dec_select_stack();
  } else 
  {
    DEBUG_PRINTF("ubasic.c: end_select_statement(): end_select without select-statement\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_END_SELECT;
  }
}

/*---------------------------------------------------------------------------*/
static void case_statement(void)
{
  int select_value, case_value_1, case_value_2, case_value_eq;
  short case_run, case_goto = 0, case_gosub = 0;
  int cur_ln, gosub_ln = 0;
  
  accept(TOKENIZER_CASE);
  if(select_stack_ptr > 0) {
    select_value = select_stack[select_stack_ptr - 1].select_value;
    case_run = select_stack[select_stack_ptr - 1].case_run;
  
    if (!case_run) {
      case_value_1 = expr();
      case_value_eq = (select_value == case_value_1);
      if (case_value_eq) { DEBUG_PRINTF("case_statement: case_value_eq %d, case_value %d\n", case_value_eq, case_value_1); }  

      if(tokenizer_token() == TOKENIZER_TO) {
        accept(TOKENIZER_TO);
        case_value_2 = expr();
        if (case_value_1 < case_value_2) {
          case_value_eq = ((select_value >= case_value_1) && (select_value <= case_value_2));
          DEBUG_PRINTF("case_statement: case_value %d to %d\n", case_value_1, case_value_2);
        } else {
          case_value_eq = ((select_value >= case_value_2) && (select_value <= case_value_1));
          DEBUG_PRINTF("case_statement: case_value %d to %d\n", case_value_2, case_value_1);
        }
      } else if (tokenizer_token() == TOKENIZER_COMMA) {
        do {
          accept(TOKENIZER_COMMA);
          if (case_value_eq) {
            case_value_2 = expr();
          } else {
            case_value_1 = expr();
            case_value_eq = (select_value == case_value_1);
          }
        } while (tokenizer_token() == TOKENIZER_COMMA);
        DEBUG_PRINTF("case_statement: case_value_eq %d, case_value_comma %d\n", case_value_eq, case_value_1);
      }
      
      accept(TOKENIZER_SEMICOLON);
      if (case_value_eq) {
        case_goto = (tokenizer_token() == TOKENIZER_GOTO);
        case_gosub = (tokenizer_token() == TOKENIZER_GOSUB);
//GOSUB - save curr linenumber
        cur_ln = tokenizer_line_number();
//GOSUB
        statement();
//GOSUB  - save new linenumber, reset to curr linenumber
      if (case_gosub) { 
        gosub_ln = tokenizer_line_number();
        jump_line(cur_ln+1);
        DEBUG_PRINTF("case_statement: GOSUB: toLN=%d, nextLN=%d\n", gosub_ln, cur_ln+1);
      }
//GOSUB
        DEBUG_PRINTF("case_statement: case execute\n");
        case_run = 1;
        select_stack[select_stack_ptr - 1].case_run = case_run;
      } else {
        DEBUG_PRINTF("case_statement: case jump; case_run: %d\n", case_run);
        accept_cr();
      }
    } else {accept_cr();}
//REM
    while ((tokenizer_token() == TOKENIZER_REM) && (!case_goto)) {statement();}
//REM
    if (case_goto) { dec_select_stack(); } else {
      if ((tokenizer_token() != TOKENIZER_CASE) && (tokenizer_token() != TOKENIZER_CASE_ELSE) && 
         (tokenizer_token() != TOKENIZER_END_SELECT)) {
         DEBUG_PRINTF("ubasic.c: select_statement(): don't found case, case_else or end_select\n");
         ended = 1;
         ubasic_error = UBASIC_E_PARSE;
      } else { 
//GOSUB test for end_select and set to gosub-linenumber
        if (tokenizer_token() == TOKENIZER_END_SELECT) { end_select_statement(); }
        if (case_gosub) {
          gosub_stack[gosub_stack_ptr-1] = tokenizer_line_number();
          jump_line(gosub_ln);
          DEBUG_PRINTF("end_select_statement: GOSUB: returnLN=%d\n", gosub_stack[gosub_stack_ptr-1]);
        }
      }  
//GOSUB        
    }
  } else {
    DEBUG_PRINTF("case_statement: SELECT-Stack fail\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_END_SELECT;
  }
}
 
/*---------------------------------------------------------------------------*/
static void case_else_statement(void)
{
  short case_goto = 0, case_gosub = 0;
  int cur_ln, gosub_ln = 0;
  
  accept(TOKENIZER_CASE_ELSE);
  if(select_stack_ptr > 0) {
    if (!select_stack[select_stack_ptr - 1].case_run) {
      case_goto = (tokenizer_token() == TOKENIZER_GOTO); 
      case_gosub = (tokenizer_token() == TOKENIZER_GOSUB); 
//GOSUB - save curr linenumber
      cur_ln = tokenizer_line_number();
//GOSUB
      statement();
//GOSUB  - save new linenumber, reset to curr linenumber
      if (case_gosub) { 
        gosub_ln = tokenizer_line_number();
        jump_line(cur_ln+1);
        DEBUG_PRINTF("case_else_statement: GOSUB: toLN=%d, nextLN=%d\n", gosub_ln, cur_ln+1);
      }
//GOSUB
      DEBUG_PRINTF("case_else_statement: case_else execute\n");
    } else {
      DEBUG_PRINTF("case_else_statement: case_else jump; case_run: %d\n", select_stack[select_stack_ptr - 1].case_run);
      accept_cr();
    }
//REM
    while ((tokenizer_token() == TOKENIZER_REM) && (!case_goto)) {statement();}
//REM
    if (case_goto) { dec_select_stack(); } else { 
//GOSUB test for end_select and set to gosub-linenumber
      if (tokenizer_token() != TOKENIZER_END_SELECT) {
        DEBUG_PRINTF("ubasic.c: select_statement(): don't found end_select\n");
        ended = 1;
        ubasic_error = UBASIC_E_PARSE;
      } else { 
          end_select_statement(); 
        if (case_gosub) {
          gosub_stack[gosub_stack_ptr-1] = tokenizer_line_number();
          jump_line(gosub_ln);
          DEBUG_PRINTF("end_select_statement: GOSUB: returnLN=%d\n", gosub_stack[gosub_stack_ptr-1]);
        }
      }  
//GOSUB      
    }
  } else {
    DEBUG_PRINTF("case_else_statement: SELECT-Stack fault\n");
    ended = 1;
    ubasic_error = UBASIC_E_UNMATCHED_END_SELECT;
  }
}
  
/*---------------------------------------------------------------------------*/
static void select_statement(void)
{
  int select_value;
  
  accept(TOKENIZER_SELECT);
  select_value = expr();  
  accept(TOKENIZER_CR);
//REM
    while (tokenizer_token() == TOKENIZER_REM) {statement();}
//REM
  
  if(select_stack_ptr < MAX_SELECT_STACK_DEPTH) {
    select_stack[select_stack_ptr].select_value = select_value;
    select_stack[select_stack_ptr].case_run = 0;
    DEBUG_PRINTF("select_statement: new select, value %d\n",select_stack[select_stack_ptr].select_value);
    select_stack_ptr++;
    if (tokenizer_token() != TOKENIZER_CASE) {
      DEBUG_PRINTF("ubasic.c: select_statement(): don't found case-statement\n");
      ended = 1;
      ubasic_error = UBASIC_E_PARSE;
    }
    else { case_statement(); }
  } else {
    DEBUG_PRINTF("select_statement: SELECT-stack depth exceeded\n");
    ended = 1;
    ubasic_error = UBASIC_E_SELECT_STACK_EXHAUSTED;
  }
}
/*---------------------------------------------------------------------------*/
#endif

 
static void
statement(void)
{
 char mybuf[32];
  ubasic_token token;
  token = tokenizer_token();
  yield = 0;
 sprintf(mybuf,"Token %d, Ended ? %d\n",token,ended);
if(block_comment && (token != TOKENIZER_COMMENT_BLOCK))accept_cr(); 
else
{
  switch(token) 
 {
  case TOKENIZER_PRINT_SCREEN:
    print_screen_statement();
    break;
  case TOKENIZER_PRNT:
#ifdef OPT_ALARIK
    print_statement();
#else
    prnt_statement();
#endif
    break;

  case TOKENIZER_SLEEP:
    sleep_statement();
    yield = 1;    //speed addition
    break;

  case TOKENIZER_SLEEP_S:
    sleep_s_statement();
    yield = 1;    //speed addition
    break;

  case TOKENIZER_SLEEP_M:
    sleep_m_statement();
    yield = 1;    //speed addition
    break;

  case TOKENIZER_CLICK:
    click_statement();
    yield = 1;    //speed addition
    break;
  case TOKENIZER_PRESS:
    press_statement();
    yield = 1;    //speed addition
    break;
  case TOKENIZER_RELEASE:
    release_statement();
    yield = 1;    //speed addition
    break;
  case TOKENIZER_SHOOT:
    shoot_statement();
    yield = 1;    //speed addition
    break;
 
  case TOKENIZER_SET_AV96_DIRECT:
    set_av96_direct_statement();
    break;  
  case TOKENIZER_SET_TV96_DIRECT:
    set_tv96_direct_statement();
    break;  
  case TOKENIZER_SET_TV:
    set_tv_statement();
    break;
  case TOKENIZER_SET_AV:
    set_av_statement();
    break;
  case TOKENIZER_SET_ND_FILTER: 
    set_nd_filter_statement(); 
    break; 
  case TOKENIZER_NDFI: 
    ndfi_statement(); 
    break; 
  case TOKENIZER_NDFO: 
    ndfo_statement(); 
    break; 
  case TOKENIZER_NDFF: 
    ndff_statement(); 
    break; 
  case TOKENIZER_SET_ZOOM:
    set_zoom_statement();
    break;
  case TOKENIZER_SET_ZOOM_FL:
    set_zoom_fl_statement();
    break;
    
  case TOKENIZER_SET_ZOOM_RFL:
    set_zoom_rfl_statement();
    break;
    
  case TOKENIZER_SET_ZOOM_REL:
    set_zoom_rel_statement();
    break;
 
  case TOKENIZER_SET_FOCUS:
    set_focus_statement();
    break;
 
  case TOKENIZER_SET_PROP:
    set_prop_statement();
    break;
 
 
 case TOKENIZER_SET_ISO:
    set_iso_statement();
    break;

  case TOKENIZER_WAIT_CLICK:
    wait_click_statement();
    yield = 1;    //speed addition
    break;
  case TOKENIZER_IS_KEY:
    is_key_statement();
    break;

  case TOKENIZER_WHEEL_LEFT:
    wheel_left_statement();
    break;
  case TOKENIZER_WHEEL_RIGHT:
    wheel_right_statement();
     break;

  case TOKENIZER_IF:
    if_statement();
    break;
  case TOKENIZER_ELSE:
    else_statement();
    break;
  case TOKENIZER_ENDIF:
    endif_statement();
    break;
  case TOKENIZER_GOTO:
    goto_statement();
    break;
  case TOKENIZER_GOSUB:
    gosub_statement();
    break;
  case TOKENIZER_RETURN:
    return_statement();
    break;
  case TOKENIZER_FOR:
    for_statement();
    break;
  case TOKENIZER_NEXT:
    next_statement();
    break;
  case TOKENIZER_DO:
    do_statement();
    break;
  case TOKENIZER_UNTIL:
    until_statement();
    break;
  case TOKENIZER_WHILE:
    while_statement();
    break;
  case TOKENIZER_WEND:
    wend_statement();
    break;
  case TOKENIZER_END:
    end_statement();
    break;
  case TOKENIZER_LET:
    accept(TOKENIZER_LET);
   
  case TOKENIZER_VARIABLE:
    let_statement();
    break;
  case TOKENIZER_REM:
    rem_statement();
    break;
  case TOKENIZER_COMMENT_BLOCK:
    bc_statement();
    break;
  case TOKENIZER_CLS:
    cls_statement();
    break;

  case TOKENIZER_SET_RAW:
    set_raw_statement();
    break;

  case TOKENIZER_SET_SCRIPT_AUTOSTART:
    set_autostart_statement();
    break;

  case TOKENIZER_EXIT_ALT:
    exit_alt_statement();
    break;

  case TOKENIZER_SHUT_DOWN:
    shutdown_statement();
    break;
 
	case   TOKENIZER_MD_DETECT_MOTION:
		md_detect_motion_statement();
    yield = 1;    //speed addition
		break;
	case  TOKENIZER_MD_GET_CELL_DIFF:
		md_get_cell_diff_statement();
		break;

  case TOKENIZER_BLUE_ON:
    blue_on_statement();
    break;

  case TOKENIZER_BLUE_OFF:
    blue_off_statement();
    break;

  case TOKENIZER_AFLF:
    aflf_statement();
    break;

  case TOKENIZER_AFLO:
    aflo_statement();
    break;

  case TOKENIZER_AFLOFF:
    afloff_statement();
    break;

  case TOKENIZER_AF:
    af_statement();
    break;
    
  case TOKENIZER_PLAY_SOUND: 
 	play_sound_statement(); 
 	break;   

  case TOKENIZER_BEEP: 
 	beep_statement(); 
 	break;
 
  case TOKENIZER_SET_RESOLUTION: 
 	set_resolution_statement(); 
 	break;
 
  case TOKENIZER_SET_QUALITY: 
 	set_quality_statement(); 
 	break;

  case TOKENIZER_TLPF:
 	 set_tlpf_statement();
 	 break;

  case TOKENIZER_TLPO:
 	 set_tlpo_statement();
 	 break;
 

  case TOKENIZER_SET_BE: 
 	set_bracketing_exposures_statement(); 
 	break; 

  case TOKENIZER_SET_FS: 
 	set_focus_step_statement(); 
 	break; 
 
  case TOKENIZER_SS: 
 	save_stack_statement(); 
 	break; 
 
 case TOKENIZER_AFB: 
 	afb_statement(); 
 	break; 

  case TOKENIZER_SSB: 
 	ssb_statement(); 
 	break; 

  case TOKENIZER_BL: 
 	bl_statement(); 
 	break; 

  case TOKENIZER_BD: 
 	bd_statement(); 
 	break; 

  case TOKENIZER_BA: 
 	ba_statement(); 
 	break; 

  case TOKENIZER_WUD: 
 	wud_statement(); 
 	break; 

  case TOKENIZER_WFSP: 
 	wfsp_statement(); 
 	break; 
 
 case TOKENIZER_STRING:
   
  print_statement();
      break;

   case TOKENIZER_TL:
      tl_statement();
      break;
      
   case TOKENIZER_ISHOTS:
     ishots_statement();
     break;     

  case TOKENIZER_AE_LOCK:
 	 lock_ae_statement();
 	 break;

  case TOKENIZER_AE_UNLOCK:
 	 unlock_ae_statement();
 	 break;

  case TOKENIZER_SET_AFLOCK:
 	 set_aflock_statement();
 	 break;

  case TOKENIZER_LA:
 	 la_statement();
 	 break;

  case TOKENIZER_UF:
 	 uf_statement();
 	 break;

  case TOKENIZER_SDMCN:
 	 sdmcn_statement();
 	 break;

  case TOKENIZER_SDMCF:
 	 sdmcf_statement();
 	 break;

  case TOKENIZER_SDMCL:
 	 sdmcl_statement();
 	 break;

  case TOKENIZER_SDMCLL:
 	 sdmcll_statement();
 	 break;

  case TOKENIZER_SHOOT_MOVIE:
 	 shoot_movie_statement();
 	 break;
 
  case TOKENIZER_RECORD:
       record_statement();
       break;

  case TOKENIZER_PLAYBACK:
       playback_statement();
       break;

  case TOKENIZER_SET_MAX_SPT:
    set_max_spt_statement();
    break;

  case TOKENIZER_SLEEP_UNTIL:
    su_statement();
    break;

  case TOKENIZER_FT:
    ft_statement();
    break;

  case TOKENIZER_SYNC_ON:
    so_statement();
    break;

  case TOKENIZER_SYNC_OFF:
    sf_statement();
    break;

  case TOKENIZER_SD:
    sd_statement();
    break;

  case TOKENIZER_UB:
    ub_statement();
    break;

  case TOKENIZER_UP:
    up_statement();
    break;

  case TOKENIZER_GET_ZOOM:
    get_zoom_statement();
    break;
 
  case TOKENIZER_GET_FOCUS:
    get_focus_statement();
    break;

  case TOKENIZER_GET_LUMINANCE:
   get_luminance_statement();
   break;

   case TOKENIZER_SWS:
   sws_statement();
   break;

   case TOKENIZER_SWNM:
   swnm_statement();
   break;

   case TOKENIZER_SSPD:
   sspd_statement();
   break;

   case TOKENIZER_AP:
   ap_statement();
   break;

   case TOKENIZER_LINE_ONE:
   line_one_statement();
   break;

   case TOKENIZER_LINE_TWO:
   line_two_statement();
   break;

   case TOKENIZER_LINE_THREE:
   line_three_statement();
   break;

   case TOKENIZER_LOG:
   log_statement();
   break;
   
   case TOKENIZER_DISABLE_LOGGING:
   disable_logging_statement();
   break;

   case TOKENIZER_ENABLE_LOGGING:
   enable_logging_statement();
   break;

   case TOKENIZER_CEV:
   change_ev_statement();
   break;

   case TOKENIZER_TUP:
   tup_statement();
   break;

   case TOKENIZER_UAFL:
   uafl_statement();
   break;

   case TOKENIZER_UPL:
   upl_statement();
   break;

    case TOKENIZER_SET_LED:
    set_led_statement();
    break;

    case TOKENIZER_SAVE_FOCUS:
    save_focus_statement();
    break;

    case TOKENIZER_SAVE_ZOOM:
    save_zoom_statement();
    break;

    case  TOKENIZER_RESTORE_FOCUS:
    restore_focus_statement();
    break;

    case TOKENIZER_RESTORE_ZOOM:
    restore_zoom_statement();
    break;

    case TOKENIZER_AF_FIRST:
    af_first_statement();
    break;

    case TOKENIZER_AF_ALL:
    af_all_statement();
    break;

    case TOKENIZER_SV:
    sv_statement();
    break;

    case TOKENIZER_STPV:
    stpv_statement();
    break;

    case TOKENIZER_SM:
    sm_statement();
    break;

    case TOKENIZER_SET_USER_TV_BY_ID_REL:
    set_user_tv_by_id_rel_statement();
    break;

  case TOKENIZER_SET_AV_REL:
    set_av_rel_statement();
    break;
 
 case TOKENIZER_CONSOLE_REDRAW:
    console_redraw_statement();
    break;
  
  case TOKENIZER_FSON:
    shoot_fast_statement();
    break;

  case TOKENIZER_PFSAT:
  prepare_for_shooting_fast_statement();
    break;
  case TOKENIZER_FAST_ISO_MAX:
    fast_iso_statement();
    break;

  case TOKENIZER_COVER_LENS:
    cover_lens_statement();
    break;

  case TOKENIZER_MB:
    mb_statement();
    break;

 case TOKENIZER_FLASH_OFF:
    flash_off_statement();
    break; 

 case TOKENIZER_FLASH_ON:
    flash_on_statement();
    break; 
 
 case TOKENIZER_START_CLOCK: 
    start_clock_statement(); 
    break;

 case TOKENIZER_DISABLE_VIDEO_OUT: 
    disable_video_out_statement(); 
    break;

 case TOKENIZER_ENABLE_VIDEO_OUT: 
    enable_video_out_statement(); 
    break;

 case TOKENIZER_CAPTURE_MODE:
    capture_mode_statement(); 
    break;

  case TOKENIZER_REMOVE:
    remove_statement();
    break;

  case TOKENIZER_GET_CONFIG_VALUE:
    get_config_value_statement();
    break;

  case TOKENIZER_SET_CONFIG_VALUE:
    set_config_value_statement();
    break;

  case TOKENIZER_SET_SV96:
    set_sv96_statement();
    break;  

  case TOKENIZER_MF_OFF:
    mf_off_statement();
    break;  

  case TOKENIZER_MF_ON:
    mf_on_statement();
    break;  

   case TOKENIZER_CS:
    cs_statement();
    break;
    
   case TOKENIZER_AEB_ON:
    aeb_on_statement();
    break;   

    case TOKENIZER_AEB_OFF:
    aeb_off_statement();
    break;    

   case TOKENIZER_CS_OFF:
    cs_off_statement();
    break;
 
   case TOKENIZER_FOCUS_OUT:
    focus_out_statement();
    break; 

   case TOKENIZER_FOCUS_IN:
    focus_in_statement();
    break; 

   case TOKENIZER_FOCUS_AT_NP:
    focus_at_np_statement();
    break;
 
   case TOKENIZER_SET_FOCUS_RANGE:
    set_focus_range_statement();
    break;

   case TOKENIZER_CFPS:
    change_fps_statement();
    break;
 
  case TOKENIZER_LOG_BV:
      log_bv_statement();
      break;

  case TOKENIZER_VZOOM_IN_OUT:
      vzoom_in_out_statement();
      break;

  case TOKENIZER_ENABLE_BIG_ZOOM:
      ebz_statement();
      break;

  case TOKENIZER_DISABLE_BIG_ZOOM:
      dbz_statement();
      break;

  case TOKENIZER_SH:
      sh_statement();
      break;

  case TOKENIZER_EH:
      eh_statement();
      break;

  case TOKENIZER_SMIN:
      smin_statement();
      break;

  case TOKENIZER_EM:
      em_statement();
      break;

  case TOKENIZER_LG:
      lg_statement();
      break;
 
 case TOKENIZER_CONSOLE_POSITION:
      console_position_statement();
      break;

  case TOKENIZER_SET_HDR_TV96:
       shdrtv96_statement();
       break;
 
  case TOKENIZER_BURSTSEQ:
      burstseq_statement();
      break;

  case TOKENIZER_SDMBM:
       sdmbm_statement();
       break;

  case TOKENIZER_SDM2D:
       sdm2dm_statement();
       break;

  case TOKENIZER_SDM3D:
       sdm3dm_statement();
       break;
       
  case TOKENIZER_SDM3DSPORTS:
       sdm3dsports_statement();
       break;
       
  case TOKENIZER_SDMAUTOHDR:
       sdmautohdr_statement();
       break;  

  case TOKENIZER_SDMUHDR:      
       sdmuserhdr_statement();
       break; 
       
  case TOKENIZER_SDMTM:
       sdmtm_statement();
       break;        
 
  case TOKENIZER_WARNING:
       warning_statement();
       break;  
  
   case  TOKENIZER_SDM_RESET:
       sdm_reset_statement();
       break; 
       
  case  TOKENIZER_LINE:      
        sdm_line_statement();
       break;
 
 case  TOKENIZER_SCROLL:
        scroll_statement();
       break;
       
  case  TOKENIZER_IS_OFF:
        is_off_statement();
       break;
        
  case  TOKENIZER_IS_ON:
        is_on_statement();
       break;

  case TOKENIZER_SET_AF_ASSIST:
        af_assist_statement();
       break;
       
 
   case TOKENIZER_PSUSBAT:
    accept(TOKENIZER_PSUSBAT);
    int hpenable= expr();
    if ( hpenable > 0) start_usb_HPtimer(hpenable*1000);
    else start_usb_HPtimer(1000);
    accept_cr(); 
    break;

   case TOKENIZER_PSUSBOFF:
    accept(TOKENIZER_PSUSBOFF);
    stop_usb_HPtimer();
    accept_cr(); 
    break;  

   case TOKENIZER_SCJR_STATE:
    accept(TOKENIZER_SCJR_STATE);
#if defined(PROPCASE_CANON_RAW)
#if defined(CAM_HAS_NATIVE_RAW)
   int var;
   var =expr();
   if(var<0)var=0;
   if(var>2)var=2;
   shooting_set_prop(PROPCASE_CANON_RAW,var);
#endif
#endif
    accept_cr(); 
      break; 
 
   case TOKENIZER_SET_SHOOT_FAST_PARAM:    
      set_fast_param_statement();
      break;

  case TOKENIZER_BZDP:
      bzdp_statement();
      break;
      
#if defined(OPT_ALARIK)
  case TOKENIZER_SELECT:
      select_statement();
      break;

  case TOKENIZER_CASE:
      case_statement();
      break;

  case TOKENIZER_CASE_ELSE:
      case_else_statement();
      break;

  case TOKENIZER_SET_USER_AV_BY_ID:
      set_uav_by_id_statement();
      break;

  case TOKENIZER_SET_USER_TV_BY_ID:
      set_utv_by_id_statement();
      break;

  case TOKENIZER_SET_CONSOLE_LAYOUT:
      set_console_layout();
      break;

  case TOKENIZER_SET_CONSOLE_AUTOREDRAW:
      set_console_autoredraw();
      break;
 
#endif
  default:
    DEBUG_PRINTF("ubasic.c: statement(): not implemented %d\n", token);
    ended = 1;
    ubasic_error = UBASIC_E_UNK_STATEMENT;
  }
 }
}
//---------------------------------------------------------------------------

static void
line_statement(void) //Speed-up mods by Dave Mitchell
{
 int t1, t2, count; 
  /*line numbers have been removed */
  DEBUG_PRINTF("----------- Line number %d ---------\n", tokenizer_line_number());
  
  if (tokenizer_token() == TOKENIZER_LABEL) 
 {
#ifdef DEBUG
      tokenizer_label(string, sizeof(string));
      DEBUG_PRINTF("line_statement: label: %s\n", string );
#endif
      accept(TOKENIZER_LABEL);
      accept(TOKENIZER_CR);
      return;
  }

  //speed addition
  t1 = get_tick_count();
  count = 0;
  while (1) 
    {
      statement();
      count++;
      t2 = get_tick_count();
      if (yield)          //must yield for eg 'press'
         break;
      if (t2 != t1)       //a 'tick' has occurred
         break;        
      if ((count == speed) && !block_comment) //we've done enough
         break;
    }
  return;
}
//---------------------------------------------------------------------------
void
ubasic_run(void)
{
  if(tokenizer_finished()) {
    DEBUG_PRINTF("uBASIC program finished\n");
    return;
  }
  line_statement();
}
//---------------------------------------------------------------------------
int
ubasic_finished(void)
{
  return ended || tokenizer_finished();
}
//---------------------------------------------------------------------------
void
ubasic_set_variable(int varnum, int value)
{
  if(varnum >= 0 && varnum < MAX_VARNUM) {
    variables[varnum] = value;
  }
}
//---------------------------------------------------------------------------
int
ubasic_get_variable(int varnum)
{
  if(varnum >= 0 && varnum < MAX_VARNUM) {
    return variables[varnum];
  }
  return 0;
}
//---------------------------------------------------------------------------
void
ubasic_end() {
  if (print_screen_d >= 0) {
    close(print_screen_d);
    print_screen_d = -1;
    print_screen_p = 0;
  }
}
//---------------------------------------------------------------------------

