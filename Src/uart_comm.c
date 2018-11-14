
#include "stm32l4xx.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_usart.h"
//#include "stm32l4xx_ll_exti.h"
#include "SetupPeriph.h"

#include "uart_comm.h"


//#include "global_variables.h"

#include  <stdio.h>


ErrorStatus Data_transmite_UART_9B(uint16_t mass[], uint8_t size_parcel,  USART_TypeDef *USARTx){

	uint32_t counter=0;
	uint8_t i=0;

	if(LL_USART_IsActiveFlag_ORE(USARTx) == 1){
		LL_USART_ClearFlag_ORE(USARTx);
	}

	if(LL_USART_IsActiveFlag_RXNE(USARTx) == 1){
		LL_USART_ReceiveData9(USARTx);
	}


	counter=0;
	while( LL_USART_IsActiveFlag_TXE(USARTx) == RESET ){
		counter++;
		if( counter == 1000000 ){
			Error_Handler();
			return ERROR;
		}
	}

	for ( i=0 ; i < size_parcel; i++ ){

		counter=0;
		while( LL_USART_IsActiveFlag_TC( USARTx ) == RESET ){
			counter++;
			if( counter == 1000000 ){//150ms
				Error_Handler();
				return ERROR;
			}
		}

		if ( i == 0 ){
			LL_USART_TransmitData9( USARTx, 0x100 | mass[i] );//set 9 bit for command (first) bite 

		}else{
			LL_USART_TransmitData9( USARTx, mass[i] );

		}
	}

	return SUCCESS;
}


/**
  * @brief  Receive data throught UARTx 4 bytes
  * @param  void
  * @retval head byte put MSB
  */
uint32_t Data_receive_UART_9B(uint8_t size_rec_data , USART_TypeDef *USARTx){

	uint32_t counter = 0;
	uint16_t receive_data[4]= {0};
	uint8_t i = 0;

	/*if(LL_USART_IsActiveFlag_ORE(USARTx) == 1){
		LL_USART_ClearFlag_ORE(USARTx);
		LL_USART_ReceiveData9(USARTx); //???????????????
	}*/

	for( i = 0; i < size_rec_data; i++ ){

		counter=0;
		while( LL_USART_IsActiveFlag_RXNE(USARTx) == RESET ){
			counter++;
			if(counter==100000){
				Error_Handler();
				LL_USART_ClearFlag_ORE(USARTx);
				return 0xFFFFFFFF;
			}
		}

		receive_data[i] = LL_USART_ReceiveData9(USARTx);
	}

	if( (receive_data[0] & 0x0100) != 0x0100 ){
		Error_Handler();
		return 0xFFFFFFFF;
	}

	return (receive_data[0] << 24) | ( ( (uint8_t)receive_data[1] ) << 16 ) | ( ( (uint8_t)receive_data[2] ) << 8 ) | ( (uint8_t)receive_data[3] );
}



/**
  * @brief  Receive ADC data throught UARTx 4 bytes
  * @param  void
  * @retval 0 or 1 ,  1 = return error 
  */
ErrorStatus ADC_data_receive_UART(uint8_t receive_data[], uint8_t size_rec_data , USART_TypeDef *USARTx){

	uint32_t counter=0;
	uint16_t head_byte = 0;
	uint8_t i=0;
	
	/*if(LL_USART_IsActiveFlag_ORE(USARTx) == 1){
		LL_USART_ClearFlag_ORE(USARTx);
		LL_USART_ReceiveData9(USARTx); //???????????????
	}*/

	for( i = 0; i < size_rec_data; i++ ){
		counter=0;
		while( LL_USART_IsActiveFlag_RXNE(USARTx) == RESET ){
			counter++;
			if(counter==200000){
				Error_Handler();
				LL_USART_ClearFlag_ORE(USARTx);
				return ERROR;
			}
		}

		if( i == 0 ){
			head_byte = LL_USART_ReceiveData9(USARTx);
			receive_data[i] = (uint8_t)head_byte;
		}else{
			receive_data[i] = (uint8_t)LL_USART_ReceiveData9(USARTx);
		}
	}

	if( (head_byte & 0x0100) != 0x0100 ){
		Error_Handler();
		return ERROR;
	}

	return SUCCESS;
}


uint32_t Transfer_command_UART_9B(uint16_t tx_mass[], uint8_t size_tx_data, uint8_t size_rx_data,  USART_TypeDef *USARTx){

	ErrorStatus ret_tx;
	uint32_t ret_rx = 0;
	uint8_t i = 0;

    do{ // Two attempt to transfer command 
    	ret_tx = Data_transmite_UART_9B (tx_mass, size_tx_data, USARTx);

    	if(ret_tx == SUCCESS){
			ret_rx = Data_receive_UART_9B (size_rx_data , USARTx);
		}

		if(ret_tx == ERROR || ret_rx == 0xFFFFFFFF){
			i++;
		}

    }while( i != 2 || ret_rx != 0xFFFFFFFF); 
	
	if( i == 2 || ret_rx == 0xFFFFFFFF){
		return 0xFFFFFFFF;
	}else{
		return ret_rx;
	}

}

/*

uint32_t Transfer_ADC_data_UART_9B (uint16_t tx_mass[], uint8_t size_tx_data, uint8_t size_rx_data,  USART_TypeDef *USARTx){

	ErrorStatus ret_tx;
	uint32_t ret_rx = 0;
	uint8_t i = 0;

    ????

}*/



