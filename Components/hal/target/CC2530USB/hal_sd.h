#if defined ( SD_SUPPLY)

#ifndef _SD_H_
#define _SD_H_

#define INIT_CMD0_ERROR		0x01
#define INIT_CMD1_ERROR		0x02
#define WRITE_BLOCK_ERROR	0x03
#define READ_BLOCK_ERROR	0x04

#define SD_CMD_SET		1
#define SD_CRC			0
#define SD_CMD_COMPLEX          1


typedef struct SD_VOLUME_INFO
 { //SD/SD Card info
   uint16   size_MB;
   uint8    sector_multiply;
   uint16   sector_count;
   uint8    name[6];
 } hal_SD_volume_info_t;


#if SD_CMD_SET
/*****************************************************************************************

 * �������
 */

#define R1			1
#define R1B			2
#define R2			3
#define R3			4

/* ��λSD �� Reset cards to idle state */
#define CMD0 0
#define CMD0_R R1

/* ��OCR�Ĵ��� Read the OCR (MMC mode, do not use for SD cards) */
#define CMD1 1
#define CMD1_R R1

/* ��CSD�Ĵ��� Card sends the CSD */
#define CMD9 9
#define CMD9_R R1

/* ��CID�Ĵ��� Card sends CID */
#define CMD10 10
#define CMD10_R R1

/* ֹͣ�����ʱ�����ݴ��� Stop a multiple block (stream) read/write operation */
#define CMD12 12
#define CMD12_R R1B

/* �� Card_Status �Ĵ��� Get the addressed card's status register */
#define CMD13 13
#define CMD13_R R2

/***************************** ������ Block read commands **************************/

/* ���ÿ�ĳ��� Set the block length */
#define CMD16 16
#define CMD16_R R1

/* ������ Read a single block */
#define CMD17 17
#define CMD17_R R1

/* �����,ֱ����������CMD12Ϊֹ Read multiple blocks until a CMD12 */
#define CMD18 18
#define CMD18_R R1

/***************************** ��д��� Block write commands *************************/
/* д���� Write a block of the size selected with CMD16 */
#define CMD24 24
#define CMD24_R R1

/* д��� Multiple block write until a CMD12 */
#define CMD25 25
#define CMD25_R R1

/* дCSD�Ĵ��� Program the programmable bits of the CSD */
#define CMD27 27
#define CMD27_R R1

/***************************** д���� Write protection *****************************/
/* Set the write protection bit of the addressed group */
#define CMD28 28
#define CMD28_R R1B

/* Clear the write protection bit of the addressed group */
#define CMD29 29
#define CMD29_R R1B

/* Ask the card for the status of the write protection bits */
#define CMD30 30
#define CMD30_R R1

/***************************** �������� Erase commands *******************************/
/* ���ò��������ʼ��ַ Set the address of the first write block to be erased */
#define CMD32 32
#define CMD32_R R1

/* ���ò��������ֹ��ַ Set the address of the last write block to be erased */
#define CMD33 33
#define CMD33_R R1

/* ������ѡ��Ŀ� Erase the selected write blocks */
#define CMD38 38
#define CMD38_R R1B

/***************************** �������� Lock Card commands ***************************/
/* ����/��λ���������/������ Set/reset the password or lock/unlock the card */
#define CMD42 42
#define CMD42_R	R1B
/* Commands from 42 to 54, not defined here */

/***************************** Ӧ������ Application-specific commands ****************/
/* ��ֹ��һ������ΪӦ������  Flag that the next command is application-specific */
#define CMD55 55
#define CMD55_R R1

/* Ӧ�������ͨ��I/O  General purpose I/O for application-specific commands */
#define CMD56 56
#define CMD56_R R1

/* ��OCR�Ĵ���  Read the OCR (SPI mode only) */
#define CMD58 58
#define CMD58_R R3

/* ʹ�ܻ��ֹ CRC Turn CRC on or off */
#define CMD59 59
#define CMD59_R R1

/***************************** Ӧ������ Application-specific commands ***************/
/* ��ȡ SD Status�Ĵ��� Get the SD card's status */
#define ACMD13 13
#define ACMD13_R R2

/* �õ���д�뿨�еĿ�ĸ��� Get the number of written write blocks (Minus errors ) */
#define ACMD22 22
#define ACMD22_R R1

/* ��д֮ǰ,����Ԥ�Ȳ����Ŀ�ĸ��� Set the number of write blocks to be pre-erased before writing */
#define ACMD23 23
#define ACMD23_R R1

/* ��ȡOCR�Ĵ��� Get the card's OCR (SD mode) */
#define ACMD41 41
#define ACMD41_R R1

/* ����/�Ͽ�CD/DATA[3]�����ϵ��������� Connect or disconnect the 50kOhm internal pull-up on CD/DAT[3] */
#define ACMD42 42
#define ACMD42_R R1

/* ��ȡSCR�Ĵ��� Get the SD configuration register */
#define ACMD51 51
#define ACMD51_R R1

