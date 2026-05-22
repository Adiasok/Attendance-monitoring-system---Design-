/******************************************************************************
Based on Sparkfun's BQ27441 library
***************************************/

#include "Arduino.h"
#include <Wire.h>
#include "SparkFunBQ27220.h"
#include "BQ27220_Definitions.h"

/*****************************************************************************
 ************************** Initialization Functions *************************
 *****************************************************************************/
// Initializes class variables
BQ27220::BQ27220() : _deviceAddress(BQ27220_I2C_ADDRESS), _sealFlag(false), _userConfigControl(false)
{
}

// Initializes I2C and verifies communication with the BQ27220.
bool BQ27220::begin(void)
{
	uint16_t deviceID = 0;

	Wire.begin(GPIO_NUM_21, GPIO_NUM_22);

	delay(10);

	deviceID = deviceType(); // Read deviceType from BQ27220

	Serial.print("Device ID: ");
	Serial.println(deviceID, HEX);

	if (deviceID == BQ27220_DEVICE_ID)
	{

		return true; // If device ID is valid, return true
	}

	return true; // Otherwise return false
}

// Configures the design capacity of the connected battery.
bool BQ27220::setCapacity(uint16_t capacity)
{
	// Write to STATE subclass (82) of BQ27220 extended memory.
	// Offset 0x0A (10)
	// Design capacity is a 2-byte piece of data - MSB first
	// Unit: mAh
	uint8_t capMSB = capacity >> 8;
	uint8_t capLSB = capacity & 0x00FF;
	uint8_t capacityData[2] = {capMSB, capLSB};
	// return writeExtendedData(BQ27220_ID_STATE, 10, capacityData, 2);
	// modifyCapacity(0x929D, capacityData);

	modifyCapacity(0x929F, capacityData); // Design Capacity
	modifyCapacity(0x929D, capacityData); // Full Charge Capacity (FCC)

	return true;
}

// Configures the design energy of the connected battery.
bool BQ27220::setDesignEnergy(uint16_t energy)
{
	// Write to STATE subclass (82) of BQ27220 extended memory.
	// Offset 0x0C (12)
	// Design energy is a 2-byte piece of data - MSB first
	// Unit: mWh
	uint8_t enMSB = energy >> 8;
	uint8_t enLSB = energy & 0x00FF;
	uint8_t energyData[2] = {enMSB, enLSB};
	return writeExtendedData(BQ27220_ID_STATE, 12, energyData, 2);
}

// Configures the terminate voltage.
bool BQ27220::setTerminateVoltage(uint16_t voltage)
{
	// Write to STATE subclass (82) of BQ27220 extended memory.
	// Offset 0x0F (16)
	// Termiante voltage is a 2-byte piece of data - MSB first
	// Unit: mV
	// Min 2500, Max 3700
	if (voltage < 2500)
		voltage = 2500;
	if (voltage > 3700)
		voltage = 3700;

	uint8_t tvMSB = voltage >> 8;
	uint8_t tvLSB = voltage & 0x00FF;
	uint8_t tvData[2] = {tvMSB, tvLSB};
	return writeExtendedData(BQ27220_ID_STATE, 16, tvData, 2);
}

// Configures taper rate of connected battery.
bool BQ27220::setTaperRate(uint16_t rate)
{
	// Write to STATE subclass (82) of BQ27220 extended memory.
	// Offset 0x1B (27)
	// Termiante voltage is a 2-byte piece of data - MSB first
	// Unit: 0.1h
	// Max 2000
	if (rate > 2000)
		rate = 2000;
	uint8_t trMSB = rate >> 8;
	uint8_t trLSB = rate & 0x00FF;
	uint8_t trData[2] = {trMSB, trLSB};
	return writeExtendedData(BQ27220_ID_STATE, 27, trData, 2);
}

/*****************************************************************************
 ********************** Battery Characteristics Functions ********************
 *****************************************************************************/

// Reads and returns the battery voltage
uint16_t BQ27220::voltage(void)
{
	return readWord(BQ27220_COMMAND_VOLTAGE);
}

