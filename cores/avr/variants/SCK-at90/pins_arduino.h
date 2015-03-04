/**

  PINS DEFINITIONS FOR AT90USB1286/7
  pins_arduino.h - Pin definition functions for Arduino

  @author
  Parker Dillmann
  
  @author
  C. A. Church 

  @version
  Pral2a

*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

//I2C Mapping
static const uint8_t SDA = 25;
static const uint8_t SCL = 24;

//SPI Port Mapping
static const uint8_t SS   = 8; 
static const uint8_t MOSI = 10;
static const uint8_t MISO = 11;
static const uint8_t SCK  = 9;


// Mapping of analog pins as digital I/O
// A6-A7 share with digital pins
static const uint8_t A0 = 0;
static const uint8_t A1 = 1;
static const uint8_t A2 = 2;
static const uint8_t A3 = 3;
static const uint8_t A4 = 4;
static const uint8_t A5 = 5;
static const uint8_t A6 = 6;
static const uint8_t A7 = 7;

#define TX_RX_LED_INIT	DDRD |= (1<<4), DDRD |= (1<<5)
#define TXLED0			PORTD |= (1<<4)
#define TXLED1			PORTD &= ~(1<<4)
#define RXLED0			PORTD |= (1<<5)
#define RXLED1			PORTD &= ~(1<<5)

 // CC: PCInt is only available on PORTB (digital 8-15)
//#define digitalPinToPCICR(p)    ((p) >= 8 && (p) <= 15) ? (&PCICR) : ((uint8_t *)0)
//#define digitalPinToPCICRbit(p) 0
//#define digitalPinToPCMSK(p)    ((p) >= 8 && (p) <= 15) ? (&PCIMSK0) : ((uint8_t *)0) 
//#define digitalPinToPCMSKbit(p) ((p) - 8) 

#define digitalPinToPCICR(p)	(((p) >= 20 && (p) <= 27) ? &PCICR : NULL)
#define digitalPinToPCICRbit(p)	(0)
#define digitalPinToPCMSK(p)	(((p) >= 20 && (p) <= 27) ? &PCMSK0 : NULL)
#define digitalPinToPCMSKbit(p)	(((p) - 20) & 7)

extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &DDRA,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	(uint16_t) &DDRE,
	(uint16_t) &DDRF,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE,
	(uint16_t) &PORTF,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	(uint16_t) &PINA,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	(uint16_t) &PINE,
	(uint16_t) &PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[48] = {
	PA, // D0  - PA0
	PA,	// D1  - PA1
	PA, // D2  - PA2
	PA,	// D3  - PA3
	PA,	// D4  - PA4
	PA, // D5  - PA5
	PA, // D6  - PA6
	PA, // D7  - PA7
	
	PB, // D8  - PB0
	PB,	// D9  - PB1
	PB, // D10 - PB2
	PB,	// D11 - PB3
	PB,	// D12 - PB4
	PB, // D13 - PB5
	PB, // D14 - PB6
	PB, // D15 - PB7
	
	PC, // D16 - PC0
	PC,	// D17 - PC1
	PC, // D18 - PC2
	PC,	// D19 - PC3
	PC,	// D20 - PC4
	PC, // D21 - PC5
	PC, // D22 - PC6
	PC, // D23 - PC7
	
	PD, // D24 - PD0
	PD,	// D25 - PD1
	PD, // D26 - PD2
	PD,	// D27 - PD3
	PD,	// D28 - PD4
	PD, // D29 - PD5
	PD, // D30 - PD6
	PD, // D31 - PD7
	
	PE, // D32 - PE0
	PE,	// D33 - PE1
	PE, // D34 - PE2
	PE,	// D35 - PE3
	PE,	// D36 - PE4
	PE, // D37 - PE5
	PE, // D38 - PE6
	PE, // D39 - PE7
	
	PF, // D40 - A0 - PF0
	PF,	// D41 - A1 - PF1
	PF, // D42 - A2 - PF2
	PF,	// D43 - A3 - PF3
	PF,	// D44 - A4 - PF4
	PF, // D45 - A5 - PF5
	PF, // D46 - A6 - PF6
	PF, // D47 - A7 - PF7
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[48] = {
	_BV(0), // D0  - PA0
	_BV(1),	// D1  - PA1
	_BV(2), // D2  - PA2
	_BV(3),	// D3  - PA3
	_BV(4),	// D4  - PA4
	_BV(5), // D5  - PA5
	_BV(6), // D6  - PA6
	_BV(7), // D7  - PA7
	
	_BV(0), // D8  - PB0
	_BV(1),	// D9  - PB1
	_BV(2), // D10 - PB2
	_BV(3),	// D11 - PB3
	_BV(4),	// D12 - PB4
	_BV(5), // D13 - PB5
	_BV(6), // D14 - PB6
	_BV(7), // D15 - PB7
	
	_BV(0), // D16 - PC0
	_BV(1),	// D17 - PC1
	_BV(2), // D18 - PC2
	_BV(3),	// D19 - PC3
	_BV(4),	// D20 - PC4
	_BV(5), // D21 - PC5
	_BV(6), // D22 - PC6
	_BV(7), // D23 - PC7
	
	_BV(0), // D24 - PD0
	_BV(1),	// D25 - PD1
	_BV(2), // D26 - PD2
	_BV(3),	// D27 - PD3
	_BV(4),	// D28 - PD4
	_BV(5), // D29 - PD5
	_BV(6), // D30 - PD6
	_BV(7), // D31 - PD7
	
	_BV(0), // D32 - PE0
	_BV(1),	// D33 - PE1
	_BV(2), // D34 - PE2
	_BV(3),	// D35 - PE3
	_BV(4),	// D36 - PE4
	_BV(5), // D37 - PE5
	_BV(6), // D38 - PE6
	_BV(7), // D39 - PE7
	
	_BV(0), // D40 - A0 - PF0
	_BV(1),	// D41 - A1 - PF1
	_BV(2), // D42 - A2 - PF2
	_BV(3),	// D43 - A3 - PF3
	_BV(4),	// D44 - A4 - PF4
	_BV(5), // D45 - A5 - PF5
	_BV(6), // D46 - A6 - PF6
	_BV(7), // D47 - A7 - PF7
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[48] = {
	NOT_ON_TIMER,	//PA0	
	NOT_ON_TIMER,	//PA1
	NOT_ON_TIMER,	//PA2
	NOT_ON_TIMER,	//PA3
	NOT_ON_TIMER,	//PA4
	NOT_ON_TIMER,	//PA5
	NOT_ON_TIMER,	//PA6
	NOT_ON_TIMER,	//PA7
	
	NOT_ON_TIMER,	//PB0
	NOT_ON_TIMER,	//PB1
	NOT_ON_TIMER,	//PB2
	NOT_ON_TIMER,	//PB3
	TIMER2A,		//PB4
	TIMER1A,		//PB5
	TIMER1B,		//PB6
	TIMER0A,		//PB7
	
	NOT_ON_TIMER,	//PC0
	NOT_ON_TIMER,	//PC1
	NOT_ON_TIMER,	//PC2
	NOT_ON_TIMER,	//PC3
	TIMER3C,		//PC4
	TIMER3B,		//PC5
	TIMER3A,		//PC6
	NOT_ON_TIMER,	//PC7
	
	TIMER0B,		//PD0	
	TIMER2B,		//PD1
	NOT_ON_TIMER,	//PD2
	NOT_ON_TIMER,	//PD3
	NOT_ON_TIMER,	//PD4
	NOT_ON_TIMER,	//PD5
	NOT_ON_TIMER,	//PD6
	NOT_ON_TIMER,	//PD7
	
	NOT_ON_TIMER,	//PE0	
	NOT_ON_TIMER,	//PE1
	NOT_ON_TIMER,	//PE2
	NOT_ON_TIMER,	//PE3
	NOT_ON_TIMER,	//PE4
	NOT_ON_TIMER,	//PE5
	NOT_ON_TIMER,	//PE6
	NOT_ON_TIMER,	//PE7
	
	NOT_ON_TIMER,	//PF0	
	NOT_ON_TIMER,	//PF1
	NOT_ON_TIMER,	//PF2
	NOT_ON_TIMER,	//PF3
	NOT_ON_TIMER,	//PF4
	NOT_ON_TIMER,	//PF5
	NOT_ON_TIMER,	//PF6
	NOT_ON_TIMER,	//PF7
};

const uint8_t PROGMEM analog_pin_to_channel_PGM[8] = {
	0,	// A0		PF0					ADC0
	1,	// A1		PF1					ADC1	
	2,	// A2		PF2					ADC2	
	3,	// A3		PF3					ADC3
	4,	// A4		PF4					ADC4	
	5,	// A5		PF5					ADC5	
	6,	// A6		PF6					ADC6
	7,	// A7		PF7					ADC7
};

#endif /* ARDUINO_MAIN */
#endif /* Pins_Arduino_h */