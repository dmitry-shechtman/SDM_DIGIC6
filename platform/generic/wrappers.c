#include "camera.h"
#include "lolevel.h"
#include "platform.h"
#include "conf.h"
#include "math.h"
#include "stdlib.h"

#if defined(CAMERA_ixus105)
volatile long *printled=(void*)0xC0220108;
#elif defined(CAMERA_s95)
volatile long *printled=(void*)0xC0223030;
#else
volatile long *printled=(void*)0xc0220084;
#endif

int idLog[1000];
int idLogSize;
extern unsigned short gLcdStart;
#if defined CAM_DRYOS
 		#define _U      0x01    /*upper */ 
 		#define _L      0x02    /*lower */ 
 		#define _D      0x04    /*digit */ 
 		#define _C      0x08    /*cntrl */ 
 		#define _P      0x10    /*punct */ 
 		#define _S      0x20    /*white space (space/lf/tab) */ 
 		#define _X      0x40    /*hex digit */ 
 		#define _SP     0x80    /*hard space (0x20) */ 
 		unsigned char _ctype[] = { 
 		_C,_C,_C,_C,_C,_C,_C,_C,                        /*0-7 */ 
 		_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,         /*8-15 */ 
 		_C,_C,_C,_C,_C,_C,_C,_C,                        /*16-23 */ 
 		_C,_C,_C,_C,_C,_C,_C,_C,                        /*24-31 */ 
 		_S|_SP,_P,_P,_P,_P,_P,_P,_P,                    /*32-39 */ 
 		_P,_P,_P,_P,_P,_P,_P,_P,                        /*40-47 */ 
 		_D,_D,_D,_D,_D,_D,_D,_D,                        /*48-55 */ 
 		_D,_D,_P,_P,_P,_P,_P,_P,                        /*56-63 */ 
 		_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,      /*64-71 */ 
 		_U,_U,_U,_U,_U,_U,_U,_U,                        /*72-79 */ 
 		_U,_U,_U,_U,_U,_U,_U,_U,                        /*80-87 */ 
 		_U,_U,_U,_P,_P,_P,_P,_P,                        /*88-95 */ 
 		_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,      /*96-103 */ 
 		_L,_L,_L,_L,_L,_L,_L,_L,                        /*104-111 */ 
 		_L,_L,_L,_L,_L,_L,_L,_L,                        /*112-119 */ 
 		_L,_L,_L,_P,_P,_P,_P,_C,                        /*120-127 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*128-143 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*144-159 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*160-175 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*176-191 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*192-207 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*208-223 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /*224-239 */ 
 		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};               /*240-255 */  
#endif

#if defined(CAMERA_a1000)
#define RECORD 0x1001        
#define PLAYBACK 0x1003      
#define MOVIE 0x105B         

#elif !CAM_HAS_PLAYBACK_PB 
#define RECORD 0x1065       
#define PLAYBACK 0x1061     
#define MOVIE 0x105B        
#elif CAM_HAS_PLAYBACK_PB 
#define RECORD 0x1001    
#define PLAYBACK 0x1003  
#define MOVIE 0x9A5      
#endif

#define LEVENT_INVALID_INDEX 0xFFFFFFFF

#if CAM_MULTIPART
extern int cp;  
#endif
extern int gFrameCount;

static int std_period;
static int current_fps;

void get_standard_period()
{
 #if defined (CAMERA_s100)
 std_period = *(volatile int*)(0x00606014); 	
#else
 std_period = *(volatile int*)(0x00406014); 	
#endif
 current_fps = std_period;
}

void fps(int x)
{
 current_fps+=x;
#if defined (CAMERA_s100)
 *(volatile int*)(0x00606014)= current_fps;	
#else
 *(volatile int*)(0x00406014)= current_fps;	
#endif
 *(volatile int*)(0xC0F06014) = current_fps; 	
 *(volatile int*)(0xC0F06000) = 1;
}

 
void wr_kbd_read_keys_r2(void *status) 
{
  _kbd_read_keys_r2(status);
}

void msleep(long msec)
{
    _SleepTask(msec);
}

#ifndef CAM_DRYOS

long task_lock()
{
    return _taskLock();
}

long task_unlock()
{
    return _taskUnlock();
}

const char *task_name(int id)
{
    return _taskName(id);
}

int task_id_list_get(int *idlist,int size)
{
    return _taskIdListGet(idlist,size);
}

void remount_filesystem()
{
    _Unmount_FileSystem();
    _Mount_FileSystem();
}

#endif

long get_property_case(long id, void *buf, long bufsize)
{
#if defined(CAMERA_g7x) || defined(CAMERA_m3)
    if(id==PROPCASE_SHOOTING) 
     {
        int r=_GetPropertyCase(PROPCASE_SHOOTING_STATE, buf, bufsize);
        if(*(char *)buf > 1) 
         {
          *(char *)buf = 1;
         } 
        else 
         {
          *(char *)buf = 0;
         }
        return r;
     }
#endif
    return _GetPropertyCase(id, buf, bufsize);
}

long set_property_case(long id, void *buf, long bufsize)
{
    
#if defined(CAMERA_g7x) || defined(CAMERA_m3)
  if(id==PROPCASE_SHOOTING) 
  {
    return 0;
  }
#endif
    return _SetPropertyCase(id, buf, bufsize);
}

void __attribute__((weak)) vid_bitmap_refresh()   
{
    _RefreshPhysicalScreen(1);
}

long get_parameter_data(long id, void *buf, long bufsize)
{
    return _GetParameterData(id|0x4000, buf, bufsize);
}

long set_parameter_data(long id, void *buf, long bufsize)
{
    return _SetParameterData(id|0x4000, buf, bufsize);
}

long lens_get_zoom_pos()
{
#if !defined(CAMERA_m3)
    return _GetZoomLensCurrentPosition();
#else
  return 0;
#endif
}

