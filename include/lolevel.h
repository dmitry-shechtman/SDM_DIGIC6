#ifndef LOLEVEL_H
#define LOLEVEL_H

#ifndef LOW_LEVEL
#error File can be included when platform is built only!
#endif

#ifndef __arm__
#error File can be included in ARM mode only!
#endif

#define rwd(); asm("ldr r0,=0xC0410004\n" "ldr r1,=0xAA55\n" "str r1,[r0]\n" :::"r0","r1");
#define rwd2();if((shooting_get_tick_count()- WDStart)>=8000){asm("ldr r0,=0xC0410004\n" "ldr r1,=0xAA55\n""str r1,[r0]\n":::"r0","r1");WDStart = shooting_get_tick_count();}
//Canon stuff 
extern long *_GetSystemTime(long *t);
extern long _GetZoomLensCurrentPosition();
extern long _GetZoomLensCurrentPoint();
extern long _MoveZoomLensWithPoint(short *pos);
extern long _SetZoomActuatorSpeedPercent(short *perc);
extern volatile long zoom_busy;
extern long _GetFocusLensCurrentPosition();
extern long _GetFocusLensSubjectDistance();
extern long _GetFocusLensSubjectDistanceFromLens();
extern void _MoveFocusLensToDistance(short *dist); 
extern void _PutInNdFilter();
extern void _PutOutNdFilter();
extern volatile long focus_busy;
extern volatile long zoom_busy;
extern long _GetCurrentAvValue();
extern long _GetCurrentTargetDistance();
extern long _GetPropertyCase(long opt_id, void *buf, long bufsize);
extern long _SetPropertyCase(long opt_id, void *buf, long bufsize);
extern long _IsStrobeChargeCompleted();
extern long _VbattGet();
extern int _GetBatteryTemperature();
extern int _GetCCDTemperature();
extern int _GetOpticalTemperature();
extern void _PT_PlaySound(short , void*);  
extern void _RefreshPhysicalScreen(long f);
extern void _Unmount_FileSystem();
extern void _Mount_FileSystem();
extern void _SleepTask(long msec);
extern int _CreateTask (const char *name, int prio, int stack_size /*?*/, 
void *entry, long parm /*?*/);
extern void _SleepTask(long msec);
extern void __attribute__((noreturn)) _ExitTask();
extern int _taskNameToId(char* taskName); //VxWorks only, task's name first letter must be 't', maximum 10 chars total 
extern const char *_taskName(int taskID); //VxWorks only for now
extern int _taskIdListGet(int *idlist,int max); //VxWorks only for now
extern void _taskSuspend(int taskId); 
extern void _taskResume(int taskId);
extern int _errnoOfTaskGet(int tid);
extern long _IsStrobeChargeCompleted();
extern unsigned _ExecuteEventProcedure(const char *name, ...);
// 7 calls functions and sets some MMIOs, but doesn't disable caches and actually restart
// 3 skips one function call on some cameras, but does restart
void _Restart(unsigned option);
// boot an fir/fi2 file
void _reboot_fw_update(const char* bootfile);

long _SetEventFlag(void *flag, long what);
long _CheckAnyEventFlag(void *flag, long mask, long *res);
long _GetEventFlagValue(void *flag, long *res);

long _ReceiveMessageQueue(void *msgq, long *dst, long unk1 );

//Canon stuff with nonoriginal naming 
extern long _GetParameterData(long id, void *buf, long size);
extern long _SetParameterData(long id, void *buf, long size);
extern void _UpdateMBROnFlash(int driveno, long offset, char *str);

//standart C library 
// extern int _creat (const char *name, int flags);
extern int _open (const char *name, int flags, int mode );
extern int _close (int fd);
extern int _write (int fd, void *buffer, long nbytes);
extern int _read (int fd, void *buffer, long nbytes);
extern int _lseek (int fd, long offset, int whence);
extern long _mkdir(const char *dirname);

extern int _Open (const char *name, int flags, int mode );
extern int _Close (int fd);
extern int _Write (int fd, void *buffer, long nbytes);
extern int _Read (int fd, void *buffer, long nbytes);
extern int _Lseek (int fd, long offset, int whence);
extern int _Remove(const char *name);

