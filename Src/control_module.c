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
#include "uart_comm.h"
#include "control_module.h"


#include  <stdio.h>


/**
  * @brief  Default setup control module 
  * @param  void
  * @retval void
  */

void Default_Setup_CM( _REG_302 *reg302_ptr ){

	Disable_IO0_global_clock();

	Set_Output_mode_D0_D7(); 		
	Set_Output_mode_D8_D15();   

	Reset_CLK300();
	Reset_CLK302_1();		
	Reset_CLK302_2();
	Reset_CLK302_3();
	Reset_CLK304();
	Reset_EN304();
	Set_RST304();
	Reset_CLK306();

	//Default setup board and cross borad Address IC = 0x20, cross board.
	//Address IC TCA9554 = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x03, 0x00); // Set pin as output, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1 , 0x20, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x20
	//Address IC TCA9554 = 0x26
	I2C_write_reg_TCA9554(I2C1 , 0x26, 0x03, 0x00); // Set pin as output, Address IC = 0x26
	I2C_write_reg_TCA9554(I2C1 , 0x26, 0x01, 0xFF); // OFF all analog module in block, Address IC = 0x26
	////////////////////////////////////////

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
	////////////////////////////////////////


	//Check that  Block 2 is present
	uint16_t mass[4] = {0x06,0x00,0x00,0x00};
	uint32_t tmp = 0;

	Data_transmite_UART_9B (mass, 4,  USART3);
	tmp = Data_receive_UART_9B (4 , USART3);

	if(tmp == 0xFFFFFFFF){
		reg302_ptr->block2_ready = 0;
	}else{
		reg302_ptr->block2_ready = 1;
	}
	////////////////////////////////////////


	//Write default statu reg302
	Write_reg302_D0_D7 ( *(uint32_t *)reg302_ptr );

}

/**
  * @brief  Read reg ISA bus D0..D15
  * @param  void
  * @retval data D0..D15
  */
uint32_t Read_reg304_D0_D15( void ){

	uint32_t data_D0_D7=0, data_D8_D15=0;
	uint32_t data_D0_D15=0;

	//Change diraction ISA bus
	Set_Input_mode_D0_D7(); 		
	Set_Input_mode_D8_D15();
	Set_EN304();
	__NOP();

	data_D0_D7 = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7);

	data_D8_D15 = LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7);

	//Reset_EN304();
	//Set_Output_mode_D0_D7(); 		
	//Set_Output_mode_D8_D15();

	data_D0_D15 = ( data_D8_D15 << 8 ) | data_D0_D7;

	data_D0_D15 = data_D0_D15 & 0x0000FFFF; //clear MSB 

	return data_D0_D15;
}


/**
  * @brief  Write reg304 ISA bus D0..D15
  * @param  void
  * @retval data D0..D15
  */
void Write_reg304_D0_D15( uint32_t data_D0_D15 ){

	uint32_t data_D0_D7=0, data_D8_D15=0;
	uint32_t invert_data_D0_D7=0, invert_data_D8_D15=0;

	//Change diraction ISA bus
	Reset_EN304();
	Set_Output_mode_D0_D7(); 		
	Set_Output_mode_D8_D15();

	//write bite to PA0..PA7 
	data_D0_D7 = data_D0_D15 & 0x000000FF;
	invert_data_D0_D7 = ( ~ data_D0_D7 ) & 0x000000FF;
	WRITE_REG(GPIOA->BSRR,  ( ( invert_data_D0_D7 << 16 ) | data_D0_D7) ); //Set bits.  Maybe use BRR register to reset bits 
	

	data_D8_D15 =  (data_D0_D15 >> 8 ) & 0x000000FF;
	invert_data_D8_D15 = ( ~ data_D8_D15 ) & 0x000000FF;
	WRITE_REG(GPIOC->BSRR,  ( ( invert_data_D8_D15 << 16 ) | data_D8_D15) ); //Set bits. Maybe use BRR register to reset bits 

	 Pulse_CLK304();
}

/**
  * @brief  Write reg304 ISA bus D0..D7
  * @param  data_D0_D7
  * @retval void
  */
void Write_reg302_D0_D7 ( uint32_t data_reg302 ){

	
	uint32_t invert_data_D0_D7=0, data_D0_D7=0;

	//Change diraction ISA bus
	Reset_EN304();
	Set_Output_mode_D0_D7(); 		
	Set_Output_mode_D8_D15();

	//write bite to PA0..PA7 
	data_D0_D7 = data_reg302 & 0x000000FF;
	invert_data_D0_D7 = ( ~ data_D0_D7 ) & 0x000000FF;
	WRITE_REG(GPIOA->BSRR,  ( ( invert_data_D0_D7 << 16 ) | data_D0_D7) ); //Set bits.  Maybe use BRR register to reset bits 
	
	Pulse_CLK302_1();
}




/**
  * @brief  Get and parse command from ISA
  * @param  void
  * @retval void
  */
void Get_Parse_ISA_command (void){
/*
	uint16_t data1_D0_D15 = 0, data2_D0_D15 = 0, data3_D0_D15 = 0;
	uint8_t number_command =0;

	// Read data D0..D15
	data1_D0_D15 = (uint16_t) Read_data_D0_D15();
	
	// Parse and processing command  and prepare answer
	number_command  =  (uint8_t)(data_D0_D15 >> 8);


	switch(number_command){

		case 1:  //Switch ON power analog mod;
			Write_reg304_D0_D15( 0x01);
			Write_reg302_D0_D7 ( uint32_t data_reg302 )
			while(INTERRUPT)
			data2_D0_D15 = (uint16_t) Read_data_D0_D15();
			Write_reg304_D0_D15( 0x01);
			Write_reg302_D0_D7 ( uint32_t data_reg302 )
			while(INTERRUPT)
			data3_D0_D15 = (uint16_t) Read_data_D0_D15();
			Write_reg304_D0_D15( 0x01);
			Write_reg302_D0_D7 ( uint32_t data_reg302 )


		break;

		default:
		break;

	}
*/

}



