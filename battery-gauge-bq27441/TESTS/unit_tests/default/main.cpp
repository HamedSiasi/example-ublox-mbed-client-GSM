#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "battery_gauge_bq27441.h"

using namespace utest::v1;

// ----------------------------------------------------------------
// COMPILE-TIME MACROS
// ----------------------------------------------------------------

// Pick some sensible minimum and maximum numbers
#define MAX_TEMPERATURE_READING_C  80
#define MIN_TEMPERATURE_READING_C -20
#define MIN_VOLTAGE_READING_MV     0
#define MAX_VOLTAGE_READING_MV     12000 // Bigger than a 3 cell LiPo
#define MAX_CURRENT_READING_MA     2000
#define MIN_CURRENT_READING_MA    -2000
#define MIN_CAPACITY_READING_MAH   0
#define MAX_CAPACITY_READING_MAH   30000 // A very big battery indeed

// The maximum size of configuration block
// that we can handle in one go
#define MAX_CONFIG_BLOCK_SIZE 32

// ----------------------------------------------------------------
// PRIVATE VARIABLES
// ----------------------------------------------------------------

// I2C interface
I2C * gpI2C = new I2C(I2C_SDA_B, I2C_SCL_B);

// An empty array, so that we can check for emptiness
static const char zeroArray[MAX_CONFIG_BLOCK_SIZE] = {0};

// ----------------------------------------------------------------
// PRIVATE FUNCTIONS
// ----------------------------------------------------------------

// Print a buffer as a nice hex string
static void printBytesAsHex(const char * pBuf, uint32_t size)
{
    uint32_t x;

    printf (" 0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
    for (x = 1; x <= size; x++, pBuf++)
    {
        if (x % 16 == 8) {
            printf ("%02x  ", *pBuf);
        } else if (x % 16 == 0) {
            printf ("%02x\n", *pBuf);
        } else {
            printf ("%02x-", *pBuf);
        }
    }
    
    if (x % 16 !=  1) {
        printf("\n");
    }
}

// ----------------------------------------------------------------
// TESTS
// ----------------------------------------------------------------

// Test that the BQ27441 battery gauge can be initialised
void test_init() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    
    TEST_ASSERT_FALSE(pBatteryGauge->init(NULL));
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
}

// Test that battery capacity monitoring can be performed
void test_monitor() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->enableGauge());
    
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
#ifdef TARGET_UBLOX_C030
    // Battery detection is not supported on C030
    TEST_ASSERT(pBatteryGauge->disableBatteryDetect());
#endif

    // Normal case
    TEST_ASSERT(pBatteryGauge->enableGauge());
    TEST_ASSERT(pBatteryGauge->isGaugeEnabled());

    // TODO do something to assess whether it's actually working
    TEST_ASSERT(pBatteryGauge->disableGauge());
    TEST_ASSERT(!pBatteryGauge->isGaugeEnabled());
    
    // Normal case, slow mode
    TEST_ASSERT(pBatteryGauge->enableGauge(true));    
    TEST_ASSERT(pBatteryGauge->isGaugeEnabled());
    // TODO do something to assess whether it's actually working slowly
    TEST_ASSERT(pBatteryGauge->disableGauge());
    TEST_ASSERT(!pBatteryGauge->isGaugeEnabled());
}

// Test that battery detection can be performed
// TODO: find a way to check that a battery is not detected correctly
void test_battery_detection() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->isBatteryDetected());
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
#ifdef TARGET_UBLOX_C030
    // Battery detection is not supported on C030
    TEST_ASSERT(pBatteryGauge->disableBatteryDetect());
#endif
    TEST_ASSERT(pBatteryGauge->isBatteryDetected());
}

// Test that a temperature reading can be performed
void test_temperature() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    int32_t temperatureC = MIN_TEMPERATURE_READING_C - 1;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->getTemperature(&temperatureC));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    TEST_ASSERT(pBatteryGauge->getTemperature(&temperatureC));
    printf ("Temperature %d C.\n", (int) temperatureC);
    // Range check
    TEST_ASSERT((temperatureC >= MIN_TEMPERATURE_READING_C) && (temperatureC <= MAX_TEMPERATURE_READING_C));
    
    // The parameter is allowed to be NULL
    TEST_ASSERT(pBatteryGauge->getTemperature(NULL));
}

