/*
 * control_module.c
 *
 *  Created on: Oct 21, 2018
 *      Author: muxamor
 */


#include "stm32l4xx.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_iwdg.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"
#include "command_ISA.h"
#include "control_module.h"


#include "global_variables.h"


#include  <stdio.h>


/**
  * @brief  Default setup control module 
  * @param  void
  * @retval void
  */

void Default_Setup_CM( _REG_302 *reg302_ptr ){

	Disable_IO0_global_clock();

	Set_Output_mode_D0_D15();

	Reset_CLK300();
	Reset_CLK302_1();		
	Reset_CLK302_2();
	Reset_CLK302_3();
	Reset_CLK304();
	Set_EN304();
	Set_RST304();
	Reset_CLK306();

	FLAG_interrupt_INT3 = 0; // 1 = Command is come 
	FLAG_interrupt_PULSE = 0; // 1 = Pulse (global clock) is come
	loop_counter = -1; // counter of package 
	counter_ADC_data_ready = 0; // counter of package 
	overwrite_fifo = 0; //1 =overwrite fifo flag

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

		//Write Tlow registr addr=0x02 Tlow=20 in 12bit resolution = 0x1E0
		//I2C_write_reg_16bit_TMP75(I2C1, add_TMP75, 0x02, (0x140 << 4) );

		//Write Thigh registr addr=0x03 Thigh=40 in 12bit resolution = 0x280
		I2C_write_reg_16bit_TMP75(I2C1, add_TMP75, 0x03, (0x280 << 4) );

		//Write Thigh registr addr=0x03 Thigh=25 in 12bit resolution = 0x280
		//I2C_write_reg_16bit_TMP75(I2C1, add_TMP75, 0x03, (0x190 << 4) );
	}
	////////////////////////////////////////

	//Check that  Block 2 is present
	uint16_t mass[4] = {0x0006,0x0000,0x0000,0x0000};
	uint32_t tmp = 0;


	tmp = Transfer_command_UART_9B( mass, 4, 4, USART3 );
	//Data_transmite_UART_9B (mass, 4,  USART3);
	//tmp = Data_receive_UART_9B (4 , USART3);
	if(tmp == 0xFFFFFFFF){
		reg302_ptr->block2_ready = 0;
	}else{
		reg302_ptr->block2_ready = 1;
	}

	//Write default statu reg302
	Write_reg302_D0_D7 ( *(uint32_t *)reg302_ptr, 1, 1, 0 );

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
	Set_Input_mode_D0_D15();
	__NOP();

	Reset_EN304();
	__NOP();
	__NOP();
	__NOP();

	///data_D0_D7 = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7);
	data_D0_D7 = LL_GPIO_ReadInputPort(GPIOA) & 0x000000FF;

	//data_D8_D15 = LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7);
	data_D8_D15 = LL_GPIO_ReadInputPort(GPIOC) & 0x000000FF;

	data_D0_D15 = ( data_D8_D15 << 8 ) | data_D0_D7;

	data_D0_D15 = data_D0_D15 & 0x0000FFFF; //clear MSB 

	Pulse_RST304();
	__NOP();

	Set_EN304();
	__NOP();

	Set_Output_mode_D0_D15();
	
	return data_D0_D15;
}


/**
  * @brief  Write reg304 ISA bus D0..D15
  * @param  void
  * @retval data D0..D15
  */
void Write_reg304_D0_D15( uint16_t data_D0_D15 ){

	uint32_t data_D0_D7=0, data_D8_D15=0;
	uint32_t invert_data_D0_D7=0, invert_data_D8_D15=0;

	//Change diraction ISA bus
	//Set_EN304();

	//Set_Output_mode_D0_D15();
	//__NOP();

	//write bite to PA0..PA7 

	data_D0_D7 = data_D0_D15 & 0x000000FF;
	invert_data_D0_D7 = ( ~ data_D0_D7 ) & 0x000000FF;
	WRITE_REG(GPIOA->BSRR,  ( ( invert_data_D0_D7 << 16 ) | data_D0_D7) ); //Set bits.  Maybe use BRR register to reset bits

	data_D8_D15 =  (data_D0_D15 >> 8 ) & 0x000000FF;
	invert_data_D8_D15 = ( ~ data_D8_D15 ) & 0x000000FF;
	WRITE_REG(GPIOC->BSRR,  ( ( invert_data_D8_D15 << 16 ) | data_D8_D15) ); //Set bits. Maybe use BRR register to reset bits 
	__NOP();
	__NOP();

	Pulse_CLK304();
}