#endif
/**************************************************************************
 *                     LOCAL FUNTION PROTOTYPE
 */
#if SD_CMD_COMPLEX

#define SD_CMD_TIMEOUT   	 	100

#define SD_BLOCKSIZE 			512			/* SD����ĳ��� */

#define SD_BLOCKSIZE_NBITS		9  

/* SD����Ϣ�ṹ�嶨�� */ 
typedef struct SD_STRUCT
{	
  uint32 block_num;				/* ���п������ */
  uint32 block_len;				/* ���Ŀ鳤��(��λ:�ֽ�) */
  uint32 erase_unit;				/* һ�οɲ����Ŀ���� */
  
  uint32 timeout_read;			/* ���鳬ʱʱ��(��λ: 8 SPI clock) */
  uint32 timeout_write;			/* д�鳬ʱʱ��(��λ: 8 SPI clock) */
  uint32 timeout_erase;			/* ���鳬ʱʱ��(��λ: 8 SPI clock) */
	
}sd_struct;

extern sd_struct sds;

/* �ȴ����� Wait Type */
#define SD_WAIT_READ			  0x00		//���ȴ�
#define SD_WAIT_WRITE			  0x01		//д�ȴ�
#define SD_WAIT_ERASE		 	  0x02		//�����ȴ�

#define SD_READREG_TIMEOUT		  8

/* CSD��һЩ����ֽ�λ��(���ֽ���ǰ) */
#define TAAC_POS 			1			//TACC
#define NSAC_POS			2			//NSAC

#define READ_BL_LEN_POS		        5			//READ_BL_LEN

#define C_SIZE_POS1			6			//C_SIZE upper  2-bit
#define C_SIZE_POS2			7			//C_SIZE middle 8-bit
#define C_SIZE_POS3			8			//C_SIZE lower	2-bit

#define C_SIZE_MULT_POS1	        9			//C_SIZE_MULT upper 2-bit
#define C_SIZE_MULT_POS2	        10			//C_SIZE_MULT lower 1-bit	

#define SECTOR_SIZE_POS1	        10			//SECTOR_SIZE upper 5-bit
#define SECTOR_SIZE_POS2	        11			//SECTOR_SIZE lower 2-bit

#define R2WFACTOR_POS 		        12			//R2WFACTOR_POS

//CSD��һЩ�������
#define TAAC_MSK			0x07		//TACC ������
#define NSAC_MSK			0x78		//NSAC ������

#define READ_BL_LEN_MSK		        0x0F		//READ_BL_LEN ������

#define C_SIZE_MSK1			0x03		//C_SIZE ��2λ����
#define C_SIZE_MSK3			0xC0		//C_SIZE ��2λ����

#define C_SIZE_MULT_MSK1 	        0x03		//C_SIZE_MULT �ĸ�2λ����
#define C_SIZE_MULT_MSK2 	        0x80		//C_SIZE_MULT �ĵ�2λ����

#define R2WFACTOR_MSK		        0x1C		//R2WFACTOR ����

#define SECTOR_SIZE_MSK1	        0x3F		//SECTOR_SIZE �ĸ�5λ
#define SECTOR_SIZE_MSK2	        0x80		//SECTOR_SIZE �ĵ�2λ

/* �����ڳ�ʼ���׶�,�ȴ�SD���˳�����״̬�����Դ��� */
#define SD_IDLE_WAIT_MAX     	1000

/* R1��R2���ֽڴ����� R1 and upper byte of R2 error code */
#define MSK_IDLE          		  0x01
#define MSK_ERASE_RST     		  0x02
#define MSK_ILL_CMD       		  0x04
#define MSK_CRC_ERR       		  0x08
#define MSK_ERASE_SEQ_ERR 		  0x10
#define MSK_ADDR_ERR      		  0x20
#define MSK_PARAM_ERR     		  0x40

/* �������� Data Tokens */
#define SD_TOK_READ_STARTBLOCK    0xFE
#define SD_TOK_WRITE_STARTBLOCK   0xFE
#define SD_TOK_READ_STARTBLOCK_M  0xFE
#define SD_TOK_WRITE_STARTBLOCK_M 0xFC
#define SD_TOK_STOP_MULTI         0xFD

/* ������Ӧ���� Data Response Tokens */
#define SD_RESP_DATA_MSK		  0x0F		//������Ӧ����
#define SD_RESP_DATA_ACCETPTED	  0x05		//���ݱ�����
#define SD_RESP_DATA_REJECT_CRC	  0x0B      //����CRC��������ܾ�
#define SD_RESP_DATA_REJECT_WRITE 0x0D		//����д��������ܾ�

/* ������ error code */
#define   SD_NO_ERR			0x00			// ����ִ�гɹ�
#define   SD_ERR_NO_CARD		0x01			// SD��û����ȫ���뵽������
#define   SD_ERR_USER_PARAM      	0x02			// �û�ʹ��API����ʱ����ڲ����д���
#define   SD_ERR_CARD_PARAM		0x03			// ���в����д����뱾ģ�鲻���ݣ�
#define	  SD_ERR_VOL_NOTSUSP            0x04			// ����֧��3.3V����
#define   SD_ERR_OVER_CARDRANGE		0x05			// ����������������Χ

