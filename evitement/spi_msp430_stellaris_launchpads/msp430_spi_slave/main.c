#include <msp430g2553.h>
#include "layer2p.h"

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

int truc;
void test_received(unsigned char *data, unsigned int size) {
	//Attention: l'alignement et la taille des ints ne sont pas 
	//les mêmes que sur le stellaris
	struct some_data {
		char chars[5];
		unsigned int a1;
		unsigned int a2;
		unsigned int a3;
	} *data2;
	data2 = (struct some_data*) data;

	data2->a3++;
}
void test_errored(void) {
	truc++;
}

layer2p_ctx ctx;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer

	layer2p_ctx_init(&ctx);
	layer2p_set_callback(&ctx, &test_received);
	layer2p_set_error_callback(&ctx, &test_errored);
	layer2p_reset(&ctx);

  while (!(P1IN & BIT4));    // Attente SPI

  //Configuration sorties
  P1SEL = BIT1 + BIT2 + BIT4;
  P1SEL2 = BIT1 + BIT2 + BIT4;
  
  //Configuration UCA0
  UCA0CTL1 = UCSWRST; // **Put state machine in reset**
  UCA0CTL0 |= UCCKPH | UCCKPL | UCSYNC | UCMSB; // 3-pin, 8-bit SPI slave, Phase and polarity
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
  unsigned int data = UCA0RXBUF; //Reception
  UCA0TXBUF = data; //Emission
  layer2p_receive(&ctx, data);
}

