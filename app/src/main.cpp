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
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        LOG_ERR("my_driver device not ready");
        return 0;
    }

    LOG_INF("my_driver LED heartbeat %d msec.", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    /* Homework task 2: exercise custom extension API function - (modifies data) reset LED-on counter in driver's data struct */
    my_driver_reset_led_on_ctr(dev);

    while (1) {
        unsigned int on_counts = 0;

        /* Homework task 2: exercise custom extension API function - read & reset LED-on counter from driver */
        my_driver_get_led_on_ctr(dev, &on_counts);
        if (on_counts >= 10)
            my_driver_reset_led_on_ctr(dev);

        /* fetch -> LED on */
        if (sensor_sample_fetch(dev) < 0) {
            LOG_ERR("sample_fetch failed");
            return 0;
        }
        /* Homework task 2: exercise custom extension API function - read LED-on counter from driver */
        my_driver_get_led_on_ctr(dev, &on_counts);
        LOG_INF("LED state: ON (%u times)", on_counts);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        /* get -> LED off (val reports the state it had before) */
        if (sensor_channel_get(dev, SENSOR_CHAN_ALL, &val) < 0) {
            LOG_ERR("channel_get failed");
            return 0;
        }
        LOG_INF("LED state: OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
#endif
    return 0;
}
