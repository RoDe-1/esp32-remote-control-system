# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS main.c rgb_led.c wifi_app.c http_server.c DHT22.c app_nvs.c wifi_reset_button.c sntp_time_sync.c emqx_mqtt.c
						INCLUDE_DIRS "."
						EMBED_FILES webpage/index.html webpage/app.css webpage/app.js webpage/jquery-3.3.1.min.js webpage/favicon.ico
						EMBED_TXTFILES "emqx_ca.crt"
						)
