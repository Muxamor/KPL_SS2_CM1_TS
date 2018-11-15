
#ifndef __UART_COMM_H
#define __UART_COMM_H
#ifdef __cplusplus
 extern "C" {
#endif

 extern void _Error_Handler(char *, int);

ErrorStatus Data_transmite_UART_9B(uint16_t mass[], uint8_t size_parcel, USART_TypeDef *USARTx);
uint32_t Data_receive_UART_9B(uint8_t size_rec_data , USART_TypeDef *USARTx);
ErrorStatus ADC_data_receive_UART(uint8_t receive_data[], uint8_t size_rec_data , USART_TypeDef *USARTx);
uint32_t Transfer_command_UART_9B(uint16_t tx_mass[], uint8_t size_tx_data, uint8_t size_rx_data,  USART_TypeDef *USARTx);
ErrorStatus Transfer_ADC_data_UART_9B (uint16_t tx_mass[], uint8_t size_tx_data, uint8_t rx_ADC_data[], uint8_t size_rx_data, uint8_t addr_analog_module, USART_TypeDef *USARTx);



#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */


