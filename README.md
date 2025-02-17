# 🌡️ Projeto Sensor de Temperatura - Raspberry Pi Pico W

Este é um projeto IoT (Internet das Coisas) baseado na Raspberry Pi Pico W, que exibe a temperatura em um display OLED, controla LEDs RGB WS2812B com base na temperatura

A temperatura é obtida através da API OpenWeatherMap, permitindo que o dispositivo forneça dados meteorológicos em tempo real para sua localização. 🌍📡

 ## __📌 Funcionalidades__
✔ Wi-Fi integrado: Conecta-se automaticamente à rede Wi-Fi.
✔ Consulta a API OpenWeatherMap: Obtém a temperatura em tempo real.
✔ Exibição no OLED: A temperatura é exibida no display SSD1306.
✔ Controle de LEDs RGB WS2812B:

🔴 Vermelho se a temperatura for maior que 25°C
⚪ Branco se a temperatura for até 25°C
✔ Notificações no Telegram: Envia alertas sobre a temperatura para o celular. 📩📱
🛠️ Componentes Utilizados
📡 Raspberry Pi Pico W
📺 Display OLED SSD1306 (I2C)
💡 Matriz de LEDs WS2812B (Neopixel)
🌍 API OpenWeatherMap

## 🚀 Como Configurar
1️⃣ Instale as dependências
Antes de compilar, certifique-se de que seu SDK do Raspberry Pi Pico está configurado corretamente.

2️⃣ Crie o arquivo de configuração
No diretório do projeto, crie um arquivo chamado config.h e adicione suas credenciais:

>*📄 config.h*

```
#ifndef CONFIG_H
#define CONFIG_H

// 📡 Wi-Fi
#define WIFI_SSID "SUA_REDE_WIFI"
#define WIFI_PASSWORD "SUA_SENHA_WIFI"

// 🌍 OpenWeatherMap API
#define API_KEY "SUA_CHAVE_API"
#define CITY "Picos"
#define COUNTRY "BR"
#define WEATHER_HOST "api.openweathermap.org"
#define WEATHER_PATH "/data/2.5/weather?q=" CITY "," COUNTRY "&appid=" API_KEY "&units=metric"

#endif
```

## __Melhorias Futuras__

- 📍 GPS para localização automática na OpenWeatherMap,
- 🌡️ Uso de um sensor físico de temperatura (ex: DHT22),
- 📊 Dashboard para monitoramento via Web,
- ⚙️ Implementação completa da requisição HTTP,
- ⚙️ Envio de notificações por meio de um bot no telegram,

