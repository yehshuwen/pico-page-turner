#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // wireless chip cyw43
#include "hardware/sync.h"

#define PIN_NEXT 14 // next page
#define PIN_PREV 15
#define DEBOUNCE_MS 30
#define EDGES (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE) // 0b1100

static volatile bool next_stable = false; // button: ON/OFF
static volatile bool prev_stable = false;
static volatile bool next_event = false; // button presseed event
static volatile bool prev_event = false;

static int64_t debounce_done(alarm_id_t id, void *user_data) {
    (void)id;
    uint pin = (uint)(uintptr_t)user_data;
    bool pressed = !gpio_get(pin);

    switch (pin) {
        case PIN_NEXT:
            if (pressed && !next_stable) next_event = true;
            next_stable = pressed;
            break;
        case PIN_PREV:
            if (pressed && !prev_stable) prev_event = true;
            prev_stable = pressed;
            break;
        default:
            return 0;
    }
    gpio_set_irq_enabled(pin, EDGES, true); // reopen pin ISR
    __sev(); // send event wake CPU
    return 0;
}

// ISR
// uint gpio: which pin trigger
// uint32_t events: which event trigger
static void gpio_isr(uint gpio, uint32_t events) {
    (void)events;
    switch (gpio) {
        case PIN_NEXT:
        case PIN_PREV:
            gpio_set_irq_enabled(gpio, EDGES, false);  // because of bounce -> turn off pin ISR
            add_alarm_in_ms(DEBOUNCE_MS, debounce_done, (void*)(uintptr_t)gpio, true); // set 30ms alarm execute debounce_done 
            break;
        default:
            break;
    }
}

static void button_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

int main(void) {
    stdio_init_all();

    // init
    if (cyw43_arch_init()) {
        printf("CYW43 init failed\n");
        return -1;
    }

    button_init(PIN_NEXT);
    button_init(PIN_PREV);

    gpio_set_irq_enabled_with_callback(PIN_NEXT, EDGES, true, &gpio_isr); // registor and enable ISR
    gpio_set_irq_enabled(PIN_PREV, EDGES, true);

    int count = 0;

    while (true) {
        if (next_event) {
            next_event = false;
            count++;
            printf("[%d] NEXT pressed\n", count);
        }
        if (prev_event) {
            prev_event = false;
            count++;
            printf("[%d] PREV pressed\n", count);
        }

        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, next_stable || prev_stable);

        __wfe();  // CPU sleep
    }
}