#ifndef KBD_H
#define KBD_H

//-------------------------------------------------------------------
extern void kbd_sched_shoot();
extern void kbd_sched_click(long key);
extern void kbd_sched_press(long key);
extern void kbd_sched_release(long key);
extern void kbd_sched_delay(long msec);
extern int keyid_by_name (const char *n);
extern int get_vkey(int key);
const char* key_name_from_id(int keyId); 
//-------------------------------------------------------------------
#endif
