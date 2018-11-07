
#ifndef __GLOBAL_VARIABLES_H
#define __GLOBAL_VARIABLES_H

#ifdef __cplusplus
 extern "C" {
#endif

// extern void _Error_Handler(char *, int);
 extern uint8_t FLAG_interrupt_INT3;
 extern uint8_t FLAG_interrupt_PULSE; 
 extern int16_t loop_counter; 
 extern uint8_t counter_ADC_data_ready;

#ifdef __cplusplus
}
#endif


#endif /* __GLOBAL_VARIABLES_H */