#ifndef SCRIPT_H
#define SCRIPT_H

//-------------------------------------------------------------------
#define SCRIPT_USER_DEFAULT_FILENAME	    "A/SDM/SCRIPTS3/2dTLapse.txt"
#define SCRIPT_SYSTEM_DEFAULT_FILENAME	    "A/SDM/SCRIPTS2/A_.txt"
#define SCRIPT_NUM_PARAMS           26
#define SCRIPT_DATA_PATH            "A/SDM/PARAMS/"
//-------------------------------------------------------------------
extern const char *state_ubasic_script;

extern char script_title[],script_title2[];
extern char paramdesc[7][40];
extern char script_params[SCRIPT_NUM_PARAMS][28];
extern char script2_params[SCRIPT_NUM_PARAMS][28];
extern char script3_params[SCRIPT_NUM_PARAMS][28];
extern int script_param_order[SCRIPT_NUM_PARAMS];
//-------------------------------------------------------------------
extern void script_load(const char *fn, int saved_params);
extern void script_console_clear();
extern void script_console_add_line(const char *str);
extern void script_console_draw();
extern int console_line_length();
extern void console_add_line(const char *str,int x);
extern int load_params_values(const char *fn, int update_vars, int read_param_set);
// extern void save_params_values(int unconditional);
extern void lua_script_exec(char *script);
char* get_whole_console(void);
//-------------------------------------------------------------------
// Structure used to hold a single script parameter
typedef struct _sc_param
{
    char            *name;          // Parameter name
    char            *desc;          // Title / description
    int             val;            // Current value
    int             def_val;        // Default value (from script, used to reset values)
    int             old_val;        // Previous value (to detect change)
    int             range;          // Min / Max values for validation
    short           range_type;     // Specifies if range values is signed (-9999-32767) or unsigned (0-65535)
                                    // Note: -9999 limit on negative values is due to current gui_menu code (and because menu only displays chars)
    short           data_type;      // See defines above
    int             option_count;   // Number of options for parameter
    char*           option_buf;     // Memory buffer to store option names for parameter
    const char**    options;        // Array of option names (points into option_buf memory)

    struct _sc_param* next;         // Next parameter in linked list
} sc_param;
extern sc_param* chdk_script_params;
#endif
