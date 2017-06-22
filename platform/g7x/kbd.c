#include "stdlib.h"
#include "lolevel.h"
#include "platform.h"
#include "core.h"
#include "keyboard.h"
#include "conf.h"
#include "platform_kbd.h"

typedef struct 
{
    short grp;
    short hackkey;
    long canonkey;
} KeyMap;

long kbd_new_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
long kbd_prev_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
long kbd_mod_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
static  KeyMap keymap[];
static int alt_mode_led=0;
extern void _GetKbdState(long*);
static int usb_power=0;
static int remote_key, remote_count;
static int shoot_counter=0;
extern void _platformsub_kbd_fetch_data(long*);
#define DELAY_TIMEOUT 10000

static  KeyMap keymap[] = {
    { 0, KEY_SHOOT_FULL      ,0x00040001 }, // Found @0xfc613714, levent 0x01
    { 0, KEY_SHOOT_HALF      ,0x00040000 }, // Found @0xfc61378c, levent 0x00
    { 0, KEY_SHOOT_FULL_ONLY ,0x00000001 }, // Found @0xfc613714, levent 0x01
    { 0, KEY_ZOOM_OUT        ,0x00000002 }, // Found @0xfc61371c, levent 0x04
    { 0, KEY_ZOOM_IN         ,0x00000004 }, // Found @0xfc613724, levent 0x03
    { 0, KEY_VIDEO           ,0x00000008 }, // Found @0xfc61372c, levent 0x02
    { 0, KEY_MENU            ,0x00000010 }, // Found @0xfc613734, levent 0x14
    { 0, KEY_UP              ,0x00000020 }, // Found @0xfc61373c, levent 0x06
    { 0, KEY_DOWN            ,0x00000040 }, // Found @0xfc613744, levent 0x07
    { 0, KEY_RIGHT           ,0x00000080 }, // Found @0xfc61374c, levent 0x09
    { 0, KEY_LEFT            ,0x00000100 }, // Found @0xfc613754, levent 0x08
    { 0, KEY_SET             ,0x00000200 }, // Found @0xfc61375c, levent 0x0a
    { 0, KEY_ERASE           ,0x00000400 }, // ring func / erase, from watching  physw
    { 0, KEY_PLAYBACK        ,0x00000800 }, // Found @0xfc61376c, levent 0x101
    { 0, KEY_WIFI            ,0x00010000 }, // wifi button, from watching physw
//    { 0, KEY_POWER           ,0x00020000 }, // Found @0xfc613784, levent 0x100
    { 0, 0, 0 }
};
// ev comp and mode dial is in word 1, lower bits, encoding unclear
// flash open word 0, 0x00004000 = open
// no battery door switch, cam runs with door open, no physw change

long __attribute__((naked,noinline)) wrap_kbd_p1_f() {

    asm volatile(
        "push    {r1-r7, lr}\n"
        "movs    r4, #0\n"
        "bl      my_kbd_read_keys\n"
        "b       _kbd_p1_f_cont\n"
    );

    return 0;
}

// no stack manipulation needed here, since we create the task directly
void __attribute__((noinline)) mykbd_task() {
    extern void kbd_p2_f_my();

    while (physw_run) {
        _SleepTask(physw_sleep_delay);

        if (wrap_kbd_p1_f() == 1) {             // autorepeat ?
            _kbd_p2_f();
// TODO SX280
//            kbd_p2_f_my();                      // replacement of _kbd_p2_f (in sub/<fwver>/boot.c)
        }
    }

    _ExitTask();
}


