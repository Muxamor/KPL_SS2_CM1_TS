
#ifndef CONTROL_MODULE_H_
#define CONTROL_MODULE_H_

#ifdef __cplusplus
 extern "C" {
#endif

 extern void _Error_Handler(char *, int);

 typedef struct{

 	uint8_t cm_state_start_stop; // 1=start 0=stop
 	uint8_t first_adc_package; // 1=first ADC package after reset module  0 = not first
 	


 }_STATUS_CONTROL_MODULE; 




 void Default_Setup_CM(void);



#ifdef __cplusplus
}
#endif


#endif /* CONTROL_MODULE_H_ */
