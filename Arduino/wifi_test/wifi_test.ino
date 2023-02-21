#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11
#define DEBUG true

SoftwareSerial esp = SoftwareSerial(rxPin, txPin);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(9600);
  esp.begin(9600);

  String res = sendData("AT",1500,DEBUG);
  res.toLowerCase();

  if(res.indexOf("ok")<0){
     return;
  }

  initWifi();
}

void loop() {  
  delay(10000);
  sendData("AT+CIPMUX=1", 5000, DEBUG);
  sendData("AT+CIPSTART=0,\"TCP\",\"10.42.0.1\",3000",5000,DEBUG);

  String cmd = "GET /hej/meddig HTTP/1.1\r\nHost: 10.42.0.1\r\n\r\n";
  sendData("AT+CIPSEND=0,"+String(cmd.length()),5000,DEBUG);

  sendData(cmd,5000,DEBUG);
  delay(15000);
  sendData("AT+CIPCLOSE=0",5000,DEBUG);
}

void initWifi(){
  // Reset module and connect to wifi then send a post request
  sendData("AT+RST",1000,DEBUG);
  sendData("AT+CWMODE=1",1000,DEBUG);
  sendData("AT+CWJAP=\"Test\",\"test1234\"",2000,DEBUG);
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";                                             
    esp.println(command);                                          
    long int time = millis();                                      
    while( (time+timeout) > millis())                                 
    {      
      while(esp.available())                                      
      {
        char c = esp.read();                                     
        response+=c;                                                  
      }  
    }    
    if(debug)                                                        
    {
      Serial.print(response);
    }    
    return response;                                                  
}
