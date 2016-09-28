#pragma once

#include <utils/circularbuffer.h>

class PACKER{
public:
	typedef enum{
	} PACKAGE_TYPE;
	virtual uint16_t Pack(PACKAGE_TYPE package_type, uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){return 0;}
	virtual int16_t Unpack(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest, uint16_t sizemax)=0;
	virtual uint16_t GetPreferedPackSize(PACKAGE_TYPE package_type){return 0;}
	template <class T> static void SwapEndian(const T source, T& dest);
};


class PACKER_APCO25: public PACKER{
public:
	typedef enum{
		PACKAGE_TYPE_NONE,
		PACKAGE_TYPE_HEADER,
		PACKAGE_TYPE_LDU1,
		PACKAGE_TYPE_LDU2,
		PACKAGE_TYPE_AUDIO,
		PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE,
		PACKAGE_TYPE_DATA_SERIAL,
		PACKAGE_TYPE_DATA_NET,
		PACKAGE_TYPE_UNKNOWN,
	} PACKAGE_TYPE;

	typedef struct{
		uint8_t 	MI[7];								//Message Indicator
		uint8_t 	MFID; 								//ManufacturerID
		uint8_t 	ALGID;								//Algorithm ID
		uint16_t	KID;								//Key ID
		uint16_t	TGID;								//Talk Group ID
	} APCO25_HEADER_DATA_UNIT ;

	typedef enum{
		APCO25_DUI_HEADER_DATA_UNIT			=0x00,		//Header Data Unit
		APCO25_DUI_TERMINATOR_NO_LC			=0x03,		//Terminator without subsequent Link Control
		APCO25_DUI_LDU1						=0x05,		//Logical Link Data Unit 1
		APCO25_DUI_LDU2						=0x0A,		//Logical Link Data Unit 2
		APCO25_DUI_PDU						=0x0C,		//Packet Data Unit
		APCO25_DUI_TERMINATOR_LC			=0x0F,		//Terminator with subsequent Link Control
	} APCO25_DUI;

	typedef enum{
		APCO25_SAP_UNENCRYPTED_USER_DATA				=0x00,	//Unencrypted User Data
		APCO25_SAP_ENCRYPTED_USER_DATA					=0x01,	//Encrypted User Data
		APCO25_SAP_CIRCUIT_DATA							=0x02,	//Circuit Data
		APCO25_SAP_CIRCUIT_DATA_CONTROL					=0x03,	//Circuit Data Control
		APCO25_SAP_PACKET_DATA							=0x04,	//Packet Data
		APCO25_SAP_ARP									=0x05,	//Address Resolution Protocol (ARP)
		APCO25_SAP_SNDCP								=0x06,	//SNDCP Packet Data Control
		APCO25_SAP_EXTENDED_ADDRESS						=0x1F,	//Extended Address
		APCO25_SAP_REGISTRATION_AUTH					=0x20,	//Registration and Authorization
		APCO25_SAP_CHANNEL_REASSIGNMENT					=0x21,	//Channel Re-assignment
		APCO25_SAP_SYSTEM_CONFIGURATION					=0x22,	//System Configuration
		APCO25_SAP_MR_LOOPBACK							=0x23,	//MR Loopback
		APCO25_SAP_MR_STATISTICS						=0x24,	//MR Statistics
		APCO25_SAP_MR_OUT_OF_SERVICE					=0x25,	//MR Out-of-Service
		APCO25_SAP_MR_PAGING							=0x26,	//MR Paging
		APCO25_SAP_MR_CONFIGURATION						=0x27,	//MR Configuration
		APCO25_SAP_UNENCRYPTED_KEY_MNGT					=0x28,	//Unencrypted Key Management message
		APCO25_SAP_ENCRYPTED_KEY_MNGT					=0x29,	//Encrypted Key Management message
		APCO25_SAP_NON_PROTECTED_TRUNK_CONTROL			=0X3D,	//Non-Protected Trunking Control
		APCO25_SAP_PROTECTED_TRUNK_CONTROL				=0x3F,	//Protected Trunking Control
	} APCO25_SAP;

