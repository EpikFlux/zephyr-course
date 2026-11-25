#define DT_DRV_COMPAT our_driver

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
	struct gpio_dt_spec led;
};

static int our_driver_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct our_driver_config *cfg = dev->config;

	LOG_INF("sample_fetch: LED ON");
	return gpio_pin_set_dt(&cfg->led, 1);
}

static int our_driver_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct our_driver_config *cfg = dev->config;

	LOG_INF("channel_get: LED OFF");
	val->val1 = 0;
	val->val2 = 0;
	return gpio_pin_set_dt(&cfg->led, 0);
}

static DEVICE_API(sensor, our_driver_api) = {
	.sample_fetch = our_driver_sample_fetch,
	.channel_get = our_driver_channel_get,
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
	static const struct our_driver_config our_driver_cfg_##inst = {    \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),                 \
	};                                                                 \
	DEVICE_DT_INST_DEFINE(inst, our_driver_init, NULL, NULL,           \
			      &our_driver_cfg_##inst, POST_KERNEL,         \
			      CONFIG_SENSOR_INIT_PRIORITY, &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_INIT)