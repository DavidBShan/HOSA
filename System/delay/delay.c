#include "delay.h"
#include "sys.h" 
//note us is microseconds, ms is milliseconds

//factors for microsecond and millisecond delay
static u8  fac_us=0;
static u16 fac_ms=0;

//initialize delay module
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//config systick to use HCLK/8 as clock source
	
	//calculate delays for factors for microsecond and millisecond based on system clock
	fac_us=SystemCoreClock/8000000;	
	fac_ms=(u16)fac_us*1000;
}							

//Delay in microseconds    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //set load register to generate desired microseconds
	SysTick->VAL=0x00;        //reset timer
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //start timer
	//wait until delay has passed
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //close timer
	SysTick->VAL =0X00;       //reset timer
}


//Note: pay attention to range: largest delay is 1864
//This is because nms <= 0xffffff*8*1000/SYSCLK
//generate delay in milliseconds
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	//set load register with number of clock cycles to achieve certain milliseconds
	SysTick->LOAD=(u32)nms*fac_ms;
	//clear timer
	SysTick->VAL =0x00;
	//start systick timer
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	//wait until delay has passed
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	//close timer
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;   
	//clear timer
	SysTick->VAL =0X00; 
} 

































