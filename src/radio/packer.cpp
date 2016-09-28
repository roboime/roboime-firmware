#include <radio/packer.h>
#include <utils/crc.h>
#include <utils/fec.h>
#include <utils/tpp_debug.h>
#include <stdio.h>

PACKER_APCO25::PACKER_APCO25():
	_lastunpacktype(PACKAGE_TYPE_NONE),
	_unpacktype(PACKAGE_TYPE_UNKNOWN),
	_unpackbuffer(0,1600)
{
}


uint16_t PACKER_APCO25::Pack(PACKAGE_TYPE package_type, uint8_t *buffersource, uint16_t size, uint8_t *bufferdest){
	switch(package_type){
	case PACKAGE_TYPE_AUDIO:
		return PackRAW(buffersource, size, bufferdest, 0, APCO25_DUI_LDU1);
	case PACKAGE_TYPE_DATA_NET:
		return PackUnconfirmedDataPackage(buffersource, size, bufferdest, 0, 0xff, 0);
	case PACKAGE_TYPE_DATA_SERIAL:
		return PackUnconfirmedDataPackage(buffersource, size, bufferdest, 0, 0xfe, 0);
	case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		return PackUnconfirmedDataPackage(buffersource, size, bufferdest, 0, 0x00, 0);
	case PACKAGE_TYPE_UNKNOWN:
	case PACKAGE_TYPE_HEADER:
	case PACKAGE_TYPE_LDU1:
	case PACKAGE_TYPE_LDU2:
	case PACKAGE_TYPE_NONE:
		return 0;
	}
	return 0;
}


void PACKER_APCO25::CheckForType(uint8_t *&buffer, uint16_t &size){
	if(_unpackbuffer.Ocupied()<8){
		uint16_t sizetemp=8-_unpackbuffer.Ocupied();
		if(size>=sizetemp){
			_unpackbuffer.In(buffer, sizetemp);
			buffer+=sizetemp;
			size-=sizetemp;
		} else {
			_unpackbuffer.In(buffer, size);
			buffer+=size;
			size=0;
			return;
		}
	}

	APCO25_NETWORK_IDENTIFIER nid;
	nid._data64=*(uint64_t*)_unpackbuffer.GetRearrangedBuffer();
	uint8_t nerrs=0;
	nerrs=NID_BCH_DECODE(nid);

	if(nerrs!=0xFF && nid.DUI==APCO25_DUI_LDU1){
		_unpacktype=PACKAGE_TYPE_AUDIO;
		_unpackbuffer.Out(0,8);
		TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("PACKAGE_TYPE_AUDIO\tNAC: %04X DUI: %02X NID ERRORS: %3d\n", nid.NAC, nid.DUI, nerrs));
	} else if(nerrs!=0xFF && nid.DUI==APCO25_DUI_PDU){
		_unpackbuffer.In(buffer, size);
		size=0;
		if(_unpackbuffer.Ocupied()>20){
			APCO25_UNCONFIRMED_DATA_PACKET &packet=*(APCO25_UNCONFIRMED_DATA_PACKET*)_unpackbuffer.GetRearrangedBuffer();
			if(packet.header.MFID==0xFF){
				_unpacktype=PACKAGE_TYPE_DATA_NET;
				TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("PACKAGE_TYPE_DATA_NET\tNAC: %04X DUI: %02X NID ERRORS: %3d\n", nid.NAC, nid.DUI, nerrs));
			} else if(packet.header.MFID==0xFE){
				_unpacktype=PACKAGE_TYPE_DATA_SERIAL;
				TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("PACKAGE_TYPE_DATA_SERIAL\tNAC: %04X DUI: %02X NID ERRORS: %3d\n", nid.NAC, nid.DUI, nerrs));
			}
			_unpackbuffer.Out(0,8);
		}
	} else {
		_unpacktype=PACKAGE_TYPE_NONE;
		TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("PACKAGE_TYPE_NONE\tNAC: %04x DUI: %02x NID ERRORS: %3d\n", nid.NAC, nid.DUI, nerrs));
	}
}

int16_t PACKER_APCO25::Unpack(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t sizemax) {
	if(_unpacktype==PACKAGE_TYPE_UNKNOWN){
		CheckForType(buffersource, size);
	}
	switch (_unpacktype) {
	case PACKAGE_TYPE_LDU1:
	case PACKAGE_TYPE_LDU2:
	case PACKAGE_TYPE_AUDIO:
		return UnpackRAW(buffersource, size, bufferdest, sizemax);
		break;
	case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		return UnpackRAW(buffersource, size, bufferdest, sizemax);
		break;
	case PACKAGE_TYPE_DATA_SERIAL:
		return UnpackUnconfirmedDataPackage(buffersource, size, bufferdest, sizemax);
		break;
	case PACKAGE_TYPE_DATA_NET:
		return UnpackUnconfirmedDataPackage(buffersource, size, bufferdest, sizemax);
		break;
	case PACKAGE_TYPE_HEADER:
	case PACKAGE_TYPE_NONE:
		ResetUnpack();
		return 0;
	case PACKAGE_TYPE_UNKNOWN:
		break;
	}
	return 0;

}

