*Projekt status og beskrivelse*

**Status oversigt**

I samarbejde med automatik afdelingen skal der udvikles et system, som gør det muligt og vide hvor de forskellige vogne befinder sig på transportbåndet.


**Løsning**
En skanner station der vedbrug af arduino / RFID skannere, kan skanne de individuelle vogne når de køre forbi. Skanner stationen sender data'en til en Raspberry PI der er forbundet til PLC'en.

Automatik afdelingen har bedt om 3 stationer, med mulighed for at skalere op. Derfor er der indkøbt ind til 4 stationer til en start.

***Hver stationen består af:***

1 Arduino.
1 RFID RC522 reader/writer til Arduino.
1 ESP8266 Serial WIFI Wireless Transceiver module.
1 Breadboard.
1 Strøm forsyning.
Desuden er der købt ethernet modul til hver station. Hvis wifi modulet ikke er den rette løsning.