long lens_get_zoom_point()
{
#if !defined(CAMERA_m3)
    return _GetZoomLensCurrentPoint();
#else
 return 0;
#endif
}

int is_zoom_busy()
{
#if !defined(CAMERA_m3)
  return zoom_busy;
#else
 return 0;
#endif
}

void lens_set_zoom_point(long newpt)
{
#if !defined(CAMERA_m3)
    if (newpt < 0) {
        newpt = 0;
    } else if (newpt >= zoom_points) {
        newpt = zoom_points-1;
    }

#if defined(CAM_USE_ALT_SET_ZOOM_POINT)

	if (lens_get_zoom_point() != newpt)
	{
		//Get current digital zoom mode & state
		//state == 1 && mode == 0 --> Digital Zoom Standard
		int digizoom_pos;
		get_property_case(PROPCASE_DIGITAL_ZOOM_POSITION,&digizoom_pos,sizeof(digizoom_pos));
	
	
		if ((shooting_get_digital_zoom_state() == 1) && (shooting_get_digital_zoom_mode() == 0) && (digizoom_pos != 0))
		{
		//reset digital zoom in case camera is in this zoom range	
			extern void _PT_MoveDigitalZoomToWide();
			_PT_MoveDigitalZoomToWide();
		}

  #if defined(CAM_USE_ALT_PT_MoveOpticalZoomAt)
	//SX30 - _MoveZoomLensWithPoint crashes camera
	//PT_MoveOpticalZoomAt works, and updates PROPCASE_OPTICAL_ZOOM_POSITION; but doesn't wait for zoom to finish
        //IXUS220, SX220/230 - _MoveZoomLensWithPoint does not notify the JPEG engine of the new focal length,
        //causing incorrect lens distortion fixes to be applied; _PT_MoveOpticalZoomAt works     
		extern void _PT_MoveOpticalZoomAt(long*);
		_PT_MoveOpticalZoomAt(&newpt);
  #else
	    _MoveZoomLensWithPoint((short*)&newpt);
  #endif	
		//have to sleep here, zoom_busy set in another task, without sleep this will hang
		while (zoom_busy) msleep(10);	
		//g10,g12 & sx30 only use this value for optical zoom
		zoom_status=ZOOM_OPTICAL_MAX;

  #if !defined(CAM_USE_ALT_PT_MoveOpticalZoomAt) 
	    _SetPropertyCase(PROPCASE_OPTICAL_ZOOM_POSITION, &newpt, sizeof(newpt));
  #endif
	}
#else
    _MoveZoomLensWithPoint((short*)&newpt);
    //tight loop here hangs some cameras (the task that clears zoom_busy
    //is starved; seen at least on S95 and IXUS 220), so stick in a sleep    
    while (zoom_busy) msleep(10);

    if (newpt==0) zoom_status=ZOOM_OPTICAL_MIN;
    else if (newpt >= zoom_points) zoom_status=ZOOM_OPTICAL_MAX;
    else zoom_status=ZOOM_OPTICAL_MEDIUM;
    _SetPropertyCase(PROPCASE_OPTICAL_ZOOM_POSITION, &newpt, sizeof(newpt));
#endif
#endif
}
 

void lens_set_focus_pos(long newpos)    
{
#if !defined(CAMERA_m3)
 if(shooting_can_focus())
  {
    if (newpos >= MAX_DIST) newpos = INFINITY_DIST; //Set to 0xFFFFFFFF that will work on all cameras
    _MoveFocusLensToDistance((short*)&newpos);
    while ((shooting_is_flash_ready()!=1) || (focus_busy)) msleep(10);
    newpos = _GetFocusLensSubjectDistance();
    _SetPropertyCase(PROPCASE_SUBJECT_DIST1, &newpos, sizeof(newpos));
    _SetPropertyCase(PROPCASE_SUBJECT_DIST2, &newpos, sizeof(newpos));
  }
#endif
}

long stat_get_vbatt()
{
    return _VbattGet();
}

long get_tick_count()
{
   long t;
#if !CAM_DRYOS
    _GetSystemTime(&t);
    return t;
#else 
    return (int)_GetSystemTime(&t); 
#endif 
}

int open (const char *name, int flags, int mode )
{
#if !CAM_DRYOS
    
    
    flags = (flags & ~(O_TRUNC|O_CREAT|O_APPEND)) | ((flags & (O_TRUNC|O_CREAT)) << 1);
#else
    if(!name || name[0]!='A')return -1; 
#endif 
#if defined(CAM_STARTUP_CRASH_FILE_OPEN_FIX)    //enable fix for camera crash at startup when opening the conf / font files 
                                                //see http://chdk.setepontos.com/index.php?topic=6179.0 
 #define O_RDONLY 0                             //copied from stdlib.h (including stdlib.h causes compile errors due to function definition mismatch - needs fixing) 
  if (flags == O_RDONLY)                        //At startup opening the conf / font files conflicts with Canon task if use _Open. Camera can randomly crash. 
  return _open(name, flags, mode); 
 #endif
    return _Open(name, flags, mode);
}

int close (int fd)
{
    return _Close(fd);
}
int write (int fd, void *buffer, long nbytes)
{
    return _Write(fd, buffer, nbytes);
}
int read (int fd, void *buffer, long nbytes)
{
    return _Read(fd, buffer, nbytes);
}
int lseek (int fd, long offset, int whence)
{
    return _lseek(fd, offset, whence); /*yes, it's lower-case lseek here since Lseek calls just lseek (A610) */
}

long mkdir(const char *dirname)
{
#ifdef MKDIR_RETURN_ONE_ON_SUCCESS
    
    if(_MakeDirectory_Fut(dirname,1)) return 0;
    else                              return 1;
#else
    return _MakeDirectory_Fut(dirname,-1); 
#endif
}

