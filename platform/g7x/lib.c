#include "platform.h"
#include "lolevel.h"

extern unsigned char clearScreen,gPlayRecMode;
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

long hook_raw_size()             { return CAM_RAW_ROWPIX*CAM_RAW_ROWS*CAM_SENSOR_BITS_PER_PIXEL/8; }

extern int state_kbd_script_run; 
void vid_bitmap_refresh() 
{
 if(gPlayRecMode<2)
{
 if(state_kbd_script_run ||!gPlayRecMode ||is_video_recording())
 {
  extern void _displaybusyonscreen();
  extern void _undisplaybusyonscreen();   
  // clears perfectly but blinks and is asynchronous
  _displaybusyonscreen();
  _undisplaybusyonscreen();
 }
 else clearScreen=1;
 }
}

void shutdown() {
//***TODO***
/*
    volatile long *p = (void*)0xC022001C;

    asm(
        "MRS     R1, CPSR\n"
        "AND     R0, R1, #0x80\n"
        "ORR     R1, R1, #0x80\n"
        "MSR     CPSR_cf, R1\n"
        :::"r1","r0"
    );

    *p = 0x44;  // power off.
*/
    while(1);
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
    _PostLogicalEventForNotPowerType(0x1005,0); // PressOffButton
}

int get_flash_params_count(void) {
    return 0xde; // @0xfc0afeb8
}

void JogDial_CW(void) {
    _PostLogicalEventToUI(0x872, 1);    //RotateJogDialRight
}

void JogDial_CCW(void) {
    _PostLogicalEventToUI(0x873, 1);    //RotateJogDialLeft
}

extern  int     active_raw_buffer;
extern  char*   raw_buffers_canon_raw[];
extern  char*   raw_buffers_jpeg[];

char *hook_raw_image_addr()
{
    // observed values 0-2, 3 would index something that doesn't look like a raw fb in the jpeg case
    int i=active_raw_buffer&3;
    if(i>2) {
        i=0;
    }
    int vmode;
    get_property_case(PROPCASE_SHOOTING_MODE, &vmode, 4);
    if(shooting_mode_canon2chdk(vmode) == MODE_AUTO) 
     {
        // AUTO mode (canon raw can't be enabled in AUTO)
        return (char *)0x46f04300; // TODO unclear if this is only buffer, or if used in all AUTO sub modes
    }else if(shooting_get_prop(PROPCASE_IMAGE_FORMAT) == 1) {
    // canon raw disabled - uses up to 3 raw buffers
        return raw_buffers_jpeg[i];
    } else {
        // canon raw enabled - different address, not clear if it ever uses multiple buffers
        return raw_buffers_canon_raw[i];
    }
    // TODO most scene modes seem to use different addresse(s)
}
 
extern char active_viewport_buffer;
extern void* viewport_buffers[];

void *vid_get_viewport_fb() {
    return (void*)0x43334300; // "first" viewport adr, "VRAM Address  : %p", contains d6 uyvy
// TODO sx280 values
    // Return first viewport buffer - for case when vid_get_viewport_live_fb not defined
    //return (void*)0x43115100; // uyvy buffers with signed(?) chroma components
    // return (void*)0x4b25fc00; // uyvy buffers (more than 4), pixel format compatible with earlier DIGIC uyvy
}
void *vid_get_viewport_fb_d()    { return (void*)0x43334300; } // TODO

void *vid_get_viewport_live_fb() {
    return 0; //TODO
}

int vid_get_viewport_width() {
    extern int _GetVRAMHPixelsSize();
// TODO: this is the actual width, pixel format is uyvy (16bpp)
    return _GetVRAMHPixelsSize();
}

long vid_get_viewport_height() {
    extern int _GetVRAMVPixelsSize();
// TODO return half height?
    return _GetVRAMVPixelsSize();
}

// Y multiplier for cameras with 480 pixel high viewports (CHDK code assumes 240)
int vid_get_viewport_yscale() {
    return 2;
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
    return (void *)0x41441000; // from sub_fc0f7c21, alt 4153e200
}

// TODO
// Functions for PTP Live View system
int vid_get_viewport_display_xoffset_proper()   { return vid_get_viewport_display_xoffset() * 2; }
int vid_get_viewport_display_yoffset_proper()   { return vid_get_viewport_display_yoffset() * 2; }
int vid_get_viewport_width_proper()             { return vid_get_viewport_width(); }
int vid_get_viewport_height_proper()            { return vid_get_viewport_height() * 2; }
int vid_get_viewport_fullscreen_height()        { return 480; }
int vid_get_palette_type()                      { return 3; }
int vid_get_palette_size()                      { return 256 * 4; }

void *vid_get_bitmap_active_buffer() {
    return bitmap_buffer[active_bitmap_buffer&1];
}

// the opacity buffer defines opacity for the bitmap overlay's pixels
// found near BmpDDev.c line 215 assert fc0f7b58
volatile char *opacity_buffer[2] = {(char*)0x4163b400, (void*)0x416b9d00};

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

    if (old_displaytype == displaytype) {
        return;
    }
    old_displaytype = displaytype;

    switch(displaytype) {
        case 0:
        case 3:
        case 4:
        case 5:
            // lcd
            camera_screen.width = camera_screen.physical_width = camera_screen.buffer_width = 640;
            camera_screen.height = camera_screen.buffer_height = 480;
            camera_screen.size = camera_screen.buffer_size = 640*480;
            break;
        case 1:
        case 2:
        case 8:
        case 9:
            // tv-out
            camera_screen.width = 720;
            camera_screen.physical_width = camera_screen.buffer_width = 736;
            camera_screen.height = camera_screen.buffer_height = 480;
            camera_screen.size = 720*480;
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

char *camera_jpeg_count_str()
{
    extern char jpeg_count_str[];
	return jpeg_count_str;
}
