#ifndef CAMERA_H
#define CAMERA_H

//----------------------------------------------------------
//Default values
//----------------------------------------------------------
    #undef  CAM_SZ				  //
    #undef  CAM_RAW_ROWPIX                  //Number of pixels in RAW row
    #undef  CAM_RAW_ROWS                    //Number of rows in RAW
    #undef  CAM_CURTAIN                     //Camera has flash second curtain option
    #undef  CAM_DRYOS                       //Camera is DryOS-based
  //  #define CAM_DIGIC                 3   //Cameras processor
    #undef  CAM_PROPSET                     //Camera's properties group (the generation)
    #undef  CAM_PB_MODE                     //Can you switch to playback mode with script using switch override method
    #undef  CAM_USE_PBB                     //Use first playback buffer
    #undef  CAM_USES_EXMEM                  //Uses extended memory
    #undef  CAM_NEED_HP                     //half-press
    #undef  CAM_MOVIE_PLE                   //movie mode by post logical event
    #undef  CAM_HAS_VIDEO_CONTROL_BUTTON    //Has video pushbutton for start/stop recording
    #undef  CAM_HAS_PLAYBACK_PB             //Has playback pushbutton
    #define CAM_OUTLINE_MODE            1   //Supports outline mode
    #undef  CAM_SWIVEL_SCREEN               //Camera has rotated LCD screen
    #define CAM_USE_ZOOM_FOR_MF         1   //Zoom lever can be used for manual focus adjustments
    #undef  CAM_NEED_SET_ZOOM_DELAY         // Define to add a delay after setting the zoom position before resetting the focus position in shooting_set_zoom 
    #undef  CAM_USE_ALT_SET_ZOOM_POINT      // Define to use the alternate code in lens_set_zoom_point()
    #undef  CAM_USE_ALT_PT_MoveOpticalZoomAt// Define to use the PT_MoveOpticalZoomAt() function in lens_set_zoom_point()
    #undef  CAM_USE_OPTICAL_MAX_ZOOM_STATUS // Use ZOOM_OPTICAL_MAX to reset zoom_status when switching from digital to optical zoom in gui_std_kbd_process()
    #define CAM_HAS_DISP_BUTTON             1
    #define CAM_REMOTE                  1   //Camera supports USB-remote
    #define CAM_SYNCH                   1   //camera supports ordinary synch
    #undef  CAM_PRECISION_SYNCH		  //Camera supports SDM precision synch 
    #undef  CAM_SUPPORTS_SIO                //camera supports serial I/O
    #undef  CAM_MULTIPART                   //Camera supports SD-card multipartitioning
    #define CAM_HAS_ZOOM_LEVER          1   //Camera has dedicated zoom buttons
    #undef  CAM_DRAW_EXPOSITION             //Output expo-pair on screen (for cameras which (sometimes) don't do that)
    #define CAM_HAS_ERASE_BUTTON        1   //Camera has dedicated erase button
    #define CAM_HAS_IRIS_DIAPHRAGM      1   //Camera has real diaphragm mechanism
    #undef  CAM_HAS_ND_FILTER               //Camera has build-in ND filter
    #undef  CAM_HAS_NATIVE_RAW 
    #undef  CAM_TURNS_OFF_LCD
    #undef  STD_360_BUF
    #undef  NEW_REC_PLAY
    #undef  NEW_MOVIE
    #undef  CAM_STARTUP_CRASH_FILE_OPEN_FIX
    #define CAM_HAS_MANUAL_FOCUS            1   // Camera has native manual focus mode (disables MF shortcut feature)
    #undef  CAM_SD_OVER_IN_AF                   // Camera allows SD override if MF & AFL not set
    #undef  CAM_SD_OVER_IN_AFL                  // Camera allows SD override when AFL is set
    #undef  CAM_SD_OVER_IN_MF                   // Camera allows SD override when MF is set

    #define CAM_CAN_SD_OVER_NOT_IN_MF   1   //Camera allows subject distance (focus) override when not in manual focus mode
    #define CAM_CAN_SD_OVERRIDE         1   //Camera allows to do subject distance override
    #define CAM_HAS_MANUAL_FOCUS        1   //Camera has manual focus mode
    #define CAM_HAS_USER_TV_MODES       1   //Camera has tv-priority or manual modes with ability to set tv value
    #undef  CAM_SHOW_OSD_IN_SHOOT_MENU      //On some cameras Canon shoot menu has additional functionality and useful in this case to see CHDK OSD in this mode  
    #define CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO  1 //Camera can unlock optical zoom in video (if it is locked)
    #undef  CAM_FEATURE_FEATHER             //Cameras with "feather" or touch wheel.
    #undef  CAM_CONSOLE_LOG_ENABLED         //Development: internal camera stdout -> A/stdout.txt
    #define CAM_CHDK_HAS_EXT_VIDEO_MENU 1   //In CHDK adjustable video compression has been implemented
    #undef  CAM_CAN_MUTE_MICROPHONE         //Camera has function to mute microphone
    #define CAM_BITMAP_PALETTE          1   //which color set is used for this camera
    #define CAM_X_STRETCH               1   // 1 = no stretch, 2 = 2x, 3 = 9/4 times
    #define CAM_EMUL_KEYPRESS_DELAY     40  // Delay to interpret <alt>-button press as longpress
    #define CAM_EMUL_KEYPRESS_DURATION  5   // Length of keypress emulation
    #define ANA_BUTTON                  KEY_SHOOT_HALF  // for anaglyph playback   
    #define CAM_HAS_VIDEO_CONTROL_BUTTON  1          
    #define DNG_SUPPORT                 1 
    #define EDGE_OVERLAY_COLOUR         0x66 // pink for palette 1 cameras               
    #define CAM_MAKE                    "Canon"
    #define CAM_SENSOR_BITS_PER_PIXEL   10  // Bits per pixel. 10 is standard, 12 is supported except for curves
    #define CAM_WHITE_LEVEL             ((1<<CAM_SENSOR_BITS_PER_PIXEL)-1)
    #define CAM_BLACK_LEVEL             31
    #define CAM_UNCACHED_BIT            0x10000000 // bit indicating the uncached memory
    #undef  CAN_CONTROL_AV_OUTPUT            // using uBasic enable_video_out
    #define MOVIE_TYPE                  "AVI"
    #define LOWRESIMG                      4 
    #define CAM_HAS_IS                     1
    #define EDGE_HMARGIN                    0   // define sup and inf screen margins on edge overlay without overlay.  Necessary to save memory buffer space. sx200is needs values other than 0
    #define CAM_TS_BUTTON_BORDER            0   // Define this to leave a border on each side of the OSD display for touch screen buttons.
                                                // Used on the IXUS 310 to stop the OSD from overlapping the on screen buttons on each side
    #define CAM_MENU_BORDERWIDTH            30  // Defines the width of the border on each side of the CHDK menu. The CHDK menu will have this
                                                // many pixels left blank to the on each side. Should not be less than 10 to allow room for the
                                                // scroll bar on the right.
    #undef CAM_USES_ASPECT_CORRECTION      
    #define CAM_SCREEN_WIDTH           360 // Width of bitmap screen in CHDK co-ordinates (360 or 480)
    #define CAM_SCREEN_HEIGHT          240 // Height of bitmap screen in CHDK co-ordinates (always 240 on all cameras so far)
    #define CAM_BITMAP_WIDTH           360 // Actual width of bitmap screen in bytes (may be larger than displayed area)
    #define CAM_BITMAP_HEIGHT          240 // Actual height of bitmap screen in rows (240 or 270)
    #undef CAM_DATE_FOLDER_NAMING          
    #undef CAM_DRIVE_MODE_FROM_TIMER_MODE    // use PROPCASE_TIMER_MODE to check for multiple shot custom timer.
    #define CAMERA_MIN_DIST         0           // Define min distance that can be set in _MoveFocusLensToDistance (allow override - e.g. G12 min dist = 1)
    #define CAMERA_MAX_DIST         65535       // Define max distance that can be set in _MoveFocusLensToDistance (allow override for superzooms - SX30/SX40)
    #undef  LOW_LIGHT				// camera has Lowlight mode
    #undef  CAM_KEY_CLICK_DELAY                 // additional delay between press and release for scripted click
    #undef  STD_PERIOD				// for precision synch and movie synch
    #define PERIOD_REF			101
    #define CAM_USB_EVENTID         0x902       // Levent ID for USB control. Changed to 0x202 in DryOS R49 so needs to be overridable.
    #define SYNCH_FPS                  24
    #define SYNCH_SPEED			    4
    #define PERIOD_COUNT		((volatile int*)0xC0F07008) // LiveView refresh
    #undef CAM_FILE_COUNTER_IS_VAR              // file counter is variable file_counter_var in stubs, not a param
    #undef  CAM_HOTSHOE_OVERRIDE                // hot shoe flash-detection can be overridden
    #define CAM_REMOTE_USB_HIGHSPEED        1   // Enable highspeed measurements of pulse width & counts on USB port 
    #define CAM_REMOTE_HIGHSPEED_LIMIT 1000     // Set lowest timer value settable by user
    #define CAM_MARKET_ISO_BASE             100 // Base 'market' ISO value
    #undef CAM_HAS_CMOS
    #undef CAM_HAS_MOVIE_DIGEST_MODE
    #undef UI_CS_PROP
    #undef UI_CT_PROP
    #undef UI_AEB_PROP
    #undef COUNTS_BEFORE_INT
    #define ND_FACTOR			288
    #undef  CAM_HAS_JOGDIAL                     // Camera has a "jog dial"
    #undef  CAM_HAS_GPS  
    #undef CAM_SIMPLE_MOVIE_STATUS      
