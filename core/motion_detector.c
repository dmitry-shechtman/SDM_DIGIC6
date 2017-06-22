/**

Motion detection module
 

	Author: mx3 (Max Sagaydachny) . win1251 (        )
	Email: win.drivers@gmail.com
 	Skype: max_dtc
 	ICQ#: 125-985-663
 	Country: Ukraine
 	Sity: Kharkiv
 
 
 	20070912 mx3: first version

 	20070918 mx3: speed optimization,
                /--/-COLUMNS, ROWS to split picture into
                 |  |
                 |  |  MEASURE MODE (Y,U,V R,G,B) - U=0, Y=1, V=2, R=3, G=4, B=5
                 |  |  |
                 |  |  |  TIMEOUT
                 |  |  |  |
                 |  |  |  |  COMPARISON INTERVAL (msec)
                 |  |  |  |  |
                 |  |  |  |  |  THRESHOLD ( difference in cell to trigger detection)
                 |  |  |  |  |  |
                 |  |  |  |  |  |  DRAW GRID (0-no, 1-yes)
                 |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  RETURN VARIABLE number of cells with motion detected
                 |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |           OPTIONAL PARAMETERS:
                 |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  REGION (masking) mode: 0-no regions, 1-include,
                 |  |  |  |  |  |  |  |  |      2-exclude
                 |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  REGION FIRST COLUMN
                 |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  REGION FIRST ROW
                 |  |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  |  REGION LAST COLUM
                 |  |  |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  |  |  REGION LAST ROW
                 |  |  |  |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  |  |  |  PARAMETERS: 1-make immediate shoot,
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   2-log debug information into file (* see note below!),
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   4-dump liveview image from RAM to a file,
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   8-on immediate shoot, don't release shutter.
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   16- shoot when still
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   32- display sample points (even if draw_grid = 0)
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   OR-ed values are accepted, e.g. use 9 for
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |   immediate shoot & don't release shutter
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  PIXELS STEP - Speed vs. Accuracy
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |    adjustments (1-use every pixel,
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |    2-use every second pixel, etc)
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  MILLISECONDS DELAY to begin
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |   triggering - can be useful
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |   for calibration with DRAW-
                 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |   GRID option.
md_detect_motion a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p

nobody tried to use MD to get overall luminosity to automatically
adjust shutter speed override?

MD setup:
set delay interval to 2-3 secs
timeout=delay_interval+1
threshold=255 (so it will not trigger ) 
cols=1
rows=1

md_get_cell_diff 1, 1, overall_luminocity

shutter_override_time = some_formulae ( overall_luminocity )

I don't have camera nearby to test it.

I have thought about timelapse movie script which would automatically
override shutter speed at night. I'm planning to make 2 days timelapse
movie ( it seems 8gb SD card and power adapter will help also :-) )

NOTE: when MD stops working on timeout, cells containg absolute values
instead of difference.

*/

void dump_memory();

#define MD_REC_CALLS_CNT 2048
#define MD_INFO_BUF_SIZE 4096

#include "motion_detector.h"

#include "gui.h"
#include "draw_palette.h"
#include "kbd.h"
#include "keyboard.h"
extern int get_luminance;
extern char* imgbuf;
int gDone_once =0;
int xoffset;  
int yoffset; 

#define MD_XY2IDX(x,y) ((y)*motion_detector.columns+x)

void kbd_sched_shoot();
void md_kbd_sched_immediate_shoot(int no_release);

enum {

	MD_MEASURE_MODE_U=0,
	MD_MEASURE_MODE_Y=1,
	MD_MEASURE_MODE_V=2,
	MD_MEASURE_MODE_R=3,
	MD_MEASURE_MODE_G=4,
	MD_MEASURE_MODE_B=5
};

enum {
	MD_DO_IMMEDIATE_SHOOT=1,
	MD_MAKE_DEBUG_LOG_FILE=2,
	MD_MAKE_RAM_DUMP_FILE=4,
      MD_NO_SHUTTER_RELEASE_ON_SHOOT=8,
      MD_SHOOT_WHEN_STILL=16,
      MD_SHOW_SAMPLE_POINTS=32
};

enum {
	MD_REGION_NONE=0,
	MD_REGION_INCLUDE=1,
	MD_REGION_EXCLUDE=2
};

