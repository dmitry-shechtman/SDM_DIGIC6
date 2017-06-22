#include "platform.h"
#include "core.h"
#include "conf.h"
#include "font.h"
#include "keyboard.h"
#include "stdlib.h"
#include "gui.h"
#include "raw.h"
#include "motion_detector.h"
#include "draw_palette.h"
#include "lang.h"
 
extern int gSDMShootMode,kbd_blocked,gVideoStartTime,gVideoStopTime;
extern long temp1,temp2;
extern int _EngDrvRead(int gpio_reg),usb_HPtimer_handle;
extern volatile enum Gui_Mode gui_mode;
unsigned char gPrevStateVideoSynch,gSerialCount=0,ConfRestoreFinished;
unsigned char movSynchMode,gDisablePowerDown,gMovieSyncAutostart;
long FastLapseStart,FastLapseEnd; 
int syncStart;
static int raw_need_postprocess;
static volatile int spytask_can_start;
unsigned int burstCount,recStart;
unsigned int gFPS,gImageCount=0;
char *imgbuf = 0;
int imgbuf_size=0;
int gLowLightCount = 0;
int gDirectoryNumber;
extern int previous_focus,gInitialFocus,inf_focus,bls,afl;
extern int LowestSynchCnt,pulseCount,hdrLight96,hdrDark96;
extern long gEnableScripting;
extern volatile long zoom_busy;
extern unsigned char gBrightActivated,syncDone,calibDone,displayedMode,gHdrPhase,bootOrientation;
extern unsigned long period1,period2;
extern short int* pAFbuf;
unsigned char gPlayRecMode;
 
#define GRAY_MODE 1
#define COLOR_MODE 2
#if defined(OPT_LOG)
#define SDMLOG(x) if (logfd>=0){write(logfd,#x,strlen(#x));}
#else
#define SDMLOG(x)
#endif
static int ptp_raw_send_active;
int beenInRec=0;
static int clip(int v) 
{
if (v<0) v=0;
if (v>255) v=255;
return v;
}

static int clips(int v) 
{
if (v<-127) v=-127;
if (v>127) v=127;
return v;
}

void allocate_imgbuf()
{
#if !defined(OPT_NEW_OVERLAY)
int x;
 if(!imgbuf)
 {
#if defined(CAMERA_ixus100)
  imgbuf_size = 360 * 160 *3;                                          
#elif defined(CAMERA_g1x)||defined(CAMERA_g10)||defined(CAMERA_g11)|| defined(CAMERA_g12)|| defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus105)|| defined(CAMERA_ixus220)|| defined(CAMERA_sx1)|| defined(CAMERA_sx20)|| defined(CAMERA_s110)
  imgbuf_size = 720 * 240 * 3;
#elif defined(CAMERA_sx220hs)
  imgbuf_size = 960 * 240 * 3;
#else
  imgbuf_size = 360 * 240 *3;                                         
#endif
  imgbuf = malloc(imgbuf_size);                                       
  if(imgbuf)memset(imgbuf,COLOR_TRANSPARENT,imgbuf_size);             
 } 
#endif     
}

