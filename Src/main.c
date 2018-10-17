
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SetupPeriph.h"

#include  <stdio.h>

/****************************TODO*************************

**********************************************************/
//LL_mDelay(1);
//LL_RCC_ClocksTypeDef check_RCC_Clocks,  *CHECK_RCC_CLOCKS=&check_RCC_Clocks; // Only for check setup clock. Not need use in release


int main(void){

	LL_Init();
	SystemClock_Config(); //Setup system clock at 80 MHz
	USART1_Init();
	USART3_Init();




}


