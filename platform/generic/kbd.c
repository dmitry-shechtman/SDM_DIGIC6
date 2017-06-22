#include "lolevel.h"
#include "platform.h"
#include "conf.h"
#include "core.h"
#include "keyboard.h"
#include "..\core\draw_palette.h"
#include "..\core\gui_osd.h"

typedef struct 
{
	short fun;      //1 = permanent, 2 = toggle
	short hackkey;
	long canonkey0; 
	long canonkey1;
	long canonkey2;
} SwitchMap;

static long switch_mod_state[3]; 
static SwitchMap switchmap[];    
static int switch_override = 0;  
int pw; 

volatile long *mmio0 = (void*)0xc0220200; 
volatile long *mmio1 = (void*)0xc0220204;
volatile long *mmio2 = (void*)0xc0220208;
volatile long *mmio3 = (void*)0xc022020C;
 
#define DELAY_TIMEOUT 10000

typedef struct 
{
 long hackkey;
 long canonkey;
} KeyMap;

extern int inf_focus;
extern char *imgbuf; 
extern int block_script,ub,sertx;

#if defined(CAMERA_a700)
static long menu_key_mask=0x00000400;
#else
static long menu_key_mask=0x00000200;
#endif
static long kbd_new_state[3];
static long kbd_prev_state[3];
static long kbd_mod_state;
static KeyMap keymap[];
static long last_kbd_key = 0;
static int usb_power=0;
static int shoot_counter=0;
static int remote_key=0;
static int remote_count=0;
#define NEW_SS (0x2000)
#define SD_READONLY_FLAG (0x20000)

#if defined(CAMERA_a530) || defined(CAMERA_a540)
#define USB_MASK 0x4000
#define USB_IDX 2
#endif

// USB support for IXUS800 suggested by nirschi in CHDK forum AllBest Builds

#if defined(CAMERA_a610) || defined(CAMERA_a620) || defined(CAMERA_a630) || defined(CAMERA_a640) || defined(CAMERA_ixus65) || defined(CAMERA_ixus800) 
#define USB_MASK 0x8000000
#define USB_IDX 1
#endif

#if defined(CAMERA_a700)|| defined(CAMERA_a710)
#define USB_MASK 8
#define USB_IDX 0
#endif

#define SWITCH_MASK0 (0x00000000)
#define SWITCH_MASK1 (0xf0000000) 
#define SWITCH_MASK2 (0x00000000)

static SwitchMap switchmap[] = 
 {
        { 1, MOVIE_MODE, 0x00000000, 0xb0000000, 0x00000000 }, 
#if !defined(CAMERA_a540)
        { 1, PLAYBACK_MODE, 0x00000000, 0x00000000, 0x00002000 }, 
#else
        { 1, PLAYBACK_MODE, 0x00000000, 0x00000000, 0x00000400 }, 
#endif
	  { 0, 0, 0, 0, 0 }
 };


long get_mmio(void)
{
long x;	

#if defined(CAMERA_a530) || defined(CAMERA_a540)
    x = (long)*mmio2;
#endif
	
#if defined(CAMERA_a610) || defined(CAMERA_a620) || defined(CAMERA_a630) || defined(CAMERA_a640) || defined(CAMERA_ixus800) 
        x = (long)*mmio1;
#endif

#if defined(CAMERA_a700) || defined(CAMERA_a710)
        x = (long)*mmio0;
#endif
	
return x;	
}

#define LED_AF 0xc0220080
#define LED_PR 0xc0220084

void sio_led(int state)
   {
    if(!conf.use_af_led)debug_led(state); 
    else 
     {
      if(state) ubasic_set_led(9,1,100); 
      else ubasic_set_led(9,0,0); 
     }
   }

#if defined(CAMERA_a530) || defined(CAMERA_a540) || defined(CAMERA_a610) || defined(CAMERA_a620) || defined(CAMERA_a630) || defined(CAMERA_a640) || defined(CAMERA_a700)|| defined(CAMERA_a710) || defined(CAMERA_ixus60) || defined (CAMERA_ixus800)

#include "wurb1.c"

#endif

#ifndef MALLOCD_STACK
static char kbd_stack[NEW_SS];
#endif

long __attribute__((naked)) wrap_kbd_p1_f() ;

static void __attribute__((noinline)) mykbd_task_proceed()
{
    while (physw_run){
	_SleepTask(10);                                      //4->11ms, 12->22ms, i.e. quantised to multiples of 11ms. (?)
	if (wrap_kbd_p1_f() == 1){ // autorepeat ?
	    _kbd_p2_f();
	}
    }
}

