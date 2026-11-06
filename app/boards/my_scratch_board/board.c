#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int board_init_message(void)
{
    printk("Board Initialized\n");
    return 0;
}

SYS_INIT(board_init_message, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);