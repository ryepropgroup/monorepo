import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from PIL import Image
import numpy as np

# Load and resize the image
image_path = "pid.png"
img = Image.open(image_path)
img_resized = img.resize((1280, 720), Image.LANCZOS)

# Convert resized image to array for matplotlib
img_resized_array = np.array(img_resized)

# Display the image
plt.figure(figsize=(12.8, 7.2))  # Size in inches (1280x720 pixels)
plt.imshow(img_resized_array)
plt.axis('on')  # Show axes to help determine coordinates
plt.show()
