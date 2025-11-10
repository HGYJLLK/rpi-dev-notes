#!/usr/bin/env python3
"""
树莓派3B控制舵机示例程序
使用gpiozero库控制舵机
"""

from gpiozero import Servo
from time import sleep

# 初始化舵机，连接到GPIO 17引脚
# min_pulse_width和max_pulse_width根据舵机型号调整
# 标准舵机通常是0.5ms-2.5ms
servo = Servo(17, min_pulse_width=0.5/1000, max_pulse_width=2.5/1000)

def main():
    """主函数：演示舵机控制"""

    print("舵机控制程序启动")
    print("GPIO引脚: 17")

    try:
        while True:
            # 移动到最小位置（-1）
            print("移动到 -1 位置（0度）")
            servo.min()
            sleep(1)

            # 移动到中间位置（0）
            print("移动到 0 位置（90度）")
            servo.mid()
            sleep(1)

            # 移动到最大位置（1）
            print("移动到 1 位置（180度）")
            servo.max()
            sleep(1)

            # 移动到自定义位置（-0.5到1之间）
            print("移动到 -0.5 位置（45度）")
            servo.value = -0.5
            sleep(1)

            print("移动到 0.5 位置（135度）")
            servo.value = 0.5
            sleep(1)

    except KeyboardInterrupt:
        print("\n程序被用户中断")
    finally:
        # 清理资源
        servo.close()
        print("舵机资源已释放")

if __name__ == "__main__":
    main()
