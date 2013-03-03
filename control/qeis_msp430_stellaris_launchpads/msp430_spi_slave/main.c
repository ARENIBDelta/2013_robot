#include <msp430g2553.h>

/*
 * Communication SPI entre un kit EK-LM4F120XL et un kit
 * MSP-EXP430G2.
 * 2013-02-14 Erwan Martin <erwan@martin.tl>
 *
 * Deux commandes distinctes permettent de récupérer soit
 * 12 octets (quadratures) soit 12 octets (erreurs).
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 */

unsigned char spi_buffer[13];
unsigned char *spi_p = spi_buffer;
unsigned char spi_state = 0;

int main(void) {
  WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
  while (!(P1IN & BIT4));
  
  //Pineout
  P1SEL = BIT1 + BIT2 + BIT4;
  P1SEL2 = BIT1 + BIT2 + BIT4;

  //Config SPI
  UCA0CTL1 = UCSWRST;                    //Reste state machine
  UCA0CTL0 |= UCCKPL | UCMSB + UCSYNC;   //Config SPI
  UCA0CTL1 &= ~UCSWRST;                  //Start state machine
  IE2 |= UCA0RXIE | UCA0TXIE;            //Enable RX + TX interrupts

  while(1) __bis_SR_register(LPM0_bits + GIE);
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR (void)
{
  static unsigned char data;
  data = UCA0RXBUF;   //reception

  if (data == 0x55) {
  	//On demande les quadratures
  	spi_buffer[0]  = 0x11;
  	spi_buffer[1]  = 0x22;
  	spi_buffer[2]  = 0x33;
  	spi_buffer[3]  = 0x44;
  	spi_buffer[4]  = 0x55;
  	spi_buffer[5]  = 0x66;
  	spi_buffer[6]  = 0x77;
  	spi_buffer[7]  = 0x88;
  	spi_buffer[8]  = 0x99;
  	spi_buffer[9]  = 0xAA;
  	spi_buffer[10] = 0xBB;
  	spi_buffer[11] = 0xCC;
  	spi_p = spi_buffer;
  	spi_state = 1;
  	UCA0TXBUF = *spi_p;
  	spi_p++;
  }
  if (data == 0xAA) {
  	//On demande les erreurs
  	spi_buffer[0]  = 0x01;
  	spi_buffer[1]  = 0x02;
  	spi_buffer[2]  = 0x04;
  	spi_buffer[3]  = 0x04;
  	spi_buffer[4]  = 0x05;
  	spi_buffer[5]  = 0x06;
  	spi_buffer[6]  = 0x07;
  	spi_buffer[7]  = 0x08;
  	spi_buffer[8]  = 0x09;
  	spi_buffer[9]  = 0x0A;
  	spi_buffer[10] = 0x0B;
  	spi_buffer[11] = 0x0C;
  	spi_p = spi_buffer;
  	spi_state = 1;
  	UCA0TXBUF = *spi_p;
  	spi_p++;
  }
}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR (void)
{
  //Envoi du caractère suivant du buffer
  unsigned char data;
  data = *spi_p;
  
  //Si on dépasse, on retourne au début
  if (spi_p++ - spi_buffer >= 12) {
    spi_p = spi_buffer;
  }
  UCA0TXBUF = data;
}
