void setup()
{
  pinMode(6, OUTPUT);
}

void loop()
{
  int analog = analogRead(0);
  float analog_decimal = analog / 1023.0f;
  uint8_t pwm = analog_decimal * 255;
  analogWrite(6, pwm);
  Serial.println(analog, DEC);
//  delay(1);
}