extern long _Fopen_Fut(const char *filename, const char *mode);
extern long _Fclose_Fut(long file);
extern long _Fread_Fut(void *buf, long elsize, long count, long f);
extern long _Fwrite_Fut(const void *buf, long elsize, long count, long f);
extern long _Fseek_Fut(long file, long offset, long whence);
extern long _Fflush_Fut(long file);
extern long _Feof_Fut(long file);
extern long _Fflush_Fut(long file);
extern long _RenameFile_Fut(const char *oldname, const char *newname);
extern long _DeleteFile_Fut(const char *name);
extern long _MakeDirectory_Fut(const char *name,int unk);
extern long _DeleteDirectory_Fut(const char *name);
extern char *_Fgets_Fut(char *buf, int n, long f);
// extern long _RenameFile_Fut(const char *oldname, const char *newname);
// extern long _DeleteFile_Fut(const char *name);
extern long _MakeDirectory_Fut(const char *name,int unk);

extern int _rename(const char *oldname, const char *newname);

extern int _isdigit(int c);
extern int _isspace(int c);
extern int _isalpha(int c);
extern int _isupper(int c);
extern int _islower(int c);
extern int _ispunct(int c);
extern int _isxdigit(int c);

extern long _strlen(const char *s);
extern int _strcmp(const char *s1, const char *s2);
extern int _strncmp(const char *s1, const char *s2, long n);
extern char *_strchr(const char *s, int c);
extern char *_strcpy(char *dest, const char *src);
extern char *_strncpy(char *dest, const char *src, long n);
extern char *_strcat(char *dest, const char *app);
extern char *_strrchr(const char *s, int c);
extern char *_strpbrk(const char *s, const char *accept);

extern long _strtol(const char *nptr, char **endptr, int base);

extern int _tolower(int c);
extern int _toupper(int c);

extern int _vsprintf(char *buf, const char *fmt, __builtin_va_list va_list);

extern void *_malloc(long size);
extern void _free(void *p);
extern void *_AllocateUncacheableMemory(long size);
extern void _FreeUncacheableMemory(void *p);

#if defined(OPT_EXMEM_MALLOC) || defined(CAM_USES_EXMEM)
// dryos + some vxworks only takes 3 params in reality
// on some vxworks the function that is easy to match with sig finder takes an additional param,
// set to zero in the real AllocateExMem
// versions that don't expect a 4th param will just ignore it
extern void *_exmem_alloc(int pool_id,int size,int unk,int unk2); 
#endif

// vxworks only
// used on a few cameras that don't have memPartInfoGet, see CAM_NO_MEMPARTINFO
extern int _memPartFindMax(int mempart_id); 
extern int _memPartInfoGet(int mempart_id,int *info);

extern void *_memchr(const void *s, int c, int n);
extern void *_memcpy(void *dest, const void *src, long n);
extern void *_memset(void *s, int c, int n);
extern int _memcmp(const void *s1, const void *s2, long n);

extern void _qsort (void *__base, int __nelem, int __size, int (*__cmp)(const void *__e1, const void *__e2));

//VxWorks 
extern long _taskLock();
extern long _taskUnlock();
extern int _taskCreateHookAdd (void *createHook);
extern int _taskDeleteHookAdd (void *deleteHook);
extern long _iosDevAdd(void*,void*,int);
extern long _iosDrvInstall(void*,void*,void*,void*,void*,void*,void*);
extern void _GiveSemaphore(int sem);

//misc 
extern long physw_sleep_delay;
extern const char aPhysw;
extern long physw_run;
extern long _kbd_p1_f();
extern void _kbd_p2_f();
extern void _kbd_pwr_on();
extern void _kbd_pwr_off();
extern void _kbd_read_keys_r2(void*p);
extern long physw_status[3], physw_copy[3];
extern int flash_duration[3];

void __attribute__((naked,noinline)) mykbd_task();
extern void capt_seq_task();
extern void movie_record_task();
extern void init_file_modules_task();
extern void exp_drv_task();
extern void _ExitTask(void);

void kbd_fetch_data(long *dst);

extern long playrec_mode; //used on S-series only
extern void *led_table;
extern void _UniqueLedOn(void *addr, long brightness);
extern void _UniqueLedOff(void *addr);
struct led_control {int led_num; int action; int brightness; int blink_count;};
extern int _PostLEDMessage(struct led_control *);
int _LEDDrive(int led, int action); 

