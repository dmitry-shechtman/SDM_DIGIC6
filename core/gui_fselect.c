#include "stdlib.h"
#include "keyboard.h"
#include "platform.h"
#include "core.h"
#include "lang.h"
#include "gui.h"
#include "draw_palette.h"
#include "gui_lang.h"
#include "mbox_popup.h"
#include "gui_fselect.h"
#include "conf.h"
#include"kbd.h"
#include "batt_grid.h"
#define NUMBER_LINES               7
#define SIZE_SIZE               7
#define TIME_SIZE               14
#define SPACING                 1

#define MARKED_OP_NONE          0
#define MARKED_OP_CUT           1
#define MARKED_OP_COPY          2
#define MARKED_BUF_SIZE         0x10000

static char gridName[32]={'\0'};
 char gridShortName[45];
extern unsigned short gMenuPressed;
extern volatile enum Gui_Mode gui_mode;
unsigned char gRunNow,UserScript=1,UserInit,gridFlag;
static unsigned char nameSize;		
static char current_dir[100];
char selected_file[100];
static char buf[100];
static char marked_dir[100];
static enum Gui_Mode    gui_fselect_mode_old;
char EasyMode[13];
struct fitem {
    unsigned int    n;
    char            *name;
    unsigned char   attr;
    unsigned long   size;
    unsigned long   mtime;
    unsigned char   marked;
    struct fitem    *prev, *next;
};
static struct fitem *head=NULL, *top, *selected, *last;
static unsigned int count;
static coord x, y, w, h;
static int gui_fselect_redraw,num_lines;
static char *fselect_title;
static void (*fselect_on_select)(const char *fn);
static void fselect_goto_next(int step);
static void gui_fselect_free_data() {
    struct fitem  *ptr = head, *prev;

    while (ptr) {
        if (ptr->name)
            free(ptr->name);
        prev=ptr;
        ptr=ptr->next;
        free(prev);
    }
    head=top=selected=last=NULL;
    count=0;
}

extern int fselect_sort_nothumb(const void* v1, const void* v2);
int fselect_sort(const void* v1, const void* v2) {
    struct fitem *i1=*((struct fitem **)v1), *i2=*((struct fitem **)v2);

    if (i1->attr & DOS_ATTR_DIRECTORY) {
        if (i2->attr & DOS_ATTR_DIRECTORY) {
            if (i1->name[0]=='.' && i1->name[1]=='.' && i1->name[2]==0) {
                return -1;
            } else if (i2->name[0]=='.' && i2->name[1]=='.' && i2->name[2]==0) {
                return 1;
            } else {
                return strcmp(i1->name, i2->name);
            }
        } else {
            return -1;
        }
    } else {
        if (i2->attr & DOS_ATTR_DIRECTORY) {
            return 1;
        } else {
            return strcmp(i1->name, i2->name);
        }
    }
}

static int ez_count;
static void gui_fselect_read_dir(const char* dir) 
{
    DIR           *d;
    struct dirent *de;
    static struct stat   st;
    struct fitem  **ptr = &head, *prev = NULL;
    int    i;
    count=0; 
    gui_fselect_free_data();

#ifdef CAM_DRYOS_2_3_R39
	if(dir[0]=='A' && dir[1]==0)
		d = opendir("A/");
	else
    d = opendir(dir);
#else
    d = opendir(dir);
#endif
    if (d) 
     {
        de = readdir(d);
        while (de) 
         {
            if (de->name[0] != 0xE5  && (de->name[0]!='.' || de->name[1]!=0)) 
               {
                *ptr = malloc(sizeof(struct fitem));
                if (*ptr) 
                 {
                    (*ptr)->n = count;
                    (*ptr)->name = malloc(strlen(de->name)+1);
                    if ((*ptr)->name)
                        strcpy((*ptr)->name, de->name);
                    sprintf(buf, "%s/%s", dir, de->name);
                    if (stat(buf, &st)==0) 
                       {
                        (*ptr)->attr=st.st_attrib;
                        (*ptr)->size=st.st_size;
                        (*ptr)->mtime=st.st_mtime;
                       } 
                     else 
                       {
                        (*ptr)->attr=(de->name[0]=='.' && de->name[1]=='.' && de->name[2]==0)?DOS_ATTR_DIRECTORY:0xFF;
                        (*ptr)->size=(*ptr)->mtime=0;
                       }
                    (*ptr)->marked=0;
                    (*ptr)->prev=prev;
                    prev=*ptr;
                    ptr = &((*ptr)->next);
                    ++count;
                 }   
              }
            de = readdir(d);
        }
        closedir(d);
     }

    *ptr=NULL;
    ez_count=count;
    if (count) 
     {
        
        ptr=malloc(count*sizeof(struct fitem*));
        if (ptr) 
           {
            prev=head;
            count=0;
            while (prev) 
              {
                ptr[count++]=prev;
                prev=prev->next;
              }
            
            qsort(ptr, count, sizeof(struct fitem*), fselect_sort_nothumb);
            
            for (i=0; i<count-1; ++i) 
              {
                ptr[i]->n=i;
                ptr[i]->next=ptr[i+1];
                ptr[i+1]->prev=ptr[i];
              }
            ptr[0]->prev=ptr[count-1]->next=NULL;
            ptr[count-1]->n=count-1;
            head=ptr[0];
            free(ptr);
           }
     }

    top = selected = head;
}

