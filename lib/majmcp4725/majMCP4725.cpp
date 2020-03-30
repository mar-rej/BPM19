/*****************************************************************************/
/*
    @file     majMCP4725.cpp
    @author   marrej
    @brief      Driver for microchip MCP4725 I2C DAC
    @version  0.0.1
    @history
        23/12/2018  0.0.1   marrej  First try
*/
/*****************************************************************************/
#include "Arduino.h"
#include <Wire.h>
#include "majMCP4725.h"

/*****************************************************************************/
/*
    @brief  Instantiates a new MCP4725 class
*/
/******************************************************************************/
majMCP4725::majMCP4725() {

}

/******************************************************************************/
/*
    @brief  Hardware Configuration
    @param[in]  uint8_t     addr
        Address for the MCP4725
    @param[out] void
*/
/******************************************************************************/
void majMCP4725::begin(uint8_t addr) {
    _i2caddr = addr;
    _mode = FAST_MODE;
    _power = NORM_PWR;
    _output = 0;
}

/******************************************************************************/
/*
    @brief  Set the mode
    @param[in]  uint8_t     mode
        set the mode for MCP4725
    @param[out] void
*/
/******************************************************************************/
void majMCP4725::setMode(uint8_t mode) {
    if ((mode == FAST_MODE) || (mode == DAC_MODE) || (mode == EEPROM_DAC_MODE)){
        _mode = mode;
    } else {
        _mode = FAST_MODE;
    }
}

/******************************************************************************/
/*
    @brief  Get the mode
    @param[in]  void
    @param[out] uint8_t     mode
        set the mode for MCP4725
*/
/******************************************************************************/
uint8_t majMCP4725::getMode(void) {
    return(_mode);
}

/******************************************************************************/
/*
    @brief  Set the power
    @param[in]  uint8_t     power
        set the powet mode for MCP4725
    @param[out] void
*/
/******************************************************************************/
void majMCP4725::setPower(uint8_t power) {
    if (power < 0x04){
        _power = power;
    } else {
        _power = NORM_PWR;
    }
}

/******************************************************************************/
/*
    @brief  Get the power mode
    @param[in]  void
    @param[out] uint8_t     power
        set the mode for MCP4725
*/
/******************************************************************************/
uint8_t majMCP4725::getPower(void) {
    return(_power);
}

/******************************************************************************/
/*
    @brief  Sets the output voltage to a fraction of source vref.
            (Value can be 0..4095)

    @param[in]  output
                The 12-bit value representing the relationship between
                the DAC's input voltage and its output voltage.
    @param[out] void
*/
/*****************************************************************************/
void majMCP4725::setVoltage(uint16_t output, uint8_t *ret){

    uint8_t mcp4725_SV_buff, error;

    *ret = 0;

    // send the MCP4725 address and write command
    Wire.beginTransmission(_i2caddr);
    error = Wire.endTransmission();

    // Le composant à répondu
    if (error != 0){
      return;
    }

    Wire.beginTransmission(_i2caddr);

    // switch the diferents modes
    if (_mode == FAST_MODE){
        // write the mode, the power and the first bits
        mcp4725_SV_buff = _mode | (_power << 3);
        mcp4725_SV_buff = mcp4725_SV_buff + (output / 256);
        *ret = mcp4725_SV_buff;
        Wire.write(mcp4725_SV_buff);
        Wire.write(output % 256);
        Wire.endTransmission();
    } else {
        // write the mode and the power
        mcp4725_SV_buff = _mode | _power;
        *ret = mcp4725_SV_buff;
        Wire.write(mcp4725_SV_buff);

        // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
        Wire.write(output / 16);
        // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.write((output % 16) << 4);

    }
    Wire.endTransmission();
    _output = output;
}

/******************************************************************************/
/*
    @brief  Get the power mode
    @param[in]  void
    @param[out] uint8_t     power
        set the mode for MCP4725
*/
/******************************************************************************/
uint16_t majMCP4725::getVoltage(void) {
    return(_output);
}