// Reads and returns the specified current measurement
int16_t BQ27220::current(current_measure type)
{
	int16_t current = 0;
	switch (type)
	{
	case AVG:
		current = (int16_t)readWord(BQ27220_COMMAND_AVG_CURRENT);
		break;
	case STBY:
		current = (int16_t)readWord(BQ27220_COMMAND_STDBY_CURRENT);
		break;
	case MAX:
		current = (int16_t)readWord(BQ27220_COMMAND_MAX_CURRENT);
		break;
	}

	return current;
}

// Reads and returns the specified capacity measurement
uint16_t BQ27220::capacity(capacity_measure type)
{
	uint16_t capacity = 0;
	switch (type)
	{
	case REMAIN:
		return readWord(BQ27220_COMMAND_REM_CAPACITY);
		break;
	case FULL:
		return readWord(BQ27220_COMMAND_FULL_CAPACITY);
		break;
	case AVAIL:
		capacity = readWord(BQ27220_COMMAND_NOM_CAPACITY);
		break;
	case AVAIL_FULL:
		capacity = readWord(BQ27220_COMMAND_AVAIL_CAPACITY);
		break;
	case REMAIN_F:
		capacity = readWord(BQ27220_COMMAND_REM_CAP_FIL);
		break;
	case REMAIN_UF:
		capacity = readWord(BQ27220_COMMAND_REM_CAP_UNFL);
		break;
	case FULL_F:
		capacity = readWord(BQ27220_COMMAND_FULL_CAP_FIL);
		break;
	case FULL_UF:
		capacity = readWord(BQ27220_COMMAND_FULL_CAP_UNFL);
		break;
	case DESIGN:
		capacity = readWord(BQ27220_EXTENDED_CAPACITY);
	}

	return capacity;
}

// Reads and returns measured average power
int16_t BQ27220::power(void)
{
	return (int16_t)readWord(BQ27220_COMMAND_AVG_POWER);
}

// Reads and returns specified state of charge measurement
uint16_t BQ27220::soc(soc_measure type)
{
	uint16_t socRet = 0;
	switch (type)
	{
	case FILTERED:
		socRet = readWord(BQ27220_COMMAND_SOC);
		break;
	case UNFILTERED:
		socRet = readWord(BQ27220_COMMAND_SOC_UNFL);
		break;
	}

	return socRet;
}

// Reads and returns specified state of health measurement
uint8_t BQ27220::soh(soh_measure type)
{
	uint16_t sohRaw = readWord(BQ27220_COMMAND_SOH);
	uint8_t sohStatus = sohRaw >> 8;
	uint8_t sohPercent = sohRaw & 0x00FF;

	if (type == PERCENT)
		return sohPercent;
	else
		return sohStatus;
}

// Reads and returns specified temperature measurement
uint16_t BQ27220::temperature(temp_measure type)
{
	uint16_t temp = 0;
	switch (type)
	{
	case BATTERY:
		temp = readWord(BQ27220_COMMAND_TEMP);
		break;
	case INTERNAL_TEMP:
		temp = readWord(BQ27220_COMMAND_INT_TEMP);
		break;
	}
	return temp;
}

/*****************************************************************************
 ************************** GPOUT Control Functions **************************
 *****************************************************************************/
// Get GPOUT polarity setting (active-high or active-low)
bool BQ27220::GPOUTPolarity(void)
{
	uint16_t opConfigRegister = opConfig();

	return (opConfigRegister & BQ27220_OPCONFIG_GPIOPOL);
}

// Set GPOUT polarity to active-high or active-low
bool BQ27220::setGPOUTPolarity(bool activeHigh)
{
	uint16_t oldOpConfig = opConfig();

	// Check to see if we need to update opConfig:
	if ((activeHigh && (oldOpConfig & BQ27220_OPCONFIG_GPIOPOL)) ||
		(!activeHigh && !(oldOpConfig & BQ27220_OPCONFIG_GPIOPOL)))
		return true;

	uint16_t newOpConfig = oldOpConfig;
	if (activeHigh)
		newOpConfig |= BQ27220_OPCONFIG_GPIOPOL;
	else
		newOpConfig &= ~(BQ27220_OPCONFIG_GPIOPOL);

	return writeOpConfig(newOpConfig);
}