void PACKER_APCO25::NID_BCH_CODE(APCO25_NETWORK_IDENTIFIER &nid){
	SwapEndian(BCH::bch_63_16_encode(nid.NID), nid._data64);
}

uint8_t PACKER_APCO25::NID_BCH_DECODE(APCO25_NETWORK_IDENTIFIER &nid){
	uint8_t nerrs=0;
	uint64_t nid64;
	SwapEndian(nid._data64, nid64);
	BCH::bch_63_16_decode(nid64, &nerrs);
	nid._data64=nid64;
	return nerrs;
}

void PACKER_APCO25::PrintPacket(PACKAGE_TYPE type, uint8_t* buffer) {
	APCO25_UNCONFIRMED_DATA_PACKET &packet=*(APCO25_UNCONFIRMED_DATA_PACKET*)buffer;
	APCO25_NETWORK_IDENTIFIER nid;
	SwapEndian(packet.nid._data64, nid._data64);

	TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("NID:\t\t"));
	for(uint16_t i=0; i<8; i++){
		TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("%02X ", buffer[i]));
	}
	TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("\n NAC:\t\t%i\n", nid.NAC));
	TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" DUI:\t\t%i\n", nid.DUI));

	switch (type) {
		case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		{
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("HEADER:\t\t"));
			for(uint16_t i=0; i<12; i++){
				TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("%02X ", buffer[i+8]));
			}
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("\n A/N:\t\t%i\n", packet.header.AN));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" I/O:\t\t%i\n", packet.header.IO));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" SAP:\t\t%i\n", packet.header.SAP));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" MFID:\t\t%i\n", packet.header.MFID));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" LID:\t\t%X %X %X\n", packet.header.LID[0], packet.header.LID[1], packet.header.LID[2]));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" FMF:\t\t%i\n", packet.header.FMF));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" BLOCKS:\t%i\n", packet.header.BLOCKS));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" PAD COUNT:\t%i\n", packet.header.PAD_COUNT));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" DATA OFFSET:\t%i\n", packet.header.DATA_OFFSET));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" CRC16:\t\t%i\n", packet.header.CRC16));
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("DATA:\t\t"));
			for(uint8_t j=0;j<packet.header.BLOCKS;j++){
				for(uint16_t i=0; i<12; i++){
					TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("%02X ", buffer[j*12+i+20]));
				}
				TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" -> |"));
				for(uint16_t i=0; i<12; i++){
					if(buffer[j*12+i+20]>32){
						TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("%c", buffer[j*12+i+20]));
					} else {
						TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, (" "));
					}
				}
				TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("|\n\t\t"));
			}
			TPP_DEBUGF(PACKER_DEBUG | TPP_DBG_TRACE, ("\n"));
		}
			break;
		default:
			break;
	}
}


template <class T> void PACKER::SwapEndian(const T source, T& dest){
	uint8_t *buffersource=(uint8_t*)&source;
	uint8_t *bufferdest=(uint8_t*)&dest;
	uint16_t size;
	size=sizeof(T);
	bufferdest+=size-1;
	while(size--){
		*(bufferdest--)=*(buffersource++);
	}
}

PACKER_APCO25::PACKAGE_TYPE PACKER_APCO25::GetLastUpackedType() {
	PACKAGE_TYPE type = _lastunpacktype;
	_lastunpacktype = PACKAGE_TYPE_NONE;
	return type;
}

uint16_t PACKER_APCO25::GetPreferedPayloadSize(PACKAGE_TYPE package_type) {
	switch (package_type) {
	case PACKAGE_TYPE_AUDIO:
		return 320;
	case PACKAGE_TYPE_DATA_NET:
	case PACKAGE_TYPE_DATA_SERIAL:
	case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		return 1520;
	case PACKAGE_TYPE_HEADER:
	case PACKAGE_TYPE_LDU1:
	case PACKAGE_TYPE_LDU2:
	case PACKAGE_TYPE_NONE:
	case PACKAGE_TYPE_UNKNOWN:
		break;
	}
	return 0;
}

uint16_t PACKER_APCO25::GetOverheadSize(PACKAGE_TYPE package_type) {
	switch (package_type) {
	case PACKAGE_TYPE_AUDIO:
		return 8;
	case PACKAGE_TYPE_DATA_NET:
	case PACKAGE_TYPE_DATA_SERIAL:
	case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		return 32;
	case PACKAGE_TYPE_HEADER:
	case PACKAGE_TYPE_LDU1:
	case PACKAGE_TYPE_LDU2:
	case PACKAGE_TYPE_NONE:
	case PACKAGE_TYPE_UNKNOWN:
		break;
	}
	return 0;
}

