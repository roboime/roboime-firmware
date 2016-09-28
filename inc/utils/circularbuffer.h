#pragma once
#include <inttypes.h>
#include <algorithm>

template <typename T,  uint8_t accept_data_when_full = true> class CircularBuffer{
public:
	CircularBuffer(T* buffer, uint16_t size, uint16_t contentsize=0):
		_buffer(buffer),
		_buffersize(size),
		_head(contentsize%size),
		_tail(0),
		_contentsize(contentsize),
		_memoryallocated(0)
	{
		if(!_buffer){
			_buffer=new T[size];
			if(_buffer==NULL){
				_memoryallocated=0;
			}
			_memoryallocated=1;
		}
	}
	~CircularBuffer(){
		if(_memoryallocated){
			delete[] _buffer;
		}
	}
	uint16_t In(const T &item){
		return In(&item, 1);
	}

	uint16_t In(const T* buffer, uint16_t size){
		if(size==0){
			return size;
		}
		uint16_t temp=Free();
		if(size>temp){
			if(accept_data_when_full){
				Out(0, size-temp);
				temp=Free();
			}
			size=temp;
		}
		temp=0;
		if((_head+size)>=_buffersize){
			temp=(_buffersize-_head);
			std::copy(buffer, buffer+temp, _buffer+_head);
			_head=0;
		}
		std::copy(buffer+temp, buffer+size, _buffer+_head);
		_contentsize+=size;
		_head+=size-temp;
		return size;
	}

	uint16_t In(CircularBuffer<T> &CB){
		uint16_t size=CB.Ocupied();
		T cb[size];
		CB.Out(cb,size);
		uint16_t size_in=In(cb, size);
		if(size_in<size){
			CB.In(cb+size_in, size-size_in);
		}
		return size_in;
	}

	uint16_t InBeginning(const T &item){
		return InBeginning(&item, 1);
	}

	uint16_t InBeginning(const T* buffer, uint16_t size){
		if(size==0){
			return size;
		}
		uint16_t temp=Free();
		if(size>temp){
			if(accept_data_when_full){
				Out(0, size-temp);
				temp=Free();
			}
			size=temp;
		}
		temp=0;

		if(_tail < size){
			temp=_tail;
			std::copy(buffer, buffer+temp, _buffer);
			_tail=_buffersize;
		}
		std::copy(buffer+temp, buffer+size, _buffer+_tail-size);
		_contentsize+=size;
		_tail-=(size-temp);
		_tail%=_buffersize;
		return size;
	}

	uint16_t Out(CircularBuffer<T> &CB, uint16_t size){
		if(size==0){
			return size;
		}
		T cb[size];
		size=Out(cb, size);
		CB.In(cb, size);
		return size;
	}

	uint16_t Out(T* buffer, uint16_t size){
		if(size==0){
			return size;
		}
		uint16_t temp=Ocupied();
		if(size>temp){
			size=temp;
		}
		if(!size) return 0;
		temp=0;
		if((_tail+size)>=_buffersize){
			temp=(_buffersize-_tail);
			if(buffer) std::copy(_buffer+_tail, _buffer+_tail+temp, buffer);
			_tail=0;
		}
		if(buffer) std::copy(_buffer+_tail, _buffer+_tail+size-temp, buffer+temp);
		_contentsize-=size;
		_tail+=size-temp;
		if(!_contentsize){
			Clear();
		}
		return size;
	}

	T Out(){
		T temp;
		Out(&temp,1);
		return temp;
	}

	uint16_t OutEnd(T* buffer, uint16_t size){
		if(size==0){
			return size;
		}
		uint16_t temp=Ocupied();
		if(size>temp){
			size=temp;
		}
		if(!size) return 0;
		temp=size;
		if(_head<size){
			temp=(size-_head);
			if(buffer) std::copy(_buffer, _buffer+_head, buffer+temp);
			_head=0;
		}
		if(_head==0){
			_head=_buffersize;
		}
		if(buffer) std::copy(_buffer+_head-temp, _buffer+_head, buffer);
		_contentsize-=size;
		_head-=temp;
		_head%=_buffersize;
		if(!_contentsize){
			Clear();
		}
		return size;
	}

	T &operator[](uint16_t index){
		return _buffer[(_tail+index)%_buffersize];
	}

	uint16_t Free(){
		return _buffersize-_contentsize;
	}
	uint16_t Ocupied(){
		return _contentsize;
	}
	void Clear(){
		_head=0;
		_tail=0;
		_contentsize=0;
	}

	T* GetRearrangedBuffer(){
		if(_tail!=0){
			uint16_t size=Ocupied();
			T temp[size];
			Out(temp, size);
			In(temp, size);
		}
		//TODO Rearrange Buffer
		return _buffer;
	}

protected:
	T* _buffer;
	uint16_t _buffersize;
	uint16_t _head;
	uint16_t _tail;
	uint16_t _contentsize;
	uint8_t _memoryallocated;
};
