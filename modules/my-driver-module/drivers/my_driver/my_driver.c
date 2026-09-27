#define DT_DRV_COMPAT my_driver

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <my_driver/my_driver.h>

LOG_MODULE_REGISTER(my_driver, CONFIG_SENSOR_LOG_LEVEL);

struct my_driver_config {
	struct gpio_dt_spec led; /* read-only, from DTS */
};

struct my_driver_data {
	bool is_led_on; /* runtime state */
    unsigned int led_on_ctr; /* for task 2: keeps track of number of times the led has turned on since boot time */
};

/* sensor_sample_fetch() -> turn LED on */
static int my_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct my_driver_config *cfg = dev->config;
	struct my_driver_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	ret = gpio_pin_set_dt(&cfg->led, 1);
	if (ret < 0) {
		LOG_ERR("failed to turn LED on (%d)", ret);
		return ret;
	}

	data->is_led_on = true;
	LOG_DBG("LED on");

    data->led_on_ctr += 1;

	return 0;
}

/* sensor_channel_get() -> turn LED off, report state it had before */
static int my_driver_channel_get(const struct device *dev, enum sensor_channel chan,
				 struct sensor_value *val)
{
	const struct my_driver_config *cfg = dev->config;
	struct my_driver_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	val->val1 = data->is_led_on ? 1 : 0;
	val->val2 = 0;

	ret = gpio_pin_set_dt(&cfg->led, 0);
	if (ret < 0) {
		LOG_ERR("failed to turn LED off (%d)", ret);
		return ret;
	}

	data->is_led_on = false;
	LOG_DBG("LED off");

	return 0;
}

/* Extension API (declared in my_driver/my_driver.h) */
int my_driver_get_led_on_ctr(const struct device *dev, unsigned int *ctr)
{
	const struct my_driver_data *data = dev->data;

	if (ctr == NULL) {
		return -EINVAL;
	}

	*ctr = data->led_on_ctr;

	return 0;
}

int my_driver_reset_led_on_ctr(const struct device *dev)
{
	struct my_driver_data *data = dev->data;

	data->led_on_ctr = 0;
	LOG_DBG("LED on-counter reset");

	return 0;
}

static DEVICE_API(sensor, my_driver_api) = {
	.sample_fetch = my_driver_sample_fetch,
	.channel_get = my_driver_channel_get,
};

static int my_driver_init(const struct device *dev)
{
	const struct my_driver_config *cfg = dev->config;
	struct my_driver_data *data = dev->data;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}

	data->is_led_on = false;

    data->led_on_ctr = 0;

	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define MY_DRIVER_DEFINE(inst)                                   		\
	static struct my_driver_data data_##inst;                    		\
	static const struct my_driver_config cfg_##inst = {          		\
		.led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios), 	\
	};                                                           		\
	DEVICE_DT_INST_DEFINE(inst,                                  		\
			      my_driver_init, NULL,                          		\
			      &data_##inst, &cfg_##inst,                     		\
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,      		\
			      &my_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MY_DRIVER_DEFINE)
