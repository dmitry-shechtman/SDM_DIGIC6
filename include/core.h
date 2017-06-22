#ifndef CORE_H
#define CORE_H
#include "camera.h"
void core_spytask();

void core_hook_task_create(void *tcb);
void core_hook_task_delete(void *tcb);

long kbd_process();

void gui_init();

void conf_restore_finished();
void core_rawdata_available();
void core_spytask_can_start();

#define NOISE_REDUCTION_AUTO_CANON      (0)
#define NOISE_REDUCTION_OFF             (1)
#define NOISE_REDUCTION_ON              (2)

long core_get_noise_reduction_value();
void gui_draw_read(int arg);

#endif

