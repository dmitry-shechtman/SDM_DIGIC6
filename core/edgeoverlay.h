#ifndef EDGE_OVERLAY_H
#define EDGE_OVERLAY_H

#define EDGE_SAVE_DIR "A/SDM/EDGES"
#define XINC 6
#define YINC 2

#if !defined(OPT_NEW_OVERLAY)
//margin in which around the center nothing is displayed. Good for not
//interfering too much with the OSD
#define MARGIN 30
//stuff influencing the algorithm
#if defined (CAMERA_ixus100)
#define CALCYMARGIN 30
#else
#define CALCYMARGIN 3
#endif
#define NSTAGES 4
//steps for up/down/left/right moving the overlay in ALT mode

void edge_overlay();
void save_edge_overlay(void);
void load_edge_overlay( const char* );
void free_memory_edge_overlay(void);
void unlock_overlay();

#else

struct libedgeovr_sym 
{
 int  version;
 void (*edge_overlay)();
 void (*load_edge_overlay)(const char* fn);
 void (*save_edge_overlay)(void);
};

extern struct libedgeovr_sym* libedgeovr;
extern struct libedgeovr_sym* module_edgeovr_load();
extern void module_restore_edge(void **buf, int *state);
extern void module_save_edge(void* buf, int state);
#endif

#endif
