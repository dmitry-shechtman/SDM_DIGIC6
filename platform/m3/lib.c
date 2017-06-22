#include "platform.h"
#include "lolevel.h"

static char* frame_buffer[2];

static char* frame_buffer[2];

void switch_mode(int mode)
{
    if ( mode == 0 ) 
    {
     _Rec2PB();
     _set_control_event(0x80000000|CAM_USB_EVENTID); // 0x10A5 ConnectUSBCable
    } 
    else if ( mode == 1 ) 
    {
     _set_control_event(CAM_USB_EVENTID); // 0x10A6 DisconnectUSBCable
     _PB2Rec();
    } 
}
 
long vid_get_bitmap_width(){ return 360; }
long vid_get_bitmap_screen_width() { return 360; }
long vid_get_bitmap_height(){ return 240; }
long vid_get_bitmap_screen_height() { return 240; }
long vid_get_bitmap_buffer_width() { return 736; }
long vid_get_bitmap_buffer_height() { return 480; }

void vid_bitmap_refresh() 
{
    extern void _displaybusyonscreen();
    extern void _undisplaybusyonscreen();
  // clears perfectly but blinks and is asynchronous
    _displaybusyonscreen();
    _undisplaybusyonscreen();
}

void shutdown() {
    while(1)   _SleepTask(500);
}

#define LED_PR 0xd20b0994 // green LED on the back

void debug_led(int state) {
    volatile long *p = (void*)LED_PR;
    *p = ((state) ? 0x4d0002 : 0x4c0003);
}

void ubasic_set_led(int led, int state, int bright) 
{
 static char led_table[7]={0,0,0,0,0,4,0};
 _LEDDrive(led_table[(led-4)%sizeof(led_table)], state<=1 ? !state : state);
} 

void shutdown_soft() {
//	*(int*)0xd20b0810  = 0x4d0002;	 // Orange Led = on

	_PostLogicalEventForNotPowerType(0x1005,0);
}

int get_flash_params_count(void) {
    return 0x91;
}

void JogDial_CW(void) {
    _PostLogicalEventToUI(0x872, 1);    //RotateJogDialRight
}

void JogDial_CCW(void) {
    _PostLogicalEventToUI(0x873, 1);    //RotateJogDialLeft
}

extern  int     active_raw_buffer;
extern  char*   raw_buffers[];

char *hook_raw_image_addr()
{
    return raw_buffers[(active_raw_buffer&1)];
}

char *hook_alt_raw_image_addr()
{
    return raw_buffers[((active_raw_buffer&1)^1)];
}

extern char active_viewport_buffer;
extern void* viewport_buffers[];

void *vid_get_viewport_fb() {
    // Return first viewport buffer - for case when vid_get_viewport_live_fb not defined
    //return (void*)0x43115100; // uyvy buffers with signed(?) chroma components
    return (void*)0x4367AF00; //0x4b25fc00; uyvy buffers (more than 4), pixel format compatible with earlier DIGIC uyvy
}

void *vid_get_viewport_live_fb() {
	return (void*)0x4367AF00;   
   //return 0; //TODO
}

int vid_get_viewport_width() {
/*
loc_fc134980: ; 4 refs (GetVRAMHPixelsSize)
fc134980:   487d        ldr r0, [pc, #500]  ; 0xfc134b78: (0002ca18) 
fc134982:   f8d0 00ac   ldr.w   r0, [r0, #172]  ; 0xac
fc134986:   4770        bx  lr
*/
    extern int _GetVRAMHPixelsSize();
// TODO: this is the actual width, pixel format is uyvy (16bpp)
    return _GetVRAMHPixelsSize() ;
}

long vid_get_viewport_height() {
/*
loc_fc134988: ; 3 refs (GetVRAMVPixelsSize)
fc134988:   487b        ldr r0, [pc, #492]  ; 0xfc134b78: (0002ca18) 
fc13498a:   f8d0 00b0   ldr.w   r0, [r0, #176]  ; 0xb0
fc13498e:   4770        bx  lr
*/
    extern int _GetVRAMVPixelsSize();
// return half height
    return _GetVRAMVPixelsSize();
}

