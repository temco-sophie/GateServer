/******************************************************************************
 * INCLUDE
 */
#include "osal.h"
#include "mmc.h"
#include "fat.h"
#include "MT.h"
#include "OSAL_Clock.h"
#include "OnBoard.h"
#include "stdio.h"

/******************************************************************************
 * @fn      DELAY_Us
 * 
 * @brief   Delay wait 
 * 
 * @param   uint16- us time to wait
 * 
 * @return  none
 */

void DELAY_Us(uint16 loop)
{
  while(loop--)
    asm("NOP");
}

/******************************************************************************
 * @fn      mmc_init
 * 
 * @brief   Initialize mmc support
 * 
 * @param   none
 * 
 * @return  uint8 - status
 */
uint8 mmc_init ( void)
{
	uint8 Timeout = 0;
	uint8 a,b;
	uint8 CMD[] = {0x40,0x00,0x00,0x00,0x00,0x95};
        
        P0SEL &= ~0x3c;
        
        P0DIR |= 0x38;
        P0DIR &= ~0x04;
        
	SPI_DI=1; 
	SPI_Clock=1; 
	MMC_Chip_Select=1;    

	for( a=0;a<200;a++){
          asm("NOP");
	};	
	for ( b = 0;b<0x0f;b++) 
	{
		mmc_write_byte(0xff);
	}
	while(mmc_write_command (CMD) !=1)
	{
		if (Timeout++ > 5)
		{
			return(1); 
		}
	}
	//while(1){};
	Timeout = 0;
	CMD[0] = 0x41;//Commando 1
	CMD[5] = 0xFF;
	while( mmc_write_command (CMD) !=0)
	{
		if (Timeout++ > 100)
		{
			return(2); 
		}
	}
	MMC_Disable();
    
	return(0);  
//	printf("success\r\n");  
//	while(1){};//  成功
}

/******************************************************************************
 * @fn      mmc_read_byte
 * 
 * @brief   read one byte by simulate SPI port
 * 
 * @param   none
 * 
 * @return  uint8 - The byte was been read
 */

uint8 mmc_read_byte (void)
{
	uint8 Byte = 0;
	uint8 i = 0;
	SPI_DI=1;
	for (i=0; i<8; i++) 
	{
		SPI_Clock=0;    
		DELAY_Us(10);
		Byte=Byte<<1;                            //  先接收最高位。
		if (SPI_DI==1) 
		{
				Byte |= 0x01;
		}

		SPI_Clock=1;    
		DELAY_Us(10);
	}
	return (Byte);
}

/******************************************************************************
 * @fn      mmc_write_byte
 * 
 * @brief   write one byte by simulate SPI port
 * 
 * @param   uint8 - The byte was wanted to be writen
 * 
 * @return  none
 */

void mmc_write_byte (uint8 Byte)

{
	uint8 i ;
	for (i =0; i<8; i++) 
	{
		if (Byte&0x80)	     //   先写高位的。
		{
			SPI_DO=1; 
		}
		else
		{
			SPI_DO=0; 
		}	
		SPI_Clock=0;   
		DELAY_Us(10);
		Byte=Byte<<1;
		SPI_Clock=1;   
		DELAY_Us(10);
	}
	SPI_DO=1;          
}

/******************************************************************************
 * @fn      mmc_write_command
 * 
 * @brief   write a SD command by simulate SPI port
 * 
 * @param   cmd - command will be writen
 * 
 * @return  none
 */

uint8 mmc_write_command (uint8 *cmd)

{
	uint8 tmp = 0xff;
	uint16 Timeout = 0;
	uint8 a;
	
//	while(1)
	{
		MMC_Disable();//DELAY_Us(5);
		mmc_write_byte(0xFF);
		MMC_Enable();//DELAY_Us(5);
		for ( a = 0;a<0x06;a++) //send 6 Byte Command 
		{
			mmc_write_byte(cmd[a]);
		}
	//	DELAY_Us(5000);
	}
	while (tmp == 0xff)	
	{
		tmp = mmc_read_byte();
		if (Timeout++ > 500)
		{
			break;
		}
//		ComSendByte( tmp);
//		ComSendByte('b');
	}
	return(tmp);
}