static struct {
	int *curr; //points to buff1 or buff2
	int *prev; //points to buff2 or buff1
	int buff1[MOTION_DETECTOR_CELLS];
	int buff2[MOTION_DETECTOR_CELLS];
	int points[MOTION_DETECTOR_CELLS];

	int columns;
	int rows;
	int threshold;
	int pixel_measure_mode;
	int timeout;
	int measure_interval;

	int last_measure_time;
	int start_time;

	int running;
	int detected_cells;

	int draw_grid;
	int clipping_region_mode;
	int clipping_region_row1;
	int clipping_region_column1;
	int clipping_region_row2;
	int clipping_region_column2;

	int previous_picture_is_ready;

	int ret_var_num;
	int parameters;
	int pixels_step;
	int msecs_before_trigger;

//debug
	int comp_calls_cnt;
	int comp_calls[MD_REC_CALLS_CNT];

} motion_detector;

//motion_detector.curr=NULL;

static int overlay=0;                        

static int clip(int v) {
    if (v<0) v=0;
    if (v>255) v=255;
    return v;
}

int md_init_motion_detector(
 int columns,
 int rows,
 int pixel_measure_mode,
 int detection_timeout,
 int measure_interval,
 int threshold,
 int draw_grid,
 int ret_var_num,
 int clipping_region_mode,
 int clipping_region_column1,
 int clipping_region_row1,
 int clipping_region_column2,
 int clipping_region_row2,
 int parameters,
 int pixels_step,
 int msecs_before_trigger
){

	motion_detector.comp_calls_cnt=0;
	motion_detector.previous_picture_is_ready=0;
	motion_detector.curr=motion_detector.buff1;
	motion_detector.prev=motion_detector.buff2;

	if(		pixel_measure_mode != MD_MEASURE_MODE_Y 
		&&	pixel_measure_mode != MD_MEASURE_MODE_U
		&&	pixel_measure_mode != MD_MEASURE_MODE_V
		&&	pixel_measure_mode != MD_MEASURE_MODE_R
		&&	pixel_measure_mode != MD_MEASURE_MODE_G
		&&	pixel_measure_mode != MD_MEASURE_MODE_B
		){
		pixel_measure_mode = MD_MEASURE_MODE_Y;
	}

	if( columns<1 || rows<1 || columns * rows > MOTION_DETECTOR_CELLS ){
		columns=3;
		rows=3;
	}

	if(msecs_before_trigger<0){
		msecs_before_trigger=0;
	}

	if (pixels_step<1){
		pixels_step=1;
	}

	if(detection_timeout<0){
		detection_timeout=0;
	}

	if(measure_interval<0) {
		measure_interval=0;
	}

	if(threshold<0) 
      {
       if((motion_detector.parameters & MD_SHOOT_WHEN_STILL)!=0)threshold=1;
       else threshold=0;
	}

	motion_detector.msecs_before_trigger=msecs_before_trigger;
	motion_detector.parameters = parameters;
	motion_detector.pixels_step=pixels_step;
	motion_detector.columns=columns;
	motion_detector.rows=rows;
	motion_detector.ret_var_num=ret_var_num;


	motion_detector.pixel_measure_mode=pixel_measure_mode;
	motion_detector.timeout=detection_timeout;
	motion_detector.measure_interval=measure_interval;
	motion_detector.threshold=threshold;
	motion_detector.draw_grid=draw_grid;

	if (clipping_region_column1>clipping_region_column2){
		motion_detector.clipping_region_column2=clipping_region_column1;
		motion_detector.clipping_region_column1=clipping_region_column2;
	} else {
		motion_detector.clipping_region_column2=clipping_region_column2;
		motion_detector.clipping_region_column1=clipping_region_column1;
	}

	if (clipping_region_row1>clipping_region_row2){
		motion_detector.clipping_region_row2=clipping_region_row1;
		motion_detector.clipping_region_row1=clipping_region_row2;
	} else {
		motion_detector.clipping_region_row2=clipping_region_row2;
		motion_detector.clipping_region_row1=clipping_region_row1;
	}

	if (clipping_region_mode!=MD_REGION_NONE && clipping_region_mode!=MD_REGION_INCLUDE && clipping_region_mode!=MD_REGION_EXCLUDE){
		clipping_region_mode=MD_REGION_NONE;
	}
	motion_detector.clipping_region_mode=clipping_region_mode;

	motion_detector.detected_cells=0;
	motion_detector.previous_picture_is_ready=0;
      motion_detector.start_time=get_tick_count();

	motion_detector.last_measure_time = motion_detector.start_time - motion_detector.measure_interval;

	motion_detector.running=1;

      if((motion_detector.parameters & MD_SHOOT_WHEN_STILL)!=0)
       {
        kbd_sched_release(KEY_SHOOT_HALF);
	  kbd_sched_motion_detector();
        kbd_sched_press(KEY_SHOOT_HALF);
       }
      else kbd_sched_motion_detector();
	draw_clear();
      overlay=1;
	return 1;
}

