#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

#define SS_PIN D8
#define RST_PIN D0

#define NEGIN_HAJISOBHANI "4c661249"
#define AMIRHOSSEIN_ALIBAKHSHI "336ff31a"

bool present_students[] = {false, false}; // [is_AMIRHOSSEIN_ALIBAKHSHI_present, is_NEGIN_HAJISOBHANI_present]

Servo myservo;  // create servo object to control a servo
bool is_instructor_in_class = false; // a boolean variable for saving the LED's state

const int SERVO_PIN = 4; // assiging servo pin: D2
const int LDR_PIN = A0; // assiging LDR pin: A0
const int LED_PIN = 5;  // assiging LED pin: D1
const int BUZZER_PIN = 0; // assiging buzzer pin: D3

#define CLASS_TIME_IN_MINUTE 15
#define ENTERANCE_DEADLINE_IN_MINUTE 10

#define MIN 60000
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char *ssid     = "Dellink";
const char *password = "wtkh-daah-y8bj";

unsigned long start_time;
unsigned long enterance_deadline;
unsigned long end_time;

bool is_class_started = false;
bool is_deadline_missed = false;
bool is_class_ended = false;

void setup() {
  Serial.begin(9600);

  myservo.attach(SERVO_PIN);  // attaching servo to the servo object
  myservo.write(0);

  SPI.begin(); // Init SPI bus
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  print_current_time();
  Serial.println("Connected to the Internet!");
  Serial.println("Waiting for the instructor to start the class...");

  timeClient.begin();
  timeClient.setTimeOffset(16200); // GMT+4:30 (4.5*3600)

}
void loop() {
  timeClient.update();
  check_instructor_attendance();
  check_class_time();
  waiting_for_studants();
}
/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

String get_student_tag(byte *buffer, byte bufferSize) {
  String student_tag = "";
  for (byte i = 0; i < bufferSize; i++) {
    student_tag += String(buffer[i], HEX);
  }
  return student_tag;
}

String get_student_name_by_tag(String tag) {
  if (tag == NEGIN_HAJISOBHANI) {
    return "Negin Hajisobhani";
  }
  if (tag == AMIRHOSSEIN_ALIBAKHSHI) {
    return "Amirhossein Alibakhshi";
  }
  return "Unknown Student";
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void get_time_after_x_sec(int hour, int minute, int second, int offset_in_seconds) {
  second += offset_in_seconds;
  if (second >= 60) {
    minute += second / 60;
    second = second % 60;
  }
  if (minute >= 60) {
    hour += minute / 60;
    minute = minute % 60;
  }
  if (hour >= 24) {
    hour = hour % 24;
  }
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}

void print_class_time_info () {
  print_current_time();
  Serial.println("Class Started!");
  int seconds = timeClient.getSeconds();
  int minutes = timeClient.getMinutes();
  int hours = timeClient.getHours();
  Serial.print("  - Start Time : ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("  - Deadline   : ");
  get_time_after_x_sec(hours, minutes, seconds, 60 * ENTERANCE_DEADLINE_IN_MINUTE);
  Serial.print("  - End Time   : ");
  get_time_after_x_sec(hours, minutes, seconds, 60 * CLASS_TIME_IN_MINUTE);
}

void start_class() {
  is_class_started = true;
  start_time = timeClient.getEpochTime();
  // TODO: Convert 1 to 60
  enterance_deadline = start_time + 1 * ENTERANCE_DEADLINE_IN_MINUTE;
  end_time = start_time + 1 * CLASS_TIME_IN_MINUTE;
  print_class_time_info();
}

void check_instructor_attendance() {
  int sensor_value = analogRead(LDR_PIN); // reading the LDR value

  if (sensor_value > 900) { // putting the LDR in a dark place...
    is_instructor_in_class = !is_instructor_in_class;
    if (is_class_started && !is_instructor_in_class) {
      is_class_started = false;
      is_deadline_missed = false;
      is_class_ended = false;

    }
    digitalWrite(LED_PIN, is_instructor_in_class ? HIGH : LOW); // changing the state of LED
    if (is_instructor_in_class && !is_class_started) {
      start_class();
    }
    delay(1000); // waiting for the LDR to be in a lighter place
  }
}

void waiting_for_studants() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent()) return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial()) return;
  
  print_current_time();
  Serial.println("A tag was detectad!");
  Serial.print("  - PICC type       : ");
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  
  Serial.print("  - Tag Information : ");
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.print(" (");
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println(")");

  String student_tag = get_student_tag(rfid.uid.uidByte, rfid.uid.size);
  
  Serial.print("  - Student Name    : ");
  Serial.println(get_student_name_by_tag(student_tag));
  
  if (!is_class_started) {
    Serial.println("The class has not started yet!");
    delay(500);
    return;
  }
  //  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
  //      rfid.uid.uidByte[1] != nuidPICC[1] ||
  //      rfid.uid.uidByte[2] != nuidPICC[2] ||
  //      rfid.uid.uidByte[3] != nuidPICC[3] ) {
  //    Serial.println(F("A new card has been detected."));
  //  } else {
  //    Serial.println(F("Card read previously."));
  //  }
  validate_attendance(student_tag);

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void check_class_time() {
  if (is_class_started) {
    unsigned long current_time = timeClient.getEpochTime();
    if (current_time > end_time && !is_class_ended) {
      is_class_ended = true;
      print_current_time();
      Serial.println("The Class has been ended!");
    } else if (current_time > enterance_deadline && !is_deadline_missed) {
      is_deadline_missed = true;
      print_current_time();
      Serial.println("No one can enter the class anymore!");
    }
  }
}

void validate_attendance(String student_tag) {
  if (is_deadline_missed) {
    Serial.print("Sorry, you can not enter the class because ");
    Serial.println(is_class_ended ? "it is over!" : "you were too late!");
    buzz();
  } else {
    Serial.println("Welcome to the class!");
    open_door_for_students(student_tag);
  }
}

//void print_student_info() {
//  
//  
//}

void print_current_time() {
  Serial.println("----------------------------------------------------------------------------------");
  Serial.print("[");
  Serial.print(timeClient.getFormattedTime());
  Serial.print("] ");
}

void open_door_for_students(String student_tag) {
  Serial.print("[");
  Serial.print(timeClient.getFormattedTime());
  Serial.println("] Openning the door...");
  myservo.write(180);
    
  if (student_tag == AMIRHOSSEIN_ALIBAKHSHI) {
    present_students[0] = true
  } else if (student_tag == NEGIN_HAJISOBHANI) {
    present_students[1] = true
  }
  
  delay(3000); // leaving the door open for 3 seconds

  Serial.print("[");
  Serial.print(timeClient.getFormattedTime());
  Serial.println("] Closing the door...");
  myservo.write(0);
}
void buzz() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000); // active the buzzer for 1 second
  digitalWrite(BUZZER_PIN, LOW);
}