// -----------------------------------------------------------------------------------------------------------
#if defined (CAMERA_g7x)
    #define CAM_PROPSET                         7
    #define CDM                         99
    #define UI_CS_PROP			    0x8010 
    #define UI_CT_PROP			    0x8011 	
    #define STD_PERIOD 			    _EngDrvRead(0xC0F06014)
    #undef PERIOD_COUNT
    #define PERIOD_COUNT		((volatile int*)0xC0F0703C) // LiveView refresh
    #undef SYNCH_FPS
    #define SYNCH_FPS                           30
    #define CAM_AV_OVERRIDE_IRIS_FIX            1   // for cameras that require _MoveIrisWithAv function to override Av (for bracketing).
    #define CAM_HAS_NATIVE_ND_FILTER            1   // Camera has built-in ND filter with Canon menu support for enable/disable
    #define CAM_HAS_ND_FILTER                   1           // Camera has built-in ND filter (in addition to iris)
    #define CAM_NEED_HP 				1
    #undef  CAM_KEY_PRESS_DELAY
    #define CAM_KEY_PRESS_DELAY                 60          // delay after a press

    #define CAM_ADJUSTABLE_ALT_BUTTON           1
    #define CAM_STARTUP_CRASH_FILE_OPEN_FIX     1
    #define CAM_ALT_BUTTON_NAMES                { "Playback", "Video", "Wifi" }
    #define CAM_ALT_BUTTON_OPTIONS              { KEY_PLAYBACK, KEY_VIDEO, KEY_WIFI}
    #define CAM_DRIVE_MODE_FROM_TIMER_MODE      1

    // zebra and histo don't currently work, and default zebra shortcut conflicts with canon AF lock
    #define SHORTCUT_TOGGLE_HISTO               KEY_DUMMY
    #define SHORTCUT_TOGGLE_ZEBRA               KEY_DUMMY

    #define CAM_DRYOS                           1
    #define CAM_DRYOS_2_3_R39                   1
    #define CAM_DRYOS_2_3_R47                   1

    #define CAM_RAW_ROWPIX                      5632
    #define CAM_RAW_ROWS                        3710

    #define CAM_HAS_CMOS                        1

    // unlocked by default, not needed
    #undef  CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO
    #define CAM_HAS_VIDEO_BUTTON                1

    #undef  CAM_HAS_DISP_BUTTON
    #undef  CAM_USE_ZOOM_FOR_MF