void md_save_calls_history(){
/*
	char buf[200], fn[30];
	char big[MD_INFO_BUF_SIZE];
	int big_ln;
	int calls,i, ln, fd;
  static struct utimbuf t;
    unsigned long t2;
    static struct tm *ttm;

	if( (motion_detector.parameters & MD_MAKE_DEBUG_LOG_FILE) == 0 ){
		return;
	}


	strcpy(fn,"A/MD_INFO.TXT");//,BUILD_NUMBER,motion_detector.pixels_step);
	fd = open(fn, O_WRONLY|O_CREAT, 0777);
	if( fd>=0) {
		script_console_add_line("Writing info file...");
		lseek(fd,0,SEEK_END);
    t2 = time(NULL);
    ttm = localtime(&t2);
    big_ln=sprintf(big, 
				"\r\n--- %04u-%02u-%02u  %02u:%02u:%02u\r\n"
				"CHDK Ver: %s [ #%s ]\r\nBuild Date: %s %s\r\nCamera:  %s [ %s ]\r\n"
				"[%dx%d], threshold: %d, interval: %d, pixels step: %d\r\n"
				"region: [%d,%d-%d,%d], region type: %d\r\n"
				"wait interval: %d, parameters: %d, calls: %d, detected cells: %d\r\n", 
				1900+ttm->tm_year, ttm->tm_mon+1, ttm->tm_mday, ttm->tm_hour, ttm->tm_min, ttm->tm_sec,
				BUILD_NUMBER, BUILD_NUMBER, __DATE__, __TIME__, PLATFORM, PLATFORMSUB,
				motion_detector.columns, motion_detector.rows, motion_detector.threshold, motion_detector.measure_interval, motion_detector.pixels_step,
				motion_detector.clipping_region_column1, motion_detector.clipping_region_row1, motion_detector.clipping_region_column2, motion_detector.clipping_region_row2, motion_detector.clipping_region_mode,
				motion_detector.msecs_before_trigger, motion_detector.parameters, motion_detector.comp_calls_cnt,
				motion_detector.detected_cells
		);

		calls = ( motion_detector.comp_calls_cnt < MD_REC_CALLS_CNT) ?motion_detector.comp_calls_cnt: MD_REC_CALLS_CNT;

		for(i=0;i<calls;i++){
			ln=sprintf(buf,"[%d] - %d\r\n",i,motion_detector.comp_calls[i]);
			if(big_ln+ln>MD_INFO_BUF_SIZE){
	      write(fd,big,big_ln);
				big_ln=0;
			} 
			memcpy(big+big_ln,buf,ln+1);
			big_ln+=ln;
		}
    write(fd,big,big_ln);
		close(fd);
	  t.actime = t.modtime = time(NULL);
    utime(fn, &t);
	}
*/
}

static void mx_dump_memory(void *img){
	char fn[36];
	int fd, i;
	static int cnt=0;

    started();
    mkdir("A/MD");

		do {
			cnt++;
			sprintf(fn, "A/MD/%04d.FB", cnt);
			fd = open(fn, O_RDONLY, 0777);

			if(fd>=0){
				close(fd);
			}
		} while(fd>=0);

		sprintf(fn, "A/MD/%04d.FB", cnt );
		fd = open(fn, O_WRONLY|O_CREAT, 0777);
		if (fd) {
#if !defined(CAMERA_tx1)
	    write(fd, img,vid_get_viewport_width()*screen_height* vid_get_viewport_yscale()*3);
#else
	    write(fd, img, (screen_width-120)*screen_height*3);
#endif
	    close(fd);
		}
  vid_bitmap_refresh();
  finished();

}

