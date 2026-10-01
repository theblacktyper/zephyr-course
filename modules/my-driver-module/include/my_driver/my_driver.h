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

/**
 * @brief Put the device under manual control (e.g. from the shell).
 *
 * Latched: once set, it stays set until reset/power cycle. Apps that
 * auto-drive the LED should stop when my_driver_is_manual_mode() is true.
 *
 * @param dev  my_driver device
 *
 * @return true if this call switched to manual mode, false if it was already set.
 */
bool my_driver_set_manual_mode(const struct device *dev);

/**
 * @brief Check whether the device is under manual control.
 *
 * @param dev  my_driver device
 */
bool my_driver_is_manual_mode(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif /* MY_DRIVER_H_ */
