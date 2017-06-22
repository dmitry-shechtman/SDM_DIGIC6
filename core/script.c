#include "stdlib.h"
#include "keyboard.h"
#include "platform.h"
#include "core.h"
#include "gui.h"
#include "draw_palette.h"
#include "conf.h"
#include "script.h"
#include "gui_menu.h"
#include "ubasic.h"
 
#define SCRIPT_CONSOLE_NUM_LINES    15
#define SCRIPT_CONSOLE_LINE_LENGTH  45     
#define MAX_PARAM_NAME_LEN          64     
 
sc_param        *chdk_script_params = 0;             
static sc_param *tail = 0;
int             script_param_count;             
 
OSD_pos consoleOrigin;
static int num_lines;
static int line_length;
const char *state_ubasic_script=NULL; //ERR99
char cfg_name[100] = "\0";
char cfg_set_name[100] = "\0";
 
static const char *ubasic_script_default =
"@title Default TLAPSE\n"
"@param a first delay minutes\n"
"@default a 0\n"
"@param b first delay seconds\n"
"@default b 10\n"
"@param c shoot interval minutes\n"
"@default c 0\n"
"@param d shoot interval seconds\n"
"@default d 5\n"
"@param e number of repeats\n"
"@default e 3\n"
"@param f endless mode N/Y    0/1\n"
"@default f 1\n"
"@param g single/cont/time/burst\n"
"@default g 0\n"
"@param h Tv exposures\n"
"@default h 3\n"
"@param i Focus exposures\n"
"@default i 3\n"
"@param j bracket Tv/foc/Both   0/2\n"
"@default j 0\n"
"@param k light/dark/alternate   0/1/2\n"
"@default k 2\n"
"@param l num of 1/3 EV steps\n"
"@default l 3\n"
"@param m equal/auto/digi  0-2\n"
"@default m 0\n"
"@param n focus-step\n"
"@default n 50\n"
"@param p digiscope focus ref\n"
"@default p 500\n"
"@param q blank screen N/Y 0/1\n"
"@default q 0\n"
"@param r shutdown N/Auto/USB 0/1/2\n"
"@default r 0\n"
"@param s save stack N/Y 0/1\n"
"@default s 0\n"
"@param t Sunrise mode\n"
"@default t 0\n"
"@param u TXT/CSV 0/1\n"
"@default u 0\n"
"auto_focus_first\n"
"time_lapse a, b, c, d, e, f, g, h, i, j, k, l, m, n, p, q, r, s, t, u\n"
"exit_alt\n"
"end\n";

unsigned char lineYPos,scriptZoomSize;
char paramdesc[7][40]={{'\0'},{'\0'}};
char script_title[36],script_title2[36];
char script_params[SCRIPT_NUM_PARAMS][28];
int script_param_order[SCRIPT_NUM_PARAMS];
static char script_params_update[SCRIPT_NUM_PARAMS];
char script_console_buf[SCRIPT_CONSOLE_NUM_LINES][SCRIPT_CONSOLE_LINE_LENGTH+1];
static int script_console_lines=0;
extern int gConsoleLine,gPTPsession;
extern unsigned char gScriptFolder,paramSetChanged,gConsoleScroll,bigZoomYPos,mini_started,startup_script;

#define IS_SPACE(p)     ((*p == ' ')  || (*p == '\t'))
#define IS_EOL(p)       ((*p == '\n') || (*p == '\r'))

const char* skip_whitespace(const char* p)  { while (IS_SPACE(p)) p++; return p; }                                      
const char* skip_to_token(const char* p)    { while (IS_SPACE(p) || (*p == '=')) p++; return p; }                       
const char* skip_token(const char* p)       { while (*p && !IS_EOL(p) && !IS_SPACE(p) && (*p != '=')) p++; return p; }  
const char* skip_toeol(const char* p)       { while (*p && !IS_EOL(p)) p++; return p; }                                 
const char* skip_eol(const char *p)         { p = skip_toeol(p); if (*p == '\r') p++; if (*p == '\n') p++; return p; }  

const char* skip_tochar(const char *p, char end)
{
    while (!IS_EOL(p) && (*p != end)) p++;
    return p;
}

sc_param* find_param(char *name)
{
    sc_param *p = chdk_script_params;
    while (p)
    {
        if (strcmp(name, p->name) == 0)
            break;
        p = p->next;
    }
    return p;
}

