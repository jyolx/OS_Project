// User program to create a file and rename it from old_name.txt to new_name.txt

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main()
{
    printf("Creating file old_name.txt...\n");

    int fd = open("old_name.txt", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("Error creating file\n");
        exit(1);
    }

    // Write something into the file
    printf("Writing to file...\n");
    write(fd, "This is a test file for renaming.\n", 35);
    close(fd);

    // Rename the file
    printf("Renaming file old_name.txt to new_name.txt...\n");
    if (rename("old_name.txt", "new_name.txt") < 0) {
        printf("Error renaming file\n");
        exit(1);
    }

    // Check if the file exists after renaming
    printf("Opening renamed file new_name.txt...\n");
    fd = open("new_name.txt", 0);
    if (fd < 0) {
        printf("Error opening renamed file\n");
        exit(1);
    }

    printf("File renamed successfully to new_name.txt\n");
    close(fd);
    
    exit(0);
}
