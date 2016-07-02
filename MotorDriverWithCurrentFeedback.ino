
#include <stdio.h>
const int THROTTLE = A0;
const int CURRENT_READ = A7;
const float CURRENT_LOWER_LIMIT_AMP = 15.0f;                                       //[A]
const float CURRENT_UPPER_LIMIT_AMP = 20.0f;                                       //[A]
const float MOTOR_MAX_CURRENT_AMP = 240.0f;                                        //[A]
const float MOTOR_CURRENT_PER_DECIMAL = MOTOR_MAX_CURRENT_AMP;                     //[A/%]
const float MOTOR_CURRENT_PER_DECIMAL_REVERSED = 1 / MOTOR_CURRENT_PER_DECIMAL;    //[%/A]
const float ADC_SAMPLING = 1023.0f;
const float ADC_MAX_VOLTAGE = 5.0f;
volatile uint8_t &MOTOR_PWM = OCR0A;
const float PWM_SAMPLING_RANGE = 255.0f;
char buffer[40];
float decimal_pwm = 0.0f;

float convert_volts_to_amps(int adc_volts)
{
   /*
   *  f(0.5V) = -20A
   *  f(4.5V) = 20A
   *  f(U) = 10*U - 25 [A]
  */ 
  float adc_amps = adc_volts * 10.0f;
  return adc_amps - 25.0f;
}

float convert_adc_to_volts(float adc, float adc_voltage = ADC_MAX_VOLTAGE)
{
  /*
  * Converts adc value into voltage based on ADC_SAMPLING constant
  */
  float adc_decimal = adc / ADC_SAMPLING;
  float volts = adc_voltage * adc_decimal;
  return volts;
}

float convert_throttle_volts_to_current(float adc_volts, float current_limit = CURRENT_UPPER_LIMIT_AMP)
{
  /*
  * Converts volts (given the adc limit) to current based on the current limit so
  * a whole potentiometer range can be utilized to set the PWM.
  */
  float adc_voltage_decimal = adc_volts / ADC_MAX_VOLTAGE;
  return current_limit * adc_voltage_decimal;
}

float convert_throttle_adc_to_decimal(int adc)
{
  /*
  * Computes ADC in percentage given ADC_SAMPLING
  */
  float adc_decimal = adc / ADC_SAMPLING;
  return adc_decimal;
}

float compute_overcurrent_pwm_decimal(float current)
{
  /*
  * Compute overcurrent and return a percentage of PWM
  * that needs to be reduced.
  * current - total sensored current
  * return value - percentage of PWM that should be substracted
  *                from the 100%
  */
  float overcurrent = current - CURRENT_UPPER_LIMIT_AMP;
  return overcurrent * MOTOR_CURRENT_PER_DECIMAL_REVERSED;
}

float compute_current_pwm_decimal(float current)
{
  /*
  * Compute percentage of PWM at given current knowing that
  * the current cannot be exceeded (compared to the max value).
  */
   return current * MOTOR_CURRENT_PER_DECIMAL_REVERSED;
}

uint8_t convert_decimal_to_pwm(float percentage)
{
  /*
  * Converts PWM in percentage to PWM in range of the given
  * PWM.
  * User needs to set PWM_SAMPLING_RANGE to 255 or (2^16)-1.
  */
  return percentage * PWM_SAMPLING_RANGE;
}

float convert_pwm_to_decimal(int pwm)
{
  /*
  *  Converts PWM into percentage given PWM_SAMPLING_RANGE
  */
  float pwm_decimal = pwm / PWM_SAMPLING_RANGE;
  return pwm_decimal;
}



void pwm_init()
{
  /*
  * Inits PWM:
  * Prescaler 1
  * @PD6
  * FastPWM
  * Initial value: 0
  */
  DDRD |= 1<<PD6 | 1<<PD5;
  TCCR0A |= 1<<COM0A1 | 1<<WGM01 | 1<<WGM00;
  TCCR0B |= 1<<CS00;
  MOTOR_PWM = 0;
}

void setup()
{
  Serial.begin(9600);
  pwm_init();
//pinMode(6, OUTPUT);
}

void loop()
{
  int throttle_adc = analogRead(THROTTLE);                                      //0-1023
  int current_adc_voltage_read = analogRead(CURRENT_READ);                      //0-1023
  float current_in_voltage = convert_adc_to_volts(current_adc_voltage_read);
  float current = convert_volts_to_amps(current_in_voltage);
  
  if(current > CURRENT_UPPER_LIMIT_AMP)
  {    
    float decimal = convert_pwm_to_decimal(MOTOR_PWM);
    float overcurrent_decimal = compute_overcurrent_pwm_decimal(current);
    decimal_pwm = decimal - overcurrent_decimal;
  }
  else
  {
    if(current < -5.0f)
      decimal_pwm = 0.0f;
     else
       decimal_pwm = convert_throttle_adc_to_decimal(throttle_adc);
  }

    MOTOR_PWM = convert_decimal_to_pwm(decimal_pwm);;
  
  int current_mamps = 1000.0f * current;
  sprintf(buffer, "%d,%d,%d", throttle_adc, current_adc_voltage_read, current_mamps);                        //0-1023, 0-1023, mA
  Serial.println(buffer);

}
