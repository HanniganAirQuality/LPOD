/***************************************************************************************
 * @project Landfill POD
 *
 * @file    LPOD_V2-0-0
 * @author  Percy Smith, percy.smith@colorado.edu
 * @brief   Initial test for POC of moving to Particle (Electron/Boron) from Arduino MKR 1500 
 * 
 * @date    July 30, 2024
 * @version V2.0.0
 * @log     V2 is a version that's designed to work on the Particle platform 
 ***************************************************************************************/
#include "Particle.h"
#include "LPOD_node.h"

/* RECOMMENDED SETTINGS */
// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);
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

// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here

  Particle.connect();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