void my_kbd_read_keys() {
    kbd_prev_state[0] = kbd_new_state[0];
    kbd_prev_state[1] = kbd_new_state[1];
    kbd_prev_state[2] = kbd_new_state[2];

    // note assumed kbd_pwr_on has been called if needed
    kbd_fetch_data(kbd_new_state);

    long status = kbd_process();
    checkKeyPress();
    if (status == 0)
     {
        // leave it alone...
        physw_status[0] = kbd_new_state[0];
        physw_status[1] = kbd_new_state[1];
        physw_status[2] = kbd_new_state[2];
     } 
    else 
     {
       platform_kbd_hook();
        // override keys
        physw_status[0] = (kbd_new_state[0] & (~KEYS_MASK0)) | (kbd_mod_state[0] & KEYS_MASK0);
        physw_status[1] = (kbd_new_state[1] & (~KEYS_MASK1)) | (kbd_mod_state[1] & KEYS_MASK1);
        physw_status[2] = (kbd_new_state[2] & (~KEYS_MASK2)) | (kbd_mod_state[2] & KEYS_MASK2);
      }
    remote_key = (physw_status[USB_IDX] & USB_MASK)==USB_MASK; 
    if (remote_key)
        remote_count += 1;
    else if (remote_count) 
     {
        usb_power = remote_count;
        remote_count = 0;
     }
 
    if (conf.remote_enable)physw_status[USB_IDX] = physw_status[USB_IDX] & ~(USB_MASK);
     physw_status[SD_READONLY_IDX] = physw_status[SD_READONLY_IDX] & ~SD_READONLY_FLAG;
}

int get_usb_power(int edge) 
{
  int x;
  if (edge) return remote_key;
  x = usb_power;
  usb_power = 0;
  return x;
}

extern void _GetKbdState(long *);

void kbd_fetch_data(long *dst)
{
    _GetKbdState(dst);
    _kbd_read_keys_r2(dst);
}

void kbd_key_press(long key) {
    int i;

    for (i=0;keymap[i].hackkey;i++){
        if (keymap[i].hackkey == key){
            kbd_mod_state[keymap[i].grp] &= ~keymap[i].canonkey;
            return;
        }
    }
}

void kbd_key_release(long key) {
   int i;

    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key){
            kbd_mod_state[keymap[i].grp] |= keymap[i].canonkey;
            return;
        }
    }
}

void kbd_key_release_all() {
    kbd_mod_state[0] |= KEYS_MASK0;
    kbd_mod_state[1] |= KEYS_MASK1;
    kbd_mod_state[2] |= KEYS_MASK2;
}

long kbd_is_key_pressed(long key) {
    int i;

    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key) {
			if ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0)
                return 1;
        }
    }
    return 0;
}

long kbd_is_key_clicked(long key) {
    int i;

    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key){
            return ((kbd_prev_state[keymap[i].grp] & keymap[i].canonkey) != 0) &&
                ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0);
        }
    }
    return 0;
}

long kbd_get_pressed_key() {
    int i;

    for (i=0;keymap[i].hackkey;i++) {
        if ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0) {
            return keymap[i].hackkey;
        }
    }
    return 0;
}

long kbd_get_clicked_key() {
    int i;

    for (i=0;keymap[i].hackkey;i++) {
        if (((kbd_prev_state[keymap[i].grp] & keymap[i].canonkey) != 0) && ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0)) {
            return keymap[i].hackkey;
        }
    }
    return 0;
}

static long last_kbd_key = 0;

void kbd_reset_autoclicked_key() 
{
	last_kbd_key = 0;
}

long kbd_get_autoclicked_key() {
	
	static long last_kbd_time = 0, press_count = 0;
	register long key, t;
	
	key=kbd_get_clicked_key();
	if (key) {
		last_kbd_key = key;
		press_count = 0;
	//	last_kbd_time = get_tick_count();
		return key;
	} else {
		if (last_kbd_key && kbd_is_key_pressed(last_kbd_key)) {
		//	t = get_tick_count();
			if (t-last_kbd_time>((press_count)?175:500)) {
				++press_count;
				last_kbd_time = t;
				return last_kbd_key;
			} else {
				return 0;
			}
		} else {
			last_kbd_key = 0;
			return 0;
		}
	}
}

long kbd_use_zoom_as_mf() 
{
 return 0;
}

#include "../generic/wurb2.c"

