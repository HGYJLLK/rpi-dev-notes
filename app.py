import io
import threading
import cv2
from flask import Flask, Response, render_template, request, jsonify
from picamera2 import Picamera2
from adafruit_servokit import ServoKit
import config

app = Flask(__name__)

# ── 摄像头 (CSI, picamera2) ───────────────────────
picam = Picamera2()
picam.configure(picam.create_video_configuration(
    main={"size": (config.CAMERA_WIDTH, config.CAMERA_HEIGHT), "format": "RGB888"}
))
picam.start()

def _generate_mjpeg():
    while True:
        frame = picam.capture_array()
        frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        frame_bgr = cv2.rotate(frame_bgr, cv2.ROTATE_180)
        ret, buf = cv2.imencode('.jpg', frame_bgr, [cv2.IMWRITE_JPEG_QUALITY, 75])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n'
                   + buf.tobytes() + b'\r\n')

# ── PCA9685 舵机 (I2C bus 1, addr 0x40) ──────────
kit   = ServoKit(channels=16)
servo = kit.servo[config.SERVO_CHANNELS["servo_1"]]
servo.actuation_range = config.SERVO_RANGE_DEG
servo.set_pulse_width_range(config.SERVO_MIN_US, config.SERVO_MAX_US)

current_angle = config.SERVO_CENTER
servo.angle   = current_angle

# ── 路由 ──────────────────────────────────────────
@app.route('/')
def index():
    return render_template('index.html',
                           angle=current_angle,
                           servo_max=config.SERVO_RANGE_DEG)

@app.route('/video_feed')
def video_feed():
    return Response(_generate_mjpeg(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/servo', methods=['POST'])
def set_servo():
    global current_angle
    data  = request.get_json(force=True)
    angle = int(data.get('angle', config.SERVO_CENTER))
    angle = max(0, min(config.SERVO_RANGE_DEG, angle))
    servo.angle   = angle
    current_angle = angle
    return jsonify({'angle': angle})

# ── 启动 ──────────────────────────────────────────
if __name__ == '__main__':
    app.run(host=config.SERVER_HOST, port=config.SERVER_PORT, threaded=True)
