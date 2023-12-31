#include <stm32f10x.h>
#include <stdio.h>
#include "usart1.h"
#include <stdbool.h>

unsigned char Usart1RecBuf[USART1_RXBUFF_SIZE];  // Buffer for USART1 received data
unsigned int  RxCounter = 0;                    // Counter for received data

bool USART_RX_FLAG = 0;  // Flag to indicate whether USART has received data

// Function to initialize USART1
void uart1_Init(u32 bound)
{
    // GPIO Configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Enable clock for USART1, GPIOA, and AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    USART_DeInit(USART1);  // Deinitialize USART1

    // Configure USART1_TX (PA.9) as push-pull output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART1_RX (PA.10) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1 NVIC Configuration
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // USART Configuration
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // Enable USART1 RX interrupt
    USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  // Disable USART1 idle interrupt
    USART_Cmd(USART1, ENABLE);  // Enable USART1
}

// USART1 IRQ Handler
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  // USART receive interrupt
    {
        Usart1RecBuf[RxCounter] = USART_ReceiveData(USART1);
        RxCounter++;

        if (RxCounter > (USART1_RXBUFF_SIZE - 1))
        {
            USART_RX_FLAG = 1;  // Set receive flag
            RxCounter = 0;      // Reset counter
        }
    }

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // Clear USART1 RXNE interrupt flag
}

// Function to send a string through USART1
void uart1_SendStr(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART1->SR & 0X40) == 0);  // Wait for transmission to complete
        USART1->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

// Function to send data through USART1
void uart1_send(unsigned char *bufs, unsigned char len)
{
    if (len != 0xFF)
    {
        while (len--)
        {
            while ((USART1->SR & 0X40) == 0);  // Wait for transmission to complete
            USART1->DR = (u8)*bufs;
            bufs++;
        }
    }
    else
    {
        for (; *bufs != 0; bufs++)
        {
            while ((USART1->SR & 0X40) == 0);  // Wait for transmission to complete
            USART1->DR = (u8)*bufs;
        }
    }
}