uint16_t PACKER_APCO25::GetPreferedPackageSize(PACKAGE_TYPE package_type) {
	uint16_t resp = 0;
	switch (package_type) {
	case PACKAGE_TYPE_AUDIO:
	case PACKAGE_TYPE_DATA_NET:
	case PACKAGE_TYPE_DATA_SERIAL:
	case PACKAGE_TYPE_UNCONFIRMED_DATA_PACKAGE:
		resp = GetPreferedPayloadSize(package_type) + GetOverheadSize(package_type);
		break;
	case PACKAGE_TYPE_HEADER:
	case PACKAGE_TYPE_LDU1:
	case PACKAGE_TYPE_LDU2:
	case PACKAGE_TYPE_NONE:
		break;
	case PACKAGE_TYPE_UNKNOWN:
		resp = 2048;
		break;
	}
	return resp;
}

uint16_t PACKER_APCO25::GetUnpackSize() {
	return GetPreferedPackageSize(_unpacktype);
}
uint16_t PACKER_APCO25::PackUnconfirmedDataPackage(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t nac, uint8_t mfid, uint8_t dataoffset) {
	if(size>1532) size=1532;

	APCO25_UNCONFIRMED_DATA_PACKET &packet=*(APCO25_UNCONFIRMED_DATA_PACKET*)bufferdest;
	packet.nid._data64=0;
	packet.nid.NAC=nac;
	packet.nid.DUI=APCO25_DUI_PDU;
	NID_BCH_CODE(packet.nid);

	packet.header._data8[0]=0;
	packet.header.AN=0;
	packet.header._data8[1]=0xC0;
	packet.header.SAP=APCO25_SAP_UNENCRYPTED_USER_DATA;
	packet.header.MFID=mfid;
	packet.header.LID[0]=0x01;
	packet.header.LID[1]=0x02;
	packet.header.LID[2]=0x03;
	packet.header.FMF=1;
	packet.header.BLOCKS=(size+3)/12+1;
	packet.header._data8[7]=0;
	packet.header.PAD_COUNT=(uint16_t)12*packet.header.BLOCKS-4-size;
	packet.header._data8[8]=0;
	packet.header._data8[9]=0;
	packet.header.DATA_OFFSET=dataoffset;
	packet.header.CRC16=CRC_CCIT::Calculate(bufferdest, 10);
	std::copy(buffersource, buffersource+size, &(packet.data[0].data[0]));
	std::fill(&(packet.data[0].data[0])+size,&(packet.data[0].data[0])+size+packet.header.PAD_COUNT, 0 );
	uint32_t *crc=(uint32_t*)&packet.data[packet.header.BLOCKS-1].data[8];
	*crc=CRC_32::Calculate(buffersource, size)+1;
	return (packet.header.BLOCKS+1)*12+8;
}

uint16_t PACKER_APCO25::PackRAW(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t nac, uint8_t dui) {
	APCO25_UNCONFIRMED_DATA_PACKET &packet=*(APCO25_UNCONFIRMED_DATA_PACKET*)bufferdest;
	packet.nid._data64=0;
	packet.nid.NAC=nac;
	packet.nid.DUI=dui;
	NID_BCH_CODE(packet.nid);
	std::copy(buffersource, buffersource+size, bufferdest+8);
	return (size+8);
}

void PACKER_APCO25::ResetUnpack() {
	_unpacktype = PACKAGE_TYPE_UNKNOWN;
	_unpackbuffer.Clear();
}


int16_t PACKER_APCO25::UnpackRAW(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t sizemax) {
	uint16_t preferedsize=GetPreferedPayloadSize(_unpacktype);
	_unpackbuffer.In(buffersource, size);
	if(_unpackbuffer.Ocupied()<preferedsize){
		return _unpackbuffer.Ocupied()-preferedsize;
	} else {
		if(sizemax>preferedsize) sizemax=preferedsize;
		preferedsize=_unpackbuffer.Out(bufferdest,sizemax);
		_lastunpacktype=_unpacktype;
		ResetUnpack();
		return preferedsize;
	}
	return 0;
}

int16_t PACKER_APCO25::UnpackUnconfirmedDataPackage(uint8_t* buffersource, uint16_t size, uint8_t* bufferdest, uint16_t sizemax) {
	uint16_t preferedsize=GetPreferedPayloadSize(_unpacktype);
	uint16_t datasize=0;
	_unpackbuffer.In(buffersource, size);
	if(_unpackbuffer.Ocupied()>12){
		APCO25_DATA_PACKET_HEADER &header=*(APCO25_DATA_PACKET_HEADER*)_unpackbuffer.GetRearrangedBuffer();
		datasize=header.BLOCKS*12-header.PAD_COUNT-4;
		preferedsize=header.BLOCKS*12+12;
	}

	if(_unpackbuffer.Ocupied()<preferedsize){
		return _unpackbuffer.Ocupied()-preferedsize;
	} else {
		if(sizemax>datasize) sizemax=datasize;
		_unpackbuffer.Out(0,12);
		sizemax=_unpackbuffer.Out(bufferdest,sizemax);
		_lastunpacktype=_unpacktype;
		ResetUnpack();
		return sizemax;
	}
	return 0;
}
