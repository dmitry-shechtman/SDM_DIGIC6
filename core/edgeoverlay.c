#include "edgeoverlay.h"
#include "platform.h"
#include "conf.h"
#include "keyboard.h"
#include "stdlib.h"
#include "draw_palette.h"

static int inmem=0;
static int full_press = 0;
static int shotTaken = 0;
#if !defined(OPT_NEW_OVERLAY)

//This edge overlay code has major changes to the "old" one. One major change
//is, that it doesn cause my cam (ixus 950) to crash anymore by mistaking the
//boundaries of the viewport buffer it can read from and write pixels to.
//Unfortunately it still has the bug (that is also present in the original
//version) that when you move the overlay too much you again overwrite data.
//One might not necessarily notice it at once, or at all, but for me
//unfortunately it overwrote vital data for the chdk menu structure.
//Also the old version was flickering for me and wasn't playing well with the
//chdk osd. I tried to change a bit about that, which makes it also a bit
//faster updating, at least on my cam.
//And then of course, this can load the viewport to a seperate file 

//the way we save edge overlays on their own...
#define EDGE_FILE_PREFIX "EDG_"
#define EDGE_FILE_FORMAT EDGE_FILE_PREFIX "%04d.edg"
extern char *imgbuf;
extern int imgbuf_size;
char * imgbuf_end = 0;
int two_shot=0;
int hs,zs;

//whole viewport size in bytes ??
//width in bytes of one viewport line ??
static int viewport_width;
//flag to remember if current buffer is already saved, so hitting save won't
//save it again
static int is_saved = 0;
//set this to 1 when things need to be moved, so that a redraw clears "old"
//pixels... Otherwise it just will write pixels that need edge overlay data.
//This sometimes leaves trails when the pixel alignment isn't nice but it is
//better than writing "transparent" to everwhere, essentially overwriting
//important things that will cause flickering.
static int need_redraw = 0;

//debug output that waits
void out_wait( const char* buf );

void unlock_overlay()
{
 shotTaken = 0;
 full_press = 0;
}

void get_viewport_size( ) {
	static int viewport_height;

	//since screen_height is used in the drawing downwards, we should use it
	//here too to calculate the buffer we need...
#if !defined(CAMERA_g12)
	viewport_height = screen_height;
#else
	viewport_height = 480;
#endif
#if defined(CAMERA_ixus980)
	viewport_width = screen_width * 3 / 2;
#elif defined(CAMERA_sx200is) || defined(CAMERA_ixus95)||defined(CAMERA_sx130is)|| defined(CAMERA_sx150is) || defined(CAMERA_sx160is) 
      viewport_width=vid_get_viewport_width()*3;
#elif defined(CAMERA_g11)||defined(CAMERA_g12)||defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus220)
      viewport_width=720*3;
#else
	viewport_width = screen_width * 3;
#endif
}

void ensure_allocate_imagebuffer( ) 
{ 

}

//scans a filename for the number of the edge detection file it contains
int get_edge_file_num( const char* fn )
{
	int num = 0;
	if( strncmp(fn,EDGE_FILE_PREFIX,sizeof(EDGE_FILE_PREFIX)-1) == 0 )
	{ //has the correct beginning at least, now try to read as a number...
		fn += sizeof(EDGE_FILE_PREFIX);
		while( *fn == '0' ) 
		{
			++fn;
		}
		while( isdigit(*fn) )
		{
			num *= 10;
			num += *fn - '0';
			++fn;
		}
		//ignore anything else after it, that is like the ending etc.
	}
	return num;
}

//we eat up to 300k of memory, for people needing it we have a menu point
//where they can manually free it. makes of course only sense when the edge
//overlay is not active.

void free_memory_edge_overlay(void)
{
 int x;

	char buf[64];
      conf.outline_mode=0;      
      conf.stereo_mode=0;
      conf.invert_playback=0;

      
  if(imgbuf)
  {

#if defined(CAMERA_ixus980)
      for(x = 0; x < vid_get_viewport_height()*screen_width*3 / 2;x++) imgbuf[x]=0;               
#elif defined(CAMERA_g11)||defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)||defined(CAMERA_ixus105)|| defined(CAMERA_ixus220) || defined(CAMERA_sx1)|| defined(CAMERA_sx20)
      for(x = 0; x < 720*240*3;x++) imgbuf[x]=0; 
#elif defined(CAMERA_g12)
      for(x = 0; x < 720*480*3;x++) imgbuf[x]=0;
#elif defined(CAMERA_ixus100)                                                                     
      for(x = 0; x < (360 * 160 *3);x++) imgbuf[x]=0;                                               
#else
        for(x = 0; x < vid_get_viewport_height()*vid_get_viewport_width()*3;x++) imgbuf[x]=0;     
#endif
	free(imgbuf);
	imgbuf = 0;
      play_sound(4);
	sprintf(buf,"Freed %u bytes",imgbuf_size);
	draw_string(30, 20, buf, conf.osd_color,1);
	imgbuf_size = 0;
  }

}

