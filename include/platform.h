#ifndef PLATFORM_H
#define PLATFORM_H

#include "camera.h"

#define SSID_INVALID (-32767)
#define ASID_INVALID (-32767)

#define MODE_MASK               0x0300
#define MODE_REC                0x0100
#define MODE_PLAY               0x0200
#define MODE_SCREEN_MASK        0x0C00
#define MODE_SCREEN_OPENED      0x0400
#define MODE_SCREEN_ROTATED     0x0800

#define MODE_SHOOTING_MASK      0x00FF
#define FALSE    0
#define TRUE     1
#define abs(x) ((x) >= 0 ? (x) : -(x))

#if CAM_PROPSET == 1

 #define PROPCASE_DRIVE_MODE              6
 #define PROPCASE_FOCUS_MODE             12
 #define PROPCASE_MANUAL_FOCUS           12 // 0 = auto, 1 = manual
 #define PROPCASE_FLASH_MODE             16 // 0 = auto, 1 = ON, 2 = OFF
 #define PROPCASE_CURTAIN                20
 #define PROPCASE_FLASH_OVERRIDE         28
 #define PROPCASE_FLASH_FIRED            78 
 #define PROPCASE_FLASH_MANUAL_OUTPUT    29
 #define PROPCASE_FLASH_FIRE             79
 #define PROPCASE_FLASH_ADJUST_MODE      15 // 0 = auto, 1 = manual
 #define PROPCASE_METERING_MODE           9 
 #define PROPCASE_WB_ADJ                100
 #define PROPCASE_USER_TV                40
 #define PROPCASE_CAMERA_TV	           69
 #define PROPCASE_MIN_AV                 77
 #define PROPCASE_AV		           39
 #define PROPCASE_USER_AV                39
 #define PROPCASE_ALLBEST_AV	           68
 #define PROPCASE_ISO		           21
 #define PROPCASE_ISO_MODE               21
 #define PROPCASE_SV                     73
 #define PROPCASE_DELTA_SV               70
 #define PROPCASE_SV_MARKET              72
 #define PROPCASE_BV                     71
 #define PROPCASE_SUBJECT_DIST1          65 // Subject Dist 2 ("Near limit", most linear to real distance. You can get-and-set this prop_id, but it doesnt affect the near limit and/or the focus - tested with A590).
 #define PROPCASE_SUBJECT_DIST2          66
 #define PROPCASE_SHOOTING	          205
 #define PROPCASE_WILL_USE_FLASH        221
 #define PROPCASE_OVEREXPOSURE           76
// #define PROPCASE_MF_ACTIVE            12 // should be 67 ?
 #define PROPCASE_FOCUS_OK               67 // 0 = not focused, non-zero = focused
 #define PROPCASE_SHOOTING_MODE           0 // Shooting MODE Dial position 
 #define PROPCASE_QUALITY                23 // 0,1,2 from best to worst 
 #define PROPCASE_RESOLUTION             24 // S3IS: 0,1,2,4,8 for L,M1,M2,S,W 
 #define PROPCASE_IS_MODE	          229 // IS (0,1,2,3 = continuous, shoot only, panning, off) 
 #define PROPCASE_EV_CORRECTION_1        25 // 	EV correction (units: +/- 96 per 1 stop range) (On s3is and i guess on more models: Also used in movie mode!, but AE lock has to be activated -> 205 to 1)
 #define PROPCASE_EV_CORRECTION_2        26
 #define PROPCASE_ORIENTATION_SENSOR     37 // SD900: 0=Normal, 270=Left, 90=Right also S2: -1=inverse not 180 
 #define PROPCASE_DIGITAL_ZOOM_STATE     58 // S3IS: 1=On, 0=Off SD900: 2=1.4x / 2.3x, 1=Default, 0=Off 
 #define PROPCASE_DIGITAL_ZOOM_POSITION  57 // S3IS: 0 - 6, 0 is no digital zoom, 6 is "most digital zoom" (48x)SD900: 3=2.3x, 1=1.4x, 0=no Zoom / Default 
 #define PROPCASE_DISPLAY_MODE          181 // Display mode (record mode only) (0 = show info icons, 1 = Do not show info icons, 2 = LCD off, 3 = EVF) 
 #define PROPCASE_OPTICAL_ZOOM_POSITION  99 // tested a540, sd400
#define PROPCASE_STITCH_DIRECTION        52 // 0=left>right, 1=right>left. Some cams have more
#define PROPCASE_STITCH_SEQUENCE         48 // counts shots in stitch sequence, positive for left-right, negative for right-left
#define PROPCASE_AF_LOCK                    193

#elif CAM_PROPSET == 2 
 #define PROPCASE_DRIVE_MODE            102
 #define PROPCASE_FOCUS_MODE            133	// 0 = Auto and Macro, 1 = manual
 #define PROPCASE_MANUAL_FOCUS          133 // 0 = auto, 1 = manual
 #define PROPCASE_FLASH_MODE            143
 #define PROPCASE_CURTAIN                64     
 #define PROPCASE_FLASH_OVERRIDE        127
 #define PROPCASE_FLASH_FIRED           122 
 #define PROPCASE_FLASH_MANUAL_OUTPUT   141
 #define PROPCASE_FLASH_FIRE            122
 #define PROPCASE_FLASH_ADJUST_MODE     121
 #define PROPCASE_METERING_MODE         155
 #define PROPCASE_WB_ADJ                269 
 #define PROPCASE_USER_TV               264
 #define PROPCASE_CAMERA_TV	          262
 #define PROPCASE_MIN_AV                 25
 #define PROPCASE_AV		           26
 #define PROPCASE_USER_AV                26
 #define PROPCASE_ALLBEST_AV	           23
 #define PROPCASE_BV                     34
 #define PROPCASE_SUBJECT_DIST1         245
 #define PROPCASE_SUBJECT_DIST2          65
 #define PROPCASE_ISO		          149
 #define PROPCASE_ISO_MODE              149
 #define PROPCASE_SV                    247
 #define PROPCASE_DELTA_SV               79
 #define PROPCASE_SV_MARKET             246
 #define PROPCASE_SHOOTING	          206
 #define PROPCASE_WILL_USE_FLASH        208
 #define PROPCASE_OVEREXPOSURE          103
 #define PROPCASE_FOCUS_OK              115 // ready to shoot, focus OK
 #define PROPCASE_SHOOTING_MODE          49
 #define PROPCASE_QUALITY                57 
 #define PROPCASE_RESOLUTION            218 
 #define PROPCASE_IS_MODE	          145  // 0/1/2/3  CONT/SHOOT/PAN/OFF
 #define PROPCASE_EV_CORRECTION_1       107
 #define PROPCASE_EV_CORRECTION_2       207
 #define PROPCASE_ORIENTATION_SENSOR    219
 #define PROPCASE_DIGITAL_ZOOM_STATE     94
 #define PROPCASE_DIGITAL_ZOOM_POSITION  95
 #define PROPCASE_DIGITAL_ZOOM_MODE      91      //G10 Digital Zoom Mode/State 0 = off/standard, 2 = 1.7x, 3 = 2.1x
 #define PROPCASE_DISPLAY_MODE          105
 #define PROPCASE_TIMER_MODE            223 
 #define PROPCASE_OPTICAL_ZOOM_POSITION 195
 #define PROPCASE_CANON_RAW		     57
#define PROPCASE_STITCH_DIRECTION       233     //D10 0=left>right, 1=right>left
#define PROPCASE_STITCH_SEQUENCE        238     //D10 counts shots in stitch sequence, positive=left>right, negative=right>left
#define PROPCASE_AE_LOCK                    3       //G10 0 = AE not locked, 1 = AE locked
#define PROPCASE_AF_LOCK                    11
#define PROPCASE_AF_FRAME                   8       //G10 1 = FlexiZone, 2 = Face AiAF / Tracking AF
#define PROPCASE_CONTINUOUS_AF              12      //G10 0 = Continuous AF off, 1 = Continuous AF on
#define PROPCASE_ND_FILTER_STATE            192     // 0 = out, 1 = in

