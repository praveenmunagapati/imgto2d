import cv2
import numpy as np
import os

img = np.zeros((200, 200, 3), dtype=np.uint8)
img[:] = 255 # White background
cv2.circle(img, (100, 100), 50, (0, 0, 0), -1) # Black circle
cv2.rectangle(img, (20, 20), (180, 180), (128, 128, 128), 5) # Gray border
cv2.putText(img, 'TEST', (50, 105), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

if not os.path.exists('test_outputs'):
    os.mkdir('test_outputs')

cv2.imwrite('test_outputs/test_input.png', img)
print("Created test_outputs/test_input.png")
