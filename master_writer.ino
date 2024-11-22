#include <Wire.h>


int batteryValue = 0;  
int speedValue = 0;     
float x = 0.0, y = 0.0, z = 0.0; 

bool batteryIncreasing = true;
bool speedIncreasing = true;

void setup() {
  Wire.begin();             // Join I2C bus
  Serial.begin(9600);       // Initialize Serial communication
  randomSeed(analogRead(3)); // Initialize random seed
  delay(1000);         // Wait for Serial to initialize

  batteryValue = random(0,101);
  speedValue = random(1,8);  
}


void loop() {
  for (int times = 0; times < 150; times++) {

    // Update batteryValue (0: [0-100])
    if (batteryIncreasing) {
      batteryValue++;
      if (batteryValue >= 100) batteryIncreasing = false;
    } else {
      batteryValue--;
      if (batteryValue <= 0) batteryIncreasing = true;
    }

    // Update speedValue (1: [1-7])
    if (speedIncreasing) {
      speedValue++;
      if (speedValue >= 7) speedIncreasing = false;
    } else {
      speedValue--;
      if (speedValue <= 1) speedIncreasing = true;
    }

    // Update x, y, z (2: random float values with variance)
    x += random(-10, 10) / 10.0; // Variance ±1.0
    y += random(-10, 10) / 10.0; // Variance ±1.0
    z += random(-10, 10) / 10.0; // Variance ±1.0

    // Transmit data over I2C
    Wire.beginTransmission(4); // Transmit to device #4
    Wire.write("DATA");        // Identifier
    Wire.write(batteryValue);  // Send battery value
    Wire.write(speedValue);    // Send speed value
    Wire.endTransmission();    // Stop transmitting

    // Output to Serial Monitor
    Serial.print("{ B: "); 
    Serial.print(batteryValue); 
    Serial.print(", S: "); 
    Serial.print(speedValue); 
    Serial.print(", T: [");
    Serial.print(x, 2); 
    Serial.print(", "); 
    Serial.print(y, 2); 
    Serial.print(", "); 
    Serial.print(z, 2); 
    Serial.println("] }");

    delay(500); // Wait 500 ms before the next update
  }

  // Halt after 150 iterations
  while (true);
}