#elif CAM_PROPSET == 3
#define PROPCASE_DRIVE_MODE			102
#define PROPCASE_FOCUS_MODE			133	// 0 = Auto and Macro, 1 = manual
#define PROPCASE_MANUAL_FOCUS             133
#define PROPCASE_FLASH_MODE    		143
#define PROPCASE_CURTAIN                   64
#define PROPCASE_FLASH_OVERRIDE           127 // not confirmed, added for compilation
#define PROPCASE_FLASH_FIRED		      122
#define PROPCASE_FLASH_MANUAL_OUTPUT      141
#define PROPCASE_FLASH_FIRE               122
#define PROPCASE_FLASH_ADJUST_MODE        121
#define PROPCASE_METERING_MODE		155
#define PROPCASE_WB_MODE                  270     // 0 = Auto, 1 = Daylight, 2 = Cloudy, 3 = Tungsten, 4 = Fluorescent, 5 = Fluorescent H, 7 = Custom
#define PROPCASE_WB_ADJ                   271
#define PROPCASE_CAMERA_TV	            264
#define PROPCASE_USER_TV       		266
#define PROPCASE_MIN_AV       		 25
#define PROPCASE_AV            		 26 // this is CHDK PROPCASE_USER_AV
#define PROPCASE_USER_AV      		 26
#define PROPCASE_ALLBEST_AV	             23 // this is CHDK PROPCASE_AV
#define PROPCASE_BV            		 34
#define PROPCASE_SUBJECT_DIST1 		247
#define PROPCASE_SUBJECT_DIST2 		 65
#define PROPCASE_ISO		    	      149
#define PROPCASE_ISO_MODE		    	149
#define PROPCASE_SV            		249
#define PROPCASE_DELTA_SV      		 79
#define PROPCASE_SV_MARKET     		248
#define PROPCASE_SHOOTING      		208
#define PROPCASE_WILL_USE_FLASH           210
#define PROPCASE_OVEREXPOSURE 		103
#define PROPCASE_FOCUS_OK                 115   // guessed, for compiling only
#define PROPCASE_SHOOTING_MODE		 49
#define PROPCASE_QUALITY			 57
#define PROPCASE_RESOLUTION			220
#define PROPCASE_IS_MODE			145 // 0/1/2/3  CONT/SHOOT/PAN/OFF
#define PROPCASE_EV_CORRECTION_1		107
#define PROPCASE_EV_CORRECTION_2		209
#define PROPCASE_ORIENTATION_SENSOR		221
#define PROPCASE_DIGITAL_ZOOM_STATE        94
#define PROPCASE_DIGITAL_ZOOM_POSITION     95
#define PROPCASE_DIGITAL_ZOOM_MODE         91     // Digital Zoom Mode/State 0 = off/standard, 2 = 1.4x, 3 = 2.3x
#define PROPCASE_DISPLAY_MODE             105
#define PROPCASE_TIMER_MODE               225
#define PROPCASE_FLASH_MANUAL_OUTPUT      141
#define PROPCASE_FLASH_ADJUST_MODE       	121
#define PROPCASE_BRACKET_MODE              29
#define PROPCASE_ASPECT_RATIO             294
#define PROPCASE_OPTICAL_ZOOM_POSITION    197
#define PROPCASE_AF_LOCK                    11      // 0 = AF not locked, 1 = AF locked
#define PROPCASE_CONTINUOUS_AF              12      // 0 = Continuous AF off, 1 = Continuous AF on
#define PROPCASE_SERVO_AF                   297     // 0 = Servo AF off, 1 = Servo AF on

#elif CAM_PROPSET == 4

#define PROPCASE_AV2                       22  // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD 
#define PROPCASE_ALLBEST_AV			 23  // This values causes the actual aperture value to be overriden 
#define PROPCASE_MIN_AV				 25
#define PROPCASE_AV				 26
#define PROPCASE_USER_AV			 26
#define PROPCASE_BRACKET_MODE			 29
#define PROPCASE_BV				 34
#define PROPCASE_FOCUS_OK                 115   // guessed, for compiling only
#define PROPCASE_SHOOTING_MODE		 49
#define PROPCASE_QUALITY			 57
#define PROPCASE_FLASH_SYNC_CURTAIN		 64
#define PROPCASE_SUBJECT_DIST2		 65
#define PROPCASE_DELTA_SV			 79
#define PROPCASE_DIGITAL_ZOOM_MODE         91 // Digital Zoom Mode/State 0 = off/standard, 2 = 1.7x, 3 = 2.1x
#define PROPCASE_DIGITAL_ZOOM_STATE		 94 // Digital Zoom Mode/State 0 = Digital Zoom off, 1 = Digital Zoom on 
#define PROPCASE_DIGITAL_ZOOM_POSITION     95
#define PROPCASE_DRIVE_MODE			102
#define PROPCASE_WILL_USE_FLASH           212
#define PROPCASE_OVEREXPOSURE 		103
#define PROPCASE_DISPLAY_MODE			105
#define PROPCASE_EV_CORRECTION_1		107
#define PROPCASE_FLASH_ADJUST_MODE       	121
#define PROPCASE_FLASH_FIRE			122
#define PROPCASE_FLASH_FIRED			122
#define PROPCASE_FOCUS_MODE			133 // 0 = Auto and Macro, 1 = manual
#define PROPCASE_FLASH_MODE			143
#define PROPCASE_FLASH_MANUAL_OUTPUT    	141
#define PROPCASE_FLASH_EXP_COMP           127    // APEX96 units
#define PROPCASE_EXT_FLASH_EXP_COMP       289    // APEX96 units (for external flash if connected)
#define PROPCASE_IS_MODE			145  // 0 = Continuous, 2 = only Shoot, 3 = pan, 4 = OFF 
#define PROPCASE_ISO_MODE		    	149
#define PROPCASE_ISO		    		149
#define PROPCASE_FLASH_OVERRIDE           127  // confirmed on S95
#define PROPCASE_VIDEO_FRAMERATE          168     // 0=30fps, 1=24fps, 2=240fps, 3=120fps; timelapse in miniature: 8=5x, 9=10x, 10=20x (SX220)
#define PROPCASE_VIDEO_RESOLUTION         171     // 1=240p, 2=480p, 4=720p, 5=1080p (SX220)
#define PROPCASE_AE_LOCK			  3 // 0 = AE not locked, 1 = AE locked
#define PROPCASE_AF_LOCK                   11 // 0 = AF not locked, 1 = AF locked 
#define PROPCASE_AF_FRAME			  8 // 1 = FlexiZone, 2 = Face AiAF / Tracking AF
#define PROPCASE_CONTINUOUS_AF		 12 // 0 = Continuous AF off, 1 = Continuous AF on
#define PROPCASE_SERVO_AF	            299 // 0 = Servo AF off, 1 = Servo AF on
#define PROPCASE_METERING_MODE		157
#define PROPCASE_OPTICAL_ZOOM_POSITION	199
#define PROPCASE_EV_CORRECTION_2		211
#define PROPCASE_RESOLUTION			222 // 0 = L, 1 = M1, 2 = M2, 4 = S, 7 = Low Light (G12)
#define PROPCASE_ORIENTATION_SENSOR		223
#define PROPCASE_TIMER_MODE			227
#define PROPCASE_STITCH_DIRECTION         237     // 0=left>right, 1=right>left. Some cams have more
#define PROPCASE_STITCH_SEQUENCE          242     // counts shots in stitch sequence, positive=left>right, negative=right>left
#define PROPCASE_SUBJECT_DIST1		249
#define PROPCASE_SV_MARKET			250
#define PROPCASE_SV				347  // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD 
#define PROPCASE_CAMERA_TV			266  // Need to set this value for overrides to work correctly  
#define PROPCASE_TV2                      265  // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_USER_TV                  268
#define PROPCASE_WB_MODE                  272     // WARNING!!!: This was not verified. 0=AWB, 1=Sunlight, 2=Cloudy, 3=Tungsten, 4=Fluorescent, 5=Fluorescent H, 6=Flash, 7=Underwater, 8=Custom 1, 9=Custom
#define PROPCASE_WB_ADJ                   273
#define PROPCASE_ASPECT_RATIO			300
#define PROPCASE_SHOOTING			303
#define PROPCASE_GPS                      359  //for sx230hs
#define PROPCASE_CANON_RAW		      214  // 0 = RAW,1 = JPG, 2 = RAW+JPG
#define PROPCASE_ND_FILTER_STATE          196     // 0 = out, 1 = in