// Get GPOUT function (BAT_LOW or SOC_INT)
bool BQ27220::GPOUTFunction(void)
{
	uint16_t opConfigRegister = opConfig();

	return (opConfigRegister & BQ27220_OPCONFIG_BATLOWEN);
}

// Set GPOUT function to BAT_LOW or SOC_INT
bool BQ27220::setGPOUTFunction(gpout_function function)
{
	uint16_t oldOpConfig = opConfig();

	// Check to see if we need to update opConfig:
	if ((function && (oldOpConfig & BQ27220_OPCONFIG_BATLOWEN)) ||
		(!function && !(oldOpConfig & BQ27220_OPCONFIG_BATLOWEN)))
		return true;

	// Modify BATLOWN_EN bit of opConfig:
	uint16_t newOpConfig = oldOpConfig;
	if (function)
		newOpConfig |= BQ27220_OPCONFIG_BATLOWEN;
	else
		newOpConfig &= ~(BQ27220_OPCONFIG_BATLOWEN);

	// Write new opConfig
	return writeOpConfig(newOpConfig);
}

// Get SOC1_Set Threshold - threshold to set the alert flag
uint8_t BQ27220::SOC1SetThreshold(void)
{
	return readExtendedData(BQ27220_ID_DISCHARGE, 0);
}

// Get SOC1_Clear Threshold - threshold to clear the alert flag
uint8_t BQ27220::SOC1ClearThreshold(void)
{
	return readExtendedData(BQ27220_ID_DISCHARGE, 1);
}

// Set the SOC1 set and clear thresholds to a percentage
bool BQ27220::setSOC1Thresholds(uint8_t set, uint8_t clear)
{
	uint8_t thresholds[2];
	thresholds[0] = constrain(set, 0, 100);
	thresholds[1] = constrain(clear, 0, 100);
	return writeExtendedData(BQ27220_ID_DISCHARGE, 0, thresholds, 2);
}

// Get SOCF_Set Threshold - threshold to set the alert flag
uint8_t BQ27220::SOCFSetThreshold(void)
{
	return readExtendedData(BQ27220_ID_DISCHARGE, 2);
}

// Get SOCF_Clear Threshold - threshold to clear the alert flag
uint8_t BQ27220::SOCFClearThreshold(void)
{
	return readExtendedData(BQ27220_ID_DISCHARGE, 3);
}

// Set the SOCF set and clear thresholds to a percentage
bool BQ27220::setSOCFThresholds(uint8_t set, uint8_t clear)
{
	uint8_t thresholds[2];
	thresholds[0] = constrain(set, 0, 100);
	thresholds[1] = constrain(clear, 0, 100);
	return writeExtendedData(BQ27220_ID_DISCHARGE, 2, thresholds, 2);
}

// Check if the SOC1 flag is set
bool BQ27220::socFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_SOC1;
}

// Check if the SOCF flag is set
bool BQ27220::socfFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_SOCF;
}

// Check if the ITPOR flag is set
bool BQ27220::itporFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_ITPOR;
}

// Check if the FC flag is set
bool BQ27220::fcFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_FC;
}

// Check if the CHG flag is set
bool BQ27220::chgFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_CHG;
}

// Check if the DSG flag is set
bool BQ27220::dsgFlag(void)
{
	uint16_t flagState = flags();

	return flagState & BQ27220_FLAG_DSG;
}

// Get the SOC_INT interval delta
uint8_t BQ27220::sociDelta(void)
{
	return readExtendedData(BQ27220_ID_STATE, 26);
}

// Set the SOC_INT interval delta to a value between 1 and 100
bool BQ27220::setSOCIDelta(uint8_t delta)
{
	uint8_t soci = constrain(delta, 0, 100);
	return writeExtendedData(BQ27220_ID_STATE, 26, &soci, 1);
}

// Pulse the GPOUT pin - must be in SOC_INT mode
bool BQ27220::pulseGPOUT(void)
{
	return executeControlWord(BQ27220_CONTROL_PULSE_SOC_INT);
}

/*****************************************************************************
 *************************** Control Sub-Commands ****************************
 *****************************************************************************/

// Read the device type - should be 0x0421
uint16_t BQ27220::deviceType(void)
{
	return readControlWord(BQ27220_CONTROL_DEVICE_TYPE);
}