int read_easyModes_dir()
{
 gui_fselect_read_dir("A/SDM/SCRIPTS");
 return ez_count;
}

void gui_easy_fselect_init(int title, const char* dir, void (*on_select)(const char *fn)) 
{
 int i;
 int width  = vid_get_bitmap_screen_width();
  int height = vid_get_bitmap_screen_height();
 
  if((conf.camera_orientation==1)||(conf.camera_orientation==3))
   {
    x = (height-(8*3*FONT_WIDTH))>>1;
    y = (width-(3*FONT_HEIGHT))>>1;
   }
  else
   {
    x = (width-(8*3*FONT_WIDTH))>>1;
    y = (height-(3*FONT_HEIGHT))>>1;
   }
    fselect_title = lang_str(title);
    strcpy(current_dir, dir);
    gui_fselect_read_dir(current_dir); 
    top = selected = head;
    last = NULL;
    selected = selected->next;
    selected_file[0]=0;
 
    if(conf.easy_pos)
    {
     for(i=0;i<=conf.easy_pos;i++)
     {
      fselect_goto_next(1);
     }
    }
     gui_fselect_redraw = 1;
 
    fselect_on_select = on_select;
 
    gui_fselect_mode_old = gui_get_mode();
    draw_clear();
    gui_fselect_redraw = 2;			
    gui_set_mode(GUI_MODE_FSELECT);
}

void gui_fselect_init(int title, const char* dir, void (*on_select)(const char *fn)) {
   if(strcmp(dir,"A/SDM/SCRIPTS3")==0)UserScript=1;
   else UserScript=0;
   if((strcmp(dir,"A/SDM/SCRIPTS")==0)&& (strlen(dir)==13))
    gui_easy_fselect_init(title,dir,(*on_select));
   else
    {
    int i; 
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))num_lines=NUMBER_LINES;
    else num_lines=NUMBER_LINES+10;
    if(screen_width==320)nameSize = 12;
    else nameSize = 15;
    if((conf.camera_orientation==1)||(conf.camera_orientation==3))nameSize = 12;
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
     w = (1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH; 
    else
     w = (1+nameSize+SPACING+SIZE_SIZE+SPACING+7)*FONT_WIDTH; 

    h = FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+FONT_HEIGHT+2;

#if defined(CAMERA_tx1)
    x = (screen_width-120-w)>>1;
#else
    x = (screen_width-w)>>1;
#endif
   if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
    y = (screen_height-h)>>1;
   else
    y = (screen_width-h)>>1;
   if((conf.camera_orientation==1)||(conf.camera_orientation==3))	
    {
     x = (screen_height-w)>>1;    					
    }

    fselect_title = lang_str(title);
    strcpy(current_dir, dir);
    gui_fselect_read_dir(current_dir);  
    top = selected = head; 
    if (strcmp(current_dir,"A/SDM/GRIDS")==0)
    {
     selected=top->next;
     conf.show_grid_lines=1;
    }     
    selected_file[0]=0; 
    last = NULL; 
  
 if(UserScript)                   
 {
  if(conf.user_pos)
   {
    UserInit=1;
    fselect_goto_next(conf.user_pos);
    UserInit=0;
    gui_fselect_redraw = 1;
   }
 }  

    fselect_on_select = on_select;
    gui_fselect_mode_old = gui_get_mode();
    gui_fselect_redraw = 2;			
    gui_set_mode(GUI_MODE_FSELECT);
  }
}

