#include "stdlib.h"
#include "keyboard.h"
#include "platform.h"
#include "core.h"
#include "conf.h"
#include "gui.h"
#include "draw_palette.h"
#include "batt_grid.h"
// -------------------------------------------------------------------
static char osd_buf[32];
static unsigned char widescreen;
static unsigned char gridColour[]={COLOR_BLACK,COLOR_WHITE,COLOR_RED,COLOR_GREEN,COLOR_BLUE};
static unsigned char gridColourP[]={COLOR_BLACK_P,COLOR_WHITE_P,COLOR_RED_P,COLOR_GREEN_P,COLOR_BLUE_P};
extern int gZoomPoint;
// -------------------------------------------------------------------
long get_batt_average() 
{
    #define VOLTS_N         100
    static unsigned short   volts[VOLTS_N] = {0};
    static unsigned int     n = 0, rn = 0;
    static unsigned long    volt_aver = 0;

    volt_aver-=volts[n];
    volts[n]=(unsigned short)stat_get_vbatt();
    volt_aver+=volts[n];
    if (++n>rn) rn=n;
    if (n>=VOLTS_N) n=0;
    return volt_aver/rn;
}

// -------------------------------------------------------------------
unsigned long get_batt_perc() 
{
    unsigned long v;
    v = get_batt_average();
    if (v>conf.batt_volts_max) v=conf.batt_volts_max;
    if (v<conf.batt_volts_min) v=conf.batt_volts_min;
    return (v-conf.batt_volts_min)*100/(conf.batt_volts_max-conf.batt_volts_min);
}

// -------------------------------------------------------------------
static void gui_batt_draw_icon () 
{
    coord x;
    int xx=((conf.camera_orientation==1)||(conf.camera_orientation==3))?2:conf.batt_icon_pos.x ;
    int perc = get_batt_perc();
    color cl = (perc<=10)?COLOR_RED:(conf.batt_icon_color&0xFF);
    int y=((conf.camera_orientation==1)||(conf.camera_orientation==3))?conf.batt_icon_pos.y+FONT_HEIGHT+2:conf.batt_icon_pos.y;

    //battery icon
    draw_rect(xx+3-1,    y+1,     xx+3+25+1, y+1+10,  cl);
    draw_rect(xx+3-3,    y+1+2,   xx+3-2,    y+1+8,   cl);
    draw_line(xx+3-4,    y+1+2-1, xx+3-4,    y+1+8+1, COLOR_BLACK);  // l
    draw_line(xx+3-2,    y+1-1,   xx+3+25+2, y+1-1,   COLOR_BLACK);  // t
    draw_line(xx+3-2,    y+1+11,  xx+3+25+2, y+1+11,  COLOR_BLACK);  // b
    draw_line(xx+3+25+2, y+1-1,   xx+3+25+2, y+1+10,  COLOR_BLACK);  // r
    
    //battery fill
    x=xx+3+1+25-(perc/4);
    if((conf.camera_orientation==1)||(conf.camera_orientation==3))
     {
      if (x<=xx+3) x=xx+3+1;
      if (x>xx+3+25+1) x=xx+3+25+1;
     }
    else
     {
      if (x<=conf.batt_icon_pos.x+3) x=conf.batt_icon_pos.x+3+1;
      if (x>conf.batt_icon_pos.x+3+25+1) x=conf.batt_icon_pos.x+3+25+1;
     }
    draw_filled_rect(xx+3,y+1+1, x-1, y+1+9, MAKE_COLOR(COLOR_TRANSPARENT, COLOR_BLACK));
    draw_filled_rect(x, y+1+1, xx+3+25, y+1+9, MAKE_COLOR(cl, cl));
}

// -------------------------------------------------------------------
static void gui_batt_draw_charge()
{
    int x=conf.batt_txt_pos.x ;
    sprintf(osd_buf, "%3d%%", get_batt_perc());
    int y=((conf.camera_orientation==1)||(conf.camera_orientation==3))?conf.batt_txt_pos.y+FONT_HEIGHT+2:conf.batt_txt_pos.y;
    osd_buf[5]=0;
    if((conf.camera_orientation==1)||(conf.camera_orientation==3))
     {
      draw_string(34, y, osd_buf,conf.osd_color,1);
     }
    else
     {
      draw_string(x, y, osd_buf,conf.osd_color,1);
     }
}

