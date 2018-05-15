/*
 * RC522.c
 *
 *  Created on: 10 mar. 2018
 *      Author: Alejo & jcmaeso
 * 
 * Código basado en el codigo de el codido de Tilen Majerle, hecho para la placa de desarrollo
 * STM32F4 (https://stm32f4-discovery.net/2014/07/library-23-read-rfid-tag-mfrc522-stm32f4xx-devices/)
 * y en el codigo de Miguel Balboa hecho para arduino (https://github.com/miguelbalboa/rfid).
 * Hemos adaptado el codigo para funcionar en codigo c con la libreria BCM2835 creada por mikem
 * (http://www.airspayce.com/mikem/bcm2835/index.html) para raspberry.
 * 
 * Codigo bajo licencia GNU GPL
 * enjoy it!
 */

#include "RC522.h"

//private variables
const uint8_t antenna_Gain[] = {18, 23, 18, 23, 33, 38, 43, 48};
/**
 * @brief Inicia el driver de spi e inicializa los registros del soc RC522
 * 
 * @return Status_t resultado de la operación 
 */
Status_t RC522_Init(void)
{
	Status_t state = STATUS_OK;
#ifdef SPI_DEV
	printf("Setup: %d \n", wiringPiSPISetup(spidev0, DEFAULT_SPEED));

#endif
#ifdef bcm_spi
	state = bcm_spi_init();
#endif

	RC522_Reset();
	//These registers define the timer settings.
	RC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
	RC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E); //ftimer = 13.56Mhz / (2*TPreScaler+1)
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
	//configures the receiver gain
	RC522_WriteRegister(MFRC522_REG_RF_CFG, 0x70);
	//controls the setting of the transmission modulatio
	RC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40); //100% modulation

	RC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
	//turn on antenna
	RC522_AntennaOn();
	return state;
}

/**
 * @brief Comprueba si hay una tarjeta disponible
 * Comprueba si hay una tarjeta en el rango de deteccion. Para ello envia
 * el commando idle a la tajeta, si respode, la tajeta esta disponible.
 * @param id Puntero que contedrá el UUID de la terjeta
 * @return Status_t Estado del proceso
 */

Status_t RC522_Check(uint8_t *id)
{
	Status_t status;
	status = RC522_Request(PICC_REQIDL, id);
	if (status == STATUS_OK)
	{
		status = RC522_Anticoll(id);
	}
	RC522_Halt();

	return status;
}

/*I/O Functions*/
/**
 * @brief Escribe un registro del soc 
 * Escribe un registro del soc. Al usar SPI, las direcciones de los registros
 * pasan a ser de 7 bits, dejando el mas significativo para escritura(0)
 * @param addr Direccion del registro
 * @param val Datos a escribir
 */

void RC522_WriteRegister(uint8_t addr, uint8_t val)
{
	uint8_t buffer[2];

#ifdef SPI_DEV
	buffer[0] = (addr << 1) & 0x7E;
	buffer[1] = val;
	wiringPiSPIDataRW(spidev0, buffer, 2);
#endif
#ifdef bcm_spi
	buffer[0] = (addr << 1) & 0x7E; //mascara 0b01111111
	buffer[1] = val;
	bcm2835_spi_transfern(buffer, 2);
#endif
}

/**
 * @brief Escribe un registro del soc 
 * Escribe un registro del soc. Al usar SPI, las direcciones de los registros
 * pasan a ser de 7 bits, dejando el mas significativo para lectura(1)
 * @param addr Direccion del registro
 * @return uint8_t valor del registro leido
 */
uint8_t RC522_ReadRegister(uint8_t addr)
{
	uint8_t buffer[2];
	buffer[0] = ((addr << 1) & 0x7E) | 0x80; //mascara 0b111111111
	buffer[1] = 0x00;
#ifdef SPI_DEV
	wiringPiSPIDataRW(spidev0, buffer, 2);
#endif
#ifdef bcm_spi
	bcm2835_spi_transfern(buffer, 2);
#endif
	return buffer[1];
}

/**
 * @brief Aplica una máscara a un registro especificado
 * 
 * @param reg registro a enmascarar
 * @param mask mascara a aplicar
 */

void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) | mask);
}

/**
 * @brief Limpia una mascara de un registro especificado
 * 
 * @param reg registro a limpiar
 * @param mask mascara a deshacer
 */
void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) & (~mask));
}

