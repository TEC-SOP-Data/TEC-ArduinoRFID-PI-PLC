#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define DEBUG true
#define stationID 1

// WIFI Defines
#define rxPin 5
#define txPin 6

SoftwareSerial esp = SoftwareSerial(rxPin, txPin);

// RFID Defines
#define SSPin 10
#define RSTPin 9

MFRC522 rfid(SSPin, RSTPin);

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(9600);
  esp.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  if(!sendATCommand("AT",2000,"OK")){
    Serial.println("No response from wifi module");
  }

  sendATCommand("AT+RST",1000,"OK");
  sendATCommand("AT+CWMODE=1",1000,"OK");
  sendATCommand("AT+CWJAP=\"Test\",\"test1234\"",10000,"WIFI CONNECTED");
  sendATCommand("AT+CIPMUX=1",5000,"asdasdas");
}

void loop() {
  String RFIDResponse = GetID();
  if(RFIDResponse!=""){
    sendHTTP(RFIDResponse);
  }
}

void sendHTTP(String cartID){
  if(!sendATCommand("AT+CIPSTART=0,\"TCP\",\"10.42.0.1\",3000",3000,"CONNECT")){
    Serial.println("Failed to connect to master");
    return;
  }

  String cmd = "GET /"+String(stationID)+"/"+String(cartID)+" HTTP/1.1\r\nHost: 10.42.0.1\r\n\r\n";

  sendATCommand("AT+CIPSEND=0,"+String(cmd.length()),5000,">");
  sendATCommand(cmd,5000,"true");
  sendATCommand("AT+CIPCLOSE=0",5000,"CLOSED");
}

bool sendATCommand(String command, int timeout, String expectedResponse) {
  String response = "";
  esp.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (esp.available()) {
      char c = esp.read();
      response += c;
      if (response.indexOf(expectedResponse) != -1) {
        if(DEBUG){
          Serial.println(response);
        }
        return true;
      }
    }
  }

  if(DEBUG){
    Serial.println(response);
  }

  return false;
}

String GetID()
{
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { // If a new card is present
    String uidString = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i], HEX); // Append each byte of the UID to the string
    }

    Serial.print("UID tag: "); // Display the UID
    Serial.println(uidString);
    rfid.PICC_HaltA(); // Stop reading
    return uidString;
  }
  return "";
}


