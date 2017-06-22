#include "stdlib.h"
#include "keyboard.h"
#include "platform.h"
#include "core.h"
#include "lang.h"
#include "conf.h"
#include "gui.h"
#include "draw_palette.h"
#include "gui_lang.h"
#include "math.h"
#include "gui_osd.h"
#include "batt_grid.h"
#include "kbd.h"
 
extern char *imgbuf;  
extern int imgbuf_size;
extern int gDngFinish,gDngStart;
extern int gPTPsession;
extern volatile long zoom_busy;
extern int movSynchErr,gZoomPoint,gFocalPoint;                      
extern unsigned char displayedMode,gDidShowFocus,gNearFar,gSyncZoomStep;
int PrevZoom;
float realfl,realav,required_av = 0.0f, v, deviation,coc, max_distance, percent_deviation,fov,inf_np,my_max_deviation;
float entrance,mag,exitp;
int shift,myfl,myhyp,rfshift,rf_focus=1000;
long fp,sensor_width;
extern long myav;
extern int subject_far;
extern unsigned char gEnableBigZoom,syncQ;
unsigned char bigZoomXPos,bigZoomYPos;

typedef struct 
{
    int     title;
    OSD_pos *pos;
    OSD_pos size;
} OSD_elem;

typedef ShutterSpeed *SsPointer;
SsPointer ss = NULL;

typedef ISOTable *isoPointer;
isoPointer ip = NULL;

unsigned char clip8(signed short x){ if (x<0) x=0; else if (x>255) x=255; return x; } 

static OSD_elem osd[]={
    {(int)"Frames",                     &conf.frames_pos,       {6*FONT_WIDTH,FONT_HEIGHT}      },
    {LANG_OSD_LAYOUT_EDITOR_STATES,     &conf.mode_state_pos,   {3*FONT_WIDTH, 3*FONT_HEIGHT}   },
    {LANG_OSD_LAYOUT_EDITOR_MISC,       &conf.values_pos,       {8*FONT_WIDTH, 5*FONT_HEIGHT}   },
    {LANG_OSD_LAYOUT_EDITOR_ZOOM,       &conf.zoom_pos,         {2*FONT_WIDTH, 2*FONT_HEIGHT}   },
    {LANG_OSD_LAYOUT_EDITOR_INCREMENT,  &conf.increment_pos,    {5*FONT_WIDTH, FONT_HEIGHT}     },
    {LANG_OSD_LAYOUT_EDITOR_BAT_ICON,   &conf.batt_icon_pos,    {28, 12}                        },
    {LANG_OSD_LAYOUT_EDITOR_BAT_TEXT,   &conf.batt_txt_pos,     {5*FONT_WIDTH, FONT_HEIGHT}     },
    {LANG_OSD_LAYOUT_EDITOR_CLOCK,      &conf.clock_pos,        {5*FONT_WIDTH, FONT_HEIGHT}     },
    {LANG_OSD_LAYOUT_EDITOR_STEREO,     &conf.stereo_osd_pos,   {17*FONT_WIDTH, 2*FONT_HEIGHT}  },
    {LANG_OSD_LAYOUT_EDITOR_SYNCH,      &conf.synch_pos,        {25*FONT_WIDTH, FONT_HEIGHT}    },
    {LANG_MENU_OVERRIDE,                &conf.overrides_pos,    {3*FONT_WIDTH, 3*FONT_HEIGHT}   },
    {0}
};
static int osd_to_draw;
static int curr_item;
static char osd_buf[64];
static int step;
static unsigned char *img_buf, *scr_buf;
static int timer = 0;

void gui_osd_init() {
    osd_to_draw = 1;
    curr_item = 0;
    step = 10;
    draw_restore();
}

void gui_osd_draw() 

