#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const byte rxPin = 0; // Wire this to Tx Pin of SIM900
const byte txPin = 1; // Wire this to Rx Pin of SIM900

// We'll use a software serial interface to connect to SIM900
SoftwareSerial SIM900(rxPin, txPin);

volatile unsigned long Rotations;
volatile unsigned long ContactBounceTime;
float WindSpeed; // Speed m/s
float diameter = 130; // Anemometer Diameter (set to the value for your cup-to-cup in mm)
float perimeter = (diameter / 1000) * 3.14159265; // Perimeter in Meters
float afactor = 3.5; // External Resistances From Bearing or other... higher number will result in faster speed. lower numbers will result in slower speeds defalt = 2.5
float RotationsPerSecond;
const int WindSensorPin = 2;

const int lightResistor = A0;
const int lightResistor2 = A1;
const int Led1 = 12;
String led1value = "";

Adafruit_BMP085 bmp;

#define DHTPIN 5
#define DHTTYPE DHT11 
DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  SIM900.begin(9600); // Change this to the baudrate used by SIM900
  pinMode(WindSensorPin, INPUT);
  pinMode(lightResistor, INPUT);
  pinMode(lightResistor2, INPUT);
  pinMode(Led1, OUTPUT);
  bmp.begin();
  dht.begin();
}

void loop() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  Serial.println(bmp.readSealevelPressure(232));
  Serial.println(bmp.readTemperature());
  Serial.println(event.temperature);
  dht.humidity().getEvent(&event);
  Serial.println(event.relative_humidity);
  WindSpeed = readWindSpeed();
  led1value = readWindDirectionValue();
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
  SIM900.println("AT+HTTPTERM\r");
  delay(500);
}

float readWindSpeed(){
  Rotations = 0; // Set Rotations count to 0 ready for calculations

  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
  delay (10000); // Wait 10 seconds to average
  detachInterrupt(digitalPinToInterrupt(WindSensorPin));

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

String readWindDirectionValue(){
    digitalWrite(Led1,HIGH);
    delay(10);
    String result = String(analogRead(lightResistor)) + "-" + String(analogRead(lightResistor2));
    digitalWrite(Led1,LOW);
    return result;
    
}


