

#ifndef __COMMAND_ISA_H
#define __COMMAND_ISA_H

#ifdef __cplusplus
 extern "C" {
#endif

 #include "control_module.h"

 extern void _Error_Handler(char *, int);

 ErrorStatus ISA_Command_100( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_200( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_400( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_500( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_600( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_700( uint16_t word1_D0_D15, _REG_302 *reg302_ptr);
 void ISA_Command_800( void );
 ErrorStatus ISA_Command_900( uint16_t word1_D0_D15, _STATUS_CONTROL_MODULE *status_control_mod, _REG_302 *reg302_ptr );
 ErrorStatus ISA_Command_A00( uint16_t word1_D0_D15, _REG_302 *reg302_ptr );
 ErrorStatus ISA_Command_B00( uint16_t word1_D0_D15 );
 ErrorStatus ISA_Command_E00( uint16_t word1_D0_D15, _STATUS_CONTROL_MODULE *status_control_mod, _REG_302 *reg302_ptr );


#ifdef __cplusplus
}
#endif


#endif /* __COMMAND_ISA_H */
