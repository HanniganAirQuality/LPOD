/*******************************************************************************
 * @file    ads_module.cpp
 * @brief   Splits ADS1115 code from .ino & updates from ADS1015.h --> ADS1115.h
 *
 * @cite    XPOD >> ads_module.cpp by Ajay Kandagal, ajka9053@colorado.edu
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 14, 2024
 * @log     Retrofitted ads_module.cpp from YPOD_V3.5.0
******************************************************************************/
#include "ads_module.h"

/**************************************************************************/
 /*!
 *    @brief  ADS_Module object; Assigns addresses & channels to ADS1115 modules
 *            Addresses include: GND 0x48 & 5V 0x49
 */
/**************************************************************************/
ADS_Module::ADS_Module()
{
  ads_module[FIG1].addr = 0x48;
  ads_module[FIG1].channel = 0;

  ads_module[FIG2].addr = 0x48;
  ads_module[FIG2].channel = 1;

  ads_module[FIG3].addr = 0x48;
  ads_module[FIG3].channel = 2;

  ads_module[OPEN_CHANNEL].addr = 0x48;
  ads_module[OPEN_CHANNEL].channel = 3;

  ads_module[B4_WORKER].addr = 0x49;
  ads_module[B4_WORKER].channel = 0;

  ads_module[VOLT_REF1].addr = 0x49;
  ads_module[VOLT_REF1].channel = 1;

  ads_module[B4_AUXILIARY].addr = 0x49;
  ads_module[B4_AUXILIARY].channel = 2;

  ads_module[VOLT_REF2].addr = 0x49;
  ads_module[VOLT_REF2].channel = 3;

  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
    ads_module[i].status = false;
} //ADS_Module()

/**************************************************************************/
 /*!
 *   @brief  Start ADS_Module by init all ads sensors on ADS1115
 *   @return True if find ADS1115 channels (4 & 4), 
 *           False if status of channels is false
 */
/**************************************************************************/
bool ADS_Module::begin()
{
  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
  {
    if (ads_module[i].module.begin(ads_module[i].addr))
      ads_module[i].status = true;
  }

  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
  {
    if (ads_module[i].status == false)
      return false;
  }

  return true;
} //bool ADS_Module::begin()

/**************************************************************************/
 /*!
 *    @brief  Reads raw sensor readings; no signal processing!!
 *        @param  ads_sensor_id index of the sensor to be read (in id_e form)
 *    @return Raw ADS1115 reading for relevant channel (or -999 for error)
 */
/**************************************************************************/
uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)
{
  ads_module_t *sensor = &ads_module[ads_sensor_id];

  if (!sensor->status)
    return -999;

  return sensor->module.readADC_SingleEnded(sensor->channel);
} //uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)

/**************************************************************************/
 /*!
 *    @brief  Updates values and returns structured dataset
 *    @return ads_data structured dataset (w/o heaters)
 */
/**************************************************************************/
ads_data ADS_Module::return_updated()
{
  ads_data dataset;
  dataset.Fig1 = read_raw(FIG1);
  delay(100);
  dataset.Fig2 = read_raw(FIG2);
  delay(100);
  dataset.Fig3 = read_raw(FIG3);
  delay(100);
  // dataset.Unused = read_raw(OPEN_CHANNEL);
  delay(100);
  dataset.Worker = read_raw(B4_WORKER);
  delay(100);
  // dataset.Unused2 = read_raw(VOLT_REF1);
  delay(100);
  dataset.Auxiliary = read_raw(B4_AUXILIARY);
  delay(100);
  // dataset.Unused3 = read_raw(VOLT_REF2);
  delay(100);

  return dataset;
} //ads_data ADS_Module::return_updated()

