
#include "driver/spi_master.h"
#include "esp_log.h"
#include "string.h"
#include "time.h"

void send_receive_task(void* param) {
    if(param == NULL) return;
    spi_device_handle_t dev = (spi_device_handle_t)*((spi_device_handle_t*)(param));
    
    static char send[255] = "Hello from master!";

    spi_transaction_t trans = {
        .tx_buffer = send,
        .rx_buffer = NULL,
        .length = 255*8
    };

    while(1){
        spi_device_transmit(dev,&trans);
        ESP_LOGI("INFO", "Sent %d bytes to slave. Message: %s", strlen(send), send);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void app_main() {

    spi_bus_config_t bus_conf = {
        .miso_io_num = 19,
        .mosi_io_num = 23,
        .sclk_io_num = 18
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_conf, SPI_DMA_CH_AUTO));


    spi_device_handle_t dev;
    spi_device_interface_config_t dev_conf = {
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 300,
        .spics_io_num = 26,
        .queue_size = 10
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST,&dev_conf,&dev));
    
    TaskHandle_t task;
    xTaskCreate(send_receive_task,"Sender Task",4096,&dev,10,&task);

    while(1){
        ESP_LOGI("PING","Test...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

}