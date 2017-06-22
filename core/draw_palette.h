#ifndef DRAW_PALETTE_H
#define DRAW_PALETTE_H
//-------------------------------------------------------------------
#include "gui.h"

//-------------------------------------------------------------------

#define PALETTE_MODE_DEFAULT    0
#define PALETTE_MODE_SELECT     1

//-------------------------------------------------------------------
extern void gui_palette_init(int mode, color st_color, void (*on_select)(color clr));
extern void gui_palette_kbd_process();
extern void gui_palette_draw();

#if CAM_BITMAP_PALETTE==1
#define COLOR_TRANSPARENT	0x00
#define COLOR_WHITE		0x11
#define COLOR_WHITE_P		0x11
#define COLOR_RED		      0x22
#define COLOR_RED_P		0x22
#define COLOR_PINK		0x2A
#define COLOR_PINK_P		0x2A
#define COLOR_GREEN		0xF5
#define COLOR_GREEN_P		0xF5
#define COLOR_BLUE_LT		0xDD
#define COLOR_BLUE_LT_P		0x99
#define COLOR_BLUE		0xDF
#define COLOR_BLUE_P          0xAC
#define COLOR_YELLOW		0x6C
#define COLOR_YELLOW_P		0x66
#define COLOR_BLACK		0xFF
#define COLOR_BLACK_P		0xFF
#define COLOR_BG 		      0x44
#define COLOR_BG_P 		0x4D
#define COLOR_FG 		      COLOR_WHITE
#define COLOR_FG_P 		COLOR_WHITE_P
#define COLOR_SELECTED_BG	COLOR_RED
#define COLOR_SELECTED_BG_P	COLOR_RED_P
#define COLOR_SELECTED_FG	COLOR_WHITE
#define COLOR_SELECTED_FG_P   COLOR_WHITE_P
#define COLOR_ALT_BG 		COLOR_GREY
#define COLOR_ALT_BG_P        COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==2 // G11
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0xA0
#define COLOR_RED_P	    0x2C // orange
#define COLOR_PINK	    0x24
#define COLOR_PINK_P        0x24
#define COLOR_GREEN         0xAA
#define COLOR_GREEN_P	    0xCB
#define COLOR_BLUE_LT       0xA9
#define COLOR_BLUE_LT_P	    0xC8 // green
#define COLOR_BLUE          0xA9
#define COLOR_BLUE_P        0xC8 // green
#define COLOR_YELLOW        0x9A
#define COLOR_YELLOW_P	    0xBC
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x93
#define COLOR_BG_P 	    0xFA // partially transparent ?
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==3		// IXUS980
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01        
#define COLOR_RED           0x60
#define COLOR_RED_P	    0x60
#define COLOR_PINK	    0x80
#define COLOR_PINK_P	    0xC8      
#define COLOR_GREEN         0x87
#define COLOR_GREEN_P	    0xD2
#define COLOR_BLUE_LT       0x70
#define COLOR_BLUE_LT_P	    0x70
#define COLOR_BLUE          0x78
#define COLOR_BLUE_P        0x78
#define COLOR_YELLOW        0x8F
#define COLOR_YELLOW_P	    0x8F
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x1E
#define COLOR_BG_P 	    0x1E
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P  
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==4		// SX200
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01      
#define COLOR_RED           0x2B
#define COLOR_RED_P	    0x2B
#define COLOR_PINK	    0x1F
#define COLOR_PINK_P	    0x1F     
#define COLOR_GREEN         0x99
#define COLOR_GREEN_P	    0xAC
#define COLOR_BLUE_LT       0x2D
#define COLOR_BLUE_LT_P	    0x2D
#define COLOR_BLUE          0x3B
#define COLOR_BLUE_P        0x3B
#define COLOR_YELLOW        0x9A
#define COLOR_YELLOW_P	    0xAD
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x86
#define COLOR_BG_P 	    0xB1
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P  
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==5			// IXUS95,IXUS100
#define COLOR_TRANSPARENT        0x00	   
#define COLOR_WHITE		   0x01	   
#define COLOR_WHITE_P		   0x01	   
#define COLOR_RED       	   0x70	   
#define COLOR_RED_P 		   0x70
#define COLOR_PINK       	   0x70	   
#define COLOR_PINK_P 		   0x70	   
#define COLOR_GREEN 		   0xA6	   
#define COLOR_GREEN_P 		   0xc3	   
#define COLOR_BLUE_LT  		   0x79	   
#define COLOR_BLUE_LT_P 	   0x80	   
#define COLOR_BLUE 		   0x87	   
#define COLOR_BLUE_P		   0x96	   
#define COLOR_YELLOW		   0xa7	   
#define COLOR_YELLOW_P		   0x54	   
#define COLOR_BLACK 		   0xff	   
#define COLOR_BLACK_P 		   0xff	   
#define COLOR_BG                 0x1e	   
#define COLOR_BG_P  		   0x1e	   
#define COLOR_FG       		   COLOR_WHITE	   
#define COLOR_FG_P  		   COLOR_WHITE_P	   
#define COLOR_SELECTED_BG 	   COLOR_RED	   
#define COLOR_SELECTED_BG_P 	   COLOR_RED_P	   
#define COLOR_SELECTED_FG 	   COLOR_WHITE	   
#define COLOR_SELECTED_FG_P  	   COLOR_WHITE_P	   
#define COLOR_ALT_BG  		   COLOR_GREY	   
#define COLOR_ALT_BG_P           COLOR_GREY_P	

