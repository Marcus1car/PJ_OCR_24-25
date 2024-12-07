import os
import shutil
from pathlib import Path
import cv2  # OpenCV for displaying images

# Source directory containing images
SOURCE_DIR = "./lvl3_1"
# Destination directory to save categorized images
DEST_DIR = "./renamed"

# Ensure the destination directory exists
os.makedirs(DEST_DIR, exist_ok=True)

# Supported image extensions
IMAGE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".gif"}

def get_images_from_directory(directory):
    """Get a list of image file paths in a directory."""
    return [file for file in Path(directory).iterdir() if file.suffix.lower() in IMAGE_EXTENSIONS]

def main():
    images = get_images_from_directory(SOURCE_DIR)
    
    if not images:
        print(f"No images found in {SOURCE_DIR}.")
        return

    for image_path in images:
        # Read and display the image
        image = cv2.imread(str(image_path))
        cv2.imshow("Image Viewer", image)

        print(f"Press a key for '{image_path.name}', or 'q' to quit:")
        key = cv2.waitKey(0)  # Wait for a key press

        if key == ord('7'):  # Quit on 'q'
            print("Exiting.")
            break
        pressed_char = chr(key)
        
        if pressed_char.isalnum():
            new_name = f"{pressed_char}_{image_path.name}"
            new_path = Path(DEST_DIR) / new_name
            shutil.copy(str(image_path), str(new_path))
            print(f"Copied to: {new_path}")
        else:
            print("Invalid key pressed. Skipping.")

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()