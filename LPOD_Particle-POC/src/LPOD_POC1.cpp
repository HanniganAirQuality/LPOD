/***************************************************************************************
 * @project Landfill POD
 *
 * @file    LPOD_POC1
 * @author  Percy Smith, percy.smith@colorado.edu
 * @brief   Initial test for POC of moving to Particle (Electron/Boron) from Arduino MKR 1500 
 * 
 * @date    July 30, 2024
 * @version POC1
 * @log     POC1 is a proof-of-concept version that's designed to work on the Particle platform 
 ***************************************************************************************/
#include "Particle.h"
#include "LPOD_node.h"

/* RECOMMENDED SETTINGS */
// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);
// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

/* MODULE INITIALIZATION */
#if DS3231_ENABLED 
  #include <Adafruit_RTClib_RK.h>
  RTC_DS3231 ds3231;  //DO NOT NAME THIS RTC IT THINKS ITS A MACRO !!!
  char bufftime[] = "YYYY-MM-DDThh:mm:ss";
  int Y,M,D,h,m,s;
#endif
#if SD_ENABLED
  #include <SdFat.h>
  SdFat sd;
  SdFile file;
  char fileName[] = "LPODID_YYYY_MM_DD.CSV";
#endif
#if ADS_ENABLED
  #include "ads_module.h"
  ADS_Module ads_module;
  ads_data ads;
#endif
#if BME_ENABLED
  #include "Adafruit_BME680.h" //for some reason it gotta be him--> "" ?
  Adafruit_BME680 bme680;
#endif
#if MET_ENABLED
  #include "wind_vane.h"
  long lastWindCheck = 0;
  volatile long lastWindIRQ = 0;
  volatile byte windClicks = 0;
  wind_vane windVane;
  void wspeedIRQ()  {
    if(millis() - lastWindIRQ > 10)
    {
      lastWindIRQ = millis();
      windClicks++;
    }
  } //void wspeedIRQ()
  
  float get_wind_speed(){
    float deltaTime = millis() - lastWindCheck; //750ms

    deltaTime /= 1000.0; //Covert to seconds

    float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

    windClicks = 0; //Reset and start watching for new wind
    lastWindCheck = millis();

    windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

    return (windSpeed);
  } //float get_wind_speed()
#endif //MET_ENABLED

/***************************************************************************************/
// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
  #if SERIAL_ENABLED
    Serial.begin(9600);
  #endif
  Wire.begin();
  SPI.begin();

  #if CELL_ENABLED
    Particle.connect();
  #endif
  #if SD_ENABLED
    pinMode(SD_CS, OUTPUT);
  #endif

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
  #if DS3231_ENABLED
    ds3231.begin();
    #if ADJUST_DATETIME
      ds3231.adjust(DateTime(F(__DATE__), F(__TIME__))); 
    #endif  //ADJUST_DATETIME
    DateTime now = ds3231.now();     //pulls setup() time so we have one file name per run in a day
      Y = now.year();
      M = now.month();
      D = now.day();
  #endif //DS3231_ENABLED
  #if ADS_ENABLED
    ads_module.begin();
  #endif  //ADS_ENABLED
  #if MET_ENABLED
    pinMode(MET_SPD, INPUT);
    pinMode(MET_DIR, INPUT);
    attachInterrupt(digitalPinToInterrupt(MET_SPD), wspeedIRQ, FALLING);  // attaching wind speed interrupt to pin 3
  #endif
  /*  SD Card & File Setup  */
  #if SD_ENABLED
    digitalWrite(SD_CS, LOW);       //Pull SD_CS pin LOW to initialize SPI comms
    delay(100);
    sd.begin(SD_CS);                //Initialize SD Card with relevant chip select pin
    delay(100);
    // Serial.println(sd.hasDedicatedSpi());

    // Establish contact with SD card - if initialization fails, run until success
    while (!sd.begin(SD_CS))  { //getting stuck here...
      #if SERIAL_ENABLED
          Serial.println(F("insert sd card to begin"));
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);      //attempt to initialize again
    } //while(!sd.begin(SD_CS))
    //File Naming (FORMATTING HAS TO BE CONSISTENT WITH GLOBAL DECLARATION!!)
    sprintf(fileName, "%s_%04u_%02u_%02u.CSV", lpodID, Y, M, D);    //char array for fileName
    delay(100);   
    file.open(fileName, O_CREAT | O_APPEND | O_WRITE);  //open with create, append, write permissions
    delay(100);
    file.close();                                       //close file, we opened so loop() is faster 
    digitalWrite(SD_CS, HIGH);    //release chip select on SD - allow other comm with SPI
  #endif  //SD_ENABLED
}

/***************************************************************************************/
// loop() runs over and over again, as quickly as it can execute.
void loop() {
  #if DS3231_ENABLED
    DateTime now = ds3231.now();
    Y = now.year();  M = now.month();  D = now.day();  h = now.hour();  m = now.minute();  s = now.second();
    sprintf(bufftime, "%04u-%02u-%02uT%02u:%02u:%02u", Y, M, D, h, m, s);
  #endif  //DS3231_ENABLED
  #if MET_ENABLED
    float wind_speed = get_wind_speed();
    float wind_dir_volt = windVane.get_direction();
    float wind_dir_degree = windVane.degree_direction(wind_dir_volt);
    String wind_dir_cardinal = windVane.cardinal_direction(wind_dir_volt);
  #endif //MET_ENABLED Data Gathering
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
    delay(100);
    while (!sd.begin(SD_CS)) {
      #if SERIAL_ENABLED
        Serial.println("Issues opening SD in loop");
      #endif  //SERIAL_ENABLED
      sd.begin(SD_CS);
    }
    if (sd.begin(SD_CS)) {
      delay(150);
      file.open(fileName, O_CREAT | O_APPEND | O_WRITE); 
      delay(100);
      // if(file.isOpen()) {
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
        delay(100);
        file.print(bme680.readPressure());
        file.print(F(","));
        delay(100);
        file.print(bme680.readHumidity());
        file.print(F(","));
        delay(100);
        file.print(bme680.readGas());
        file.print(F(","));
        delay(100);
      #endif  //BME_ENABLED
      #if MET_ENABLED
        file.print(wind_speed);
        file.print(F(","));
        delay(100);
        file.print(wind_dir_volt);
        file.print(F(","));
        delay(100);
        // file.print(wind_dir_cardinal);
        // file.print(F(","));
      #endif //MET_ENABLED
      file.println();
      delay(100);
      file.sync();
      file.close();
      delay(1000);
      digitalWrite(SD_CS, HIGH);
      // } //if file.isOpen();
    } //if (sd.begin(SD_CS))
  #endif  //SD_ENABLED
  //ECHO TO SERIAL 
  #if SERIAL_ENABLED
    Serial.println();
    #if DS3231_ENABLED
      Serial.print(bufftime);
      Serial.print(F(","));
      delay(500);
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
    #if MET_ENABLED
      Serial.print(wind_speed);
      Serial.print(F(","));
      Serial.print(wind_dir_volt);
      // Serial.print(F(","));
      // Serial.print(wind_dir_cardinal);
      // Serial.print(F(","));
    #endif //MET_ENABLED
  #endif  //SERIAL_ENABLED

}