int remove(const char *name)
{
#ifdef CAM_DRYOS_2_3_R39
    // For DryOS R39 and later need to check if 'name' is a file or directory
    // and call appropriate delete function.
    struct stat st;
    if (stat(name,&st) == 0)
    {
        if (st.st_attrib & DOS_ATTR_DIRECTORY)
        	return _DeleteDirectory_Fut(name);
        else
        	return _DeleteFile_Fut(name);
    }
    return -1;  
#else
    return _DeleteFile_Fut(name);
#endif
}

long DeleteFile_Fut(const char *name)
{
  return 0;
}

int isdigit(int c) {
#if !CAM_DRYOS
    return _isdigit(c);
#else 
    return _ctype[c]&_D; 
#endif 
}

int isspace(int c) {
#if !CAM_DRYOS
    return _isspace(c);
#else 
    return _ctype[c]&_S; 
#endif 
}

int isalpha(int c) {
#if !CAM_DRYOS
    return _isalpha(c);
#else 
    return _ctype[c]&(_U|_L); 
#endif 
}

int isupper(int c) {
#if !CAM_DRYOS
    return _isupper(c);
#else 
    return _ctype[c]&_U; 
#endif 
}

long strlen(const char *s) {
    return _strlen(s);
}

int strcmp(const char *s1, const char *s2) {
    return _strcmp(s1, s2);
}

int strncmp(const char *s1, const char *s2, long n) {
    return _strncmp(s1, s2, n);
}

char *strchr(const char *s, int c) {
    return _strchr(s, c);
}

char *strcpy(char *dest, const char *src) {
    return _strcpy(dest, src);
}

char *strncpy(char *dest, const char *src, long n) {
    return _strncpy(dest, src, n);
}

char *strcat(char *dest, const char *app) {
    return _strcat(dest, app);
}

char *strrchr(const char *s, int c) {
    return _strrchr(s, c);
}

long strtol(const char *nptr, char **endptr, int base) {
    return _strtol(nptr, endptr, base);
}

char *strpbrk(const char *s, const char *accept) {
#if !CAM_DRYOS
    return _strpbrk(s, accept);
#else 
 	const char *sc1,*sc2; 
 	 
 		    for( sc1 = s; *sc1 != '\0'; ++sc1) { 
 		     for( sc2 = accept; *sc2 != '\0'; ++sc2) { 
 		      if (*sc1 == *sc2) return (char *) sc1; 
 		     } 
 		    } 
 		return (void*)0; 
#endif 
}

long sprintf(char *s, const char *st, ...)
{
    long res;
    __builtin_va_list va;
    __builtin_va_start(va, st);
    res = _vsprintf(s, st, va);
    __builtin_va_end(va);
    return res;
}

void xmlPrintf(int fileDescriptor,char *name,char *format, ...)
{
 char xmlLine[80];
 if(fileDescriptor>=0)
  {
   __builtin_va_list va;
   __builtin_va_start(va,format);
    sprintf(xmlLine,"<property>\n\t<name>%s</name>\n\t<value>",name);
    write(fileDescriptor,xmlLine,strlen(xmlLine));
   _vsprintf(xmlLine,format, va);
    write(fileDescriptor,xmlLine,strlen(xmlLine));
    sprintf(xmlLine,"</value>\n</property>\n");
    write(fileDescriptor,xmlLine,strlen(xmlLine));
   __builtin_va_end(va);
  }
}

void csvPrintf(int fileDescriptor,char *format, ...)
{
 char csvLine[80];
 if(fileDescriptor>=0)
  {
   __builtin_va_list va;
   __builtin_va_start(va,format);
   _vsprintf(csvLine,format, va);
   write(fileDescriptor,csvLine,strlen(csvLine));
   __builtin_va_end(va);
  }
}

unsigned long time(unsigned long *timer) {
    return _time(timer);
}

int utime(const char *file, struct utimbuf *newTimes) {
#if !CAM_DRYOS
  return _utime(file, newTimes);
#else
 int res=0;
 int fd;
 fd = _open(file, 0, 0);

#ifdef CAM_DRYOS_2_3_R39
   if (fd>=0) {
       _close(fd);
       res=_SetFileTimeStamp((char*)file, ((int*)newTimes)[0] , ((int*)newTimes)[1]);
   }
#else
     if (fd>=0) {
      res=_SetFileTimeStamp((int)fd, ((int*)newTimes)[0] , ((int*)newTimes)[1]);
      _close(fd);
     }
     
#endif
  return (res)?0:-1;
#endif
}

struct tm *localtime(const unsigned long *_tod) {
#if !CAM_DRYOS
    return _localtime(_tod);
#else
//for DRYOS cameras do something with this!  - sizeof(x[]) must be >= sizeof(struct tm) :  'static int x[9];'
  static int x[9];
  return _LocalTime(_tod, &x);
#endif
}

long strftime(char *s, unsigned long maxsize, const char *format, const struct tm *timp) 
{
 return _strftime(s,maxsize,format,timp);
}
struct tm *get_localtime()
{
 time_t t = time(NULL);
 return localtime(&t);
}
 

double _log(double x) {
    return __log(x);
}

double _log10(double x) {
    return __log10(x);
}

double _pow(double x, double y) {
    return __pow(x, y);
}

double _sqrt(double x) {
    return __sqrt(x);
}