{ 
    if (osd_to_draw) 
     {
        int i;
        draw_restore();
        gui_batt_draw_osd();
        gui_osd_draw_state();
        gui_osd_draw_values();
        gui_osd_draw_clock();

        for (i=1; i<=2; ++i) {
            draw_rect((osd[curr_item].pos->x>=i)?osd[curr_item].pos->x-i:0, (osd[curr_item].pos->y>=i)?osd[curr_item].pos->y-i:0, 
                      osd[curr_item].pos->x+osd[curr_item].size.x+i-1, osd[curr_item].pos->y+osd[curr_item].size.y+i-1,
                      COLOR_GREEN);
        }
        sprintf(osd_buf, " %s:  x:%d y:%d s:%d ", lang_str(osd[curr_item].title), osd[curr_item].pos->x, osd[curr_item].pos->y, step);
        draw_string(0, (osd[curr_item].pos->x<strlen(osd_buf)*FONT_WIDTH+4 && osd[curr_item].pos->y<FONT_HEIGHT+4)?screen_height-FONT_HEIGHT:0,
                    osd_buf, MAKE_COLOR(COLOR_RED, COLOR_WHITE),1);
        osd_to_draw = 0;
     }
}

void gui_osd_kbd_process() 
{
switch(get_vkey(kbd_get_autoclicked_key()))
 { 
  case KEY_LEFT:
       if (osd[curr_item].pos->x > 0) 
         {
          osd[curr_item].pos->x-=(osd[curr_item].pos->x>=step)?step:osd[curr_item].pos->x;
          osd_to_draw = 1;
         }
  break;
  case KEY_RIGHT:
#if !defined(CAMERA_tx1)
       if (osd[curr_item].pos->x < screen_width-osd[curr_item].size.x) 
        {
         osd[curr_item].pos->x+=(screen_width-osd[curr_item].size.x-osd[curr_item].pos->x>step)?step:screen_width-osd[curr_item].size.x-osd[curr_item].pos->x;
 #else
       if (osd[curr_item].pos->x < screen_width-120-osd[curr_item].size.x) 
        {
         osd[curr_item].pos->x+=(screen_width-120-osd[curr_item].size.x-osd[curr_item].pos->x>step)?step:screen_width-120-osd[curr_item].size.x-osd[curr_item].pos->x;
#endif
         osd_to_draw = 1;
        }
  break;
  case KEY_UP:
       if (osd[curr_item].pos->y > 0) 
        {
         osd[curr_item].pos->y-=(osd[curr_item].pos->y>=step)?step:osd[curr_item].pos->y; 
         osd_to_draw = 1;
        }
  break;
 
 case KEY_DOWN:
       if (osd[curr_item].pos->y < screen_height-osd[curr_item].size.y) 
        {
         osd[curr_item].pos->y+=(screen_height-osd[curr_item].size.y-osd[curr_item].pos->y>step)?step:screen_height-osd[curr_item].size.y-osd[curr_item].pos->y;
         osd_to_draw = 1;
        }
  break;
  case KEY_SET:
     ++curr_item;
     if (!osd[curr_item].pos) 
     curr_item = 0;
     osd_to_draw = 1;
  break;
#if defined(CAM_HAS_DISP_BUTTON) 
  case KEY_DISPLAY:
     step=(step==1)?10:1;
     osd_to_draw = 1;
  break;
#endif
 }
}

static void sprintf_dist(char *buf, float dist) {
    if (dist<=0 || dist>(int) INFINITY) 
    {
        sprintf(buf, " INF");
    } else if (dist<10000) 
    {
        sprintf(buf, "%d.%03d", (int)(dist/1000), (int)(dist)%1000);
    } else if (dist<100000) 
    {
        sprintf(buf, "%02d.%d", (int)(dist/1000), (int)(dist/100)%10);
    } else 
    {
        sprintf(buf, "%4d", (int)(dist/1000));
    }
}

void load_bitmap(char *img, int viewport_size, char *fn)
{
   int fd;
   fd = open(fn, O_RDONLY, 0777);
    if (fd>=0) 
      {
      int rcnt = read(fd, img, viewport_size *3);
      close(fd);
      }
   return;
}

int get_memory_info()
{
    int size, l_size, d;
    char* ptr;
    size = 16;
    while (1) 
       {
        ptr= malloc(size);
        if (ptr) 
          {
            free(ptr);
            size <<= 1;
          } 
          else break;
        }

    l_size = size;
    size >>= 1;
    d=1024;
    while (d) 
      {
        ptr = malloc(size);
        if (ptr) 
         {
            free(ptr);
            d = l_size-size;
            if (d<0) d=-d;
            l_size = size;
            size += d>>1;
          } 
         else 
          {
            d = size-l_size;
            if (d<0) d=-d;
            l_size = size;
            size -= d>>1;
           }       
        }
     return size-1;
}

void gui_osd_draw_state() 
{
    int n=0, gui_mode=gui_get_mode();
   if (((conf.save_raw || gui_mode==GUI_MODE_OSD))&& !displayedMode)
    {
      if(!gDngFinish)
        draw_string(conf.mode_state_pos.x-FONT_WIDTH, conf.mode_state_pos.y," DNG ",(conf.strip_offset && conf.raw_strip_mode && shooting_get_drive_mode())? MAKE_COLOR(conf.osd_color>>8,COLOR_RED):(!conf.strip_offset && conf.raw_strip_mode && shooting_get_drive_mode())?MAKE_COLOR(conf.osd_color>>8,COLOR_BLUE):conf.osd_color,1);
      else if((shooting_get_tick_count()-gDngFinish)<2000) 
           {
            sprintf(osd_buf,"%4d",gDngFinish-gDngStart);
            draw_string(conf.mode_state_pos.x-FONT_WIDTH, conf.mode_state_pos.y,osd_buf,MAKE_COLOR(conf.osd_color>>8,COLOR_RED),1);
           }
      else if(gDngFinish) gDngFinish=0;
        n+=FONT_HEIGHT;
    }
}
 
void gui_osd_draw_big_zoom()
{
 static int zcnt = 30; 
 int fl,zoom;
 char sn[6];
 char sf[6];
 static int nfi;
 static int fmd; 
 static int fm = 0; 
 static int didShowFocus=0,gotFocus=0;
 int XPos,YPos;
 unsigned char zf_size = 0;
 
 if(conf.zf_and_depth&&!state_kbd_script_run)zf_size = 2;
 else zf_size = conf.zf_size;
 
 if(bigZoomXPos && bigZoomYPos && state_kbd_script_run )
 {
  XPos = bigZoomXPos;
  YPos = bigZoomYPos;
 }
 
 else
{
 if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
  XPos = (screen_width/2)-(4*FONT_WIDTH*zf_size)-((FONT_WIDTH/2)*zf_size);
 else
  XPos = (screen_height/2)-(4*FONT_WIDTH*zf_size)-((FONT_WIDTH/2)*zf_size);
 YPos=conf.zoom_pos.y;
}

#if !defined(CAMERA_m3)
 unsigned char mmode = MODE_IS_VIDEO(mode_get()& MODE_SHOOTING_MASK);

 zoom=gZoomPoint;
 if(zoom_busy)zcnt=30;
 
   if(((zoom != PrevZoom)||conf.zf_big_permanent)&&!gDidShowFocus) 
    {
	 if(bigZoomXPos && bigZoomYPos && state_kbd_script_run )
	  {
	   switch (conf.zoom_value) 
       {
      
        case ZOOM_SHOW_FL:
            fl=get_focal_length(zoom);
            sprintf(osd_buf, "%3d.%-2d", fl/1000, fl%1000/100);
            break;
        case ZOOM_SHOW_FOV:
        case ZOOM_SHOW_EFL:
            fl=get_effective_focal_length(zoom);
            sprintf(osd_buf, "%4d", fl/1000);
            break;
            
         case ZOOM_SHOW_STEP:   
             sprintf(osd_buf, "%2d", gSyncZoomStep);
            break;
            
        case ZOOM_SHOW_X:
        default:
            fl=get_zoom_x(zoom);
            sprintf(osd_buf, "%ld/%d.%dx", zoom, fl/10, fl%10);
            break;   
	    }
	  }
	  else
    {
     switch (conf.zoom_value) 
     {    
        case ZOOM_SHOW_FL:
            fl=get_focal_length(zoom);
            sprintf(osd_buf, " %3d.%-2d", fl/1000, fl%1000/100);
            break;
        case ZOOM_SHOW_FOV:
        case ZOOM_SHOW_EFL:
            fl=get_effective_focal_length(zoom);
            sprintf(osd_buf, "   %3d", fl/1000);
            break;
            
         case ZOOM_SHOW_STEP:   
             sprintf(osd_buf, "    %2d", gSyncZoomStep);
            break;
            
        case ZOOM_SHOW_X:
        default:
            fl=get_zoom_x(zoom);
            sprintf(osd_buf, "%  ld/%d.%dx", zoom, fl/10, fl%10);
            break;         
     }
  }
          draw_string(XPos,YPos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);   
     YPos+=FONT_HEIGHT*zf_size;
 
   if(((conf.zf_and_depth&&!state_kbd_script_run)||state_kbd_script_run) && !zoom_busy && !mmode) 
    {
     nfi = nearForInfinity();
     fmd = farForMaxDeviation();
       if(!gEnableBigZoom) 
        {
           if(nfi <1000)
           sprintf(osd_buf,"%3dmm/INF",nfi);
           else if(nfi<=3000)
           sprintf(osd_buf," %1d.%1dm/INF",nfi/1000,(nfi%1000)/100);
           else sprintf(osd_buf," %2dm/INF ",nfi/1000);
           if(!mmode && !(state_kbd_script_run && gEnableBigZoom))
            draw_string(XPos,YPos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);
            YPos+=FONT_HEIGHT*zf_size;
           if(conf.rangefinder_near<1000)
            sprintf(sn,"%3dmm/",conf.rangefinder_near);
           else if(conf.rangefinder_near<=3000)
            sprintf(sn," %1d.%1dm/",conf.rangefinder_near/1000,(conf.rangefinder_near%1000)/100);
           else sprintf(sn,"  %2d/",conf.rangefinder_near/1000);

           if(fmd<1000)
            sprintf(sf,"%-3dmm",fmd);
           else if(fmd<=3000)
            sprintf(sf,"%-1d.%-1dm ",fmd/1000,((fmd+50)%1000)/100);
           else sprintf(sf,"%-2dm  ",(fmd+500)/1000);
            sprintf(osd_buf,"%s%s",sn,sf);
           if(!mmode && !(state_kbd_script_run && gEnableBigZoom))
            draw_string(XPos,YPos,osd_buf,(conf.rangefinder_near<(conf.stereo_spacing*conf.minimum_dist_factor))?MAKE_COLOR(COLOR_WHITE,COLOR_RED):(conf.camera_position)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),zf_size);
            YPos+=FONT_HEIGHT*zf_size;  
                      
            int far;
            if((conf.rangefinder_far>=CAMERA_MAX_DIST)||(conf.rangefinder_far<0))far=65000;
            else far = conf.rangefinder_far;            
            long base = ((get_sensor_width()*far/conf.stereo_deviation/gFocalPoint)*conf.rangefinder_near)/(far-conf.rangefinder_near);

       if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3)) 
       {
        sprintf(osd_buf,"N:%d F:%d B:%d",conf.rangefinder_near,far,base);       
        XPos = (screen_width-(strlen(osd_buf)*FONT_WIDTH*zf_size))/2;
        draw_string(XPos,YPos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);        
        YPos+=FONT_HEIGHT*zf_size;
        }
       else 
       {
        base = (base*CAM_JPEG_HEIGHT)/CAM_JPEG_WIDTH;
        sprintf(osd_buf,"N:%d F:%d",conf.rangefinder_near,far);       
        XPos = (screen_height-(strlen(osd_buf)*FONT_WIDTH*zf_size))/2;     
        draw_string(XPos,YPos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);           
        YPos+=FONT_HEIGHT*zf_size;
        sprintf(osd_buf,"B:%d",base);        
        draw_string(XPos,YPos,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);         
        YPos+=FONT_HEIGHT*zf_size;
       }
      }
    }
   }  
 
  if(!zcnt--)
   {
    if(zoom != PrevZoom)vid_bitmap_refresh();
    PrevZoom = zoom;
    zcnt=30;
   }
 #endif

  if(gDidShowFocus==1)
   {
    if(gNearFar) 
    {
     if(conf.rangefinder_far== -1)sprintf(osd_buf," INF  F");
     else sprintf(osd_buf," %d F",conf.rangefinder_far);
    }
    else 
    {
     if(conf.rangefinder_near== -1)sprintf(osd_buf," Error:INF for near !");
     else sprintf(osd_buf,"%5d N",conf.rangefinder_near);
    }   
    draw_string(XPos,conf.zoom_pos.y,osd_buf,(conf.camera_position == 1)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_BLUE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),zf_size);    
   }
