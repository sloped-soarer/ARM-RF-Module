#ifndef _DEVO8_TARGET_H_
#define _DEVO8_TARGET_H_

#include "../common/devo/common_devo.h"

#define TXID 0x08
#define VECTOR_TABLE_LOCATION 0x4000
#define SPIFLASH_SECTOR_OFFSET 54
#define SPIFLASH_SECTORS 1024

// Edited by UNIMOD
#define HAS_STANDARD_GUI    0
#define HAS_ADVANCED_GUI    0
#define HAS_PERMANENT_TIMER 0
#define HAS_TELEMETRY       1
#define HAS_TOUCH           0
#define HAS_RTC             0
#define HAS_VIBRATINGMOTOR  0
#define HAS_DATALOG         0
#define HAS_SCANNER         0
#define HAS_EXTRA_SWITCHES  0
#define HAS_MULTIMOD_SUPPORT 0

#define MIN_BRIGHTNESS 1 
#define DEFAULT_BATTERY_ALARM 4000
#define DEFAULT_BATTERY_CRITICAL 3800
#define MAX_BATTERY_ALARM 12000
#define MIN_BATTERY_ALARM 3300
#define MAX_POWER_ALARM 60

#define NUM_OUT_CHANNELS 16 // unimod rick M-Link has 16 channels max.
#define NUM_VIRT_CHANNELS 10

#define NUM_TRIMS 10
#define NUM_TOGGLES 4
#define MAX_POINTS 13
#define NUM_MIXERS ((NUM_OUT_CHANNELS + NUM_VIRT_CHANNELS) * 4)

#define INP_HAS_CALIBRATION 4

/* Compute voltage from y = 0.00209x + 0.3026 */
#define VOLTAGE_NUMERATOR 209
#define VOLTAGE_OFFSET    303

/* Define button placement for chantest page*/
/* these are supposed to more closely match real Tx button layout */
/* negative value: box at the right side of the label */
#define CHANTEST_BUTTON_PLACEMENT { \
    {51, 120}, {51, 105}, {-229, 120}, {-229, 105}, \
    {-106, 135}, {21, 135}, {-259, 135}, {174, 135}, \
    {30, 71}, {30, 56}, {-250, 71}, {-250, 56}, \
    {185, 220}, {185, 200}, {-95, 220}, {-95, 200}, {200, 180}, {-80, 180}, \
    }
#endif //_DEVO8_TARGET_H_