	typedef union{
		uint64_t _data64;
		uint8_t _data8[8];
		struct{
			uint16_t _paritynid[3];								//Parity Bits
			uint16_t NID;
		};
		struct{ 											//To be accessed as Little Endian format
			uint16_t _parity16[3];							//Parity Bits
			uint16_t DUI:4;									//Data Unit ID
			uint16_t NAC:12;								//Network Access Code
		};

	} APCO25_NETWORK_IDENTIFIER;

	typedef union{
		uint8_t _data8[12];
		struct{
			uint8_t :1;										//Zeroed bit
			uint8_t AN:1;									//A/N (bit 6 of octet 0) = 1 to indicate that confirmation for this packet is desired.
			uint8_t IO:1;									//I/O (bit 5 of octet 0) = 1 for outbound messages, 0 for inbound messages.
			uint8_t FORMAT:5;								//Format (bits 4 – 0 of octet 0) = %10110 to identify this message as a data packet with confirmed delivery.
			uint8_t :2;										//Oned bits
			uint8_t SAP:6;									//SAP Identifier identifies the Service Access Point to which the data is directed.
															//Defined SAP values are in a supplementary document.
			uint8_t MFID;									//Manufacturer's ID identifies the manufacturer for non-standard data functions.
															//Defined values are in a supplementary document.
			uint8_t LID[3];									//Logical Link ID identifies the MR (subscriber unit) which sent the packet for
															//inbound messages, or the MR to which the packet is directed for
															//outbound messages.
			uint8_t BLOCKS:7;								//Blocks to Follow specifies the number of blocks in the packet not including the
															//header block.
			uint8_t FMF:1;									//FMF (bit 7 of octet 6) is the Full Message Flag (FMF). The FMF = 1 on the first
															//try for the complete packet and FMF = 0 on subsequent retries.
															//The value of FMF=1 is used in the receiver to signify that the
															//Blocks to Follow and Pad Octet Count fields indicate the amount
															//of data being transported in the complete packet.
			uint8_t PAD_COUNT:5;							//Pad Octet Count specifies the number of pad octets which have been appended
															//to the data octets to form an integer number of blocks. The actual
															//number of data octets is given by the formula below.
															//Number Data Octets = 16 x Blocks to Follow – 4 – Pad Octet Count
			uint8_t :3;										//Zeroed Bits
			uint8_t FSNF:4;									//FSNF is the Fragment Sequence Number Field. This is used to consecutively
															//number message fragments that together make up a longer data
															//message. The most significant bit shall be asserted for the last
															//fragment in the chain, and shall be cleared otherwise. The three
															//least significant bits correspond to the Fragment Sequence Number
															//(FSN). They shall be set to %000 for the first fragment and shall be
															//incremented for each subsequent fragment. When the number
															//reaches %111 the next increment shall be %001 and not %000. A
															//logical message consisting of a single fragment (or packet) shall
															//have a value of %1000 for the FSNF
			uint8_t NS:3;									//N(S) is the sequence number of the packet. This is used to identify each request
															//packet so that the receiver may correctly order the received
															//message segments and eliminate duplicate copies. It is
															//incremented modulo 8 for each new data packet that is transmitted.
															//The transmitter shall not increment this counter for an automatic
															//retry. The receiver maintains a receiver variable V(R) which is the
															//sequence number of the last valid packet to be received. The
															//receiver accepts packets with N(S) = V(R) or V(R)+1. If N(S)=V(R),
															//then the packet is a duplicate. If N(S) = V(R)+1, then the packet is
															//the next one in the sequence.
			uint8_t SYN:1;									//Syn (bit 7 of octet 8) is a flag used to re-synchronize the physical
															//sublayer sequence numbers. The receiver accepts the N(S) and FSNF
															//fields in this message if the Syn bit is asserted. This bit effectively
															//disables the rejection of duplicate messages when it is asserted. It
															//should only be asserted on specially defined registration messages.
															//On all user data messages, it should be cleared.
			uint8_t DATA_OFFSET:6;							//Data Header Offset is an offset pointer that divides the data portion of the
															//packet into two components, data header and data information. A
															//data header offset value of 3 would specify that the first three octets
															//in the user data comprise the data header and that the actual
															//information begins in the fourth byte. The use of a data header is
															//dependent on the application.
			uint8_t :2;										//Zeroed bits
			uint16_t CRC16;									//Header CRC is the CRC parity check described in Section 6.2 for the header block
		};
	} APCO25_DATA_PACKET_HEADER;

