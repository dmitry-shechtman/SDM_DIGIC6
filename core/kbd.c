#include "stdlib.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "conf.h"
#include "lang.h"
#include "ubasic.h"
#include "script.h"
#include "gui_lang.h"
#include "motion_detector.h"
#include "math.h"
#include "draw_palette.h"

#if defined(CAMERA_a1000)
#define RECORD 0x1001        
#define PLAYBACK 0x1003      
#define MOVIE 0x105B         

#elif !CAM_HAS_PLAYBACK_PB 
#define RECORD 0x1065
#define PLAYBACK 0x1061
#define MOVIE 0x105B

#elif CAM_HAS_PLAYBACK_PB 
#define RECORD 0x1001
#define PLAYBACK 0x1003
#define MOVIE 0x9A5
#endif

static unsigned char SyncRecMovMode;  
unsigned int sinceBoot;
unsigned int UsbPulses;
int keyid_by_name(const char *n);
static int inCsmMode(int action);
#define SCRIPT_SLEEP            3
#define SCRIPT_PRESS            4
#define SCRIPT_RELEASE          5
#define SCRIPT_PR_WAIT_SAVE     6
#define SCRIPT_WAIT_SAVE        7
#define SCRIPT_WAIT_FLASH       8
#define SCRIPT_WAIT_EXPHIST     9
#define SCRIPT_PR_WAIT_EXPHIST  10
#define SCRIPT_WAIT_CLICK       11
#define SCRIPT_MOTION_DETECTOR  12
#define SCRIPT_BRACKET_WAIT     13
#define SCRIPT_WFSP             14
#define SCRIPT_TL               15
#define SCRIPT_USBUP            17
#define SCRIPT_END_PLE          18
#define REFRESH_USB             19
#define RF                      20 
#define SCRIPT_SU               23 
#define SCRIPT_BAFLF            24 
#define SCRIPT_PLAYBACK         26
#define SCRIPT_RECORD           27
#define SCRIPT_MOVIE            28
#define AF_LOCK                 32
#define SCRIPT_FAST_FINISH      34
#define SCRIPT_COVER_LENS       35
#define FILL_BV_BUFFER          36
#define SCRIPT_CSM              37
#define SCRIPT_SET_FOCUS_RANGE  38
#define SCRIPT_PM               39
#define SCRIPT_FOCUSED          40
#define SCRIPT_VZIO             41
#define SCRIPT_BURST_FINISH     43
#define FAST_KAP                44
#define RESTORE_CAPTURE_MODE    45
#define KBD_STACK_SIZE         100
 
extern short gAp96;
extern PHOTO_PARAM photo_param_put_off; 
extern struct CMenu script3_submenu; 
extern volatile enum Gui_Mode gui_mode;
extern onHP onHalfPress;
extern int fdLog,fdKapLog,gTv96max,gUsbHigh,bls,gBracketingCounter,gZoomPoint;               
extern int gTv,gFastTv,gAv,gBv,gMaxIso96,gNDfactor,checkLensCap,avBv,gHdrPhase,gTv96min,hdrDark96,hdrLight96;
extern int gFrameCount,gFramesRecorded,gLowLightCount,gSlitfd;
extern int movie_status;
extern int gScriptMovie;
extern int ttyMasterSlave;
extern int afl,ael,has_refocused,af_all;
extern int playback;
static long kbd_int_stack[KBD_STACK_SIZE];
int kbd_int_stack_ptr;
extern int gDone_once;
extern int fdtv;                   
extern int fdauto;
extern int fddist;
extern int fdsingle;
extern int fdsun;
extern int gFlashDisable;
extern int put_off_ev96;
extern int inf_focus;
extern unsigned char gStartHour,gEndHour,gStartMinutes,gEndMinutes,displayedMode,exitWelcome,mini_started,startup_script;
extern int gSDMShootMode;
extern unsigned char gScriptFolder,gFastExit,gExAlt,gConsoleScroll,ConfRestoreFinished;
extern unsigned int recStart;
unsigned char gStopVideoCapture=FALSE;            
volatile enum _VideoMode VideoMode = VIDEO_NULL;
time_t gScriptStart;                

#define KBD_STACK_PUSH(v) kbd_int_stack[kbd_int_stack_ptr++] = (v);
#define KBD_STACK_PREV(p) (kbd_int_stack[kbd_int_stack_ptr-(p)])
 
char *kaplogbuf=NULL;
static int CurrentCaptureMode=0; 
static int BvIndex;
unsigned int gVideoStartTime,gVideoStopTime;
long lastClickedKey;
int computedBv;
int gNumOfBvReadings;
int rawBv[BVBUFFSIZE];         
static int bufcnt = 0;         
static short apertures[15]= {160,176,208,243,275,304,336,368,400,432,464,480,496,512,544 };
static int BvPrev = -1000;
static int checkBv=5000;       
char normal_script[32];
static char osdbuf[32];        
static int soft_half_press = 0;
static unsigned char anyISO;
unsigned char gRawMode,gVideoReady,ZoomIn=1,welcomeDone=0,forceNDout,gSyncZoomStep,clearScreen;
short kapbv96;
int fastShoot,fastRetVarNum,gCurrentNDFilterState,gCurrentNDFilterConstantExposure,isoset,tvset,avset,fastTv; 
int gPTPsession; 
int gSZMode;              
int kbd_blocked;
int key_pressed,skey_pressed,ezkey_pressed;
unsigned short gMenuPressed;
static struct tm *ttm;
int state_kbd_script_run = 0;
int stack_saved = 0;
int cf;                        
int rrm;                       
int su_first;                  
static int afcnt;              
unsigned long gDetectUsb;      
int gSwitchType = STANDARD; 
static long delay_target_ticks;
static long kbd_last_clicked;
 
static int mvideo,mplay;
int nSW=0;                      
static int nRmt=0;              
static int nWt=0;               
static int nCount=0;            
int nCa=0;                      
static int nCount2=0;           
static int nTxvideo=0;          
int nTxzoom=0;                  
static int nPlyname=KEY_LEFT;   
static int nIntzpos;            
static int nReczoom=0;          
static int nTxtblcr=0;          
static int nConffin=0;
static int burstShutdown=0;
long gEnableScripting;          
int shutter_int=0;              
int gShotsRemaining=0; 
int init_start = 1;
static int tl_first;            
static int log_status;          
int bracket_running,tl_mod;

#if defined(CAMERA_a450) || defined(CAMERA_a460)
 #define ZOOM_IN KEY_UP
 #define ZOOM_OUT KEY_DOWN
#elif defined(CAMERA_sx200is)||defined(CAMERA_sx240hs)||defined(CAMERA_sx260hs)||defined(CAMERA_sx280hs)
#define ZOOM_IN KEY_ZOOM_IN
#define ZOOM_OUT KEY_ZOOM_OUT
#define ZOOM_IN_SLOW KEY_ZOOM_IN_SLOW
#define ZOOM_OUT_SLOW KEY_ZOOM_OUT_SLOW
#else
 #define ZOOM_IN KEY_ZOOM_IN
 #define ZOOM_OUT KEY_ZOOM_OUT
#endif

#if !defined(CAMERA_sx200is)&& !defined(CAMERA_sx240hs)&& !defined(CAMERA_sx260hs)&& !defined(CAMERA_sx280hs)
static int nTxzname=ZOOM_IN; 
#else
static int nTxzname=ZOOM_IN_SLOW; 
#endif
 
static int nFirst=0;
 

#if defined(CAMERA_a450)|| defined(CAMERA_a470)|| defined(CAMERA_a480)|| defined(CAMERA_a490)|| defined(CAMERA_a495)|| defined (CAMERA_a800)||defined(CAMERA_ixus40) || defined(CAMERA_ixus50) || defined(CAMERA_ixus55)|| defined(CAMERA_ixus60) || defined(CAMERA_ixus65) || defined(CAMERA_ixus70) || defined(CAMERA_ixus80)|| defined(CAMERA_ixus85)|| defined(CAMERA_ixus95) || defined(CAMERA_ixus700) || defined(CAMERA_ixus750) || defined(CAMERA_ixus850) || defined(CAMERA_ixus75)||defined(CAMERA_ixus100)
#define ZSTEP_TABLE_SIZE 7
static int nTxtbl[]={0,1,2,3,4,5,6};
#endif

#if defined(CAMERA_s2is) || defined(CAMERA_s3is) || defined(CAMERA_s5is)|| defined(CAMERA_sx1)|| defined(CAMERA_sx10)|| defined(CAMERA_sx20)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,11,25,41,64,86,105,128};
#endif

#if defined(CAMERA_tx1)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,18,43,55,76,93,113,124};
#endif

#if defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,30,60,90,120,150,180,200};
#endif

#if defined(CAMERA_a460) || defined(CAMERA_a530) || defined (CAMERA_a540) || defined(CAMERA_a550) || defined(CAMERA_a560) || defined(CAMERA_a570) || defined(CAMERA_a2200) || defined(CAMERA_ixus860)|| defined(CAMERA_ixus960) || defined(CAMERA_ixus980)|| defined(CAMERA_a590)|| defined(CAMERA_ixus90)|| defined(CAMERA_ixus130)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,1,2,3,4,5,6,7};
#endif

#if defined(CAMERA_a610) ||  defined(CAMERA_a620) || defined(CAMERA_a630) || defined(CAMERA_a640) || defined(CAMERA_ixus800) || defined(CAMERA_ixus950)
#define ZSTEP_TABLE_SIZE 9
static int nTxtbl[]={0,1,2,3,4,5,6,7,8};
#endif

#if defined (CAMERA_a700) || defined(CAMERA_a710) || defined (CAMERA_a720) || defined (CAMERA_a1000)|| defined (CAMERA_a2000)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,2,4,6,8,10,12,14};
#endif

#if defined(CAMERA_a650) || defined(CAMERA_g7)|| defined(CAMERA_g9)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,2,4,6,8,10,12,13};
#endif

#if defined(CAMERA_ixus970)  
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,2,4,6,8,9,10,11};
#endif

#if defined(CAMERA_a810)|| defined(CAMERA_ixus220)|| defined(CAMERA_a2300)|| defined(CAMERA_a2400)|| defined(CAMERA_a2500)|| defined(CAMERA_a3400)
#define ZSTEP_TABLE_SIZE 10
static int nTxtbl[]={0,6,12,18,25,32,39,47,55,63};
#endif

#if defined(CAMERA_s90)|| defined(CAMERA_s95)
#define ZSTEP_TABLE_SIZE 10
static int nTxtbl[]={0,1,2,3,4,5,6,7,8,9};
#endif
 

#if defined(CAMERA_s100)|| defined(CAMERA_g15)|| defined(CAMERA_s110)
#define ZSTEP_TABLE_SIZE 7
static int nTxtbl[]={0,16,32,62,78,102,120};
#endif

#if defined(CAMERA_sx240hs)||defined(CAMERA_sx260hs)||defined(CAMERA_sx280hs)||defined(CAMERA_g1x)||defined(CAMERA_g7x)||defined(CAMERA_ixus132)||defined(CAMERA_ixus140)||defined(CAMERA_ixus160_elph160)||defined(CAMERA_m3)
#define ZSTEP_TABLE_SIZE 9
static int nTxtbl[]={0,16,24,32,50,62,78,90,100};
#endif


#if defined(CAMERA_sx100is)|| defined(CAMERA_sx110is) || defined(CAMERA_sx120is)
#define ZSTEP_TABLE_SIZE 8
static int nTxtbl[]={0,3,6,9,13,16,20,22};
#endif

#if defined(CAMERA_sx200is) ||defined(CAMERA_sx210is) || defined(CAMERA_sx220hs) || defined(CAMERA_sx230hs)
#define ZSTEP_TABLE_SIZE 7
static int nTxtbl[]={0,16,32,62,78,102,125};
#endif

#if defined(CAMERA_sx130is)||defined(CAMERA_sx150is)
#define ZSTEP_TABLE_SIZE 9
static int nTxtbl[]={0,16,32,48,64,80,96,112,127};
#endif

#if defined(CAMERA_a4000)||defined(CAMERA_sx160is)||defined(CAMERA_sx170is)
#define ZSTEP_TABLE_SIZE 9
static int nTxtbl[]={0,16,32,48,64,80,96,112,126};
#endif

#if defined(CAMERA_a3200)
#define ZSTEP_TABLE_SIZE 12
static int nTxtbl[]={0,1,2,3,4,5,6,7,8,9,10,11};
#endif

#if defined(CAMERA_g10)||defined(CAMERA_g11)||defined(CAMERA_g12) 
#define ZSTEP_TABLE_SIZE 14
static int nTxtbl[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13};
#endif

#if defined(CAMERA_ixus870)||defined(CAMERA_ixus105)||defined(CAMERA_ixus115)
#define ZSTEP_TABLE_SIZE 10
static int nTxtbl[]={0,1,2,3,4,5,6,7,8,9};
#endif

int TxTblIndex(int zp)
{
 int i=0;
 if(zp>nTxtbl[ZSTEP_TABLE_SIZE-1])return ZSTEP_TABLE_SIZE-1;
 while((zp>nTxtbl[i])&&(i<ZSTEP_TABLE_SIZE))i++;
 if((zp==nTxtbl[i])||(i==ZSTEP_TABLE_SIZE-1))return i;
 if(abs(nTxtbl[i-1]-zp)<abs(nTxtbl[i]-zp))return i-1;
 else return i;
}

int zpFromIndex(int index)
{
 return nTxtbl[index];
}

int get_soft_half_press()
{
 return soft_half_press;
}

short int vv;
void presynch(void);
int time_lapse();

static void kbd_sched_wfsp()
{
    KBD_STACK_PUSH(SCRIPT_WFSP);
}
 
static void kbd_sched_tl()
{
    KBD_STACK_PUSH(SCRIPT_TL);
}

void kbd_sched_delay(long msec)
{
    KBD_STACK_PUSH(msec);
    KBD_STACK_PUSH(SCRIPT_SLEEP);
}

void kbd_sched_motion_detector(){
    KBD_STACK_PUSH(SCRIPT_MOTION_DETECTOR);
}

void kbd_sched_press(long key)
{
//WARNING stack program flow is reversed
    kbd_sched_delay(20);

    KBD_STACK_PUSH(key);
    KBD_STACK_PUSH(SCRIPT_PRESS);
}

void kbd_sched_release(long key)
{
//WARNING stack program flow is reversed
    kbd_sched_delay(20);

    KBD_STACK_PUSH(key);
    KBD_STACK_PUSH(SCRIPT_RELEASE);
}

void md_kbd_sched_immediate_shoot(int no_release)
{
    kbd_int_stack_ptr-=1;//REMOVE MD ITEM
  
    //stack operations are reversed!
    if (!no_release)  //only release shutter if allowed
    {
      kbd_sched_release(KEY_SHOOT_FULL);
      kbd_sched_delay(20);
    }
    KBD_STACK_PUSH(SCRIPT_MOTION_DETECTOR); //it will removed right after exit from this function
    kbd_key_press(KEY_SHOOT_FULL); //not a stack operation... pressing right now
}

static void kbd_sched_click(long key)
{
//WARNING stack program flow is reversed
    kbd_sched_release(key);
    kbd_sched_delay(80);
    kbd_sched_press(key);
}

static void kbd_sched_wait_click(int timeout)
{
//WARNING stack program flow is reversed
    KBD_STACK_PUSH(timeout);
    KBD_STACK_PUSH(SCRIPT_WAIT_CLICK);
}

void kbd_sched_shoot()
{
//WARNING stack program flow is reversed

    KBD_STACK_PUSH(SCRIPT_WAIT_SAVE);

    KBD_STACK_PUSH(KEY_SHOOT_FULL);
    KBD_STACK_PUSH(SCRIPT_RELEASE);

    kbd_sched_delay(20);

    KBD_STACK_PUSH(KEY_SHOOT_FULL);
    KBD_STACK_PUSH(SCRIPT_PRESS);

    kbd_sched_delay(70);

    KBD_STACK_PUSH(KEY_SHOOT_HALF);
    KBD_STACK_PUSH(SCRIPT_PRESS);
    KBD_STACK_PUSH(SCRIPT_PR_WAIT_SAVE);

}