//saves the actual active overlay data to a file... Well, actually the
//viewport is saved...
void save_edge_overlay(void)
{

	char fn[64];
	char msg[64];
	FILE *fd;
	DIR* d;
	int fnum = 0;
	int fr = 0;
	int zoom = 0;
	struct dirent* de;
	static struct utimbuf t;
	//nothing to save? then dont save
	if( !imgbuf ) return;
	zoom = shooting_get_zoom();

	//first figure out the most appropriate filename to use
	d = opendir(EDGE_SAVE_DIR);
	if( ! d )
	{
		return;
	}

	while( (de = readdir(d)) )
	{
		fr = get_edge_file_num(de->name);
		if( fr > fnum )
		{
			fnum = fr;
		}
	}

	++fnum; //the highest is set, we use the next one
	get_viewport_size();
	//open the right file
	sprintf(fn, EDGE_SAVE_DIR "/" EDGE_FILE_FORMAT, fnum );
	fd = fopen(fn, "wb");
	if(fd !=NULL)
	{

		//write the data
		fwrite(imgbuf,imgbuf_size,1,fd);
		fwrite(&zoom,sizeof(zoom),1,fd);
		is_saved = 1;
		fclose(fd);
#if !defined(CAMERA_ixus105)
		t.actime = t.modtime = time(NULL);
		utime(fn, &t);
#endif
		sprintf(msg, "Saved as %s",fn);
		draw_string(30, 10, msg, conf.osd_color,1);
            play_sound(4);
	}

	closedir(d);

}

//load the viewport copy thats being used for edge detection (and from that
//displaying) from a file
void load_edge_overlay( const char* fn ) 
{
#if !defined(CAMERA_a800)							
	FILE *fd;
	int ret,ret2;
	int zoom;

	is_saved = 1; //won't want to save it again, its already there
	get_viewport_size();
      if(!imgbuf)allocate_imgbuf();
      imgbuf_end = imgbuf + (imgbuf_size);
	fd = fopen(fn,"rb");
	if( fd != NULL )
	{
#if defined(CAMERA_ixus100)
		ret = fread(imgbuf,960*160,1,fd);            
#else
		ret = fread(imgbuf,imgbuf_size,1,fd);
#endif
		ret2 = fread (&zoom,sizeof(zoom),1,fd);
		fclose(fd);
		if( (ret == 1) && (ret2 == 1) )
		{
			inmem = 1; //fake having loaded stuff
			if (conf.edge_overlay_zoom)	{
				shooting_set_zoom(zoom);
			}
		}
	}
#endif
}

