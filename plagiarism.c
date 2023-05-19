#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//DEFINE CONSTANTS
#define MAX_FILE_LENGTH 10000
#define MAX_WORD_LENGTH 50
#define MAX_NUMBER_OF_FILES 5

//Structure for word frequency
typedef struct wordFreq {
    char word[MAX_WORD_LENGTH];
    int freq[MAX_NUMBER_OF_FILES];
}wordFreq;

//Function for tokenizing string into words and update word frequency
void tokenizeFile(char *str, wordFreq *wordFrequency, int *wordCount, int fileIndex) {
    char *token;
    char delims[] = " \n\t\",.;:-?!()[]{}";
    int i;

    token = strtok(str, delims);
    while (token != NULL) {
        //Remove any punctuation marks
        while (ispunct(*token)) {
            token++;
        }
        i = strlen(token) - 1;
        while (ispunct(token[i])) {
            token[i--] = '\0';
        }

        //Convert any letters to lowercase 
        for (i = 0; token[i]; i++) {
            token[i] = tolower(token[i]);
        }

        //Skip any empty strings
        if (strlen(token) == 0) {
            token = strtok(NULL, delims);
            continue;
        }

        //Renew word frequency and store in frequency array 
        int found = 0;
        for (i = 0; i < *wordCount; i++) {
            if (strcmp(wordFrequency[i].word, token) == 0) {
                wordFrequency[i].freq[fileIndex]++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(wordFrequency[*wordCount].word, token);
            for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
                if (i == fileIndex) {
                    wordFrequency[*wordCount].freq[i] = 1;
                } else {
                    wordFrequency[*wordCount].freq[i] = 0;
                }
            }
            (*wordCount)++;
        }

        token = strtok(NULL, delims);
    }
}

//Function to compare and sort word frequency in descending order
int sortWords(const void *a, const void *b) {
    const wordFreq *wordFreqA = (const wordFreq *)a;
    const wordFreq *wordFreqB = (const wordFreq *)b;

    //Compare the word frequency of words in the two different files
    //If same total frequency sort alphabetically
    int totalFreqA = 0, totalFreqB = 0;
    for (int i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        totalFreqA += wordFreqA->freq[i];
        totalFreqB += wordFreqB->freq[i];
    }

    if (totalFreqA < totalFreqB) {
        return 1;
    } else if (totalFreqA > totalFreqB) {
        return -1;
    } else {
        return strcmp(wordFreqA->word, wordFreqB->word);
    }
}

//Function to create a HTML file to display the results table
void createWordFrequencyHTML(wordFreq *wordFrequency, int wordCount, char *testFile1, char *testFile2) {
    FILE *fp = fopen("wordFrequency.html", "w");
    if (fp == NULL) {
        printf("Error opening wordFrequency.html for writing.\n");
        exit(1);
    }

    // Write HTML content to the file
    fprintf(fp, "<html>\n");
    fprintf(fp, "<head><title>Word Frequency</title></head>\n");
    fprintf(fp, "<body>\n");
    fprintf(fp, "<h1>Word Frequency for Files: %s, %s</h1>\n", testFile1, testFile2);
    fprintf(fp, "<table>\n");
    fprintf(fp, "<tr><th>Word</th><th>%s</th><th>%s</th></tr>\n", testFile1, testFile2);

    // Print results table
    for (int i = 0; i < wordCount; i++) {
        fprintf(fp, "<tr><td>%s</td>", wordFrequency[i].word);
        for (int j = 0; j < MAX_NUMBER_OF_FILES; j++) {
            fprintf(fp, "<td>%d</td>", wordFrequency[i].freq[j]);
        }
        fprintf(fp, "</tr>\n");
    }

    fprintf(fp, "</table>\n");
    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");

    fclose(fp);
}

int main() {
    //Initialise any variables
    char testFile[MAX_NUMBER_OF_FILES][MAX_WORD_LENGTH] = {"test1.txt", "test2.txt", "test3.txt", "test4.txt", "test5.txt"};
    FILE *fp[MAX_NUMBER_OF_FILES];
    char file_contents[MAX_NUMBER_OF_FILES][MAX_FILE_LENGTH];
    wordFreq wordFrequency[MAX_FILE_LENGTH];
    int wordCount = 0;
    int i, j, c;
    int file1, file2;

    //Allow user to input 2 files and scan their response
    printf("Enter the numbers corresponding to the two files you want to compare word frequency for:\n");
    for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        printf("%d. %s\n", i + 1, testFile[i]);
    }
    printf("Enter the first file number: ");
    scanf("%d", &file1);
    printf("Enter the second file number: ");
    scanf("%d", &file2);

    //Open the required files 
    fp[file1 - 1] = fopen(testFile[file1 - 1], "r");
    fp[file2 - 1] = fopen(testFile[file2 - 1], "r");

    //Ensure files are opened properly 
    for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        if (i != file1 - 1 && i != file2 - 1) {
            //Set the file pointers of any unchosen files to NULL so their word frequencies are not calculated
            fp[i] = NULL; 
        } else if (fp[i] == NULL) {
            fprintf(stderr, "Error opening file %s\n", testFile[i]);
            exit(1);
        }
    }

    //Read content of the selected files
    for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        if (fp[i] != NULL) {
            int pos = 0;
            while ((c = fgetc(fp[i])) != EOF && pos < MAX_FILE_LENGTH - 1) {
                file_contents[i][pos++] = c;
            }
            file_contents[i][pos] = '\0';
        }
    }

    // Tokenize the content and count the word frequency
    for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        if (fp[i] != NULL) {
            tokenizeFile(file_contents[i], wordFrequency, &wordCount, i);
        }
    }

    //Sort the word frequency array based on the total frequency of the word between the two files and word order 
    qsort(wordFrequency, wordCount, sizeof(wordFreq), sortWords);

    //Create the HTML file
    createWordFrequencyHTML(wordFrequency, wordCount, testFile[file1 - 1], testFile[file2 - 1]);

    //Display successful message in terminal
    printf("Results file 'wordFrequency.html' generated successfully.\n");

    //Close the selected files 
    for (i = 0; i < MAX_NUMBER_OF_FILES; i++) {
        if (fp[i] != NULL) {
            fclose(fp[i]);
        }
    }

    return 0;
}