void lua_script_exec( char *script )
{
}

void script_start()
{
    int i;
    stack_saved = 0;
    if(!auto_started && !mini_started)normal_script[0]=0; 
    state_kbd_script_run = 1;
    delay_target_ticks = 0;
    ft_time.hr=99;              
    kbd_int_stack_ptr = 0;
    kbd_last_clicked = 0;
    conf.flag_1=conf.flag_2=conf.flag_3=conf.flag_4=0;
    conf.user_1=conf.user_2=conf.user_3=conf.user_4=0;
    conf.user_5=conf.user_6=conf.user_7=conf.user_8=0;     
    kbd_key_release_all();
    ubasic_init(state_ubasic_script);
   if(gPTPsession) conf.synch_enable=0;          
       for (i=0; i<SCRIPT_NUM_PARAMS; ++i) 
      {
       if(!gScriptFolder)
        ubasic_set_variable(i, conf.ubasic_vars[i]);   
       else if(gScriptFolder==1)
        ubasic_set_variable(i, conf.ubasic_vars2[i]);   
       else if(!mini_started && !startup_script)
        ubasic_set_variable(i, conf.ubasic_vars3[i]);   
      }
   conf_update_prevent_shutdown();
   kaplogbuf = (char*)malloc(1000); 		
   kaplogbuf[0]='\0';
   gStartHour=gEndHour=gStartMinutes=gEndMinutes=0;
   gScriptStart=time(NULL);
   getUsbDuration();                                  
}

void script_end()
{
    ubasic_end();                                                 
    if(tl.running) enable_shutdown();
    enable_shutdown();
    init_start = 1;
    conf.sdm_console=0;
    tl.running=0;
    kbd_key_release_all();
    state_kbd_script_run = 0;
     if(!((displayedMode &&(displayedMode<5))||((displayedMode>=9)&& (displayedMode<=12))||((displayedMode>=16)&& (displayedMode<=18))||((displayedMode>=31)&& (displayedMode<=33)))) 
     {
      if(normal_script[0]==0) conf.tv_override_enable=0; 
     }
     conf.user_range_set=0;
     if(!gPTPsession)
     {
      if(normal_script[0]){script_load(normal_script,0);normal_script[0]=0;} 
      script_console_clear();                                     
     }
    gDone_once=0;                                                 
    gScriptMovie=0;
    gConsoleScroll=0;
    mini_started=startup_script=0;
    conf.flag_1=conf.flag_2=conf.flag_3=conf.flag_4=0;                                         
    has_refocused=0;
    if(fdLog>=0) {close(fdLog);fdLog=-1;}                                    
    if(gSlitfd>=0){close(gSlitfd);gSlitfd=-1;}
    if(kaplogbuf[0])
     {
      free(kaplogbuf);
      kaplogbuf=NULL;
     }
    conf_update_prevent_shutdown();
    
    if(!conf.sleep && !bls)							    
    {
     TurnOnDisplay();
     bls=1;                                                     
    }
    
    script_console_clear();
    vid_bitmap_refresh();
    if(gExAlt)
    {
     gExAlt=0;
     exit_alt();
    }
}

void calcShoot96()
{
           int a,avmin,Bv;
           short ecComp;
           unsigned short userPreferredAv,userMinimumAv;
           char *p = get_systemString_address();
           ecComp=fast_params.exp_comp;
           if(ecComp>6)ecComp=6;                              
           if(ecComp<-6)ecComp=-6;
           fast_params.exp_comp=ecComp;
           tvset=gFastTv;                                     
           if(avBv==0)Bv=onHalfPress.Bv96;                                       
           else Bv = computedBv;             
           kapbv96=Bv-(ecComp*32)+fast_params.ev_step;     
           if(!gMaxIso96)gMaxIso96=shooting_get_sv96_from_iso(shooting_iso_market_to_real(400));
 
 #if defined(CAM_HAS_IRIS_DIAPHRAGM)
  #if defined(CAM_HAS_ND_FILTER)                       
           kap.nd=2;                                   
           a = fast_params.pref_ap;
           if(a<1)a=8;
           if(a>15)a=15;
           --a;
           userPreferredAv=apertures[a];
           
           a = fast_params.min_ap;
           if(a<1)a=12;
           if(a>15)a=15;
           --a;
           userMinimumAv=apertures[a];                                   
           avmin = shooting_get_prop(PROPCASE_MIN_AV);                  
           avset = userPreferredAv;                          
         
          if(avset<avmin)avset=avmin;
        
           isoset = gFastTv+avset-kapbv96;                      
 
           
           if(isoset>gMaxIso96)                   
            {
             isoset=gMaxIso96;
             avset = isoset+kapbv96-tvset;        
 
             if(avset<avmin)                      
              {
               avset=avmin;
               tvset=isoset+kapbv96-avset;        
              }
            }
                     
           else if(isoset<411)                              
            {
             isoset=411;                                    
             tvset=isoset+kapbv96-avset;              
             if(tvset>gFastTv)                     
             {
              tvset=gFastTv;
              avset = kapbv96 +isoset - tvset;        
              if(avset>userMinimumAv)     
               {
                avset=userMinimumAv;
                tvset=isoset+kapbv96-avset;          
               }               
             }
            } 
           if(tvset>gFastTv)
            {
             kap.nd=1;                             
             kapbv96-=ND_FACTOR;
             isoset = gFastTv+avset-kapbv96;                      
 
           
           if(isoset>gMaxIso96)                   
            {
             isoset=gMaxIso96;
             avset = isoset+kapbv96-tvset;        
 
             if(avset<avmin)                      
              {
               avset=avmin;
               tvset=isoset+kapbv96-avset;        
              }
            }
              
           else if(isoset<411)                              
            {
             isoset=411;                                    
             tvset=isoset+kapbv96-avset;              
             if(tvset>gFastTv)                     
             {
              tvset=gFastTv;
              avset = kapbv96 +isoset - tvset;        
              if(avset>userMinimumAv)     
               {
                avset=userMinimumAv;
                tvset=isoset+kapbv96-avset;          
               }               
             }
            }                      
            }           
          
          kap.Tv96=tvset;
          kap.Av96=avset;
          kap.Sv96=isoset;
          kap.Bv96=kapbv96;
          get_tv(p,tvset);                            
          shooting_set_av96_direct(avset, SET_NOW);
          shooting_set_tv96_direct(tvset,SET_NOW);
          shooting_set_sv96(isoset,SET_NOW);
          shooting_set_nd_filter_state(kap.nd, SET_NOW); 
          if(fastRetVarNum>=0)ubasic_set_variable(fastRetVarNum,shooting_get_iso_from_sv96(isoset)); 
          return; 
 #else

           kap.nd=0;   
           a = fast_params.pref_ap;
           if(a<1)a=1;
           if(a>15)a=15;
           --a;
           userPreferredAv=apertures[a];
           
           a = fast_params.min_ap;
           if(a<1)a=1;
           if(a>15)a=15;
           --a;
           userMinimumAv=apertures[a];                                   
           avmin = shooting_get_prop(PROPCASE_MIN_AV);                  
           avset = userPreferredAv;                          
         
          if(avset<avmin)avset=avmin;                          
        
           isoset = gFastTv+avset-kapbv96;                      
           
           if(isoset>gMaxIso96)                   
            {
             isoset=gMaxIso96;
             avset = isoset+kapbv96-tvset;        
 
             if(avset<avmin)                      
              {
               avset=avmin;
               tvset=isoset+kapbv96-avset;        
              }
            }
                      
           else if(isoset<411)                              
            {
             isoset=411;                                    
             tvset=isoset+kapbv96-avset;              
             if(tvset>gFastTv)                     
             {
              tvset=gFastTv;
              avset = kapbv96 +isoset - tvset;        
              if(avset>userMinimumAv)     
               {
                avset=userMinimumAv;
                tvset=isoset+kapbv96-avset;          
               }               
             }
            }  
          
          kap.Tv96=tvset;
          kap.Av96=avset;
          kap.Sv96=isoset;
          kap.Bv96=kapbv96;
          get_tv(p,tvset);                            
          shooting_set_av96_direct(avset, SET_NOW);
          shooting_set_tv96_direct(tvset,SET_NOW);
          shooting_set_sv96(isoset,SET_NOW); 
          if(fastRetVarNum>=0)ubasic_set_variable(fastRetVarNum,shooting_get_iso_from_sv96(isoset));
          return; 
 #endif
 
 
 
#else
 #if defined(CAM_HAS_ND_FILTER)                       
 
           kap.nd=2;                                   
           avset = shooting_get_prop(PROPCASE_MIN_AV);                  
           tvset=gFastTv;
           isoset = gFastTv+avset-kapbv96;                      
           if(isoset>gMaxIso96)
            {
             isoset=gMaxIso96;
             tvset=isoset+kapbv96-avset;                    

            }
           else if(isoset<411)                              
            {
             isoset=411;
             tvset=isoset+kapbv96-avset;                        
            }  

          if(tvset>gFastTv) 
           { 
            kap.nd=1;                                         
            isoset = gFastTv+avset-kapbv96-ND_FACTOR;            
            if(isoset>gMaxIso96)
             {
              isoset=gMaxIso96;             
              tvset=isoset+kapbv96-ND_FACTOR-avset;                    
             }
            else if(isoset<411)                              
             {
              isoset=411;
              tvset=isoset+kapbv96-ND_FACTOR-avset;                        
             }  
           }
          kap.Tv96=tvset;
          kap.Av96=avset;
          kap.Sv96=isoset;
          kap.Bv96=kapbv96;
          get_tv(p,tvset);                            
          shooting_set_tv96_direct(tvset,SET_NOW);
          shooting_set_sv96(isoset,SET_NOW);
          shooting_set_nd_filter_state(kap.nd, SET_NOW); 
          if(fastRetVarNum>=0)ubasic_set_variable(fastRetVarNum,shooting_get_iso_from_sv96(isoset));	         
          return;
           
 #else    
 
           kap.nd=0;  
           avset = shooting_get_prop(PROPCASE_MIN_AV);       
           tvset=gFastTv;
           
           isoset = gFastTv+avset-kapbv96;                      
           if(isoset>gMaxIso96)
            {
             isoset=gMaxIso96;
             tvset=isoset+kapbv96-avset;                    
            }
           else if(isoset<411)                              
            {
             isoset=411;
             tvset=isoset+kapbv96-avset;                        
            }  

          kap.Tv96=tvset;
          kap.Av96=avset;
          kap.Sv96=isoset;
          kap.Bv96=kapbv96;
          get_tv(p,tvset);                            
          shooting_set_tv96_direct(tvset,SET_NOW);
          shooting_set_sv96(isoset,SET_NOW);
          if(fastRetVarNum>=0)ubasic_set_variable(fastRetVarNum,shooting_get_iso_from_sv96(isoset));
          return;          
 #endif
#endif     
}

static unsigned char unlock;

void process_script()
{
 static int entry; 
 static unsigned int tr = 0; 
    long t;
    int j;

    if (kbd_int_stack_ptr)
  {
	switch (KBD_STACK_PREV(1))
   {
      case FILL_BV_BUFFER:
           rawBv[bufcnt]=gBv;
           if(++bufcnt>gNumOfBvReadings-1)
            {
             sortBv();                                  
             averageBv(50,80);                          
             kbd_int_stack_ptr-=1;                      
             bufcnt = 0;
            }
           else {kbd_int_stack_ptr+=11;play_sound(4);}   
           return;
 
      case SCRIPT_COVER_LENS:                           
          play_sound(4);
          if(!entry)                                    
           {
            if((gBv<(BvPrev-288))||(gBv<0))             
             {
              debug_led(1);
              entry = 1;                                
             }
              BvPrev = gBv;                		  
              kbd_int_stack_ptr+=9;                     
           }
          else                                          
           {
            if(gBv>0)                                   
             {
              debug_led(0);
              entry=0;
              checkLensCap=0;					              
              afcnt=shooting_get_tick_count()+(5*1000); 
              kbd_int_stack_ptr-=1;                     
             }
            else 
             {
              kbd_int_stack_ptr+=9;                     
             }
           }
           return;                            
	case SCRIPT_SU:                            
          if(su()==0)
           {
	      kbd_int_stack_ptr-=1;                
	     }
	    return;                                

	case SCRIPT_VZIO:
          if(vzio()==0)
           {
	      kbd_int_stack_ptr-=1;                
	     }
	    return;                                
	case SCRIPT_CSM:                           
          
          if(inCsmMode(KBD_STACK_PREV(2))==0)    
           {
            kbd_int_stack_ptr-=2;                
	     }
	    return;                                
      case SCRIPT_BAFLF:                         
          if(baflf()==0)
           {
	      kbd_int_stack_ptr-=1;                
	     }
	    return;                                

	case SCRIPT_MOTION_DETECTOR:
	    if(md_detect_motion()==0)
           {
	      kbd_int_stack_ptr-=1;
	     }
	    return;
	case SCRIPT_PRESS:
	    kbd_key_press(KBD_STACK_PREV(2));
	    kbd_int_stack_ptr-=2; //pop op.
	    return;
	case SCRIPT_RELEASE:
	    kbd_key_release(KBD_STACK_PREV(2));
       if((KBD_STACK_PREV(2)==KEY_SHOOT_FULL)&&!tl.running && (displayedMode==30)&& gBracketingCounter)
        {
         play_sound(6);
         gBracketingCounter=0;
        }
	    kbd_int_stack_ptr-=2; //pop op.
	    return;
	case SCRIPT_SLEEP:
	    t = get_tick_count();
	    
	    if (delay_target_ticks == 0)
           {
		delay_target_ticks = t+KBD_STACK_PREV(2);
	     } 
           else 
           {
		if (delay_target_ticks <= t)
               {
		    delay_target_ticks = 0;
		    kbd_int_stack_ptr-=2; //pop sleep op.
		   }
	      }
	    return;
	case SCRIPT_BRACKET_WAIT:
         
          if(get_inf_focus()|| inf_focus){shutter_int=0;conf.tv_bracket_value=0;conf.bracket_type=0;inf_focus=0; kbd_int_stack_ptr-=1;} //pop wait op
	    return;

	case SCRIPT_FOCUSED:
          if(unlock || shooting_get_prop_int(PROPCASE_SHOOTING))
           {
            if(unlock)
            {
             UnlockAF();
             unlock=0;
             afl=0;
            }
            kbd_int_stack_ptr-=1; 
           }
	    return;

	case SCRIPT_WFSP:
          if(!tr)
           {
            int tmp = KBD_STACK_PREV(2);
            tr = get_tick_count();
            if(tmp)tr+=tmp*1000;
            else tr+=10000000;
           }
          if(MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK))gVideoReady=1; 
          if(get_usb_bit() || (get_tick_count()>tr)) 
           {
            tr=0;
            if(MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK))gVideoReady=0;
            kbd_int_stack_ptr-=2; 				
           }
	    return;
 
     case SCRIPT_TL:
          if(time_lapse()==0)
           {
	      kbd_int_stack_ptr-=1;
	     }
          return;

     case SCRIPT_SET_FOCUS_RANGE:            
          if(set_focus_range()==0)			
           {
	      kbd_int_stack_ptr-=1;
	     }
	    return;

     case SCRIPT_USBUP:                              
	   kbd_int_stack_ptr-=1;                       
         conf.remote_enable = 0;                     
         debug_led(1);
#if defined(CAM_PB_MODE)                             
         KBD_STACK_PUSH(PLAYBACK_MODE);
         KBD_STACK_PUSH(SCRIPT_RELEASE);
         KBD_STACK_PUSH(REFRESH_USB);
         kbd_sched_delay(1500);
         KBD_STACK_PUSH(REFRESH_USB);
         kbd_sched_delay(1000*usbup_time+1000);
         KBD_STACK_PUSH(PLAYBACK_MODE);
         KBD_STACK_PUSH(SCRIPT_PRESS)               
#else                                               
         KBD_STACK_PUSH(SCRIPT_RECORD);
         KBD_STACK_PUSH(REFRESH_USB);
         kbd_sched_delay(1500);
         KBD_STACK_PUSH(REFRESH_USB);
         kbd_sched_delay(1000*usbup_time+1000);
         KBD_STACK_PUSH(SCRIPT_PLAYBACK);
#endif	     
         return;

 
     case SCRIPT_END_PLE:                           
	    kbd_int_stack_ptr-=1;
#if defined(CAMERA_a480)
          kbd_sched_delay(900);
          kbd_sched_click(KEY_SET);
          kbd_sched_delay(900);
          kbd_sched_click(KEY_PRINT);
          kbd_sched_delay(900);
#elif defined(CAMERA_a2000)
          record_mode();
#else
          playback_mode();
          record_mode();
#endif
          return;

     case REFRESH_USB:
          conf.remote_enable=1;                      
          kbd_int_stack_ptr-=1;                      
          RefreshUSBMode();
          debug_led(0);
          return;

     case RF:                                       
          kbd_int_stack_ptr-=1;                     
          record_mode();
          if(shooting_can_focus()) lens_set_focus_pos(cf+fpd); 
          return;

	case SCRIPT_PR_WAIT_SAVE:
	    state_shooting_progress = SHOOTING_PROGRESS_NONE;
	    kbd_int_stack_ptr-=1; 
	    return;
	case SCRIPT_WAIT_SAVE:
        {
	    if (state_shooting_progress == SHOOTING_PROGRESS_DONE)
		kbd_int_stack_ptr-=1; 
	    return;
	   }
	case SCRIPT_WAIT_FLASH:
        {
	    if (shooting_is_flash_ready())
		kbd_int_stack_ptr-=1; 
	    return;
	   }
	case SCRIPT_WAIT_EXPHIST:
         {
	    if (state_expos_recalculated)            
           {
		kbd_int_stack_ptr-=1; 
	     }
	    return;
	   }
	case SCRIPT_PR_WAIT_EXPHIST: 
         {
	    if (shooting_in_progress() || mvideo) 
           {
		state_expos_recalculated = 0;
		kbd_int_stack_ptr-=1;                   
	     }
	    return;
	   }
        case SCRIPT_WAIT_CLICK: 
          {
            t = get_tick_count();
	     if (delay_target_ticks == 0)
            {

		delay_target_ticks = t+((KBD_STACK_PREV(2))?KBD_STACK_PREV(2):86400000);
	      } 
          else 
            {
                kbd_last_clicked = lastClickedKey;
                if (kbd_last_clicked || delay_target_ticks <= t) 
                  {
                    if (!kbd_last_clicked) 
                    kbd_last_clicked=0xFFFF;   
        	        delay_target_ticks = 0;
                    kbd_int_stack_ptr-=2; 
                   }
	       }
	       return;
          }
        case SCRIPT_PM: 
          {
           kbd_int_stack_ptr-=1; 
           shooting_set_mode_chdk(2);
           return;
          }
        case SCRIPT_PLAYBACK: 
          {
           kbd_int_stack_ptr-=1; 
           playback_mode();
           return;
          }
        case SCRIPT_RECORD: 
          {
           kbd_int_stack_ptr-=1; 
           record_mode();
           return;
          }
        case SCRIPT_MOVIE: 
          {
           kbd_int_stack_ptr-=1; 
           ubasic_mm();
           return;
          }
        case AF_LOCK: 
          {
           kbd_int_stack_ptr-=1; 
           afl=1;
           DoAFLock();
           play_sound(4);
           return;
          }
	  case RESTORE_CAPTURE_MODE:
          {
           kbd_int_stack_ptr-=1;
           shooting_set_mode_chdk(CurrentCaptureMode);
           return;
          }

 
 // ------------------------------------------------------------------------ 
 //                       Based on CHDK KAP algorithm
 //         http://chdk.wikia.com/wiki/KAP_UAV_Exposure_Control_Script
 // ------------------------------------------------------------------------  
      
        case FAST_KAP: 
          {      
           kbd_int_stack_ptr-=1;
           calcShoot96();
           return;           
          }
 
        case SCRIPT_FAST_FINISH: 
          {
           kbd_int_stack_ptr-=1;                                                 
           conf.nd_filter_state = gCurrentNDFilterState;
           conf.nd_constant_exposure = gCurrentNDFilterConstantExposure;             
           return;
          }
          
         case SCRIPT_BURST_FINISH: 
          {
           if(!conf.flag_1 || get_usb_bit())
           {
            conf.flag_1=0;
            gLowLightCount=0;
	         kbd_int_stack_ptr-=1;                
	        } 
           return;
          }     

	default:
	    script_end();
	}
    }

    ubasic_run();          

    if (ubasic_finished()) 
    {
     script_end();
    }   
}

