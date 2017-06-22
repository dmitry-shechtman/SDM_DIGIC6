#include "stdlib.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "conf.h"
#include "font.h"
#include "lang.h"
#include "ubasic.h"
#include "gui.h"
#include "draw_palette.h"
#include "gui_lang.h"
#include "kbd.h"

static volatile int full_palette;
static color cl;
static volatile char counter;
static int palette_mode;
static void (*palette_on_select)(color clr);
static int gui_palette_redraw;
#define COUNTER_N 100

#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
extern char* bitmap_buffer[];
extern int active_bitmap_buffer;
#else
static char* frame_buffer[2];
#endif

void gui_palette_init(int mode, color st_color, void (*on_select)(color clr)) {
    full_palette = (mode!=PALETTE_MODE_SELECT);
    cl = st_color;
    palette_mode = mode;
    palette_on_select = on_select;
    counter = COUNTER_N;
    gui_palette_redraw = 1;
}

void gui_palette_kbd_process() 
{
    switch (get_vkey(kbd_get_autoclicked_key())) {
 
    case KEY_DOWN:
    	if (!full_palette) {
            cl = ((((cl>>4)+1)<<4)|(cl&0x0f))&0xFF;
            gui_palette_redraw = 1;
        }
        break;
    case KEY_UP:
    	if (!full_palette) {
            cl = ((((cl>>4)-1)<<4)|(cl&0x0f))&0xFF;
            gui_palette_redraw = 1;
        }
        break;
    case KEY_LEFT:
    	if (!full_palette) {
            cl = ((((cl&0x0f)-1)&0x0f)|(cl&0xf0))&0xFF;
            gui_palette_redraw = 1;
        }
        break;
    case KEY_RIGHT:
    	if (!full_palette) {
            cl = ((((cl&0x0f)+1)&0x0f)|(cl&0xf0))&0xFF;
            gui_palette_redraw = 1;
        }
        break;
    case KEY_SET:
        if (palette_mode!=PALETTE_MODE_SELECT) {
            full_palette = !full_palette;
            counter = 0;
            gui_palette_redraw = 1;
        } else {
            if (palette_on_select) 
                palette_on_select(cl);
           gui_set_mode(GUI_MODE_MENU);
        }
        break;
    }
}

