#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>
#include <Adafruit_BME280.h>
#include <RH_RF95.h>

#include "dtostrf.c"

#define LED 13

// LoRa Radio
#define RFM95_CS 8 // select
#define RFM95_RST 4 // reset
#define RFM95_INT 3 // interupt
#define RF95_FREQ 915.0

// No pullups (yet) for the following two pins:
#define SDA 20 //wire data pin
#define SCL 21 //wire clock pin

#define SEALEVELPRESSURE_HPA (1013.25) // arbitrary value

// Instantiate the HDR light sensor
Adafruit_TSL2591 lightSensor = Adafruit_TSL2591(2591);
Adafruit_BME280 bmeSensor;

// Instantiate LoRa Radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);


void setup(void) {
  // radio setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // Serial monitoring/debuging
  Serial.begin(9600);
  while (!Serial) { // Wait for serial port to be available
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  // gives time for Serial to be available for monitoring before logging begins
  digitalWrite(LED, HIGH);
  delay(25000);
  digitalWrite(LED, LOW);
   Serial.println("Serial logging started...");

   if (lightSensor.begin() && bmeSensor.begin()) {
     Serial.println("Sensorshttp://stackoverflow.com/questions/7228438/convert-double-float-to-string initialized.");
   }
   else {
     Serial.println("Sensor inits failed...halting program");
     while (1);
   }
   

   ////  Light Sensor Configs
   // Gain (can be changed on the fly)
   lightSensor.setGain(TSL2591_GAIN_MED); // 25x gain
   // Timing
   lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)


   //// LoRa radio setup
   // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Transmit power set to 23dbm (max)
  rf95.setTxPower(23, false);


  Serial.println("Setup complete.");
  Serial.println();
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


int16_t packetnum = 0;  // packet counter

//Main loop
void loop(void) {
  lightPrint();
  bmePrint();
  delay(1000);


  // float nearest = roundf(val * 100) / 100;
  // temp max char length : (- or 3rd digit)XX.XX = 6 + null
  // pressure max char length : XXXX.XX = 7 + null
  // humidity max char length : XXX.XX = 6 + null
  
  //               0     6     12         23     30    36   41
  char packet[43] = "temp: XXX.XX-pressure: XXXX.XX-hum: XXX.XX";
  dtostrf(bmeSensor.readTemperature(), 6, 2, packet+6);
  packet[12] = '\n';
  dtostrf(bmeSensor.readPressure() / 100.0F, 7, 2, packet+23);
  packet[30] = '\n';  
  dtostrf(bmeSensor.readHumidity(), 6, 2, packet+36);
  
  packet[42] = 0;
  
  //  itoa(packetnum++, radiopacket+13, 10);
  Serial.println("Sending :"); Serial.println(packet);
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)packet, 43);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(5000)) { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len)) {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");   
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else {
      Serial.println("Receive failed");
    }
  }
  else {
    Serial.println("No reply, is there a listener around?");
  }
  delay(1000);
}