void ubasic_camera_press(const char *s)
{
    long k = keyid_by_name(s);
    if (k > 0) {
	kbd_sched_press(k);
    } else {
	ubasic_error = 3;
    }
}

void ubasic_camera_release(const char *s)
{
    long k = keyid_by_name(s);
    if (k > 0) {
	kbd_sched_release(k);
    } else {
	ubasic_error = 3;
    }
}

void ubasic_camera_click(const char *s)
{
    long k = keyid_by_name(s);
    if (k > 0) {
	kbd_sched_click(k);
    } else {
	ubasic_error = 3;
    }
}

void ubasic_camera_wait_click(int timeout)
{
    kbd_sched_wait_click(timeout);
}

int ubasic_camera_is_clicked(const char *s)
{
    if(strcmp("last_pressed",s)==0) return lastClickedKey;
    long k = keyid_by_name(s);
    if (k==0xFF) return get_usb_power(1);  
    if (k > 0) {
        return (kbd_last_clicked == k);
    } else {
	ubasic_error = 3;
    }
    return 0;
}

int ubasic_camera_is_pressed(const char *s)
{
    if(strcmp("last_pressed",s)==0) return kbd_last_clicked;
    long k = keyid_by_name(s);
    if (k==0xFF) return get_usb_power(1);  
    if (k > 0) 
     {
      return kbd_is_key_pressed(k);
     } 
    return 0;
}

void ubasic_camera_sleep(long v)
{
    kbd_sched_delay(v);
}

void ubasic_bracketing_done()
{
 KBD_STACK_PUSH(KEY_SHOOT_FULL);               
 KBD_STACK_PUSH(SCRIPT_RELEASE);
 KBD_STACK_PUSH(SCRIPT_BRACKET_WAIT);
}

void ubasic_wfsp(int action)
{
  KBD_STACK_PUSH(action);
  kbd_sched_wfsp();
}
 
void ubasic_safe_set_zoom(int step)
{
 int zp = gZoomPoint;
 if (step < 0) step = 0;
 else if (step >= zoom_points)step = zoom_points-1;
 int steps = step-zp; 
 int dir = 1;
 if(steps<0)dir=0;
 steps = abs(steps);

 if(steps!=0)
  {
   int i;
   kbd_sched_release(KEY_SHOOT_HALF);
   kbd_sched_delay(1000);
   kbd_sched_press(KEY_SHOOT_HALF);
   for(i=0;i<steps;i++)
    {
     kbd_sched_delay(1000);
     if(dir)
      {
       kbd_sched_release(KEY_ZOOM_IN);
       kbd_sched_delay(10);
       kbd_sched_press(KEY_ZOOM_IN);
      }
     else
      {
       kbd_sched_release(KEY_ZOOM_OUT);
       kbd_sched_delay(10);
       kbd_sched_press(KEY_ZOOM_OUT);
      }
    }
  }
}

void ubasic_cont_shoot(int action)
{
 int i;
 unsigned char cnt;
 #if defined(CAMERA_g7x)
  if(!gPlayRecMode){play_sound(6);return;} 
#endif
     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                     
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);

 if((CDM>0) && (CDM<7))
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
#if defined(CAM_HAS_NATIVE_RAW)
     if(shooting_get_prop(PROPCASE_CANON_RAW)!=1) 
      cnt= CDM -1; 
     else cnt = CDM;
 #else
     cnt=CDM;
#endif
   for(i=0;i<cnt;i++)
    {
     kbd_sched_delay(800);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
    }
     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }

 else if(CDM==9)
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
     kbd_sched_delay(500);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(500); 
   for(i=0;i<2;i++)
    {
     kbd_sched_delay(800);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
    }
     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }

 else if(CDM==10)
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
     kbd_sched_delay(500);
 
   for(i=0;i<2;i++)
    {
     kbd_sched_delay(800);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
    }

     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }

 else if(CDM==11) 
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
     kbd_sched_delay(500);
 
   for(i=0;i<4;i++)
    {
     kbd_sched_delay(800);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
    }

     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }
  
   else if(CDM==12) 
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
     kbd_sched_delay(500);
 
     kbd_sched_delay(800);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
 
     kbd_sched_delay(1000);
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_SET);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }
  
   else if(CDM==13) 
  {
     kbd_sched_delay(500);
     KBD_STACK_PUSH(action);
     KBD_STACK_PUSH(SCRIPT_CSM); 
     kbd_sched_delay(500); 
     KBD_STACK_PUSH(KEY_UP); 
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(50*conf.click_time);                                   
     KBD_STACK_PUSH(KEY_UP);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(1000); 
  }

 else if(CDM==0)				
  {
   kbd_sched_delay(500);
   KBD_STACK_PUSH(action);
   KBD_STACK_PUSH(SCRIPT_CSM); 
  }
 else
  {
   kbd_sched_delay(500);
   KBD_STACK_PUSH(action);
   KBD_STACK_PUSH(SCRIPT_CSM); 
  }

  KBD_STACK_PUSH(SCRIPT_PM);		
  kbd_sched_delay(1000);
}

void ubasic_camera_shoot()
{
kbd_sched_shoot();
}

void ubasic_set_focus_range()  
{
 kbd_sched_delay(2000);
 KBD_STACK_PUSH(SCRIPT_SET_FOCUS_RANGE);
}

void ubasic_rm()                               
{
 #if defined(CAMERA_a480)
  kbd_sched_click(KEY_SET);
  kbd_sched_click(KEY_PRINT);
#else
 if(!rrm) record_mode();                       
 else if(rrm==1)                               
  {
   KBD_STACK_PUSH(PLAYBACK_MODE);              
   KBD_STACK_PUSH(SCRIPT_RELEASE);
  }
 else if (rrm==2)                             
  {
#if !defined(CAMERA_a1000)
   playback_mode();
#endif
   record_mode();
  }
 else if(rrm==3)                              
  {
   KBD_STACK_PUSH(MOVIE_MODE);
   KBD_STACK_PUSH(SCRIPT_RELEASE);
  }
#endif
}

void ubasic_pm()                               
{
 if(shooting_can_focus()) cf=lens_get_focus_pos_from_lens(); 
#if defined CAM_PB_MODE                        
  rrm=1;
  KBD_STACK_PUSH(PLAYBACK_MODE);
  KBD_STACK_PUSH(SCRIPT_PRESS);
#else                                          
 rrm=0;
 playback_mode();
#endif
}

void ubasic_mm()                               
{
gScriptMovie=1;
#if !defined  CAM_MOVIE_PLE                    
  rrm=3;
  KBD_STACK_PUSH(MOVIE_MODE);
  KBD_STACK_PUSH(SCRIPT_PRESS);
#else
   rrm=2;
kbd_sched_delay(1000);
KBD_STACK_PUSH(KEY_SHOOT_HALF);
KBD_STACK_PUSH(SCRIPT_PRESS);
kbd_sched_delay(1000);
  movie_mode();
#endif
}

void ubasic_ulf()					
{
 if(!shooting_get_prop(PROPCASE_FOCUS_MODE))     
  {UnlockAF();afl=0;}                            
 else
  UnlockMF();
}

unsigned char enable_focus_override()
{ 
 #if defined(PROPCASE_CONTINUOUS_AF)
 if(!(MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK))) 
  {
   if(shooting_get_prop(PROPCASE_CONTINUOUS_AF)){play_sound(6);return 4;} 
  }
 #endif
 #if defined(PROPCASE_SERVO_AF)
 if(shooting_get_prop(PROPCASE_SERVO_AF)){play_sound(6);return 5;}
 #endif
 
 if(conf.focus_override_method==3) 
 {
  if(!DoMFLock())DoAFLock(); 
  if(!shooting_get_prop(PROPCASE_FOCUS_MODE)){play_sound(6);return 6;} 
  return 3;
 } 
 else if(conf.focus_override_method==2) 
 {
  DoAFLock(); 
  if(!shooting_get_prop(PROPCASE_AF_LOCK)){play_sound(6);return 7;} 
  afl=1;
  return 2;
 }
 else if(conf.focus_override_method==1) 
 {
  return 1;
 }
 else {play_sound(6);return 0;}
}
 
void disable_focus_override()
{ 
 photo_param_put_off.subj_dist=0; 
 if((conf.focus_override_method==3)||(conf.focus_override_method==1)) UnlockMF(); 
 else if(conf.focus_override_method==2) 
 {
  unlock=1; 
  kbd_sched_delay(500);
  KBD_STACK_PUSH(KEY_SHOOT_HALF);
  KBD_STACK_PUSH(SCRIPT_RELEASE);
  kbd_sched_delay(500);
  KBD_STACK_PUSH(SCRIPT_FOCUSED); 
  KBD_STACK_PUSH(KEY_SHOOT_HALF);
  KBD_STACK_PUSH(SCRIPT_PRESS);
 }
}
 
 void ubasic_af()      
{
 if(shooting_get_prop(PROPCASE_FOCUS_MODE))shooting_set_mf(); 
 UnlockAF();
 afl=0;
 kbd_sched_delay(500);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_RELEASE);
 kbd_sched_delay(500);
 KBD_STACK_PUSH(SCRIPT_FOCUSED); 
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_PRESS);
}
 

void ubasic_sm(t) 
{
 CurrentCaptureMode=shooting_get_prop(PROPCASE_SHOOTING_MODE);
 KBD_STACK_PUSH(RESTORE_CAPTURE_MODE);
  shooting_set_mode_chdk(9); 
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
	    kbd_sched_click(KEY_VIDEO);
#else
          kbd_sched_click(KEY_SHOOT_FULL);
#endif
	    kbd_sched_delay(1000*t+1000);
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
	    kbd_sched_click(KEY_VIDEO);
#else
          kbd_sched_click(KEY_SHOOT_FULL);
#endif
          kbd_sched_delay(1000);
}

void ubasic_usbupload(t)                        
{
 usbup_time = t;
 KBD_STACK_PUSH(SCRIPT_USBUP);
}
 
void ubasic_su(int hr, int min)     
{
 st_time.hr=hr;
if(st_time.hr<0)st_time.hr=0;
if(st_time.hr>23)st_time.hr=23;
 st_time.min=min;
if(st_time.min<0)st_time.min=0;
if(st_time.min>59)st_time.min=59;
su_first=1;                         
conf.alt_prevent_shutdown=2;        
 KBD_STACK_PUSH(SCRIPT_SU);
 TurnOffBackLight();
}

void ubasic_baflf(int duration)     
{
 afcnt=shooting_get_tick_count()+(duration*1000);
 KBD_STACK_PUSH(SCRIPT_BAFLF);
}
 
void ubasic_smov()
{
 if(get_movie_status()==VIDEO_RECORD_STOPPED)
 {
  gFrameCount=conf.video_frame_count;                               
  gFramesRecorded=0;
  CurrentCaptureMode=shooting_get_prop(PROPCASE_SHOOTING_MODE);
  shooting_set_mode_chdk(9);			           
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
  kbd_sched_click(KEY_VIDEO);
#else
  kbd_sched_click(KEY_SHOOT_FULL);
#endif
  kbd_sched_delay(1000);
  }
  else
  {
   KBD_STACK_PUSH(RESTORE_CAPTURE_MODE);
   kbd_sched_delay(1000);
 #if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
  kbd_sched_click(KEY_VIDEO);
#else
  kbd_sched_click(KEY_SHOOT_FULL);
#endif 
  }  
}  

