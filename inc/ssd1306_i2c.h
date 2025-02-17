#ifndef SSD1306_I2C_H
#define SSD1306_I2C_H

#include <stdlib.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


extern int ssd1306_width;
extern int ssd1306_height;
extern int ssd1306_i2c_address;


#define ssd1306_set_memory_mode               0x20
#define ssd1306_set_column_address            0x21
#define ssd1306_set_page_address              0x22
#define ssd1306_set_horizontal_scroll         0x26
#define ssd1306_set_scroll                    0x2E
#define ssd1306_set_display_start_line        0x40
#define ssd1306_set_contrast                  0x81
#define ssd1306_set_charge_pump               0x8D
#define ssd1306_set_segment_remap             0xA0
#define ssd1306_set_entire_on                 0xA4
#define ssd1306_set_all_on                    0xA5
#define ssd1306_set_normal_display            0xA6
#define ssd1306_set_inverse_display           0xA7
#define ssd1306_set_mux_ratio                 0xA8
#define ssd1306_set_display                   0xAE
#define ssd1306_set_common_output_direction   0xC0
#define ssd1306_set_display_offset            0xD3
#define ssd1306_set_display_clock_divide_ratio 0xD5
#define ssd1306_set_precharge                 0xD9
#define ssd1306_set_common_pin_configuration  0xDA
#define ssd1306_set_vcomh_deselect_level      0xDB


typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t pages;
    uint8_t address;
    i2c_inst_t *i2c_port;
    bool external_vcc;
    uint8_t *ram_buffer;
    size_t bufsize;
    uint8_t port_buffer[2];
} ssd1306_t;


void ssd1306_init(ssd1306_t *oled, i2c_inst_t *i2c, uint8_t addr,
                  uint8_t width, uint8_t height);


void ssd1306_clear(ssd1306_t *oled);

void ssd1306_show(ssd1306_t *oled);

void ssd1306_draw_string(ssd1306_t *oled, int x, int y, int color, const char *text);

void ssd1306_init_bm(ssd1306_t *ssd, uint8_t width, uint8_t height,
                     bool external_vcc, uint8_t address, i2c_inst_t *i2c);
void ssd1306_config(ssd1306_t *ssd);
void ssd1306_draw_bitmap(ssd1306_t *ssd, const uint8_t *bitmap);


void ssd1306_scroll(bool set);

void ssd1306_command(ssd1306_t *ssd, uint8_t cmd);

#endif 
