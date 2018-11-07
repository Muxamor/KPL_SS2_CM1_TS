
#ifndef CONTROL_MODULE_H_
#define CONTROL_MODULE_H_

#ifdef __cplusplus
 extern "C" {
#endif


 extern void _Error_Handler(char *, int);



  typedef struct{

 	uint8_t head_byte; // 
 	uint8_t cyclic_code; // 
 	uint8_t ADC_data_byte_MSB;
 	uint8_t ADC_data_byte_LSB;
 
 }_ADC_DATA_Package; 


 #define FIFO_SIZE 4096
	
 typedef struct{

 	_ADC_DATA_Package FIFO_buf_ADC[FIFO_SIZE];
 	uint16_t FIFO_HEAD;
	uint16_t FIFO_TAIL;
	uint16_t COUNT_DATA_IN_FIFO;

 }_FIFO;

 typedef struct{

 	uint8_t cm_state_start_stop; // 1=start 0=stop
 	uint8_t cm_check_status_analog_mod; //1=need check status analog modules
 	uint8_t first_adc_package; // 1=first ADC package after reset module  0 = not first
	
 }_STATUS_CONTROL_MODULE; 

 void write_to_FIFO( _FIFO  *FIFO_adc_data_ptr, _ADC_DATA_Package adc_package );


 typedef struct{

 	uint8_t reg_304_ready_get_command : 1; 
 	uint8_t harware_1bit_reg302		  : 1; 
 	uint8_t harware_2bit_reg302		  : 1; 
 	uint8_t block2_ready			  : 1;
 	uint8_t buffer_empty			  : 1;
 	uint8_t buffer_error			  : 1;
 	uint8_t harware_6bit_reg302		  : 1;
 	uint8_t harware_7bit_reg302		  : 1;  

 }_REG_302; 


 typedef struct{

 	uint8_t power_module_on; // =1 analog module is on, =0 analog module is off
 	uint8_t module_dead; // =1 analog module is dead, =0 analog module is ok  
 	uint8_t amp_factor_K1;
 	uint8_t amp_factor_K2;
 	uint8_t Fcut_value; 
 	uint8_t Status_module; 


 }_ANALOG_MODULE_CONF; 


 void Default_Setup_CM( _REG_302 *reg302_ptr );
 uint32_t Read_reg304_D0_D15( void );
 void Write_reg304_D0_D15( uint16_t data_D0_D15 );
 void Write_reg302_D0_D7 ( uint16_t data_reg302, uint8_t write_CLK302_1, uint8_t write_CLK302_2, uint8_t write_CLK302_3 );
 void Write_reg300_D0_D15 (uint16_t data_D0_D15);
 uint8_t wait_interrupt_INT3(void);
 void Get_Parse_ISA_command (_REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[], _STATUS_CONTROL_MODULE *stat_cont_mod);


#ifdef __cplusplus
}
#endif


#endif /* CONTROL_MODULE_H_ */