#if !defined(CAMERA_m3)
  else if(gDidShowFocus==2)
   play_sound(6);
#endif
   if (!kbd_is_key_pressed(KEY_SHOOT_HALF)&& gDidShowFocus) 
    {
     draw_string(XPos,conf.zoom_pos.y,"     ",MAKE_COLOR(COLOR_TRANSPARENT,COLOR_TRANSPARENT),zf_size);
     gDidShowFocus = 0;
    }
}

void set_prev_zoom(int zp)
{
 PrevZoom = zp;
}


void gui_osd_draw_values() {
    int zp=gZoomPoint, av=shooting_get_real_av(), fl, lfp;
#if !defined(CAMERA_m3)
    switch (conf.zoom_value) {
        case ZOOM_SHOW_FL:
            fl=get_focal_length(zp);
            sprintf(osd_buf, "Z:%d.%dmm%8s", fl/1000, fl%1000/100, "");
            break;
        case ZOOM_SHOW_EFL:
            fl=get_effective_focal_length(zp);
            sprintf(osd_buf, "Z:%3dmm%8s", fl/1000, "");
            break;
        case ZOOM_SHOW_X:
        default:
            fl=get_zoom_x(zp);
            sprintf(osd_buf, "Z:%ld/%d.%dx%8s", zp, fl/10, fl%10, "");
            break;
    }
    osd_buf[8]=0;
    draw_string(conf.values_pos.x, conf.values_pos.y, osd_buf,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE),1);
