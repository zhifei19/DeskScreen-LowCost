idf_component_register(SRCS 
                            "app_main.c"
                            "src/hal/ds_timer.c"
                            "src/hal/ds_nvs.c"
                            "src/fs/ds_spiffs.c"
                            "src/ds_system_data.c"
                    INCLUDE_DIRS 
                            "."
                            "./include/hal"
                            "./include/fs"
                            "./include")

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
