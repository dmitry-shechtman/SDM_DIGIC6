#ifndef BATT_GRID_H
#define BATT_GRID_H

//-------------------------------------------------------------------
unsigned long get_batt_perc();
extern void gui_batt_draw_osd();
long get_batt_average();
//-------------------------------------------------------------------
extern char grid_title[36];
extern void gui_grid_draw_osd(int force);
extern void grid_lines_load(const char *fn);
//------------------------------------------------------------------

#endif
