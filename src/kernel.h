#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include "time.h"
#include "random.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define COLOR_GREEN_ON_BLACK 0x02

extern uint16_t terminal_row;
extern uint16_t terminal_col;

size_t strlen(const char *str);
void terminal_putchar(int x, int y, char c, char colour);
void kernel_main();
void terminal_print(const char* str, char colour);
void terminal_scroll();
void terminal_clear(char colour);
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
void vga_disable_cursor();
void print_header();

#endif