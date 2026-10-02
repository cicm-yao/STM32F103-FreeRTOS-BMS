#include "Com_BQ769.h"

static uint8_t TemNA = 6;

static const uint16_t TemD[31] =
    {
        2890, 2817, 2735, 2642, 2541, 2431, 2318, 2202, 2127, 2007,
        1846, 1687, 1546, 1422, 1309, 1202, 1099, 1001, 907, 817,
        727, 677, 613, 550, 496, 452, 414, 379, 345, 311, 277};


int16_t Com_BQ769_TempChange(uint16_t uiADCV)
{
    uint8_t ucA = 32, TemNK = 0;
    int16_t uiD = 0;

    TemNK = TemNA;
    if (uiADCV < TemD[TemNK])
    {
        while (--ucA)
        {
            if (TemNK < 30)
            {
                TemNK++;
                if (uiADCV >= TemD[TemNK])
                {
                    uiD = -200 + TemNK * (int)40;
                    uiD = uiD - ((int)40) * (uiADCV - TemD[TemNK]) / (TemD[TemNK - 1] - TemD[TemNK]);
                    break;
                }
            }
            else
            {
                uiD = 1000; // 100
                break;
            }
        }
    }
    else if (uiADCV >= TemD[TemNK - 1])
    {
        while (--ucA)
        {
            if (TemNK > 1)
            {
                TemNK--;
                if (uiADCV < TemD[TemNK - 1])
                {
                    uiD = -200 + TemNK * (int)40;
                    uiD = uiD - ((int)40) * (uiADCV - TemD[TemNK]) / (TemD[TemNK - 1] - TemD[TemNK]);
                    break;
                }
            }
            else
            {
                uiD = -200; //-30
                break;
            }
        }
    }
    else
    {
        uiD = -200 + TemNK * (int)40;
        uiD = uiD - ((int)40) * (uiADCV - TemD[TemNK]) / (TemD[TemNK - 1] - TemD[TemNK]);
    }
    TemNA = TemNK;

    if (uiD & 0x8000)
    {
        uiD &= ~0x8000;
        uiD = 0x8000 - uiD;
        uiD = uiD * 90 / 100;
        uiD &= 0x7fff;
        // uiD = 0x8000-uiD;
        uiD |= 0x8000;
    }
    else
    {
        uiD = uiD * 90 / 100;
    }
    return uiD;
}

uint16_t SocOcvTab[101] =
    {
        3282,                                                       // 0%~1%
        3309, 3334, 3357, 3378, 3398, 3417, 3434, 3449, 3464, 3477, // 0%~10%
        3489, 3500, 3510, 3520, 3528, 3536, 3543, 3549, 3555, 3561, // 11%~20%
        3566, 3571, 3575, 3579, 3583, 3586, 3590, 3593, 3596, 3599, // 21%~30%
        3602, 3605, 3608, 3611, 3615, 3618, 3621, 3624, 3628, 3632, // 31%~40%
        3636, 3640, 3644, 3648, 3653, 3658, 3663, 3668, 3674, 3679, // 41%~50%
        3685, 3691, 3698, 3704, 3711, 3718, 3725, 3733, 3741, 3748, // 51%~60%
        3756, 3765, 3773, 3782, 3791, 3800, 3809, 3818, 3827, 3837, // 61%~70%
        3847, 3857, 3867, 3877, 3887, 3897, 3908, 3919, 3929, 3940, // 71%~80%
        3951, 3962, 3973, 3985, 3996, 4008, 4019, 4031, 4043, 4055, // 81%~90%
        4067, 4080, 4092, 4105, 4118, 4131, 4145, 4158, 4172, 4185, // 91~100%
};



float Com_BQ769_VolToSoc(uint16_t fVol)
{

    if (fVol < SocOcvTab[0])

    {
        return 0;
    }
    else if (fVol > SocOcvTab[100])
    {
        return 100;
    }

    uint8_t index = 0;
    while (fVol > SocOcvTab[index])
    {
        index++;
    }

    float soc = index + (fVol - SocOcvTab[index]) * 1.0 / (SocOcvTab[index + 1] - SocOcvTab[index]);
    return soc;
}
