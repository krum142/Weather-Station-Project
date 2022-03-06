#include <SoftwareSerial.h>

const byte rxPin = 0; // Wire this to Tx Pin of SIM900
const byte txPin = 1; // Wire this to Rx Pin of SIM900

// We'll use a software serial interface to connect to SIM900
SoftwareSerial SIM900(rxPin, txPin);

volatile unsigned long Rotations;
volatile unsigned long ContactBounceTime;
float WindSpeed; // Speed m/s
float diameter = 130; // Anemometer Diameter (set to the value for your cup-to-cup in mm)
float perimeter = (diameter / 1000) * 3.14159265; // Perimeter in Meters
float afactor = 4.5; // External Resistances From Bearing or other... higher number will result in faster speed. lower numbers will result in slower speeds defalt = 2.5
float RotationsPerSecond;
const int WindSensorPin = 2;

const int lightResistor = A0;
const int lightResistor2 = A1;
const int Led1 = 14;
String led1value = "";

void setup() {
  Serial.begin(115200);
  SIM900.begin(9600); // Change this to the baudrate used by SIM900
  pinMode(WindSensorPin, INPUT);
  pinMode(lightResistor, INPUT);
  pinMode(lightResistor2, INPUT);
  pinMode(Led1, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
}

void loop() {
  
  WindSpeed = getWindSpeed();
  led1value = readSensorValue();
  Serial.println(led1value + " " + String(WindSpeed));
  sendHttp("ID=" + led1value + "&Name=" + String(WindSpeed));
//  while (SIM900.available()){
//     String inData = SIM900.readString();
//     Serial.println("Got reponse from SIM900: " + inData);
//
//  } 
  //delay(15000);
}

void sendHttp(String p){
  SIM900.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r");
  delay(500);
  SIM900.println("AT+SAPBR=3,1,\"APN\",\"internet.vivacom.bg\"\r");
  delay(500);
  SIM900.println("AT+SAPBR=1,1\r");
  delay(500);
  SIM900.println("AT+HTTPINIT\r");
  delay(500);
  SIM900.println("AT+HTTPPARA=\"URL\",\"http://{IP}:100/test?" + p + "\"\r");
  delay(500);
  SIM900.println("AT+HTTPACTION=0\r");
  delay(500);
  SIM900.println("AT+TERM\r");
  delay(500);
}

float getWindSpeed(){
  Rotations = 0; // Set Rotations count to 0 ready for calculations

  //sei(); // Enables interrupts
  delay (10000); // Wait 10 seconds to average
  //cli(); // Disable interrupts

  // Convert to m/s using the derived formula (see notes)

  RotationsPerSecond = (Rotations / 10.0);
  //WindSpeed = ((RotationsPerSecond) * perimeter * afactor);
  return ((RotationsPerSecond) * perimeter * afactor);
}

void isr_rotation () {

  if ((millis() - ContactBounceTime) > 15 ) { // Debounce the switch contact

    Rotations++;

    ContactBounceTime = millis();

  }

}

String readSensorValue(){
    digitalWrite(Led1,HIGH);
    delay(10);
    String result = String(analogRead(lightResistor)) + "-" + String(analogRead(lightResistor2));
    digitalWrite(Led1,LOW);
    return result;
    
}