// -------------------------------------------------------------------
static void gui_batt_draw_volts() 
{
    unsigned long v;
    v = get_batt_average();
    sprintf(osd_buf, "%ld.%03ld", v/1000, v%1000);
    osd_buf[5]=0;
    draw_string(((conf.camera_orientation==1)||(conf.camera_orientation==3))?conf.batt_icon_pos.x+33:conf.batt_txt_pos.x, conf.batt_txt_pos.y, osd_buf,conf.osd_color,1);
}

// -------------------------------------------------------------------
void gui_batt_draw_osd() 
{
    if (conf.batt_perc_show) {
        gui_batt_draw_charge();
    } else if (conf.batt_volts_show) {
        gui_batt_draw_volts();
    }
    
    if (conf.batt_icon_show) {
        gui_batt_draw_icon();
    }
}

// -------------------------------------------------------------------

// -------------------------------------------------------------------
#define GRID_BUF_SIZE               0x1000
#define GRID_REDRAW_INTERVAL        4
// -------------------------------------------------------------------
typedef enum 
{
    GRID_ELEM_LINE,
    GRID_ELEM_RECT,
    GRID_ELEM_FILLED_RECT,
    GRID_ELEM_ELLIPSE,
    GRID_ELEM_FILLED_ELLIPSE,
} grid_elem_type;

struct gline 
{
    grid_elem_type      type;
    coord               x0,y0,x1,y1;
    color               clf, clb;
    struct gline        *next;
};

char grid_title[36];

static int interval = GRID_REDRAW_INTERVAL;
static struct gline *head=NULL, *top=NULL;


static const char *grid_default =
 
 "@title 8x8 Net\n"
 "@line 0, 29, 359, 29, 3\n"
 "@line 0, 59, 359, 59, 3\n"
 "@line 0, 89, 359, 89, 3\n"
 "@line 0, 119, 359, 119, 3\n"
 "@line 0, 149, 359, 149, 3\n"
 "@line 0, 179, 359, 179, 3\n"
 "@line 0, 209, 359, 209, 3\n"

 "@line 44, 0, 44, 239, 3\n"
 "@line 89, 0, 89, 239, 3\n"
 "@line 134, 0, 134, 239, 3\n"
 "@line 179, 0, 179, 239, 3\n"
 "@line 224, 0, 224, 239, 3\n"
 "@line 269, 0, 269, 239, 3\n"
 "@line 314, 0, 314, 239, 3\n";


// -------------------------------------------------------------------
static void grid_lines_free_data() {
    struct gline  *ptr = head, *prev;

    while (ptr) {
        prev=ptr;
        ptr=ptr->next;
        free(prev);
    }
    head=top=NULL;
}

// -------------------------------------------------------------------
static void process_title(const char *title) {
    register const char *ptr = title;
    register int i=0;

    while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
    while (i<(sizeof(grid_title)-1) && ptr[i] && ptr[i]!='\r' && ptr[i]!='\n') {
        grid_title[i]=ptr[i];
        ++i;
    }
    grid_title[i]=0;
}

// -------------------------------------------------------------------
static void process_element(const char *str, int n, grid_elem_type type) {
    register const char *ptr = str;
    char *nptr;
    register int i;
    long nums[6];
    struct gline  *gptr;

    for (i=0; i<(sizeof(nums)/sizeof(nums[0])) && i<n; ++i) {
        while (ptr[0]==' ' || ptr[0]=='\t' || ptr[0]==',') ++ptr; 
        nums[i] = strtol(ptr, &nptr, 0);
        if (nptr == ptr) { 
            return;
        } else {
            ptr = nptr;
        }
    }

    gptr = malloc(sizeof(struct gline));
    if (gptr) {
        gptr->type=type;
#if defined(CAMERA_sx220hs) || defined(CAMERA_sx230hs)
        int offset = 60;
        if(widescreen)offset=0;
        gptr->x0=nums[0]+offset; 
        gptr->x1=nums[2]+offset;
#else
        gptr->x0=nums[0];  
        gptr->x1=nums[2];
#endif
        gptr->y0=nums[1]; 
        gptr->y1=nums[3];
        gptr->clf=nums[4]; 
        gptr->clb=nums[5];
	if (!head) head=gptr;
	if (top) top->next=gptr;
        gptr->next=NULL;
	top=gptr;
    }
}

