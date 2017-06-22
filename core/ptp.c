
#include "camera.h"

#include "stdlib.h"
#include "ptp.h"
#include "script.h"
#include "keyboard.h"
#include "platform.h"
#include "conf.h"
#include "draw_palette.h"
#include "dng.h"
#include "batt_grid.h"
#define BUF_SIZE 0xC000                       

static int handle_ptp(int h, 
                      ptp_data *data,
                      int opcode,
                      int sess_id,
                      int trans_id,
                      int param1,
                      int param2,
                      int param3,
                      int param4,
                      int param5);

int gnPTP_data;                               
char gPTPbuf[32];
extern int kbd_blocked;                       
extern int gPTPsession;
extern int gFarPoint,gNearPoint,gZoomPoint,gFocalPoint;
extern volatile long raw_data_available;
extern const char* my_version_number;
extern void _set_control_event(int);
extern int beenInRec;
char script_fn[45];


void init_chdk_ptp()
{
  int r;
  r = 0x17;
  while ( r==0x17 )
  {
    r = add_ptp_handler(PTP_OC_CHDK,handle_ptp,0);
    msleep(250);
  }
  ExitTask();
}

static int recv_ptp_data(ptp_data *data, char *buf, int size)
  
{
  while ( size >= BUF_SIZE )
  {
    data->recv_data(data->handle,buf,BUF_SIZE,0,0);
    

    size -= BUF_SIZE;
    buf += BUF_SIZE;
  }
  if ( size != 0 )
  {
    data->recv_data(data->handle,buf,size,0,0);
    
  }

  return 1;
}
 
static int send_ptp_data(ptp_data *data, char *buf, int size)
  
{
  int tmpsize;
  
  tmpsize = size;
  while ( size >= BUF_SIZE )
  {
    if ( data->send_data(data->handle,buf,BUF_SIZE,tmpsize,0,0,0) )
    {
      return 0;
    }

    tmpsize = 0;
    size -= BUF_SIZE;
    buf += BUF_SIZE;
  }
  if ( size != 0 )
  {
    if ( data->send_data(data->handle,buf,size,tmpsize,0,0,0) )
    {
      return 0;
    }
  }

  return 1;
}

