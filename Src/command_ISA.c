
#include "stm32l4xx.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_exti.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"
#include "control_module.h"
#include "command_ISA.h"

#include "global_variables.h"

#include  <stdio.h>

ErrorStatus ISA_Command_100( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint16_t word2_D0_D15 = 0, word3_D0_D15 = 0;
	uint8_t  i=0;
	ErrorStatus ret1, ret2;
	uint16_t mass[4];
	uint32_t tmp=0;
	
	//Answer at the 1 word command 100
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr, 1, 1, 0 );

	if( wait_interrupt_INT3() ){
		Error_Handler();
		return ERROR;
	}

	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	ret1 = I2C_write_reg_TCA9554(I2C1, 0x20, 0x01, (~((uint8_t)word2_D0_D15) ) ); // ON/OFF analog module in block1, Address IC = 0x20
	ret2 = I2C_write_reg_TCA9554(I2C1, 0x26, 0x01, (~( (uint8_t)(word2_D0_D15>>8) ) ) ); // ON/OFF analog module in block1, Address IC = 0x26
			
	if(ret1==ERROR || ret2==ERROR){				
		word2_D0_D15 = 0x0000; // //for write 0x00 to analog_mod_config[i].power_module_on 
		Write_reg304_D0_D15( 0x02);
	}else{
		Write_reg304_D0_D15( 0x01);
	}

	for(i=0; i<16; i++){
		analog_mod_config[i].power_module_on = (uint8_t)(( word2_D0_D15 >> i ) & 0x0001);
	}

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr, 1, 1, 0 );

	if( wait_interrupt_INT3() ){
		Error_Handler();
		return ERROR;
	}

	word3_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	mass[0] = 0x0006;
	mass[1] = 0x0000;
	mass[2] = (word3_D0_D15>>8) & 0x00FF;
	mass[3] = word3_D0_D15 & 0x00FF;
	Data_transmite_UART_9B ( mass , 4,  USART3);
	tmp = Data_receive_UART_9B (4 , USART3);

	if(tmp == 0xFFFFFFFF || ((uint8_t)(tmp >> 16)) != 0x01 ){
		reg302_ptr->block2_ready = 0;
		word3_D0_D15 = 0x0000; //for write 0x00 to analog_mod_config[i].power_module_on 
		Write_reg304_D0_D15( 0x02);
	}else{
		reg302_ptr->block2_ready = 1;
		Write_reg304_D0_D15( 0x01);
	}

	for(i=16; i<32; i++){ //Save setting analog module
		analog_mod_config[i].power_module_on = (uint8_t)(( word3_D0_D15 >> i ) & 0x0001);
	}

	return SUCCESS; 
}


ErrorStatus ISA_Command_200( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t  i=0;
	uint16_t mass[4];
	uint32_t tmp=0;
	
	I2C_write_reg_TCA9554(I2C1, 0x20, 0x01, 0xFF); // ON/OFF analog module in block1, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1, 0x26, 0x01,  0xFF); // ON/OFF analog module in block1, Address IC = 0x26

	mass[0] = 0x0006;
	mass[1] = 0x0000;
	mass[2] = 0x0000;
	mass[3] = 0x0000;
	Data_transmite_UART_9B (mass , 4,  USART3);
	tmp = Data_receive_UART_9B (4 , USART3);

	if(tmp == 0xFFFFFFFF || ((uint8_t)(tmp >> 16)) != 0x01 ){
		reg302_ptr->block2_ready = 0;
	}

	for(i=0; i<32; i++){
		analog_mod_config[i].power_module_on = 0x00;
	}

	Write_reg304_D0_D15(0x01);

	return SUCCESS; 
}


ErrorStatus ISA_Command_400( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;

	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	addr_analog_mod = ((uint8_t)word1_D0_D15) >> 3;

	if(addr_analog_mod <= 15){
		USARTx = USART1;
	}else{
		USARTx = USART3;
	}

	mass[0] = (word1_D0_D15 & 0x00F8) | 0x0002; // Set K1 command
	mass[1] = word1_D0_D15 & 0x0003;
	mass[2] = 0x0000;
	mass[3] = 0x0000;

	Data_transmite_UART_9B (mass , 4,  USARTx);
	tmp = Data_receive_UART_9B (4 , USARTx);

	if(tmp == 0xFFFFFFFF || ((uint8_t)(tmp >> 16)) != 0x01){
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 0;
		}
		analog_mod_config[addr_analog_mod].amp_factor_K1 = 0x00;
		Write_reg304_D0_D15(0x02);

	}else{
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 1;
		}
		analog_mod_config[addr_analog_mod].amp_factor_K1 = (uint8_t)(word1_D0_D15 & 0x0003);
		Write_reg304_D0_D15(0x01);
	}

	return SUCCESS; 
}


