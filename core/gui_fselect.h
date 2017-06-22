#ifndef GUI_FSELECT_H
#define GUI_FSELECT_H

//-------------------------------------------------------------------
extern void gui_fselect_init(int title, const char* dir, void (*on_select)(const char *fn));
extern void gui_easy_fselect_init(int title, const char* dir, void (*on_select)(const char *fn));
extern char* gui_fselect_result();
extern void gui_fselect_kbd_process();
extern void gui_easy_fselect_kbd_process();
extern void gui_fselect_draw();
extern void gui_easy_fselect_draw();
extern int read_easyModes_dir();
//-------------------------------------------------------------------
#endif
