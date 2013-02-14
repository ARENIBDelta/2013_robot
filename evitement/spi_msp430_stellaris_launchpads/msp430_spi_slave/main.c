#include <msp430.h>

/*
 * Test de communication SPI entre un kit EK-LM4F120XL et un kit
 * MSP-EXP430G2.
 * 2013-02-14 Erwan Martin <erwan@martin.tl>
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 */

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
  while (!(P1IN & BIT4));    // Attente SPI
  
  //Configuration sorties
  P1SEL = BIT1 + BIT2 + BIT4;
  P1SEL2 = BIT1 + BIT2 + BIT4;
  
  //Configuration UCA0
  UCA0CTL1 = UCSWRST; // **Put state machine in reset**
  UCA0CTL0 = UCCKPH | UCCKPL | UCSYNC | UCMSB; // 3-pin, 8-bit SPI slave, Phase and polarity
  UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
  IE2 |= UCA0RXIE; // Enable USCI0 RX interrupt

  while(1) {
      __bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, enable interrupts
  }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR (void)
{
  while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX buffer ready?
  unsigned int lol = UCA0RXBUF; //Reception
  UCA0TXBUF = lol; //Emission
}