#if !defined(OPT_NEW_INVERT)
void invert() 
{
 int x;
 unsigned char* tmp;
 static int cnt=0; 
 static unsigned char *pb_img;
 static unsigned char *pb_img2; 
 static int phase=0;                                                  

 if(!pb_img)pb_img=vid_get_viewport_fb_d();                           
 else if(!pb_img2)                                                    
  {
   tmp=vid_get_viewport_fb_d();
   if(tmp && (tmp!=pb_img))pb_img2=tmp;                               
  }
 if(conf.invert_playback &&((mode_get()&MODE_MASK) == MODE_PLAY))     
 {
  if(!imgbuf)allocate_imgbuf();                                       
  if(imgbuf && pb_img)                                                
  {
   if(!cnt)tmp=pb_img;                                                
   else tmp=pb_img2;                                                  

   if(!((pb_img[0]==255)&&(pb_img[1]==254)&&(pb_img[2]==253))&& tmp)  
    {
   if(!phase)                                                         
    {  
     memset(imgbuf,COLOR_TRANSPARENT,imgbuf_size);                    
     for(x=0;x<imgbuf_size/2;x+=6)                                    
      { 
       imgbuf[x]  =tmp[imgbuf_size-x-6];                              
       imgbuf[x+1]=tmp[imgbuf_size-x-1];
       imgbuf[x+2]=tmp[imgbuf_size-x-4];
       imgbuf[x+3]=tmp[imgbuf_size-x-2];
       imgbuf[x+4]=tmp[imgbuf_size-x-3];
       imgbuf[x+5]=tmp[imgbuf_size-x-5];
      }
      phase=1;                                                        
      return;
     }
    else                                                              
     {
       for(x=imgbuf_size/2;x<imgbuf_size;x+=6)                        
      { 
       imgbuf[x]  =tmp[imgbuf_size-x-6];                              
       imgbuf[x+1]=tmp[imgbuf_size-x-1];
       imgbuf[x+2]=tmp[imgbuf_size-x-4];
       imgbuf[x+3]=tmp[imgbuf_size-x-2];
       imgbuf[x+4]=tmp[imgbuf_size-x-3];
       imgbuf[x+5]=tmp[imgbuf_size-x-5];
      }
       phase=0; 
       memcpy(tmp,imgbuf,imgbuf_size);                                
       tmp[0] = 255;                                                  
       tmp[1] = 254;
       tmp[2] = 253;                                                  
       cnt=1-cnt;                                                     
     }
    }  
   }  
 } 

}   
#else

void invert() 
{
 int x;
 unsigned char* tmp;
 static int cnt=0; 
 static unsigned char *pb_img=NULL;
 static unsigned char *pb_img2=NULL; 
 static int phase=0; 
#if defined(CAMERA_ixus100)
  imgbuf_size = 360 * 160 *3;                                          
#elif defined(CAMERA_g11)|| defined(CAMERA_g10)|| defined(CAMERA_g12)|| defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus105)|| defined(CAMERA_ixus115)|| defined(CAMERA_ixus220)|| defined(CAMERA_sx1)|| defined(CAMERA_sx20)|| defined(CAMERA_sx240hs)|| defined(CAMERA_sx260hs)
  imgbuf_size = 720 * 240 * 3;
#elif defined(CAMERA_sx220hs)||defined(CAMERA_sx230hs)
  imgbuf_size = 960 * 240 * 3;
#else
  imgbuf_size = 360 * 240 *3;                                           
#endif
 if(conf.invert_playback &&((mode_get()&MODE_MASK) == MODE_PLAY))     
 {
if(!phase)pb_img=vid_get_viewport_fb_d();
 if(!imgbuf)imgbuf = malloc(imgbuf_size/3);   				
 if(!((pb_img[0]==255)&&(pb_img[1]==254)&&(pb_img[2]==253)))  	
  {
   if(!phase)
    {
     memcpy(imgbuf,pb_img,imgbuf_size/3);  				
     for(x=0;x<imgbuf_size/3;x+=6)  					
      { 										
       pb_img[x]  =pb_img[imgbuf_size-x-6];                       
       pb_img[x+1]=pb_img[imgbuf_size-x-1];
       pb_img[x+2]=pb_img[imgbuf_size-x-4];
       pb_img[x+3]=pb_img[imgbuf_size-x-2];
       pb_img[x+4]=pb_img[imgbuf_size-x-3];
       pb_img[x+5]=pb_img[imgbuf_size-x-5];
      }
      phase=1;                                                       
      return;
    }

   else if(phase==1)								
    {											
     for(x=0;x<imgbuf_size/3;x+=6)                                    
      { 
       pb_img[(2*imgbuf_size/3)+x]  =imgbuf[imgbuf_size/3-x-6];        
       pb_img[(2*imgbuf_size/3)+x+1]=imgbuf[imgbuf_size/3-x-1];
       pb_img[(2*imgbuf_size/3)+x+2]=imgbuf[imgbuf_size/3-x-4];
       pb_img[(2*imgbuf_size/3)+x+3]=imgbuf[imgbuf_size/3-x-2];
       pb_img[(2*imgbuf_size/3)+x+4]=imgbuf[imgbuf_size/3-x-3];
       pb_img[(2*imgbuf_size/3)+x+5]=imgbuf[imgbuf_size/3-x-5];
      }
     phase=2;
     return;
    }
   else if(phase==2)								
    {
     memcpy(imgbuf,pb_img+(imgbuf_size/3),imgbuf_size/3);            
     memset(pb_img+(imgbuf_size/3),COLOR_TRANSPARENT,imgbuf_size/3);
     for(x=0;x<imgbuf_size/3;x+=6)  					
      { 										
       pb_img[imgbuf_size/3+x]  =imgbuf[imgbuf_size/3-x-6];        
       pb_img[imgbuf_size/3+x+1]=imgbuf[imgbuf_size/3-x-1];
       pb_img[imgbuf_size/3+x+2]=imgbuf[imgbuf_size/3-x-4];
       pb_img[imgbuf_size/3+x+3]=imgbuf[imgbuf_size/3-x-2];
       pb_img[imgbuf_size/3+x+4]=imgbuf[imgbuf_size/3-x-3];
       pb_img[imgbuf_size/3+x+5]=imgbuf[imgbuf_size/3-x-5];
      }
     pb_img[0]=255;
     pb_img[1]=254;
     pb_img[2]=253;
     phase = 0;
     return;
    }
  }
   else  phase = 0;
 }
}
#endif