char* gui_fselect_result() {
    if (selected_file[0])
        return selected_file;
    else
        return NULL;
}

void gui_fselect_draw_initilal() 
{
    int i;
    int rec=((mode_get()&MODE_MASK) == MODE_REC);
  if (strcmp(current_dir,"A/SDM/GRIDS")!=0)
  {
    
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
     {
      draw_rect(x-3, y-3, x+w+5, y+h+5, COLOR_BLACK); //shadow
      draw_rect(x-2, y-2, x+w+6, y+h+6, COLOR_BLACK); //shadow
      draw_rect(x-1, y-1, x+w+7, y+h+7, COLOR_BLACK); //shadow
      draw_filled_rect(x-4, y-4, x+w+4, y+h+4, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_WHITE:COLOR_WHITE_P)); 			//main box
      draw_filled_rect(x-2, y-2, x+w+2, y+FONT_HEIGHT+2, MAKE_COLOR(COLOR_BLACK, (rec)?COLOR_WHITE:COLOR_WHITE_P)); 				//title
      draw_filled_rect(x-2, y+h-FONT_HEIGHT-2, x+w+2, y+h+2, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P)); //footer
      draw_rect(x-2, y-2, x+w+2, y+h+2,(rec)? COLOR_WHITE:COLOR_WHITE_P); //border
      draw_rect(x-3, y-3, x+w+3, y+h+3,(rec)? COLOR_WHITE:COLOR_WHITE_P); //border
     }
    else
     {
      draw_filled_rect(x-3, y-4, x+w+4, y+h+4, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_WHITE:COLOR_WHITE_P)); 			//main box
      draw_filled_rect(x-2, y-2, x+w+2, y+FONT_HEIGHT+2, MAKE_COLOR(COLOR_BLACK, (rec)?COLOR_WHITE:COLOR_WHITE_P)); 				//title
      draw_filled_rect(x-2, y+h-FONT_HEIGHT-2, x+w+2, y+h+2, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P)); //footer
      draw_rect(x-2, y-2, x+w+2, y+h+2,(rec)? COLOR_WHITE:COLOR_WHITE_P); //border
      draw_rect(x-3, y-3, x+w+3, y+h+3,(rec)? COLOR_WHITE:COLOR_WHITE_P); //border
     }
    i = strlen(fselect_title);
    draw_string(x+((w-i*FONT_WIDTH)>>1), y, fselect_title, MAKE_COLOR(COLOR_BLACK, (rec)?COLOR_WHITE:COLOR_WHITE_P),1); //title text
   }
}

