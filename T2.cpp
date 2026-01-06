#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

// Definición del pin GPIO para el LED
#define PIN_LED GPIO_NUM_2

// Variable compartida para almacenar la lectura del sensor
static int lectura_sensor = 0;

// PROCESO 1: Simulación de lectura de sensor
void proceso_lectura_sensor(void *parametros) {
    while (1) {
        lectura_sensor++;
        printf("Sensor leído: %d\n", lectura_sensor);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera de 1 segundo
    }
}

// PROCESO 2: Control de parpadeo del LED
void proceso_control_led(void *parametros) {
    int estado_led = 0;

    while (1) {
        estado_led = !estado_led;
        gpio_set_level(PIN_LED, estado_led);
        printf("LED: %s\n", estado_led ? "ON" : "OFF");
        vTaskDelay(pdMS_TO_TICKS(500)); // Espera de 500 milisegundos
    }
}

// PROCESO 3: Monitoreo y reporte del sistema
void proceso_monitoreo_sistema(void *parametros) {
    while (1) {
        printf("Estado del sistema | Sensor=%d\n", lectura_sensor);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Espera de 2 segundos
    }
}


void app_main(void) {

    // Inicialización del pin LED como salida digital
    gpio_reset_pin(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LED, 0);

    // Creación del proceso de lectura del sensor
    xTaskCreate(
        proceso_lectura_sensor,
        "Proceso Sensor",
        2048,
        NULL,
        2,      // Prioridad media
        NULL
    );

    // Creación del proceso de control del LED
    xTaskCreate(
        proceso_control_led,
        "Proceso LED",
        2048,
        NULL,
        1,      // Prioridad baja
        NULL
    );

    // Creación del proceso de monitoreo del sistema
    xTaskCreate(
        proceso_monitoreo_sistema,
        "Proceso Monitor",
        2048,
        NULL,
        3,      // Prioridad alta
        NULL
    );
}