#elif CAM_PROPSET == 5

#define PROPCASE_AE_LOCK                         3          // 0 = AE not locked, 1 = AE locked
#define PROPCASE_REAL_FOCUS_MODE                 6          //??? WIKI|Propcase focus_mode
#define PROPCASE_AF_FRAME                        8          // 1 = FlexiZone, 2 = Face AiAF / Tracking AF
#define PROPCASE_AF_LOCK                         11         // 0 = AF not locked, 1 = AF locked
#define PROPCASE_CONTINUOUS_AF                   12         // 0 = Continuous AF off, 1 = Continuous AF on
#define PROPCASE_FOCUS_STATE                     18         //???
#define PROPCASE_FOCUS_OK                       115   // guessed, for compiling only
#define PROPCASE_AV2                             22         // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_ALLBEST_AV                      23 
#define PROPCASE_AV                              23         // This values causes the actual aperture value to be overriden
#define PROPCASE_MIN_AV                          25
#define PROPCASE_USER_AV                         26
#define PROPCASE_BRACKET_MODE                    29
#define PROPCASE_BV                              34
#define PROPCASE_SHOOTING_MODE                   49
#define PROPCASE_QUALITY                         57
#define PROPCASE_FLASH_SYNC_CURTAIN              64
#define PROPCASE_SUBJECT_DIST2                   65    
#define PROPCASE_DELTA_SV                        79
#define PROPCASE_DIGITAL_ZOOM_MODE               91         // Digital Zoom Mode/State 0 = off/standard, 2 = 1.5x, 3 = 2.0x
#define PROPCASE_DIGITAL_ZOOM_STATE              94         // Digital Zoom Mode/State 0 = Digital Zoom off, 1 = Digital Zoom on
#define PROPCASE_DIGITAL_ZOOM_POSITION           95
#define PROPCASE_DRIVE_MODE                      102
#define PROPCASE_OVEREXPOSURE                    103
#define PROPCASE_DISPLAY_MODE                    105
#define PROPCASE_EV_CORRECTION_1                 107
#define PROPCASE_FLASH_ADJUST_MODE               121
#define PROPCASE_FLASH_FIRE                      122
#define PROPCASE_FLASH_FIRED                     122
#define PROPCASE_FOCUS_MODE                      133		// 0 = Auto and Macro, 1 = manual
#define PROPCASE_FLASH_MANUAL_OUTPUT             141        // !not sure, but required for compile; from propset4
#define PROPCASE_FLASH_MODE                      143        // 0 = Auto, 1 = ON, 2 = OFF
#define PROPCASE_FLASH_EXP_COMP                  127    // APEX96 units
#define PROPCASE_IS_MODE                         145        // 0 = Continuous, 2 = only Shoot, 3 = pan, 4 = OFF
#define PROPCASE_ISO                             149
#define PROPCASE_ISO_MODE                        149
#define PROPCASE_FLASH_OVERRIDE                  127  // not confirmed, added for compilation
#define PROPCASE_METERING_MODE                   157
#define PROPCASE_ND_FILTER_STATE                 196        // 0 = out, 1 = in
#define PROPCASE_OPTICAL_ZOOM_POSITION           199
#define PROPCASE_SHOOTING                        210
#define PROPCASE_EV_CORRECTION_2                 211
#define PROPCASE_WILL_USE_FLASH                  212
#define PROPCASE_IS_FLASH_READY                  212
#define PROPCASE_RESOLUTION                      222        // 0 = L, 1 = M1, 2 = M2, 4 = S, 7 = Low Light
#define PROPCASE_ORIENTATION_SENSOR              223
#define PROPCASE_TIMER_MODE                      227        // 0 = OFF, 1 = 2 sec, 2 = 10 sec, 3 = Costom
#define PROPCASE_TIMER_DELAY                     228        // timer delay in msec
#define PROPCASE_STITCH_DIRECTION                237        // 0=left>right, 1=right>left. Some cams have more
#define PROPCASE_STITCH_SEQUENCE                 242        // counts shots in stitch sequence, positive=left>right, negative=right>left
#define PROPCASE_SUBJECT_DIST1                   249
#define PROPCASE_SV_MARKET                       250
#define PROPCASE_TV2                             265        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_CAMERA_TV                       266
#define PROPCASE_USER_TV                         268
#define PROPCASE_WB_ADJ                          273
#define PROPCASE_SERVO_AF                        299        // 0 = Servo AF off, 1 = Servo AF on
#define PROPCASE_ASPECT_RATIO                    300        // 0 = 4:3, 1 = 16:9, 2 = 3:2, 3 = 1:1
#define PROPCASE_SV                              347        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_GPS                             358        // (CHDKLover, August 2011) - contains a 272 bytes long structure
#define PROPCASE_CANON_RAW		      	 214        // 0 = RAW,1 = JPG, 2 = RAW+JPG ********** CHECK THIS !!!! **********
#define PROPCASE_VIDEO_RESOLUTION			 170        // 5 = 1920x1080,4 = 1280x720,2 = 640x480,1 = 320x240
#define PROPCASE_VIDEO_FRAMERATE                 168        // 1=25 2=24 0=30 3=240 4 = 120 , miniature mode 9=5x 10=10x 11=20x (sx160)  may crash if doesn't match what canon uses for current resolution 

#elif CAM_PROPSET == 6
#define PROPCASE_AE_LOCK                         3          // 0 = AE not locked, 1 = AE locked
#define PROPCASE_AF_ASSIST_BEAM                  5          // 0=disabled,  1=enabled
#define PROPCASE_REAL_FOCUS_MODE                 6          //??? WIKI|Propcase focus_mode
#define PROPCASE_AF_FRAME                        8          // 1 = FlexiZone, 2 = Face AiAF / Tracking AF
#define PROPCASE_AF_LOCK                         11         // 0 = AF not locked, 1 = AF locked
#define PROPCASE_CONTINUOUS_AF                   12         // 0 = Continuous AF off, 1 = Continuous AF on
#define PROPCASE_FOCUS_STATE                     18         //???
#define PROPCASE_AV2                             22         // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_ALLBEST_AV                      23         // This values causes the actual aperture value to be overriden
#define PROPCASE_AV                              23
#define PROPCASE_MIN_AV                          25
#define PROPCASE_USER_AV                         26
#define PROPCASE_BRACKET_MODE                    29
#define PROPCASE_BV                              34
#define PROPCASE_SHOOTING_MODE                   49
#define PROPCASE_CUSTOM_SATURATION               55         // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_QUALITY                         57
#define PROPCASE_CUSTOM_CONTRAST                 59         // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_FLASH_SYNC_CURTAIN              64
#define PROPCASE_SUBJECT_DIST2                   65
#define PROPCASE_DATE_STAMP                      66         // 0 = Off, 1 = Date, 2 = Date & Time
#define PROPCASE_DELTA_SV                        79
#define PROPCASE_DIGITAL_ZOOM_MODE               91         // Digital Zoom Mode/State 0 = off/standard, 2 = 1.5x, 3 = 2.0x
#define PROPCASE_DIGITAL_ZOOM_STATE              94         // Digital Zoom Mode/State 0 = Digital Zoom off, 1 = Digital Zoom on
#define PROPCASE_DIGITAL_ZOOM_POSITION           95
#define PROPCASE_DRIVE_MODE                      102
#define PROPCASE_OVEREXPOSURE                    103
#define PROPCASE_DISPLAY_MODE                    105
#define PROPCASE_EV_CORRECTION_1                 107
#define PROPCASE_FOCUS_OK                       115   // guessed, for compiling only
#define PROPCASE_FLASH_ADJUST_MODE               121
#define PROPCASE_FLASH_FIRE                      122
#define PROPCASE_FLASH_FIRED                     122
#define PROPCASE_FLASH_OVERRIDE                  127  // not confirmed, added for compilation
#define PROPCASE_FOCUS_MODE                      133	// 0 = Auto and Macro, 1 = manual
#define PROPCASE_FLASH_MANUAL_OUTPUT             141        // !not sure, but required for compile; from propset4
#define PROPCASE_FLASH_MODE                      143        // 0 = Auto, 1 = ON, 2 = OFF
#define PROPCASE_FLASH_EXP_COMP                  127    // APEX96 units
#define PROPCASE_IS_MODE                         145        // 0 = Continuous, 1 = only Shoot, 2 = OFF
#define PROPCASE_ISO                             149
#define PROPCASE_ISO_MODE                        149
#define PROPCASE_METERING_MODE                   157        // 0 = Evaluative, 1 = Spot, 2 = Center weighted avg