// -------------------------------------------------------------------
static void parse_grid_file(const char *fn, const char *ptr) {
    char *c;

    c=strrchr(fn, '/');
    strncpy(grid_title, (c)?c+1:fn, sizeof(grid_title));
    grid_title[sizeof(grid_title)-1]=0;

    while (ptr[0]) {
        while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
        if (ptr[0]=='@') {
            if (strncmp("@title", ptr, 6)==0) {
                ptr+=6;
                process_title(ptr);
            } else if (strncmp("@line", ptr, 5)==0) {
                ptr+=5;
                process_element(ptr, 5, GRID_ELEM_LINE);
            } else if (strncmp("@rectf", ptr, 6)==0) {
                ptr+=6;
                process_element(ptr, 6, GRID_ELEM_FILLED_RECT);
            } else if (strncmp("@rect", ptr, 5)==0) {
                ptr+=5;
                process_element(ptr, 5, GRID_ELEM_RECT);
            } else if (strncmp("@elpsf", ptr, 6)==0) {
                ptr+=6;
                process_element(ptr, 5, GRID_ELEM_FILLED_ELLIPSE);
            } else if (strncmp("@elps", ptr, 5)==0) {
                ptr+=5;
                process_element(ptr, 5, GRID_ELEM_ELLIPSE);
            }
        }
        while (ptr[0] && ptr[0]!='\n') ++ptr;
        if (ptr[0]) ++ptr;
    }
}

// -------------------------------------------------------------------
void grid_lines_load(const char *fn) 
{
    char *buf;
    const char *grid;
    int fd;

    if (fn[0]) {
        buf = umalloc(GRID_BUF_SIZE);
        if (!buf) return;
        grid = grid_default;
        fd = open(fn, O_RDONLY, 0777);
        if (fd>=0) {
            int rcnt = read(fd, buf, GRID_BUF_SIZE);
            if (rcnt > 0) {
                if (rcnt == GRID_BUF_SIZE) 
        	    buf[GRID_BUF_SIZE-1] = 0;
                else
        	    buf[rcnt] = 0;
                grid = buf;
            }
            close(fd);
            strcpy(conf.grid_lines_file, fn);
        } else {
            conf.grid_lines_file[0] = 0;
        }
          grid_lines_free_data();
          if((fn[strlen(fn)-1]=='w')||(fn[strlen(fn)-1]=='W'))widescreen=1;
          else widescreen=0;
          parse_grid_file(fn, grid);
        ufree(buf);
    }
}