/* SD������ܷ��صĴ����� */
#define   SD_ERR_CMD_RESPTYPE	 	0x10			// �������ʹ���
#define   SD_ERR_CMD_TIMEOUT     	0x11			// SD������Ӧ��ʱ
#define   SD_ERR_CMD_RESP		0x12			// SD������Ӧ����
			
/* ������������ */
#define   SD_ERR_DATA_CRC16      	0x20			// ������CRC16У�鲻ͨ��
#define   SD_ERR_DATA_START_TOK		0x21			// ���������ʱ�����ݿ�ʼ���Ʋ���ȷ
#define	  SD_ERR_DATA_RESP		0x22			// д�������ʱ��SD��������Ӧ���Ʋ���ȷ

/* �ȴ������� */
#define   SD_ERR_TIMEOUT_WAIT    	0x30			// д�������ʱ��������ʱ����
#define   SD_ERR_TIMEOUT_READ    	0x31			// ��������ʱ����
#define	  SD_ERR_TIMEOUT_WRITE	 	0x32			// д������ʱ����
#define   SD_ERR_TIMEOUT_ERASE   	0x33			// ����������ʱ����
#define	  SD_ERR_TIMEOUT_WAITIDLE 	0x34			// ��ʼ��SD��ʱ���ȴ�SD���������״̬��ʱ����

/* д�������ܷ��صĴ����� */
#define	  SD_ERR_WRITE_BLK		0x40			// д�����ݴ���
#define	  SD_ERR_WRITE_BLKNUMS          0x41			// д���ʱ����Ҫд��Ŀ�����ȷд��Ŀ�����һ��
#define   SD_ERR_WRITE_PROTECT		0x42			// ����ǵ�д�������ش���д����λ��


extern uint8 halSD_Init(void);
extern uint8 hal_SendCmd( uint8 *param);
extern uint8 halSD_ActiveInit(void);
extern uint8 halSD_GetCardInfo( void);

extern uint8 hal_send_cmd_complex( uint8 cmd, uint8* param, uint8 resptype, uint8 *resp);

extern void halSD_PackParam( uint8 *parameter, uint32 value);
extern uint8 halSD_BlockCommand( uint8 cmd, uint8 resptype, uint32 parameter);
extern uint8 halSD_ResetSD( void);
extern uint8 halSD_ReadCSD( uint8 csdlen, uint8 *recbuf);
extern uint8 halSD_StopTransmission(void);
extern uint8 halSD_ReadCard_Status( uint8 len, uint8 *buffer);
extern uint8 halSD_SetBlockLen( uint32 length);
extern uint8 halSD_ReadSingleBlock( uint32 blockaddr);
extern uint8 halSD_ReadMultipleBlock( uint32 blockaddr);
extern uint8 halSD_WriteSingleBlock( uint32 blockaddr);
extern uint8 halSD_WriteMultipleBlock( uint32 blockaddr);
extern uint8 halSD_EraseStartBlock(uint32 startblock);
extern uint8 halSD_EraseEndBlock(uint32 startblock);
extern uint8 halSD_EraseSelectedBlock(void);
extern uint8 halSD_ReadOCR(uint8 ocrlen, uint8 *recbuf);
extern uint8 halSD_ReadSD_Status(uint8 sdslen, uint8 *recbuf);
extern uint8 halSD_ReadSCR(uint8 scrlen, uint8 *recbuf);
extern uint8 halSD_ReadRegister( uint32 len, uint8 *recbuf);
extern uint8 halSD_ReadBlockData(uint32 len, uint8 *recbuf);
extern uint8 halSD_WriteBlockData(uint8 bmulti, uint32 len, uint8 *sendbuf);
extern void halSD_StopMultiToken(void);
extern uint8 halSD_WaitBusy(uint8 waittype);
extern void halSD_SPIDelay(uint8 value);
#else
extern uint8 halWriteCmdSD(uint8 *CMD);

extern uint8 halSDInit( void);
extern uint8 halReadBlockSD(uint8 *CMD, uint8 *Buffer, uint16 Size);
extern uint8 halReadCID(uint8 *Buffer);
extern uint8 halReadCSD(uint8 *Buffer);
extern hal_SD_volume_info_t *halGetVolumeInfoSD( void);
extern uint8 halReadSectorSD(uint32 sector,uint8 *buffer);
extern void halGetDataSD(uint16 size,uint8 *buffer);
extern uint8 halWriteSectorSD(uint32 addr,uint8 *Buffer);
extern uint8 halStartReadSectorSD( uint32 sector);
extern void halGetData_LBA( uint32 lba, uint16 size, uint8* buffer);
extern void halGetSectorOffset( uint32 lba, uint16 offset);
extern void hal_LBA_close( void);

#endif  // SD_CMD_COMPLEX
#endif  // _SD_H_

#endif