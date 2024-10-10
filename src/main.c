#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main() {
    const char *directory = ".";  // Directory path, "." for the current directory
    struct dirent *entry;
    
    // Open the directory
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // Read and print each entry
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    // Close the directory
    closedir(dir);
    return EXIT_SUCCESS;
}