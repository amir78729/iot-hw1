#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define SS_PIN D8
#define RST_PIN D0

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

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus

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

  timeClient.begin();
  timeClient.setTimeOffset(16200); // GMT+4:30 (4.5*3600)

}
void loop() {
  timeClient.update();
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3] ) {

    start_time = timeClient.getEpochTime();
    enterance_deadline = start_time + 60000 * ENTERANCE_DEADLINE_IN_MINUTE;
    end_time = start_time + 60000 * CLASS_TIME_IN_MINUTE;

    Serial.println(start_time);
    Serial.println(enterance_deadline);
    Serial.println(end_time);

    print_class_time_info();

    //    Serial.println(epochTime);
    Serial.println(F("A new card has been detected."));
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
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
  Serial.println("Class Started!");
  int seconds = timeClient.getSeconds();
  int minutes = timeClient.getMinutes();
  int hours = timeClient.getHours();
  Serial.print("- Start Time: ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("- Enterance Deadline: ");
  get_time_after_x_sec(hours, minutes, seconds, 60 * ENTERANCE_DEADLINE_IN_MINUTE);
  Serial.print("- End Time: ");
  get_time_after_x_sec(hours, minutes, seconds, 60 * CLASS_TIME_IN_MINUTE);
}