#endif
#if !defined(CAMERA_ixus40)
    lfp=lens_get_focus_pos_from_lens();
#else
    lfp = shooting_get_prop_int(PROPCASE_SUBJECT_DIST1);
#endif
    strcpy(osd_buf, "D:");
    sprintf_dist(osd_buf+2, lfp);
    sprintf(osd_buf+strlen(osd_buf), "%8s", "");
    osd_buf[8]=0;
    draw_string(conf.values_pos.x, conf.values_pos.y+FONT_HEIGHT, osd_buf,MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE),1);

    if(subject_far<64000)sprintf(osd_buf,"F:%5d",subject_far);
    else  sprintf(osd_buf,"  INF");
    draw_string(conf.values_pos.x, conf.values_pos.y+2*FONT_HEIGHT, osd_buf,(deviation<my_max_deviation)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),1);

    sprintf(osd_buf,"N:%5d",conf.rangefinder_near,"");
    draw_string(conf.values_pos.x, conf.values_pos.y+3*FONT_HEIGHT, osd_buf,(conf.rangefinder_near>conf.stereo_spacing*conf.minimum_dist_factor)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED),1);

   color cl;
    cl = (1.342*realav > coc*1000)?MAKE_COLOR(COLOR_TRANSPARENT,COLOR_RED):MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE);
