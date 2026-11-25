#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));
	struct sensor_value v;

	if (!device_is_ready(dev)) {
		return -1;
	}
	while (1) {
		sensor_sample_fetch(dev);                   
		k_msleep(500);
		sensor_channel_get(dev, SENSOR_CHAN_ALL, &v); 
		k_msleep(500);
	}
}