#define PROPCASE_CUSTOM_BLUE                     176        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_GREEN                    177        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_RED                      178        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_SKIN_TONE                179        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_MY_COLORS                       187        // 0 = Off, 1 = Vivid, 2 = Neutral, 3 = B/W, 4 = Sepia, 5 = Positive Film, 6 = Lighter Skin Tone, 7 = Darker Skin Tone, 8 = Vivid Red, 9 = Vivid Green, 10 = Vivid Blue, 11 = Custom Color
#define PROPCASE_ND_FILTER_STATE                 195        // 0 = out, 1 = in
#define PROPCASE_OPTICAL_ZOOM_POSITION           198 
#define PROPCASE_SHOOTING                        209 
#define PROPCASE_EV_CORRECTION_2                 210
#define PROPCASE_WILL_USE_FLASH                  211
#define PROPCASE_RESOLUTION                      221        // 0 = L, 1 = M1, 2 = M2, 4 = S, 7 = Low Light
#define PROPCASE_ORIENTATION_SENSOR              222
#define PROPCASE_TIMER_MODE                      226        // 0 = OFF, 1 = 2 sec, 2 = 10 sec, 3 = Custom
#define PROPCASE_TIMER_DELAY                     227        // timer delay in msec
#define PROPCASE_CUSTOM_SHARPNESS                228        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_STITCH_DIRECTION                236        // 0=left>right, 1=right>left. Some cams have more
#define PROPCASE_STITCH_SEQUENCE                 241        // counts shots in stitch sequence, positive=left>right, negative=right>left
#define PROPCASE_SUBJECT_DIST1                   248
#define PROPCASE_SV_MARKET                       249
#define PROPCASE_TV2                             264        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_CAMERA_TV                       265        // Need to set this value for overrides to work correctly
#define PROPCASE_USER_TV                         267
#define PROPCASE_WB_MODE                         271        // 0 = Auto, 1 = Daylight, 2 = Cloudy, 3 = Tungsten, 4 = Fluorescent, 5 = Fluorescent H, 7 = Custom
#define PROPCASE_WB_ADJ                          272
#define PROPCASE_SERVO_AF                        298        // 0 = Servo AF off, 1 = Servo AF on
#define PROPCASE_ASPECT_RATIO                    299        // 0 = 4:3, 1 = 16:9, 2 = 3:2, 3 = 1:1
#define PROPCASE_SV                              346        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_GPS                             357        // (CHDKLover, August 2011) - contains a 272 bytes long structure
#define PROPCASE_CANON_RAW		     		 214  // 0 = RAW,1 = JPG, 2 = RAW+JPG ********** CHECK THIS !!!! **********
#define PROPCASE_VIDEO_FRAMERATE                 167        // 1=25, 0=30 (elph130),5 = 60,4 = 120,3 = 240, may crash if doesn't match what canon uses for current resolution 
#define PROPCASE_VIDEO_RESOLUTION                170        // 5 = 1920x1080,4=1280x720, 2=640x480 (elph130),1 = 320x240  
// !not sure
//#define PROPCASE_DIGITAL_ZOOM_MODE               91         // Digital Zoom Mode/State 0 = off/standard, 2 = 1.7x, 3 = 2.1x

#elif CAM_PROPSET == 7

#define PROPCASE_AE_LOCK                         3          // 0 = AE not locked, 1 = AE locked
#define PROPCASE_AF_ASSIST_BEAM                  5          // 0=disabled,  1=enabled
#define PROPCASE_REAL_FOCUS_MODE                 6          // 0 = AF, 1 = Macro, 4 = MF (g7x)
#define PROPCASE_AF_FRAME                        8          // 1 = FlexiZone, 2 = Face AiAF / Tracking AF
#define PROPCASE_AF_LOCK                         11         // 0 = AF not locked, 1 = AF locked (not verified, g7x AF lock just enables MF at current dist)
#define PROPCASE_CONTINUOUS_AF                   12         // 0 = Continuous AF off, 1 = Continuous AF on (g7x)
#define PROPCASE_FOCUS_STATE                     18         // 1 AF done
// G7x both AV, not verified which does over and which does exif
#define PROPCASE_AV2                             22         // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_AV                              23         // This values causes the actual aperture value to be overriden
#define PROPCASE_ALLBEST_AV                      23
// mismatch from propset 6 starts here, +2
#define PROPCASE_MIN_AV                          27         // or 28?
// ps6 +4
#define PROPCASE_USER_AV                         30         // or 29, values differ slightly. 29 appears to have round APEX96 vals, 30 matches PROPCASE_AV
#define PROPCASE_BRACKET_MODE                    33         // 0 = 0ff, 1 = exposure, 2 = focus (MF only) (g7x)
#define PROPCASE_BV                              38
#define PROPCASE_SHOOTING_MODE                   53         // 54 shows C as distinct mode
// ps6 +5
#define PROPCASE_CUSTOM_SATURATION               60         // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_QUALITY                         62
#define PROPCASE_CUSTOM_CONTRAST                 64         // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_FLASH_SYNC_CURTAIN              69         // 0 first, 1 second
#define PROPCASE_SUBJECT_DIST2                   70
// TODO Guessed as ps6 +5, g7x has no date stamp option
//#define PROPCASE_DATE_STAMP                      71         // 0 = Off, 1 = Date, 2 = Date & Time
#define PROPCASE_DELTA_SV                        84         // TODO not certain
// ps6 + 6
// TODO maybe different from older cams (off / standard are different)
#define PROPCASE_DIGITAL_ZOOM_MODE               97         // Digital Zoom Mode/State 0 = off, 1=standard, 2 = 1.5x, 3 = 2.0x
// TODO does not seem to exist in ps7, combined with _MODE
// #define PROPCASE_DIGITAL_ZOOM_STATE           
#define PROPCASE_DIGITAL_ZOOM_POSITION           101        // also 269?
#define PROPCASE_DRIVE_MODE                      108        // 0 = single, 1 = cont, 2 = cont AF
#define PROPCASE_OVEREXPOSURE                    109        // TODO guessed
#define PROPCASE_DISPLAY_MODE                    111
#define PROPCASE_EV_CORRECTION_1                 113
#define PROPCASE_FLASH_OVERRIDE                  127  // not confirmed, added for compilation
#define PROPCASE_FLASH_ADJUST_MODE               127    // TODO ??? guessed ps6 +6
#define PROPCASE_FLASH_FIRE                      128    // TODO guessed ps6 +6
#define PROPCASE_FLASH_FIRED                     128
#define PROPCASE_FLASH_EXP_COMP                  133    // APEX96 units
#define PROPCASE_FOCUS_MODE                      139    // 0 = auto, 1 = MF
#define PROPCASE_FLASH_MANUAL_OUTPUT             147        // TODO guessed (ps6 had unsure comments too)
#define PROPCASE_FLASH_MODE                      149        // 0 = Auto, 1 = ON, 2 = OFF
// TODO values changed?
#define PROPCASE_IS_MODE                         151        // 0 = Continuous, 1 = only Shoot, 2 = OFF (399 "dynamic IS" setting?)
#define PROPCASE_ISO_MODE                        155
#define PROPCASE_ISO                             155
#define PROPCASE_METERING_MODE                   163        // 0 = Evaluative, 1 = Spot, 2 = Center weighted avg
#define PROPCASE_VIDEO_FRAMERATE                 173        // 0=30,1=25,3=24,7=60,8=50
#define PROPCASE_VIDEO_RESOLUTION                176        // 5=1920x1280, 4=1280x720 2=640x480 (g7x)
#define PROPCASE_CUSTOM_BLUE                     182        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_GREEN                    182        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_RED                      184        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_CUSTOM_SKIN_TONE                185        // Canon Menu slide bar values: 255, 254, 0, 1, 2
#define PROPCASE_MY_COLORS                       193        // 0 = Off, 1 = Vivid, 2 = Neutral, 3 = B/W, 4 = Sepia, 5 = Positive Film, 6 = Lighter Skin Tone, 7 = Darker Skin Tone, 8 = Vivid Red, 9 = Vivid Green, 10 = Vivid Blue, 11 = Custom Color
#define PROPCASE_ND_FILTER_STATE                 201        // 0 = out, 1 = in
#define PROPCASE_OPTICAL_ZOOM_POSITION           204 
#define PROPCASE_EXPOSURE_LOCK                   215        // Old PROPCASE_SHOOTING value - gets set when set_aelock called or AEL button pressed
#define PROPCASE_EV_CORRECTION_2                 216        // g7x ok, ps6 +6
#define PROPCASE_IS_FLASH_READY                  217        // not certain
#define PROPCASE_WILL_USE_FLASH                  217
#define PROPCASE_IMAGE_FORMAT                    219        // 0 = RAW, 1 = JPEG, 2 = RAW+JPEG (g7x)
#define PROPCASE_RESOLUTION                      227        // 0 = L, 2 = M1, 3 = M2, 5 = S
#define PROPCASE_ORIENTATION_SENSOR              228
#define PROPCASE_TIMER_MODE                      232        // 0 = OFF, 1 = 2 sec, 2 = 10 sec, 3 = Custom
#define PROPCASE_TIMER_DELAY                     233        // timer delay in msec
#define PROPCASE_CUSTOM_SHARPNESS                234        // Canon Menu slide bar values: 255, 254, 0, 1, 2
// TODO guessed propset 6 +6, stitch not present on g7x
//#define PROPCASE_STITCH_DIRECTION                242        // 0=left>right, 1=right>left. Some cams have more
//#define PROPCASE_STITCH_SEQUENCE                 247        // counts shots in stitch sequence, positive=left>right, negative=right>left
// g7x OK propset 6 +6
#define PROPCASE_SUBJECT_DIST1                   254        // 262 MF value?
#define PROPCASE_SV_MARKET                       255
// ps6 + 7, TV vs TV2 not verified
#define PROPCASE_TV2                             271        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
#define PROPCASE_TV                              272        // Need to set this value for overrides to work correctly
#define PROPCASE_CAMERA_TV                       272
#define PROPCASE_USER_TV                         274
// ps6 + 8, TODO note values changed from ps6
#define PROPCASE_WB_MODE                         279        // 0 = Auto, 1 = Daylight, 2 = Shade, 3 = Cloudy, 4 = Tungsten, 5 = Fluorescent, 7 = flash, 11 = under water, 6 = Fluorescent H, 9 = Custom 1, 10 = custom 2
#define PROPCASE_WB_ADJ                          280
#define PROPCASE_SERVO_AF                        306        // 0 = Servo AF off, 1 = Servo AF on
#define PROPCASE_ASPECT_RATIO                    207        // 0 = 4:3, 1 = 16:9, 2 = 3:2, 3 = 1:1, 4 = 4:5
#define PROPCASE_SV                              354        // (philmoz, May 2011) - this value causes overrides to be saved in JPEG and shown on Canon OSD
// TODO GPS guessed, ps6 + 8
// #define PROPCASE_GPS                             365        // (CHDKLover, August 2011) - contains a 272 bytes long structure
#define PROPCASE_TIMER_SHOTS                     384        // Number of shots for TIMER_MODE=Custom
#define PROPCASE_SHOOTING_STATE                  359        // Goes to 1 soon after half press, 2 around when override hook called, 3 after shot start, back to 2 when shoot_full released, back to 0 when half released
#define PROPCASE_SHOOTING                       1001        // fake, emulated by wrapper using SHOOTING_STATE
#define PROPCASE_FOCUS_OK                       115   // guessed, for compiling only

