#define F_CPU 8000000
#include "motoare.h"
#include <avr/io.h>
#include <avr/delay.h>
#define WHEEL_PWM_PERIOD 0xff
// #define LEFT_WHEEL_DUTY 0x7D
// #define RIGHT_WHEEL_DUTY 0x7D
#define LEFT_DUTY  70
#define RIGHT_DUTY 70


uint8_t reverse_flag_left=0;
uint8_t reverse_flag_right=0;

void motor_dir_port_init(){
	
	/* left wheel */
	PORTE.DIR |= PIN3_bm;
	PORTE.DIR |= PIN2_bm;
	
	PORTE.OUT &= ~PIN2_bm;
	PORTE.OUT |= PIN3_bm;
	
	/* right wheel */
	PORTF.DIR |= PIN6_bm;
	PORTF.DIR |= PIN4_bm;
	
	PORTF.OUT &= ~PIN6_bm;
	PORTF.OUT |= PIN4_bm;
}

void LEFT_WHEEL_TCB3_PWM_init (void)
{
	
	PORTB_DIR |= PIN5_bm;
	PORTB_OUT |= PIN5_bm;
	
	/* Load CCMP register with the period and duty cycle of the PWM */
	//TCB3.CCMP = TCB_CMP_EXAMPLE_VALUE;
	TCB3.CCMPL = WHEEL_PWM_PERIOD; // period of pwm
	TCB3.CCMPH = 0;
	//TCB3.CCMPH = left_pwm_duty(); // duty cycle
	/* Enable TCB3 and Divide CLK_PER by 2 */
	TCB3.CTRLA |= TCB_ENABLE_bm;
	

	/* Enable Pin Output and configure TCB in 8-bit PWM mode */
	TCB3.CTRLB |= TCB_CCMPEN_bm;
	TCB3.CTRLB |= TCB_CNTMODE_PWM8_gc;
}

void RIGHT_WHEEL_TCB1_PWM_init(void){
	PORTF.DIR |= PIN5_bm;
	PORTF.OUT |= PIN5_bm;
	/* Load CCMP register with the period and duty cycle of the PWM */

	TCB1.CCMPL = WHEEL_PWM_PERIOD; // period of pwm
	TCB1.CCMPH = 0;
	//TCB1.CCMPH = right_pwm_duty(); // duty cycle
	/* Enable TCB3 and Divide CLK_PER by 2 */
	TCB1.CTRLA |= TCB_ENABLE_bm;
	
	/* Enable Pin Output and configure TCB in 8-bit PWM mode */
	TCB1.CTRLB |= TCB_CCMPEN_bm;
	TCB1.CTRLB |= TCB_CNTMODE_PWM8_gc;
	PORTMUX.TCBROUTEA |= PIN1_bm;
	
}

void motor_start_left_wheel(uint8_t left_pwm){
	
	//TCB3.CTRLA &= ~PIN0_bm;  // disable TCB3
	//TCB3.CCMPL = 0xff;
	// go forward
	//if(reverse_flag_left==1){
	PORTE.OUT &= ~PIN2_bm;
	PORTE.OUT |= PIN3_bm;
	reverse_flag_left=1;
	//}
	
	TCB3.CCMPH = left_pwm_duty(left_pwm);
	//TCB3.CCMPH = left_pwm_duty(); // change duty cycle
	//TCB3.CNT = 0x00;
	//TCB3.CTRLA |=  TCB_ENABLE_bm; // re-enable TCB3;
}


void motor_stop_left_wheel(){
	TCB3.CCMPH = 0; // change duty cycle
}


void motor_start_right_wheel(uint8_t right_pwm){
	
	//if(reverse_flag_right==1){
	PORTF.OUT &= ~PIN6_bm;
	PORTF.OUT |= PIN4_bm;
	reverse_flag_right=0;
	//}
	TCB1.CCMPH = right_pwm_duty(right_pwm);
}


void motor_stop_right_wheel(){
	TCB1.CCMPH = 0; // change duty cycle
}

void motor_reverse_left_wheel(uint8_t left_pwm){
	
	PORTE.OUT &= ~PIN3_bm;
	PORTE.OUT |= PIN2_bm;
	
	TCB3.CCMPH = left_pwm_duty(left_pwm);
	reverse_flag_left=1;
}

void motor_reverse_right_wheel(uint8_t right_pwm){
	
	PORTF.OUT &= ~PIN4_bm;
	PORTF.OUT |= PIN6_bm;
	
	TCB1.CCMPH = right_pwm_duty(right_pwm);
	reverse_flag_right=1;
}





uint8_t left_pwm_duty(uint8_t left_duty){
	
	int duty=left_duty;
	duty=(float)duty/100  * WHEEL_PWM_PERIOD;
	
	return duty;
}

uint8_t right_pwm_duty(uint8_t right_duty){
	
	int duty=right_duty;
	duty=(float)duty/100  * WHEEL_PWM_PERIOD;
	
	return duty;
}