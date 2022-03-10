#include <Servo.h>

Servo myservo;  // create servo object to control a servo
const int TRIGGER_PIN = 12; // assiging trigger pin for ultrasonic sensor: D6
const int ECHO_PIN = 14; // assiging echo pin for ultrasonic sensor: D5
const int SERVO_PIN = 5; // assiging servo pin: D1
const int LED_PIN = 4; // assiging servo pin: D2

const int TIMER_DURATION = 5000;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;
bool targetDetected = false;
bool isOpen = false;
int timerMillies = -1;
int pos = 0;

void setup() {
  myservo.attach(SERVO_PIN);  // attaching servo to the servo object
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT); // Sets the TRIGGER_PIN as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the ECHO_PIN as an Input
  pinMode(LED_PIN, OUTPUT); // Sets the ECHO_PIN as an Input
  myservo.write(pos);
}

void loop() {
  
  // Clears the TRIGGER_PIN
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY / 2;

  if (distanceCm > 10 ) { // no object was detected closer than 10cm
    targetDetected = false;
    
    if (isOpen) {
      Serial.println("closing the door");
      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(pos);
      }
      isOpen = false;
    }

  } else { // an object was detected closer than 10cm
    if (!targetDetected) { // the object was appeared recently
      Serial.println("the object was appeared recently");
      targetDetected = true;
      timerMillies = millis();
    } else {
      
      if (millis() > timerMillies + TIMER_DURATION) { // the timer has been finished and we can open the door
        
        if (!isOpen) {
          Serial.println("the timer has been finished and we can open the door");
          for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
            myservo.write(pos);
          }
          isOpen = true;
        }
        
      } else { // we should wait for timer...
        Serial.print("waiting for timer...(");
        Serial.print(TIMER_DURATION - (millis() - timerMillies));
        Serial.println("ms remaining)");
      }
    }
  }
  digitalWrite(LED_PIN, targetDetected ? HIGH : LOW);
}