void gui_palette_draw() {
    unsigned int x, y;
    char f=0;
    color c;
    static char buf[64];

#if defined(CAMERA_tx1)
 int  sw = vid_get_bitmap_width()-120;
#else
 int  sw = vid_get_bitmap_width();
#endif

if((conf.camera_orientation==1)||(conf.camera_orientation==3))sw-=120;
    switch (palette_mode) 
     {
        case PALETTE_MODE_DEFAULT:
            if (full_palette) 
               {
                if (!counter || counter == COUNTER_N) 
                   {
                    for (y=0; y<screen_height; ++y) 
                       {
                        for (x=0; x<sw; ++x) 
                          {
                            c = ((y*16/screen_height)<<4)|(x*16/sw);
                            draw_pixel(x, y, c);
                          }
                        }
                      if (counter) 
                       {
                        draw_txt_string(6, 7, lang_str(LANG_PALETTE_TEXT_1), MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
                        draw_txt_string(6, 8, lang_str(LANG_PALETTE_TEXT_2), MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
                       }
                     }
                    if (counter)
                    --counter;
                   f=1;
               } 
            
            if (!full_palette) 
              {
                sprintf(buf, " %s:0x%02X   ", lang_str(LANG_PALETTE_TEXT_COLOR), cl);
                draw_txt_string(0, 0, buf, MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
                draw_filled_rect(20, 20, sw-20, screen_height-20, MAKE_COLOR(cl, COLOR_WHITE));
              }
            break;
        case PALETTE_MODE_SELECT:
            if (gui_palette_redraw) 
              {
                #define CELL_SIZE   13
                #define BORDER_SIZE 8
                #define CELL_ZOOM   5
                draw_string(8*FONT_WIDTH, 0, "Arrow keys to change  ", MAKE_COLOR(COLOR_BLACK, COLOR_WHITE),1);
                sprintf(buf, " %s:0x%02X ", lang_str(LANG_PALETTE_TEXT_COLOR), cl);
                draw_string(0, 0, buf, MAKE_COLOR(COLOR_BLACK, COLOR_WHITE),1);
              for (y=0; y<16; ++y) 
                    {
                     for (x=0; x<16; ++x) 
                      {
                       c = (y<<4)|x;
                       draw_filled_rect(BORDER_SIZE+x*CELL_SIZE, BORDER_SIZE+y*CELL_SIZE+FONT_HEIGHT, BORDER_SIZE+(x+1)*CELL_SIZE, BORDER_SIZE+(y+1)*CELL_SIZE+FONT_HEIGHT, MAKE_COLOR(c, COLOR_BLACK));
                      }
                    }
                draw_filled_rect(0, FONT_HEIGHT, sw-1, FONT_HEIGHT+BORDER_SIZE-1, MAKE_COLOR(COLOR_GREY, COLOR_GREY)); 
                draw_filled_rect(0, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE+1, sw-1, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE+BORDER_SIZE, MAKE_COLOR(COLOR_GREY, COLOR_GREY)); 
                draw_filled_rect(0, FONT_HEIGHT+BORDER_SIZE, BORDER_SIZE-1, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE, MAKE_COLOR(COLOR_GREY, COLOR_GREY)); 
                draw_filled_rect(BORDER_SIZE+16*CELL_SIZE+1, FONT_HEIGHT+BORDER_SIZE, BORDER_SIZE+16*CELL_SIZE+BORDER_SIZE, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE, MAKE_COLOR(COLOR_GREY, COLOR_GREY)); 
                draw_filled_rect(sw-BORDER_SIZE, FONT_HEIGHT+BORDER_SIZE, sw-1, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE, MAKE_COLOR(COLOR_GREY, COLOR_GREY)); 

                y=(cl>>4)&0x0F; x=cl&0x0F;
                draw_filled_rect(BORDER_SIZE+x*CELL_SIZE-CELL_ZOOM, FONT_HEIGHT+BORDER_SIZE+y*CELL_SIZE-CELL_ZOOM, BORDER_SIZE+(x+1)*CELL_SIZE+CELL_ZOOM, FONT_HEIGHT+BORDER_SIZE+(y+1)*CELL_SIZE+CELL_ZOOM, MAKE_COLOR(cl, COLOR_RED));  
                draw_rect(BORDER_SIZE+x*CELL_SIZE-CELL_ZOOM-1, FONT_HEIGHT+BORDER_SIZE+y*CELL_SIZE-CELL_ZOOM-1, BORDER_SIZE+(x+1)*CELL_SIZE+CELL_ZOOM+1, FONT_HEIGHT+BORDER_SIZE+(y+1)*CELL_SIZE+CELL_ZOOM+1, COLOR_RED); 
                draw_filled_rect(BORDER_SIZE+16*CELL_SIZE+BORDER_SIZE, FONT_HEIGHT+BORDER_SIZE,sw-BORDER_SIZE-1, FONT_HEIGHT+BORDER_SIZE+16*CELL_SIZE, MAKE_COLOR(cl, COLOR_WHITE));

                gui_palette_redraw = 0;
            }
            break;
    }
}

 
unsigned int    screen_width=0, screen_height=0, screen_size=0;
unsigned int    screen_buffer_width=0, screen_buffer_height=0, screen_buffer_size=0;
void            (*draw_pixel_proc)(coord x,coord y,color cl);
 
 static void draw_pixel_std(coord x,coord y,color cl)
{

int xx,yy,i,j;
 
if (conf.camera_orientation==2)

{
#ifdef CAM_USES_ASPECT_CORRECTION
 xx=2*screen_width-2*x;
#else
 xx=screen_width-x;
 #endif
 yy=screen_height-y-1;
}
else if (conf.camera_orientation==1)
{
#ifdef CAM_USES_ASPECT_CORRECTION
xx=2*(screen_width-1)-y*2;
#else
xx=screen_width-1-y;
#endif
yy=x;
}
else if (conf.camera_orientation==3)
{
#ifdef CAM_USES_ASPECT_CORRECTION
 xx=2*y;
#else
 xx=y;
 #endif
 yy=screen_height-x-1;
}
else
{
#ifdef CAM_USES_ASPECT_CORRECTION
 xx=2*x;
#else
 xx=x;
 #endif
 yy=y;
}
 static unsigned int offset;
 unsigned int offsetx;
 offset=yy * screen_buffer_width + xx;   
#ifndef THUMB_FW
    
#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
	bitmap_buffer[active_bitmap_buffer][offset] = cl;
   #ifdef CAM_USES_ASPECT_CORRECTION
   bitmap_buffer[active_bitmap_buffer][offset+1] = cl;
   #endif
#else
	frame_buffer[0][offset] = frame_buffer[1][offset] = cl;
   #ifdef CAM_USES_ASPECT_CORRECTION
	frame_buffer[0][offset] = frame_buffer[1][offset+1] = cl;   
   #endif
#endif

 
#else
    

#ifndef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
#error DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY is required for DIGIC 6 ports
#endif
    register int cli = cl ^ 0xffffffff;   
    extern volatile char *opacity_buffer[];
    static unsigned int prev_offs = 0xffffffff;
    register unsigned int offs2;
    offset*=2;                            
  for(i=0;i<2;i++)                       
  {
   offset+=i*screen_buffer_width;
   for(j=0;j<2;j++)                     
   {
    offset+=j;
    offs2 = (offset>>1)<<2;
    if (cli != 0xffffffff)                          
    {
        if (prev_offs != offs2)                    
        {
            bitmap_buffer[active_bitmap_buffer][offs2+2] = 0x80-((((int)cli)<<5)&0xe0);    
            bitmap_buffer[active_bitmap_buffer][offs2+0] = 0x80-((((int)cli)<<2)&0xe0);    
            prev_offs = offs2;
        }
        if (offset&1) 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+3] = (cli&0xc0);    
        }
        else 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+1] = (cli&0xc0);    
        }
        
        opacity_buffer[active_bitmap_buffer][offset] = (cli&16)?0x60:0xff;
    }
    else 
    {
        if (prev_offs != offs2)
        {
            bitmap_buffer[active_bitmap_buffer][offs2+2] = 0x80;    
            bitmap_buffer[active_bitmap_buffer][offs2+0] = 0x80;    
            prev_offs = offs2;
        }
        if (offset&1) 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+3] = 0;    
        }
        else 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+1] = 0;    
        }
        
        opacity_buffer[active_bitmap_buffer][offset] = 0;
    }
    }
    }
