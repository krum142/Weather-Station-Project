#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WindSensorPin (2) // The pin location of the anemometer sensor D2

volatile unsigned long Rotations; // Fan rotation counter used in interrupt routine

volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float WindSpeed; // Speed m/s

float diameter = 130; // Anemometer Diameter (set to the value for your cup-to-cup in mm)

float perimeter = (diameter / 1000) * 3.14159265; // Perimeter in Meters

float afactor = 2.5; // External Resistances From Bearing or other... higher number will result in faster speed. lower numbers will result in slower speeds

float RotationsPerSecond;


void setup() {

  Serial.begin(9600);

  pinMode(WindSensorPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
  //attachInterrupt(digitalPinToInterrupt(WindSensorPin), testMethod, FALLING);
  //Serial.println("Wind Speed Test");

  //Serial.println("Rotations\tM/S");


  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  
}
void loop() {
  
 
  Rotations = 0; // Set Rotations count to 0 ready for calculations

  sei(); // Enables interrupts
  displaySpeed(WindSpeed);
  delay (10000); // Wait 10 seconds to average
  cli(); // Disable interrupts

  // Convert to m/s using the derived formula (see notes)

  RotationsPerSecond = (Rotations / 10.0);
  WindSpeed = ((RotationsPerSecond) * perimeter * afactor);

  //Serial.print(Rotations); Serial.print("\t\t");
  Serial.println(WindSpeed);
  
  
}

// This is the function that the interrupt calls to increment the rotation count

void displaySpeed(float windspeed){
   display.clearDisplay();
  
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);
  
  
  display.println(windspeed);
  display.display();

  display.clearDisplay();
}

void isr_rotation () {

  if ((millis() - ContactBounceTime) > 15 ) { // Debounce the switch contact

    Rotations++;

    ContactBounceTime = millis();

  }

}