void gui_easy_fselect_draw() 
{
 char *p;
 char mybuf[15],name[10];
 if(gui_fselect_redraw==2)
  {
   draw_clear(); 
   if(gPlayRecMode>0)
    {
     draw_string(x-52,y,"SL",MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
     draw_string(x+(3*FONT_WIDTH*8+4),y,"RST",MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
    }
    else
    {
     draw_string(x-52,y,"SL",MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);
     draw_string(x+(3*FONT_WIDTH*8+4),y,"RST",MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);
    }    
   strcpy(mybuf,selected->name);
   p=strchr(mybuf,'.');
   *p='\0';
   sprintf(name,"%-8s",mybuf);
   if(gPlayRecMode>0)
    draw_string(x,y,name,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),3);  
   else
    draw_string(x,y,name,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE_P,COLOR_WHITE):MAKE_COLOR(COLOR_RED_P,COLOR_WHITE),3);   
 if(selected->prev)
   {
    strcpy(mybuf,selected->prev->name);
    p=strchr(mybuf,'.');
    *p='\0';
    sprintf(name,"%-8s",mybuf);
    if(gPlayRecMode>0)
     draw_string(x,y-52,name,MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
    else
     draw_string(x,y-52,name,MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);   
   }
   else
   {
    if(gPlayRecMode>0)
     draw_string(x,y-52,"        ",MAKE_COLOR(COLOR_BG,COLOR_BG),3);
    else
     draw_string(x,y-52,"        ",MAKE_COLOR(COLOR_BG_P,COLOR_BG_P),3);    
   }
   if(selected->next)
   {
    strcpy(mybuf,selected->next->name);
    p=strchr(mybuf,'.');
    *p='\0';
    sprintf(name,"%-8s",mybuf);
    if(gPlayRecMode>0)
     draw_string(x,y+52,name,MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
    else
     draw_string(x,y+52,name,MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);    
   }
   else
   {
    if(gPlayRecMode>0)
     draw_string(x,y+52,"        ",MAKE_COLOR(COLOR_BG,COLOR_BG),3); 
    else
     draw_string(x,y+52,"        ",MAKE_COLOR(COLOR_BG_P,COLOR_BG_P),3);         
   } 
    gui_fselect_redraw = 0;                               
  } 
}
void gui_fselect_draw() {
  if((strcmp(current_dir,"A/SDM/SCRIPTS")==0)&& (strlen(current_dir)==13))gui_easy_fselect_draw();
  else if (strcmp(current_dir,"A/SDM/GRIDS")!=0) 
   {
    int i, j;
    struct fitem  *ptr;
    char buf[48];
    struct tm *time;
    int rec = ((mode_get()&MODE_MASK) == MODE_REC);
    color cl_markered = (rec)?COLOR_YELLOW:COLOR_YELLOW_P;
    color cl,cl2;

    if (gui_fselect_redraw) 
      {
        if (gui_fselect_redraw==2)
            gui_fselect_draw_initilal();

for (i=0, ptr=top; i< num_lines && ptr; ++i, ptr=ptr->next) 
          {

            for (j=0; j<nameSize && ptr->name[j]; ++j) 
                buf[j]=ptr->name[j];
            if (j==nameSize && ptr->name[j]) buf[nameSize-1]='>'; //too long name
            if (ptr->attr & DOS_ATTR_DIRECTORY && ptr->attr != 0xFF) 
              {
                if (j<nameSize) 
                {
                    buf[j++]='/';
                } 
               else 
                {
                    buf[nameSize-2]='>';
                    buf[nameSize-1]='/';
                }
              }
            for (; j<nameSize && (buf[j++]=' '););//fill upto NAME_SIZE
            buf[nameSize]=0;
            
           if(ptr==selected)
            {
            if(rec)
             {
              cl=MAKE_COLOR(COLOR_RED,(ptr->marked)?cl_markered:COLOR_WHITE);
              cl2=MAKE_COLOR(COLOR_RED,COLOR_WHITE);
             }
            else
             {
              cl=MAKE_COLOR(COLOR_RED_P,(ptr->marked)?cl_markered:COLOR_WHITE_P);
              cl2=MAKE_COLOR(COLOR_RED_P,COLOR_WHITE_P);
             }
            }
           else
           {
            if(rec)
            {
             cl=MAKE_COLOR(COLOR_GREY,(ptr->marked)?cl_markered:COLOR_WHITE);
             cl2=MAKE_COLOR(COLOR_GREY,COLOR_WHITE);
            }
            else
            {
             cl=MAKE_COLOR(COLOR_GREY_P,(ptr->marked)?cl_markered:COLOR_WHITE_P);
             cl2=MAKE_COLOR(COLOR_GREY_P,COLOR_WHITE_P);
            }
           }
            if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3)) 									    
             {
              draw_string(x+FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1);                   
              draw_string(x+(1+nameSize)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, "\x7C",cl2,1); 
             }
            else  													    
             {
              draw_string(x, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1);
              draw_string(x+(nameSize*FONT_WIDTH), y+FONT_HEIGHT+4+i*FONT_HEIGHT, "\x7C",cl2,1);   
             }
 
            if (ptr->attr & DOS_ATTR_DIRECTORY) 
              {
                if (ptr->attr == 0xFF) 
                 {
                  sprintf(buf, "  ???  ");
                 } 
                else if (ptr->name[0]=='.' && ptr->name[1]=='.' && ptr->name[2]==0) 
                 {
                  sprintf(buf, "<UpDir>");
                 } 
                else 
                 {
                  sprintf(buf, "< Dir >");
                 }
               } 
              else 
               {
                if (ptr->size < 10000000)			
                    sprintf(buf, "%7lu", ptr->size);
                else if (ptr->size < 1024000000)
                    sprintf(buf, "%6luK", ptr->size>>10);
                else
                    sprintf(buf, "%6luM", ptr->size>>20);
               }
            if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
             draw_string(x+(nameSize+SPACING+1)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1);
            else
             draw_string(x+(nameSize+SPACING)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1); 
            if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                            					
             draw_string(x+(1+nameSize+SPACING+SIZE_SIZE)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, "\x7C",cl2,1); 
            else draw_string(x+(1+nameSize+SIZE_SIZE)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, "\x7C",cl2,1);
 
            if (ptr->mtime) 
              {
                time = localtime(&(ptr->mtime));
               if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))							
                 sprintf(buf, "%2u/%02u/%02u %02u:%02u", time->tm_mday, time->tm_mon+1, (time->tm_year<100)?time->tm_year:time->tm_year-100, time->tm_hour, time->tm_min);
 		   else sprintf(buf, "%2u/%02u/%02u", time->tm_mday, time->tm_mon+1, (time->tm_year<100)?time->tm_year:time->tm_year-100); 
             } 
             else 
              {
               if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))sprintf(buf, "%14s", "");			
               else sprintf(buf, "%8s", "");								
              }
            if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
             draw_string(x+(nameSize+SPACING+SIZE_SIZE+SPACING+1)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1);
            else
             draw_string(x+(nameSize+SPACING+SIZE_SIZE+SPACING)*FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, buf,cl,1);
        } 

        if (i<num_lines) 
        {
         if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
         draw_filled_rect(x+FONT_WIDTH, y+FONT_HEIGHT+4+i*FONT_HEIGHT, x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE)*FONT_WIDTH, 
                             y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT-1,MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_GREY:COLOR_GREY_P));
         else draw_filled_rect(x, y+FONT_HEIGHT+4+i*FONT_HEIGHT, x+2+(1+nameSize+SIZE_SIZE+TIME_SIZE-5)*FONT_WIDTH, 
                             y+FONT_HEIGHT+6+num_lines*FONT_HEIGHT-1, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_GREY:COLOR_GREY_P));
        }

        i=strlen(current_dir);

         if (i>nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE) 
          {
            i-=nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE -1;
            draw_char(x+FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4, '<', MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_WHITE:COLOR_WHITE_P),1);
            draw_string(x+FONT_WIDTH*2, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4, current_dir+i, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P,(rec)? COLOR_WHITE:COLOR_WHITE_P),1); 
          } 

       else 
        {
         draw_string(x+FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+4, current_dir, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_WHITE:COLOR_WHITE_P),1); 
         if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))                     	
           draw_filled_rect(x+(1+i)*FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+4, 
                             x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE)*FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+4+FONT_HEIGHT, MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_GREY:COLOR_GREY_P)); 
         else 
           draw_filled_rect(x+(1+i)*FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+4, 
                             x+(1+nameSize+SIZE_SIZE+3)*FONT_WIDTH, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT+4+2+FONT_HEIGHT,MAKE_COLOR((rec)?COLOR_GREY:COLOR_GREY_P, (rec)?COLOR_GREY:COLOR_GREY_P));
    }

        if ((count>num_lines))              
        {
            i=num_lines*FONT_HEIGHT-1 -1;
            j=i*num_lines/count;
            if (j<20) j=20;
            i=(i-j)*selected->n/(count-1);
           if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
           {
            draw_filled_rect(x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+2, y+FONT_HEIGHT+4+1, 
                             x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+6, y+FONT_HEIGHT+4+1+i, MAKE_COLOR(COLOR_BLACK, COLOR_BLACK));
            draw_filled_rect(x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+2, y+FONT_HEIGHT+4+i+j, 
                             x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+6, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT-1-1, MAKE_COLOR(COLOR_BLACK, COLOR_BLACK));
            draw_filled_rect(x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+2, y+FONT_HEIGHT+4+1+i, 
                             x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+6, y+FONT_HEIGHT+4+i+j, MAKE_COLOR((rec)?COLOR_WHITE:COLOR_WHITE_P, (rec)?COLOR_WHITE:COLOR_WHITE_P));
           }
        } 
        else if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
        {
            draw_filled_rect(x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+2, y+FONT_HEIGHT+4+1, 
                             x+(1+nameSize+SPACING+SIZE_SIZE+SPACING+TIME_SIZE+1)*FONT_WIDTH+6, y+FONT_HEIGHT+4+num_lines*FONT_HEIGHT-1-1, MAKE_COLOR(COLOR_BLACK, COLOR_BLACK));
        }

        gui_fselect_redraw = 0;
    } 
  }
  else  
  {
   if(gui_fselect_redraw ==2)
   {
    conf.show_grid_lines=1;
    sprintf(gridName,"%s/%s",current_dir,selected->name);
    grid_lines_load(gridName);
    gui_grid_draw_osd(1);
    vid_bitmap_refresh();
   }
  }
}

