

#ifndef __ANALOG_MOD_CONTROL_H
#define __ANALOG_MOD_CONTROL_H

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
 ErrorStatus ISA_Command_700( uint16_t word1_D0_D15, _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );
 ErrorStatus ISA_Command_800( _REG_302 *reg302_ptr, _ANALOG_MODULE_CONF  analog_mod_config[] );


#ifdef __cplusplus
}
#endif


#endif /* __ANALOG_MOD_CONTROL_H */