ErrorStatus ISA_Command_500( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;
	uint16_t word2_D0_D15 = 0;
	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	//Answer at the 1 word command 500
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr, 1, 1, 0 );

	//Get and processing second word
	if( wait_interrupt_INT3() ){
		Error_Handler();
		return ERROR;
	}

	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	addr_analog_mod = ((uint8_t)word1_D0_D15) >> 3;

	if(addr_analog_mod <= 15){
		USARTx = USART1;
	}else{
		USARTx = USART3;
	}

	mass[0] = (word1_D0_D15 & 0x00F8) | 0x03; // Set K2 command

	switch (word2_D0_D15 & 0x0FFF){
		case 0xFFF:
			mass[1] = 0x00;
			break;

		case 0x400:
			mass[1] = 0x01;
			break;

		case 0x200:
			mass[1] = 0x02;
			break;

		case 0x100:
			mass[1] = 0x03;
			break;

		case 0x080:
			mass[1] = 0x04;
			break;

		case 0x040:
			mass[1] = 0x05;
			break;

		case 0x020:
			mass[1] = 0x06;
			break;

		case 0x010:
			mass[1] = 0x07;
			break;

		case 0x008:
			mass[1] = 0x08;
			break;

		case 0x004:
			mass[1] = 0x09;
			break;

		case 0x002:
			mass[1] = 0x0A;
			break;

		case 0x001:
			mass[1] = 0x0B;
			break;

		default:
			mass[1] = 0x00;
			break;
	}

	mass[2] = 0x0000;
	mass[3] = 0x0000;

	Data_transmite_UART_9B (mass , 4,  USARTx);
	tmp = Data_receive_UART_9B (4 , USARTx);

	if(tmp == 0xFFFFFFFF || ((uint8_t)(tmp >> 16)) != 0x01){
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 0;
		}
		analog_mod_config[addr_analog_mod].amp_factor_K2 = 0x00;
		Write_reg304_D0_D15(0x02);

	}else{
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 1;
		}
		analog_mod_config[addr_analog_mod].amp_factor_K2 =(uint8_t)mass[1];
		Write_reg304_D0_D15(0x01);
	}

	return SUCCESS; 
}


ErrorStatus ISA_Command_600( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;
	uint16_t word2_D0_D15 = 0;
	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	//Answer at the 1 word command 600
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr, 1, 1, 0 );

	if( wait_interrupt_INT3() ){
		Error_Handler();
		return ERROR;
	}

	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();
	addr_analog_mod =  ((uint8_t)word1_D0_D15) >> 3;

	if(addr_analog_mod <= 15){
		USARTx = USART1;
	}else{
		USARTx = USART3;
	}

	mass[0] = (word1_D0_D15 & 0x00F8) | 0x04; // Set Fcut command
	mass[1] = word2_D0_D15 & 0x00FF;
	mass[2] = 0x00;
	mass[3] = 0x00;

	Data_transmite_UART_9B(mass , 4,  USARTx);
	tmp = Data_receive_UART_9B(4 , USARTx);

	if(tmp == 0xFFFFFFFF || ((uint8_t)(tmp >> 16)) != 0x01){
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 0;
		}
		analog_mod_config[addr_analog_mod].Fcut_value = 0x01;
		Write_reg304_D0_D15(0x02);

	}else{
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 1;
		}
		analog_mod_config[addr_analog_mod].Fcut_value = (uint8_t)(word2_D0_D15 & 0X00FF);
		Write_reg304_D0_D15(0x01);
	}

	return SUCCESS; 
}


ErrorStatus ISA_Command_700( uint16_t word1_D0_D15, _REG_302 *reg302_ptr ){

	uint8_t addr_analog_mod=0;
	uint8_t number_request=0;

	uint16_t mass[4];
	uint16_t table_amp_factorK2[12] = {0xFFF, 0x400, 0x200, 0x100, 0x080, 0x040, 0x020, 0x010, 0x008, 0x004, 0x002, 0x001 }; 

	uint16_t answer_ISA_D15_D0=0x0000;
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	addr_analog_mod = ((uint8_t)word1_D0_D15) >> 3;
	number_request = ((uint8_t)word1_D0_D15) & 0x07;

	if(addr_analog_mod <= 15){
		USARTx = USART1;
	}else{
		USARTx = USART3;
	}

	switch(number_request ){

		case 0x01: //Request amplifier factor K1
			mass[0] = (word1_D0_D15 & 0x00F8) | 0x0002;
			break;

		case 0x02: //Request amplifier factor K2
			mass[0] = (word1_D0_D15 & 0x00F8) | 0x0003;
			break;

		case 0x03: //Request Fcut
			mass[0] = (word1_D0_D15 & 0x00F8) | 0x0004;
			break;

		case 0x05: // Request state byte
			mass[0] = (word1_D0_D15 & 0x00F8) | 0x0005;
			break;

		default:
			mass[0] = (word1_D0_D15 & 0x00F8) | 0x0005;
			break;
	}

	mass[1] = 0x0080;	
	mass[2] = 0x0000;
	mass[3] = 0x0000;

	Data_transmite_UART_9B(mass , 4,  USARTx);
	tmp = Data_receive_UART_9B(4 , USARTx);

	if( tmp == 0xFFFFFFFF ){
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 0;
		}
		Write_reg304_D0_D15(0x02);

	}else{
		if( USARTx == USART3 ){
			reg302_ptr->block2_ready = 1;
		}

    	switch(number_request){

			case 0x01: //Request amplifier factor K1 
				answer_ISA_D15_D0 = (( (uint16_t)(tmp>>8) ) & 0x0300) | 0x0001;
				break;

			case 0x02: //Request amplifier factor K2

				answer_ISA_D15_D0 = table_amp_factorK2[ ((uint8_t)(tmp>>16)) ];
				answer_ISA_D15_D0 = (answer_ISA_D15_D0<<4)| 0x0001;				 	
				break;

			case 0x03: //Request Fcut
				answer_ISA_D15_D0 = (( (uint16_t)(tmp>>8) ) & 0xFF00) | 0x0001;
				break;

			case 0x05: // Request state byte
				answer_ISA_D15_D0 = ( (uint16_t)(tmp>>16) ) & 0x00FF;
				break;

			case 0x04:
			case 0x06:
			case 0x07:
				answer_ISA_D15_D0 = 0x0001;
				break;

			default:
				answer_ISA_D15_D0 = 0x0001;
				break;
		}

    	printf("answer_ISA_D15_D0=%lu \r\n ",(unsigned long) answer_ISA_D15_D0);

		Write_reg304_D0_D15(answer_ISA_D15_D0);
	}

	return SUCCESS; 
}