#else
 #error unknown camera processor

#endif

// Video recording current status constants, see movie_status variable
#define VIDEO_RECORD_NEVER_STARTED    0
#define VIDEO_RECORD_STOPPED          1
#define VIDEO_RECORD_IN_PROGRESS      4
#define VIDEO_RECORD_STOPPING		  5
#define VIDEO_DIAL_BUT_NOT_RECORDING  6

//Optical & digital zoom status constants, see zoom_status variable
#define ZOOM_OPTICAL_MIN         1
#define ZOOM_OPTICAL_MAX         2
#define ZOOM_OPTICAL_MEDIUM      3
#define ZOOM_OPTICAL_IN          4
#define ZOOM_OPTICAL_OUT         5
#define ZOOM_OPTICAL_REACHED_MIN 7

#define ZOOM_DIGITAL_MAX         9
#define ZOOM_DIGITAL_MEDIUM      10
#define ZOOM_DIGITAL_IN          11
#define ZOOM_DIGITAL_OUT         12
#define ZOOM_DIGITAL_REACHED_MAX 13

//Keyboard repeat and initial delays   only used by ixus65 ?
#define KBD_REPEAT_DELAY  140
#define KBD_INITIAL_DELAY 300

#define SET_NOW      1
#define SET_LATER    0
#define INFINITY 65500
#define MIN_DIST                    CAMERA_MIN_DIST     // Defined in camera.h (can be overridden in platform_camera.h)
#define MAX_DIST                    CAMERA_MAX_DIST     // Defined in camera.h (can be overridden in platform_camera.h)
#define INFINITY_DIST               0xFFFFFFFF          // Value to send to firmware to select 'infinity' focus

// flash hot shoe sense bit override macro, for use in platform kbd.c
#ifdef CAM_HOTSHOE_OVERRIDE
#define HOTSHOE_OVERRIDE \
    if (conf.hotshoe_override == 1) { \
        physw_status[HOTSHOE_IDX] = physw_status[HOTSHOE_IDX] & ~HOTSHOE_FLAG; \
    } \
    else if (conf.hotshoe_override == 2) { \
        physw_status[HOTSHOE_IDX] = physw_status[HOTSHOE_IDX] | HOTSHOE_FLAG; \
    }
#endif

typedef struct
{
 short Tv96;
 short Av96;
 short RealAV;
 short Bv96;
 short Sv96;
 long FocusFromSensor;
 long FocusFromLens;
 int NearPoint;
 int FarPoint;
 int Hyperfocal;
 long ZoomPoint;
 int FocalLength;
 int ISO;
 int FlashMode;
}onHP;

struct _kap
{
 short Tv96;
 short Av96;
 short RealAV;
 short Bv96;
 short Sv96;
 unsigned char nd;
}kap;

struct _fast_params
{
 short exp_comp;
 short pref_ap;
 short min_ap;
 short ev_step;
}fast_params;

typedef struct 
{
 int hackmode;  // platform independent mode values from MODE_xxx enum
 int canonmode;// PROPCASE_SHOOTING_MODE value
} CapturemodeMap;

typedef struct {
    short tv96;
    short dtv96;
    short tv96_step;
    int subj_dist;
    short dsubj_dist;
    short subj_dist_step;
    short shoot_counter;
    short type;
} EXPO_BRACKETING_VALUES;

typedef struct {
    short av96;
    short tv96;
    short sv96;
    short subj_dist;
    short nd_filter;
} PHOTO_PARAM;

typedef struct {
    int led_num;
    int action;
    int brightness;
    int blink_count;
} LED_CONTROL;

struct _HPTimer
{
 unsigned int units;
 unsigned int interval;
}HPTimer;

enum SwitchType
 {
  NOT_DEFINED=0,
  RICOH,
  STANDARD,     
 };

enum 
{
 DIRECT	=0,
 C_FAST,
 T_FAST,
 S_FAST,
 BURST,
 SPORTS,
 FLAPSE,
 T_SYNC,
 STEREO,
};

enum {
	SCRIPT_CONSOLE_NUM_LINES = 3,
	SCRIPT_CONSOLE_LINE_LENGTH  = 36,
	};