// not implemented
    #undef  CAM_CHDK_HAS_EXT_VIDEO_MENU
    #undef  CAM_VIDEO_CONTROL

    #define CAM_HAS_JOGDIAL                     1
// TODO not implemented
    #undef  CAM_USE_ZOOM_FOR_MF

// OK
    #undef  CAM_UNCACHED_BIT
    #define CAM_UNCACHED_BIT                    0x40000000

    #undef  CAM_CIRCLE_OF_CONFUSION
    #define CAM_CIRCLE_OF_CONFUSION             11  // CoC value for camera/sensor (see http://www.dofmaster.com/digital_coc.html)

// OK
    #define CAM_DNG_LENS_INFO                   { 88,10,368,10,18,10,28,10 }
// OK
    #define cam_CFAPattern                      0x02010100

    #define cam_CalibrationIlluminant1          17

    // from CR2 converted to DNG with adobe DNG converter
    #define CAM_COLORMATRIX1 \
    11125, 10000, -5937, 10000,  355, 10000, \
    -2979, 10000, 10926, 10000, 2360, 10000, \
     -116, 10000,   835, 10000, 6064, 10000

    #define cam_CalibrationIlluminant2          21      // D65

    #define CAM_COLORMATRIX2 \
     9602, 10000, -3823, 10000, -937, 10000, \
    -2984, 10000, 11495, 10000, 1675, 10000, \
     -407, 10000,  1415, 10000, 5049, 10000,

    #define CAM_FORWARDMATRIX1 \
     4264, 10000, 4193, 10000, 1187, 10000, \
     1832, 10000, 7811, 10000,  357, 10000, \
      854, 10000,   16, 10000, 7381, 10000

    #define CAM_FORWARDMATRIX2 \
	 4036, 10000, 4056, 10000, 1551, 10000, \
     1780, 10000, 7757, 10000,  463, 10000, \
      764, 10000,    1, 10000, 7487, 10000

    #define CAM_JPEG_WIDTH                      5472
    #define CAM_JPEG_HEIGHT                     3648
    #undef  CAM_BITMAP_PALETTE
    #define CAM_BITMAP_PALETTE                  30

// TODO - from cr2
    #define CAM_ACTIVE_AREA_X1                  132
    #define CAM_ACTIVE_AREA_Y1                  40
    #define CAM_ACTIVE_AREA_X2                  5604
    #define CAM_ACTIVE_AREA_Y2                  3688

    #define PARAM_CAMERA_NAME                   3
// TODO 
    #define CAM_DATE_FOLDER_NAMING              0x400
    #define CAM_FILE_COUNTER_IS_VAR             1     // file counter is variable file_counter_var in stubs, not a param
    #undef  CAM_SENSOR_BITS_PER_PIXEL
    #define CAM_SENSOR_BITS_PER_PIXEL           12

    // black level appears to be ~512 in normal shooting (ISO125 P or M)
    #undef CAM_BLACK_LEVEL
    #define CAM_BLACK_LEVEL                     511

// TODO expdrv not implemented, but camera does multi-minute exposures natively
// this allows > 60s in overrid (untested)
    #define CAM_EXT_TV_RANGE                    1

    // TODO actual range untested
    // #undef CAM_EXT_AV_RANGE
    // #define CAM_EXT_AV_RANGE                    ?   // Number of 1/3 stop increments to extend the Av range beyond the Canon default smallest aperture

    #define CAM_SHOW_OSD_IN_SHOOT_MENU          1

    #define DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY   1           // Draw pixels on active bitmap buffer only.
    // TODO
    #define DRAW_ON_YUV_OVERLAY                 1
    #undef  CAM_BITMAP_WIDTH
    #undef  CAM_BITMAP_HEIGHT
    #undef  CAM_SCREEN_WIDTH
    #undef  CAM_SCREEN_HEIGHT
    #define CAM_BITMAP_WIDTH                    736 // Actual width of bitmap screen in <s>bytes</s> pixels
    #define CAM_BITMAP_HEIGHT                   480 // Actual height of bitmap screen in rows
    #define CAM_SCREEN_WIDTH                    720 // Width of bitmap screen in CHDK co-ordinates (360 or 480)
    #define CAM_SCREEN_HEIGHT                   480 // Height of bitmap screen in CHDK co-ordinates (always 240 on all cameras so far)
