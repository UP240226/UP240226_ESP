#include <stdio.h> // Librería estándar de entrada y salida
#include "freertos/FreeRTOS.h" // Librería necesaria para usar FreeRTOS
#include "freertos/task.h" // Librería necesaria para usar las tareas de FreeRTOS
#include "driver/gpio.h" // Librería necesaria para manejar los pines GPIO
#include "esp_log.h" // Librería necesaria para usar el sistema de logs de ESP-IDF

// Definición de los pines para el LED y el botón
#define LED GPIO_NUM_23
#define BUTTON GPIO_NUM_19
#define INT_PIN GPIO_NUM_5 // Define el pin GPIO5 como pin de interrupción
uint16_t int_count = 0; // Variable para contar interrupciones
bool button_state = false; // Bandera para indicar si el botón fue presionado


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
        


// Manejador de la interrupción externa
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    int_count++; // Incrementa el contador de interrupciones
    button_state = true; // Indica que el botón fue presionado
    vTaskDelay(pdMS_TO_TICKS(10)); // Pequeña demora para evitar rebotes
}

// Función principal del programa
void app_main(void)
{
    gpio_reset_pin(INT_PIN); // Resetea la configuración previa del pin
    gpio_set_direction(INT_PIN, GPIO_MODE_INPUT); // Configura el pin como entrada

    gpio_set_pull_mode(INT_PIN, GPIO_PULLUP_ONLY); // Activa resistencia pull-up interna
    gpio_set_intr_type(INT_PIN, GPIO_INTR_POSEDGE); // Configura interrupción por flanco de subida

    gpio_install_isr_service(0); // Instala el servicio de ISR para GPIO
    gpio_isr_handler_add(INT_PIN, gpio_isr_handler, NULL); // Registra el manejador de la interrupción

    gpio_intr_enable(INT_PIN); // Habilita la interrupción en el pin

    while(1) // Bucle principal
    {
        if(int_count > 3) // Si se detectó una pulsación
        {
            printf("Interrupción detectada\n");
            printf("%d\n", int_count); // Muestra el contador por consola
            button_state = false; // Reinicia la bandera
            sos(); // Envía el mensaje SOS en código Morse
            int_count = 0; // Reinicia el contador
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms antes de repetir
    }
}