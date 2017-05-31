/*
 * Robo.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "Robo.h"
#include "pins.h"

#define sin_phi 0.8480
#define cos_phi 0.52
#define sin_theta 0.7313
#define cos_theta -0.682

Robo::Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, adc *sensorAdc, NRF24L01P *mynrf24, Switch *Switch, bool testmode):
	_nrf24(mynrf24),
	_testmode(testmode),
	printv(false)
{
	motors[0]=roboMotor0;
	motors[1]=roboMotor1;
	motors[2]=roboMotor2;
	motors[3]=roboMotor3;

	roboAdc = sensorAdc;
	roboAdc->ADC_Config();

	drible = new dibre();

	high_kick = new GPIO(GPIOD, GPIO_Pin_8);
	chute_baixo = new GPIO(GPIOD, GPIO_Pin_10);

	_id = Switch->id;

	channel=100;
	address=0xE7E7E7E700;
	last_packet_ms = 0;
}

void Robo::init(){
	_nrf24->Init();
	_nrf24->Config();

	_nrf24->StartRX_ESB(channel, address + GetId(), 32, 1);
	_nrf24->TxPackage_ESB(channel, address + GetId(), 0,(uint8_t*) "TESTE", 5);
	while(_nrf24->Busy()){
		_nrf24->InterruptCallback();
	}
	_nrf24->StartRX_ESB(channel, address + GetId(), 32, 1);
}

void Robo::HighKick(){
	float sensorValue2 = roboAdc->readSensor(3);
	if(sensorValue2>0.20){
		high_kick->Set();
		for(int i=0;i<0xeee2;i++);
		high_kick->Reset();
	}
}

void Robo::ChuteBaixo(){
	float sensorValue2 = roboAdc->readSensor(3);
	if(sensorValue2>0.17){
		chute_baixo->Set();
		for(int i=0;i<0xeee2;i++);
		chute_baixo->Reset();
	}
}

void Robo::control_pos(){
	for(int i=0; i<4; i++){
		motors[i]->Control_Pos(pos[i]);
	}
}
void Robo::control_speed(){
  vBat = 4.3*roboAdc->adc_getConversion();
  if(vBat>6){
    for(int i=0; i<4; i++){
	  motors[i]->Control_Speed(speed[i]); //manda a velocidade speed[i] pro motor[i] na unidade m/s
    }
  }
  else{//medida de proteção: se a bateria estiver fraca, o robô para
    for(int i=0; i<4; i++){
	  motors[i]->SetDutyCycle(0);
    }
  }
}

//armazena as velocidades lineares dos centros das RODAS em ptr
void Robo::get_wheel_speeds(float ptr[]){
	for(int i=0; i<4; i++){
		ptr[i]=motors[i]->real_wheel_speed;
	}
	return;
}

//recebe as velocidades radial, tangente em m/s e w em rad/s
//grava em speed[] os valores em m/s da velocidade DAS RODAS
void Robo::set_speed(float v_r, float v_t, float w){
	float R = 0.09; //Raio do robo = 9cm

	speed[0] = -v_r*cos_phi + v_t*sin_phi + w*R;
	speed[2] = -v_r*cos_phi - v_t*sin_phi + w*R;
	speed[3] = -v_r*cos_theta - v_t*sin_theta + w*R;
	speed[1] = -v_r*cos_theta + v_t*sin_theta + w*R;
/*		speed[0] = 0.176; //teste: para cada roda girar com período 1s
		speed[2] = 0.176;
		speed[3] = 0.176;
		speed[1] = 0.176;*/
	//speed[] está em m/s. Cuidado para manter a mesma unidade qnd passar pros motores

}

void Robo::set_speed(float v[]){
	speed[0] = v[0];
	speed[1] = v[1];
	speed[2] = v[2];
	speed[3] = v[3];
}

void Robo::set_motor_speed(uint8_t motnr, float vel) {
	if(motnr<4){
		speed[motnr]=vel;
	}
}