// TODO
//    #define CAM_SUPPORT_BITMAP_RES_CHANGE       1
    
    #undef  CAM_VIEWPORT_PIXEL_FORMAT
    #define CAM_VIEWPORT_PIXEL_FORMAT           2   // mandatory on DIGIC6, pixel format is YUY2 (uyvy)

    #undef  EDGE_HMARGIN
    #define EDGE_HMARGIN                        10

    //#define CAM_HAS_FILEWRITETASK_HOOK          1
    //#define CAM_FILEWRITETASK_SEEKS             1
     #define CAM_ZEBRA_NOBUF                     1

    #undef  CAMERA_MIN_DIST
    #define CAMERA_MIN_DIST                     50         // Override min subject distance, min get_focus() value using MF
    #undef  CAMERA_MAX_DIST
    #define CAMERA_MAX_DIST                     177552     // Override max subject distance, max get_focus() value at max zoom using MF before it goes to -1


    //#define CAM_CHDK_HAS_EXT_VIDEO_TIME         1
    #define CAM_SIMPLE_MOVIE_STATUS             1
    #undef  CAM_USB_EVENTID
    #define CAM_USB_EVENTID                     0x202       // Levent ID for USB control. Changed in DryOS R49, R50 so needs to be overridable.

//    #define CAM_HAS_MOVIE_DIGEST_MODE           1 

// TODO - haven't checked default values, these seem to work
    #define CAM_USE_ALT_SET_ZOOM_POINT          1           // Define to use the alternate code in lens_set_zoom_point()
    #define CAM_USE_ALT_PT_MoveOpticalZoomAt    1           // Define to use the PT_MoveOpticalZoomAt() function in lens_set_zoom_point()
    #define CAM_NEED_SET_ZOOM_DELAY             300

    #define MKDIR_RETURN_ONE_ON_SUCCESS         1    // mkdir() return 1 on success, 0 on fail.

    #undef  REMOTE_SYNC_STATUS_LED
    // #define REMOTE_SYNC_STATUS_LED              0xC022C30C  // TODO specifies an LED that turns on while camera waits for USB remote to sync
 
// crashes with ASSERT!! FocusLensController.c Line 1106 on shoot
//    #define CAM_SD_OVER_IN_AF                   1
    #define CAM_SD_OVER_IN_AFL                  1
//    #define CAM_SD_OVER_IN_MF                   1
 
    #undef  CAM_MARKET_ISO_BASE
    #define CAM_MARKET_ISO_BASE                 200 // Override base 'market' ISO value
 
    // "real" to "market" conversion definitions
    #define SV96_MARKET_OFFSET                  0   // market and real appear to be identical on this cam
    #define ISO_MARKET_TO_REAL_MULT         1
    #define ISO_MARKET_TO_REAL_SHIFT        0
    #define ISO_MARKET_TO_REAL_ROUND        0
    #define ISO_REAL_TO_MARKET_MULT         1
    #define ISO_REAL_TO_MARKET_SHIFT        0
    #define ISO_REAL_TO_MARKET_ROUND        0

    #define COLOR_GREY            0x7F
    #define COLOR_GREY_P          0x7F
    #define COLOR_LIGHT_GREY      0x7F
    #define COLOR_DARK_GREY       0xBF
    #define COLOR_LIGHT_GREY_P    0x7F
    #define COLOR_DARK_GREY_P     0xBF


// -----------------------------------------------------------------------------------------------------------
#elif defined(CAMERA_m3)
    #define CDM                         		10  // Canon drive mode	
