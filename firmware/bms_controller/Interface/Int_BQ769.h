#ifndef __INT_BQ769__
#define __INT_BQ769__

#include "i2c.h"
#include "Com_debug.h"
#include "Inf_BQ769_BSP.h"
#include "Com_BQ769.h"
#include "FreeRTOS.h"
#include "task.h"
#define BQ769_I2C_ADDRESS 0x08
#define BQ769_I2C_ADDRESS_WRITE 0x10
#define BQ769_I2C_ADDRESS_READ 0x11

// 三元锂电池 的电压值 V
#define TLB_OV_PROTECT 4.25       // 单体过压保护电压
#define TLB_OV_RELIEVE 4.10       // 单体过压恢复电压
#define TLB_UV_PROTECT 2.80       // 单体欠压保护电压
#define TLB_UV_RELIEVE 3.00       // 单体欠压恢复电压
#define TLB_SHUTDOWN_VOLTAGE 2.80 // 自动关机电压
#define TLB_BALANCE_VOLTAGE 3.30  // 均衡起始电压

/**
 * @brief 初始化BQ769芯片
 *
 */
void Int_BQ769_Init(void);

/**
 * @brief 读取BQ769芯片的寄存器
 *
 * @param reg 寄存器地址
 * @param buf 存放读取数据
 * @param len 读取长度
 * @return uint8_t 0:成功 1:失败
 */
uint8_t Int_BQ769_Read_Register(uint8_t reg, uint8_t *buf, uint8_t len);

/**
 * @brief 读取单节电池的电压
 *
 * @param cell_num 电池的编号  1-5
 * @return float 电池的电压
 */
float Int_BQ769_Get_Cell_Voltage(uint8_t cell_num);

/**
 * @brief 读取电池的总电压
 *
 * @return float 电压值
 */
float Int_BQ769_Get_Bat_Voltage(void);

/**
 * @brief 读取输出电路的电流
 *
 * @return float
 */
float Int_BQ769_Get_Bat_Current(void);

/**
 * @brief 获取电池的温度
 *
 * @return float
 */
float Int_BQ769_Get_Bat_temp(void);

/**
 * @brief 控制放电开关
 *
 */
void Int_BQ769_discharge(uint8_t enable);

/**
 * @brief 控制充电开关
 *
 */
void Int_BQ769_charge(uint8_t enable);

#endif // __INT_BQ769__
