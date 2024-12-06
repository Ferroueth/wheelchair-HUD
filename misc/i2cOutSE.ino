#include <Wire.h>
 
 
int batteryValue = 0;  
int speedValue = 0;     
float x = 0.0, y = 0.0, z = -9.0;
 
bool batteryIncreasing = true;
bool speedIncreasing = true;

String getGearChar() {
  short n = random(0, 4);
  switch(n){
    case 0:
      return "P";
      break;
    case 1:
      return "N";
      break;
    case 2:
      return "D";
      break;
    case 3:
      return "R";
      break;
    default:
      return "E";
  }
  return "0";
}
 
void setup() {
  Wire.begin();             // Join I2C bus
  Serial.begin(9600);       // Initialize Serial communication
  randomSeed(analogRead(3)); // Initialize random seed
  delay(1000);         // Wait for Serial to initialize
 
  batteryValue = random(0,101);
  speedValue = random(1,120);  
}
 
 
void loop() {
  for (int times = 0; times < 500; times++) {
 
    // Update batteryValue (0: [0-100])
    if (times % 10 == 0){
      if (batteryIncreasing) {
        batteryValue++;
        if (batteryValue >= 100) batteryIncreasing = false;
      } else {
        batteryValue--;
        if (batteryValue <= 0) batteryIncreasing = true;
      }
    }
    
 
    // Update speedValue (1: [1-120])
    if (speedIncreasing) {
      speedValue++;
      if (speedValue >= 120) speedIncreasing = false;
    } else {
      speedValue--;
      if (speedValue <= 1) speedIncreasing = true;
    }
 
    // Update x, y, z (2: random float values with variance)
    if (times > 50){
      x += random(-10, 10) / 10.0; // Variance ±1.0
      y += random(-10, 10) / 10.0; // Variance ±1.0
      z += random(-10, 10) / 10.0; // Variance ±1.0
    }
 
    // Transmit data over I2C
    String message = "B" + String(batteryValue) + ",S" + String(speedValue) + ",X" +
                 String(x, 2) + ",Y" + String(y, 2) + ",Z" + String(z, 2);
    if(random(0,6) < 2){
      message = message + ",G" + getGearChar();
    }
    Wire.beginTransmission(85);
    Wire.write(message.c_str()); // Send the message as a C-string
    Wire.endTransmission();
 
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
 
    delay(34); // Wait 500 ms before the next update, test the frames
  }
 
  // Halt after 10 iterations
  while (true);
}