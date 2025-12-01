#include "time.h"
#include "kernel.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS   0x04
#define CMOS_DAY     0x07
#define CMOS_MONTH   0x08
#define CMOS_YEAR    0x09
#define CMOS_STATUS_A 0x0A
#define CMOS_STATUS_B 0x0B

int is_leap_year(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(uint8_t month, uint16_t year) {
    if (month == 2) {
        return is_leap_year(year) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else {
        return 31;
    }
}

int get_update_in_progress_flag() {
    outb(CMOS_ADDRESS, CMOS_STATUS_A);
    return (inb(CMOS_DATA) & 0x80);
}

uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void get_current_time(time_t* time) {
    while (get_update_in_progress_flag());

    time->second = get_rtc_register(CMOS_SECONDS);
    time->minute = get_rtc_register(CMOS_MINUTES);
    time->hour = get_rtc_register(CMOS_HOURS);
    time->day = get_rtc_register(CMOS_DAY);
    time->month = get_rtc_register(CMOS_MONTH);
    time->year = get_rtc_register(CMOS_YEAR);

    uint8_t register_b = get_rtc_register(CMOS_STATUS_B);

    if (!(register_b & 0x04)) {
        time->second = (time->second & 0x0F) + ((time->second / 16) * 10);
        time->minute = (time->minute & 0x0F) + ((time->minute / 16) * 10);
        time->hour = ( (time->hour & 0x0F) + (((time->hour & 0x70) / 16) * 10) ) | (time->hour & 0x80);
        time->day = (time->day & 0x0F) + ((time->day / 16) * 10);
        time->month = (time->month & 0x0F) + ((time->month / 16) * 10);
        time->year = (time->year & 0x0F) + ((time->year / 16) * 10);
    }
    
    if (!(register_b & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }

    time->year += 2000;

    time->hour += 5;
    if (time->hour >= 24) {
        time->hour -= 24;
        time->day++;
        if (time->day > days_in_month(time->month, time->year)) {
            time->day = 1;
            time->month++;
            if (time->month > 12) {
                time->month = 1;
                time->year++;
            }
        }
    }
}
