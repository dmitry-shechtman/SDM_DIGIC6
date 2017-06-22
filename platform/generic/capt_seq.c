// generic capt_seq functions

#include "conf.h"
#include "stdlib.h"
#define RAWDATA_AVAILABLE (1)
#define RAWDATA_SAVED (2)

// some cams use different values
// note: many cams ROM also accepts a value of 3, meaning unknown
#ifndef NR_ON
#define NR_ON (2)
#endif
#ifndef NR_OFF
#define NR_OFF (1)
#endif

extern int gSDMShootMode,gLowLightCount;
extern long FastLapseStart,FastLapseEnd;
static long raw_save_stage;
volatile long shutter_open_time=0; // for DNG EXIF creation
long gDataReady,gShutterOpen;
time_t gDaySeconds;
 
void __attribute__((naked,noinline)) capt_seq_hook_set_nr()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n");
    switch (core_get_noise_reduction_value()){
    case NOISE_REDUCTION_AUTO_CANON:
        // leave it alone
#if defined(NR_AUTO)		     // If value defined store if (e.g. for G12 & SX30 need to reset back to 0 to enable auto)
        *nrflag = NR_AUTO;
#endif
        break;
    case NOISE_REDUCTION_OFF:
        *nrflag = NR_OFF;
        break;
    case NOISE_REDUCTION_ON:
        *nrflag = NR_ON;
        break;
    }
     shutter_open_time=_time((void*)0); 
 
   if(conf.PulseOption==1)
   {
    ubasic_set_led(9,1,100);
    _SleepTask(10);
    ubasic_set_led(9,0,0);
   }
 gDaySeconds=time(NULL);
 int gShutterOpen = (int)shooting_get_tick_count();
 conf.flag_8 = gShutterOpen;
 asm volatile("LDMFD   SP!, {R0-R12,PC}\n");
}

 
void __attribute__((naked,noinline)) capt_seq_hook_raw_here()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n");
 gDataReady=(int)get_tick_count();
long time = get_tick_count();
if((gSDMShootMode==BURST)&&!gLowLightCount)FastLapseEnd=FastLapseStart=time;
else if((gSDMShootMode==BURST)&&gLowLightCount)FastLapseEnd=time;
 gDataReady=(int)time;
#ifdef PAUSE_FOR_FILE_COUNTER
    // Some cameras need a slight delay for the file counter to be updated correctly
    // before raw_savefile tries to get the file name & directory.
 
   if (conf.save_raw) 
    {
        int fc = get_file_counter();
        int tc = get_tick_count() + PAUSE_FOR_FILE_COUNTER;
        while ((get_file_counter() == fc) && (get_tick_count() < tc)) 
        {
         _SleepTask(10);
        }
    }
#endif
    raw_save_stage = RAWDATA_AVAILABLE;
        if(conf.PulseOption==2)
    {
     ubasic_set_led(9,1,100);
     _SleepTask(10);
     ubasic_set_led(9,0,0);
    }
    core_rawdata_available();
    while (raw_save_stage != RAWDATA_SAVED){
	_SleepTask(10);
    }
 asm volatile("LDMFD   SP!, {R0-R12,PC}\n");
}
 
void hook_raw_save_complete()
{
 raw_save_stage = RAWDATA_SAVED;
 if(conf.PulseOption==3)
  {
   ubasic_set_led(9,1,100);
   _SleepTask(10);
   ubasic_set_led(9,0,0);
  }
}