void ubasic_ssmov()
{
 if(get_movie_status()==VIDEO_RECORD_STOPPED)
 {
  gFrameCount=conf.video_frame_count;                               
  gFramesRecorded=0;
  CurrentCaptureMode=shooting_get_prop(PROPCASE_SHOOTING_MODE);
  shooting_set_mode_chdk(9);			           
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
  kbd_sched_click(KEY_VIDEO);
#else
  kbd_sched_click(KEY_SHOOT_FULL);
#endif
  kbd_sched_delay(1000);
  }
  else
  {
   KBD_STACK_PUSH(RESTORE_CAPTURE_MODE);
   kbd_sched_delay(1000);
 #if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
  kbd_sched_click(KEY_VIDEO);
#else
  kbd_sched_click(KEY_SHOOT_FULL);
#endif 
  }  
  KBD_STACK_PUSH(0);                      
  KBD_STACK_PUSH(SCRIPT_WFSP);			   
  return;
}  
 
void ubasic_vzoom_in_out()
{
 KBD_STACK_PUSH(SCRIPT_VZIO);
}

void fill_bv_buffer()		
{
 bufcnt = 0;
 KBD_STACK_PUSH(FILL_BV_BUFFER); 
 kbd_sched_delay(500);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_RELEASE);
 KBD_STACK_PUSH(SCRIPT_FOCUSED);
 kbd_sched_delay(1000);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_PRESS);
 kbd_sched_delay(1000);
}

void ubasic_cover_lens()
{
 KBD_STACK_PUSH(SCRIPT_BAFLF);
 KBD_STACK_PUSH(SCRIPT_COVER_LENS);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_RELEASE);
 KBD_STACK_PUSH(SCRIPT_FOCUSED);
 kbd_sched_delay(1000);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_PRESS);
 if(checkLensCap==0)checkLensCap=5;  
 kbd_sched_delay(checkLensCap*1000); 
}

void ubasic_fastShoot(int maxiso)
{
 conf.nd_constant_exposure = 0;
 kbd_sched_shoot();
 KBD_STACK_PUSH(FAST_KAP);
 KBD_STACK_PUSH(SCRIPT_FOCUSED);
 KBD_STACK_PUSH(KEY_SHOOT_HALF);
 KBD_STACK_PUSH(SCRIPT_PRESS);
}

void ubasic_fastKAP()
{
 conf.nd_constant_exposure = 0;
 KBD_STACK_PUSH(FAST_KAP);
}

void ubasic_burst()
{
 conf.flag_1 = 1;                                           
 KBD_STACK_PUSH(KEY_SHOOT_FULL);
 KBD_STACK_PUSH(SCRIPT_RELEASE);                            
 KBD_STACK_PUSH(SCRIPT_BURST_FINISH);                       
 KBD_STACK_PUSH(KEY_SHOOT_FULL);                            
 KBD_STACK_PUSH(SCRIPT_PRESS);
}

void BootLock()
{
}
 
void script_autostart()
{
  msleep(500);                                    
 strcpy(normal_script,conf.user_script_file);        
 const char *fn = "A/SDM/SCRIPTS3/Startup.txt";
  gScriptFolder=2;
FILE *fdstartup = NULL;

fdstartup=fopen(fn,"r");

 if(fdstartup != NULL)                          
   {
    fclose(fdstartup);
    startup_script=1;
    script_load(fn,0);                            
   }
  else 
   {
    fclose(fdstartup);
    script_load(normal_script,0);                 
   }
  
	auto_started = 1;
	kbd_blocked = 1;
	gui_kbd_enter(); 
	script_console_add_line("! Autostart !"); 
   script_start();
}

void exit_alt()                                    
{ 
 draw_restore();
 kbd_blocked = 0;                                  
 gui_kbd_leave();                                  
 kbd_key_press(KEY_SHOOT_HALF);
 msleep(1000);
 kbd_key_release(KEY_SHOOT_HALF);
}

int inCsmMode(int action)
{
 if((shooting_get_prop(PROPCASE_DRIVE_MODE)!=1)&& action) 
  {
   if(((CDM>0) && (CDM<7))||(CDM==13))
    {
     if(!shooting_get_prop(PROPCASE_DRIVE_MODE))         
      {
       kbd_sched_delay(600);
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
      }
     else                                               
      {
       kbd_sched_delay(500);
       KBD_STACK_PUSH(KEY_LEFT);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_LEFT);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
      }
      return 0;
    }
   else if(CDM==0)					
    {
     kbd_sched_delay(300);
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(conf.click_time*50);                                     
     KBD_STACK_PUSH(KEY_DOWN);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(400);
     return 1;
    }
   else if(CDM==8)
    {
     kbd_sched_delay(300);
     KBD_STACK_PUSH(KEY_TIMER);
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(conf.click_time*50);                                     
     KBD_STACK_PUSH(KEY_TIMER);
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(400);
     return 1;
    }
   else if(CDM==9)
    {
     kbd_sched_delay(300);
     if(!(shooting_get_prop(PROPCASE_DRIVE_MODE))){KBD_STACK_PUSH(KEY_DOWN);}
     else {KBD_STACK_PUSH(KEY_UP);}
     KBD_STACK_PUSH(SCRIPT_RELEASE);
     kbd_sched_delay(conf.click_time*50);                                     
     if(!(shooting_get_prop(PROPCASE_DRIVE_MODE))){KBD_STACK_PUSH(KEY_DOWN);}
     else {KBD_STACK_PUSH(KEY_UP);}
     KBD_STACK_PUSH(SCRIPT_PRESS);
     kbd_sched_delay(400);
     return 0; 
    }
   else if((CDM==10)||(CDM==11)||(CDM==12))  
    {
       kbd_sched_delay(600);
       KBD_STACK_PUSH(KEY_DOWN);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_DOWN);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
    }
    else return 0;
   }

  else if(shooting_get_prop(PROPCASE_DRIVE_MODE) && !action)
   {
     if((CDM>0) && (CDM<7))
     {
       kbd_sched_delay(600);
       KBD_STACK_PUSH(KEY_LEFT);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_LEFT);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
     }
    else if((CDM==0)||(CDM==9))
     {
      kbd_sched_delay(300);
      KBD_STACK_PUSH(KEY_UP);
      KBD_STACK_PUSH(SCRIPT_RELEASE);
      kbd_sched_delay(conf.click_time*50);                                     
      KBD_STACK_PUSH(KEY_UP);
      KBD_STACK_PUSH(SCRIPT_PRESS);
      kbd_sched_delay(400);
      return 1;
     }
    else if(CDM==8)
     {
      kbd_sched_delay(300);
      KBD_STACK_PUSH(KEY_TIMER);
      KBD_STACK_PUSH(SCRIPT_RELEASE);
      kbd_sched_delay(conf.click_time*50);                                     
      KBD_STACK_PUSH(KEY_TIMER);
      KBD_STACK_PUSH(SCRIPT_PRESS);
      kbd_sched_delay(400);
      return 1;
     }
    else if((CDM==10)||(CDM==11)||(CDM==12)) 
     {
       kbd_sched_delay(600);
       KBD_STACK_PUSH(KEY_UP);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_UP);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(KEY_RIGHT);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
     }
     else  if(CDM==13) 
     {
      int key;
      if(shooting_get_prop(PROPCASE_DRIVE_MODE)==1)key=KEY_LEFT;
      else key=KEY_RIGHT;
       kbd_sched_delay(600);
       KBD_STACK_PUSH(key);
       KBD_STACK_PUSH(SCRIPT_RELEASE);
       kbd_sched_delay(conf.click_time*50);                                     
       KBD_STACK_PUSH(key);
       KBD_STACK_PUSH(SCRIPT_PRESS);
       kbd_sched_delay(400);
       return 1;
     }
     else return 0;
   }
  else return 0; 
}

int baflf() 
{
 static int ls=1; 
 if(shooting_get_tick_count()>=afcnt) 
 {
   ubasic_set_led(9,0,0);
  ls=1;
  return 0;
 }
 ubasic_set_led(9,ls,0);
 ls = !ls;
 if(!ls) kbd_sched_delay(10);                    
 else kbd_sched_delay(500);                      
 return 1;
}

int vzio()						
{
 char buf[5];
 static unsigned char zoomDirection,prevPressed;
 unsigned char currentZoomStep;
 static int pressedCount;
 currentZoomStep = shooting_get_zoom();
 if(kbd_is_key_pressed(KEY_SET))return 0;
 if(!zoomDirection)
  sprintf(buf," %c ",(char)30);
 else
  sprintf(buf,"%c%c%c",(char)30,(char)30,(char)30);
 if(get_usb_power(1))
  {
   pressedCount++;
   if(!prevPressed)zoomDirection = !zoomDirection;
   if(pressedCount>40)
    {
 #if defined(CAMERA_sx200is)||defined(CAMERA_sx240hs)||defined(CAMERA_sx260hs)||defined(CAMERA_sx280hs)||defined(CAMERA_sx50hs)
     if(zoomDirection )kbd_key_press(KEY_ZOOM_IN_SLOW);
     else kbd_key_press(KEY_ZOOM_OUT_SLOW);
#else
     if(zoomDirection )kbd_key_press(KEY_ZOOM_IN);
     else kbd_key_press(KEY_ZOOM_OUT);
#endif
    }
   prevPressed = 1;
   draw_string((screen_width>>1)-12+vid_get_viewport_display_xoffset(),48,buf,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_TRANSPARENT),1);
  }
 else
  {
   draw_string((screen_width>>1)-12+vid_get_viewport_display_xoffset(),48,buf,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
   prevPressed = 0;
   pressedCount = 0;
#if defined(CAMERA_sx200is)||defined(CAMERA_sx240hs)||defined(CAMERA_sx260hs)||defined(CAMERA_sx280hs)||defined(CAMERA_sx50hs)
   kbd_key_release(KEY_ZOOM_IN_SLOW);
   kbd_key_release(KEY_ZOOM_OUT_SLOW);
#else
   kbd_key_release(KEY_ZOOM_IN);
   kbd_key_release(KEY_ZOOM_OUT);
#endif
  }
 return 1;
}

int su()                                       
{
 long t;
 char buf[64];
 static long led_time;
 static int led_count = 3;
 t=time(NULL);                                  
 ttm = localtime(&t);                           
 if((ttm->tm_hour==st_time.hr)&& (ttm->tm_min>=st_time.min)) 
 {
  TurnOnBackLight();
  return 0;
 }
  if(su_first)
 {
  led_time=(unsigned long)time(NULL)+30;
  su_first=0;
 }
     if(!(--led_count)) debug_led(0);
     if((unsigned long)time(NULL)==led_time)
     {
      debug_led(1);
      led_count = 3;
      led_time+=30;
     }
 return 1;
}


void init_tl(int a,int b, int c, int d,int e,int f,int g,int h,int i,int j,int k,int l,int m,int n,int p,int q,int r,int s, int t, int u)
{
int ovr_x = 112;
int ovr_y = 5;
tl_first = 1;                                                       
 tl.start_delay = 60*a +b;                                          
 if(tl.start_delay<2)tl.start_delay =2;

 tl.shoot_interval= 60*c + d;                                       
 if(tl.shoot_interval<1)tl.shoot_interval =1;

 tl.num_exposures=e;                                                
 if(tl.num_exposures<1)tl.num_exposures = 1;

 tl.endless_mode=f;
 if(tl.endless_mode<0) tl.endless_mode = 0;
 
 tl.shoot_mode=g;                                                    
 if(tl.shoot_mode<0) tl.shoot_mode =0;                               
 if(tl.shoot_mode>4) tl.shoot_mode=1;                                
 if(tl.shoot_mode==3)                                                
 {
  conf.tv_bracket_value=0;                                           
  conf.dist_mode=0;                                                  
 }

 tl.focus_mode = m;                                                   

 tl.ev_steps = l;
  if(tl.ev_steps<1) tl.ev_steps =1;
  if(tl.ev_steps>12) tl.ev_steps=12;

 tl.bracket_type=j;                                                   
 if(tl.bracket_type<0) tl.bracket_type =0;

 if(!tl.bracket_type && tl.shoot_mode && (tl.shoot_mode<3))          
  {
   conf.tv_bracket_value = tl.ev_steps;
   conf.dist_mode=0;                                                  
  }

 if(tl.bracket_type>=1)                                               
  {
    conf.tv_bracket_value=0;                                          
    if((tl.focus_mode<0)|| (tl.focus_mode>2)) tl.focus_mode=0;        
    conf.dist_mode=tl.focus_mode+1;
    if(tl.focus_mode==1)conf.bracket_type=3;                          
  }

 if(!tl.bracket_type)tl.bkt_exposures=h;                               
 else tl.bkt_exposures=i;                                             
 if(tl.bkt_exposures<3) tl.bkt_exposures=3;    
 if(tl.bkt_exposures>19) tl.bkt_exposures=19;                        
 if((tl.bkt_exposures%2) != 1) tl.bkt_exposures-= 1;                  
 tl.bkt_exposures = ((tl.bkt_exposures+1)/2)+1;
 if((tl.shoot_mode==2)&&(tl.bkt_exposures>9))tl.bkt_exposures=9;       
 conf.bracket_type = tl.bkt_exposures;
 
 tl.focus_tv_exposures=h;                                             

   if(n<1 || n>5000) conf.subj_dist_bracket_value = 1;                
    else  conf.subj_dist_bracket_value = n; 

   if(p<450) conf.distance_setting = 450;                             
   else if(conf.distance_setting>5000)conf.distance_setting  = 5000;
   else conf.distance_setting = p; 

    if((tl.shoot_mode && tl.shoot_mode<3) && (tl.bracket_type != 2)&&(tl.shoot_interval<(2*tl.bkt_exposures))) tl.shoot_interval=2*tl.bkt_exposures; 
    else if((tl.shoot_mode && tl.shoot_mode<3) && (tl.bracket_type == 2))                   
    {
     if(tl.shoot_interval<(2*tl.bkt_exposures*tl.focus_tv_exposures))tl.shoot_interval=2*tl.bkt_exposures*tl.focus_tv_exposures;
     conf.tv_bracket_value=0;                                           
    }

 if(((tl.shoot_mode==1)||(tl.shoot_mode==2))&&(tl.bracket_type>=1)&&(tl.focus_mode==1)&&(tl.num_exposures==1))tl.shoot_interval=201; 

 tl.exposure_mode=k;                                                     
  if(((tl.bracket_type==0)&& (shooting_get_drive_mode()==3)))  
  {
   if(tl.exposure_mode<0) tl.exposure_mode=0;
   if(tl.exposure_mode>2) tl.exposure_mode=2;
   conf.bracket_type=tl.exposure_mode;
  }

 tl.sun=t;                                                        

if(!tl.shoot_mode && tl.sun)                                      
 {
   if(tl.exposure_mode<0) tl.exposure_mode=0;
   if(tl.exposure_mode>2) tl.exposure_mode=2;
 }

if(tl.sun && !tl.shoot_mode) tl.num_exposures=conf.sunrise_shots; 

tl.suncsv=u;                                                     
 
 if(!tl.shoot_mode)
 {
  conf.dist_mode=0; 
  tl.bracket_type=0; 
 }

 tl.shutdown=r;
 if(tl.shutdown<0) tl.shutdown=0;
 if(tl.shutdown>2) tl.shutdown=2;

 tl.blank_screen = q; 
 if(tl.blank_screen<0) tl.blank_screen=0;

 log_status=conf.save_auto;                                 
 if(s<=0)conf.save_auto=0;       
 else conf.save_auto=1; 
 tl.target_time = (unsigned long)time(NULL)+tl.start_delay;

 tl.led_time = (unsigned long)time(NULL)+ 30;

 if(!tl.shoot_mode && !shooting_get_drive_mode())                 tl_mod = 0;
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==0))  tl_mod = 1;                     
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==0))  tl_mod = 2;                     
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==1)&&(tl.focus_mode==0))tl_mod = 3;   
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==1)&&(tl.focus_mode==0))tl_mod = 4;   
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==1)&&(tl.focus_mode==1))tl_mod = 5;   
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==1)&&(tl.focus_mode==1))tl_mod = 6;   
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==1)&&(tl.focus_mode==2))tl_mod = 7;   
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==1)&&(tl.focus_mode==2))tl_mod = 8;  
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==2)&&(tl.focus_mode==0))tl_mod = 9; 
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==2)&&(tl.focus_mode==1))tl_mod = 10; 
 else if ((shooting_get_drive_mode()==1)&& (tl.bracket_type==2)&&(tl.focus_mode==2))tl_mod = 11; 
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==2)&&(tl.focus_mode==0))tl_mod = 12; 
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==2)&&(tl.focus_mode==1))tl_mod = 13; 
 else if ((shooting_get_drive_mode()==3)&& (tl.bracket_type==2)&&(tl.focus_mode==2))tl_mod = 14; 

 unsigned int mmode = mode_get();
 conf.line_length=20;
 bracket_running = 1;
 tl.running=1;        
 kbd_sched_tl();
}

