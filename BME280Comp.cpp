#include "BME280Comp.h"

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, BME280_S32_t& t_fine, struct BME280CalibTP const& calib)
{
	BME280_S32_t var1, var2, T;

	var1 = ((((adc_T>>3) -((BME280_S32_t)calib.T1<<1))) * ((BME280_S32_t)calib.T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BME280_S32_t)calib.T1)) * ((adc_T>>4) - ((BME280_S32_t)calib.T1))) >> 12) * ((BME280_S32_t)calib.T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, BME280_S32_t const& t_fine, struct BME280CalibTP const& calib)
{
	BME280_S64_t var1, var2, p;

	var1 = ((BME280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)calib.P6;
	var2 = var2 + ((var1*(BME280_S64_t)calib.P5)<<17);
	var2 = var2 + (((BME280_S64_t)calib.P4)<<35);
	var1 = ((var1 * var1 * (BME280_S64_t)calib.P3)>>8) + ((var1 * (BME280_S64_t)calib.P2)<<12);
	var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)calib.P1)>>33;
	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BME280_S64_t)calib.P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BME280_S64_t)calib.P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)calib.P7)<<4);
	return (BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H, BME280_S32_t const& t_fine, struct BME280CalibH const& calib)
{
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)calib.H4) << 20) - (((BME280_S32_t)calib.H5) * v_x1_u32r)) +
	((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)calib.H6)) >> 10) * (((v_x1_u32r *
	((BME280_S32_t)calib.H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
	((BME280_S32_t)calib.H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)calib.H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r>>12);
}

