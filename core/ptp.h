#ifndef __PTP_H
#define __PTP_H


#define PTP_CHDK_VERSION_MAJOR 2  
#define PTP_CHDK_VERSION_MINOR 0  
#define PTP_SDM_VERSION_MAJOR 2  
#define PTP_SDM_VERSION_MINOR 0  
#define PTP_OC_CHDK 0x9999

#define PTP_RC_OK 0x2001
#define PTP_RC_GeneralError           0x2002
#define PTP_RC_ParameterNotSupported  0x2006
#define PTP_RC_AccessDenied           0x200F
#define PTP_RC_SDM_BufferAllocation   0x02F0
#define PTP_RC_SDM_FileOpenError      0x02F1
#define PTP_RC_SDM_TempDataNull       0x02F2
#define PTP_RC_SDM_DirectoryOpenError 0x02F3

enum {
  PTP_CHDK_Version = 0,     
                            
  PTP_CHDK_GetMemory,       
                            
                            
  PTP_CHDK_SetMemory,       
                            
                            
  PTP_CHDK_CallFunction,    
                            
  PTP_CHDK_TempData,        
                            
  PTP_CHDK_UploadFile,      
  PTP_CHDK_DownloadFile,    
                            
  PTP_CHDK_ExecuteScript,   
                            
                            
                            
  PTP_CHDK_ScriptStatus,    
                            
                            
                            
                            
  PTP_CHDK_ScriptSupport,   
                            
                            
  PTP_CHDK_ReadScriptMsg,   
                            
                            
                            
                            
                            
                            
                            
                            
  PTP_CHDK_WriteScriptMsg,  
                            
                            
                            
                            
  PTP_CHDK_GetVideoSettings,
  PTP_CHDK_GetScriptOutput, 
  PTP_CHDK_OpenDir,         
  PTP_CHDK_ReadDir,         
  PTP_CHDK_CloseDir,        
  PTP_CHDK_GetShootingModesList, 
  PTP_CHDK_StartDownloadFile,
  PTP_CHDK_ResumeDownloadFile,
  PTP_CHDK_EndDownloadFile,
  PTP_CHDK_Shutdown,    
                            
  PTP_CHDK_GetPropCase,     
                            
                            
  PTP_CHDK_GetParamData,    
                            
  PTP_CHDK_SwitchMode,      
  PTP_SDM_RECORD, 
  PTP_SDM_PLAY,
  PTP_CHDK_SwitchLed,       
  PTP_SDM_Version,  
  PTP_SDM_SHOOT,
  PTP_SDM_GETCAMINFO, 
  PTP_SDM_DOWNLOAD_CRW,
  PTP_SDM_GETFOLDER,
  PTP_SDM_GET_HDR,
  PTP_SDM_SET_FOCUS,
  PTP_SDM_NEAR_FAR, 
  PTP_SDM_SET_ZOOM,
  PTP_SDM_MSS,
  PTP_SDM_MOVIESTATUS,
  PTP_SDM_SM,
  PTP_SDM_SET_PW,
} ptp_chdk_command;

struct videosettings{
long live_image_buffer_width;
long live_image_width;
long live_image_height;
long bitmap_buffer_width;
long bitmap_width;
long bitmap_height;
unsigned palette[16]; 
} ;

struct camsettings
{
 char camName[10];
 char firmwareVersion[5];
 char osVersion[8];
 char sdm_version[5];
 long sdm_ptp_version_major;
 long sdm_ptp_version_minor;
 char shootModes[20];
 long lcdStatus;
 long zoomSteps;
 long lastFileNumber;
 long currentDirNumber;
 long batVolt;
 long batVoltMax;
 long batVoltMin;
 long sdFree;
 long sdTotal;
 long flashMode;
 char dngPrefix[5];
} ;

struct fileinfo{
 char name[128];
 unsigned long	atime;
 unsigned long	mtime;
 unsigned long	ctime;
 unsigned long	size;
 unsigned long  attr;
};

#define PTP_CHDK_SL_LUA    0
#define PTP_CHDK_SL_UBASIC 1

#endif 