#if defined(CAMERA_ixus100)
void edge_overlay()
{
	static int shotTaken = 0;
	static int imgmem = 0;
	static int ymin = 0;
	static int thresh;
      static int yoffset = 40 ;                    
	thresh = conf.edge_overlay_thresh;           //40
	static int full_press = 0;                   //cure for flaky behavior. due to multiple  returns to the script during one full press
	static char strbuf[7] = "Frozen";
	static unsigned char *img;
	int i, hi, c;
	int x, y, h, v, ymax, y1, x1, y2;
	const char * ptrh1;
	const char * ptrh2;
	const char * ptrv1;
	const char * ptrv2;
	char xbuf[64];
	char * optr;
      int screen_width=360;
	is_saved = 0;                                //a new one, we could potentially save it

	if((mode_get()&MODE_MASK) != MODE_PLAY)
	{
	 img = vid_get_viewport_fb();                
	}
	else
	{
	 img = vid_get_viewport_fb_d();              
	}                      
	static int viewport_height;
	viewport_height = 240;
	viewport_width = 360 * 3;
      img+= viewport_width * yoffset;                     
      if(!imgbuf)allocate_imgbuf();
      imgbuf_end = imgbuf+imgbuf_size;
	if(imgbuf == 0) return;                              //ensure failed, make the best we can out of it

	if(conf.edge_overlay_play || ((mode_get()&MODE_MASK) != MODE_PLAY))  
       {
		if ((kbd_is_key_pressed(KEY_SHOOT_HALF)||kbd_is_key_pressed(KEY_SHOOT_FULL)) && (conf.edge_overlay_lock!=1)) 
            {
			if (kbd_is_key_pressed(KEY_SHOOT_FULL) && !full_press)  //first full press
                  {
				shotTaken = 1 - shotTaken;                        //'1' after first shot
				memcpy(imgbuf,img,imgbuf_size);                   
				ymin = 0;
				inmem = 1;
				full_press = 1;                                   //to prevent multiple re-entry while full-pressed
				return;
			}
			if(shotTaken)                                           
                  {            
		       return;
			}

			memcpy(imgbuf,img,imgbuf_size);                           
			ymin = 0;
			inmem = 1;
			return;
		}

		else full_press = 0;                                           

		if (inmem && (ymin <180)) 
               {
			ymax = ymin + (180 / NSTAGES);                       
			if(ymax > 180) ymax = 180;
			for (y=ymin; y<ymax; y++) 
                     {
				ptrh1 = imgbuf + y * viewport_width + 7;       
				ptrh2 = imgbuf + y * viewport_width - 5;       
				ptrv1 = imgbuf + (y + 1) * viewport_width + 1; 
				ptrv2 = imgbuf + (y - 1) * viewport_width + 1; 
				optr = imgbuf + y * viewport_width + 3;        
				for (x=12; x<(screen_width- 4) * 3; x+=6)      
                            {
					h = ptrh1[x] - ptrh2[x];                
					if(h  < 0) h = -h;                      
					v = ptrv1[x] - ptrv2[x];
					if(v  < 0) v = -v;                      
					optr[x] = h + v;                        
					h = ptrh1[x + 3] - ptrh2[x + 3];        
					if(h  < 0) h = -h;                      
					v = ptrv1[x + 3] - ptrv2[x + 3];
					if(v  < 0) v = -v;                      
					optr[x + 2] = h + v;                    
				    }
			    }
			    ymin += 180 / NSTAGES;                          
			    return;
		    }
		if(inmem &&(ymin >= 180) && 
			((gui_get_mode() == GUI_MODE_NONE) || (gui_get_mode() == GUI_MODE_ALT)))
                     {

				for (y=0; y<180; y++)                          
                         {
					y1 = y + yoffset;                        
					if((y1 < 0) || (y1 >= 240))              
                              {

					}
					else 
                              {
						for (x=0; x < screen_width; x+=2)  
                                       {
							x1 = x;
							if((x1 < 12) || (x1 >= screen_width-13))      
                                            {
								if( need_redraw )
								{
								 draw_pixel(x, y, 0);                     
								}
							   }
							  else 
                                             {

								if(imgbuf[y * viewport_width + x1 * 3 + 3]  > thresh) 
								{
								 draw_pixel(x, y1, conf.edge_overlay_color );         
								}
								else if( need_redraw || (draw_get_pixel(x,y1) == conf.edge_overlay_color) )
								{
								 draw_pixel(x, y1, 0);                                
								}

							    }
						     } 
				        } 

				   if(shotTaken)
                             { 
                              draw_string(30, 10, strbuf, conf.osd_color,1);     //print 'Frozen' after first image
                              if(!two_shot)
                              {
                               two_shot=1;
                              }
                             }
			      } //_for
			      if( need_redraw )
			      {
				 --need_redraw;
			      }
			return;
		  }
	} 

	else {                             //not enabled for play mode
		full_press = 0;
		inmem = 0;
		shotTaken = 0;
		ymin = 30;
	     }
	return;
}


#else

