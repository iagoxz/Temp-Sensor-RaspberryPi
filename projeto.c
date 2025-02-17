#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"
#include "ssd1306_i2c.h"
#include "hardware/pio.h"
#include "ws2818b.pio.h"
#include "config.h"  


#define LED_PIN 7
#define LED_COUNT 25

typedef struct { uint8_t G, R, B; } npLED_t;
npLED_t leds[LED_COUNT];
PIO np_pio;
uint sm;


void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
}


void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}


void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}


void connect_wifi() {
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi!\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_MIXED_PSK, 30000)) {
        printf("Falha ao conectar no Wi-Fi!\n");
    } else {
        printf("Wi-Fi conectado!\n");
    }
}

// Estou apenas simulando a requisiçao HTTP pois nao tive tempo de conclui-la de fato ao tempo do envio do projeto
float fetch_temperature() {
    char response[512] = {0};

    // Simula resposta da API
    printf("Buscando temperatura na API...\n");
    strcpy(response, "{\"main\":{\"temp\":24}}");

 
    float temp = 0.0;
    char *ptr = strstr(response, "\"temp\":");
    if (ptr) {
        sscanf(ptr + 7, "%f", &temp);
        printf("Temperatura recebida: %.1fC\n", temp);
    } else {
        printf("Erro ao ler JSON!\n");
    }

    return temp;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

   
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);


    ssd1306_t oled;
    ssd1306_init(&oled, i2c1, 0x3C, 128, 64);
    ssd1306_clear(&oled);
    ssd1306_show(&oled);

    npInit(LED_PIN);

    connect_wifi();

    while (true) {
        float temp = fetch_temperature();

        char buffer[20];
        snprintf(buffer, sizeof(buffer), "Temp: %.1fC", temp);
        ssd1306_clear(&oled);
        ssd1306_draw_string(&oled, 10, 10, 1, "Temperatura Atual:");
        ssd1306_draw_string(&oled, 10, 30, 1, buffer);
        ssd1306_show(&oled);

        if (temp <= 25) {
            for (int i = 0; i < LED_COUNT; i++) {
                npSetLED(i, 255, 255, 255); 
            }
        } else {
            for (int i = 0; i < LED_COUNT; i++) {
                npSetLED(i, 255, 0, 0);  
            }
        }
        npWrite();

        printf("LEDs atualizados para temperatura: %.1fC\n", temp);

        sleep_ms(2000);  
    }
}