/******************************************************************************
 * @fn      mmc_write_sector
 * 
 * @brief   write a SD sector
 * 
 * @param   addr - address of SD card
 *          Buffer - pointer to the buffer that will be written
 * 
 * @return  status
 */


uint8 mmc_write_sector (uint32 addr,uint8 *Buffer)
{
	uint8 tmp;
	uint16 a ;
	uint8 cmd[] = {0x58,0x00,0x00,0x00,0x00,0xFF}; 
	
//	LED_MMC_WR=0;	  
	  
	addr = addr << 9; //addr = addr * 512
	
	cmd[1] = ((addr & 0xFF000000) >>24 );
	cmd[2] = ((addr & 0x00FF0000) >>16 );
	cmd[3] = ((addr & 0x0000FF00) >>8 );

	tmp = mmc_write_command (cmd);
	if (tmp != 0)
	{
//		LED_MMC_WR=1;	  
		return(tmp);
	}
			
	for (a=0;a<100;a++)
	{
		mmc_read_byte();
	}
	
	mmc_write_byte(0xFE);	
	
	for ( a=0;a<512;a++)
	{
		mmc_write_byte(*Buffer++);
	}
	
	mmc_write_byte(0xFF); //Schreibt Dummy CRC
	mmc_write_byte(0xFF); //CRC Code wird nicht benutzt
	
	while (mmc_read_byte() != 0xff){};
	
	MMC_Disable();
	
//	LED_MMC_WR=1;	  
	return(0);
}

/******************************************************************************
 * @fn      mmc_read_block
 * 
 * @brief   Read a block to Buffer
 * 
 * @param   cmd - command to read block
 *          Buffer - valid data buffer at least 'Bytes' bytes in size
 *          Bytes - max length number of bytes to copy to Buffer
 * 
 * @return  none
 */

void mmc_read_block(uint8 *cmd,uint8 *Buffer,uint16 Bytes)
{	
	uint16  a;
	if (mmc_write_command (cmd) != 0)
	{
		 return;
	}
	while (mmc_read_byte() != 0xfe){};

	for ( a=0;a<Bytes;a++)
	{
		*Buffer++ = mmc_read_byte();
	}
	mmc_read_byte();//CRC - Byte wird nicht ausgewertet
	mmc_read_byte();//CRC - Byte wird nicht ausgewertet
	
	MMC_Disable();
	
	return;
}

/******************************************************************************
 * @fn      mmc_read_sector
 * 
 * @brief   read a SD sector
 * 
 * @param   addr - address of SD card
 *          Buffer - valid data buffer
 * 
 * @return  status
 */

uint8 mmc_read_sector (uint32 addr,uint8 *Buffer)
{	
	uint8 cmd[] = {0x51,0x00,0x00,0x00,0x00,0xFF}; 
	
//	LED_MMC_WR=0;	  
	addr = addr << 9; //addr = addr * 512

	cmd[1] = ((addr & 0xFF000000) >>24 );
	cmd[2] = ((addr & 0x00FF0000) >>16 );
	cmd[3] = ((addr & 0x0000FF00) >>8 );

    mmc_read_block(cmd,Buffer,512);

//	LED_MMC_WR=1;	  
	return(0);
}

/******************************************************************************
 * @fn      mmc_write_event
 * 
 * @brief   Write all kinds of events to SD in one format
 * 
 * @param   modbusId - the id of gate who send the event
 *          data - pointer to the buf that will be writen
 *          event - event id 
 * 
 * @return  none
 */


