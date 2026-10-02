#include "Int_BQ769.h"

// 静态存储增益值  偏移值
// 单位是uV
static uint16_t BQ769_Gain;
static float BQ769_Gain_mV;
static int8_t BQ769_Offset;

// 声明一个寄存器组 => 包含所有的BQ芯片寄存器
static RegisterGroup BQ769_RegisterGroup;

// 测量电流引脚的电阻值 单位是毫欧姆
#define BQ_R 5

uint8_t BQ769x0_CRC8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00;       // 初始值固定为0
    const uint8_t key = 0x07; // 文档强制指定!
    while (len--)
    {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ key; // key=0x07
            else
                crc <<= 1;
        }
    }
    return crc;
}

/**
 * @brief 如果写入1个字节 => CRC循环冗余校验 包含的内容是
 * 从设备地址 + 寄存器地址 + 写入的数据
 * 1byte + 1crc
 * 如果写入的是多个字节 => CRC循环冗余校验 包含的内容是从设备地址 和 写入的数据
 *
 * 1byte + 1crc + 1byte  + 1crc
 *
 * @param reg
 * @param data
 */
void Int_BQ769_WriteReg(uint8_t reg, uint8_t data)
{
    // BQ芯片读写需要进行CRC校验
    // HAL_I2C_Mem_Write(&hi2c2, BQ769_I2C_ADDRESS_WRITE, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);

    // 1. 计算CRC
    uint8_t buf[4] = {0};
    buf[0] = BQ769_I2C_ADDRESS_WRITE;
    buf[1] = reg;
    buf[2] = data;
    uint8_t crc = BQ769x0_CRC8(buf, 3);
    buf[3] = crc;
    // 2. 使用I2C写入芯片
    // 进入临界区
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Write(&hi2c2, BQ769_I2C_ADDRESS_WRITE, reg, I2C_MEMADD_SIZE_8BIT, buf + 2, 2, 1000);
    // 退出临界区
    taskEXIT_CRITICAL();
}

// 用于BQ芯片读取寄存器的时候  帮助缓存CRC校验的值
uint8_t BQ769_BUFF[128] = {0};

/**
 * @brief 读取BQ769芯片的寄存器
 *
 * @param reg 寄存器地址
 * @param buf 存放读取数据
 * @param len 读取长度
 * @return uint8_t 0:成功 1:失败
 */
uint8_t Int_BQ769_Read_Register(uint8_t reg, uint8_t *buf, uint8_t len)
{
    // 1. 读取寄存器的内容 => 包含CRC的内容
    // 进入临界区
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Read(&hi2c2, BQ769_I2C_ADDRESS_READ, reg, I2C_MEMADD_SIZE_8BIT, BQ769_BUFF, len * 2, 1000);
    // 退出临界区
    taskEXIT_CRITICAL();
    // 2. 进行CRC校验  => 挑选第一个字节的CRC进行校验
    // BQ769_BUFF[0] => 寄存器的数据  BQ769_BUFF[1] => CRC的值
    uint8_t tmp[2] = {0};
    tmp[0] = BQ769_I2C_ADDRESS_READ;
    tmp[1] = BQ769_BUFF[0];
    uint8_t crc = BQ769x0_CRC8(tmp, 2);

    if (crc != BQ769_BUFF[1])
    {
        // CRC校验失败
        debug_printf("CRC校验失败\r\n");
        return 1;
    }

    // 3. CRC校验成功 将数据读取返回
    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = BQ769_BUFF[i * 2];
    }

    return 0;
}

