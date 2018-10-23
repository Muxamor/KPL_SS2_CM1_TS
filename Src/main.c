
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"

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
	//USART3_Init();
///	I2C1_Init();


///Only FOR TESTE!!!!!!!!!!!!!!!!!!!!!!!!!

	//I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00); // Set pin as output, Address IC = 0x20
	//I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0x00); // OFF all analog module in block, Address IC = 0x20

	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13);
	uint16_t massiv[4];

//while(1){
	LL_mDelay(40000);
	massiv[0] = 0x0000;
	massiv[1] = 0x00FF;
	massiv[2] = 0x00BB;
	massiv[3] = 0x00CC;

	Data_transmite_UART_9B (massiv, USART1);
//}
	while(1);

////////////////////////////////////////////////////////////////////////////////////////////////////





	//I2C_write_reg_DS3232(I2C1, 0x68, 03, 5);
 	//I2C_read_reg_DS3232(I2C1, 0x68, 03);

	//Default setup board and cross borad Address IC = 0x20, cross board.
	//Address IC TCA9554 = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00); // Set pin as output, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x20
	//Address IC TCA9554 = 0x26
	//I2C_write_reg_TCA9554(I2C1 , 0x26, 0x03, 0x00); // Set pin as output, Address IC = 0x26
	//I2C_write_reg_TCA9554(I2C1 , 0x26, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x26

	//Default setup temperature senser TMP75
	uint8_t add_TMP75 = 0x48;
	for(/*empty*/; add_TMP75 < 0x4E; add_TMP75++){ // Setup all temperatura IC TMP75 Block 1
		
		I2C_write_reg_8bit_TMP75(I2C1, add_TMP75, 0x01, 0x00); //Configuration registr addr=0x01 TMP75: OS R1R0 F1F0 POL TM SD
													           // OS=0   R1R0 = 00 00   F1F0 = 00 00   POL = 00   TM = 00  SD = 00
		//Write Tlow registr addr=0x02 Tlow=30 in 12bit resolution = 0x1E0
		I2C_write_reg_16bit_TMP75(I2C1, add_TMP75, 0x02, (0x1E0 << 4) );

		//Write Thigh registr addr=0x03 Thigh=40 in 12bit resolution = 0x280
		I2C_write_reg_16bit_TMP75(I2C1, add_TMP75, 0x03, (0x280 << 4) );
	}





}


