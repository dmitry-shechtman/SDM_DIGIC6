#include "platform.h"
#include "conf.h"
#include "stdlib.h"
#include "raw.h"
#include "draw_palette.h"
#include "gui.h"
#include "keyboard.h"
#include "dng.h"
#define RAW_TARGET_DIRECTORY    "A/DCIM/%03dCANON"
#define RAW_TARGET_FILENAME     "ST%s_%04d.CRW"
#define RAW_BRACKETING_FILENAME "%04d_%03d%s"
#define STEREO_TARGET_DIRECTORY    "A/DCIM/%03dCANON"
#define STEREO_TARGET_FILENAME     "%04d.%s"
 
static struct tm *ttm;
extern time_t gDaySeconds;
extern short kapbv96; 
extern float realfl,max_distance,deviation,percent_deviation,coc,fov,my_max_deviation;
extern long myav,fp,sensor_width;
extern Conf conf;
extern int _EngDrvRead(int gpio_reg);
extern int shift,myfl,myhyp,enablePutOff,gAvOverride;
extern char script_title[];
extern char log_file[];
extern char *kaplogbuf;
static char SerialNumber[30]={'/0'};
extern char *imgbuf;
extern unsigned char displayedMode,gSDM_will_use_flash,gImageCount,gSerialCount,gEVfromScript;
static const char* modes[]={ "65\"","50\"", "40\"", "30\"", "25\"","20\"","15\"","13\"","10\"","8\"","6\"","5\"","4\"","3.2\"","2.5\"","2\"","1.6\"","1.3\"","1\"","Off","0.8","0.6","0.5","0.4","0.3","1/4","1/5","1/6","1/8","1/10","1/13","1/15","1/20","1/25","1/30","1/40","1/50","1/60","1/80", "1/100","1/125","1/160","1/200","1/250","1/320","1/400","1/500","1/640","1/800", "1/1000","1/1250","1/1600","1/2000", "1/2500","1/3200","1/4000", "1/5000", "1/6400", "1/8000", "1/10000", "1/12800", "1/16000", "1/20000", "1/26000","1/32000","1/40000"};
static const char* tv_modes[]={ "Off", "1/3 EV","2/3 EV", "1 EV", "4/3 EV", "5/3 EV", "2 EV"};
#if !defined(OPT_NEW_OVERLAY)
extern int two_shot,hs,zs;
#endif
extern unsigned int gSlitBytes;
extern int gPTPsession,fpd;
extern int gDirectoryNumber;
extern onHP onHalfPress;
extern int idLog[],fdKapLog;
extern int idLogSize;
extern unsigned short gOnReleaseCount;
int gBracketingCounter; 
int gDngFinish,gDngStart;
int gSlitfd=-1;
void wia();
void save_prop_ids();
static char fn[64];
static char dir[32];
int delay_status;
int delay_user_value;

void save_viewport() 
{
#if !defined(CAMERA_tx1)
int viewport_size = vid_get_viewport_height() * screen_width;
#else
int viewport_size = vid_get_viewport_height() * (screen_width-120);
#endif
   int fd;
   char fn[64];
static char *imgbuf;
imgbuf = vid_get_viewport_fb();
if(imgbuf == 0) return;
   mkdir("A/SDM/VPDATA");
   sprintf(fn, "A/SDM/VPDATA/EdgeData_%d",(gPTPsession)?lastObjectNumber()+1:get_target_file_num());

    fd = open(fn, O_WRONLY|O_CREAT, 0777);                
    if (fd>=0) {
      write(fd, imgbuf, viewport_size *3);
      close(fd);
      }
   return;
}
 
void getSerialNumber()
{
 struct stat st;
 FILE *fd = NULL;
 int rcnt;
 char *buf,*ptr,*start;
 SerialNumber[0]='/0'; 
 if (stat("A/FirmInfo.txt",&st)==0)    
 {
  fd = fopen("A/FirmInfo.txt","rb");     
  if(fd)                               
  {
   buf = (char*)malloc(st.st_size+1);
   if(!buf)                            
    {
     fclose(fd);
     return;
    }
     rcnt = fread(buf, 1, st.st_size,fd); 
     if(!rcnt){free(buf);fclose(fd);return;} 
     buf[rcnt]=0;                            
     fclose(fd);                             
     ptr=buf;                                
     while (ptr[0] && strncmp("Serial ",ptr,7)!=0)++ptr;
     if(ptr[0])
     {
      start = ptr;                          
       while (ptr[0] && ptr[0]!='\n') ++ptr; 
       ptr[0]=0;                             
       strcpy(SerialNumber,&start[11]);
       conf.serial_number=strtol(&SerialNumber[strlen(SerialNumber)-6],NULL,10);
       free(buf);
       return;
     }
   }
 }
}