#elif CAM_BITMAP_PALETTE==6 //S90
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0xA0
#define COLOR_RED_P	    0x2C 
#define COLOR_PINK	    0xA0
#define COLOR_PINK_P        0x2C  
#define COLOR_GREEN         0xA3
#define COLOR_GREEN_P	    0xCB
#define COLOR_BLUE_LT       0xA2
#define COLOR_BLUE_LT_P	    0xCB // green
#define COLOR_BLUE          0xA1
#define COLOR_BLUE_P        0xCB // green
#define COLOR_YELLOW        0x9A
#define COLOR_YELLOW_P	    0xBC
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x5A
#define COLOR_BG_P 	    0xFE
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==7 // SX120
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0xA0
#define COLOR_RED_P	    0x2B // orange
#define COLOR_PINK	    0x1E
#define COLOR_PINK_P        0x1E 
#define COLOR_GREEN         0xAA
#define COLOR_GREEN_P	    0xCB
#define COLOR_BLUE_LT       0xA9
#define COLOR_BLUE_LT_P	    0xCB // green
#define COLOR_BLUE          0xA1
#define COLOR_BLUE_P        0xCB // green
#define COLOR_YELLOW        0x9A
#define COLOR_YELLOW_P	    0xBE
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x61
#define COLOR_BG_P 	    0x51 // partially transparent ?
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==8 		// A490,G12,S95
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0x66
#define COLOR_RED_P	    0xA0 // use  red
#define COLOR_PINK	    0x66 // use red
#define COLOR_PINK_P        0xA0  
#define COLOR_GREEN         0x6B
#define COLOR_GREEN_P	    0xA1
#define COLOR_BLUE_LT       0x68 
#define COLOR_BLUE_LT_P	    0xA1 // use green
#define COLOR_BLUE          0x67 
#define COLOR_BLUE_P        0xA2
#define COLOR_YELLOW        0x60
#define COLOR_YELLOW_P	    0x90
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x5A
#define COLOR_BG_P 	    0x93 // partially transparent ?
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P
 