// Test that a voltage reading can be performed
void test_voltage() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    int32_t voltageMV = MIN_VOLTAGE_READING_MV - 1;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->getVoltage(&voltageMV));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    TEST_ASSERT(pBatteryGauge->getVoltage(&voltageMV));
    printf ("Voltage %.3f V.\n", ((float) voltageMV) / 1000);
    // Range check
    TEST_ASSERT((voltageMV >= MIN_VOLTAGE_READING_MV) && (voltageMV <= MAX_VOLTAGE_READING_MV));
    
    // The parameter is allowed to be NULL
    TEST_ASSERT(pBatteryGauge->getVoltage(NULL));
}

// Test that a current reading can be performed
void test_current() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    int32_t currentMA = MIN_CURRENT_READING_MA - 1;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->getCurrent(&currentMA));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    TEST_ASSERT(pBatteryGauge->getCurrent(&currentMA));
    printf ("Current %.3f A.\n", ((float) currentMA) / 1000);
    // Range check
    TEST_ASSERT((currentMA >= MIN_CURRENT_READING_MA) && (currentMA <= MAX_CURRENT_READING_MA));
    
    // The parameter is allowed to be NULL
    TEST_ASSERT(pBatteryGauge->getCurrent(NULL));
}

// Test that a remaining capacity reading can be performed
void test_remaining_capacity() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    int32_t capacityMAh = MIN_CAPACITY_READING_MAH - 1;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->getRemainingCapacity(&capacityMAh));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    TEST_ASSERT(pBatteryGauge->getRemainingCapacity(&capacityMAh));
    printf ("Remaining capacity %.3f Ah.\n", ((float) capacityMAh) / 1000);
    // Range check
    TEST_ASSERT((capacityMAh >= MIN_CAPACITY_READING_MAH) && (capacityMAh <= MAX_CAPACITY_READING_MAH));

    // The parameter is allowed to be NULL
    TEST_ASSERT(pBatteryGauge->getRemainingCapacity(NULL));
}

// Test that a remaining percentage reading can be performed
void test_remaining_percentage() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    int32_t batteryPercent = 101;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->getRemainingPercentage(&batteryPercent));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
#ifdef TARGET_UBLOX_C030
    // Battery detection is not supported on C030
    // and battery detect is required for the "gauging" type APIs
    TEST_ASSERT(pBatteryGauge->disableBatteryDetect());
#endif
    TEST_ASSERT(pBatteryGauge->getRemainingPercentage(&batteryPercent));
    printf ("Remaining percentage %d%%.\n", (int) batteryPercent);
    // Range check
    TEST_ASSERT((batteryPercent >= 0) && (batteryPercent <= 100));

    // The parameter is allowed to be NULL
    TEST_ASSERT(pBatteryGauge->getRemainingPercentage(NULL));
}

// Test advanced functions to read the configuration of the chip
void test_advanced_config_1() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t subClassId = 80; // IT Cfg
    int32_t offset = 0;
    int32_t length = MAX_CONFIG_BLOCK_SIZE - offset;
    char data1[MAX_CONFIG_BLOCK_SIZE];
    uint32_t deadArea1 = 0xdeadbeef;
    char data2[MAX_CONFIG_BLOCK_SIZE];
    uint32_t deadArea2 = 0xdeadbeef;
    
    // Initialise the battery gauge
    TEST_ASSERT(pBatteryGauge->init(gpI2C));

    // Read IT Cfg (total length 79 bytes), starting from 0, into data1
    subClassId = 80;
    memset(&(data1[0]), 0, sizeof (data1));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data1[0]), length);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea1);
    
    // Read it again, with an offset of 16 bytes, into data2
    offset = 16;
    length = MAX_CONFIG_BLOCK_SIZE - 16;
    memset(&(data2[0]), 0, sizeof (data2));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data2[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data2[0]), length);
    // The second 16 bytes of data1 and the first 16 bytes of data2 should match
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&(data1[16]), &(data2[0]), 16);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea2);

    // Read the next block of IT Cfg into data1
    offset = MAX_CONFIG_BLOCK_SIZE;
    length = MAX_CONFIG_BLOCK_SIZE;
    memset(&(data1[0]), 0, sizeof (data1));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data1[0]), length);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea1);

    // Read the only the first 16 bytes, from the same offset into IT Cfg, into data2
    length = 16;
    memset(&(data2[0]), 0, sizeof (data2));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data2[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data2[0]), length);
    // The first 16 bytes of data1 and data2 should match
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&(data1[0]), &(data2[0]), length);
    // The remainder of data2 should be zero
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&(zeroArray[0]), &(data2[length]), sizeof(data2) - length);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea2);
}

