#ifndef GUARD_CONSTANTS_SIIRTC_H
#define GUARD_CONSTANTS_SIIRTC_H

#define SIIRTCINFO_INTFE  0x01 // frequency interrupt enable
#define SIIRTCINFO_INTME  0x02 // per-minute interrupt enable
#define SIIRTCINFO_INTAE  0x04 // alarm interrupt enable
#define SIIRTCINFO_24HOUR 0x40 // 0: 12-hour mode, 1: 24-hour mode
#define SIIRTCINFO_POWER  0x80 // power on or power failure occurred

#define HOURS_PER_DAY       24
#define MINUTES_PER_HOUR    60
#define SECONDS_PER_MINUTE  60
#define DAYS_PER_WEEK       7

#define DAY_SUNDAY          0
#define DAY_MONDAY          1
#define DAY_TUESDAY         2
#define DAY_WEDNESDAY       3
#define DAY_THURSDAY        4
#define DAY_FRIDAY          5
#define DAY_SATURDAY        6
#define DAYS_COUNT          7

#define MONTH_JAN           1
#define MONTH_FEB           2
#define MONTH_MAR           3
#define MONTH_APR           4
#define MONTH_MAY           5
#define MONTH_JUN           6
#define MONTH_JUL           7
#define MONTH_AUG           8
#define MONTH_SEP           9
#define MONTH_OCT           10
#define MONTH_NOV           11
#define MONTH_DEC           12
#define MONTH_COUNT         MONTH_DEC

#endif // GUARD_CONSTANTS_SIIRTC_H