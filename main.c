#include  <msp430x20x3.h>

#define   	LED_OUT         BIT0              	// Bit location for LED
#define		SCLK			BIT5				// Bit Location for SCLK
#define		SDO				BIT6				//
#define		SDI				BIT7				//
#define		PIR				BIT6				// Bit Location for PIR
#define   	SENSOR_PWR      BIT7              	// Bit location for power to sensor
#define   	THRESHOLD       50                	// Threshold for motion

static unsigned int result_old = 0;         // Storage for last conversion
static unsigned int motion=0;				// Variable for Motion status (no-motion =0,motion =1)
static unsigned int reed=0;					// Variable for Reed Status
static unsigned int prevreed=0;				// Previous reed status
static unsigned int test=0;
char LED_ENABLE = 1;                        // LED control

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD; 								// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;
	P1DIR = 0xFF;                             // Unused pins as outputs
	P2DIR = 0xFF;                             // Unused pins as outputs
	P1OUT = 0xFF;							//Pull SCLK High
	P2OUT = 0xFF;							//



/*  WDTCTL = WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL; // ACLK/32768, int timer: ~10s
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL3 |= LFXT1S_2;						// ACLK source VLO Clk
  BCSCTL1 |= DIVA_2;                        // ACLK = VLO/4


  P1OUT = 0x20;        		        	    // P1OUTs P1.5
  P1SEL = 0x08;                             // Select VREF function
  P1DIR = 0xEF;                             // Unused pins as outputs
  P1REN |= 0x10;		                    // P1.4 pullup
  //P1IE |= 0x10;                             // P1.4 interrupt enabled
  //P1IES |= 0x10;                            // P1.4 Hi/lo edge
  //P1IFG &= ~0x10;                           // P1.4 IFG cleared

  P2OUT = 0x40 + SENSOR_PWR;                // P2OUTs 2.6/2.7
  P2SEL &= ~(0x40+SENSOR_PWR);              // P2.6/P2.7 = GPIO
  P2DIR = 0xff;                             // Unused pins as outputs

  SD16CTL = SD16VMIDON + SD16REFON + SD16SSEL_1;// 1.2V ref, SMCLK
  SD16INCTL0 = SD16GAIN_4 + SD16INCH_4;     // PGA = 4x, Diff inputs A4- & A4+
  SD16CCTL0 =  SD16SNGL + SD16IE;           // Single conversion, 256OSR, Int enable
  SD16CTL &= ~SD16VMIDON;                   // VMID off: used to settle ref cap
  SD16AE = SD16AE1 + SD16AE2;               // P1.1 & P1.2: A4+/- SD16_A inputs

  // Wait for PIR sensor to settle: 1st WDT+ interval
  P1SEL |= LED_OUT;                         // Turn LED on with ACLK (for low Icc)
  while(!(IFG1 & WDTIFG));                  // ~5.4s delay: PIR sensor settling
  P1SEL &= ~LED_OUT;                        // Turn LED off with ACLK (for low Icc)

/*  // Reconfig WDT+ for normal operation: interval of ~341msec
  WDTCTL = WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+0x01;// ACLK/8192, int timer: 341msec*2=682ms
  BCSCTL1 &= ~DIVA_2;
  BCSCTL1 |= DIVA_0;                        // ACLK = VLO(12khz)/1
  IE1 |= WDTIE;                             // Enable WDT interrupt
*/
/*  //Initial Outputs
  P1OUT |= SCLK;							//Pull SCLK High
  P1OUT |= SDO;
  P1OUT |= SDI;
  P2OUT |= PIR;								//Pull PIR High
*/
  _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 with interrupts
}

/******************************************************
// SD16_A interrupt service routine
******************************************************/
/*#pragma vector = SD16_VECTOR
__interrupt void SD16ISR(void)
{ unsigned int result_new;

  SD16CTL &= ~SD16REFON;                    // Turn off SD16_A ref
  result_new = SD16MEM0;                    // Save result (clears IFG)

  if (result_new > result_old)              // Get difference between samples
    result_old = result_new - result_old;
  else
    result_old = result_old - result_new;

  if (result_old > THRESHOLD)               // If motion detected...
     {if (motion==1)
        {P1OUT |= LED_OUT;                   // Turn LED on
         motion=1;
        }
     else{
     P1OUT |= LED_OUT;                   // Turn LED on
	 P1OUT &= ~SCLK;						//Pull SCLK LOW
	 P2OUT &= ~PIR;						//Pull PIR LOW
	 __delay_cycles(5000);				//wait 5ms
	 P1OUT |= SCLK;						//Pull SCLK High
	 motion=1;}
     }
  else {
	 if (motion== 1)
	  {
	  P1OUT &= ~SCLK;						//Pull SCLK LOW
	  P2OUT |= PIR;							//Pull PIR High
	  __delay_cycles(5000);					//wait 5ms
	  P1OUT |= SCLK;						//Pull SCLK High
	  }
	 motion=0;
  }
  result_old = SD16MEM0;                    // Save last conversion

  __bis_SR_register_on_exit(SCG1+SCG0);     // Return to LPM3 after reti
}*/

/******************************************************
// Watchdog Timer interrupt service routine
******************************************************/

/*#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	/*test=P1IN;
	if ((P1IN&0x10)==0x10)
	{
		reed=1;
	}
	else
	{
		reed=0;
	}

	if (prevreed!=reed)
	{
		P1OUT &= ~SCLK;						//Pull SCLK LOW
		__delay_cycles(10000);				//wait 5ms
		P1OUT |= SCLK;						//Pull SCLK High
	}*/

 /* if (!(P1OUT & LED_OUT))                   // Has motion already been detected?
  {
    SD16CTL |= SD16REFON;                   // If no, turn on SD16_A ref
    SD16CCTL0 |= SD16SC;                    // Set bit to start new conversion
    prevreed=reed;
    __bic_SR_register_on_exit(SCG1+SCG0);   // Keep DCO & SMCLK on after reti
  }
  else
    P1OUT &= ~LED_OUT;                      // If yes, turn off LED, measure on next loop

  prevreed=reed;

}

// Port 1 interrupt service routine
/*#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	if ((P1IFG&0x10)==0x10) {
		P1OUT &= ~SCLK;						//Pull SCLK LOW
		__delay_cycles(10000);					//wait 5ms
		P1OUT |= SCLK;						//Pull SCLK High
		P1IFG &= ~0x10;							// P1.4 IFG cleared
		}

}*/
