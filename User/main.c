#include "sys.h"
#include "delay.h"
#include "lcd1602.h"
#include "gpio.h"
#include "usart1.h"
#include "usart3.h"
#include "ds18b20.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdbool.h>

#define    NUM    7 

#define STM32_RX1_BUF       Usart1RecBuf 
#define STM32_Rx1Counter    RxCounter
#define STM32_RX1BUFF_SIZE  USART1_RXBUFF_SIZE

#define STM32_RX3_BUF       Usart3RecBuf 
#define STM32_Rx3Counter    Rx3Counter
#define STM32_RX3BUFF_SIZE  USART3_RXBUFF_SIZE

unsigned char heartrate = 0;//heart rate
unsigned char heartrate_min = 55;//min heart rate
unsigned char heartrate_max = 120;//max heart rate

unsigned char SBP = 0;//Systolic Blood Pressure
unsigned char DBP = 0;//Diastolic Blood Pressure
/*Note: the critical value of hypertension is systolic blood pressure >= 140 or diastolic blood presssure >= 90*/
unsigned char SBP_Max = 140;
unsigned char DBP_Max = 90;
/*×¢£ºthe criticial value of hypotention is systolic blood pressure <= 90 or diastolic blood pressure <= 60*/
unsigned char SBP_Min = 90;
unsigned char DBP_Min = 60;

float    temperature=0.0;//temperature
float    temperature_min=20.0;//min temperature
float    temperature_max=37.5;//max temperature

char display[16];    //display buffer
unsigned char setn = 0;//configuration indicator
bool sendFlag = 0; 

#define  length   16     //length of arrays
#define  min_len  0      //min index
#define  max_len  6      //max index

u16  sbp_buf[length];
long   sbp_sumdat=0;
u16  dbp_buf[length];
long  dbp_sumdat=0;

//Function to get average value from array
unsigned int Get_Average_Value(long sumdat,u16 *buf)  
{
		unsigned char i,j,k;
		unsigned int temp,avgValue;  
		unsigned long int mindat=0,maxdat=0;
		
	//Sort array in ascending order
		for(j=0;j<length;j++)    
		{
			for(k=0;k<length-j;k++)
			{
				if(buf[k] > buf[k+1])
				{
					temp = buf[k+1];
					buf[k+1] = buf[k];
					buf[k] = temp;
				}
			}
		} 

		// Sum  Excluding the max data
		for(i=(unsigned int)(length-max_len);i<length;i++)
		{
			maxdat = maxdat + buf[i];
		}
		
		//Calculate average
		avgValue = ((unsigned int)(sumdat - mindat - maxdat)/(unsigned int)(length - min_len - max_len));
		return avgValue;
}

