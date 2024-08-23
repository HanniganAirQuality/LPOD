/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "LPOD_node.h"

#include "RTClib.h"
#include "SdFat-Particle/SdFat.h"
#include "ads_module.h"
#include "Adafruit_BME680.h" //for some reason it gotta be him--> "" ?

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
// SerialLogHandler logHandler(LOG_LEVEL_INFO);

#if USE_DELAY
  const std::chrono::milliseconds logPeriod = 30s;
  unsigned long lastLog;
#endif

#if SD_ENABLED
  SdFat sd;
  SdFile file;
  char fileName[] = "LPODID_YYYY_MM_DD.CSV";
#endif  //SD_ENABLED
#if DS3231_ENABLED 
  RTC_DS3231 ds3231;  //DO NOT NAME THIS RTC IT THINKS ITS A MACRO !!!
  char bufftime[] = "YYYY-MM-DDThh:mm:ss";
  int Y,M,D,h,m,s;
#endif //DS3231_ENABLED

#if ADS_ENABLED
  ADS_Module ads_module;
  ads_data ads;
#endif
#if BME_ENABLED
  Adafruit_BME680 bme680;
#endif

#if CELL_ENABLED
  const std::chrono::milliseconds cellPushPeriod = 30s;
  unsigned long cellLastLog;
  // The event name to publish with
  const char *eventName = "LPODA2";
  // void publishTest();
#endif

/***************************************************************************************/
// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif

  #if DS3231_ENABLED
    ds3231.begin();
      DateTime firsttime = ds3231.now();
      ds3231.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  #endif //DS3231_ENABLED
  
  #if ADS_ENABLED
    ads_module.begin();
  #endif  //ADS_ENABLED

  #if BME_ENABLED    
    bme680.begin(BME_SENSOR_ADDR);
    if (!bme680.begin(BME_SENSOR_ADDR)) {
      Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
      while (1);
    } //if(!bme680.begin(BME680_CS))
    //Set up oversampling an initialize filter
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150);
  #endif  //BME_ENABLED
  #if SD_ENABLED
    pinMode(SD_CS, OUTPUT);

    digitalWrite(SD_CS, LOW);
    sd.begin(SD_CS);
    while(!sd.begin(SD_CS)){
      #if SERIAL_ENABLED
        Serial.println(F("Insert SD Card to begin!"));
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);
    }
      DateTime now = ds3231.now();
        Y = now.year();
        M = now.month();
        D = now.day();
      sprintf(fileName, "%s_%04u_%02u_%02u.CSV", lpodID, Y, M, D);
        delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      Serial.println(fileName);
      file.close();
      digitalWrite(SD_CS, HIGH);
    #endif
}

/***************************************************************************************/
// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  #if DS3231_ENABLED
    DateTime now = ds3231.now();
    Y = now.year();  M = now.month();  D = now.day();  h = now.hour();  m = now.minute();  s = now.second();
    sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);
  #endif  //DS3231_ENABLED
  
  #if ADS_ENABLED
    ads = ads_module.return_updated();
    // Serial.println(F("you did it"));
  #endif  //ADS_ENABLED

  #if BME_ENABLED
    if(!bme680.performReading())  {
      Serial.println(F("Failed to perform reading :("));
      return;
    }
  #endif  //BME_ENABLED

  if (millis() - lastLog >= logPeriod.count())
  {
    lastLog = millis();
    #if SD_ENABLED
    digitalWrite(SD_CS, LOW);
    sd.begin(SD_CS);
    while(!sd.begin(SD_CS)){
      #if SERIAL_ENABLED
        Serial.println("Cannot find SD Card!");
      #endif
      sd.begin(SD_CS);
    } 
    if(sd.begin(SD_CS)){
      delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      delay(1000);
      file.println();
      #if DS3231_ENABLED
        file.print(bufftime);
        file.print(F(","));
        delay(100);
      #endif  //DS3231_ENABLED
      
      #if ADS_ENABLED
        file.print(ads.Fig1); 
        file.print(F(","));
        delay(100);
        file.print(ads.Fig2); 
        file.print(F(","));
        delay(100);
        file.print(ads.Fig3); 
        file.print(F(","));
        delay(100);
        file.print(ads.Worker); 
        file.print(F(","));
        delay(100);
        file.print(ads.Auxiliary); 
        file.print(F(","));
        delay(100);
      #endif  //ADS_ENABLED

      #if BME_ENABLED
        file.print(bme680.readTemperature());
        file.print(F(","));
        file.print(bme680.readPressure());
        file.print(F(","));
        file.print(bme680.readHumidity());
        file.print(F(","));
        file.print(bme680.readGas());
        file.print(F(","));
        delay(100);
      #endif  //BME_ENABLED

      file.sync();
      delay(100);
      file.close();
      delay(100);
    }
    #endif
    #if SERIAL_ENABLED
      Serial.println();
      #if DS3231_ENABLED
        Serial.print(bufftime);
        Serial.print(F(","));
      #endif  //DS3231_ENABLED
      
      #if ADS_ENABLED
        Serial.print(ads.Fig1); 
        Serial.print(F(","));
        delay(100);
        Serial.print(ads.Fig2); 
        Serial.print(F(","));
        delay(100);
        Serial.print(ads.Fig3); 
        Serial.print(F(","));
        delay(100);
        Serial.print(ads.Worker); 
        Serial.print(F(","));
        delay(100);
        Serial.print(ads.Auxiliary); 
        Serial.print(F(","));
        delay(100);
      #endif  //ADS_ENABLED

      #if BME_ENABLED
        Serial.print(bme680.readTemperature());
        Serial.print(F(","));
        Serial.print(bme680.readPressure());
        Serial.print(F(","));
        Serial.print(bme680.readHumidity());
        Serial.print(F(","));
        Serial.print(bme680.readGas());
        Serial.print(F(","));
        delay(100);
      #endif  //BME_ENABLED
    #endif

    #if CELL_ENABLED
      if(Particle.connected()){
        // if (millis() - cellLastLog >= cellPushPeriod.count()) {
          // cellLastLog = millis();

          char buff[128];
          snprintf(buff, sizeof(buff), "[%u,%u,%u,%u,%u,%0.2f,%0.2f,%f,%u]", 
            ads.Fig1, ads.Fig2, ads.Fig3, ads.Worker, ads.Auxiliary,
            bme680.readTemperature(), bme680.readPressure(), 
            bme680.readHumidity(), bme680.readGas());

            Particle.publish(eventName, buff, PRIVATE);
        }
        // }
    #endif

  }

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}

// #if CELL_ENABLED
// void publishTest() {
//     char buf[128];

//     snprintf(buf, sizeof(buf), "[%d,%d]", );

//     Particle.publish(eventName, buf, PRIVATE);
//     Log.info("published: %s", buf);
// }  
// #endif