if (CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE our_driver.c)
    if(CONFIG_OUR_DRIVER)
        target_sources(app PRIVATE sensor_shell.c)
    endif()
endif()