#define   CAM_PRECISION_SYNCH   1
    #define STD_PERIOD 			    _EngDrvRead(0xC0F06014)
    #undef PERIOD_COUNT
    #define PERIOD_COUNT		((volatile int*)0xC0F0703C) // LiveView refresh
    #undef SYNCH_FPS
    #define SYNCH_FPS                           30
    #define CAM_DISABLE_RAW_IN_LOW_LIGHT_MODE   1 // there's no low light mode, so just in case
    #define CAM_DISABLE_RAW_IN_HANDHELD_NIGHT_SCN   1 // raw would make absolutely no sense in this mode
    #define CAM_DISABLE_RAW_IN_HQ_BURST         1 // raw wouldn't make any sense in this mode either
    #define CAM_DISABLE_RAW_IN_HYBRID_AUTO      1 // For cameras that lock up while saving raw in "Hybrid Auto" mode
    #define CAM_DISABLE_RAW_IN_AUTO             1 // disabled due to wrong raw buffer selection at quick shutter press
    #define CAM_DISABLE_RAW_IN_SPORTS           1 // sports mode seems to use a no-yet-found code path and corrupts JPEG/DNG

    #define DNG_VERT_RLE_BADPIXELS              1
    #define CAM_AV_OVERRIDE_IRIS_FIX            1
    #define CAM_HAS_ND_FILTER                   1           // Camera has built-in ND filter (in addition to iris)
    #undef  CAM_KEY_PRESS_DELAY
    #define CAM_KEY_PRESS_DELAY                 60          // delay after a press
    #define CAM_ADJUSTABLE_ALT_BUTTON           1
    #define CAM_ALT_BUTTON_NAMES                { "Print", "Video", "Display" }
    #define CAM_ALT_BUTTON_OPTIONS              { KEY_PRINT, KEY_VIDEO, KEY_DISPLAY }
    #define CAM_DRIVE_MODE_FROM_TIMER_MODE      1

    #define CAM_DRYOS                           1
    #define CAM_DRYOS_2_3_R39                   1
    #define CAM_DRYOS_2_3_R47                   1
    #define CAM_PROPSET                         7

    #define CAM_RAW_ROWPIX                      6000
    #define CAM_RAW_ROWS                        4000

    #define CAM_HAS_CMOS                        1

    #undef  CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO
    #define CAM_HAS_VIDEO_BUTTON                1
    #define CAM_VIDEO_QUALITY_ONLY              1

    #undef  CAM_VIDEO_CONTROL

    #define CAM_HAS_JOGDIAL                     0 // 1 by me
    #undef  CAM_USE_ZOOM_FOR_MF

    #undef  CAM_UNCACHED_BIT
    #define CAM_UNCACHED_BIT                    0x40000000

    #define CAM_DNG_LENS_INFO                   { 45,10,900,10,35,10,68,10 }
    #define cam_CFAPattern                      0x02010100

    #define CAM_COLORMATRIX1                            \
    544808, 1000000, -174047, 1000000, -80399, 1000000, \
    -75055, 1000000,  440444, 1000000,  11367, 1000000, \
     -5801, 1000000,   71589, 1000000, 118914, 1000000

    #define cam_CalibrationIlluminant1          17

    #define CAM_JPEG_WIDTH                      6000
    #define CAM_JPEG_HEIGHT                     4000

    #define CAM_ACTIVE_AREA_X1                  96
    #define CAM_ACTIVE_AREA_Y1                  18
    #define CAM_ACTIVE_AREA_X2                  6168
    #define CAM_ACTIVE_AREA_Y2                  4062

    #define PARAM_CAMERA_NAME                   3
    #define CAM_DATE_FOLDER_NAMING              0x400
    #define CAM_FILE_COUNTER_IS_VAR             1     // file counter is variable file_counter_var in stubs, not a param
    #undef  CAM_SENSOR_BITS_PER_PIXEL
    #define CAM_SENSOR_BITS_PER_PIXEL           12

    #define CAM_EXT_TV_RANGE                    1

    #undef  CAM_HAS_ERASE_BUTTON
    #define CAM_SHOW_OSD_IN_SHOOT_MENU          1

    #define DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY   1           // Draw pixels on active bitmap buffer only.
    #define DRAW_ON_YUV_OVERLAY                 1
    #undef  CAM_BITMAP_WIDTH
    #undef  CAM_BITMAP_HEIGHT
    #undef  CAM_SCREEN_WIDTH
    #undef  CAM_SCREEN_HEIGHT
    #define CAM_BITMAP_WIDTH                    736 // Actual width of bitmap screen in <s>bytes</s> pixels
    #define CAM_BITMAP_HEIGHT                   480 // Actual height of bitmap screen in rows
    #define CAM_SCREEN_WIDTH                    720 // Width of bitmap screen in CHDK co-ordinates (360 or 480)
    #define CAM_SCREEN_HEIGHT                   480 // Height of bitmap screen in CHDK co-ordinates (always 240 on all cameras so far)
    #define CAM_SUPPORT_BITMAP_RES_CHANGE       1
    
    #undef  CAM_VIEWPORT_PIXEL_FORMAT
    #define CAM_VIEWPORT_PIXEL_FORMAT           2   // mandatory on DIGIC6, pixel format is YUY2 (uyvy)

    #undef  EDGE_HMARGIN
    #define EDGE_HMARGIN                        16

    #define CAM_STARTUP_CRASH_FILE_OPEN_FIX     1           // enable workaround for camera crash at startup when opening the conf / font files
                                                            // see http://chdk.setepontos.com/index.php?topic=6179.0

    //#define CAM_HAS_FILEWRITETASK_HOOK          1
    //#define CAM_FILEWRITETASK_SEEKS             1

    #undef  CAM_BITMAP_PALETTE
    #define CAM_BITMAP_PALETTE                  29

    #define CAM_ZEBRA_NOBUF                     1

    //#define CAM_QUALITY_OVERRIDE                1

    #undef  CAMERA_MIN_DIST
    #define CAMERA_MIN_DIST                     100         // Override min subject distance
    #undef  CAMERA_MAX_DIST
    #define CAMERA_MAX_DIST                     1550000     // Override max subject distance; manually checked up to 1550388, with MF max 1369863 (double step)

    //#define CAM_HAS_GPS                         1

    #define CAM_USE_SUNRISE                     1

    //#define CAM_CHDK_HAS_EXT_VIDEO_TIME         1

    #undef  CAM_USB_EVENTID
    #define CAM_USB_EVENTID                     0x202       // Levent ID for USB control. Changed in DryOS R49, R50 so needs to be overridable.

    #define CAM_HAS_MOVIE_DIGEST_MODE           1 

    #define CAM_USE_ALT_SET_ZOOM_POINT          1           // Define to use the alternate code in lens_set_zoom_point()
    #define CAM_USE_ALT_PT_MoveOpticalZoomAt    1           // Define to use the PT_MoveOpticalZoomAt() function in lens_set_zoom_point()
    #define CAM_NEED_SET_ZOOM_DELAY             300

    #define MKDIR_RETURN_ONE_ON_SUCCESS         1    // mkdir() return 1 on success, 0 on fail.

    #undef  REMOTE_SYNC_STATUS_LED
    // #define REMOTE_SYNC_STATUS_LED              0xC022C30C  // TODO specifies an LED that turns on while camera waits for USB remote to sync
 
    #define CAM_SD_OVER_IN_AF  1
    #define CAM_SD_OVER_IN_MF  1
    #define COLOR_GREY            0x7F
    #define COLOR_GREY_P          0x7F
    #define COLOR_LIGHT_GREY      0x7F
    #define COLOR_DARK_GREY       0xBF
    #define COLOR_LIGHT_GREY_P    0x7F
    #define COLOR_DARK_GREY_P     0xBF
