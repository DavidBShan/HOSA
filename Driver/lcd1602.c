#include "lcd1602.h"
#include "delay.h"
#include "sys.h"

// Reverse the bits of a byte
u8 Reverse_Bit(u8 data)
{
    u8 i = 0 ,temp = 0;;
	  
	// Loop through each bit in input data 
	  for(i = 0; i < 8; i++)
	 {
		 // Extract each bit from the input data, shift it to the opposite position, and accumulate in 'temp'
	   temp += (((data >> i) & 0x01) << (7 - i));
	 }
	 
	 //Return reversed byte
	 return temp;
}

/*------------------------------------------------
Write Command to LCD
------------------------------------------------*/
void LCD_Write_Com(unsigned char com) 
{                              
  LCD1602_RS=0;	 // Set Register Select to 0 for Command
	delay_us(1);
  LCD1602_RW=0; // Set Read/Write to 0 for write
  delay_us(1);
  LCD1602_EN=1;  // Enable LCD
	delay_us(1);
	// When not affecting A8-A15, write command to A0-A7
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Reverse_Bit(com));
	delay_us(100);
  LCD1602_EN=0; //Disable LCD
}

/*------------------------------------------------
Write Data to LCD
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
{                                  
  LCD1602_RS=1;   // Set Register Select to 1 for data              
	delay_us(1);
  LCD1602_RW=0;  // Set Read/Write to 0 for write
  delay_us(1);	
  LCD1602_EN=1;   // Enable LCD                  
	delay_us(1);
	//When not affecting A8-A15, write data to A0-A7
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Reverse_Bit(Data));
	delay_us(100);
  LCD1602_EN=0;
}
/*------------------------------------------------
Write string to LCD
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);  //Set cursor to the beginning of the first line
 	}
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);  //Set cursor to the beginning of the second line
 	}        
	while (*s) //Check if end of string is reached
 	{     
		 LCD_Write_Data( *s);//Write character to LCD
		 s ++;  //Increment pointer
 	}
 }
/*------------------------------------------------
Write a character to LCD
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
{     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);    //Set cursor to beginning of first line
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);  //Set cursor to beginning of second line 
 	}        
 LCD_Write_Data( Data); //Write character to LCD
}
 

 /*------------------------------------------------
Clear LCD
------------------------------------------------*/
 void LCD_Clear(void) 
{ 
 LCD_Write_Com(0x01); //Send clear display command to LCD
 delay_ms(5); // Delay to allow for clear
}

/*------------------------------------------------*/
 void LCD_Init(void) 
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);  //Enable GPIOA GIOPC Clock
	  
		 // Configure GPIOA pins 0-7 as output (A0-A7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Configure pins as output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // Set GPIO speed
    GPIO_Init(GPIOA, &GPIO_InitStructure); // Initialize GPIOA
    
    // Configure GPIOC pins 13-15 as output (LCD1602 RS-RW-EN)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // Configure pins as output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // Set GPIO speed
    GPIO_Init(GPIOC, &GPIO_InitStructure);   // Initialize GPIOC
	
   LCD_Write_Com(0x38);
   delay_ms(5);  
	 LCD_Write_Com(0x38);
   delay_ms(5);  
	 LCD_Write_Com(0x38);
   delay_ms(5);  
	 //Turn off display
   LCD_Write_Com(0x08);
   delay_ms(5); 	
	 //Clear display
   LCD_Write_Com(0x01);
   delay_ms(5); 
	 // Set cursor direction and specify display shift
   LCD_Write_Com(0x06); 
	 delay_ms(5); 
	 //Turn on display, cursor, and blinking
   LCD_Write_Com(0x0C);
	 delay_ms(5); 
}