#if !defined(OPT_NEW_STEREO_PB)
void stereo_pair() 
{
  int x, y;
  int ry, yd, ru, rv, vd, lu, lv;
 static unsigned char *img;
 static int viewport_height;
 static int viewport_width;
 static int iw;
 static int stereonum = 0;

 if(!conf.stereo_mode) return;
img=(((mode_get()&MODE_MASK) == MODE_PLAY)?vid_get_viewport_fb_d():(kbd_is_key_pressed(ANA_BUTTON))?vid_get_viewport_fb():vid_get_viewport_live_fb());

if (img==NULL) img = vid_get_viewport_fb();
#if defined(CAMERA_ixus980)
viewport_width = screen_width * 3/2;
#elif defined(CAMERA_sx200is)|| defined(CAMERA_g10)|| defined(CAMERA_g11)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus90)|| defined(CAMERA_ixus95)|| defined(CAMERA_ixus220)
viewport_width= vid_get_viewport_width()*3;
#else
viewport_width = screen_width * 3;
#endif
viewport_height = vid_get_viewport_height();
iw = viewport_width / 4;

 if(((mode_get()&MODE_MASK) == MODE_PLAY)&& !conf.camera_orientation) 
 {
     play_sound(4);
       if(imgbuf == 0)allocate_imgbuf() ;                             
       if(stereonum == 0) 
       {
         if((img[0] == 255) && (img[1] == 254) && (img[2] == 253)) return;

         if(imgbuf) 
           {
             memcpy(imgbuf,img,imgbuf_size);

                 img[0] = 255; 
                 img[1] = 254;
                 img[2] = 253;
                 stereonum = 2;   
           } 

             return;
       }  

      if(stereonum == 1) 
      {
        if(imgbuf) 
        {
          if((img[0] == 255) && (img[1] == 254) && (img[2] == 253)) return;
            stereonum = 2;
            return;
        }
          return;
     } 

       if(stereonum == 2) 
       {
        if(conf.stereo_mode == GRAY_MODE)
          {

             for(x = 0; x < imgbuf_size; x+=6)
            {
              ry = img[x + 1];
              yd = ry - imgbuf[x + 1];
              img[x + 1] = (1225 * imgbuf[x + 1] + 2871 * ry) / 4096; 
              img[x] = (signed char)clips((692 * yd)/4096); 
              img[x + 2] = (signed char)clips(-yd/2); 
              img[x + 3] = (1225 * imgbuf[x + 3] + 2871 * img[x + 3]) / 4096; 
              img[x + 4] = (1225 * imgbuf[x + 4] + 2871 * img[x + 4]) / 4096; 
              img[x + 5] = (1225 * imgbuf[x + 5] + 2871 * img[x + 5]) / 4096; 
            }
        }

       else
       {
         for(x = 0; x < imgbuf_size; x+=6)
           {
             ry = img[x + 1];
            yd = ry - imgbuf[x + 1];
            ru = (signed char) img[x];
            rv = (signed char) img[x + 2];
            lu = (signed char) imgbuf[x];
            lv = (signed char) imgbuf[x + 2];
           vd = rv - lv;
           img[x] = (signed char)clips(ru + (971 * vd + 692 * yd) / 4096); 
           img[x + 2] = (signed char)clips(-yd/2 + (2871 * lv + 1225 * rv) / 4096); 
           img[x + 1] = clip((1225 * imgbuf[x + 1] + 2871 * ry - 1712 * vd) / 4096); 
           img[x + 3] = clip((1225 * imgbuf[x + 3] + 2871 * 
           img[x + 3] - 1712 * vd) / 4096); 
           img[x + 4] = clip((1225 * imgbuf[x + 4] + 2871 * 
           img[x + 4] - 1712 * vd) / 4096); 
           img[x + 5] = clip((1225 * imgbuf[x + 5] + 2871 * 
          img[x + 5] - 1712 * vd) / 4096); 
         }
     }

           img[0] = 255;//indicate that this is stereo
           img[1] = 254;
           img[2] = 253;
           for(y=0;y<screen_height;y++) 
            for(x = 0; x < screen_width;x++) 
             draw_pixel(x,y,0); 
           stereonum = 0;
           return;
     } 
} 
return;
}

