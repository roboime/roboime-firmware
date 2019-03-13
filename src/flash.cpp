#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "flash.h"
//#include "stdint.h"

void WriteFlash(uint32_t FLASH_SECTOR_ADDRESS, uint32_t NEXT_FLASH_SECTOR_ADDRESS, uint32_t data)
{
  volatile uint32_t id_value;
  id_value = (uint32_t)(0x80000000 | data);

  uint32_t saved_value;
  /* Enable the flash control register access */
  FLASH_Unlock();

  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

  //uwStartSector = GetSector(FLASH_SECTOR);
  //uwEndSector = GetSector(FLASH_SECTOR);

  /* Program the user Flash area word by word ********************************/
  /* area defined by FLASH_SECTOR - only one sector being acted upon*/
  /*
   * What I must do here is: run through the addresses of the flash sector and find the first one with a
   * 0x1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX (it is a 32bit word), since i'm looking at the first address,
   * search for a 0x1XXXXXXX. If it is true, then write it as zero and jump to the next usable address
   * (address += 4). There, write 0x8000XXXX (which is the new id).
   *
   * */
  uint32_t uwAddress = FLASH_SECTOR_ADDRESS;
  uint32_t FLASH_SECTOR_LIMIT = NEXT_FLASH_SECTOR_ADDRESS - 4 ;

  while(1)
  {
  	if (uwAddress >= FLASH_SECTOR_LIMIT)
  	{
  		uwAddress = FLASH_SECTOR_ADDRESS;
  	    /* Start the erase operation */
  	    /* Only writing on one sector of the flash, so, only need to erase that sector*/
  	    /* Only gonna erase one time every 1k write operations - only gonna erase if the last address is occupied*/
  		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
  		   be done by word */
  		if (FLASH_EraseSector(GetSector(FLASH_SECTOR_ADDRESS), VoltageRange_3) != FLASH_COMPLETE)
  		{
  		  /* Error occurred while sector erase.
  			 User can add here some code to deal with this error  */
  		  while (1)
  		  {
  		  }
  		}
  	}
  	FLASH_Lock();
  	saved_value = ReadMemoryAddress(uwAddress);
  	FLASH_Unlock();
  	if (saved_value & (1<<31))
  	{
  		//write 0x0000000
  		if (FLASH_ProgramWord(uwAddress, 0x00000000) == FLASH_COMPLETE)
  		{
  		}
  		else
  		{
  		  /* Error occurred while writing data in Flash memory.
  			 User can add here some code to deal with this error */
  		  while (1)
  		  {
  		  }
  		}
  		uwAddress += 4;
  		//write 0x8000000id
  		if (FLASH_ProgramWord(uwAddress, id_value) == FLASH_COMPLETE)
  		{
  		}
  		else
  		{
  		  /* Error occurred while writing data in Flash memory.
  			 User can add here some code to deal with this error */
  		  while (1)
  		  {
  		  }
  		}
  		break;
  	}
  	uwAddress += 4;
  }

  /* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock();
}

uint32_t ReadMemoryAddress(uint32_t address)
{
  uint32_t written_data = *(__IO uint32_t*)address;

  return written_data;
}

void DeleteFlashSector(uint32_t FLASH_SECTOR_ADDRESS)
{
	FLASH_Unlock();

	/* Erase the user Flash area ************************************************/
	/* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

	/* Clear pending flags (if any) */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
	   be done by word */
	if (FLASH_EraseSector(GetSector(FLASH_SECTOR_ADDRESS), VoltageRange_3) != FLASH_COMPLETE)
	{
	  /* Error occurred while sector erase.
		 User can add here some code to deal with this error  */
	  while (1)
	  {
	  }
	}
	FLASH_Lock();
}

int ReadID(uint32_t FLASH_SECTOR_ADDRESS, uint32_t NEXT_FLASH_SECTOR_ADDRESS)
{
	//must read through flash_sector to find the id
	int id=0;
	uint32_t uwAddress = FLASH_SECTOR_ADDRESS;
	uint32_t FLASH_SECTOR_LIMIT = NEXT_FLASH_SECTOR_ADDRESS;
	uint32_t saved_value;

	while(1)
	{
		saved_value = ReadMemoryAddress(uwAddress);
		if (saved_value & (1<<31))
		{
			saved_value <<= 1;
			saved_value >>= 1;
			id = saved_value;
			uwAddress += 4;
			break;
		}
		uwAddress += 4;
		if (uwAddress == FLASH_SECTOR_LIMIT)
		{
			return 16; //16 would be the code for error.
		}
	}

	return id;
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;
  }
  return sector;
}