	typedef struct{
		uint8_t data[12];
	} APCO25_UNCONFIRMED_DATA_BLOCK;

	typedef struct{
		uint8_t data[8];
		uint32_t CRC32;
	} APCO25_UNCONFIRMED_DATA_END_BLOCK;

	typedef struct{
		APCO25_NETWORK_IDENTIFIER nid;
		APCO25_DATA_PACKET_HEADER header;
		APCO25_UNCONFIRMED_DATA_BLOCK data[126];
		APCO25_UNCONFIRMED_DATA_END_BLOCK enddata;
	} APCO25_UNCONFIRMED_DATA_PACKET;

	PACKER_APCO25();
	uint16_t Pack(PACKAGE_TYPE package_type, uint8_t *buffersource, uint16_t size, uint8_t *bufferdest);
	void PrintPacket(PACKAGE_TYPE type, uint8_t *buffer);
	int16_t Unpack(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest, uint16_t sizemax);
	PACKAGE_TYPE GetLastUpackedType();
	uint16_t GetPreferedPayloadSize(PACKAGE_TYPE package_type);
	uint16_t GetOverheadSize(PACKAGE_TYPE package_type);
	uint16_t GetPreferedPackageSize(PACKAGE_TYPE package_type);
	uint16_t GetUnpackSize();
protected:
	void NID_BCH_CODE(APCO25_NETWORK_IDENTIFIER &nid);
	uint8_t NID_BCH_DECODE(APCO25_NETWORK_IDENTIFIER &nid);
	uint16_t PackUnconfirmedDataPackage(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest, uint16_t nac=0, uint8_t mfid=0, uint8_t dataoffset=0);
	uint16_t PackRAW(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t nac, uint8_t dui);
	void CheckForType(uint8_t *&buffer, uint16_t &size);
	void ResetUnpack();
	int16_t UnpackRAW(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest, uint16_t sizemax);
	int16_t UnpackUnconfirmedDataPackage(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest, uint16_t sizemax);

	PACKAGE_TYPE _lastunpacktype;
	PACKAGE_TYPE _unpacktype;
	CircularBuffer<uint8_t> _unpackbuffer;
};

class PACKER_BASIC: public PACKER{
public:

	typedef enum{
		PACKAGE_TYPE_NONE,
		PACKAGE_TYPE_UNKNOWN,
		PACKAGE_TYPE_AUDIO,
		PACKAGE_TYPE_DATA_SERIAL,
		PACKAGE_TYPE_DATA_NET,
	} PACKAGE_TYPE;

	PACKER_BASIC():
		_packagenumber(0),
		_unpackbuffer(0,2048),
		_unpacktype(PACKAGE_TYPE_UNKNOWN),
		_lastunpacktype(PACKAGE_TYPE_NONE),
		_dataserialsize(0)
	{
	}

	uint16_t Pack(PACKAGE_TYPE package_type, uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){
		switch(package_type){
		case PACKAGE_TYPE_AUDIO:
			return PackAudio(buffersource, size, bufferdest);
		case PACKAGE_TYPE_DATA_SERIAL:
			return PackDataSerial(buffersource, size, bufferdest);
		case PACKAGE_TYPE_DATA_NET:
			return PackDataNet(buffersource, size, bufferdest);
		case PACKAGE_TYPE_NONE:
		case PACKAGE_TYPE_UNKNOWN:
			break;
		}
		return 0;
	}