#endif
}
 

 void draw_pixel_grid(coord x,coord y,color cl)   
{
  unsigned int offsetx;
 int i,j; 
   int  offset=y * screen_buffer_width + ASPECT_XCORRECTION(x);   
#ifndef THUMB_FW
    
#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
	bitmap_buffer[active_bitmap_buffer][offset] = cl;
#else
	frame_buffer[0][offset] = frame_buffer[1][offset] = cl;
#endif
#else
    

#ifndef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
#error DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY is required for DIGIC 6 ports
#endif
    register int cli = cl ^ 0xffffffff;   
    extern volatile char *opacity_buffer[];
    static unsigned int prev_offs = 0xffffffff;
    register unsigned int offs2;
    offset*=2;                            
  for(i=0;i<2;i++)                       
  {
   offset+=i*screen_buffer_width;
   for(j=0;j<2;j++)                     
   {
    offset+=j;
    offs2 = (offset>>1)<<2;
    if (cli != 0xffffffff)                          
    {
        if (prev_offs != offs2)                    
        {
            bitmap_buffer[active_bitmap_buffer][offs2+2] = 0x80-((((int)cli)<<5)&0xe0);    
            bitmap_buffer[active_bitmap_buffer][offs2+0] = 0x80-((((int)cli)<<2)&0xe0);    
            prev_offs = offs2;
        }
        if (offset&1) 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+3] = (cli&0xc0);    
        }
        else 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+1] = (cli&0xc0);    
        }
        
        opacity_buffer[active_bitmap_buffer][offset] = (cli&16)?0x60:0xff;
    }
    else 
    {
        if (prev_offs != offs2)
        {
            bitmap_buffer[active_bitmap_buffer][offs2+2] = 0x80;    
            bitmap_buffer[active_bitmap_buffer][offs2+0] = 0x80;    
            prev_offs = offs2;
        }
        if (offset&1) 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+3] = 0;    
        }
        else 
        {
            bitmap_buffer[active_bitmap_buffer][offs2+1] = 0;    
        }
        
        opacity_buffer[active_bitmap_buffer][offset] = 0;
    }
    }
    }
