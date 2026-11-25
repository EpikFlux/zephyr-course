#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include "../drivers/our_driver/our_driver.h"

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));
	struct sensor_value v;
	int count = 0;

	if (!device_is_ready(dev)) {
		return -1;
	}

	while (1) {
		sensor_sample_fetch(dev);
		k_msleep(500);
		sensor_channel_get(dev, SENSOR_CHAN_ALL, &v);
		k_msleep(500);

		if (++count % 5 == 0) {
			our_driver_set_inverted(dev, (count / 5) % 2);
		}
	}
}