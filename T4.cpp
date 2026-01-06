#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "driver/gpio.h"

// Configuración del puerto de comunicación serial
#define PUERTO_SERIAL      UART_NUM_0     // Usar UART_NUM_0 para simulación en Wokwi
#define PIN_TX_SERIAL      GPIO_NUM_17    // Pin de transmisión
#define PIN_RX_SERIAL      GPIO_NUM_16    // Pin de recepción
#define TAMANO_BUFFER      256            // Tamaño del buffer de datos

// Configuración del LED indicador
#define PIN_LED_INDICADOR  GPIO_NUM_2

// Variables globales compartidas entre tareas
static int estado_led = 0;
static int contador_comandos = 0;

// Función para eliminar caracteres de salto de línea
void eliminar_saltos_linea(char *cadena) {
    int longitud = strlen(cadena);
    while (longitud > 0 && (cadena[longitud - 1] == '\n' || cadena[longitud - 1] == '\r')) {
        cadena[longitud - 1] = '\0';
        longitud--;
    }
}

// Inicialización del puerto de comunicación serial
void inicializar_comunicacion_serial(void) {
    uart_config_t configuracion_serial = {
        .baud_rate = 115200,                        // Velocidad de transmisión
        .data_bits = UART_DATA_8_BITS,              // 8 bits por dato
        .parity    = UART_PARITY_DISABLE,           // Sin bit de paridad
        .stop_bits = UART_STOP_BITS_1,              // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE       // Sin control de flujo por hardware
    };

    uart_param_config(PUERTO_SERIAL, &configuracion_serial);
    uart_set_pin(PUERTO_SERIAL, PIN_TX_SERIAL, PIN_RX_SERIAL,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(PUERTO_SERIAL, TAMANO_BUFFER, TAMANO_BUFFER, 0, NULL, 0);
}

// Función para transmitir mensajes por el puerto serial
void transmitir_mensaje_serial(const char *mensaje) {
    uart_write_bytes(PUERTO_SERIAL, mensaje, strlen(mensaje));
}

// PROCESO 1: Gestión de comunicación serial y procesamiento de comandos
void proceso_comunicacion_serial(void *parametros) {
    uint8_t datos_recibidos[TAMANO_BUFFER];

    transmitir_mensaje_serial("Sistema integrado listo\r\n");

    while (1) {
        // Leer datos del puerto serial
        int longitud = uart_read_bytes(
            PUERTO_SERIAL,
            datos_recibidos,
            TAMANO_BUFFER - 1,
            50 / portTICK_PERIOD_MS
        );

        if (longitud > 0) {
            datos_recibidos[longitud] = '\0';
            char *comando = strtok((char *)datos_recibidos, "\r\n");

            if (comando != NULL) {
                eliminar_saltos_linea(comando);
                contador_comandos++;

                // Procesar comando "status"
                if (strcmp(comando, "status") == 0) {
                    transmitir_mensaje_serial("Estado: OK\r\n");
                }
                // Procesar comando "led on"
                else if (strcmp(comando, "led on") == 0) {
                    estado_led = 1;
                    transmitir_mensaje_serial("LED encendido\r\n");
                }
                // Procesar comando "led off"
                else if (strcmp(comando, "led off") == 0) {
                    estado_led = 0;
                    transmitir_mensaje_serial("LED apagado\r\n");
                }
                // Procesar comando "info"
                else if (strcmp(comando, "info") == 0) {
                    char mensaje_info[100];
                    sprintf(mensaje_info,
                            "Comandos: %d | LED: %s\r\n",
                            contador_comandos,
                            estado_led ? "ON" : "OFF");
                    transmitir_mensaje_serial(mensaje_info);
                }
                // Comando no reconocido
                else {
                    transmitir_mensaje_serial("Comando no valido\r\n");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// PROCESO 2: Control del estado del LED
void proceso_actualizacion_led(void *parametros) {
    while (1) {
        gpio_set_level(PIN_LED_INDICADOR, estado_led);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// PROCESO 3: Monitoreo periódico del sistema
void proceso_monitoreo_periodico(void *parametros) {
    while (1) {
        char reporte_estado[80];
        sprintf(reporte_estado,
                "[Monitor] LED=%s | Cmd=%d\r\n",
                estado_led ? "ON" : "OFF",
                contador_comandos);
        transmitir_mensaje_serial(reporte_estado);

        vTaskDelay(pdMS_TO_TICKS(3000)); // Reporte cada 3 segundos
    }
}

void app_main(void) {

    // Inicializar comunicación serial
    inicializar_comunicacion_serial();

    // Configurar el LED como salida digital
    gpio_reset_pin(PIN_LED_INDICADOR);
    gpio_set_direction(PIN_LED_INDICADOR, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LED_INDICADOR, 0);

    // Crear proceso de comunicación serial (prioridad alta)
    xTaskCreate(proceso_comunicacion_serial, "Proceso Serial", 4096, NULL, 3, NULL);
    
    // Crear proceso de control del LED (prioridad baja)
    xTaskCreate(proceso_actualizacion_led, "Proceso LED", 2048, NULL, 1, NULL);
    
    // Crear proceso de monitoreo (prioridad media)
    xTaskCreate(proceso_monitoreo_periodico, "Proceso Monitor", 2048, NULL, 2, NULL);
}