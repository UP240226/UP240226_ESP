#include "sdkconfig.h"
#include "freertos/FreeRTOS.h" // Librería necesaria para usar FreeRTOS
#include "freertos/task.h" // Librería necesaria para usar las tareas de FreeRTOS
#include "driver/gpio.h" // Librería necesaria para manejar los pines GPIO

// Definición de los pines para el LED y el botón
#define LED GPIO_NUM_23
#define BUTTON GPIO_NUM_19

 // Duraciones en milisegundos para el código Morse
        #define DOT_MS 200
        #define DASH_MS 500
        #define BETWEEN_SYMBOLS_MS 200
        #define BETWEEN_LETTERS_MS 500

        static void dot(void)
        {
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(DOT_MS));
            gpio_set_level(LED, 0);
            vTaskDelay(pdMS_TO_TICKS(BETWEEN_SYMBOLS_MS));
        }

        static void dash(void)
        {
            gpio_set_level(LED, 1);
            vTaskDelay(pdMS_TO_TICKS(DASH_MS));
            gpio_set_level(LED, 0);
            vTaskDelay(pdMS_TO_TICKS(BETWEEN_SYMBOLS_MS));
        }

        static void sos(void)
        {
            // S: ...
            for (int i = 0; i < 3; ++i) dot();
            vTaskDelay(pdMS_TO_TICKS(BETWEEN_LETTERS_MS));
            // O: ---
            for (int i = 0; i < 3; ++i) dash();
            vTaskDelay(pdMS_TO_TICKS(BETWEEN_LETTERS_MS));
            // S: ...
            for (int i = 0; i < 3; ++i) dot();
        }

        bool doubleClick(void)
{
    const TickType_t debounceDelay = pdMS_TO_TICKS(50);
    const TickType_t doubleClickTimeout = pdMS_TO_TICKS(400);
    TickType_t startTime;

    // Espera la primera pulsación
    while (gpio_get_level(RDY) == 1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Pequeño delay para debounce
    vTaskDelay(debounceDelay);

    // Verifica si se soltó el botón
    while (gpio_get_level(RDY) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Marca el tiempo de la primera pulsación
    startTime = xTaskGetTickCount();

    // Espera la segunda pulsación dentro del tiempo permitido
    while ((xTaskGetTickCount() - startTime) < doubleClickTimeout)
    {

        if (gpio_get_level(RDY) == 0)
        {
            // Espera a que se suelte el botón nuevamente
            vTaskDelay(debounceDelay);
            while (gpio_get_level(RDY) == 0)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            ESP_LOGI("BUTTON", "Double click detected");
            return true;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Evita WDT
    }

    return false; // No se detectó doble clic
}

        
void app_main(void)
{
    // Reinicia la configuración de los pines LED y botón
    gpio_reset_pin(LED);
    gpio_reset_pin(BUTTON);

    // Configura el pin del LED como salida
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Configura el pin del botón con resistencia pull-up
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
    // Configura el pin del botón como entrada
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);

    // Bucle principal
    while(true)
    {
        // Lee el estado del botón (0 si está presionado, 1 si no)
        int status = gpio_get_level(BUTTON);

        // Si el botón está presionado (nivel bajo)
        if(status == true)
        {
            printf("Botón presionado\n");
            gpio_set_level(LED, 1); // Enciende el LED
            sos(); // Llama a la función sos cuando se presiona el botón
        }
        else
        {
            printf("...\n");
            gpio_set_level(LED, 0); // Apaga el LED
        }

        // Espera 20 ms antes de repetir
        vTaskDelay(pdMS_TO_TICKS(100)) ;
  
    }
    
}