sc_param* new_param(char *name)
{
    sc_param *p = malloc(sizeof(sc_param));
    memset(p, 0, sizeof(sc_param));
    if (tail)
    {
        tail->next = p;
        tail = p;
    }
    else
    {
        chdk_script_params = tail = p;
    }
    script_param_count++;

    p->name = malloc(strlen(name)+1);
    strcpy(p->name, name);

    return p;
}

const char* get_token(const char *p, char *buf, int maxlen)
{
    p = skip_whitespace(p);
    int l = skip_token(p) - p;
    int n = (l <= maxlen) ? l : maxlen;
    strncpy(buf, p, n);
    buf[n] = 0;
    return p + l;
}
 
static void process_title(const char *title) 
{
    register const char *ptr = title;
    register int i=0;

    while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
    while (i<(sizeof(script_title)-1) && ptr[i] && ptr[i]!='\r' && ptr[i]!='\n') 
    {
      script_title[i]=ptr[i];
      ++i;
    }
    script_title[i]=0;
}
 
static void process_description(const char *desc, int line) 
{
    register const char *ptr = desc;
    register int i=0;
    unsigned char leadingSpace=0;
    while (ptr[0]=='\t') ++ptr;                                                    
    while (i<(sizeof(paramdesc[0])-1) && ptr[i] && ptr[i]!='\r' && ptr[i]!='\n')   
      {paramdesc[line][i]=ptr[i];++i;}
       paramdesc[line][i]=0;                                                       
}
 
static int process_param(const char *param, int update) 
{
    register const char *ptr = param;
    register int n, i=0,j;
    char parm;
    char buf[26];
    while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;  
    if (ptr[0] && (ptr[0]>='a' && ptr[0]<='a'+SCRIPT_NUM_PARAMS) && (ptr[1]==' ' || ptr[1]=='\t')) 
     {
        n=ptr[0]-'a';
        parm = ptr[0]; 												
        ptr+=2;
        while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;  
	  script_params_update[n] = 1;									
        while (i<(sizeof(script_params[0])-1) && ptr[i] && ptr[i]!='\r' && ptr[i]!='\n')
         {
		if (update) 
		{ 
		} 
            else 
            {
     		 script_params[n][i]=ptr[i];
            }
            ++i;
         }
		if (!update)                                                                           
             {
              if(vid_get_bitmap_screen_width()>=360)
               {
                if(i>22) i=22;
                for(j=0;j<25-i-1;j++)
                {
                 script_params[n][i+j]=' ';									   
                }
                 script_params[n][24]=parm;                                                         
                script_params[n][25]='\0';                
               }
              else                                                                                 
               {
                {
                 if(i>17) i=17;
                 for(j=0;j<20-i-1;j++)										   
                 {
                   script_params[n][i+j]=' ';                
                 }
                 script_params[n][19]=parm;                                                        
                 script_params[n][20]='\0';                                                                               
                }
               }                               
             }
        n++;
      } 
     else n=0; 
   return n; 
}
 
static void process_default(const char *param, char update) 
{
    register const char *ptr = param;
    register int n;
                          
    while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
    if (ptr[0] && (ptr[0]>='a' && ptr[0]<='a'+SCRIPT_NUM_PARAMS) && (ptr[1]==' ' || ptr[1]=='\t')) 
    {
        n=ptr[0]-'a';
        ptr+=2;

	  if (startup_script || mini_started || !update || script_params_update[n])
        {
         if(!gScriptFolder)						
         {
        	 conf.ubasic_vars[n] = strtol(ptr, NULL, 0);	
         }
         else if(gScriptFolder==1)					
         {
        	 conf.ubasic_vars2[n] = strtol(ptr, NULL, 0);	
         }
        else if(mini_started || startup_script)
		{
		 ubasic_set_variable(n,strtol(ptr, NULL, 0)); 
		}
        else								
         {
          conf.ubasic_vars3[n] = strtol(ptr, NULL, 0);   	
         }
 
        }
    } 
}

const char* get_name(const char *p, int maxlen, sc_param **sp, int create)
{
    char str[MAX_PARAM_NAME_LEN+1];
    *sp = 0;
    p = skip_whitespace(p);
    if (p[0] && isalpha(p[0]))
    {
        p = get_token(p, str, maxlen);  
        *sp = find_param(str);          
        if ((*sp == 0) && create)       
            *sp = new_param(str);
    }
    return p;
}

