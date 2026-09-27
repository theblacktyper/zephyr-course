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

/**
 * @brief Reset the LED-on counter to 0.
 *
 * @param dev  my_driver device
 *
 * @return 0 on success.
 */
int my_driver_reset_led_on_ctr(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif /* MY_DRIVER_H_ */
