#include "stdlib.h"
#include "keyboard.h"
#include "platform.h"
#include "core.h"
#include "conf.h"
#include "gui.h"
#include "font.h"
#include "draw_palette.h"
#include "gui_read.h"
#include "kbd.h"
#include "mbox_popup.h"
#include "lang.h"
static int read_file;
static int read_file_size;
static int fontSize;
static unsigned int bufpos,sdm_colour;
static int read_on_screen;
static int read_to_draw;
static coord x, y, h, w;
#define READ_BUFFER_SIZE  300
static char buffer[READ_BUFFER_SIZE+1];
static int xx, yy;
static int pause;
int gLanguageCode;
static char fn[15];
extern int kbd_blocked;
extern volatile enum Gui_Mode gui_mode;
unsigned char exitWelcome;

void getLanguageCode()
{
 struct stat st;
 char *buf;
 int fd = -1,rcnt;
  if (stat("A/SDM/TEXTS/lang.txt",&st) == 0)
 {
  buf=umalloc(st.st_size+1);
  if(buf)
  {
   fd = open("A/SDM/TEXTS/lang.txt", O_RDONLY, 0777);
   if(fd>=0)
    {
     rcnt = read(fd, buf, st.st_size);                                  
     buf[rcnt] = 0;
     close(fd); 
     gLanguageCode=strtol(buf, NULL, 0);
    }
    ufree(buf);
   }
  }
 if((gLanguageCode==5) || (gLanguageCode==6)|| (gLanguageCode==9))conf.font_cp = FONT_CP_WIN_1250;
 else conf.font_cp = FONT_CP_WIN_1252;
 font_init();
 font_set(conf.font_cp);
}

void SetCameraPosition(unsigned int btn)
{
 if(!btn) return;
 conf.camera_position = (btn == MBOX_BTN_YES);
 
  if(!gLanguageCode)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/ENGLISH.LNG");
 }
 if(gLanguageCode==1)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/DEUTSCH.LNG");
 }
 else if(gLanguageCode==2)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/FRANCAIS.LNG");
 }
 if(gLanguageCode==3)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/HUN.LNG");
 }
 else if(gLanguageCode==4)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/ITALIAN.LNG");
 }
 else if(gLanguageCode==5)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/SLOVENE.LNG");
 } 
  else if(gLanguageCode==6)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/POLISH.LNG");
 }
  else if(gLanguageCode==7)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/INDONESIAN.LNG");
 }
  else if(gLanguageCode==8)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/SPANISH.LNG");
 }

  else if(gLanguageCode==9)
 {
  strcpy(conf.lang_file, "A/SDM/LANG/ROMANIAN.LNG");
 }
 conf_save_new_settings_if_changed();
 exitWelcome=1;
 kbd_blocked=0;
}
 
int gui_read_init(const char* file) 
{
    static struct stat   st;
    read_file = open(file, O_RDONLY, 0777);
    if (strcmp(file, conf.reader_file)!=0) 
    {
     strcpy(conf.reader_file, file);
    }
    conf.reader_pos = 0;
    read_on_screen = 0;
    fontSize = 1;
    read_file_size = (read_file>=0 && stat((char*)file, &st)==0)?st.st_size:0; 
    if (read_file_size<=conf.reader_pos) 
    {
     conf.reader_pos = 0;
    }
    pause = 0;
    read_to_draw = 1;
    if(conf.splash_show)          
    {    
     if(vid_get_bitmap_screen_width()>360)
      x = (vid_get_bitmap_screen_width()-360)/2; 
     sdm_colour=MAKE_COLOR(COLOR_LIGHT_GREY_P, COLOR_WHITE);
    }
    else 
    {
     sdm_colour=conf.reader_color;
    }                    
 
 
#if !defined(CAMERA_tx1)
   if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                 
    {
     w=screen_width;
     h = (conf.splash_show)?screen_height:screen_height-FONT_HEIGHT;
    }
   else                                           
    {
     w=screen_height;
     h = screen_width;
    }
  
    if(conf.splash_show)   
   {
  if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                 
    {
     draw_filled_rect(0, 0, screen_width, screen_height, MAKE_COLOR(COLOR_LIGHT_GREY_P, COLOR_LIGHT_GREY_P));
     }
    else              
     {
      draw_filled_rect(0,0, screen_height, screen_width, MAKE_COLOR(COLOR_LIGHT_GREY_P,COLOR_LIGHT_GREY_P));
     }
   }
 
     else       

     {
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                 
    {
     draw_filled_rect(0, 0, screen_width, screen_height-FONT_HEIGHT, MAKE_COLOR((conf.reader_color>>8)&0xFF, (conf.reader_color>>8)&0xFF));
     }
    else
     {
      draw_filled_rect(0, 0, screen_height, screen_width, MAKE_COLOR((conf.reader_color>>8)&0xFF, (conf.reader_color>>8)&0xFF));
     }
   }
 
#else
    if(conf.splash_show)  
     {
      w=screen_width-120;
      h=screen_height;
      draw_filled_rect(0, 0, screen_width-120, screen_height, MAKE_COLOR(COLOR_GREY_P, COLOR_GREY_P));     
     }
    else
    {     
     draw_filled_rect(0, 0, screen_width-120, screen_height, MAKE_COLOR((conf.reader_color>>8)&0xFF, (conf.reader_color>>8)&0xFF));
    }
#endif
 
    return (read_file >= 0);
}

