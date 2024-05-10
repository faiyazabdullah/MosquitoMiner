import cv2
import time
import socket
from ultralytics import YOLO

# Load YOLO model
model = YOLO('best.pt')

# URL of the video feed
url = "http://192.168.62.109:5000/video_feed"

UDP_IP = "192.168.62.109"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


# Open the video stream
cap = cv2.VideoCapture(url)

# # # Initialize the webcam
# cap = cv2.VideoCapture(0)

# Check if the webcam is opened correctly
if not cap.isOpened():
    raise IOError("Cannot open webcam")

# Define the vertical line positions
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
line1_x = width // 3
line2_x = 2 * width // 3

# Frame rate calculation initialization
fps = 0
frame_count = 0
start_time = time.time()
# Initialize frame counter and skip factor
frame_counter = 0
skip_factor = 5  # Process every third frame, adjust as needed

count=0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Increment frame counter
    frame_counter += 1

    # Skip frames if necessary
    if frame_counter % skip_factor != 0:
        # print("This is not in model")
           # Calculate and display FPS
        frame_count += 1
        if frame_count >= 10:
            end_time = time.time()
            fps = frame_count / (end_time - start_time)
            frame_count = 0
            start_time = time.time()
        cv2.putText(frame, f'FPS: {fps:.2f}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.imshow('Object Detection', frame) 
        continue

    # Reset frame counter if it exceeds the skip factor
    if frame_counter >= skip_factor:
        frame_counter = 0

    frame = cv2.flip(frame, 0)
    # Draw two vertical lines
    cv2.line(frame, (line1_x, 0), (line1_x, frame.shape[0]), (0, 255, 0), 2)
    cv2.line(frame, (line2_x, 0), (line2_x, frame.shape[0]), (0, 255, 0), 2)

    # Perform object detection
    results = model.predict(frame)

    # Draw bounding boxes and center points only for bottles or cups
    for box in results[0].boxes:
        class_id = int(box.cls[0])
        class_name = model.names[class_id]
        confidence = box.conf[0]  # Calculate confidence here
        
        if class_name.lower() in ['bottle', 'cup'] and confidence > 0.3:  # Check confidence here
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)
            cx = (x1 + x2) // 2
            cy = (y1 + y2) // 2
            cv2.circle(frame, (cx, cy), 5, (0, 255, 255), -1)
            count=0
            
            # Classify position based on the vertical lines
            if cx < line1_x:
                position = "right"
                sock.sendto(position.encode(), (UDP_IP, UDP_PORT))
            elif cx > line2_x:
                position = "left"
                sock.sendto(position.encode(), (UDP_IP, UDP_PORT))
            else:
                position = "between"
                sock.sendto(position.encode(), (UDP_IP, UDP_PORT))
            
            # Draw position info
            cv2.putText(frame, f'{class_name}: {confidence:.2f} ({position})', (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1, cv2.LINE_AA)
        else:
            count +=1
            if count>2:
                nothing="NO"
                sock.sendto(nothing.encode(), (UDP_IP, UDP_PORT))
    # Calculate and display FPS
    frame_count += 1
    if frame_count >= 10:
        end_time = time.time()
        fps = frame_count / (end_time - start_time)
        frame_count = 0
        start_time = time.time()

    cv2.putText(frame, f'FPS: {fps:.2f}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2, cv2.LINE_AA)

    # Display the frame
    cv2.imshow('Object Detection', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam and close the window
cap.release()
cv2.destroyAllWindows()