#endif
}

 
void draw_pixel(coord x, coord y, color cl) 
{
 if((draw_pixel_proc!=draw_pixel_grid)&&((conf.camera_orientation==1)||(conf.camera_orientation==3)))
 {
  if((x>screen_height)||(y>screen_width)){return;}
 }
 else if(draw_pixel_proc!=draw_pixel_grid)
 {
  if((x>screen_width)||(y>screen_height))return;
 }
 else if((x>360)||(y>screen_height))return;
 draw_pixel_proc(x,y, cl);
}

 
color draw_get_pixel(coord x, coord y)
{
#ifndef THUMB_FW
    if ((x < 0) || (y < 0) || (x >=  screen_width) || (y >=  screen_height)) return 0;
#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
    return bitmap_buffer[0][y * screen_buffer_width + ASPECT_XCORRECTION(x)];
#else
    return frame_buffer[0][y * screen_buffer_width + ASPECT_XCORRECTION(x)];
#endif
#else
    
    return 0;
#endif
}
 void draw_set_draw_proc(void (*pixel_proc)(coord x,coord y,color cl)) 
{
    draw_pixel_proc = (pixel_proc)?pixel_proc:draw_pixel_std;
}

#ifndef THUMB_FW

#define GUARD_VAL   COLOR_DARK_GREY

void draw_set_guard()
{
#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
    *((unsigned char*)(bitmap_buffer[0])) = GUARD_VAL;
    *((unsigned char*)(bitmap_buffer[1])) = GUARD_VAL;
#else
    *((unsigned char*)(frame_buffer[0])) = GUARD_VAL;
    *((unsigned char*)(frame_buffer[1])) = GUARD_VAL;
#endif
}

int draw_test_guard()
{
#ifdef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
    if (*((unsigned char*)(bitmap_buffer[active_bitmap_buffer])) != GUARD_VAL) return 0;
#else
    if (*((unsigned char*)(frame_buffer[0])) != GUARD_VAL) return 0;
    if (*((unsigned char*)(frame_buffer[1])) != GUARD_VAL) return 0;
#endif
    return 1;
}

#else 

extern volatile char *opacity_buffer[];

void draw_set_guard()
{
    opacity_buffer[active_bitmap_buffer][0] = 0x42;
}

int draw_test_guard()
{
    if (opacity_buffer[active_bitmap_buffer][0] != 0x42) return 0;
    return 1;
}

#endif

void draw_init() {
    register int i;
    screen_width = vid_get_bitmap_width();
    screen_height  = vid_get_bitmap_height();
    screen_size = screen_width * screen_height;
    screen_buffer_width = vid_get_bitmap_buffer_width();
    screen_buffer_height = vid_get_bitmap_buffer_height();
    screen_buffer_size = screen_buffer_width * screen_buffer_height;
#ifndef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
    frame_buffer[0] = vid_get_bitmap_fb();
    frame_buffer[1] = frame_buffer[0] +  screen_buffer_size;
#endif
    draw_set_draw_proc(NULL);                            
}

static inline int abs2(int v) 
{
    return ((v<0)?-v:v);
}
 
