# Raspberry Pi 5 - 项目配置
# 主机: hym.lan  IP: 192.168.10.148

# ── 网络 ──────────────────────────────────────────
HOST = "192.168.10.149"
HOSTNAME = "hym.lan"
SSH_USER = "hym"
SSH_PASS = ""  # 不提交密码

# ── I2C 舵机驱动板 (PCA9685) ──────────────────────
PCA9685_I2C_ADDR = 0x40       # 默认地址
PCA9685_FREQ_HZ  = 50         # 舵机 PWM 频率

# 舵机通道定义
SERVO_CHANNELS = {
    "servo_1": 0,             # 0号通道 (已接舵机)
}

# 舵机参数 (270° 舵机)
SERVO_RANGE_DEG = 270
SERVO_MIN_US    = 500
SERVO_MAX_US    = 2500
SERVO_CENTER    = 135   # 默认居中位置

# ── Web 服务 ──────────────────────────────────────
SERVER_HOST = "0.0.0.0"
SERVER_PORT = 5000

# ── 摄像头 ────────────────────────────────────────
CAMERA_INDEX  = 0             # /dev/video0
CAMERA_WIDTH  = 640
CAMERA_HEIGHT = 480
CAMERA_FPS    = 30
