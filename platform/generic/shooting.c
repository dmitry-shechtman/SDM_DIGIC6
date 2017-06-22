#include "lolevel.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "math.h"
#include "stdlib.h"
#include "conf.h"
#include "ubasic.h"
#include "..\core\gui_lang.h"
#include "lang.h"
#include "..\..\core\gui_osd.h"
#include "..\..\core\draw_palette.h"
 
static const unsigned MODESCNT=(sizeof(modemap)/sizeof(modemap[0]));
static const double inv_log_2 = 1.44269504088906;   
 
#define SS_SIZE (sizeof(shutter_speeds_table)/sizeof(shutter_speeds_table[0]))
#define SSID_MIN (shutter_speeds_table[0].prop_id)             
#define SSID_MAX (shutter_speeds_table[SS_SIZE-1].prop_id)     

#define USE_NEAR_FAR 2 

#define AS_SIZE (sizeof(aperture_sizes_table)/sizeof(aperture_sizes_table[0]))
#define ASID_MIN (aperture_sizes_table[0].id)
#define ASID_MAX (aperture_sizes_table[AS_SIZE-1].id)

#define AV96_MIN (aperture_sizes_table[0].prop_id)
#define AV96_MAX (aperture_sizes_table[AS_SIZE-1].prop_id)

#define ISO_SIZE (sizeof(iso_table)/sizeof(iso_table[0]))
#define ISO_MIN (iso_table[0].id)
#define ISO_MAX (iso_table[ISO_SIZE-1].id)
#define ISO_MIN_VALUE (iso_table[1-iso_table[0].id].prop_id)
#define ISO_MAX_VALUE (iso_table[ISO_SIZE-1].prop_id)
#define STO 10                

#if defined(CAMERA_s100)
 volatile int* LVcounter= (int*)0xC0F070C8;
#else
 volatile int* LVcounter= (int*)0xC0F07008;
#endif

#if defined(SDM_TESTS)
extern int flash_duration[3];
#endif
 
extern unsigned int tl_target_shot_count; 
unsigned short gForceSv;
extern unsigned char gEnableBigZoom,displayedMode;
unsigned char gBrightActivated,gHdrPhase,gTooBright,gTooDark,gNearFar=1,HPTimerFinished,gSDM_will_use_flash,gHPtimerCBfunc;
int gTv,gAv,gAvOverride,gBv,gSv,gNDfactor,gInitialFocus,hdrND,initialTv96,gZoomPoint,gFocalPoint,gMaxIso96;
int Hdr3dTv96;
static unsigned int gpwidth;
extern int computedBv,nSW,usb_HPtimer_handle,gCurrentFocus,gBracketingCounter;
int sertx =0;                  
char pcode;                    
char lobyte;                   
char hibyte;                   
int sd_repeat;                 
int rx_ok;                     
int up=0;                      
int ub=0;                      
int pressRec,releaseRec,pressPlay,releasePlay,pressPower,manualFocus;
int enablePutOff;
static short iso_market_base=0;
static short sv96_base=0;
static short svm96_base=0;
static short sv96_base_tmp=0;
static short gForceTv;
extern int rf_focus;
extern int subject_far;
extern int has_refocused;
const int ovr[] = {-576,-544,-512,-480,-448,-416,-384,-352,-320,-288,-256,-224,-192,-160,-128,-96,-64,-32,-1,0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512,544,576,608,640,672,704,736,768,800,832,864,896,928,960,992,1024,1056,1084,1116,1148,1180,1212,1244,1276,1308,1340,1372,1404,1436,1468};
const char* tv_value[]={ "65\"","50\"", "40\"", "30\"", "25\"","20\"","15\"","13\"","10\"","8\"","6\"","5\"","4\"","3.2\"","2.5\"","2\"","1.6\"","1.3\"","1\"","Null","0.8","0.6","0.5","0.4","0.3","1/4","1/5","1/6","1/8","1/10","1/13","1/15","1/20","1/25","1/30","1/40","1/50","1/60","1/80", "1/100","1/125","1/160","1/200","1/250","1/320","1/400","1/500","1/640","1/800", "1/1000","1/1250","1/1600","1/2000", "1/2500","1/3200","1/4000", "1/5000", "1/6400", "1/8000", "1/10000", "1/12800", "1/16000", "1/20000", "1/26000","1/32000","1/40000"};
#define OVR_SIZE (sizeof(tv_value)/sizeof(tv_value[0]));
extern float exitp,mag,entrance,realfl,realav;
extern long myav;
extern int delay_status,myhyp,gFlashDisable,self_timer_handle,usb_HPtimer_handle;
extern int delay_user_value,put_off_ev96;
static const double log_2=0.6931471805599;//natural logarithm of 2
static const double sqrt2=1.4142135623731;//square root from 2 
static const double k=12.5;//K is the reflected-light meter calibration constant 
static short save_tv;
static short delta_tv, step_tv; 
PHOTO_PARAM photo_param_put_off;
#ifndef CAM_FILE_COUNTER_IS_VAR
const unsigned int param_file_counter = PARAM_FILE_COUNTER;
#endif
static int sunrise=0; 
static short value=0; 
static int sb =0;        
static void send_byte(char b);
static void mdelay(int d);
static char gpbuf[64];         
int inf_focus;
int current_focus;
int previous_focus,srf;
int fdtv = -1;                     
int fddist = -1;                   
int fdauto = -1;                   
int fdsingle = -1;                 
#if OPT_SUNSET
int fdsun = -1;                    
#endif
int fdhdr = -1;                    
int previous_focus,srf;
int gNearPoint=1000;
int gFarPoint=MAX_DIST;
int gStereoShift;
static int gRangeFinder;
int block_script,usb_pulse,gCoarseDelay,gPeriodTenMsec;
int gTv96min,gTv96max;
int hdrDark96,hdrLight96;
unsigned char gUpPressed,gDownPressed,gLeftPressed,gRightPressed,gSetPressed,gDispPressed,gHalfPressed,gFullPressed,gDidShowFocus;
#if defined(KEY_POWER)
unsigned char gPowerPressed;
#endif
EXPO_BRACKETING_VALUES bracketing;
extern int gNumOfBvReadings;
extern int rawBv[BVBUFFSIZE];        
static int sortedBv[BVBUFFSIZE];
onHP onHalfPress;
unsigned char gAutoIsoDisabled;
 
/*---------------------------------------------------------------------------------------------------------
     High Precision USB Remote Timer Callback routines from  CHDK
 ---------------------------------------------------------------------------------------------------------*/

int self_timer_good(int time, int interval) 
{
 static unsigned char count = 0;
 self_timer_handle=0;
 ++count;
 if(count<HPTimer.interval)start_self_timer();
 else 
  {
   HPTimerFinished=1;
   stop_self_timer();
   count=0;
  }
 return 0;
}
int self_timer_bad(int time, int interval) 
{ 
 return (self_timer_good(time, interval));
}

int usb_HPtimer_error_count;

int usb_HPtimer_good(int time, int interval) 
{
 usb_HPtimer_handle=0;
 start_usb_HPtimer(interval) ;
 usb_high_count();
 return 0;
}
 
int usb_HPtimer_bad(int time, int interval) 
{
    usb_HPtimer_error_count++;
    return (usb_HPtimer_good(time, interval));
}


void shooting_set_iso_mode(int v);

int compareInt(const void* a, const void* b)
{
 return *(int*)a - *(int*)b;
}

void sortBv()
{
 memcpy(sortedBv,rawBv,gNumOfBvReadings*sizeof(int));
 qsort(sortedBv,gNumOfBvReadings,sizeof(int),compareInt);
}

int averageBv(int low,int high)
{
 int i,firstIndex,lastIndex,samples,sum=0;
 firstIndex = ((low*gNumOfBvReadings)/100)-1;
 lastIndex = ((high*gNumOfBvReadings)/100);
 samples = lastIndex - firstIndex;
 for(i=firstIndex;i<lastIndex;i++)
  sum+=sortedBv[i];
 computedBv = sum/samples;
 return computedBv;
}
void printBv()
{
 int fd,i;
 char buf[32];
 fd = open("A/rawBv.txt",O_WRONLY|O_CREAT, 0777);
 for(i=0;i<gNumOfBvReadings;i++)
  {
   sprintf(buf,"%-8d\n",rawBv[i]);
   write(fd,buf,strlen(buf));
  }
  close(fd);
 fd = open("A/sortedBv.txt",O_WRONLY|O_CREAT, 0777);
 for(i=0;i<gNumOfBvReadings;i++)
  {
   sprintf(buf,"%-8d\n",sortedBv[i]);
   write(fd,buf,strlen(buf));
  }
  sprintf(buf,"50%% to 80%% averaged %d of %d readings total\n",averageBv(50,80),gNumOfBvReadings);
  write(fd,buf,strlen(buf));
  close(fd);
}

   void presynch()
  {  
    typedef ShutterSpeed *SsPointer;
    SsPointer ss2 = NULL;
    int m=mode_get()&MODE_SHOOTING_MASK;
    short int vv;  
    delay_status = conf.synch_delay_enable;
    delay_user_value = conf.synch_flash_delay; 
    static short AutoIsoValue;               
    if(!gAutoIsoDisabled)AutoIsoValue=shooting_get_canon_iso_mode();			    

    if(conf.synch_enable && (get_tick_count()>5000))                                    
     {
       if(conf.bright_screen && shooting_will_use_flash())gBrightActivated = 1;
         if((shooting_will_use_flash()||conf.slave_flash) && !conf.bright_screen)       
         {
          if(!conf.camera_position) 
           {
            if(conf.add_synch_delays &&(conf.synch_fine_delay || conf.synch_coarse_delay ) )                                                   
             {
              conf.synch_delay_enable = 1;                                              
             }
             else                                                                       
             {
              conf.synch_delay_enable = 0;                                              
             }
             conf.synch_flash_delay = 0;
            } 
           else if(conf.camera_position)   
            {
             conf.synch_delay_enable = 1;                                                
             ss2 = (SsPointer)shooting_get_camera_tv_line();
             if(conf.tv_override_enable) conf.synch_flash_delay = (int)(5000/pow(2,ovr[conf.tv_override]/96.0)+0.5);
             else conf.synch_flash_delay = (int)(5000/pow(2,ss2->prop_id/96.0)+0.5); 
             if(conf.synch_flash_delay >80) conf.synch_flash_delay = 80;
           }
        } 

      else                                                                               
        {
         if(conf.add_synch_delays &&(conf.synch_fine_delay || conf.synch_coarse_delay ))                                                   
          {
           conf.synch_delay_enable = 1;                                              
          }
           else                                                                       
          {
           conf.synch_delay_enable = 0;                                              
          }
         conf.synch_flash_delay = 0;
        }      
     } 
  }


void get_tv(char *buf,int val96)
{
  int intValue=(int)(shooting_get_shutter_speed_from_tv96(val96)*100000); 
   if(intValue<=50000) 
    {   
     sprintf(buf,"1/%d ",100000/intValue);        
     }
    else if((intValue>50000) && (intValue<95000))        
     {
      sprintf(buf+strlen(buf),"0.%1d ",intValue/10000);     
     }     
    else if((intValue>=95000) && (intValue < 110000))    
     sprintf(buf+strlen(buf),"1 ");       
    else sprintf(buf+strlen(buf),"%2d.%1d ",intValue/100000,((intValue+5000)%100000)/10000); 
}

int shooting_get_tv()
{
    short int tvv;
    long i;
    _GetPropertyCase(PROPCASE_USER_TV, &tvv, sizeof(tvv));
    for (i=0;i<SS_SIZE;i++){
	if (shutter_speeds_table[i].prop_id == tvv)
	    return shutter_speeds_table[i].id;
    }
    return 0;
}

const ShutterSpeed *shooting_get_tv_line()
{
    short int tvv;
    long i;
    _GetPropertyCase(PROPCASE_USER_TV, &tvv, sizeof(tvv));
    for (i=0;i<SS_SIZE;i++){
	if (shutter_speeds_table[i].prop_id == tvv)
	    return &shutter_speeds_table[i];
    }
    return 0;

}

const ShutterSpeed *shooting_get_camera_tv_line()
{
    short int tvv;
    long i;
    _GetPropertyCase(PROPCASE_CAMERA_TV, &tvv, sizeof(tvv));
    for (i=0;i<SS_SIZE;i++)
    {
	if (shutter_speeds_table[i].prop_id >= tvv)
       {
        if(shutter_speeds_table[i].prop_id == tvv)
	    return &shutter_speeds_table[i];
         else
          { 
           if((abs(shutter_speeds_table[i].prop_id - tvv))<=(abs(shutter_speeds_table[i-1].prop_id - tvv)))
                return &shutter_speeds_table[i];
           else return &shutter_speeds_table[i-1];
          }
        }
     }
    return 0;
}

char* shooting_get_camera_tv_string()
{
    short int tvv;
    long i;
    _GetPropertyCase(PROPCASE_CAMERA_TV, &tvv, sizeof(tvv));
    for (i=0;i<=65;i++)
    {
	if (ovr[i] >= tvv)
       {
        if(ovr[i] == tvv) 
         {
          if(i==19) i=18;                                  
	    return (char*)tv_value[i];
         }
         else                                              
          { 
           if((abs(ovr[i] - tvv))<(abs(ovr[i-1] - tvv)))   
             {
                if(i==19) i=18;
                return (char*)tv_value[i];                 
             }
           else
             {
              if(i==20) i=19;
              return (char*)tv_value[i-1];                
             }
           }
         }
     }                                                    
    return (void*)"?";
}

char* get_tv_string_for_tv96(int tvv)
{
    long i;
    for (i=0;i<=65;i++)
    {
	if (ovr[i] >= tvv)
       {
        if(ovr[i] == tvv) 
         {
          if(i==19) i=18;                                  
	    return (char*)tv_value[i];
         }
         else                                              
          { 
           if((abs(ovr[i] - tvv))<(abs(ovr[i-1] - tvv)))   
             {
                if(i==19) i=18;
                return (char*)tv_value[i];                 
             }
           else
             {
              if(i==20) i=19;
              return (char*)tv_value[i-1];                
             }
           }
         }
     }                                                    
    return (void*)"?";
}

