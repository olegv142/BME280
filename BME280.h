#pragma once

#include <stdint.h>
#include "BME280Comp.h"

// Operation mode
typedef enum {
	md_Sleep,
	md_Forced,
	md_Normal = 3,
} BME280Mode_t;

// Normal mode sampling period
typedef enum {
	sp_0_5ms,
	sp_62_5ms,
	sp_125ms,
	sp_250ms,
	sp_500ms,
	sp_1000ms,
	sp_10ms,
	sp_20ms
} BME280SamplPeriod_t;

// Sampling filter
typedef enum {
	sf_1,
	sf_2,
	sf_4,
	sf_8,
	sf_16
} BME280SamplFilter_t;

// Oversampling mode
typedef enum {
	sx_off, // No sampling at all
	sx_1x,
	sx_2x,
	sx_4x,
	sx_8x,
	sx_16x,
} BME280SamplMode_t;

#define BME280_DEF_I2C_ADDR 0x76

class BME280Sensor {
public:
	BME280Sensor(uint8_t addr = BME280_DEF_I2C_ADDR)
		: m_addr(addr), m_uid(0) {}

	// Initialize interface
	void begin();

	// Check the sensor is present
	bool probe();

	// Initialize divice given the sampling period and filtering settings
	bool init(BME280SamplPeriod_t p, BME280SamplFilter_t f);

	// Start measuring with given mode and sampling settings
	bool start(BME280Mode_t m, BME280SamplMode_t t, BME280SamplMode_t p, BME280SamplMode_t h);
	
	// Read measuring results as 32 bit integers values
	bool read32(
		int32_t* T,  // temperature in 0.01 DegC units
		uint32_t* P, // pressure in Pa with Q24.8 format (24 integer bits and 8 fractional bits)
		uint32_t* H  // humidity in %% with Q22.10 format (22 integer and 10 fractional bits)
	);

	// Returns true if device is busy measuring
	bool is_busy();

	// Returns unique id of the sensor based on its calibration data
	uint32_t uid() const { return m_uid; }

private:
	// Read BME280 internal register given its address
	bool read_reg(uint8_t reg, uint8_t* val);

	// Read BME280 internal registers array starting from given address
	bool read_regs(uint8_t reg, uint8_t* buff, uint8_t len);

	// Write BME280 internal register given its address
	bool write_reg(uint8_t reg, uint8_t data);

	// Read 20-bit sample given its base address
	bool read_20bit(uint8_t reg, uint32_t* val);

	// Read 16-bit sample given its base address
	bool read_16bit(uint8_t reg, uint16_t* val);

	// Read calibration parameters 
	bool read_calibration();
	bool read_calib_tp();
	bool read_calib_h();

	uint8_t  m_addr;
	uint32_t m_uid;

	struct BME280CalibTP m_calib_tp;
	struct BME280CalibH  m_calib_h;
};
