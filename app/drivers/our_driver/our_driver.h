#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

struct our_driver_api
{
    struct sensor_driver_api sensor_api;
    int (*set_inverted)(const struct device *dev, bool inverted);
};

static inline int our_driver_set_inverted(const struct device *dev, bool inverted)
{
    const struct our_driver_api *api = (const struct our_driver_api *)dev->api;

    if (api->set_inverted == NULL)
    {
        return -ENOSYS;
    }
    return api->set_inverted(dev, inverted);
}

#endif 