void dumpMsgId();
void dumpDigic();
void save_ui_props();
 
int raw_savefile() 
{
#if !defined(CAMERA_m3)
  if(gSerialCount==1)WriteFirmInfoToFile();
  else if(gSerialCount==2)getSerialNumber();
#endif
    int fd=-1,m=(mode_get()&MODE_SHOOTING_MASK);
    static struct utimbuf t;
    static unsigned char tl_slit_enabled=0;
    tl_slit_enabled=(tl.running && conf.raw_strip_mode && !conf.strip_offset);
    if(shooting_get_drive_mode()&& conf.dist_mode ) 
     {
      int dly = shooting_get_tick_count();
      while((shooting_get_tick_count()-dly)<1){}
      msleep(500);
     }
 
 if(gSDM_will_use_flash)
   {
    int adjmode,flashmode;
    adjmode = 0; 
    flashmode = 1; 
    int flash = 1;
    set_property_case(PROPCASE_FLASH_ADJUST_MODE, &adjmode, sizeof(adjmode));
    set_property_case(PROPCASE_FLASH_MODE, &flashmode, sizeof(flashmode));
    set_property_case(PROPCASE_FLASH_FIRE, &flash, sizeof(flash));
   }
 
    shooting_bracketing();
 
if(conf.raw_strip_mode && shooting_get_drive_mode())
     {
        if(state_shooting_progress != SHOOTING_PROGRESS_PROCESSING)
            gBracketingCounter = 1;               
        else if(gBracketingCounter && (state_shooting_progress == SHOOTING_PROGRESS_PROCESSING))
            gBracketingCounter++;
      
     }
    else
     {
        gDngStart=get_tick_count();
  
     }
 
 if(!shooting_get_drive_mode())gDngStart=get_tick_count();   
    state_shooting_progress = SHOOTING_PROGRESS_PROCESSING; 
 
    if (conf.save_raw || tl_slit_enabled) 
   {
        long v;
        t.actime = t.modtime = time(NULL);
        static int file_num,dir_num;
                                                                     
 
        if(gBracketingCounter==1)                                                       
        {
         if(gPTPsession)file_num=lastObjectNumber()+1;
         else file_num=get_target_file_num();
         dir_num=get_target_dir_num();                                                  
         gDngStart=get_tick_count();
        }

        if((!(conf.strip_offset && conf.raw_strip_mode && shooting_get_drive_mode()))||tl_slit_enabled)    
         {
           char name[32];
           char dbuf[10];
#if defined(CAM_DATE_FOLDER_NAMING)
           get_target_dir_name(name);
           strncpy(dbuf,name+7,8);
           dbuf[8] = '\0';
#else
           sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif 
          sprintf(dir,"A/DCIM/CRW/%s",dbuf);                                            
          mkdir(dir);                                                            
         }
                                                                                        
        else                                                                            
         { 
          sprintf(dir, "A/DCIM/%04d",dir_num);                                         
          mkdir(dir);
          sprintf(dir, "A/DCIM/%04d/LEFT",dir_num);                                    
          mkdir(dir);
          sprintf(dir, "A/DCIM/%04d/RIGHT",dir_num);                                   
          mkdir(dir);
         }
        
        struct t_data_for_exif* exif_data = NULL;  
        char *thumbnail_buf = NULL;
 
     if((!shooting_get_drive_mode()&&!tl_slit_enabled)||(shooting_get_drive_mode()&& !conf.raw_strip_mode))  
        {
         exif_data=capture_data_for_exif(1);
         sprintf(fn,"%s/ST%s_%04d.HDR",dir,(conf.camera_position)?"R":"L",(gPTPsession)?lastObjectNumber()+1:get_target_file_num());
         fd = open(fn, O_WRONLY|O_CREAT, 0777);
         if (fd>=0) 
          {
              create_dng_header(exif_data,1);
              if (get_dng_header())
              {
               write(fd, get_dng_header(), get_dng_header_size());
               close(fd);
               free_dng_header();
              }
           } 
        } 
 
      int raw_start,row_start_left,row_start_right,half_width;
       if((conf.raw_strip_mode && ((conf.strip_images>1)||((conf.strip_images==1)&&(gBracketingCounter==1)))&& gBracketingCounter)||tl_slit_enabled)               
        {
         if((conf.strip_width<32)&&conf.strip_offset)conf.strip_width=32;
         if((gBracketingCounter<=conf.strip_images)||tl_slit_enabled)
         {
          if(conf.strip_offset&&(conf.strip_width%2)) conf.strip_width-=1;                                      
          half_width=CAM_ACTIVE_AREA_Y1 + ((CAM_ACTIVE_AREA_Y2 - CAM_ACTIVE_AREA_Y1)/2);
          if(conf.strip_offset)                                                            
          row_start_left=half_width-(half_width*conf.strip_offset/100);
          else
          row_start_left=half_width-(conf.strip_width/2);                                  
          if(row_start_left%2)row_start_left-=1;                                           
          raw_start=row_start_left*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL/8;
          if(conf.strip_offset)                                                            
           {
            if(conf.camera_orientation==1)                                                      
            sprintf(fn,"A/DCIM/%04d/LEFT/",dir_num);                                       
            else                                                                           
            sprintf(fn,"A/DCIM/%04d/RIGHT/",dir_num);                                      
            sprintf(fn+strlen(fn), RAW_BRACKETING_FILENAME,file_num,gBracketingCounter,".RAW");
           }
           
          else if (((gBracketingCounter==1)&& !conf.strip_offset)||(tl_slit_enabled && (tl.shot_count==1)))                           
           {
            sprintf(fn,"%s/%04d.CRW",dir,file_num);                                    
           }

           if(conf.strip_offset ||(!conf.strip_offset &&((gBracketingCounter==1)||(tl_slit_enabled && (tl.shot_count==1)))))          
           {
            gSlitfd = open(fn, O_WRONLY|O_CREAT, 0777);                                     
           }
           if(conf.strip_offset)                                                           
           { 
            if(gSlitfd>=0)                                                                  
             {

              write(gSlitfd, hook_raw_image_addr()+raw_start, CAM_RAW_ROWPIX*conf.strip_width*CAM_SENSOR_BITS_PER_PIXEL/8);
              close(gSlitfd);
             }
           }
           else                                                                            
           {
            write(gSlitfd, hook_raw_image_addr()+raw_start, CAM_RAW_ROWPIX*conf.strip_width*CAM_SENSOR_BITS_PER_PIXEL/8); 
           }
          if(conf.strip_offset)
           {
            row_start_right=half_width+(half_width*conf.strip_offset/100)-conf.strip_width;
            if(row_start_right%1)row_start_right-=1;                                        
            raw_start=row_start_right*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL/8;
            if(conf.camera_orientation==1)                                                       
            sprintf(fn,"A/DCIM/%04d/RIGHT/",dir_num);                                       
            else                                                                            
            sprintf(fn,"A/DCIM/%04d/LEFT/",dir_num);                                        
            sprintf(fn+strlen(fn), RAW_BRACKETING_FILENAME,file_num,gBracketingCounter,".RAW");
            gSlitfd = open(fn, O_WRONLY|O_CREAT, 0777);    
            if(gSlitfd>=0)                                                                   
             {
              write(gSlitfd, hook_raw_image_addr()+raw_start, CAM_RAW_ROWPIX*conf.strip_width*CAM_SENSOR_BITS_PER_PIXEL/8);
              close(gSlitfd);
             }
            }
           if(conf.blank_jpg)                                                              
           memset((unsigned char*)hook_raw_image_addr(),COLOR_BLACK,hook_raw_size());      
          }
          else                                                                             
          { 
           gDngFinish=get_tick_count();
           if(!conf.strip_offset)                                                           
           gBracketingCounter=0;
          } 
        }
 
       if((((gBracketingCounter==1)&& conf.raw_strip_mode)&& shooting_get_drive_mode()))
      {
         exif_data=capture_data_for_exif(0);                                
         if(conf.strip_offset)                                              
          {  
           sprintf(dir,"A/DCIM/%04d/LEFT",dir_num);                        
           sprintf(fn, "%s/", dir);                                         
           sprintf(fn+strlen(fn),"STRIPS.HDR");                             
          }
         else
          {
           sprintf(fn, "%s/", dir);                                         
           sprintf(fn+strlen(fn),"%04d.HDR",file_num);                      
          }

         fd = open(fn, O_WRONLY|O_CREAT, 0777);
         if (fd>=0) 
          {
             create_dng_header(exif_data,0);
             if (get_dng_header())
              {
               write(fd, get_dng_header(), get_dng_header_size());
               close(fd);
              }
          }

          if(conf.strip_offset)                                             
          {
            sprintf(dir,"A/DCIM/%04d/RIGHT",dir_num);                      
           sprintf(fn, "%s/", dir);
           sprintf(fn+strlen(fn),"STRIPS.HDR");                             
           fd = open(fn, O_WRONLY|O_CREAT, 0777);
           if (fd>=0) 
           {
            write(fd, get_dng_header(), get_dng_header_size());
            close(fd);
           }
          } 
         if(!conf.strip_offset && (gBracketingCounter==conf.strip_images)) 
         gBracketingCounter=0;
         free_dng_header();
     }
     else if(tl_slit_enabled)
     {
       sprintf(fn, "%s/", dir);                                         
       sprintf(fn+strlen(fn),"%04d.HDR",file_num);                      
       fd = open(fn, O_WRONLY|O_CREAT, 0777);
      if (fd>=0) 
      {
       conf.strip_images=tl.shot_count;
       exif_data=capture_data_for_exif(0);
       create_dng_header(exif_data,0);
       if (get_dng_header())
        {
         write(fd, get_dng_header(), get_dng_header_size());
         close(fd);
        } 
       free_dng_header();        
      }      
     }
 
      if((!shooting_get_drive_mode()&&!tl_slit_enabled)||(shooting_get_drive_mode()&& !conf.raw_strip_mode)||(conf.strip_offset&&conf.raw_strip_mode))
       { 
        started();
        sprintf(fn, "%s/", dir);                                                                                  
        sprintf(fn+strlen(fn), RAW_TARGET_FILENAME,(conf.camera_position)?"R":"L",(gPTPsession)?lastObjectNumber()+1:get_target_file_num()); 
        fd = open(fn, O_WRONLY|O_CREAT, 0777);
        if (fd>=0) 
           {
            write(fd,hook_raw_image_addr(),hook_raw_size());  
            close(fd);
            utime(fn, &t);
           }

         if(conf.blank_jpg)
            memset((unsigned char*)hook_raw_image_addr(),COLOR_BLACK,hook_raw_size()); 
            
         finished();
         gDngFinish=get_tick_count();
         return (fd >= 0);     
       }
   } 

    return 0;
}

