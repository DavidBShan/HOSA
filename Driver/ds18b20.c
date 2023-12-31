#include "ds18b20.h"
#include "delay.h"

/*******************************************************************************
Function Name: DS18B20_GPIO_Init
Description: Initialize GPIO settings for DS18B20
Inputs: None
Outputs: None
Returns: None
Note: None
*******************************************************************************/
void DS18B20_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable the clock for the GPIO port used by DS18B20
    RCC_APB2PeriphClockCmd(RCC_DS18B20_PORT, ENABLE);

    // Configure the GPIO pin for DS18B20 as output push-pull
    GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);

    // Set the initial state of the DS18B20 pin to HIGH (1)
    GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN);
}

/*******************************************************************************
Function Name: DS18B20_Init
Description: Initialize the DS18B20 sensor
Inputs: None
Outputs: None
Returns: 0 if initialization is successful, 1 otherwise
Note: None
*******************************************************************************/
u8 DS18B20_Init(void)
{
    unsigned char wait = 0;

    DS18B20_IO_OUT();    // Set DS18B20 pin as output
    DS18B20_OUT_0;       // Pull the pin low
    delay_us(750);       // Delay for 480us (reset pulse duration)
    DS18B20_OUT_1;       // Release the pin
    delay_us(15);        // Wait for 15us
    DS18B20_IO_IN();     // Set DS18B20 pin as input

    // Wait for the DS18B20 to pull the line low (indicating its presence)
    while (READ_DS18B20_IO && wait++ < 200)
        delay_us(1);

    if (wait >= 200)
        return 1;  // Initialization unsuccessful

    else
        wait = 0;

    // Wait for the DS18B20 to release the line (indicating it is ready for communication)
    while (!READ_DS18B20_IO && wait++ < 240)
        delay_us(1);

    if (wait >= 240)
        return 1;  // Initialization unsuccessful

    else
        return 0;  // Initialization successful
}

/*******************************************************************************
Function Name: DS18B20_ReadByte
Description: Read one byte of data from DS18B20
Inputs: None
Outputs: None
Returns: The read byte
Note: None
*******************************************************************************/
unsigned char DS18B20_ReadByte(void)
{
    unsigned char i;
    unsigned char dat = 0;

    for (i = 0; i < 8; i++)
    {
        dat >>= 1;                // Right shift the data byte
        DS18B20_IO_OUT();         // Set DS18B20 pin as output
        DS18B20_OUT_0;            // Pull the pin low
        delay_us(2);             
        DS18B20_OUT_1;            // Release the pin
        DS18B20_IO_IN();          // Set DS18B20 pin as input
        delay_us(12);         

        if (READ_DS18B20_IO)
            dat |= 0x80;  // Set the MSB if the line is high
        delay_us(60);       // Wait for 60 microseconds (complete the bit duration)
    }

    return dat;  // Return the read byte
}

/*******************************************************************************
Function Name: DS18B20_WriteByte
Description: Write one byte of data to DS18B20
Inputs: unsigned char dat - The byte of data to be written
Outputs: None
Returns: None
Note: None
*******************************************************************************/
void DS18B20_WriteByte(unsigned char dat)
{
    unsigned char i;
    unsigned char temp;

    DS18B20_IO_OUT();  // Set DS18B20 pin as output

    for (i = 1; i <= 8; i++)
    {
        temp = dat & 0x01;  // Extract the LSB
        dat = dat >> 1;     // Right shift the data byte

        if (temp)
        {
            DS18B20_OUT_0;  // Pull the pin low
            delay_us(2);
            DS18B20_OUT_1;  // Release the pin (write 1)
            delay_us(60);
        }
        else
        {
            DS18B20_OUT_0;  // Pull the pin low (write 0)
            delay_us(60);
            DS18B20_OUT_1;  // Release the pin
            delay_us(2);
        }
    }
}

/*******************************************************************************
Function Name: ReadTemperature
Description: Read temperature from DS18B20 sensor
Inputs: None
Outputs: None
Returns: The read temperature in degrees Celsius (float)
Note: None
*******************************************************************************/
float ReadTemperature(void)
{
    unsigned char TPH;      // High byte of temperature
    unsigned char TPL;      // Low byte of temperature
    short i16 = 0;
    float f32 = 0;

    // Initialize the DS18B20 sensor
    DS18B20_Init();

    // Send the command to start temperature conversion
    DS18B20_WriteByte(0xCC);  // Skip ROM command
    DS18B20_WriteByte(0x44);  // Convert temperature command

    // Initialize the DS18B20 sensor again
    DS18B20_Init();

    // Send the command to read temperature
    DS18B20_WriteByte(0xCC);  // Skip ROM command
    DS18B20_WriteByte(0xBE);  // Read scratchpad command

    // Read the low and high bytes of temperature
    TPL = DS18B20_ReadByte();  // Read the low byte
    TPH = DS18B20_ReadByte();  // Read the high byte

    // Combine the high and low bytes
    i16 = 0;
    i16 = (TPH << 8) | TPL;

    // Convert the raw data to temperature (12-bit resolution)
    f32 = i16 * 0.0625;

    return f32;  // Return the temperature in degrees Celsius (float)
}