/*Antenna functions*/
/**
 * @brief Enciende la antena
 * Activa la antena. Para ello comprueba que el bit de activacion esta desactivado
 * , si lo está, lo activa aplicando la mascara pertinente 
 * 
 */
void RC522_AntennaOn(void)
{
	uint8_t temp;

	temp = RC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if ((temp & 0x03) != 0X03)
	{													//si no esta ativa
		RC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03); //lo activa
	}
}
/**
 * @brief Apaga la antena
 * Apaga la antena limpiando el bit de activacion del registro
 * 
 */
void RC522_AntennaOff(void)
{
	RC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}
/**
 * @brief Lee la ganancia establecida para la antena
 * 
 * @return uint8_t valor de la gananacia discretizado, pueden ser {18, 23, 18, 23, 33,38,43,48} dBi
 */
uint8_t RC522_GetAntennaGain()
{
	uint8_t temp;
	temp = (RC522_ReadRegister(MFRC522_REG_RF_CFG) & 0x70) >> 4;
	return antenna_Gain[temp];
}

/**
 * @brief hace un soft-reset en el soc 
 * 
 */
void RC522_Reset(void)
{
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

/*Tag functions*/
/**
 * @brief Solicita un recurso especifico a la tarjeta
 * 
 * @param reqMode recurso a solicitar. ver rc522.h
 * @param TagType UUID de la tajeta
 * @return Status_t resultado de la funcion
 */
Status_t RC522_Request(uint8_t reqMode, uint8_t *TagType)
{
	Status_t status;
	uint16_t backBits; //The received data bits

	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07); //TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = RC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != STATUS_OK) || (backBits != 0x10))
	{ //hay respuesta y tenemos al menos 8b de respuesta
		status = STATUS_ERROR;
	}

	return status;
}

/**
 * @brief Envia datos a la tarjeta
 * 
 * @param command comando a ejecutar (autenticacion o trasmision)
 * @param sendData datos a enviar
 * @param sendLen tamaño de los datos
 * @param backData datos devueltos por la tarjeta
 * @param backLen numero de bits devueltos
 * @return Status_t estado de la operacion
 */
Status_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen)
{

	Status_t status = STATUS_ERROR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;

	uint8_t n;
	uint16_t i;
	//determina el valor de los registros irqEn (interrupcion) y waitIRq (tiempo de espera a IRQ)
	//en funcion del tipo de comando
	switch (command)
	{
	case PCD_AUTHENT:
	{
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case PCD_TRANSCEIVE:
	{
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}
	//escribo y limpio los registros necesarios
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	RC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	RC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	RC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80); //indice de cola maximo

	//envio los datos a la cola
	for (i = 0; i < sendLen; i++)
	{
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}

	//Execute the command (trasmito los datos)
	RC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE)
	{
		RC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80); //StartSend=1,transmission of data starts
	}
	//espero a que la trasmision se complete
	i = 2000; //i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do
	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = RC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

	RC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80); //StartSend=0

	//leo los datos recibidos
	if (i != 0)
	{
		if (!(RC522_ReadRegister(MFRC522_REG_ERROR) & 0x13))
		{ //si no ha habido errores
			status = STATUS_OK;

			if (command == PCD_TRANSCEIVE)
			{
				n = RC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);			   //leo el indice superior de la cola de datos
				lastBits = RC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07; //leo el indice de bytes leidos
				if (lastBits)
				{ //determino el numero de bits recibidos
					*backLen = (n - 1) * 8 + lastBits;
				}
				else
				{
					*backLen = n * 8;
				}

				if (n == 0)
				{
					n = 1;
				}
				if (n > MFRC522_MAX_LEN)
				{
					n = MFRC522_MAX_LEN;
				}

				for (i = 0; i < n; i++)
				{ //leo los bytes recibidos
					backData[i] = RC522_ReadRegister(MFRC522_REG_FIFO_DATA);
				}
			}
		}
		else
		{
			status = STATUS_ERROR;
		}
	}

	return status;
}

/**
 * @brief Protocolo anticolicion
 * Ejecuta el comando anticolicion establecido en la norma 14443.
 * Los UUID devueltos tienen  8 bytes (no siempre, leer iso), el ultimo
 * byte es un XOR de los anteriores. Si hay una colision, este valor de la operacion
 * no coincidira con los datos recibidos
 * @param serNum puntero que almacenará el UUID
 * @return Status_t estado de la operación
 */
