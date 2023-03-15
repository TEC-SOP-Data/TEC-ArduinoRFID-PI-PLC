#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

// --------------------------------------
// DO NOT CHANGE ANYTHING ABOVE THIS LINE
// IF YOU DO NOT KNOW WHAT YOU ARE DOING
#define stationID 1   // Define an integer constant named stationID and set it to the id of the station

#define WIFISSID "Test"  // Define a string constant named WIFISSID and set it to the wifi SSID
#define WIFIPASS "test1234"  // Define a string constant named WIFIPASS and set it to the wifi password

#define WEBSERVERIP "10.42.0.1" // Define a string constant named WEBSERVERIP and set it to the IP address of the web server
#define WEBSERVERPORT "3000"      // Define an string constant named WEBSERVERPORT and set it to the port of the web server

#define DEBUG true    // Define a boolean constant named DEBUG and set to true or false to enable or disable debug messages
// DO NOT CHANGE ANYTHING BELOW THIS LINE
// IF YOU DO NOT KNOW WHAT YOU ARE DOING
// --------------------------------------

// WIFI pin defines
#define rxPin 5       // Define an integer constant named rxPin and set it to 5
#define txPin 6       // Define an integer constant named txPin and set it to 6

SoftwareSerial esp = SoftwareSerial(rxPin, txPin);  // Create a SoftwareSerial object named esp using rxPin and txPin

// RFID pin defines
#define SSPin 10      // Define an integer constant named SSPin and set it to 10
#define RSTPin 9      // Define an integer constant named RSTPin and set it to 9

MFRC522 rfid(SSPin, RSTPin);  // Create an MFRC522 object named rfid using SSPin and RSTPin

void setup() {
  pinMode(rxPin, INPUT);   // Set the rxPin to INPUT mode
  pinMode(txPin, OUTPUT);  // Set the txPin to OUTPUT mode

  Serial.begin(9600);   // Start the serial communication with a baud rate of 9600
  esp.begin(9600);      // Start the SoftwareSerial communication with a baud rate of 9600

  SPI.begin();     // Initialize the SPI communication
  rfid.PCD_Init(); // Initialize the RFID module

  if(!sendATCommand("AT",2000,"OK")){ // Check if the WIFI module responds to the AT command within 2000ms
    Serial.println("No response from wifi module");  // If not, print a message to the serial monitor
  }

  sendATCommand("AT+RST",1000,"OK");    // Send an AT command to reset the module
  sendATCommand("AT+CWMODE=1",1000,"OK"); // Send an AT command to set the WIFI mode
  sendATCommand("AT+CWJAR=\""+String(WIFISSID)+"\",\""+String(WIFIPASS)+"\"",10000,"WIFI CONNECTED"); // Send an AT command to connect to a WIFI network
  sendATCommand("AT+CIPMUX=1",5000,"---------"); // Send an AT command to set up the connection mode
}

void loop() {
  String RFIDResponse = GetID();  // Get the RFID tag ID
  if(RFIDResponse!=""){          // If the ID is not empty
    sendHTTP(RFIDResponse);      // Send an HTTP request with the ID
  }
}

void sendHTTP(String cartID){
  if(!sendATCommand("AT+CIPSTART=0,\"TCP\",\""+String(WEBSERVERIP)+"\","+String(WEBSERVERPORT),3000,"CONNECT")){ // Check if the ESP8266 connects to the server within 3000ms
    Serial.println("Failed to connect to master");  // If not, print a message to the serial monitor and return
    return;
  }

  String cmd = "GET /"+String(stationID)+"/"+String(cartID)+" HTTP/1.1\r\nHost: "+WEBSERVERIP+"\r\n\r\n"; // Construct an HTTP request string

  sendATCommand("AT+CIPSEND=0,"+String(cmd.length()),5000,">");  // Send an AT command to set up the data transmission
  sendATCommand(cmd,5000,"true");  // Send the HTTP request
  sendATCommand("AT+CIPCLOSE=0",5000,"CLOSED");  // Close the connection
}
// This function sends an AT command to the ESP8266 wifi module and waits for an expected response
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

// This function reads the RFID tag and returns the UID as a string
String GetID(){
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { // If a new card is present
    String uidString = "";

    // Iterate through each byte of the UID and append it to the string
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i], HEX);
    }

    // Print the UID to the serial monitor
    if(DEBUG){
      Serial.print("UID tag: ");
      Serial.println(uidString);
    }

    // Stop reading the card and return the UID
    rfid.PICC_HaltA();
    return uidString;
  }

  // If no new card is present, return an empty string
  return "";
}
