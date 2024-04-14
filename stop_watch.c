#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char sm=0, mh=0 ,hours_1=0 , hours_0=0 , minutes_1=0 , minutes_0 = 0 , second_0=0 , second_1=0 , CNT = 0 ;

void reset(void){
  // Configure PD2 (INT0) pin for input
  DDRD &= ~(1<<PD2);
  // Enable external interrupt INT0
  GICR |= (1<<INT0);
  // Configure INT0 to trigger on falling edge
  MCUCR |=(1<<ISC01);
  MCUCR &= ~(1<<ISC00);
  // Enable global interrupts
  SREG |= (1<<7);
}

void pause(void){
  // Configure PD3 (INT1) pin for input
  DDRD &= ~(1<<PD3);
  // Enable external interrupt INT1
  GICR |= (1<<INT1);
  // Configure INT1 to trigger on falling edge
  MCUCR &= ~(1<<ISC10);
  MCUCR |=(1<<ISC11);
  // Enable global interrupts
  SREG |= (1<<7);
}

void Resume(void){
  // Configure PB2 (INT2) pin for input
  DDRB &= ~(1<<PB2); 
  // Enable external interrupt INT2
  GICR |= (1<<INT2);
  // Configure INT2 to trigger on falling edge
  MCUCSR &= ~(1<<ISC2);
  // Enable global interrupts
  SREG |= (1<<7);
}

void Timer_init(void){
  // Set initial value for the timer
  TCNT0 = 6;
  // Configure Timer0: Normal mode, Prescaler 1024
  TCCR0 |= (1<<CS02) | (1<<CS00);
  // Enable Timer0 overflow interrupt
  TIMSK |= (1<<TOIE0);
  // Enable global interrupts
  SREG |= (1<<7);
}

void seven_seg_init(void){
  // Configure first 4 bits of port C for output
  DDRC = 0x0F;
  // Configure first 6 bits of port A for output
  DDRA = 0x3F;
  // Initialize port C to 0
  PORTC = 0;
}

void seven_seg_display(void){
  // Display hours, minutes, and seconds on the 7-segment display
  PORTA = (1<<PA0);
  PORTC = (PORTC&0xF0) | ( second_0&0x0F);
  _delay_ms(2);

  PORTA = (1<<PA1);
  PORTC = (PORTC&0xF0) | ( second_1&0x0F);
  _delay_ms(2);

  PORTA = (1<<PA2);
  PORTC = (PORTC&0xF0) | (minutes_0&0x0F);
  _delay_ms(2);

  PORTA = (1<<PA3);
  PORTC = (PORTC&0xF0) | (minutes_1&0x0F);
  _delay_ms(2);

  PORTA = (1<<PA4);
  PORTC = (PORTC&0xF0) | (hours_0&0x0F);
  _delay_ms(2);

  PORTA = (1<<PA5);
  PORTC = (PORTC&0xF0) | (hours_1&0x0F);
  _delay_ms(2);
}

ISR(TIMER0_OVF_vect){
  // Reload Timer0
  TCNT0 = 6;
  // Increment counter
  CNT++;
  // Increment seconds and minutes accordingly
  if(CNT == 4){
    second_0++;
    sm++;
    if(second_0 == 10){
      second_1++;
      second_0=0;
    }
    CNT=0;
  }
  if(sm == 60){
    sm=0;
    second_1=0;
    mh++;
    minutes_0++;
    if(minutes_0 == 10){
      minutes_1++;
      minutes_0=0;
    }
  }
  if(mh == 60){
    mh=0;
    minutes_1=0;
    hours_0++;
    if(hours_0 == 10){
      hours_1++;
      hours_0=0;
    }
  }
}

// Reset button ISR
ISR(INT0_vect){
  // Reset all time variables
  hours_1=0;
  hours_0=0;
  minutes_1=0;
  second_0=0;
  second_1=0;
  minutes_0=0;
  sm=0;
  mh=0;
  CNT=0;
}

// Pause button ISR
ISR(INT1_vect){
  // Disable Timer0 [NO,clk]
  TCCR0 = 0;
}

// Resume button ISR
ISR(INT2_vect){
  // Enable Timer0
  TCCR0 = (1<<CS02) | (1<<CS00);
}

int main(void) {
  // Initialize 7-segment display, buttons, and timer
  seven_seg_init();
  reset();
  pause();
  Resume();
  Timer_init();  

  // Main loop to continuously display time on 7-segment display
  while (1) {
    seven_seg_display();   
  }

  return 0;
}
