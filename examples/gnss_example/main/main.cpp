/*
 * Example program to use a GNSS receiver (LC76G) with elrebo-de/generic_uart
 */

#include <string>
#include <vector>
#include <sstream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "generic_uart.hpp"
#include "esp_log.h"

static const char *tag = "LC76G GNSS receiver";

extern "C" void app_main(void)
{
    vTaskDelay(500 / portTICK_PERIOD_MS); // delay 0.5 seconds

    ESP_LOGI(tag, "LC76G GNSS receiver Example Program");

    // set UART configuration
    const uart_port_t uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {
           .allow_pd = 0,
           .backup_before_sleep = 0,
        },
    };
    const int uart_tx_pin = 26;
    const int uart_rx_pin = 32;

    GenericUart gnssUart(
                 "LC76G GNSS UART",
                 uart_num,
                 &uart_config,
                 uart_tx_pin,
                 uart_rx_pin);

    ESP_LOGI(tag, "wait for GNSS messages");

    uint8_t* data = (uint8_t*) malloc(2048);

    while (1) {
        // reads data from UART with a timeout 0f 100 ms
        int len = uart_read_bytes(uart_num, data, 2048 - 1, 100 / portTICK_PERIOD_MS);

        // every second the LC76G sends data (less than 2k)
        if (len > 0) {
            data[len] = '\0';

            std::string allMessages((char *)data);

            // parse message $GNVTG
            std::size_t position = allMessages.find("$GNVTG");
            if(position != std::string::npos) {
                std::stringstream gnvtg(allMessages.substr(position));

                std::string segment;
                std::vector<std::string> seglist;

                while(std::getline(gnvtg, segment, ','))
                {
                   seglist.push_back(segment);
                }

                std::string trueTrackAngle(seglist[1]);
                std::string magneticTrackAngle(seglist[3]);
                std::string speedOverGroundKnots(seglist[5]);
                std::string speedOverGroundKilometers(seglist[7]);

                // print data
                ESP_LOGI(tag,
                         "$GNVTG\n TrueTrackAngle: %s,\n MagneticTrackAngle: %s,\n SpeedOverGround(Knots): %s,\n SpeedOverGround(km/h): %s",
                         trueTrackAngle.c_str(),
                         magneticTrackAngle.c_str(),
                         speedOverGroundKnots.c_str(),
                         speedOverGroundKilometers.c_str());
            }

            // parse message $GNRMC
            position = allMessages.find("$GNRMC");
            if(position != std::string::npos) {
                std::stringstream gnrmc(allMessages.substr(position));

                std::string segment;
                std::vector<std::string> seglist;

                while(std::getline(gnrmc, segment, ','))
                {
                   seglist.push_back(segment);
                }

                std::string time(seglist[1]);
                std::string validity(seglist[2]);
                std::string latitude(seglist[3].append(",").append(seglist[4]));
                std::string longitude(seglist[5].append(",").append(seglist[6]));
                std::string speedOverGroundKnots(seglist[7]);
                std::string trueTrackAngle(seglist[8]);
                std::string date(seglist[9]);
                std::string variation(seglist[10].append(",").append(seglist[11]));

                // print data
                ESP_LOGI(tag,
                         "$GNRMC\n Time: %s,\n Validity: %s,\n Latitude: %s,\n Longitude: %s,\n SpeedOverGround(Knots): %s,\n TrueTrackAngle: %s,\n Date: %s,\n Variation: %s",
                         time.c_str(),
                         validity.c_str(),
                         latitude.c_str(),
                         longitude.c_str(),
                         speedOverGroundKnots.c_str(),
                         trueTrackAngle.c_str(),
                         date.c_str(),
                         variation.c_str());
            }

            // parse message $GNGGA
            position = allMessages.find("$GNGGA");
            if(position != std::string::npos) {
                std::stringstream gngga(allMessages.substr(position));

                std::string segment;
                std::vector<std::string> seglist;

                while(std::getline(gngga, segment, ','))
                {
                   seglist.push_back(segment);
                }

                std::string time(seglist[1]);
                std::string latitude(seglist[2].append(",").append(seglist[3]));
                std::string longitude(seglist[4].append(",").append(seglist[5]));
                std::string quality(seglist[6]);
                std::string numberOfSatellites(seglist[7]);
                std::string hdop(seglist[8]);
                std::string orthometricHeight(seglist[9]);
                std::string geoidSeparation(seglist[11]);

                // print data
                ESP_LOGI(tag,
                         "$GNGGA\n Time: %s,\n Latitude: %s,\n Longitude: %s,\n Quality: %s,\n NrOfSatellites: %s,\n HDOP: %s,\n OrthometricHeight: %s,\n GeoidSeparation: %s",
                         time.c_str(),
                         latitude.c_str(),
                         longitude.c_str(),
                         quality.c_str(),
                         numberOfSatellites.c_str(),
                         hdop.c_str(),
                         orthometricHeight.c_str(),
                         geoidSeparation.c_str());
            }
        }
    }
    free(data);
}
