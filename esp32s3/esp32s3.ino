#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// 创建PCA9685对象，默认I2C地址为0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// 定义舵机通道
#define SERVO_CH12 12
#define SERVO_CH13 13
#define SERVO_CH14 14
#define SERVO_CH15 15

// 舵机脉冲宽度设置（根据舵机型号调整）
#define SERVOMIN  102  // 对应0.5ms脉冲宽度（0度）
#define SERVOMAX  512  // 对应2.5ms脉冲宽度（180度）

// ESP32-S3的I2C引脚（使用您能看到的引脚）
#define SDA_PIN 21
#define SCL_PIN 20 // 确保这里是 20 (或您实际连接的引脚)

void setup() {
  // *** 修改点：使用 Serial0 ***
  Serial0.begin(115200); 
  Serial0.println("PCA9685舵机控制测试 (使用UART口)");

  // 初始化I2C (使用您定义的引脚)
  Wire.begin(SDA_PIN, SCL_PIN);

  // 初始化PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);  // 舵机工作频率为50Hz

  delay(100);
  Serial0.println("PCA9685初始化完成"); // *** 修改点 ***
}

void loop() {
  Serial0.println("舵机移动到0度"); // *** 修改点 ***
  setServoAngle(SERVO_CH12, 0);
  setServoAngle(SERVO_CH13, 0);
  setServoAngle(SERVO_CH14, 0);
  setServoAngle(SERVO_CH15, 0);
  delay(1000);

  Serial0.println("舵机移动到90度"); // *** 修改点 ***
  setServoAngle(SERVO_CH12, 90);
  setServoAngle(SERVO_CH13, 90);
  setServoAngle(SERVO_CH14, 90);
  setServoAngle(SERVO_CH15, 90);
  delay(1000);

  Serial0.println("舵机移动到180度"); // *** 修改点 ***
  setServoAngle(SERVO_CH12, 180);
  setServoAngle(SERVO_CH13, 180);
  setServoAngle(SERVO_CH14, 180);
  setServoAngle(SERVO_CH15, 180);
  delay(1000);

  Serial0.println("舵机移动到90度"); // *** 修改点 ***
  setServoAngle(SERVO_CH12, 90);
  setServoAngle(SERVO_CH13, 90);
  setServoAngle(SERVO_CH14, 90);
  setServoAngle(SERVO_CH15, 90);
  delay(1000);
}

// 设置舵机角度（0-180度）
void setServoAngle(uint8_t channel, uint8_t angle) {
  // 限制角度范围
  if (angle > 180) angle = 180;

  // 将角度转换为脉冲宽度
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);

  // 设置PWM
  pwm.setPWM(channel, 0, pulse);
}