#elif CAM_BITMAP_PALETTE==9 //IXUS105
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0x66
#define COLOR_RED_P	    0x5C 
#define COLOR_PINK	    0x1E 
#define COLOR_PINK_P        0x2E 
#define COLOR_GREEN         0x69
#define COLOR_GREEN_P	    0xA1
#define COLOR_BLUE_LT       0x68 
#define COLOR_BLUE_LT_P	    0xA1 
#define COLOR_BLUE          0x67 
#define COLOR_BLUE_P        0x9F
#define COLOR_YELLOW        0x6C
#define COLOR_YELLOW_P	    0x9C
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x62
#define COLOR_BG_P 	    0x62 // partially transparent ?
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==10 // sx20
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01      
#define COLOR_RED           0x91
#define COLOR_RED_P	    0x2C
#define COLOR_PINK	    0xE0
#define COLOR_PINK_P	    0x20 // orange    
#define COLOR_GREEN         0xA9
#define COLOR_GREEN_P	    0xAC
#define COLOR_BLUE_LT       0x2D
#define COLOR_BLUE_LT_P	    0x80 // grey
#define COLOR_BLUE          0x92
#define COLOR_BLUE_P        0xA0 // green
#define COLOR_YELLOW        0x8B
#define COLOR_YELLOW_P	    0xAF
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0xFA
#define COLOR_BG_P 	    0xB0
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P  
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==11 // sx130is
#define COLOR_TRANSPARENT 0x00
#define COLOR_WHITE 0x01
#define COLOR_WHITE_P 0x01
#define COLOR_RED 0x66
#define COLOR_RED_P 0xA0
#define COLOR_PINK 0x1E
#define COLOR_PINK_P 0x1E
#define COLOR_GREEN 0x6B
#define COLOR_GREEN_P 0xA1
#define COLOR_BLUE_LT 0x68
#define COLOR_BLUE_LT_P 0x89
#define COLOR_BLUE 0x67
#define COLOR_BLUE_P 0xA2
#define COLOR_YELLOW 0x60
#define COLOR_YELLOW_P 0x90
#define COLOR_BLACK 0xFF
#define COLOR_BLACK_P 0xFF
#define COLOR_BG 0x5A
#define COLOR_BG_P 0x93
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P  
#define COLOR_ALT_BG        COLOR_GREY
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==12 //Not used
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P 0x01
#define COLOR_RED           0x1e
#define COLOR_RED_P 0xA0
#define COLOR_PINK 0x1E
#define COLOR_PINK_P 0x1E
#define COLOR_GREY          0x1a
#define COLOR_GREY_P 0x16
#define COLOR_GREEN         0x17
#define COLOR_GREEN_P 0xA1
#define COLOR_BLUE_LT       0x10
#define COLOR_BLUE_LT_P 0x89
#define COLOR_BLUE          0x14
#define COLOR_BLUE_P 0xA2
#define COLOR_YELLOW        0x16
#define COLOR_YELLOW_P 0x90
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P 0xFF
#define COLOR_BG            0x62
#define COLOR_BG_P 0x93
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   0x0e
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P 
#define COLOR_ALT_BG        COLOR_BG
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==13 //A495
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x11
#define COLOR_WHITE_P       0x11
#define COLOR_RED           0x22 // brown
#define COLOR_RED_P         0x22 // brown
#define COLOR_PINK          0x24 // light brown
#define COLOR_PINK_P        0x24 // light brown
#define COLOR_GREEN         0x7c
#define COLOR_GREEN_P       0x7c
#define COLOR_BLUE_LT       0x58
#define COLOR_BLUE_LT_P     0xC1
#define COLOR_BLUE          0xF8
#define COLOR_BLUE_P        0xC4 // NA
#define COLOR_YELLOW        0xDD
#define COLOR_YELLOW_P      0xDD
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P       0xFF
#define COLOR_BG            0x36
#define COLOR_BG_P          0x34
#define COLOR_FG            COLOR_WHITE
#define COLOR_FG_P 	    COLOR_WHITE_P
#define COLOR_SELECTED_BG   COLOR_RED
#define COLOR_SELECTED_BG_P COLOR_RED_P
#define COLOR_SELECTED_FG   COLOR_WHITE
#define COLOR_SELECTED_FG_P COLOR_WHITE_P 
#define COLOR_ALT_BG        COLOR_BG
#define COLOR_ALT_BG_P      COLOR_GREY_P