void raw_postprocess() 
{

}


void wia()
{
 sprintf(dir, RAW_TARGET_DIRECTORY,100);
 mkdir(dir);
sprintf(fn, "%s/ST%s_0001.CRW", dir,(conf.camera_position)?"L":"R");
remove(fn);
 sprintf(fn, "%s/ST%s_0001.CRW", dir,(conf.camera_position)?"R":"L");  
remove(fn);
 int fd = open(fn, O_WRONLY|O_CREAT, 0777);                            
 if (fd>=0)                                                            
  {
   sprintf(dir,"WIA Loader\n");
   write(fd,dir,11);
   close(fd);                                                          
  }
}

void save_prop_ids()
{
#if 0
int i,fd;
static char mybuf[80];
char *p = mybuf;
static int filenum = 1;
static struct utimbuf t;
t.actime = t.modtime = time(NULL);
sprintf(dir,"A/SDM/PROP_ID");                                                
mkdir(dir);
sprintf(fn, "%s/", dir);
sprintf(fn+strlen(fn),"PROP%03d.txt",filenum);
 fd = open(fn, O_WRONLY|O_CREAT, 0777);  
 
 if (fd>=0)                              
  { 
    for(i=0;i<401;i++)                   
       {
        sprintf(mybuf,"                ");       
        sprintf(mybuf,"%3d %11d\n",i,shooting_get_prop(i));     
        write(fd,mybuf,16);                               
       }

       close(fd);                        
       filenum++;                    
  }
 #endif
}
 

