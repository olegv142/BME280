#pragma once

#include <stdint.h>

typedef int32_t  BME280_S32_t;
typedef uint32_t BME280_U32_t;
typedef int64_t  BME280_S64_t;

// Temperature and pressure calibration settings
struct BME280CalibTP {
	uint16_t	T1;
	int16_t		T2;
	int16_t		T3;
	uint16_t	P1;
	int16_t		P2;
	int16_t		P3;
	int16_t		P4;
	int16_t		P5;
	int16_t		P6;
	int16_t		P7;
	int16_t		P8;
	int16_t		P9;
} __attribute__((packed));

// Humidity calibration settings
struct BME280CalibH {
	uint8_t		H1;
	int16_t		H2;
	uint8_t		H3;
	int16_t		H4;
	int16_t		H5;
	int8_t		H6;
} __attribute__((packed));

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, BME280_S32_t& t_fine, struct BME280CalibTP const& calib);

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, BME280_S32_t const& t_fine, struct BME280CalibTP const& calib);

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H, BME280_S32_t const& t_fine, struct BME280CalibH const& calib);