#elif CAM_BITMAP_PALETTE==14    // Not used

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x9f
    #define COLOR_RED_P         0x9f
    #define COLOR_PINK          0x1e    // Orange
    #define COLOR_PINK_P        0x1e    // Orange
    #define COLOR_GREY          0x1a
    #define COLOR_GREY_P        0x1a
    #define COLOR_GREEN         0xa0
    #define COLOR_GREEN_P       0xa0
    #define COLOR_BLUE_LT       0x96
    #define COLOR_BLUE_LT_P     0x96
    #define COLOR_BLUE          0xa1
    #define COLOR_BLUE_P        0xa1
    #define COLOR_YELLOW        0x92
    #define COLOR_YELLOW_P      0x92
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x4B
    #define COLOR_BG_P          0x4B
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   0x0e
    #define COLOR_SELECTED_BG_P 0x0e
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==15    // Used by :- SX210is,IXUS130

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x66
    #define COLOR_RED_P         0x1E // orange
    #define COLOR_PINK          0x66    
    #define COLOR_PINK_P        0x1E // orange  
    #define COLOR_GREEN         0x6B
    #define COLOR_GREEN_P       0x9F // green
    #define COLOR_BLUE_LT       0x68
    #define COLOR_BLUE_LT_P     0x9F // green
    #define COLOR_BLUE          0x67
    #define COLOR_BLUE_P        0x9F // green
    #define COLOR_YELLOW        0x60
    #define COLOR_YELLOW_P      0x90
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x55
    #define COLOR_BG_P          0x00
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==16    // Not used

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x11
    #define COLOR_WHITE_P       0x11
    #define COLOR_RED           0x2F
    #define COLOR_RED_P         0x2F 
    #define COLOR_PINK          0x24    
    #define COLOR_PINK_P        0x24   
    #define COLOR_GREY          0x1f
    #define COLOR_GREY_P        0x1f
    #define COLOR_GREEN         0xFC
    #define COLOR_GREEN_P       0xFC 
    #define COLOR_BLUE_LT       0xEE
    #define COLOR_BLUE_LT_P     0xEE
    #define COLOR_BLUE          0xEF
    #define COLOR_BLUE_P        0xEF
    #define COLOR_YELLOW        0xDD
    #define COLOR_YELLOW_P      0xDD
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x0F
    #define COLOR_BG_P          0x00
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==17    // Used by :- S100

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x5D
    #define COLOR_RED_P         0x9F 
    #define COLOR_PINK          0x57    
    #define COLOR_PINK_P        0x5C   
    #define COLOR_GREEN         0x5A
    #define COLOR_GREEN_P       0xA0 
    #define COLOR_BLUE_LT       0x52
    #define COLOR_BLUE_LT_P     0x9E
    #define COLOR_BLUE          0x5E
    #define COLOR_BLUE_P        0xA1
    #define COLOR_YELLOW        0x51
    #define COLOR_YELLOW_P      0x9B
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0xF1
    #define COLOR_BG_P          0x00
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==18    // Used by :- A800

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x11
    #define COLOR_WHITE_P       0x11
    #define COLOR_RED           0x22
    #define COLOR_RED_P         0x22 
    #define COLOR_PINK          0xE2    
    #define COLOR_PINK_P        0xC2   
    #define COLOR_GREEN         0xC6
    #define COLOR_GREEN_P       0xC6 
    #define COLOR_BLUE_LT       0xE5
    #define COLOR_BLUE_LT_P     0x4C
    #define COLOR_BLUE          0xE8
    #define COLOR_BLUE_P        0x5C
    #define COLOR_YELLOW        0xD4
    #define COLOR_YELLOW_P      0xD8
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x43
    #define COLOR_BG_P          0x43
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==19    // Used by :- A810,A2300,A2400,A3400,A4000,IXUS132,IXUS140

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x5B
    #define COLOR_RED_P         0xC2 
    #define COLOR_PINK          0x1E    
    #define COLOR_PINK_P        0xC8   
    #define COLOR_GREEN         0x59
    #define COLOR_GREEN_P       0xC4 
    #define COLOR_BLUE_LT       0x57
    #define COLOR_BLUE_LT_P     0xC7
    #define COLOR_BLUE          0x5C
    #define COLOR_BLUE_P        0xC6
    #define COLOR_YELLOW        0x50
    #define COLOR_YELLOW_P      0xCA
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x4A
    #define COLOR_BG_P          0xB5
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG
    #define COLOR_HISTO_R               COLOR_RED
    #define COLOR_HISTO_R_PLAY          COLOR_RED_P
    #define COLOR_HISTO_B               COLOR_BLUE
    #define COLOR_HISTO_B_PLAY          COLOR_BLUE_P
    #define COLOR_HISTO_G               COLOR_GREEN
    #define COLOR_HISTO_G_PLAY          COLOR_GREEN_P
    #define COLOR_HISTO_BG              COLOR_BLUE
    #define COLOR_HISTO_BG_PLAY         COLOR_BLUE_P
    #define COLOR_HISTO_RG              COLOR_YELLOW
    #define COLOR_HISTO_RG_PLAY         COLOR_YELLOW_P
    #define COLOR_HISTO_RB              COLOR_RED
    #define COLOR_HISTO_RB_PLAY         COLOR_RED_P

