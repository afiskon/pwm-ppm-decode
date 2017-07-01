#include <Arduino.h>
#include <PinChangeInt.h>

// see http://playground.arduino.cc/Main/PinChangeInt + https://github.com/GreyGnome/PinChangeInt

#define PPM_PIN 6
#define MAX_CHANNELS 12

volatile int pwm_value[MAX_CHANNELS] = { 0 };
volatile int prev_time = 0;
volatile int curr_channel = 0;

volatile bool overflow = false;

void rising()
{
  int tstamp = micros();

  /* overflow should never acutally happen, but who knows... */
  if(curr_channel < MAX_CHANNELS) {
    pwm_value[curr_channel] = tstamp - prev_time;
    if(pwm_value[curr_channel] > 2100) { /* it's actually a sync */
      pwm_value[curr_channel] = 0;
      curr_channel = 0;
    } else
      curr_channel++;
  } else
    overflow = true;

  prev_time = tstamp;
}

void setup() {
  pinMode(PPM_PIN, INPUT_PULLUP);
  PCintPort::attachInterrupt(PPM_PIN, &rising, RISING);

  Serial.begin(9600);
}

void loop() {
  for(uint8_t i = 0; i < MAX_CHANNELS; i++)
    Serial.println("ch" + String(i + 1) + " = " + String(pwm_value[i]));
  if(overflow)
    Serial.println("OVERFLOW!");
  Serial.println("----------------");
  delay(1000);
}