static int ss;                                                         
unsigned int tl_target_shot_count;

int time_lapse()
{
 static int led_count = 20;
 static unsigned long led_time,usb_detect_time,finished_time;
 static int usb_detect = 0;
 static unsigned long prev_time_left,movieStart;
 static unsigned long current_time_left,auto_start;
 static int shot_count;                                       
 static int focus_tv_be;                                               
 static int current_override,start_tv;                                 
 static int tv_bkt_cnt;                                                
 static int ctvoe;                                                     
 static int sd =0;                                                      
 static int tldone = 0;							                              
 static int pos_x = 16;
 static int pos_y = 50,m;
 static int auto_exposures,alldone;
 color tr = MAKE_COLOR(COLOR_TRANSPARENT,COLOR_GREEN);                  
 color cc = MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED);                    
 char buf[64];
 static int ls;                                                         
 static int hpd;                                                        
 static int fld;                                                        
 static int burst_time;                                                 
 static long secremain;                                                 
 static long t;                                                         
 static int final_exposure;                                             
 static int delay_done=0;                                               
 static int start_in_minutes,end_in_minutes;
 static unsigned char dailyFinished=0,burstInProgress=0;                
 
if(tl_first)
{
 if(gStartHour&&gEndHour&&gStartMinutes&&gEndMinutes)
 {
  start_in_minutes=(gStartHour-100)*60+(gStartMinutes-100);
  end_in_minutes=(gEndHour-100)*60+(gEndMinutes-100);
 }
 m=mode_get()&MODE_SHOOTING_MASK;
 if(tl.shoot_mode && (tl.shoot_mode<3)) conf.synch_enable=1;            
 if((tl.bracket_type>=1)&& shooting_can_focus()&&(tl.focus_mode!=2)&& shooting_get_drive_mode()) conf.distance_setting=lens_get_focus_pos()-fpd;                                               
 
 if(tl.bracket_type==2)
  {
 current_override=conf.tv_override;                                    
    #if defined(CAMERA_tx1)
  conf.tv_override=shooting_get_tv96_index(shooting_get_tv96());
#else
    if (!(m==MODE_M || m==MODE_TV)) conf.tv_override=shooting_get_tv96_index(shooting_get_tv96()); 
    else if (!((conf.tv_override >=0) && (conf.tv_override <=65) && (conf.tv_override != 19) && conf.tv_override_enable)) conf.tv_override =shooting_get_tv96_index(shooting_get_user_tv96());                           	   
#endif
   start_tv=conf.tv_override;
    ctvoe=conf.tv_override_enable;
    conf.tv_override_enable = 1;                                         
  }

  if(!tl.shoot_mode)                                                     
  {
#if defined(CAMERA_tx1)
    ss=shooting_get_tv96();
#else
    if (!(m==MODE_M || m==MODE_TV))ss=shooting_get_tv96(); 
        else 
          {
	    if (conf.tv_override >=0 && conf.tv_override <=65 && conf.tv_override != 19 && conf.tv_override_enable)ss=shooting_get_tv96();
           else ss=shooting_get_user_tv96();  
	    }
#endif
  }

  if(!tl.shoot_mode && (shooting_get_prop(PROPCASE_FLASH_MODE)!=2))               
 {
  if((ss>=96) && (tl.shoot_interval<4)) tl.shoot_interval=4;                      
  else if (ss<96 && ss>=0 && tl.shoot_interval < 5)tl.shoot_interval=5;           
  else if (ss<0 && ((ss-192)< shooting_get_tv96_from_shutter_speed((float)tl.shoot_interval)))  tl.shoot_interval = (int)(shooting_get_shutter_speed_from_tv96(ss-192))+2; 
 }

 else if(!tl.shoot_mode && (shooting_get_prop(PROPCASE_FLASH_MODE)==2))           
 {
  if (ss<96 && ss>=0 && tl.shoot_interval < 2)tl.shoot_interval=2;                
  else if(ss<0 && ((ss-192)< shooting_get_tv96_from_shutter_speed((float)tl.shoot_interval)))  tl.shoot_interval = (int)(shooting_get_shutter_speed_from_tv96(ss-192));
 }

 prev_time_left=999999;
 tl.shot_count=0;
 tv_bkt_cnt=1;
 tl.ss = 1;                                                                       
 tl_first=0;
 alldone=0;
 hpd=0;
 fld=0;
 ls = 0;                                                                         
 auto_exposures=tl.num_exposures;                                         
 focus_tv_be=tl.focus_tv_exposures;                                              
                                               
 
if((!tl.shoot_mode && !shooting_get_focus_mode() && !has_refocused && !af_all) || ((tl.shoot_mode && (tl.shoot_mode<3)) && !shooting_get_focus_mode() && !tl.bracket_type))  
  {
   kbd_sched_delay(100);
   kbd_sched_release(KEY_SHOOT_HALF);
   kbd_sched_delay(2000);
   DoAFLock(); 
   kbd_sched_press(KEY_SHOOT_HALF);
   hpd=1; 
   return 1;                                                                  
  }

  if(tl.shoot_mode==3)                                                       
  {
   if(tl.num_exposures==1) burst_time=1;                                     
   else 
   {
     burst_time=0;                                                           
     tl_target_shot_count=tl.shot_count+tl.num_exposures;                      
   }                                                
  }

  if(conf.save_raw)
    secremain=tl.shoot_interval*GetRawCount();
  else
    secremain=tl.shoot_interval*GetJpgCount();
   
}

if(tldone)                                                                    
{
 tldone = 0;
 ls=0;                                                                        
 conf.save_auto=log_status;                                                   
 return 0;                                                                    
}
 
 
 if(!tl_first && hpd && !fld)                                                  
 {
   hpd=0;
   fld=1;
   afl=1;                                                       
   return 1;
 }
 
