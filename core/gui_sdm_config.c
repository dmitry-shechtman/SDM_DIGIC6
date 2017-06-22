#include "stdlib.h"
#include "gui.h"
#include "draw_palette.h"
#include "conf.h"
#include "platform.h"
#include "gui_lang.h"
#include "lang.h"

#define SCREEN_COLOUR 0xff
#define TITLE_COLOUR (MAKE_COLOR(COLOR_BLUE,COLOR_WHITE))
static int need_redraw;
color cl;

void gui_sdm_config_draw()
{
 int bg,second_column,n;
 int line =0,prop_id;
 short int vv;

#if defined(CAMERA_ixus980) || defined(CAMERA_ixus100)
 int  sw = vid_get_bitmap_width()-360;
#elif defined(CAMERA_tx1)
 int  sw = vid_get_bitmap_width()-120;
#else
 int  sw = vid_get_bitmap_width();
#endif
 bg = (!conf.camera_position)?COLOR_RED:COLOR_BLUE;    

  if(need_redraw)
   {
    static char str[32];

    draw_filled_rect(0,0,sw-1,FONT_HEIGHT,MAKE_COLOR(bg,bg));
    draw_filled_rect(0,FONT_HEIGHT,sw-1,screen_height-1,MAKE_COLOR(SCREEN_COLOUR,SCREEN_COLOUR));

    sprintf(str,"SDM Ver:%s",SDM_VERSION);
    draw_string(4,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);

    sprintf(str,(!conf.camera_position)?lang_str(LANG_LEFT_CAMERA):lang_str(LANG_RIGHT_CAMERA));
    draw_string(sw/2- 6*FONT_WIDTH,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);

    sprintf(str,"Synch %s",(conf.synch_enable)?lang_str(LANG_MENU_ON):lang_str(LANG_MENU_OFF));
    draw_string(sw-10*FONT_WIDTH,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);

    bg = SCREEN_COLOUR;

    second_column = 24*FONT_WIDTH;

    line+= (int)(1.2*FONT_HEIGHT);
#if defined(CAMERA_a610) || defined(CAMERA_a620) || defined(CAMERA_a630) || defined(CAMERA_a640) || defined(CAMERA_a710) || defined(CAMERA_a720) || defined(CAMERA_a570) || defined (CAMERA_ixus800) || defined(CAMERA_g7)  || defined(CAMERA_a550) || defined(CAMERA_a460) || defined(CAMERA_ixus70) || defined(CAMERA_ixus55) || defined(CAMERA_ixus950)|| defined(CAMERA_a560)
    
    sprintf(str,lang_str(LANG_MENU_SYNCH_DELAY_VALUE ));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    n = conf.synch_coarse_delay * 1000 + conf.synch_fine_delay;
    if(conf.synch_delay_enable) sprintf(str,"%d.%04d",n/10000,n%10000);
    else sprintf(str,lang_str(LANG_SELECT_INACTIVE));
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;
#endif
     
    get_property_case(PROPCASE_FLASH_MODE,&vv, sizeof(vv));               
    sprintf(str,lang_str(LANG_SELECT_FLASH));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    if(vv == 0) sprintf(str,"AUTO");
    if(vv == 1) sprintf(str,lang_str(LANG_MENU_ON));
    if(vv == 2) sprintf(str,lang_str(LANG_MENU_OFF));
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;

    sprintf(str,lang_str(LANG_MENU_SCRIPT_REMOTE_ENABLE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    sprintf(str,(conf.remote_enable)?lang_str(LANG_ENABLED):lang_str(LANG_DISABLED));
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;

    sprintf(str,lang_str(LANG_MENU_OSD_CAM_SPACING));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    sprintf(str,"%d",conf.stereo_spacing);
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;

    sprintf(str,lang_str(LANG_MENU_OSD_DEV_FACTOR));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    sprintf(str,"%d",conf.stereo_deviation);
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
      line+= FONT_HEIGHT;

    sprintf(str,lang_str(LANG_MENU_OSD_NEAR_FACTOR ));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    sprintf(str,"%d",conf.minimum_dist_factor);
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;

    sprintf(str,lang_str(LANG_MENU_OSD_FOCUS_MODE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    if(conf.focus_mode == 0)sprintf(str,lang_str(LANG_MENU_INFINITY));
    if(conf.focus_mode == 1)sprintf(str,lang_str(LANG_MENU_HYPERFOCAL));
    if(conf.focus_mode == 2)sprintf(str,lang_str(LANG_SELECT_MANUAL));
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
       line+= FONT_HEIGHT;

#if !defined (CAMERA_ixus700) 
    sprintf(str,lang_str(LANG_MENU_OSD_DISTANCE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    sprintf(str,"%d",conf.distance_setting);
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;
#endif

    sprintf(str,lang_str(LANG_MENU_RAW_SHUTTER_OVERRIDE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
   if (conf.tv_override_enable)sprintf(str,"%s",tv_enum());
   else sprintf(str,"Off");
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;
#if !defined (CAMERA_ixus700) && !defined (CAMERA_ixus800) && !defined (CAMERA_a560) && !defined(CAMERA_ixus70) && !defined(CAMERA_ixus55)&& !defined (CAMERA_a460) && !defined(CAMERA_ixus950) && !defined(ixus850)
    sprintf(str,lang_str(LANG_MENU_RAW_AV_OVERRIDE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    if (conf.av_override_value == 0) sprintf(str,lang_str(LANG_MENU_OFF));
    else
     {
      prop_id=shooting_get_aperture_from_av96(shooting_get_av96_override_value());	
	sprintf(str, "%d.%02d", (int)prop_id/100, (int)prop_id%100 );
     }
     draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
        line+= FONT_HEIGHT;
#endif

    sprintf(str,lang_str(LANG_MENU_TV_BRACKET_VALUE));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    if(conf.tv_bracket_value == 0) sprintf(str,lang_str(LANG_MENU_OFF));
    else
    {
      if(conf.bracket_type == 0)sprintf(str,"%s %s",tv_bracket_values_enum(),lang_str(LANG_MENU_ALTERNATE));
      if(conf.bracket_type == 1)sprintf(str,"%s %s",tv_bracket_values_enum(),lang_str(LANG_MENU_SLOWER));
      if(conf.bracket_type == 2)sprintf(str,"%s %s",tv_bracket_values_enum(),lang_str(LANG_MENU_FASTER));
    }
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
       line+= FONT_HEIGHT;
#if !defined (CAMERA_ixus700) 
    sprintf(str,lang_str(LANG_MENU_IMAGE_STACK));
    draw_string(0,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
    if(conf.dist_mode == 0)    sprintf(str,lang_str(LANG_MENU_OFF));
    else if(conf.dist_mode==1) sprintf(str,"%d %s",conf.subj_dist_bracket_value,lang_str(LANG_MENU_OUTWARDS));
    else if(conf.dist_mode==2) sprintf(str,"AUTO");
    else if(conf.dist_mode==3) sprintf(str,"Digiscope");
    draw_string(second_column,line,str,MAKE_COLOR(bg,COLOR_WHITE),1);
       line+= FONT_HEIGHT;
#endif

    need_redraw = 0;
   }
}

 void gui_sdm_config_init()
{
 need_redraw = 1;
}

