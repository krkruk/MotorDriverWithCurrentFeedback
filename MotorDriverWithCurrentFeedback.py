# THROTTLE = A0;
# CURRENT_READ = A7;
CURRENT_LOWER_LIMIT_AMP = 20
CURRENT_UPPER_LIMIT_AMP = 30
MOTOR_MAX_CURRENT_AMP = 240
MOTOR_CURRENT_PER_PERCENT = MOTOR_MAX_CURRENT_AMP * 0.01
MOTOR_CURRENT_PER_PERCENT_REVERSED = 1 / MOTOR_CURRENT_PER_PERCENT;
ADC_SAMPLING = 1024-1;
ADC_MAX_VOLTAGE = 5.0;
PWM_SAMPLING_RANGE = 255;

def convert_volts_to_amps(adc_volts):
    """
    *  f(0.5V) = -20A
    *  f(4.5V) = 20A
    *  f(U) = 10*U - 25 [A]
    """
    return 10 * adc_volts - 25

def convert_adc_to_volts(adc, adc_voltage = ADC_MAX_VOLTAGE):
    """
    * Converts adc value into voltage based on ADC_SAMPLING constant
    """

    return (adc * adc_voltage) / ADC_SAMPLING

def convert_throttle_volts_to_current(adc_volts, current_limit = CURRENT_UPPER_LIMIT_AMP):
    """
    * Converts volts (given the adc limit) to current based on the current limit so
    * a whole potentiometer range can be utilized to set the PWM.
    """
    return adc_volts * current_limit / ADC_MAX_VOLTAGE


def convert_throttle_adc_to_percentage(adc):
  return adc * 100.0 / ADC_SAMPLING


def compute_overcurrent_pwm_percent(current):
    """
    * Compute overcurrent and return a percentage of PWM
    * that needs to be reduced.
    * current - total sensored current
    * return value - percentage of PWM that should be substracted
    *                from the 100%
    """
    overcurrent = current - CURRENT_UPPER_LIMIT_AMP
    return overcurrent * MOTOR_CURRENT_PER_PERCENT_REVERSED

def compute_current_pwm_percent(current):
    """
    * Compute percentage of PWM at given current knowing that
    * the current cannot be exceeded (compared to the max value).
    """
    return current * MOTOR_CURRENT_PER_PERCENT_REVERSED

def convert_percentage_to_pwm(percentage):
    """
    * Converts PWM in percentage to PWM in range of the given
    * PWM.
    * User needs to set PWM_SAMPLING_RANGE to 255 or (2^16)-1.
    """
    return percentage * PWM_SAMPLING_RANGE

\
def convert_pwm_to_percentage(pwm):
  return pwm * 100 / PWM_SAMPLING_RANGE


if __name__ == "__main__":
    import time
    import random
    print("-----ACS-----")
    # for i in range(0, 1024, 15):
    #     time.sleep(0.1)
    #     current_in_voltage = convert_adc_to_volts(i)
    #     current = convert_volts_to_amps(current_in_voltage)
    #     print("ADC: {} -- VOLTS: {} -- AMPS: {}".format(i, current_in_voltage, current))

    # print("-----THROTTLE-----")
    # for i in range(0, 1024, 15):
    #     time.sleep(0.1)
    #     pwm_percentage = convert_throttle_adc_to_percentage(i)
    #     print("PWM: {}".format(pwm_percentage))

    print("------OVERCURRENT-----")
    for amp in range(20, 60):
        time.sleep(0.1)
        pwm_percent = 100
        overcurrent_percent = compute_overcurrent_pwm_percent(amp)
        after_pwm = pwm_percent - overcurrent_percent
        print("AMPS: {} --- PWM: {} --- OVERCURRENT: {} --- AFTER: {}".format(amp, 1/pwm_percent, overcurrent_percent, after_pwm))
