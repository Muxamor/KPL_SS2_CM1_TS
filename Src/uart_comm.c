
#include "stm32l4xx.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_usart.h"
//#include "stm32l4xx_ll_exti.h"
#include "SetupPeriph.h"

#include "uart_comm.h"


//#include "global_variables.h"

#include  <stdio.h>


ErrorStatus Data_transmite_UART_9B (uint16_t mass[], uint8_t size_parcel,  USART_TypeDef *USARTx){

	uint32_t counter=0;

	counter=0;
	while( LL_USART_IsActiveFlag_TXE(USARTx) == RESET ){
		counter++;
		if(counter==10000000){
			Error_Handler();
			return ERROR;
		}
	}

	for ( uint8_t i=0 ; i<size_parcel; i++ ){

		if (i==0){
			LL_USART_TransmitData9( USARTx, 0x100 | mass[i] );//set 9 bit for command (first) bite 

		}else{
			LL_USART_TransmitData9( USARTx, mass[i] );

		}

		counter=0;
		while( LL_USART_IsActiveFlag_TC( USARTx ) == RESET ){
			counter++;
			if(counter==10000000){//150ms
				Error_Handler();
				return ERROR;
			}
		}

	}

	return SUCCESS;
}


/**
  * @brief  Receive data throught UARTx 4 bytes
  * @param  void
  * @retval head byte put MSB
  */
uint32_t Data_receive_UART_9B (uint8_t size_rec_data , USART_TypeDef *USARTx){

	uint32_t counter=0;
	uint8_t receive_data[4]= {0};

	for(uint8_t i=0; i<size_rec_data; i++ ){
		counter=0;
		while( LL_USART_IsActiveFlag_RXNE(USARTx) == RESET ){
			counter++;
			if(counter==1000000){
				Error_Handler();
				LL_USART_ClearFlag_ORE(USARTx);
				return 0xFFFFFFFF;
			}
		}

		receive_data[i] = (uint8_t) LL_USART_ReceiveData9(USARTx);
	}

	return (receive_data[0]<<24) | (receive_data[1]<<16) | (receive_data[2]<<8) | receive_data[3];
}



/**
  * @brief  Receive ADC data throught UARTx 4 bytes
  * @param  void
  * @retval 0 or 1 ,  1 = return error 
  */
ErrorStatus ADC_data_receive_UART (uint8_t receive_data[], uint8_t size_rec_data , USART_TypeDef *USARTx){

	uint32_t counter=0;
	
	for(uint8_t i=0; i<size_rec_data; i++ ){
		counter=0;
		while( LL_USART_IsActiveFlag_RXNE(USARTx) == RESET ){

			counter++;

			if(counter==10000000){
				LL_USART_ClearFlag_ORE(USARTx);
				for(i=0; i<size_rec_data; i++){
					if(i==0){
						receive_data[i] = receive_data[i] & 0xFE;
					}else{
						receive_data[i] = 0xFF;
					}
				}
				return ERROR;
			}
		}

		receive_data[i] = (uint8_t) LL_USART_ReceiveData9(USARTx);
	}

	return SUCCESS;
}