void ISA_Command_800( void ){
	
	I2C_write_reg_TCA9554(I2C1, 0x20, 0x01, 0xFF); // ON/OFF analog module in block1, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1, 0x26, 0x01,  0xFF); // ON/OFF analog module in block1, Address IC = 0x26
	
	/* Need command to send reset command to CM vodule in block 2
	mass[0] = 0x0006;
	mass[1] = 0x0000;
	mass[2] = 0x0000;
	mass[3] = 0x0000;
	Data_transmite_UART_9B (mass , 4,  USART3);
	Data_receive_UART_9B (4 , USART3);*/

	Write_reg304_D0_D15(0x01);

	while(1); // Wait watch dog
}

ErrorStatus ISA_Command_900( uint16_t word1_D0_D15, _STATUS_CONTROL_MODULE *status_control_mod ){

	uint8_t state_control_module = 0;
	uint16_t mass[4];

	state_control_module = (uint8_t)word1_D0_D15;

	mass[0] = 0x0000;
	mass[2] = 0x0000;
	mass[3] = 0x0000;

	if(state_control_module == 0x01){//Start mode
		mass[1] = 0x00FF;
		status_control_mod->cm_state_start_stop = 1; //Set Start 
		status_control_mod->cm_check_status_analog_mod = 1; 
		//counter_ADC_data_ready = 0;
		//loop_counter = 0;
		//INTERRUPT_PULSE_Enable();
		//FLAG_interrupt_PULSE = 0;

	}else{ // Stop mode
		mass[1] = 0x0000;
		status_control_mod-> cm_state_start_stop = 0; //Set Stop state
		INTERRUPT_PULSE_Disable();
		FLAG_interrupt_PULSE = 0;
		Disable_IO0_global_clock(); 
	}

	Data_transmite_UART_9B (mass, 4,  USART1);
	Data_transmite_UART_9B (mass, 4,  USART3);

	Write_reg304_D0_D15(0x01);

	return SUCCESS; 
}


ErrorStatus ISA_Command_A00( uint16_t word1_D0_D15, _REG_302 *reg302_ptr ){

	uint8_t addr_watch_registr = 0;
	uint8_t value_watch_registr = 0;
	uint16_t word2_D0_D15 = 0;
	ErrorStatus ret;

	addr_watch_registr = (uint8_t)word1_D0_D15;

	//Answer at the 1 word command A00
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr, 1, 1, 0 );

	if( wait_interrupt_INT3() ){
		Error_Handler();
		return ERROR;
	}

	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	value_watch_registr = (uint8_t)(word2_D0_D15 & 0x00FF); 

	ret = I2C_write_reg_DS3232(I2C1 , 0x68, addr_watch_registr, value_watch_registr); 

	if(ret == ERROR){
		Write_reg304_D0_D15( 0x02);
	}else{

		Write_reg304_D0_D15( 0x01);
	}

	return SUCCESS; 
}

ErrorStatus ISA_Command_B00( uint16_t word1_D0_D15 ){

	uint8_t addr_read_watch_registr = 0;
	uint8_t value_read_watch_registr = 0;

	addr_read_watch_registr = (uint8_t)word1_D0_D15;

	value_read_watch_registr = I2C_read_reg_DS3232(I2C1 , 0x68, addr_read_watch_registr);

	Write_reg304_D0_D15( (uint16_t)value_read_watch_registr );

	return SUCCESS; 
}









