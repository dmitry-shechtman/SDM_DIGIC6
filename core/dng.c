#include "camera.h"
#include "stdlib.h"
#include "string.h"
#include "platform.h"
#include "math.h"
#include "dng.h"
#include "conf.h"
#include "keyboard.h"
int cam_DefaultCropSize[]={CAM_JPEG_WIDTH, CAM_JPEG_HEIGHT};    
const int cam_DefaultCropOrigin[]={CAM_DEFAULT_CROP_ORIGIN_W,CAM_DEFAULT_CROP_ORIGIN_H};
const int cam_BaselineExposure[]={-1,2};
const int cam_BaselineNoise[]={1,1};
const int cam_BaselineSharpness[]={4,3};
const int cam_LinearResponseLimit[]={1,1};
const int cam_AnalogBalance[]={1,1,1,1,1,1};
const int cam_ColorMatrix1[9*2]={CAM_COLORMATRIX1};
const char cam_name[32];
const short cam_PreviewBitsPerSample[]={8,8,8};
const char img_desc[]="SDM CanonAssistant";
const char cam_chdk_ver[]="SDM ver. "BUILD_NUMBER;
const int cam_Resolution[]={180,1};
int cam_AsShotNeutral[]={1000,1000,1000,1000,1000,1000};
int cam_ActiveArea[]={CAM_ACTIVE_AREA_Y1, CAM_ACTIVE_AREA_X1, CAM_ACTIVE_AREA_Y2, CAM_ACTIVE_AREA_X2};

//warning: according to TIFF format specification, elements must be sorted by tag value in ascending order!
struct dir_entry IFD0[]={
 {0xFE,   T_LONG,      1,  0},                                  
 {0x100,  T_LONG,      1,  CAM_RAW_ROWPIX},                     
 {0x101,  T_LONG,      1, 0},                                   
 {0x102,  T_SHORT,     1,  CAM_SENSOR_BITS_PER_PIXEL},          
 {0x103,  T_SHORT,     1,  1},                                  
 {0x106,  T_SHORT,     1,  0x8023},                             
 {0x10E,  T_ASCII,     sizeof(img_desc),(int)img_desc},         
 {0x10F,  T_ASCII,     sizeof(CAM_MAKE),  (int)CAM_MAKE},       
 {0x110,  T_ASCII,     32, (int)cam_name},                      
 {0x111,  T_LONG,      1,  0},                                  
 {0x112,  T_SHORT,     1,  0},                                  
 {0x115,  T_SHORT,     1,  1},                                  
 {0x116,  T_SHORT,     1, 0},                                   
 {0x117,  T_LONG,      1, 0},                                   
 {0x11A,  T_RATIONAL,  1,  (int)cam_Resolution},                
 {0x11B,  T_RATIONAL,  1,  (int)cam_Resolution},                
 {0x11C,  T_SHORT,     1,  1},                                  
 {0x128,  T_SHORT,     1,  3},                                  
 {0x131,  T_ASCII,     sizeof(cam_chdk_ver),(int)cam_chdk_ver}, 
 {0x132,  T_ASCII,     20, 0},                                  
 {0x828D, T_SHORT,     2,  0x00020002},                         
 {0x828E, T_BYTE,      4,  cam_CFAPattern},
 {0x8298, T_ASCII,     1,  0},                                  
 {0x8769, T_LONG,      1,  0},                                  
 {0x9216, T_BYTE,      4,  0x00000001},                         
 {0xC612, T_BYTE,      4,  0x00000101},                         
 {0xC614, T_ASCII,     32, (int)cam_name},                      
 {0xC61A, T_LONG,      1,  CAM_BLACK_LEVEL},                    
 {0xC61D, T_LONG,      1,  CAM_WHITE_LEVEL},                    
 {0xC61F, T_LONG,      2,  (int)cam_DefaultCropOrigin},
 {0xC620, T_LONG,      2,  (int)cam_DefaultCropSize},
 {0xC621, T_SRATIONAL, 9,  (int)cam_ColorMatrix1},
 {0xC627, T_RATIONAL,  3,  (int)cam_AnalogBalance},
 {0xC628, T_RATIONAL,  3,  (int)cam_AsShotNeutral},
 {0xC62A, T_SRATIONAL, 1,  (int)cam_BaselineExposure},
 {0xC62B, T_RATIONAL,  1,  (int)cam_BaselineNoise},
 {0xC62C, T_RATIONAL,  1,  (int)cam_BaselineSharpness},
 {0xC62E, T_RATIONAL,  1,  (int)cam_LinearResponseLimit},
 {0xC65A, T_SHORT,     1,  cam_CalibrationIlluminant1}, 
 {0xC68D, T_LONG,      4,  (int)cam_ActiveArea},
 {0}
};