int shooting_get_tv96_index(int v)                       
{
    short int tvv = v;
    long i;
    for (i=0;i<=65;i++)
    {
	if (ovr[i] >= tvv)
       {
        if(ovr[i] == tvv)
         {
          if(i==19) i=18;
	    return i;
         }
         else
          { 
           if((abs(ovr[i] - tvv))<=(abs(ovr[i-1] - tvv)))
             {
                if(i==19) i=18;
                return i;
             }
           else
             {
              if(i==20) i=19;
              return i-1;
             }
           }
         }
       }
    return 19;
}

int shooting_get_tv96_from_index(int v)
{
  return ovr[v];
}

void shooting_set_tv(int v)
{
    long i;


    for (i=0;i<SS_SIZE;i++){
	if (shutter_speeds_table[i].id == v){
	    short int vv = shutter_speeds_table[i].prop_id;
	    _SetPropertyCase(PROPCASE_USER_TV, &vv, sizeof(vv));
  		_SetPropertyCase(PROPCASE_CAMERA_TV, &v, sizeof(v));       
	    return;
	}
    }

}

void shooting_set_tv_rel(int v)
{
    int cv = shooting_get_tv();
    shooting_set_tv(cv+v);
}

short int get_camera_tv(void){
  short int vv;
 _GetPropertyCase(PROPCASE_CAMERA_TV, &vv, sizeof(vv));
 return vv;
}

void set_camera_tv(short int v){
  short int vv=v;
 _SetPropertyCase(PROPCASE_CAMERA_TV, &vv, sizeof(vv));
}

void set_camera_tv_override(int v)
{
 set_camera_tv(ovr[v]);
}

//TODO sd990 hack for overrides
//caller must save regs

int captseq_hack_override_active() 
{
 if (state_kbd_script_run)
 	if ( photo_param_put_off.tv96 ) return 1;
 if(conf.disable_overrides)
 	return 0;
 if((conf.tv_override!=19) && conf.tv_override_enable)
 	return 1;
 return 0;
}

void disable_preflash()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n"); 
 int flash,power,flashmode;
 
   if(shooting_will_use_flash())gSDM_will_use_flash=1;
   else gSDM_will_use_flash=0;
  
  
  if((displayedMode==6)&&conf.slave_flash)
    {
     flashmode  = 1;   
     power = conf.slave_flash-1; 
     _SetPropertyCase(PROPCASE_FLASH_ADJUST_MODE, &flashmode, sizeof(flashmode));
     _SetPropertyCase(PROPCASE_FLASH_MANUAL_OUTPUT, &power, sizeof(power)); 
    }  
 #if defined(PROPCASE_FLASH_EXP_COMP)
 if ((conf.flash_exp_comp!=9)&&!conf.slave_flash)     
        {
            flashmode = 0;   
            flash = 1;  
            power = (conf.flash_exp_comp - 9) * 32;
            set_property_case(PROPCASE_FLASH_ADJUST_MODE, &flashmode, sizeof(flashmode));
            set_property_case(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));
#if defined(PROPCASE_FLASH_EXP_COMP)
            set_property_case(PROPCASE_FLASH_EXP_COMP, &power, sizeof(power));
#endif
#if defined(PROPCASE_EXT_FLASH_EXP_COMP)
            set_property_case(PROPCASE_EXT_FLASH_EXP_COMP, &power, sizeof(power));
#endif    
        }
#endif 
 asm volatile("LDMFD   SP!, {R0-R12,LR}\n");
}

void shooting_expo_param_override(void)
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n");
 int tv,x,i;
 short vv;
 
 int m=mode_get()&MODE_SHOOTING_MASK;
#if defined SDM_TESTS
#if defined(CAMERA_a620)
 volatile long *mmio = (void*)0xc0220204; 
 #define USB_MASK 0x8000000

#elif defined(CAMERA_a570)|| defined(CAMERA_a590) || defined(CAMERA_ixus70)
 static long usb_physw[3];
 #define USB_MASK 0x40000
#endif
#endif
if(conf.user_1==80976)conf.tv_override_enable=0;            
 sb=shooting_get_bv96(); 
 gBv = sb;
 enablePutOff = state_kbd_script_run;
int count; 
 volatile int* counter;
 counter= PERIOD_COUNT;
 int tmp;
 
  unsigned int tick = get_tick_count();
 do{}	 
 while(tick == get_tick_count());
 tmp=*(counter) & 0xffff;
 tick = get_tick_count();
 do{}	 
 while(tick == get_tick_count());
           count=*(counter) & 0xffff;
          if(count>tmp)gPeriodTenMsec=count-tmp;
          else gPeriodTenMsec=STD_PERIOD-tmp+count;
if(conf.SsTenMsecCount==9999)conf.SsTenMsecCount=gPeriodTenMsec; 
else 
{
 if(gPeriodTenMsec>((conf.SsTenMsecCount*3)/2)) conf.SsTenMsecCount=  gPeriodTenMsec;  
 else  
 if((gPeriodTenMsec<conf.SsTenMsecCount)&&(gPeriodTenMsec>((conf.SsTenMsecCount*9)/10)))conf.SsTenMsecCount=gPeriodTenMsec;               
} 

if(up)                              
{

  if(!conf.use_af_led)debug_led(1); 
  else ubasic_set_led(9,1,100);
  mdelay(conf.bw);                  
  if(!conf.use_af_led)debug_led(0); 
  else ubasic_set_led(9,0,0);       
  up=0;                             

} 

if(ub)                              
{

#if !defined(SDM_TESTS)
 for(i=0;i<7500/conf.bw;i++)        
 {
  if(!conf.use_af_led)debug_led(1); 
  else ubasic_set_led(9,1,100);
  mdelay(conf.bw);                  
  if(!conf.use_af_led)debug_led(0); 
  else ubasic_set_led(9,0,0);       
  mdelay(conf.bw);                  
 }

#else
#if defined(CAMERA_a620) || defined(CAMERA_a570) || defined(CAMERA_ixus70) || defined(CAMERA_a590)
long x;
do 
 {   
#if defined(CAMERA_a620)
 x=(long)*mmio;                    
#elif defined(CAMERA_a570) || defined(CAMERA_a590) || defined(CAMERA_ixus70)
 usb_physw[2] = 0;                                             
 _kbd_read_keys_r2(usb_physw);
 x=usb_physw[2];
#endif                          
}
while (!(x&USB_MASK));             
int tick = get_tick_count();       
debug_led(1);                      
do                                 
{
#if defined(CAMERA_a620)
 x=(long)*mmio;                    
#elif defined(CAMERA_a570) || defined(CAMERA_ixus70)
 usb_physw[2] = 0;                                             
 _kbd_read_keys_r2(usb_physw);
 x=usb_physw[2];
#endif
}
while (x&USB_MASK);                
debug_led(0);                      
usb_pulse=get_tick_count()-tick;
#endif
#endif
kbd_key_release(KEY_SHOOT_HALF);
block_script=0;
ub=0;                             
} 

else if(sertx)                      
{
 int t,u;
 rx_ok = 0;                         
 for(x=0;x<sd_repeat+1;x++)        
 {
  if(!conf.use_af_led) debug_led(1);                     
  else ubasic_set_led(9,1,100); 
  mdelay(4*conf.bw);
  if(!conf.use_af_led) debug_led(0);
  else ubasic_set_led(9,0,100); 
  mdelay((int)conf.bw);             

  send_byte(pcode);
  mdelay(2*conf.bw);                
  send_byte(lobyte);
  mdelay(2*conf.bw);                
  if(sd_repeat)
  {
   send_byte(hibyte);
   mdelay(2*conf.bw);                
  }
 }
  if(sd_repeat)                     
  {
   t=get_tick_count();
   do
   {
    u=get_usb_power(1);
   }
    while(((get_tick_count()-t)< STO)&& !u);  
 
    if(u)rx_ok=1;                      
 
  }
 kbd_key_release(KEY_SHOOT_HALF);
 block_script=0;
 sertx=0;
}
#if !defined(CAMERA_m3)
 onHalfPress.ZoomPoint=lens_get_zoom_point();
#endif
 onHalfPress.FlashMode = shooting_get_prop(PROPCASE_FLASH_MODE);
 _GetPropertyCase(PROPCASE_CAMERA_TV, &vv, sizeof(vv)); 	
 onHalfPress.Tv96=vv;
 gTv=vv;
 if((conf.user_1==80978)&&!(get_usb_bit()))
  {
   if(conf.tv_override_enable)
    Hdr3dTv96=ovr[conf.tv_override];
   else
    Hdr3dTv96=gTv;
  }
 
 _GetPropertyCase(PROPCASE_ALLBEST_AV, &vv, sizeof(vv));
 onHalfPress.Av96=vv;
 gAv=vv;
 onHalfPress.RealAV=(short)shooting_get_real_av();
#if !defined(CAMERA_m3)
 onHalfPress.FocalLength = get_focal_length(onHalfPress.ZoomPoint);
 onHalfPress.Hyperfocal=shooting_get_hyperfocal_distance();
#endif
 onHalfPress.Sv96=shooting_get_sv96();
 gSv=onHalfPress.Sv96;
 _GetPropertyCase(PROPCASE_BV, &vv, sizeof(vv));
 onHalfPress.Bv96=vv;

 if (!tl.running && !(!shooting_get_drive_mode()&&(conf.dist_mode==3))) presynch(); 
 if (enablePutOff)                                                 
 {

  if(photo_param_put_off.tv96 && !put_off_ev96)                    
   {
     shooting_set_tv96_direct(photo_param_put_off.tv96, SET_NOW);
     photo_param_put_off.tv96=0;
   }

   else                                                             
    {
      if ((conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override_enable)&& !(!tl.shoot_mode && tl.running && tl.sun)) 
      {
	  if (conf.tv_override != 19) set_camera_tv(ovr[conf.tv_override]);
      }
      else if (!tl.shoot_mode && tl.running && tl.sun && (tl.shot_count>1))             
      {
       set_camera_tv(value);
      } 
    }
#if !defined(CAMERA_m3)
  if (has_refocused && !shooting_get_focus_mode() && !shooting_get_drive_mode())       
   {
    ubasic_shooting_set_focus(conf.focus_pos,SET_NOW);				
   }
#endif
  } 
  
  else if((conf.user_1==80978)&&get_usb_bit())shooting_set_tv96_direct(Hdr3dTv96,SET_NOW);      
 
else if(!conf.bright_screen)                                                               
 {
   if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override_enable && !conf.disable_overrides) 
     {
	   if (conf.tv_override != 19) shooting_set_tv96_direct(ovr[conf.tv_override],SET_NOW);
     }
  }

 if (enablePutOff && photo_param_put_off.av96) 
  {
   shooting_set_av96_direct(photo_param_put_off.av96, SET_NOW);
   photo_param_put_off.av96=0;
  }
 else 

  if (conf.av_override_value && !conf.disable_overrides) shooting_set_av96_direct(shooting_get_av96_override_value(), SET_NOW);

 if (enablePutOff && (photo_param_put_off.sv96))        
  {
   shooting_set_sv96(photo_param_put_off.sv96, SET_NOW);
   photo_param_put_off.sv96=0; 
  }
  
  else if(conf.iso_override)
  {
   shooting_set_iso_real(shooting_iso_market_to_real(conf.iso_override), SET_NOW);
  }

#if !defined(CAMERA_m3)
 if (enablePutOff && photo_param_put_off.subj_dist)   
    {
     ubasic_shooting_set_focus(photo_param_put_off.subj_dist,SET_NOW);
     if(conf.user_2!=666) photo_param_put_off.subj_dist=0;
    }   

 else
  {

    current_focus = lens_get_focus_pos_from_lens();
    if(conf.user_range_set && !(conf.dist_mode && shooting_get_drive_mode())&& !conf.disable_overrides) 
   {
     if(!conf.focus_mode) allbest_shooting_set_focus(MAX_DIST);                                
     else if(conf.focus_mode == 1)allbest_shooting_set_focus(myhyp+fpd);                    
     else if (conf.focus_mode == 2) allbest_shooting_set_focus(conf.distance_setting+fpd);  
     else if(conf.focus_mode == 3) allbest_shooting_set_focus(rf_focus+fpd);                
   }
 

if((conf.dist_mode)&& shooting_can_focus() && (shooting_get_drive_mode()!=0)&&(state_shooting_progress != SHOOTING_PROGRESS_PROCESSING))
  {
  if (state_kbd_script_run)  lens_set_focus_pos(conf.distance_setting+fpd);    
  else if(displayedMode==8)lens_set_focus_pos(gNearPoint+fpd);   
  }    
 }
#endif

#if defined(CAM_HAS_ND_FILTER) 

 _GetPropertyCase(PROPCASE_SV, &mss_putoff.iso, sizeof(mss_putoff.iso));                                                                           
 mss_putoff.tv= shooting_get_tv96(); 
#if !defined(CAMERA_m3) 
#if !defined(CAM_HAS_IRIS_DIAPHRAGM) 
 if(onHalfPress.Av96<(get_mav(shooting_get_zoom())+32)) mss_putoff.nd=0;                                  
 else if(onHalfPress.Av96> (get_xav(shooting_get_zoom())-32)) mss_putoff.nd=1;                            
 gNDfactor=onHalfPress.Av96-get_mav(shooting_get_zoom()); 