//paint the edge overlay
void edge_overlay()
{
	static int imgmem = 0;
	static int ymin = 0;
	static int thresh;
	thresh = conf.edge_overlay_thresh;           //40
	static int xoffset = 0;
	static int yoffset = 0;
	static char strbuf[7] = "Frozen";
	static unsigned char *img;
	int i, hi, c;
	int x, y, h, v, ymax, y1, x1, y2;
	const char * ptrh1;
	const char * ptrh2;
	const char * ptrv1;
	const char * ptrv2;
	char xbuf[64];
	char * optr;
#if defined (CAMERA_sx200is)|| defined(CAMERA_ixus95)|| defined(CAMERA_sx130is)|| defined(CAMERA_sx150is)|| defined(CAMERA_sx160is)
      int screen_width=vid_get_viewport_width();
#elif defined (CAMERA_g11)||defined(CAMERA_g12)||defined(CAMERA_g15)|| defined(CAMERA_s90)|| defined(CAMERA_s95)||defined(CAMERA_s100)|| defined(CAMERA_sx40hs)|| defined(CAMERA_sx50hs)|| defined(CAMERA_ixus220)
      int screen_width=vid_get_viewport_width();
#endif
	is_saved = 0;                                //a new one, we could potentially save it
	if((mode_get()&MODE_MASK) != MODE_PLAY)
	{
		img = vid_get_viewport_fb();
	}
	else
	{
		img = vid_get_viewport_fb_d();
	}
	get_viewport_size();
      if(!imgbuf)allocate_imgbuf();
      imgbuf_end = imgbuf+imgbuf_size;
	if(imgbuf == 0)return;                     //ensure failed, make the best we can out of it
	if(conf.edge_overlay_play || ((mode_get()&MODE_MASK) != MODE_PLAY))  
       {
		//setup offsets for moving the edge overlay around. Always set
		//need_redraw so that we actually do a complete redraw, overwriting
		//also old pixels
		if ((kbd_is_key_pressed(KEY_SHOOT_HALF)||kbd_is_key_pressed(KEY_SHOOT_FULL)) && (conf.edge_overlay_lock!=1)) 
            {
			if (kbd_is_key_pressed(KEY_SHOOT_FULL) && !full_press)  //first full press
                  {
				shotTaken = 1 - shotTaken;                        //'1' after first shot
				memcpy(imgbuf,img,imgbuf_size);
				ymin = CALCYMARGIN;
				inmem = 1;
				full_press = 1;                                   //to prevent multiple re-entry while full-pressed
				xoffset = 0;
				yoffset = 0;
				return;
			}
			if(shotTaken)                                           
                  {            
		       return;
			}

			memcpy(imgbuf,img,imgbuf_size);                       
			ymin = CALCYMARGIN;
			inmem = 1;
			xoffset = 0;
			yoffset = 0;

			return;
		}
		else full_press = 0;                                           
		if (inmem && (ymin < screen_height-CALCYMARGIN)) 
               {
			ymax = ymin + (screen_height - 2 * CALCYMARGIN) / NSTAGES;
			if(ymax > screen_height - CALCYMARGIN) ymax = screen_height - CALCYMARGIN;
			for (y=ymin; y<ymax; y++) 
                     {
				ptrh1 = imgbuf + y * viewport_width + 7;     
				ptrh2 = imgbuf + y * viewport_width - 5;     
				ptrv1 = imgbuf + (y + 1) * viewport_width + 1; 
				ptrv2 = imgbuf + (y - 1) * viewport_width + 1; 
				optr = imgbuf + y * viewport_width + 3;        
				for (x=12; x<(screen_width- 4) * 3; x+=6)     
                            {
					h = ptrh1[x] - ptrh2[x];                
					if(h  < 0) h = -h;                      
					v = ptrv1[x] - ptrv2[x];
					if(v  < 0) v = -v;                      
					optr[x] = h + v;                        
					h = ptrh1[x + 3] - ptrh2[x + 3];        
					if(h  < 0) h = -h;                      
					v = ptrv1[x + 3] - ptrv2[x + 3];
					if(v  < 0) v = -v;                      
					optr[x + 2] = h + v;                    
				    }
			    }
			    ymin += (screen_height - 2 * CALCYMARGIN) / NSTAGES;
			    return;
		    }
		if(inmem &&(ymin >= screen_height-CALCYMARGIN) && 
			((gui_get_mode() == GUI_MODE_NONE) || (gui_get_mode() == GUI_MODE_ALT)))
                     {

				for (y=MARGIN; y<screen_height-MARGIN; y++) 
                         {
					y1 = y + yoffset;
					if((y1 < CALCYMARGIN) || (y1 >= screen_height - CALCYMARGIN)) 
                              {

					}
					else 
                              {
						for (x=MARGIN; x < screen_width - MARGIN; x+=2) 
                                       {
							x1 = x + xoffset;
							//leave a margin normally, only write to it when a
							//full redraw is requested
							if((x1 < 12) || (x1 >= screen_width-13)) 
                                            {
								if( need_redraw )
								{
								 draw_pixel(x, y, 0);
								}
							   }
							  else 
                                             {
								//draw a pixel if the threshold is reached. If
								//not, draw it transparent only if we want a
								//complete redraw to overwrite spurious pixels
								//or if the color of the existing pixel is the
								//same as the overlay color
								if(imgbuf[y1 * viewport_width + x1 * 3 + 3]  > thresh)
								{
								 draw_pixel(x, y, conf.edge_overlay_color );
								}
								else if( need_redraw || (draw_get_pixel(x,y) == conf.edge_overlay_color) )
								{
								 draw_pixel(x, y, 0);
								}

							    }
						     } 
				        } 

				       //disabled drawing the grid, the new way of drawing the
				       //overlay should leave the standard grid intact, allowing the
				       //custom grid to remain intact too.
				   if(shotTaken)
                             { 
                                draw_string(30, 10, (!conf.edge_overlay_lock)?"Frozen":"Locked",conf.osd_color,1);
                              if(!two_shot)
                              {
                               two_shot=1;
                              }
                             }
			      } //_for

			        //If a complete redraw was requested, decrement the request. That
			        //way we do it as much as it was requested, also in one run. Will
			        //cause some flickering, but better than nothing.
			      if( need_redraw )
			      {
				 --need_redraw;
			      }
			return;
		  }
	} 

	else {                             //not enabled for play mode
		full_press = 0;
		inmem = 0;
		shotTaken = 0;
		ymin = 0;
		xoffset = 0;
		yoffset = 0;
	     }
	return;

}