Status_t RC522_Anticoll(uint8_t *serNum)
{
	Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00); //TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == STATUS_OK)
	{
		//Check card serial number
		for (i = 0; i < 4; i++)
		{
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			status = STATUS_ERROR;
		}
	}
	return status;
}
/**
 * @brief Calcula la operacion de comprobacionde errores
 * 
 * @param pIndata datos a comprobar
 * @param len longitud del crc
 * @param pOutData resultado de la operación 
 */

void RC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;

	RC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);  //CRCIrq = 0
	RC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80); //Clear the FIFO pointer
	//Write_MFRC522(CommandReg, PCD_IDLE);

	//Writing data to the FIFO
	for (i = 0; i < len; i++)
	{
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata + i));
	}
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	//Wait CRC calculation is complete
	i = 0xFF;
	do
	{
		n = RC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i != 0) && !(n & 0x04)); //CRCIrq = 1

	//Read CRC calculation result
	pOutData[0] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}
/**
 * @brief Selecciona una tarjeta para trabajar
 * Conocida la UUID de la tarjeta, se envia el comando de seleccion a la tarjeta
 * para poder operar con ella.
 * @param serNum UUID de la tarjeta a seleccionar
 * @return uint8_t numero de bytes de respuesta
 */

uint8_t RC522_SelectTag(uint8_t *serNum)
{
	uint8_t i;
	Status_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9];

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++)
	{
		buffer[i + 2] = *(serNum + i);
	}
	RC522_CalculateCRC(buffer, 7, &buffer[7]);
	status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == STATUS_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}
	else
	{
		size = 0;
	}

	return size;
}
/**
 * @brief Ejecuta el comando de autenticacion 
 * Esto permite acceder la memoria de las tarjetas cifradas
 * @param authMode tipo de autenticacion (WUPA, WUPB), en funcion de la tarjeta (ver iso 14443-3)
 * @param BlockAddr bloque de memoria a escribir
 * @param Sectorkey sector a escribir
 * @param serNum UUID de la tarjeta
 * @return Status_t  resultado de la operacion
 */
Status_t RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
	Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12];

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++)
	{
		buff[i + 2] = *(Sectorkey + i);
	}
	for (i = 0; i < 4; i++)
	{
		buff[i + 8] = *(serNum + i);
	}
	status = RC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != STATUS_OK) || (!(RC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08)))
	{
		status = STATUS_ERROR;
	}

	return status;
}
/**
 * @brief Lee un bloque de memoria de la tarjeta
 * Las tarjetas MiraFare classic (las que disponemos) tienen 1k de memoria organizada en 16 sectores de 4 bloques.
 * Cada sector tiene 16 bytes. Esta funcion lee el bloque de  memoria especificado
 * @param blockAddr numero de bloque de memoria
 * @param recvData datos leidos (16 bytes)
 * @return Status_t  resultado de la operacion 
 */
Status_t RC522_Read(uint8_t blockAddr, uint8_t *recvData)
{
	Status_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	RC522_CalculateCRC(recvData, 2, &recvData[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != STATUS_OK) || (unLen != 0x90))
	{
		status = STATUS_ERROR;
	}

	return status;
}

Status_t RC522_Write(uint8_t blockAddr, uint8_t *writeData)
{
	Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	RC522_CalculateCRC(buff, 2, &buff[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != STATUS_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = STATUS_ERROR;
	}

	if (status == STATUS_OK)
	{
		for (i = 0; i < 16; i++)
		{
			buff[i] = *(writeData + i);
		}
		RC522_CalculateCRC(buff, 16, &buff[16]);
		status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != STATUS_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = STATUS_ERROR;
		}
	}

	return status;
}

void RC522_Halt(void)
{
	uint16_t unLen;
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	RC522_CalculateCRC(buff, 2, &buff[2]);

	RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}
/**
 * @brief Inicializa el puerto SPI de la raspberry
 * 
 * @return Status_t resultado de la operacion 
 */
Status_t bcm_spi_init()
{
	if (!bcm2835_init())
	{
		printf("failed!. Are you root??\n");
		return STATUS_ERROR;
	}
	if (!bcm2835_spi_begin())
	{
		printf("failed! Are you root??\n");
		return STATUS_ERROR;
	}
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);   //  default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);				   // Sets the clock polariy and phase. CPOL = 0, CPHA = 0
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); // 64 = 3.90625MHz on Rpi2, 6.250MHz on RPI3
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);				   //  default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);   //  default
	return STATUS_OK;
}