static unsigned char usbCount=0;

 
 if((tl.shutdown==2) && get_usb_power(1) && !usb_detect)                  
  {
   usb_detect=1;
   ++usbCount; 
   return 1;
  }

   if(!tl.num_exposures && (tl.shutdown==1) && !conf.flag_1 && tl.shoot_mode && !bracket_running && !sd)         
   { 
    sd=1;
    kbd_sched_delay(2000);
    power_down();
    return 1;
   }
 
  if(tl.endless_mode && usbCount && !conf.flag_1 && (tl.shoot_mode!=4) && (tl.shutdown==2)&& !sd)                         
   { 
    sd=1;
    kbd_sched_delay(2000);  
    power_down();
    return 1;   
   }

   else if((tl.shoot_mode==4) && usbCount  && (tl.shutdown==2) &&(get_movie_status()==VIDEO_RECORD_STOPPED)&& !sd)                           
   { 
    sd=1;
    kbd_sched_delay(2000);  
    power_down();
    return 1;
   }

 
   if(!tl.num_exposures && !bracket_running && !tl.endless_mode && !tl.shoot_mode && (((unsigned long)time(NULL)-auto_start)>3)) 
  {
   if(conf.ricoh_ca1_mode || (!conf.ricoh_ca1_mode && conf.use_af_led))      
   {
    debug_led(ls);                                                           
   }
    ls = !ls;
    if(!ls) kbd_sched_delay(30);                                              
    else kbd_sched_delay(3000);                                               
  }

  if((tl.num_exposures<0) && bracket_running)
  {
    sprintf(buf,"! Overrun error !");                                        
    console_add_line(buf,1);
    sprintf(buf,"! Sequence terminated !");                                  
    console_add_line(buf,3);
    tldone = 1;
    tl_first=1;
    kbd_sched_delay(3000);                                                   
  }

 if(bracket_running)                                                              
 { 
 if(((gScriptFolder==1) && (strncmp(conf.script_file,"A/SDM/SCRIPTS2/K_.txt",18)==0))||(!gScriptFolder && (strncmp(conf.easy_script_file,"A/SDM/SCRIPTS/KAP.txt",18)==0))) 
 {
#if defined(CAM_HAS_IRIS_DIAPHRAGM)
  sprintf(buf," ISO %d  ND %s Mecha Pos %4d ",shooting_iso_real_to_market(shooting_get_iso_from_sv96(isoset)),(forceNDout)?"OUT":"IN",GetMechaPos());
#else
  sprintf(buf," ISO %d  Mecha Pos %4d ",shooting_iso_real_to_market(shooting_get_iso_from_sv96(isoset)),GetMechaPos());
#endif   
  draw_string(conf.synch_pos.x-5*FONT_WIDTH,conf.synch_pos.y+3*FONT_HEIGHT,buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),1);
  sprintf(buf,"%6s",get_tv_string_for_tv96(fastTv));
  draw_string(conf.synch_pos.x-5*FONT_WIDTH,conf.synch_pos.y+4*FONT_HEIGHT,buf,(fastTv>=gFastTv)?MAKE_COLOR(COLOR_GREEN,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),2);
 
  int fpl = lens_get_focus_pos_from_lens();
  if(fpl!= -1)sprintf(buf,"%6d mm",fpl);
  else sprintf(buf," INFINITY");
  draw_string(conf.synch_pos.x+9*FONT_WIDTH,conf.synch_pos.y+4*FONT_HEIGHT,buf,(fpl == -1)?MAKE_COLOR(COLOR_GREEN,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),2);
 }
  if((tl.shoot_mode==4)&&(get_movie_status() ==VIDEO_RECORD_STOPPED) && tl.blank_screen)
   {
    TurnOffBackLight();
    bls=0;
   }
 
   if((tl.shoot_mode==3)&& burstInProgress  && (tl.shot_count>=tl_target_shot_count)&& !conf.flag_1)
   {
    burstInProgress=0;
    usb_detect=0;
    KBD_STACK_PUSH(KEY_SHOOT_FULL);                                      
    KBD_STACK_PUSH(SCRIPT_RELEASE); 
    tl_target_shot_count+=tl.num_exposures;
    return 1;
   }
 
    if(dailyFinished && !conf.flag_1)                                          
   {
    dailyFinished=0; 
    if(tl.shutdown==1)                                                                       
    {
     kbd_sched_delay(2000);  
     power_down();
     return 1;  
    } 
    if(tl.blank_screen){TurnOnBackLight();bls=1;}   
    tl_first = 1;                                                                           
    if(!shooting_get_focus_mode())                                                         
     {                                                             
      UnlockAF();                                                                           
      afl=0;                                                                                
     }
    return 0;                                                                                
   }
 
   if((tl.shoot_mode==1)&& inf_focus && (tl.bracket_type!=2)) 
    {
      kbd_sched_release(KEY_SHOOT_FULL);                                     
       if(!tl.num_exposures && !alldone)
       {
        alldone=1;
        finished_time=(unsigned long)time(NULL);                             
       }  

      if(!tl.num_exposures && (((unsigned long)time(NULL)-finished_time)>2) )    
      { 
        if(!tl.bracket_type && !shooting_get_focus_mode())                       
         {
           UnlockAF();                                                           
           afl=0;                                                                
         } 
        if(tl.blank_screen){TurnOnBackLight();bls=1;}   
        console_add_line("! Sequence completed",1);
        conf.dist_mode=0;
        conf.tv_bracket_value=0;                                                
        if((tl.focus_mode==1)&&(tl.bracket_type==1))                             
        {
         lens_set_focus_pos(conf.distance_setting+fpd);                          
        }                                                  
        usb_detect = 0;
        bracket_running = 0;
        tl.ss=1;                                                                 
        shutter_int=0; 
        tldone = 1;
        tl_first = 1;                                                       
      }
        if(tl.num_exposures)
        {
         shutter_int=0;                                                             
         tl.ss=0;                                                                   
        }
      return 1;
    }
 

     if(alldone && (tl.bracket_type==2) && (((unsigned long)time(NULL)-finished_time)>(2+2*final_exposure))&& !tl.num_exposures && !focus_tv_be)
     {
        alldone=0;
        if(tl.blank_screen){TurnOnBackLight();bls=1;}  
        console_add_line("! Sequence completed",1);
        conf.dist_mode=0;                                               
        if((tl.focus_mode==1)&&(tl.bracket_type==1))                                
        {
         lens_set_focus_pos(conf.distance_setting+fpd);
         sprintf(buf,"# %d stack images",bracketing.shoot_counter+1);
         console_add_line(buf,2);
        }                                                
        sprintf(buf,"# Sequence took %3d secs", finished_time-auto_start+1);
        conf.tv_override=current_override;                                           
        conf.tv_override_enable = ctvoe;                                             
        usb_detect = 0;
        bracket_running=0;
        tldone = 1;
        conf.save_auto=log_status;
        tl_first = 1;
        kbd_sched_delay(3000);                                                   
     }


   if((tl.shoot_mode==1)&& shutter_int && (tl.bracket_type==2) && tl.shot_count) 
   {
     ++tv_bkt_cnt;
      if(--focus_tv_be)                                                          
      {
       kbd_sched_press(KEY_SHOOT_FULL);                                        
       kbd_sched_delay(2000);                                                    
       if(!tl.exposure_mode)conf.tv_override-=tl.ev_steps;                       
       if(tl.exposure_mode==1)conf.tv_override+=tl.ev_steps;                     
       if(tl.exposure_mode==2)                                                   
        {
          if(tv_bkt_cnt%2)conf.tv_override=start_tv+(tv_bkt_cnt/2)*tl.ev_steps;
          else conf.tv_override=start_tv-(tv_bkt_cnt/2)*tl.ev_steps;
        }
        if(conf.tv_override==19)conf.tv_override=18;                             
       } 
       final_exposure=(int)shooting_get_shutter_speed_from_tv96(shooting_get_tv96_from_index(conf.tv_override));

      if(!focus_tv_be && tl.num_exposures)                                       
      {
       tl.ss=0;                                                                   
       focus_tv_be = tl.focus_tv_exposures;                                       
       conf.tv_override=start_tv;                                                 
       tv_bkt_cnt=1;
      }                                                                                                                   
      kbd_sched_release(KEY_SHOOT_FULL);                                          
      shutter_int=0;                                                              

       if(!tl.num_exposures && !focus_tv_be && !alldone)                          
       {
        alldone=1;
        finished_time=(unsigned long)time(NULL);
        kbd_sched_release(KEY_SHOOT_FULL);   
       }
      return 1;                    

   }
 
  if(get_inf_focus() && (tl.shoot_mode==2) && tl.shot_count && (tl.bracket_type==2) && (((unsigned long)time(NULL)-auto_start)>4)) 
   {
     ++tv_bkt_cnt;
      if(--focus_tv_be)                                                           
      {
       auto_start=(unsigned long)time(NULL); 
       kbd_sched_click(KEY_SHOOT_FULL);                                        
       kbd_sched_delay(1000);                                                     
       if(!tl.exposure_mode)conf.tv_override-=tl.ev_steps;                        
       if(tl.exposure_mode==1)conf.tv_override+=tl.ev_steps;                      
       if(tl.exposure_mode==2)                                                    
        {
          if(tv_bkt_cnt%2)conf.tv_override=start_tv+(tv_bkt_cnt/2)*tl.ev_steps;
          else conf.tv_override=start_tv-(tv_bkt_cnt/2)*tl.ev_steps;
        }
        if(conf.tv_override==19)conf.tv_override=18;                             
       } 
      else                                                                         
      {
       tl.shot_count=0;
       tl.ss = 0;                                                                  
       if(tl.num_exposures)                                                        
        {
         focus_tv_be = tl.focus_tv_exposures;                                      
         conf.tv_override=start_tv;                                                
         tv_bkt_cnt=1;
        }
       else                                                                        
        {
          if(tl.blank_screen){TurnOnBackLight();bls=1;}  
          finished_time=(unsigned long)time(NULL);  
          console_add_line("! Sequence completed",1);
          conf.dist_mode=0;                                                                                              
          sprintf(buf,"# Sequence took %3d secs", finished_time-auto_start+1);
          console_add_line(buf,3);
          conf.tv_override=current_override;                                         
          conf.tv_override_enable = ctvoe;                                           
          usb_detect = 0;
          bracket_running=0;                                                              
          tl.shot_count=0;
          tldone = 1;
          tl_first = 1;
          lens_set_focus_pos(conf.distance_setting+fpd);                              
          delay_done=0;
          kbd_sched_delay(3000);                                                     
        }                                                                    
      }                                                                                                                                                     
      return 1;  
   }


  if(get_inf_focus() && (tl.shoot_mode==2) && (tl.bracket_type!=2) && !tl.num_exposures && (((unsigned long)time(NULL)-auto_start)>3)) 
   {
       finished_time=(unsigned long)time(NULL);
       console_add_line("! Sequence completed",1);
       conf.dist_mode=0;
       conf.tv_bracket_value=0;
       if(tl.blank_screen){TurnOnBackLight();bls=1;}  
        if(!tl.bracket_type && !shooting_get_focus_mode())                           
         {
           UnlockAF();
           afl=0;
         } 
       if(tl.bracket_type==1)lens_set_focus_pos(conf.distance_setting+fpd);         
       sprintf(buf,"# Sequence took %3d secs", finished_time-auto_start+1);
       console_add_line(buf,3);
       usb_detect = 0;
       bracket_running = 0;                                                              
       tldone = 1;
       tl_first = 1;
       kbd_sched_delay(3000);                                                       
       return 1;
   } 

   if(!tl.num_exposures && !tl.endless_mode && !tl.shoot_mode) 
    {
     if((!tl.shutdown || ((tl.shutdown==1)&& sd))&& (((unsigned long)time(NULL)-auto_start)>3))                                           
     {
      if(!shutter_int && !(tl.shoot_mode==1))
      {
       if(tl.blank_screen)
       {   
        TurnOnBackLight(); 
        bls=1;        
       } 
      }
      usb_detect = 0;
      bracket_running = 0;  
      tl_first = 1;
      if(!shooting_get_focus_mode())                                                         
      {                                                             
       UnlockAF();                                                                           
       afl=0;                                                                                
      }
       return 0;                                                                             
     }  
     else if((tl.shutdown==1)&& !sd)                                                         
     {
      sd=1;                                                                                  
      kbd_sched_delay(2000);
      power_down();
      return 1;
     }                                                                            
    }
   if((shooting_get_prop(PROPCASE_FLASH_MODE)!=2) && shooting_get_drive_mode() && !gFlashDisable) 
   {                                                                                              
    play_sound(6); 
    console_add_line("! Flash must be OFF",1);
    console_add_line("# Full-press to finish",2);
    bracket_running = 0;                                                                          
    return 1;
   }

   if(!tl.shoot_mode && shooting_get_drive_mode())                                           
   {
    play_sound(6); 
    console_add_line("! Not single-shot mode",1);
     console_add_line("# Full-press to finish",2);
    bracket_running = 0;                                                                          
    return 1;
   }

   if(((tl.shoot_mode==1)||(tl.shoot_mode==3)) && (shooting_get_drive_mode()!=1))            
   {
    play_sound(6); 
    console_add_line("! Not continuous mode",1);
    console_add_line("# Full-press to finish",2);
    bracket_running = 0;                                                                          
    return 1;
   }

   if((tl.shoot_mode==2) && (shooting_get_drive_mode()!=3))                      
   {
    play_sound(6); 
    console_add_line("! Not timer mode",1);
    console_add_line("# Full-press to finish",2);
    bracket_running = 0;                                                                          
    return 1;
    }
   
   if((tl.bracket_type>=1) && !shooting_can_focus())                                         
   {
    play_sound(6); 
    console_add_line("! Cannot override focus !",1);
    console_add_line("# Full-press to finish",2);
    bracket_running = 0;                                                                          
    return 1;
    }

         
 if(tl.blank_screen)                                                    
    {
     if(!(--led_count)) 
     {
      if(conf.ricoh_ca1_mode)                                                    
       debug_led(0);                                                            
      else ubasic_set_led(9,0,0);                                               
     }
     if((unsigned long)time(NULL)==tl.led_time)
     {
      if(conf.ricoh_ca1_mode )
       debug_led(1);
      else ubasic_set_led(9,1,100);
#if !defined(CAMERA_a620)
      play_sound(4); 
#endif
      led_count = 20;
      tl.led_time+=30;
     }
    }

  current_time_left=tl.target_time-(unsigned long)time(NULL);

 
 t=time(NULL);                                                            
   ttm = localtime(&t); 
   static unsigned char valid;
   valid=0;
   if((unsigned long)t >=tl.target_time)                               
   {
    tl.target_time+= tl.shoot_interval;
    if(conf.save_raw)
     secremain=tl.shoot_interval*(GetRawCount()-1);
    else
     secremain=tl.shoot_interval*(GetJpgCount()-1);
    auto_start=(unsigned long)time(NULL);                                      
 
  unsigned char daily = (gStartHour&&gEndHour&&gStartMinutes&&gEndMinutes);   
  if(daily)
  {
   int min_now=ttm->tm_min+(ttm->tm_hour*60);
   if(end_in_minutes>start_in_minutes)                                    
    {
     if((min_now>=start_in_minutes)&&(min_now<=end_in_minutes))valid=1;
     else valid=0;
    }
    else                                                                  
    {
     if((min_now>=start_in_minutes)||(min_now<end_in_minutes))valid=1;
     else valid=0;
    }
   if(min_now==end_in_minutes) dailyFinished=1;                                                
  }

  if(bls&&tl.blank_screen){TurnOffBackLight();bls=0;} 
 
  if ((tl.shoot_mode==3)&&((daily&&valid)||!daily))	                        
     {      
      KBD_STACK_PUSH(KEY_SHOOT_FULL);                                           
      KBD_STACK_PUSH(SCRIPT_PRESS);
      KBD_STACK_PUSH(SCRIPT_FOCUSED);                                            
      KBD_STACK_PUSH(KEY_SHOOT_HALF);
      KBD_STACK_PUSH(SCRIPT_PRESS); 
       conf.flag_1=1;                                                            
       burstInProgress=1;
       prev_time_left=tl.shoot_interval;                                                     
      return 1;
     }                                      
     prev_time_left=tl.shoot_interval;

     if(!tl.endless_mode)                                                       
      {
       if(!(--tl.num_exposures))finished_time=(unsigned long)time(NULL);         
      }
    
    if(!tl.shoot_mode && ((daily&&valid)||!daily))  
    {
     if(conf.ricoh_ca1_mode)
     {
      if(af_all>=0)                       
       {
        KBD_STACK_PUSH(KEY_SHOOT_HALF);
        KBD_STACK_PUSH(SCRIPT_RELEASE);
        kbd_sched_delay(100);
       }
      KBD_STACK_PUSH(KEY_SHOOT_FULL_ONLY);
      KBD_STACK_PUSH(SCRIPT_RELEASE);
      kbd_sched_delay(100);
      KBD_STACK_PUSH(KEY_SHOOT_FULL_ONLY);
      KBD_STACK_PUSH(SCRIPT_PRESS);
      if(af_all>=0)                       
      {
       KBD_STACK_PUSH(SCRIPT_FOCUSED);   
       KBD_STACK_PUSH(KEY_SHOOT_HALF);
       KBD_STACK_PUSH(SCRIPT_PRESS); 
      }
     }
    }

    else if((tl.shoot_mode==2)&& ((daily&&valid)||!daily)) {kbd_sched_click(KEY_SHOOT_FULL);}  
     else if ((tl.shoot_mode==1)&& ((daily&&valid)||!daily))
     {
      kbd_sched_press(KEY_SHOOT_FULL);   
      KBD_STACK_PUSH(SCRIPT_FOCUSED);   
      KBD_STACK_PUSH(KEY_SHOOT_HALF);
      KBD_STACK_PUSH(SCRIPT_PRESS); 
      conf.flag_1=1;                   
     }
     
     else if ((tl.shoot_mode==4)&& ((daily&&valid)||!daily))               
     {
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
	    kbd_sched_click(KEY_VIDEO);
#else
       kbd_sched_click(KEY_SHOOT_FULL);
#endif
	    kbd_sched_delay(1000*tl.num_exposures+1000);
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
	    kbd_sched_click(KEY_VIDEO);
#else
       kbd_sched_click(KEY_SHOOT_FULL);
#endif
      movieStart=time(NULL);
      gFramesRecorded=0;
     }
    }
    if(time(NULL)>movieStart+3)usb_detect=0; 
 current_time_left=tl.target_time-(unsigned long)time(NULL);
 
 
 if((current_time_left<prev_time_left) && (!tl.blank_screen|| (tl.blank_screen && !tl.shot_count)) && tl.num_exposures) 
   {
    sprintf(buf,"%5d",current_time_left);
    draw_string(pos_x,pos_y,buf, tr,2);
    sprintf(buf,"%5d",(tl.endless_mode)?tl.shot_count:tl.num_exposures);
    draw_string(pos_x,pos_y+(2*FONT_HEIGHT),buf, cc,2);
    prev_time_left=current_time_left;
   }
   
    if(tl.blank_screen && tl.shot_count)
   {
      TurnOffBackLight();
      bls=0;
   } 
  }  
 
   return 1;
}

static int gUsbDuration;

int getUsbDuration()
{
 int pwidth = 0;
 pwidth = gUsbDuration;
 gUsbDuration = 0;
 return pwidth;
}

void usb_high_count()            
{
  static int USBcnt;
   if(get_usb_bit())++USBcnt;
   else if (USBcnt)
   {
    gUsbDuration = USBcnt;
    USBcnt=0;
   }
 }

void set_key_press(int nSet)
{
	key_pressed = nSet;
	kbd_blocked = nSet;
	nRmt = nSet;
}
 

