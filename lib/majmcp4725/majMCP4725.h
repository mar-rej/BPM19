/*
    rej version 0.0.1 -> creation
*/

#ifndef MAJMCP4725_H
#define MAJMCP4725_H

#include "Arduino.h"
#include <Wire.h>

// Définition des commandes et registres
// Commandes
#define FAST_MODE		0x00
#define DAC_MODE		0x40
#define EEPROM_DAC_MODE	0x60

// Power Mode
#define NORM_PWR 	0x00
#define ONEK_PWR	0x01
#define HUNK_PWR	0x02
#define FHUK_PWR	0x03

class majMCP4725{
  public:
	majMCP4725();
	void begin(uint8_t a);
	void setVoltage(uint16_t output, uint8_t *ret);
	void setMode(uint8_t mode);
	void setPower(uint8_t power);
	uint16_t getVoltage(void);
	uint8_t getMode(void);
	uint8_t getPower(void);

  private:
	uint8_t _i2caddr;
	uint8_t _mode;
	uint8_t	_power;
	uint16_t _output;
};
#endif
