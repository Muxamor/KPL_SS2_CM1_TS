
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"
#include "analog_mod_control.h"
#include "control_module.h"

#include  <stdio.h>

/****************************TODO*************************
1. Check first front signal I2C on the board. Speed = 400kHz 

						Attention !!!!!!
The structure of the software is not optimal because it was originally
issued an incorrect description of the work of the exchange protocol.
 !!!!If there is time to redo the structure!!!!!!!
**********************************************************/
//LL_mDelay(1);
//LL_RCC_ClocksTypeDef check_RCC_Clocks,  *CHECK_RCC_CLOCKS=&check_RCC_Clocks; // Only for check setup clock. Not need use in release

uint8_t FLAG_interrupt_INT3; // 1 = Command is come 
uint8_t FLAG_interrupt_PULSE; // 1 = Pulse (global clock) is come
uint8_t loop_counter; // counter of package 
uint8_t counter_ADC_data_ready; // counter of package 

int main(void){

	FLAG_interrupt_INT3 = 0;
	FLAG_interrupt_PULSE = 0;
	loop_counter = 0;
	counter_ADC_data_ready =0;

	_REG_302 reg_302 = {
						.reg_304_ready_get_command     = 1, 
 						.harware_1bit_reg302		   = 0, 
 						.harware_2bit_reg302		   = 0, 
 						.block2_ready			       = 0,
 						.buffer_empty	     		   = 1,
 						.buffer_error				   = 0,
 						.harware_6bit_reg302		   = 0,
 						.harware_7bit_reg302		   = 0, 
						};

	 _REG_302 *REG302_ptr = &reg_302;

	 _ANALOG_MODULE_CONF analog_mod_config[32] = {0};

	LL_Init();
	SystemClock_Config(); //Setup system clock at 80 MHz
	SetupGPIO();
	USART1_Init();
	USART3_Init();
	I2C1_Init();
	SetupInterrupt();
	
	Default_Setup_CM(REG302_ptr);

	while(1){

		if( FLAG_interrupt_INT3 == 1 ){
			Get_Parse_ISA_command (REG302_ptr, analog_mod_config );
		}



	}

}

 

/*//Only FOR TESTE!!!!!!!!!!!!!!!!!!!!!!!!!

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

////////////////////////////////////////////////////////////////////////////////////////////////////*/