#elif CAM_BITMAP_PALETTE==20    // Used by :- A3200/Paul Thoele

    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x6C
    #define COLOR_RED_P         0x9F 
    #define COLOR_PINK          0x1E    
    #define COLOR_PINK_P        0x1E   
    #define COLOR_GREEN         0x5A
    #define COLOR_GREEN_P       0xA0 
    #define COLOR_BLUE_LT       0x58
    #define COLOR_BLUE_LT_P     0xA1
    #define COLOR_BLUE          0x6D
    #define COLOR_BLUE_P        0xA1
    #define COLOR_YELLOW        0x51
    #define COLOR_YELLOW_P      0x9B
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x4A
    #define COLOR_BG_P          0x3A
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==21    // Used by :- A2200/Michael Layefsky
    #define COLOR_TRANSPARENT   0x00
    #define COLOR_WHITE         0x01
    #define COLOR_WHITE_P       0x01
    #define COLOR_RED           0x6C
    #define COLOR_RED_P         0x9F 
    #define COLOR_PINK          0x1E    
    #define COLOR_PINK_P        0xC8   
    #define COLOR_GREEN         0x5A
    #define COLOR_GREEN_P       0xA0 
    #define COLOR_BLUE_LT       0x58
    #define COLOR_BLUE_LT_P     0xC7
    #define COLOR_BLUE          0x6D
    #define COLOR_BLUE_P        0xA1
    #define COLOR_YELLOW        0x51
    #define COLOR_YELLOW_P      0x92
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x4A
    #define COLOR_BG_P          0xB5
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==22    // Used by :- IXUS220,IXUS115
    #define COLOR_TRANSPARENT   0x00 
    #define COLOR_WHITE         0x01 
    #define COLOR_WHITE_P       0x01 
    #define COLOR_RED           0x6C  
    #define COLOR_RED_P         0x9F 
    #define COLOR_PINK          0x1E   
    #define COLOR_PINK_P        0x1E
    #define COLOR_GREEN         0x90
    #define COLOR_GREEN_P       0xA0
    #define COLOR_BLUE_LT       0x58 
    #define COLOR_BLUE_LT_P     0xA1
    #define COLOR_BLUE          0x6D
    #define COLOR_BLUE_P        0xA1
    #define COLOR_YELLOW        0x51
    #define COLOR_YELLOW_P      0x9B
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0x4A
    #define COLOR_BG_P          0x98
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==23    // Used by :- SX150is
    #define COLOR_TRANSPARENT   0x00 
    #define COLOR_WHITE         0x01 
    #define COLOR_WHITE_P       0x01 
    #define COLOR_RED           0x5D  
    #define COLOR_RED_P         0x9F 
    #define COLOR_PINK          0x5C   
    #define COLOR_PINK_P        0x1E
    #define COLOR_GREEN         0x81
    #define COLOR_GREEN_P       0xA0
    #define COLOR_BLUE_LT       0x82 
    #define COLOR_BLUE_LT_P     0xA1
    #define COLOR_BLUE          0x5E
    #define COLOR_BLUE_P        0xA1
    #define COLOR_YELLOW        0x51
    #define COLOR_YELLOW_P      0x9B
    #define COLOR_BLACK         0xFF
    #define COLOR_BLACK_P       0xFF
    #define COLOR_BG            0xF8
    #define COLOR_BG_P          0x98
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==24    // Used by :- SX220is,SX230is
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x01
    #define COLOR_WHITE_P        0x01
    #define COLOR_RED            0x6C
    #define COLOR_RED_P          0x9F 
    #define COLOR_PINK           0x1E    
    #define COLOR_PINK_P         0x1E   
    #define COLOR_GREEN          0x90
    #define COLOR_GREEN_P        0xA0 
    #define COLOR_BLUE_LT        0x58
    #define COLOR_BLUE_LT_P      0x9B
    #define COLOR_BLUE           0x6D
    #define COLOR_BLUE_P         0xA1
    #define COLOR_YELLOW         0x51
    #define COLOR_YELLOW_P       0x9B
    #define COLOR_BLACK          0xFF
    #define COLOR_BLACK_P        0xFF
    #define COLOR_BG             0x4C
    #define COLOR_BG_P           0x12
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==25    // Used by :- S110,SX240hs,SX260hs
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x01
    #define COLOR_WHITE_P        0x01
    #define COLOR_RED            0x5B
    #define COLOR_RED_P          0xC3 
    #define COLOR_PINK           0x1E    
    #define COLOR_PINK_P         0xC8   
    #define COLOR_GREEN          0x59
    #define COLOR_GREEN_P        0xC4 
    #define COLOR_BLUE_LT        0x57
    #define COLOR_BLUE_LT_P      0xC7
    #define COLOR_BLUE           0x5C
    #define COLOR_BLUE_P         0xC6
    #define COLOR_YELLOW         0x50
    #define COLOR_YELLOW_P       0xCA
    #define COLOR_BLACK          0xFF
    #define COLOR_BLACK_P        0xFF
    #define COLOR_BG             0x53
    #define COLOR_BG_P           0xB4
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==26    // Used by :- G1x
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x01
    #define COLOR_WHITE_P        0x01
    #define COLOR_RED            0x5D
    #define COLOR_RED_P          0x9F 
    #define COLOR_PINK           0x3D    
    #define COLOR_PINK_P         0x71   
    #define COLOR_GREEN          0x81
    #define COLOR_GREEN_P        0xA0 
    #define COLOR_BLUE_LT        0x58
    #define COLOR_BLUE_LT_P      0xA1
    #define COLOR_BLUE           0x5E
    #define COLOR_BLUE_P         0xA1
    #define COLOR_YELLOW         0x51
    #define COLOR_YELLOW_P       0x9B
    #define COLOR_BLACK          0xFF
    #define COLOR_BLACK_P        0xFF
    #define COLOR_BG             0x4A
    #define COLOR_BG_P           0x0F
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==27    // Used by :- A2500,SX50,SX160is,SX170is
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x01
    #define COLOR_WHITE_P        0x01
    #define COLOR_RED            0x5B
    #define COLOR_RED_P          0xC2 
    #define COLOR_PINK           0x1E    
    #define COLOR_PINK_P         0xC8   
    #define COLOR_GREEN          0x7E
    #define COLOR_GREEN_P        0xC4 
    #define COLOR_BLUE_LT        0x57
    #define COLOR_BLUE_LT_P      0xC7
    #define COLOR_BLUE           0x5C
    #define COLOR_BLUE_P         0xC6
    #define COLOR_YELLOW         0x50
    #define COLOR_YELLOW_P       0xC9
    #define COLOR_BLACK          0x1D
    #define COLOR_BLACK_P        0x1D
    #define COLOR_BG             0x4B
    #define COLOR_BG_P           0x45 // from Sue Storey.I am assuming previous 0xB5 value for SX160is was incorrect.
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==28    // Used by :- SX40hs
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x4D
    #define COLOR_WHITE_P        0x5D
    #define COLOR_RED            0x5D
    #define COLOR_RED_P          0x9F 
    #define COLOR_PINK           0x3D    
    #define COLOR_PINK_P         0x69   
    #define COLOR_GREEN          0x5A
    #define COLOR_GREEN_P        0x9E 
    #define COLOR_BLUE_LT        0x52
    #define COLOR_BLUE_LT_P      0x72
    #define COLOR_BLUE           0x5E
    #define COLOR_BLUE_P         0xA1
    #define COLOR_YELLOW         0x51
    #define COLOR_YELLOW_P       0x90
    #define COLOR_BLACK          0x1D
    #define COLOR_BLACK_P        0x4A
    #define COLOR_BG             0x00
    #define COLOR_BG_P           0x74
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==29    // Used by :- SX270hs,EOS M3
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x3F
    #define COLOR_WHITE_P        0x3F
    #define COLOR_RED            0xFA
    #define COLOR_RED_P          0xFA 
    #define COLOR_PINK           0x92    
    #define COLOR_PINK_P         0x92   
    #define COLOR_GREEN          0xB3
    #define COLOR_GREEN_P        0xB3 
    #define COLOR_BLUE_LT        0x93
    #define COLOR_BLUE_LT_P      0x93
    #define COLOR_BLUE           0xD7
    #define COLOR_BLUE_P         0xD7
    #define COLOR_YELLOW         0x18
    #define COLOR_YELLOW_P       0x18
    #define COLOR_BLACK          0xFF
    #define COLOR_BLACK_P        0xFF
    #define COLOR_BG             0xC6
    #define COLOR_BG_P           0xC6
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==30          // used by G7x
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x3F
    #define COLOR_WHITE_P        0x3F
    #define COLOR_RED            0x9A
    #define COLOR_RED_P          0x9A 
    #define COLOR_PINK           0x52    
    #define COLOR_PINK_P         0x52   
    #define COLOR_GREEN          0x75
    #define COLOR_GREEN_P        0x75 
    #define COLOR_BLUE_LT        0x14
    #define COLOR_BLUE_LT_P      0x14
    #define COLOR_BLUE           0x96
    #define COLOR_BLUE_P         0x96
    #define COLOR_YELLOW         0x19
    #define COLOR_YELLOW_P       0x19
    #define COLOR_BLACK          0xFF
    #define COLOR_BLACK_P        0xFF
    #define COLOR_BG             0xBF 
    #define COLOR_BG_P           0xBF
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==31   // Used by :- G15
    #define COLOR_TRANSPARENT    0x00
    #define COLOR_WHITE          0x01
    #define COLOR_WHITE_P        0x01
    #define COLOR_RED            0x5B
    #define COLOR_RED_P          0xC2 
    #define COLOR_PINK           0x1E    
    #define COLOR_PINK_P         0xC8   
    #define COLOR_GREEN          0x7E
    #define COLOR_GREEN_P        0xC4 
    #define COLOR_BLUE_LT        0x57
    #define COLOR_BLUE_LT_P      0xC7
    #define COLOR_BLUE           0x5C
    #define COLOR_BLUE_P         0xC6
    #define COLOR_YELLOW         0x50
    #define COLOR_YELLOW_P       0xCA
    #define COLOR_BLACK          0x1D
    #define COLOR_BLACK_P        0x1D
    #define COLOR_BG             0x54
    #define COLOR_BG_P           0xB5
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG

#elif CAM_BITMAP_PALETTE==32   // Used by :- G10
#define COLOR_TRANSPARENT   0x00
#define COLOR_WHITE         0x01
#define COLOR_WHITE_P	    0x01  
#define COLOR_RED           0x69
#define COLOR_RED_P	    0x2C  
#define COLOR_PINK	    0x4C
#define COLOR_PINK_P        0x4C
#define COLOR_GREEN         0x87
#define COLOR_GREEN_P	    0x87
#define COLOR_BLUE_LT       0xAA
#define COLOR_BLUE_LT_P	    0xAA  
#define COLOR_BLUE          0x78
#define COLOR_BLUE_P        0x87  
#define COLOR_YELLOW        0x96
#define COLOR_YELLOW_P	    0x90  
#define COLOR_BLACK         0xFF
#define COLOR_BLACK_P	    0xFF
#define COLOR_BG            0x28  
#define COLOR_BG_P 	    0x1F  
    #define COLOR_FG            COLOR_WHITE
    #define COLOR_FG_P          COLOR_WHITE
    #define COLOR_SELECTED_BG   COLOR_RED
    #define COLOR_SELECTED_BG_P COLOR_RED_P
    #define COLOR_SELECTED_FG   COLOR_WHITE
    #define COLOR_SELECTED_FG_P COLOR_WHITE
    #define COLOR_ALT_BG        COLOR_BG
    #define COLOR_ALT_BG_P      COLOR_BG