#endif

#else

#define EDGE_FILE_PREFIX "EDG_"
#define EDGE_FILE_FORMAT EDGE_FILE_PREFIX "%04d.edg"
#define EDGE_SLICES     2

typedef enum _edge_fsm_state
{
    EDGE_LIVE = 0,
    EDGE_FROZEN
} edge_fsm_state_t;

typedef struct _bit_vector
{
    int nBits;
    unsigned char* ptr;
    int ptrLen;
    int nElem;
} bit_vector_t;

static edge_fsm_state_t fsm_state = EDGE_LIVE;
static bit_vector_t* edgebuf = NULL;
static int xoffset = 0, yoffset = 0;
static unsigned char* smbuf = NULL;

static int slice = 0;           
static int slice_height;        

static int viewport_byte_width; 
static int viewport_yscale;     
static int viewport_height;     
static int viewport_width;      
static int viewport_xoffset;
static int viewport_yoffset;

static inline void bv_setbit(const bit_vector_t* bm, int pos, int val)
{
    int bp = pos - ((pos >> 3) << 3);
    if (val == 0)
        bm->ptr[pos >> 3] &= ~(1 << bp);
    else
        bm->ptr[pos >> 3] |=  (1 << bp);
}

static inline int bv_getbit(const bit_vector_t* bm, int pos)
{
    
    
    
    

    int bp = pos - ((pos >> 3) << 3);
    return (bm->ptr[pos >> 3] & (1 << bp)) >> bp;
}

int bv_get(const bit_vector_t* bm, int pos)
{
    int ret = 0;
    int i = bm->nBits - 1;
    int bitpos = pos * bm->nBits;
    do
    {
        ret |= (bv_getbit(bm, bitpos + i) << i);
    }while(--i >= 0);

    return ret;
}

bit_vector_t* bv_create(int len, int nbits)
{
    bit_vector_t* bm = (bit_vector_t*)malloc(sizeof(bit_vector_t));
    if (bm == NULL)
        return NULL;

    bm->ptrLen = len * nbits / 8 + 1;
    bm->nElem = len;
    bm->nBits = nbits;
    bm->ptr = malloc(bm->ptrLen);
    if (bm->ptr == NULL)
    {
        free(bm);
        return NULL;
    }

    return bm;
}

void bv_set(const bit_vector_t* bm, int pos, int val)
{
    int i = bm->nBits - 1;
    int bitpos = pos * bm->nBits;
    do
    {
        bv_setbit(bm, bitpos + i, val & (1<<i));
    }while(--i >= 0);
}

void bv_free(bit_vector_t* bm)
{
    if (bm)
    {
        if (bm->ptr)
        {
            free(bm->ptr);
            bm->ptr = NULL;
        }

        free(bm);
    }
}

static void get_viewport_size()
{
    viewport_height = vid_get_viewport_height(); //don't trace bottom lines
    viewport_width = vid_get_viewport_width();
    viewport_byte_width = vid_get_viewport_byte_width();
    viewport_yscale = vid_get_viewport_yscale();

	viewport_xoffset = vid_get_viewport_display_xoffset();
	viewport_yoffset = vid_get_viewport_display_yoffset();

    slice_height = viewport_height / EDGE_SLICES;
}

