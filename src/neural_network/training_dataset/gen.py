import random
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps
import numpy as np
import os

# Define the alphabet and the image size
alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" + "ABCDEFGHIJKLMNOPQRSTUVWXYZ".lower()
image_size = (32, 32)

# Directory to save generated images
output_dir = os.path.join("images", "mix")
os.makedirs(output_dir, exist_ok=True)

# List your font files here
FONT_DIR = "./fonts_new"
font_files = [os.path.join(FONT_DIR, font) for font in os.listdir(FONT_DIR)]

def add_random_variation(image):
    # Convert to grayscale if not already
    image = image.convert("L")
    
    # 1. Random Rotation
    angle = random.uniform(-15, 15)  # Rotate between -15 and 15 degrees
    image = image.rotate(angle, fillcolor="white")
    
    # # 2. Gaussian Blur
    if random.random() < 0.5:  # 50% chance to apply blur
         image = image.filter(ImageFilter.GaussianBlur(radius=random.uniform(0.5, 1.3)))
    
    # # 3. Noise

    # # 4. Artifacts: randomly erase a small region
    # if random.random() < 0.3:  # 30% chance to add artifacts
    #     draw = ImageDraw.Draw(image)
    #     rect_x1 = random.randint(0, 30)
    #     rect_y1 = random.randint(0, 30)
    #     rect_x2 = rect_x1 + random.randint(2, 8)
    #     rect_y2 = rect_y1 + random.randint(2, 8)
    #     draw.rectangle([rect_x1, rect_y1, rect_x2, rect_y2], fill="white")
        
    if random.random() < 0.5:  # 50% chance to add noise
        noise = np.random.normal(0, 25, (32, 32))  # Mean = 0, Stddev = 25
        noise_img = np.array(image).astype(np.float32) + noise
        noise_img = np.clip(noise_img, 0, 255).astype(np.uint8)
        image = Image.fromarray(noise_img)
    
    return image

def find_max_font_size(letter, max_font_size, image_size, font_path):
    font_size = max_font_size
    while font_size > 0:
        # Load the font with the current size
        font = ImageFont.truetype(font_path, font_size)
        # Create a temporary image to calculate text size
        temp_image = Image.new("L", image_size, color="white")
        temp_draw = ImageDraw.Draw(temp_image)
        # Get the bounding box for the letter
        text_bbox = temp_draw.textbbox((0, 0), letter, font=font)
        text_width = text_bbox[2] - text_bbox[0]
        text_height = text_bbox[3] - text_bbox[1]

        # Check if the letter fits within the image boundaries
        if text_width <= image_size[0] and text_height <= image_size[1]:
            return font_size  # Return the largest size that fits
        font_size -= 1  # Decrease font size if it doesn't fit

    return 1  # Minimum font size in case no fit is found
        
for font_path in font_files:
    font_name = os.path.splitext(os.path.basename(font_path))[0].lower()
    
    # Load the font at a size suitable for 28x28 images
    # font_size = 24  # Adjust this if needed
    # print(f"Using font: {font_name}")
    # font = ImageFont.truetype(font_path, font_size)
    
    for letter in alphabet:
        if letter.islower():
            continue
        optimal_font_size = find_max_font_size(letter, 50, image_size, font_path)-16
        font = ImageFont.truetype(font_path, optimal_font_size)
        # Create a blank image with a white background
        image = Image.new("L", image_size, color="white")
        draw = ImageDraw.Draw(image)
        
        # Calculate the position to center the text
        text_bbox = draw.textbbox((0,0), letter, font=font, )
        text_width = text_bbox[2] - text_bbox[0]
        text_height = text_bbox[3] - text_bbox[1]

        # Calculate the position to center the text
        text_x = (image_size[0] - text_width) // 2
        text_y = (image_size[1] - text_height) // 2 
        offset_x, offset_y = font.getoffset(letter)
        text_x -= offset_x // 2
        text_y -= offset_y // 2

        # Draw the letter onto the image
        draw.text((text_x, text_y), letter, font=font, fill="black", )

        # Save the image with a name that includes the font name and letter
        if letter.islower():
            image_filename = f"{letter.lower()}_low_{font_name}.png"
        else:
            image_filename = f"{letter.lower()}_cap_{font_name}.png"
        image_path = os.path.join(output_dir, image_filename)
        #image.save(image_path)
        for i in range(10):
            varied_image = add_random_variation(image)
            if letter.islower():
                varied_filename = f"{letter.lower()}_low_{font_name}_variation_{i}_1.png"
            else:
                varied_filename = f"{letter.lower()}_cap_{font_name}_variation_{i}_1.png"
            varied_image.save(os.path.join(output_dir, varied_filename))
            print(f"Saved {varied_filename}")
        
        print(f"Saved {image_path}")
