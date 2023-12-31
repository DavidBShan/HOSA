#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"

#define key1 PBin(12)
#define key2 PBin(13)
#define key3 PBin(14)

#define beep   PBout(6)

void KEY_GPIO_Init(void);//Òý½Å³õÊ¼»¯

#endif