long kbd_process()
{
 if(!ConfRestoreFinished)return 0;
#if defined(KEY_POWER)
if((gui_get_mode()==GUI_MODE_ALT)&& gPowerPressed)
 {
  kbd_blocked = 0;
  gui_kbd_leave();
 }
#endif

static int nCER=0;
static int nReverseDelay=0;
static unsigned char FastExitCount;
static long time_full_pressed;         
static unsigned int lowCount,highCount,pulseCount,pulseTime;


unsigned int mmode=0;
unsigned int nCrzpos,i;
unsigned int drmode = 0;
  lastClickedKey=kbd_get_clicked_key();
 
 static clsDirection=1;
 
if(clearScreen && !state_kbd_script_run && !is_video_recording())
{
clearScreen=0;
shooting_set_mode_canon(shooting_get_prop(PROPCASE_SHOOTING_MODE));
}

   if(conf.ricoh_ca1_mode)
  {
	drmode = shooting_get_drive_mode();
	mmode = mode_get();
	mplay = (mmode&MODE_MASK)==MODE_PLAY;
   mvideo=MODE_IS_VIDEO(mmode&MODE_SHOOTING_MASK);
  }
    if(conf.sleep)
     {
      int tc = shooting_get_tick_count();
      if(tc > (conf.user_2+60000))
       {            
        conf.user_2=tc;
        play_sound(4);
       }
     }
    if(conf.sleep && kbd_is_key_pressed(KEY_SHOOT_HALF)&& !kbd_is_key_pressed(KEY_SHOOT_FULL))
    {
     conf.sleep=0;
     conf.alt_prevent_shutdown=conf.user_3;   
     conf.user_2=conf.user_3=0;				    
     bls=1;
     TurnOnDisplay();
     gui_set_need_restore();
     kbd_key_release_all();          
     return 1;
     }
     
   if((shooting_get_tick_count()< (recStart+3000))&& kbd_is_key_pressed(KEY_MENU)&&!conf.splash_show)
   {
    kbd_key_release_all();          
    return 1;
   }
 
  if(mvideo && !conf.video_frame_count && !is_video_recording()/* && (movie_status!=VIDEO_RECORD_STOPPING)*/ && (gFullPressed||(kbd_is_key_pressed(KEY_VIDEO))))
  {
   gFramesRecorded=0;
   gVideoStartTime=shooting_get_tick_count();
  }
 
  if(conf.splash_show && (shooting_get_tick_count()>3000) && !welcomeDone && mplay)
  {
   struct stat st;
   kbd_key_release_all();
   if(stat("A/SDM/TEXTS/focal.txt", &st)!=0)
    ZoomFocalLengths();
   welcome_menu();
   return 1;
  }
 
 if(skey_pressed)
            {
               if (kbd_is_key_pressed(conf.alt_mode_button) ||                                       
                        ((skey_pressed >= CAM_EMUL_KEYPRESS_DELAY) &&                               
                         (skey_pressed < CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION)))      
                {
                        if (skey_pressed <= CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION)      
                                skey_pressed++;
                        if (skey_pressed == CAM_EMUL_KEYPRESS_DELAY)                                 
                                kbd_key_press(conf.alt_mode_button);
                        else if (skey_pressed == CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION) 
                                kbd_key_release(conf.alt_mode_button);
                        return 1;                                                                   
                } 
                else                                                                                
                if(kbd_get_pressed_key() == 0)                                                     
                {
                 if(skey_pressed<CAM_EMUL_KEYPRESS_DELAY)                                          
                  {  
                   play_sound(5);                                                                  
                   msleep(500);
                   play_sound(5);
                   if((displayedMode==6)||(displayedMode==6)||((displayedMode>8) && (displayedMode<15))||((displayedMode>30) && (displayedMode<34)))               
                    {
                     if((strcmp("A/SDM/SCRIPTS3/ZOOM_3D.TXT",conf.user_script_file)==0)||(strcmp("A/SDM/SCRIPTS3/ZOOM3DDE.TXT",conf.user_script_file)==0)) 
                     {
                     if(gScriptFolder!=2)                                                          
                     {
                      gScriptFolder=2;
                      script_load(conf.user_script_file,1);
                     }
                     kbd_blocked=1;                                                                
                     gui_kbd_enter();
                     script_start();
                     }
                     else
                     {
                      user_scripts_menu();
                     }
                    }    
                   else
                    {                  
                     user_scripts_menu();                               
                    }
                   skey_pressed = 0;                                                            
                   draw_restore();
                  }
                 else skey_pressed = 0; 
                 kbd_key_release_all();
                 return 1;                                                                   
                }
                kbd_key_release_all();
                return 1;                                                                       
                }
 
     if(!mplay)
      {
        if (kbd_is_key_pressed(conf.alt_mode_button)&&((gui_mode==GUI_MODE_NONE)||(!state_kbd_script_run&&conf.alt_files_shortcut&&(gui_mode==GUI_MODE_ALT)&&(gScriptFolder==2))))            
        {
                skey_pressed = 1;
                kbd_key_release_all();          
                return 1;
        }
       }      
     
       
       if(gFastExit)
       { 
        ++FastExitCount;
        if(FastExitCount==1)draw_restore();
        else if(FastExitCount>100)
         {
          gFastExit=0;
          FastExitCount=0;
          kbd_blocked=0;
          gui_kbd_leave();
         }
       }
 
        if (key_pressed && !nRmt && !kbd_blocked)
        {
                if (kbd_is_key_pressed(KEY_MENU) ||                                    
                        ((key_pressed >= CAM_EMUL_KEYPRESS_DELAY) &&                               
                         (key_pressed < CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION)))      
                {
  
                        if (key_pressed <= CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION)      
                                key_pressed++;                                                      
                        if (key_pressed == CAM_EMUL_KEYPRESS_DELAY)                                 
                                kbd_key_press(KEY_MENU);
                        else if (key_pressed == CAM_EMUL_KEYPRESS_DELAY+CAM_EMUL_KEYPRESS_DURATION) 
                                kbd_key_release(KEY_MENU);
                        return 1;                                                                   
                } 
                else                                                                                 
                if (kbd_get_pressed_key() == 0)                                                     
                {
                       if (key_pressed != 100 && (key_pressed < CAM_EMUL_KEYPRESS_DELAY))          
                        {
                           kbd_blocked = 1;
                           gui_kbd_enter();
                        }
                        key_pressed = 0;                                                            
                        return 1;                                                                   
                }
                return 1;                                                                          

        }                 

        
       if(kbd_is_key_pressed(KEY_MENU)&&!kbd_is_key_pressed(KEY_SHOOT_HALF)&&(gui_mode==GUI_MODE_NONE))
        {
                key_pressed = 1;
                kbd_key_release_all();          
                return 1;
        }
		
	   if(kbd_is_key_pressed(KEY_MENU)&&kbd_is_key_pressed(KEY_SHOOT_HALF)&&(gui_mode==GUI_MODE_NONE))
        {
			
                key_pressed = 0;
                kbd_key_release_all();          
                return 1;
        }

        
       if (kbd_blocked && (nRmt==0))                                            
        {
         if(kbd_is_key_pressed(KEY_MENU)){++gMenuPressed;}
	      if (kbd_is_key_clicked(KEY_SHOOT_FULL)) 		
  {
           play_sound(4);
	        key_pressed = 100;
	        if (!state_kbd_script_run)
             {
              script_console_clear();
              conf.cs=1;                                                       
		  script_start();
	       } 
 
            else if (state_kbd_script_run == 2) 
             {
              script_end();
             }

            else                                              
             {
              state_kbd_script_run = 2;
              if (jump_label("restore") == 0)                  
               {              
      
		          script_end();
	         }
            
	        }
      
            }
                if (state_kbd_script_run) process_script();                         
                else gui_kbd_process();                                             
        }  
 
      else           
  {
  
   if(shooting_get_tick_count()>8000)
    {
     int vm;
     get_property_case(PROPCASE_SHOOTING_MODE, &vm, 4);
    }

                   
if(nWt>0) {nWt--;return 1;}


#if CAM_NEED_HP
if(nConffin==1){nConffin=0;nFirst=1;}
if(nFirst==1)
{
 sinceBoot=shooting_get_tick_count()-sinceBoot;
    if(nSW==0)
    {
        nSW=1;
#if !defined(CAMERA_g9)
        nWt=50;
#else
        nWt=50;
#endif
        kbd_key_release_all();
        kbd_key_press(KEY_SHOOT_HALF); 
        soft_half_press = 1;
        set_key_press(1);
        return 1;
    }
    else if(nSW==1)
    {
        nSW=2;
        nWt=10;
        kbd_key_release(KEY_SHOOT_HALF); 
        soft_half_press = 0;
        set_key_press(1);
        gEnableScripting=shooting_get_tick_count(); 
        return 1;
    }
    else if(nSW==2)
    {
        set_key_press(0);
        nWt=10;
        nSW=0;
        nFirst=0;
        return 1;
    }
}
#endif
 
 if(conf.ricoh_ca1_mode && (shooting_get_tick_count()>(recStart+5000)))  
  {

   
if((gDetectUsb==666)&&get_usb_bit() && !conf.on_release && (gSDMShootMode==STEREO)){kbd_key_release_all();return 1;}
if(gDetectUsb==666)gDetectUsb=0;

if(mplay && kbd_is_key_pressed(conf.anaglyph_button)&& conf.stereo_mode ) 
{
 nWt=100;
 kbd_key_release_all();      
 stereo_pair();
 return 1;
} 
 

 if (gHalfPressed && gLeftPressed && !mplay)
 {
  ++nCount2;
  if(nCount2>20)
   {
    ael = !ael;
    if(ael)DoAELock();
    else UnlockAE();
    nCount2=0;
   }
    kbd_key_release_all();
    return 1;
 }
 
 if (gHalfPressed && gRightPressed && !mplay)
 {
  ++nCount2;
  if(nCount2>20)
   {
    if(!shooting_get_focus_mode())
    {
     afl = !afl;
     if(afl)DoAFLock();
     else UnlockAF();
    }
    else if (shooting_get_focus_mode()==1)
    {
     UnlockMF();
     afl=0;
    }
    nCount2=0;
   }
  kbd_key_release_all();
  return 1;
 }
 
if(inf_focus && (shooting_get_drive_mode()!=3))		
{
 			nSW=116;
			nWt=20;
			kbd_key_release(KEY_SHOOT_FULL); 
                  set_key_press(1);
                  soft_half_press = 0;
                  conf.tv_override=(gTv96max>=0)?19+(gTv96max/32):18+(gTv96max/32);  
           return 1; 
}
 
if(gStopVideoCapture && !nSW)  
 {
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
  kbd_key_press(KEY_VIDEO);                                                    
#else
  kbd_key_press(KEY_SHOOT_FULL);           
#endif 
  nSW=1;
  nWt=20;
  return 1;
 }
if((nSW==1)&& gStopVideoCapture)
 {
  nSW=0;
  nWt=50;
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)  
  kbd_key_release(KEY_VIDEO); 
#if defined(CAMERA_m3)
movie_status=VIDEO_RECORD_STOPPED;
#endif                                                     
#else
  kbd_key_release(KEY_SHOOT_FULL);           
#endif 
 
  if(conf.video_frame_count)
   gFrameCount = conf.video_frame_count;
  gStopVideoCapture=0;
  return 1;
 }
 
 if(gLowLightCount && conf.burst_frame_count && (gLowLightCount>=conf.burst_frame_count))
 {
  nSW=0;
  nWt=50;
  set_key_press(0); 
  kbd_key_release(KEY_SHOOT_FULL);
  debug_led(0);			
  gLowLightCount=0;
  TurnOnDisplay();
  return 1;
 }
 
if(exitWelcome==1){exitWelcome=0;nFirst=1;}
if(nFirst==1)
{
    if(nSW==0)
    {
        nSW=1;
        nWt=50;
        kbd_key_release_all();
        kbd_key_press(KEY_SHOOT_HALF); 
        soft_half_press = 1;
        set_key_press(1);
        return 1;
    }
    else if(nSW==1)
    {
        nSW=2;
        nWt=10;
        kbd_key_release(KEY_SHOOT_HALF); 
        soft_half_press = 0;
        set_key_press(1);
        return 1;
    }
    else if(nSW==2)
    {
        set_key_press(0);
        nWt=10;
        nSW=0;
        nFirst=0;
        return 1;
    }
}

#if !defined(CAMERA_m3)
        if((nSW==10)&& conf.remote_zoom_enable && !nCount2 && ((mmode&MODE_MASK)==MODE_REC))     
    {
	  nTxzoom=1; 
      nSW=0;
     gSZMode=1;
     nCount2=0;  
	  debug_led(1);
 #if !defined(CAMERA_sx200is)&& !defined(CAMERA_sx240hs)&& !defined(CAMERA_sx260hs)&& !defined(CAMERA_sx280hs)   
     nTxzname==KEY_ZOOM_IN;
 #else
     nTxzname==KEY_ZOOM_IN_SLOW;
 #endif
    }
 #endif 

    if((nSW==11)&& conf.remote_zoom_enable && !nCount2 /*&& ((mmode&MODE_MASK)==MODE_REC)*/)     
    {
     nSW=0;
     gSZMode=0;
     nCount2=0;  // zoom count 
     if(!mvideo)	 
      shooting_set_mode_chdk(9);
	 else
	  shooting_set_mode_chdk(SyncRecMovMode);
    }	
	
    if((nSW==12)&& conf.remote_zoom_enable && !nCount2 && !is_video_recording())     
    {
      nCa=2;
      nSW=115; 
      nWt=50;
      shutter_int=0;   
      if(((get_movie_status()==VIDEO_RECORD_STOPPED)||(get_movie_status()==6))&& conf.video_frame_count && (gFrameCount<=2) && mvideo){gFrameCount=conf.video_frame_count;}
      else if(((get_movie_status()==VIDEO_RECORD_STOPPED)||(get_movie_status()==6))&& !conf.video_frame_count && mvideo){gFramesRecorded=0;} 
      if(!(srf && !conf.srb))
#if !defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
       kbd_key_press(KEY_SHOOT_FULL); 
#else
       {
        if(mvideo||is_video_recording())
          kbd_key_press(KEY_VIDEO);
        else
          kbd_key_press(KEY_SHOOT_FULL); 
        }      
#endif
         gVideoStopTime=shooting_get_tick_count();
         set_key_press(1);          
	     nCount=0; 
         gDetectUsb=(unsigned long)time(NULL);  
	     return 1;  
      }

	if (kbd_is_key_pressed(KEY_SHOOT_HALF) && nTxzoom>0) 
          {
		nCount2=0;
		nTxzoom=0;
		nReczoom=0;
		nTxvideo=0;
		debug_led(0);
      gSZMode=0;
	    }

	if (mplay && (kbd_is_key_pressed(KEY_LEFT) || kbd_is_key_pressed(KEY_RIGHT)))
           {
		nPlyname=KEY_LEFT;
		if(kbd_is_key_pressed(KEY_RIGHT)) nPlyname=KEY_RIGHT;
		}

    if(conf.remote_zoom_enable>0 && ((mmode&MODE_MASK)==MODE_REC))     
    {
#if !defined(CAMERA_sx200is)&& !defined(CAMERA_sx240hs)&& !defined(CAMERA_sx260hs)&& !defined(CAMERA_sx280hs)
	if (kbd_is_key_pressed(KEY_ZOOM_IN) || kbd_is_key_pressed(KEY_ZOOM_OUT)) 
         {
		nCount2=0;                                                
		nTxvideo=0;
		if(kbd_is_key_pressed(KEY_ZOOM_IN))                       
                 {
			if(nTxzname==KEY_ZOOM_IN) nTxzoom++;                
			else nTxzoom=1;                                     
			nTxzname=KEY_ZOOM_IN;                               
         ZoomIn=1;
			}
		else {
			if(nTxzname==KEY_ZOOM_OUT) nTxzoom++;               
			else nTxzoom=1;                                     
			nTxzname=KEY_ZOOM_OUT;                              
         ZoomIn=0;
			}
#else
	if (kbd_is_key_pressed(KEY_ZOOM_IN_SLOW) || kbd_is_key_pressed(KEY_ZOOM_OUT_SLOW)) 
         {
		nCount2=0;                                                
		nTxvideo=0;
		if(kbd_is_key_pressed(KEY_ZOOM_IN_SLOW))                       
                 {
			if(nTxzname==KEY_ZOOM_IN_SLOW) nTxzoom++;                
			else nTxzoom=1;                                     
			nTxzname=KEY_ZOOM_IN_SLOW;                               
         ZoomIn=1;
			}
		else {
			if(nTxzname==KEY_ZOOM_OUT_SLOW) nTxzoom++;               
			else nTxzoom=1;                                     
			nTxzname=KEY_ZOOM_OUT_SLOW;                              
         ZoomIn=0;
			}

#endif

		if(nTxzoom<35)                                            
                 {
			kbd_key_release_all();
                  gSZMode=1;
			debug_led(1);
			}
		else {                                                    
			debug_led(0);
                  gSZMode=0;
			return 0;
			}
		return 1;
	   }
     }


	if(!get_usb_power(1) && nSW<100 && nCount==0 && nTxzoom>0) 
           {
		nCount2++;                                            
		if(nCount2>conf.zoom_timeout*100)                     
                 {
			 if(nTxzoom>0)
                       {
                        UsbPulses=0;
				nTxzoom=0;
				nReczoom=0;
				debug_led(0);
                gSZMode=0;				
				}
			 nCount2=0;
			}
		return 1;
	     }

 
 if((((gSDMShootMode==BURST)&&(get_usb_power(1)||gHalfPressed))||(((displayedMode&&(displayedMode<=4))||((displayedMode>=19) && (displayedMode<=22)))&&get_usb_power(1)))&& !shutter_int && !mvideo&& nSW<100 && nCount==0)
 {
  if(recStart && (shooting_get_tick_count()> (recStart+3000)))  
   {
       conf.synch_enable=0;
       nCa=2;
	 nSW=115;  
	 nWt=100;
       shutter_int=0;
       kbd_key_press(KEY_SHOOT_FULL);  
	 nCount=0; 
       return 1;
      }
   }
	else if(get_usb_power(1) && nSW<100 && nCount==0)                                                         
       {
        nCount2=0;
        kbd_key_release_all();
        conf.synch_enable=1; 
       } 
 unsigned char cond1,cond2;
  cond1 = (conf.dist_mode && !shooting_get_drive_mode()&& (shooting_get_prop(PROPCASE_FLASH_MODE)==2))||((conf.dist_mode==2)&&(shooting_get_drive_mode()==1));                    
  cond2 = (gSDMShootMode==7)&& is_video_recording() && !nTxzoom;     
 
 if(get_usb_bit() && (cond1||cond2)&& !mplay)
        {   
         if(((unsigned long)time(NULL))>(gDetectUsb+conf.lockout_time))  
          {
           nCa=2;
           nSW=115; 
           nWt=50;
           shutter_int=0;  
           if(((get_movie_status()==VIDEO_RECORD_STOPPED)/*||(movie_status==6)*/)&& conf.video_frame_count && (gFrameCount<=2) && mvideo){gFrameCount=conf.video_frame_count;}
           else if(((get_movie_status()==VIDEO_RECORD_STOPPED)/*||(movie_status==6)*/)&& !conf.video_frame_count && mvideo){gFramesRecorded=0;} 
 if(!(srf && !conf.srb))
 {
  if(is_video_recording())   
  {
	gStopVideoCapture=1;
	nSW=0;
    gVideoStopTime=shooting_get_tick_count();
#if defined(CAMERA_m3)
      movie_status=VIDEO_RECORD_STOPPED;
#endif  
  }
  else
  {
   kbd_key_press(KEY_SHOOT_FULL);   
    set_key_press(1);
  }	
    return 1;    
 }
 }   
        else 
          {
           set_key_press(0);
           nWt=50;
	     nSW=0;
	     nCa=0;        
           nCount=0; 
	     return 1; 
          }
        }                                                                                              
	if(get_usb_power(1) && nSW<100 && nCount<6)                                                         
      {
       nCount++;
		return 1;                                                                                     
		}
	if(nCount>0 && nSW<100)                                                                             
          {
		if(mplay)                                                                                                                                                                        
                {
			if(get_usb_power(1)) return 1;                                                          
			kbd_key_release_all();                                                                  
                  if(playback) kbd_key_press(KEY_SET);                                                    
			else kbd_key_press(nPlyname);                                                                                                                      
			set_key_press(1);         
         
			nCount=0;                                                                                                                                                              
			nCa=2;                                                                                  
			nSW=101;
			nWt=5;
			return 1;                                                                               
		    }                                                                                         
		   if(nTxvideo>49) nTxvideo=0;
		   if(nCount<5)													    
                {
                 nCa=1;
                 gSwitchType=RICOH;	                                                                
                }
		   else {nCa=2;gSwitchType=STANDARD;}

				                                                          
         
		   nCount=0;                                                                                  
		   nSW=109;                                                                                   
	    }  

	if(nCa==2)
         {
     		if(nSW==101)
                {
			kbd_key_release_all();                                                                 
			set_key_press(0);
			nWt=50;
			nSW=0;
			nCa=0;
			return 1;                                                                              
		    } 

		if(nSW==109)
                {
                 nCER=0;
                 
			if(nTxzoom>0)                                                                                   
                      {
                       if(get_usb_bit())        
                        {
                         highCount++;
                         if(lowCount)lowCount=0; 
                         return 0;
                        }
                        else
                        {
                         lowCount++;            
                         if(highCount){pulseCount++;highCount=0;} 
                        }
                        if(pulseCount && (lowCount<60))    
                         {
                          return 0;
                         }
                         
                            if(pulseCount && (lowCount>=60))
                            {
                             UsbPulses=pulseCount;
                              pulseCount=0;
                              lowCount=0;
                              
                               if(UsbPulses==2)
                              {
                               UsbPulses=0;
                               ZoomIn=!ZoomIn;
                              }
                              
                              
                              else if((UsbPulses==1) ||(UsbPulses>=3)  )  
                              {
                             
                              if(UsbPulses >=3)
                               {
                                if(ZoomIn)
                                 {
                                  if(nTxtblcr<(ZSTEP_TABLE_SIZE-1)) 
                                   nTxtblcr=(ZSTEP_TABLE_SIZE-2);
                                  else                               
                                   {
                                   nTxtblcr=1;
                                    ZoomIn=0;
                                   }
                                  }
                                else   
                                 {
                                  if(nTxtblcr) 
                                  {
                                   ZoomIn=0;
                                   nTxtblcr=1;
                                   }
                                  else            
                                   {
                                    nTxtblcr=(ZSTEP_TABLE_SIZE-2); 
                                    ZoomIn=1;
                                   }
                                 }
                               } 
                              
                              if(ZoomIn)                                                         
                                  {
						nTxtblcr++;                                                                    
						if(nTxtblcr>(ZSTEP_TABLE_SIZE-1)) 
                    {
                     nTxtblcr=(ZSTEP_TABLE_SIZE-2);                                               
                     ZoomIn=0;                   
                    }
					    }
					  else                                                                               
                   {                                                                                
					     nTxtblcr--;                                                                    
					     if(nTxtblcr<0) 
                     {
                      nTxtblcr=1;                                                                  
                      ZoomIn=1;                      
                     }
					    }
                 gSyncZoomStep=nTxtblcr;
                 shooting_set_zoom(nTxtbl[nTxtblcr]); 
                 play_sound(4);
                 UsbPulses=0;
                 } 
               }
               debug_led(1); 
               gSZMode=1;
               nSW=0;
               nCount=0;
               nTxzoom=1;                                                                          
               nWt=70;
               return 0;                                                                      
               }                                                                                                                     
				  nTxzoom=0;                                                                              
				  nReczoom=0;                                                                             
			                                                                                               
                                  
                    if((((conf.dist_mode||conf.tv_bracket_value)&&state_kbd_script_run))&&!mvideo)         	
                  {
                    nSW=114;
                    nWt=2;
                    kbd_key_release_all();                                                                      
                    set_key_press(1);
                    return 1;                                                                                   
                  }
         		nSW=110;
			nWt=2;
			kbd_key_release_all();                                                                        
			if(!(srf && !conf.srb)&& !is_video_recording())kbd_key_press(KEY_SHOOT_HALF);                                        
                  soft_half_press = 1;
                  set_key_press(1);
			return 1;                                                                                     
	          } 
 
		if(nSW==110)
                  {
                    if (shooting_in_progress() || nCER>100) 
                      {
                       if(mvideo && !conf.video_frame_count && !is_video_recording())gFramesRecorded=0;         
				            nCER=0;
                        nSW=113;
			    }
			  else {nCER++;return 1;}
			}
        
		if(nSW==113)
                 {
                  if(!conf.dc && get_usb_bit() && !mvideo) nSW=114;         
                  else
                   {
                      if(get_usb_bit())        
                        {
                         nCount++;
                         if(lowCount)lowCount=0; 
                        }
                       else
                        {
                         lowCount++;            
                         if(nCount){pulseCount++;pulseTime=nCount;nCount=0;} 
                        }

                        if(pulseCount && (lowCount>=50))
                         {                           
                          UsbPulses=pulseCount;
                          pulseCount=0;
                         }
                     }
                   if(kbd_is_key_pressed(KEY_SHOOT_HALF)|| (conf.dc && !nTxzoom && (UsbPulses==1) &&(pulseTime<40)))    
                   {
                    kbd_key_release(KEY_SHOOT_HALF);
                    nCount=0;                                      
                    set_key_press(0);
                    nWt=50;                                        
                    nCa=0;
                    UsbPulses=0;
			 if(mvideo && !is_video_recording()) 
			  {
			   gStopVideoCapture=1;
#if defined(CAMERA_m3)
                     movie_status=VIDEO_RECORD_IN_PROGRESS;
#endif   	
			   nSW=0;
			  }
			  else nSW=0; 
                    return 1;                                      
                   }

                 else if(UsbPulses==2 && pulseTime>5 && conf.remote_zoom_enable && ((mmode&MODE_MASK)==MODE_REC))
                   {
                    play_sound(4);
                    nCount=0; 
                    UsbPulses=0;                                     
                    set_key_press(0);
                    nWt=50;                                        
                    nSW=10;
                    return 1; 
                   }

                 else if(UsbPulses==3 && pulseTime>5 && conf.remote_zoom_enable && ((mmode&MODE_MASK)==MODE_REC))
                   {
                    nCount=0; 
                    UsbPulses=0;                                     
                    set_key_press(0);
					if(!mvideo)SyncRecMovMode=mode_get()&MODE_SHOOTING_MASK;
                    nWt=50;                                        
                    nSW=11;
                    return 1; 
                   } 
 
                  else if(conf.dc && get_usb_bit() && (nCount >=90) && !mvideo) 
                   {
                    nCount=0;
                    UsbPulses=0;
                    nSW=114;
                   }          
                  else return 1;                                               
		     }
		if(nSW==114)
                 {
			nSW=115;
			nWt=2;
                  shutter_int=0;
			if(!(srf && !conf.srb))
                   {
                    if((get_movie_status()==VIDEO_RECORD_STOPPED)&& conf.video_frame_count && (gFrameCount<=2) && mvideo){gFrameCount=conf.video_frame_count;}	
#if !defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
           kbd_key_press(KEY_SHOOT_FULL);                                           
#else         
            if(mvideo)
			{
             kbd_key_press(KEY_VIDEO); 		 
			}
            else
             kbd_key_press(KEY_SHOOT_FULL);        
#endif
                    time_full_pressed=(unsigned long)time(NULL);          
                   }
                  set_key_press(1);
			nCount=0;                                               
			return 1;                                               
			}
		if(nSW==115)
                  {

                   if(drmode==1 && (!shutter_int && (((conf.bracket_type>2)&& conf.tv_bracket_value)||conf.dist_mode)))               
                    {
			   return 1;
			  }

                    else if(drmode==1 && !shutter_int && (!conf.tv_bracket_value || conf.fastlapse) && !conf.dist_mode && (conf.bracket_type<=2)) 
                    {
                     return 1;
                    }

                   else if(drmode==1 && !shutter_int && !conf.tv_bracket_value && !conf.dist_mode && (conf.bracket_type>2)&& !gHalfPressed && !get_usb_power(1)&&(((unsigned long)time(NULL)-time_full_pressed)>1)) 
                    {
                     return 1;
                    }

			nSW=116;
			nWt=2;
			if(!(srf && !conf.srb))
#if !defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
           kbd_key_release(KEY_SHOOT_FULL);                                          
#else
           {
            if(is_video_recording())
			{
             kbd_key_release(KEY_VIDEO); 
		    }
            else
             kbd_key_release(KEY_SHOOT_FULL); 
           }             
#endif 
                  set_key_press(1);
                  soft_half_press = 0;
			return 1;                                                
			}
		if(!get_usb_power(1) && nSW==116)                           
                 {
                if(conf.burst_shutdown && (drmode==1) && !conf.tv_bracket_value && !conf.dist_mode && (conf.bracket_type>2))
                 {
                  if((++burstShutdown)>200)power_down(); 
                  else return 1;
                 }
                  set_key_press(0);
			nWt=50;
			nSW=0;
			nCa=0;
			return 1;                                             
			}
		}
	if(nCa==1)
         {
		if(get_usb_power(1) && nSW>108 && nSW<120)                
       {
			nCount++;                                           
         
		 }
 
 		if(nSW==109)
                {
                 nCER=0;
			        if(nTxvideo>0) {nSW=121;return 1;}                                                              
                 if((((conf.dist_mode||conf.tv_bracket_value)&&state_kbd_script_run))&&!mvideo)         	
                  {
                    nSW=113;
                    nWt=2;
                    kbd_key_release_all();                                                                      
                    set_key_press(1);
                    return 1;                                                                                   
                  }
         		nSW=110;
			nWt=2;

       if(!nTxzoom)
        {    
			kbd_key_release_all();                                                                        
			if(!(srf && !conf.srb))kbd_key_press(KEY_SHOOT_HALF);                                         
                  soft_half_press = 1;
                  set_key_press(1);
			return 1; 
         }         
         nSW=113;
			nWt=2;
	          } 

		if(nSW==110)
                  {
                   if (shooting_in_progress() || mvideo || nCER>100) 
                      {
                        if(mvideo && !conf.video_frame_count && !is_video_recording())gFramesRecorded=0; 
                        nCER=0;
                        nSW=113;
			    }
			  else {nCER++;return 1;}
			}

		if(nSW==114)
          {
                 if(nTxzoom>0)                                                                                   
                      {
				if(nTxzoom<100)                                                                           
                            {
  if(ZoomIn)
  {
						nTxtblcr++;                                                                    
						if(nTxtblcr>(ZSTEP_TABLE_SIZE-1)) 
                    {
                     nTxtblcr=(ZSTEP_TABLE_SIZE-2);                                                  
                     ZoomIn=0;
                    }
					    }
					  else                                                                               
                   {                                                                                
					     nTxtblcr--;                                                                    
					     if(nTxtblcr<0) 
                     {
                      nTxtblcr=1;                                                     
                      ZoomIn=1;
                     }
					    }
               gSyncZoomStep=nTxtblcr;
               shooting_set_zoom(nTxtbl[nTxtblcr]);  
               nSW=0;
               
               nCount=0;
               gSZMode=1;
               nTxzoom=1;                                                                          
               nWt=70;
               return 0;
				   } 
              gSZMode=0;                                                                              
				  nTxzoom=0;                                                                              
				  nReczoom=0;                                                                             
			    }  
        }

		if(get_usb_power(1))                                           
                  {
			return 1;
			}
		if(nCount>0 && nSW==113)                               
                  {
			if(nCount<9)                                     
                        {
				if(nTxzoom>0)                              
                             {
					kbd_key_release_all();               
					set_key_press(0);
					nTxzoom=0;                           
               gSZMode=0;
					nReczoom=0;                          
					nSW=0;                               
					nCa=0;                               
               
					nCount=0;                            
					nWt=10;
					debug_led(0);                        
					return 1;
					}
				   nSW=125;                                
				   nWt=10;
				   if(!(srf && !conf.srb))kbd_key_release(KEY_SHOOT_HALF);        
                           soft_half_press = 0;
	                     set_key_press(1);
                        
				   nCount=0;                               
				   return 1;
				}

			  else{                                          
                         
				if(nTxzoom>0)                              
                             {
					nCount=0;                            
					nSW=114;                             
					return 1;
                             }
                             
				  nSW=124;
				  nWt=2;
                          shutter_int=0;
				  if(!(srf && !conf.srb))
                           {
                            if((get_movie_status()==VIDEO_RECORD_STOPPED)&& conf.video_frame_count&& (gFrameCount<=2) && mvideo){gFrameCount=conf.video_frame_count;}
                            kbd_key_press(KEY_SHOOT_FULL);        
                           }
                          set_key_press(1);
                          
				  nCount=0;
				  return 1;
				}
			}
         
		if(nSW==124)
                  {
                    if((drmode==1) && !shutter_int && (((conf.bracket_type>2)&& conf.tv_bracket_value)||conf.dist_mode)) 
                    {
			   return 1;
			  }

                    else if(drmode==1 && !shutter_int && !conf.tv_bracket_value && !conf.dist_mode && (conf.bracket_type<=2))           
                    {
                     return 1;
                    }

			 nSW=125;
			 nWt=2;
   			if(!(srf && !conf.srb))
#if !defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
           kbd_key_release(KEY_SHOOT_FULL);                                          
#else
           {
            if(mvideo||is_video_recording())
             kbd_key_release(KEY_VIDEO);
            else
             kbd_key_release(KEY_SHOOT_FULL); 
           }             
#endif 
                   soft_half_press = 0;
                   set_key_press(1);
			 return 1;
			}
		if(!get_usb_power(1) && nSW==125)      
                 {
                  set_key_press(0);
			nWt=50;                          
			nSW=0;
			nCa=0;
			return 1;
			}                  
		} 

      if(displayedMode && (displayedMode<5) && kbd_is_key_clicked(KEY_SET))  
      {
      hdrDark96=hdrLight96=gTv96max=gTv96min=gHdrPhase=0;
      conf.bracket_type=3;   
      conf.tv_bracket_value=6;
      conf.subj_dist_bracket_value=2;  
      kbd_key_release_all();
      nWt=50; 
      return 1;
      }

  } 

 if (conf.use_zoom_mf && kbd_use_zoom_as_mf()) 
  {
   return 1;
  } 

 other_kbd_process();
 }
  return kbd_blocked; 
}