// -----------------------------------------------------------------------------------------------------------
#elif defined (CAMERA_sx280hs)
 
    #define CAM_DISABLE_RAW_IN_LOW_LIGHT_MODE   1 // there's no low light mode, so just in case
    #define CAM_DISABLE_RAW_IN_HANDHELD_NIGHT_SCN   1 // raw would make absolutely no sense in this mode
    #define CAM_DISABLE_RAW_IN_HQ_BURST         1 // raw wouldn't make any sense in this mode either
    #define CAM_DISABLE_RAW_IN_HYBRID_AUTO      1 // For cameras that lock up while saving raw in "Hybrid Auto" mode
    #define CAM_DISABLE_RAW_IN_AUTO             1 // disabled due to wrong raw buffer selection at quick shutter press
    #define CAM_DISABLE_RAW_IN_SPORTS           1 // sports mode seems to use a no-yet-found code path and corrupts JPEG/DNG

    #define CAM_AV_OVERRIDE_IRIS_FIX            1
    #define CAM_HAS_ND_FILTER                   1           // Camera has built-in ND filter (in addition to iris)
    #undef  CAM_KEY_PRESS_DELAY
    #define CAM_KEY_PRESS_DELAY                 60          // delay after a press
    #define CAM_ADJUSTABLE_ALT_BUTTON           1
    #define CAM_ALT_BUTTON_NAMES                { "Playback", "Video", "Display" }
    #define CAM_ALT_BUTTON_OPTIONS              { KEY_PLAYBACK, KEY_VIDEO, KEY_DISPLAY }
    #define CAM_DRIVE_MODE_FROM_TIMER_MODE      1

    #define CAM_DRYOS                           1
    #define CAM_DRYOS_2_3_R39                   1
    #define CAM_DRYOS_2_3_R47                   1
    #define CAM_PROPSET                         6
    #define CDM                         99
    #define UI_CS_PROP			    0x800D
    #define UI_CT_PROP			    0x800E 	
    #define STD_PERIOD 			    _EngDrvRead(0xC0F06014)
    #undef PERIOD_COUNT
    #define PERIOD_COUNT		((volatile int*)0xC0F0703C) // LiveView refresh
    #undef SYNCH_FPS
    #define SYNCH_FPS                           30
//    #define SYNCH_FPS                           60
    #undef SYNCH_SPEED