void __attribute__((naked,noinline)) mykbd_task()
{
    /*WARNING
     * Stack pointer manipulation performed here!
     * This means (but not limited to):
     *	function arguments destroyed;
     *	function CAN NOT return properly;
     *	MUST NOT call or use stack variables before stack
     *	is setup properly;
     *
     */

    register int i;
    register long *newstack;

#ifndef MALLOCD_STACK
    newstack = (void*)kbd_stack;
#else
    newstack = malloc(NEW_SS);
#endif

    for (i=0;i<NEW_SS/4;i++)
	newstack[i]=0xdededede;

    asm volatile (
	"MOV	SP, %0"
	:: "r"(((char*)newstack)+NEW_SS)
	: "memory"
    );

    mykbd_task_proceed();
    _ExitTask();
}


long __attribute__((naked,noinline)) wrap_kbd_p1_f()
{

    asm volatile(
                "STMFD   SP!, {R4-R7,LR}\n"
                "SUB     SP, SP, #0xC\n"
                "BL      my_kbd_read_keys\n"
		"B	 _kbd_p1_f_cont\n"
    );
    return 0; //shut up the compiler
}


void my_kbd_read_keys()
{
    kbd_prev_state[0] = kbd_new_state[0];
    kbd_prev_state[1] = kbd_new_state[1];
    kbd_prev_state[2] = kbd_new_state[2];
    _kbd_pwr_on();
    kbd_fetch_data(kbd_new_state);
if(!block_script) // normal operation
{	
          checkKeyPress();

      if (kbd_process() == 0)
    {
	//leave it alone...
		physw_status[0] = kbd_new_state[0];
		physw_status[1] = kbd_new_state[1];
		physw_status[2] = kbd_new_state[2];
            if(imgbuf)physw_status[2]  = physw_status[2] | menu_key_mask ; 

     } 
     else 
     {
        		                                     // override keys

platform_kbd_hook();
		physw_status[0] = kbd_new_state[0];
            physw_status[1] = kbd_new_state[1];
		physw_status[2] = (kbd_new_state[2] & (~0x1fff)) |(kbd_mod_state & 0x1fff); 
  

        if (switch_override == 1) 
              { 
                physw_status[1] = (physw_status[1] & (~SWITCH_MASK1)) | (switch_mod_state[1] & SWITCH_MASK1);
              }

       else if (switch_override == 2) 
        {
#if !defined(CAMERA_a540)
          physw_status[2] = physw_status[2] & 0xffffdfff;
#else
          physw_status[2] = physw_status[2] & 0xfffffbff;
#endif
        }

      }
 }

else if(ub || sertx) 
 {
  physw_status[0] = kbd_new_state[0];
  physw_status[1] = kbd_new_state[1];
  physw_status[2] = (kbd_new_state[2] & (~0x1fff)) |(kbd_mod_state & 0x1fff); 
 }
      remote_key = (kbd_new_state[USB_IDX] & USB_MASK)==USB_MASK; 
      if (remote_key) 
      {
 
        remote_count += 1; 
      } 
      else if (remote_count) 
      { 
          usb_power = remote_count; 
          remote_count = 0; 
      }

   
	if (conf.remote_enable)
      {

#if !defined(CAMERA_a530) && !defined(CAMERA_a540)
		 physw_status[USB_IDX] = kbd_new_state[USB_IDX] & ~USB_MASK;
#endif
      }
        _kbd_read_keys_r2(physw_status);
        physw_status[2] = physw_status[2] & ~SD_READONLY_FLAG;
        _kbd_pwr_off();
}

void kbd_key_press(long key)
{
    int i;
   if(key<30 || key>34) 
   {
    for (i=0;keymap[i].hackkey;i++)
      {
	if (keymap[i].hackkey == key)
         {
	    kbd_mod_state &= ~keymap[i].canonkey;
          return;
	   }
       }
   }

    else if ((key >29) && (key<35)) 
          { 
           if (key == 34) 
            {            
             switch_override = 2;
              } 
              else 
              {
               switch_override = 1;
              }
            for (i=0; switchmap[i].hackkey; i++)
              {
                if (switchmap[i].hackkey == key) 
                  {
                    switch_mod_state[0] |= switchmap[i].canonkey0; 
                    switch_mod_state[1] |= switchmap[i].canonkey1;
                    switch_mod_state[2] |= switchmap[i].canonkey2;
                    return;
                  }
              }
          }
}



void kbd_key_release(long key)
{
    int i;

    if (key < 30 || key>34) 
   { 

    for (i=0;keymap[i].hackkey;i++)
     {
	if (keymap[i].hackkey == key)
         {
	    kbd_mod_state |= keymap[i].canonkey;
          return;
	   }
      }
   }

    else if ((key >29) && (key<35)) 
   { 
     switch_override = 0;
     for (i=0; switchmap[i].hackkey; i++) 
        {
         if (switchmap[i].hackkey == key)
           {
             switch_mod_state[0] &= ~switchmap[i].canonkey0; 
             switch_mod_state[1] &= ~switchmap[i].canonkey1;
             switch_mod_state[2] &= ~switchmap[i].canonkey2;
             return;
           }
         }
    }
}