#else
#error CAM_BITMAP_PALETTE not defined
#endif

#define FONT_WIDTH 		   8
#define FONT_HEIGHT             16
#define BIG_FONT_WIDTH 		FONT_WIDTH*3
#define BIG_FONT_HEIGHT       FONT_HEIGHT*3
//-------------------------------------------------------------------
extern unsigned int		screen_width, screen_height, screen_size;
extern unsigned int           screen_buffer_width, screen_buffer_height, screen_buffer_size;
//-------------------------------------------------------------------

void draw_pixel_grid(coord x,coord y,color cl);
void draw_init();
void draw_set_draw_proc(void (*pixel_proc)(coord x,coord y,color cl));
extern color draw_get_pixel(coord x, coord y);
extern void draw_line(coord x1, coord y1, coord x2, coord y2, color cl);

//draw frame
extern void draw_rect(coord x1, coord y1, coord x2, coord y2, color cl);
//color: hi_byte - BG; lo_byte - FG
extern void draw_filled_rect(coord x1, coord y1, coord x2, coord y2, color cl);

extern int draw_char(coord x, coord y, const char ch, color cl,int m);
extern int draw_string(coord x, coord y, const char *s, color cl, int m);
extern void draw_big_char(coord x, coord y, const char ch, color cl);
extern void draw_big_string(coord x, coord y, const char *s, color cl);
extern void draw_big_string_centre_len(int x, int y, int len, const char *str, color cl); 

