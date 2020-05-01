#ifndef  __HUMID_H
#define  __HUMID_H


#ifdef __cplusplus
 extern "C" {
#endif

#define HUMID_Address            0x40           //ʹ��7λ��ַ 8λ��ַ��0x80 SHT20 HTU21D
#define HUMIDTempReg_Address     0xF3
#define HUMIDHumiReg_Address     0xF5
#define HUMIDUserRReg_Address     0xE7
#define HUMIDUserWReg_Address     0xE6

extern ErrorStatus HUMIDStatus;// �����ʪ��IC�Ƿ�ͨѶ����
extern uint16_t HUMID_Temp;
extern uint16_t HUMID_Humi;
extern float HUMID_TempConvert;
extern float HUMID_HumiConvert;

ErrorStatus HUMID_Init(uint8_t Resolution);
ErrorStatus HUMID_ReadTemp(void);
ErrorStatus HUMID_ReadHumi(void);

#ifdef __cplusplus
}
#endif

#endif