void Get_JFC103_Data(void)  // Function to read data from JFC103 sensor
{
    static u8 ZeroCount1 = NUM;  // Counter for consecutive zero values in the heart rate data
    static u8 ZeroCount3 = NUM;  // Counter for consecutive zero values in the blood pressure data
    static u8 count1 = 0, count2 = 0;  // Counters for the arrays used to calculate averages

    // Check if USART data reception is successful
    if (USART_RX_FLAG == 1)           
    {
        // Check if the received data starts with 0xFF
        if (STM32_RX1_BUF[0] == 0xFF)
        {
            // Heart rate data processing
            if (STM32_RX1_BUF[65] == 0)
            {
                if (ZeroCount1 > 0) ZeroCount1--;  // Decrement the zero counter
                else heartrate = STM32_RX1_BUF[65];  // Get heart rate data
            }
            else
            {
                if (STM32_RX1_BUF[65] > 50)
                    heartrate = STM32_RX1_BUF[65];
                ZeroCount1 = NUM;  // Reset the zero counter
            }

            // Blood pressure data processing
            if (STM32_RX1_BUF[71] == 0 && STM32_RX1_BUF[72] == 0)
            {
                if (ZeroCount3 > 0) ZeroCount3--;  // Decrement the zero counter
                else
                {
                    SBP = STM32_RX1_BUF[71];  // Get systolic blood pressure data
                    DBP = STM32_RX1_BUF[72];  // Get diastolic blood pressure data
                }
            }
            else
            {
                ZeroCount3 = length;  // Reset the zero counter

                // Adjusting abnormal systolic blood pressure data
                if (STM32_RX1_BUF[71] > sbp_init_data)
                {
                    SBP = sbp_init_data - (STM32_RX1_BUF[71] - sbp_init_data);
                }
                else
                {
                    // Store and process systolic blood pressure data
                    sbp_buf[count1] = STM32_RX1_BUF[71];
                    sbp_sumdat = sbp_sumdat + sbp_buf[count1];
                    count1++;

                    // Calculate average when the array is full
                    if (count1 >= length)
                    {
                        count1 = 0;
                        SBP = Get_Average_Value(sbp_sumdat, sbp_buf);
                        sbp_sumdat = 0;
                    }
                }

                // Adjusting abnormal diastolic blood pressure data
                if (STM32_RX1_BUF[72] > dbp_init_data)
                {
                    DBP = dbp_init_data - (STM32_RX1_BUF[72] - dbp_init_data);
                }
                else
                {
                    // Store and process diastolic blood pressure data
                    dbp_buf[count2] = STM32_RX1_BUF[72];
                    dbp_sumdat = dbp_sumdat + dbp_buf[count2];
                    count2++;

                    // Calculate average when the array is full
                    if (count2 >= length)
                    {
                        count2 = 0;
                        DBP = Get_Average_Value(dbp_sumdat, dbp_buf);
                        dbp_sumdat = 0;
                    }
                }
            }
        }
        USART_RX_FLAG = 0;  // Reset the USART reception flag
    }
}


void displaySetValue(void)
{
    switch (setn)
    {
    case (1):
        // Display heart rate minimum and maximum
        sprintf(display, "%03d", (int)heartrate_min);
        LCD_Write_String(4, 1, (u8 *)display); // Display heart rate minimum

        sprintf(display, "%03d", (int)heartrate_max);
        LCD_Write_String(13, 1, (u8 *)display); // Display heart rate maximum

        LCD_Write_Com(0x80 + 0x40 + 6);
        LCD_Write_Com(0x0F);
        break;

    case (2):
        // Display heart rate minimum and maximum
        sprintf(display, "%03d", (int)heartrate_min);
        LCD_Write_String(4, 1, (u8 *)display); // Display heart rate minimum

        sprintf(display, "%03d", (int)heartrate_max);
        LCD_Write_String(13, 1, (u8 *)display); // Display heart rate maximum

        LCD_Write_Com(0x80 + 0x40 + 15);
        LCD_Write_Com(0x0F);
        break;

    case (3):
        // Display temperature minimum and maximum
        sprintf(display, "%4.1f", temperature_min);
        LCD_Write_String(5, 0, (u8 *)display); // Display temperature minimum

        sprintf(display, "%4.1f", temperature_max);
        LCD_Write_String(5, 1, (u8 *)display); // Display temperature maximum

        LCD_Write_Com(0x80 + 8);
        LCD_Write_Com(0x0F);
        break;

    case (4):
        // Display temperature minimum and maximum
        sprintf(display, "%4.1f", temperature_min);
        LCD_Write_String(5, 0, (u8 *)display); // Display temperature minimum

        sprintf(display, "%4.1f", temperature_max);
        LCD_Write_String(5, 1, (u8 *)display); // Display temperature maximum

        LCD_Write_Com(0x80 + 0x40 + 8);
        LCD_Write_Com(0x0F);
        break;

    case (5):
        // Display low blood pressure minimum and maximum
        sprintf(display, "%03d/%03dmmHg", (int)SBP_Min, (int)DBP_Min);
        LCD_Write_String(3, 1, (u8 *)display); // Display low blood pressure

        LCD_Write_Com(0x80 + 0x40 + 5);
        LCD_Write_Com(0x0F);
        break;

    case (6):
        // Display low blood pressure minimum and maximum
        sprintf(display, "%03d/%03dmmHg", (int)SBP_Min, (int)DBP_Min);
        LCD_Write_String(3, 1, (u8 *)display); // Display low blood pressure

        LCD_Write_Com(0x80 + 0x40 + 9);
        LCD_Write_Com(0x0F);
        break;

    case (7):
        // Display high blood pressure minimum and maximum
        sprintf(display, "%03d/%03dmmHg", (int)SBP_Max, (int)DBP_Max);
        LCD_Write_String(3, 1, (u8 *)display); // Display high blood pressure

        LCD_Write_Com(0x80 + 0x40 + 5);
        LCD_Write_Com(0x0F);
        break;

    case (8):
        // Display high blood pressure minimum and maximum
        sprintf(display, "%03d/%03dmmHg", (int)SBP_Max, (int)DBP_Max);
        LCD_Write_String(3, 1, (u8 *)display); // Display high blood pressure

        LCD_Write_Com(0x80 + 0x40 + 9);
        LCD_Write_Com(0x0F);
        break;
    }
}

void keyscan(void)
{
    // Check if key1 is pressed
    if (key1 == 0)
    {
        delay_ms(10); // Debounce
        // Check if key1 is still pressed
        if (key1 == 0)
        {
            while (key1 == 0); // Wait for key1 to be released

            beep = 0; // Disable buzzer

            setn++; // Record the number of times key1 is pressed

            // Handle different settings based on the number of key presses
            if (setn == 1) // Set heart rate minimum
            {
                printf("%c", 0x88); // Enter the setting and disable display
                LCD_Write_String(0, 0, "  Set Heartrate "); // Display text
                LCD_Write_String(0, 1, "min:     max:   ");
            }
            if (setn == 2) // Set heart rate maximum
            {
                LCD_Write_String(0, 0, "min:            ");
                LCD_Write_String(0, 1, "max:            ");
                LCD_Write_Char(9, 0, 0xDF);
                LCD_Write_Char(10, 0, 'C');
                LCD_Write_Char(9, 1, 0xDF);
                LCD_Write_Char(10, 1, 'C');
            }
            if (setn == 3) // Set temperature minimum
            {
                LCD_Write_String(0, 0, "min:            ");
                LCD_Write_String(0, 1, "max:            ");
                LCD_Write_Char(9, 0, 0xDF);
                LCD_Write_Char(10, 0, 'C');
                LCD_Write_Char(9, 1, 0xDF);
                LCD_Write_Char(10, 1, 'C');
            }
            if (setn == 4) // Set temperature maximum
            {
                LCD_Write_String(0, 0, "min:            ");
                LCD_Write_String(0, 1, "max:            ");
                LCD_Write_Char(9, 0, 0xDF);
                LCD_Write_Char(10, 0, 'C');
                LCD_Write_Char(9, 1, 0xDF);
                LCD_Write_Char(10, 1, 'C');
            }
            if (setn == 5) // Set low blood pressure minimum
            {
                LCD_Write_String(0, 0, " Set Hypotension");
                LCD_Write_String(0, 1, "                ");
            }
            if (setn == 6) // Set low blood pressure maximum
            {
                LCD_Write_String(0, 0, " Set Hypotension");
                LCD_Write_String(0, 1, "                ");
            }
            if (setn == 7) // Set high blood pressure minimum
            {
                LCD_Write_String(0, 0, "Set Hypertension");
                LCD_Write_String(0, 1, "                ");
            }
            if (setn == 8) // Set high blood pressure maximum
            {
                LCD_Write_String(0, 0, "Set Hypertension");
                LCD_Write_String(0, 1, "                ");
            }

            displaySetValue(); // Display the current setting value

            // Exit settings when setn exceeds a certain value
            if (setn >= 9)
            {
                setn = 0;
                LCD_Write_Com(0x0C); // Reset display
                LCD_Clear(); // Clear the screen
                printf("%c", 0x8A); // Send the command to display data

                /* Display the temperature unit */
                LCD_Write_Char(14, 0, 0xDF);
                LCD_Write_Char(15, 0, 'C');
            }
        }
    }

    // Check if key2 is pressed
    if (key2 == 0)
    {
        delay_ms(100);
        // Check if key2 is still pressed
        if (key2 == 0)
        {
            // Adjust settings based on the current setting number
            if (setn == 1) // Increment heart rate minimum
            {
                if (heartrate_min < heartrate_max) heartrate_min++;
            }
            if (setn == 2) // Increment heart rate maximum
            {
                if (heartrate_max < 200) heartrate_max++;
            }
            if (setn == 3) // Increment temperature minimum
            {
                if (temperature_min < temperature_max) temperature_min += 0.1;
            }
            if (setn == 4) // Increment temperature maximum
            {
                if (temperature_max < 99.9) temperature_max += 0.1;
            }
            if (setn == 5) // Increment low blood pressure minimum
            {
                if (SBP_Min < SBP_Max) SBP_Min++;
            }
            if (setn == 6) // Increment low blood pressure maximum
            {
                if (DBP_Min < DBP_Max) DBP_Min++;
            }
            if (setn == 7) // Increment high blood pressure minimum
            {
                if (SBP_Max < 300) SBP_Max++;
            }
            if (setn == 8) // Increment high blood pressure maximum
            {
                if (DBP_Max < SBP_Max) DBP_Max++;
            }

            displaySetValue(); // Display the current setting value
        }
    }

    // Check if key3 is pressed
    if (key3 == 0)
    {
        delay_ms(100);
        // Check if key3 is still pressed
        if (key3 == 0)
        {
            // Handle different settings based on the current setting number
            if (setn == 0) // Display data
            {
                printf("%c", 0x8A); // Send the command to display data
            }
            if (setn == 1) // Decrement heart rate minimum
            {
                if (heartrate_min > 0) heartrate_min--;
            }
            if (setn == 2) // Decrement heart rate maximum
            {
                if (heartrate_max > heartrate_min) heartrate_max--;
            }
            if (setn == 3) // Decrement temperature minimum
            {
                if (temperature_min > 0.0) temperature_min -= 0.1;
            }
            if (setn == 4) // Decrement temperature maximum
            {
                if (temperature_max > temperature_min) temperature_max -= 0.1;
            }
            if (setn == 5) // Decrement low blood pressure minimum
            {
                if (SBP_Min > 0) SBP_Min--;
            }
            if (setn == 6) // Decrement low blood pressure maximum
            {
                if (DBP_Min > 0) DBP_Min--;
            }
            if (setn == 7) // Decrement high blood pressure minimum
            {
                if (SBP_Max > SBP_Min) SBP_Max--;
            }
            if (setn == 8) // Decrement high blood pressure maximum
            {
                if (DBP_Max > DBP_Min) DBP_Max--;
            }

            displaySetValue(); // Display the current setting value
        }
    }
}