enum {
MODE_AUTO               =1,
MODE_P                  ,
MODE_TV                 ,
MODE_AV                 ,
MODE_M                  ,
MODE_PORTRAIT           ,
MODE_NIGHT              ,
MODE_LANDSCAPE          ,
MODE_VIDEO_STD          ,
MODE_VIDEO_SPEED        ,  // 10
MODE_VIDEO_COMPACT      ,
MODE_VIDEO_MY_COLORS    ,
MODE_VIDEO_COLOR_ACCENT ,
MODE_VIDEO_COLOR_SWAP   , 
MODE_STITCH             ,
MODE_MY_COLORS          ,
MODE_SCN_UNDERWATER,   
MODE_SCN_NIGHT_SNAPSHOT ,
MODE_LONG_SHUTTER       , // "long shutter" mode on cameras without true manual mode. Allows manual shutter >= 1 second, uses manual shutter value propcase. Usually found under func menu in "manual" mode.
MODE_SCN_LANDSCAPE      , // 20
MODE_COLOR_SWAP         ,
MODE_SCN_SNOW           ,
MODE_SCN_BEACH          ,
MODE_SCN_FIREWORK       ,
MODE_SCN_COLOR_ACCENT   ,
MODE_SCN_COLOR_SWAP     ,
MODE_VIDEO_HIRES        ,
MODE_SCN_AQUARIUM       ,
MODE_COLOR_ACCENT       ,
MODE_SCN_NIGHT_SCENE    , // 30
MODE_SCN_ISO_3200       ,
MODE_SCN_SPORT          ,
MODE_SCN_KIDS_PETS      ,
MODE_INDOOR             ,
MODE_KIDS_PETS          ,
MODE_NIGHT_SNAPSHOT     ,
MODE_DIGITAL_MACRO      ,
MODE_SCN_FOLIAGE        ,
MODE_VIDEO_TIME_LAPSE   ,
MODE_SCN_INDOOR         , // 40
MODE_SCN_PORTRAIT       ,
MODE_SUPER_MACRO        , 
MODE_VIDEO_PORTRAIT     ,     
MODE_VIDEO_NIGHT        ,        
MODE_VIDEO_INDOOR       ,       
MODE_VIDEO_FOLIAGE      ,      
MODE_VIDEO_SNOW         ,         
MODE_VIDEO_BEACH        ,        
MODE_VIDEO_AQUARIUM     ,
MODE_VIDEO_SUPER_MACRO  , // 50
MODE_VIDEO_STITCH       ,  
MODE_VIDEO_MANUAL       ,  
MODE_SPORTS             ,
MODE_QUICK  		,
MODE_SCN_SUNSET         , 
MODE_SCN_CREATIVE_EFFECT, // "creative light effect", only known on ixus950_sd850
MODE_EASY,  
MODE_SCN_DIGITAL_MACRO  ,
MODE_SCN_STITCH         ,
MODE_SCN_LONG_SHUTTER   ,  // 60
MODE_LOWLIGHT           ,
MODE_SCN_NOSTALGIC      , // s90
MODE_SCN_SMART_SHUTTER  , // sx130is
MODE_SCN_LOWLIGHT       , // sx130is
MODE_SCN_SUPER_VIVID    , // sx130is
MODE_SCN_POSTER_EFFECT  , // sx130is
MODE_SCN_FISHEYE        , // sx130is
MODE_SCN_MINIATURE      , // sx130is
MODE_SCN_HDR		, // g12
MODE_VIDEO_MINIATURE    , // 70
MODE_VIDEO_IFRAME_MOVIE , // sx220
MODE_VIDEO_MOVIE_DIGEST    , // sx220 (the camera automatically record a short video clip (up to approximately 4 seconds) every time you shoot a still image.)
MODE_SCN_HIGHSPEED_BURST, // sx220
MODE_SCN_BEST_IMAGE        , // sx220
MODE_SCN_TOY_CAMERA    , // sx220
MODE_SCN_MONOCHROME    , // sx220
MODE_SCN_WINK_SELF_TIMER, // sx30/g12 (Smart Shutter, Wink Self Timer mode)
MODE_SCN_FACE_SELF_TIMER, // sx30/g12 (Smart Shutter, Face Self Timer mode)
MODE_VIDEO_SUPER_SLOW   , // IXUS 310 HS Super Slow Motion Movie
MODE_SUPER_VIVID        ,  // 80
MODE_POSTER_EFFECT      ,
MODE_BEACH              ,
MODE_FOLIAGE            ,
MODE_SNOW               ,
MODE_FIREWORK           ,
MODE_LIVE               , // A3300is
MODE_DISCREET           , // A3300is
MODE_BLUR_REDUCTION     , // a800
MODE_SCN_SOFTFOCUS      , // sx260 asm1989
MODE_SCN_SMOOTH_SKIN    , // 90
MODE_DIGITAL_IS         , // a810/a2300
MODE_VIDEO_STD_RECORDING, // SX50hs
MODE_BACKGROUND_DEFOCUS,
MODE_STAR_PORTRAIT ,
MODE_STAR_NIGHTSCAPE,
MODE_STAR_TRAILS,
MODE_HYBRID_AUTO,
MODE_VIDEO_M,
MODE_VIDEO_STAR_TIME_LAPSE,
MODE_HDR,
MODE_VIDEO_MANUAL_EXPOSURE,
MODE_SCN_HANDHELD_NIGHT,
MODE_SCN_FOOD,
MODE_CREATIV_ASSIST,
MODE_SCENE_INTELLIGENT_AUTO,
MODE_SCN_CLOSE_UP,
MODE_SCN_SPORTS,
};

enum _VideoMode
{
 VIDEO_NULL=0,
 VIDEO_BURST,
};

#define MODE_SCREEN_MASK        0x0C00
#define MODE_SCREEN_OPENED      0x0400
#define MODE_SCREEN_ROTATED     0x0800

#define MODE_IS_VIDEO(m)   (((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_STD || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SPEED ||  \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SUPER_SLOW ||  \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_COMPACT || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_MY_COLORS || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_COLOR_ACCENT || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_COLOR_SWAP || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_TIME_LAPSE || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_PORTRAIT || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_NIGHT || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_INDOOR || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_FOLIAGE || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SNOW  || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_BEACH || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_AQUARIUM || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SUPER_MACRO || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_STITCH || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_MANUAL|| \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_MINIATURE|| \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_HIRES || \
				    ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_IFRAME_MOVIE || \
                            ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_MOVIE_DIGEST)
                           
#define MODE_IS_SYNCH_VIDEO(m)(((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_STD || \
					 ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SPEED ||  \
					 ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_SUPER_SLOW ||  \
					 ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_STD_RECORDING ||  \
					 ((m)&MODE_SHOOTING_MASK)==MODE_VIDEO_IFRAME_MOVIE)

typedef struct {
    short int id; // hacks id
    short int prop_id; // Canons id
    char name[8];
    short int shutter_dfs_value; // shutter speed to use dark frame substraction
} ISOTable;

typedef struct {
    short int id; // hacks id
    short int prop_id; // Canons id
    char name[8];
    long usec;
} ShutterSpeed;

typedef struct {
    short int id; // hacks id
    short int prop_id; // Canons id
    char name[8];
} ApertureSize;

#ifdef CAM_SWIVEL_SCREEN
// 0 not open, non-zero open
int screen_opened(void);
// 0 not rotated, non-zero rotated
int screen_rotated(void);
#endif

