
#ifndef CONTROL_MODULE_H_
#define CONTROL_MODULE_H_

#ifdef __cplusplus
 extern "C" {
#endif

 extern void _Error_Handler(char *, int);

 typedef struct{

 	uint8_t cm_state_start_stop; // 1=start 0=stop
 	uint8_t first_adc_package; // 1=first ADC package after reset module  0 = not first
 	uint8_t block2_present;  // 1=block2 present 0 = block2 not present



 }_STATUS_CONTROL_MODULE; 




 void Default_Setup_CM( void );
 uint32_t Read_data_D0_D15( void );
 void Write_reg304_D0_D15( uint32_t data_D0_D15 );
 void Write_reg302_D0_D7 ( uint32_t data_D0_D7 );



#ifdef __cplusplus
}
#endif


#endif /* CONTROL_MODULE_H_ */