#endif                                                
#endif

 if (enablePutOff && (photo_param_put_off.nd_filter))                                                        
  {
#if !defined(CAMERA_m3)
 #if !defined(CAM_HAS_IRIS_DIAPHRAGM)
   if(conf.nd_constant_exposure)                                                                             
     {                  
       if(photo_param_put_off.nd_filter == 1)                                                                
         {                                                          
          if (!mss_putoff.nd)                                                                                   
            shooting_set_tv96_direct(mss_putoff.tv-(get_xav(shooting_get_zoom())-get_mav(shooting_get_zoom())), SET_NOW);   
         }     
       else  
         {
          if(mss_putoff.nd)                                                                                  
          shooting_set_tv96_direct(mss_putoff.tv+get_xav(shooting_get_zoom())-get_mav(shooting_get_zoom()), SET_NOW);  
         }
     }
#endif
 #endif
   shooting_set_nd_filter_state(photo_param_put_off.nd_filter, SET_NOW);                                       
   photo_param_put_off.nd_filter=0;  
  }
 
 else if (conf.nd_filter_state && !conf.disable_overrides)                                                     
  {
#if !defined(CAMERA_m3)
#if !defined(CAM_HAS_IRIS_DIAPHRAGM)
   if(conf.nd_constant_exposure)                                                                               
     {     
       if(conf.nd_filter_state == 1)         
         { 
          if(!mss_putoff.nd) 
           {          
           shooting_set_tv96_direct(mss_putoff.tv-(get_xav(shooting_get_zoom())-get_mav(shooting_get_zoom())), SET_NOW);  
           }
           else
           {                                                                                   
           }  
         }           
       else if(conf.nd_filter_state == 2)                                                                     
           {
            if(mss_putoff.nd)
             {
              shooting_set_tv96_direct(mss_putoff.tv+gNDfactor, SET_NOW); 
             }
            else
             {                                                                                   
             }
           }
       }  
#endif
#endif
    shooting_set_nd_filter_state(conf.nd_filter_state, SET_NOW);
   } 
#endif  

#if !defined(CAMERA_m3) 
if(!((shooting_get_drive_mode()==1)&& conf.tv_bracket_value && (conf.bracket_type>=3)&&(gZoomPoint==(zoom_points-1)) &&!conf.bright_screen)) 
#else
if(!((shooting_get_drive_mode()==1)&& conf.tv_bracket_value && (conf.bracket_type>=3)) &&!conf.bright_screen)
#endif
 {
  if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override_enable && !conf.disable_overrides) 
     {
	if (conf.tv_override != 19) shooting_set_tv96_direct(ovr[conf.tv_override], SET_NOW);
     }
  }
 
  if(conf.user_1 == 80975)
  {
   gForceTv=1056;
   int actualAv=shooting_get_prop(PROPCASE_MIN_AV);
   int evCorrection=shooting_get_prop(PROPCASE_EV_CORRECTION_1);
   int bv=gBv-evCorrection;
   gForceSv=gForceTv+actualAv-bv;
   if(gForceSv>gMaxIso96)
    {
     gForceSv=gMaxIso96;
     gForceTv=gForceSv+bv-gAv;
    }
    shooting_set_tv96_direct(gForceTv, SET_NOW);
    shooting_set_sv96(gForceSv, SET_NOW);
    shooting_set_nd_filter_state(2,SET_NOW);
    gAvOverride=gAv-288;
  }
 
  if(conf.user_1 == 80977)
  { 
   gForceTv=onHalfPress.Sv96+gBv-shooting_get_prop(PROPCASE_EV_CORRECTION_1)-shooting_get_prop(PROPCASE_MIN_AV)-288;
   shooting_set_tv96_direct((gForceTv<-576)?-576:gForceTv, SET_NOW);
   shooting_set_nd_filter_state(1,SET_NOW);
  }
#if !defined(CAMERA_m3) 
 if (gZoomPoint==(zoom_points-1))
#else
 if(1)		 
#endif
 {   
  do {_SleepTask(20);}
  while(!shooting_get_prop(PROPCASE_SHOOTING)&& gHalfPressed); 
  gRangeFinder=lens_get_focus_pos_from_lens();   		 
  if(gHalfPressed)
  {
   gNearFar=!gNearFar;
   if(!gNearFar)conf.rangefinder_near=gRangeFinder;
   else conf.rangefinder_far=subject_far=gRangeFinder;
  }

 #if defined(CAMERA_m3)
 gDidShowFocus = 1;
 #else
  if((conf.zf_size && !state_kbd_script_run) || (state_kbd_script_run && gEnableBigZoom))
  {
    if(gHalfPressed)gDidShowFocus = 1;
    else gDidShowFocus = 2;
  }
#endif
 }
 
 if((shooting_get_drive_mode()==1)&& conf.tv_bracket_value && (conf.bracket_type>=3)&&!tl.running)
  {  
   static int fast96,slow96;  
   int n,tmp;
#if !defined(CAMERA_m3)
    if((gZoomPoint==(zoom_points-1))&&(shooting_get_drive_mode()==1)&& conf.tv_bracket_value && (conf.bracket_type>=3) && (conf.user_1 != 80976))  
#else
    if((shooting_get_drive_mode()==1)&& conf.tv_bracket_value && (conf.bracket_type>=3) && (conf.user_1 != 80976))
#endif
      {
       if((!gHdrPhase||gHdrPhase==3)){hdrDark96=onHalfPress.Bv96;gHdrPhase=1;gTooBright=gTooDark=0;conf.tv_override_enable=1;} 
       else if(gHdrPhase==1)                                                             
        {
         gHdrPhase=2;
         conf.tv_override_enable=1;
         hdrLight96=onHalfPress.Bv96;
         if(hdrDark96>hdrLight96)                                                         
          {
           tmp=hdrDark96;
           hdrDark96=hdrLight96;
           hdrLight96=tmp;
          }
          tmp=abs(hdrLight96-hdrDark96);
          if(tmp<64)                                                                   
           {
            play_sound(6);
            gHdrPhase=0;
            conf.bracket_type=3;
            conf.tv_bracket_value=1;
           }
          else play_sound(4);                                    
        }
       }
#if !defined(CAMERA_m3)
        else if((gHdrPhase==2) && (gZoomPoint!=(zoom_points-1)))  
#else
        else if(gHdrPhase==2)
#endif                        
         {
          int brightTv,darkTv,bracketPairs,atomsPerStep;
          hdrND=0;
          brightTv=hdrLight96+onHalfPress.Sv96-onHalfPress.Av96;
          if(brightTv>1056){hdrND=brightTv-1056;brightTv=1056;gTooBright=1;}           
          if(hdrND)hdrND=2^((hdrND+48)/96);                                             
          darkTv=hdrDark96+onHalfPress.Sv96-onHalfPress.Av96;
          if(darkTv<-576){darkTv=-576;gTooDark=1;}                                        
          tmp=abs(brightTv-darkTv);  
  
         if(tmp==1632)                                                                   
          {
           play_sound(6);
           conf.bracket_type=6;
           conf.tv_bracket_value=6;
           initialTv96 =224;
           gTv96max=224+4*6*32;
           gTv96min=224-4*6*32;
          }
         
         else  
          {         
           tmp=((tmp+32)/64)*64;                         
           int atomPairs=tmp/64;              
           if(atomPairs<8)                         
           {
            bracketPairs=1;
            if(atomPairs<7)atomsPerStep=atomPairs;
            else atomsPerStep=6;                   
           }
           else if(atomPairs<=13)                   
           {
            bracketPairs=2;
            atomsPerStep=atomPairs/2;              
           }
           else if(atomPairs<19)
           {
            bracketPairs=3;
            if(atomPairs<14)atomsPerStep=4;
            else if(atomPairs<17)atomsPerStep=5;
            else atomsPerStep=6;                   
           }
           else
           {
            bracketPairs=4;
            if(atomPairs<23)atomsPerStep=5;
            else atomsPerStep=6;                   
           }
         conf.tv_bracket_value=atomsPerStep;
         conf.bracket_type=2+bracketPairs;
          initialTv96 = brightTv-(bracketPairs*32*conf.tv_bracket_value);
          gTv96max=initialTv96+(32*conf.tv_bracket_value*bracketPairs); 
          gTv96min=initialTv96-(32*conf.tv_bracket_value*bracketPairs); 
          }
         set_camera_tv(gTv96max); 
           conf.tv_override=(gTv96max>=0)?19+(gTv96max/32):18+(gTv96max/32);          
            gHdrPhase=3;                                                
       }
       
       else if(conf.user_1 == 80976)                                    
       {
        if (!(m==MODE_M || m==MODE_TV))        
         initialTv96=gTv; 
        else 
         initialTv96=shooting_get_user_tv96(); 
         n= ((3+((conf.bracket_type-3)*2))-1)/2; 
        gTv96max=initialTv96+(32*conf.tv_bracket_value*n); 
        gTv96min=initialTv96-(32*conf.tv_bracket_value*n); 
        if(gTv96max>1056)
         {gTv96max=1056;gTooBright=1;play_sound(6);}     
        else gTooBright=0;
        if(gTv96min<-576){gTv96min=-576;gTooDark=1;play_sound(6);}  
        else gTooDark=0;
        
        conf.tv_override_enable=1; 
        shooting_set_tv96_direct(gTv96max,SET_NOW);      
        conf.tv_override=(gTv96max>=0)?19+(gTv96max/32):18+(gTv96max/32);
        gHdrPhase=3;
       }
       else if(gHdrPhase!=3)
       {
        n= ((3+((conf.bracket_type-3)*2))-1)/2; 
	       if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override != 19 && conf.tv_override_enable) 
           initialTv96=gTv;
          else 
           {
            if (!(m==MODE_M || m==MODE_TV))        
             initialTv96=gTv; 
            else 
             initialTv96=shooting_get_user_tv96(); 
           }            
         
        gTv96max=initialTv96+(32*conf.tv_bracket_value*n); 
        gTv96min=initialTv96-(32*conf.tv_bracket_value*n);
        if(gTv96max>1056){gTv96max=1056;gTooBright=1;play_sound(6);} 
        else gTooBright=0;
        if(gTv96min<-576){gTv96min=-576;gTooDark=1;play_sound(6);}        
        else gTooDark=0;
        }          
      } 
 

 int focus=lens_get_focus_pos();
 if(focus>0)               								
  {
   onHalfPress.FocusFromSensor = focus;
   onHalfPress.FocusFromLens = lens_get_focus_pos_from_lens();
   onHalfPress.NearPoint=shooting_get_nearpoint(onHalfPress.FocusFromLens);
   int fp = shooting_get_farpoint(onHalfPress.FocusFromLens);
   if(fp>0)onHalfPress.FarPoint= fp;						
   else onHalfPress.FarPoint = -1;							
  }
 else												
  {
   onHalfPress.FocusFromSensor = -1;
   onHalfPress.FocusFromLens = -1;
   onHalfPress.NearPoint=onHalfPress.Hyperfocal;
   onHalfPress.FarPoint= -1;
  }
  
#if defined(CAMERA_a620)
if(state_kbd_script_run && tl.running)
{
 int tv96 = onHalfPress.Bv96*onHalfPress.Sv96/onHalfPress.Av96;
 shooting_set_tv96_direct(tv96, SET_NOW);
}
#endif
 
TenMsecCount();
 asm volatile("LDMFD   SP!, {R0-R12,LR}\n");
}


short shooting_get_tv96_from_shutter_speed(float t)
{
  if (t>0) return (short) (96.0*log(1.0/t)/log_2);  
  else return (-10000);
}

float shooting_get_shutter_speed_from_tv96(short tv)
{
  return  pow(2,(float)((-1)*tv)/96.0 );  
}

short shooting_get_tv96()
{
    short tv96;
    _GetPropertyCase(PROPCASE_CAMERA_TV, &tv96, sizeof(tv96));
    return tv96;
}
int shooting_get_user_tv_id()
{
#if CAM_HAS_USER_TV_MODES 
    short tvv;
    long i;
    _GetPropertyCase(PROPCASE_USER_TV, &tvv, sizeof(tvv)); 
    for (i=0;i<SS_SIZE;i++){
	if (shutter_speeds_table[i].prop_id == tvv)
	    return shutter_speeds_table[i].id;
    }
#endif   
    return 0;
}

int my_shooting_get_tv_override_value()
{
  return ovr[conf.tv_override];
}

short shooting_get_user_tv96()
{
#if CAM_HAS_USER_TV_MODES
    short tv;
    _GetPropertyCase(PROPCASE_USER_TV, &tv, sizeof(tv));
    return tv;
#else 
    return 0;
#endif
    
}

void shooting_set_user_tv_by_id(int v)
{
#if CAM_HAS_USER_TV_MODES 
    long i;
 for (i=0;i<SS_SIZE;i++){
	if (shutter_speeds_table[i].id == v){
	    short vv = shutter_speeds_table[i].prop_id;
	    _SetPropertyCase(PROPCASE_USER_TV, &vv, sizeof(vv));
  		_SetPropertyCase(PROPCASE_CAMERA_TV, &v, sizeof(v));       
	    return;
	 }
 }
#endif 
}

void shooting_set_user_tv96(short v)
{
#if CAM_HAS_USER_TV_MODES
 long i;

 for (i=0;i<SS_SIZE;i++){
  	if (shutter_speeds_table[i].prop_id == v){
  		_SetPropertyCase(PROPCASE_USER_TV, &v, sizeof(v));
  		_SetPropertyCase(PROPCASE_CAMERA_TV, &v, sizeof(v));
	    return;
	  }
  }
#endif 
}

void shooting_set_tv96(short v, short is_now)  
{
 long i;

 for (i=0;i<SS_SIZE;i++){
  	if (shutter_speeds_table[i].prop_id == v){
       shooting_set_tv96_direct(v, is_now);
       return;
     }
  }
}

void shooting_set_tv96_direct(short v, short is_now)
{
 if ((mode_get()&MODE_MASK) != MODE_PLAY)
  {
    if(is_now)
       {
	  _SetPropertyCase(PROPCASE_CAMERA_TV, &v, sizeof(v));
        #ifdef PROPCASE_TV2
        _SetPropertyCase(PROPCASE_TV2, &v, sizeof(v));   
        #endif
       }
         else photo_param_put_off.tv96=v ? v : 1; 
  }
}

void shooting_set_user_tv_by_id_rel(int v)
{
#if CAM_HAS_USER_TV_MODES 
    int cv = shooting_get_user_tv_id();
    shooting_set_user_tv_by_id(cv+v);
#endif  
}

//                               From CHDK code by philmoz

#if !defined(SV96_MARKET_OFFSET)                 
#define SV96_MARKET_OFFSET          69 
#endif

// Conversion values for pow(2,-69/96) 'market' to 'real', and pow(2,69/96) 'real' to 'market'
// Uses integer arithmetic to avoid floating point calculations. Values choses to get as close
// to the desired multiplication factor as possible within normal ISO range.

