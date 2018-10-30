

#ifndef __ANALOG_MOD_CONTROL_H
#define __ANALOG_MOD_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

 extern void _Error_Handler(char *, int);

 typedef struct{

 	uint8_t power_module_on; // =1 analog module is on, =0 analog module is off
 	uint8_t amp_factor_K1;
 	uint8_t amp_factor_K2;
 	uint8_t Fcut_value; 
 	uint8_t Status_module; 
 	  

 }_ANALOG_MODULE_CONFIG; 



#ifdef __cplusplus
}
#endif


#endif /* __ANALOG_MOD_CONTROL_H */