extern unsigned char gPlayRecMode;
void ptp_raw_sending_active(int active);
void save_xml();
void csvPrintf(int fileDescriptor,char *format, ...);
void xmlPrintf(int fileDescriptor,char *name,char *format, ...);
int farForMaxDeviation();
int nearForInfinity();
int get_focal_length(int zp);
int get_step_for_real_fl(int fl);
int get_effective_focal_length(int zp);
void dumpFocalLengths();
void set_prev_zoom(int zp);
int get_zoom_x(int zp);
int get_fov(int zp);
int get_mav(int zp);
int get_xav(int zp);
int get_memory_info();
long get_tick_count();
void remount_filesystem();
long get_parameter_data(long id, void *buf, long bufsize);
long set_parameter_data(long id, void *buf, long bufsize);
long get_property_case(long id, void *buf, long bufsize);
long set_property_case(long id, void *buf, long bufsize);
long get_exposure_counter();
long get_file_counter();
long get_file_next_counter();
long get_target_dir_num();
long get_target_file_num();
long get_physw_copy(int x);
void other_kbd_process();
void checkKeyPress();
void kbd_key_press(long key);
void kbd_key_release(long key);
void kbd_key_release_all();
long kbd_is_key_pressed(long key);
long is_key_pressed_now(long key);
long kbd_is_key_clicked(long key);
long kbd_get_pressed_key();
long kbd_get_clicked_key();
long kbd_get_autoclicked_key();
void kbd_reset_autoclicked_key();
long kbd_use_zoom_as_mf();
void kbd_set_alt_mode_key_mask(long key);
void wait_until_remote_button_is_released(void);
long vid_is_bitmap_shown();
void *vid_get_bitmap_fb();
long vid_get_bitmap_width();
long vid_get_bitmap_height();
long vid_get_bitmap_screen_width(); 
long vid_get_bitmap_screen_height(); 
long vid_get_bitmap_buffer_width(); 
long vid_get_bitmap_buffer_height(); 
void *vid_get_viewport_fb();
void *vid_get_viewport_fb_d();
void *vid_get_viewport_live_fb();
extern void *vid_get_viewport_active_buffer();  // Return active live viewport memory address
extern int vid_get_viewport_xoffset();          // X offset of viewport edge relative to the viewport buffer (in CHDK screen pixels)
extern int vid_get_viewport_yoffset();          // Y offset of viewport top relative to the viewport buffer (in CHDK screen pixels)
extern int vid_get_viewport_display_xoffset();  // X offset of viewport edge relative to LCD screen (in CHDK screen pixels)
extern int vid_get_viewport_display_yoffset();  // Y offset of viewport top relative to LCD screen (in CHDK screen pixels)
extern int vid_get_viewport_byte_width();       // Physical width of viewport row in bytes
extern int vid_get_viewport_yscale();           // Y multiplier for cameras with 480 pixel high viewports (CHDK code assumes 240)
extern int vid_get_viewport_image_offset();     // Byte offset from start of viewport memory to first displayed pixel
extern int vid_get_viewport_row_offset();       // Difference between physical width of viewport and displayed width (in bytes)
void vid_bitmap_refresh();
long vid_get_viewport_height();
int vid_get_viewport_width();
void vid_turn_off_updates();
void vid_turn_on_updates();
void *hook_raw_fptr();
void *hook_raw_ret_addr();
void reverse_bytes_order(char* start, int count); 
char *hook_raw_image_addr();
long hook_raw_size();
void hook_raw_install();
void hook_raw_save_complete();
long lens_get_zoom_pos();
long lens_get_zoom_point();
void lens_set_zoom_point(long newpt);
void ZoomFocalLengths();
void sortBv();
void ptp_sz(int v);
int zpFromIndex(int index);
int TxTblIndex(int zp);
int averageBv(int low,int high);
int  is_zoom_busy();
int DoMFLock(void);
int UnlockMF(void);
int GetMechaPos();
long lens_get_focus_pos();
void lens_set_focus_pos(long newpos);
int set_focus_range();
long lens_get_target_distance();
int shooting_in_progress();
int get_inf_focus();
int shooting_is_flash_ready();
void disable_preflash();
extern int fpd;
int shooting_get_tv();
char* shooting_get_tv_str();
char* shooting_get_camera_tv_string();
char* get_tv_string_for_tv96(int tvv);
short shooting_get_bv96();
void shooting_set_mf();
int shooting_get_tv96_index(int v);
int shooting_get_tv96_from_index(int v);
short shooting_get_svm96();
void shooting_set_sv96(short sv96, short is_now);
short shooting_get_sv96();
void ubasic_ulf();
void ubasic_burst();
void ubasic_safe_set_zoom(int step);
void ubasic_shooting_set_focus(int v, short is_now);
void ubasic_set_focus_range();
void ubasic_smov();
void ubasic_ssmov();
void allocate_imgbuf();
void set_camera_tv_override(int v);
int shooting_get_luminance();
void shooting_set_tv(int v);
void shooting_set_tv_rel(int v);
const ShutterSpeed *shooting_get_tv_line();
const ShutterSpeed *shooting_get_camera_tv_line();
int shooting_get_av();
char* shooting_get_av_str();
void shooting_set_av(int v);
void shooting_set_av_rel(int v);
int shooting_get_real_av();
int shooting_get_day_seconds();
int shooting_get_tick_count();
void shooting_set_prop(int id, int v);
int shooting_get_prop(int id);
int shooting_get_prop_int(int id);
short shooting_get_canon_overexposure_value();
extern const int zoom_points;
int shooting_get_zoom();
void shooting_set_zoom(int v);
void shooting_set_zoom_rel(int v);
int shooting_get_focus();
void shooting_set_focus(int v);
int shooting_get_iso();
short get_min_iso();
short get_max_iso();
int __attribute__((weak)) switch_mode_usb(int mode);
void resetUsbDuration();
short shooting_get_canon_iso_mode();
short shooting_get_iso_from_sv96(short sv96);
short shooting_get_iso_real();
short shooting_get_iso_market();
const  ISOTable *shooting_get_iso_line();
void shooting_set_iso(int v);
void shooting_set_iso_real(short iso, short is_now);
void shooting_set_iso_direct(int v);
short shooting_iso_real_to_market(short isom);
short shooting_sv96_market_to_real(short sv96);
short shooting_sv96_real_to_market(short sv96);
void ManualFocusMode();
void shooting_video_bitrate_change(int v);
int is_video_recording();
int get_movie_status();
extern int auto_started;
void tv_bracketing(int stage);
short shooting_get_real_aperture();
short shooting_get_aperture_sizes_table_size();
short shooting_get_aperture_sizes_table_prop_id(short i);
short shooting_get_max_aperture_sizes_table_prop_id();
short shooting_get_aperture_from_av96(short av96);
void  shooting_set_av96_direct(short v, short is_now);
short shooting_get_av96_override_value();
short shooting_get_av96();
short shooting_get_tv96();
int shooting_get_user_av_id();
short shooting_get_user_av96();
int shooting_get_user_tv_id();
int my_shooting_get_tv_override_value();
short shooting_get_tv96_from_shutter_speed(float t);
float shooting_get_shutter_speed_from_tv96(short tv);
short shooting_get_user_tv96();
void shooting_set_av96_direct(short v,short is_now);
void shooting_set_av96(short v,short is_now);
#if defined(OPT_ALARIK)
void shooting_set_user_av_by_id(int v);
#endif
void shooting_set_user_av96(short v);
void shooting_set_nd_filter_state(short v, short is_now);
void shooting_set_user_tv_by_id(int v);
void shooting_set_user_tv_by_id_rel(int v);
void shooting_set_user_tv96(short v);
void shooting_set_tv96(short v, short is_now);
void shooting_set_tv96_direct(short v, short is_now);
short shooting_get_drive_mode();
short shooting_get_focus_mode();
int shooting_focus_out();
int shooting_focus_in();
void shooting_bracketing(void);
void shooting_subject_distance_bracketing(void);
void shooting_subject_distance_auto_bracketing();
int shooting_get_subject_distance_bracket_value();
int shooting_get_subject_distance();
long lens_get_focus_pos();
int shooting_get_hyperfocal_distance();
int shooting_get_near_limit_from_subj_dist(int s);
int shooting_get_nearpoint(int distance);
int shooting_get_farpoint(int distance);
short shooting_get_min_real_aperture();
extern int movie_status,simple_movie_status;
extern int zoom_status;
int shooting_set_mode_canon(int mode);
int shooting_mode_chdk2canon(int hackmode);
int shooting_mode_canon2chdk(int canonmode);
int shooting_set_mode_chdk(int mode);
void BootLock();
void WarnLowCardSpace(void);
void rw();
// returns 0 if in play, nonzero if rec
int rec_mode_active(void); 
void disp_add_line(char *buf);
void ExitTask(void);
void allbest_shooting_set_focus(int v);
short shooting_can_focus();
void TurnOnBackLight(void);
void TurnOffBackLight(void);
void TurnOnDisplay(void); 
void TurnOffDisplay(void); 
void DoAFLock(void); 
void UnlockAF(void); 
void DoAELock(void); 
void UnlockAE(void);
int shooting_get_flash_status();
int shooting_will_use_flash();
void wr_kbd_read_keys_r2(void *status);
int lens_to_focal_plane_width; 
long lens_get_focus_pos_from_lens();
int shooting_get_lens_to_focal_plane_width(); 
int get_bracketing_counter();
int mode_get();
long stat_get_vbatt();
long get_vbatt_min();
long get_vbatt_max();
void ubasic_camera_set_raw(int mode);
void ubasic_camera_set_nr(int mode);
int ubasic_camera_get_nr();
int ubasic_camera_script_autostart();
void script_start();
void ubasic_camera_set_script_autostart();
void ubasic_camera_set_tv_override(int mode);
int ubasic_camera_get_tv_override();
int ubasic_camera_is_pressed(const char *s);
void ubasic_sv();
void ubasic_stpv();
void ubasic_fastShoot(int maxiso);
void ubasic_cover_lens();
void ubasic_set_focus_now(int d);
void ubasic_af();
void ubasic_cont_shoot(int action);
void ubasic_vzoom_in_out();
int vzio();
void console_redraw();
void printBv();
void fill_bv_buffer();
void fps(int x);
void get_standard_period();
short shooting_get_svm96_from_iso(short iso);
void bgndBeeper();
int lastObjectNumber();
int next_focus(int currentFocus);
int prev_focus(int currentFocus);
inline int conf_save_new_settings_if_changed();
void user_scripts_menu();
void TenMsecCount();
void exit_alt();
void power_down(void);
int get_sd_status();
void disable_shutdown();
void enable_shutdown();
void JogDial_CW(void);
void JogDial_CCW(void);
void change_video_tables(int a, int b);
void set_camera_tv(short int v);
int Get_JogDial(void);
long get_jogdial_direction(void);
char * get_pbb();
int review_fullscreen_mode(void);
int get_usb_power(int edge);
int getUsbDuration();
int get_usb_bit();
long get_sensor_width();
#if CAM_USES_EXMEM
void exmem_malloc_init(void);
#endif

