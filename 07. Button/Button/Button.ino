const int LDR_PIN = A0; // assiging LDR pin: A0
const int LED_PIN = 5;  // assiging LED pin: D1

bool isActive = false; // a boolean variable for saving the LED's state

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int sensor_value = analogRead(LDR_PIN); // reading the LDR value
  Serial.print("input value: ");
  Serial.print(sensor_value);
  Serial.print("  \tLED is ");
  Serial.print(isActive ? "ON" : "OFF");
  Serial.print("\t");
  if (sensor_value > 900) { // putting the LDR in a dark place...
    Serial.print("*STATUS CHANGED!!*");
    isActive = !isActive;
    digitalWrite(LED_PIN, isActive ? HIGH : LOW); // changing the state of LED
    delay(1000); // waiting for the LDR to be in a lighter place
  }
  Serial.println("");
  delay(200);
}