// Enter configuration mode - set userControl if calling from an Arduino sketch
// and you want control over when to exitConfig
bool BQ27220::enterConfig(bool userControl)
{
	if (userControl)
		_userConfigControl = true;


	if (sealed())
	{
		delayMicroseconds(66);
		_sealFlag = true;
		unseal(); // Must be unsealed before making changes
	}

	Serial.print("Unsealed");
	Serial.println();
	delayMicroseconds(66);

	
	// // The BQ27220 boots up in UNSEAL, but not in FULL ACCESS. Enter
	// // FULL ACCESS to gain access to the Data Memory.
	// uint8_t command[2] = {0xFF, 0xFF};
	// i2cWriteBytes(0x00, command, 2);


	uint8_t _full_access[2] = {0xFF, 0xFF};
	i2cWriteBytes(0x00, &_full_access[0], 1);
	delayMicroseconds(66);
	i2cWriteBytes(0x01, &_full_access[1], 1);
	delayMicroseconds(66);
	i2cWriteBytes(0x00, &_full_access[0], 1);
	delayMicroseconds(66);
	i2cWriteBytes(0x01, &_full_access[1], 1);
	delayMicroseconds(66);


	Serial.print("Full Access");
	Serial.println();

	delayMicroseconds(66);

	if (executeControlWord(BQ27220_CONTROL_SET_CFGUPDATE))
	{
		delay(1100);

		int16_t timeout = BQ72441_I2C_TIMEOUT;

		while ((timeout+=-100) && (!(flags() & BQ27220_FLAG_CFGUPMODE)))
			delay(100);

		if (timeout > 0)
			return true;
	}

	return false;
}

// Exit configuration mode with the option to perform a resimulation
bool BQ27220::exitConfig(bool resim)
{
	// There are two methods for exiting config mode:
	//    1. Execute the EXIT_CFGUPDATE command
	//    2. Execute the SOFT_RESET command
	// EXIT_CFGUPDATE exits config mode _without_ an OCV (open-circuit voltage)
	// measurement, and without resimulating to update unfiltered-SoC and SoC.
	// If a new OCV measurement or resimulation is desired, SOFT_RESET or
	// EXIT_RESIM should be used to exit config mode.
	if (resim)
	{
		flags();
		delayMicroseconds(66);
		
		if (softReset())
		{
			delayMicroseconds(66);

			int16_t timeout = BQ72441_I2C_TIMEOUT;
			while ((timeout--) && ((flags() & BQ27220_FLAG_CFGUPMODE)))
				delay(1);
			if (timeout > 0)
			{
				if (_sealFlag)
					seal(); // Seal back up if we IC was sealed coming in
				return true;
			}
		}
		return false;
	}
	else
	{
		return executeControlWord(BQ27220_CONTROL_EXIT_CFGUPDATE);
	}
}

// Read the flags() command
uint16_t BQ27220::flags(void)
{
	uint16_t _reg_data =  readWord(BQ27220_COMMAND_FLAGS);

	// Serial.print("Flags: ");
	// Serial.println(_reg_data, BIN);

	return _reg_data;
}

// Read the CONTROL_STATUS subcommand of control()
uint16_t BQ27220::status(void)
{
	return readControlWord(BQ27220_CONTROL_STATUS);
}

/***************************** Private Functions *****************************/

// Check if the BQ27220-G1A is sealed or not.
bool BQ27220::sealed(void)
{
	// RSVD RSVD RSVD RSVD RSVD CFGUPDATE RSVD RSVD
	// BTPINT SMTH INITCOMP VDQ EDV2 SEC1 SEC0 CALMD
	// SEC[1:0] = Defines Current Security Access
	// 11 = Sealed Access
	// 10 = Unsealed Access
	// 01 = Full Access
	uint16_t stat = flags() & 0x0006; // 0000 0000 0000 0110

	Serial.print("Sealed Status: ");
	Serial.println(stat, BIN);

	if (stat == 0x06)
		return true;
	else
		return false;
}

// Seal the BQ27220-G1A
bool BQ27220::seal(void)
{
	Serial.println("Sealing");
	return readControlWord(BQ27220_CONTROL_SEALED);
}

