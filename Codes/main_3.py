import io
import picamera
from flask import Flask, Response

app = Flask(_name_)

def generate_frames():
    with picamera.PiCamera() as camera:
        camera.resolution = (416, 416)
        camera.framerate = 30
        stream = io.BytesIO()

        for _ in camera.capture_continuous(stream, 'jpeg', use_video_port=True):
            stream.seek(0)
            yield b'--frame\r\nContent-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n'
            stream.seek(0)
            stream.truncate()

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if _name_ == '_main_':
    app.run(host='0.0.0.0', port=5000, threaded=True)