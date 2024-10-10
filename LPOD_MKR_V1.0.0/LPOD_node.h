/*******************************************************************************
 * @file    LPOD_node.h
 * @brief   Settings and config
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 14, 2024
 * @log     The LPOD was completely re-designed, so I had to completely start over on firmware
******************************************************************************/
#ifndef _LPOD_NODE_H
#define _LPOD_NODE_H

#include <Arduino.h>

/****************** SENSOR CONFIGS ********************/
#define SERIAL_ENABLED        1
#define RTC_ENABLED           1 //I2C
#define SD_ENABLED            0 //SPI
#define ADS_ENABLED           1 //I2C
#define BME_ENABLED           1 //I2C
#define MET_ENABLED           0 //Analog & Digital
#define CELL_ENABLED          0

/****************** PIN DEFINITIONS ********************/
//SD Writing Definitions
#define SD_CS         4
//MET Station
#define MET_DIR       A6
#define MET_SPD       6
//LED Definitions
#define BLUE_LED      2
#define RED_LED       3
#define GREEN_LED     5

#define BME_SENSOR_ADDR       (0x76)

const char lpodID[] = "LPODA2";

#endif // _LPOD_NODE_H