// Test advanced functions to write configuration to the chip
void test_advanced_config_2() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t subClassId = 80; // IT Cfg
    int32_t offset = 0;
    int32_t length = MAX_CONFIG_BLOCK_SIZE - offset;
    char data1[MAX_CONFIG_BLOCK_SIZE];
    uint32_t deadArea1 = 0xdeadbeef;
    char data2[MAX_CONFIG_BLOCK_SIZE];
    
    // Initialise the battery gauge
    TEST_ASSERT(pBatteryGauge->init(gpI2C));

    // Read Delta Voltage, two bytes at offset 39 in sub-class State, into data1
    subClassId = 82;
    offset = 39;
    length = 2;
    memset(&(data1[0]), 0, sizeof (data1));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data1[0]), length);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea1);
    
    // Copy Delta Voltage, change the lower byte and then write it back
    (data1[1])++;
    printf ("Modified data block:\n");
    printBytesAsHex(&(data1[0]), length);
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
    printf("%d bytes written to subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);

    // Read it back and check that the Delta Voltage really is the new value
    subClassId = 82;
    offset = 32;
    length = 9;
    memset(&(data2[0]), 0, sizeof (data2));
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data2[0])));
    printf("%d bytes received from subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
    printBytesAsHex(&(data2[0]), length);
    TEST_ASSERT_EQUAL_UINT32 (0xdeadbeef, deadArea1);
    TEST_ASSERT_EQUAL_UINT32 (data1[0], data2[7]);
    TEST_ASSERT_EQUAL_UINT32 (data1[1], data2[8]);
    
    // Now put Delta Voltage back as it was
    (data2[8])--;
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    printf("%d bytes written to subClassID %d, offset %d:\n", (int) length, subClassId, (int) offset);
}

// Test fail cases of the advanced configuration functions
void test_advanced_config_3() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t subClassId = 80; // IT Cfg
    int32_t offset = 0;
    int32_t length = MAX_CONFIG_BLOCK_SIZE - offset;
    char data1[MAX_CONFIG_BLOCK_SIZE];
    
    // All calls should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
        
    // Initialise the battery gauge
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    
    // Perform some reads of bad length/offset combinations
    offset = 0;
    length = 33;
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
    offset = 1;
    length = 32;
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
    offset = 31;
    length = 2;
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
    offset = 32;
    length = 33;
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data1[0])));
}

// Send a control word to the BQ27441 battery gauge chip
void test_advanced_control() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint16_t controlWord = 0x0002; // get FW version
    uint16_t response = 0;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSendControlWord(controlWord, &response));
    
    // Initialise the battery gauge
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->advancedSendControlWord(controlWord, &response));
    // FW version must be 0x0109
    TEST_ASSERT_EQUAL_UINT16(0x0109, response);

    // The parameter is allowed to be null
    TEST_ASSERT(pBatteryGauge->advancedSendControlWord(controlWord, NULL));
}

// Read using a standard command from the BQ27441 battery gauge chip
void test_advanced_get() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t address = 0x02; // Temperature
    uint16_t value = 0;
    int32_t temperatureC = -1;
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->advancedGet(address, &value));
    
    // Initialise the battery gauge
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    
    // Normal case
    TEST_ASSERT(pBatteryGauge->advancedGet(address, &value));
    // Get the temperature via the standard API command
    TEST_ASSERT(pBatteryGauge->getTemperature(&temperatureC));
    // Convert the value returned into a temperature reading and compare
    // it with the real answer, allowing a 1 degree tolerance in case
    // it has changed between readings.
    TEST_ASSERT_INT32_WITHIN (1, temperatureC, ((int32_t) value / 10) - 273);

    // The parameter is allowed to be null
    TEST_ASSERT(pBatteryGauge->advancedGet(address, NULL));
}