extern long _LockMainPower();
extern long _UnlockMainPower();
extern void _SetAutoShutdownTime(int t);


//math 
extern int _rand(void);
extern void* _srand(unsigned int seed);

extern double __log(double x);
extern double __log10(double x);
extern double __pow(double x, double y);
extern double __sqrt(double x);

//time 
extern int _utime(const char *file, void *newTimes);
extern unsigned long _time(unsigned long *timer);
extern void *_localtime(const unsigned long *_tod);
extern void *_LocalTime(const unsigned long *_tod, void * t_m); //DRYOS
extern long _strftime(char *s, unsigned long maxsize, const char *format, const  void *timp);
extern long _mktime(void *timp); //VXWORKS
extern long _mktime_ext(void *tim_extp); //DRYOS, doesn't take a struct tm *
#ifdef CAM_DRYOS_2_3_R39
int _SetFileTimeStamp(char *file_path, int time1, int time2);
#else
extern int _SetFileTimeStamp(int fd, int time1, int time2);
#endif
//file 
extern void *_opendir(const char* name);
extern void *_readdir(void *d);
extern int _ReadFastDir(void *d, void* dd); // DRYOS
extern int   _closedir(void *d);
extern void  _rewinddir(void *d);
extern int   _stat(char *name, void *pStat);
extern unsigned long _GetDrive_ClusterSize(int drive);
extern unsigned long _GetDrive_TotalClusters(int drive);
extern unsigned long _GetDrive_FreeClusters(int drive);
extern int _WriteSDCard(unsigned int drive, unsigned int start_sect, unsigned int num_sect, void *buf);
extern void _UnsetZoomForMovie(void);
void _TurnOffMic(void);
void _TurnOnMic(void);

extern void _MakeAFScan(int*, int); 
extern void _ExpCtrlTool_StartContiAE(int, int); 
extern void _ExpCtrlTool_StopContiAE(int, int); 
#if defined(CAMERA_s95)
extern short cdsgain;
extern void _SetCDSGain(void);
extern void _SetDeltaGain(short*);
#endif
extern int some_flag_for_af_scan; 
extern int parameter_for_af_scan; 
extern void _SetAE_ShutterSpeed(short* tv);

extern void _EnterToCompensationEVF(void);
extern void _ExitFromCompensationEVF(void);
extern void _RefreshUSBMode(void);
extern void _PostLogicalEventForNotPowerType(int mode,int z);
extern int  _PostLogicalEventToUI(int event, int unk);
extern void  _DisableNotificationPTP(void);
extern unsigned int mode_magic;
extern void _TurnOnBackLight(void);
extern void _TurnOffBackLight(void);
extern void _TurnOnDisplay(void); 
extern void _TurnOffDisplay(void); 
extern void _UIFS_WriteFirmInfoToFile(int);

//reyalp used to calculate size of loaded CHDK
//these are linker symbols, with no actual data attached!
extern const char _start,_end;
extern int _EngDrvRead(int gpio_reg);
extern void _DoAELock(void);
extern void _UnlockAE(void);
extern void _DoAFLock(void);
extern void _UnlockAF(void);
extern void _MFOn(void);
extern void _MFOff(void);
extern void _PT_MFOn(void);
extern void _PT_MFOff(void);
extern void _PTM_SetCurrentItem(int prop,int state);
extern void _SS_MFOn(void);
extern void _SS_MFOff(void);
extern void _ScreenLock();
extern void _ScreenUnLock();
extern int _apex2us(int);
extern int _EngDrvRead(int);
#ifdef CAM_HAS_GPS
extern void _GPS_UpdateData();
#endif
#if defined(OPT_PTP)
extern int _add_ptp_handler(int, void*, int);
#endif
extern void _SetCurrentCaptureModeType();
extern void _DisableNotificationPTP();
extern void _set_control_event(int);
extern void _PB2Rec();
extern void _Rec2PB();
#ifdef CAM_HAS_GPS
extern void _GPS_UpdateData();
#endif
#ifdef CAMERA_ixus105
extern long _DisableISDriveError();
#endif
#endif
