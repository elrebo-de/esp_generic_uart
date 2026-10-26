idf_component_register(SRCS "generic_uart.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_driver_uart"
                    PRIV_REQUIRES "esp_driver_gpio"
                    )

