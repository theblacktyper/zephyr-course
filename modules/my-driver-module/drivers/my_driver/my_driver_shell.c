#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <my_driver/my_driver.h>

static const struct device *dev = DEVICE_DT_GET_ANY(my_driver);

/* First "sensor fetch/read" command takes over the LED: app's auto-toggle stops until reset */
static void take_manual_control(const struct shell *sh)
{
    if (dev != NULL && my_driver_set_manual_mode(dev)) {
        shell_print(sh, "(auto-toggle disabled; shell has control until reset)");
    }
}

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

    take_manual_control(sh);

    int ret = sensor_sample_fetch(dev);
    if (0 == ret) {
        shell_print(sh, "Sensor (LED) set ON!");
    }
    return ret;
}
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    if (!device_is_ready(dev)) {
        shell_error(sh, "device not ready");
        return -ENODEV;
    }

    take_manual_control(sh);

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

/* Register root command "version" */
SHELL_CMD_REGISTER(version, NULL, "Show app version", cmd_version);

/* Create subcommands array for "sensor" */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch cmd", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read cmd", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Info cmd", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);
/* Register root command "sensor" with subcommands */
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands", NULL);
