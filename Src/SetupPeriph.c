/**
  ******************************************************************************
  * @author  Ivan Neskorodev
  * @Emil    ivan.neskorodev@gmail.com
  * @version V0.0
  * @date    09.07.2018
  * @brief   Function for work with IC on the SS board
  *
  *
  ******************************************************************************
*/

#include "stm32l4xx.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_usart.h"
#include "SetupPeriph.h"

#include  <stdio.h>

//#include "stm32l4xx_ll_dma.h"



void LL_Init(void){

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* System interrupt init*/
	/* MemoryManagement_IRQn interrupt configuration */
	NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* BusFault_IRQn interrupt configuration */
	NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* UsageFault_IRQn interrupt configuration */
	NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* SVCall_IRQn interrupt configuration */
	NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* DebugMonitor_IRQn interrupt configuration */
	NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* PendSV_IRQn interrupt configuration */
	NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	/* SysTick_IRQn interrupt configuration */
	//NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void){

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

	if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4){
		Error_Handler();
	}

	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

  	//Enable HSE
  	LL_RCC_HSE_EnableBypass();
  	LL_RCC_HSE_Enable();
   	// Wait till HSE is ready
  	while(LL_RCC_HSE_IsReady() != 1);

	/*
	//Enable HSI
	LL_RCC_HSI_Enable();
	//Wait till HSI is ready
	while(LL_RCC_HSI_IsReady() != 1);
	//LL_RCC_HSI_SetCalibTrimming(16);//not need to do this
    */

	// Enable LSI
	LL_RCC_LSI_Enable();

	/* Wait till LSI is ready */
	while(LL_RCC_LSI_IsReady() != 1);

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 10, LL_RCC_PLLR_DIV_2); // LL_RCC_PLLSOURCE_HSE
	LL_RCC_PLL_Enable();
	LL_RCC_PLL_EnableDomain_SYS();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1);

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_Init1msTick(80000000);
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SetSystemCoreClock(80000000);

  /* Setup clock source for USART1 and I2C */
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);

	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));
}

/**
  * @brief  This function setup GPIO .
  * @retval None
*/
void SetupGPIO(void){

	LL_GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);


	/* Enable/Disable global clock  */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
	/*For change mode D0..D15 need use  */
	/*Enable_IO0_global_clock()    		*/
	/*Disable_IO0_global_clock()    	*/


	/* Configure data D0..D15 ISA port       */
	/* D0..D7 = PA0..PA7, D8..D15 = PC0..PC7 */ 
	GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; //
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode =  LL_GPIO_MODE_OUTPUT; //
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*For change mode D0..D15 need use  */
	/*Set_Output_mode_D0_D7()   		*/
	/*Set_Output_mode_D8_D15()   		*/
	/*8Set_Input_mode_D0_D7()    		*/
	/*Set_Iutput_mode_D8_D15()  		*/


	/* Configure CLK300 (PC8)       */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);

	/*For set/reset CLK300 need use  */
	/*Set_CLK300()   	             */
	/*Reset_CLK300()   		         */

	/* Configure CLK302_1 (PC10)       */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_10);

	/*For set/reset CLK302_1 need use  */
	/*Set_CLK302_1()   				   */
	/*Reset_CLK302_1()   	       	   */


	/* Configure CLK302_2 (PC11)       */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_11);

	/*For set/reset CLK302_2 need use  */
	/*Set_CLK302_2()   				   */
	/*Reset_CLK302_2()   	       	   */


	/* Configure CLK302_3 (PC12)       */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_12);

	/*For set/reset CLK302_3 need use  */
	/*Set_CLK302_3()   				   */
	/*Reset_CLK302_3()   	       	   */


	/* Configure CLK304               */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_9);

	/*For set/reset CLK304   need use  */
	/*Set_CLK304()   				   */
	/*Reset_CLK304()   	       	  	   */
	/*Pulse_CLK304()				   */


	/* Configure EN304               */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);

	/*For set/reset CLK304   need use  */
	/*Set_EN304()   				   */
	/*Reset_EN304()   	       	  	   */


	/* Configure RST304               */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_1);

	/*For set/reset CLK304   need use  */
	/*Set_RST304()   				   */
	/*Reset_RST304()   	       	  	   */


	/* Configure CLK306               */
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH ;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_2);

	/*For set/reset CLK306   need use  */
	/*Set_RST306()   				   */
	/*Reset_RST306()   	       	  	   */




}

