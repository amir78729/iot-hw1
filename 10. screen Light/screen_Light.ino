const int LDR_PIN = A0; // assiging LDR pin: A0
const int LED_PIN = 5;  // assiging LED pin: D1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  int brightness = analogRead(LDR_PIN); //value between 0 and 1023
  analogWrite(LED_PIN, (1023 - brightness) / 4);
  Serial.println(brightness);
}