int get_type_size(int type)
{
 switch(type)
 {
  case T_BYTE:      return 1;
  case T_ASCII:     return 1; 
  case T_SHORT:     return 2;
  case T_LONG:      return 4;
  case T_RATIONAL:  return 8;
  case T_SBYTE:     return 1;
  case T_UNDEFINED: return 1;
  case T_SSHORT:    return 2;
  case T_SLONG:     return 4;
  case T_SRATIONAL: return 8;
  case T_FLOAT:     return 4;
  case T_DOUBLE:    return 8;
  default: return 0;
 }
}

struct {struct dir_entry* entry; int count;} IFD_LIST[]={{IFD0,0}};

#define IFDs (sizeof(IFD_LIST)/sizeof(IFD_LIST[0]))

#define TIFF_HDR_SIZE (8)

char* tmp_buf;
int tmp_buf_size;
int tmp_buf_offset;
void add_to_buf(void* var, int size)
{
 memcpy(tmp_buf+tmp_buf_offset,var,size);
 tmp_buf_offset+=size;
}
 
char *get_date_for_exif(unsigned long time)
{
 static char buf[20];
 struct tm *ttm;
 ttm = localtime(&time);
 sprintf(buf, "%04d:%02d:%02d %02d:%02d:%02d", ttm->tm_year+1900, ttm->tm_mon+1, ttm->tm_mday, ttm->tm_hour, ttm->tm_min, ttm->tm_sec);
 return buf;
}

void create_dng_header(struct t_data_for_exif* exif_data,int hdr_type)
{
 int var;
 int i,j;
 int extra_offset;
 int raw_offset;

 for (j=0;j<IFDs;j++) 
  {
  for(i=0; IFD_LIST[j].entry[i].tag; i++) 
   {
    switch (IFD_LIST[j].entry[i].tag) 
    {
     case 0x132 :   IFD_LIST[j].entry[i].offset=(int)get_date_for_exif(exif_data->time);
                    break;

     case 0x101:    if(hdr_type && ((!shooting_get_drive_mode()&&!tl.running)||(shooting_get_drive_mode()&&!conf.raw_strip_mode)))IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWS;  
                    else if(shooting_get_drive_mode()&&!tl.running)
                    {
                     if(conf.strip_offset)
                     IFD_LIST[j].entry[i].offset=(int)conf.strip_width;                                   
                     else
                     IFD_LIST[j].entry[i].offset=(int)conf.strip_width*conf.strip_images;                 
                    } 
                    else if(tl.running)
                     IFD_LIST[j].entry[i].offset=(int)conf.strip_width*conf.strip_images;                 
                    break;   

     case 0x116:    if(hdr_type && ((!shooting_get_drive_mode()&&!tl.running)||(shooting_get_drive_mode()&&!conf.raw_strip_mode)))IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWS; 
                    else if(shooting_get_drive_mode()&&!tl.running)
                    {
                     if(conf.strip_offset)
                     IFD_LIST[j].entry[i].offset=(int)conf.strip_width;                                   
                     else
                     IFD_LIST[j].entry[i].offset=(int)conf.strip_width*conf.strip_images;                
                    } 
                    else if(tl.running)
                      IFD_LIST[j].entry[i].offset=(int)conf.strip_width*conf.strip_images;                                                         
                    break; 

     case 0x117:     if(hdr_type && ((!shooting_get_drive_mode()&&!tl.running)||(shooting_get_drive_mode()&&!conf.raw_strip_mode)))IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWPIX*CAM_RAW_ROWS*CAM_SENSOR_BITS_PER_PIXEL/8;
                    else if(shooting_get_drive_mode()&&!tl.running)
                    {
                     if(conf.strip_offset)
                     IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWPIX*conf.strip_width*CAM_SENSOR_BITS_PER_PIXEL/8;
                     else
                     IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWPIX*conf.strip_width*conf.strip_images*CAM_SENSOR_BITS_PER_PIXEL/8;
                    }
                    else if(tl.running)
                     IFD_LIST[j].entry[i].offset=(int)CAM_RAW_ROWPIX*conf.strip_width*conf.strip_images*CAM_SENSOR_BITS_PER_PIXEL/8;
                    break;
     case 0x0112:   IFD_LIST[j].entry[i].offset=get_orientation_for_exif(exif_data->orientation);        
                    break;                 
    }
   }
  }
  

 raw_offset=TIFF_HDR_SIZE;

 for (j=0;j<IFDs;j++) 
 {
  IFD_LIST[j].count=0;
  raw_offset+=6; 
  for(i=0; IFD_LIST[j].entry[i].tag; i++) 
  {
   int size_ext;
   IFD_LIST[j].count++;
   raw_offset+=12; 
   size_ext=get_type_size(IFD_LIST[j].entry[i].type)*IFD_LIST[j].entry[i].count;
   if (size_ext>4) raw_offset+=size_ext+(size_ext&1);
  }
 }

 raw_offset=(raw_offset/512+1)*512; 
 tmp_buf_size=raw_offset;
 tmp_buf=umalloc(raw_offset);
 tmp_buf_offset=0;
 if (!tmp_buf) return;

 extra_offset=TIFF_HDR_SIZE;

 for (j=0;j<IFDs;j++) 
 {
  extra_offset+=6+IFD_LIST[j].count*12; 
  for(i=0; IFD_LIST[j].entry[i].tag; i++) 
  {
   if (IFD_LIST[j].entry[i].tag==0x111)  
   {
      IFD_LIST[j].entry[i].offset=raw_offset;
   }
  }
 }

 var=0x4949;                        
 add_to_buf(&var, sizeof(short));
 var=42;                            //An arbitrary but carefully chosen number that further identifies the file as a TIFF file.
 add_to_buf(&var, sizeof(short));
 var=0x8;                           
 add_to_buf(&var, sizeof(int));

//--------------- writing IFDs  ----------------------------

 for (j=0;j<IFDs;j++) 
{
  int size_ext;
  var=IFD_LIST[j].count;
  add_to_buf(&var, sizeof(short));
  for(i=0; IFD_LIST[j].entry[i].tag; i++) 
  {
   add_to_buf(&IFD_LIST[j].entry[i].tag, sizeof(short));
   add_to_buf(&IFD_LIST[j].entry[i].type, sizeof(short));
   add_to_buf(&IFD_LIST[j].entry[i].count, sizeof(int));
   size_ext=get_type_size(IFD_LIST[j].entry[i].type)*IFD_LIST[j].entry[i].count;
   if (size_ext<=4) add_to_buf(&IFD_LIST[j].entry[i].offset, sizeof(int));
   else 
   {
    add_to_buf(&extra_offset, sizeof(int));
    extra_offset+=size_ext+(size_ext&1);    
   }
  }
  var=0; 
  add_to_buf(&var, sizeof(int));
 }

 for (j=0;j<IFDs;j++) 
{
  int size_ext;
  char zero=0;
  for(i=0; IFD_LIST[j].entry[i].tag; i++) 
   {
    size_ext=get_type_size(IFD_LIST[j].entry[i].type)*IFD_LIST[j].entry[i].count;
    if (size_ext>4)
    {
     add_to_buf((void*)IFD_LIST[j].entry[i].offset, size_ext);
     if (size_ext&1) add_to_buf(&zero, 1);
    }
   }
 }

 for (i=tmp_buf_offset; i<tmp_buf_size; i++) tmp_buf[i]=0;

}

