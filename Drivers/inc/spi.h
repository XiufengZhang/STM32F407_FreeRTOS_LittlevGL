#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

    void SPI1_Init(void);
    void SPI2_Init(void);
    ErrorStatus SPI1_SendData(uint8_t SendData);  //SPI总线写一个字节
    ErrorStatus SPI1_ReadData(uint8_t *ReadData); //SPI总线读写一个字节
    ErrorStatus SPI2_SendData(uint8_t SendData);  //SPI总线写一个字节
    ErrorStatus SPI2_ReadData(uint8_t *ReadData); //SPI总线读写一个字节

#ifdef __cplusplus
}
#endif

#endif