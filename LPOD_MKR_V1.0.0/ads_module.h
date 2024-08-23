/*******************************************************************************
 * @file    ads_module.h
 * @brief   Splits ADS1115 code from .ino & updates from ADS1015.h --> ADS1115.h
 *
 * @cite    XPOD >> ads_module.h by Ajay Kandagal, ajka9053@colorado.edu
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 14, 2024
 * @log     Retrofitted ads_module.h from YPOD_V3.5.0
******************************************************************************/
#ifndef _ADS_MODULE_H
#define _ADS_MODULE_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

#include "LPOD_node.h"

/*! Index: FIG1, FIG2, FIG3, OPEN_CHANNEL, B4_WORKER, VOLT_REF1, B4_AUXILIARY, VOLT_REF2, ADS_SENSOR_COUNT */
enum ads_sensor_id_e
{
  FIG1 = 0,
  FIG2,
  FIG3,
  OPEN_CHANNEL,
  B4_WORKER,
  VOLT_REF1,
  B4_AUXILIARY,
  VOLT_REF2,
  ADS_SENSOR_COUNT
};  //enum ads_sensor_id_e

/*! (per each sensor) addr, channel, status, module (ADS1115) */
struct ads_module_t
{
    uint8_t addr;
    int8_t channel;
    bool status;
    Adafruit_ADS1115 module;
}; //struct ads_module_t

/*! ADS data structure (ALL DATA) as uint16_t */
struct ads_data
{
  uint16_t Fig1;
  uint16_t Fig2;
  uint16_t Fig3;
  // uint16_t Unused;
  uint16_t Worker;
  // uint16_t Unused2;
  uint16_t Auxiliary;
  // uint16_t Unused3;
};  //struct ads_heaters

/*! ADS1115 to include 3 Figaros and an Alphasense B4 Sensor */
class ADS_Module {
  public:
    ADS_Module();
    bool begin();

    uint16_t read_raw(ads_sensor_id_e ads_sensor_id);
    ads_data return_updated();

  private:
    ads_module_t ads_module[ADS_SENSOR_COUNT];
};  //class ADS_Module

#endif  //_ADS_MODULE_H