#define swap(v1, v2)   {v1^=v2; v2^=v1; v1^=v2;}
void draw_line(coord x1, coord y1, coord x2, coord y2, color cl) {
     unsigned char steep = abs2(y2 - y1) > abs2(x2 - x1);
     if (steep) {
         swap(x1, y1);
         swap(x2, y2);
     }
     if (x1 > x2) {
         swap(x1, x2);
         swap(y1, y2);
     }
     int deltax = x2 - x1;
     int deltay = abs2(y2 - y1);
     int error = 0;
     int y = y1;
     int ystep = (y1 < y2)?1:-1;
     int x;
     for (x=x1; x<=x2; ++x) {
         if (steep) draw_pixel(y, x, cl);
         else draw_pixel(x, y, cl);
         error += deltay;
         if ((error<<1) >= deltax) {
             y += ystep;
             error -= deltax;
         }
     }
}
void draw_rect(coord x1, coord y1, coord x2, coord y2, color cl) 
{
    unsigned int xMin, yMin, xMax, yMax, x, y;
    if (x1>x2) 
    {
    	xMax=x1; xMin=x2;
    } else 
    {
    	xMin=x1; xMax=x2;
    }
    if (y1>y2) 
    {
    	yMax=y1; yMin=y2;
    } else 
    {
    	yMin=y1; yMax=y2;
    }
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
     {
      if (xMax>= screen_width)xMax=screen_width-1;
      if (xMin>= screen_width)xMin=screen_width-1;
      if (yMax>=screen_height) yMax=screen_height-1;
      if (yMin>=screen_height) yMin=screen_height-1;
     }
    else
     {
      if (xMax>= screen_height)xMax=screen_height-1;
      if (xMin>= screen_height)xMin=screen_height-1;
      if (yMax>=screen_width) yMax=screen_width-1;
      if (yMin>=screen_width) yMin=screen_width-1;
     }

    for (y=yMin; y<=yMax; ++y) 
    {
	draw_pixel(xMin, y, cl & 0xff);
	draw_pixel(xMax, y, cl & 0xff);
    }

    for (x=xMin+1; x<=xMax-1; ++x) 
    {
        draw_pixel(x, yMin, cl & 0xff);
        draw_pixel(x, yMax, cl & 0xff);
    }
}

void draw_filled_rect(coord x1, coord y1, coord x2, coord y2, color cl) 
{
    unsigned int xMin, yMin, xMax, yMax, x, y;
    if (x1>x2) {
    	xMax=x1; xMin=x2;
    } else {
    	xMin=x1; xMax=x2;
    }
    if (y1>y2) {
    	yMax=y1; yMin=y2;
    } else {
    	yMin=y1; yMax=y2;
    }
    if((conf.camera_orientation!=1)&&(conf.camera_orientation!=3))
     {
      if (xMax>= screen_width)xMax=screen_width-1;
      if (xMin>= screen_width)xMin=screen_width-1;
      if (yMax>=screen_height) yMax=screen_height-1;
      if (yMin>=screen_height) yMin=screen_height-1;
     }
    else
     {
      if (xMax>= screen_height)xMax=screen_height-1;
      if (xMin>= screen_height)xMin=screen_height-1;
      if (yMax>=screen_width) yMax=screen_width-1;
      if (yMin>=screen_width) yMin=screen_width-1;
     }

    draw_rect(x1, y1, x2, y2, cl);
    for (y=yMin+1; y<=yMax-1; ++y) {
	for (x=xMin+1; x<=xMax-1; ++x) {
	    draw_pixel(x, y, cl>>8);
	}
    }
}

int draw_string(coord x, coord y, const char *s, color cl,int m) 
{
 char *start;                               
 start=(char*)s;

    while(*s)
     {
	draw_char(x, y, *s, cl,m);
	s++;
	x+=m*FONT_WIDTH;
	if ((x>=screen_width) && (*s)){
	    draw_char(x-m*FONT_WIDTH,y, '>', cl,m);
	    break;
	}
    }
    return (s-start)*FONT_WIDTH;              
}


int draw_char(coord x, coord y, const char ch, color cl, int m) 
{
    const unsigned char *sym = (unsigned char*)current_font +
	    ((const unsigned char)ch)*FONT_HEIGHT;  
                  
    int i, ii,j,jj;
  if(m<0)m=0;
  if(m>16)m=16;

    for (i=0; i<FONT_HEIGHT; i++)                                                         
    {
	for (ii=0; ii<FONT_WIDTH; ii++)                                                     
      {
        for(j=0;j<m;j++)                                                                  
         {
           for(jj=0;jj<m;jj++)                                                            
           {
            draw_pixel(x+jj+(m*ii),y+j+(m*i), (sym[i] & (0x80>>ii))? cl&0xff : cl>>8);
           }  
         }
      } 
    }   
      return FONT_WIDTH;  
}