// -------------------------------------------------------------------
void gui_grid_draw_osd(int force) 
{
    struct gline  *ptr;
    char osd_buf[32];
    int delta;  
    static int pz=0;  
    static int cz=0; 
    coord xpos0; 
    coord xpos1; 
    int offset; 
    unsigned char fcolour,bcolour;
 
 
    if ((force || --interval==0)&& conf.show_grid_lines)                         
     {
      draw_set_draw_proc(draw_pixel_grid); 
        for (ptr=head; ptr; ptr=ptr->next) 
          {

            xpos0 = ptr->x0;
            xpos1 = ptr->x1;
            fcolour=ptr->clf;
            bcolour=ptr->clb;
            if(fcolour>4)fcolour=1;
            if(bcolour>4)bcolour=1;
            if ((mode_get() & MODE_MASK) != MODE_PLAY)
             {
              fcolour=gridColour[fcolour];
              bcolour=gridColour[bcolour];
             }
             else
             {
              fcolour=gridColourP[fcolour];
              bcolour=gridColourP[bcolour];
             }
            switch (ptr->type) 
              {
                case GRID_ELEM_LINE:
                    draw_line(xpos0, ptr->y0,xpos1 , ptr->y1, (conf.grid_force_color)?conf.grid_color:fcolour);
                    break;
                case GRID_ELEM_RECT:
                    draw_rect(xpos0,ptr->y0,xpos1,ptr->y1, (conf.grid_force_color)?conf.grid_color:fcolour);
                    break;
                case GRID_ELEM_FILLED_RECT:
                    draw_filled_rect(xpos0, ptr->y0,xpos1 , ptr->y1, (conf.grid_force_color)?conf.grid_color:MAKE_COLOR(bcolour, fcolour));
                    break;
                case GRID_ELEM_ELLIPSE:
                    draw_ellipse(xpos0, ptr->y0, (unsigned int)(xpos1), (unsigned int)(ptr->y1), (conf.grid_force_color)?conf.grid_color:fcolour);
                    break;
                case GRID_ELEM_FILLED_ELLIPSE:
                    draw_filled_ellipse(xpos0, ptr->y0, (unsigned int)(xpos1), (unsigned int)(ptr->y1), (conf.grid_force_color)?conf.grid_color:MAKE_COLOR(bcolour, 0));
                    break;
               }
            } 
          draw_set_draw_proc(NULL);
		 interval = GRID_REDRAW_INTERVAL;  
      }  
 
         if((--interval==0)&& conf.enable_yaw_guideline)           
         {
               cz=gZoomPoint;
               if((conf.camera_orientation==1)||(conf.camera_orientation==3))  
                 delta=(int)(320*conf.yaw/get_fov(cz));                                       
               else  
                 delta =(int)(360*conf.yaw/get_fov(cz));                                                  
               offset=delta;
// --------------------------------------------------------------------------------------------------------
             if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3)) 
             {
               if(conf.enable_yaw_guideline==1) 
                {
                draw_line(screen_width/2,0,screen_width/2,screen_height,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                while(offset<(360/2))
                 {
                  draw_line((screen_width/2-1)+offset,0,(screen_width/2-1)+offset,239,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                  draw_line((screen_width/2-1)-offset,0,(screen_width/2-1)-offset,239,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                  offset+=delta;
                 }
                } 

               else if((conf.enable_yaw_guideline==2)) 
                {
#if defined(CAMERA_sx220hs) || defined(CAMERA_sx230hs)
                draw_line(60,239,419,0,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                while(offset<(360))
                 {
                  draw_line(offset+60,239,419,(2*offset)/3,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                  draw_line(60,239-((2*offset)/3),419-offset,0,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
#else
                draw_line(0,239,359,0,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                while(offset<(360))
                 {
                  draw_line(offset,239,359,(2*offset)/3,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                  draw_line(0,239-((2*offset)/3),359-offset,0,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
#endif 
                offset+=delta;
                 }
                } 
              } 
// ---------------------------------------------------------------------------------------------------------
             else if((conf.camera_orientation==1)||(conf.camera_orientation==3)) 
              {
                if(conf.camera_orientation==1) 
                {
                  if((delta<(screen_height/2))&& delta && !conf.disable_deviation_guideline) 
                  draw_line(119+delta,0,119+delta,240,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                 else if(conf.disable_deviation_guideline) 
                  {
                   while(offset<(screen_height/2))
                    {
                     draw_line(119+offset,0,119+offset,240,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                     draw_line(119-offset,0,119-offset,240,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                     offset+=delta;
                    }
                   }
                 } 
// ---------------------------------------------------------------------------------------------------------
               else 
                {
                   while(offset<(screen_height/2))
                   {
                    draw_line(239+offset,0,239+offset,240,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                    draw_line(239-offset,0,239-offset,240,(conf.grid_force_color)?conf.grid_color:COLOR_RED);
                    offset+=delta;
                   }
                 } 		 
                }   
 
			interval = GRID_REDRAW_INTERVAL;		
		 } 
// ---------------------------------------------------------------------------------------------------------

             if(conf.enable_yaw_guideline && (cz!=pz))
             {
              pz=cz;
              draw_restore();
             }
}
