
#include "stm32l4xx.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_gpio.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"
#include "control_module.h"
#include "analog_mod_control.h"

#include "global_variables.h"

#include  <stdio.h>

ErrorStatus ISA_Command_100( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint16_t word2_D0_D15 = 0, word3_D0_D15 = 0;
	uint8_t  i=0;
	ErrorStatus ret1, ret2;
	uint16_t mass[4];
	uint32_t tmp=0;
	uint32_t counter=0;

	//Answer at the 1 word command 100
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	//Get and processing second word
	counter=0;
	while( FLAG_interrupt_INT3==0 ){
		counter++;
		if(counter==10000000){
			Error_Handler();
			goto exit_error;
		}
	}

	FLAG_interrupt_INT3 = 0;
	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();
	ret1 = I2C_write_reg_TCA9554(I2C1, 0x20, 0x01, (~((uint8_t)word2_D0_D15) ) ); // ON/OFF analog module in block1, Address IC = 0x20
	ret2 = I2C_write_reg_TCA9554(I2C1, 0x26, 0x01, (~( (uint8_t)(word2_D0_D15>>8) ) ) ); // ON/OFF analog module in block1, Address IC = 0x26
			
	if(ret1==ERROR || ret2==ERROR){
				
		for(i=0; i<16; i++){
			analog_mod_config[i].power_module_on = 0x00;
		}
		Error_Handler();
		Write_reg304_D0_D15( 0x02);
		
	}else{
		
		for(i=0; i<16; i++){
			analog_mod_config[i].power_module_on = ( word2_D0_D15 >> i ) & 0x0001;
		}
		Write_reg304_D0_D15( 0x01); 
	}
			
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	//Get and processing third word
	counter=0;
	while( FLAG_interrupt_INT3==0 ){
		counter++;
		if(counter==10000000){
			Error_Handler();
			goto exit_error;
		}
	}

	FLAG_interrupt_INT3= 0;
	word3_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	mass[0] = 0x06;
	mass[1] = 0x00;
	mass[2] = (word3_D0_D15>>8) & 0x00FF;
	mass[3] = word3_D0_D15 & 0x00FF;
	Data_transmite_UART_9B ( mass , 4,  USART3);
	tmp = Data_receive_UART_9B (4 , USART3);

	if(tmp == 0xFFFFFFFF || tmp != 0x06010000){
		reg302_ptr->block2_ready = 0;
		for(i=16; i<32; i++){
			analog_mod_config[i].power_module_on = 0x00;
		}

	}else{
		reg302_ptr->block2_ready = 1;
		for(i=16; i<32; i++){
			 analog_mod_config[i].power_module_on = ( word3_D0_D15 >> i ) & 0x0001;
		}
	}

	if(reg302_ptr->block2_ready == 0){
		Write_reg304_D0_D15( 0x02);
	}else{
		Write_reg304_D0_D15( 0x01);
	}
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	return SUCCESS; 

	exit_error:
		return ERROR;

}


ErrorStatus ISA_Command_200( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t  i=0;

	uint16_t mass[4];
	uint32_t tmp=0;
	
	I2C_write_reg_TCA9554(I2C1, 0x20, 0x01, 0xFF); // ON/OFF analog module in block1, Address IC = 0x20
	I2C_write_reg_TCA9554(I2C1, 0x26, 0x01,  0xFF); // ON/OFF analog module in block1, Address IC = 0x26

	mass[0] = 0x06;
	mass[1] = 0x00;
	mass[2] = 0x00;
	mass[3] = 0x00;
	Data_transmite_UART_9B ( mass , 4,  USART3);
	tmp = Data_receive_UART_9B (4 , USART3);

	if(tmp == 0xFFFFFFFF || tmp != 0x06010000){
		reg302_ptr->block2_ready = 0;
	}

	for(i=0; i<32; i++){
		analog_mod_config[i].power_module_on = 0x00;
	}

	Write_reg304_D0_D15( 0x01);

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	return SUCCESS; 
}


ErrorStatus ISA_Command_400( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;

	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	addr_analog_mod = word1_D0_D15  >> 3;

	if(addr_analog_mod < 16){
		USARTx = USART1;

	}else{
		USARTx = USART3;
	}

	mass[0] = (word1_D0_D15 & 0xF8)| 0x02; // Set K1 command 
	mass[1] = word1_D0_D15 & 0x03;
	mass[2] = 0x00;
	mass[3] = 0x00;

	Data_transmite_UART_9B ( mass , 4,  USARTx);
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

		analog_mod_config[addr_analog_mod].amp_factor_K1 = word1_D0_D15 & 0x03;
		Write_reg304_D0_D15(0x01);
	}

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	return SUCCESS; 
}


ErrorStatus ISA_Command_500( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;
	uint16_t word2_D0_D15 = 0;
	uint32_t counter=0;

	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	

	//Answer at the 1 word command 500
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	//Get and processing second word
	counter=0;
	while( FLAG_interrupt_INT3==0 ){
		counter++;
		if(counter==10000000){
			Error_Handler();
			goto exit_error;
		}
	}

	FLAG_interrupt_INT3 = 0;
	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	
	addr_analog_mod = word1_D0_D15 >> 3;

	if(addr_analog_mod < 16){
		USARTx = USART1;

	}else{
		USARTx = USART3;
	}

	mass[0] = ((word1_D0_D15 & 0xF8)| 0x03) &0X00FF; // Set K2 command 

	switch (word2_D0_D15 & 0XFFF){
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

	mass[2] = 0x00;
	mass[3] = 0x00;

	Data_transmite_UART_9B ( mass , 4,  USARTx);
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

		analog_mod_config[addr_analog_mod].amp_factor_K2 = word2_D0_D15 & 0X0FFF;
		Write_reg304_D0_D15(0x01);
	}

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	return SUCCESS; 

	exit_error:
		return ERROR; 

}


ErrorStatus ISA_Command_600( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] ){

	uint8_t addr_analog_mod=0;
	uint16_t word2_D0_D15 = 0;
	uint32_t counter=0;

	uint16_t mass[4];
	uint32_t tmp=0;

	USART_TypeDef *USARTx;
	
	//Answer at the 1 word command 600
	Write_reg304_D0_D15( 0x01);
	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	//Get and processing second word
	counter=0;
	while( FLAG_interrupt_INT3==0 ){
		counter++;
		if(counter==10000000){
			Error_Handler();
			goto exit_error;
		}
	}

	FLAG_interrupt_INT3 = 0;
	word2_D0_D15 = (uint16_t) Read_reg304_D0_D15();

	
	addr_analog_mod =  ((uint8_t)word1_D0_D15) >> 3;

	if(addr_analog_mod < 16){
		USARTx = USART1;

	}else{
		USARTx = USART3;
	}

	mass[0] = ((word1_D0_D15 & 0xF8)| 0x04) & 0x00FF; // Set Fcut command 
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

	reg302_ptr->reg_304_ready_get_command = 1;
	Write_reg302_D0_D7 ( *(uint32_t*)reg302_ptr );

	return SUCCESS; 

	exit_error:
		return ERROR; 

}