/**
  * @brief  Write reg304 ISA bus D0..D7
  * @param  data_D0_D7
  * @retval void
  */
void Write_reg302_D0_D7 ( uint16_t data_reg302, uint8_t write_CLK302_1, uint8_t write_CLK302_2, uint8_t write_CLK302_3 ){

	uint32_t invert_data_D0_D7=0, data_D0_D7=0;

	//Change diraction ISA bus
	//Set_EN304();

	//Set_Output_mode_D0_D15();
	//__NOP();

	//write bite to PA0..PA7 
	data_D0_D7 = data_reg302 & 0x000000FF;
	invert_data_D0_D7 = ( ~ data_D0_D7 ) & 0x000000FF;
	WRITE_REG(GPIOA->BSRR,  ( ( invert_data_D0_D7 << 16 ) | data_D0_D7) ); //Set bits.  Maybe use BRR register to reset bits
	__NOP();
	__NOP();
	
	if (write_CLK302_1 == 1 ){
		Pulse_CLK302_1();
	}

	if( write_CLK302_2 == 1 ){
		Pulse_CLK302_2();
	}

	if( write_CLK302_3 == 1 ){
		Pulse_CLK302_3();
	}
}


/**
  * @brief  Write reg300 ISA bus D0..D7
  * @param  data_D0_D15
  * @retval void
  */
void Write_reg300_D0_D15 (uint16_t data_D0_D15){

	uint32_t data_D0_D7=0, data_D8_D15=0;
	uint32_t invert_data_D0_D7=0, invert_data_D8_D15=0;

	//Change diraction ISA bus
	//Set_EN304();

	//Set_Output_mode_D0_D15();
	//__NOP();

	//write bite to PA0..PA7
	data_D0_D7 = data_D0_D15 & 0x000000FF;
	invert_data_D0_D7 = ( ~ data_D0_D7 ) & 0x000000FF;
	WRITE_REG(GPIOA->BSRR,  ( ( invert_data_D0_D7 << 16 ) | data_D0_D7) ); //Set bits.  Maybe use BRR register to reset bits

	data_D8_D15 =  (data_D0_D15 >> 8 ) & 0x000000FF;
	invert_data_D8_D15 = ( ~ data_D8_D15 ) & 0x000000FF;
	WRITE_REG(GPIOC->BSRR,  ( ( invert_data_D8_D15 << 16 ) | data_D8_D15) ); //Set bits. Maybe use BRR register to reset bits
	__NOP();
	__NOP();

	Pulse_CLK300();
}

/**
  * @brief  Wait interrupt_INT3
  * @param  data_D0_D15
  * @retval 0 or 1 = ERROR 
 */

uint8_t wait_interrupt_INT3(void){
	
	uint32_t counter=0;

	while( FLAG_interrupt_INT3==0 ){

		//LL_IWDG_ReloadCounter(IWDG);
		counter++;
		if(counter==1000000){
			return 1;
		}
	}

	FLAG_interrupt_INT3 = 0;

	return 0;
}



/**
  * @brief  Get and parse command from ISA
  * @param  void
  * @retval void
  */