static const char* get_values(sc_param *p, const char *ptr, char end)
{
    ptr = skip_whitespace(ptr);
    int len = skip_tochar(ptr, end) - ptr;

    if (p)
    {
        p->range = 0;
        p->range_type = MENUITEM_ENUM2|MENUITEM_SCRIPT_PARAM;

        p->option_buf = malloc(len+1);
        strncpy(p->option_buf, ptr, len);
        p->option_buf[len] = 0;

        const char *s = p->option_buf;
        int cnt = 0;
        while (*s)
        {
            s = skip_whitespace(skip_token(s));
            cnt++;
        }
        p->option_count = cnt;
        p->options = malloc(cnt * sizeof(char*));

        s = p->option_buf;
        cnt = 0;
        while (*s)
        {
            p->options[cnt] = s;
            s = skip_token(s);
            if (*s)
            {
                *((char*)s) = 0;
                s = skip_whitespace(s+1);
            }
            cnt++;
        }
    }

    ptr += len;
    if (end && (*ptr == end)) ptr++;
    return ptr;
}

static void process_values(const char *ptr)
{
    sc_param *p;
    ptr = get_name(ptr, MAX_PARAM_NAME_LEN, &p, 1);
    get_values(p, ptr, 0);
}
 
static void script_scan(const char *fn, int update_vars) 
{
    register const char *ptr = state_ubasic_script;                    
    register int i, j=0, n;
 
   if(gScriptFolder==1)
    {
    char *c;
    c=strrchr(fn, '/');                                               
    strncpy(script_title2, (c)?c+1:fn, sizeof(script_title2));
    script_title2[sizeof(script_title2)-1]=0;
    }
    for (i=0; i<SCRIPT_NUM_PARAMS; ++i) 
    {
     script_params[i][0]=0;
     script_param_order[i]=0;
    }

    while (ptr[0]) 
    {
        while (ptr[0]==' ' || ptr[0]=='\t') ++ptr; 
        if (ptr[0]=='@') 
        {
            if (strncmp("@title", ptr, 6)==0) 
            {
                ptr+=6;
                process_title(ptr);
            } 
           else if (strncmp("@param", ptr, 6)==0) 
            {
                ptr+=6;
                 n=process_param(ptr, 0); 
                                         
                if (n>0 && n<=SCRIPT_NUM_PARAMS) 
                {
                  script_param_order[j]=n;
                  j++;
                }
            } 
           else if ((update_vars||mini_started|| startup_script) && strncmp("@default", ptr,8)==0) 
            {
                ptr+=8;
                process_default(ptr, 0); 
            }
           else if (strncmp("@values", ptr, 7)==0)
            {
                process_values(ptr+7);
            }
        }
        while (ptr[0] && ptr[0]!='\n') ++ptr; 
        if (ptr[0]) ++ptr;
    }
}
 
void set_params_values_name(const char *fn, int param_set)
{
	int shift;
	register char *ptr = (param_set >= 0 ? cfg_set_name : cfg_name);  
	const char *name;

	if (fn == NULL || fn[0] == 0) { ptr[0] = 0; return; }

	strncpy(ptr, SCRIPT_DATA_PATH, 100); ptr[99]=0;                   
	shift = strlen(SCRIPT_DATA_PATH);
	name = strrchr(fn, '/');                                          
	if (name) name++; else name=fn;                                   
	strncpy(ptr+shift, name, 100-shift); ptr[99]=0;
	shift = strlen(ptr); if (shift >= 100) shift=99;

	if (param_set >= 0) sprintf(ptr+shift-4, "%d.txt\0", param_set);  
      else  strcpy(ptr+shift-3, "cfg\0");                            
}