void conf_restore_finished()
{
 if(conf.extend_lens==1) nConffin=1;
}

static const struct Keynames {
    int keyid;
    char *keyname;
} keynames[] = {
#if defined(CAMERA_sx200is)||defined(CAMERA_sx240hs)||defined(CAMERA_sx260hs)||defined(CAMERA_sx280hs)||defined(CAMERA_sx50hs)
    { KEY_ZOOM_IN_SLOW, "zoom_in_slow"},
    { KEY_ZOOM_OUT_SLOW, "zoom_out_slow"},
#endif
    { KEY_UP,           "up"         },
    { KEY_DOWN,         "down"       },
    { KEY_LEFT,         "left"       },
    { KEY_RIGHT,        "right"      },
    { KEY_SET,          "set"        },
    { KEY_SHOOT_HALF,   "shoot_half" },
    { KEY_SHOOT_FULL,   "shoot_full" },
    { KEY_SHOOT_FULL_ONLY,"shoot_full_only"},
    { KEY_ZOOM_IN,      "zoom_in"    },
    { KEY_ZOOM_OUT,     "zoom_out"   },
    { KEY_MENU,         "menu"       },
    { KEY_DISPLAY,      "display"    },
    { KEY_PRINT,        "print"      },
    { KEY_ERASE,        "erase"      },
    { KEY_ISO,          "iso"        },
    { KEY_FLASH,        "flash"      },
    { KEY_MF,           "mf"         },
    { KEY_MACRO,        "macro"      },
    { KEY_VIDEO,        "video"      },
    { KEY_TIMER,        "timer"      },
    { KEY_EXPO_CORR,    "expo_corr"  },
    { KEY_MICROPHONE,   "fe"         },
    { KEY_POWER,        "power"      },
    { 0xFF,             "remote"     },
    { 0xFFFF,           "no_key"     },
    { MOVIE_MODE,       "movie_mode"},
    { TV_MODE,          "tv_mode"},
    { PLAYBACK_MODE,    "playback_mode"},
};

 int keyid_by_name(const char *n)
{
    int i;
    for (i=0;i<sizeof(keynames)/sizeof(keynames[0]);i++)
	if (strcmp(keynames[i].keyname,n) == 0)
	    return keynames[i].keyid;
    return 0;
}

static const int vkeys[4][4]= {{KEY_UP,KEY_LEFT,KEY_RIGHT,KEY_DOWN},
                               {KEY_DOWN,KEY_RIGHT,KEY_LEFT,KEY_UP},
                               {KEY_LEFT,KEY_DOWN,KEY_UP,KEY_RIGHT},
                               {KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_LEFT}};

int get_vkey(int key)
{
 if((key-KEY_UP)>3) return key;
 short row,col;
 row=key-KEY_UP;
 if(conf.camera_orientation==2)col=3;  
 else if(conf.camera_orientation==3)col=2; 
 else col=conf.camera_orientation;
 return vkeys[row][col];
}

const char* key_name_from_id(int keyId)
{
int i;
for (i=0;i<sizeof(keynames)/sizeof(keynames[0]);i++)
   if (keyId == keynames[i].keyid)
      return keynames[i].keyname;
return 0;
}

int kbd_is_blocked() 
{
  return kbd_blocked;
}

