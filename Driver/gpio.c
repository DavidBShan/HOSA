#include "gpio.h"

//////////////////////////////////////////////////////////////////////////////////	 
//GPIO Configuration						  
////////////////////////////////////////////////////////////////////////////////// 	   

// Function to initialize GPIO for KEY
void KEY_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;  // Declare a structure to configure GPIO
    
    // Enable the clock for GPIOA, GPIOB, and AFIO (Alternate Function I/O)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    // Disable JTAG to release PB3 and PB4 for general-purpose I/O
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
    // Configure GPIOB pins 12, 13, and 14 as input with pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Input Pull-up mode
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    // Configure GPIOB pin 6 as output in push-pull mode with 50MHz speed
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // Output Push-pull mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // Set GPIO speed to 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Set the initial state of GPIOB pin 6 to LOW (0)
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // Output 0	
}



