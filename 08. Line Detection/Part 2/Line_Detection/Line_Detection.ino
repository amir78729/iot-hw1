const int LDR_PIN = 0; // assiging LDR pin: D3
const int LDR_R_PIN = 0; // assiging LDR pin: D3
const int LDR_L_PIN = 0; // assiging LDR pin: D3

const int LED_PIN = 5; // assiging LED pin: D1
const int BUZZER_PIN = 4; // assiging buzzer pin: D2

const int BRIGHTNESS_THRESHHOLD = 40;
const int SERVE_MOVE_RIGHT_DEGREE = 30;
const int SERVE_MOVE_LEFT_DEGREE = -30;
const int SERVE_MOVE_STRAIGHT_DEGREE = 0;

enum directions{Left, Straight, Right};


bool isActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  Serial.print("Serial.println(analogRead(LDR_PIN)): ");
  Serial.println(analogRead(LDR_PIN));
  Serial.print("Serial.println(digitalRead(LDR_PIN)): ");
  Serial.println(digitalRead(LDR_PIN));
//  int sensor_value = map(analogRead(LDR_PIN), 4, 1024, 0, 100); // mapping analog value between [0, 100]
//  Serial.print("input value: ");
//  Serial.println(sensor_value);
//  digitalWrite(BUZZER_PIN, sensor_value > BRIGHTNESS_THRESHHOLD ? HIGH : LOW); // activate buzzer if the line was not detected
//  digitalWrite(LED_PIN, sensor_value <= BRIGHTNESS_THRESHHOLD ? HIGH : LOW); // activate LED if the line was detected
//  Serial.println(digitalRead(LDR_PIN));
  String robot_status = get_robot_status();
  make_decision(robot_status);
  Serial.println(robot_status); 
  delay(200);
}

// returning robot's state in a 3-digit string (containing 0 and 1)
String get_robot_status() {
  return String(digitalRead(LDR_L_PIN)) + String(digitalRead(LDR_PIN)) + String(digitalRead(LDR_R_PIN));
}

// a function for moving the servo motor
void turn(enum directions dir) {
  if (dir == Right) {
    // SERVE_MOVE_RIGHT_DEGREE   
    Serial.println("Turning Right...");
  } else if (dir == Straight) {
    // SERVE_MOVE_STRAIGHT_DEGREE   
    Serial.println("Moving Straight...");
  } else if (dir == Left) {
    // SERVE_MOVE_LEFT_DEGREE   
    Serial.println("Turning Left...");
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
