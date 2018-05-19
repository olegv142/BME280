
#include "BME280.h"

BME280Sensor g_bme;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  g_bme.begin();

  Serial.println(g_bme.init(sp_62_5ms, sf_16));
  Serial.println(g_bme.uid());
  Serial.println(g_bme.start(md_Normal, sx_1x, sx_16x, sx_1x));
}

void format_T(int32_t T, char* buf, int buf_sz)
{
    int t = T / 100, d = T - (int32_t)t * 100;
    snprintf(buf, buf_sz, "%d.%02d", t, d >= 0 ? d : -d);
}

void format_H(uint32_t H, char* buf, int buf_sz)
{
    int h = H >> 10, d = 10 * (H - ((uint32_t)h << 10)) / (1 << 10);
    snprintf(buf, buf_sz, "%d.%01d", h, d);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  int32_t  T; // temperature in 0.01 DegC units
  uint32_t P; // pressure in Pa with Q24.8 format (24 integer bits and 8 fractional bits)
  uint32_t H; // humidity in %% with Q22.10 format (22 integer and 10 fractional bits)
  if (g_bme.read32(&T, &P, &H))
  {
    char str[16] = {0};
    format_T(T, str, 15);
    Serial.print("T=");
    Serial.println(str);
    Serial.print("P=");
    Serial.println(P >> 8);
    format_H(H, str, 15);
    Serial.print("H=");
    Serial.println(str);
    Serial.println();
  } else {
    Serial.println("readout failed");
  }
}

