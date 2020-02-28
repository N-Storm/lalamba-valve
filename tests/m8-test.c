#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

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


int main() {
  DDRB = 1 << PB5;
  USART_Init();
  USART_Transmit('A');
  while (1) {
    PORTB ^= 1 << PB5;
    USART_Transmit((PORTB & (1 << PB5) ? 'X' : 'O'));
    _delay_ms(1000);
  }
}
