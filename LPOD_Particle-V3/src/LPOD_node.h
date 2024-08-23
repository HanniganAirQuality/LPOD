/*******************************************************************************
 * @file    LPOD_node.h
 * @brief   Settings and config
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    August 6, 2024
 * @log     Retrofitted LPOD_node.h from LPOD_V1.1.0 --> Particle
******************************************************************************/
#ifndef _LPOD_NODE_H
#define _LPOD_NODE_H

#include "Particle.h"

/****************** SENSOR CONFIGS ********************/
#define SERIAL_ENABLED        1
#define DS3231_ENABLED        1 //I2C
  #define ADJUST_DATETIME     0
#define SD_ENABLED            0 //SPI
#define ADS_ENABLED           1 //I2C
#define BME_ENABLED           1 //I2C
#define MET_ENABLED           0 //Analog & Digital
#define CELL_ENABLED          0

#define USE_DELAY              1

/****************** PIN DEFINITIONS ********************/
//SD Writing Definitions
#define SD_CS         4
//MET Station
#define MET_DIR       A1
#define MET_SPD       6
//LED Definitions
#define BLUE_LED      2
#define RED_LED       3
#define GREEN_LED     5

/****************** SET ADDR & CONST ********************/
#define BME_SENSOR_ADDR       (0x76)
#define SEALEVELPRESSURE_HPA  (1013.25)

const char lpodID[] = "LPODA3";

#endif // _LPOD_NODE_H
