
#include "esp_camera.h"
#include "USB.h"      // 必须首先包含 USB.h
#include "UVC.h"      // 再包含 UVC.h

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5
#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-S3 UVC 摄像头启动...");

  camera_config_t config;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;

  // 摄像头配置
  config.xclk_freq_hz = 20000000;
  config.ledc_timer = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;

  // 关键：必须使用 JPEG 格式！UVC 传输的是 JPEG 图像
  config.pixel_format = PIXFORMAT_JPEG;
  
  // S3 通常没有 PSRAM，所以我们使用 DRAM。
  // 如果你的 S3 板子有 PSRAM，可以将 fb_location 改为 CAMERA_FB_IN_PSRAM
  // 并增加 fb_count (例如 2 或 3)
  config.frame_size = FRAMESIZE_VGA; // 640x480
  config.jpeg_quality = 12;         // 质量 (0-63, 越低质量越高)
  config.fb_count = 1;              // DRAM 只能放1帧
  config.fb_location = CAMERA_FB_IN_DRAM;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  // 1. 初始化摄像头
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("摄像头初始化失败，错误 0x%x\n", err);
    Serial.println("请检查：1. 引脚定义是否正确 2. 摄像头是否插好 3. 摄像头硬件是否完好");
    return;
  }
  
  // 2. 启动 UVC USB 摄像头
  // 这会告诉电脑“我是一个摄像头”
  // 参数必须与 config.frame_size 一致
  UVC.begin(FRAMESIZE_VGA, 30);
  
  Serial.println("UVC 服务已启动。请将 USB 线切换到 OTG 端口！");
  Serial.println("如果电脑没有识别，请检查 '工具' -> 'USB Mode' 设置。");
}

void loop() {
  // 3. 检查 UVC (电脑) 是否准备好接收数据
  if (!UVC.available()) {
    delay(5); // 等待电脑准备好
    return;
  }

  // 4. 从摄像头获取一帧图像
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("获取帧失败");
    return;
  }

  // 5. 将这一帧 (JPEG 图像) 通过 USB 发送给电脑
  UVC.write(fb->buf, fb->len);

  // 6. 释放帧内存，准备拍下一张
  esp_camera_fb_return(fb);
}