int update_desc_only(const char *fn)
{
	int i, fd=-1, rcnt;
	register const char *ptr;
	struct stat st;
	char *buf;
	static int descnt;                                               
	set_params_values_name(fn, conf.script_param_set);                
	if (stat(cfg_set_name,&st) != 0)                                  
		return 0;
	buf=umalloc(st.st_size+1);
	if(!buf) return 0;                                                
	fd = open(cfg_set_name, O_RDONLY, 0777);
	if (fd < 0)                                                       
    {
	   ufree(buf);
	   return 0;
	 }
	rcnt = read(fd, buf, st.st_size);                                 
	buf[rcnt] = 0;
	close(fd);                                                        
	for(i = 0; i < SCRIPT_NUM_PARAMS; ++i) script_params_update[i]=0;
    ptr = buf;
    descnt=0;  
    
    while (ptr[0]) 
	{
        while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;                   
        if (ptr[0]=='@')
	   {
            if (strncmp("@param", ptr, 6) == 0)                      
              ptr+=6;
            else if (strncmp("@default", ptr, 8)==0)                  
              ptr+=8;
            else if (strncmp("@desc ", ptr,6)==0)                    
             {
              ptr+=6;
              process_description(ptr,descnt);                       
              descnt+=1;                                             
              if((descnt==1)&& (paramdesc[0][0]=='\0'))strcpy(paramdesc[0],script_title2); 
             }
       }
        while (ptr[0] && ptr[0]!='\n') ++ptr; 
        if (ptr[0]) ++ptr;
   }
	 ufree(buf);
	 return 1;
}
 
 
int load_params_values(const char *fn, int update_vars, int read_param_set)
{
	int i, fd=-1, rcnt;
	register const char *ptr;
	struct stat st;
	char *buf;
	static int descnt;                                               
	if (fn == NULL || fn[0] == 0 || !update_vars) return 0;
	set_params_values_name(fn, conf.script_param_set);                
	if (stat(cfg_set_name,&st) != 0)                                  
		return 0;
	buf=umalloc(st.st_size+1);
	if(!buf) return 0;                                                
	fd = open(cfg_set_name, O_RDONLY, 0777);
	if (fd < 0)                                                       
       {
        for(i=0;i<7;i++)
         {
          paramdesc[i][0]= '\0';            
         }
	   ufree(buf);
	   return 0;
	 }
	rcnt = read(fd, buf, st.st_size);                                 
	buf[rcnt] = 0;
	close(fd);                                                        
	for(i = 0; i < SCRIPT_NUM_PARAMS; ++i) script_params_update[i]=0;
    ptr = buf;
    descnt=0;  
    
    while (ptr[0]) 
	{
        while (ptr[0]==' ' || ptr[0]=='\t') ++ptr;                   
        if (ptr[0]=='@')
	   {
            if (strncmp("@param", ptr, 6) == 0)                      
		 {
              ptr+=6;
		      process_param(ptr, 0);                 
             } 
            else if (strncmp("@default", ptr, 8)==0)                  
             {
              ptr+=8;
		       process_default(ptr, 1);               
             }
            else if (strncmp("@desc ", ptr,6)==0)                    
             {
              ptr+=6;
              process_description(ptr,descnt);                       
              descnt+=1;                                             
              if((descnt==1)&& (paramdesc[0][0]=='\0'))strcpy(paramdesc[0],script_title2); 
             }
         }
        while (ptr[0] && ptr[0]!='\n') ++ptr; 
        if (ptr[0]) ++ptr;
      }
	 ufree(buf);
	 return 1;
}
 