	uint16_t GetPreferedPayloadSize(PACKAGE_TYPE package_type){
		switch(package_type){
		case PACKAGE_TYPE_AUDIO:
			return 320;
		case PACKAGE_TYPE_DATA_SERIAL:
			return 160;
		case PACKAGE_TYPE_DATA_NET:
			return 1500;
		case PACKAGE_TYPE_NONE:
		case PACKAGE_TYPE_UNKNOWN:
			break;
		}
		return 0;
	}

	uint16_t GetPreferedPackageSize(PACKAGE_TYPE package_type){
		uint16_t resp=0;
		switch (package_type) {
		case PACKAGE_TYPE_AUDIO:
		case PACKAGE_TYPE_DATA_SERIAL:
		case PACKAGE_TYPE_DATA_NET:
			resp=GetPreferedPayloadSize(package_type);
			break;
		case PACKAGE_TYPE_NONE:
			break;
		case PACKAGE_TYPE_UNKNOWN:
			resp=2046;
			break;
		}
		if(resp) resp+=2;
		return resp;
	}

	uint16_t PackAudio(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){
		// PackType+PackNum Size Data
		uint8_t *sizep=bufferdest;
		PackBase(bufferdest);
		bufferdest++;
		*bufferdest++=size;
		while(size--){
			*bufferdest++=*buffersource++;
		}
		return bufferdest-sizep;
	}

	uint16_t PackDataSerial(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){
		// PackType+PackNum Size Data
		uint8_t *sizep=bufferdest;
		PackBase(bufferdest);
		*bufferdest|=0x30;
		bufferdest++;
		*bufferdest++=size;
		while(size--){
			*bufferdest++=*buffersource++;
		}
		return bufferdest-sizep;
	}

	uint16_t PackDataNet(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){
		// PackType+PackNum Size Data
		uint8_t *sizep=bufferdest;
		PackBase(bufferdest);
		*bufferdest|=0xC0;
		bufferdest++;
		*bufferdest++=size>>8;
		*bufferdest++=size & 0xFF;
		while(size--){
			*bufferdest++=*buffersource++;
		}
		return bufferdest-sizep;
	}

	uint16_t PackBase(uint8_t*& bufferdest){
		uint8_t *sizep=bufferdest;
		_packagenumber++;
		*bufferdest=_packagenumber & 0x0F;
		return bufferdest-sizep;
	}

	void ResetUnpack(){
		_unpacktype=PACKAGE_TYPE_UNKNOWN;
		_unpackbuffer.Clear();
		_dataserialsize=0;
	}

	int16_t Unpack(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest=0, uint16_t sizemax=0){
		if(_unpacktype==PACKAGE_TYPE_UNKNOWN){
			CheckForType(buffersource, size);
		}
		switch (_unpacktype) {
		case PACKAGE_TYPE_AUDIO:
			return UnpackAudio(buffersource, size, bufferdest, sizemax);
		case PACKAGE_TYPE_DATA_SERIAL:
			return UnpackDataSerial(buffersource, size, bufferdest, sizemax);
		case PACKAGE_TYPE_DATA_NET:
			return UnpackDataNet(buffersource, size, bufferdest, sizemax);
		case PACKAGE_TYPE_NONE:
			ResetUnpack();
			return 0;
		case PACKAGE_TYPE_UNKNOWN:
			break;
		}
		return 0;
	}

