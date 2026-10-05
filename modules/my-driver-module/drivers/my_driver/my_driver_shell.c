#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <my_driver/my_driver.h>

static const struct device *dev = DEVICE_DT_GET_ANY(my_driver);

static int cmd_version(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "App version: 1.0.0");
    return 0;
}

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    if (!device_is_ready(dev)) {
        shell_error(sh, "device not ready");
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    unsigned int on_count;
    if (0 == ret) {
        my_driver_get_led_on_ctr(dev, &on_count);
        shell_print(sh, "Sensor (LED) set ON! %u", on_count);
    }
    return ret;
}
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    if (!device_is_ready(dev)) {
        shell_error(sh, "device not ready");
        return -ENODEV;
    }

    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
    if (0 == ret) {
        shell_print(sh, "Sensor (LED) value: %d (LED now OFF)", val.val1);
    }
    return ret;
}
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    if (dev == NULL) {
        shell_error(sh, "no my_driver device in devicetree");
        return -ENODEV;
    }

    shell_print(sh, "dev_name : %s", dev->name);
    shell_print(sh, "   ready : %s", device_is_ready(dev) ? "yes" : "no");
    return 0;
}
/* sensor set <value>: set LED-on counter via extension API (argc enforced by SHELL_CMD_ARG) */
static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
    if (!device_is_ready(dev)) {
        shell_error(sh, "device not ready");
        return -ENODEV;
    }

    /* strict parse: digits only (rejects "", "-1", "abc", "5x") */
    const char *arg = argv[1];
    char *end;

    if (!isdigit((unsigned char)arg[0])) {
        shell_error(sh, "invalid value '%s' (expected 0-%u)", arg, MY_DRIVER_LED_ON_CTR_MAX);
        return -EINVAL;
    }

    errno = 0;
    unsigned long value = strtoul(arg, &end, 10);

    if (errno != 0 || *end != '\0') {
        shell_error(sh, "invalid value '%s' (expected 0-%u)", arg, MY_DRIVER_LED_ON_CTR_MAX);
        return -EINVAL;
    }
    if (value > MY_DRIVER_LED_ON_CTR_MAX) {
        shell_error(sh, "value %lu out of range (0-%u)", value, MY_DRIVER_LED_ON_CTR_MAX);
        return -EINVAL;
    }

    int ret = my_driver_set_led_on_ctr(dev, (unsigned int)value);
    if (0 == ret) {
        shell_print(sh, "LED on-counter set to %lu", value);
    }
    return ret;
}

/* Register root command "version" */
SHELL_CMD_REGISTER(version, NULL, "Show app version", cmd_version);

/* Create subcommands array for "sensor" */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch cmd", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read cmd", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Info cmd", cmd_sensor_info),
    /* mandatory = 2 (subcommand name + <value>), optional = 0 */
    SHELL_CMD_ARG(set, NULL, "Set LED-on counter: set <0-9>", cmd_sensor_set, 2, 0),
    SHELL_SUBCMD_SET_END
);
/* Register root command "sensor" with subcommands */
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands", NULL);