void script_load(const char *fn, int saved_params) 
{
    int i, update_vars;
    FILE *fd = NULL;
    struct stat st;
    char *ScriptFile="";   
    int numberParams = 7;
    unsigned char no_name = 0;
    if(!gScriptFolder)
     {
       ScriptFile= conf.easy_script_file;
     }
     
    else if(gScriptFolder==1)
     {
      ScriptFile= conf.script_file;
     }
     
    else if(gScriptFolder==2)
     {
       ScriptFile= conf.user_script_file;
     }
  
    if(gScriptFolder==1)
   {   
    for(i=0;i<numberParams;i++)paramdesc[i][0]='\0'; 
   }   
   
    if(state_ubasic_script && state_ubasic_script != ubasic_script_default)                     
     free((void *)state_ubasic_script);                                                         
    state_ubasic_script = ubasic_script_default;                                                
 
    if (!fn[0])                                                                                 
     {                                                                                          
        if (!ScriptFile[0])                                                                     
        { 
         no_name = 1;
          if(gScriptFolder==1)
           fd =  fopen(SCRIPT_SYSTEM_DEFAULT_FILENAME, "rb");
          else if(gScriptFolder==2)
            fd =  fopen(SCRIPT_USER_DEFAULT_FILENAME, "rb");                                
           if (fd) 
            {
             if(gScriptFolder==1)
              strcpy((char *)fn,SCRIPT_SYSTEM_DEFAULT_FILENAME);
             else
              strcpy((char *)fn,SCRIPT_USER_DEFAULT_FILENAME);                                                                 
            }
        }
     } 
    else                                                                                        
     {
       fd = fopen(fn, "rb");
       if (!fd) 
        {
         update_vars = 1;                                                                       
        }
     }
     if(no_name) update_vars = 1;
     else
      update_vars = strcmp(fn, ScriptFile) != 0;                              			            
    if(stat((char *)fn,&st) != 0 || st.st_size == 0) 
      {
        update_vars = 1;                                                                        
        if(fd) 
          {
            fclose(fd);
            fd=0;
          }
       }

    if (fd)
     {
        int rcnt;
        char *buf;

        buf = malloc(st.st_size+2);                                                              
        if(!buf)                                                                                 
          {
            fclose(fd);
            return;
          }

        rcnt = fread(buf, 1, st.st_size,fd);                                                    
        if (rcnt > 0)
         {
          buf[rcnt] = '\n'; 
          buf[rcnt+1] = 0;
          state_ubasic_script = buf;                                                             
          if((strcmp(fn,"A/SDM/SCRIPTS3/Startup.txt") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_1.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_2.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_3.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_4.TXT") != 0)\
                                                           &&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_5.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_6.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_7.TXT") != 0)&&(strcmp(fn,"A/SDM/SCRIPTSM/MINI_8.TXT") != 0))
           strcpy(ScriptFile, fn);                                                                       
          else update_vars = 0;
         }
        else 
        {
         free(buf);
        }
        fclose(fd);
     } 
     
 
 script_scan(ScriptFile, update_vars);                                                
 if(gScriptFolder==1 && update_vars)
  load_params_values(ScriptFile, update_vars, 0);
 else if((gScriptFolder==1)&& !update_vars && !paramSetChanged)
  update_desc_only(ScriptFile);
 gui_update_script_submenu(); 
}
 
void script_console_clear() 
{
    register int i;

    for (i=0; i<SCRIPT_CONSOLE_NUM_LINES; ++i) 
    {
     script_console_buf[i][0]=0;
    }
    script_console_lines=0;
    draw_restore();
}
 
