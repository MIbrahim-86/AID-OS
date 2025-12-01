#include "keyboard.h"
#include "kernel.h"
#include "pic.h"

unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    0, 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/',   0,   0,   0, ' ',   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

unsigned char keyboard_map_shifted[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',    0, 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?',   0,   0,   0, ' ',   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t buffer_head = 0;
static uint32_t buffer_tail = 0;
static int shift_held = 0;
static int caps_lock_on = 0;

void keyboard_wait_for_ack() {
    while(inb(0x60) != 0xFA);
}

void set_keyboard_leds() {
    uint8_t led_state = 0;
    if (caps_lock_on) {
        led_state |= (1 << 2);
    }
    
    outb(0x60, 0xED);
    keyboard_wait_for_ack();
    outb(0x60, led_state);
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_held = 1;
    } 
    else if (scancode == 0xAA || scancode == 0xB6) {
        shift_held = 0;
    }
    else if (scancode == 0x3A) {
        caps_lock_on = !caps_lock_on;
        set_keyboard_leds();
    }
    // Up arrow
    else if (scancode == 0x48) {
        if (((buffer_head + 1) % KEYBOARD_BUFFER_SIZE) != buffer_tail) {
            keyboard_buffer[buffer_head] = UP_ARROW;
            buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        }
    }
    // Down arrow
    else if (scancode == 0x50) {
        if (((buffer_head + 1) % KEYBOARD_BUFFER_SIZE) != buffer_tail) {
            keyboard_buffer[buffer_head] = DOWN_ARROW;
            buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        }
    }
    else if (scancode < 0x80) {
        char ascii_char;
        char unshifted_char = keyboard_map[scancode];
        
        int is_alpha = (unshifted_char >= 'a' && unshifted_char <= 'z');
        
        int effective_shift = shift_held;
        if (is_alpha && caps_lock_on) {
            effective_shift = !effective_shift;
        }

        if (effective_shift) {
            ascii_char = keyboard_map_shifted[scancode];
        } else {
            ascii_char = unshifted_char;
        }

        if (ascii_char != 0) {
            if (((buffer_head + 1) % KEYBOARD_BUFFER_SIZE) != buffer_tail) {
                keyboard_buffer[buffer_head] = ascii_char;
                buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
            }
        }
    }

    PIC_sendEOI(1);
}

void keyboard_init() {
    set_keyboard_leds();
}

char get_char() {
    while (buffer_head == buffer_tail) {
    }

    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}
