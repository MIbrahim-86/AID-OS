#include "isr.h"
#include "kernel.h"

void fault_handler(struct registers *r)
{
    terminal_print("Interrupt: ", 0x0F);
    char s[3];
    s[0] = (r->int_no / 10) + '0';
    s[1] = (r->int_no % 10) + '0';
    s[2] = '\0';
    terminal_print(s, 0x0F);
    terminal_print(", Error code: ", 0x0F);
    
    char err_s[5];
    err_s[0] = '0';
    err_s[1] = 'x';
    unsigned char high = (r->err_code >> 4) & 0x0F;
    unsigned char low = r->err_code & 0x0F;
    err_s[2] = high < 10 ? high + '0' : high + 'a' - 10;
    err_s[3] = low < 10 ? low + '0' : low + 'a' - 10;
    err_s[4] = '\0';
    terminal_print(err_s, 0x0F);

    terminal_print("\n", 0x0F);
    
    for (;;);
}

