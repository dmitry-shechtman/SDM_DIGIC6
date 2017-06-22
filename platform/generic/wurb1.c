 
extern int gLowLightCount;
extern long FastLapseStart,FastLapseEnd;
static unsigned long offset = 0;
extern int gSwitchType;
extern int nCa,nSW,gSDMShootMode;
extern unsigned long gDetectUsb; 
extern unsigned char HPTimerFinished;
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

void wait_until_remote_button_is_released(void)
{
 long x;
 int count1;
 int count2;
 int tick,tick2,tick3;
 int nSwitch;
 int prev_usb_power,cur_usb_power;
 static unsigned long MinutesStart,WDStart;
 static unsigned long nextTargetTickCount;
 static long usb_physw[3];
 
 asm volatile ("STMFD SP!, {R0-R11,LR}\n"); 

 tick=get_tick_count();
 tick2 = tick;

 if((gSDMShootMode==BURST)&&!gLowLightCount)FastLapseStart=get_tick_count();
 else if((gSDMShootMode==BURST)&&gLowLightCount)FastLapseEnd=get_tick_count();

 if((shooting_get_drive_mode()==1) && conf.fastlapse && conf.burst_frame_count && !gLowLightCount)
  {
   while(1)										
   {
    do										
     {
      x=get_mmio();
      ++offset;             
     }
     while((tick == get_tick_count())&&((!(x&USB_MASK) && (gSwitchType==RICOH)) || ((x&USB_MASK) && (gSwitchType==STANDARD))));
     if(((x&USB_MASK) && (gSwitchType==RICOH)) || (!(x&USB_MASK) && (gSwitchType==STANDARD)))break;
     tick = get_tick_count();                                            
     offset = 0;										 
     if((tick-tick2)>8000){tick2=0;rwd();}					 
   }
   FastLapseStart=get_tick_count();
   MinutesStart = FastLapseStart;
   WDStart = FastLapseStart;
  }

else if((shooting_get_drive_mode()==1) && conf.fastlapse && conf.burst_frame_count && gLowLightCount)
 {
  if(conf.FastLapseDelay)
 {
  nextTargetTickCount = gLowLightCount*conf.FastLapseDelay*100;
  while((shooting_get_tick_count()-FastLapseStart)<= nextTargetTickCount)
   {
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
   x=get_mmio();
   get_tick_count(); 								
   ((!(x&USB_MASK) && (gSwitchType==RICOH)) || ((x&USB_MASK) && (gSwitchType==STANDARD))); 	
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
		else {                                                                                                            
          if(!(conf.sunrise && conf.srb))                                                 
           {                                
			prev_usb_power=0;
			nSwitch = 0;
			do                                            
				{     
				x=get_mmio();
				cur_usb_power = x&USB_MASK;
				if(cur_usb_power)                         
             {
				  if(!prev_usb_power)                 
               {
					 tick2 = get_tick_count();   
					 prev_usb_power=cur_usb_power; 
					}
				  else                              
               {
					 if((int)get_tick_count()-tick2>1000) {debug_led(0);}
					}
				 }
				   else
                {                                           
					  if(prev_usb_power)                        
                  {
						tick3 = (int)get_tick_count()-tick2; 

						if(nSwitch==10) 
                   {
						  if(tick3>50) shutter_int=1; 
						  nSwitch=20;
						 }

						if(nSwitch==0 && tick3>0)                
                   {
						  if(tick3<50)                   
                     {
							 nSwitch=10;
							}
						  else
                     {
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
             do{}
             while (!(shooting_get_prop(PROPCASE_SHOOTING)));
             debug_led(1);
              if(conf.on_release && gSwitchType==STANDARD)  
               {
                do 
                {                                                                
                 rwd();
                 x=get_mmio();
                }
                while((x&USB_MASK)==USB_MASK);
               }

               else if(!conf.on_release || gSwitchType==RICOH) 
               {
                if(gSwitchType==STANDARD)_SleepTask(2000);
                do 
                {                   
                 rwd();
                 x=get_mmio();
                }
                while((x & USB_MASK)!=USB_MASK);
                gDetectUsb=666;
               }
          }
         }
     } 
  
    else                                                        
     {
      do
       x=get_mmio();
       while((x&USB_MASK) &&  ((int)get_tick_count()-tick < DELAY_TIMEOUT));
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
 }
 }
debug_led(0);
asm volatile ("LDMFD SP!, {R0-R11,LR}\n"); 
}
