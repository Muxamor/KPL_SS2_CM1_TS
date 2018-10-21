 /*
 * uart_comm.c
 *
 *  Created on: Jul 10, 2018
 *      Author: muxamor
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_usart.h"
//#include "stm32l4xx_ll_exti.h"
#include "SetupPeriph.h"

#include "uart_comm.h"


//#include "global_variables.h"

#include  <stdio.h>


ErrorStatus Data_transmite_UART_9B (uint16_t mass[], USART_TypeDef *USARTx){

	uint32_t counter=0;

	for ( uint8_t i=0 ; i<4; i++ ){

		counter=0;
		while( LL_USART_IsActiveFlag_TXE(USARTx) == RESET ){
			counter++;
			if(counter==1000000){//150ms
				Error_Handler();
				goto exit_error;
			}
		}

		if (i==0){
			LL_USART_TransmitData9( USARTx, 0x100 | mass[i] );//set 9 bit for command (first) bite 

		}else{
			LL_USART_TransmitData9( USARTx, mass[i] );

		}
	}

	counter=0;
	while( LL_USART_IsActiveFlag_TC( USARTx ) == RESET ){
		counter++;
		if(counter==1000000){//150ms
			Error_Handler();
			goto exit_error;
		}
	}

	return SUCCESS;

	exit_error:

	#ifdef DEBUGprintf
			printf("ERROR transmitting data through UART1");
	#endif

	return ERROR;
}




