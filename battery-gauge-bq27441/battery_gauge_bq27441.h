/* mbed Microcontroller Library
 * Copyright (c) 2017 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BATTERY_GAUGE_BQ27441_H
#define BATTERY_GAUGE_BQ27441_H

/**
 * @file battery_gauge_bq27441.h
 * This file defines the API to the TI BQ27441 battery gauge chip.
 */

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/** Device I2C address. */
#define BATTERY_GAUGE_BQ27441_ADDRESS 0x55

/** The default seal code. */
#define SEAL_CODE_DEFAULT 0x8000

/* ----------------------------------------------------------------
 * CLASSES
 * -------------------------------------------------------------- */

/** BQ27441 battery gauge driver. */
class BatteryGaugeBq27441 {
public:

    /** Constructor. */
    BatteryGaugeBq27441(void);
    /** Destructor. */
    ~BatteryGaugeBq27441(void);

    /** Initialise the BQ27441 chip.  Once initialised
    * the chip is put into its lowest power state.  Any API call
    * will awaken the chip from this state and then return it once
    * more to the lowest possible power state.
    * @param pI2c a pointer to the I2C instance to use.
    * @param address 7-bit I2C address of the battery gauge chip.
    * @param sealCode the 16 bit seal code that will unseal the device if it is sealed.
    * @return true if successful, otherwise false.
    */
    bool init (I2C * pI2c, uint8_t address = BATTERY_GAUGE_BQ27441_ADDRESS, uint16_t sealCode = SEAL_CODE_DEFAULT);

    /** Switch on the battery gauge.  Battery gauging must be switched on
    * for the battery capacity and percentage readings to be valid. The
    * chip will consume more when battery gauging is switched on.
    * @param isSlow set this to true to save power if the battery current is not fluctuating very much.
    * @return true if successful, otherwise false.
    */
    bool enableGauge (bool isSlow = false);

    /** Switch off the battery gauge.
    * @return true if successful, otherwise false.
    */
    bool disableGauge (void);

    /** Check whether battery gauging is enabled or not.
    * @return true if battery gauging is enabled, otherwise false.
    */
    bool isGaugeEnabled(void);
    
    /** Enable the battery detect pin of the chip.
    * Default is enabled.
    * @return true if successful, otherwise false.
    */
    bool enableBatteryDetect ();

    /** Disable the battery detect pin of the chip and
    * assume that the battery is ALWAYS connected.
    * Default is that battery detect is enabled.
    * @return true if successful, otherwise false.
    */
    bool disableBatteryDetect (void);

    /** Determine whether a battery has been detected or not.
    * If battery detection is disabled, this function will always
    * return true.
    * @return true if a battery has been detected, otherwise false.
    */
    bool isBatteryDetected (void);
    
    /** Read the temperature of the BQ27441 chip.
    * If battery gauging is off this function will take ~1 second
    * to return while the ADCs are activated and the reading is taken.
    * If battery gauging is on, the last temperature reading taken
    * will be returned without delay.
    * @param pTemperatureC place to put the temperature reading.
    * @return true if successful, otherwise false.
    */
    bool getTemperature (int32_t *pTemperatureC);

    /** Read the voltage of the battery.
    * If battery gauging is off this function will take ~1 second
    * to return while the ADCs are activated and the reading is taken.
    * If battery gauging is on, the last voltage reading taken
    * will be returned without delay.
    * @param pVoltageMV place to put the voltage reading.
    * @return true if successful, otherwise false.
    */
    bool getVoltage (int32_t *pVoltageMV);

    /** Read the current flowing from the battery.
    * If battery gauging is off this function will take ~1 second
    * to return while the ADCs are activated and the reading is taken.
    * If battery gauging is on, the last current reading taken
    * will be returned without delay.
    * @param pCurrentMA place to put the current reading.
    * @return true if successful, otherwise false.
    */
    bool getCurrent (int32_t *pCurrentMA);

    /** Read the remaining available battery energy.
    * The battery capacity reading will only be valid if
    * battery gauging is switched on.
    * @param pCapacityMAh place to put the capacity reading.
    * @return true if successful, otherwise false.
    */
    bool getRemainingCapacity (int32_t *pCapacityMAh);

    /** Read the state of charge of the battery as a percentage.
    * The remaining percentage will only be valid if battery
    * gauging is switched on.
    * @param pBatteryPercent place to put the reading.
    * @return true if successful, otherwise false.
    */
    bool getRemainingPercentage (int32_t *pBatteryPercent);

    /** An advanced function to read configuration data from the BQ27441 chip memory.
    * Please refer to the TI BQ27441 technical reference manual for details of classId,
    * offset, the meanings of the data structures and their lengths.
    * PLEASE READ THIS HEADER FILE DIRECTLY, DOXYGEN MANGLES THE NEXT BIT
    * Note: the chip handles the data for each sub-class in 32 byte blocks and the offset/
    * length combination used must respect this.  For instance:
    *
    *    Sub-class N (length 87 bytes)
    *             bytes 0 to 31                      bytes 32 to 63                  bytes 64 to 86
    *     --------------------------------  --------------------------------  -----------------------
    *    |         Data Block 0           ||    xx    Data Block 1  yy      ||zz       Data Block 2  |
    *     --------------------------------  --------------------------------  -----------------------
    *
    * To read item xx, 2 bytes long at offset 36, one would specify an offset of 36 and a length
    * of 2.  To read both xx and yy at the same time (yy being 2 bytes long at offset 56),
    * one could specify an offset of 36 and a length of 21.  However, one could not read xx, yy
    * and zz at the same time, or yy and zz at the same time, since they fall into different blocks;
    * two separate reads are required.
    * @param subClassId the sub-class ID of the block.
    * @param offset the offset of the data within the class.
    * @param length the amount of data to read.
    * @param pData a place to put the read data.
    * @return true if successful, otherwise false.
    */
    bool advancedGetConfig(uint8_t subClassId, int32_t offset, int32_t length, char * pData);

