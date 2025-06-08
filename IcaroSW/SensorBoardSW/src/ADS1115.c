/* Apache License 2.0  | Author Contact: jdwifwaf@gmail.com */
#include <stdio.h>
#include <driver/i2c_master.h>
#include <esp_log.h>
#include <string.h>

#include "system.h"
#include "ADS1115.h"

static const char *ADS_TAG = "ADS1115";
static ads1115_t *ads_cfg;

static esp_err_t ADS1115_read_to_rwbuff(uint8_t reg_adr);          // Move these to the header file if you need additional r/w capabilities
static esp_err_t ADS1115_write_reg(uint16_t val, uint16_t reg);    // Move these to the header file if you need additional r/w capabilities
static esp_err_t i2c_handle_write(uint8_t dev_adr, uint8_t w_adr, uint8_t w_len, uint8_t *buff) ;
static esp_err_t i2c_handle_read(uint8_t dev_adr, uint8_t r_adr, uint8_t r_len, uint8_t *buff);

//inline implementations
extern inline esp_err_t ADS1115_request_single_ended_AIN0();
extern inline esp_err_t ADS1115_request_single_ended_AIN1();
extern inline esp_err_t ADS1115_request_single_ended_AIN2();
extern inline esp_err_t ADS1115_request_single_ended_AIN3();

extern inline esp_err_t ADS1115_request_diff_AIN0_AIN1();
extern inline esp_err_t ADS1115_request_diff_AIN0_AIN3();
extern inline esp_err_t ADS1115_request_diff_AIN1_AIN3();
extern inline esp_err_t ADS1115_request_diff_AIN2_AIN3();

static inline esp_err_t ADS1115_set_lo_thresh(uint16_t value);
static inline esp_err_t ADS1115_set_hi_thresh(uint16_t value);

esp_err_t ADS1115_initiate(ads1115_t *cfg)
{
    esp_log_level_set(ADS_TAG, ADS1115_DEBUG_LEVEL);

    return ADS1115_set_config(cfg);
}

esp_err_t ADS1115_set_config(ads1115_t *cfg)
{
    if(!cfg)
        return ESP_ERR_INVALID_ARG;

    ads_cfg = cfg;
    return ESP_OK;
}

bool ADS1115_get_conversion_state()
{
    ADS1115_read_to_rwbuff(ADS1115_REG_CFG);
    return (ads_cfg->rw_buff[0] & 0x80) ? true : false;
}

esp_err_t ADS1115_request_by_definition(uint8_t def)
{
    ads_cfg->reg_cfg &= ADS1115_CFG_MS_MUX_OMASK;
    ads_cfg->reg_cfg |= (def << 8) & 0xFF00;
    ads_cfg->reg_cfg |= ADS1115_CFG_MS_OS_ACTIVE & 0xFF00;
    return ADS1115_write_reg(ads_cfg->reg_cfg, ADS1115_REG_CFG);
}

//#include <esp_log.h>
int16_t ADS1115_get_conversion()
{
    ESP_LOGE("LIB","%d", ADS1115_read_to_rwbuff(ADS1115_REG_CONV));
    return (int16_t)BYTES_INT(ads_cfg->rw_buff[0], ads_cfg->rw_buff[1]);

}

esp_err_t ADS1115_set_thresh_by_definition(uint8_t thresh, uint16_t val)
{
    if(thresh != ADS1115_REG_LO_THRESH || thresh != ADS1115_REG_HI_THRESH)
        return ESP_ERR_INVALID_ARG;

    return ADS1115_write_reg(val, thresh);
}

static esp_err_t ADS1115_write_reg(uint16_t val, uint16_t reg)
{
    ads_cfg->rw_buff[0] = (uint8_t)(val >> 8) & 0xFF;
    ads_cfg->rw_buff[1] = (uint8_t)val & 0xFF;

    return i2c_handle_write(ads_cfg->dev_addr, reg, 2, ads_cfg->rw_buff);
}

static esp_err_t ADS1115_read_to_rwbuff(uint8_t reg_adr)
{
    return i2c_handle_read(ads_cfg->dev_addr, reg_adr, 2, ads_cfg->rw_buff);
}

static esp_err_t i2c_handle_write(uint8_t dev_adr, uint8_t w_adr, uint8_t w_len, uint8_t *buff)
{
  esp_err_t ret_err = ESP_OK;

  uint8_t buf[50];
  buf[0] = w_adr;
  memcpy(&buf[1], buff, w_len);

  return  i2c_master_transmit(i2cDeviceAds1115,
    buf,
    w_len+1,
    1000);

}

static esp_err_t i2c_handle_read(uint8_t dev_adr, uint8_t r_adr, uint8_t r_len, uint8_t *buff)
{
  memset(buff, 0, ADS_RW_BUFF_SIZE);

  i2c_master_transmit(i2cDeviceAds1115, &r_adr, 1, -1);
  return i2c_master_receive(i2cDeviceAds1115, buff, r_len, -1);

}