#else

void stereo_pair() 
{
 int x, y;
 int ry, yd, ru, rv, vd, lu, lv;
 static unsigned char *img1 = NULL;
 static unsigned char *img2 = NULL;
 static int stereonum = 0;
 FILE *fd = NULL;
 if(!conf.stereo_mode || (gui_get_mode()!=GUI_MODE_NONE)) return;
#if defined(CAMERA_ixus100)
  imgbuf_size = 360 * 160 *3;                                          
#elif defined(CAMERA_g11)|| defined(CAMERA_g12)|| defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus105)|| defined(CAMERA_ixus220)|| defined(CAMERA_sx1)|| defined(CAMERA_sx20)
  imgbuf_size = 720 * 240 * 3;
#elif defined(CAMERA_sx220hs)||defined(CAMERA_sx230hs)|| defined(CAMERA_sx240hs)|| defined(CAMERA_sx260hs)
  imgbuf_size = 960 * 240 * 3;
#else
  imgbuf_size = 360 * 240 *3;                                           
#endif

 if(((mode_get()&MODE_MASK) == MODE_PLAY)&& !conf.camera_orientation) 
 {                          
   if(!stereonum && !img1) 
    {
     img1=vid_get_viewport_fb_d();
     if(img1)
      {
       play_sound(4);
       fd = fopen("A/FirstImage.buf", "wb");
	 if(fd !=NULL)
	  {
	   fwrite(img1,imgbuf_size,1,fd);
	   fclose(fd);
	  }
        stereonum =1;
      } 
     return;
    }
       if(stereonum && img1 && !img2) 
       {
        img2=vid_get_viewport_fb_d();
        play_sound(4);
	  fd = fopen("A/FirstImage.buf","rb");
	  if( fd != NULL )
        {
         fread(img1,imgbuf_size,1,fd);
         fclose(fd);
         remove("A/FirstImage.buf");
         if(conf.stereo_mode == GRAY_MODE)
          {
             for(x = 0; x < imgbuf_size; x+=6)
            {
              ry = img2[x + 1];
              yd = ry - img1[x + 1];
              img2[x + 1] = (1225 * img1[x + 1] + 2871 * ry) / 4096; 
              img2[x] = (signed char)clips((692 * yd)/4096); 
              img2[x + 2] = (signed char)clips(-yd/2); 
              img2[x + 3] = (1225 * img1[x + 3] + 2871 * img2[x + 3]) / 4096; 
              img2[x + 4] = (1225 * img1[x + 4] + 2871 * img2[x + 4]) / 4096; 
              img2[x + 5] = (1225 * img1[x + 5] + 2871 * img2[x + 5]) / 4096; 
            }
          }
       else if(conf.stereo_mode == COLOR_MODE)
       {
         for(x = 0; x < imgbuf_size; x+=6)
           {
            ry = img2[x + 1];
            yd = ry - img1[x + 1];
            ru = (signed char) img2[x];
            rv = (signed char) img2[x + 2];
            lu = (signed char) img1[x];
            lv = (signed char) img1[x + 2];
            vd = rv - lv;
            img2[x] = (signed char)clips(ru + (971 * vd + 692 * yd) / 4096); 
            img2[x + 2] = (signed char)clips(-yd/2 + (2871 * lv + 1225 * rv) / 4096); 
            img2[x + 1] = clip((1225 * img1[x + 1] + 2871 * ry - 1712 * vd) / 4096); 
            img2[x + 3] = clip((1225 * img1[x + 3] + 2871 * 
            img2[x + 3] - 1712 * vd) / 4096); 
            img2[x + 4] = clip((1225 * img1[x + 4] + 2871 * 
            img2[x + 4] - 1712 * vd) / 4096);                                          	 
            img2[x + 5] = clip((1225 * img1[x + 5] + 2871 * 
            img2[x + 5] - 1712 * vd) / 4096); 
           }
       }

           img2[0] = 255; 
           img2[1] = 254;
           img2[2] = 253;
    }
           stereonum = 0;
           img1 = img2 = NULL;
           return;
     }
}  
return;
}
#endif

