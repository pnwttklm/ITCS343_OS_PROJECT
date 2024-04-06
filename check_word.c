#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *file;
    char word[100];
    char filename[] = "list.txt";
    int found = 0;

    // Open the file containing the word list
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file %s.\n", filename);
        return 1;
    }

    // Get the word to search for from user input
    printf("Enter the word to search for: ");
    fgets(word, sizeof(word), stdin);

    // Remove newline character if present
    if (word[strlen(word) - 1] == '\n')
        word[strlen(word) - 1] = '\0';

    // Check if the entered word is in the word list
    while (fgets(word, sizeof(word), file)) {
        // Remove newline character if present
        if (word[strlen(word) - 1] == '\n')
            word[strlen(word) - 1] = '\0';

        // Compare word with user input
        if (strcmp(word, user_input) == 0) {
            found = 1;
            break;
        }
    }

    // Close the file
    fclose(file);

    // Output whether the word is in the list or not
    if (found) {
        printf("The word '%s' is in the list.\n", user_input);
    } else {
        printf("The word '%s' is not in the list.\n", user_input);
    }

    return 0;
}

