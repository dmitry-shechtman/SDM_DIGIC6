#ifndef FONT_H
#define FONT_H

#include "gui.h"

//-------------------------------------------------------------------
#define FONT_CP_WIN     0
#define FONT_CP_DOS     1
#define  FONT_CP_WIN_1250       0
#define  FONT_CP_WIN_1251       1
#define  FONT_CP_WIN_1252       2
#define  FONT_CP_WIN_1254       3
#define  FONT_CP_WIN_1257       4

//-------------------------------------------------------------------
extern unsigned char current_font[256][16];

//-------------------------------------------------------------------
extern void font_init();
extern void font_set(int codepage);
extern int rbf_load(char *file);
//-------------------------------------------------------------------
#endif

