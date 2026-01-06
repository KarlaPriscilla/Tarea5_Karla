#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "driver/gpio.h"

// Definiciones de configuración del puerto serial
#define PUERTO_UART      UART_NUM_2     // Puerto UART utilizado (cambiar a UART_NUM_0 para simulación)
#define PIN_TRANSMISION  GPIO_NUM_17    // Pin GPIO para transmisión TX
#define PIN_RECEPCION    GPIO_NUM_16    // Pin GPIO para recepción RX
#define TAMANO_BUFFER    256            // Tamaño del buffer de comunicación

// Definición del pin del LED
#define PIN_LED          GPIO_NUM_2

// Variables globales para el seguimiento del sistema
static int contador_comandos = 0;
static int estado_led = 0;

// Función para eliminar caracteres de nueva línea al final de una cadena
void limpiar_salto_linea(char *cadena) {
    int longitud = strlen(cadena);
    while (longitud > 0 && (cadena[longitud - 1] == '\n' || cadena[longitud - 1] == '\r')) {
        cadena[longitud - 1] = '\0';
        longitud--;
    }
}

// Inicialización del puerto UART con parámetros específicos
void inicializar_uart(void) {
    uart_config_t configuracion_uart = {
        .baud_rate = 115200,                        // Velocidad de transmisión
        .data_bits = UART_DATA_8_BITS,              // 8 bits de datos
        .parity    = UART_PARITY_DISABLE,           // Sin paridad
        .stop_bits = UART_STOP_BITS_1,              // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE       // Sin control de flujo
    };

    uart_param_config(PUERTO_UART, &configuracion_uart);
    uart_set_pin(PUERTO_UART, PIN_TRANSMISION, PIN_RECEPCION,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(PUERTO_UART, TAMANO_BUFFER, TAMANO_BUFFER, 0, NULL, 0);
}

// Envía un mensaje de respuesta a través del puerto UART
void enviar_respuesta(const char *mensaje) {
    uart_write_bytes(PUERTO_UART, mensaje, strlen(mensaje));
}

// Procesa los comandos recibidos y ejecuta las acciones correspondientes
void procesar_comando(char *comando) {
    contador_comandos++;

    if (strcmp(comando, "status") == 0) {
        enviar_respuesta("Estado: Sistema operativo\r\n");
    }
    else if (strcmp(comando, "led on") == 0) {
        gpio_set_level(PIN_LED, 1);
        estado_led = 1;
        enviar_respuesta("LED encendido\r\n");
    }
    else if (strcmp(comando, "led off") == 0) {
        gpio_set_level(PIN_LED, 0);
        estado_led = 0;
        enviar_respuesta("LED apagado\r\n");
    }
    else if (strcmp(comando, "info") == 0) {
        char informacion[100];
        sprintf(informacion,
                "UART2 | Baud: 115200 | Comandos: %d | LED: %s\r\n",
                contador_comandos,
                estado_led ? "ON" : "OFF");
        enviar_respuesta(informacion);
    }
    else if (strcmp(comando, "reset") == 0) {
        contador_comandos = 0;
        enviar_respuesta("Variables reiniciadas\r\n");
    }
    else {
        enviar_respuesta("Comando no reconocido\r\n");
    }
}

// Función principal del programa
void app_main(void) {
    uint8_t datos[TAMANO_BUFFER];

    // Inicializar comunicación UART
    inicializar_uart();

    // Configurar el pin del LED como salida
    gpio_reset_pin(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LED, 0);

    // Mensaje de inicio del sistema
    enviar_respuesta("Sistema listo. Esperando comandos...\r\n");

    // Bucle principal de ejecución
    while (1) {
        // Leer datos del puerto UART
        int longitud = uart_read_bytes(
            PUERTO_UART,
            datos,
            TAMANO_BUFFER - 1,
            20 / portTICK_PERIOD_MS
        );

        // Si se recibieron datos, procesarlos
        if (longitud > 0) {
            datos[longitud] = '\0';

            // Extraer el comando de la cadena recibida
            char *comando = strtok((char *)datos, "\r\n");
            if (comando != NULL) {
                limpiar_salto_linea(comando);
                procesar_comando(comando);
            }
        }

        // Pequeña pausa para no saturar el procesador
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}