void swap_partitions(void);
int get_part_count(void);
// void create_partitions(void);
extern char * camera_jpeg_count_str();
void get_target_dir_name(char *out);
unsigned int GetJpgCount(void);
unsigned int GetRawCount(void);
unsigned int GetFreeCardSpaceKb(void);
unsigned int GetTotalCardSpaceKb(void);
extern int recreview_hold;
extern int canon_menu_active;  
extern char canon_shoot_menu_active;  
extern EXPO_BRACKETING_VALUES bracketing;
void __attribute__((noreturn)) shutdown();
void debug_led(int state);
#define started() debug_led(1)
#define finished() debug_led(0)
void play_sound(short sound);
void draw_spm_logo(int x, int y);
void RefreshUSBMode(void);
void PostLogicalEventForNotPowerType(int mode,int unk);
void  PostLogicalEventToUI(unsigned event, unsigned unk);
void DisableNotificationPTP(void);
unsigned levent_index_for_name(const char *name);
unsigned levent_id_for_name(const char *name);
typedef struct 
{
    char *name; /* name of event, may point to empty string */
    unsigned id; /* event id number used with canon event fuctions (post etc) */
    unsigned param; /* unknown 0-3 */
} levent_def;
extern levent_def levent_table[];
void movie_mode();
void record_mode();
void recordMode();
void playMode();
void playback_mode();
int hdrfs();
void ubasic_rm();
void ubasic_pm();
void ubasic_mm();
void ubasic_musbc();
void ubasic_busbc();
void ubasic_set_led(int led, int state, int bright);
void CloseShutter();
void OpenShutter();
int RomLog();
void save_romlog();
void manualFocusMode();
void set_key_press(int nSet);
void platform_kbd_hook();
int start_usb_HPtimer(int interval);
int stop_usb_HPtimer();
void cb_slitcam_menu_change();
void welcome_menu();
void ubasic_fastKAP();
short shooting_get_sv96_from_iso(short iso);
short shooting_iso_market_to_real(short isom);
void iso_init();
void getModeIDs();
int start_self_timer();
int stop_self_timer();
void getLanguageCode();
void WriteFirmInfoToFile();
unsigned char enable_focus_override();
void disable_focus_override();
void get_tv(char *buf,int val96);
void get_av(char *buf,int val);
void get_ev_correction(char *buf,int val);
void get_date(char *buf,unsigned char format);
void get_flash_ev_comp(char *buf,int val);
void get_time(char *buf);
char *get_systemString_address();
void log_kap();
void usb_high_count();
unsigned char inGridSelection();
void stereo_pair();
int get_vkey(int key);
#if defined(CAM_HAS_GPS)
 extern void GPS_UpdateData();
#endif

#if defined(OPT_PTP)
typedef struct {
    int code;
    int sess_id;
    int trans_id;
    int num_param;
    int param1;
    int param2;
    int param3;
    int param4;
    int param5;
} PTPContainer;

typedef struct {
    int handle;
    int (*send_data)(int handle, const char *buf, int part_size, int total_size, int, int, int); // (0xFF9F525C), total_size should be 0 except for the first call
    int (*recv_data)(int handle, char *buf, int size, int, int); // (0xFF9F5500)
    int (*send_resp)(int handle, PTPContainer *resp); // (0xFF9F5688)
    int (*get_data_size)(int handle); // (0xFF9F5830)
    int (*send_err_resp)(int handle, PTPContainer *resp); // (0xFF9F5784)
    int unknown1; // ???
    int (*f2)(); // ??? (0xFF8D5B24)
    int (*f3)(); // ??? (0xFF8D5B5C)
    // more??
} ptp_data;

typedef int (*ptp_handler)(int, ptp_data*, int, int, int, int, int, int, int, int);

int add_ptp_handler(int opcode, ptp_handler handler, int unknown);

void init_chdk_ptp();
void init_chdk_ptp_task();

#endif // OPT_PTP
void ExitTask();
void shutdown_soft();

int reboot(const char *fw_update); // fw_update == NULL implies normal reboot

void switch_mode(int mode); // 0 = playback, 1 = record; only call while USB connected

/*for ptp image transfer*/
long vid_get_image_buffer_width_for_ptp(); //               ^   -------------------------  <-
long vid_get_image_width_for_ptp();        //               |   |                 |     |  (buffer
long vid_get_image_height_for_ptp();       //             image |                 |     |   height
long vid_get_bitmap_buffer_width_for_ptp();//             height|                 |     |   not used
long vid_get_bitmap_width_for_ptp();       //               |   |<--image width-->|     |   for ptp
long vid_get_bitmap_height_for_ptp();      //               v   |-----------------      |   image
void *vid_get_bitmap_fb_for_ptp();         //                   |<----buffer  width---->|   transfer)
unsigned int *vid_get_palette();           //                   -------------------------  <-


// Data returned from GetMemInfo & GetExMemInfo functions stored in this data structure
typedef struct 
{
    int start_address;
    int end_address;
    int total_size;
    int allocated_size;
    int allocated_peak;
    int allocated_count;
    int free_size;
    int free_block_max_size;
    int free_block_count;
} cam_meminfo;

typedef struct 
{
    char    latitudeRef[4];
    int     latitude[6];
    char    longitudeRef[4];
    int     longitude[6];
    char    heightRef[4];
    int     height[2];
    int     timeStamp[6];
    char    status[2];
    char    mapDatum[7];
    char    dateStamp[11];
    char    unknown2[160];
} tGPS;

#if defined(CAM_FIRMWARE_MEMINFO)
extern void GetMemInfo(cam_meminfo*);
#endif

#ifdef OPT_EXMEM_MALLOC
void exmem_malloc_init(void);
void GetExMemInfo(cam_meminfo*);
#endif

#endif


 





