#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_system.h"

// Definiciones de hardware y tiempos
#define PIN_INDICADOR_LED GPIO_NUM_2
#define TIEMPO_SUEÑO_SEG  10

void app_main(void) {

    // Inicialización del pin del LED como salida
    gpio_reset_pin(PIN_INDICADOR_LED);
    gpio_set_direction(PIN_INDICADOR_LED, GPIO_MODE_OUTPUT);

    // Identificar el motivo por el cual el procesador se activó
    esp_sleep_wakeup_cause_t motivo_despertar = esp_sleep_get_wakeup_cause();

    if (motivo_despertar == ESP_SLEEP_WAKEUP_TIMER) {
        printf("El sistema se ha activado mediante el temporizador interno\n");
    } else {
        printf("Inicio del sistema por encendido o reinicio manual\n");
    }

    // Fase de ejecución activa
    printf("Dispositivo en modo operativo\n");
    gpio_set_level(PIN_INDICADOR_LED, 1); // Encender LED para indicar actividad

    // Mantener el sistema encendido durante 5 segundos antes de dormir
    vTaskDelay(pdMS_TO_TICKS(5000)); 

    // Configuración y entrada al modo de bajo consumo (Deep Sleep)
    printf("Iniciando modo Deep Sleep durante %d segundos...\n", TIEMPO_SUEÑO_SEG);
    gpio_set_level(PIN_INDICADOR_LED, 0); // Apagar LED para ahorrar energía

    // Configurar el temporizador para despertar en el tiempo definido (convertido a microsegundos)
    esp_sleep_enable_timer_wakeup(TIEMPO_SUEÑO_SEG * 1000000ULL);

    // Ejecutar la entrada al modo de sueño profundo
    esp_deep_sleep_start();
}