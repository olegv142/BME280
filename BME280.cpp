#include "BME280.h"

#include <Wire.h>
#include "fnv.h"

// Internal registers
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

// Initialize interface
void BME280Sensor::begin()
{
	Wire.begin();
}

// Check the sensor is present
bool BME280Sensor::probe()
{
	uint8_t val;
	return read_reg(ID, &val) && val == ID_VAL;
}

// Initialize divice given the sampling period and filtering settings
bool BME280Sensor::init(BME280SamplPeriod_t p, BME280SamplFilter_t f)
{
	return read_calibration() && write_reg(CONFIG, (p << 5) | (f << 2));
}

// Start measuring with given mode and sampling settings
bool BME280Sensor::start(BME280Mode_t m, BME280SamplMode_t t, BME280SamplMode_t p, BME280SamplMode_t h)
{
	return write_reg(CTL_HUM, h) && write_reg(CTL_MEAS, (t << 5) | (p << 2) | m);
}

// Returns true if device is busy measuring
bool BME280Sensor::is_busy()
{
	uint8_t sta;
	return read_reg(STATUS, &sta) && (sta & (MEASURING | UPDATING));
}

// Read BME280 internal register given its address
bool BME280Sensor::read_reg(uint8_t reg, uint8_t* val)
{
	Wire.beginTransmission(m_addr);
	Wire.write(reg);
	if (0 != Wire.endTransmission(false))
		return false;
	if (1 != Wire.requestFrom(m_addr, (uint8_t)1))
		return false;
	*val = Wire.read();
	return true;
}

// Read BME280 internal registers array starting from given address
bool BME280Sensor::read_regs(uint8_t reg, uint8_t* buff, uint8_t len)
{
	Wire.beginTransmission(m_addr);
	Wire.write(reg);
	if (0 != Wire.endTransmission(false))
		return false;
	if (len != Wire.requestFrom(m_addr, len))
		return false;
	for (; len; --len, ++buff) {
		*buff = Wire.read();
	}
	return true;
}

// Write BME280 internal register given its address
bool BME280Sensor::write_reg(uint8_t reg, uint8_t data)
{
	Wire.beginTransmission(m_addr);
	Wire.write(reg);
	Wire.write(data);
	return 0 == Wire.endTransmission();
}

// Read calibration parameters 
bool BME280Sensor::read_calibration()
{
	if (!read_calib_tp() || !read_calib_h())
		return false;

	fnv_init(&m_uid); // Initialize unique id
	fnv_update(&m_uid, (uint8_t const*)&m_calib_tp, sizeof(m_calib_tp));
	fnv_update(&m_uid, (uint8_t const*)&m_calib_h,  sizeof(m_calib_h));
	return true;
}

bool BME280Sensor::read_calib_tp()
{
	return read_regs(CALIB1, (uint8_t*)&m_calib_tp, sizeof(m_calib_tp));
}

bool BME280Sensor::read_calib_h()
{
	if (!read_reg(0xA1, &m_calib_h.H1))
		return false;

	uint8_t buf[7];
	if (!read_regs(CALIB2, buf, sizeof(buf)))
		return false;

	m_calib_h.H2 = buf[0];
	m_calib_h.H2 |= (uint16_t)buf[1] << 8;
	m_calib_h.H3 = buf[2];
	m_calib_h.H4 = (uint16_t)buf[3] << 4;
	m_calib_h.H4 |= buf[4] & 0xf;
	m_calib_h.H5 = buf[4] >> 4;
	m_calib_h.H5 |= (uint16_t)buf[5] << 4;
	m_calib_h.H6 = buf[6];

	return true;
}

// Read 20-bit sample given its base address
bool BME280Sensor::read_20bit(uint8_t reg, uint32_t* val)
{
	uint8_t buf[3];
	if (!read_regs(reg, buf, 3))
		return false;
	*val = ((uint32_t)buf[0] << 12) | ((uint32_t)buf[1] << 4) | (buf[2] >> 4);
	return true;
}

// Read 16-bit sample given its base address
bool BME280Sensor::read_16bit(uint8_t reg, uint16_t* val)
{
	uint8_t buf[2];
	if (!read_regs(reg, buf, 2))
		return false;
	*val = ((uint16_t)buf[0] << 8) | buf[1];
	return true;
}

// Read measuring results as 32 bit integers values
bool BME280Sensor::read32(
		int32_t* T,  // temperature in 0.01 DegC units
		uint32_t* P, // pressure in Pa with Q24.8 format (24 integer bits and 8 fractional bits)
		uint32_t* H  // humidity in %% with Q22.10 format (22 integer and 10 fractional bits)
	)
{
	uint32_t raw_t, raw_p;
	if (!read_20bit(TEMP, &raw_t) || !read_20bit(PRESS, &raw_p))
		return false;

	uint16_t raw_h;
	if (!read_16bit(HUM, &raw_h))
		return false;

	int32_t  t_fine;
	*T = BME280_compensate_T_int32(raw_t, t_fine, m_calib_tp);
	*P = BME280_compensate_P_int64(raw_p, t_fine, m_calib_tp);
	*H = BME280_compensate_H_int32(raw_h, t_fine, m_calib_h);
	return true;
}
