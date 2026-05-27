/*******************************************************************************
 * @file    LPOD_DataStorage.h
 * @brief   Short Term Storage File
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 29, 2025
 * @log     Repairs the Worker & Auxiliary to be int16_t not uint16_t
******************************************************************************/
#ifndef _LPOD_DATASTORAGE_H
#define _LPOD_DATASTORAGE_H

#include "Particle.h"

#define RUN_TO_PUBLISH      19

struct LPOD_data
{
    uint16_t Fig1[RUN_TO_PUBLISH];
    uint16_t Fig2[RUN_TO_PUBLISH];
    uint16_t Fig3[RUN_TO_PUBLISH];
    int16_t CO_Worker[RUN_TO_PUBLISH];
    int16_t CO_Auxiliary[RUN_TO_PUBLISH]; // return to check firmware wtf u doing please omh

    float Temperature[RUN_TO_PUBLISH];
    float Rel_Humidity[RUN_TO_PUBLISH];
};

class LPOD {
    public:
        LPOD();

    private:

};

#endif    //BORON_DATA_STORAGE_H