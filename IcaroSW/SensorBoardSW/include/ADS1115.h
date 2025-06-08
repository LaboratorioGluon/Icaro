/* Apache License 2.0  | Author Contact: jdwifwaf@gmail.com */
#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>

/* ESP32 Specifics ------------------------------- */
#define ADS1115_DEBUG_LEVEL ESP_LOG_DEBUG

/* Device Address -------------------------------- */
#define ADS1115_DEF_DEV_ADR 0b1001000 >> 1 // 0x48

/* Address Pointer Register ---------------------- */
#define ADS1115_REG_CONV                  0x00
#define ADS1115_REG_CFG                   0x01
#define ADS1115_REG_LO_THRESH             0x02
#define ADS1115_REG_HI_THRESH             0x03

/* Conversion Register --------------------------- */
#define ADS1115_CONV_RESULT_MASK          0xFFF0
#define ADS1115_CONV_RESULT               0x0000

/* Config Register ------------------------------- */
// Most Significant Byte //
#define ADS1115_CFG_MS_OS_ACTIVE          0x8000  // when reading, 1 is busy, 0 is ready. When writing, starts a covnersion from powerdown state
#define ADS1115_CFG_MS_MUX_OMASK          0x8F00  // mask out

// inline functions pass these defintions, so are shifted instead of an uint16 argument
#define ADS1115_CFG_MS_MUX_DIFF_AIN0_AIN1 0x00    // default
#define ADS1115_CFG_MS_MUX_DIFF_AIN0_AIN3 0x10
#define ADS1115_CFG_MS_MUX_DIFF_AIN1_AIN3 0x20
#define ADS1115_CFG_MS_MUX_DIFF_AIN2_AIN3 0x30
#define ADS1115_CFG_MS_MUX_SNGL_AIN0_GND  0x40
#define ADS1115_CFG_MS_MUX_SNGL_AIN1_GND  0x50
#define ADS1115_CFG_MS_MUX_SNGL_AIN2_GND  0x60
#define ADS1115_CFG_MS_MUX_SNGL_AIN3_GND  0x70

#define ADS1115_CFG_MS_PGA_FSR_6_144V     0x0000   // only expresses full-scale range of ADC scaling. Do not apply more than VDD + 0.3 V to the analog inputs
#define ADS1115_CFG_MS_PGA_FSR_4_096V     0x0200   // only expresses full-scale range of ADC scaling. Do not apply more than VDD + 0.3 V to the analog inputs
#define ADS1115_CFG_MS_PGA_FSR_2_048V     0x0400
#define ADS1115_CFG_MS_PGA_FSR_1_024V     0x0600   // default
#define ADS1115_CFG_MS_PGA_FSR_0_512V     0x0800
#define ADS1115_CFG_MS_PGA_FSR_0_256V     0x0C00
#define ADS1115_CFG_MS_PGA_FSR_0_256V     0x0E00
#define ADS1115_CFG_MS_MODE_CON           0x0000
#define ADS1115_CFG_MS_MODE_SS            0x0100

// Least Significant Byte //
#define ADS1115_CFG_LS_DR_128SPS          0x00
#define ADS1115_CFG_LS_DR_250SPS          0x20
#define ADS1115_CFG_LS_DR_490SPS          0x40
#define ADS1115_CFG_LS_DR_920SPS          0x60
#define ADS1115_CFG_LS_DR_1600SPS         0x80   // default
#define ADS1115_CFG_LS_DR_2400SPS         0xA0
#define ADS1115_CFG_LS_DR_3300SPS         0xC0

#define ADS1115_CFG_LS_COMP_MODE_TRAD     0x00   // default
#define ADS1115_CFG_LS_COMP_MODE_WIN      0x10
#define ADS1115_CFG_LS_COMP_POL_LOW       0x00   // default
#define ADS1115_CFG_LS_COMP_POL_HIGH      0x08
#define ADS1115_CFG_LS_COMP_LAT_NON       0x00   // default
#define ADS1115_CFG_LS_COMP_LAT_LATCH     0x04
#define ADS1115_CFG_LS_COMP_QUE_ONE       0x00   // assert after x conversion..
#define ADS1115_CFG_LS_COMP_QUE_TWO       0x01
#define ADS1115_CFG_LS_COMP_QUE_FOUR      0x02
#define ADS1115_CFG_LS_COMP_QUE_DIS       0x03   // default; disable comparator and set ALERT/RDY pin to high-impedance

/* Lo_tresh / Hi_thresh Register ---------------- */
#define ADS1115_THRESH_VALUE_MASK         0xFFF0

#define BYTES_INT(A,B) (((A << 8) & 0xFF00) | B)

#define ADS_RW_BUFF_SIZE 2

typedef struct {
   uint16_t reg_cfg;
   uint8_t rw_buff[ADS_RW_BUFF_SIZE];
   uint8_t dev_addr;
} ads1115_t;

esp_err_t ADS1115_initiate(ads1115_t *cfg);
esp_err_t ADS1115_set_config(ads1115_t *cfg);

int16_t ADS1115_get_conversion();
bool ADS1115_get_conversion_state();

esp_err_t ADS1115_request_by_definition(uint8_t def);
esp_err_t ADS1115_set_thresh_by_definition(uint8_t thresh, uint16_t val);

static inline esp_err_t ADS1115_set_lo_thresh(uint16_t value) { return ADS1115_set_thresh_by_definition(ADS1115_REG_LO_THRESH, value); };
static inline esp_err_t ADS1115_set_hi_thresh(uint16_t value) { return ADS1115_set_thresh_by_definition(ADS1115_REG_HI_THRESH, value); };

static inline esp_err_t ADS1115_request_single_ended_AIN0() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_SNGL_AIN0_GND); };
static inline esp_err_t ADS1115_request_single_ended_AIN1() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_SNGL_AIN1_GND); };
static inline esp_err_t ADS1115_request_single_ended_AIN2() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_SNGL_AIN2_GND); };
static inline esp_err_t ADS1115_request_single_ended_AIN3() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_SNGL_AIN3_GND); };

static inline esp_err_t ADS1115_request_diff_AIN0_AIN1() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_DIFF_AIN0_AIN1); };
static inline esp_err_t ADS1115_request_diff_AIN0_AIN3() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_DIFF_AIN0_AIN3); };
static inline esp_err_t ADS1115_request_diff_AIN1_AIN3() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_DIFF_AIN1_AIN3); };
static inline esp_err_t ADS1115_request_diff_AIN2_AIN3() { return ADS1115_request_by_definition(ADS1115_CFG_MS_MUX_DIFF_AIN2_AIN3); };

#endif // ADS1115_H