// Test that the chip can be sealed and unsealed
void test_advanced_seal() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t subClassId = 80; // IT Cfg
    int32_t offset = 78; // Position of the "TermV valid t" item at offset 78
    int32_t length = 1; // Length of "TermV valid t"
    char data1[MAX_CONFIG_BLOCK_SIZE];
    char data2[MAX_CONFIG_BLOCK_SIZE];
    char data3[MAX_CONFIG_BLOCK_SIZE];
    int32_t value;
    
    memset(&(data1[0]), 0, sizeof (data1));
    memset(&(data2[0]), 0, sizeof (data2));
    memset(&(data3[0]), 0, sizeof (data3));

    // Make sure that the device is not sealed from a previous field test run
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
#ifdef TARGET_UBLOX_C030
    // Battery detection is not supported on C030
    TEST_ASSERT(pBatteryGauge->disableBatteryDetect());
#endif
    TEST_ASSERT(pBatteryGauge->advancedUnseal());

    delete pBatteryGauge;
    pBatteryGauge = new BatteryGaugeBq27441();
    // Calls should fail if the battery gauge has not been initialised
    printf ("Calling advancedIsSealed()...\n");
    TEST_ASSERT_FALSE(pBatteryGauge->advancedIsSealed());
    printf ("Calling advancedSeal()...\n");
    TEST_ASSERT_FALSE(pBatteryGauge->advancedSeal());
    printf ("Calling advancedUnseal()...\n");
    TEST_ASSERT_FALSE(pBatteryGauge->advancedUnseal());
    
    // Normal case
    printf ("Calling init()...\n");
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    printf ("Calling advancedIsSealed()...\n");
    TEST_ASSERT_FALSE(pBatteryGauge->advancedIsSealed());
    // This call should pass
    printf ("Calling advancedGetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId , offset, length, &(data1[0])));
    
    // Now seal it
    printf ("Calling advancedSeal()...\n");
    TEST_ASSERT(pBatteryGauge->advancedSeal());
    printf ("Calling advancedIsSealed()...\n");
    TEST_ASSERT(pBatteryGauge->advancedIsSealed());
    memcpy (&(data2[0]), &(data1[0]), sizeof (data2));
    // Try to increment the "TermV valid t" item
    (data2[0])++;
    // These calls should all be unaffected by sealing
    printf ("Calling advancedSetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    printf ("Calling advancedGetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data3[0])));
    TEST_ASSERT(memcmp (&(data2[0]), &(data3[0]), sizeof (data2)) == 0);
     // Put "TermV valid t" back as it was
    (data2[0])--;
    printf ("Calling advancedSetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    printf ("Calling enableGauge()...\n");
    TEST_ASSERT(pBatteryGauge->enableGauge());
    printf ("Calling isBatteryDetected()...\n");
    TEST_ASSERT(pBatteryGauge->isBatteryDetected());
    printf ("Calling getTemperature()...\n");
    TEST_ASSERT(pBatteryGauge->getTemperature(&value));
    printf ("Calling getVoltage()...\n");
    TEST_ASSERT(pBatteryGauge->getVoltage(&value));
    printf ("Calling getCurrent()...\n");
    TEST_ASSERT(pBatteryGauge->getCurrent(&value));
    printf ("Calling getRemainingCapacity()...\n");
    TEST_ASSERT(pBatteryGauge->getRemainingCapacity(&value));
    printf ("Calling getRemainingPercentage()...\n");
    TEST_ASSERT(pBatteryGauge->getRemainingPercentage(&value));
    printf ("Calling enableGauge(\"true\")...\n");
    TEST_ASSERT(pBatteryGauge->enableGauge(true));
    printf ("Calling disableGauge()...\n");
    TEST_ASSERT(pBatteryGauge->disableGauge());

    // Now unseal it
    printf ("Calling advancedUnseal()...\n");
    TEST_ASSERT(pBatteryGauge->advancedUnseal());
    printf ("Calling advancedIsSealed()...\n");
    TEST_ASSERT_FALSE(pBatteryGauge->advancedIsSealed());
    // These calls should all still work
    // Try to increment the "TermV valid t" item
    (data2[0])++;
    printf ("Calling advancedSetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    printf ("Calling advancedGetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data3[0])));
    TEST_ASSERT(memcmp (&(data2[0]), &(data3[0]), sizeof (data2)) == 0);
     // Put "TermV valid t" back as it was
    (data2[0])--;
    printf ("Calling advancedSetConfig()...\n");
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    printf ("Calling enableGauge(\"true\")...\n");
    TEST_ASSERT(pBatteryGauge->enableGauge(true));
    printf ("Calling isBatteryDetected()...\n");
    TEST_ASSERT(pBatteryGauge->isBatteryDetected());
    printf ("Calling getTemperature()...\n");
    TEST_ASSERT(pBatteryGauge->getTemperature(&value));
    printf ("Calling getVoltage()...\n");
    TEST_ASSERT(pBatteryGauge->getVoltage(&value));
    printf ("Calling getCurrent()...\n");
    TEST_ASSERT(pBatteryGauge->getCurrent(&value));
    printf ("Calling getRemainingCapacity()...\n");
    TEST_ASSERT(pBatteryGauge->getRemainingCapacity(&value));
    printf ("Calling getRemainingPercentage()...\n");
    TEST_ASSERT(pBatteryGauge->getRemainingPercentage(&value));
    printf ("Calling disableGauge()...\n");
    TEST_ASSERT(pBatteryGauge->disableGauge());

    // TODO: I had some tests in here to check that init() and
    // advancedUnseal() behave when given the wrong seal code.
    // However, as soon as the chip gets a wrong seal code it
    // refuses to unseal again (I tried a 4 second delay but
    // that didn't help).  This needs investigating.
}

// Reset the BQ27441 battery gauge chip at the outset
void test_advanced_reset() {
    BatteryGaugeBq27441 * pBatteryGauge = new BatteryGaugeBq27441();
    uint8_t subClassId = 80; // IT Cfg
    int32_t offset = 78; // Position of the "TermV valid t" item at offset 78
    int32_t length = 1;  // Length of "TermV valid t"
    char data1[MAX_CONFIG_BLOCK_SIZE];
    char data2[MAX_CONFIG_BLOCK_SIZE];
    char data3[MAX_CONFIG_BLOCK_SIZE];
    
    memset(&(data1[0]), 0, sizeof (data1));
    memset(&(data2[0]), 0, sizeof (data2));
    memset(&(data3[0]), 0, sizeof (data3));
    
    // Call should fail if the battery gauge has not been initialised
    TEST_ASSERT_FALSE(pBatteryGauge->advancedReset());
    
    TEST_ASSERT(pBatteryGauge->init(gpI2C));
    TEST_ASSERT(pBatteryGauge->advancedUnseal());
    
    // Normal case
    // Increment the "TermV valid t" item
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data1[0])));
    memcpy (&(data2[0]), &(data1[0]), sizeof (data2));
    (data2[0])++;
    TEST_ASSERT(pBatteryGauge->advancedSetConfig(subClassId, offset, length, &(data2[0])));
    // Read it back to make sure it was set
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data3[0])));
    TEST_ASSERT(memcmp (&(data2[0]), &(data3[0]), sizeof (data2)) == 0);
    
    // Now reset the chip and check that the value is back to what it was before
    TEST_ASSERT(pBatteryGauge->advancedReset());
    TEST_ASSERT(pBatteryGauge->advancedGetConfig(subClassId, offset, length, &(data3[0])));
    TEST_ASSERT(memcmp (&(data1[0]), &(data3[0]), sizeof (data1)) == 0);
}

// ----------------------------------------------------------------
// TEST ENVIRONMENT
// ----------------------------------------------------------------

// Setup the test environment
utest::v1::status_t test_setup(const size_t number_of_cases) {
    // Setup Greentea, timeout is long enough to run these tests with
    // DEBUG_BQ27441 defined
    // Note: timeout is quite long as the chip has 4 second
    // timeouts in quite a lot of cases.
    GREENTEA_SETUP(480, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

// Test cases
Case cases[] = {
    Case("Initialisation", test_init),
    Case("Monitoring", test_monitor),
    Case("Battery detection", test_battery_detection),
    Case("Temperature read", test_temperature),
    Case("Voltage read", test_voltage),
    Case("Current read", test_current),
    Case("Remaining capacity read", test_remaining_capacity),
    Case("Remaining percentage read", test_remaining_percentage),
    Case("Advanced config read", test_advanced_config_1),
    Case("Advanced config write", test_advanced_config_2),
    Case("Advanced config read/write fail cases", test_advanced_config_3),
    Case("Advanced control", test_advanced_control),
    Case("Advanced get", test_advanced_get),
    Case("Advanced seal", test_advanced_seal),
    Case("Advanced reset", test_advanced_reset)
};

Specification specification(test_setup, cases);

// ----------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------

// Entry point into the tests
int main() {    
    bool success = false;
    
    if (gpI2C != NULL) {        
        success = !Harness::run(specification);
    } else {
        printf ("Unable to instantiate I2C interface.\n");
    }
    
    return success;
}

// End Of File