static void ensure_allocate_imagebuffer()
{
    if (edgebuf == NULL)
    {
        edgebuf = bv_create(viewport_height * viewport_width, 1);
        if (edgebuf != NULL)
            memset(edgebuf->ptr, 0, edgebuf->ptrLen);
    }
}

static void reset_edge_overlay()
{
    if (smbuf != NULL)
    {
        free(smbuf);
        smbuf = NULL;
    }

    if (edgebuf != NULL)
    {
   
        bv_free(edgebuf);
        edgebuf = NULL;
    }

    fsm_state = EDGE_LIVE;
    slice = 0;

    
  
}

static int is_buffer_ready()
{
    if (edgebuf == NULL) return 0;
    if (edgebuf->ptr == NULL) return 0; 
    return 1;
}

static int get_edge_file_num(const char* fn)
{
    int num = 0;
    if( strncmp(fn,EDGE_FILE_PREFIX,sizeof(EDGE_FILE_PREFIX)-1) == 0 )
    {
        
        fn += sizeof(EDGE_FILE_PREFIX);
        while( *fn == '0' ) 
        {
            ++fn;
        }
        while( isdigit(*fn) )
        {
            num *= 10;
            num += *fn - '0';
            ++fn;
        }
        
    }
    return num;
}

void save_edge_overlay(void)
{

    char fn[64];
    char msg[64];
    FILE *fd;
    DIR* d;
    int fnum = 0;
    int fr = 0;
    int zoom = 0;
    struct dirent* de;
    static struct utimbuf t;
    

    if( !is_buffer_ready() )
    {
        draw_string(0, 0, "No overlay to save.", conf.osd_color,1);
        return;
    }

    zoom = shooting_get_zoom();

    
    d = opendir(EDGE_SAVE_DIR);
    if( ! d )
    {
        return;
    }

    while( (de = readdir(d)) )
    {
        fr = get_edge_file_num(de->name);
        if( fr > fnum )
        {
            fnum = fr;
        }
    }
    ++fnum; 
    get_viewport_size();
    
    sprintf(fn, EDGE_SAVE_DIR "/" EDGE_FILE_FORMAT, fnum );
    fd = fopen(fn, "wb");
    if(fd !=NULL)
    {
        
        fwrite(edgebuf->ptr,edgebuf->ptrLen,1,fd);
        fwrite(&zoom,sizeof(zoom),1,fd);
        fclose(fd);
        t.actime = t.modtime = time(NULL);
        utime(fn, &t);
        sprintf(msg, "Saved as %s",fn);
        draw_string(0, 0, msg, conf.osd_color,1);
    }
    closedir(d);
}

void load_edge_overlay(const char* fn)
{
    FILE *fd;
    int zoom;

    get_viewport_size();
    ensure_allocate_imagebuffer( );
    fd = fopen(fn,"rb");
    if( fd != NULL )
    {
        int ret = fread(edgebuf->ptr,edgebuf->ptrLen,1,fd);
        int ret2 = fread (&zoom,sizeof(zoom),1,fd);
        fclose(fd);
        if( (ret == 1) && (ret2 == 1) )
        {
            fsm_state = EDGE_FROZEN;    
            if (conf.edge_overlay_zoom)
            {
                shooting_set_zoom(zoom);
            }
        }
    }
}

static void average_filter_row(const unsigned char* ptrh1,  
                               unsigned char* smptr,        
                               int x, int x_max)
{
    const unsigned char* ptrh2 = ptrh1 + viewport_byte_width*viewport_yscale;  
    const unsigned char* ptrh3 = ptrh2 + viewport_byte_width*viewport_yscale;  

    for (; x<x_max; x+=6)
    {
        *(smptr + x + 1) = (*(ptrh1 + x - 1) +
                            *(ptrh1 + x + 1) +
                            *(ptrh1 + x + 3) +

                            *(ptrh2 + x - 1) +
                            *(ptrh2 + x + 1) +
                            *(ptrh2 + x + 3) +

                            *(ptrh3 + x - 1) +
                            *(ptrh3 + x + 1) +
                            *(ptrh3 + x + 3)) / 9u;

        *(smptr + x + 3) = (*(ptrh1 + x + 1) +
                            *(ptrh1 + x + 3) +
                            *(ptrh1 + x + 4) +

                            *(ptrh2 + x + 1) +
                            *(ptrh2 + x + 3) +
                            *(ptrh2 + x + 4) +

                            *(ptrh3 + x + 1) +
                            *(ptrh3 + x + 3) +
                            *(ptrh3 + x + 4)) / 9u;

        *(smptr + x + 4) = (*(ptrh1 + x + 3) +
                            *(ptrh1 + x + 4) +
                            *(ptrh1 + x + 5) +

                            *(ptrh2 + x + 3) +
                            *(ptrh2 + x + 4) +
                            *(ptrh2 + x + 5) +

                            *(ptrh3 + x + 3) +
                            *(ptrh3 + x + 4) +
                            *(ptrh3 + x + 5)) / 9u;

        *(smptr + x + 5) = (*(ptrh1 + x + 4) +
                            *(ptrh1 + x + 5) +
                            *(ptrh1 + x + 7) +

                            *(ptrh2 + x + 4) +
                            *(ptrh2 + x + 5) +
                            *(ptrh2 + x + 7) +

                            *(ptrh3 + x + 4) +
                            *(ptrh3 + x + 5) +
                            *(ptrh3 + x + 7)) / 9u;
    }

    
    smptr[x+1] = smptr[x-5];
    smptr[x+3] = smptr[x-3];
    smptr[x+4] = smptr[x-2];
    smptr[x+5] = smptr[x-1];
}