void Get_Parse_ISA_command (_REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[], _STATUS_CONTROL_MODULE *stat_cont_mod ){

	uint16_t word1_D0_D15 = 0;
	uint8_t number_command = 0;
	

	// Read data D0..D15
	word1_D0_D15 = (uint16_t) Read_reg304_D0_D15();
	
	// Parse and processing command  and prepare answer
	number_command  =  (uint8_t)(word1_D0_D15 >> 8);

	if( stat_cont_mod->cm_state_start_stop == 1 && number_command == 9 ){ // Start mode

		 ISA_Command_900( word1_D0_D15, stat_cont_mod, reg302_ptr );
	}else if( stat_cont_mod->cm_state_start_stop == 0 ){ // Stop mode

		switch(number_command){

			case 0x01:  //Switch ON power analog mod;
				ISA_Command_100(reg302_ptr, analog_mod_config);
				break;	

			case 0x02:
				ISA_Command_200(reg302_ptr, analog_mod_config);
				break;

			case 0x04:
				ISA_Command_400(word1_D0_D15, reg302_ptr, analog_mod_config);
				break;

			case 0x05:
				ISA_Command_500(word1_D0_D15, reg302_ptr, analog_mod_config);
				break;

			case 0x06:
				ISA_Command_600(word1_D0_D15, reg302_ptr, analog_mod_config);
				break;

			case 0x07:
				ISA_Command_700(word1_D0_D15, reg302_ptr);
				break;

			case 0x08:
				ISA_Command_800();
				break;

			case 0x09: 
				ISA_Command_900(word1_D0_D15, stat_cont_mod, reg302_ptr);
				break;

			case 0x0A:
				ISA_Command_A00(word1_D0_D15, reg302_ptr);
				break;

			case 0x0B:
				ISA_Command_B00(word1_D0_D15);
				break;

			default:
				Error_Handler();
				Write_reg304_D0_D15(0x02);
				break;
		}
	}

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 (*(uint32_t*)reg302_ptr, 1, 0, 0);
}


uint8_t Write_FIFO( _FIFO  *FIFO_adc_data_ptr, _ADC_DATA_Package adc_package ){


	FIFO_adc_data_ptr->FIFO_buf_ADC[FIFO_adc_data_ptr->FIFO_HEAD] = adc_package;

	FIFO_adc_data_ptr->FIFO_HEAD = FIFO_adc_data_ptr->FIFO_HEAD + 1;

	if( FIFO_adc_data_ptr->FIFO_HEAD == FIFO_SIZE ){ 
		FIFO_adc_data_ptr->FIFO_HEAD = 0;//New loop in FIFO
	}

	FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO = FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO + 1;

	//Overwrite FIFO buffer
	if(FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO > FIFO_SIZE){
		FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO = FIFO_SIZE;
		
		FIFO_adc_data_ptr->FIFO_TAIL = FIFO_adc_data_ptr->FIFO_TAIL + 1; //FIFO lost data
		if( FIFO_adc_data_ptr->FIFO_TAIL == FIFO_SIZE ){
			FIFO_adc_data_ptr->FIFO_TAIL = 0; // New loop in FIFO
		}
		Error_Handler();

		return 1;  // Error was overwrite buffer
	}

	return 0;
}


_ADC_DATA_Package Read_FIFO( _FIFO  *FIFO_adc_data_ptr ){

	_ADC_DATA_Package ret = { 
							.head_byte = 0xFF, 
 							.cyclic_code = 0xFF, 
 							.ADC_data_byte_MSB = 0xFF,
 							.ADC_data_byte_LSB = 0xFF,
	};

	if( FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO != 0 ){

	 	ret = FIFO_adc_data_ptr->FIFO_buf_ADC[FIFO_adc_data_ptr->FIFO_TAIL];

		FIFO_adc_data_ptr->FIFO_TAIL = FIFO_adc_data_ptr->FIFO_TAIL + 1;

	 	if(FIFO_adc_data_ptr->FIFO_TAIL == FIFO_SIZE ){ 
			FIFO_adc_data_ptr->FIFO_TAIL = 0;//New loop in FIFO
		}

		FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO = FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO - 1;
	}

	return ret;
}


void Clear_FIFO( _FIFO  *FIFO_adc_data_ptr ){

	FIFO_adc_data_ptr->FIFO_HEAD = 0;
	FIFO_adc_data_ptr->COUNT_DATA_IN_FIFO = 0;
	FIFO_adc_data_ptr->FIFO_TAIL = 0; 
}