#if !defined(CAMERA_sx130is)&& !defined(CAMERA_ixus115) && !defined(CAMERA_ixus220)&& !defined(CAMERA_sx150is)&& !defined(CAMERA_sx160is)&& !defined(CAMERA_sx220hs)&& !defined(CAMERA_sx230hs)&& !defined(CAMERA_a495)&& !defined(CAMERA_a800)&& !defined(CAMERA_a3200)&& !defined(CAMERA_a810)&& !defined(CAMERA_a2200)&& !defined(CAMERA_a2300)&& !defined(CAMERA_a2400)&& !defined(CAMERA_a3400)&& !defined(CAMERA_a4000)
#if defined(CAM_USES_EXMEM)
 void *mem_exmem_heap;

 		//I set this up to 16 mb and it still booted... 
 		#define EXMEM_HEAP_SIZE (1024*1024*8) 
		 
            static  void *exmem_heap;	 
 		void *suba_init(void *heap, unsigned size, unsigned rst, unsigned mincell); 
 		void *suba_alloc(void *heap, unsigned size, unsigned zero); 
 		int suba_free(void *heap, void *p); 
 		 
 		void exmem_malloc_init() 
            { 
 		        
 		        void *mem = _exmem_alloc(0,EXMEM_HEAP_SIZE,0,0);
 		        if(mem) 
                     { 
                      mem_exmem_heap = mem;
 		          exmem_heap = suba_init(mem,EXMEM_HEAP_SIZE,1,1024); 
 		         } 
 		} 
 		 
 		void *malloc(long size) 
            { 
 		        if(exmem_heap) 
 		             return suba_alloc(exmem_heap,size,0); 
 		        else 
 		             return _malloc(size); 
 		}
 
 		void free(void *p) 
            { 
 		        if(exmem_heap) 
 		             suba_free(exmem_heap,p); 
 		        else 
                        {
 		             _free(p); 
                        }
                    
 		} 
 	
#else 
     

void *malloc(long size) 
{
    return _malloc(size);
}

void free(void *p) 
{
    return _free(p);
}

#endif
#else

#ifdef OPT_EXMEM_MALLOC
//I set this up to 16 mb and it still booted...
#ifndef EXMEM_HEAP_SKIP
#define EXMEM_HEAP_SKIP 0
#endif
#ifndef EXMEM_BUFFER_SIZE
#define EXMEM_BUFFER_SIZE (1024*1024*2) //default size if not specified by camera
#endif
#define EXMEM_HEAP_SIZE (EXMEM_BUFFER_SIZE+EXMEM_HEAP_SKIP)	//desired space + amount to skip for the movie buffers (if needed)

static void *exmem_heap;
void *exmem_start = 0, *exmem_end = 0;
int exmem_size = 0;

void *suba_init(void *heap, unsigned size, unsigned rst, unsigned mincell);
void *suba_alloc(void *heap, unsigned size, unsigned zero);
int suba_free(void *heap, void *p);

void exmem_malloc_init() {

	void *mem = _exmem_alloc(0,EXMEM_HEAP_SIZE,0,0);
	if(mem) {
#if defined(OPT_CHDK_IN_EXMEM)
		//If loading CHDK into exmem then move heap start past the end of CHDK
		//and reduce available space by CHDK size (MEMISOSIZE)
		//round MEMISOSIZE up to next 4 byte boundary if needed (just in case)
		exmem_start = mem + ((MEMISOSIZE+3)&0xFFFFFFFC);
		exmem_size = EXMEM_BUFFER_SIZE - ((MEMISOSIZE+3)&0xFFFFFFFC);
#else
		//Set start & size based on requested values
		exmem_start = mem;
		exmem_size = EXMEM_BUFFER_SIZE;
#endif
		exmem_end = exmem_start + exmem_size;
#if defined(OPT_EXMEM_TESTING)
		//For testing exmem allocated memory for corruption from normal camera operations
		//set the above #define. This will allocate the memory; but won't use it (exmem_heap is set to 0)
		//Instead all the memory is filled with the guard value below.
		//In gui_draw_debug_vals_osd (gui.c) the memory is tested for the guard value and if any
		//corruption has occurred then info about the memory locations that were altered is displayed
		//If OPT_EXMEM_TESTING is defined then OPT_CHDK_IN_EXMEM should not be set.
		unsigned long *p;
		for (p=(unsigned long*)exmem_start; p<(unsigned long*)exmem_end; p++) *p = 0xDEADBEEF;
		exmem_heap = 0;
#else
		//Normal operation, use the suba allocation system to manage the memory block
		exmem_heap = suba_init(exmem_start,exmem_size,1,8);
#endif
	}
}

void *malloc(long size) {
	if(exmem_heap)
		return suba_alloc(exmem_heap,size,0);
	else
		return _malloc(size);
}
void free(void *p) {
	if(exmem_heap && (p >= exmem_heap))
		suba_free(exmem_heap,p);
	else
		_free(p);
}

//regular malloc
#else
void *malloc(long size) {
    return _malloc(size);
}

void free(void *p) {
    return _free(p);
}
#endif
#endif

void *memcpy(void *dest, const void *src, long n) {
    return _memcpy(dest, src, n);
}

void *memset(void *s, int c, int n) {
    return _memset(s, c, n);
}

int memcmp(const void *s1, const void *s2, long n) {
    return _memcmp(s1, s2, n);
}

int rand(void) {
    return _rand();
}

int rename(const char *oldname, const char *newname) 
{
 return _RenameFile_Fut(oldname, newname);
}

void *srand(unsigned int seed) {
    return _srand(seed);
}

void qsort(void *__base, int __nelem, int __size, int (*__cmp)(const void *__e1, const void *__e2)) {
    _qsort(__base, __nelem, __size, __cmp);
}

DIR *opendir(const char* name)
{
    //Create CHDK DIR structure
    DIR *dir = malloc(sizeof(DIR));
    //If malloc failed return failure
    if (dir == 0) return NULL;

    //Save camera internal DIR structure (we don't care what it is)
#if defined(CAM_DRYOS)
    extern void *_OpenFastDir(const char* name);
    dir->cam_DIR = _OpenFastDir(name);
#else
    extern void *_opendir(const char* name);
    dir->cam_DIR = _opendir(name);
#endif
    //Init readdir return value
    dir->dir.name[0] = 0;

    //If failed clean up and return failure
    if (!dir->cam_DIR)
    {
        free(dir);
        return NULL;
    }

    return dir;
}


