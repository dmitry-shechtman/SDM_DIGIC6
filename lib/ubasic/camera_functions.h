
void ubasic_camera_press(const char *s);
void ubasic_camera_release(const char *s);
void ubasic_camera_wait_click(int timeout);
int ubasic_camera_is_clicked(const char *s);
void ubasic_camera_click(const char *s);
void ubasic_camera_sleep(long v);
void ubasic_camera_shoot();
void ubasic_camera_set_raw(int mode);
void ubasic_camera_get_raw();
void ubasic_camera_set_nr(int to);
int ubasic_camera_get_nr();
void shooting_set_prop(int id, int v);
int shooting_get_prop(int id);
long stat_get_vbatt();
int shooting_get_day_seconds();
int shooting_get_tick_count();
int ubasic_camera_script_autostart();
int get_usb_power(int edge);
void ubasic_camera_set_script_autostart(int state);
void exit_alt();
void ubasic_bracketing_done();
void ubasic_wfsp();
void ubasic_scts();
void ubasic_blcd();
void ubasic_sm(int t);
void ubasic_usbupload(int t);
#include "../../core/motion_detector.h"