void free_dng_header(void)
{
 ufree(tmp_buf);
 tmp_buf=NULL;
}

char* get_dng_header(void)
{
 return tmp_buf;
}

int get_dng_header_size(void)
{
 return tmp_buf_size;
}

unsigned short get_orientation_for_exif(short orientation)
{
 switch(orientation)
 {
  case 90:  return 6;
  case 270: return 8;
  default: return 1;
 }
}

struct t_data_for_exif* capture_data_for_exif(int hdr_type)
{
 static struct t_data_for_exif data;
 extern volatile long shutter_open_time; 
 int wb[3];
 if (shutter_open_time) { data.time=shutter_open_time;shutter_open_time=0;}
 else  data.time=time(NULL);
 get_property_case(PROPCASE_ORIENTATION_SENSOR, &data.orientation, sizeof(data.orientation));
 get_parameter_data(PARAM_CAMERA_NAME, &cam_name, sizeof(cam_name));
 get_property_case(PROPCASE_WB_ADJ, &wb, sizeof(wb));  
 cam_AsShotNeutral[1]=wb[1];
 cam_AsShotNeutral[3]=wb[0];
 cam_AsShotNeutral[5]=wb[2];
 cam_ActiveArea[0]=0;
if(conf.raw_strip_mode && shooting_get_drive_mode() && !tl.running)
 {
  if(conf.strip_offset)                                             
   {
    cam_ActiveArea[2]=(int)conf.strip_width;
    cam_DefaultCropSize[1]=(int)conf.strip_width;
   }
  else                                                              
   {
    cam_ActiveArea[2]=(int)conf.strip_width*conf.strip_images;
    cam_DefaultCropSize[1]=(int)conf.strip_width*conf.strip_images;
   }
 }
 else if(tl.running)                                                 
 {
  cam_ActiveArea[2]=(int)conf.strip_width*conf.strip_images;
  cam_DefaultCropSize[1]=(int)conf.strip_width*conf.strip_images;
 }
 return &data;
}