#ifndef CAM_DRYOS

//Internal VxWorks dirent structure returned by readdir
struct __dirent
{
    char            name[100];
};

extern void *_readdir(void *d);

struct dirent* readdir(DIR *d)
{
    if (d && d->cam_DIR)
    {
        //Get next entry from firmware function
        struct __dirent *de = _readdir(d->cam_DIR);
        //Return next directory name if present, else return 0 (end of list)
        if (de)
        {
            strcpy(d->dir.name,de->name);
            return &d->dir;
        }
        else
        {
            d->dir.name[0] = 0;
        }
    }
    return NULL;
}

#else //dryos

extern int _ReadFastDir(void *d, void* dd); 

struct dirent * readdir(DIR *d)
{
    if (d && d->cam_DIR)
    {
        _ReadFastDir(d->cam_DIR, d->dir.name);
        return d->dir.name[0]? &d->dir : NULL;
    }
    return NULL;
}

#endif 

int closedir(DIR *d)
{
    int rv = -1;
    if (d && d->cam_DIR)
    {
        rv = _closedir(d->cam_DIR);
        
        d->cam_DIR = 0;
        
        free(d);    
    }
    return rv;
}

long Fopen_Fut(const char *filename, const char *mode){
 return _Fopen_Fut(filename,mode);
}

long Fclose_Fut(long file){
 return _Fclose_Fut(file);
}

long Fread_Fut(void *buf, long elsize, long count, long f){
 return _Fread_Fut(buf,  elsize,  count, f);
}

long Fwrite_Fut(const void *buf, long elsize, long count, long f){
  return _Fwrite_Fut(buf, elsize, count, f);
}

long Fseek_Fut(long file, long offset, long whence){
 return  _Fseek_Fut(file, offset, whence);
}

long Fflush_Fut(long file) {
 return _Fflush_Fut(file);
}

long RenameFile_Fut(const char *oldname, const char *newname) 
{
 return 0;
}

int stat(const char *name, struct stat *pStat)
{
    // sanity check. canon firmware hangup if start not from 'A/'    
    if ( !name || (name[0] | 0x20)!='a' || name[1]!='/' ) return 1;
     // use temp stat stucture to match camera firmware
    // and copy values across to output
    struct stat lStat;
    int rv = _stat((char*)name, &lStat);
    if (pStat)
    {
        if (rv == 0)
        {
            pStat->st_attrib = lStat.st_attrib;
            pStat->st_size = lStat.st_size;
            pStat->st_ctime = lStat.st_ctime;
            pStat->st_mtime = lStat.st_mtime;
        }
        else
        {
            memset( pStat, 0, sizeof(struct stat));
        }
    }
    return rv;
}

void *umalloc(long size) {
    return _AllocateUncacheableMemory(size);
}

void ufree(void *p) {
    return _FreeUncacheableMemory(p);
}

static int shutdown_disabled = 0;
void disable_shutdown() {
    if (!shutdown_disabled) {
        _LockMainPower();
        shutdown_disabled = 1;
    }
}

void enable_shutdown() {
    if (shutdown_disabled) {
        _UnlockMainPower();
        shutdown_disabled = 0;
    }
}

int get_sd_status()
{
 return shutdown_disabled;
}

void TurnOnBackLight(void)
{
 _TurnOnBackLight();
}

void TurnOffBackLight(void)
{
 _TurnOffBackLight();
}

void TurnOnDisplay(void) 
{ 
 _TurnOnDisplay(); 
} 
	 
void TurnOffDisplay(void) 
{ 
 _TurnOffDisplay(); 
} 

void DoAELock(void)
{
  if ((mode_get() & MODE_MASK) != MODE_PLAY)
  {
     _DoAELock();
  }
}

void UnlockAE(void)
{
  if ((mode_get() & MODE_MASK) != MODE_PLAY)
  {
     _UnlockAE();
  }
}

void DoAFLock(void)
{
  if ((mode_get() & MODE_MASK) != MODE_PLAY)
  {
     int af_lock=1;
     _DoAFLock();
     set_property_case(PROPCASE_AF_LOCK,&af_lock,sizeof(af_lock));
  }
}

void UnlockAF(void)
{
  if ((mode_get() & MODE_MASK) != MODE_PLAY)
  {
     int af_lock=0;
     _UnlockAF();
     set_property_case(PROPCASE_AF_LOCK,&af_lock,sizeof(af_lock));
  }
}

void CloseShutter()
{
#if defined(CAM_DRYOS)
 char* action = "CloseMechaShutter";
#else
  char* action = "CloseMShutter";
#endif
 _ExecuteEventProcedure(action);
 play_sound(4);
}

void OpenShutter()
{
 #if defined(CAM_DRYOS)
 char* action = "OpenMechaShutter";
#else
  char* action = "OpenMShutter";
#endif
 _ExecuteEventProcedure(action);
 play_sound(3);
}

void __attribute__((weak)) vid_turn_off_updates()
{
}

void __attribute__((weak)) vid_turn_on_updates()
{
}

#if defined(CAM_HAS_IS)
void __attribute__((weak)) LeftShift()
{
}

void __attribute__((weak)) RightShift()
{
}
#endif

unsigned int GetFreeCardSpaceKb(void)
{
 return (_GetDrive_FreeClusters(0)*(_GetDrive_ClusterSize(0)>>9))>>1;
}

unsigned int GetTotalCardSpaceKb(void)
{
 return (_GetDrive_TotalClusters(0)*(_GetDrive_ClusterSize(0)>>9))>>1;
}

unsigned int GetJpgCount(void)
{
 return strtol(camera_jpeg_count_str(),((void*)0),0);
}

