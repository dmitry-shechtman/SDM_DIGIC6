#ifndef GUI_H
#define GUI_H

typedef unsigned int    coord;
typedef unsigned short 	color;

#define MAKE_COLOR(bg, fg)	((color)((((char)(bg))<<8)|((char)(fg))))
#define FG_COLOR(color)     ((unsigned char)(color & 0xFF))
#define BG_COLOR(color)     ((unsigned char)(color >> 8))

enum Gui_Mode { GUI_MODE_NONE = 0,
                GUI_MODE_ALT,
                GUI_MODE_MENU,
                GUI_MODE_PALETTE,
                GUI_MODE_MBOX,
                GUI_MODE_DEBUG,
                GUI_MODE_FSELECT,
                GUI_MODE_READ,
                GUI_MODE_OSD,
                GUI_MODE_MPOPUP};
extern void gui_draw_read(int arg);
extern void gui_redraw();
extern void gui_set_need_restore();
extern void gui_force_restore();
extern void draw_pixel(coord x, coord y, color cl);
extern enum Gui_Mode gui_get_mode();
extern void gui_set_mode(enum Gui_Mode mode);
extern void gui_update_script_submenu();
const char* video_bitrate_enum();
const char* tv_enum();
const char* tv_bracket_values_enum();
const char* focus_enum();


#endif
