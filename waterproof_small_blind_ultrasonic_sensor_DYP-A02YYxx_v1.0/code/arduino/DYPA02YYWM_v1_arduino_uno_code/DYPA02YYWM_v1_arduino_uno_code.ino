/**
 * 
 * Author: Ritesh Talreja, Made in China, Warehouse: Shenzhen, Guangdong.
 * 
 * Components: Arduino UNO, DYPA02YYWM  v1.0
 * 
 * Arduino UNO +5V    --> DYPA02YYUM Pin 1 Red
 * Arduino UNO GND    --> DYPA02YYUM Pin 2 Black
 * Arduino UNO Pin 11 --> DYPA02YYUM Pin 3
 * Arduino UNO Pin 10 --> DYPA02YYUM Pin 4
 * 
 */

#define TRIGGER_PIN 11
#define PWM_OUTPUT_PIN 10

long duration;
float distance;

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(PWM_OUTPUT_PIN, INPUT);
}

void loop()
{
  // The sensor is triggered by a falling edge of a HIGH pulse that 
  // is more than 60 microseconds in duration.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(100);
  digitalWrite(TRIGGER_PIN, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  // If no object detected, fixed pulse width of 35ms is sent
  // by the sensor.
  pinMode(PWM_OUTPUT_PIN, INPUT);
  duration = pulseIn(PWM_OUTPUT_PIN, HIGH);
 
  // Convert the pulse width duration into a distance
  distance = duration;
  distance = distance / 58;
  
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(250);
}
