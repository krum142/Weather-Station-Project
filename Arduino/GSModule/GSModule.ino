//#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const byte rxPin = 0; // Wire this to Tx Pin of SIM900
const byte txPin = 1; // Wire this to Rx Pin of SIM900

// We'll use a software serial interface to connect to SIM900
//SoftwareSerial SIM900(rxPin, txPin);

volatile unsigned long rotations;
volatile unsigned long contactBounceTime;
volatile unsigned long lastReadOfWindSpeed;
float windSpeed; // Speed m/s
float diameter = 130; // Anemometer Diameter (set to the value for your cup-to-cup in mm)
float perimeter = (diameter / 1000) * 3.14159265; // Perimeter in Meters
float afactor = 3.5; // External Resistances From Bearing or other... higher number will result in faster speed. lower numbers will result in slower speeds defalt = 2.5
float rotationsPerSecond;
const int WindSensorPin = 2;
int readWindSpeedTime = 10000; //miliseconds

const int lightResistor = A0;
const int lightResistor2 = A1;
const int led1 = 12;

Adafruit_BMP085 bmp;

#define DHTPIN 5
#define DHTTYPE DHT11 
DHT_Unified dht(DHTPIN, DHTTYPE);

String data = "";

void setup() {
  Serial.begin(9600);
 // SIM900.begin(9600); // Change this to the baudrate used by SIM900
  pinMode(WindSensorPin, INPUT);
  pinMode(lightResistor, INPUT);
  pinMode(lightResistor2, INPUT);
  pinMode(led1, OUTPUT);
  bmp.begin();
  dht.begin();
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
}

void loop() {
  Serial.println(readWindSpeedTime);
  delay(10000);
  
  readWindSpeed(readWindSpeedTime); //time to read wind Speed in ms 
  data = "{\"WindDirection\":\"" + readWindDirectionValue() + "\", \"WindSpeed\":\"" + String(windSpeed) + "\", " + readTempHumiditySensor() + ", " + readTempPressureSensor(5) + "}";

  
  sendHttp(data);
}

void sendHttp(String p){
  Serial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r");
  delay(500);
  Serial.println("AT+SAPBR=3,1,\"APN\",\"internet.vivacom.bg\"\r");
  delay(500);
  Serial.println("AT+SAPBR=1,1\r");
  delay(500);
  Serial.println("AT+HTTPINIT\r");
  delay(500);
  Serial.println("AT+HTTPPARA=\"URL\",\"http://{IP}:100/Send\"\r");
  delay(500);
  Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r");
  delay(500);
  Serial.println("AT+HTTPDATA="+ String(p.length()) +",1500\r");
  delay(500);
  Serial.println(p); // Feed Data into AT+HTTPDATA
  delay(750);
  serialFlush();
  Serial.println("AT+HTTPACTION=1\r");
  delay(5000);
  if(Serial.find("+HTTPACTION:1,201")){
    serialFlush();
    Serial.println("AT+HTTPREAD=0,200\r");
    String httpData = Serial.readString();
    httpData = httpData.substring(httpData.indexOf("$") + 1, httpData.lastIndexOf("$"));
    String temp = httpData.substring(0,httpData.indexOf(","));
    httpData.remove(0,temp.length() + 1);
    String temp1 = httpData;
    Serial.println("x- "+ temp +" -x");
    Serial.println("z- "+ temp1 +" -z");
    readWindSpeedTime = temp1.toInt();
  }  
  //Serial.println("AT+HTTPTERM\r");
}

// void readResponse(){
// while (Serial.available()){
//        String inData = Serial.readString();
//        Serial.println("SIM900: " + inData);
  
//    } 
// }

void readWindSpeed(int timeToRead){
  if( (millis() - lastReadOfWindSpeed) >= timeToRead ){
    float passedSeconds = (millis() - lastReadOfWindSpeed) / 1000.0;
    //Serial.println(String(passedSeconds) + "---" + String(millis() - lastReadOfWindSpeed)  );
    // Convert to m/s using the derived formula (see notes)
    rotationsPerSecond = (rotations / passedSeconds); //(Rotations / 10.0) Angular Velocity;
    windSpeed = ((rotationsPerSecond) * perimeter * afactor);
    lastReadOfWindSpeed = millis();
    rotations = 0; // Set Rotations count to 0 ready for calculations
  }
}

void isr_rotation () {
  if ((millis() - contactBounceTime) > 15 ) { // Debounce the switch contact
    rotations++;
    contactBounceTime = millis();
  }
}

String readWindDirectionValue(){
    digitalWrite(led1,HIGH);
    delay(10);
    String result = String(analogRead(lightResistor)) + "-" + String(analogRead(lightResistor2));
    digitalWrite(led1,LOW);

    return result;
}

String readTempHumiditySensor(){
  sensors_event_t event;
  String result = "\"Humidity\":\"";
  dht.humidity().getEvent(&event);
  result += String(event.relative_humidity) + "\", \"TemperatureS1\":\"";
  dht.temperature().getEvent(&event);
  result += String(event.temperature) + "\"";
  
  return result;
}

String readTempPressureSensor(int height){
  String result = "\"Pressure\":\"";
  result += String(bmp.readSealevelPressure(232)) + "\", \"TemperatureS2\":\"";
  result += String(bmp.readTemperature()) + "\"";
  return result;
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}
