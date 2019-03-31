#ifndef __SSDLCD_H
#define __SSDLCD_H

#ifdef __cplusplus
extern "C"
{
#endif

//#define TFT43IN480272PIX
//#define TFT50IN480272PIX
#define TFT50IN800480PIX
// #define TFT70IN800480PIX

#define SSDLCD_COMMAND ((uint32_t)0x6C000000) //BANK1 A23数据线 0x60000000低电平写指令 BANK4 A18数据线0x6C000000
#define SSDLCD_DATA ((uint32_t)0x6C080000)    //BANK1 A23数据线 0x61000000高电平读写数据 BANK4 A18数据线0x6C080000

#define SSD_REFERENCE_CLOCK 10 //外部晶振频率10Mhz，或者8Mhz

#define SSD_soft_reset 0x01               //软复位
#define SSD_get_address_mode 0x0B         //读取ARM地址模式
#define SSD_set_address_mode 0x36         //设置ARM地址模式
#define SSD_set_display_on 0x29           //开启显示
#define SSD_read_ddb 0xA1                 //读取产品描述信息
#define SSD_set_lcd_mode 0xB0             //设置LCD模式
#define SSD_set_hori_period 0xB4          //设置LCD水平驱动参数
#define SSD_set_vert_period 0xB6          //设置LCD垂直驱动参数
#define SSD_set_gpio_conf 0xB8            //GPIO设置
#define SSD_set_gpio_value 0xBA           //GPIO0输出值设置
#define SSD_set_dbc_conf 0xD0             //设置LCD自动白平衡
#define SSD_set_pll 0xE0                  //启动PLL
#define SSD_set_pll_mn 0xE2               //设置时钟频率
#define SSD_get_pll_mn 0xE3               //读取时钟频率
#define SSD_set_lshift_freq 0xE6          //设置像素时钟频率
#define SSD_get_lshift_freq 0xE7          //读取像素时钟频率
#define SSD_set_pixel_data_interface 0xF0 //设置像素格式
#define SSD_get_pixel_data_interface 0xF1 //读取像素格式

//LCD分辨率设置
#ifdef TFT43IN480272PIX
#define SSD_HOR_RESOLUTION 480 //LCD水平分辨率
#define SSD_VER_RESOLUTION 272 //LCD垂直分辨率
//LCD驱动参数设置,每个TFTLCD厂家不同
#define SSD_REFERENCE_DCLK 12 //DCLK Frequency 福瑞达
#define SSD_HOR_PULSE_WIDTH 1 //水平脉宽 行同步信号宽度
#define SSD_HOR_BACK_PORCH 43 //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
#define SSD_HOR_FRONT_PORCH 2 //水平前廊
#define SSD_VER_PULSE_WIDTH 1 //垂直脉宽 垂直同步信号宽度
#define SSD_VER_BACK_PORCH 12 //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
#define SSD_VER_FRONT_PORCH 1 //垂直前廊
#elif defined TFT50IN480272PIX
#define SSD_HOR_RESOLUTION 480 //LCD水平分辨率
#define SSD_VER_RESOLUTION 272 //LCD垂直分辨率
//LCD驱动参数设置,每个TFTLCD厂家不同
// #define SSD_REFERENCE_DCLK 9   //DCLK Frequency 海创通科技 福瑞达 三元晶
// #define SSD_HOR_PULSE_WIDTH 4  //水平脉宽 行同步信号宽度
// #define SSD_HOR_BACK_PORCH 40  //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
// #define SSD_HOR_FRONT_PORCH 4  //水平前廊
// #define SSD_VER_PULSE_WIDTH 3  //垂直脉宽 垂直同步信号宽度
// #define SSD_VER_BACK_PORCH 5   //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
// #define SSD_VER_FRONT_PORCH 2  //垂直前廊
#define SSD_REFERENCE_DCLK 9   //DCLK Frequency 佳显电子
#define SSD_HOR_PULSE_WIDTH 2  //水平脉宽 行同步信号宽度
#define SSD_HOR_BACK_PORCH 4   //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
#define SSD_HOR_FRONT_PORCH 2  //水平前廊
#define SSD_VER_PULSE_WIDTH 1  //垂直脉宽 垂直同步信号宽度
#define SSD_VER_BACK_PORCH 2   //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
#define SSD_VER_FRONT_PORCH 1  //垂直前廊
#elif defined TFT50IN800480PIX
#define SSD_HOR_RESOLUTION 800 //LCD水平分辨率
#define SSD_VER_RESOLUTION 480 //LCD垂直分辨率
//LCD驱动参数设置,每个TFTLCD厂家不同
// #define SSD_REFERENCE_DCLK 33  //DCLK Frequency 海创通科技
// #define SSD_HOR_PULSE_WIDTH 1  //水平脉宽 行同步信号宽度
// #define SSD_HOR_BACK_PORCH 46  //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
// #define SSD_HOR_FRONT_PORCH 16 //水平前廊
// #define SSD_VER_PULSE_WIDTH 1  //垂直脉宽 垂直同步信号宽度
// #define SSD_VER_BACK_PORCH 23  //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
// #define SSD_VER_FRONT_PORCH 7  //垂直前廊
// #define SSD_REFERENCE_DCLK 37  //DCLK Frequency 大显1
// #define SSD_HOR_PULSE_WIDTH 3  //水平脉宽 行同步信号宽度
// #define SSD_HOR_BACK_PORCH 40  //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
// #define SSD_HOR_FRONT_PORCH 40 //水平前廊
// #define SSD_VER_PULSE_WIDTH 3  //垂直脉宽 垂直同步信号宽度
// #define SSD_VER_BACK_PORCH 29  //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
// #define SSD_VER_FRONT_PORCH 13 //垂直前廊
#define SSD_REFERENCE_DCLK 37  //DCLK Frequency 大显2
#define SSD_HOR_PULSE_WIDTH 3  //水平脉宽 行同步信号宽度
#define SSD_HOR_BACK_PORCH 100 //水平后廊 包含同步信号宽度 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
#define SSD_HOR_FRONT_PORCH 6  //水平前廊
#define SSD_VER_PULSE_WIDTH 3  //垂直脉宽 垂直同步信号宽度
#define SSD_VER_BACK_PORCH 20  //垂直后廊 包含同步信号宽度 垂直同步信号后，无效行的个数
#define SSD_VER_FRONT_PORCH 6  //垂直前廊
#elif defined TFT70IN800480PIX
#define SSD_HOR_RESOLUTION 800 //LCD水平分辨率
#define SSD_VER_RESOLUTION 480 //LCD垂直分辨率
//LCD驱动参数设置,每个TFTLCD厂家不同
#define SSD_REFERENCE_DCLK 33  //DCLK Frequency 思坦德 群创光电
#define SSD_HOR_PULSE_WIDTH 1  //水平脉宽 行同步信号宽度
#define SSD_HOR_BACK_PORCH 46  //水平后廊 水平同步信号开始到行有效数据开始之间的相素时钟（CLK）个数
#define SSD_HOR_FRONT_PORCH 16 //水平前廊
#define SSD_VER_PULSE_WIDTH 1  //垂直脉宽 垂直同步信号宽度
#define SSD_VER_BACK_PORCH 23  //垂直后廊 垂直同步信号后，无效行的个数
#define SSD_VER_FRONT_PORCH 7  //垂直前廊
#else
#error "The TFT LCD type has not been defined"
#endif

#define SSD_HT (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH - 1) //horizontal total period
#define SSD_HPS (SSD_HOR_BACK_PORCH)                                               //Horizontal Sync Pulse Start Position
#define SSD_HPW (SSD_HOR_PULSE_WIDTH - 1)                                          //Horizontal Sync Pulse Width
#define SSD_LPS (0)                                                                //Horizontal Display Period Start Position

#define SSD_VT (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH - 1) //vertical total period
#define SSD_VPS (SSD_VER_BACK_PORCH)                                               //HVertical Sync Pulse Start Position
#define SSD_VPW (SSD_VER_PULSE_WIDTH - 1)                                          //Vertical Sync Pulse Width
#define SSD_FPS (0)                                                                //Vertical Display Period Start Position

    extern ErrorStatus SSDLCDStatus; //标记SSDLCD驱动IC初始化是否正常 0失败 1正常

    void SSDFSMC_Init(uint8_t HighStatus);
    void SSDLCD_Clear(uint16_t Color);
    ErrorStatus SSDLCD_Init(void);

#ifdef __cplusplus
}
#endif

#endif