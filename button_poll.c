#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // wireless chip cyw43

#define PIN_NEXT 14 // next page
#define PIN_PREV 15
#define DEBOUNCE_MS 30
#define NUM_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))

typedef struct {
    uint pin;
    const char *name;
    bool stable;
    bool last_value;
    uint32_t last_change_time;
} button_t;

static button_t buttons[] = {
    { PIN_NEXT, "NEXT", false, false, 0 },
    { PIN_PREV, "PREV", false, false, 0 }
};

static void button_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

static uint32_t now_ms(void) {
    return to_ms_since_boot(get_absolute_time());
}

static bool button_pressed_event(button_t *b) {
    bool value = !gpio_get(b->pin);
    uint32_t t = now_ms();

    // if raw value change reset timer
    if (value != b->last_value) {
        b->last_value = value;
        b->last_change_time = t;
    }

    if ((t - b->last_change_time) >= DEBOUNCE_MS && value != b->stable) {
        b->stable = value;
        return b->stable;
    }
    return false;
}

int main(void) {
    stdio_init_all();

    // init
    if (cyw43_arch_init()) {
        printf("CYW43 init failed\n");
        return -1;
    }

    // init button
    for (size_t i = 0; i < NUM_BUTTONS; i++) {
        gpio_init(buttons[i].pin);
        gpio_set_dir(buttons[i].pin, GPIO_IN);
        gpio_pull_up(buttons[i].pin);
    }

    // counter
    int count = 0;

    while (true) {
        bool any_pressed = false;
        for (size_t i = 0; i < NUM_BUTTONS; i++) {
            if (button_pressed_event(&buttons[i])) {
                count++;
                printf("[%d] %s pressed\n", count, buttons[i].name);
            }
            if (buttons[i].stable) any_pressed = true;
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, any_pressed);

        sleep_ms(1);
    }
}