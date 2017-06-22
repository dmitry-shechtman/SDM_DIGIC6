#include "stdlib.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "conf.h"
#include "ubasic.h"
#include "font.h"
#include "lang.h"
#include "gui.h"
#include "draw_palette.h"
#include "gui_menu.h"
#include "gui_read.h"
#include "kbd.h"
#include "gui_lang.h"

#define MENUSTACK_MAXDEPTH  3

typedef struct {
    CMenu       *menu;
    int         curpos;
    int         toppos;
} CMenuStacked;
 
extern char selected_file[];
extern volatile enum Gui_Mode gui_mode;
extern unsigned char gScriptFolder;
static CMenu        *curr_menu;
static CMenuStacked gui_menu_stack[MENUSTACK_MAXDEPTH];
static unsigned int gui_menu_stack_ptr;
static int          gui_menu_curr_item;
static int          gui_menu_top_item;
int          gui_menu_redraw;
unsigned char g8_16_forced=0,gFastExit=0; 
static int          count;
static int        x, y, w, num_lines;
static int          len_bool, len_int, len_enum, len_space, len_br1, len_br2, cl_rect;
static int          int_incr,incr_modified;
static unsigned char *item_color;
static OSD_pos menu_title_pos;
#if CAM_MULTIPART
extern int cp;         
#endif
time_t t,menu_time;              
static struct tm *ttm; 
static char buf[64];          
char mybuf[64];
static struct 
{
int x;
int yy;
int w;
}title;
static void gui_menu_set_curr_menu(CMenu *menu_ptr, int top_item, int curr_item) {
    curr_menu = menu_ptr;
    gui_menu_top_item = top_item;
    gui_menu_curr_item = curr_item;
}


void gui_menu_init(CMenu *menu_ptr) 
{
   int width  = vid_get_bitmap_screen_width();
   int height = vid_get_bitmap_screen_height();

    if (menu_ptr) 
    {
       gui_menu_set_curr_menu(menu_ptr, 0, 0);  
       gui_menu_stack_ptr = 0;
    }
if((conf.camera_orientation==1)||(conf.camera_orientation==3))        
    {
     w = height;
     height = width;
     x = 0;
 #if defined(CAMERA_s100)  
    if(conf.camera_orientation==3)x = 30;
 #endif   
    }

   else
 
    { 
#if defined(CAMERA_g11)|| defined (CAMERA_s90)|| defined (CAMERA_s95)|| defined (CAMERA_s100)
    w = screen_width-10-10;
#else
      w = width-32-32; 
#endif   
      x = 30; 
    }
    num_lines = (height-26-16)/FONT_HEIGHT-2;
    y = ((height-num_lines*FONT_HEIGHT)>>1);
    len_bool = FONT_WIDTH;
    len_int = 6*FONT_WIDTH;
    len_enum = 7*FONT_WIDTH;
    len_space = FONT_WIDTH;
    len_br1 = FONT_WIDTH;
    len_br2 = FONT_WIDTH;
    cl_rect = FONT_HEIGHT - 4;

    int_incr = 1;
    
    gui_menu_redraw=2;
}
int str_width(const char *str)
{
 return strlen(str)*FONT_WIDTH;
}
void draw_string_centre_len(int x, int y, int len, const char *str, color cl,int m) 
{
 int spaces = len/FONT_WIDTH - strlen(str);
 int left = (spaces/2)+strlen(str);
  sprintf(mybuf,"%*s",left,str);
  draw_string(x,y,mybuf,cl,m);
  spaces = spaces-spaces/2;
  sprintf(mybuf,"%*s",spaces," ");         
  draw_string(x+(left*FONT_WIDTH),y,mybuf,cl,m);
}
int draw_string_len(int x, int y, int len, const char *str, color cl,int m) 
{
  sprintf(mybuf,"%-*s",len/8,str);
  draw_string(x,y,mybuf,cl,m); 
  return len;
}
int draw_string_right_len(int x, int y, int len, const char *str, color cl,int m) 
{
  sprintf(mybuf,"%*.*s",len/8,len/8,str);
  draw_string(x,y,mybuf,cl,m);
  return len;
} 

static void gui_menu_color_selected(color clr)  
{
    *item_color = (unsigned char)(clr&0xFF);
    gui_menu_redraw=2;
    draw_restore();
    gui_force_restore();
}

static void gui_menu_back() 
{
  if(!gui_menu_stack_ptr&&!strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT3_SUB_TITLE)))     
    {
     conf_save_new_settings_if_changed();  
     gui_mode = GUI_MODE_ALT;
     draw_restore();
     gFastExit=1;                            
    }   
    else if (gui_menu_stack_ptr > 0)
    {
        gui_menu_stack_ptr--;
        gui_menu_set_curr_menu(gui_menu_stack[gui_menu_stack_ptr].menu, gui_menu_stack[gui_menu_stack_ptr].toppos, gui_menu_stack[gui_menu_stack_ptr].curpos);
        gui_menu_redraw=2;
        draw_restore();
        gui_force_restore();
    }
}
 