// UNseal the BQ27220-G1A
bool BQ27220::unseal(void)
{
	// To unseal the BQ27220, write the key to the control
	// command. Then immediately write the same key to control again.
	Serial.print("Unsealing");
	Serial.println();
	
	uint8_t _data[2] = {0xAA, 0};

	// executeControlWord(0x0414);
	_data[1] = 0x14;
	i2cWriteBytes(0x00, &_data[1], 1);
	delayMicroseconds(66);
	_data[1] = 0x04;
	i2cWriteBytes(0x01, &_data[1], 1);
	delayMicroseconds(66);

	// executeControlWord(0x3672);
	_data[1] = 0x72;
	i2cWriteBytes(0x00, &_data[1], 1);
	delayMicroseconds(66);
	_data[1] = 0x36;
	i2cWriteBytes(0x01, &_data[1], 1);
	delayMicroseconds(66);

	return true;
}

// Read the 16-bit opConfig register from extended data
uint16_t BQ27220::opConfig(void)
{
	return readWord(BQ27220_EXTENDED_OPCONFIG);
}

// Write the 16-bit opConfig register in extended data
bool BQ27220::writeOpConfig(uint16_t value)
{
	uint8_t opConfigMSB = value >> 8;
	uint8_t opConfigLSB = value & 0x00FF;
	uint8_t opConfigData[2] = {opConfigMSB, opConfigLSB};

	// OpConfig register location: BQ27220_ID_REGISTERS id, offset 0
	return writeExtendedData(BQ27220_ID_REGISTERS, 0, opConfigData, 2);
}

// Issue a soft-reset to the BQ27220-G1A
bool BQ27220::softReset(void)
{
	return executeControlWord(BQ27220_CONTROL_EXIT_CFGUPDATE);
}

// Read a 16-bit command word from the BQ27220-G1A
uint16_t BQ27220::readWord(uint16_t subAddress)
{
	uint8_t data[2];
	i2cReadBytes(subAddress, data, 2);
	return ((uint16_t)data[1] << 8) | data[0];
}

// Read a 16-bit subcommand() from the BQ27220-G1A's control()
uint16_t BQ27220::readControlWord(uint16_t function)
{
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	uint8_t data[2] = {0, 0};

	i2cWriteBytes((uint8_t)0, command, 2);

	if (i2cReadBytes((uint8_t)0, data, 2))
	{
		return ((uint16_t)data[1] << 8) | data[0];
	}

	return false;
}

// Execute a subcommand() from the BQ27220-G1A's control()
bool BQ27220::executeControlWord(uint16_t function)
{
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	uint8_t data[2] = {0, 0};

	if (i2cWriteBytes((uint8_t)0, command, 2))
		return true;

	return false;
}

/*****************************************************************************
 ************************** Extended Data Commands ***************************
 *****************************************************************************/

// Issue a BlockDataControl() command to enable BlockData access
bool BQ27220::blockDataControl(void)
{
	uint8_t enableByte = 0x00;
	return i2cWriteBytes(BQ27220_EXTENDED_CONTROL, &enableByte, 1);
}

// Issue a DataClass() command to set the data class to be accessed
bool BQ27220::blockDataClass(uint8_t id)
{
	return i2cWriteBytes(BQ27220_EXTENDED_DATACLASS, &id, 1);
}

// Issue a DataBlock() command to set the data block to be accessed
bool BQ27220::blockDataOffset(uint8_t offset)
{
	return i2cWriteBytes(BQ27220_EXTENDED_DATABLOCK, &offset, 1);
}

// Read the current checksum using BlockDataCheckSum()
uint8_t BQ27220::blockDataChecksum(void)
{
	uint8_t csum[1];
	i2cReadBytes(BQ27220_EXTENDED_CHECKSUM, csum, 1);
	return csum[0];
}

// Use BlockData() to read a byte from the loaded extended data
uint8_t BQ27220::readBlockData(uint8_t offset)
{
	uint8_t ret;
	uint8_t address = offset + BQ27220_EXTENDED_BLOCKDATA;
	i2cReadBytes(address, &ret, 1);
	return ret;
}

