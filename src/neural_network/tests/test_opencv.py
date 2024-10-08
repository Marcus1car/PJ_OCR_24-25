import cv2

# Load the image
image = cv2.imread('path_to_your_image.png')
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply thresholding
_, thresh = cv2.threshold(gray, 150, 255, cv2.THRESH_BINARY_INV)

# Find contours
contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Loop through contours and identify different zones
for contour in contours:
    x, y, w, h = cv2.boundingRect(contour)

    # Heuristic for identifying zones:
    if w > 300 and h > 300:  # Large area likely to be grid
        cv2.rectangle(image, (x, y), (x + w, y + h), (0, 0, 255), 2)  # Red for grid border
        
        # Subdivide the grid into cells
        cell_width = w // 10  # Assuming 10x10 grid
        cell_height = h // 10
        for i in range(10):
            for j in range(10):
                cell_x = x + j * cell_width
                cell_y = y + i * cell_height
                cv2.rectangle(image, (cell_x, cell_y), (cell_x + cell_width, cell_y + cell_height), (0, 255, 0), 1)  # Green for letters

    elif w > 100 and h > 300:  # Vertical area likely to be the word list
        cv2.rectangle(image, (x, y), (x + w, y + h), (0, 165, 255), 2)  # Orange for word list border
        
        # Subdivide the word list into words
        word_height = h // 10  # Assuming 10 words
        for i in range(10):
            word_y = y + i * word_height
            cv2.rectangle(image, (x, word_y), (x + w, word_y + word_height), (128, 0, 128), 1)  # Purple for words
            
            # Subdivide each word into characters
            char_width = w // 10  # Estimate character width
            for j in range(10):
                char_x = x + j * char_width
                cv2.rectangle(image, (char_x, word_y), (char_x + char_width, word_y + word_height), (255, 0, 0), 1)  # Blue for characters

# Display the result
cv2.imshow('Detected Zones', image)
cv2.waitKey(0)
cv2.destroyAllWindows()

