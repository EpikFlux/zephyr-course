#include <stdlib.h>                                   
#include <errno.h>                                     
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include "our_driver.h" 

static const struct device *const sensor_dev =
	DEVICE_DT_GET(DT_NODELABEL(our_driver));

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	int ret = sensor_sample_fetch(sensor_dev);

	if (ret < 0) {
		shell_error(sh, "sensor_sample_fetch failed: %d", ret);
		return ret;
	}
	shell_print(sh, "Sample fetched");
	return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	struct sensor_value val;
	int ret = sensor_channel_get(sensor_dev, SENSOR_CHAN_ALL, &val);

	if (ret < 0) {
		shell_error(sh, "sensor_channel_get failed: %d", ret);
		return ret;
	}
	shell_print(sh, "Value: %d.%06d", val.val1, val.val2);
	return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Device name: %s", sensor_dev->name);
	shell_print(sh, "Ready: %s", device_is_ready(sensor_dev) ? "yes" : "no");
	return 0;
}

static int cmd_set(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2) {
		shell_error(sh, "Missing argument. Usage: sensorroot set <0|1>");
		return -EINVAL;
	}

	char *end;
	long value = strtol(argv[1], &end, 10);

	if (*end != '\0' || end == argv[1]) {
		shell_error(sh, "'%s' is not a number. Usage: sensorroot set <0|1>",
			    argv[1]);
		return -EINVAL;
	}
	if (value < 0 || value > 1) {
		shell_error(sh, "Value %ld out of range (allowed: 0 or 1)", value);
		return -EINVAL;
	}

	int ret = our_driver_set_inverted(sensor_dev, (bool)value);

	if (ret < 0) {
		shell_error(sh, "our_driver_set_inverted failed: %d", ret);
		return ret;
	}
	shell_print(sh, "inverted = %ld", value);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensorroot,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch()", cmd_fetch),
	SHELL_CMD(read, NULL, "Call sensor_channel_get() and print result", cmd_read),
	SHELL_CMD(info, NULL, "Print device name and ready state", cmd_info),
	SHELL_CMD_ARG(set, NULL, "Set inverted flag: set <0|1>", cmd_set, 2, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensorroot, &sub_sensorroot, "Sensor driver commands", NULL);