unsigned int GetRawCount(void)
{
 return GetFreeCardSpaceKb()/((hook_raw_size() / 1024)+GetFreeCardSpaceKb()/GetJpgCount());
}

void record_mode()
{
#if defined(NEW_REC_PLAY)
PostLogicalEventToUI(4097,0);
 msleep(300);
PostLogicalEventToUI(4098,0);
#else
 #if !defined(CAMERA_a1000)
 PostLogicalEventForNotPowerType((int)PLAYBACK,0);
 #endif
 msleep(10);
 PostLogicalEventForNotPowerType((int)RECORD,0);
 msleep(10);
#endif
}

void playback_mode()
{
#if defined(NEW_REC_PLAY)
 PostLogicalEventToUI(4099,0);
 PostLogicalEventToUI(4100,0);
#else
 PostLogicalEventForNotPowerType((int)PLAYBACK,0);
#endif
}

void movie_mode()
{
#if defined(CAMERA_s100)
shooting_set_mode_chdk(9);
#elif defined(NEW_MOVIE)
shooting_set_mode_canon(2598);
#elif defined CAM_MOVIE_PLE 
int old=mode_magic;
 PostLogicalEventForNotPowerType((int)MOVIE,0);
 msleep(10);
 mode_magic=old;
#endif
}

void RefreshUSBMode(void)
{
  _RefreshUSBMode();                                                     
}

void PostLogicalEventForNotPowerType(int mode,int unk)
{
 _PostLogicalEventForNotPowerType(mode,0);
}

void PostLogicalEventToUI(unsigned id, unsigned x) 
{
 _PostLogicalEventToUI(id,x);
}

#if CAM_MULTIPART

#define SECTOR_SIZE 512
char *mbr_buf;
static unsigned long drive_sectors;

int mbr_read(char* mbr_sector, unsigned long drive_total_sectors, unsigned long *part_start_sector,  unsigned long *part_length){
//return value: 1 - success, 0 - fail
 
 int offset=0x10; 
 int valid;
  
 if ((mbr_sector[0x1FE]!=0x55) || (mbr_sector[0x1FF]!=0xAA)) return 0; //signature check 

 mbr_buf=_AllocateUncacheableMemory(SECTOR_SIZE);
 _memcpy(mbr_buf,mbr_sector,SECTOR_SIZE);
 drive_sectors=drive_total_sectors;

 while(offset>=0) {
 
  *part_start_sector=(*(unsigned short*)(mbr_sector+offset+0x1C8)<<16) | *(unsigned short*)(mbr_sector+offset+0x1C6); 
  *part_length=(*(unsigned short*)(mbr_sector+offset+0x1CC)<<16) | *(unsigned short*)(mbr_sector+offset+0x1CA); 

  valid= (*part_start_sector) && (*part_length) &&
         (*part_start_sector<=drive_total_sectors) && 
         (*part_start_sector+*part_length<=drive_total_sectors) &&
         ((mbr_sector[offset+0x1BE]==0) || (mbr_sector[offset+0x1BE]==0x80)); //status: 0x80 (active) or 0 (non-active)

  if (valid && ((mbr_sector[0x1C2+offset]==0x0B) || (mbr_sector[0x1C2+offset]==0x0C))) break;   //FAT32 secondary partition

  offset-=0x10;

 }

 return valid;
}

int mbr_read_dryos(unsigned long drive_total_sectors, char* mbr_sector ){
//Called only in DRYOS
 mbr_buf=_AllocateUncacheableMemory(SECTOR_SIZE);
 _memcpy(mbr_buf,mbr_sector,SECTOR_SIZE);
 drive_sectors=drive_total_sectors;
 return drive_total_sectors;
}

int get_part_count(void)
{
 unsigned long part_start_sector, part_length;
 char part_status, part_type;
 int i;
 int count=0;
 for (i=0; i<=1;i++){
  part_start_sector=(*(unsigned short*)(mbr_buf+i*16+0x1C8)<<16) | *(unsigned short*)(mbr_buf+i*16+0x1C6); 
  part_length=(*(unsigned short*)(mbr_buf+i*16+0x1CC)<<16) | *(unsigned short*)(mbr_buf+i*16+0x1CA); 
  part_status=mbr_buf[i*16+0x1BE];
  part_type=mbr_buf[0x1C2+i*16];
  if ( part_start_sector && part_length && part_type && ((part_status==0) || (part_status==0x80)) ) count++;
 }
 return count;
}

void swap_partitions(void)
{
 int i;
 char c;
 for(i=0;i<16;i++){
  c=mbr_buf[i+0x1BE];
  mbr_buf[i+0x1BE]=mbr_buf[i+0x1CE];
  mbr_buf[i+0x1CE]=c;
 }
 _WriteSDCard(0,0,1,mbr_buf);
 play_sound(4);
 if(cp==1)cp=2;
 else cp=1;
}

#endif

int synch_led(int x)
{
 return x; 
}

int afLedOff(int x)
{
 static int movieTv,movieISO;
 printled[0]|=1<<1;                
 return x; 
}

void logID(volatile int* x)
{
 asm volatile ("STMFD SP!, {R0-R11,LR}\n");
 if(idLogSize<996)
 {
  idLog[idLogSize++]=get_tick_count();
  idLog[idLogSize++]= *x;
 }
 printled[0]^=1<<1;
 asm volatile ("LDMFD SP!, {R0-R11,LR}\n");
}

void movie_switch()
{
 asm volatile ("STMFD SP!, {R0-R11,LR}\n");
 shooting_set_mode_chdk(9);
 asm volatile ("LDMFD SP!, {R0-R11,LR}\n");
}

#define USB_MASK (0x4000000)  		

void rw()
{
 asm volatile("ldr r0,=0xC0410004\n" "ldr r1,=0xAA55\n" "str r1,[r0]\n" :::"r0","r1");
}

void video_wait()
{
}

