#include <Servo.h>

Servo myservo;  // create servo object to control a servo
const int SERVO_PIN = 0; // assiging servo pin: D3
const int LDR_PIN = 16; // assiging LDR pin: D0
const int LDR_R_PIN = 5; // assiging LDR pin: D1
const int LDR_L_PIN = 4; // assiging LDR pin: D2

const int SERVE_MOVE_RIGHT_DEGREE = 30;
const int SERVE_MOVE_LEFT_DEGREE = -30;
const int SERVE_MOVE_STRAIGHT_DEGREE = 0;

enum directions{Left, Straight, Right};

void setup() {
  myservo.attach(SERVO_PIN);  // attaching servo to the servo object
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);
  pinMode(LDR_R_PIN, INPUT);
  pinMode(LDR_L_PIN, INPUT);
}

void loop() {
  String robot_status = get_robot_status();
  make_decision(robot_status);
  delay(200);
}

// returning robot's state in a 3-digit string (containing 0 and 1)
String get_robot_status() {
  String result = String(digitalRead(LDR_L_PIN)) + String(digitalRead(LDR_PIN)) + String(digitalRead(LDR_R_PIN));
  Serial.println(result);
  return result;
}

// a function for moving the servo motor
void turn(enum directions dir) {
  if (dir == Right) {
    // SERVE_MOVE_RIGHT_DEGREE   
    Serial.println("Turning Right...");
    myservo.write(SERVE_MOVE_RIGHT_DEGREE + 90);
  } else if (dir == Straight) {
    // SERVE_MOVE_STRAIGHT_DEGREE   
    Serial.println("Moving Straight...");
    myservo.write(SERVE_MOVE_STRAIGHT_DEGREE + 90);
  } else if (dir == Left) {
    // SERVE_MOVE_LEFT_DEGREE   
    Serial.println("Turning Left...");
    myservo.write(SERVE_MOVE_LEFT_DEGREE + 90);
  } 
}

// changing robot's state using it's current state 
void make_decision(String robot_status) {
  if (robot_status == "110" || robot_status == "100") {
    turn(Right);
  } else if (robot_status == "011" || robot_status == "001") {
    turn(Left);
  } else {
    turn(Straight);
  }
}
