/*
 * control_module.c
 *
 *  Created on: Oct 21, 2018
 *      Author: muxamor
 */


#include "stm32l4xx.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_gpio.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "control_module.h"

#include  <stdio.h>


/**
  * @brief  Default setup control module 
  * @param  void
  * @retval void
  */

void Default_Setup_CM(void){

	Disable_IO0_global_clock();
	Set_Output_mode_D0_D7(); 		
	Set_Output_mode_D8_D15();   

	Reset_CLK300();
	Reset_CLK302_1();		
	Reset_CLK302_2();
	Reset_CLK302_3();
	Reset_CLK304();
	Set_EN304();
	Set_RST304();
	Reset_CLK306();



	//Default setup board and cross borad Address IC = 0x20, cross board.
	//Address IC TCA9554 = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00); // Set pin as output, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x20
	//Address IC TCA9554 = 0x26
	I2C_write_reg_TCA9554(I2C1 , 0x26, 0x03, 0x00); // Set pin as output, Address IC = 0x26
	I2C_write_reg_TCA9554(I2C1 , 0x26, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x26

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