static char *p=NULL;
static unsigned char inBlock;
static int lineCount=0;
char mybuf[50];
static void read_goto_next_line()
{
 unsigned char offset = 0;
 if(lineCount)
  {
   yy += FONT_HEIGHT*fontSize;  
   fontSize=1;
   offset=1;
  }
if((yy<=y+h-FONT_HEIGHT))
{ 
 if(conf.splash_show)                  
  { 
   if(buffer[bufpos+offset]=='~')
    {
     fontSize=2;
     ++bufpos;
    }
    if(buffer[bufpos+offset]=='#')
    {
     sdm_colour=MAKE_COLOR(COLOR_GREY_P,COLOR_WHITE);
     bufpos++;
    }
     else if(buffer[bufpos+offset]=='$')
    {
      sdm_colour=MAKE_COLOR(COLOR_DARK_GREY_P,COLOR_WHITE);
     bufpos++;
    }   
     else if(buffer[bufpos+offset]=='!')
    {
      sdm_colour=MAKE_COLOR(COLOR_GREY_P,COLOR_WHITE);
     bufpos++;
    }   
      else if(buffer[bufpos+offset]=='@')
    {
     sdm_colour=MAKE_COLOR(COLOR_TRANSPARENT,COLOR_WHITE);
     bufpos++;
    }
   else  sdm_colour=MAKE_COLOR(COLOR_LIGHT_GREY_P, COLOR_WHITE);   
  }  
else        
  {
   if((buffer[bufpos]=='#')&&!inBlock){sdm_colour=MAKE_COLOR(COLOR_BLACK,COLOR_WHITE);bufpos++;inBlock=1;}
   else if((buffer[bufpos]=='#')&&inBlock){sdm_colour=conf.reader_color;bufpos++;inBlock=0;}
   else if(inBlock)sdm_colour=MAKE_COLOR(COLOR_BLACK,COLOR_WHITE);
   else
   {
    p = (char*)&buffer[bufpos];
    if(strncmp("rem ",p,4)==0){sdm_colour=MAKE_COLOR(COLOR_BLACK,COLOR_WHITE);bufpos+=4;}
    else if((buffer[bufpos]=='@')||(buffer[bufpos]==':')){sdm_colour=MAKE_COLOR(COLOR_WHITE,(gPlayRecMode>0)?COLOR_DARK_GREY:COLOR_DARK_GREY_P);}
    else if((strncmp("for ",p,4)==0)||(strncmp("next ",p,5)==0)){sdm_colour=MAKE_COLOR((gPlayRecMode>0)?COLOR_RED:COLOR_RED_P,COLOR_WHITE);}  
    else if((strncmp("do",p,2)==0)||(strncmp("until ",p,6)==0)){sdm_colour=MAKE_COLOR((gPlayRecMode>0)?COLOR_GREEN:COLOR_GREEN_P,COLOR_WHITE);}
    else if((strncmp("while ",p,6)==0)||(strncmp("wend",p,4)==0)){sdm_colour=MAKE_COLOR((gPlayRecMode>0)?COLOR_BLUE:COLOR_BLUE_P,COLOR_WHITE);} 
    else if((strncmp("if ",p,3)==0)||(strncmp("else",p,4)==0)||(strncmp("endif",p,5)==0)){sdm_colour=MAKE_COLOR(COLOR_WHITE,(gPlayRecMode>0)?COLOR_BLUE:COLOR_BLUE_P);} 
    else sdm_colour=conf.reader_color;    
   }    
  }
  xx  = x;  
 if(!conf.splash_show)draw_filled_rect(xx, yy, x+w-1, yy+FONT_HEIGHT-1, MAKE_COLOR(sdm_colour>>8, sdm_colour>>8)); 
 else draw_filled_rect(xx, yy, w, yy+FONT_HEIGHT*fontSize, MAKE_COLOR(sdm_colour>>8, sdm_colour>>8));
 }
}
static int read_fit_next_char(int ch) {
    return (xx+FONT_WIDTH < x+w);
}