void kbd_key_release_all()
{
  kbd_mod_state |= 0x1fff;
  switch_override = 0;
}

long kbd_is_key_pressed(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++){
	if (keymap[i].hackkey == key){
	    return ((kbd_new_state[2] & keymap[i].canonkey) == 0) ? 1:0;
	}
    }
    return 0;
}

long kbd_is_key_clicked(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++){
	if (keymap[i].hackkey == key){
	    return ((kbd_prev_state[2] & keymap[i].canonkey) != 0) &&
		    ((kbd_new_state[2] & keymap[i].canonkey) == 0);
	}
    }
    return 0;
}

long kbd_get_pressed_key()
{
    int i;
    for (i=0;keymap[i].hackkey;i++){
	if ((kbd_new_state[2] & keymap[i].canonkey) == 0){
	    return keymap[i].hackkey;
	}
    }
    return 0;
}

long kbd_get_clicked_key()                                     
{
    int i;
    for (i=0;keymap[i].hackkey;i++)
      {
	if (((kbd_prev_state[2] & keymap[i].canonkey) != 0) && 
	    ((kbd_new_state[2] & keymap[i].canonkey) == 0))
          { 
	     return keymap[i].hackkey; 
	    }
       }
    return 0; 
}

void kbd_reset_autoclicked_key() {
    last_kbd_key = 0;
}

long kbd_get_autoclicked_key() 
{
    static long last_kbd_time = 0, press_count = 0;
    register long key, t;

    key=kbd_get_clicked_key();  
    if (key) 
    {
        last_kbd_key = key;                                                                                
        press_count = 0; 
        last_kbd_time = get_tick_count(); 
        return key; 
    } 
    else 
    {
        if (last_kbd_key && kbd_is_key_pressed(last_kbd_key)) 
         {
            t = get_tick_count(); 
            if (t-last_kbd_time>((press_count)?175:500)) 
              {
                ++press_count; 
                last_kbd_time = t; 
                return last_kbd_key; 
               } 
               else 
               {
                return 0; 
               }
           } 
           else                                                         
           {
             last_kbd_key = 0;
             return 0;
           }
      }
}


long kbd_use_zoom_as_mf() {
    static long v;
    static long zoom_key_pressed = 0;

    if (kbd_is_key_pressed(KEY_ZOOM_IN) && (mode_get()&MODE_MASK) == MODE_REC) {
        get_property_case(PROPCASE_MANUAL_FOCUS, &v, 4);
        if (v) {
            kbd_key_release_all();
            kbd_key_press(KEY_RIGHT);
            zoom_key_pressed = KEY_ZOOM_IN;
            return 1;
        }
    } else {
        if (zoom_key_pressed==KEY_ZOOM_IN) {
            kbd_key_release(KEY_RIGHT);
            zoom_key_pressed = 0;
            return 1;
        }
    }
    if (kbd_is_key_pressed(KEY_ZOOM_OUT) && (mode_get()&MODE_MASK) == MODE_REC) {
        get_property_case(PROPCASE_MANUAL_FOCUS, &v, 4);
        if (v) {
            kbd_key_release_all();
            kbd_key_press(KEY_LEFT);
            zoom_key_pressed = KEY_ZOOM_OUT;
            return 1;
        }
    } else {
        if (zoom_key_pressed==KEY_ZOOM_OUT) {
            kbd_key_release(KEY_LEFT);
            zoom_key_pressed = 0;
            return 1;
        }
    }
    return 0;
}



int get_usb_power(int edge)
{
  int x; 
  if(edge) return remote_key; 
  x = usb_power;  
  usb_power = 0; 
  return x;
}

int get_usb_bit() 
{
	int x ;
	
	#if defined(CAMERA_a530) || defined(CAMERA_a540)
		x = (long)*mmio2;
	#elif 		defined(CAMERA_a610) || defined(CAMERA_a620) || defined(CAMERA_a630)\
			|| defined(CAMERA_a640) ||	defined(CAMERA_ixus800_sd700) || defined(CAMERA_a410)\
            || defined(CAMERA_a420) || defined(CAMERA_a430)
		x = (long)*mmio1;
	#elif defined(CAMERA_a710) || defined(CAMERA_a700)
		x = (long)*mmio0;
	#else
		x = 0 ; 
	#endif

	return( ( x & USB_MASK) ==USB_MASK) ; 
}


long is_key_pressed_now(long key)
{
 int i;
 long kbd_tmp_state[3];
 kbd_fetch_data(kbd_tmp_state);
 for (i=0;keymap[i].hackkey;i++)
  {
   if (keymap[i].hackkey == key)
    {
     return ((kbd_tmp_state[2] & keymap[i].canonkey) == 0) ? 1:0;
    }
  }
 return 0;
}


