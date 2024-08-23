/***************************************************************************************
 * @project Landfill POD
 *
 * @file    LPOD_V1.0.0
 * @author  Percy Smith, percy.smith@colorado.edu
 * @brief   Central firmware to collect data through the LPOD designed by Rylee Beach
 * 
 * @date    July 14, 2024
 * @version V1.0.0
 * @log     The LPOD was completely re-designed, so I had to completely start over on firmware
 *          Additionally, because of hardware problems, I have not tested this yet!!! 
 *          Installed Arduino MKR NB 1500 Board Package
 *          RTC DS3231 firmware authored (success)
 *          Included ads_module - authored (success)
 *          BME 680 firmware authored (success - hardware modification)
 ***************************************************************************************/
#include <Arduino.h>          
#include <SPI.h>              //P - last tested with "SPI@1.0"      
#include <Wire.h>             //P - last tested with "Wire@1.0"
#include <SdFat.h>            //P - last tested with "SdFat@2.2.3"
#include <RTClib.h>           //P - last tested with "RTClib@2.1.4"
/*  Header Files  */
#include "LPOD_node.h"

/*  Module Initialization */
#if RTC_ENABLED 
  //RTC DS3231 Module - to re-initialize time, use RTClib>examples>ds3231
  RTC_DS3231 ds3231;  //DO NOT NAME THIS RTC IT THINKS ITS A MACRO !!!
  char bufftime[] = "YYYY-MM-DDThh:mm:ss";
  int Y,M,D,h,m,s;
#endif
#if SD_ENABLED
  SdFat sd;
  File file;
  char fileName[] = "LPODID_YYYY_MM_DD.CSV";
#endif
#if ADS_ENABLED
  #include "ads_module.h"
  ADS_Module ads_module;
  ads_data ads;
#endif
//BME 680 - Temperature, Pressure, Relative Humidity, VOC - Bosch (Replacement part)
#if BME_ENABLED
  #include <Adafruit_BME680.h>
  Adafruit_BME680 bme680;
#endif  //BME180

/***************************************************************************************/
void setup() {
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif  //SERIAL_ENABLED
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
  #if RTC_ENABLED
    ds3231.begin(); 
    DateTime now = ds3231.now();     //pulls setup() time so we have one file name per run in a day
      Y = now.year();
      M = now.month();
      D = now.day();
    // ds3231.adjust(DateTime(F(__DATE__), F(__TIME__)));
  #endif //RTC_ENABLED
  #if ADS_ENABLED
    ads_module.begin();
  #endif  //ADS_ENABLED
  /*  SD Card & File Setup  */
  #if SD_ENABLED
    digitalWrite(SD_CS, LOW);       //Pull SD_CS pin LOW to initialize SPI comms
    sd.begin(SD_CS);                //Initialize SD Card with relevant chip select pin
    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CS))  {
      #if SERIAL_ENABLED
          Serial.println(F("insert sd card to begin"));
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);      //attempt to initialize again
    } //while(!sd.begin(SD_CS))
    //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
    sprintf(fileName, "%s_%04u_%02u_%02u.CSV", lpodID, Y, M, D);    //char array for fileName
    delay(100);   
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
    file.close();                                       //close file, we opened so loop() is faster 
    digitalWrite(SD_CS, HIGH);    //release chip select on SD - allow other comm with SPI
  #endif  //SD_ENABLED
}

/***************************************************************************************/
void loop() {
  #if RTC_ENABLED
    DateTime now = ds3231.now();
    Y = now.year();  M = now.month();  D = now.day();  h = now.hour();  m = now.minute();  s = now.second();
    sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);
  #endif  //RTC_ENABLED
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
  #if SD_ENABLED
    digitalWrite(SD_CS, LOW);
    while (!sd.begin(SD_CS)) {
      #if SERIAL_ENABLED
        Serial.println("error in loop");
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);
    }
    if (sd.begin(SD_CS)) {
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE); 
      delay(100);
      if(file.isOpen()) {
        #if RTC_ENABLED
          file.print(bufftime);
          file.print(F(","));
        #endif  //RTC_ENABLED
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
          // file.print(ads.Unused); 
          // file.print(F(","));
          // delay(100);
          file.print(ads.Worker); 
          file.print(F(","));
          delay(100);
          // file.print(ads.Unused2); 
          // file.print(F(","));
          // delay(100);
          file.print(ads.Auxiliary); 
          file.print(F(","));
          delay(100);
          // file.print(ads.Unused3); 
          // file.print(F(","));
          // delay(100);
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
        #endif  //BME_ENABLED
      }
    }
  #endif  //SD_ENABLED
  //ECHO TO SERIAL 
  #if SERIAL_ENABLED
    Serial.println();
    #if RTC_ENABLED
      Serial.print(bufftime);
      Serial.print(F(","));
      delay(500);
    #endif  //RTC_ENABLED
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
      // Serial.print(ads.Unused); 
      // Serial.print(F(","));
      // delay(100);
      Serial.print(ads.Worker); 
      Serial.print(F(","));
      delay(100);
      // Serial.print(ads.Unused2); 
      // Serial.print(F(","));
      // delay(100);
      Serial.print(ads.Auxiliary); 
      Serial.print(F(","));
      delay(100);
      // Serial.print(ads.Unused3); 
      // Serial.print(F(","));
      // delay(100);
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
    #endif  //BME_ENABLED
  #endif  //SERIAL_ENABLED

}
