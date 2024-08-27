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
#include "LPOD_DataStorage.h"

#include "RTClib.h"
#include "SdFat-Particle/SdFat.h"
#include "ads_module.h"
#include "Adafruit_BME680.h" // it gotta be him--> "" 

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
// SerialLogHandler logHandler(LOG_LEVEL_INFO);

#if USE_DELAY
  const std::chrono::milliseconds logPeriod = 5s;
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
  const int char_limit = 864;
  const char *eventName = "TEST";
  char buff[char_limit] = ""; 
  int i = 0;

  LPOD_data lpoddata;
#endif

/***************************************************************************************/
// setup() runs once, when the device is first turned on
void setup() {
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(SD_CS, OUTPUT);

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
    digitalWrite(SD_CS, LOW);
    sd.begin(SD_CS);
    while(!sd.begin(SD_CS)){
      #if SERIAL_ENABLED
        Serial.println(F("Insert SD Card to begin!"));
        digitalWrite(RED_LED, HIGH);
        delay(100);
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);
    }
    digitalWrite(RED_LED, LOW);
      DateTime now = ds3231.now();
        Y = now.year();
        M = now.month();
        D = now.day();
      sprintf(fileName, "%s_%04u_%02u_%02u.CSV", lpodID, Y, M, D);
        delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      #if SERIAL_ENABLED
        Serial.println(fileName);
      #endif
      file.close();
      digitalWrite(SD_CS, HIGH);
    #endif
}

