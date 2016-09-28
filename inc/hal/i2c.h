#pragma once

#include <inttypes.h>

#include <hal/io_pin.h>

class I2C{
public:
	I2C(IO_Pin &sdapin, IO_Pin &sclpin):_sdapin(&sdapin),_sclpin(&sclpin){};
	virtual uint8_t WriteRegBuffer (uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size)=0;
	virtual uint8_t ReadRegBuffer(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size)=0;
	virtual uint8_t WriteReg16Buffer (uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size)=0;
	virtual uint8_t ReadReg16Buffer(uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size)=0;
	virtual uint8_t GetSDAStatus(){ return _sdapin->Read();}
	virtual uint8_t GetSCLStatus(){ return _sclpin->Read();}
	virtual uint8_t SelfTest(){ return !(GetSDAStatus() && GetSCLStatus());}
	virtual uint8_t WriteRegByte (uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t val){
		return WriteRegBuffer(DeviceAddress, RegisterAddress, &val, 1);
	}
	virtual uint8_t ReadRegByte(uint8_t DeviceAddress, uint8_t RegisterAddress){
		uint8_t res=0;
		ReadRegBuffer(DeviceAddress, RegisterAddress, &res, 1);
		return res;
	}
	virtual uint16_t ReadRegHalfWord(uint8_t DeviceAddress, uint8_t RegisterAddress){
		uint16_t res=0;
		ReadRegBuffer(DeviceAddress, RegisterAddress, (uint8_t*)&res, 2);
		res=(res<<8 & 0xFF00) | (res>>8 & 0xFF);
		return res;
	}
protected:
	IO_Pin *_sdapin;
	IO_Pin *_sclpin;
};