static int calc_edge_overlay()
{
    int shutter_fullpress = kbd_is_key_pressed(KEY_SHOOT_FULL);

    const unsigned char* img = vid_get_viewport_active_buffer();
    const unsigned char*  ptrh1 = NULL;    
    const unsigned char*  ptrh2 = NULL;    
    const unsigned char*  ptrh3 = NULL;    
    unsigned char*  smptr = NULL;    
    int x, y, xdiv3;
    int conv1, conv2;

    const int y_min = slice   *slice_height;
    const int y_max = (slice+1)*slice_height;
    const int x_min = 6;
    const int x_max = (viewport_width - 2) * 3;

    img += vid_get_viewport_image_offset();	

    xoffset = 0;
    yoffset = 0;

    
    ensure_allocate_imagebuffer();
    if( !is_buffer_ready() ) return 0;


    for (y = y_min; y < y_max; ++y)
    {
        shutter_fullpress |= kbd_is_key_pressed(KEY_SHOOT_FULL);

        ptrh1 = img + (y-1) * viewport_byte_width*viewport_yscale;
        ptrh2 = ptrh1 + viewport_byte_width*viewport_yscale;
        ptrh3 = ptrh2 + viewport_byte_width*viewport_yscale;

        

        for (x = x_min, xdiv3 = x_min/3; x < x_max; x += 6, xdiv3 += 2)
        {
            
            conv1 = *(ptrh1 + x + 1) * ( 1) +
                    *(ptrh1 + x + 4) * (-1) +

                    *(ptrh2 + x + 1) * ( 2) +
                    *(ptrh2 + x + 4) * (-2) +

                    *(ptrh3 + x + 1) * ( 1) +
                    *(ptrh3 + x + 4) * (-1);
            if  (conv1 < 0)     
                conv1 = -conv1;

            
            conv2 = *(ptrh1 + x + 1) * ( 1) +
                    *(ptrh1 + x + 3) * ( 2) +
                    *(ptrh1 + x + 4) * ( 1) +

                    *(ptrh3 + x + 1) * (-1) +
                    *(ptrh3 + x + 3) * (-2) +
                    *(ptrh3 + x + 4) * (-1);
            if  (conv2 < 0)     
                conv2 = -conv2;

            if (conv1 + conv2 > conf.edge_overlay_thresh)
            {
                bv_set(edgebuf, y + xdiv3, 1);
            }

            

            
            conv1 = *(ptrh1 + x + 5) * ( 1) +
                    *(ptrh1 + x + 9) * (-1) +

                    *(ptrh2 + x + 5) * ( 2) +
                    *(ptrh2 + x + 9) * (-2) +

                    *(ptrh3 + x + 5) * ( 1) +
                    *(ptrh3 + x + 9) * (-1);
            if  (conv1 < 0)     
                conv1 = -conv1;

            
            conv2 = *(ptrh1 + x + 5) * ( 1) +
                    *(ptrh1 + x + 7) * ( 2) +
                    *(ptrh1 + x + 9) * ( 1) +

                    *(ptrh3 + x + 5) * (-1) +
                    *(ptrh3 + x + 7) * (-2) +
                    *(ptrh3 + x + 9) * (-1);
            if  (conv2 < 0)     
                conv2 = -conv2;

            if (conv1 + conv2 > conf.edge_overlay_thresh)
            {
                bv_set(edgebuf, y*viewport_width + xdiv3+1, 1);
            }
        }   
    }   
    return shutter_fullpress;
}

