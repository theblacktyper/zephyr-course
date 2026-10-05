#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <my_driver/my_driver.h>

#define USE_MY_DRIVER

#ifndef USE_MY_DRIVER
    /* now using alias created in app.overlay */
    #define LED_NODE DT_ALIAS(led2)

    static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#else
    static const struct device *dev = DEVICE_DT_GET_ANY(my_driver);
#endif

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
#ifndef USE_MY_DRIVER
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    LOG_INF("led0 heartbeat %d msec.", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);  // second requirement from module 4 assignment
    }
#else
    if (!device_is_ready(dev)) {
        LOG_ERR("my_driver device not ready");
        return 0;
    }

    // LOG_INF("my_driver LED heartbeat %d msec.", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    my_driver_set_led_on_ctr(dev, 0);

    /* LED is driven from the shell ("sensor fetch/read"); main just blocks */
    while (1) {
        k_sleep(K_FOREVER);     // block forever, other threads keep running
    }
#endif
    return 0;
}
