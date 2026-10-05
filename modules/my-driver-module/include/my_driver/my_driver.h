#ifndef MY_DRIVER_H_
#define MY_DRIVER_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get how many times the LED has been turned on (via sensor_sample_fetch).
 *
 * @param dev  my_driver device
 * @param ctr  Output pointer for the current counter value.
 *
 * @return 0 on success, -EINVAL if ctr is NULL.
 */
int my_driver_get_led_on_ctr(const struct device *dev, unsigned int *ctr);

/** Highest value accepted by my_driver_set_led_on_ctr(). */
#define MY_DRIVER_LED_ON_CTR_MAX 9U

/**
 * @brief Set the LED-on counter (0 resets it).
 *
 * @param dev    my_driver device
 * @param value  New counter value, 0..MY_DRIVER_LED_ON_CTR_MAX.
 *
 * @return 0 on success, -EINVAL if value is out of range.
 */
int my_driver_set_led_on_ctr(const struct device *dev, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif /* MY_DRIVER_H_ */
