const int LDR_PIN = A0; // assiging LDR pin: A0
const int LED_PIN = 5; // assiging LED pin: D1
const int BUZZER_PIN = 4; // assiging buzzer pin: D2
const int BRIGHTNESS_TRASHHOLD = 40; // assiging buzzer pin: D2

bool isActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int sensor_value = map(analogRead(LDR_PIN), 4, 1024, 0, 100); // mapping analog value between [0, 100]
  Serial.print("input value: ");
  Serial.println(sensor_value);
  digitalWrite(BUZZER_PIN, sensor_value > BRIGHTNESS_TRASHHOLD ? HIGH : LOW); // activate buzzer if the line was not detected
  digitalWrite(LED_PIN, sensor_value <= BRIGHTNESS_TRASHHOLD ? HIGH : LOW); // activate LED if the line was detected
  delay(200);
}
