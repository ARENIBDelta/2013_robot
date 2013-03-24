#include <msp430g2553.h>

/*
 * Communication SPI entre un kit EK-LM4F120XL et un kit
 * MSP-EXP430G2.
 * 2013-02-14 Erwan Martin <erwan@martin.tl>
 *
 * Deux commandes distinctes permettent de récupérer soit
 * 12 octets (quadratures) soit 12 octets (erreurs).
 * 
 * Le stellaris peut simuler une entrée quadrature sur
 * P2.0 et P2.1 
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 * PE1 (PE1)    -> P1.5 (P1.5)
 * PE2 (PE2)    -> P2.0 (P2.0)
 * PE3 (PE3)    -> P2.1 (P2.1)
 */

#define ETAT_1 1
#define ETAT_2 2
#define ETAT_3 3
#define ETAT_4 4

unsigned char spi_buffer[6];
unsigned char *spi_p = spi_buffer;
unsigned char spi_state = 0;

unsigned int qs[3];
unsigned int etats[3];
unsigned int errs[3];

void handle_input(unsigned char index, unsigned char inputs) {
	if ( qs[index] == 65535 ){ 
		qs[index] = 0;
	}

    switch (etats[index]){
    case ETAT_1:
    	if (inputs == 0x02){
    		qs[index]++;
    		etats[index] = 2;
    		break;
    	}
		if (inputs == 0x01){
			qs[index]--;
			etats[index] = 4;
			break;
		}
		if (inputs & 0x03){
			errs[index]++;
			etats[index] = 3;
			break;
		}
		break;
    case ETAT_2:
    	if (inputs == 0x03){
    		qs[index]++;
    		etats[index] = 3;
    		break;
    	}
		if (inputs == 0x00){
			qs[index]--;
			etats[index] = 1;
			break;
		}
		if (inputs == 0x01){
			errs[index]++;
			etats[index] = 4;
			break;
		}
		break; 
    case ETAT_3:
    	if (inputs == 0x01){
    		qs[index]++;
    		etats[index] = 4;
    		break;
    	}
		if (inputs == 0x02){
			qs[index]--;
			etats[index] = 2;
			break;
		}
		if (inputs == 0x00){
			errs[index]++;
			etats[index] = 1;
			break;
		}
		break;    	
    case ETAT_4:
    	if (inputs == 0x00){
    		qs[index]++;
    		etats[index] = 1;
    		break;
    	}
		if (inputs == 0x03){
			qs[index]--;
			etats[index] = 3;
			break;
		}
		if (inputs == 0x02){
			errs[index]++;
			etats[index] = 2;
			break;
		}
		break;
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {

  if (P1IFG & BIT5) {
	  //Config SPI
	  UCA0CTL1 = UCSWRST;                    //Reset state machine
	  UCA0CTL0 |= UCCKPL | UCMSB + UCSYNC;   //Config SPI
	  UCA0CTL1 &= ~UCSWRST;                  //Start state machine
	  IE2 |= UCA0RXIE | UCA0TXIE;            //Enable RX + TX interrupts

	  spi_p = spi_buffer;

	  P1IFG &= ~BIT5;                        //Clear interrupt
  }
//  if (P1IFG & BIT6) {
//	  UCA0CTL1 = UCSWRST;
//	  P1IFG &= ~BIT6;
//  }
}

int main(void) {
  WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
  
  DCOCTL = CALDCO_16MHZ;       // DCO frequency set to 8 MHz
  BCSCTL1 = CALBC1_16MHZ;      // DCO range set to 8 MHz
  
  //Pineout SPI
  P1SEL = BIT1 + BIT2 + BIT4;
  P1SEL2 = BIT1 + BIT2 + BIT4;
  //Config CS SPI
  P1IES |= BIT5;            // high -> low is selected with IES.x = 1.
  P1IFG &= ~(BIT5);  // To prevent an immediate interrupt, clear the flag for
                            // P1.5 before enabling the interrupt.
  P1IE |= BIT5;      // Enable interrupts for P1.5 and P1.6


  //Init values
  unsigned char i;
  for(i=3; i--; ) {
  	qs[i] = 10000;
  	errs[i] = 0;
  	etats[i] = 1;
  }

  __enable_interrupt();
  
  while(1) { 
  	handle_input(0, P2IN & 0x03); //P2.0 + P2.1
  	handle_input(1, (P2IN >> 2) & 0x03); //P2.2 + P2.3
  	handle_input(2, (P2IN >> 4) & 0x03); //P2.4 + P2.5
  }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR (void)
{
  static unsigned char data;
  data = UCA0RXBUF;   //reception

  if (data == 0x55) {
  	//On demande les quadratures
  	spi_buffer[0]  = qs[0] & 0xFF;
  	spi_buffer[1]  = (qs[0] >> 8) & 0xFF;
  	spi_buffer[2]  = qs[1] & 0xFF;
  	spi_buffer[3]  = (qs[1] >> 8) & 0xFF;
  	spi_buffer[4]  = qs[2] & 0xFF;
  	spi_buffer[5]  = (qs[2] >> 8) & 0xFF;
  	spi_p = spi_buffer;
  	spi_state = 1;
  	UCA0TXBUF = *spi_p;
  	spi_p++;
  }
  if (data == 0xAA) {
  	//On demande les erreurs
  	spi_buffer[0]  = errs[0] & 0xFF;
  	spi_buffer[1]  = (errs[0] >> 8) & 0xFF;
  	spi_buffer[2]  = errs[1] & 0xFF;
  	spi_buffer[3]  = (errs[1] >> 8) & 0xFF;
  	spi_buffer[4]  = errs[2] & 0xFF;
  	spi_buffer[5]  = (errs[2] >> 8) & 0xFF;
  	spi_p = spi_buffer;
  	spi_state = 1;
  	UCA0TXBUF = *spi_p;
  	spi_p++;
  }
  if (data == 0xBB) {
  	//Reset quadratures
	qs[0] = 9700; 
	qs[1] = 9700; 
	qs[2] = 9700; 
  	UCA0TXBUF = 0;
  }
}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR (void)
{
  //Envoi du caractère suivant du buffer
  unsigned char data;
  data = *spi_p;
  
  //Si on dépasse, on retourne au début
  if (spi_p++ - spi_buffer >= 6) {
    spi_p = spi_buffer;
  }
  UCA0TXBUF = data;
}
