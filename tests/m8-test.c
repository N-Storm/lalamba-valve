#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>

int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
 
void USART_Init()
{
  /* Set baud rate to 57600 */
  UBRRL = 34;
  UCSRA = 1 << U2X;
  /* Enable receiver and transmitter */
  UCSRB = (1<<RXEN)|(1<<TXEN);
  /* Set frame format: 8N1 */
  UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
  /* Wait for empty transmit buffer */
  while ( !( UCSRA & (1<<UDRE)) );
  /* Put data into buffer, sends the data */
  UDR = data;
}

int uart_putchar(char c, FILE *stream)
{
  USART_Transmit((unsigned char)c);
  return 0;
}

int main() {
  DDRB = 1 << PB5;
  USART_Init();
  stdout = &mystdout;
  printf("Hello, world (test digit = %d)!\r\n", 123);
  USART_Transmit('A');
  while (1) {
    PORTB ^= 1 << PB5;
    USART_Transmit((PORTB & (1 << PB5) ? 'X' : 'O'));
    _delay_ms(1000);
  }
}
