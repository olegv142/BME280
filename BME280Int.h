#pragma once

//
// BME280 internal registers
//

#define ID        0xd0
#define ID_VAL    0x60
#define RESET     0xe0
#define RESET_CMD 0xb6
#define CTL_HUM   0xf2
#define STATUS    0xf3
#define CTL_MEAS  0xf4
#define CONFIG    0xf5
#define PRESS     0xf7
#define TEMP      0xfa
#define HUM       0xfd
#define CALIB1    0x88
#define CALIB2    0xe1

// Status bits
#define MEASURING 8
#define UPDATING  1