#define ISO_MARKET_TO_REAL_MULT     9955
#define ISO_MARKET_TO_REAL_SHIFT    14
#define ISO_MARKET_TO_REAL_ROUND    8192
#define ISO_REAL_TO_MARKET_MULT     3371
#define ISO_REAL_TO_MARKET_SHIFT    11
#define ISO_REAL_TO_MARKET_ROUND    1024
 

#define ISO_MARKET_TO_REAL(x)       (((int)x * ISO_MARKET_TO_REAL_MULT + ISO_MARKET_TO_REAL_ROUND) >> ISO_MARKET_TO_REAL_SHIFT)
#define ISO_REAL_TO_MARKET(x)       (((int)x * ISO_REAL_TO_MARKET_MULT + ISO_REAL_TO_MARKET_ROUND) >> ISO_REAL_TO_MARKET_SHIFT)

static short canon_iso_base=0;
static short canon_sv96_base=0;

const  ISOTable *shooting_get_iso_line()
{
    short int isov;
    long i;
    _GetPropertyCase(PROPCASE_ISO, &isov, sizeof(isov));
    for (i=0;i<ISO_SIZE;i++){
	if (iso_table[i].prop_id == isov)
	    return &iso_table[i];
    }
    return 0;
 }

short shooting_get_sv96_from_iso(short iso)
{
    
    if (iso > 0)
        return (short)( log((double)(iso)*32.0/100.0)*96.0*(inv_log_2)+0.5 );
    return 0;
}
 
void iso_init()
{
    
    if (iso_table[1-iso_table[0].id].prop_id == 50)
        canon_iso_base = 50;
    else
        canon_iso_base = CAM_MARKET_ISO_BASE;

    
    
    canon_sv96_base = shooting_get_sv96_from_iso(canon_iso_base);
}
 
 short shooting_iso_market_to_real(short isom)
{
   return ISO_MARKET_TO_REAL(isom);
}

 short shooting_iso_real_to_market(short isom)
{
   return ISO_REAL_TO_MARKET(isom);
}

short shooting_sv96_market_to_real(short sv96)
{
   return sv96 - SV96_MARKET_OFFSET;
}

short shooting_sv96_real_to_market(short sv96)
{
   return sv96 + SV96_MARKET_OFFSET;
}

int shooting_get_iso()
{
    short int isov;
    long i;
    _GetPropertyCase(PROPCASE_ISO, &isov, sizeof(isov));
    for (i=0;i<ISO_SIZE;i++){
	if (iso_table[i].prop_id == isov)
	    return iso_table[i].id;
    }
    return 0;
}

short shooting_get_canon_iso_mode()
{
    short isom;
    _GetPropertyCase(PROPCASE_ISO_MODE, &isom, sizeof(isom));
     return isom;
}

short get_min_iso()
{
 return ISO_MIN_VALUE;
}

short get_max_iso()
{
 return ISO_MAX_VALUE;
}

void shooting_set_iso_mode(int v)
{
 long i;
 for (i=0;i<ISO_SIZE;i++){
	if (iso_table[i].id == v){
	    short vv = iso_table[i].prop_id;
	    _SetPropertyCase(PROPCASE_ISO_MODE, &vv, sizeof(vv));
	    return;
	}
  }
}

short shooting_get_svm96_from_iso(short iso)
{
   if  (iso>0) return (short)(log((double)(iso)*32.0/100.0)*96.0/(log_2));
   return 0;   
}  

short shooting_get_iso_from_sv96(short sv96)
{
    
    return (short)( (double)pow(2, (((double)sv96)/96.0))*100.0/32.0 + 0.5 );
}

short shooting_get_iso_market_base()
{
  if (iso_market_base==0) 
     {
      if (ISO_MIN_VALUE==50) iso_market_base=50;
      else iso_market_base=100;
     }
  return iso_market_base; 
}

short shooting_get_base_sv96()
{
   short dsv,sv;
   if (shooting_get_canon_iso_mode()<50) 
   { 
       _GetPropertyCase(PROPCASE_DELTA_SV, &dsv, sizeof(dsv));
       _GetPropertyCase(PROPCASE_SV, &sv, sizeof(sv));
       sv96_base=(sv-dsv);
   }
   return sv96_base;
}

short shooting_get_iso_real()
{
    short sv;
    _GetPropertyCase(PROPCASE_SV, &sv, sizeof(sv));
    if (sv == 0) { 
     return 0;
    }
    return shooting_get_iso_from_sv96(sv);
}

short shooting_get_svm96_base()
{
	if (svm96_base==0) svm96_base=shooting_get_svm96_from_iso(shooting_get_iso_market_base());
	return svm96_base;
}

short shooting_get_iso_base()         
{
	sv96_base=shooting_get_base_sv96();
	if (sv96_base!=0) return shooting_get_iso_from_sv96(sv96_base);
	else return 0;
}

short shooting_get_svm96()
{
    short  sv;
     _GetPropertyCase(PROPCASE_SV_MARKET, &sv, sizeof(sv));
     return sv;
}

short shooting_get_sv96()
{
    short  sv;
     _GetPropertyCase(PROPCASE_SV, &sv, sizeof(sv));
     return sv;
}

short shooting_get_iso_market()
{

    short iso_mode=shooting_get_canon_iso_mode();
    if (iso_mode < 50)                             
     {
      short iso_b=shooting_get_iso_base();
      if (iso_b) return (short)((shooting_get_iso_market_base()*shooting_get_iso_real())/iso_b);
     }
    return iso_mode;
}

void shooting_set_iso(int v)
{
    long i;

    for (i=0;i<ISO_SIZE;i++){
	if (iso_table[i].id == v){
	    short int vv = iso_table[i].prop_id;
	    _SetPropertyCase(PROPCASE_ISO, &vv, sizeof(vv));
	    return;
	}
    }
}

void shooting_set_sv96(short sv96, short is_now)
{
  if ((mode_get()&MODE_MASK) != MODE_PLAY)
    {
        if (is_now)
        {
            while ((shooting_is_flash_ready()!=1) || (focus_busy)) msleep(10);

            short iso_mode = shooting_get_canon_iso_mode();
            if (iso_mode >= 50)
                shooting_set_iso_mode(0);   

            short dsv96 = sv96 + SV96_MARKET_OFFSET - canon_sv96_base;
            set_property_case(PROPCASE_SV_MARKET, &canon_sv96_base, sizeof(canon_sv96_base));
            set_property_case(PROPCASE_SV,        &sv96, sizeof(sv96));
            set_property_case(PROPCASE_DELTA_SV,  &dsv96, sizeof(dsv96));
        }
        else   
           photo_param_put_off.sv96 = sv96;
    }
}

void shooting_set_iso_real(short iso, short is_now)
{
  if ((mode_get() & MODE_MASK) != MODE_PLAY)
    {
        if (iso)
        {
#ifdef CAM_MIN_ISO_OVERRIDE
            
            if ((iso < ISO_MARKET_TO_REAL(CAM_MIN_ISO_OVERRIDE))) iso = ISO_MARKET_TO_REAL(CAM_MIN_ISO_OVERRIDE);
#endif
            shooting_set_sv96(shooting_get_sv96_from_iso(iso), is_now);
        }
    }
}

void shooting_set_iso_direct(int v)
{
    short int vv = v;
    _SetPropertyCase(PROPCASE_ISO, &vv, sizeof(vv));
}

void shooting_expo_iso_override(void)
{
asm volatile("STMFD   SP!, {R0-R12,LR}\n");
  
    if ((state_kbd_script_run) && (photo_param_put_off.sv96))
    {
        shooting_set_sv96(photo_param_put_off.sv96, SET_NOW);
        
    }

 #if defined(CAM_HAS_ND_FILTER) && defined(CAM_HAS_NATIVE_ND_FILTER)
 
    if ((state_kbd_script_run) && (photo_param_put_off.nd_filter))
    {
        shooting_set_nd_filter_state(photo_param_put_off.nd_filter, SET_NOW);
    }
 
    
    else if (conf.nd_filter_state)
        shooting_set_nd_filter_state(conf.nd_filter_state, SET_NOW);
        
 #endif

    
  disable_preflash();
 asm volatile("LDMFD   SP!, {R0-R12,LR}\n");
}

void shooting_set_flash_override()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n"); 
 int flash,power,flashmode;
  #if defined(CAMERA_m3)
  if(conf.bright_screen || gFlashDisable || (!conf.camera_position && (displayedMode==6)&&!conf.slave_flash))
  {
	  play_sound(6);
     flashmode = 1;  // manual flash
     flash = 1;      // fire flash
     power = 0;      // dim three stops
    _SetPropertyCase(PROPCASE_FLASH_ADJUST_MODE, &flashmode, sizeof(flashmode));                                             
     _SetPropertyCase(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));      
     _SetPropertyCase(PROPCASE_FLASH_MANUAL_OUTPUT, &power, sizeof(power));	  
  }
 #else
 
if(conf.bright_screen || gFlashDisable || (!conf.camera_position && (displayedMode==6)&&!conf.slave_flash))
   {
#if !defined(PROPCASE_FLASH_EXP_COMP)
 
   if(!conf.left_flash_dim) 
   {
    flash = 0;
    power = 0;
    flashmode = 2;  
    _SetPropertyCase(PROPCASE_FLASH_MODE, &flashmode, sizeof(flashmode));	   
    _SetPropertyCase(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));            
    _SetPropertyCase(PROPCASE_FLASH_MANUAL_OUTPUT, &power, sizeof(power)); 
    }
    else                                        
    {
     flashmode = 1;  
     flash = 1;      
     power = 0;      
     _SetPropertyCase(PROPCASE_FLASH_ADJUST_MODE, &flashmode, sizeof(flashmode));  
     _SetPropertyCase(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));          
     _SetPropertyCase(PROPCASE_FLASH_MANUAL_OUTPUT, &power, sizeof(power)); 
    }
#else
 
  if(conf.left_flash_dim &&!conf.bright_screen&&!gFlashDisable && (shooting_get_prop(PROPCASE_FLASH_MODE)<2)) 
    {
     flashmode = 0;   
     power = -288;  
     flash = 1;   
     set_property_case(PROPCASE_FLASH_ADJUST_MODE, &flashmode, sizeof(flashmode));
     set_property_case(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));
     set_property_case(PROPCASE_FLASH_EXP_COMP, &power, sizeof(power));
    }
    else if(shooting_get_prop(PROPCASE_FLASH_MODE)<2)              
    {
      flash = 0;    
      power = 0;
      flashmode = 2;  
      _SetPropertyCase(PROPCASE_FLASH_MODE, &flashmode, sizeof(flashmode));	   
      _SetPropertyCase(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));            
      _SetPropertyCase(PROPCASE_FLASH_MANUAL_OUTPUT, &power, sizeof(power));     
    }
#endif    
    }
   else if ((displayedMode==6)&& conf.slave_flash)
    {
     flash = 1;      
     _SetPropertyCase(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));          
    }
 #endif
 asm volatile("LDMFD   SP!, {R0-R12,LR}\n");
}
 
void get_av(char *buf,int val)
{
 int intValue = shooting_get_aperture_from_av96(val);
 sprintf(buf,"%d.%02d",intValue/100,intValue%100);
}

int shooting_get_av()
{
    short int avv;
    long i;
    _GetPropertyCase(PROPCASE_AV, &avv, sizeof(avv));
    for (i=0;i<AS_SIZE;i++){
	if (aperture_sizes_table[i].prop_id == avv)
	    return aperture_sizes_table[i].id;
    }
    return 0;
}

short shooting_get_aperture_from_av96(short av96) 
{
	if (av96) return (short)((pow(sqrt2, ((double)av96)/96.0))*100.0);
	else return -1;
}

int shooting_get_real_av() 
{
    return (int)(((double)pow(1.4142135623730950488016887242097, ((double)_GetCurrentAvValue())/96.0))*100.0);
}

short shooting_get_real_aperture() 
{
#if defined(CAMERA_ixus700) || defined (CAMERA_ixus800) || defined(ixus70) || defined (CAMERA_ixus850)
 return shooting_get_aperture_from_av96(shooting_get_av96()); 
#else 
    return shooting_get_aperture_from_av96(_GetCurrentAvValue());
#endif
}

short shooting_get_min_real_aperture() 
{
	 short av96;
	_GetPropertyCase(PROPCASE_MIN_AV, &av96, sizeof(av96));
	if (av96) return shooting_get_aperture_from_av96(av96);
	else return shooting_get_real_aperture();
}

short shooting_get_aperture_sizes_table_size()
{
    return AS_SIZE;
}

short shooting_get_av96_override_value()
{
  if (conf.av_override_value<=AS_SIZE)	return (short) aperture_sizes_table[conf.av_override_value-1].prop_id;
  else return (short) (AV96_MAX+32*(conf.av_override_value-AS_SIZE));

}

short shooting_get_av96()
{
    short av;
    _GetPropertyCase(PROPCASE_ALLBEST_AV, &av, sizeof(av));
    return av;
}

int shooting_get_user_av_id()
{
 #if CAM_HAS_IRIS_DIAPHRAGM
    short avv;
    long i;
    _GetPropertyCase(PROPCASE_USER_AV, &avv, sizeof(avv));
    for (i=0;i<AS_SIZE;i++){
			if (aperture_sizes_table[i].prop_id == avv)
	   		 return aperture_sizes_table[i].id;
    }
#endif  
    return 0;
}

short shooting_get_user_av96()
{
#if CAM_HAS_IRIS_DIAPHRAGM
    short av;
    _GetPropertyCase(PROPCASE_AV, &av, sizeof(av));
    return av;
#else     
    return 0;
#endif  
}

void shooting_set_av(int v)
{
#if CAM_HAS_IRIS_DIAPHRAGM
    long i;
    for (i=0;i<AS_SIZE;i++)
    {
	if (aperture_sizes_table[i].id == v)
      {
	    short int vv = aperture_sizes_table[i].prop_id;
	    _SetPropertyCase(PROPCASE_AV, &vv, sizeof(vv));
	    return;
	}
    }
#endif
}

void shooting_set_av_rel(int v)
{
#if CAM_HAS_IRIS_DIAPHRAGM
    int cv = shooting_get_av();
    shooting_set_av(cv+v);
#endif
}