    /** An advanced function to write configuration data to the BQ27441 chip memory.
    * Please refer to the TI BQ27441 technical reference manual for details of classId,
    * offset, the meanings of the data structures and their lengths.  See also the note above
    * advancedGetConfig() about how to use offset and length.  If this function is used to
    * change the seal code for the chip then init() should be called once more to
    * update the seal code used by this driver.
    * @param subClassId the sub-class ID of the block.
    * @param offset the offset of the data within the class.
    * @param length the length of the data to be written.
    * @param pData a pointer to the data to be written.
    * @return true if successful, otherwise false.
    */
    bool advancedSetConfig(uint8_t subClassId, int32_t offset, int32_t length, const char * pData);

    /** Send a control word (see section 4.1 of the BQ27441 technical reference manual).
    * @param controlWord the control word to send.
    * @param pDataReturned a place to put the word of data that could be returned,
    *        depending on which control word is used (may be NULL).
    * @return true if successful, otherwise false.
    */
    bool advancedSendControlWord (uint16_t controlWord, uint16_t *pDataReturned);
    
    /** Read two bytes starting at a given address on the chip.
    * See sections 4.2 to 4.20 of the BQ27441 technical reference manual for the list
    * of addresses.
    * @param address the start address to read from.  For instance, for temperature this is 0x02.
    * @param pDataReturned a place to put the word of data returned.
    * @return true if successful, otherwise false.
    */
    bool advancedGet (uint8_t address, uint16_t *pDataReturned);
    
    /** Check if the chip is SEALED or UNSEALED.
    * @return true if it is SEALED, otherwise false.
    */
    bool advancedIsSealed(void);

    /** Put the chip into SEALED mode.  SEALED mode is
    * used to prevent accidental writes to the chip when it
    * is in a production device.  All of the functions in this
    * class are able to work with a SEALED chip, provided the
    * correct seal code is provided to the init() function.
    * @return true if successful, otherwise false.
    */
    bool advancedSeal(void);
    
    /** Send the seal code to the chip to unseal it.
    * @param sealCode the 16 bit seal code that will unseal the chip if it is sealed.
    * @return true if successful, otherwise false.
    */
    bool advancedUnseal(uint16_t sealCode = SEAL_CODE_DEFAULT);

    /** Do a hard reset of the chip, reinitialising RAM data to defaults from ROM.
    * Note: the SEALED/UNSEALED status of the chip is unaffected.
    * @return true if successful, otherwise false.
    */
    bool advancedReset(void);
    
protected:
    /** Pointer to the I2C interface. */
    I2C * gpI2c;
    /** The address of the device. */
    uint8_t gAddress;
    /** The seal code for the device. */
    uint16_t gSealCode;
    /** Flag to indicate device is ready. */
    bool gReady;
    /** Flag to indicate that monitor mode is active. */
    bool gGaugeOn;

    /** Read two bytes starting at a given address.
    * Note: gpI2c should be locked before this is called.
    * @param registerAddress the register address to start reading from.
    * @param pBytes place to put the two bytes.
    * @return true if successful, otherwise false.
    */
    bool getTwoBytes (uint8_t registerAddress, uint16_t *pBytes);

    /** Compute the checksum of a block of memory in the chip.
    * @param pData a pointer to the 32 byte data block.
    * @return the checksum value.
    */
    uint8_t computeChecksum(const char * pData);

    /** Read data of a given length and class ID.
    * Note: gpI2c should be locked before this is called.
    * @param subClassId the sub-class ID of the block.
    * @param offset the offset of the data within the class.
    * @param pData a place to put the read data.
    * @param length the size of the place to put the data block.
    * @return true if successful, otherwise false.
    */
    bool readExtendedData(uint8_t subClassId, int32_t offset, int32_t length, char * pData);
    
    /** Write an extended data block.
    * Note: gpI2c should be locked before this is called.
    * @param subClassId the sub-class ID of the block.
    * @param offset the offset of the data within the class.
    * @param pData a pointer to the data to be written.
    * @param length the size of the data to be written.
    * @return true if successful, otherwise false.
    */
    bool writeExtendedData(uint8_t subClassId, int32_t offset, int32_t length, const char * pData);

    /** Check if the chip is SEALED or UNSEALED.
    * Note: gpI2c should be locked before this is called.
    * @return true if it is SEALED, otherwise false.
    */
    bool isSealed(void);

    /** Put the chip into SEALED mode.
    * Note: gpI2c should be locked before this is called.
    * @return true if successful, otherwise false.
    */
    bool seal(void);

    /** Unseal the chip.
    * Note: gpI2c should be locked before this is called.
    * @param sealCode the 16 bit seal code that will unseal the chip if it is sealed.
    * @return true if successful, otherwise false.
    */
    bool unseal(uint16_t sealCode);
    
    /** Make sure that the chip is awake and has taken a reading.
    * Note: the function does its own locking of gpI2C so that it isn't
    * held for the entire time we wait for ADC readings to complete.
    * @return true if successful, otherwise false.
    */
    bool makeAdcReading(void);

    /** Set Hibernate mode.
    * Note: gpI2c should be locked before this is called.
    * @return true if successful, otherwise false.
    */
    bool setHibernate(void);
};

#endif

/* End Of File */
