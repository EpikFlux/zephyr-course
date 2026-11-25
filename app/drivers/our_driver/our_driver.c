#define DT_DRV_COMPAT our_driver

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
	struct gpio_dt_spec led;
};

struct our_driver_data {
	bool inverted;
};

static int our_driver_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct our_driver_config *cfg = dev->config;
	struct our_driver_data *data = dev->data;

	int level = data->inverted ? 0 : 1;

	LOG_INF("sample_fetch: LED %s", level ? "ON" : "OFF");
	return gpio_pin_set_dt(&cfg->led, level);
}

static int our_driver_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct our_driver_config *cfg = dev->config;
	struct our_driver_data *data = dev->data;

	int level = data->inverted ? 1 : 0;

	LOG_INF("channel_get: LED %s", level ? "ON" : "OFF");
	val->val1 = level;
	val->val2 = 0;
	return gpio_pin_set_dt(&cfg->led, level);
}

static int our_driver_set_inverted_impl(const struct device *dev, bool inverted)
{
	struct our_driver_data *data = dev->data;

	data->inverted = inverted;
	LOG_INF("inverted set to %d", inverted);
	return 0;
}

static const struct our_driver_api our_driver_api = {
	.sensor_api = {
		.sample_fetch = our_driver_sample_fetch,
		.channel_get = our_driver_channel_get,
	},
	.set_inverted = our_driver_set_inverted_impl,
};

static int our_driver_init(const struct device *dev)
{
	const struct our_driver_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define OUR_DRIVER_INIT(inst)                                              \
	static struct our_driver_data our_driver_data_##inst = {           \
		.inverted = false,                                         \
	};                                                                 \
	static const struct our_driver_config our_driver_cfg_##inst = {    \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),                 \
	};                                                                 \
	DEVICE_DT_INST_DEFINE(inst, our_driver_init, NULL,                 \
			      &our_driver_data_##inst,                     \
			      &our_driver_cfg_##inst, POST_KERNEL,         \
			      CONFIG_SENSOR_INIT_PRIORITY,                 \
			      &our_driver_api.sensor_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_INIT)