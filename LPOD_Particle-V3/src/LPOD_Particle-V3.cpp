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
  const std::chrono::milliseconds logPeriod = 5s;
  unsigned long lastLog;
#endif

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
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  // if (Network.listening())
  // {
  //     // If we are in listening mode (blinking dark blue), don't
  //     // output by USB serial, because it can conflict with
  //     // serial commands.
  //     return;
  // }
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
  }
  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