extern volatile enum _VideoMode VideoMode;
extern unsigned char displayedMode;

#if CAM_CAN_MUTE_MICROPHONE
int mute_on_zoom(int x)             
{
 static int old_busy=0;
 int busy=zoom_busy||focus_busy;
 if (conf.mute_on_zoom && (old_busy!=busy)) 
 { 
  if (busy) _TurnOffMic(); else _TurnOnMic();
  old_busy=busy;
 }
      gFramesRecorded++;
if((gFramesRecorded==30)&&conf.camera_position&&(displayedMode==6))play_sound(4);

if(VideoMode==VIDEO_BURST){if((gFrameCount--)<2)gStopVideoCapture=1;}
 return x; 
}
#endif

int video_frame_counter(int x)             
{
      gFramesRecorded++;
if((gFramesRecorded==30)&&conf.camera_position&&(displayedMode==6))play_sound(4);

if(VideoMode==VIDEO_BURST){if((gFrameCount--)<2)gStopVideoCapture=1;}
 return x; 
}

long __attribute__((weak)) get_jogdial_direction(void)
{
 return 0;
}

	void play_sound(short sound) 
 		{ 
 		    short tmp = 0xC211; 
 		    switch (sound) 
 		{ 
 		  case 0: 
 		    tmp = 0x2001; //startup sound 
 		    break; 
 		  case 1: 
 		    tmp = 0x2002; //shutter sound 
 		    break; 
 		  case 2: 
 		    tmp = 0x2003; //button press sound 
 		    break; 
 		  case 3: 
 		    tmp = 0x2004; //self-timer sound 
 		    break; 
 		  case 4: 
 		   tmp = 0xC211; //short beep 
 		    break; 
 		  case 5: 
 		   tmp = 50000; //AF confirmation  
 		    break; 
 		  case 6: 
 		   tmp = 0xC507; //error beep imo 
 		    break; 
 		  case 7: 
 		   tmp = 0x400D; //LONG ERROR BEEP CONTINIUOUS- warning, cannot be stopped (yet) 
 		    break; 
 		 } 
 		    _PT_PlaySound(tmp, 0); 
 		} 
 #if defined(OPT_PTP)		 
 int add_ptp_handler(int opcode, ptp_handler handler, int unknown)
{
  return _add_ptp_handler(opcode,handler,unknown);
}

//this would make more sense in generic/main.c but not all a cameras use it
void init_chdk_ptp_task() {
  _CreateTask("InitCHDKPTP", 0x19, 0x200, init_chdk_ptp, 0);
};

#endif

void ExitTask(void)
{
 _ExitTask();
}

#ifdef CAM_DRYOS
int __attribute__((weak)) switch_mode_usb(int mode)
{
 if ( mode == 0 ) 
  {
   _Rec2PB();
   _set_control_event(0x80000000|CAM_USB_EVENTID); 
  } 
 else if ( mode == 1 ) 
  {
   _set_control_event(CAM_USB_EVENTID); 
   _PB2Rec();
  } 
 else return 0;
 return 1;
}
#endif

//TODO not in sigs for vx yet
#ifndef CAM_DRYOS
void __attribute__((weak)) _reboot_fw_update(const char *fw_update)
{
	return;
}
#endif

int GetMechaPos()                          
{
return 0;
}
 
void save_romlog()
{
 static unsigned char registered;
 if (stat("A/ROMLOG.LOG",0)    == 0) remove("A/ROMLOG.LOG");
 if (stat("A/RomLogErr.txt",0) == 0) remove("A/RomLogErr.txt");
_ExecuteEventProcedure("System.Create");
_ExecuteEventProcedure("GetLogToFile","A/ROMLOG.LOG",1);
}

int reboot(const char *bootfile) 
{
 #if !defined (CAM_DRYOS)
 bootfile == (char *)NULL;
 #endif
	if(bootfile == NULL) 
       {
	  if(_ExecuteEventProcedure("DispDev_EnableEventProc") == -1) 
         {
	    return 0;
	   }
	  if(_ExecuteEventProcedure("DispCon_TurnOffDisplay") == -1) 
         {
	    return 0;
	   }
	    _Restart(0);
	 }
   else
    {
	int namelen=strlen(bootfile);
	if(namelen > 3 && (strncmp(bootfile + namelen - 4,".FI",3) == 0)) 
       {
		_reboot_fw_update(bootfile);
		//if _reboot_fw_update returns, it failed or is not implemented
		return 0;
	 }
    }
  return 0; 
}

int __attribute__((weak)) vid_get_viewport_yscale() 
{
 return 1;               
}

//use _GetFocusLensSubjectDistance for this on dryos, vx functions are basically equivlent
//not used in CHDK currently for either OS

#ifdef CAM_DRYOS
long __attribute__((weak)) _GetCurrentTargetDistance()
{
#if !defined(CAMERA_m3)
	return _GetFocusLensSubjectDistance();
#else
return 0;
#endif
}
#endif

//TODO this belongs lib.c, but not all cameras include it
//same as bitmap width for most cameras, override in platform/sub/lib.c as needed

int __attribute__((weak)) vid_get_viewport_width() 
{
 return vid_get_bitmap_width();
}

long GetCurrentAvValue()            { return _GetCurrentAvValue(); }

#ifdef OPT_EXT_TV_RANGE
int  apex2us(int apex_tv)
{
	short tv;
	tv = shooting_get_tv96();
	if (tv<-576 || tv!=apex_tv) return 1000000.0*pow(2.0, -tv/96.0);
	else return _apex2us(apex_tv);
}
#endif

int __attribute__((weak)) vid_get_viewport_xoffset() 
{
	return 0;
}

int __attribute__((weak)) vid_get_viewport_yoffset() 
{
	return 0;
}

int __attribute__((weak)) vid_get_viewport_display_xoffset() 
{
	return vid_get_viewport_xoffset();
}