// Y multiplier for cameras with 480 pixel high viewports (CHDK code assumes 240)
int vid_get_viewport_yscale() {
    return 1; //2
}

int vid_get_viewport_yoffset() {
    // this seems to be always 0, buffer always begins with actual display data (widescreen or not)
    return 0;
}

int vid_get_viewport_display_xoffset() {
    return 0;
}

int vid_get_viewport_display_yoffset() {
    return 0;
}

extern int active_bitmap_buffer;
extern char* bitmap_buffer[];

void *vid_get_bitmap_fb() {
    // Return first bitmap buffer address
    return bitmap_buffer[0];
}

// Functions for PTP Live View system
int vid_get_viewport_display_xoffset_proper()   { return vid_get_viewport_display_xoffset() * 2; }
int vid_get_viewport_display_yoffset_proper()   { return vid_get_viewport_display_yoffset() * 2; }
int vid_get_viewport_width_proper()             { return vid_get_viewport_width(); }
int vid_get_viewport_buffer_width_proper()		{ return (16 + vid_get_viewport_width() ); }

int vid_get_viewport_height_proper()            { return vid_get_viewport_height() ; } // *2
int vid_get_viewport_fullscreen_height()        { return 480; }
int vid_get_palette_type()                      { return -1; } //3
//int vid_get_palette_size()                      { return 256 * 4 + 128; }
int vid_get_palette_size()                      { return 0; }
int vid_get_aspect_ratio()                    	{ return 0; }
int vid_get_viewport_byte_width() 				{ return 736 * 2; }


void *vid_get_bitmap_active_buffer() {
    return bitmap_buffer[active_bitmap_buffer&1];
}

volatile char *opacity_buffer[2] = {(char*)0x41741000, (void*)0x41801000};


void *vid_get_bitmap_active_stencil() { // no longer needed?
    if (active_bitmap_buffer&1) {
        return (void*)0x41801000;
    }
    else {
        return (void*)0x41741000;
		
    }
}

void *vid_get_bitmap_active_palette() {
    return (void*)0x8000; // just to return something valid, no palette needed on this cam
}

#if 0
#ifdef CAM_SUPPORT_BITMAP_RES_CHANGE
/*
 * needed because bitmap buffer resolutions change when an external display is used
 * an extra screen erase doesn't seem to be needed
 */
void update_screen_dimensions() {
    // see sub_fc18618a in 102b and 102c for the values
    extern int displaytype;
    static int old_displaytype = -1;

   /*  if (old_displaytype == displaytype) {
        return;
    }
    old_displaytype = displaytype; */
//*(int*)0xd20b0810  = 0x4d0002;	 // Orange Led = on
    switch(displaytype) {
        case 0:
        case 3:
        case 4:
        case 5:
		case 10:
            // lcd
			
            camera_screen.width = 720;
			camera_screen.physical_width = camera_screen.buffer_width = 736;
            camera_screen.height = camera_screen.buffer_height = 480;
            camera_screen.size = camera_screen.buffer_size = 736*480;
            break;
        case 1:
        case 2:
        case 8:
        case 9:
            // tv-out
            camera_screen.width = 736;
            camera_screen.physical_width = camera_screen.buffer_width = 736;
            camera_screen.height = camera_screen.buffer_height = 480;
            camera_screen.size = 736*480;
            camera_screen.buffer_size = 736*480;
            break;
        case 6:
        case 7:
            // hdmi
            camera_screen.width = camera_screen.physical_width = camera_screen.buffer_width = 960;
            camera_screen.height = camera_screen.buffer_height = 540;
            camera_screen.size = camera_screen.buffer_size = 960*540;
            break;
    }
}
#endif
#endif