void menu_title(x,y,w)
{
 int rec=((mode_get()&MODE_MASK) == MODE_REC);
if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT2_TITLE))==0)
 gScriptFolder=1;
else if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT3_TITLE))==0)
 gScriptFolder=2;
 
else if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_MAIN_SCRIPT_PARAM))==0)
 {
  gScriptFolder=0;
  script_load(conf.user_script_file,0);
 }
 t=time(NULL);                                                  
 menu_time=t;
 if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_BASIC_TITLE))!=0)
 {
 if((!curr_menu->title_char_size)&&(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT2_SUB_TITLE))!=0))                                    
   {                                                          
    draw_string_centre_len(x,y,w,lang_str(curr_menu->title),MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
   }
 }
}
void gui_menu_kbd_process() 
{
 static char sbuf[7];
 int rec=((mode_get()&MODE_MASK) == MODE_REC);
 int zi;
 unsigned char ac_key;
 static unsigned char prev_key;
 static unsigned int pressedDuration=0;
 if(conf.camera_orientation==1) zi=1;  
 else zi=0;
 ac_key = kbd_get_pressed_key();
 if(ac_key==conf.alt_mode_button)           
 {
  if((gScriptFolder==1)&&conf.script_file)   
  {
   gui_mode = GUI_MODE_READ;
   gui_read_init(conf.script_file);   
  }
  else if((gScriptFolder==2)&& conf.user_script_file)   
  {
   gui_mode = GUI_MODE_READ;
   gui_read_init(conf.user_script_file);   
  }
  else if(selected_file)  
   {
    gui_mode = GUI_MODE_READ;
    gui_read_init(selected_file);
   }
 }
 
 else if(ac_key)
 {
  ++pressedDuration; 
  prev_key=ac_key;
 }
 
 else if(!ac_key && pressedDuration)
 {
  pressedDuration=0;
 switch(get_vkey(prev_key))
 {
        case KEY_UP:
            do 
             {
                if (gui_menu_curr_item>0) 
                {
                    if (gui_menu_curr_item-1==gui_menu_top_item && gui_menu_top_item>0) 
                        --gui_menu_top_item;
                    --gui_menu_curr_item;
                } 
               else 
                {
                    int i;
                    while (curr_menu->menu[gui_menu_curr_item+1].text)
                        ++gui_menu_curr_item;
                    gui_menu_top_item = gui_menu_curr_item - num_lines +1;
                    if (gui_menu_top_item<0) gui_menu_top_item=0;
                }
             } while ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_TEXT || 
                     (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_SEPARATOR);
#if CAM_HAS_ZOOM_LEVER
              if(gui_menu_stack_ptr)
               {
		    sprintf(sbuf, "±%d",int_incr);
		    draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"     ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
		    draw_string(conf.increment_pos.x,conf.increment_pos.y,sbuf,MAKE_COLOR((rec)?COLOR_SELECTED_BG:COLOR_SELECTED_BG_P,(rec)?COLOR_SELECTED_FG:COLOR_SELECTED_FG_P),1);
               }
#endif
            gui_menu_redraw=1;
            break;

 
        case KEY_DOWN:
            do 
             {
                if (curr_menu->menu[gui_menu_curr_item+1].text) 
                 {
                    int i;
                    for (i=0; i<num_lines-1 && curr_menu->menu[gui_menu_top_item+i].text; ++i);
                    if (i==num_lines-1 && curr_menu->menu[gui_menu_top_item+i].text 
                        && gui_menu_top_item+i-1==gui_menu_curr_item && curr_menu->menu[gui_menu_top_item+i+1].text)
                        ++gui_menu_top_item;
                    ++gui_menu_curr_item;
                 } 
                else 
                 {
                    gui_menu_curr_item = gui_menu_top_item = 0;
                 }
             } while ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_TEXT || 
                     (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_SEPARATOR);
#if CAM_HAS_ZOOM_LEVER
              if(gui_menu_stack_ptr)
               {
		    sprintf(sbuf, "±%d",int_incr);
		    draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"     ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
		    draw_string(conf.increment_pos.x,conf.increment_pos.y,sbuf,MAKE_COLOR((rec)?COLOR_SELECTED_BG:COLOR_SELECTED_BG_P,(rec)? COLOR_SELECTED_FG:COLOR_SELECTED_FG_P),1);
               }
#endif
            gui_menu_redraw=1;
            break;
 

        case KEY_LEFT:
            if (gui_menu_curr_item>=0) 
              {
                switch (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK) 
                  {
                    case MENUITEM_INT:
                        switch (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_ARG_MASK) 
                           {
                            case MENUITEM_ARG_INC:
                                *(curr_menu->menu[gui_menu_curr_item].value) -= curr_menu->menu[gui_menu_curr_item].arg;
                                break;
                            case MENUITEM_ARG_ADDR_INC:
                                *(curr_menu->menu[gui_menu_curr_item].value) -= *(int *)(curr_menu->menu[gui_menu_curr_item].arg);
                                break;
                            default:
                                *(curr_menu->menu[gui_menu_curr_item].value) -= int_incr;
                                break;

                           }
                        if (*(curr_menu->menu[gui_menu_curr_item].value) < -9999) *(curr_menu->menu[gui_menu_curr_item].value) = -9999;
                        if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_UNSIGNED) 
                           {
                            if (*(curr_menu->menu[gui_menu_curr_item].value) < 0) *(curr_menu->menu[gui_menu_curr_item].value) = 0;
                            if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_MIN) 
                              {
                                if (*(curr_menu->menu[gui_menu_curr_item].value) < (unsigned short)(curr_menu->menu[gui_menu_curr_item].arg & 0xFFFF)) 
                                    *(curr_menu->menu[gui_menu_curr_item].value) = (unsigned short)(curr_menu->menu[gui_menu_curr_item].arg & 0xFFFF);
                              }
                            } 
                            else 
                            {
                            if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_MIN) 
                              {
                                if (*(curr_menu->menu[gui_menu_curr_item].value) < (short)(curr_menu->menu[gui_menu_curr_item].arg & 0xFFFF)) 
                                    *(curr_menu->menu[gui_menu_curr_item].value) = (short)(curr_menu->menu[gui_menu_curr_item].arg & 0xFFFF);
                              }
                             }
                        if ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_ARG_MASK) == MENUITEM_ARG_CALLBACK && curr_menu->menu[gui_menu_curr_item].arg) 
                           {
                            ((void (*)())(curr_menu->menu[gui_menu_curr_item].arg))();
                           }
                        if (curr_menu->on_change) 
                           {
                            curr_menu->on_change(gui_menu_curr_item);
                           }