static void fselect_delete_file_cb(unsigned int btn) {
    if (btn==MBOX_BTN_YES) {
        started();
        sprintf(selected_file, "%s/%s", current_dir, selected->name);
        remove(selected_file);
        finished();
        selected_file[0]=0;
        gui_fselect_read_dir(current_dir);
    }
    gui_fselect_redraw = 2;
}

static void fselect_delete_folder_cb(unsigned int btn) 
{
    DIR           *d;
    struct dirent *de;
    int           i;

    if (btn==MBOX_BTN_YES) 
     {
        sprintf(current_dir+strlen(current_dir), "/%s", selected->name);
        d = opendir(current_dir);
        if (d) 
          {
            de = readdir(d);
            while (de) 
              {
                if (de->name[0] != 0xE5 && (de->name[0]!='.' || (de->name[1]!='.' && de->name[1]!=0) || (de->name[1]=='.' && de->name[2]!=0))) 
                  {
                    started();
                    sprintf(selected_file, "%s/%s", current_dir, de->name);
                    remove(selected_file);
                    finished();
                  }
                de = readdir(d);
               }
              closedir(d);
           }
        started();
        remove(current_dir);
        finished();
        i=strlen(current_dir);
        while (current_dir[--i] != '/');
        current_dir[i]=0;
        selected_file[0]=0;
        gui_fselect_read_dir(current_dir);
    }
    gui_fselect_redraw = 2;
}

