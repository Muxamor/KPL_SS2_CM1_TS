
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SetupPeriph.h"
#include "i2c_cm.h"
#include "uart_comm.h"
#include "command_ISA.h"
#include "control_module.h"

#include  <stdio.h>



/****************************TODO*************************

						Attention !!!!!!
The structure of the software is not optimal because it was originally
issued an incorrect description of the work of the exchange protocol.
 !!!!If there is time to redo the structure!!!!!!!
**********************************************************/
//LL_mDelay(1);
//LL_RCC_ClocksTypeDef check_RCC_Clocks,  *CHECK_RCC_CLOCKS=&check_RCC_Clocks; // Only for check setup clock. Not need use in release

uint8_t FLAG_interrupt_INT3; // 1 = Command is come 
uint8_t FLAG_interrupt_PULSE; // 1 = Pulse (global clock) is come
int16_t loop_counter; // counter of package 
uint8_t overwrite_fifo;
uint8_t counter_ADC_data_ready; // counter of package 

int main(void){


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

	 _STATUS_CONTROL_MODULE stat_contr_mod = {
	 										   .cm_state_start_stop = 0,
 											   .first_adc_package = 1,
											   .FIFO_no_empty = 0,
											   .restriction_settings_Fcut = 0,
											 };

	 _STATUS_CONTROL_MODULE *STATUS_CONT_MOD_ptr = &stat_contr_mod;

	 _ANALOG_MODULE_CONF analog_mod_config[32] = {0};

 	_FIFO fifo_buf = {
 					   .FIFO_HEAD = 0,
 					   .FIFO_TAIL = 0,
 					   .COUNT_DATA_IN_FIFO = 0,
 					 };	

 	_FIFO *FIFO_ADC_DATA_ptr = &fifo_buf;

 	_ADC_DATA_Package ADC_data_package;

 	uint16_t array_u16[4] = {0};
 	uint8_t array_u8[4] = {0};
 	USART_TypeDef *USARTxx;
 	uint8_t addr_a_mod = 0;
 	uint8_t ret = 0;
 	ErrorStatus ret_AM_ADC;
 	uint16_t data_D0_D15;

	LL_Init();
	SystemClock_Config(); //Setup system clock at 80 MHz
	SetupGPIO();
	USART1_Init();
	USART3_Init();
	I2C1_Init();
	SetupInterrupt();
	IWDG_Init();

	Default_Setup_CM(REG302_ptr);

	//Reset Interrupt and caunters;	FLAG_interrupt_INT3 = 0;
	FLAG_interrupt_PULSE = 0;
	loop_counter = 0;
	counter_ADC_data_ready = 0;

	//LL_IWDG_ReloadCounter(IWDG);

	while(1){
		LL_IWDG_ReloadCounter(IWDG);

		//Processing command from PC 
		if( FLAG_interrupt_INT3 == 1 ){
			FLAG_interrupt_INT3 = 0;
			Get_Parse_ISA_command(REG302_ptr, analog_mod_config, STATUS_CONT_MOD_ptr);
		}



		//Sart mode, Read data ADC from analog modules and put to FIFO buffer
		if(FLAG_interrupt_PULSE == 1 && STATUS_CONT_MOD_ptr-> cm_state_start_stop == 1 && STATUS_CONT_MOD_ptr->cm_check_status_analog_mod == 0 ){
			FLAG_interrupt_PULSE = 0;

			LL_IWDG_ReloadCounter(IWDG);

			if(counter_ADC_data_ready < 76){

				for( addr_a_mod = 0; addr_a_mod < 32; addr_a_mod++ ){

					if(analog_mod_config[addr_a_mod].power_module_on == 1 ){

						ADC_data_package.head_byte = ( (addr_a_mod << 3) | 0x01 );
						ADC_data_package.cyclic_code = loop_counter;
						ADC_data_package.ADC_data_byte_MSB = 0x00;
						ADC_data_package.ADC_data_byte_LSB = 0x00;

						Write_FIFO( FIFO_ADC_DATA_ptr, ADC_data_package );
					}
				}
			}else{
				LL_IWDG_ReloadCounter(IWDG);
				//Read ADC data from analogs modules and put to FIFO buf. Also need check error flag in ADC package from modules. 
				for( addr_a_mod = 0; addr_a_mod < 32; addr_a_mod++ ){

					if(analog_mod_config[addr_a_mod].power_module_on == 1 ){

						array_u16[0] = (addr_a_mod<<3) | 0x01;

						if(addr_a_mod < 16){
							USARTxx = USART1;
						}else{
							USARTxx = USART3;
						}

						ret_AM_ADC = Transfer_ADC_data_UART_9B (array_u16, 1, array_u8, 4, addr_a_mod, USARTxx); // Ask ADC data twice if was error
						//Data_transmite_UART_9B(array_u16 , 1,  USARTxx);
						//ret_AM_ADC = ADC_data_receive_UART(array_u8, 4, USARTxx);

						if(ret_AM_ADC == ERROR ){
						//if(( ret_AM_ADC == ERROR ) || ( array_u8[0] >> 3 != addr_a_mod ) ){
							Error_Handler();
							ADC_data_package.head_byte = addr_a_mod<<3;
							ADC_data_package.cyclic_code = loop_counter;
							ADC_data_package.ADC_data_byte_MSB = 0;
							ADC_data_package.ADC_data_byte_LSB = 0;

						}else{
							//TODO Check error flag. If lost synchronization need restart ADC.
							ADC_data_package.head_byte = array_u8[0];
							ADC_data_package.cyclic_code = loop_counter;
							ADC_data_package.ADC_data_byte_MSB = array_u8[2];
							ADC_data_package.ADC_data_byte_LSB = array_u8[3];
						}

						ret = Write_FIFO( FIFO_ADC_DATA_ptr, ADC_data_package );

						if(ret == 1){ // Overwrite FIFO
							overwrite_fifo = 1;
							REG302_ptr-> buffer_error = 1; //buffer overwrite
							Write_reg302_D0_D7(*(uint32_t*)REG302_ptr, 0, 0, 1);
							REG302_ptr-> buffer_error = 0;
						}else{
							overwrite_fifo = 0; // FIFO no overwrite
						}
					}

					if( FIFO_ADC_DATA_ptr->COUNT_DATA_IN_FIFO >= 32){//Edge to set flg FIFO no empty   /*FIFO_SIZE/64*/  
						STATUS_CONT_MOD_ptr->FIFO_no_empty = 1;
						REG302_ptr->buffer_empty = 0; //Buffer NO Empty
						Write_reg302_D0_D7(*(uint32_t*)REG302_ptr, 0, 1, 0);
					}

				}
			}

		}



		//Start mode. Send data to ISA if we have data more than half buffer.
		if( STATUS_CONT_MOD_ptr->cm_state_start_stop == 1 && STATUS_CONT_MOD_ptr->FIFO_no_empty == 1 ){


			if( Read_pin_INT2() == 1 || STATUS_CONT_MOD_ptr->first_adc_package == 1 ){

				STATUS_CONT_MOD_ptr->first_adc_package = 0;

				LL_IWDG_ReloadCounter(IWDG);
				ADC_data_package = Read_FIFO( FIFO_ADC_DATA_ptr );

				data_D0_D15 = (ADC_data_package.head_byte << 8) | ADC_data_package.cyclic_code;
				Write_reg304_D0_D15( data_D0_D15 );

				data_D0_D15 = (ADC_data_package.ADC_data_byte_MSB << 8) | ADC_data_package.ADC_data_byte_LSB;
				Write_reg300_D0_D15( data_D0_D15 );
			}

			if( FIFO_ADC_DATA_ptr->COUNT_DATA_IN_FIFO == 0){
				STATUS_CONT_MOD_ptr->FIFO_no_empty = 0;
				REG302_ptr->buffer_empty = 1; //Buffer Empty
				Write_reg302_D0_D7( *(uint32_t*)REG302_ptr, 0, 1, 0 );
			}

		}



		//Start mode, Pre-start preparation and check status analog module
		if( STATUS_CONT_MOD_ptr-> cm_state_start_stop == 1 && STATUS_CONT_MOD_ptr->cm_check_status_analog_mod == 1 ){

			STATUS_CONT_MOD_ptr->cm_check_status_analog_mod = 0;

			//TODO Check status enabled analog module. If we have wrong status need reconfigure broken module.

 			Clear_FIFO( FIFO_ADC_DATA_ptr );

 			counter_ADC_data_ready = 0;
			loop_counter = -1;

			INTERRUPT_PULSE_Enable(); 
		    FLAG_interrupt_PULSE = 0; 

			Enable_IO0_global_clock(); 

		    //Send start command to analog module;
		    array_u16[0] = 0x0000;
		    array_u16[1] = 0x00FF;
		    array_u16[2] = 0x0000;
		    array_u16[3] = 0x0000;
		    Data_transmite_UART_9B( array_u16, 4, USART1 );
		    Data_transmite_UART_9B( array_u16, 4, USART3 );
		}

	}




}

 