void shooting_set_av96_direct(short v, short is_now)
{  
#if CAM_HAS_IRIS_DIAPHRAGM
if ((mode_get()&MODE_MASK) != MODE_PLAY)
 {
  if (is_now) 
       {
        _SetPropertyCase(PROPCASE_ALLBEST_AV, &v, sizeof(v));

#ifdef PROPCASE_AV2
            set_property_case(PROPCASE_AV2, &v, sizeof(v));   
#endif
        }
        else photo_param_put_off.av96=v;
  }
#endif
}

void shooting_set_av96(short v, short is_now)
{
#if CAM_HAS_IRIS_DIAPHRAGM
if ((mode_get()&MODE_MASK) != MODE_PLAY)
  {
   long i;
    for (i=0;i<AS_SIZE;i++)
     {
	if (aperture_sizes_table[i].prop_id == v)
        {
	   shooting_set_av96_direct(v, is_now);
	   return;
	  }
      }
    }
  #endif
}

void shooting_set_user_av96(short v)
{
#if CAM_HAS_IRIS_DIAPHRAGM
    long i;

 for (i=0;i<AS_SIZE;i++){
	if (aperture_sizes_table[i].prop_id == v){
		  _SetPropertyCase(PROPCASE_USER_AV, &v, sizeof(v));
	
	    return;
	}
 }
#endif
}

#if defined(OPT_ALARIK)
void shooting_set_user_av_by_id(int v)
{
#if CAM_HAS_IRIS_DIAPHRAGM
    long i;
    if ((mode_get()&MODE_MASK) != MODE_PLAY)
    {
        for (i=0;i<AS_SIZE;i++)
        {
            if (aperture_sizes_table[i].id == v)
            {
                short vv = aperture_sizes_table[i].prop_id;
                set_property_case(PROPCASE_USER_AV, &vv, sizeof(vv));
                return;
            }
        }
    }
#endif
}
#endif

void shooting_set_nd_filter_state(short v, short is_now)
{
#if CAM_HAS_ND_FILTER
    if ((mode_get()&MODE_MASK)!=MODE_PLAY)
    {
        if (is_now)
        {
            if (v==1)
                _PutInNdFilter();
            else if (v==2)
                _PutOutNdFilter();
#if defined(CAM_HAS_NATIVE_ND_FILTER) && defined(PROPCASE_ND_FILTER_STATE)//G10,G12,G15,G1x and S110 for example
            int n = (v==1) ? 1 : 0;
            set_property_case(PROPCASE_ND_FILTER_STATE, &n, sizeof(n));
#endif
        }
        else
            photo_param_put_off.nd_filter = v;
    }
#endif
}

void get_ev_correction(char *buf,int val)
{
 if(val==0)sprintf(buf,"0");
 else 
  {
   if((short int)val>0)sprintf(buf,"+");   
   if(!(val%96))sprintf(buf+strlen(buf),"%1d",val/96);
   else sprintf(buf+strlen(buf),"%1d/3",val/32);
  }
}

#if defined(PROPCASE_FLASH_EXP_COMP)
static const char *flash_ev_comp[19] = {"-3", "-2.6", "-2.3", "-2", "-1.6", "-1.3", "-1", "-2/3", "-1/3", "0", "+1/3", "+2/3", "+1", "+1.3", "+1.6", "+2", "+2.3", "+2.6", "+3" };
void get_flash_ev_comp(char *buf,int val)
{
 sprintf(buf,"%s",flash_ev_comp[val]);
}
#endif

static const char *month[12]={"January","February","March","April","May","June","July","August","September","October","November","December"};