int __attribute__((weak)) vid_get_viewport_display_yoffset() 
{
	return vid_get_viewport_yoffset();
}

int __attribute__((weak)) vid_get_viewport_byte_width() 
{
	return 720 * 6 / 4;     
}

void *vid_get_viewport_active_buffer()
{
  void *p;

  if ( (mode_get()&MODE_MASK) == MODE_PLAY )
  {
    p = vid_get_viewport_fb_d();
  } else {
    p = vid_get_viewport_live_fb();
    if ( !p )
    {
      p = vid_get_viewport_fb();
    }
  } 
  return p;
}
 

int vid_get_viewport_image_offset() 
{
	return (vid_get_viewport_yoffset() * vid_get_viewport_byte_width() * vid_get_viewport_yscale()) + (vid_get_viewport_xoffset() * 3);
}

int vid_get_viewport_row_offset() 
{
	return (vid_get_viewport_byte_width() * vid_get_viewport_yscale()) - (vid_get_viewport_width() * 3);
}
 

unsigned levent_index_for_name(const char *name)
{
  unsigned i;
  
  for (i=0; levent_table[i].name; i++) {
    
    if (strcmp(levent_table[i].name,name) == 0) {
        return i;
    }
  }
  return LEVENT_INVALID_INDEX;
}

unsigned levent_id_for_name(const char *name)
{
  unsigned i = levent_index_for_name(name);
  if (i!=LEVENT_INVALID_INDEX)
    return levent_table[i].id;
  else
    return 0; 
}

/*
void save_ui_props()
{
int i,fd,id;
static char mybuf[80],fn[20];
static int filenum = 1;
static struct utimbuf t;
t.actime = t.modtime = time(NULL);

sprintf(fn,"A/UI_PROP%03d.txt",filenum);
 fd = open(fn, O_WRONLY|O_CREAT, 0777);  
 
 if (fd>=0)                              
  {  
   if(_ExecuteEventProcedure("UI.CreatePublic") == -1) 
    {if(_ExecuteEventProcedure("UI_RegistDebugEventProc") == -1){close(fd);play_sound(6);return;}}
    for(i=0x8000;i<0x8020;i++)                   
       {
        id = _ExecuteEventProcedure("PTM_GetCurrentItem",i);
        sprintf(mybuf,"0x%04x %d\n",i,id);     
        write(fd,mybuf,strlen(mybuf));                               
       } 
       close(fd);                        
       filenum++;                    
  }
}
*/

void save_ui_props()
{
int i,fd,id;
static char mybuf[80],fn[20];
static int filenum = 1;
static struct utimbuf t;
t.actime = t.modtime = time(NULL);
 
sprintf(fn,"A/UI_PROP%03d.txt",filenum);
 fd = open(fn, O_WRONLY|O_CREAT, 0777); 
 if (fd>=0) 
  {  
    for(i=0x8000;i<0x8020;i++) 
       {
        id =  _PTM_GetCurrentItem(i);
        sprintf(mybuf,"0x%04x %d\n",i,id); 
        write(fd,mybuf,strlen(mybuf)); 
       } 
       close(fd); 
       filenum++;                    
  }
}

extern int _SetHPTimerAfterNow(int delay, int(*good_cb)(int, int), int(*bad_cb)(int, int), int );
extern int _CancelHPTimer(int);
extern int usb_HPtimer_bad(int, int);
extern int usb_HPtimer_good(int, int);
extern int self_timer_bad(int, int);
extern int self_timer_good(int, int);
int usb_HPtimer_handle=0;
int usb_HPtimer_error_count=0;
int self_timer_handle=0;
static int ARM_usb_HPtimer_good(int time, int interval) { return usb_HPtimer_good(time, interval); }
static int ARM_usb_HPtimer_bad(int time, int interval) { return usb_HPtimer_bad(time, interval); }
static int ARM_HPtimer_good(int time, int interval) { return self_timer_good(time, interval); }
static int ARM_HPtimer_bad(int time, int interval) { return self_timer_bad(time, interval); }

int start_self_timer()                            
{
 int duration = HPTimer.units;
    if ( self_timer_handle == 0 )
    {
     self_timer_handle = _SetHPTimerAfterNow(duration,ARM_HPtimer_good,ARM_HPtimer_bad,duration);
     if (!(self_timer_handle & 0x01))return 1 ;  
     self_timer_handle = 0 ;
    }
    return 0;   
}

int stop_self_timer() 
{
    if( self_timer_handle ) 
    {
	_CancelHPTimer(self_timer_handle);
	self_timer_handle = 0 ;
	return 1 ;
    }
    return 0;
}

int start_usb_HPtimer(int interval)            
{
#ifdef CAM_REMOTE_USB_HIGHSPEED
    if ( usb_HPtimer_handle == 0 )
    {
	if(interval < CAM_REMOTE_HIGHSPEED_LIMIT) interval=CAM_REMOTE_HIGHSPEED_LIMIT;
        usb_HPtimer_handle = _SetHPTimerAfterNow(interval,ARM_usb_HPtimer_good,ARM_usb_HPtimer_bad,interval);      
   if (!(usb_HPtimer_handle & 0x01)) return 1 ; 
        usb_HPtimer_handle = 0 ;
    }
#endif
    return 0;   
}

int stop_usb_HPtimer() 
{
#ifdef CAM_REMOTE_USB_HIGHSPEED
    if( usb_HPtimer_handle ) 
    {
	_CancelHPTimer(usb_HPtimer_handle);
	usb_HPtimer_handle = 0 ;
	return 1 ;
    }
#endif
    return 0;
}

void WriteFirmInfoToFile()
{
 _UIFS_WriteFirmInfoToFile(0);
}

void __attribute__((weak)) JogDial_CW(void)     {}
void __attribute__((weak)) JogDial_CCW(void)    {}

