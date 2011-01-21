#ifndef ONEWIRE_H
#define ONEWIRE_H

#define OUTPUT_REG PORTB
#define INPUT_REG  PINB
#define MODE_REG   DDRB
#define IO_PIN     5

#define HIGH_TIME  55
#define LOW_TIME   5

#define ID_LEN     8

#include <stdint.h>
#include <avr/io.h>

extern unsigned char id[ID_LEN];

uint8_t read_ibutton_id();
void write(uint8_t data);
uint8_t read();
void delay_us(unsigned long delay);
uint8_t crc8();

#endif