#if CAM_HAS_ZOOM_LEVER
                        if (incr_modified) 
                           {
				    incr_modified=0;
                            draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"    ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
				   }
#endif
                        gui_menu_redraw=1;
                        break;

                    case MENUITEM_ENUM:
                        if (curr_menu->menu[gui_menu_curr_item].value) {
                            ((const char* (*)(int change, int arg))(curr_menu->menu[gui_menu_curr_item].value))(-1, curr_menu->menu[gui_menu_curr_item].arg);
                        }
                        if (curr_menu->on_change) 
                        {
                         curr_menu->on_change(gui_menu_curr_item);
                        }
                        gui_menu_redraw=1;
                        break;
                    case MENUITEM_UP:
                        gui_menu_back();
                        break;
                }
            } else {
                gui_menu_back();
            }
            break;


        case KEY_RIGHT:
            if (gui_menu_curr_item>=0) {
                switch (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK){
                    case MENUITEM_INT:

                        switch (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_ARG_MASK) {
                            case MENUITEM_ARG_INC:
                                *(curr_menu->menu[gui_menu_curr_item].value) += curr_menu->menu[gui_menu_curr_item].arg;
                                break;
                            case MENUITEM_ARG_ADDR_INC:
                                *(curr_menu->menu[gui_menu_curr_item].value) += *(int *)(curr_menu->menu[gui_menu_curr_item].arg);
                                break;
                            default:
                                *(curr_menu->menu[gui_menu_curr_item].value) += int_incr;
                                break;
                        }
                        if (*(curr_menu->menu[gui_menu_curr_item].value) > 999999) 
                            *(curr_menu->menu[gui_menu_curr_item].value) = 999999;
                        if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_UNSIGNED) {
                            if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_MAX) {
                                if (*(curr_menu->menu[gui_menu_curr_item].value) > (unsigned short)((curr_menu->menu[gui_menu_curr_item].arg>>16) & 0xFFFF)) 
                                    *(curr_menu->menu[gui_menu_curr_item].value) = (unsigned short)((curr_menu->menu[gui_menu_curr_item].arg>>16) & 0xFFFF);
                            }
                        } else {
                            if ( curr_menu->menu[gui_menu_curr_item].type & MENUITEM_F_MAX) {
                                if (*(curr_menu->menu[gui_menu_curr_item].value) > (short)((curr_menu->menu[gui_menu_curr_item].arg>>16) & 0xFFFF)) 
                                    *(curr_menu->menu[gui_menu_curr_item].value) = (short)((curr_menu->menu[gui_menu_curr_item].arg>>16) & 0xFFFF);
                            }
                        }
                        if ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_ARG_MASK) == MENUITEM_ARG_CALLBACK && curr_menu->menu[gui_menu_curr_item].arg) {
                            ((void (*)())(curr_menu->menu[gui_menu_curr_item].arg))();
                        }
                        if (curr_menu->on_change) {
                            curr_menu->on_change(gui_menu_curr_item);
                        }

