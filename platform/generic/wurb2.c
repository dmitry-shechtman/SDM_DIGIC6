extern int gLowLightCount;
extern long FastLapseStart,FastLapseEnd;
static unsigned long offset = 0;
extern int gSwitchType;
extern int nCa,nSW,gSDMShootMode;
extern unsigned long gDetectUsb;
extern unsigned char HPTimerFinished,gSDM_will_use_flash; 
unsigned short gOnReleaseCount; 
void TenMsecCount()
{
 unsigned long count = 0;
 long usb_physw[3];
 unsigned int tick = get_tick_count();
 do{}										
 while(tick == get_tick_count());
 tick = get_tick_count();						
 do										
 {
  usb_physw[2] = 0; 
  _kbd_read_keys_r2(usb_physw);
  get_tick_count(); 							
  usb_physw[2]& USB_MASK;
  ++count;             
 }
 while(tick == get_tick_count());
 int c = (int)((count+5)/10);
 if(c>conf.CountForOneMsec){conf.CountForOneMsec=c;conf_save();}
}

int get_usb_bit() 
{
 long usb_physw[3];
 usb_physw[USB_IDX] = 0;
 _kbd_read_keys_r2(usb_physw);
 return(( usb_physw[USB_IDX] & USB_MASK)==USB_MASK) ; 
}

