/*
   CCS PICC 4.093 - MPLAB 8.83 (Release)
   PIC16F88 - 2MHZ (internal)   
   Compile as "Release"
*/

#include <16F88.h>
#FUSES NOWDT       //No Watch Dog Timer
#FUSES INTRC_IO    //Internal RC Osc
#FUSES NOPUT       //No Power Up Timer
#FUSES NOMCLR      //Master Clear pin used for I/O
#FUSES NOBROWNOUT  //Reset when brownout detected
#FUSES NOLVP       //No low voltage programing, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD       //No EE protection
#FUSES NOWRT       //Program memory not write protected
#FUSES NODEBUG     //Debug mode for use with ICD
#FUSES NOPROTECT   //Code not protected from reading
#FUSES NOFCMEN     //Fail-safe clock monitor disabled
#FUSES NOIESO      //Internal External Switch Over mode disabled

#use delay(clock=2M) //2MHz
/*
   RB0 <- MOUSE SX BUTTON INTERRUPT
   RB1 -> MOO OUTPUT
   RB2 <- CONJUMPER 
*/
#define CLICKINT  	PIN_B0
#define MOO  		PIN_B1
#define CONFJUMPER 	PIN_B2
#bit INTF_BIT = 0x0B.1 

#define CONF1 1000
#define CONF2 10000

int16 clickCount;
char got_interrupt; 

int16 getS() {
    if (input(CONFJUMPER)) {
		return CONF1;
    }
    return CONF2;
}

#INT_EXT
void ext_isr(void)
{
	got_interrupt = TRUE; 
	++clickCount;
	disable_interrupts(INT_EXT);  
}

void main(void)
{
    setup_oscillator(OSC_2MHZ);
    setup_adc_ports(NO_ANALOGS);                   
    setup_adc(ADC_OFF);                        
    setup_comparator(NC_NC_NC_NC);
	output_float(CLICKINT);
	output_drive(MOO);
	output_float(CONFJUMPER);
	ext_int_edge(L_TO_H); //click release
	output_low(MOO);
    
    clickCount = 0; 
    got_interrupt = FALSE;     
    INTF_BIT = 0; 
    enable_interrupts(INT_EXT);
    enable_interrupts(GLOBAL);
	
    while(1) {
		if (got_interrupt) {
			if (clickCount > getS()) {
				clickCount = 0;    
				output_hight(MOO);
				delay_ms(200);
				output_low(MOO);    
			} 
			got_interrupt = FALSE;
			delay_ms(50); //debounce
			INTF_BIT = 0; //clear interrupt
			enable_interrupts(INT_EXT); 
		}
	}
}

