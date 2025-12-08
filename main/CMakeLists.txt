idf_component_register(
    SRCS "campo_iot_main.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash driver esp_adc
)
