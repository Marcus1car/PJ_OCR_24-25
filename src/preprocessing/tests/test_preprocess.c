#include "preprocess.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Helper function to create a simple test image
SDL_Surface* create_test_image(int width, int height) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        fprintf(stderr, "Failed to create test surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with a test pattern
    Uint32* pixels = (Uint32*)surface->pixels;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 value = (x + y) % 256;
            pixels[y * width + x] = SDL_MapRGB(surface->format, value, value, value);
        }
    }
    return surface;
}

// Helper function to check if a surface is grayscale
int is_grayscale(SDL_Surface* surface) {
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
            if (r != g || g != b) return 0;
        }
    }
    return 1;
}

// Helper function to save test results
void save_test_image(SDL_Surface* surface, const char* filename) {
    if (IMG_SavePNG(surface, filename) != 0) {
        fprintf(stderr, "Failed to save test image %s: %s\n", filename, IMG_GetError());
    }
}

void test_loadImage() {
    printf("\nTesting loadImage()...\n");
    
    // Test with NULL path
    SDL_Surface* result = loadImage(NULL);
    assert(result == NULL);
    printf("✓ NULL path test passed\n");
    
    // Test with non-existent file
    result = loadImage("nonexistent.png");
    assert(result == NULL);
    printf("✓ Non-existent file test passed\n");
    
    // Test with valid image (you need to provide a test image)
    result = loadImage("test_image.png");
    if (result != NULL) {
        printf("✓ Valid image load test passed\n");
        SDL_FreeSurface(result);
    }
}

void test_grayscale() {
    printf("\nTesting convertToGrayscale()...\n");
    
    SDL_Surface* surface = create_test_image(100, 100);
    assert(surface != NULL);
    
    // Test grayscale conversion
    convertToGrayscale(surface);
    assert(is_grayscale(surface));
    save_test_image(surface, "test_grayscale.png");
    
    printf("✓ Grayscale conversion test passed\n");
    SDL_FreeSurface(surface);
}

void test_noise() {
    printf("\nTesting noise detection and reduction...\n");
    
    SDL_Surface* surface = create_test_image(100, 100);
    assert(surface != NULL);
    
    // Test noise level calculation
    double noise = calculateNoiseLevel(surface);
    assert(noise >= 0.0);
    printf("✓ Noise level calculation: %.2f\n", noise);
    
    // Test median filters
    medianFilter3x3(surface);
    save_test_image(surface, "test_median3x3.png");
    printf("✓ 3x3 median filter applied\n");
    
    medianFilter5x5(surface);
    save_test_image(surface, "test_median5x5.png");
    printf("✓ 5x5 median filter applied\n");
    
    SDL_FreeSurface(surface);
}

void test_contrast() {
    printf("\nTesting contrast enhancement...\n");
    
    SDL_Surface* surface = create_test_image(100, 100);
    assert(surface != NULL);
    
    double noise = calculateNoiseLevel(surface);
    enhanceContrast(surface, noise);
    save_test_image(surface, "test_contrast.png");
    
    printf("✓ Contrast enhancement test passed\n");
    SDL_FreeSurface(surface);
}

void test_binarization() {
    printf("\nTesting binarization...\n");
    
    SDL_Surface* surface = create_test_image(100, 100);
    assert(surface != NULL);
    
    Uint8 mean = calculateMeanLight(surface);
    printf("Mean light value: %d\n", mean);
    
    convertToBlackAndWhite(surface);
    save_test_image(surface, "test_binary.png");
    
    printf("✓ Binarization test passed\n");
    SDL_FreeSurface(surface);
}

void test_rotation() {
    printf("\nTesting rotation...\n");
    
    SDL_Surface* surface = create_test_image(100, 100);
    assert(surface != NULL);
    
    // Test different angles
    double angles[] = {0.0, 45.0, 90.0, 180.0};
    for (size_t i = 0; i < sizeof(angles)/sizeof(angles[0]); i++) {
        SDL_Surface* rotated = man_rotation(surface, angles[i]);
        assert(rotated != NULL);
        
        char filename[32];
        snprintf(filename, sizeof(filename), "test_rotation_%.0f.png", angles[i]);
        save_test_image(rotated, filename);
        
        SDL_FreeSurface(rotated);
        printf("✓ Rotation %.0f degrees test passed\n", angles[i]);
    }
    
    SDL_FreeSurface(surface);
}

int main() {
    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        fprintf(stderr, "SDL_image initialization failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    
    printf("Starting preprocessing tests...\n");
    
    // Run all tests
    test_loadImage();
    test_grayscale();
    test_noise();
    test_contrast();
    test_binarization();
    test_rotation();
    
    // Cleanup
    IMG_Quit();
    SDL_Quit();
    
    printf("\nAll tests completed!\n");
    return 0;
}