void Int_BQ769_WKUP(void)
{
    HAL_GPIO_WritePin(BQ_WKUP_GPIO_Port, BQ_WKUP_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(BQ_WKUP_GPIO_Port, BQ_WKUP_Pin, GPIO_PIN_RESET);
    // 应该较长时间等待 => 等待芯片重启功能完成
    HAL_Delay(1000);
}

void Int_BQ769_Read_Gain_offset(void)
{
    // 3.读取只读寄存器  增益值
    uint8_t gain1 = 0;
    uint8_t gain2 = 0;
    Int_BQ769_Read_Register(BQ_ADCGAIN1, &gain1, 1);
    Int_BQ769_Read_Register(BQ_ADCGAIN2, &gain2, 1);
    // 148  163
    debug_printf("gain1:%d\r\n", gain1);
    debug_printf("gain2:%d\r\n", gain2);

    // 4. 计算增益值
    BQ769_Gain = 365 + (((gain1 & 0x0C) << 1) | (gain2 & 0xE0) >> 5);
    BQ769_Gain_mV = BQ769_Gain / 1000.0;
    debug_printf("BQ769_Gain:%d\r\n", BQ769_Gain);

    // 5. 读取偏移值
    Int_BQ769_Read_Register(BQ_ADCOFFSET, (uint8_t *)&BQ769_Offset, 1);
    debug_printf("BQ769_Offset:%d\r\n", BQ769_Offset);
}

/**
 * @brief 初始化BQ769芯片
 *
 */
void Int_BQ769_Init(void)
{
    // 1. 安全启动进行船模式
    Int_BQ769_WriteReg(0x04, 0x00);
    Int_BQ769_WriteReg(0x04, 0x01);
    Int_BQ769_WriteReg(0x04, 0x02);
    HAL_Delay(1000);

    // 2. 唤醒芯片
    Int_BQ769_WKUP();

    // 3. 读取增益值和偏移值
    Int_BQ769_Read_Gain_offset();

    // 4. 配置sysctrl1寄存器为0x18 => 打开ADC转换 选择温度测量是外部的电池盒
    BQ769_RegisterGroup.SysCtrl1.SysCtrl1Byte = 0X18;

    // 5. 配置sysctrl2寄存器为0x43 => 连续进行ADC转换测量 同时打开充放电开关
    BQ769_RegisterGroup.SysCtrl2.SysCtrl2Byte = 0x43;

    // 6. 配置保护寄存器1
    BQ769_RegisterGroup.Protect1.Protect1Bit.SCD_DELAY = BMS_SCD_DELAY_400us;
    BQ769_RegisterGroup.Protect1.Protect1Bit.SCD_THRESH = 0X07;
    // 7. 配置保护寄存器2
    BQ769_RegisterGroup.Protect2.Protect2Bit.OCD_DELAY = BMS_OCD_DELAY_1280ms;
    BQ769_RegisterGroup.Protect2.Protect2Bit.OCD_THRESH = 0X0F;
    // 8. 配置保护寄存器3
    BQ769_RegisterGroup.Protect3.Protect3Bit.OV_DELAY = BMS_OV_DELAY_2s;
    BQ769_RegisterGroup.Protect3.Protect3Bit.UV_DELAY = BMS_UV_DELAY_4s;

    // 9. 设置过压和欠压的值
    // 默认OV => 0xAC => 对应14位ADC表示电压的值是10952 => 对于过压的值是4.183V
    // 0xAC => 拼接14位ADC的值 10952  => 10952 * gain(378) / 1000 + offset(44)
    // 实际开发需要根据购买的电池的电压情况 设置对应的过电压和欠电压保护的值
    // 设置过压保护4.25V => 填写值到OV寄存器中
    // (4.25 * 1000 - offset(44)) / mV_gain(0.378) => 14位ADC => 中间8位
    BQ769_RegisterGroup.OVTrip = (((uint16_t)((TLB_OV_PROTECT * 1000 - BQ769_Offset) / BQ769_Gain_mV)) >> 4) & 0xFF;

    BQ769_RegisterGroup.UVTrip = (((uint16_t)((TLB_UV_PROTECT * 1000 - BQ769_Offset) / BQ769_Gain_mV)) >> 4) & 0xFF;
    debug_printf("TLB_OV_PROTECT:%d\r\n", BQ769_RegisterGroup.UVTrip);
    debug_printf("TLB_UV_PROTECT:%d\r\n", BQ769_RegisterGroup.OVTrip);

    // 10. 配置库伦计数器
    BQ769_RegisterGroup.CCCfg = 0x19;

    // 11. 将数值写入到芯片中
    Int_BQ769_WriteReg(BQ_SYS_CTRL1, BQ769_RegisterGroup.SysCtrl1.SysCtrl1Byte);
    Int_BQ769_WriteReg(BQ_SYS_CTRL2, BQ769_RegisterGroup.SysCtrl2.SysCtrl2Byte);
    Int_BQ769_WriteReg(BQ_PROTECT1, BQ769_RegisterGroup.Protect1.Protect1Byte);
    Int_BQ769_WriteReg(BQ_PROTECT2, BQ769_RegisterGroup.Protect2.Protect2Byte);
    Int_BQ769_WriteReg(BQ_PROTECT3, BQ769_RegisterGroup.Protect3.Protect3Byte);
    Int_BQ769_WriteReg(BQ_OV_TRIP, BQ769_RegisterGroup.OVTrip);
    Int_BQ769_WriteReg(BQ_UV_TRIP, BQ769_RegisterGroup.UVTrip);
    Int_BQ769_WriteReg(BQ_CC_CFG, BQ769_RegisterGroup.CCCfg);

    uint8_t bq_data[8] = {0};
    Int_BQ769_Read_Register(BQ_SYS_CTRL1, bq_data, 8);

    debug_printf("sysctrl1:%02x", bq_data[0]);
    debug_printf("sysctrl2:%02x", bq_data[1]);
    debug_printf("protect1:%02x", bq_data[2]);
    debug_printf("protect2:%02x", bq_data[3]);
    debug_printf("protect3:%02x", bq_data[4]);
    debug_printf("ov_trip:%02x", bq_data[5]);
    debug_printf("uv_trip:%02x", bq_data[6]);
    debug_printf("cc_cfg:%02x", bq_data[7]);
    debug_printf("初始化完成");
}

/**
 * @brief 读取单节电池的电压
 *
 * @param cell_num 电池的编号  1-5
 * @return float 电池的电压
 */
float Int_BQ769_Get_Cell_Voltage(uint8_t cell_num)
{
    if (cell_num < 1 || cell_num > 5)
    {
        debug_printf("cell_num_error:%d\r\n", cell_num);
        return 0.0;
    }

    // 1. 根据电池的编号找到正确的寄存器地址
    uint8_t tmp[2] = {0};
    uint8_t reg_addr = BQ_VC1_HI + (cell_num - 1) * 2;

    // 2. 读取两个寄存器的值
    Int_BQ769_Read_Register(reg_addr, tmp, 2);

    // 3. 拼接得到ADC的值
    uint16_t adc = ((tmp[0] & 0x3F) << 8) | tmp[1];

    // 4. 计算电压值 V(cell) = GAIN x ADC(cell) + OFFSET
    float voltage = (BQ769_Gain_mV * adc + BQ769_Offset) / 1000.0;

    return voltage;
}

/**
 * @brief 读取电池的总电压
 *
 * @return float 电压值
 */
float Int_BQ769_Get_Bat_Voltage(void)
{
    // 因为芯片最多能够容纳15节电池 => 76920芯片最多是5节电池 总电压会超出16位ADC的测量范围
    // 所以寄存器表示的是1/4的总电压
    // V(BAT) = 4 × GAIN × ADC(cell) + (#Cells x OFFSET)
    // 1. 读取寄存器的值
    uint8_t tmp[2] = {0};
    Int_BQ769_Read_Register(BQ_BAT_HI, tmp, 2);
    uint16_t adc = ((tmp[0]) << 8) | tmp[1];
    float voltage = (4 * BQ769_Gain_mV * adc + 5 * BQ769_Offset) / 1000.0;
    return voltage;
}

/**
 * @brief 读取输出电路的电流
 *
 * @return float
 */
float Int_BQ769_Get_Bat_Current(void)
{
    uint8_t tmp[2] = {0};
    // 1. 读取库伦计数器的值
    Int_BQ769_Read_Register(BQ_CC_HI, tmp, 2);
    // 2. 拼接16位的ADC值
    uint16_t adc = ((tmp[0]) << 8) | tmp[1];
    // 3. 计算两个引脚直接的电压值 => 单位 uV => mV
    float vol = ((int16_t)adc) * 8.44 / 1000.0;
    debug_printf("vol:%f\r\n", vol);
    // 4. 计算电流值  mV / mΩ = A
    float current = vol / 5.0;
    return current;
}

/**
 * @brief 获取电池的温度
 *
 * @return float
 */
float Int_BQ769_Get_Bat_temp(void)
{
    // 1. 读取TS1寄存器的值
    uint8_t tmp[2] = {0};
    Int_BQ769_Read_Register(BQ_TS1_HI, tmp, 2);
    // 2. 拼接14位的ADC值
    uint16_t adc = ((tmp[0] & 0x3F) << 8) | tmp[1];
    // 3. 通过芯片手册换算当前热敏电阻的电压值 换算单位是mV
    float voltage = (382 * adc) / 1000.0;
    // 4. 计算电池的温度值
    float temp = Com_BQ769_TempChange((uint16_t)voltage) / 10;
    return temp;
}

/**
 * @brief 控制放电开关
 *
 */
void Int_BQ769_discharge(uint8_t enable)
{
    if (enable)
    {
        // 开启放电
        BQ769_RegisterGroup.SysCtrl2.SysCtrl2Bit.DSG_ON = 1;
    }
    else
    {
        // 关闭放电
        BQ769_RegisterGroup.SysCtrl2.SysCtrl2Bit.DSG_ON = 0;
    }
    // 将数值写入到芯片中
    Int_BQ769_WriteReg(BQ_SYS_CTRL2, BQ769_RegisterGroup.SysCtrl2.SysCtrl2Byte);

    // 读取寄存器的值 打印
    uint8_t bq_data = 0;
    Int_BQ769_Read_Register(BQ_SYS_CTRL2, &bq_data, 1);
    debug_printf("sysctrl2:%02x", bq_data);
}

/**
 * @brief 控制充电开关
 *
 */
void Int_BQ769_charge(uint8_t enable)
{
    if (enable)
    {
        // 开启充电
        BQ769_RegisterGroup.SysCtrl2.SysCtrl2Bit.CHG_ON = 1;
    }
    else
    {
        // 关闭充电
        BQ769_RegisterGroup.SysCtrl2.SysCtrl2Bit.CHG_ON = 0;
    }
    // 将数值写入到芯片中
    Int_BQ769_WriteReg(BQ_SYS_CTRL2, BQ769_RegisterGroup.SysCtrl2.SysCtrl2Byte);
}