// Use BlockData() to write a byte to an offset of the loaded data
bool BQ27220::writeBlockData(uint8_t offset, uint8_t data)
{
	uint8_t address = offset + BQ27220_EXTENDED_BLOCKDATA;
	return i2cWriteBytes(address, &data, 1);
}

// Read all 32 bytes of the loaded extended data and compute a
// checksum based on the values.
uint8_t BQ27220::computeBlockChecksum(void)
{
	uint8_t data[32];
	i2cReadBytes(BQ27220_EXTENDED_BLOCKDATA, data, 32);

	uint8_t csum = 0;
	for (int i = 0; i < 32; i++)
	{
		csum += data[i];
	}
	csum = 255 - csum;

	return csum;
}

// Use the BlockDataCheckSum() command to write a checksum value
bool BQ27220::writeBlockChecksum(uint8_t csum)
{
	return i2cWriteBytes(BQ27220_EXTENDED_CHECKSUM, &csum, 1);
}

// Read a byte from extended data specifying a class ID and position offset
uint8_t BQ27220::readExtendedData(uint8_t classID, uint8_t offset)
{
	uint8_t retData = 0;
	if (!_userConfigControl)
		enterConfig(false);

	if (!blockDataControl())	  // // enable block data memory control
		return false;			  // Return false if enable fails
	if (!blockDataClass(classID)) // Write class ID using DataBlockClass()
		return false;

	blockDataOffset(offset / 32); // Write 32-bit block offset (usually 0)

	computeBlockChecksum(); // Compute checksum going in
	uint8_t oldCsum = blockDataChecksum();
	/*for (int i=0; i<32; i++)
		Serial.print(String(readBlockData(i)) + " ");*/
	retData = readBlockData(offset % 32); // Read from offset (limit to 0-31)

	if (!_userConfigControl)
		exitConfig();

	return retData;
}

// Write a specified number of bytes to extended data specifying a 
// class ID, position offset.
bool BQ27220::writeExtendedData(uint8_t classID, uint8_t offset, uint8_t * data, uint8_t len)
{
	if (len > 32)
		return false;
	
	if (!_userConfigControl) enterConfig(false);
	
	if (!blockDataControl()) // // enable block data memory control
		return false; // Return false if enable fails
	if (!blockDataClass(classID)) // Write class ID using DataBlockClass()
		return false;
	
	blockDataOffset(offset / 32); // Write 32-bit block offset (usually 0)
	computeBlockChecksum(); // Compute checksum going in
	uint8_t oldCsum = blockDataChecksum();

	// Write data bytes:
	for (int i = 0; i < len; i++)
	{
		// Write to offset, mod 32 if offset is greater than 32
		// The blockDataOffset above sets the 32-bit block
		writeBlockData((offset % 32) + i, data[i]);
	}
	
	// Write new checksum using BlockDataChecksum (0x60)
	uint8_t newCsum = computeBlockChecksum(); // Compute the new checksum
	writeBlockChecksum(newCsum);

	if (!_userConfigControl) exitConfig();
	
	return true;
}

