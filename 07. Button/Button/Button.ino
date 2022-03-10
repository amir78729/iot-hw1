const int LDR_PIN = A0;
const int LED_PIN = 5;

bool isActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int sensor_value = analogRead(LDR_PIN);
  Serial.print("input value: ");
  Serial.print(sensor_value);
  Serial.print("  \tLED is ");
  Serial.print(isActive ? "ON" : "OFF");
  Serial.print("\t");
  if (sensor_value < 100) {
    Serial.print("*STATUS CHANGED!!*");
    isActive = !isActive;
    digitalWrite(LED_PIN, isActive ? HIGH : LOW);
    delay(1000);
  }
  Serial.println("");
  delay(200);
}
