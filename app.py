import threading
import cv2
from flask import Flask, Response, render_template, request, jsonify
from adafruit_servokit import ServoKit
import config

app = Flask(__name__)

# ── 摄像头 ────────────────────────────────────────
camera = cv2.VideoCapture(config.CAMERA_INDEX)
camera.set(cv2.CAP_PROP_FRAME_WIDTH,  config.CAMERA_WIDTH)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, config.CAMERA_HEIGHT)
camera.set(cv2.CAP_PROP_FPS,          config.CAMERA_FPS)

_frame_lock   = threading.Lock()
_current_frame = None

def _capture_loop():
    global _current_frame
    while True:
        ret, frame = camera.read()
        if ret:
            with _frame_lock:
                _current_frame = frame

def _generate_mjpeg():
    while True:
        with _frame_lock:
            if _current_frame is None:
                continue
            ret, buf = cv2.imencode('.jpg', _current_frame,
                                    [cv2.IMWRITE_JPEG_QUALITY, 75])
        if ret:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n'
                   + buf.tobytes() + b'\r\n')

# ── PCA9685 舵机 ──────────────────────────────────
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
    t = threading.Thread(target=_capture_loop, daemon=True)
    t.start()
    app.run(host=config.SERVER_HOST, port=config.SERVER_PORT, threaded=True)
