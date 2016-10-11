#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define LED 13

// No pullups for the following two pins:
#define SDA 20 //wire data pin
#define SCL 21 //wire clock pin

#define SEALEVELPRESSURE_HPA (1013.25) // arbitrary value

// Instantiate bme sensor
Adafruit_BME280 bme;




void setup () {
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

  if (!bme.begin()) {
    Serial.println("BME sensor not found...halting");
    while (1);
  } 
}




void loop() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  delay(2000);
}