void log_kap()
{
 int intValue,user=0;
 char kapbuf[12];
 static tConfigVal configVal = {0,0,0,0};
 if( conf_getValue(300, &configVal) == CONF_VALUE)
  user=configVal.numb;
 if(((user == 70000)||(user == 70001))&&(fdKapLog>=0))      
 {
    sprintf(kaplogbuf+strlen(kaplogbuf),"IMG_%04d.JPG Canon values ",get_exposure_counter());
     
    get_tv(&kapbuf[0],onHalfPress.Tv96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-3s%-7s","Tv:",kapbuf);   
 
    get_av(&kapbuf[0],onHalfPress.Av96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-5s","Av:",kapbuf);   
   
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-4d ","ISO:",onHalfPress.Sv96);  
    
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-4d ","Bv:",onHalfPress.Bv96);
     
    get_ev_correction(&kapbuf[0],conf.user_2*32);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-5s","Ec:",kapbuf);      
     
    ttm = localtime(&gDaySeconds); 
    sprintf(kapbuf,"%02d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
    if(!conf.flag_6) sprintf(kaplogbuf+strlen(kaplogbuf),"%s\n",kapbuf);
    else sprintf(kaplogbuf+strlen(kaplogbuf),"%s.%03d\n",kapbuf,(conf.flag_8-conf.flag_6)%1000);
 
     sprintf(kaplogbuf+strlen(kaplogbuf),"             SDM   values ");   
   
    get_tv(&kapbuf[0],kap.Tv96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-3s%-7s","Tv:",kapbuf); 
    
    get_av(&kapbuf[0],kap.Av96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-5s","Av:",kapbuf);
   
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%3d\n","ISO:",shooting_get_iso_market()); 
    
    
    sprintf(kaplogbuf+strlen(kaplogbuf),"%26s","APEX96 ");
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-3s%-7d","Tv:",kap.Tv96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-5d","Av:",kap.Av96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-5d","Sv:",kap.Sv96);
    sprintf(kaplogbuf+strlen(kaplogbuf),"%-4s%-4d\n","Bv:",kapbv96);
    
    sprintf(kaplogbuf+strlen(kaplogbuf),"%29s%-7s","ND:",(!kap.nd)?"---":(kap.nd==1)?"IN":"OUT"); 
   
    if(lens_get_focus_pos_from_lens()>0)sprintf(kaplogbuf+strlen(kaplogbuf),"%-6s%10d","Focus:",lens_get_focus_pos_from_lens()); 
    else sprintf(kaplogbuf+strlen(kaplogbuf),"Focus:  Infinity");
    
    sprintf(kaplogbuf+strlen(kaplogbuf),"%11s%5d\n\n","MechaPos:",GetMechaPos()); 
     int fdKapLog = open(log_file, O_WRONLY|O_CREAT|O_APPEND, 0777); 
     if(fdKapLog>=0)
      {   
       write(fdKapLog, kaplogbuf, strlen(kaplogbuf));
       close(fdKapLog);
       kaplogbuf[0]='\0';
      }
    configVal.numb = 0;
    configVal.isNumb = 1; 
    conf_setValue(304,configVal);  
 } 
}

void save_xml() 
{
 char xmlFolder[19];
 char lineBuffer[80];
 char lineBuffer2[80];
 char fn[80],fn2[80];
 int fd,fileNumber,sensorWidth=get_sensor_width();
 static struct utimbuf timeBuf;
 timeBuf.actime = timeBuf.modtime = time(NULL);
 int intValue = shooting_get_real_av();
 unsigned long t;

 t = time(NULL);
 ttm = localtime(&t);
if(conf.user_1 == 70001)      // kap xml file
{
}    
else if (conf.save_xml_file) 
 {
  debug_led(1);
  char name[32];
  char dbuf[10];
#if defined(CAM_DATE_FOLDER_NAMING)
  get_target_dir_name(name);
  strncpy(dbuf,name+7,8);
  dbuf[8] = '\0';
#else
  sprintf(dbuf,"%03dCANON",get_target_dir_num());
#endif 
  if (conf.save_xml_file==1)sprintf(xmlFolder,"A/SDM/XML/%8s",dbuf);
  else sprintf(xmlFolder,"A/DCIM/%8s",dbuf);
  mkdir(xmlFolder);
  fileNumber = get_target_file_num();
  sprintf(lineBuffer,"%s/%04d.XML",xmlFolder,fileNumber);
  sprintf(fn,"%s/%04d.XML",xmlFolder,fileNumber);
  fd = open(lineBuffer, O_WRONLY|O_CREAT, 0777);
  if (fd>=0) 
   {
    sprintf(lineBuffer,"<?xml-stylesheet type=\"text/xsl\" href=\"stereo_data.xsl\"?>\n");
    write(fd,lineBuffer,strlen(lineBuffer));
    sprintf(lineBuffer,"<ShootingInfo>\n");
    write(fd,lineBuffer,strlen(lineBuffer));
    sprintf(lineBuffer,"<Link>IMG_%04d.jpg</Link>\n",(gPTPsession)?lastObjectNumber()+1:get_target_file_num());
    write(fd,lineBuffer,strlen(lineBuffer));
    if(SerialNumber[0])
     xmlPrintf(fd,"Camera Serial number","%s",SerialNumber);
    else
     xmlPrintf(fd,"Camera Serial number","%s","Unknown");
#if !defined(CAMERA_ixus160_elph160)   
    xmlPrintf(fd,"Standard period","0x%x %d",STD_PERIOD,STD_PERIOD);     
    xmlPrintf(fd,"Period count on switch release", "0x%x %d %d%\%",gOnReleaseCount,gOnReleaseCount,(gOnReleaseCount*100)/STD_PERIOD);  
#endif    
    xmlPrintf(fd,"Sensor width","%d.%02d",sensorWidth/1000,sensorWidth%1000/10);
    int smode;
    get_property_case(PROPCASE_SHOOTING_MODE, &smode, 4);
    xmlPrintf(fd,"Shooting mode","%d",shooting_mode_canon2chdk(smode));
    smode=onHalfPress.Bv96 + onHalfPress.Sv96;
    xmlPrintf(fd,"Brightness + ISO","%d",smode);
    smode=onHalfPress.Av96 + onHalfPress.Tv96 + shooting_get_prop(PROPCASE_EV_CORRECTION_1);
    xmlPrintf(fd,"Aperture + Shutter + EV correction","%d",smode);  
    xmlPrintf(fd,"ISO Market","%d",shooting_get_iso_market());
    xmlPrintf(fd,"ISO used real","%d",shooting_get_iso_real());
    xmlPrintf(fd,"ISO used sv96","%d",shooting_get_sv96());
    xmlPrintf(fd,"EV Correction (Canon)","%d",shooting_get_prop(PROPCASE_EV_CORRECTION_1));
	if(gEVfromScript)
	{
	 get_ev_correction(&name[0],fast_params.exp_comp*32);	
	 name[strlen(name)]=0;
	xmlPrintf(fd,"EV Correction (Script) ","%s",name);
	gEVfromScript=0;
	}
	else
	{
	xmlPrintf(fd,"EV Correction (Script) ","%d",0);		
	}
    xmlPrintf(fd,"Shutter speed Tv96","%d",onHalfPress.Tv96);
    intValue=(int)(shooting_get_shutter_speed_from_tv96(onHalfPress.Tv96)*100000); 
   if(intValue)
   {   
    if(intValue<=50000) 
     {   
      xmlPrintf(fd,"Real shutter speed","1/%d",100000/intValue);  
     }
    else if((intValue>50000) && (intValue<95000)) 
     {
      xmlPrintf(fd,"Real shutter speed","0.%1d",intValue/10000);     
     }     
    else if((intValue>=95000) && (intValue < 110000))  
     xmlPrintf(fd,"Real shutter speed","%s","1");       
    else xmlPrintf(fd,"Real shutter speed","%2d.%1d",intValue/100000,((intValue+5000)%100000)/10000);  
   }
    intValue=shooting_get_prop(PROPCASE_CAMERA_TV);   
    xmlPrintf(fd,"Override speed Tv96","%d",intValue);
    intValue=(int)(shooting_get_shutter_speed_from_tv96(intValue)*100000); 
   if(intValue)
   {       
     if(intValue<=50000) 
     {   
      xmlPrintf(fd,"Real override shutter speed","1/%d",100000/intValue); 
     }
    else if((intValue>50000) && (intValue<95000)) 
     {
      xmlPrintf(fd,"Real override shutter speed","0.%1d",intValue/10000);     
     }     
    else if((intValue>=95000) && (intValue < 110000)) 
     xmlPrintf(fd,"Real override shutter speed","%s","1");       
    else xmlPrintf(fd,"Real override shutter speed","%2d.%1d",intValue/100000,((intValue+5000)%100000)/10000);  
   }
    xmlPrintf(fd,"Aperture Av96","%d",onHalfPress.Av96);
    intValue = shooting_get_aperture_from_av96(onHalfPress.Av96);
    xmlPrintf(fd,"Real aperture","%d.%02d",intValue/100,intValue%100);
    xmlPrintf(fd,"Override Av96","%d",gAvOverride); 
    if(conf.user_1 == 80975)  
     {
     intValue = shooting_get_aperture_from_av96(gAvOverride);
     xmlPrintf(fd,"Real override aperture","%d.%02d",intValue/100,intValue%100); 
     }
    else xmlPrintf(fd,"Real override aperture","%s","N/A"); 
    xmlPrintf(fd,"kap.Tv96","%d",kap.Tv96);
    xmlPrintf(fd,"kap.Av96","%d",kap.Av96);
    xmlPrintf(fd,"kap.Sv96","%d",kap.Sv96);
    xmlPrintf(fd,"kap.Bv96","%d",kap.Bv96);
    xmlPrintf(fd,"kap.nd","%d",kap.nd);
    
    xmlPrintf(fd,"Brightness Bv96","%d",onHalfPress.Bv96);
    xmlPrintf(fd,"Hyperfocal distance","%d",onHalfPress.Hyperfocal);
    xmlPrintf(fd,"Focus mode","%d",shooting_get_prop(PROPCASE_FOCUS_MODE));
    xmlPrintf(fd,"Focus-stack step-size","%d",conf.stack_step_size);

    if(lens_get_focus_pos() != -1)
    xmlPrintf(fd,"Focus from sensor","%d",lens_get_focus_pos());
    else 
    xmlPrintf(fd,"Focus from sensor","%s","INFINITY");

    int focus_pos_from_lens = lens_get_focus_pos_from_lens();
    int farpoint = shooting_get_farpoint(focus_pos_from_lens);
    int nearpoint = shooting_get_nearpoint(focus_pos_from_lens);
	
    if(focus_pos_from_lens != -1)
     {
      xmlPrintf(fd,"Focus from Lens","%d",focus_pos_from_lens);
	  xmlPrintf(fd,"Near point","%d",nearpoint);  
      if(farpoint>0)
       {
        xmlPrintf(fd,"Far point","%d",farpoint);
        xmlPrintf(fd,"Depth of field","%d",farpoint-nearpoint);
       }
      else 
       {
        xmlPrintf(fd,"Far point","%s","INFINITY");
        xmlPrintf(fd,"Depth of field","%s","INFINITY");
       }	  
      }
    else 
	{
     xmlPrintf(fd,"Focus from Lens","%s","INFINITY");
     xmlPrintf(fd,"Near point","%d", shooting_get_hyperfocal_distance());
	 xmlPrintf(fd,"Far point","%s","INFINITY");
	 xmlPrintf(fd,"Depth of field","%s","INFINITY");
	}

 
    xmlPrintf(fd,"Lens mechanical position","%d",GetMechaPos());
    xmlPrintf(fd,"Lens-to-sensor distance","%d",fpd);
    xmlPrintf(fd,"Rangefinder Focus from Lens","%d",conf.rangefinder_near);
    xmlPrintf(fd,"Circle of confusion","%0d.%03d",(sensorWidth/1200)/1000,(sensorWidth/1200)%1000);
    xmlPrintf(fd,"Zoom point","%d",onHalfPress.ZoomPoint);
    xmlPrintf(fd,"Real Focal length","%2d.%01d",onHalfPress.FocalLength/1000,onHalfPress.FocalLength%1000);
    intValue = get_effective_focal_length(onHalfPress.ZoomPoint);
    xmlPrintf(fd,"Equivalent Focal length","%3d",intValue/1000);
    xmlPrintf(fd,"User variable 1","%d",conf.user_1);
    xmlPrintf(fd,"User variable 2","%d",conf.user_2);
    xmlPrintf(fd,"User variable 3","%d",conf.user_3);
    xmlPrintf(fd,"User variable 4","%d",conf.user_4);
    xmlPrintf(fd,"User variable 5","%d",conf.user_5);
    xmlPrintf(fd,"User variable 6","%d",conf.user_6);
    xmlPrintf(fd,"User variable 7","%d",conf.user_7);
    xmlPrintf(fd,"User variable 8","%d",conf.user_8);
    sprintf(lineBuffer,"</ShootingInfo>\n");
    write(fd,lineBuffer,strlen(lineBuffer));
    close(fd);
    lineBuffer[0]='\0';
    utime(fn, &timeBuf);   
   }
  }
}
 