//    #define SYNCH_SPEED			    		8
    #define SYNCH_SPEED			    		4
    #define CAM_RAW_ROWPIX                      4176
    #define CAM_RAW_ROWS                        3062

    #define CAM_HAS_CMOS                        1
    #undef  CAM_CHDK_HAS_EXT_VIDEO_MENU
    #undef  CAM_CAN_UNLOCK_OPTICAL_ZOOM_IN_VIDEO
    #define CAM_HAS_VIDEO_BUTTON                1
    #undef  CAM_VIDEO_CONTROL

    #define CAM_HAS_JOGDIAL                     1
    #undef  CAM_USE_ZOOM_FOR_MF

    #undef  CAM_UNCACHED_BIT
    #define CAM_UNCACHED_BIT                    0x40000000

    #define CAM_DNG_LENS_INFO                   { 45,10,900,10,35,10,68,10 }
    #define cam_CFAPattern                      0x02010100

    #define CAM_COLORMATRIX1                            \
    544808, 1000000, -174047, 1000000, -80399, 1000000, \
    -75055, 1000000,  440444, 1000000,  11367, 1000000, \
     -5801, 1000000,   71589, 1000000, 118914, 1000000

    #define cam_CalibrationIlluminant1          17

    #define CAM_JPEG_WIDTH                      4000
    #define CAM_JPEG_HEIGHT                     3000

    #define CAM_ACTIVE_AREA_X1                  96
    #define CAM_ACTIVE_AREA_Y1                  18
    #define CAM_ACTIVE_AREA_X2                  4168
    #define CAM_ACTIVE_AREA_Y2                  3062

    #define PARAM_CAMERA_NAME                   3
    #define CAM_DATE_FOLDER_NAMING              0x400
    #define CAM_FILE_COUNTER_IS_VAR             1     // file counter is variable file_counter_var in stubs, not a param
    #undef  CAM_SENSOR_BITS_PER_PIXEL
    #define CAM_SENSOR_BITS_PER_PIXEL           12

    #define CAM_EXT_TV_RANGE                    1
 //   #define CAM_NEED_HP 				1  // causes crash when booting with long press
    #undef  CAM_HAS_ERASE_BUTTON
    #define CAM_SHOW_OSD_IN_SHOOT_MENU          1

    #define DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY   1           // Draw pixels on active bitmap buffer only.
    #define DRAW_ON_YUV_OVERLAY                 1
    #undef  CAM_BITMAP_WIDTH
    #undef  CAM_BITMAP_HEIGHT
    #undef  CAM_SCREEN_WIDTH
    #undef  CAM_SCREEN_HEIGHT
    #define CAM_BITMAP_WIDTH                    640 // Actual width of bitmap screen in <s>bytes</s> pixels
    #define CAM_BITMAP_HEIGHT                   480 // Actual height of bitmap screen in rows
    #define CAM_SCREEN_WIDTH                    640 // Width of bitmap screen in CHDK co-ordinates (360 or 480)
    #define CAM_SCREEN_HEIGHT                   480 // Height of bitmap screen in CHDK co-ordinates (always 240 on all cameras so far)
    #define CAM_SUPPORT_BITMAP_RES_CHANGE       1
    
    #undef  CAM_VIEWPORT_PIXEL_FORMAT
    #define CAM_VIEWPORT_PIXEL_FORMAT           2   // mandatory on DIGIC6, pixel format is YUY2 (uyvy)
    #undef  ASPECT_GRID_XCORRECTION
    #define ASPECT_GRID_XCORRECTION(x)  ( ((x)<<3)/9  )
    #undef  EDGE_HMARGIN
    #define EDGE_HMARGIN                        10

    //#define CAM_HAS_FILEWRITETASK_HOOK          1
    //#define CAM_FILEWRITETASK_SEEKS             1

    #undef  CAM_BITMAP_PALETTE
    #define CAM_BITMAP_PALETTE                  29

    #define CAM_ZEBRA_NOBUF                     1

    //#define CAM_QUALITY_OVERRIDE                1

    #undef  CAMERA_MIN_DIST
    #define CAMERA_MIN_DIST                     50         // Override min subject distance
    #undef  CAMERA_MAX_DIST
    #define CAMERA_MAX_DIST                     1550000     // Override max subject distance; manually checked up to 1550388, with MF max 1369863 (double step)

    //#define CAM_HAS_GPS                         1

    //#define CAM_CHDK_HAS_EXT_VIDEO_TIME         1
    #define CAM_SIMPLE_MOVIE_STATUS             1
    #undef  CAM_USB_EVENTID
    #define CAM_USB_EVENTID                     0x202       // Levent ID for USB control. Changed in DryOS R49, R50 so needs to be overridable.

    #define CAM_HAS_MOVIE_DIGEST_MODE           1 

    #define CAM_USE_ALT_SET_ZOOM_POINT          1           // Define to use the alternate code in lens_set_zoom_point()
    #define CAM_USE_ALT_PT_MoveOpticalZoomAt    1           // Define to use the PT_MoveOpticalZoomAt() function in lens_set_zoom_point()
    #define CAM_NEED_SET_ZOOM_DELAY             300

    #define MKDIR_RETURN_ONE_ON_SUCCESS         1    // mkdir() return 1 on success, 0 on fail.

    #undef  REMOTE_SYNC_STATUS_LED
    // #define REMOTE_SYNC_STATUS_LED              0xC022C30C  // TODO specifies an LED that turns on while camera waits for USB remote to sync
 
    #define CAM_SD_OVER_IN_AF  1
    #define CAM_SD_OVER_IN_MF  1
    #define COLOR_GREY            0x7F
    #define COLOR_GREY_P          0x7F
    #define COLOR_LIGHT_GREY      0x7F
    #define COLOR_DARK_GREY       0xBF
    #define COLOR_LIGHT_GREY_P    0x7F
    #define COLOR_DARK_GREY_P     0xBF
// -----------------------------------------------------------------------------------------------------------
#elif defined(CAMERA_ixus160_elph160)
    #define CAM_PROPSET	                    6
    #define CDM                         11
  //#define CDM                         99
  //#define UI_CS_PROP			    0x8010
  //#define UI_CT_PROP			    0x8011
    #define STD_PERIOD 			    _EngDrvRead(0xC0F06014)
    #undef PERIOD_COUNT
    #define PERIOD_COUNT		((volatile int*)0xC0F0703C) // LiveView refresh
    #undef SYNCH_FPS
    #define SYNCH_FPS                           30
//    #define SYNCH_FPS                           60
    #undef SYNCH_SPEED