void mmc_write_event( uint8 modbusId, uint8 *data, uint8 event)
{
#if MMC_WRITE_ENABLE
  char *pBuf;  
  uint8 temp1[11];
  uint8 temp2[4];
  uint8 pUTCTime[20];
  
  uint16 conv1,conv2;
  uint8 *p;
  
  uint8 temp3[4];
  uint8 temp4[4];
  uint8 temp5[10];
  
  uint16 microsec;
  uint16 millisec;
  uint16 sec;
  uint32 tempTime = BUILD_UINT32( data[0], data[1], data[2], data[3]);
  
  UTCTime utcSecs;
  UTCTimeStruct utcTime;
  
  utcSecs = osal_getClock();
  osal_ConvertUTCTime( &utcTime, utcSecs);

  utcTime.month++;
  utcTime.day++;
  p = pUTCTime;
  
  memset( pUTCTime, '0', 19);
  if( utcTime.hour < 10)
    _itoa( (uint16)( utcTime.hour & 0x00FF), &pUTCTime[1], 10); 
  else
    _itoa( (uint16)( utcTime.hour & 0x00FF), pUTCTime, 10); 
  pUTCTime[2] = ':';
  
  if( utcTime.minutes < 10)
    _itoa( (uint16)( utcTime.minutes & 0x00FF), &pUTCTime[4], 10);
  else
    _itoa( (uint16)( utcTime.minutes & 0x00FF), &pUTCTime[3], 10);
  pUTCTime[5] = ':';
  
  if( utcTime.seconds < 10)
    _itoa( (uint16)( utcTime.seconds & 0x00FF), &pUTCTime[7], 10);
  else
    _itoa( (uint16)( utcTime.seconds & 0x00FF), &pUTCTime[6], 10);
  pUTCTime[8] = ' ';
  
  if( utcTime.month < 10)
    _itoa( (uint16)( utcTime.month & 0x00FF), &pUTCTime[10], 10);
  else
    _itoa( (uint16)( utcTime.month & 0x00FF), &pUTCTime[9], 10);
  pUTCTime[11] = '/';
  
  if( utcTime.day < 10)
    _itoa( (uint16)( utcTime.day & 0x00FF), &pUTCTime[13], 10);
  else
    _itoa( (uint16)( utcTime.day & 0x00FF), &pUTCTime[12], 10);
  pUTCTime[14] = '/';
  _itoa( utcTime.year, &pUTCTime[15], 10);

  for( uint8 i = 0; i < 18; i++, p++)
  {
    if( *p == 0)
      *p = '0';
  }
  pBuf = osal_mem_alloc( 512);
  
  
  _itoa( (uint16)modbusId, temp2, 10);
  
  if( pBuf)
  {
    switch( event)
    {
      case SD_RFID_EVENT:
        conv1 = BUILD_UINT16( data[1], data[0]);
        conv2 = BUILD_UINT16( data[3], data[2]);
        
        _itoa( conv1, temp1, 10);
        _itoa( conv2, &temp1[5], 10);
  
        sprintf( pBuf, "Event: RFID Log in\r\nModBus ID:%s\r\nRFID Num:%s\r\nTime:%s\r\n", temp2, temp1, pUTCTime);
        break;
      case SD_TIME_EVENT:
        
        microsec = tempTime % 1000;
        millisec = (tempTime / 1000) % 1000;
        sec = tempTime / 1000 /1000;
        
        _itoa( microsec, temp3, 10);
        _itoa( millisec, temp4, 10);
        _itoa( sec, temp5, 10);
        
        sprintf( pBuf, "Event: Time Record\r\nModBus ID:%s\r\nTime Data:%s Sec %s MilliSec %s MicroSec\r\nTime:%s\r\n", temp2, temp5, temp4, temp3, pUTCTime);
        break;
      case SD_TIME_RESULT:
        microsec = tempTime % 1000;
        millisec = (tempTime / 1000) % 1000;
        sec = tempTime / 1000 /1000;
        
        _itoa( microsec, temp3, 10);
        _itoa( millisec, temp4, 10);
        _itoa( sec, temp5, 10);
        sprintf( pBuf, "Event: Time Result\r\nModBus ID:%s\r\nResult:%s Sec %s MilliSec %s MicroSec\r\nTime:%s\r\n", temp2, temp5, temp4, temp3, pUTCTime);
        break;
      default:
        sprintf( pBuf, "UNKNOW EVENT\r\nMODBUS ID:%s\r\nDATA:%s\r\nTime:%s", temp2, temp1, pUTCTime);
        break;
    }
  }
  
  mmc_write_file( (uint8*)pBuf, strlen(pBuf));
  osal_mem_free( pBuf);
#endif
}

