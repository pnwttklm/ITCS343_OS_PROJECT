#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100 // Maximum length of a word

int main() {
    FILE *file;
    char *words[1000]; // Array to store words, assuming there are 1000 words
    char word[MAX_WORD_LENGTH];
    int word_count = 0;

    // Open the file
    file = fopen("list.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // Read each word from the file and store it in the array
    while (fgets(word, MAX_WORD_LENGTH, file)) {
        // Remove the newline character if present
        word[strcspn(word, "\n")] = 0;

        // Allocate memory for the word and copy it into the array
        words[word_count] = malloc(strlen(word) + 1); // +1 for null terminator
        if (words[word_count] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        strcpy(words[word_count], word);
        word_count++;

        // Break if reached the limit of words
        if (word_count >= 1000) {
            break;
        }
    }

    // Close the file
    fclose(file);

    // Now you have all words in the 'words' array
    // Do whatever you need to do with the array here
    // char input[100];
    // char input = "cat";
    for(int i = 0; i < word_count; i++) {
        if(strcmp(words[i], "a") == 0) {
            printf("Found the word cat at index %d\n", i);
            break;
        }
        if(i == word_count - 1) {
            printf("Word not found\n");
        }
    }

    // Free memory for each word
    for (int i = 0; i < word_count; i++) {
        free(words[i]);
    }

    return 0;
}