int md_detect_motion(void)
{

#define DISPLAYED_POINTS 1000                 
	int *tmp;
	unsigned char * img;
      int idx,tmp1, tmp2, tick, in_clipping_region;
      int vp_w=0;
      int vp_h=0;
      int do_calc=0;
      int x_step=0;  
      int y_step=0;
      int byteOffset;  
	int val;
	register int i, col, row, x, y;
	static char buf[128];
      static int cell_prev=0;
      static int cell_prev2=0;
      static int cell_curr;
      static int cnt = 0;  
      static short spotmeter[3]={0};                 
      static unsigned short int *points = NULL;      
	if(!motion_detector.running) return 0;
	tick=shooting_get_tick_count();

    if(points==NULL) 
   {
    points= (unsigned short int*)malloc(sizeof(unsigned short int)*DISPLAYED_POINTS*2);     
   }
   vp_w=vid_get_viewport_width();									 
   vp_h=vid_get_viewport_height();
 
img = vid_get_viewport_live_fb();
if(img==NULL){img = vid_get_viewport_fb();}
if(get_luminance)
 {
  int cy;
  int centre=vp_w>>1;
  int vp_bw = vid_get_viewport_byte_width() * vid_get_viewport_yscale();
  if((motion_detector.start_time + motion_detector.timeout <= tick))
     {
      for(cy=0;cy<3;cy++){spotmeter[cy]=0;}
           if(1)                                                               	 
                 {
			for(x=centre-12;x<(centre+12);x+=2)                              	 
                      {
				for(y=112;y<128;y+=2)                                      	 
                            {					
				      spotmeter[0]+=img[ (y*vp_bw)+(x*3) + 1];                	 
					if((x%2)==0)
                              {
					 spotmeter[1]+=(signed char)img[(y*vp_bw)+(x*3)];       	 
					 spotmeter[2]+=(signed char)img[(y*vp_bw)+(x*3) + 2];   	 
					} 
                              else 
                              {
					 spotmeter[1]+=(signed char)img[(y*vp_bw)+((x-1)*3)];
					 spotmeter[2]+=(signed char)img[(y*vp_bw)+((x-1)*3) + 2];
					}
				    } 
			    } 

                        ubasic_set_variable(0,spotmeter[0]/96);
                        ubasic_set_variable(1,spotmeter[1]/96);
                        ubasic_set_variable(2,spotmeter[2]/96);
                        get_luminance=0;
                        motion_detector.curr[0]=0;
                        motion_detector.running = 0;
		            return 0;
                } 
      } 
      return 1;
  } 

else
  {

	// if comparison calls < 2048 save tick time in array
	if(motion_detector.comp_calls_cnt < MD_REC_CALLS_CNT) 
      {
		motion_detector.comp_calls[motion_detector.comp_calls_cnt]=tick;
	}
	motion_detector.comp_calls_cnt++;
	// ----------------------------------------------------
	//             operation has timed-out  
      // ----------------------------------------------------
	if(motion_detector.start_time + motion_detector.timeout < tick ) 
      {
		md_save_calls_history();
		motion_detector.running = 0;
            if(points)                                                                
             {
              free(points);
              points=NULL;
             }
            if((motion_detector.parameters & MD_SHOOT_WHEN_STILL)!=0)
             {
              cell_curr=cell_prev=0;
              ubasic_set_variable(motion_detector.ret_var_num,0);                      
              overlay=0;                                                               
              md_draw_grid();
             }
            gDone_once = 0;
		return 0;
	}

    img+= (vid_get_viewport_yoffset() * vid_get_viewport_byte_width() * vid_get_viewport_yscale()) + (vid_get_viewport_xoffset() * 3);
   xoffset = vid_get_viewport_display_xoffset();	 
   yoffset = vid_get_viewport_display_yoffset();	  
    if(!gDone_once && ((motion_detector.parameters & MD_SHOW_SAMPLE_POINTS)!=0))       
   {
	int x_size = CAM_SCREEN_WIDTH-xoffset * 2;
	int y_size = CAM_SCREEN_HEIGHT-yoffset * 2;   
 
    x_step=vp_w/motion_detector.columns;							   
	y_step=vp_h/motion_detector.rows; 								                                               
      x = y = cnt = 0;
    if(points)                                                                                 
     {
	for(row=0, col=0; row < motion_detector.rows ; )                                    
      {
            do_calc=0;
		in_clipping_region=0;
          // ----------- set status of in_clipping_region ----------
		if (
		      col+1 >= motion_detector.clipping_region_column1 
			&& col+1 <= motion_detector.clipping_region_column2
			&& row+1 >= motion_detector.clipping_region_row1
			&& row+1 <= motion_detector.clipping_region_row2
		   )
              
		    in_clipping_region=1;
		  
          // Check if outside excluded region, in clipping region or no clipping/excluded regions

		if(motion_detector.clipping_region_mode==MD_REGION_EXCLUDE && !in_clipping_region) do_calc=1;
		if(motion_detector.clipping_region_mode==MD_REGION_INCLUDE && in_clipping_region)  do_calc=1;
		if(motion_detector.clipping_region_mode==MD_REGION_NONE)                           do_calc=1;
		if(do_calc==1)
                {  
                 for(x=xoffset+col*x_step;x<(((col+1)*x_step)+xoffset);x+=motion_detector.pixels_step)        
                      {
				for(y=row*y_step;y<((row+1)*y_step);y+=motion_detector.pixels_step)  
                            {
                             points[cnt]=(unsigned short int)((x*x_size)/vp_w);
                             points[cnt+1]=(unsigned short int)((y*y_size)/vp_h);
                             cnt+=2;                           
				    }                                                                
			    }                                                                      
		     }               
		col++;                                                                           
		if(col>=motion_detector.columns)
                 {
			col=0;
			row++;
		     }
	  } // next row
         gDone_once = 1;
      }   
   }      
 
      if(points && gDone_once && ((motion_detector.parameters & MD_SHOW_SAMPLE_POINTS)!=0))   
      {
       int xx;
       for(xx=0;xx<cnt;xx+=2)
        {
         draw_filled_rect((coord)points[xx],(coord)(points[xx+1]),(coord)(points[xx]+3),(coord)(points[xx+1]+3),MAKE_COLOR(COLOR_RED,COLOR_RED));
        }
      }
// ------------------------------------------------------------   
 
	if(motion_detector.last_measure_time + motion_detector.measure_interval > tick)	 
      {
		//wait for the next time
		return 1;
	}
// ----------------------------------------------------------------------------------

	motion_detector.last_measure_time=tick;

     //--------------swap data-array pointers so we don't need to copy last data array into Previous one
	tmp=motion_detector.curr;
	motion_detector.curr=motion_detector.prev;
	motion_detector.prev=tmp;

	for(i=0 ; i<motion_detector.rows*motion_detector.columns ; i++ )				 
      {
		motion_detector.points[i]=0;
		motion_detector.curr[i]=0;
	}
 									                 
	int vp_bw = vid_get_viewport_byte_width() * vid_get_viewport_yscale();				 
      int x_step_bytes = motion_detector.pixels_step * 3;
	int y_step_bytes = motion_detector.pixels_step * vp_bw;																  
	x_step=vp_w/motion_detector.columns;
	y_step=vp_h/motion_detector.rows;
 
	for(row=0, col=0; row < motion_detector.rows ; )
      {
       int cell_y_start_bytes = ((row * vp_h) / motion_detector.rows) * vp_bw;
       int cell_y_end_bytes = cell_y_start_bytes + ((vp_h / motion_detector.rows) * vp_bw);
		do_calc=0;
		in_clipping_region=0;
           
		if (
		      col+1 >= motion_detector.clipping_region_column1 
			&& col+1 <= motion_detector.clipping_region_column2
			&& row+1 >= motion_detector.clipping_region_row1
			&& row+1 <= motion_detector.clipping_region_row2
		   )
               {
		    in_clipping_region=1;
		   }
          // Check if outside excluded region, in clipping region or no clipping/excluded regions

		if(motion_detector.clipping_region_mode==MD_REGION_EXCLUDE && in_clipping_region==0)
            {
			do_calc=1;
		} 

		if(motion_detector.clipping_region_mode==MD_REGION_INCLUDE && in_clipping_region==1)
            {
			do_calc=1;
		}

		if(motion_detector.clipping_region_mode==MD_REGION_NONE)
            {
			do_calc=1;
		}
 

		if(do_calc==1)
           {
            int x_start_bytes = ((col * vp_w) / motion_detector.columns) * 3;
            int x_end_bytes = x_start_bytes + ((vp_w / motion_detector.columns) * 3);
		      idx=MD_XY2IDX(col,row);                                                       // index of cell from row and column value
           for(x=x_start_bytes;x<x_end_bytes;x+=x_step_bytes)
          {
            for(y=cell_y_start_bytes;y<cell_y_end_bytes;y+=y_step_bytes)
 {
              // -- set 'val' depending on sampling mode (luminance or chroma)
              //ARRAY of UYVYYY values 6 bytes - 4 pixels

					int cy;
               signed char cv,cu;
					cy=img[x+y+1];              			// calc memory address from x,y for pixel luminance
					if((x%2)==0)                            				// even pixels
                              {
						cu=(signed char)img[x+y];            		// shared blue-luminance value
						cv=(signed char)img[x+y+2];        		 
					} 
               else                                    				 
                              {
						cu=(signed char)img[x+y-3];
						cv=(signed char)img[x+y-1];
					}
					switch(motion_detector.pixel_measure_mode)
                              {
					  case	MD_MEASURE_MODE_Y:
							val=cy;
							break;

					  case	MD_MEASURE_MODE_U:
							val=cu;
                                          break;

					  case	MD_MEASURE_MODE_V:
							val = cv;
							break;

					  case	MD_MEASURE_MODE_R:
                                          val = clip(((cy<<12)+ cv*5743 + 2048)/4096); //R
							break;

					  case	MD_MEASURE_MODE_G:
                                          val = clip(((cy<<12) - cu*1411 - cv*2925 + 2048)/4096); //G
							break;

					  case	MD_MEASURE_MODE_B:
                                          val = clip(((cy<<12) + cu*7258 + 2048)/4096); //B
							break;

						      default:
					            val=cy;
							break;
					}
                             
					motion_detector.curr[ idx ]+=val;
					motion_detector.points[ idx ]++;
				   } 
			    }      
		 }               

            
		col++;
		if(col>=motion_detector.columns)
            {
			col=0;
			row++;
		}
	  } 

	//<< fill "curr" array

      
	if(motion_detector.previous_picture_is_ready==0)
      {
		motion_detector.previous_picture_is_ready=1;
		motion_detector.start_time=get_tick_count();
		motion_detector.last_measure_time=motion_detector.start_time-motion_detector.measure_interval;
		return 1;
	}

          if((motion_detector.parameters & MD_SHOOT_WHEN_STILL)!=0)
           {                
             cell_curr=motion_detector.curr[0];
             tmp1=(cell_prev2<cell_prev)?cell_prev-cell_prev2:cell_prev2-cell_prev;
             tmp2=(cell_curr<cell_prev)?cell_prev-cell_curr:cell_curr-cell_prev;
             cell_prev2=cell_prev;
             cell_prev=cell_curr;
             if((motion_detector.start_time+motion_detector.msecs_before_trigger < tick))               
             {
             if((tmp2<(motion_detector.curr[0]/motion_detector.threshold))&&(tmp1<(motion_detector.curr[0]/motion_detector.threshold)))                               
              {
               motion_detector.running=0;
               cell_prev2=cell_prev=cell_curr=0;
               ubasic_set_variable(motion_detector.ret_var_num,1);                                      
               md_kbd_sched_immediate_shoot(motion_detector.parameters&MD_NO_SHUTTER_RELEASE_ON_SHOOT); 
               md_draw_grid();                                                                          
               gDone_once = 0;
             if(points)                                                                               
             {
              free(points);
              points=NULL;
             }
               return 0;
              } 
             }
              return 1;                                                                                 
           }

     //- compare arrays here and if absolute average change greater than threshold and after specified delay, record as detected

		for ( col=0, row=0; row < motion_detector.rows; )
             {
		  idx=MD_XY2IDX(col,row);                    
			tmp2=0;
			if(motion_detector.points[idx]>0)      
                  {
				motion_detector.prev[idx] = (motion_detector.curr[idx]-motion_detector.prev[idx])/motion_detector.points[idx]; 
				tmp2 = ( motion_detector.prev[idx] < 0 ) ? -motion_detector.prev[idx] : motion_detector.prev[idx] ;            
			}

			if( tmp2 > motion_detector.threshold )                                          
                  {
			  if (motion_detector.start_time+motion_detector.msecs_before_trigger < tick)   
                       {
				motion_detector.detected_cells++;                                         
			     }
			}

			col++;                                                                          
			if(col>=motion_detector.columns)
                  {
			 col=0;
			 row++;
			}
		 }
     //--------- if movement detected 
	      if( motion_detector.detected_cells > 0 )
             {
		  if (motion_detector.start_time+motion_detector.msecs_before_trigger < tick)
               {
		    motion_detector.running=0;
	          ubasic_set_variable(motion_detector.ret_var_num, motion_detector.detected_cells);
		     if(( motion_detector.parameters&MD_DO_IMMEDIATE_SHOOT ) !=0)
                  {
		       //make shoot
                   md_kbd_sched_immediate_shoot(motion_detector.parameters&MD_NO_SHUTTER_RELEASE_ON_SHOOT);
			}
                  gDone_once = 0;
                  if(points)                                                               
                 {
                  free(points);
                  points=NULL;
                 }
			return 0;
		   }
	       }
	       return 1;
         }  
}