int main(void)
{
    // Variables for time counting and data buffer
    u16 timeCount = 300;
    u16 timeCount2 = 0;
    char SEND_BUF[100];

    // Flags for display flashing and blood pressure status
    bool flashing = 0;
    bool hypotension = 0;    // Low blood pressure flag
    bool hypertension = 0;   // High blood pressure flag

    // Initialization functions
    delay_init();         // Delay function initialization
    NVIC_Configuration();  // NVIC (Nested Vector Interrupt Controller) configuration
    delay_ms(500);         // Delay for stability after power-on
    LCD_Init();            // LCD initialization
    DS18B20_GPIO_Init();   // DS18B20 GPIO initialization
    uart1_Init(38400);     // UART1 initialization
    USART3_Init(9600);     // USART3 initialization
    LCD_Write_String(0, 0, "   loading...   "); // Display loading message
    LCD_Write_String(0, 1, "                ");
    memset(STM32_RX1_BUF, 0, STM32_RX1BUFF_SIZE); // Clear receive buffer
    STM32_Rx1Counter = 0;
    printf("%c", 0x8A); // Send the command to display data
    LCD_Clear();
    KEY_GPIO_Init();    // Key GPIO initialization
    DS18B20_Init();     // DS18B20 initialization

    /* Display the temperature unit */
    LCD_Write_Char(14, 0, 0xDF);
    LCD_Write_Char(15, 0, 'C');

    memset(STM32_RX1_BUF, 0, STM32_RX1BUFF_SIZE); // Clear receive buffer
    STM32_Rx1Counter = 0;

    while (1)
    {
        keyscan(); // Handle key press events

        if (setn == 0)
        {
            // Update data and display every 30ms
            if (timeCount++ >= 30)
            {
                timeCount = 0;

                flashing = !flashing; // Toggle flashing flag

                Get_JFC103_Data();     // Get data from sensors

                temperature = ReadTemperature(); // Read temperature from DS18B20 sensor

                // Display heart rate only if it's not in the specified range and the flashing flag is set
                if ((heartrate != 0 && (heartrate >= heartrate_max || heartrate <= heartrate_min)) && flashing)
                {
                    LCD_Write_String(0, 0, "        ");
                }
                else
                {
                    sprintf(display, "Hear:%03d", heartrate);
                    LCD_Write_String(0, 0, (u8 *)display); // Display heart rate
                }

                // Set hypertension flag if blood pressure is in the specified range
                if ((SBP != 0 && DBP != 0 && (SBP >= SBP_Max || DBP >= DBP_Max)))
                    hypertension = 1;
                else
                    hypertension = 0;

                // Set hypotension flag if blood pressure is in the specified range
                if ((SBP != 0 && DBP != 0 && (SBP <= SBP_Min || DBP <= DBP_Min)))
                    hypotension = 1;
                else
                    hypotension = 0;

                // Display blood pressure only if it's not in the specified range and the flashing flag is set
                if ((hypotension || hypertension) && flashing)
                {
                    LCD_Write_String(0, 1, "            ");
                }
                else
                {
                    sprintf(display, "%03d/%03dmmHg", (int)SBP, (int)DBP);
                    LCD_Write_String(0, 1, (u8 *)display); // Display blood pressure
                }

                // Display temperature only if it's not in the specified range and the flashing flag is set
                if ((temperature >= temperature_max || temperature <= temperature_min) && flashing)
                {
                    LCD_Write_String(10, 0, "    ");
                }
                else
                {
                    sprintf(display, "%4.1f", temperature);
                    LCD_Write_String(10, 0, (u8 *)display); // Display temperature
                    LCD_Write_Char(14, 0, 0xDF);
                    LCD_Write_Char(15, 0, 'C');
                }

                // Trigger the buzzer if any of the vital signs are out of range
                if ((heartrate != 0 && (heartrate >= heartrate_max || heartrate <= heartrate_min)) ||
                    (temperature >= temperature_max || temperature <= temperature_min) ||
                    (hypotension || hypertension))
                {
                    beep = ~beep; // Toggle the buzzer
                }
                else
                {
                    beep = 0; // Turn off the buzzer
                }
            }
        }

        timeCount2++;
        if (timeCount2 >= 80)
        {
            timeCount2 = 0;
            sprintf(SEND_BUF, "Heart RateÊ:%d, TemperatureÈ:%4.1f¡æ, Blood Pressure¹:%d/%dmmHg\r\n", heartrate, temperature, SBP, DBP);
            Uart3_SendStr(SEND_BUF); // Send vital signs data through UART3
        }

        delay_ms(10); // Delay for stability
    }
}