static int draw_edge_overlay()
{
    int shutter_fullpress = kbd_is_key_pressed(KEY_SHOOT_FULL);

    int x, y;
    int x_off, y_off;

    const color cl = conf.edge_overlay_color;
    const int y_slice_min = slice   *slice_height;
    const int y_slice_max = (slice+1)*slice_height;
    const int y_min = 0;
    const int y_max = viewport_height;
    const int x_min = 2;
    const int x_max = (viewport_width - 2);

    if( !is_buffer_ready() ) return 0;

    for (y = y_slice_min; y < y_slice_max; ++y)
    {
        y_off = y + yoffset;
        
        shutter_fullpress |= kbd_is_key_pressed(KEY_SHOOT_FULL);

        if ((unsigned)(y_off-y_min) < (y_max-y_min)) 
        {
            const int y_edgebuf = (y-y_min) * viewport_width;

            for (x = x_min; x < x_max; ++x)
            {
                x_off = x + xoffset;

                if ((unsigned)(x_off-x_min) < (x_max-x_min)) 
                {
                    
                    
                    
                    const int bEdge = bv_get(edgebuf, y_edgebuf + x);
                    const int bDraw = bEdge || (draw_get_pixel(x_off+viewport_xoffset, y_off+viewport_yoffset) == conf.edge_overlay_color);
                    const color cl = bEdge ? conf.edge_overlay_color : 0;
                    if (bEdge || bDraw)
                        draw_pixel(x_off+viewport_xoffset, y_off+viewport_yoffset, cl);
                    
                }
            }   
        }
    }   

    
    
    

    if (xoffset != 0)
    {
        

        const int x_min_c = (xoffset < 0) ? x_max + xoffset : x_min;
        const int x_max_c = (xoffset > 0) ? x_min + xoffset : x_max;

        for (y = y_min; y < y_max; ++y)
        {
            for (x = x_min_c; x < x_max_c; ++x)
            {
                
                if (draw_get_pixel(x+viewport_xoffset, y+viewport_yoffset) == cl)
                    draw_pixel(x+viewport_xoffset, y+viewport_yoffset, 0 );
            }
        }
    }

    if (yoffset != 0)
    {
        

        const int y_min_c = (yoffset < 0) ? y_max + yoffset : y_min;
        const int y_max_c = (yoffset > 0) ? y_min + yoffset : y_max;

        for (y = y_min_c; y < y_max_c; ++y)
        {
            for (x = x_min; x < x_max; ++x)
            {
                
                if (draw_get_pixel(x+viewport_xoffset, y+viewport_yoffset) == cl)
                    draw_pixel(x+viewport_xoffset, y+viewport_yoffset, 0 );
            }
        }
    }

    return shutter_fullpress;
}

static void set_offset_from_overlap()
{
    const int y_max = viewport_height;
    const int x_max = (viewport_width - 2);
}


void edge_overlay()
{
	static int shotTaken = 0;
	static int imgmem = 0;
	static int ymin = 0;
     	if(conf.edge_overlay_play || ((mode_get()&MODE_MASK) != MODE_PLAY))  
       {
        get_viewport_size();
		if ((kbd_is_key_pressed(KEY_SHOOT_HALF)||kbd_is_key_pressed(KEY_SHOOT_FULL)) && (conf.edge_overlay_lock!=1)) 
            {
             if (kbd_is_key_pressed(KEY_SHOOT_FULL) && !full_press)       //first full press
                  {
				shotTaken = 1 - shotTaken;                        //'1' after first shot
				for (slice = 0; slice < EDGE_SLICES;slice++)
                           calc_edge_overlay();
				ymin = 0;
				inmem = 1;
				full_press = 1;                                   //to prevent multiple re-entry while full-pressed
				return;
			}
			if(shotTaken)                                           
                  {            
		       return;
			}                
			ymin = 0;
                  for (slice = 0; slice < EDGE_SLICES;slice++)
                   calc_edge_overlay();
			inmem = 1;
			return;
		  }
          
		else full_press = 0;

           if (inmem)
            {
		 for (slice = 0; slice < EDGE_SLICES;slice++)
               draw_edge_overlay();
            }
         } 
	else {                             //not enabled for play mode
		full_press = 0;
		inmem = 0;
		shotTaken = 0;
		ymin = 0;
		xoffset = 0;
		yoffset = 0;
	     }
	return;

}   

#endif

