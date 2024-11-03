#include "../headers/heapsort.h"

// Function to swap the the position of two elements
void swap(CCTuple *a, CCTuple *b) {
    CCTuple temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(CCTuple arr[], size_t size, int i) {
    // Find largest among root, left child and right child
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < (int)size && arr[left].nb_pixels > arr[largest].nb_pixels)
        largest = left;

    if (right < (int)size && arr[right].nb_pixels > arr[largest].nb_pixels)
        largest = right;

    // Swap and continue heapifying if root is not largest
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, size, largest);
    }
}

// Main function to do heap sort
void heapSort(CCTuple arr[], size_t size) {
    // Build max heap
    for (int i = (int)size / 2 - 1; i >= 0; i--)
        heapify(arr, size, i);

    // Heap sort
    for (int i = (int)size - 1; i >= 0; i--) {
        swap(&arr[0], &arr[i]);

        // Heapify root element to get highest element at root again
        heapify(arr, (size_t)i, 0);
    }
}

// Print an array
void printCCTupleArr(CCTuple arr[], size_t size) {
    for (size_t i = 0; i < size; ++i)
        printf("%zu ", arr[i].nb_pixels);
    printf("\n");
}