void script_console_draw() 
{
    register int i,j,l;
    static char buf[8];
    static int num_lines;
    static int cons_x;
    static int cons_y;
    static unsigned char textSize[15]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    char firstTextCharacter,secondTextCharacter,bgcolour=COLOR_TRANSPARENT,fgcolour=COLOR_WHITE;
    unsigned char mplay = ((mode_get()&MODE_MASK)==MODE_PLAY),cc;
    num_lines=15;  
    cons_x=consoleOrigin.x;
    cons_y=consoleOrigin.y;
    lineYPos=cons_y; 

      for (i=0; i<script_console_lines; ++i) 
       {
        textSize[i]=1;
        cc=1; 
        j=0; 
        l=strlen(script_console_buf[i]); 
        if(l) 
         {                      
          if((char)script_console_buf[i][0]=='~') 
           {
            if(!gConsoleScroll)textSize[i] = ((unsigned char)script_console_buf[i][1])-48;            
            l-=2;
            j+=2;	 
           }
           if((lineYPos+FONT_HEIGHT*textSize[i])>(16*FONT_HEIGHT-1))break;
           firstTextCharacter=(char)script_console_buf[i][j];
           if((firstTextCharacter=='!')||(firstTextCharacter==' ')||(firstTextCharacter=='#')||(firstTextCharacter=='@')||(firstTextCharacter=='$'))
            {l-=1;j+=1;} 
 
             if(firstTextCharacter=='!')     bgcolour=(!mplay)?COLOR_RED:COLOR_RED_P;
             else if(firstTextCharacter==' ')bgcolour=(!mplay)?COLOR_BLUE:COLOR_BLUE_P;
             else if(firstTextCharacter=='^')bgcolour=(!mplay)?COLOR_BLUE:COLOR_BLUE_P;             
             else if(firstTextCharacter=='#')bgcolour=(!mplay)?COLOR_GREEN:COLOR_GREEN_P;
             else if(firstTextCharacter=='$')bgcolour=(!mplay)?COLOR_WHITE:COLOR_WHITE_P;              
             else if(firstTextCharacter=='@')bgcolour=COLOR_TRANSPARENT; 
             else  {bgcolour=(!mplay)?COLOR_BG:COLOR_BG_P;cc=0;}
             
            secondTextCharacter=(char)script_console_buf[i][j];          
            if(cc&&((secondTextCharacter=='!')||(secondTextCharacter=='#')||(secondTextCharacter=='@')||(secondTextCharacter=='$')||(secondTextCharacter=='^')))
              {l-=1;j+=1;} 
             if(secondTextCharacter=='!')     fgcolour=(!mplay)?COLOR_RED:COLOR_RED_P;
             else if(secondTextCharacter=='#')fgcolour=(!mplay)?COLOR_GREEN:COLOR_GREEN_P;  
             else if(secondTextCharacter=='^')fgcolour=(!mplay)?COLOR_BLUE:COLOR_BLUE_P;             
             else if(secondTextCharacter=='@')fgcolour=COLOR_TRANSPARENT; 
             else  {fgcolour=(!mplay)?COLOR_FG:COLOR_FG_P;}              
            if(strcmp(&script_console_buf[i][j],"{}")==0) 
				{
				 bigZoomYPos=lineYPos;
				 scriptZoomSize=textSize[i];
				}
           else
		   {			   
           draw_string(cons_x,lineYPos, &script_console_buf[i][j],MAKE_COLOR(bgcolour,fgcolour),textSize[i]);         
           if(l<(conf.line_length/textSize[i]))
           {
          for (l; l<(conf.line_length/textSize[i]); ++l,++j) 
            {
             draw_char(cons_x+(l*FONT_WIDTH*textSize[i]),lineYPos, ' ',MAKE_COLOR(bgcolour,bgcolour),textSize[i]);           
            }
           }
           
           if(conf.line_length%textSize[i]) 
           {
            int tlx=cons_x+(l*FONT_WIDTH*textSize[i]);
            int brx=cons_x+(conf.line_length*FONT_WIDTH)-1;
            draw_filled_rect(tlx,lineYPos,brx,lineYPos+(FONT_HEIGHT*textSize[i])-1,MAKE_COLOR(bgcolour,bgcolour));          
           } 
		   }		   
         }
 
         lineYPos =lineYPos+(textSize[i]*FONT_HEIGHT);
         textSize[i+1]=1;
       }       
}

void console_redraw()
{
 draw_restore();
 script_console_draw();
}
 
void script_console_add_line(const char *str) 
{
    register int i;
 
     num_lines=15;
#if defined(OPT_PTP)
    if(gPTPsession)line_length=36;
#else
    line_length=conf.line_length;
 #endif
 
  if(gConsoleScroll)                                                                
    {
     if ((script_console_lines == num_lines))                                      
      {
        for (i=1; i<num_lines; ++i)                                                
        {
            strcpy(script_console_buf[i-1], script_console_buf[i]);
        }
        --script_console_lines;
      }

      strncpy(script_console_buf[script_console_lines], str,conf.line_length);           
      script_console_buf[script_console_lines][conf.line_length]=0;                      
      ++script_console_lines;                                                       
      script_console_draw();                                                        
    }
    
   else 
    {   
      console_add_line(str,gConsoleLine);                                            
    }
}

void console_add_line(const char *str,int x)                                       
{
    register int i;
    strncpy(script_console_buf[x-1], str, conf.line_length);                            
    script_console_buf[x-1][conf.line_length]=0;                                        
    script_console_lines=num_lines;                                                
    script_console_draw();
}
 
static inline int script_con_line_index(int i) 
{
    return i%SCRIPT_CONSOLE_NUM_LINES;
}

char* get_whole_console(void)
{
    int i,c,l=0;
    char *res, *tmp;
    for(c = 0; c < script_console_lines; ++c) 
    {
     i=script_con_line_index(c);
     l+=strlen(script_console_buf[i])+2; 
    }
    l++; 
    res=malloc(l);
    if (!res) return NULL;
    tmp=res;
    res[0]=0;
    for(c = 0; c < script_console_lines; ++c) 
    {
     i=script_con_line_index(c);
     tmp=strcat(tmp,script_console_buf[i]);
     tmp=strcat(tmp,"\r\n");
    }
    return res;
}
