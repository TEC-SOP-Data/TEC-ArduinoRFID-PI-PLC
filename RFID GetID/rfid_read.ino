#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Create an instance of the MFRC522 reader

String uidString = ""; // Create an empty string to store the UID


void setup() {
  Serial.begin(9600); // Initialize serial communication

  SPI.begin(); // Initialize SPI bus

  rfid.PCD_Init(); // Initialize RFID reader

  Serial.println("RFID reader initialized");
}

void loop() {

GetID();




}

void GetID()
{
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { // If a new card is present
    uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i], HEX); // Append each byte of the UID to the string
    }
    Serial.print("UID tag: "); // Display the UID
    Serial.println(uidString);
    rfid.PICC_HaltA(); // Stop reading
  }
}
