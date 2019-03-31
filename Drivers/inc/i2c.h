#ifndef  __I2C_H
#define  __I2C_H

#define SCL_GPIO        GPIOB
#define SCL_Pin         GPIO_PIN_4
#define SDA_GPIO        GPIOB
#define SDA_Pin         GPIO_PIN_5

void IIC_Start(void);
void IIC_Stop(void);
ErrorStatus IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t data);
uint8_t IIC_Read_Byte(void);

ErrorStatus I2CWriteByte(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pWriteData);
ErrorStatus I2CWriteData(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pWriteDataBuffer, uint8_t WriteDataCount);
ErrorStatus I2CReadByte(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pReadData);
ErrorStatus I2CReadData(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pReadDataBuffer, uint8_t ReadDataCount);

#endif