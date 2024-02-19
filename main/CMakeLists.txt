idf_component_register(SRCS 
                            "app_main.c"

                            "src/draw/ds_font.c"
                            "src/draw/ds_paint.c"

                            "src/hal/ds_timer.c"
                            "src/hal/ds_nvs.c"
                            "src/hal/ds_gpio.c"
                            "src/hal/ds_i2c.c"
                            "src/hal/ds_spi.c"

                            "src/driver/ds_ft6336.c"
                            "src/driver/ds_screen.c"
                            
                            "src/fs/ds_spiffs.c"

                            "src/net/ds_wifi_ap.c"
                            "src/net/ds_http_server.c"
                            "src/net/ds_http_request.c"
                            "src/net/ds_wifi_sta.c"
                            "src/net/ds_wifi_scan.c"
                            "src/net/ds_ap_sta.c"
                            

                            "src/ds_system_data.c"
                            "src/data/ds_data_image.c"
                        INCLUDE_DIRS 
                            "."
                            "./include/hal"
                            "./include/fs"
                            "./include/driver"
                            "./include/data"
                            "./include/net"
                            "./include/draw"
                            "./include"
                            
                        EMBED_FILES 
                            "./html/favicon.ico" 
                            "./html/upload_script.html"

                        EMBED_TXTFILES
                            "weather_root_cert.pem"
                        )

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
# spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
spiffs_create_partition_image(storage html FLASH_IN_PROJECT)
