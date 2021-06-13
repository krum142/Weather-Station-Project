#include <math.h>

#define WindSensorPin (2) // The pin location of the anemometer sensor D2

volatile unsigned long Rotations; // Fan rotation counter used in interrupt routine

volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine

float WindSpeed; // Speed m/s

float diameter = 130; // in mm

float perimeter = (diameter / 1000) * 3.14159265;

float afactor = 2.5;

float RotationsPerSecond;

void setup() {

Serial.begin(9600);

pinMode(WindSensorPin, INPUT);

attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);

Serial.println("Wind Speed Test");

Serial.println("Rotations\tM/S");

}

void loop() {

Rotations = 0; // Set Rotations count to 0 ready for calculations

sei(); // Enables interrupts

delay (10000); // Wait 3 seconds to average

cli(); // Disable interrupts

// Convert to m/s using the derived formula (see notes)

// V = N(0.35/3) = N * 0.12
RotationsPerSecond = (Rotations / 10.0);
WindSpeed = ((RotationsPerSecond) * perimeter * afactor);

Serial.print(Rotations); Serial.print("\t\t");

Serial.println(WindSpeed);

}

// This is the function that the interrupt calls to increment the rotation count

void isr_rotation () {

if ((millis() - ContactBounceTime) > 15 ) { // Debounce the switch contact

Rotations++;

ContactBounceTime = millis();

}

}