// Modify RAM Parameter
bool BQ27220::modifyCapacity(uint16_t MemoryAddress, uint8_t *data)
{
	if (!_userConfigControl)
		enterConfig(false);


	// Split the Memory Location to 2 bytes
	uint8_t _MemAddr[2] = {(uint8_t)(MemoryAddress & 0x00FF), (uint8_t)((MemoryAddress >> 8))};

	// Serial.print("Memory Address: 0x");
	// Serial.print(_MemAddr[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_MemAddr[1], HEX);
	// Serial.println();

	// The address 0x3E is used to make changes to RAM, as these changes cannot be made to a given parameter's direct address.
	i2cWriteBytes(0x3E, _MemAddr, 2);
	delayMicroseconds(66);

	// Serial.print("0x3E: 0x");
	// Serial.print(_MemAddr[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_MemAddr[1], HEX);
	// Serial.println();

	
	// Read the 1-byte checksum using MACDataSum() command (0x60).
	uint8_t oldCsum = blockDataChecksum();
	delayMicroseconds(66);

	// Serial.print("Old Checksum: 0x");
	// Serial.println(oldCsum, HEX);
	
	// Read the 1-byte block length using the MACDataLen() command (0x61)
	uint8_t oldLen[1] = {0};
	i2cReadBytes(0x61, oldLen, 1);
	delayMicroseconds(66);

	// Serial.print("Old Length: 0x");
	// Serial.println(oldLen[0], HEX);

	// Write 0x9F to 0x3E to access the MSB of Design Capacity.
	// i2cWriteBytes(0x3E, _MemAddr, 2);
	// delayMicroseconds(66);

	// Serial.print("WR 0x3E: 0x");
	// Serial.print(_MemAddr[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_MemAddr[1], HEX);
	// Serial.println();
	
	// uint8_t _MemAddrRead[2];
	// i2cReadBytes(0x3E, _MemAddrRead, 2);
	// delayMicroseconds(66);

	// Serial.print("RD 0x3E: 0x");
	// Serial.print(_MemAddrRead[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_MemAddrRead[1], HEX);
	// Serial.println();

	// Read both Design Capacity bytes starting at 0x40.
	// The Default OLD Design Capacity is BB08 is 3000mAh
	// uint8_t _old_design_cap_1[2];
	// i2cReadBytes(0x40, _old_design_cap_1, 2);
	// delayMicroseconds(66);

	// Serial.print("Old Design Capacity: 0x");
	// Serial.print(_old_design_cap_1[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_old_design_cap_1[1], HEX);
	// Serial.println();

	i2cWriteBytes(BQ27220_EXTENDED_BLOCKDATA, data, 2);
	delayMicroseconds(66);

	// Serial.print("WR 0x40: 0x");
	// Serial.print(data[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(data[1], HEX);
	// Serial.println();
	
	uint8_t _new_cs_alt = 0xFF - (0xFF & (_MemAddr[0] + _MemAddr[1] + data[0] + data[1]));
	// 2 Byte Address + 2 Byte Data + 1 Byte Check Sum + 1 Byte Length
	uint8_t _new_cs_and_len[2] = {_new_cs_alt, 0x06};
	i2cWriteBytes(BQ27220_EXTENDED_CHECKSUM, &_new_cs_and_len[0], 1);
	delayMicroseconds(66);

	// Serial.print("New Checksum: 0x");
	// Serial.println(_new_cs_and_len[0], HEX);

	i2cWriteBytes(BQ27220_EXTENDED_CHECKSUM +1, &_new_cs_and_len[1], 1);
	delayMicroseconds(66);

	// Serial.print("New Length: 0x");
	// Serial.println(_new_cs_and_len[1], HEX);

	if (!_userConfigControl)
		exitConfig();

	// Read Design Capacity
	// uint8_t _new_design_cap_1[2];
	// i2cReadBytes(0x3C, _new_design_cap_1, 2);
	// delayMicroseconds(66);

	// Serial.print("New Design Capacity: 0x");
	// Serial.print(_new_design_cap_1[0], HEX);
	// Serial.print(" 0x");
	// Serial.print(_new_design_cap_1[1], HEX);
	// Serial.println();

	return true;
}



/*****************************************************************************
 ************************ I2C Read and Write Routines ************************
 *****************************************************************************/

// Read a specified number of bytes over I2C at a given subAddress
int16_t BQ27220::i2cReadBytes(uint8_t subAddress, uint8_t *dest, uint8_t count)
{
	int16_t timeout = BQ72441_I2C_TIMEOUT;
	Wire.beginTransmission(_deviceAddress);
	Wire.write(subAddress);
	Wire.endTransmission(true);

	Wire.requestFrom(_deviceAddress, count);

	for (int i = 0; i < count; i++)
	{
		dest[i] = Wire.read();
	}

	return timeout;
}

// Write a specified number of bytes over I2C to a given subAddress
uint16_t BQ27220::i2cWriteBytes(uint8_t subAddress, uint8_t *src, uint8_t count)
{
	Wire.beginTransmission(_deviceAddress);
	Wire.write(subAddress);
	for (int i = 0; i < count; i++)
	{
		Wire.write(src[i]);
	}
	Wire.endTransmission(true);

	return true;
}

BQ27220 lipo; // Use lipo.[] to interact with the library in an Arduino sketch