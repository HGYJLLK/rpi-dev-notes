# Raspberry Pi 5 - 项目配置
# 主机: hym.lan  IP: 192.168.10.148

# ── 网络 ──────────────────────────────────────────
HOST = "192.168.10.148"
HOSTNAME = "hym.lan"
SSH_USER = "hym"
SSH_PASS = "1234567890"  # 备用: 2333

# ── I2C 舵机驱动板 (PCA9685) ──────────────────────
PCA9685_I2C_ADDR = 0x40       # 默认地址
PCA9685_FREQ_HZ  = 50         # 舵机 PWM 频率

# 舵机通道定义
SERVO_CHANNELS = {
    "servo_1": 1,             # 1号通道 (已接舵机)
}

# 舵机 PWM 脉宽范围 (单位: 微秒)
SERVO_MIN_US = 500
SERVO_MAX_US = 2500

# ── 摄像头 ────────────────────────────────────────
CAMERA_INDEX  = 0             # /dev/video0
CAMERA_WIDTH  = 640
CAMERA_HEIGHT = 480
CAMERA_FPS    = 30