//    #define SYNCH_SPEED			    		8
    #define SYNCH_SPEED			    		4
    #define CAM_DRYOS                       1
    #define CAM_DRYOS_2_3_R39               1       // Defined for cameras with DryOS version R39 or higher
    #define CAM_DRYOS_2_3_R47               1       // Defined for cameras with DryOS version R47 or higher
    #define CAM_NEED_HP 				1
    #define CAM_RAW_ROWPIX                  5248    // Found @0xff96986c
    #define CAM_RAW_ROWS                    3920    // Found @0xff969874

    #define CAM_JPEG_WIDTH                  5152
    #define CAM_JPEG_HEIGHT                 3864
    
    #define CAM_ACTIVE_AREA_X1              0
    #define CAM_ACTIVE_AREA_Y1              0
    #define CAM_ACTIVE_AREA_X2              5192
    #define CAM_ACTIVE_AREA_Y2              3900
    #define CAM_HAS_VIDEO_BUTTON                1
    #undef  CAM_UNCACHED_BIT
    #define CAM_UNCACHED_BIT  0x40000000            // Found @0xff826238

    #undef  CAM_SENSOR_BITS_PER_PIXEL
    #define CAM_SENSOR_BITS_PER_PIXEL       12
    
    #define CAM_DNG_LENS_INFO               { 50,10, 400,10, 32,10, 69,10 } // See comments in camera.h
    #undef  CAMERA_MIN_DIST
    #define CAMERA_MIN_DIST                     10         // Override min subject distance
    
    #define cam_CFAPattern                  0x02010100 // Red  Green  Green  Blue
    
    //TODO
    #define CAM_COLORMATRIX1                             \
    827547, 1000000, -290458, 1000000, -126086, 1000000, \
    -12829, 1000000,  530507, 1000000,   50537, 1000000, \
      5181, 1000000,   48183, 1000000,  245014, 1000000
    #define cam_CalibrationIlluminant1      1       // Daylight
    
    #define CAM_DATE_FOLDER_NAMING 0x080            // Found @0xffaab6e4 (pass as 3rd param to GetImageFolder)
    #define PARAM_CAMERA_NAME 3                     // Found @0xffcb1568

    // MakeDirectory_Fut not found, using mkdir
    #define MKDIR_RETURN_ONE_ON_SUCCESS

    #define CAM_HAS_FILEWRITETASK_HOOK      1       // FileWriteTask hook is available (local file write can be prevented)
    #define CAM_FILEWRITETASK_SEEKS	        1       // Camera's FileWriteTask can do Lseek() - DryOS r50 or higher, the define could also be CAM_DRYOS_2_3_R50

    #define CAM_FILE_COUNTER_IS_VAR         1       // file counter is variable file_counter_var in stubs, not a param

    #undef  CAM_USES_ASPECT_CORRECTION
    #define CAM_USES_ASPECT_CORRECTION      1       // camera uses the modified graphics primitives to map screens an viewports to buffers more sized
    #undef  CAM_BITMAP_WIDTH
    #define CAM_BITMAP_WIDTH                720     // Actual width of bitmap screen in bytes

    #define CAM_LOAD_CUSTOM_COLORS          1       // Enable loading CHDK colors into the camera palette memory/hardware
    #undef  CAM_BITMAP_PALETTE
    #define CAM_BITMAP_PALETTE                  19
    #define CAM_ADJUSTABLE_ALT_BUTTON       1

    #define CAM_ALT_BUTTON_NAMES            { "Playback", "Help" }
    #define CAM_ALT_BUTTON_OPTIONS          { KEY_PLAYBACK, KEY_HELP }

    #define DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY  1

    #define CAM_DISABLE_RAW_IN_LOW_LIGHT_MODE   1   // For cameras with 'low light' mode that does not work with raw define this
    #define CAM_DISABLE_RAW_IN_DIGITAL_IS       1   // For cameras with 'Digital IS' mode that does not work with raw define this

    #define CAM_USE_ALT_SET_ZOOM_POINT          1   // Define to use the alternate code in lens_set_zoom_point()
    #define CAM_USE_ALT_PT_MoveOpticalZoomAt    1   // Define to use the PT_MoveOpticalZoomAt() function in lens_set_zoom_point()
    #define CAM_NEED_SET_ZOOM_DELAY             100 // Define to add a delay after setting the zoom position
    
    #undef  CAM_HAS_ERASE_BUTTON
    #undef  CAM_HAS_DISP_BUTTON
    #undef  CAM_DEFAULT_ALT_BUTTON
    #define CAM_DEFAULT_ALT_BUTTON          KEY_PLAYBACK
    #define SHORTCUT_TOGGLE_RAW             KEY_VIDEO // conflicts with adjustable alt, but not many to choose from
    #define CAM_HAS_VIDEO_BUTTON            1

    #undef  CAM_MARKET_ISO_BASE
    #define CAM_MARKET_ISO_BASE             200     // Override base 'market' ISO value, from isobase.lua
    #define CAM_SD_OVER_IN_AF               1
    #define CAM_SD_OVER_IN_AFL              1
    #define CAM_SD_OVER_IN_MF               1
    #undef  CAM_HAS_IRIS_DIAPHRAGM
    #define CAM_HAS_ND_FILTER               1

    #undef CAM_AF_LED
    #define CAM_AF_LED                      1
    
    #undef  CAM_USB_EVENTID
    #define CAM_USB_EVENTID                 0x202   // Levent ID for USB control. Changed in DryOS R49 so needs to be overridable.
    #define COLOR_GREY            0x12
    #define COLOR_GREY_P          0x12
    #define COLOR_LIGHT_GREY      0x0F
    #define COLOR_DARK_GREY       0x18
    #define COLOR_LIGHT_GREY_P    0x0F
    #define COLOR_DARK_GREY_P     0x18
 //#else
 //#error camera type not defined
 #endif
#if CAM_USES_ASPECT_CORRECTION
    #define ASPECT_XCORRECTION(x)   ((x)<<1)    // See comments for CAM_USES_ASPECT_CORRECTION above
#else
    #define ASPECT_XCORRECTION(x)   (x)         // See comments for CAM_USES_ASPECT_CORRECTION above
#endif

#endif