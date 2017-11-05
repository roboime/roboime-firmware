#pragma once
#include <utils/circularbuffer.h>
#include <map>
#include <initializer_list>
#include <list>
#include <string.h>
#include <stdio.h>

class CommandLine{
public:
	typedef uint16_t (*CommandFunction)(uint16_t argc, uint8_t *argv[]);

	static uint16_t cmd_echo(uint16_t argc, uint8_t *argv8[]){
		CommandLine &cf=*(CommandLine*)argv8[argc];
		const char **argv=(const char **)argv8;
		uint16_t size=0;
		char* buffer=(char*)argv[0];
		if(argc==2){
			cf._echo=atoi(argv[1]);
		} else {
			size+=sprintf(buffer+size, "ECHO %d\r\n", cf._echo);
		}
		return size;
	}

	CommandLine(std::initializer_list<const char*> c, std::initializer_list<CommandFunction> d):
		_argc(0),
		_cmdbuffer(0,512),
		_respbuffer(0,512),
		_echo(1)
	{
		auto i = c.begin();
		auto j = d.begin();
		for(; i != c.end() && j!=d.end(); i++, j++) {
			_cmd_list.push_front(*i);
			_cmd_function_list.push_front(*j);
		}
		_cmd_list.push_front("echo");
		_cmd_function_list.push_front(cmd_echo);
	}

	void ShowPrompt(){
		if(_echo) _respbuffer.In((uint8_t*)">", 1);
	}

	void ShowCRLF(){
		if(_echo) _respbuffer.In((uint8_t*)"\r\n", 2);
	}

	void CmdCancel(){
		_cmdbuffer.Clear();
		if(_echo) _respbuffer.In((uint8_t*)"^C", 2);
	}

	void CmdBackspace(){
		if(_cmdbuffer.Ocupied()){
			_cmdbuffer.OutEnd(0,1);
			if(_echo) _respbuffer.In(8);
			if(_echo) _respbuffer.In(' ');
			if(_echo) _respbuffer.In(8);
		} else {
			if(_echo) _respbuffer.In(7);
		}
	}

	void In(CircularBuffer<uint8_t> &cb){
		uint8_t c;
		while(cb.Ocupied()){
			c=cb.Out();
			if(c<32 || c==127){
				if(c==3){
					CmdCancel();
					c='\r';
				}
				if(c==8 || c==127){
					CmdBackspace();
				}
				if(c=='\r' || c=='\n'){
					ShowCRLF();
					if(_cmdbuffer.Ocupied()){
						Process();
					}
					ShowPrompt();
				}
			} else {
				if(_echo) _respbuffer.In(c);
				_cmdbuffer.In(c);
			}
		}
	}

	void Out(CircularBuffer<uint8_t> &cb){
		_respbuffer.Out(cb, cb.Free());
	}
	uint16_t Out(uint8_t *buffer, uint16_t size){
		return _respbuffer.Out(buffer, size);
	}

	void Process(){
		uint8_t _spacelastc=1;
		uint8_t *argb=_cmdbuffer.GetRearrangedBuffer();
		uint8_t c;
		_argc=0;
		while(_cmdbuffer.Ocupied()){
			c=_cmdbuffer.Out();
			if(c==' '){
				if(_spacelastc){
					continue;
				}
				_spacelastc=1;
				c=0;
			} else {
				if(_spacelastc){
					_spacelastc=0;
					if(_argc<40){
						_argv[_argc++]=argb;
					}
				}
			}
			*(argb++)=c;
		}
		*(argb++)=0;
//		printf("argc=%d\t->|", _argc);
//		for(uint16_t i=0;i<_argc;i++){
//			printf("%s ", _argv[i]);
//		}
//		printf("|<-\n");

		auto i = _cmd_list.begin();
		auto j = _cmd_function_list.begin();
		CommandFunction cmdf=0;
		for(; i != _cmd_list.end() && j!=_cmd_function_list.end(); i++, j++) {
			if(!strcmp((char*)_argv[0], *i)){
				cmdf=*j;
				break;
			}
		}

		if(cmdf){
			_argv[_argc]=(uint8_t*)this;
			uint16_t size=cmdf(_argc, _argv);
			if(size){
				_respbuffer.In(_argv[0], size);
			}
			_argv[0][size]=0;
//			printf("resp=%d\t->|", size);
//			printf("%s ", _argv[0]);
		} else {
			_respbuffer.In((uint8_t*)"Invalid Command!\r\n", 18);
		}
		_cmdbuffer.Clear();
	}

protected:
	uint8_t _argc;
	uint8_t *_argv[40];
	CircularBuffer<uint8_t> _cmdbuffer;
	CircularBuffer<uint8_t> _respbuffer;
//	std::map<std::string, CommandFunction> _commands;
	std::list<const char*> _cmd_list;
	std::list<CommandFunction> _cmd_function_list;
	uint8_t _echo;
};