void get_date(char *buf,unsigned char format) 
{
  unsigned long tt=time(NULL);                                                           
  struct tm *ttm = localtime(&tt);                                                    
  
  switch (format)
  {
  
   case 0 :
    sprintf(buf,"%4d:%02d:%02d",1900+ttm->tm_year,ttm->tm_mon+1,ttm->tm_mday);  
   break;
   
   case 1 :
    sprintf(buf,"%2d:%02d:%02d",ttm->tm_year-100,ttm->tm_mon+1,ttm->tm_mday);   
   break;
   
   case 2 :
     sprintf(buf,"%02d:%02d:%4d",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year); 
   break;

   case 3:
     sprintf(buf,"%02d:%02d:%2d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year-100);   
   break;
   
   case 4 :
    sprintf(buf,"%02d:%02d:%4d",ttm->tm_mon+1,ttm->tm_mday,1900+ttm->tm_year);   
   break;
   
   case 5 :
    sprintf(buf,"%02d:%02d:%2d",ttm->tm_mon+1,ttm->tm_mday,ttm->tm_year-100);    
   break;
   
   case 6 :                                                                        
    strftime(buf,26,"%Y %A %d ",ttm);
    sprintf(buf+strlen(buf),"%s",month[ttm->tm_mon]);  
   break;
  } 
}

void get_time(char *buf)
{
  unsigned long tt=time(NULL);                                                           
  struct tm *ttm = localtime(&tt);                                                    
  sprintf(buf,"%02d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
}

void mdelay(int d)                            
{
 HPTimer.units=1000;                         
 HPTimer.interval=d;
 HPTimerFinished=0;
 if(start_self_timer())                      
  {
   do {}
   while(!HPTimerFinished);
   HPTimerFinished=0;
  }  
}

void getModeIDs()
{
 int i;
 for (i=0; levent_table[i].name; i++) 
 {
  if (strcmp(levent_table[i].name,     "PressRecButton")   == 0) pressRec    = levent_table[i].id;
  else if (strcmp(levent_table[i].name,"UnpressRecButton") == 0) releaseRec  = levent_table[i].id;
  else if (strcmp(levent_table[i].name,"PressPBButton")    == 0) pressPlay   = levent_table[i].id;
  else if (strcmp(levent_table[i].name,"UnpressPBButton")  == 0) releasePlay = levent_table[i].id;
  else if (strcmp(levent_table[i].name,"PressSw1AndMF")    == 0) manualFocus = levent_table[i].id;
  else if (strcmp(levent_table[i].name,"PressPowerButton")  == 0) pressPower = levent_table[i].id;
 }
}

void send_byte(char b)                        
{
 int i;

 for (i=0;i<8;i++)
	{
        if(!conf.use_af_led)debug_led(1);
        else ubasic_set_led(9,1,100); 
        if (b&1)mdelay(2*conf.bw); 
	  else mdelay(conf.bw);      
        if(!conf.use_af_led)debug_led(0);
	  else ubasic_set_led(9,0,0); 		
	  mdelay(conf.bw);           
        b>>=1;
	}
}

void shooting_set_prop(int id, int v)
{
   short vv = v;
   _SetPropertyCase(id, &vv, sizeof(vv));
   return;
}

int shooting_get_prop_int(int id)	
{
    int vv;
    get_property_case(id, &vv, sizeof(vv));
    return vv;
}

int shooting_get_prop(int id)
{
    short vv;
    _GetPropertyCase(id, &vv, sizeof(vv));
    return vv;

}

int lastObjectNumber()
{
 static int LastFile = 0;
 char name1[32],name2[32];
 char dbuf[10];
 struct stat st;
 int i, fileNum,found=0; 
#if defined(CAM_DATE_FOLDER_NAMING)
 get_target_dir_name(name1);
 strncpy(dbuf,name1+7,8);
 dbuf[8] = '\0';
#else
 sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif 
 fileNum = get_target_file_num();
 for(i=fileNum-2;i<fileNum+2;i++)
  {
   sprintf(name1,"A/DCIM/%s/IMG_%04d.JPG",dbuf,i);
   sprintf(name2,"A/DCIM/%s/MVI_%04d.%s",dbuf,i,MOVIE_TYPE);
   if(stat(name1,&st)==0){found=i;}
   else if(stat(name2,&st)==0){found=i;} 
  }
 if(!found)return LastFile;            
 else
  {
    LastFile = found;
    return found;
  }
}

#ifdef CAM_FILE_COUNTER_IS_VAR
extern long file_counter_var;
long get_file_counter()
{
    return file_counter_var;
}
long get_exposure_counter()
{
    long v = get_file_counter();
    v = ((v>>4)&0x3FFF);
    return v;
}
#else

long get_file_counter()
{
    long v = 0;
    get_parameter_data(param_file_counter, &v, 4);
    return v;
}

long get_exposure_counter()
{
    long v = 0;
    get_parameter_data(param_file_counter, &v, 4);
    v = ((v>>4)&0x3FFF);
    return v;
}
#endif

int shooting_get_day_seconds()
{
    unsigned long t;
    struct tm *ttm;
    t = time(NULL);
    ttm = localtime(&t);
    return ttm->tm_hour * 3600 + ttm->tm_min * 60 + ttm->tm_sec;
}

int shooting_get_tick_count()
{
    return (int)get_tick_count();
}

int shooting_in_progress()
{
    int t = 0;
    _GetPropertyCase(PROPCASE_SHOOTING, &t, 4);
    return t != 0;
}

short shooting_get_drive_mode()
{
 short m,n; 
 if(CAM_PROPSET == 1)
 {
  _GetPropertyCase(PROPCASE_DRIVE_MODE, &m, sizeof(m));
  _GetPropertyCase(219, &n, sizeof(n));
  if((m==2)&&(n==2))return 3; 
  else return m;
 }
else
 {
#if defined (CAM_DRIVE_MODE_FROM_TIMER_MODE)
   short n;
   _GetPropertyCase(PROPCASE_TIMER_MODE, &n, sizeof(n));
   
   
   
   if(n==3){
      return n;
   }
#endif

    _GetPropertyCase(PROPCASE_DRIVE_MODE, &m, sizeof(m));
    return m;
  }
}

short shooting_get_canon_overexposure_value()
{
    short bv = 0;
    _GetPropertyCase(PROPCASE_OVEREXPOSURE, &bv, sizeof(bv));
    return bv;
}

short shooting_get_bv96()
{
    short bv = 0;
    _GetPropertyCase(PROPCASE_BV, &bv, sizeof(bv));
    return bv;
}
 
int shooting_get_luminance()//http://en.wikipedia.org/wiki/APEX_system
{
    short bv = shooting_get_bv96();
    int b=(int)(100*k*pow(2.0,((double)(bv-168)/96.0)));
    return b;
}

int shooting_mode_canon2chdk(int canonmode) {
	int i;
	for (i=0; i < MODESCNT; i++) {
		if (modemap[i].canonmode == canonmode) 
			return modemap[i].hackmode;
	}
	return 0;
}

int shooting_mode_chdk2canon(int hackmode) 
{
 int i;
 for (i=0; i < MODESCNT; i++) 
 {
  if (modemap[i].hackmode == hackmode)
   return modemap[i].canonmode;
 }
 return -1; 
}

int __attribute__((weak)) rec_mode_active(void) 
{
    return (playrec_mode==2 || playrec_mode==4 || playrec_mode==5);
}

int  mode_get(void) 
{
    int mode, t=0xFF;
    mode = (rec_mode_active())?MODE_REC:MODE_PLAY;

#ifdef CAM_SWIVEL_SCREEN
    mode |= (screen_opened())?MODE_SCREEN_OPENED:0;
    mode |= (screen_rotated())?MODE_SCREEN_ROTATED:0;
#endif

    _GetPropertyCase(PROPCASE_SHOOTING_MODE, &t, 4);
	mode |= shooting_mode_canon2chdk(t);
    return (mode);
}

int shooting_set_mode_chdk(int mode) 
{
 int canonmode = shooting_mode_chdk2canon(mode);
 if (canonmode == -1 || !rec_mode_active())
  return 0;
 _SetCurrentCaptureModeType(canonmode);
 
 
 return 1;
}

int shooting_set_mode_canon(int canonmode) 
{
	if(canonmode == -1 || !rec_mode_active())
	 return 0;
	_SetCurrentCaptureModeType(canonmode);
	return 1;
}

void checkKeyPress()
{
 gUpPressed=kbd_is_key_pressed(KEY_UP);
 gDownPressed=kbd_is_key_pressed(KEY_DOWN);
 gLeftPressed=kbd_is_key_pressed(KEY_LEFT);
 gRightPressed=kbd_is_key_pressed(KEY_RIGHT);
 gSetPressed=kbd_is_key_pressed(KEY_SET);
#if defined(CAM_HAS_DISP_BUTTON)
 gDispPressed=kbd_is_key_pressed(KEY_DISPLAY);
#endif
 gHalfPressed=kbd_is_key_pressed(KEY_SHOOT_HALF);
 gFullPressed=kbd_is_key_pressed(KEY_SHOOT_FULL);
#if defined(KEY_POWER)
 gPowerPressed=kbd_is_key_pressed(KEY_POWER);
#endif
#if !defined(CAMERA_m3)
 gZoomPoint = lens_get_zoom_point();
 gFocalPoint = get_focal_length(gZoomPoint);
#endif
}

void platform_kbd_hook()
{
  
  if(!tl.running&&(displayedMode==5) && ((gLowLightCount>1)&&conf.burst_frame_count&&(gLowLightCount>=conf.burst_frame_count)))
  {
  if(state_kbd_script_run)  
   {
    conf.flag_1=0;
    gLowLightCount=0;
   }
  else
   {
    kbd_key_release_all();
   }			 
  }
   
  else if(!tl.running && ((displayedMode==30)||(displayedMode==29)) && state_kbd_script_run && gBracketingCounter && (gBracketingCounter>=conf.strip_images))
  {
   conf.flag_1=0;
 
  }
  
  else if(!tl.running && displayedMode &&((displayedMode<5)||((displayedMode>18)&&(displayedMode<23)))&& conf.flag_1 && state_kbd_script_run && inf_focus)
  {
    conf.flag_1=0; 
    nSW=116;       
  }
  else if(tl.running && conf.flag_1 && (tl.shoot_mode==3) && (tl.shot_count>=tl_target_shot_count)){conf.flag_1=0;}  
  else if(tl.running && conf.flag_1 && (tl.shoot_mode==1) && inf_focus){conf.flag_1=0;}
}

void ManualFocusMode()
{
 _PostLogicalEventToUI(manualFocus,0);
}

void recordMode()
{
  _PostLogicalEventToUI(pressRec,0);
  _PostLogicalEventToUI(releaseRec,0);
}

void playMode()
{
  _PostLogicalEventToUI(pressPlay,0);
  _PostLogicalEventToUI(releasePlay,0);
}

void power_down()
{
  _PostLogicalEventToUI(pressPower,0);
}

int shooting_get_zoom() 
{
#if !defined(CAMERA_m3)
    return gZoomPoint;
#else
return 0;
#endif
}

void shooting_set_zoom(int v) 
{
#if !defined(CAMERA_m3)
 long dist;
 if ((mode_get()&MODE_MASK) != MODE_PLAY)
  {
 
dist = lens_get_focus_pos_from_lens();
   lens_set_zoom_point(v);
#if defined(CAM_NEED_SET_ZOOM_DELAY) 
    msleep(CAM_NEED_SET_ZOOM_DELAY); 
#endif 
   if(conf.dist_mode !=3) shooting_set_focus(dist);
  }
#endif
}

#if CAM_PROPSET > 1
int shooting_get_digital_zoom_mode(void)
{
    int x=shooting_get_prop(PROPCASE_DIGITAL_ZOOM_MODE);
#if CAM_PROPSET == 7
    if(x==1) {
        return 0;
    }
#endif
    return x;
}
#endif

int shooting_get_digital_zoom_state(void)
{
#if CAM_PROPSET == 7
    
    int x=shooting_get_prop(PROPCASE_DIGITAL_ZOOM_MODE);
    if(x==1) {
        return 1;
    }
    return 0;
#else
    return shooting_get_prop(PROPCASE_DIGITAL_ZOOM_STATE);
#endif
}

void shooting_set_zoom_rel(int v) 
{
 int cv = shooting_get_zoom();
 shooting_set_zoom(cv+v);
}

void ptp_sz(int v)                                 
{
 if ((mode_get()&MODE_MASK) != MODE_PLAY)
  {
   lens_set_zoom_point(zpFromIndex(v));
   msleep(800);
   kbd_key_press(KEY_SHOOT_HALF);
   msleep(800);
   kbd_key_release(KEY_SHOOT_HALF);
  }
}
 
int shooting_will_use_flash()
{
 int t = 0;     
 _GetPropertyCase(PROPCASE_WILL_USE_FLASH, &t,sizeof(&t));
 return t;
}

int shooting_is_flash_ready()
{
 int t = 0;
 _GetPropertyCase(PROPCASE_FLASH_MODE, &t, sizeof(&t));
 if ((t != 2) && (shooting_in_progress()))                   
 if (shooting_will_use_flash()) return _IsStrobeChargeCompleted();
 return 1;
}

int shooting_get_flash_status(void)
{
   int v = _GetPropertyCase(PROPCASE_FLASH_MODE,&v,sizeof(v)); 
   return v;
}
void shooting_video_bitrate_change(int v)
{
 int m[]={1,2,3,4,5,6,7,8,10,12};
 if (v>=(sizeof(m)/sizeof(m[0]))) v=(sizeof(m)/sizeof(m[0]))-1;
#if CAM_CHDK_HAS_EXT_VIDEO_MENU
 change_video_tables(m[v],4);
#endif
}

#if CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO
void unlock_optical_zoom(void)
{
 if (conf.unlock_optical_zoom_for_video) _UnsetZoomForMovie();
}
#endif

int get_movie_status()
{
#ifndef CAM_SIMPLE_MOVIE_STATUS
 return movie_status;
#else
 // firmware movie_status interpreted as: zero - not recording, nonzero - recording
#if !defined(g7x)
 return simple_movie_status?VIDEO_RECORD_IN_PROGRESS:VIDEO_RECORD_STOPPED;
 #else
 return simple_movie_status>2?VIDEO_RECORD_IN_PROGRESS:VIDEO_RECORD_STOPPED;	 
 #endif
#endif
}

int is_video_recording()
{
#if !defined(CAM_SIMPLE_MOVIE_STATUS)
#if defined(CAMERA_m3)
 return (movie_status>1);
#elif defined(CAM_HAS_MOVIE_DIGEST_MODE)
    // 'movie_status' values
    //      0 - after startup
    //      1 - movie recording stopped
    //      4 - movie recording in progress, or in 'movie digest' scene mode
    //      5 - movie recording stopping
    //      6 - in video mode, not recording 
    int vmode;
    get_property_case(PROPCASE_SHOOTING_MODE, &vmode, 4); 
    vmode=shooting_mode_canon2chdk(vmode);
    return (((movie_status == VIDEO_RECORD_IN_PROGRESS) && (vmode != MODE_VIDEO_MOVIE_DIGEST)));
#else
    // 'movie_status' values
    //      0 - after startup
    //      1 - movie recording stopped
    //      4 - movie recording in progress
    //      5 - movie recording stopping
    return (movie_status > 1);
#endif
#else
    return (simple_movie_status!=0);
#endif
}


void ZoomFocalLengths()
{
#if !defined(CAMERA_m3)
 int fd=-1,i;
 char fn[32];
 sprintf(fn,"A/SDM/TEXTS/focal.txt");
 fd=open(fn,O_WRONLY|O_CREAT, 0777);
 if(fd>=0)
  {
   sprintf(fn,"35mm-equivalent focal-length table for %s\n\n",PLATFORM);
   write(fd,fn,strlen(fn));
   for(i=0;i<zoom_points;i++)
    {
     if(i!=(zoom_points-1))
      sprintf(fn,"%d,",(get_effective_focal_length(i))/1000);
     else
      sprintf(fn,"%d\n\n",(get_effective_focal_length(i))/1000);
     write(fd,fn,strlen(fn));
    }
    
    sprintf(fn,"Real focal-lengths table for %s\n\n",PLATFORM);
    write(fd,fn,strlen(fn));
   int fl;
   for(i=0;i<zoom_points;i++)
    {
     fl = get_focal_length(i);
     if(i!=(zoom_points-1))
      sprintf(fn,"%d.%1d,",(fl+50)/1000,(((fl+50)%1000))/100);
     else
      sprintf(fn,"%d.%1d\n\n",(fl+50)/1000,(((fl+50)%1000))/100);
      write(fd,fn,strlen(fn));   
    }
   close(fd);   
   }
#endif
}

int get_step_for_real_fl(int fl)
{
#if !defined(CAMERA_m3)
 int i,focal;
 if((fl<get_focal_length(0))||(fl>get_focal_length(zoom_points-1)))
  {
   play_sound(6);
   return -1;
  }
 if(fl==get_focal_length(0))return 0;
 if(fl==get_focal_length(zoom_points-1))return (zoom_points-1);
 for(i=1;i<(zoom_points-1);i++)
  {
   focal=get_focal_length(i);
   if(focal==fl) return i;                                               
   if(focal>fl)     
   {
    if((abs(focal-fl)) < (abs(fl-get_focal_length(i-1))))               
     return i;
    else return i-1;                                                    
   }
  }
 return -1;
#else
 return 0;
#endif
}

short shooting_can_focus()
{
#if !defined(CAMERA_m3)
    if(MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK)) return 1;           
    if((mode_get() & MODE_MASK) == MODE_PLAY) return 0 ;                 
    
#ifdef CAM_SD_OVER_IN_AF 
#ifdef PROPCASE_CONTINUOUS_AF
    if (shooting_get_prop(PROPCASE_CONTINUOUS_AF)) return 0;   
#endif
#ifdef PROPCASE_SERVO_AF
    if (shooting_get_prop(PROPCASE_SERVO_AF)) return 0;        
#endif
    if ((shooting_get_prop(PROPCASE_AF_LOCK)==0)           
         && (shooting_get_prop(PROPCASE_FOCUS_MODE)==0 )) return 1;
#endif 
  
#ifdef CAM_SD_OVER_IN_AFL
    if (shooting_get_prop(PROPCASE_AF_LOCK)==1 ) return 1;     
#endif
#ifdef CAM_SD_OVER_IN_MF
    if (shooting_get_prop(PROPCASE_FOCUS_MODE)==1 ) return 1;  
#endif
    return 0;
#else
return 1;
//----------------------------------------------------------
#endif
}

int shooting_get_focus() 
{
    return lens_get_focus_pos();
}

void shooting_set_focus(int v) 
{
}

short shooting_get_focus_mode()
{
    short m;
    _GetPropertyCase(PROPCASE_FOCUS_MODE, &m, sizeof(m));
    return m;
}

void ubasic_shooting_set_focus(int v, short is_now) 
{
 int s=v;

 if ((mode_get()&MODE_MASK) != MODE_PLAY)
  {
	if (is_now) 
       {
        if((s>=0) && s<CAMERA_MIN_DIST)s=CAMERA_MIN_DIST;
        if((s<0)||(s>CAMERA_MAX_DIST))s=CAMERA_MAX_DIST;
        if(!shooting_can_focus())
        {
         if(!enable_focus_override())
          photo_param_put_off.subj_dist=v;
         else lens_set_focus_pos(s);
        }
        else lens_set_focus_pos(s);
       }
      else
       photo_param_put_off.subj_dist=v;
   }
}

 
void allbest_shooting_set_focus(int v) 
{
int s = v;

      if (shooting_can_focus())
       {
	   if ((s>0)) lens_set_focus_pos(s); 
	   else 
          {
           int near=shooting_get_near_limit_from_subj_dist(s);
           if (near>0) lens_set_focus_pos(near); 
	    }
       }
}

int allbest_shooting_get_focus() 
{
	return shooting_get_subject_distance();
}

int shooting_get_near_limit_from_subj_dist(int s)
{
      int h = onHalfPress.Hyperfocal;
      int m = h*s;
      int v = h+s;
      if ((m>0) && (v>0)) return (m/v);
      else return (-1);
}

int shooting_get_lens_to_focal_plane_width() 
 { 
  return (int)(lens_get_focus_pos()-lens_get_focus_pos_from_lens()); 
 } 

 
int set_focus_range()                                                      
{
 if(shooting_can_focus())
  {
   static int PointCount,prevNear,prevFar;
   gCurrentFocus=lens_get_focus_pos_from_lens();                                  
   if(gCurrentFocus==-1)gCurrentFocus=MAX_DIST;
   if(gCurrentFocus<MIN_DIST)gCurrentFocus=MIN_DIST;
   int NextFocus;
   int tmp;
   static int count,lrcount;
 
   count++;
 
   if(!PointCount&&(count&31))                                             
    {                                                    
     if(kbd_is_key_clicked(KEY_LEFT))
      {
       play_sound(4);
	 if(gCurrentFocus==MAX_DIST)NextFocus=onHalfPress.Hyperfocal;
       else NextFocus=shooting_get_nearpoint(gCurrentFocus);                     
       if(NextFocus<MIN_DIST)NextFocus=MIN_DIST;
       if((NextFocus<gCurrentFocus))                       
        {
         lens_set_focus_pos(NextFocus+fpd);                                
         gCurrentFocus=NextFocus;
        } 
        return 1;      
      }
     else if(kbd_is_key_clicked(KEY_RIGHT))
      {
       play_sound(4);
       if(gCurrentFocus==MAX_DIST)return 1;
       NextFocus=shooting_get_farpoint(gCurrentFocus);                      
       if((NextFocus>MAX_DIST)||(NextFocus<0))NextFocus=MAX_DIST;
       if((NextFocus>gCurrentFocus)&&(NextFocus>MIN_DIST)) 
        {
         if(NextFocus==MAX_DIST)lens_set_focus_pos(INFINITY_DIST);
         else lens_set_focus_pos(NextFocus+fpd);					   
         gCurrentFocus=NextFocus;
        }
       else if(NextFocus==gCurrentFocus)
        {
         lens_set_focus_pos(NextFocus+fpd+1);					  
         gCurrentFocus=NextFocus+1;
        }
        msleep(100);
        if((lens_get_focus_pos_from_lens()>MAX_DIST)||(lens_get_focus_pos_from_lens()<0))gCurrentFocus=MAX_DIST;
         return 1;
      }
      
      else if(kbd_is_key_clicked(KEY_MENU))
      {
       play_sound(4);
       PointCount=2; 
       return 1;
      }
     else if(kbd_is_key_clicked(KEY_SET))
      {
       play_sound(4);
       gNearPoint=gCurrentFocus;
       PointCount=1;                                                       
       return 1; 
      }
     return 1;                                   
    }
   else if(PointCount==1)                                                                   
    { 
     if(kbd_is_key_clicked(KEY_LEFT)) 
      {
       play_sound(4);
       lrcount++;
       if(gCurrentFocus==MIN_DIST)return 1;
       NextFocus=shooting_get_nearpoint(gCurrentFocus);                     
       if((NextFocus<gCurrentFocus)&&(NextFocus>=MIN_DIST))                       
        {
         lens_set_focus_pos(NextFocus+fpd);                                
         gCurrentFocus=NextFocus;
        }
       else gCurrentFocus=MIN_DIST;     
      return 1;
      }             
     else if(kbd_is_key_clicked(KEY_RIGHT))
      {
       play_sound(4);
       lrcount++;
       if(gCurrentFocus==MAX_DIST)return 1;
       NextFocus=shooting_get_farpoint(gCurrentFocus);                      
       if((NextFocus>gCurrentFocus)&&(NextFocus<MAX_DIST)&&(NextFocus>MIN_DIST)) 
        {
         lens_set_focus_pos(NextFocus+fpd);                               
         gCurrentFocus=NextFocus;
        }
       else if((NextFocus<0)||(NextFocus>=MAX_DIST))
        {
         lens_set_focus_pos(INFINITY_DIST);
         gCurrentFocus=MAX_DIST;
        }
       else if(NextFocus==gCurrentFocus)
        {
         lens_set_focus_pos(NextFocus+fpd+1);                           
         gCurrentFocus=NextFocus+1;
        }
        msleep(300);
        if((lens_get_focus_pos_from_lens()>MAX_DIST)||(lens_get_focus_pos_from_lens()<0))gCurrentFocus=MAX_DIST;
       return 1;
      }
     else if(kbd_is_key_clicked(KEY_SET))
      {
       play_sound(4);
       gFarPoint=lens_get_focus_pos_from_lens();                     
       if(gFarPoint==-1)gFarPoint=MAX_DIST;
       if(gFarPoint!=gNearPoint)lrcount=0;                           
       else gFarPoint=MAX_DIST;								                  
         if(gFarPoint<gNearPoint)                                         
          {
           tmp=gFarPoint;
           gFarPoint=gNearPoint;
           gNearPoint=tmp;
          }
         if(gNearPoint<MIN_DIST)gNearPoint=MIN_DIST;				  
         if(gFarPoint>MAX_DIST)gFarPoint=MAX_DIST;
         conf.user_1=gNearPoint;
         conf.user_2=gFarPoint;
         gStereoShift = (int)((((float)get_sensor_width()/1000/conf.stereo_deviation/((float)(gFocalPoint)/1000))*gNearPoint*gFarPoint)/(gFarPoint-gNearPoint));
         conf.user_3=gStereoShift;
         ubasic_set_variable(25,gStereoShift);
         PointCount=0;
         count=0;
	   gCurrentFocus=gNearPoint;
         lens_set_focus_pos(gNearPoint+fpd);						  
         return 0;
     }                                                           
      return 1;
   }    
  else return 1;                                           
 }      
 play_sound(6);           
 return 0;
}

int shooting_get_subject_distance()
{
   
    	int h, v, m;
    static float coc;
     coc=(float)(get_sensor_width()/1200000);
      int fl=gFocalPoint; 
    	int near=lens_get_focus_pos();
        int v1=(fl*fl);
      	int av_min=shooting_get_min_real_aperture();
    
         if ((av_min!=0) && (v1))  
   
        {
		  h=v1/(coc*av_min);
    	        if ((near>0) && (near<MAX_DIST)) 
               {
    		     v=(h-near+fl);
    		     m=h*near;
    		     if ((v>0) && (m>0)) return m/v;  
       	   }
        }
       	return (-1);
}

int shooting_get_farpoint(int distance)  
{
 int hyp = onHalfPress.Hyperfocal;
 if(!fpd || (distance>=MAX_DIST))return -1;  
 unsigned int num = (hyp*1000)/(hyp-distance); 
 return (((num/10)*distance)/100);
} 

int shooting_get_nearpoint(int distance)
{
 int hyp = onHalfPress.Hyperfocal; 
 unsigned int num = (hyp*1000)/(hyp+distance); 
 return (((num/10)*distance)/100);
} 

int shooting_get_hyperfocal_distance()
{
 unsigned int flx1000 = get_focal_length(lens_get_zoom_point());  
 unsigned int num = ((flx1000/(get_sensor_width()/100))*((flx1000*12)/shooting_get_real_av()));
 num=num/10 +(flx1000/1000);
return num;
}

  
void shooting_set_mf()
{
 DoMFLock();
}

int next_focus(int currentFocus)								    
{
 float coc=(float)(get_sensor_width())/1200000;
 float realfl=(float)(gFocalPoint)/1000;
 float realav=(float)(myav)/100;
 long denom = (long)(coc*(currentFocus-realfl)-((realfl/realav)*realfl));         
 int cfar = (int)(-((realfl/realav)*realfl*currentFocus)/denom);                  
 denom = (long)(coc*cfar-((realfl*realfl)/realav));                                                
 long num = (long)(coc*cfar*realfl-((cfar*realfl*realfl)/realav));
 return(short)(num/denom);
}

int prev_focus(int currentFocus)								    
{
 float coc=(float)(get_sensor_width())/1200000;
 float realfl=(float)(gFocalPoint)/1000;
 float realav=(float)(myav)/100;                       
 long denom=(long)((currentFocus*coc*realav)+(realfl*realfl));
 long num = (long)(currentFocus*(realfl*realfl-(coc*realav*realfl)));
 return (short)(num/denom);
}
 
int shooting_focus_out()
{
 int NextFocus;
 unsigned char atInf;
 atInf=(lens_get_focus_pos_from_lens()<0);
 
  if((onHalfPress.FocusFromLens>MAX_DIST)||atInf)onHalfPress.FocusFromLens=MAX_DIST; 
  if(onHalfPress.FocusFromLens<MIN_DIST)onHalfPress.FocusFromLens=MIN_DIST;
  if(onHalfPress.FocusFromLens==MAX_DIST){conf.user_4=onHalfPress.FocusFromLens;return 1;}
  NextFocus=shooting_get_farpoint(onHalfPress.FocusFromLens); 
  if((NextFocus>MAX_DIST)||(NextFocus<0))NextFocus=MAX_DIST;
  if((NextFocus>onHalfPress.FocusFromLens)&&(NextFocus>MIN_DIST)) 
   {
    if(NextFocus>=MAX_DIST)lens_set_focus_pos(INFINITY_DIST);
    else lens_set_focus_pos(NextFocus+fpd); 
    onHalfPress.FocusFromLens=NextFocus;
   }
  else if(NextFocus==onHalfPress.FocusFromLens) 
   {
    lens_set_focus_pos(NextFocus+fpd+1);	 
    onHalfPress.FocusFromLens=NextFocus+1;
   }
   conf.user_4=onHalfPress.FocusFromLens;
 if(conf.focus_override_method==1)  photo_param_put_off.subj_dist=onHalfPress.FocusFromLens+fpd; 
  return 1;
 }
 
int shooting_focus_in()
{ 
 int NextFocus;
if((onHalfPress.FocusFromLens>=MAX_DIST)||(lens_get_focus_pos_from_lens()<0)){onHalfPress.FocusFromLens=MAX_DIST;NextFocus=onHalfPress.Hyperfocal;}
 else NextFocus=shooting_get_nearpoint(onHalfPress.FocusFromLens); 
 if((NextFocus<onHalfPress.FocusFromLens)) 
  {
   lens_set_focus_pos(NextFocus+fpd);  
   onHalfPress.FocusFromLens=NextFocus;
  } 
  else if(NextFocus) 
  {
   lens_set_focus_pos(NextFocus+fpd-1); 
   onHalfPress.FocusFromLens=NextFocus-1;
  }
  conf.user_4=onHalfPress.FocusFromLens;
 if(conf.focus_override_method==1)photo_param_put_off.subj_dist=onHalfPress.FocusFromLens+fpd; 
 return 1;  
}

int nearForInfinity()                                         
{
 if((conf.camera_orientation==1)||(conf.camera_orientation==3))
  return ((conf.stereo_spacing*gFocalPoint*conf.stereo_deviation* 4)/(get_sensor_width()*3));
 else
  return ((conf.stereo_spacing*gFocalPoint*conf.stereo_deviation)/get_sensor_width());
}

int farForMaxDeviation()
{
 long fact2;
 long fact1 = (conf.stereo_spacing*gFocalPoint)/1000;
 if((conf.camera_orientation==1)||(conf.camera_orientation==3))
  fact2 = ((get_sensor_width()*conf.rangefinder_near*3)/(4*conf.stereo_deviation))/1000;
 else
  fact2 = ((get_sensor_width()*conf.rangefinder_near)/conf.stereo_deviation)/1000;
 int far = (int)((fact1*conf.rangefinder_near)/(fact1-fact2)); 
 if((far>MAX_DIST) ||(far <0))far = MAX_DIST;
 return far;
}

void shooting_tv_bracketing()
{
 static short value;
 int m=mode_get()&MODE_SHOOTING_MASK;
 static char fn[64];
 static char mybuf[80];
 static struct tm *ttm;
 unsigned long t;
 static int imgNum;
 if (bracketing.shoot_counter==0) 
   {  
    if(get_target_file_num()==imgNum)++imgNum;
    else imgNum=get_target_file_num();  
     inf_focus = 0;
     if (fdtv>=0)close(fdtv);  
    if(conf.save_auto &&(!tl.running ||(tl.running && tl.ss))) 
     {
      sprintf(fn, "A/SDM/STACKS/tv_stack_%d.txt",imgNum);
      fdtv = open(fn, O_WRONLY|O_CREAT, 0777);  
     }
    sprintf(mybuf, "A/DCIM/HDR_BATCH_%04d.bat",imgNum);    
    fdhdr = open(mybuf, O_WRONLY|O_CREAT|O_APPEND, 0777);  
    bracketing.shoot_counter=1; 

    if(tl.running && tl.shoot_mode==1 && tl.bkt_exposures>=3) 
     bracketing.tv96=shooting_get_tv96();
   #if !defined(CAMERA_m3)
   else if((gHdrPhase==3)&&displayedMode && (displayedMode<=4)&&(gZoomPoint!=(zoom_points-1))) 
#else
   else if((gHdrPhase==3)&&displayedMode && (displayedMode<=4))
#endif 
    {bracketing.tv96=gTv96max;}  
    else
    {
#if defined(CAMERA_tx1)
    bracketing.tv96=shooting_get_tv96();
#else
    if (!(m==MODE_M || m==MODE_TV)) bracketing.tv96=shooting_get_tv96(); 
        else 
          {
	    if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override != 19 && conf.tv_override_enable) bracketing.tv96=shooting_get_tv96();
else bracketing.tv96=shooting_get_user_tv96();  
         }
#endif
       }

    bracketing.tv96_step=32*conf.tv_bracket_value;
   if(conf.save_auto &&(!tl.running ||(tl.running && tl.ss))) 
    {        
     if (fdtv>=0)                              
     {
      sprintf(mybuf,"%-50s\n","Tv (shutter speed) bracketing");
      write(fdtv,mybuf,strlen(mybuf));
      write(fdtv,"\n",1);
      sprintf(mybuf,"%-50s\n",(shooting_get_drive_mode()==3)?"Custom timer mode":"Continuous shooting mode");
      write(fdtv,mybuf,strlen(mybuf));
      write(fdtv,"\n",1);
      t = time(NULL);
      ttm = localtime(&t);
      sprintf(mybuf, "%-27s%2u:%02u:%02u\n","Time", ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
      write(fdtv,mybuf,strlen(mybuf));
      write(fdtv,"\n",1);
      if(gHdrPhase==3)
       {
        sprintf(mybuf,"%-12s%12d\n","Bv96 Bright",hdrLight96);
        write(fdtv,mybuf,strlen(mybuf));
        sprintf(mybuf,"%-12s%12d\n\n","Bv96 Dark",hdrDark96);
        write(fdtv,mybuf,strlen(mybuf));  
        sprintf(mybuf,"%-12s%12d\n\n","Bv96 Range",abs(hdrDark96)+hdrLight96);
        write(fdtv,mybuf,strlen(mybuf));         
       }
      sprintf(mybuf,"%-12s%12s\n","Tv Value","Shutter speed");
      write(fdtv,mybuf,strlen(mybuf));
      write(fdtv,"\n",1);
      sprintf(mybuf,"%-12d%12s\n",bracketing.tv96,shooting_get_camera_tv_string());
      write(fdtv,mybuf,strlen(mybuf));
     }
    }
    if (fdhdr>=0)
    {
     char name[32];
     char dbuf[10];
    #if defined(CAM_DATE_FOLDER_NAMING)
     get_target_dir_name(name);
     strncpy(dbuf,name+7,8);
     dbuf[8] = '\0';
#else
     sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif 
     sprintf(mybuf,"\nenfuse -o HDR_%04d.TIF %s/IMG_%04d.jpg ",imgNum,dbuf,imgNum);
     write(fdhdr,mybuf,strlen(mybuf));
    }   
   } 

   else
   {
    imgNum++;
     if(fdtv>=0)
      {
       sprintf(mybuf,"%-12d%12s\n",value,shooting_get_camera_tv_string());
       write(fdtv,mybuf,strlen(mybuf));
      }
     if (fdhdr>=0)
      {
       char name[32];
       char dbuf[10];
      #if defined(CAM_DATE_FOLDER_NAMING)
       get_target_dir_name(name);
       strncpy(dbuf,name+7,8);
       dbuf[8] = '\0';
      #else
       sprintf(dbuf,"%03dCANON",get_target_dir_num());
      #endif 
      sprintf(mybuf,"%s/IMG_%04d.jpg ",dbuf,imgNum);
      write(fdhdr,mybuf,strlen(mybuf));
      }         
   }
 
      bracketing.shoot_counter++;   
      bracketing.dtv96+=bracketing.tv96_step;                      
      if((conf.bracket_type == 0)||(displayedMode &&(displayedMode<5)&&!tl.running))value = bracketing.tv96-bracketing.dtv96;         
      else if(conf.bracket_type == 1) value = bracketing.tv96+bracketing.dtv96;   
      else                                                                        
        {
         if(bracketing.shoot_counter%2)value = bracketing.tv96 - bracketing.tv96_step*(bracketing.shoot_counter/2);
         else value = bracketing.tv96 + bracketing.tv96_step*(bracketing.shoot_counter/2);
	     }

        if((!conf.fastlapse && (bracketing.shoot_counter>3+((conf.bracket_type-3)*2)))||!shooting_in_progress()) {inf_focus = 1; }  
        if(value>1053)value=1053;                                       
        else if (value<-576)value=-576;                                
        shooting_set_tv96_direct(value, SET_NOW);

        if(!conf.fastlapse &&(bracketing.shoot_counter>3+((conf.bracket_type-3)*2)) && (shooting_get_drive_mode()!=3))
        { 
         if(fdtv>=0)close(fdtv); 
 
         stack_saved = 1;
        }

        else if(!shooting_in_progress()) 
        {
         if(fdtv>=0)close(fdtv);          
         stack_saved = 1;
        }
}

int shooting_get_subject_distance_bracket_value()
{
  return conf.subj_dist_bracket_value;
}


void shooting_subject_distance_auto_bracketing()
{
 static int previous_focus,target,maxDist;
 static char fn[64];
 static char osd_buf[32];
 static char mybuf[80];
 static struct tm *ttm;
 unsigned long t;                                                      
 int np;                                              
 long num,denom;
 maxDist=(int)CAMERA_MAX_DIST;
 
 

 if (bracketing.shoot_counter==0)                                      
 {                                                                    
   if (fdauto>=0)close(fdauto);                                        
   inf_focus = 0;

   if(conf.save_auto &&(!tl.running ||(tl.running && tl.ss))) 
   {
    sprintf(fn, "A/SDM/STACKS/af_stack_%d.txt",lastObjectNumber()+1);
    fdauto = open(fn, O_WRONLY|O_CREAT, 0777);                         
   }
    bracketing.shoot_counter=1;
      bracketing.dsubj_dist = onHalfPress.FocusFromLens;
     gInitialFocus=previous_focus=gNearPoint;
     target = gFarPoint;
  np=onHalfPress.FarPoint;                                              
 if(np>0)
 {
  num=np*onHalfPress.Hyperfocal-(2*onHalfPress.FocalLength*np/1000);
  denom=onHalfPress.Hyperfocal-(onHalfPress.FocalLength/1000)-np;
  bracketing.dsubj_dist = num/denom;	                                 
 }  
 else bracketing.dsubj_dist = maxDist-fpd;
  if(conf.save_auto &&(!tl.running ||(tl.running && tl.ss))) 
  {        
  if (fdauto>=0)                                                       
    { 
     if((tl.bracket_type==1)|| !tl.running) sprintf(mybuf,"%-50s\n","Auto focus bracketing");
     else sprintf(mybuf,"%-50s\n","Auto focus bracketing with Tv bracketing");
     write(fdauto,mybuf,strlen(mybuf));
     write(fdauto,"\n",1);
     sprintf(mybuf,"%-50s\n",(shooting_get_drive_mode()==1)?"Continuous shooting mode":"Custom timer mode");
     write(fdauto,mybuf,strlen(mybuf));
     write(fdauto,"\n",1);
     t = time(NULL);
     ttm = localtime(&t);
     sprintf(mybuf, "%-27s%2u:%02u:%02u\n","Time", ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
     write(fdauto,mybuf,strlen(mybuf));
     write(fdauto,"\n",1);
     sprintf(mybuf,"%-20s%2d.%1d\n","Focal length",onHalfPress.FocalLength/1000,(onHalfPress.FocalLength%1000)/100);
     write(fdauto,mybuf,strlen(mybuf));
     sprintf(mybuf,"%-21s%d.%1d\n","Aperture",myav/100,(myav%100)/10);
     write(fdauto,mybuf,strlen(mybuf));
    if(tl.bracket_type==2)
     {
      sprintf(mybuf,"%-12s%12s\n","Tv bracket",shooting_get_camera_tv_string());
      write(fdauto,mybuf,strlen(mybuf));
     }
     write(fdauto,"\n",1);
     sprintf(mybuf,"  Near  Focus    Far\n");
     write(fdauto,mybuf,strlen(mybuf));
     sprintf(mybuf,"%6d %6d %6d\n",gNearPoint,gInitialFocus,np);    
     write(fdauto,mybuf,strlen(mybuf));
     sprintf(mybuf,"%6d %6d %6d\n",shooting_get_nearpoint(bracketing.dsubj_dist),bracketing.dsubj_dist,shooting_get_farpoint(bracketing.dsubj_dist));    
     write(fdauto,mybuf,strlen(mybuf));
    }
   }
    if(tl.running)
    {
    sprintf(osd_buf,"%15d",gInitialFocus);
    draw_string(conf.synch_pos.x+10*FONT_WIDTH,conf.synch_pos.y,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
    } 
    if((bracketing.dsubj_dist>0)&&(bracketing.dsubj_dist<=target))
    {
     if((bracketing.dsubj_dist-previous_focus)<1)bracketing.dsubj_dist+=1;  
    }
    else {bracketing.dsubj_dist=target-fpd;}    
    lens_set_focus_pos(bracketing.dsubj_dist+fpd);    
  }   
 
  else 
  {
   if(fdtv>=0)
      {
       sprintf(mybuf,"%-12d%12s\n",value,shooting_get_camera_tv_string());
       write(fdtv,mybuf,strlen(mybuf));
      }
      
  if(inf_focus)                                                                                 
  {
   play_sound(4);    
   if (fdauto>=0) 
    {
     close(fdauto);
     stack_saved = 1;
    }
  }  
   else                                                                                            
 {                            
  if(tl.running)
   {
    sprintf(osd_buf,"%15d",bracketing.dsubj_dist);                                     
    draw_string(conf.synch_pos.x+10*FONT_WIDTH,conf.synch_pos.y,osd_buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);                                                
   }
  else if((displayedMode==8)&&((conf.camera_orientation!=1)&&(conf.camera_orientation!=3)))
   {
    sprintf(osd_buf,"NEAR/FAR FOCUS Stack  %5d mm",bracketing.dsubj_dist);
    draw_string(conf.synch_pos.x-5*FONT_WIDTH,conf.synch_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1); 
    sprintf(osd_buf,"Near %5d Far %5d Shift %2d ",gNearPoint,gFarPoint,gStereoShift);
    draw_string(conf.synch_pos.x-5*FONT_WIDTH,conf.synch_pos.y+FONT_HEIGHT,osd_buf,MAKE_COLOR(COLOR_BG,COLOR_WHITE),1);
 }
    np=shooting_get_farpoint(bracketing.dsubj_dist);
  num=np*onHalfPress.Hyperfocal-(2*onHalfPress.FocalLength*np/1000);
  denom=onHalfPress.Hyperfocal-(onHalfPress.FocalLength/1000)-np;
  bracketing.dsubj_dist=num/denom;                                                                 
  if((bracketing.dsubj_dist-previous_focus)<1)bracketing.dsubj_dist+=1;                            
  if((shooting_get_farpoint(bracketing.dsubj_dist)>0) && (bracketing.dsubj_dist>0))sprintf(mybuf,"%6d %6d %6d\n",shooting_get_nearpoint(bracketing.dsubj_dist),bracketing.dsubj_dist,shooting_get_farpoint(bracketing.dsubj_dist));                        
  else if ((shooting_get_farpoint(bracketing.dsubj_dist)<0) && (bracketing.dsubj_dist>0))sprintf(mybuf,"%6d %6d    Inf\n",shooting_get_nearpoint(bracketing.dsubj_dist),bracketing.dsubj_dist);
  else if (bracketing.dsubj_dist<0)sprintf( mybuf,"%6d    Inf    Inf\n",onHalfPress.Hyperfocal);
  write(fdauto,mybuf,strlen(mybuf));
  if((bracketing.dsubj_dist>target)||(bracketing.dsubj_dist<0)||(np<0))                            
  {
   play_sound(4);
   if((tl.sun & USE_NEAR_FAR)==0)
    bracketing.dsubj_dist=target-fpd;
   inf_focus =1;                                                                              
   }
    previous_focus=bracketing.dsubj_dist;
    lens_set_focus_pos(bracketing.dsubj_dist+fpd); 
   ++bracketing.shoot_counter;
  }         
 }
}

void single_shot_log()
{
 static char fn[64];
 static char mybuf[80];
 static struct tm *ttm;
 unsigned long t;                                               
 if(tl.shot_count==1)                                                 
 {
  if(fdsingle>=0) close(fdsingle);
  sprintf(fn, "A/SDM/STACKS/tl_stack_%d.txt",get_target_file_num());
  fdsingle = open(fn, O_WRONLY|O_CREAT, 0777);                               
  if (fdsingle>=0)                                                           
   {
    sprintf(mybuf,"%-50s\n","Single-shot time-lapse sequence");
    write(fdsingle,mybuf,strlen(mybuf));
    write(fdsingle,"\n",1);
    sprintf(mybuf,"%4s %6s %-8s\n","Shot","Image","  Time");
    write(fdsingle,mybuf,strlen(mybuf));
    write(fdsingle,"\n",1);
   }
  }

   if(fdsingle>=0)
   {
    t = time(NULL);
    ttm = localtime(&t);
    sprintf(mybuf,"%4d  %4d  %2u:%02u:%02u\n",tl.shot_count,get_target_file_num(),ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
    write(fdsingle,mybuf,strlen(mybuf));
    write(fdsingle,"\n",1); 
   }   
}

#if OPT_SUNSET
 
 void shooting_sunrise()
{
 static int tvr;
 static char fn[64];
 static char mybuf[80];
 static struct tm *ttm;
 static int savecsv=0;
 static int tvchange;                                                                               

 unsigned long t;                                                                              
 int m=mode_get()&MODE_SHOOTING_MASK;
  if (!bracketing.shoot_counter)                                                               
  { 
#if defined(CAMERA_tx1)
    bracketing.tv96=shooting_get_tv96();
#else
    if (!(m==MODE_M || m==MODE_TV)) bracketing.tv96=shooting_get_tv96();                        
        else                                                                                    
          {
	    if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override != 19 && conf.tv_override_enable) bracketing.tv96=shooting_get_tv96();
          else bracketing.tv96=shooting_get_user_tv96();                                        
	    }
#endif
        close(fdsun);
       if(tl.running && tl.sun && tl.suncsv)savecsv=1;

       if(savecsv)sprintf(fn, "A/SDM/STACKS/tl_stack_%d.csv",get_target_file_num());
       else sprintf(fn, "A/SDM/STACKS/tl_stack_%d.txt",get_target_file_num());
       fdsun = open(fn, O_WRONLY|O_CREAT, 0777);                                                  
       if (fdsun>=0)                                                                              
       {
         if(!savecsv)
         {
          sprintf(mybuf,"%-50s\n","Sunrise/Sunset time-lapse sequence");
          write(fdsun,mybuf,strlen(mybuf));
          write(fdsun,"\n",1);
          sprintf(mybuf,"%4s %5s %4s %4s %6s %-8s\n","Shot","Image","Tv96","dTv","Tv","  Time");
          write(fdsun,mybuf,strlen(mybuf));
          write(fdsun,"\n",1);
          t = time(NULL);
          ttm = localtime(&t);
          sprintf(mybuf,"%4d %5d %4d %4d %6s %2u:%02u:%02u\n",bracketing.shoot_counter+1,get_target_file_num(),bracketing.tv96+bracketing.dtv96,0,shooting_get_camera_tv_string(),ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
          write(fdsun,mybuf,strlen(mybuf));
         }
        else                                                                                  
         {
          sprintf(mybuf,"%-50s\n","Single-shot time-lapse sequence");
          write(fdsun,mybuf,strlen(mybuf));
          write(fdsun,"\n",1);
          sprintf(mybuf,"%s,%s,%s,%s,%s\n","Shot","Image","Bv","Luminance","Time(HH:MM:SS)");
          write(fdsun,mybuf,strlen(mybuf));
         }
       }

    if(conf.bracket_type>1) conf.bracket_type=0;                                                
    if(conf.tv_override_enable) conf.tv_override_enable=0;                                      
    tvr=conf.sunrise_repeats;
    inf_focus = 0;
    srf =0;
    tvchange=conf.sunrise_tv96;                                                                  
  }  

    bracketing.shoot_counter++;                                                                  
                                                                 
 if(bracketing.shoot_counter<conf.sunrise_kf_3)                                                  
  {
    if(bracketing.shoot_counter== conf.sunrise_kf_1) tvchange  = conf.sunrise_tv96_2;          
    else if(bracketing.shoot_counter== conf.sunrise_kf_2) tvchange = conf.sunrise_tv96_3;     
    if(!conf.sunrise_repeats) bracketing.dtv96+=tvchange;                                        
    else
    {
     if(--tvr<0)                                                                                 
      {
        tvr=conf.sunrise_repeats;
        bracketing.dtv96+=tvchange;                                                              
      }
    }                                                          

    if(!conf.bracket_type)                                                                       
    {
      if((bracketing.tv96-bracketing.dtv96)<-576)bracketing.dtv96=-576+bracketing.tv96;          
      value=bracketing.tv96-bracketing.dtv96;
      shooting_set_tv96_direct(bracketing.tv96-bracketing.dtv96, SET_NOW);   
    }
    else                                                                                          
    {
     if((bracketing.tv96+bracketing.dtv96)>1181)bracketing.dtv96=1181-bracketing.tv96;           
     value=bracketing.tv96+bracketing.dtv96;
     shooting_set_tv96_direct(bracketing.tv96+bracketing.dtv96, SET_NOW);                   
    }
  } 

 else tvchange=0;

 if(fdsun>=0)
   {
    if(!savecsv)
    {
     t = time(NULL);
     ttm = localtime(&t);
     sprintf(mybuf,"%4d %5d %4d %4d %6s %2u:%02u:%02u\n",bracketing.shoot_counter+1,get_target_file_num()+1,value,tvchange,shooting_get_camera_tv_string(),ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
     write(fdsun,mybuf,strlen(mybuf));
    }
   else
    {
     t = time(NULL);
     ttm = localtime(&t);
     sprintf(mybuf,"%3d,%4d,%5d,%7d,%2u,%02u,%02u\n",tl.shot_count,get_target_file_num(),sb,shooting_get_luminance(),ttm->tm_hour, ttm->tm_min,ttm->tm_sec);
     write(fdsun,mybuf,strlen(mybuf));
    }
   }

    if(bracketing.shoot_counter==(conf.sunrise_shots-1)||(conf.sunrise_shots<2))
     {
       inf_focus=1;                                                                              
       srf = 1; 
      if(fdsun>=0) close(fdsun);                                                                       
     }
}
#endif
int get_inf_focus()                                
{ 
 if(shooting_get_drive_mode()==1)return shutter_int;
 else return !shooting_in_progress();
}

void shooting_bracketing(void)
{
  if ((shooting_get_drive_mode()!=0) && !(tl.running && (tl.shoot_mode==3)))  
   {
     int m=mode_get()&MODE_SHOOTING_MASK;
     if (m!=MODE_STITCH) 
       {
       if (state_shooting_progress != SHOOTING_PROGRESS_PROCESSING) 
          { 
           bracketing.shoot_counter=0;
           bracketing.tv96=0;
           bracketing.dtv96=0;
           bracketing.subj_dist=0;
           bracketing.dsubj_dist=0;
           bracketing.type=0;
          }
          if (conf.tv_bracket_value)  shooting_tv_bracketing();
          else
#if OPT_SUNSET
          if(conf.sunrise) shooting_sunrise();
          else
#endif
          if(conf.dist_mode==2) shooting_subject_distance_auto_bracketing(); 
       } 

   } 
  
      else if (!tl.shoot_mode && tl.running && conf.save_auto) single_shot_log();
#if OPT_SUNSET
      else if (!tl.shoot_mode && tl.running && conf.save_auto && tl.sun)
      {
       if(tl.shot_count==1)
       {
         bracketing.shoot_counter=0;
         bracketing.tv96=0;
         bracketing.dtv96=0;
       }
       shooting_sunrise();         
      }
#endif
}

