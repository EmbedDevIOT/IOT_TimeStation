/*
* File:   AT24Cxx.cpp
* Author: Manjunath CV
*
* Created on February 16, 2016, 12:19 AM
*/

#include <Wire.h>
#include "AT24Cxx.h"

AT24Cxx::AT24Cxx(uint8_t i2c_address)
{
	Wire.begin();
	this->i2c_address = i2c_address;
}

AT24Cxx::AT24Cxx(uint8_t i2c_address, uint32_t eeprom_size)
{
	Wire.begin();
	this->i2c_address = i2c_address;
	this->eeprom_size = eeprom_size;
}

uint8_t AT24Cxx::read(uint16_t address)
{
	uint8_t first,second,data;
	Wire.beginTransmission(i2c_address);
	
	first = highByte(address);
	second = lowByte(address);
	
	Wire.write(first);      //First Word Address
	Wire.write(second);      //Second Word Address
	
	Wire.endTransmission();
	delay(10);
	// fix Copilot
	Wire.requestFrom(i2c_address, (uint8_t)1);
	delay(10);
	
	data = Wire.read();
	delay(10);
	
	return data;
}

void AT24Cxx::write(uint16_t address, uint8_t value)
{
	uint8_t first,second;
	Wire.beginTransmission(i2c_address);

	first = highByte(address);
	second = lowByte(address);

	Wire.write(first);      //First Word Address
	Wire.write(second);      //Second Word Address

	Wire.write(value);     

	delay(10);

	Wire.endTransmission();
	delay(10);
}
/*

Not yet Ready!



void AT24Cxx::write(uint16_t address, uint8_t ptr, uint8_t size)
{
	uint8_t first,second,value;
	Wire.beginTransmission(i2c_address);
	
	for(int i = 0; i < size ; i++)
	{
		first = highByte(address);
		second = lowByte(address);

		Wire.write(first);      //First Word Address
		Wire.write(second);      //Second Word Address
		
		value = *ptr++;
		Wire.write(value);     

		delay(10);
		
		address++;

	}
	Wire.endTransmission();
	delay(10);
}
*/


void AT24Cxx::update(uint16_t address, uint8_t value)
{
	uint8_t first,second,data;
	Wire.beginTransmission(i2c_address);

	first = highByte(address);
	second = lowByte(address);

	Wire.write(first);      //First Word Address
	Wire.write(second);      //Second Word Address

	Wire.endTransmission();
	delay(10);
	// fix Copilot
	Wire.requestFrom(i2c_address, (uint8_t)1);
	delay(10);

	data = Wire.read();	
	delay(10);
	
	/* Checking Value */
	if (data != value)
	{
		/* Writing Value */
		Wire.beginTransmission(i2c_address);

		first = highByte(address);
		second = lowByte(address);

		Wire.write(first);      //First Word Address
		Wire.write(second);      //Second Word Address

		Wire.write(value);

		delay(10);

		Wire.endTransmission();
		delay(10);	
	}
}


uint32_t AT24Cxx::length()
{
	return this->eeprom_size * 1024;
}

// Used to access
int AT24Cxx::operator[](uint16_t address)
{
	uint8_t first,second,data;
	Wire.beginTransmission(i2c_address);
	
	first = highByte(address);
	second = lowByte(address);
	
	Wire.write(first);      //First Word Address
	Wire.write(second);      //Second Word Address
	
	Wire.endTransmission();
	delay(10);
	// fix Copilot
	Wire.requestFrom(i2c_address, (uint8_t)1);
	delay(10);
	
	data = Wire.read();
	delay(10);
	
	return data;
}


/*
AT24Cxx::AT24Cxx(const AT24Cxx& orig) {
} 

AT24Cxx::~AT24Cxx() {
}
*/
