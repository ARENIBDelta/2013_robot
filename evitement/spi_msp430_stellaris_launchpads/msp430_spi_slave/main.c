#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  while (!(P1IN & BIT4));
  
  P1SEL = BIT1 + BIT2 + BIT4;
  P1SEL2 = BIT1 + BIT2 + BIT4;
  UCA0CTL1 = UCSWRST;                       // **Put state machine in reset**
  UCA0CTL0 = UCCKPH | UCCKPL | UCSYNC | UCMSB;      // 3-pin, 8-bit SPI slave
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI0 RX interrupt

	while(1) {
  		__bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, enable interrupts
	}
}

// Echo character
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR (void)
{
  while (!(IFG2 & UCA0TXIFG));              // USCI_A0 TX buffer ready?
  unsigned int lol = UCA0RXBUF;
  UCA0TXBUF = lol;
}