void gui_read_draw() 
{
 if (read_to_draw) 
  {
   bufpos=0;
   lineCount=0;
   int n=0; 
   int i;
   int ll; 
   int new_word=1; 
   int ii;  
   xx=x; yy=y;                                      
   lseek(read_file, conf.reader_pos, SEEK_SET);     
   read_on_screen=0;                                
   while (yy<=y+h-FONT_HEIGHT)                      
    {
    n=read(read_file, buffer, READ_BUFFER_SIZE); 
    if (n==0)                                    
     {
      read_goto_next_line();                  
      if (yy < y+h)                           
       {                                      
        if(!conf.splash_show)draw_filled_rect(x, yy, x+w-1, y+h-1, MAKE_COLOR(sdm_colour>>8, sdm_colour>>8)); 
        else draw_filled_rect(x, yy,w, y+h, MAKE_COLOR(COLOR_LIGHT_GREY_P,COLOR_LIGHT_GREY_P));              
       }
     }
     bufpos=0;   
   if((!lineCount)&&(xx==x))read_goto_next_line();            
           while (bufpos<n && yy<=y+h-(FONT_HEIGHT*fontSize)) 
            {
                switch (buffer[bufpos])                    
                {
                    case '\r':                             
                        new_word = 1;
                        break;
                    case '\n':                             
                        ++lineCount;
                        read_goto_next_line();            
                        new_word = 1;
                        break;
                    case '\t':
                        buffer[bufpos] = ' ';            
                    default:                             
                            if (buffer[bufpos] == ' ')   
                            {
                                new_word = 1;                                 
                            } 
                            else if (new_word)           
                            {
                                new_word = 0;
                                for (ii=bufpos, ll=0; ii<n && buffer[ii]!=' ' && buffer[ii]!='\t' && buffer[ii]!='\r' && buffer[ii]!='\n'; ++ii) 
                                {
                                   ll+=(FONT_WIDTH*fontSize);                
                                }
                                if (ii==n)                              
                                {
                                    memcpy(buffer, buffer+bufpos, n-bufpos);
                                    n=ii=n-bufpos;
                                    read_on_screen+=bufpos;
                                    bufpos=0;
                                    n+=read(read_file, buffer+n, READ_BUFFER_SIZE-n);   
                                    for (; ii<n && buffer[ii]!=' ' && buffer[ii]!='\t' && buffer[ii]!='\r' && buffer[ii]!='\n'; ++ii) 
                                    {
                                      ll+=(FONT_WIDTH*fontSize);  
                                    }
                                }
                                if (xx+ll>=x+w && ll<w)       
                                {
                                    read_goto_next_line();    
                                    continue;
                                }
                            }
 
                        if (!read_fit_next_char(buffer[bufpos])) 
                        {
                            read_goto_next_line();          
                            continue;
                        }
                         draw_char(xx, yy, buffer[bufpos], sdm_colour,fontSize); 
                         xx+=FONT_WIDTH*fontSize;                          
                        break;                                      
                }   
                
                if (xx >= x+w)  
                {
                    xx  = x;    
                    yy += FONT_HEIGHT*fontSize; 
                }  
               bufpos++;            
            }    
           read_on_screen+=bufpos;  
           } 
        read_to_draw = 0;
    } 
}
 
void gui_read_kbd_process() 
{ 
 unsigned char ac_key;
 ac_key = kbd_get_autoclicked_key();
  
  if((ac_key==conf.alt_mode_button) && !conf.splash_show)          
 {
  if (read_file >= 0) 
  {
   close(read_file);
   read_file=-1;
  }
   draw_restore();
   gui_mode = GUI_MODE_MENU;
 }
  
 else if(ac_key)
 {
   switch(get_vkey(ac_key))
    {
        case KEY_ZOOM_OUT:
        case KEY_UP:
        case KEY_LEFT:
            conf.reader_pos=0;
            read_to_draw = 1;
            break;
        case KEY_ZOOM_IN:
        case KEY_DOWN:
        case KEY_RIGHT:
        case KEY_SHOOT_HALF:
            if ((conf.reader_pos+read_on_screen)<read_file_size) {
                conf.reader_pos += read_on_screen;
                read_to_draw = 1;
            }
            break;
 
        case KEY_MENU:
            if (read_file >= 0) {
                close(read_file);
                read_file=-1;
            }
 
            if(conf.splash_show)                  
             {
              conf.reader_pos=0;
              conf.splash_show=0;
              conf.extend_lens=1;
              kbd_key_release_all();
              gui_mode = GUI_MODE_NONE;             
              gui_mbox_init((int)"Camera position",(int)"Right camera ?",MBOX_DEF_BTN2|MBOX_TEXT_CENTER|MBOX_BTN_YES_NO,SetCameraPosition);
             }
            break;
        }
        if((ac_key==conf.alt_mode_button)&&(read_file >= 0))  
        {
         close(read_file);
         read_file=-1;
        }
    }
   
}

