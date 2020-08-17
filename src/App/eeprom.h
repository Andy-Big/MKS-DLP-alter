#ifndef __eeprom_H
#define __eeprom_H

#include "includes.h"
#include "main.h"


extern I2C_HandleTypeDef		hI2C;

// SR2
#define _I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define _I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define _I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define _I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define _I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define _I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define _I2C_FLAG_MSL                    ((uint32_t)0x00010000)

// SR1
#define _I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)
#define _I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)
#define _I2C_FLAG_PECERR                 ((uint32_t)0x10001000)
#define _I2C_FLAG_OVR                    ((uint32_t)0x10000800)
#define _I2C_FLAG_AF                     ((uint32_t)0x10000400)
#define _I2C_FLAG_ARLO                   ((uint32_t)0x10000200)
#define _I2C_FLAG_BERR                   ((uint32_t)0x10000100)
#define _I2C_FLAG_TXE                    ((uint32_t)0x10000080)
#define _I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define _I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define _I2C_FLAG_ADD10                  ((uint32_t)0x10000008)
#define _I2C_FLAG_BTF                    ((uint32_t)0x10000004)
#define _I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define _I2C_FLAG_SB                     ((uint32_t)0x10000001)

#define I2C_EVENT_MASTER_MODE_SELECT					((uint32_t)0x00030001)
#define I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED		((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED			((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
#define I2C_EVENT_MASTER_BYTE_RECEIVED					((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING				((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTED				((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */



void		EEPROM_Init(void);
uint32_t 	_EEPROM_get_flag_status(uint32_t flag);
uint32_t 	_EEPROM_check_errors();
uint8_t		_EEPROM_CheckEvent(uint32_t I2C_EVENT);
void		_EEPROM_Send7bitAddress(uint8_t Address, uint8_t I2C_Direction);

void 		_EEPROM_abort();
uint8_t 	_EEPROM_generate_start();
uint8_t 	_EEPROM_generate_stop();
uint8_t 	_EEPROM_send_addr_send(uint8_t addr);
uint8_t 	_EEPROM_send_addr_receive(uint8_t addr);
uint8_t 	_EEPROM_send_data(uint8_t addr, uint8_t reg);

uint8_t 	EEPROM_CheckRead(uint8_t addr, uint8_t reg, uint8_t *result = 0);

uint8_t		EEPROM_ChangeSpeed(uint32_t spd);

uint8_t		EEPROM_WriteReg(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t 	EEPROM_ReadReg(uint8_t addr, uint8_t reg);

uint8_t		EEPROM_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint16_t count);
uint8_t		EEPROM_WriteBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint16_t count);


	

#endif /*__eeprom_H */

