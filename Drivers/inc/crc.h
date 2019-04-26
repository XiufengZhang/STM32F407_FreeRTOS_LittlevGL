#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern uint8_t CRC8_Array[256];
    extern uint16_t CRC16_Array[256];

    void CreateCRC8(uint8_t *pCRC, uint8_t Polynomial);
    void CreateCRC16(uint16_t *pCRC, uint16_t Polynomial);
    uint8_t GetCRC8(uint8_t *ptr, uint16_t len);
    uint16_t GetCRC16(uint8_t *ptr, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif