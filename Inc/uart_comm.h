
#ifndef __UART_COMM_H
#define __UART_COMM_H
#ifdef __cplusplus
 extern "C" {
#endif

 extern void _Error_Handler(char *, int);

ErrorStatus Data_transmite_UART_9B (uint16_t mass[], USART_TypeDef *USARTx);


#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */


