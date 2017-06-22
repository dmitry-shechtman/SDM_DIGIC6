#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BVBUFFSIZE     100	// brightness buffer
#define KEY_UP		 1
#define KEY_DOWN	       2
#define KEY_LEFT	       3
#define KEY_RIGHT	       4
#define KEY_SET		 5
#define KEY_SHOOT_HALF	 6
#define KEY_SHOOT_FULL	 7
#define KEY_ZOOM_IN	 8
#define KEY_ZOOM_OUT	 9
#define KEY_MENU	      10
#define KEY_DISPLAY	11
#define KEY_PRINT	      12     //S-series: Shortcut
#define KEY_ERASE	      13     //S-series: Func
#define KEY_ISO		14     //S-series
#define KEY_FLASH	      15     //S-series
#define KEY_MF		16     //S-series
#define KEY_MACRO	      17     //S-series
#define KEY_VIDEO	      18     //S-series
#define KEY_TIMER	      19     //S-series
#define KEY_EXPO_CORR	20     //G-series
#define KEY_MICROPHONE	21
#define KEY_FACE        22     //SX100IS
#define KEY_DUMMY	      23     //dummy key for pressing to disable energy saving in alt mode
#define KEY_ZOOM_IN1	24	// SX30 has three zoom speeds each way
#define KEY_ZOOM_IN2	25
#define KEY_ZOOM_IN3	26
#define KEY_ZOOM_OUT1	27
#define KEY_ZOOM_OUT2	28
#define KEY_ZOOM_OUT3	29
#define KEY_ZOOM_ASSIST	35	// SX30 new button
#define KEY_AE_LOCK     36	// G12 AE/FE Lock button
#define KEY_METERING	37	// G12 metering mode button
#define KEY_PLAYBACK    38 
#define KEY_LEFT_SOFT   39    // jogdial keys have two steps 
#define KEY_RIGHT_SOFT  40 
#define KEY_UP_SOFT     41 
#define KEY_DOWN_SOFT   42  
#define KEY_SHOOT_FULL_ONLY 43
#define KEY_POWER           44
//A810/A1300/A2300/A2400/A4000
#define KEY_HELP            45
#define KEY_RING_FUNC   46
// dedicated wifi connect button (G7x)
#define KEY_WIFI            47

#define KEY_ZOOM_IN_SLOW 24
#define KEY_ZOOM_OUT_SLOW 25


#define MOVIE_MODE      30     // to override mode dial
#define AUTO_MODE	      31
#define MANUAL_MODE	32
#define TV_MODE	      33
#define PLAYBACK_MODE	34     // to override mode dial


#define JOGDIAL_LEFT   100
#define JOGDIAL_RIGHT  101



struct                // globally accessible structure
{
 int start_delay;     // start delay in seconds
 int shoot_interval;  // in seconds between each shot/sequence
 int num_exposures;   // 1 to ?
 int endless_mode;
 int shoot_mode;      // single,timer, continuous
 int bkt_exposures;    // number of Tv or focus bracketing exposures in continuous mode
 int focus_tv_exposures;  // number of Tv exposures in combined focus/Tv continuous mode
 int bracket_type;    // Tv or focus
 int exposure_mode;   // lighter, darker, alternate in timer mode
 int ev_steps;        // number of 1/3 EV steps
 int sun;             // 'sunrise/set' mode in time-lapse (no external pulses needed)if non-zero
 int suncsv;          // save data as text or csv (luminance values)
 int reserved;        // number of times to repeat a particulat Tv96 exposure
 int focus_mode;      // off,equal-step,auto,digiscope
 int focus_step;
 int focus_reference; 
 int blank_screen;     // restore when script finished
 int shutdown;         // VxWorks cameras only ?
 long target_time;
 int led_time;
 int current_zoom;     // save digiscope zoom setting
 int shot_count;
 int running;          // time-lapse script running 
 int ss;               // stack-saved flag.
} tl;

struct                // start process at this time
{
 int hr;
 int min;
}st_time;

struct                // finish process at this time
{
 int hr;
 int min;
}ft_time;

struct               // put-off values for minimum shutter-speed uBasic function
{
  int tv;
  int iso;
  int nd;
}mss_putoff;
       
int usbup_time;       // time that USB upload is enabled for
extern int gLowLightCount;
extern int stack_saved,bracket_running,tl_mod;
extern int srf;         // sunrise sequence finished (used to block further USB pulses)
extern int st_tv_exp;  // hdr focus stack
extern int st_tv_ev;   // hdr focus stack
extern void gui_kbd_process();
extern void gui_kbd_enter();
extern void gui_kbd_leave();
extern void init_tl(int a,int b,int c,int d,int e,int f,int g,int h,int i,int j,int k,int l,int m,int n,int p,int q,int r,int s,int t, int u);
extern void ubasic_su(int hr, int min);
extern int su();
extern int baflf();
extern void ubasic_baflf(int duration);
extern void ubasic_hdrfs();
int get_soft_half_press();
extern unsigned char gUpPressed,gDownPressed,gLeftPressed,gRightPressed,gSetPressed,gDispPressed,gHalfPressed,gFullPressed;
#if defined(KEY_POWER)
extern unsigned char gPowerPressed;
#endif

#endif

