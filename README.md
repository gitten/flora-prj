Log (starting \[2016-10-08 Sat\])
=================================

| Headline                                    | Time      |       |      |      |
|---------------------------------------------|-----------|-------|------|------|
| **Total time**                              | **14:32** |       |      |      |
| Log (starting \[2016-10-08 Sat\])           | 14:32     |       |      |      |
| \_ Flora                                    |           | 13:27 |      |      |
| \_ Driver only firmware                     |           |       | 2:46 |      |
| \_ Sensor only firmware                     |           |       | 5:49 |      |
| \_ Sensor TSL2591 (HDR light sensor)        |           |       |      | 0:28 |
| \_ Sensor BME280 (pressure+temp+humidity... |           |       |      | 0:51 |
| \_ RadioHead Library                        |           | 1:05  |      |      |
| \_ RH<sub>RF95</sub> driver notes           |           |       | 1:05 |      |

Flora
-----

### TODO look into "reset after serial connect"

### TODO look into serial flash and radio SPI conflicts

### More sample projects

-   [github link (MIT license)](https://github.com/mikenz/Feather_M0_LoRa)

### Development enviornment

-   <http://platformio.org/>

### Driver only firmware

#### TODO debug and understand why no acknowledge reply received

##### DONE fixed

Solution  
The folowing delay in the sample code was preventing reception of packet. See following code

<!-- -->

    Serial.println("Waiting for reply..."); // delay(10); <---- だめ
    if (rf95.waitAvailableTimeout(1000)) { 
      // Should be a reply message for us now (but there was not...)   
      if (rf95.recv(buf, &len)) {
        Serial.print("Got reply: ");

      ...

     }
      else {
        Serial.println("Receive failed");
      }
     }
     else {
      Serial.println("No reply, is there a listener around?");
    }

##### TODO questions to be answered

-   why does this matter?
-   how long is a message held? how does that work anyway?

### Sensor only firmware

#### I2C

-   NOTE : these pins have no pullup by default!
-   SDA :: GPIO 20 (data pin)
-   SCL :: GPIO 21 (clock pin)

#### ATSAMD21 Serial Communications(SERCOM) custom config notes

-   [useful adafruit reference](https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports?view=all)
-   I2C (SDA & SCL) :: SERCOM 3 (pads 0 and 1)

#### Sensor TSL2591 (HDR light sensor)

-   [Adafruit doc link](https://learn.adafruit.com/adafruit-tsl2591?view=all)
-   Libraries also available through platformio lib manager
    -   [pio repo link: TSL2591 lib](http://platformio.org/lib/show/463/Adafruit%2520TSL2591%2520Library)
    -   [pio repo link: adafruit sensor support lib (required)](http://platformio.org/lib/show/31/Adafruit%2520Unified%2520Sensor)
-   I2C address :: 0x29

##### Configs

-   Gain
    -   Can be changed on the fly
-   Settings ::

<!-- -->

    tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
    lightSensor.setGain(TSL2591_GAIN_MED);      // 25x gain
    tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

#### Sensor BME280 (pressure+temp+humidity sensor)

-   [Adafruit doc link](https://learn.adafruit.com/adafruit-bme280-humidity-barometric-pressure-temperature-sensor-breakout?view=all)
-   Libraries also available through platformio lib manager
    -   [pio repo link: Adafruit BME280 lib](http://platformio.org/lib/show/166/Adafruit%2520BME280%2520Library)

RadioHead Library
-----------------

### TODO evaluate their power measurements

### RH<sub>RF95</sub> driver notes

-   [RadioHead RH<sub>RF95</sub> docs](http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html#aa7bee6fb7ad0e0d0ae5e04ed27cfd79d)
-   Only supports the LoRa mode on the HopeRF RFM95

-   61Hz precision frequency settings

-   Packet format
    -   8 symbol PREAMBLE
    -   Explicit header with header CRC (handled internally by the radio)
    -   4 octets HEADER: (TO, FROM, ID, FLAGS)
    -   0 to 251 octets DATA
    -   CRC (handled internally by the radio)
-   For 915MHz parts, src<sub>C</sub>++{rf95.setFrequency(915.0); //'rf95' is

    the driver singleton instatiation} is needed after driver/manager initialization

-   Driver interupts MUST be disabled during comms over SPI with another device. Disable with and

-   Library creator range tests
    -   rf95<sub>client</sub> base station connected to a VHF discone antenna at 8m height above ground
    -   f95<sub>server</sub> mobile connected to 17.3cm 1/4 wavelength antenna at 1m height, no ground plane.
    -   Both configured for 13dBm, 434MHz, Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range
    -   Minimum reported RSSI seen for successful comms was about -91
    -   Range over flat ground through heavy trees and vegetation approx 2km.
    -   At 20dBm (100mW) otherwise identical conditions approx 3km.
    -   At 20dBm, along salt water flat sandy beach, 3.2km.
    -   NOTE :: It should be noted that at this data rate, a 12 octet message takes 2 seconds to transmit.
    -   At 20dBm (100mW) with Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. (Default medium range) in the conditions described above
        -   Range over flat ground through heavy trees and vegetation approx 2km.

TODO review cold temp electronics thread
----------------------------------------

[stackexchange thread](http://electronics.stackexchange.com/questions/143921/why-do-electronics-have-a-low-temperature-limit)

Feather MO board w/ radio
=========================

All board related info was taken from the adafruit docs.

Link to the single page format: [Feather M0 w/ Lora Docs](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module?view=all)

Dev enviornment
---------------

-   Works with the arduino IDE after adding the adafruit board manager URL in settings
-   Built in usb-to-serial for programming and debugging
-   Adafruit's suggested open source library for the radio is [AirSpayce's Radiohead library](http://www.airspayce.com/mikem/arduino/RadioHead/)

Hardware coms
-------------

-   SPI pins which also can be configured for GPIO
-   ATSAMD21 chip has 6 serial com modules that can be used for I2C, SPI or Serial

LoRa radio
----------

### Module specs

#### [spec link](https://cdn-learn.adafruit.com/assets/assets/000/031/659/original/RFM95_96_97_98W.pdf?1460518717)

#### Hoperf Electronic RFM95W

|                       |                  |
|-----------------------|------------------|
| Frequency Range       | 869/915 MHz      |
| Spreading Factor      | 6-12             |
| Bandwidth             | 7.8-500kHz       |
| Effective Bitrate     | .018-37.5 kbps   |
| Estimated Sensitivity | -111 to -148 dBm |
|                       |                  |

### Antenna

-   If the wire antenna is undesirable, there are pads for an smt connector

### Related definitions

link budget  
Accounting of all of the gains and losses from the transmitter, through the medium (free space, cable, waveguide, fiber, etc.) to the receiver in a telecommunication system. It accounts for the attenuation of the transmitted signal due to propagation, as well as the antenna gains, feedline and miscellaneous losses. Randomly varying channel gains such as fading are taken into account by adding some margin depending on the anticipated severity of its effects. The amount of margin required can be reduced by the use of mitigating techniques such as antenna diversity or frequency hopping.

PA  
Power Amplifire

### dBm reference table

| dBm | mW    |
|-----|-------|
| 23  | 200   |
| 20  | 100   |
| 10  | 10    |
| 5   | 3.2   |
| 0   | 1     |
| -5  | 0.316 |

Power consumption
-----------------

-The following statistics are as listed in the online docs

-   ~300uA during full sleep
-   ~120mA peak during +20dBm transmit
-   ~40mA during active radio listening
-   ~2mA can be saved by putting radio in sleep mode
-   There is a BAT pin which allows battery voltage measurements

### plots

-   The Feather with RFM9x 900MHz radio set up for +20dBm power, transmitting a data payload of 20 bytes. Transmits take about 130mA for 70ms.

    ![](doc-assets/feather_lora32u4normal.png)

-   The ~13mA quiescent current is the current draw for listening (~2mA) plus ~11mA for the microcontroller. This can be reduce to amost nothing with proper sleep modes and not putting the module in active listen mode!

    ![](doc-assets/feather_lora32u4normal_closeup.png)

-   You can put the module into sleep mode by calling radio.sleep(); which will save you about 2mA.

    ![](smart-yeilds/feather_loram0modulesleep_closeup.png)\]\]

LoRaWAN spec
============

spec link: [LoraWAN spec 1RO](https://www.lora-alliance.org/portals/0/specs/LoRaWAN%2520Specification%25201R0.pdf)

topology
--------

> LoRaWAN networks typically are laid ou t in a star-of-stars topology in which gateways 1 4 relay messages between end-devices and a central network server at the backend. Gateways are connected to the network server via standard IP connections while end-devices use single-hop LoRa ™ or FSK communication to one or many gateways. All communication is generally bi-directional, although uplink communication from an end-device to the network server is expected to be the predominant traffic.

security encription
-------------------

-   Unique Network key (EUI64) and ensure security on network level
-   Unique Application key (EUI64) ensure end to end security on application level
-   Device specific key (EUI128)

Symphony Link spek
==================

useful links
------------

-   <http://www.link-labs.com/when-should-the-lorawan-specification-be-used/>
-   <http://info.link-labs.com/hubfs/Symphony_Protocol_Overview-2.pdf>
-   <http://www.link-labs.com/low-power-wide-area-network-lpwa/>
