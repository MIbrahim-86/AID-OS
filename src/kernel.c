#include "kernel.h"
#include "pic.h"
#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define COLOR_WHITE_BG 0xF0

uint16_t *video_mem = (uint16_t *)0xB8000;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour) {
  return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {
  video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_clear(char colour) {
  for (int y = 2; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminal_putchar(x, y, ' ', colour);
    }
  }
  terminal_row = 2;
  terminal_col = 0;
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

void itoa(int n, char *s) {
    int i = 0;
    if (n == 0) {
        s[i++] = '0';
        s[i] = '\0';
        return;
    }
    while(n > 0) {
        s[i++] = (n % 10) + '0';
        n = n / 10;
    }
    s[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = s[j];
        s[j] = s[k];
        s[k] = temp;
    }
}

void print_number_prefixed(int x, int y, int num, char colour) {
    char str[3];
    if (num < 10) {
        terminal_putchar(x, y, '0', colour);
        itoa(num, str);
        terminal_putchar(x + 1, y, str[0], colour);
    } else {
        itoa(num, str);
        terminal_putchar(x, y, str[0], colour);
        terminal_putchar(x + 1, y, str[1], colour);
    }
}

void terminal_print_at(int x, int y, const char *str, char colour) {
  for (size_t i = 0; i < strlen(str); i++) {
    terminal_putchar(x + i, y, str[i], colour);
  }
}

void terminal_print(const char *str, char colour) {
  for (size_t i = 0; i < strlen(str); i++) {
    if (str[i] == '\n') {
      terminal_row++;
      terminal_col = 0;
    } else {
      terminal_putchar(terminal_col, terminal_row, str[i], colour);
      terminal_col++;
      if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
      }
    }

    if (terminal_row >= VGA_HEIGHT) {
      terminal_scroll();
      terminal_row = VGA_HEIGHT - 1;
      terminal_col = 0;
    }
  }
}

void terminal_scroll() {
  for (int y = 3; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      video_mem[((y - 1) * VGA_WIDTH) + x] = video_mem[(y * VGA_WIDTH) + x];
    }
  }

  for (int x = 0; x < VGA_WIDTH; x++) {
    terminal_putchar(x, VGA_HEIGHT - 1, ' ', COLOR_GREEN_ON_BLACK);
  }
}

void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0) {
    if (*s1 != *s2) {
      return *(unsigned char *)s1 - *(unsigned char *)s2;
    }
    if (*s1 == '\0') {
      break;
    }
    s1++;
    s2++;
  }
  return 0;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void vga_disable_cursor() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}

void print_header() {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', COLOR_GREEN_ON_BLACK);
        }
    }

    time_t current_time;
    get_current_time(&current_time);
    
    char year_str[5];
    itoa(current_time.year, year_str);

    print_number_prefixed(0, 0, current_time.month, COLOR_GREEN_ON_BLACK);
    terminal_putchar(2, 0, '/', COLOR_GREEN_ON_BLACK);
    print_number_prefixed(3, 0, current_time.day, COLOR_GREEN_ON_BLACK);
    terminal_putchar(5, 0, '/', COLOR_GREEN_ON_BLACK);
    terminal_print_at(6, 0, year_str, COLOR_GREEN_ON_BLACK);

    print_number_prefixed(VGA_WIDTH - 8, 0, current_time.hour, COLOR_GREEN_ON_BLACK);
    terminal_putchar(VGA_WIDTH - 6, 0, ':', COLOR_GREEN_ON_BLACK);
    print_number_prefixed(VGA_WIDTH - 5, 0, current_time.minute, COLOR_GREEN_ON_BLACK);
    terminal_putchar(VGA_WIDTH - 3, 0, ':', COLOR_GREEN_ON_BLACK);
    print_number_prefixed(VGA_WIDTH - 2, 0, current_time.second, COLOR_GREEN_ON_BLACK);

    char *header = "Welcome to AID-OS";
    int len = strlen(header);
    int x = (VGA_WIDTH - len) / 2;
    terminal_print_at(x, 0, header, COLOR_GREEN_ON_BLACK);
    
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_putchar(i, 1, '~', COLOR_GREEN_ON_BLACK);
    }
}

void kernel_main() {

  vga_disable_cursor();

  pic_remap(0x20, 0x28);
  outb(0x21, 0xfd);
  outb(0xA1, 0xff);

  idt_init();

  keyboard_init();

  shell_init();

  asm volatile("sti");

  print_header();
  terminal_clear(COLOR_GREEN_ON_BLACK);

  terminal_print("> ", COLOR_GREEN_ON_BLACK);

  shell_run();
}