	void CheckForType(uint8_t *&buffer, uint16_t &size){
		if(_unpackbuffer.Ocupied()<3){
			if(size>=3){
				_unpackbuffer.In(buffer, 3);
				buffer+=3;
				size-=3;
			} else {
				_unpackbuffer.In(buffer, size);
				buffer+=size;
				size=0;
				return;
			}
		}
		uint16_t preferedsize=GetPreferedPayloadSize(PACKAGE_TYPE_AUDIO);

		if((_unpackbuffer[0] & 0xF0)==0 && _unpackbuffer[1]==(preferedsize & 0xff)){
			_unpacktype=PACKAGE_TYPE_AUDIO;
			_unpackbuffer.Out(0,2);
		} else if((_unpackbuffer[0] & 0xF0)==0x30){
			_unpacktype=PACKAGE_TYPE_DATA_SERIAL;
			_unpackbuffer.Out(0,1);
			_dataserialsize=_unpackbuffer.Out();
		} else if((_unpackbuffer[0] & 0xF0)==0xC0){
			_unpacktype=PACKAGE_TYPE_DATA_NET;
			_unpackbuffer.Out(0,1);
			_datanetsize=_unpackbuffer.Out()<<8;
			_datanetsize+=_unpackbuffer.Out();
			if(_datanetsize>1600) _datanetsize=1600;
		} else {
			_unpacktype=PACKAGE_TYPE_NONE;
		}
	}

	int16_t UnpackAudio(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest=0, uint16_t sizemax=0){
		uint16_t preferedsize=GetPreferedPayloadSize(PACKAGE_TYPE_AUDIO);
		_unpackbuffer.In(buffersource, size);
		if(_unpackbuffer.Ocupied()<preferedsize){
			return _unpackbuffer.Ocupied()-preferedsize;
		} else {
			if(sizemax>preferedsize) sizemax=preferedsize;
			preferedsize=_unpackbuffer.Out(bufferdest,sizemax);
			ResetUnpack();
			_lastunpacktype=PACKAGE_TYPE_AUDIO;
			return preferedsize;
		}
	}

	int16_t UnpackDataSerial(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest=0, uint16_t sizemax=0){
		if((_unpackbuffer.Ocupied()+size)>_dataserialsize){
			size=_dataserialsize-_unpackbuffer.Ocupied();
		}
		_unpackbuffer.In(buffersource, size);
		if(_unpackbuffer.Ocupied()<_dataserialsize){
			return _unpackbuffer.Ocupied()-_dataserialsize;
		} else {
			if(sizemax>_dataserialsize) sizemax=_dataserialsize;
			sizemax=_unpackbuffer.Out(bufferdest,sizemax);
			ResetUnpack();
			_lastunpacktype=PACKAGE_TYPE_DATA_SERIAL;
			return sizemax;
		}
	}

	int16_t UnpackDataNet(uint8_t *buffersource, uint16_t size, uint8_t *bufferdest=0, uint16_t sizemax=0){
		if((_unpackbuffer.Ocupied()+size)>_datanetsize){
			size=_datanetsize-_unpackbuffer.Ocupied();
		}
		_unpackbuffer.In(buffersource, size);
		if(_unpackbuffer.Ocupied()<_datanetsize){
			return _unpackbuffer.Ocupied()-_datanetsize;
		} else {
			if(sizemax>_datanetsize) sizemax=_datanetsize;
			sizemax=_unpackbuffer.Out(bufferdest,sizemax);
			ResetUnpack();
			_lastunpacktype=PACKAGE_TYPE_DATA_NET;
			return sizemax;
		}
	}

	PACKAGE_TYPE GetLastUpackedType(){
		PACKAGE_TYPE type=_lastunpacktype;
		_lastunpacktype=PACKAGE_TYPE_NONE;
		return type;
	}

	uint16_t GetUnpackSize(){
		return GetPreferedPackageSize(_unpacktype);
	}

	uint32_t _packagenumber;
	CircularBuffer<uint8_t> _unpackbuffer;
	PACKAGE_TYPE _unpacktype;
	PACKAGE_TYPE _lastunpacktype;
	uint16_t _dataserialsize;
	uint16_t _datanetsize;
};