void core_hook_task_create(void *tcb)
{
}

void core_hook_task_delete(void *tcb)
{
char *name = (char*)(*(long*)((char*)tcb+0x34));
 if (strcmp(name,"tInitFileM")==0) core_spytask_can_start();
}

long core_get_noise_reduction_value()
{
    return conf.raw_nr;
}

volatile long raw_data_available;

/*called from another process */
void core_rawdata_available()
{
 if((shooting_get_drive_mode()==1)&& ((conf.fastlapse)||(gSDMShootMode==BURST)))
  {
   if(!gLowLightCount)burstCount=0;
   ++gLowLightCount;
   ++burstCount;
  }
  else if(tl.running)++tl.shot_count; 
  ++gImageCount;
  if(!shooting_get_drive_mode())++gSerialCount;
  raw_data_available = 1;
}

void ptp_raw_sending_active(int active)
{
 ptp_raw_send_active = active;
}

void core_spytask_can_start() 
{
 spytask_can_start = 1;
}
 
void bgndBeeper()
{
 static int USBcnt;
 while(shooting_get_tick_count()<5000) msleep(1000);
 while(1)
 {
  if(usb_HPtimer_handle == 0)usb_high_count();  
  msleep(10);
 }
}
 
void core_spytask()
{
    int cnt = 1,x,y;
    int i=0;
 
    static int nCER=0;
    static int wiaDone = 0,vmode;
    raw_need_postprocess = 0;
    spytask_can_start=0;
    static unsigned int LowLightDelay = 0;
  
    while((i++<400) && !spytask_can_start) msleep(10);

    mkdir("A/DCIM");
    mkdir("A/DCIM/CRW");
    mkdir("A/SDM");
    mkdir("A/SDM/CSV");
    mkdir("A/SDM/GRIDS");
    mkdir("A/SDM/LANG");
    mkdir("A/SDM/PARAMS");
    mkdir("A/SDM/SCRIPTS");
    mkdir("A/SDM/SCRIPTS2");
    mkdir("A/SDM/SCRIPTS3");
    mkdir("A/SDM/SCRIPTSM");  
    mkdir("A/SDM/STACKS");
    mkdir("A/SDM/TEXTS");
    mkdir("A/SDM/XML");
    mkdir("A/SDM/LOGS");
    auto_started=0;
                                                                                              			
#if !defined(CAM_NEED_HP)
   gEnableScripting=shooting_get_tick_count();
#endif
    conf_restore();
    ConfRestoreFinished=1; 
if(conf.script_startup)conf.splash_show=0;			
if(!conf.splash_show)                              
 conf_restore_finished(); 
 gui_init();  
 md_init();   
 getModeIDs();
 iso_init();
 
 while (1)
  {
   if(!wiaDone && shooting_get_tick_count()> 10000) 
    {
     wiaDone = 1;
     wia();
    }
    
    if(!beenInRec && ((mode_get()&MODE_MASK)==MODE_REC))
     {
      beenInRec = 1;
      recStart=shooting_get_tick_count();
      lang_load_from_file(conf.lang_file);
      vmode=shooting_get_prop(PROPCASE_SHOOTING_MODE); 
      vmode=shooting_mode_canon2chdk(vmode);  
     }
    if(!gVideoStopTime)gVideoStopTime=shooting_get_tick_count();
 
    unsigned char syncvid;
    if(beenInRec&&((shooting_get_tick_count()-recStart)>3000)&&(displayedMode==6)&&((mode_get()&MODE_MASK)==MODE_REC) && (gui_mode == GUI_MODE_NONE)&&((shooting_get_tick_count()-gVideoStopTime)>3000) && !get_usb_bit()) 
     {
      if(!is_video_recording()&&!canon_shoot_menu_active && (canon_menu_active==(int)&canon_menu_active-4)) 
     {
       vmode=shooting_get_prop(PROPCASE_SHOOTING_MODE); 
      vmode=shooting_mode_canon2chdk(vmode);                
      syncvid = MODE_IS_SYNCH_VIDEO(vmode);    
     }
    }

    if(beenInRec&&((shooting_get_tick_count()-recStart)>3000)&&(displayedMode==6)&&((mode_get()&MODE_MASK)==MODE_REC) && (gui_mode == GUI_MODE_NONE)&&((shooting_get_tick_count()-gVideoStopTime)>3000) && !get_usb_bit()) 
     {
      if(!is_video_recording()&&!canon_shoot_menu_active && (canon_menu_active==(int)&canon_menu_active-4)) 
       {
       vmode=shooting_get_prop(PROPCASE_SHOOTING_MODE); 
      vmode=shooting_mode_canon2chdk(vmode);   
      syncvid = MODE_IS_SYNCH_VIDEO(vmode);    

     if(syncvid)  // sync mode 
     {
      if(!gPrevStateVideoSynch) 
       {
        kbd_key_release_all();
        kbd_blocked=1;
        gui_kbd_enter();
        gMovieSyncAutostart=1; 
        gPrevStateVideoSynch=1;
        syncStart=shooting_get_tick_count();
       }
     }
     else 
     {
      if(gPrevStateVideoSynch) 
       gPrevStateVideoSynch=0;
     }
     }
    }	
 
   if(!state_kbd_script_run && !conf.show_osd && (strcmp(conf.script_file,"A/SDM/SCRIPTS2/A_.txt") == 0)){conf.show_osd=1;afl=0;}    
 
 if(conf.script_startup && !auto_started && !zoom_busy)
    {
     if(!conf.extend_lens)script_autostart();                      
     else if((mode_get()&MODE_MASK)==MODE_REC) script_autostart(); 
    }
     if(shooting_get_drive_mode()&& shooting_can_focus() && conf.dist_mode && conf.user_range_set)conf.user_range_set=0; 
     if(!shooting_get_drive_mode() && !state_kbd_script_run && conf.tv_bracket_value)conf.tv_bracket_value=0; 
     if(!get_sd_status() && ((conf.dist_mode==3)|| tl.running)) disable_shutdown();   
	if (raw_data_available && !ptp_raw_send_active)
        {  
         if((shooting_get_drive_mode()==1)&& conf.fastlapse && !conf.tv_bracket_value && (state_shooting_progress != SHOOTING_PROGRESS_PROCESSING))
          {
           state_shooting_progress = SHOOTING_PROGRESS_PROCESSING;
          }
          
          if(!(((shooting_get_drive_mode()==1)&& conf.fastlapse && !conf.tv_bracket_value) || (gSDMShootMode==BURST)))raw_need_postprocess = raw_savefile(); 
 
          if(conf.bracket_intershot_delay && shooting_get_drive_mode())msleep(100*conf.bracket_intershot_delay);
          if((state_shooting_progress == SHOOTING_PROGRESS_PROCESSING)&& gLowLightCount && conf.burst_frame_count && (gLowLightCount>=conf.burst_frame_count))msleep(1000);
          else if(inf_focus||shutter_int)msleep(1000);
         log_kap();
         save_xml();         
	      hook_raw_save_complete();
	      raw_data_available = 0;
         gDirectoryNumber=get_target_dir_num();

         if (!bls || (tl.blank_screen && tl.shot_count))TurnOffDisplay();
         
	    continue;
	  }
#if defined(CAMERA_g9)
      if(auto_started && !state_kbd_script_run) 
       { 
         const char *myfn = "A/PS.FI2";
         reboot(myfn);
       }
#endif
	if (state_shooting_progress != SHOOTING_PROGRESS_PROCESSING) 
      {
       int m=mode_get()&MODE_SHOOTING_MASK;
       int mode_video=MODE_IS_VIDEO(m);
	 if ((mode_get()&MODE_MASK) != MODE_PLAY) gPlayRecMode = 1;
       if ((mode_get()&MODE_MASK) == MODE_PLAY) gPlayRecMode = 0;
       if (((mode_get()&MODE_MASK) != MODE_PLAY) && mode_video) gPlayRecMode = 2;

	    if (((cnt++) & 3) == 0)
           {
	         gui_redraw(); 
           }
           invert();
         if(gBrightActivated)
          {
           gBrightActivated = 0;
           shooting_set_prop(PROPCASE_FLASH_MODE,1); 
          } 
	  if((displayedMode==6)&&conf.slave_flash&&(shooting_get_prop(PROPCASE_FLASH_ADJUST_MODE)!=1))disable_preflash();
	 }

if(state_shooting_progress == SHOOTING_PROGRESS_PROCESSING)
{
     if(!shooting_in_progress())
       {
          
         if((shooting_get_drive_mode()==1)||(shooting_get_drive_mode()== 3)) 
          {
           if(shooting_can_focus()&& (conf.dist_mode==3))lens_set_focus_pos(previous_focus+fpd);
           if((conf.tv_bracket_value && !conf.dist_mode)||(shooting_can_focus()&& (conf.dist_mode>0)))inf_focus = 0;
          }   
          
	       state_shooting_progress = SHOOTING_PROGRESS_DONE;
          
          if (raw_need_postprocess) raw_postprocess();
          if(gLowLightCount)gLowLightCount=0;
          hdrDark96=hdrLight96=gHdrPhase=0;
          debug_led(0);        
      }
 }
   msleep(20);
    }
}


