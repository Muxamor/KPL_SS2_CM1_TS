/*
 * Setup peripheral of MC STM32L452
 */

#ifndef __SETUPPERIPH_H
#define __SETUPPERIPH_H

#define DEBUGprintf // Enable printf() message  to SWO



void LL_Init(void);
void SystemClock_Config(void);
void SetupGPIO(void);
void USART1_Init(void);
void USART3_Init(void);
void I2C1_Init(void);
void MX_IWDG_Init(void);

/*The table below gives the allowed values of the pre-emption priority and subpriority according
 to the Priority Grouping configuration performed by NVIC_PriorityGroupConfig function
  ============================================================================================================================
    NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
  ============================================================================================================================
   NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for pre-emption priority
                         |                                   |                             |   4 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
                         |                                   |                             |   3 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
                         |                                   |                             |   2 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
                         |                                   |                             |   1 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_4  |                 0-15              |            0                |   4 bits for pre-emption priority
                         |                                   |                             |   0 bits for subpriority
  ===========================================================================================================================*/

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif


/*For Enable/Disable global clock IO0(PC13) need use:             */
#define Enable_IO0_global_clock()   LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_13)
#define Disable_IO0_global_clock()  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13)

/*For change mode direction input/output ISA data port  D0..D15   */
#define Set_Output_mode_D0_D7()    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT)
#define Set_Output_mode_D8_D15()   LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT)
#define Set_Input_mode_D0_D7()     LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT)
#define Set_Input_mode_D8_D15()   LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT)

/*For set/reset CLK300 need use                 */
#define Set_CLK300()       LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8)
#define Reset_CLK300()     LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8)

/*For set/reset CLK302_1 need use               */
#define Set_CLK302_1()        LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_10)
#define Reset_CLK302_1()      LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_10)

#define Pulse_CLK302_1()      {\
								LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_10);\
								__NOP();\
								__NOP();\
								LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_10);\
							  }

/*For set/reset CLK302_2 need use               */
#define Set_CLK302_2()        LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_11)
#define Reset_CLK302_2()      LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11)

/*For set/reset CLK302_3 need use               */
#define Set_CLK302_3()        LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_12)
#define Reset_CLK302_3()      LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_12)

/*For set/reset CLK304 need use                 */
#define Set_CLK304()          LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9)
#define Reset_CLK304()        LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_9)

#define Pulse_CLK304()        {\
								LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);\
								__NOP();\
								__NOP();\
								LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_9);\
							  }
                          

/*For set/reset EN304 need use                  */
#define Set_EN304()           LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8)
#define Reset_EN304()         LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8)

/*For set/reset RST304 need use                 */
#define Set_RST304()          LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_1)
#define Reset_RST304()        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_1)

/*For set/reset CLK306 need use                 */
#define Set_CLK306()          LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_2)
#define Reset_CLK306()        LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_2)

/*For enable/desible interrupt INT3 need use:   */
#define INT3_Enable()         LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_2)             
#define INT3_Disable()        LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_2)            
  
/*For enable/desible interrupt PULSE need use:  */
#define INTERRUPT_PULSE_Enable()           LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9)                 
#define INTERRUPT_PULSE_Disable()          LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_9)          



#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* SETUPPERIPH_H_ */
