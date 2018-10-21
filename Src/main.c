
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

	//Default setup board and cross borad Address IC = 0x20, cross board.
	//Address IC TCA9554 = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00); // Set pin as output, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x20
	//Address IC TCA9554 = 0x26
	//I2C_write_reg_TCA9554(I2C1 , 0x26, 0x03, 0x00); // Set pin as output, Address IC = 0x26
	//I2C_write_reg_TCA9554(I2C1 , 0x26, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x26


	//Default setup temperature senser TMP75 
	I2C_write_reg_8bit_TMP75(I2C1, 0x48, 0x01, 0x60); //Configuration registr addr=0x01 TMP75: OS R1R0 F1F0 POL TM SD
												  //                             OS=0 
												  //							 R1R0 = 00 00
												  //							 F1F0 = 00 00
												  //							 POL = 00 
												  //							 TM = 00 
												  //							 SD = 00 	
	I2C_read_reg_8bit_TMP75(I2C1, 0x48, 0x01);
												




}


