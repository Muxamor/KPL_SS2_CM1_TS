
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"

#include  <stdio.h>

/****************************TODO*************************
1. Check first front signal I2C on the board. Speed = 400kHz 
**********************************************************/
//LL_mDelay(1);
//LL_RCC_ClocksTypeDef check_RCC_Clocks,  *CHECK_RCC_CLOCKS=&check_RCC_Clocks; // Only for check setup clock. Not need use in release


int main(void){

	LL_Init();
	SystemClock_Config(); //Setup system clock at 80 MHz
	USART1_Init();
	USART3_Init();
	I2C1_Init();
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00);
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0x00);




}


