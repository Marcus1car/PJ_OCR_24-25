import os
import shutil
from pathlib import Path
import cv2 
SOURCE_DIR = "./lvl3_1"
DEST_DIR = "./renamed"

os.makedirs(DEST_DIR, exist_ok=True)
IMAGE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".gif"}

def get_images_from_directory(directory):
    return [file for file in Path(directory).iterdir() if file.suffix.lower() in IMAGE_EXTENSIONS]

def main():
    images = get_images_from_directory(SOURCE_DIR)
    
    if not images:
        print(f"No images found in {SOURCE_DIR}.")
        return

    for image_path in images:
        image = cv2.imread(str(image_path))
        cv2.imshow("Image Viewer", image)

        print(f"Press a key for '{image_path.name}', or '7' to quit:")
        key = cv2.waitKey(0) 
        if key == ord('7'): 
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