void draw_big_string(coord x, coord y, const char *s, color cl) 
{
    while(*s)				
    {
	draw_char(x, y, *s, cl,BIG_FONT_WIDTH/FONT_WIDTH);	
	s++;
	x+=BIG_FONT_WIDTH;
	if ((x>=screen_width) && (*s))
        {
	    draw_char(x-BIG_FONT_WIDTH,y, '>', cl,BIG_FONT_WIDTH/FONT_WIDTH);
	    break;
	  }
    }
}
 
void draw_big_string_centre_len(int x, int y, int len, const char *str, color cl) 
{
    int l=0,i,j, yy;
    const char *s=str;

     j = (len-((strlen(s)*BIG_FONT_WIDTH)))>>1;   

  if(j)
     {      
       for (i=0; i<j;i++)                       
         {
         for (yy=y; yy<y+BIG_FONT_HEIGHT; ++yy) 
           {
            draw_pixel(x+i, yy, cl>>8);         
           }
         }
      }

        draw_big_string(j+x,y,s,cl);            
        l = j+x+((strlen(s)*BIG_FONT_WIDTH));   

          for (; l<len+x; ++l)                      
           {
            for (yy=y; yy<y+BIG_FONT_HEIGHT; ++yy) 
             {
              draw_pixel(l, yy, cl>>8);          
             }
           }
}
void draw_txt_rect(coord col, coord row, unsigned int length, unsigned int height, color cl) 
{
    draw_rect(col*FONT_WIDTH, row*FONT_HEIGHT, (col+length)*FONT_WIDTH-1, (row+height)*FONT_HEIGHT-1, cl);
}

void draw_txt_rect_exp(coord col, coord row, unsigned int length, unsigned int height, unsigned int exp, color cl) {
    draw_rect(col*FONT_WIDTH-exp, row*FONT_HEIGHT-exp, (col+length)*FONT_WIDTH-1+exp, (row+height)*FONT_HEIGHT-1+exp, cl);
}

void draw_txt_filled_rect(coord col, coord row, unsigned int length, unsigned int height, color cl) 
{
    draw_filled_rect(col*FONT_WIDTH, row*FONT_HEIGHT, (col+length)*FONT_WIDTH-1, (row+height)*FONT_HEIGHT-1, cl);
}

void draw_txt_filled_rect_exp(coord col, coord row, unsigned int length, unsigned int height, unsigned int exp, color cl) 
{
    draw_filled_rect(col*FONT_WIDTH-exp, row*FONT_HEIGHT-exp, (col+length)*FONT_WIDTH-1+exp, (row+height)*FONT_HEIGHT-1+exp, cl);
}

void draw_txt_string(coord col, coord row, const char *str, color cl) 
{
    if((*str == '!') || (*str=='#') || (*str==' ')|| (*str=='@')) str++;     
    draw_string(col*FONT_WIDTH, row*FONT_HEIGHT, str, cl,1);
}

void draw_txt_char(coord col, coord row, const char ch, color cl) 
{
    draw_char(col*FONT_WIDTH, row*FONT_HEIGHT, ch, cl,1);
}

void draw_clear() 
{
#ifndef DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY
    register unsigned int i;
    for (i=0; i<screen_buffer_size; ++i)
        frame_buffer[0][i] = frame_buffer[1][i] = COLOR_TRANSPARENT;
#endif
}

void draw_restore() 
{
    vid_bitmap_refresh();
}

void draw_fill(coord x, coord y, color cl_fill, color cl_bound) 
{
   if (draw_get_pixel(x, y) != cl_bound && draw_get_pixel(x, y) != cl_fill) {
       draw_pixel(x, y, cl_fill);

       draw_fill((x+1), y, cl_fill,cl_bound);
       draw_fill((x-1), y, cl_fill,cl_bound);
       draw_fill(x, (y+1), cl_fill,cl_bound);
       draw_fill(x, (y-1), cl_fill,cl_bound);
   }
}