extern void draw_txt_rect(coord col, coord row, unsigned int length, unsigned int height, color cl);
extern void draw_txt_rect_exp(coord col, coord row, unsigned int length, unsigned int height, unsigned int exp, color cl);
extern void draw_txt_filled_rect(coord col, coord row, unsigned int length, unsigned int height, color cl);
extern void draw_txt_filled_rect_exp(coord col, coord row, unsigned int length, unsigned int height, unsigned int exp, color cl);
extern void draw_txt_string(coord col, coord row, const char *str, color cl);
extern void draw_txt_char(coord col, coord row, const char ch, color cl);

extern void draw_clear();
extern void draw_restore();

extern void draw_fill(coord x, coord y, color cl_fill, color cl_bound);
extern void draw_circle(coord x, coord y, const unsigned int r, color cl);
extern void draw_ellipse(coord xc, coord yc, unsigned int a, unsigned int b, color cl);
extern void draw_filled_ellipse(coord xc, coord yc, unsigned int a, unsigned int b, color cl);

#if CAM_USES_ASPECT_CORRECTION //nandoide sept-2009 
void draw_set_aspect_xcorrection_proc(unsigned int (*xcorrection_proc)(unsigned int x));
void draw_set_aspect_ycorrection_proc(unsigned int (*ycorrection_proc)(unsigned int y));
unsigned int aspect_xcorrection_games_360(unsigned int x);
unsigned int aspect_ycorrection_games_360(unsigned int y);
void draw_set_environment(unsigned int (*xcorrection_proc)(unsigned int x), 
                                       unsigned int (*ycorrection_proc)(unsigned int y),
                                       int screenx, int screeny );
#endif

//-------------------------------------------------------------------

#endif