int md_get_cell_diff(int column, int row)
{

	if (column<1 || column > motion_detector.columns)
      {
		return 0;
	}

	if (row<1 || row > motion_detector.rows )
      {
		return 0;
	}

	return motion_detector.prev[ MD_XY2IDX(column-1,row-1) ];
}

void md_init()
{
	motion_detector.curr=motion_detector.buff1;
	motion_detector.prev=motion_detector.buff2;
	motion_detector.running=0;
	motion_detector.ret_var_num=-1;
}


int md_running()
{
	return motion_detector.running==1?1:0;//detected_cells>0?1:0;
}


void md_draw_grid()
{
    int col, row;
    int i,j;
    char mdbuff[8];

    if (!md_running() || motion_detector.draw_grid==0 /*|| camera_info.state.state_kbd_script_run==0*/)
    {
        return;
	}

    // display area size
	int x_size = CAM_SCREEN_WIDTH-xoffset * 2;
	int y_size = CAM_SCREEN_HEIGHT-yoffset * 2;

    // initial display offsets
    int y_start, y_end = yoffset;
    int x_start, x_end;
    xoffset = vid_get_viewport_display_xoffset();	// used when image size != viewport size
    yoffset = vid_get_viewport_display_yoffset();	// used when image size != viewport size
    for (i=0, row=0; row < motion_detector.rows /*&& camera_info.state.state_kbd_script_run*/; row++)
    {
        // Calc display start and end offsets
        y_start = y_end;    // reuse last end value as new start value
        y_end = yoffset + ((row + 1) * y_size) / motion_detector.rows;

        x_end = xoffset;

        for (col=0; col < motion_detector.columns; col++, i++)
        {
            // Calc display x start and end offsets
            x_start = x_end;    // reuse last end value as new start value
            x_end = xoffset + ((col + 1) * x_size) / motion_detector.columns;
		      j=MD_XY2IDX(col,row);
		      int tmp2 = ( motion_detector.prev[j] < 0 ) ? -motion_detector.prev[j] : motion_detector.prev[j] ;
            int in_clipping_region = 0;
            if ( col+1>=motion_detector.clipping_region_column1
                && col+1<=motion_detector.clipping_region_column2
                && row+1>=motion_detector.clipping_region_row1
                && row+1<=motion_detector.clipping_region_row2
                )
                in_clipping_region = 1;
 
            if ((motion_detector.clipping_region_mode==MD_REGION_EXCLUDE && in_clipping_region==0) ||
                (motion_detector.clipping_region_mode==MD_REGION_INCLUDE && in_clipping_region==1) ||
                (motion_detector.clipping_region_mode==MD_REGION_NONE))
            {
                if (motion_detector.draw_grid & 1)
                {
                  draw_rect(x_start+2, y_start+2, x_end-2, y_end-2,(tmp2 > motion_detector.threshold)?COLOR_RED:COLOR_GREEN);
                }
            }
        }  
    }  
}