int handle_ptp(int h, ptp_data *data, int opcode, int sess_id, int trans_id,
               int param1, int param2, int param3, int param4, int param5)
{
  PTPContainer ptp;
  memset(&ptp,0,sizeof(PTPContainer));
  ptp.code = PTP_RC_OK;
  ptp.sess_id = sess_id;
  ptp.trans_id = trans_id;
  ptp.num_param = 0;

  static DIR *dir;
  static char filename[256];
  static char *temp_data = NULL;
  static int temp_data_size = 0;
  static unsigned char recMode,onceOnly;
  static int resolution;

  
  static char *downloadBuf;
  static char *downloadFn;
  static FILE *downloadF;
  static int downloadFileLength;
  static int downloadRemaining;
  static char *temp_dataD = NULL;
  char *p;
  static int temp_data_sizeD = 0;

  if(!onceOnly)
   {
    gPTPsession = 1;
    kbd_blocked = 1;
    gui_kbd_enter(); 
    conf.synch_enable = 0;
    conf.remote_enable = 0;
    onceOnly=1;
    play_sound(4);
   }
    char *addr;
    tConfigVal configVal = {0,0,0,0};

  switch ( param1 )
  {
    case PTP_SDM_Version:
      ptp.num_param = 3;
      int r=0;
      if ((mode_get()&MODE_MASK) != MODE_PLAY) r = 0;
      if ((mode_get()&MODE_MASK) == MODE_PLAY) r = 1;
      if (((mode_get()&MODE_MASK) != MODE_PLAY) && MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK)) r = 2;
      ptp.param1 = PTP_SDM_VERSION_MAJOR;
      ptp.param2 = PTP_SDM_VERSION_MINOR;
      if (beenInRec)ptp.param3 = r;
      else ptp.param3 = -1;
      break;

    case PTP_CHDK_Version:
      ptp.num_param = 2;
      ptp.param1 = PTP_CHDK_VERSION_MAJOR;
      ptp.param2 = PTP_CHDK_VERSION_MINOR;
      break;

    case PTP_SDM_RECORD:
#if defined(CAM_DRYOS)
    switch_mode_usb(1);
#else
    recordMode();
#endif
      break;

    case PTP_SDM_PLAY:
#if defined(CAM_DRYOS)
    switch_mode_usb(0);
#else
     playMode();
#endif
      break;

    case PTP_SDM_GETFOLDER:
    {
     struct stat   st;
     char name[32];
     char name1[80];
     char name2[80];
     char dbuf[10];
     char buf[18];
     int i = lastObjectNumber()+1;
#if defined(CAM_DATE_FOLDER_NAMING)
     get_target_dir_name(name);
     strncpy(dbuf,name+7,8);
     dbuf[8] = '\0';
#else
     sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif 
     sprintf(buf,"%s|%04d|JPG\n",dbuf,i);
     if ( data->send_data(data->handle,(char*)buf , sizeof(buf), sizeof(buf), 0, 0, 0) )
     {
      ptp.code = PTP_RC_GeneralError;
     }
      break;
    }

    case PTP_SDM_GETCAMINFO:
     {
      struct camsettings camSettings;
      int i,j,y,s;
      char c[1];
      char t[19];
      t[18]='\0';
      t[0]='>';
      t[17]='<';
     if( conf_getValue(67, &configVal) == CONF_VALUE) 
      camSettings.lcdStatus = configVal.numb;
     else camSettings.lcdStatus = 3;
      camSettings.zoomSteps = zoom_points;

       
       for(i=0;i<16;i++)                            
        {
         s=0;y=1;                                   
                                                    
         for(j=1;j<5;j++)                           
          {
           if(shooting_mode_chdk2canon(i*4+j) != -1)
           s=s+y;                                   
           y=y*2;                                   
          }
           sprintf(c,"%x",s);                       
           t[16-i]=*c;
        }

         strcpy(camSettings.shootModes,t);     
         camSettings.sdm_ptp_version_major  = PTP_SDM_VERSION_MAJOR;
         camSettings.sdm_ptp_version_minor  = PTP_SDM_VERSION_MINOR;
         strcpy(camSettings.camName,(char *)PLATFORM);
         strcpy(camSettings.firmwareVersion,(char *)PLATFORMSUB);
#if defined CAM_DRYOS 
         strcpy(camSettings.osVersion,"dryos");
#else
         strcpy(camSettings.osVersion,"vxworks");
#endif
         strcpy(camSettings.sdm_version,(char *)my_version_number);
         strcpy(camSettings.dngPrefix,(conf.camera_position)?"STR_":"STL_");
         camSettings.lastFileNumber = lastObjectNumber();
         camSettings.currentDirNumber = get_target_dir_num();
         camSettings.batVolt = get_batt_average();
         camSettings.batVoltMax = conf.batt_volts_max;
         camSettings.batVoltMin = conf.batt_volts_min;
         camSettings.sdFree = GetFreeCardSpaceKb();
         camSettings.sdTotal = GetTotalCardSpaceKb(); 
         camSettings.flashMode = shooting_get_prop(PROPCASE_FLASH_MODE);
        if ( data->send_data(data->handle, (char *)&camSettings , sizeof(camSettings), sizeof(camSettings), 0, 0, 0) )
        {
          ptp.code = PTP_RC_GeneralError;
        }
         break;
     }

    case PTP_CHDK_OpenDir:
      {
       data->recv_data(data->handle, filename, data->get_data_size(data->handle), 0, 0);
       dir=opendir(filename);
       if (!dir) ptp.code = PTP_RC_SDM_DirectoryOpenError;
       break;
      }

    case PTP_CHDK_CloseDir:
      {
       if (dir) {
        closedir(dir);
        dir=NULL;
       }
       break;
      }

    case PTP_CHDK_ReadDir:
      {
       struct fileinfo fi;
       struct dirent *de;
       struct stat   st;
       int res;

       if (!dir) {ptp.code = PTP_RC_SDM_DirectoryOpenError; break;}

       do de=readdir(dir);
       while (de && (de->name[0]==0xE5));

       if (de) 
        {
         int dl;
         strcpy(fi.name, de->name);
         dl=strlen(filename);
         if(dl!=2)strcat(filename, "/");
         strcat(filename, de->name);
         if (stat(filename, &st)==0) 
          {
           fi.attr=st.st_attrib;
           fi.size=st.st_size;
           fi.mtime=st.st_mtime;
#ifndef CAM_DRYOS_2_3_R39
           fi.atime=st.st_atime;
#endif
    	     fi.ctime=st.st_ctime;
          }
        else 
          {
           fi.attr=0;
           fi.size=0;
           fi.mtime=0;
           fi.atime=0;
    	     fi.ctime=0;
          }
         filename[dl]=0;
        }
       else fi.name[0]=0;

       if ( data->send_data(data->handle, (char*)&fi , sizeof(fi), sizeof(fi), 0, 0, 0) )
          ptp.code = PTP_RC_GeneralError;
       break;
      }
    case PTP_CHDK_ExecuteScript:
      {
        FILE *f;
        int s,r,fn_len;
        char *buf, *fn;
	  char script_fn[32];
        char *script_end = "\nend\n";
        s = data->get_data_size(data->handle);                      
        data->recv_data(data->handle,(char *) &fn_len,4,0,0);       
        s -= 4;                                                     
        fn = (char *) malloc(fn_len+1);                             
        if ( fn == NULL )                                           
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }
         msleep(200);
         fn[fn_len] = '\0';                                         
         data->recv_data(data->handle,fn,fn_len,0,0);               
         s -= fn_len;                                               
	     if(fn[fn_len-1] == '#')                                  
		{
		 buf = (char *) malloc(0x1);                            
		 data->recv_data(data->handle,buf,s,0,0);               
		 fn[fn_len-1] = '\0';                                   
	       sprintf(script_fn,"A/SDM/SCRIPTS/%s",fn);
		 free(buf);
             free(fn);  
		}
		else                                                    
	      {
		 sprintf(script_fn,"A/SDM/SCRIPTS/PTP.bas");           
             f = fopen(script_fn,"wb");                             
             if ( f == NULL )                                       
             {
              ptp.code = PTP_RC_GeneralError;
              free(fn);
              break;
             }                                      

             buf = (char *) malloc(0x3000);                         

             if ( buf == NULL )                                     
             {
              ptp.code = PTP_RC_GeneralError;
              break;
             }

             while ( s > 0 )                                        
             {
              if ( s >= BUF_SIZE )                                  
               {
                r = data->recv_data(data->handle,buf,BUF_SIZE,0,0); 
                fwrite(buf,1,BUF_SIZE,f);                           
                s -= BUF_SIZE;                                      
               } 
             else 
               {
                data->recv_data(data->handle,buf,s,0,0);            
                fwrite(buf,1,s,f);                                  
                s = 0;
               }
	          fwrite(script_end,1,5,f);                           
             }
 
              fclose(f);                                            
              free(fn);  
              free(buf);                                            
	      }

           script_load(script_fn,0);                                
	     kbd_blocked = 1;
	     gui_kbd_enter(); 
           script_start();
           break;
      } 

    case PTP_CHDK_TempData:
      if ( temp_data != NULL )
      {
        free(temp_data);
        temp_data = NULL;
      }
      temp_data_size = data->get_data_size(data->handle);
      temp_data = (char *) malloc(temp_data_size);
      if ( temp_data == NULL )
      {
        ptp.code = PTP_RC_GeneralError;
        break;
      }
      data->recv_data(data->handle,temp_data,temp_data_size,0,0);

      break;
    case PTP_CHDK_DownloadFile:
      {
        FILE *f;
        int tmp,t,s,r,fn_len,i;
        char *buf, *fn;
        struct stat st;
        if ( temp_data == NULL )
        {
          ptp.code = PTP_RC_SDM_FileOpenError;
          break;
        }
        strcpy(gPTPbuf,temp_data);
        gPTPbuf[strlen(gPTPbuf)]='\0';

        fn = (char *) malloc(temp_data_size+1);
        if ( fn == NULL )
        {
          free(temp_data);
          temp_data = NULL;
          ptp.code = PTP_RC_SDM_BufferAllocation;
          break;
        }
        memcpy(fn,temp_data,temp_data_size);
        fn[temp_data_size] = '\0';
        draw_string(70,92,fn,MAKE_COLOR(COLOR_BLUE,COLOR_WHITE),1);
        free(temp_data);
        temp_data = NULL;
        i=0;
        while((stat(fn, &st)!=0)&& (i<5)){i++;msleep(500);}
        msleep(200);
        f = fopen(fn,"rb");
        if ( f == NULL )
        {
          ptp.code = PTP_RC_SDM_FileOpenError;
          free(fn);
          break;
        }
        free(fn);

        fseek(f,0,SEEK_END);
        s = ftell(f);
        fseek(f,0,SEEK_SET);

        buf = (char *) malloc(BUF_SIZE);
        if ( buf == NULL )
        {
          ptp.code = PTP_RC_SDM_BufferAllocation;
          break;
        }

        tmp = s;                                                  
        t = s;
        while ( (r = fread(buf,1,(t<BUF_SIZE)?t:BUF_SIZE,f)) > 0 )
        {
          t -= r;
          
          data->send_data(data->handle,buf,r,tmp,0,0,0);
          tmp = 0;                                                
        }
        fclose(f);
        

        ptp.num_param = 1;
        ptp.param1 = s;

        free(buf);

        break;
      }
    case PTP_SDM_SHOOT:
      {
       char buf[24];
       char name[32];
       char dbuf[10];
       struct stat st;
#if defined(CAM_DATE_FOLDER_NAMING)
       get_target_dir_name(name);
       strncpy(dbuf,name+7,8);
       dbuf[8] = '\0';
#else
      sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif   
       int r=0;
       if ((mode_get()&MODE_MASK) != MODE_PLAY) r = 0;
       if ((mode_get()&MODE_MASK) == MODE_PLAY) r = 1;
       if (((mode_get()&MODE_MASK) != MODE_PLAY) && MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK)) r = 2;
       if(param2 != 0)sprintf(buf,"1234|JPG|%s",dbuf);         
       else sprintf(buf,"1234|JPG|%d",r);                      
       resolution = shooting_get_prop(PROPCASE_RESOLUTION);
       
       if((r==0)&& (param2 != 0))                  
        {
         ptp_raw_sending_active(1);
         if(param3==1)
          {
           shooting_set_prop(PROPCASE_RESOLUTION,LOWRESIMG);
          }
         kbd_key_press(KEY_SHOOT_FULL);
	     msleep(1500);
         kbd_key_release(KEY_SHOOT_FULL);
         if(param3==1)
          {
           memset((unsigned char*)hook_raw_image_addr(),COLOR_BLACK,hook_raw_size());
          }
         ptp_raw_sending_active(0);
        }
        if ( data->send_data(data->handle, (char*)buf , sizeof(buf), sizeof(buf), 0, 0, 0) )
        {
         ptp.code = PTP_RC_GeneralError;
        }
       if(param3==1)shooting_set_prop(PROPCASE_RESOLUTION,LOWRESIMG);
       break;
      }
    case PTP_CHDK_SwitchMode:
      if ( param2 != 0 && param2 != 1 )
      {
        ptp.code = PTP_RC_ParameterNotSupported;
      } else 
      {
       recMode=param2;
       switch_mode(param2);
      }
      break;

    case PTP_SDM_SET_FOCUS:
    {
     char dbuf[32];
       int dist,status;
       dist = lens_get_focus_pos_from_lens();
       if(param2>0)
        {
         lens_set_focus_pos(param2+fpd);
         int newFocus = lens_get_focus_pos_from_lens();
         int prevFocus = prev_focus(newFocus);
         int nextFocus = next_focus(newFocus);
         sprintf(dbuf,"%d|%d|%d|%d|%d|%d|%d",(newFocus< dist)?1:0,newFocus,dist,nextFocus,prevFocus,shooting_get_prop(PROPCASE_FOCUS_MODE),shooting_get_prop(PROPCASE_DRIVE_MODE));
        }
       else if(param2==0)
        {
         int prevFocus = prev_focus(dist);
         int nextFocus = next_focus(dist);
         sprintf(dbuf,"%d|%d|%d|%d|%d|%d|%d",1,dist,dist,nextFocus,prevFocus,shooting_get_prop(PROPCASE_FOCUS_MODE),shooting_get_prop(PROPCASE_DRIVE_MODE));
        }
       else if(param2==-1)
        {
         int prevFocus = prev_focus(dist);
         if(prevFocus==dist)lens_set_focus_pos(prevFocus-1+fpd);  
         else lens_set_focus_pos(prevFocus+fpd);
         int newFocus = lens_get_focus_pos_from_lens();
         prevFocus = prev_focus(newFocus);
         int nextFocus = next_focus(newFocus);
         sprintf(dbuf,"%d|%d|%d|%d|%d|%d|%d",(newFocus<dist)?1:0,newFocus,dist,nextFocus,prevFocus,shooting_get_prop(PROPCASE_FOCUS_MODE),shooting_get_prop(PROPCASE_DRIVE_MODE));
        }
       else if(param2==-2)
        {
         int nextFocus = next_focus(dist);
         lens_set_focus_pos(nextFocus+fpd);
         int newFocus = lens_get_focus_pos_from_lens();
         int prevFocus = prev_focus(newFocus);
         nextFocus = next_focus(newFocus);
         sprintf(dbuf,"%d|%d|%d|%d|%d|%d|%d",(newFocus&&(newFocus> dist))?1:0,newFocus,dist,nextFocus,prevFocus,shooting_get_prop(PROPCASE_FOCUS_MODE),shooting_get_prop(PROPCASE_DRIVE_MODE));
        }

        if ( data->send_data(data->handle,(char*)dbuf , sizeof(dbuf), sizeof(dbuf), 0, 0, 0) )
        {
         ptp.code = PTP_RC_GeneralError;
        }
       break;
     }
    case PTP_CHDK_Shutdown:
    {
      ptp.code = PTP_RC_GeneralError;                  
      switch ( param2 )
      {
        case 0:                                        
          power_down();
          break;
        case 1:                                         
          power_down();
          ptp.code = PTP_RC_OK;                         
          break;
        case 2: 
          reboot(NULL);
          break;
      }
      break;
    }

     case PTP_SDM_GET_HDR:
    {
     struct t_data_for_exif* exif_data = capture_data_for_exif(1);
     create_dng_header(exif_data,1);
     char *buf = (char*)get_dng_header();
     int size = get_dng_header_size();
     if (buf)
     {
      if (data->send_data(data->handle,buf,size,size, 0, 0, 0) )
       {
        ptp.code = PTP_RC_GeneralError;
        free_dng_header();
       }
      free_dng_header();
     }
     break;
    } 
 
    case PTP_SDM_DOWNLOAD_CRW:
      {
       FILE *f;
       struct stat st;
       char dbuf[10];
       char name[32];
       char *buf = (char *)hook_raw_image_addr();
       int r = hook_raw_size();
       ptp.param1 = r;
       ptp.num_param = 5;
#if defined(CAM_DATE_FOLDER_NAMING)
       get_target_dir_name(name);
       strncpy(dbuf,name+7,8);
       dbuf[8] = '\0';
#else
      sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif   
       resolution = shooting_get_prop(PROPCASE_RESOLUTION);
       if(param2==1)shooting_set_prop(PROPCASE_RESOLUTION,LOWRESIMG);
       if ((mode_get()&MODE_MASK) != MODE_PLAY) ptp.param4 = 0;
       if ((mode_get()&MODE_MASK) == MODE_PLAY) ptp.param4 = 1;
       if (((mode_get()&MODE_MASK) != MODE_PLAY) && MODE_IS_VIDEO(mode_get()&MODE_SHOOTING_MASK)) ptp.param4 = 2;
       if(conf.camera_position)ptp.param3 = 1;            
       else ptp.param3 = 0;                               

       int lastNum = lastObjectNumber();
       sprintf(name,"A/DCIM/%s/IMG_%04d.JPG",dbuf,lastNum);
       stat(name,&st);
       if(st.st_size < 30000)ptp.param5 = lastNum;
       if(ptp.param4==0)                                  
        {
         ptp.param2 = lastObjectNumber()+1;
         ptp_raw_sending_active(1);
         kbd_key_press(KEY_SHOOT_FULL);
         while (!raw_data_available)
          {
	     msleep(100);
          } 
         kbd_key_release(KEY_SHOOT_FULL);
         data->send_data(data->handle,buf,r,r,0,0,0);
         if(param2==1)memset((unsigned char*)hook_raw_image_addr(),COLOR_BLACK,hook_raw_size());
         ptp_raw_sending_active(0);
        }
       else data->send_data(data->handle,buf,100,100,0,0,0);
       if(param2==1)shooting_set_prop(PROPCASE_RESOLUTION,resolution); 
      }
      break;

    case PTP_CHDK_GetMemory:
      {
         if ((mode_get()&MODE_MASK) == MODE_PLAY)addr=vid_get_viewport_fb_d();
         else if(!(addr=vid_get_viewport_live_fb()))addr=vid_get_viewport_fb();
       sprintf(gPTPbuf,"Buffer 0x%08x",(int)addr);
        if ( data->send_data(data->handle, addr, param3, param3, 0, 0, 0) )
        {
         ptp.code = PTP_RC_GeneralError;
        }
        break;
      }
    case PTP_CHDK_SetMemory:
      *((volatile long *) param2) = param3;
      break;
    case PTP_CHDK_CallFunction:
      {
        int s;
        int *buf = (int *) malloc((10+1)*sizeof(int));

        if ( buf == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }

        s = data->get_data_size(data->handle);
        data->recv_data(data->handle,(char *) buf,s,0,0);

        ptp.num_param = 1;
        ptp.param1 = ((int (*)(int,int,int,int,int,int,int,int,int,int)) buf[0])(buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10]);

        free(buf);
        break;
      }
    case PTP_CHDK_GetPropCase:
      {
        int *buf, i;

        if ( param3 < 1 ) 
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }

        buf = (int *) malloc(param3*sizeof(int));
        if ( buf == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }

        for (i=0; i<param3; i++)
        {
          get_property_case(param2+i,(char *) &buf[i],4);
        }

        if ( data->send_data(data->handle,(char *) buf,param3*sizeof(int),param3*sizeof(int),0,0,0) )
        {
          ptp.code = PTP_RC_GeneralError;
        }
        free(buf);
        break;
      }
    case PTP_CHDK_GetParamData:
      {
        extern long* FlashParamsTable[];
        char *buf, *p;
        int i, s;

        if ( param3 < 1 ) 
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }

        
        s = 0;
        for (i=0; i<param3; i++)
        {
          s += 4 + (FlashParamsTable[param2+i][1]>>16); 
        }

        buf = (char *) malloc(s);
        if ( buf == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }

        
        p = buf;
        for (i=0; i<param3; i++)
        {
          int t = FlashParamsTable[param2+i][1]>>16;
          
          memcpy(p,&t,4);
          p += 4;
          
          get_parameter_data(param2+i,p,t);
          p += t;
        }

        if ( data->send_data(data->handle,buf,s,s,0,0,0) )
        {
          ptp.code = PTP_RC_GeneralError;
        }
        free(buf);
        break;
      }

    case PTP_CHDK_UploadFile:
      {
        FILE *f;
        int s,r,fn_len;
        char *buf, *fn;

        s = data->get_data_size(data->handle);

        data->recv_data(data->handle,(char *) &fn_len,4,0,0);
        s -= 4;

        fn = (char *) malloc(fn_len+1);
        if ( fn == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }
        fn[fn_len] = '\0';

        data->recv_data(data->handle,fn,fn_len,0,0);
        s -= fn_len;

        f =  fopen(fn,"wb");
        if ( f == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          free(fn);
          break;
        }
        free(fn);

        buf = (char *) malloc(BUF_SIZE);
        if ( buf == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }
        while ( s > 0 )
        {
          if ( s >= BUF_SIZE )
          {
            r = data->recv_data(data->handle,buf,BUF_SIZE,0,0);
            fwrite(buf,1,BUF_SIZE,f);
            s -= BUF_SIZE;
          } else {
            data->recv_data(data->handle,buf,s,0,0);
            fwrite(buf,1,s,f);
            s = 0;
          }
        }

        fclose(f);

        free(buf);
        break;
      }

    case PTP_CHDK_StartDownloadFile:
     {

      if ( temp_dataD != NULL )                                     
      {
        free(temp_dataD);
        temp_dataD = NULL;
      }

      temp_data_sizeD = data->get_data_size(data->handle);

      temp_dataD = (char *) malloc(temp_data_sizeD);                

      data->recv_data(data->handle,temp_dataD,temp_data_sizeD,0,0); 

        downloadFn = (char *) malloc(temp_data_sizeD+1);            
        if ( downloadFn == NULL )
        {
          free(temp_dataD);
          temp_dataD = NULL;
          ptp.code = PTP_RC_GeneralError;
          break;
        }
        memcpy(downloadFn,temp_dataD,temp_data_sizeD);              
        downloadFn[temp_data_sizeD] = '\0';
        free(temp_dataD);                                           
        temp_dataD = NULL;                                          

        downloadF = fopen(downloadFn,"rb");                         
        if ( downloadF == NULL ) 
        {
          ptp.code = PTP_RC_GeneralError;
          free(downloadFn);
          break;
        }
        free(downloadFn);
        fseek(downloadF,0,SEEK_END);                                
        downloadFileLength = ftell(downloadF);                      
        downloadRemaining = downloadFileLength;
        fseek(downloadF,0,SEEK_SET);                                
        downloadBuf = (char *) malloc(BUF_SIZE);                    

        if ( downloadBuf == NULL )
        {
          ptp.code = PTP_RC_GeneralError;
          break;
        }
     }
     break;
    case PTP_CHDK_ResumeDownloadFile:
     {
       int r = 0;
       if(downloadRemaining>0)
        {
         r = fread(downloadBuf,1,(downloadRemaining<BUF_SIZE)?downloadRemaining:BUF_SIZE,downloadF); 
      
         data->send_data(data->handle,downloadBuf,r,r,0,0,0);                                            
         ptp.param1 = r;
         downloadRemaining -= r;
        }

        else
        {
         ptp.param1 = 0;
         data->send_data(data->handle,downloadBuf,r,r,0,0,0);
        }
       ptp.num_param = 1;

     }
     break;
    case PTP_CHDK_EndDownloadFile:
     {
       fclose(downloadF);
       free(downloadBuf);
     }
     break;

    case PTP_CHDK_GetScriptOutput:
      {
        char *s=get_whole_console();
        if (!s) { ptp.code = PTP_RC_GeneralError; break;}
        if ( data->send_data(data->handle, s , strlen(s)+1, strlen(s)+1, 0, 0, 0) )
        {
          ptp.code = PTP_RC_GeneralError;
        }
        free(s);
        break;
      }
    case PTP_CHDK_ScriptStatus:
      ptp.num_param = 1;
      ptp.param1 = state_kbd_script_run;
      break;
    case PTP_CHDK_GetVideoSettings:
      {
#if defined(OPT_LIVE_VIEW)
        struct videosettings vsettings;

        vsettings.live_image_buffer_width=vid_get_image_buffer_width_for_ptp();
        vsettings.live_image_width=vid_get_image_width_for_ptp();
        vsettings.live_image_height=vid_get_image_height_for_ptp();
        vsettings.bitmap_buffer_width=vid_get_bitmap_buffer_width_for_ptp();
        vsettings.bitmap_width=vid_get_bitmap_width_for_ptp();
        vsettings.bitmap_height=vid_get_bitmap_height_for_ptp();
        memcpy(vsettings.palette, vid_get_palette(), sizeof(vsettings.palette));
        if ( data->send_data(data->handle, (char*)&vsettings , sizeof(vsettings), sizeof(vsettings), 0, 0, 0) )
        {
          ptp.code = PTP_RC_GeneralError;
        }
#endif
        break;
      }
    case PTP_SDM_SET_PW:
      conf.user_1 = param2;
      break;
    case PTP_CHDK_SwitchLed:
      if ( param2 != 0 && param2 != 1 )
      {
        ptp.code = PTP_RC_ParameterNotSupported;
      } 
     else 
      {
       debug_led(param2);
      }
      break;
    case PTP_SDM_NEAR_FAR:
    ptp.num_param = 3;
    int stereoShift;
    if ( param2 == 0)gNearPoint = lens_get_focus_pos_from_lens();
    else gFarPoint = lens_get_focus_pos_from_lens();
    stereoShift =(int)(((float)get_sensor_width()/1000/conf.stereo_deviation/((float)(gFocalPoint)/1000))*gNearPoint*gFarPoint)/(gFarPoint-gNearPoint);
    if((conf.camera_orientation==1)||(conf.camera_orientation==3))
     stereoShift = 3*stereoShift/4; 
    ptp.param1 = gNearPoint;
    ptp.param2 = gFarPoint;
    ptp.param3 = stereoShift;
    break;
    case PTP_SDM_SET_ZOOM:
    ptp.num_param = 2;
    if(param3 == 999)
     {
      ptp.param1 = TxTblIndex(shooting_get_zoom());     
      ptp.param2 = TxTblIndex(param2);                  
     }
    else 
     {
      ptp_sz(param2);
      ptp.param1 = 333;
      ptp.param2 = 666;
     }
    break;

    case PTP_SDM_MSS:
    {
     char buf[24];
     char dbuf[10];
     char name[32];
#if defined(CAM_DATE_FOLDER_NAMING)
     get_target_dir_name(name);
     strncpy(dbuf,name+7,8);
     dbuf[8] = '\0';
#else
     sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif  
#if defined(CAM_HAS_VIDEO_CONTROL_BUTTON)
    kbd_key_press(KEY_VIDEO);
    msleep(1000);
    kbd_key_release(KEY_VIDEO);
#else
    kbd_key_press(KEY_SHOOT_FULL);
    msleep(2000);
    kbd_key_release(KEY_SHOOT_FULL);
#endif
    sprintf(buf,"1234|%s|%s",MOVIE_TYPE,dbuf);                                                
    if ( data->send_data(data->handle, (char*)buf , sizeof(buf), sizeof(buf), 0, 0, 0) )
     {
      ptp.code = PTP_RC_GeneralError;
     }
    break;
    }

    case PTP_SDM_MOVIESTATUS:
    ptp.num_param = 1;
    ptp.param1 = get_movie_status();
    break;

    case PTP_SDM_SM:
    ptp.num_param = 1;
    ptp.param1 = mode_get()& MODE_SHOOTING_MASK;
    break;
    default:
      ptp.code = PTP_RC_ParameterNotSupported;
      break;
   }
 
  data->send_resp( data->handle, &ptp );                      
  
  return 1;

}