/******************************************************************************
 * @fn      mmc_write_file
 * 
 * @brief   Write a file to SD
 * 
 * @param   data - pointer to the buf that will be writen
 *          len - length of
 * 
 * @return  none
 */

void mmc_write_file(uint8* data, uint16 len)
{
  uint16 i;
  
  mmc_init();
  fat_cluster_data_store();
//  datatemp=0;
  
  if(fat_write_file_lock ("GATE    TXT"))
  {
    osal_memset( SectorBuffer, 0x20, 512);
    for( i = 0; i < len; i++)
    {
      SectorBuffer[i] = data[i];
    }

    if(len < 512)
    {
      osal_memset( &SectorBuffer[len], 0x20, 512-len);
    }
    SectorBuffer[510] = '\r';
    SectorBuffer[511] = '\n';
    fat_write_file();
  }
}

/******************************************************************************
 * @fn      mmc_read_file
 * 
 * @brief   Read a file from SD
 * 
 * @param   none
 * 
 * @return  none
 */

void mmc_read_file( void)
{
  uint16  Clustervar=0;
  uint32  Size = 0;
  uint8  Dir_Attrib = 0;
  
  mmc_init();
  fat_cluster_data_store();
  if ( fat_search_file("GATE    TXT", &Clustervar, &Size, &Dir_Attrib) == 1)
  {
    if( fat_read_file( Clustervar, 0, 512))
    {
//      for( Clustervar = 0; Clustervar < 512; Clustervar ++)
      {
        MT_BuildAndSendZToolResponse( 0x67, 0x17, 128, SectorBuffer);
//        MT_BuildAndSendZToolResponse( 0x67, 0x17, 128, &SectorBuffer[128]);
//        MT_BuildAndSendZToolResponse( 0x67, 0x17, 128, &SectorBuffer[256]);
//        MT_BuildAndSendZToolResponse( 0x67, 0x17, 128, &SectorBuffer[384]);
      }
    }
  }
}
/******************************************************************************
 */



/* test code */
#if 0
uint16   datatemp=0;

void SD_test(void)
{
//	char input;
//	uint8 temp = 0;
	uint16 i;
	uint16  Clustervar=0;
	uint32  Size = 0;
	uint8  Dir_Attrib = 0;
	//printf("test sd\r\n");
	//while(1)
	{
	//	scanf("input = %c\r\n",&input);
	//	if(input == '1') // write
		{
		//	printf("write\r\n");
			mmc_init ();//初始化SD卡		
			fat_cluster_data_store();    
			datatemp=0;
			//printf("write %d\r\n",(int)temp);
			if(fat_write_file_lock ("TEST    TXT"))//文件名8个字节  
			{						 
				//printf("test.txt\r\n");
	 			for(i=0;i<512;i++)//一次写一个扇区，512个字节
				{
					SectorBuffer[i] = datatemp+0x30;//写入缓冲，ASCII码
					datatemp++;
					if(datatemp == 5)//存储数据0-9循环
						datatemp = 0;
				}
				fat_write_file();  //写入数据 
			}
		}
	//	if(input == '2') // read
		{
		//	printf("read\r\n");
		//	mmc_init ();	
		//	printf("read %d\r\n",(int)temp);
			fat_cluster_data_store();			
			if (fat_search_file("TEST    TXT",&Clustervar,&Size,&Dir_Attrib) == 1)//创建文件名
			{
				 if(fat_read_file ( Clustervar,0,512))//512个字节
				 {					
				 	for(Clustervar=0;Clustervar<512;Clustervar++)
				 	{
						//printf("%c",SectorBuffer[Clustervar]);
						//ComSendByte( SectorBuffer[Clustervar]);//读出的数据从串口发送出
						//ComSendByte( 'a');//读出的数据从串口发送出
						
				 	}	
				}
			}
		}
	}
}
#endif