void USART1_Init(void){

  	LL_USART_InitTypeDef USART_InitStruct;
  	LL_GPIO_InitTypeDef GPIO_InitStruct;

  	/* Peripheral clock enable */
  	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  
  	/**USART1 GPIO Configuration  
  	PA9   ------> USART1_TX
  	PA10   ------> USART1_RX 
  	*/
  	GPIO_InitStruct.Pin = LL_GPIO_PIN_9|LL_GPIO_PIN_10;
  	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  	/* USART1 configured as follow:
            - BaudRate = 5000000
            - Word Length = 9 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */
  	USART_InitStruct.BaudRate = 10000000;//5000000;
  	USART_InitStruct.DataWidth =  LL_USART_DATAWIDTH_9B;
  	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_8;
  	LL_USART_Init(USART1, &USART_InitStruct);

  	LL_USART_SetRXPinLevel(USART1, LL_USART_RXPIN_LEVEL_INVERTED);
  	LL_USART_SetTXPinLevel(USART1, LL_USART_TXPIN_LEVEL_INVERTED );

  	LL_USART_ConfigAsyncMode(USART1); 
  	LL_USART_Enable(USART1);

    /* Configure pins RE and TE to control transfer data throughISO3086DW, PA11=DE PA12=RE */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_11|LL_GPIO_PIN_12;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /**/
    //RE -Enable when low
    //TE - Enbale when high
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11); //Enable receive data
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12); //Enable transmit data
}


void USART3_Init(void){

  	LL_USART_InitTypeDef USART_InitStruct;
  	LL_GPIO_InitTypeDef GPIO_InitStruct;

  	/* Peripheral clock enable */
  	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
  	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  
  	/**USART1 GPIO Configuration  
  	PB10   ------> USART3_TX
  	PB11  ------> USART3_RX 
  	*/
  	GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  	/* USART3 configured as follow:
            - BaudRate = 5000000
            - Word Length = 9 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */
  	USART_InitStruct.BaudRate = 10000000;
  	USART_InitStruct.DataWidth =  LL_USART_DATAWIDTH_9B;
  	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_8;
  	LL_USART_Init(USART3, &USART_InitStruct);

  	LL_USART_SetRXPinLevel(USART3, LL_USART_RXPIN_LEVEL_INVERTED);
  	LL_USART_SetTXPinLevel(USART3, LL_USART_TXPIN_LEVEL_INVERTED );

  	LL_USART_ConfigAsyncMode(USART3); 
  	LL_USART_Enable(USART3);

}

/**
  * @brief  Setup I2C.
  * @param  None
  * @retval None
*/
void I2C1_Init(void){

	LL_I2C_InitTypeDef I2C_InitStruct;

  	LL_GPIO_InitTypeDef GPIO_InitStruct;

  	/* Peripheral clock enable */
  	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
  	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  
  	/**I2C1 GPIO Configuration  
  	PB6   ------> I2C1_SCL
  	PB7   ------> I2C1_SDA 
  	*/
  	GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  	GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /**I2C Initialization 
    */
  	LL_I2C_EnableAutoEndMode(I2C1);
	LL_I2C_DisableOwnAddress2(I2C1);
  	LL_I2C_DisableGeneralCall(I2C1);
  	LL_I2C_EnableClockStretching(I2C1);

  	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  	I2C_InitStruct.Timing = 0x00702991; // 0x10909CEC - Frequenci I2C=100 kHz  //0x00702991 - Frequenci I2C=400 kHz
  	I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  	I2C_InitStruct.DigitalFilter = 0;
  	I2C_InitStruct.OwnAddress1 = 0;
  	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK; //LL_I2C_NACK 
  	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  	LL_I2C_Init(I2C1, &I2C_InitStruct);

  	LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);

  	LL_I2C_Enable(I2C1);
}


/**
  * @brief  This function setup interrupts for all ports and inside event .
  * @param  None
  * @retval None
*/
void SetupInterrupt(void){
	
    LL_EXTI_InitTypeDef EXTI_InitStruct;

  	/*Setup interrupt INT3(PB2) RISING EDGE*/
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE2);

    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_2;
    EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);

    /*For enable/desible interrupt INT3 need use:                           */
    /*INT3_Enable()       LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_2)             */
    /*INT3_Disable()      LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_2)            */
    /*Define in SetupPeriph.h                                               */

    // EXTI interrupt init
    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0)); //Set priority №2 from 0..15

    NVIC_EnableIRQ(EXTI2_IRQn);
    INT3_Enable();

    /**********************************************/

     /*Setup interrupt PULSE(PB9)*/
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE9);

    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_9;
    EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);

    /*For enable/desible interrupt PULSE need use:                  */
    /*INTERRUPT_PULSE_Enable()           LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9)                 */
    /*INTERRUPT_PULSE_Disable()          LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_9)                */
    /*Define in SetupPeriph.h                          */

    // EXTI interrupt init
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0)); //Set priority №3 from 0..15

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    INTERRUPT_PULSE_Disable();

}



/**
  * @brief  This function setup Watch Dog.
  * @retval None
*/

void MX_IWDG_Init(void){

    LL_IWDG_Enable(IWDG);
  	LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32);
    LL_IWDG_SetWindow(IWDG, 4095);
    LL_IWDG_SetReloadCounter(IWDG, 4095);
    while (LL_IWDG_IsReady(IWDG) != 1);

    LL_IWDG_ReloadCounter(IWDG);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  *
  *
  *
  */
void _Error_Handler(char *file, int line)
{

	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
 //while(1)
  //{
 // }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

