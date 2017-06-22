//DNG related stuff

#ifndef DNG_H
#define DNG_H

struct dir_entry{unsigned short tag; unsigned short type; unsigned int count; unsigned int offset;};

#define T_BYTE 1
#define T_ASCII 2
#define T_SHORT 3
#define T_LONG 4
#define T_RATIONAL 5
#define T_SBYTE 6
#define T_UNDEFINED 7
#define T_SSHORT 8
#define T_SLONG 9
#define T_SRATIONAL 10
#define T_FLOAT 11
#define T_DOUBLE 12

 #define CAM_DEFAULT_CROP_ORIGIN_W ((CAM_ACTIVE_AREA_X2-CAM_ACTIVE_AREA_X1-CAM_JPEG_WIDTH )/2)
 #define CAM_DEFAULT_CROP_ORIGIN_H 0

//thumbnail,highly recommended that DNG_TH_WIDTH*DNG_TH_HEIGHT would be divisible by 512
#define DNG_TH_WIDTH 128
#define DNG_TH_HEIGHT 96
 

struct t_data_for_exif
{
 unsigned long time;
 short orientation;
};

void create_dng_header(struct t_data_for_exif* exif_data,int hdr_type);
void free_dng_header(void);
char* get_dng_header(void);
int get_dng_header_size(void);
unsigned short get_orientation_for_exif(short orientation);
struct t_data_for_exif* capture_data_for_exif(int hdr_type);

#endif
