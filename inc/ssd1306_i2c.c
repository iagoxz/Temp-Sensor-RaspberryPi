#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "ssd1306_font.h"
#include "ssd1306_i2c.h"

// ---------------------------------------------------------------------------
// Definição das variáveis globais (se quiser usar em funções internas)
int ssd1306_width       = 128;
int ssd1306_height      = 64;
int ssd1306_i2c_address = 0x3C;

// Fontes e macros auxiliares
static inline int ssd1306_get_font_char(uint8_t c) {
    // Exemplo de conversão: A-Z => 1..26, 0-9 => 27..36, etc.
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 27;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Funções internas (sem parâmetros) - se quiser manter macros
// ---------------------------------------------------------------------------
static void ssd1306_send_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x80, cmd};
    i2c_write_blocking(i2c1, ssd1306_i2c_address, buffer, 2, false);
}

static void ssd1306_send_command_list(uint8_t *list, int length) {
    for (int i = 0; i < length; i++) {
        ssd1306_send_command(list[i]);
    }
}

// ---------------------------------------------------------------------------
// Funções baseadas em struct
// ---------------------------------------------------------------------------
static void ssd1306_init_internal(void) {
    // Sequência de comandos para inicializar
    uint8_t commands[] = {
        ssd1306_set_display,
        ssd1306_set_memory_mode, 0x00,
        ssd1306_set_display_start_line,
        ssd1306_set_segment_remap | 0x01,
        ssd1306_set_mux_ratio, ssd1306_height - 1,
        ssd1306_set_common_output_direction | 0x08,
        ssd1306_set_display_offset, 0x00,
        ssd1306_set_common_pin_configuration,
#if ((128 == 128) && (64 == 32))
        0x02,
#elif ((128 == 128) && (64 == 64))
        0x12,
#else
        0x02,
#endif
        ssd1306_set_display_clock_divide_ratio, 0x80,
        ssd1306_set_precharge, 0xF1,
        ssd1306_set_vcomh_deselect_level, 0x30,
        ssd1306_set_contrast, 0xFF,
        ssd1306_set_entire_on,
        ssd1306_set_normal_display,
        ssd1306_set_charge_pump, 0x14,
        ssd1306_set_scroll | 0x00,
        ssd1306_set_display | 0x01,
    };

    ssd1306_send_command_list(commands, sizeof(commands));
}

void ssd1306_init(ssd1306_t *oled, i2c_inst_t *i2c, uint8_t addr,
                  uint8_t width, uint8_t height)
{

    ssd1306_width       = width;
    ssd1306_height      = height;
    ssd1306_i2c_address = addr;

    // Preenche a struct
    oled->width   = width;
    oled->height  = height;
    oled->pages   = height / 8;
    oled->address = addr;
    oled->i2c_port = i2c;
    oled->bufsize = oled->pages * oled->width + 1;
    oled->ram_buffer = calloc(oled->bufsize, 1);
    oled->ram_buffer[0] = 0x40; // byte de controle para dados
    oled->port_buffer[0] = 0x80; // byte de controle para comandos


    ssd1306_init_internal();
}

void ssd1306_clear(ssd1306_t *oled) {
    if (!oled->ram_buffer) return;
    memset(oled->ram_buffer + 1, 0, oled->bufsize - 1);
}

static void ssd1306_send_data(ssd1306_t *oled) {
    // Envia o buffer inteiro
    uint8_t cmd_column[] = {
        ssd1306_set_column_address, 0,
        (uint8_t)(oled->width - 1)
    };
    uint8_t cmd_page[] = {
        ssd1306_set_page_address, 0,
        (uint8_t)(oled->pages - 1)
    };

    // Manda definir colunas e páginas
    ssd1306_command(oled, cmd_column[0]);
    ssd1306_command(oled, cmd_column[1]);
    ssd1306_command(oled, cmd_column[2]);
    ssd1306_command(oled, cmd_page[0]);
    ssd1306_command(oled, cmd_page[1]);
    ssd1306_command(oled, cmd_page[2]);

    i2c_write_blocking(oled->i2c_port, oled->address,
                       oled->ram_buffer, oled->bufsize, false);
}

void ssd1306_show(ssd1306_t *oled) {
    ssd1306_send_data(oled);
}

void ssd1306_draw_string(ssd1306_t *oled, int x, int y, int color, const char *text) {
    (void)color; // não usado
    if (!oled->ram_buffer) return;

    // Calcula offset no ram_buffer + 1
    // Lembrando que ram_buffer[0] = 0x40
    uint8_t *buf = oled->ram_buffer + 1;

    while (*text) {
        // Desenha caractere
        if (x <= (oled->width - 8) && y <= (oled->height - 8)) {
            int row = y / 8;
            int fb_idx = row * oled->width + x;
            uint8_t c = toupper((uint8_t)*text);
            int fidx = ssd1306_get_font_char(c);

            for (int i = 0; i < 8; i++) {
                buf[fb_idx++] = font[fidx * 8 + i];
            }
        }
        x += 8;
        text++;
    }
}


void ssd1306_init_bm(ssd1306_t *ssd, uint8_t width, uint8_t height,
                     bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
    ssd->width  = width;
    ssd->height = height;
    ssd->pages  = height / 8;
    ssd->address = address;
    ssd->i2c_port = i2c;
    ssd->bufsize  = ssd->pages * ssd->width + 1;
    ssd->ram_buffer = calloc(ssd->bufsize, 1);
    ssd->ram_buffer[0] = 0x40;
    ssd->port_buffer[0] = 0x80;
    ssd->external_vcc   = external_vcc;
}

void ssd1306_config(ssd1306_t *ssd) {
}

void ssd1306_draw_bitmap(ssd1306_t *ssd, const uint8_t *bitmap) {

}

void ssd1306_scroll(bool set) {
    uint8_t commands[] = {
        ssd1306_set_horizontal_scroll | 0x00,
        0x00, 0x00, 0x00, 0x03,
        0x00, 0xFF,
        ssd1306_set_scroll | (set ? 0x01 : 0)
    };
    ssd1306_send_command_list(commands, sizeof(commands));
}

void ssd1306_command(ssd1306_t *ssd, uint8_t cmd) {
    ssd->port_buffer[1] = cmd;
    i2c_write_blocking(ssd->i2c_port, ssd->address, ssd->port_buffer, 2, false);
}