static void fselect_goto_prev(int step) {
    register int j;

    for (j=0; j<step; ++j) {
        if (selected->prev==top && top->prev) 
            top=top->prev;
        if (selected->prev) 
        {
            selected=selected->prev;
            if(UserScript && !UserInit)--conf.user_pos; 
            }
    }
       
}

static void fselect_goto_next(int step) {
    register int j, i;
    struct fitem  *ptr;

    for (j=0; j<step; ++j) {
        for (i=0, ptr=top; i<num_lines-1 && ptr; ++i, ptr=ptr->next);
        if (i==num_lines-1 && ptr && ptr->prev==selected && ptr->next)
           top=top->next;
        if (selected->next) 
        {
            selected=selected->next;
            if(UserScript && !UserInit)++conf.user_pos;
            }
            else play_sound(4);
    }
    
}
 
void gui_easy_fselect_kbd_process()
{
 char *p;
 char mybuf[15],name[10],tmp[30];
 static unsigned int lastIndex;
 unsigned char ac_key;
 ac_key = kbd_get_autoclicked_key();
  switch (get_vkey(ac_key)) 
  {
   case KEY_LEFT:
        gui_fselect_free_data();
        gui_set_mode(GUI_MODE_ALT);
        draw_restore();
        strcpy(EasyMode,"A/SDM/SCRIPTS/SLEEP.txt");
        fselect_on_select("A/SDM/SCRIPTS/SLEEP.txt"); 
   break;
   
   case KEY_RIGHT:
        gui_fselect_free_data(); 
        gui_set_mode(GUI_MODE_ALT);
        draw_restore(); 
        strcpy(EasyMode,"A/SDM/SCRIPTS/RESET.txt");
        fselect_on_select("A/SDM/SCRIPTS/RESET.txt");        
   break;

   case KEY_MENU:
        gui_fselect_free_data();
        gui_set_mode(gui_fselect_mode_old);
        draw_restore();
        if (fselect_on_select) 
         fselect_on_select(NULL);
         break;      
   
   }
   
   int vmode = MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK);
   switch (get_vkey(ac_key))
   {   
   case KEY_UP:
#if defined(CAMERA_g7x)
     if (selected && last!=NULL) 
#else
     if (selected)     
#endif 
      {
         if (selected==top->next) 
         {       
          if(last) {selected=last;conf.easy_pos=lastIndex;}  
         }
        else 
         {
          selected=selected->prev;
          --conf.easy_pos;
         }
         gui_fselect_redraw = 1;
      }
      break;
 
   case KEY_DOWN:
     if (selected) 
      {
         if (selected->next) 
         {
          selected=selected->next;
          ++conf.easy_pos;
         }   
        else                         
         {
          last = selected;  
          selected=top->next; 
          lastIndex=conf.easy_pos;
          conf.easy_pos=0;
         }
         gui_fselect_redraw = 1;
      }
      break;
   case KEY_SET:
      if (selected && (selected->attr != 0xFF) && !(selected->attr & DOS_ATTR_DIRECTORY))
      {
       if(!vmode || (strcmp(selected->name,"AIRCRAFT.TXT")==0)) 
        {      
         {
          sprintf(selected_file, "%s/%s", current_dir, selected->name);
          strcpy(EasyMode,selected->name);
          gui_fselect_free_data();     
          gui_set_mode(GUI_MODE_ALT);
          draw_restore();
          if (fselect_on_select) 
           fselect_on_select(selected_file); 
         }
        }
        else play_sound(6); 
      }        
         break;  
     }      
  
    if (gui_fselect_redraw == 1) 
      {
         strcpy(mybuf,selected->name);
   p=strchr(mybuf,'.');
   *p='\0';
   sprintf(name,"%-8s",mybuf);
   if(gPlayRecMode>0)
    draw_string(x,y,name,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE,COLOR_WHITE):MAKE_COLOR(COLOR_RED,COLOR_WHITE),3); 
   else
   draw_string(x,y,name,(conf.camera_position == 1)?MAKE_COLOR(COLOR_BLUE_P,COLOR_WHITE):MAKE_COLOR(COLOR_RED_P,COLOR_WHITE),3);   
  
 if (selected==top->next) 
 {
  if(last)
   strcpy(mybuf,last->name);
  else
   strcpy(mybuf,"        ");
  }
  else if(selected->prev)
    strcpy(mybuf,selected->prev->name); 
  else
   strcpy(mybuf,"        ");  
    p=strchr(mybuf,'.');
    *p='\0';
    sprintf(name,"%-8s",mybuf);
    if(gPlayRecMode>0)
     draw_string(x,y-52,name,MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
    else
     draw_string(x,y-52,name,MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);   
   
  if((selected==last)||(selected->next==NULL))
  {
   if(top->next)
    strcpy(mybuf,top->next->name);
   else  
    strcpy(mybuf,"        ");
  }
  else if(selected->next)
    strcpy(mybuf,selected->next->name);
  else
   strcpy(mybuf,"        ");     
    p=strchr(mybuf,'.');
    *p='\0';
    sprintf(name,"%-8s",mybuf);
    if(gPlayRecMode>0)
     draw_string(x,y+52,name,MAKE_COLOR(COLOR_BG,COLOR_LIGHT_GREY),3);
    else
     draw_string(x,y+52,name,MAKE_COLOR(COLOR_BG_P,COLOR_LIGHT_GREY_P),3);    
       gui_fselect_redraw=0;
      } 
}