/***************************************************************************************/
// loop() runs over and over again, as quickly as it can execute.
void loop() {
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
        digitalWrite(RED_LED, HIGH);
        delay(100);
      #endif
      sd.begin(SD_CS);
    } 
    digitalWrite(RED_LED, LOW);
    if(sd.begin(SD_CS)){
      delay(100);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE);
      delay(1000);
      digitalWrite(GREEN_LED, HIGH);
      delay(100);
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
      digitalWrite(GREEN_LED, LOW);
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
    //Collect data to storage up to size 
      digitalWrite(BLUE_LED, HIGH);
      lpoddata.Fig1[i] = ads.Fig1;
      delay(100);
      lpoddata.Fig2[i] = ads.Fig2;
      delay(100);
      lpoddata.Fig3[i] = ads.Fig3;
      delay(100);
      lpoddata.CO_Auxiliary[i] = ads.Auxiliary;
      delay(100);
      lpoddata.CO_Worker[i] = ads.Worker;
      delay(100);
      lpoddata.Rel_Humidity[i] = bme680.readHumidity();
      delay(100);
      lpoddata.Temperature[i] = bme680.readTemperature();
      delay(100);
      i = i+1;
      digitalWrite(BLUE_LED, LOW);
      if(i == RUN_TO_PUBLISH) {
        if(Particle.connected())  {
          digitalWrite(BLUE_LED, HIGH);
          digitalWrite(RED_LED, HIGH);
          snprintf(buff, sizeof(buff), 
          "[[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f],[%5.u,%5.u,%5.u,%5.u,%5.u,%2.2f,%2.2f]]",
              lpoddata.Fig1[0], lpoddata.Fig2[0], lpoddata.Fig3[0], lpoddata.CO_Auxiliary[0], lpoddata.CO_Worker[0], lpoddata.Temperature[0], lpoddata.Rel_Humidity[0],
              lpoddata.Fig1[1], lpoddata.Fig2[1], lpoddata.Fig3[1], lpoddata.CO_Auxiliary[1], lpoddata.CO_Worker[1], lpoddata.Temperature[1], lpoddata.Rel_Humidity[1],
              lpoddata.Fig1[2], lpoddata.Fig2[2], lpoddata.Fig3[2], lpoddata.CO_Auxiliary[2], lpoddata.CO_Worker[2], lpoddata.Temperature[2], lpoddata.Rel_Humidity[2],
              lpoddata.Fig1[3], lpoddata.Fig2[3], lpoddata.Fig3[3], lpoddata.CO_Auxiliary[3], lpoddata.CO_Worker[3], lpoddata.Temperature[3], lpoddata.Rel_Humidity[3],
              lpoddata.Fig1[4], lpoddata.Fig2[4], lpoddata.Fig3[4], lpoddata.CO_Auxiliary[4], lpoddata.CO_Worker[4], lpoddata.Temperature[4], lpoddata.Rel_Humidity[4],
              lpoddata.Fig1[5], lpoddata.Fig2[5], lpoddata.Fig3[5], lpoddata.CO_Auxiliary[5], lpoddata.CO_Worker[5], lpoddata.Temperature[5], lpoddata.Rel_Humidity[5],
              lpoddata.Fig1[6], lpoddata.Fig2[6], lpoddata.Fig3[6], lpoddata.CO_Auxiliary[6], lpoddata.CO_Worker[6], lpoddata.Temperature[6], lpoddata.Rel_Humidity[6],
              lpoddata.Fig1[7], lpoddata.Fig2[7], lpoddata.Fig3[7], lpoddata.CO_Auxiliary[7], lpoddata.CO_Worker[7], lpoddata.Temperature[7], lpoddata.Rel_Humidity[7],
              lpoddata.Fig1[8], lpoddata.Fig2[8], lpoddata.Fig3[8], lpoddata.CO_Auxiliary[8], lpoddata.CO_Worker[8], lpoddata.Temperature[8], lpoddata.Rel_Humidity[8],
              lpoddata.Fig1[9], lpoddata.Fig2[9], lpoddata.Fig3[9], lpoddata.CO_Auxiliary[9], lpoddata.CO_Worker[9], lpoddata.Temperature[9], lpoddata.Rel_Humidity[9],
              lpoddata.Fig1[10], lpoddata.Fig2[10], lpoddata.Fig3[10], lpoddata.CO_Auxiliary[10], lpoddata.CO_Worker[10], lpoddata.Temperature[10], lpoddata.Rel_Humidity[10],
              lpoddata.Fig1[11], lpoddata.Fig2[11], lpoddata.Fig3[11], lpoddata.CO_Auxiliary[11], lpoddata.CO_Worker[11], lpoddata.Temperature[11], lpoddata.Rel_Humidity[11],
              lpoddata.Fig1[12], lpoddata.Fig2[12], lpoddata.Fig3[12], lpoddata.CO_Auxiliary[12], lpoddata.CO_Worker[12], lpoddata.Temperature[12], lpoddata.Rel_Humidity[12],
              lpoddata.Fig1[13], lpoddata.Fig2[13], lpoddata.Fig3[13], lpoddata.CO_Auxiliary[13], lpoddata.CO_Worker[13], lpoddata.Temperature[13], lpoddata.Rel_Humidity[13],
              lpoddata.Fig1[14], lpoddata.Fig2[14], lpoddata.Fig3[14], lpoddata.CO_Auxiliary[14], lpoddata.CO_Worker[14], lpoddata.Temperature[14], lpoddata.Rel_Humidity[14],
              lpoddata.Fig1[15], lpoddata.Fig2[15], lpoddata.Fig3[15], lpoddata.CO_Auxiliary[15], lpoddata.CO_Worker[15], lpoddata.Temperature[15], lpoddata.Rel_Humidity[15],
              lpoddata.Fig1[16], lpoddata.Fig2[16], lpoddata.Fig3[16], lpoddata.CO_Auxiliary[16], lpoddata.CO_Worker[16], lpoddata.Temperature[16], lpoddata.Rel_Humidity[16],
              lpoddata.Fig1[17], lpoddata.Fig2[17], lpoddata.Fig3[17], lpoddata.CO_Auxiliary[17], lpoddata.CO_Worker[17], lpoddata.Temperature[17], lpoddata.Rel_Humidity[17],
              lpoddata.Fig1[18], lpoddata.Fig2[18], lpoddata.Fig3[18], lpoddata.CO_Auxiliary[18], lpoddata.CO_Worker[18], lpoddata.Temperature[18], lpoddata.Rel_Humidity[18]);
          delay(100);
          Particle.publish(eventName,buff,PRIVATE);
          #if SERIAL_ENABLED
            Serial.println();
            Serial.println(buff);
            Serial.println("Cellular Published");
          #endif //SERIAL_ENABLED
          i = 0;
          digitalWrite(BLUE_LED, LOW);
          digitalWrite(RED_LED, LOW);
        }
      }
    #endif //CELL_ENABLED

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