void draw_circle(coord x, coord y, const unsigned int r, color cl) 
{
    int dx = 0;
    int dy = r;
    int p=(3-(r<<1));

    do {
        draw_pixel((x+dx),(y+dy),cl);
        draw_pixel((x+dy),(y+dx),cl);
        draw_pixel((x+dy),(y-dx),cl);
        draw_pixel((x+dx),(y-dy),cl);
        draw_pixel((x-dx),(y-dy),cl);
        draw_pixel((x-dy),(y-dx),cl);
        draw_pixel((x-dy),(y+dx),cl);
        draw_pixel((x-dx),(y+dy),cl);

        ++dx;

        if (p<0) 
            p += ((dx<<2)+6);
        else {
            --dy;
            p += (((dx-dy)<<2)+10);
        }
    } while (dx<=dy);
}

void draw_ellipse(coord xc, coord yc, unsigned int a, unsigned int b, color cl) 
{
    int x = 0, y = b;
    long a2 = (long)a*a, b2 = (long)b*b;
    long crit1 = -((a2>>2) + (a&1) + b2);
    long crit2 = -((b2>>2) + (b&1) + a2);
    long crit3 = -((b2>>2) + (b&1));
    long t = -a2*y;
    long dxt = b2*x*2, dyt = -2*a2*y;
    long d2xt = b2*2, d2yt = a2*2;

    while (y>=0 && x<=a) {
        draw_pixel(xc+x, yc+y, cl);
        if (x!=0 || y!=0)
            draw_pixel(xc-x, yc-y, cl);
        if (x!=0 && y!=0) {
            draw_pixel(xc+x, yc-y, cl);
            draw_pixel(xc-x, yc+y, cl);
        }
        if (t + b2*x <= crit1 || t + a2*y <= crit3)
            ++x, dxt += d2xt, t += dxt;
        else if (t - a2*y > crit2)
            --y, dyt += d2yt, t += dyt;
        else {
            ++x, dxt += d2xt, t += dxt;
            --y, dyt += d2yt, t += dyt;
        }
   }
}

void draw_filled_ellipse(coord xc, coord yc, unsigned int a, unsigned int b, color cl) 
{
    int x = 0, y = b;
    int rx = x, ry = y;
    unsigned int width = 1;
    unsigned int height = 1;
    long a2 = (long)a*a, b2 = (long)b*b;
    long crit1 = -((a2>>2) + (a&1) + b2);
    long crit2 = -((b2>>2) + (b&1) + a2);
    long crit3 = -((b2>>2) + (b&1));
    long t = -a2*y;
    long dxt = 2*b2*x, dyt = -2*a2*y;
    long d2xt = 2*b2, d2yt = 2*a2;
    
    color cl_fill = ((cl >> 8) & 0xff) | (cl & 0xff00);

    if (b == 0) {
        draw_filled_rect(xc-a, yc, (a<<1)+1, 1, cl_fill);
    } else {
        while (y>=0 && x<=a) {
            if (t + b2*x <= crit1 || t + a2*y <= crit3) {
                if (height == 1)
                    ; //draw nothing 
                else if (ry*2+1 > (height-1)*2) {
                    draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+(height-1)-1, cl_fill);
                    draw_filled_rect(xc-rx, yc+ry+1, xc-rx+width-1, yc+ry+1+(1-height)-1, cl_fill);
                    ry -= height-1;
                    height = 1;
                }
                else {
                    draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+(ry*2+1)-1, cl_fill);
                    ry -= ry;
                    height = 1;
                }
                ++x, dxt += d2xt, t += dxt;
                rx++;
                width += 2;
            }
            else if (t - a2*y > crit2) {
                --y, dyt += d2yt, t += dyt;
                height++;
            }
            else {
                if (ry*2+1 > height*2) {
                    draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+height-1, cl_fill);
                    draw_filled_rect(xc-rx, yc+ry+1, xc-rx+width-1, yc+ry+1-height-1, cl_fill);
                }
                else {
                    draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+(ry*2+1)-1, cl_fill);
                }
                ++x, dxt += d2xt, t += dxt;
                --y, dyt += d2yt, t += dyt;
                rx++;
                width += 2;
                ry -= height;
                height = 1;
            }
        }

        if (ry > height) {
            draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+height-1, cl_fill);
            draw_filled_rect(xc-rx, yc+ry+1, xc-rx+width-1, yc+ry+1-height-1, cl_fill);
        }
        else {
            draw_filled_rect(xc-rx, yc-ry, xc-rx+width-1, yc-ry+(ry*2+1)-1, cl_fill);
        }
    }
}