#if CAM_HAS_ZOOM_LEVER
                        if (incr_modified) 
                         {
				  incr_modified=0;
                          draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"    ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
				 }
#endif

                        gui_menu_redraw=1;
                        break;

                    case MENUITEM_ENUM:
                        if (curr_menu->menu[gui_menu_curr_item].value) {
                            ((const char* (*)(int change, int arg))(curr_menu->menu[gui_menu_curr_item].value))(+1, curr_menu->menu[gui_menu_curr_item].arg);
                        }
                        if (curr_menu->on_change)                     
                        {
                         curr_menu->on_change(gui_menu_curr_item);    
                        }
                        gui_menu_redraw=1;
                        break;

                    case MENUITEM_SUBMENU:
                        gui_menu_stack[gui_menu_stack_ptr].menu = curr_menu;
                        gui_menu_stack[gui_menu_stack_ptr].curpos = gui_menu_curr_item;
                        gui_menu_stack[gui_menu_stack_ptr].toppos = gui_menu_top_item;
                        gui_menu_set_curr_menu((CMenu*)(curr_menu->menu[gui_menu_curr_item].value), 0, 0);
                            if ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_TEXT || (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_SEPARATOR) 
                            {
                              ++gui_menu_curr_item;
                            }

                        gui_menu_stack_ptr++;
                        
                        if (gui_menu_stack_ptr > MENUSTACK_MAXDEPTH)
                         {
                            draw_txt_string(0, 0, "E1", MAKE_COLOR(COLOR_RED, COLOR_YELLOW));
                            gui_menu_stack_ptr = 0;
                         }

                        gui_menu_redraw=2;
                        draw_restore();
                        gui_force_restore();
                        break;
                }
            }
            break;

        case KEY_SET:
            if (gui_menu_curr_item>=0) {
                switch (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK){

                    case MENUITEM_BOOL:
                        *(curr_menu->menu[gui_menu_curr_item].value) =
                                !(*(curr_menu->menu[gui_menu_curr_item].value));
                        if ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_ARG_MASK) == MENUITEM_ARG_CALLBACK && curr_menu->menu[gui_menu_curr_item].arg) {
                            ((void (*)())(curr_menu->menu[gui_menu_curr_item].arg))();
                        }
                        if (curr_menu->on_change) {
                            curr_menu->on_change(gui_menu_curr_item);
                        }
                        gui_menu_redraw=1;
                        break;

                    case MENUITEM_PROC:
                    	if (curr_menu->menu[gui_menu_curr_item].value) {
                            ((void (*)(int arg))(curr_menu->menu[gui_menu_curr_item].value))(curr_menu->menu[gui_menu_curr_item].arg);
                            if (curr_menu->on_change) {
                                curr_menu->on_change(gui_menu_curr_item);
                            }
                            gui_menu_set_curr_menu(curr_menu, 0, -1);
                            gui_menu_redraw=2;
                        }
                        break;

                    case MENUITEM_SUBMENU:
                        gui_menu_stack[gui_menu_stack_ptr].menu = curr_menu;
                        gui_menu_stack[gui_menu_stack_ptr].curpos = gui_menu_curr_item;
                        gui_menu_stack[gui_menu_stack_ptr].toppos = gui_menu_top_item;
                        gui_menu_set_curr_menu((CMenu*)(curr_menu->menu[gui_menu_curr_item].value), 0, 0);
                            if ((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_TEXT || (curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_SEPARATOR) 
                             {
                               ++gui_menu_curr_item;
                             }
      if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT3_SUB_TITLE))==0)
      {
       gScriptFolder=2;  
      }
      else if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT2_SUB_TITLE))==0)  
      {
       gScriptFolder=1;      
       script_load(conf.script_file,0);   
      }

                        gui_menu_stack_ptr++;
                        
                        if (gui_menu_stack_ptr > MENUSTACK_MAXDEPTH)
                        {
                            draw_txt_string(0, 0, "E1", MAKE_COLOR((rec)?COLOR_RED:COLOR_RED_P,(rec)? COLOR_YELLOW:COLOR_YELLOW_P));
                            gui_menu_stack_ptr = 0;
                        }

                        gui_menu_redraw=2;
                        draw_restore();
                        gui_force_restore();
                        break;

                    case MENUITEM_UP:
                        gui_menu_back();
                        break;

                    case MENUITEM_COLOR_FG:

                    case MENUITEM_COLOR_BG:
                        draw_restore();
                        item_color=((unsigned char*)(curr_menu->menu[gui_menu_curr_item].value)) + (((curr_menu->menu[gui_menu_curr_item].type & MENUITEM_MASK)==MENUITEM_COLOR_BG)?1:0);
                        gui_palette_init(PALETTE_MODE_SELECT, (*item_color)&0xFF, gui_menu_color_selected);
                        gui_set_mode(GUI_MODE_PALETTE);
                        gui_menu_redraw=2;
                        break;

                    case MENUITEM_ENUM:
                        if (curr_menu->menu[gui_menu_curr_item].value) {
                            ((const char* (*)(int change, int arg))(curr_menu->menu[gui_menu_curr_item].value))(+1, curr_menu->menu[gui_menu_curr_item].arg);
                        }
                        gui_menu_redraw=1;
                        break;
                }
            }
            break;
