// #include "esp_chip_info.h"
#include <M5Unified.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// #include <esp_system.h>
#include <esp_log.h>

#define LV_TICK_PERIOD_MS 1
#define INFO_TAG "Info"

void print_inf() {
    M5.Display.printf("AC In curr: %.02fmA\n", M5.Power.Axp192.getACINCurrent());
    M5.Display.printf("AC In volt: %.02fV\n", M5.Power.Axp192.getACINVolatge());
    // M5.Display.printf("Bat: %.02f\n", M5.Power.Axp192.getBatteryPower());
    M5.Display.printf("Bat volt: %.02fV\n", M5.Power.Axp192.getBatteryVoltage());
    M5.Display.printf("Bat discharge curr: %.02fmA\n", M5.Power.Axp192.getBatteryDischargeCurrent());
    M5.Display.printf("Temp: %.01f\n", M5.Power.Axp192.getInternalTemperature());
}

void display_info(bool refr = false) {
    static int prev_battery = INT_MAX;
    int battery = M5.Power.getBatteryLevel();

    if (prev_battery != battery) {
        prev_battery = battery;
        refr = true;
    }

    if (refr) {
        M5.Display.startWrite();
        M5.Display.setCursor(245,0);
        M5.Display.print("Bat:");

        if (battery >= 0) {
            if (battery == 100) {
                M5.Display.printf("%03d\n", battery);
            } else {
                M5.Display.printf("%02d\n", battery);
            }
        } else {
            M5.Display.print("No battery");
        }
        M5.Display.endWrite();

        print_inf();
    }
}

void button_ctrl(void *pvParameter) {
    for (;;) {
        vTaskDelay(1);
        M5.update();
        if (M5.BtnA.wasPressed()) {
            M5.Display.println("BtnA pressed");
            ESP_LOGI(INFO_TAG, "%d\n", M5.BtnA.wasPressed());
        }

        if (M5.BtnB.wasPressed()) {
            M5.Display.println("BtnB pressed");
            ESP_LOGI(INFO_TAG, "%d\n", M5.BtnB.wasPressed());
            M5.Display.clearDisplay();
            M5.Display.setCursor(1,0);
            display_info(true);
        }

        if (M5.BtnC.wasPressed()) {
            M5.Display.println("BtnC pressed");
            ESP_LOGI(INFO_TAG, "%d\n", M5.BtnC.wasPressed());
            M5.Power.setLed(1);
        }

        if (M5.BtnPWR.wasClicked()) {
            M5.Display.println("Powering off device");
            M5.Power.powerOff();
        }

        display_info();
    }
}

extern "C" void app_main()
{
    M5.begin();
    vTaskDelay(1000);

    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(1.5);
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.println("EsPwn32 Initialized.");
    ESP_LOGI(INFO_TAG, "Boot successfully");

    xTaskCreate(&button_ctrl, "button", 2048, NULL, 1, NULL);

}