void Robo::interrupt_control(){
	get_wheel_speeds(robo.real_wheel_speed);//update real_wheel_speed com as velocidades medidas
	if(controlbit&&(!stepbit)){
		control_speed();
	}
	if(!controlbit){
		for(int j=0; j<4; j++){
			if(!stepbit)motors[j]->SetDutyCycle(0);
			motors[j]->Calc_Speed();
		}
	}
	if (printv){
		robotcmd_test.kickspeedx = robo.motors[0]->real_wheel_speed;
		robotcmd_test.kickspeedz = robo.motors[1]->real_wheel_speed;
		robotcmd_test.veltangent = robo.motors[2]->real_wheel_speed;
		robotcmd_test.velnormal = robo.motors[3]->real_wheel_speed;
		robotcmd_test.velangular=(float)GetLocalTime();
		robotcmd_test.id=robo.dutycycles[0];
		robotcmd_test.spinner=true;
		uint8_t buffer[32];
		pb_ostream_t ostream=pb_ostream_from_buffer(buffer, sizeof(buffer));
		pb_encode(&ostream, grSim_Robot_Command_fields, &robotcmd_test);//escreve em ostream os dados de robotcmd_test

		_usbserialbuffer2.In((uint8_t*)buffer, (uint16_t) ostream.bytes_written);
	}
	if ((!printv)&&(printI)){
		robotcmd_test.kickspeedx = motors[0]->mina22->ReadCurrent();
		robotcmd_test.kickspeedz = motors[1]->mina22->ReadCurrent();
		robotcmd_test.veltangent = motors[2]->mina22->ReadCurrent();
		robotcmd_test.velnormal = motors[3]->mina22->ReadCurrent();
		robotcmd_test.velangular=(float)GetLocalTime();
		robotcmd_test.spinner=false;
		uint8_t buffer[32];
		pb_ostream_t ostream=pb_ostream_from_buffer(buffer, sizeof(buffer));
		pb_encode(&ostream, grSim_Robot_Command_fields, &robotcmd_test);

		_usbserialbuffer2.In((uint8_t*)buffer, (uint16_t) ostream.bytes_written);
	}
}

void Robo::interruptReceive(){
    bool status=0;
	if(_nrf24->RxSize()){
		_nrf24->StartRX_ESB(channel, address + robo.GetId(), 32, 1);
		uint8_t rxsize=_nrf24->RxSize();
		if(rxsize>32) rxsize=32;
		uint8_t buffer[32];
		_nrf24->RxData(buffer, rxsize);
		//usb_device_class_cdc_vcp.SendData((uint8_t*)buffer, rxsize);
		pb_istream_t istream=pb_istream_from_buffer(buffer, rxsize);
		status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);
		last_packet_ms = GetLocalTime();
		controlbit = true;
	}
	if(status){
		if(robotcmd.id==GetId()){
			processPacket();
		}
	}
	if((GetLocalTime()-last_packet_ms)>100){
		controlbit = false;
	}
}
void Robo::interruptTestMode(){
	cmdline.In(_usbserialbuffer);
	cmdline.Out(_usbserialbuffer);
	if(_usbserialbuffer.Ocupied()){
		//usb_device_class_cdc_vcp.SendData(_usbserialbuffer);
	}
	if(_usbserialbuffer2.Ocupied()){
		usb_device_class_cdc_vcp.SendData(_usbserialbuffer2);
	}
	//robo.controlbit = true;
}
void Robo::processPacket(){
	robo.set_speed(robotcmd.veltangent, robotcmd.velnormal, robotcmd.velangular);
	if(robotcmd.kickspeedx!=0)
	robo.ChuteBaixo();
	if(robotcmd.kickspeedz!=0)
	//robo.HighKick();
	if(robotcmd.spinner)
	robo.drible->Set_Vel(100);
}

//  5º dia: ainda estou na classe robo

void Robo::interruptTransmitter(){
    bool status=0;
	uint8_t buffer[32];
	uint8_t size=_usbserialbuffer.Out(buffer, 32);//escreve em buffer o que recebeu
	pb_istream_t istream = pb_istream_from_buffer(buffer,size);
	status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);//preenche robotcmd
	if(status){//caso haja sucesso na decodificação
		uint8_t robotid=robotcmd.id;//extrai o  id do pacote
		uint8_t buffer[32];
		pb_ostream_t ostream=pb_ostream_from_buffer(buffer, sizeof(buffer));
		pb_encode(&ostream, grSim_Robot_Command_fields, &robotcmd);//escreve em ostream os dados de robotcmd
		uint8_t size=ostream.bytes_written;
		_nrf24->TxPackage_ESB(channel, address | robotid, 0, buffer, size);
		while(_nrf24->Busy()){
			_nrf24->InterruptCallback();
		}
	}
	else {
		_usbserialbuffer.Clear();
	}
}
void Robo::interruptAckPayload(){
	char ackBuffer[20];
	int ackSize=sprintf(ackBuffer, "test \n");
	_nrf24->write_ack_payload((uint8_t *) ackBuffer, ackSize);
}