case KEY_ZOOM_OUT:
case KEY_ZOOM_OUT_SLOW:
#if CAM_HAS_ZOOM_LEVER
               if(!zi && (int_incr >= 10)) int_incr /= 10;        
               else if (zi && (int_incr <= 10000))                 
                {
                 int_incr *= 10;
                }
		    sprintf(sbuf, "±%d",int_incr);
		    draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"     ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
		    draw_string(conf.increment_pos.x,conf.increment_pos.y,sbuf,MAKE_COLOR((rec)?COLOR_SELECTED_BG:COLOR_SELECTED_BG_P, (rec)?COLOR_SELECTED_FG:COLOR_SELECTED_FG_P),1);
#endif
            break;
case KEY_ZOOM_IN:
case KEY_ZOOM_IN_SLOW:
#if CAM_HAS_ZOOM_LEVER
                if (!zi &&(int_incr <= 10000)) int_incr *= 10;     
                else if (zi && (int_incr >= 10)) int_incr /= 10;     
		    sprintf(sbuf, "±%d",int_incr);
		    draw_string(FONT_WIDTH*2+conf.increment_pos.x,conf.increment_pos.y,"     ", MAKE_COLOR(COLOR_TRANSPARENT, COLOR_TRANSPARENT),1);
		    draw_string(conf.increment_pos.x,conf.increment_pos.y,sbuf,MAKE_COLOR((rec)?COLOR_SELECTED_BG:COLOR_SELECTED_BG_P,(rec)? COLOR_SELECTED_FG:COLOR_SELECTED_FG_P),1);
 #endif

            break;
    }
    }
}

void gui_menu_draw_initial() {
    static const char *f=" *** ";
    int l, xx, yy;
   int width  = vid_get_bitmap_screen_width();
   int height = vid_get_bitmap_screen_height();
int rec=((mode_get()&MODE_MASK) == MODE_REC);

if(!curr_menu->title_char_size)                                       
 {

  if((conf.camera_orientation==1)||(conf.camera_orientation==3))
  {
   w=height;
   height = width;
   x=0;
  }

  else
 
  { 
   w = width-32-32;    
   x = 30; 
  }

    num_lines = (height-26-16)/FONT_HEIGHT-2;
    y = ((height-num_lines*FONT_HEIGHT)>>1);    
    l = str_width(lang_str(curr_menu->title));
    yy = y-FONT_HEIGHT-FONT_HEIGHT/2;
    menu_title_pos.x=x;
    menu_title_pos.y=yy;
    menu_title(x,yy,w);
 } 

 else                                                                                                                     
 {
     yy = 8;
     if((conf.camera_orientation==1)||(conf.camera_orientation==3))xx=0;
     else xx = (width-15*BIG_FONT_WIDTH)>>1;
     menu_title(xx,yy,width); 
 }

}