#if !defined(CAMERA_ixus40)
    sprintf(osd_buf, "Av:%d.%02d ", myav/100, myav%100);
#else
    sprintf(osd_buf, "Av:%d.%02d ",PROPCASE_ALLBEST_AV/100,PROPCASE_ALLBEST_AV %100);
#endif
    osd_buf[8]=0;
    draw_string(conf.values_pos.x, conf.values_pos.y+FONT_HEIGHT*4, osd_buf,cl,1);

}

void gui_osd_draw_clock() {
    unsigned long t;
    static struct tm *ttm;

    t = time(NULL);
    ttm = localtime(&t);
    sprintf(osd_buf, "%2u:%02u", ttm->tm_hour, ttm->tm_min);
    draw_string(conf.clock_pos.x, conf.clock_pos.y, osd_buf, conf.osd_color,1);
}

void gui_bg(int posx,int posy,int value,color bg,color fg,int wide,int blocks)
{
   int x;

   if(value<0)value = 0;
   if(value>100) value = 100;

    for(x=0;x<blocks;x++)
    {
     osd_buf[x]=32;
    }

    osd_buf[blocks] = 0;

    value= (int)(value*blocks*0.08);
    
    for(x=0;x<value/8;x++)
    {
     osd_buf[x]=31;
    }

    if(!value%8) osd_buf[x] = 23+value%8;
    osd_buf[blocks] = 0;

    draw_string(posx,posy, osd_buf, MAKE_COLOR(bg,fg),1);
    if(wide) 
     {
      draw_string(posx,posy+FONT_HEIGHT, osd_buf, MAKE_COLOR(bg,fg),1);
      draw_rect(posx,posy,posx+blocks*8,posy+32,COLOR_BLACK);
     }

    else
    {
    draw_string(posx,posy, osd_buf, MAKE_COLOR(bg,fg),1);
    draw_rect(posx,posy,posx+blocks*8,posy+16,COLOR_BLACK);
    }
}