void wait_until_remote_button_is_released(void)
{
int count1;
int count2;
int tick,tick2,tick3;
int nSwitch;
int prev_usb_power,cur_usb_power;
unsigned long count = 0;
 
static unsigned long MinutesStart,WDStart;
static unsigned long nextTargetTickCount;

asm volatile ("STMFD SP!, {R0-R11,LR}\n"); 

tick = get_tick_count();
tick2 = tick;
static long usb_physw[3];
debug_led(0);   
 
 if((shooting_get_drive_mode()==1)&& conf.fastlapse && conf.burst_frame_count && !gLowLightCount)
  {	
   while(1)										
   {
    do										
     {
      usb_physw[2] = 0;                                            
      _kbd_read_keys_r2(usb_physw);
      ++offset;             
     }
     
     while((tick == get_tick_count())&&((((usb_physw[2] & USB_MASK)!=USB_MASK) && (gSwitchType==RICOH)) || (((usb_physw[2] & USB_MASK)==USB_MASK) && (gSwitchType==STANDARD))));					
    if(!((((usb_physw[2] & USB_MASK)!=USB_MASK) && (gSwitchType==RICOH)) || (((usb_physw[2] & USB_MASK)==USB_MASK) && (gSwitchType==STANDARD))))break;
    tick = get_tick_count();     
    if((tick-tick2)>8000){tick2=0;rwd();}
    offset = 0;									
   }
   FastLapseStart=get_tick_count();
   MinutesStart = FastLapseStart;
   WDStart = FastLapseStart;
  }

else if((shooting_get_drive_mode()==1)&& conf.fastlapse && conf.burst_frame_count)
 {
  if(conf.FastLapseDelay)
  {
  nextTargetTickCount = gLowLightCount*conf.FastLapseDelay*100;
  while((shooting_get_tick_count()-FastLapseStart)<= nextTargetTickCount)
   {
    usb_physw[2] = 0;                                            
    _kbd_read_keys_r2(usb_physw);
    if((usb_physw[2] & USB_MASK)==USB_MASK){gLowLightCount=conf.burst_frame_count;break;}
    rwd2();
    if((shooting_get_tick_count()- MinutesStart)>=60000)  
    {
     play_sound(4);
     MinutesStart=shooting_get_tick_count();		
     offset+=conf.FastLapseFineTune;				   
    }
   }
  count1=offset;
  do
  {
   usb_physw[2] = 0; 
   _kbd_read_keys_r2(usb_physw);
   get_tick_count(); 								
   usb_physw[2]& USB_MASK; 							
   --count1;
  }
  while (count1>0);
  }
  FastLapseEnd=get_tick_count();
 }

 else if ((!shooting_get_drive_mode()|| (shooting_get_drive_mode()==1) || ((shooting_get_drive_mode()== 3) && (state_shooting_progress != SHOOTING_PROGRESS_PROCESSING)) || ((shooting_get_drive_mode()== 3)&& conf.custom_timer_synch)))
  {
  if((gSDMShootMode==C_FAST)||((gSDMShootMode==BURST)&& gFullPressed))
   {
    conf.synch_enable = 1;
    nCa = 2;
    nSW = 114;											
   }
   
  if(conf.synch_enable && (gSDMShootMode!=BURST)) 
   { 
    if(conf.ricoh_ca1_mode)
     {
      
      
      
	if(shooting_get_drive_mode()==1 && state_shooting_progress == SHOOTING_PROGRESS_PROCESSING)
    {		
		if(conf.bracket_type>2)
                {
		     shoot_counter--;
		    }
          
		  else{                            
                 if(!(conf.sunrise && conf.srb))                                                  
                 {    
			prev_usb_power=0;
			nSwitch = 0;
			do
				{     
				usb_physw[2] = 0;                                            
				_kbd_read_keys_r2(usb_physw);
				cur_usb_power = (usb_physw[2] & USB_MASK)==USB_MASK;
				if(cur_usb_power){
					if(!prev_usb_power){
						tick2 = get_tick_count();
						prev_usb_power=cur_usb_power;
						}
					else{
						if((int)get_tick_count()-tick2>1000) {debug_led(0);}
						}
					}
				else{
					if(prev_usb_power){
						tick3 = (int)get_tick_count()-tick2;
						if(nSwitch==10) {
							if(tick3>50) shutter_int=1;
							nSwitch=20;
							}
						if(nSwitch==0 && tick3>0) {
							if(tick3<50) {
							nSwitch=10;
							}
						else{
							if(tick3>1000) shutter_int=1;
								nSwitch=20;
							}
						}
						prev_usb_power=cur_usb_power;
						}
					}
				if((int)get_tick_count()-tick >= DELAY_TIMEOUT) {nSwitch=20;shutter_int=2;}
				}
			 while(nSwitch<20);
			 }
               } 
		} 
      
      
      
		else                          
      {	
			shoot_counter=0;
                  if((shooting_get_drive_mode()== 3) && (conf.dist_mode ||conf.tv_bracket_value)) shutter_int = 3;  
			if(conf.bracket_type>2)
           {
				shoot_counter=(conf.bracket_type-2)*2;
			  }
      if(!(((conf.dist_mode || conf.tv_bracket_value) && (shooting_get_drive_mode()==1))|| ((shooting_get_drive_mode()==2)&&!conf.custom_timer_synch)|| tl.running || (conf.dist_mode && !shooting_get_drive_mode()) ))
              {
#if !defined(CAMERA_m3)
               do{}
               while (!(shooting_get_prop(PROPCASE_SHOOTING)));
#endif
               debug_led(1);
               if(conf.on_release && gSwitchType==STANDARD)  
               {
                do 
                {                                                                
                 rwd();
                 usb_physw[2] = 0;                                            
                 _kbd_read_keys_r2(usb_physw);
                }
                while((usb_physw[2]&USB_MASK)==USB_MASK);
               }

               else if(!conf.on_release || gSwitchType==RICOH) 
               {
                if(gSwitchType==STANDARD)_SleepTask(2000);
                do 
                {                   
                 rwd();
                 usb_physw[2] = 0;                                            
                 _kbd_read_keys_r2(usb_physw);
                }
                while((usb_physw[2] & USB_MASK)!=USB_MASK);
                gDetectUsb=666;
               }
             }
            }
     } 

    else  
     {
      do
          {
            usb_physw[2] = 0;                                             
           _kbd_read_keys_r2(usb_physw);           
           }
        while((usb_physw[2]&USB_MASK) &&  ((int)get_tick_count()-tick < DELAY_TIMEOUT));
     }
     
 
   if (conf.synch_delay_enable)				
     { 
      int target_count = conf.synch_flash_delay;	
      if(conf.add_synch_delays)target_count += conf.synch_fine_delay;
      if (target_count) 					
       {
        for (count1=0;count1<target_count;count1++) 	
        {
         if(!(count1%100000))rwd();				
         for (count2=0;count2<conf.CountForOneMsec/10;count2++)	            
         {
          usb_physw[2] = 0; 
          _kbd_read_keys_r2(usb_physw);
          get_tick_count(); 					
          usb_physw[2]& USB_MASK;
         }
        }
       } 
       
       if(conf.synch_coarse_delay && conf.add_synch_delays)
       {
        HPTimer.units=100000;                      
        HPTimer.interval=conf.synch_coarse_delay;
        HPTimerFinished=0;
        if(start_self_timer())      
         {
          do {}
          while(!HPTimerFinished);
          HPTimerFinished=0;
         }  
       }
       
     }
     
  volatile int* counter;
  counter= PERIOD_COUNT;
  gOnReleaseCount=*(counter) & 0xffff;    

#if defined(CAM_PRECISION_SYNCH)
 
if((get_tick_count()>5000)&&(!gSDM_will_use_flash|| (gSDM_will_use_flash&&conf.precision_synch_flash)))
{
	int std_period = STD_PERIOD; 				
	int cur_cnt = *(counter) & 0xffff; 			
	int sync_time = std_period * 3;			
	int sync_period = sync_time - (std_period - cur_cnt);
#if defined(CAMERA_g15)
 if(cur_cnt> ((96*std_period)/100))
#else
   if ((std_period - cur_cnt) < (std_period/15))
#endif
	 {
	  
	  sync_period -= std_period;
	  while ((*(counter) & 0xffff) >= cur_cnt) {};
	 }

   *(volatile int*)(0xC0F06014) = sync_period; 	
	*(volatile int*)(0xC0F06000) = 1;
	while (*(volatile int*)(0xC0F06000)) {_SleepTask(10);}; 

#if !defined(CAMERA_s110)&& !defined(CAMERA_g15)            									
	*(volatile int*)(0xC0F06014) = std_period; 	
	*(volatile int*)(0xC0F06000) = 1;
#endif

	_SleepTask(40);
 }
 
#endif 
}  

} 
 asm volatile ("LDMFD SP!, {R0-R11,LR}\n"); 
}

