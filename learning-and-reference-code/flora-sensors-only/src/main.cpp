#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_BME280.h>


#define LED 13

// No pullups for the following two pins:
#define SDA 20 //wire data pin
#define SCL 21 //wire clock pin

#define SEALEVELPRESSURE_HPA (1013.25) // arbitrary value

// instantiate the HDR light sensor
Adafruit_TSL2591 lightSensor = Adafruit_TSL2591(2591);
Adafruit_BME280 bmeSensor;



void setup(void) {
  Serial.begin(9600);
  while (!Serial) { // Wait for serial port to be available
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  // gives time for SerialUSB to be available to monitor before logging begins
  digitalWrite(LED, HIGH);
  delay(25000);
  digitalWrite(LED, LOW);
   Serial.println("Serial logging started...");

   if (lightSensor.begin() && bmeSensor.begin()) {
     Serial.println("Sensors initialized.");
   }
   else {
     Serial.println("A sensor inits failed...halting program");
     while (1);
   }

   
   ////  Sensor Configs

   // Light Sensor
   // Gain (can be changed on the fly)
   lightSensor.setGain(TSL2591_GAIN_MED); // 25x gain
   // Timing
   lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)

   Serial.println("Setup complete.");
}



// Sensor print functions
void lightPrint(void) {
  uint16_t x = lightSensor.getLuminosity(TSL2591_VISIBLE);

  Serial.print("[ ");
  Serial.print(millis()); // <-- why?
  Serial.print(" ms ] ");
  Serial.print("Luminosity: ");
  Serial.println(x, DEC);
}


void bmePrint(void) {
    Serial.print("Temperature = ");
  Serial.print(bmeSensor.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bmeSensor.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmeSensor.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bmeSensor.readHumidity());
  Serial.println(" %");

  Serial.println();
}



//Main loop
void loop(void) {
  lightPrint();
  
  bmePrint();
  
  delay(1000);
}