unsigned char inGridSelection()
{
 return ((strcmp(current_dir,"A/SDM/GRIDS")==0)&&(gui_mode==GUI_MODE_FSELECT));
}
 
void gui_fselect_kbd_process() 
{
 if((strcmp(current_dir,"A/SDM/SCRIPTS")==0)&& (strlen(current_dir)==13))gui_easy_fselect_kbd_process();
 else
  {
   int i; 
    switch (get_vkey(kbd_get_autoclicked_key())) 
    {
        case KEY_UP:
            if (selected) 
            {
              fselect_goto_prev(1);
              if(strcmp(current_dir,"A/SDM/GRIDS")==0)
               {
                if(selected->attr & DOS_ATTR_DIRECTORY) play_sound(4);
                else
                 {
                  conf.show_grid_lines=1;
                  sprintf(gridName,"%s/%s",current_dir,selected->name);
                  strncpy(gridShortName,selected->name,strlen(selected->name)-4);
                  gridShortName[strlen(selected->name)-4]='\0';
                  grid_lines_load(gridName);
                  vid_bitmap_refresh();
                  gui_fselect_redraw = 0;
                 }
               }
              else gui_fselect_redraw = 1;
            }
            break;
 
        case KEY_DOWN:       
            if (selected) 
            {
              fselect_goto_next(1);
              if (strcmp(current_dir,"A/SDM/GRIDS")==0)
               {
                if(selected->prev==top)fselect_goto_next(1);
                conf.show_grid_lines=1;
                sprintf(gridName,"%s/%s",current_dir,selected->name);
                strncpy(gridShortName,selected->name,strlen(selected->name)-4);
                gridShortName[strlen(selected->name)-4]='\0';
                grid_lines_load(gridName);
                vid_bitmap_refresh();
                gui_fselect_redraw = 0;
               }
             else gui_fselect_redraw = 1;
            }
            break;

        case KEY_ZOOM_OUT:
            if (selected) 
            {
                fselect_goto_prev(num_lines-1);
                gui_fselect_redraw = 1;
            }
            break;

        case KEY_ZOOM_IN:
            if (selected) 
            {
                fselect_goto_next(num_lines-1);
                gui_fselect_redraw = 1;
            }
            break;

        case KEY_RIGHT:
            if (selected) 
            {
             if(strcmp(current_dir,"A/SDM/SCRIPTS3")!=0)                  
              {
                fselect_goto_next(1);
                gui_fselect_redraw = 1;
              }
             else if(selected->attr != 0xFF)
             {
                if (selected->attr & DOS_ATTR_DIRECTORY) 
                   {
                   } 
                 else  
                   {

                    sprintf(selected_file, "%s/%s", current_dir, selected->name);
                    gui_fselect_free_data();   
                    gui_set_mode(gui_fselect_mode_old);
                    draw_restore();
                    gRunNow=1;
                    if (fselect_on_select) 
                     fselect_on_select(selected_file);
                   }              
             }
            }
            break;

        case KEY_SET:
            if (selected && selected->attr != 0xFF) 
              {
                if (selected->attr & DOS_ATTR_DIRECTORY) 
                   {
                    if((strcmp(selected->name,"SCRIPTS")!=0) && (strcmp(selected->name,"SCRIPTS2")!=0))
                    {
                    i=strlen(current_dir);
                    if (selected->name[0]=='.' && selected->name[1]=='.' && selected->name[2]==0) 
                      {
                        while (current_dir[--i] != '/');
                        current_dir[i]=0;
                      } 
                     else 
                      {
                        sprintf(current_dir+i, "/%s", selected->name);
                      }
                     gui_fselect_read_dir(current_dir);
                     gui_fselect_redraw = 1;
                    }
                   else play_sound(6);
                   } 
                 else  
                   {
                    sprintf(selected_file, "%s/%s", current_dir, selected->name);
                    gui_fselect_free_data();    
                    gui_set_mode(gui_fselect_mode_old);
                    draw_restore();
                    if (fselect_on_select) 
                     fselect_on_select(selected_file);
                   }
             }

            break;

    #if defined (CAMERA_ixus700) || defined (CAMERA_ixus800) || defined (CAMERA_ixus850)|| defined(CAMERA_a550) || defined (CAMERA_a560) || defined(CAMERA_ixus55)|| defined (CAMERA_a460) || defined (CAMERA_ixus70)
        case KEY_DISPLAY:
    #else
        case KEY_ERASE:
    #endif
            if (selected && selected->attr != 0xFF) 
             {
                if (selected->attr & DOS_ATTR_DIRECTORY) 
                  {
                    if (selected->name[0]!='.' || selected->name[1]!='.' || selected->name[2]!=0)
                        gui_mbox_init(LANG_BROWSER_ERASE_DIR_TITLE, LANG_BROWSER_ERASE_DIR_TEXT,
                                      MBOX_TEXT_CENTER|MBOX_BTN_YES_NO|MBOX_DEF_BTN2, fselect_delete_folder_cb);
                  } 
                else 
                  {
                    gui_mbox_init(LANG_BROWSER_DELETE_FILE_TITLE, LANG_BROWSER_DELETE_FILE_TEXT,
                                  MBOX_TEXT_CENTER|MBOX_BTN_YES_NO|MBOX_DEF_BTN2, fselect_delete_file_cb);
                  }
            }
            break;

        case KEY_MENU:
            gui_fselect_free_data();
             if (strcmp(current_dir,"A/SDM/GRIDS")==0)gridFlag=1;  
             gui_set_mode(gui_fselect_mode_old);
             draw_restore();
            if (strcmp(current_dir,"A/SDM/GRIDS")!=0)
            {
             if (fselect_on_select) 
             fselect_on_select(NULL);
            }
            break;
    }
  }
}