void gui_menu_draw() {
    static char tbuf[64];
    int imenu, i, j, yy, xx;
   int width  = vid_get_bitmap_screen_width();
   int height = vid_get_bitmap_screen_height();
    color cl;
    const char *ch = "";
 int rec=((mode_get()&MODE_MASK) == MODE_REC);
      if((conf.camera_orientation==1)||(conf.camera_orientation==3))title.x=0;
     else title.x = (width-15*BIG_FONT_WIDTH)>>1;
   
  if((conf.camera_orientation==1)||(conf.camera_orientation==3))       
    {
      w = height;
      height = width;
      num_lines = (height-26-16)/FONT_HEIGHT-2;
      y = ((height-num_lines*FONT_HEIGHT)>>1);
      x = 0;
    }

   else                                                        
 
    { 
      num_lines = (height-26-16)/FONT_HEIGHT-2;
      w = width-32-32;
      x = 30;
     }
      if(!curr_menu->title_char_size) y = ((height-num_lines*FONT_HEIGHT)>>1);   
      else y = 8+BIG_FONT_HEIGHT;
      t=time(NULL); 

      if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_BASIC_TITLE))==0)      
     {    

    if(t>menu_time)
    {                                                         
      ttm = localtime(&t);                                            
      sprintf(buf,"%02d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
      draw_string((vid_get_bitmap_screen_width()-(8*FONT_WIDTH))/2, menu_title_pos.y,buf,MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);      
    }
     }
     
    else if(strcmp(lang_str(curr_menu->title),lang_str(LANG_MENU_SCRIPT2_SUB_TITLE))==0)      
    {    
     if(t>menu_time)
     {                                                         
      ttm = localtime(&t);                                             
     if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))   
      {
      draw_string_centre_len(4*FONT_WIDTH,menu_title_pos.y,vid_get_bitmap_screen_width()-(8*FONT_WIDTH)-(8*FONT_WIDTH),lang_str(curr_menu->title),MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
      sprintf(buf,"%02d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec); 
      draw_string(vid_get_bitmap_screen_width()-(8*FONT_WIDTH)-4*FONT_WIDTH, menu_title_pos.y,buf,MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);       
      }
     else
      {
       draw_string_centre_len(0,menu_title_pos.y,vid_get_bitmap_screen_height()-(8*FONT_WIDTH),lang_str(curr_menu->title),MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
       sprintf(buf,"%02d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec); 
       draw_string(vid_get_bitmap_screen_height()-(8*FONT_WIDTH), menu_title_pos.y,buf,MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);       
      }
     }
    } 
      
    if (gui_menu_redraw || (t>menu_time))
     {
        if (gui_menu_redraw==2)
            gui_menu_draw_initial();

        gui_menu_redraw=0;
       int font_height;
        for (imenu=gui_menu_top_item, i=0, yy=y; curr_menu->menu[imenu].text && i<num_lines; ++imenu, ++i)
        {
            cl = (gui_menu_curr_item==imenu)?MAKE_COLOR((rec)?COLOR_SELECTED_BG:COLOR_SELECTED_BG_P,(rec)? COLOR_SELECTED_FG:COLOR_SELECTED_FG_P):MAKE_COLOR((rec)?COLOR_BG:COLOR_BG_P,(rec)? COLOR_WHITE:COLOR_WHITE_P);
          if(!curr_menu->title_char_size)                               
           {
               if((conf.camera_orientation==1)||(conf.camera_orientation==3)) xx = 0;
             else xx = x;
            }
           else
            {
               if((conf.camera_orientation==1)||(conf.camera_orientation==3)) xx = 0;            
              else xx = (width-15*BIG_FONT_WIDTH)>>1;
            }

            switch (curr_menu->menu[imenu].type & MENUITEM_MASK) 
          {
            case MENUITEM_BOOL:
               if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                
                {
                xx+=draw_char(xx, yy, ' ', cl,1);
                xx+=draw_string_len(xx, yy, w-len_space-len_space-len_br1-len_bool-len_br2-len_space, lang_str(curr_menu->menu[imenu].text), cl,1);
                xx+=draw_string(xx, yy, " [", cl,1);
                xx+=draw_string_len(xx, yy, len_bool, (*(curr_menu->menu[imenu].value))?"\x95":"", cl,1);
                draw_string(xx, yy, "] ", cl,1);
               }

               else
                {
                 xx+=draw_string_len(xx, yy, w-len_space-len_br1-len_bool-len_br2, lang_str(curr_menu->menu[imenu].text), cl,1);
                 xx+=draw_string(xx, yy, " [", cl,1);
                 xx+=draw_string_len(xx, yy, len_bool, (*(curr_menu->menu[imenu].value))?"\x95":"", cl,1);
                 draw_string(xx, yy, "]", cl,1);
                }

                break;
            case MENUITEM_INT:

                if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                
               {
                xx+=draw_char(xx, yy, ' ', cl,1);
                xx+=draw_string_len(xx, yy, w-len_space-len_space-len_br1-len_int-len_br2-len_space, lang_str(curr_menu->menu[imenu].text), cl,1);
                xx+=draw_string(xx, yy, " [", cl,1);
                sprintf(tbuf, "%d", *(curr_menu->menu[imenu].value));
                xx+=draw_string_right_len(xx, yy, len_int, tbuf, cl,1);
                draw_string(xx, yy, "] ", cl,1);
                }

                else
               {           
                xx+=draw_string_len(xx, yy, w-len_space-len_br1-len_int-len_br2, lang_str(curr_menu->menu[imenu].text), cl,1);
                xx+=draw_string(xx, yy, " [", cl,1);
                sprintf(tbuf, "%d", *(curr_menu->menu[imenu].value));
                xx+=draw_string_right_len(xx, yy, len_int, tbuf, cl,1);
                draw_string(xx, yy, "]", cl,1);
                }
                break;
            case MENUITEM_UP:
            case MENUITEM_SUBMENU:
            case MENUITEM_PROC:
            case MENUITEM_TEXT:

             if(!curr_menu->menu[imenu].char_size)              
              {
                if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                
                {
#if CAM_MULTIPART
                if(curr_menu->menu[imenu].text==308)           
                 {
                  xx+=draw_char(xx, yy, ' ', (cp==1)?MAKE_COLOR((rec)?COLOR_GREEN:COLOR_GREEN_P,(rec)?COLOR_WHITE:COLOR_WHITE_P):MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
                  sprintf(buf,(cp==1)?"%s 1":"%s 2",lang_str(curr_menu->menu[imenu].text));
                  xx+=draw_string_len(xx, yy, w-len_space-len_space,buf,(cp==1)?MAKE_COLOR((rec)?COLOR_GREEN:COLOR_GREEN_P,(rec)?COLOR_WHITE:COLOR_WHITE_P):MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
                  draw_char(xx, yy, ' ', (cp==1)?MAKE_COLOR((rec)?COLOR_GREEN:COLOR_GREEN_P,(rec)?COLOR_WHITE:COLOR_WHITE_P):MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_WHITE:COLOR_WHITE_P),1);
                 }          
                else
                 {
#endif
                  xx+=draw_char(xx, yy, ' ', cl,1);
                  xx+=draw_string_len(xx, yy, w-len_space-len_space, lang_str(curr_menu->menu[imenu].text), cl,1);
                  draw_char(xx, yy, ' ', cl,1);
#if CAM_MULTIPART
                  }
#endif
                 }

                else                                         
                {
                 xx+=draw_string_len(xx, yy, w, lang_str(curr_menu->menu[imenu].text), cl,1);
                }
               }
               else                                         
               {
                if(((conf.camera_orientation==1)||(conf.camera_orientation==3))&&(strcmp(lang_str(curr_menu->menu[imenu].text),lang_str(LANG_MENU_MAIN_TITLE))==0))
                 sprintf(tbuf,"%-15.15s","Settings");
                else  sprintf(tbuf,"%-15.15s",lang_str(curr_menu->menu[imenu].text));
                  draw_big_string(xx,yy,tbuf,cl);
               }

                break;

            case MENUITEM_SEPARATOR:
                if (lang_str(curr_menu->menu[imenu].text)[0]) 
                {
                    j = str_width(lang_str(curr_menu->menu[imenu].text));                  
                #if defined (CAMERA_g11)|| defined (CAMERA_s90)|| defined (CAMERA_s95)|| defined (CAMERA_s100)
		       xx+=((int)w-j-len_space*2)>>1;
                #else
                    xx+=(w-j-len_space*2)>>1;                                                  
                #endif
                    if((lang_str(curr_menu->menu[imenu].text)==script_title)||(lang_str(curr_menu->menu[imenu].text)==script_title2))                                                             
                    {
                     draw_filled_rect(x, yy,x+((w-j)/2), yy+FONT_HEIGHT,MAKE_COLOR(COLOR_BLACK,COLOR_BLACK));
                     draw_string(x+((w-j)/2), yy, lang_str(curr_menu->menu[imenu].text),MAKE_COLOR(COLOR_BLACK,COLOR_WHITE),1);
                     draw_filled_rect(x+j+((w-j)/2), yy,x+w, yy+FONT_HEIGHT,MAKE_COLOR(COLOR_BLACK,COLOR_BLACK));          
                     }
                    else  if((lang_str(curr_menu->menu[imenu].text)==paramdesc[0])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[1])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[2])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[3])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[4])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[5])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[6]))  
                     {
                      draw_string(x, yy, lang_str(curr_menu->menu[imenu].text),MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,COLOR_WHITE),1);
                      draw_filled_rect(x+j, yy,x+w, yy+FONT_HEIGHT,MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_BLUE:COLOR_BLUE_P));
                     }
                    else                                                                                                                 
                     {
                      draw_char(x, yy, ' ', cl,1);
                      draw_filled_rect(x+len_space, yy, xx-1, yy+FONT_HEIGHT/2-1, MAKE_COLOR(cl>>8, cl>>8));
                      draw_line(x+len_space, yy+FONT_HEIGHT/2, xx-1, yy+FONT_HEIGHT/2, cl);
                      draw_filled_rect(x+len_space, yy+FONT_HEIGHT/2+1, xx-1, yy+FONT_HEIGHT-1, MAKE_COLOR(cl>>8, cl>>8));
                      xx+=draw_char(xx, yy, ' ', cl,1);
                      xx+=draw_string(xx, yy, lang_str(curr_menu->menu[imenu].text), cl,1);
                      xx+=draw_char(xx, yy, ' ', cl,1);            
                      draw_filled_rect(xx, yy, x+w-len_space-1, yy+FONT_HEIGHT/2-1, MAKE_COLOR(cl>>8, cl>>8));
                      draw_line(xx, yy+FONT_HEIGHT/2, x+w-1-len_space, yy+FONT_HEIGHT/2, cl);
                      draw_filled_rect(xx, yy+FONT_HEIGHT/2+1, x+w-len_space-1, yy+FONT_HEIGHT-1, MAKE_COLOR(cl>>8, cl>>8));
                      draw_char(x+w-len_space, yy, ' ', cl,1);
                     }
                }
 
              else  if((lang_str(curr_menu->menu[imenu].text)==paramdesc[0])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[1])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[2])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[3])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[4])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[5])||(lang_str(curr_menu->menu[imenu].text)==paramdesc[6]))  
                {
                  draw_filled_rect(x, yy,x+w, yy+FONT_HEIGHT,MAKE_COLOR((rec)?COLOR_BLUE:COLOR_BLUE_P,(rec)?COLOR_BLUE:COLOR_BLUE_P));
                }

                else                                                                                                                    
                {
                    draw_char(x, yy, ' ', cl,1);
                    draw_filled_rect(x+len_space, yy, x+w-len_space-1, yy+FONT_HEIGHT/2-1, MAKE_COLOR(cl>>8, cl>>8));
                    draw_line(x+len_space, yy+FONT_HEIGHT/2, x+w-1-len_space, yy+FONT_HEIGHT/2, cl);
                    draw_filled_rect(x+len_space, yy+FONT_HEIGHT/2+1, x+w-len_space-1, yy+FONT_HEIGHT-1, MAKE_COLOR(cl>>8, cl>>8));
                    draw_char(x+w-len_space, yy, ' ', cl,1);
                }

                break;

            case MENUITEM_COLOR_FG:
            case MENUITEM_COLOR_BG:
                if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                
                 {
                  xx+=draw_char(xx, yy, ' ', cl,1);
                  xx+=draw_string_len(xx, yy, w-len_space, lang_str(curr_menu->menu[imenu].text), cl,1);
                  draw_filled_rect(x+w-1-cl_rect-2-len_space, yy+2, x+w-1-2-len_space, yy+FONT_HEIGHT-1-2, 
                                 MAKE_COLOR(((*(curr_menu->menu[imenu].value))>>(((curr_menu->menu[imenu].type & MENUITEM_MASK)==MENUITEM_COLOR_BG)?8:0))&0xFF, (cl>>8)&0xFF));
                 }

                  else                                           
                   {
                    xx+=draw_string_len(xx, yy, w, lang_str(curr_menu->menu[imenu].text), cl,1);
                    draw_filled_rect(x+w-1-cl_rect-2, yy+2, x+w-1-2, yy+FONT_HEIGHT-1-2, 
                                 MAKE_COLOR(((*(curr_menu->menu[imenu].value))>>(((curr_menu->menu[imenu].type & MENUITEM_MASK)==MENUITEM_COLOR_BG)?8:0))&0xFF, (cl>>8)&0xFF));
                    }
                break;
            case MENUITEM_ENUM:
                if (curr_menu->menu[imenu].value) {
                    ch=((const char* (*)(int change, int arg))(curr_menu->menu[imenu].value))(0, curr_menu->menu[imenu].arg);
                }
               if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                
               {
                xx+=draw_char(xx, yy, ' ', cl,1);
                xx+=draw_string_len(xx, yy, w-len_space-len_space-len_br1-len_enum-len_br2-len_space, lang_str(curr_menu->menu[imenu].text), cl,1);          
                xx+=draw_string(xx, yy, " [", cl,1);
                xx+=draw_string_right_len(xx, yy, len_enum, ch, cl,1);
                draw_string(xx, yy, "] ", cl,1);
                }
                else                                             
                {
                 xx+=draw_string_len(xx, yy, w-len_br1-str_width(ch)-len_br2, lang_str(curr_menu->menu[imenu].text),cl,1);
                 xx+=draw_string(xx, yy, "[", cl,1);
                 xx+=draw_string(xx, yy, ch, cl,1);
                 draw_string(xx, yy, "]", cl,1);
                }

                break;
            } 

                      if(!curr_menu->menu[imenu].char_size) font_height = FONT_HEIGHT;  
                       else font_height = BIG_FONT_HEIGHT;
                       yy+=font_height;
        }   
    } 
   menu_time=t;
}

