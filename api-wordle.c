#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6
#define MAX_INPUT_LENGTH 50
#define NUM_GUESSES 14855
#define BUFFER_SIZE 256

#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define NORMAL "\033[0m"

void printColouredArray(char *arr, int length);

bool arrContainsWord(char arr[NUM_GUESSES][WORD_LENGTH + 1], char *word);

int numChars(char *word, char c, int length);

int getInputLength(char line[]);

int main(int argc, char **argv)
{
    FILE *guesses = fopen("valid-guesses.txt", "r");

    if(guesses != NULL)
    {
        char buffer[BUFFER_SIZE];
        char command[BUFFER_SIZE];
        char word[WORD_LENGTH + 1];
        FILE *file;

        // Get Date in YYYY-MM-DD format
        file = popen("date +'%Y-%m-%d'", "r");
        fgets(buffer, sizeof(buffer), file);
        buffer[10] = '\0'; // berid of newline character

        // Get todays wordle answer from an API
        snprintf(command, sizeof(command), "curl -s 'https://www.nytimes.com/svc/wordle/v2/%s.json' | jq '.solution'", buffer);
        file = popen(command, "r");
        
        fgetc(file); // Clear quotation
        fgets(word, sizeof(word), file); // Read in solution
        fgets(buffer, sizeof(buffer), file);  // Clear rest of junk

        pclose(file);

        char final_result[WORD_LENGTH * MAX_ATTEMPTS] = {0};
        char guess[MAX_INPUT_LENGTH] = {0};

        bool gameOn = true;
        int charsCorrect;
        int attempts = 0;

        // Add contents of guesses file to an array
        char guess_arr[NUM_GUESSES][WORD_LENGTH + 1] = {0};
        char temp[WORD_LENGTH + 1] = {0};

        for(int i = 0; i < NUM_GUESSES; i++)
        {
            fread(temp, sizeof(char), WORD_LENGTH + 1, guesses);
            temp[WORD_LENGTH] = '\0';
            strcpy(guess_arr[i], temp);
        }

        printf("Guess the word!\n");

        while(gameOn)
        {
            fgets(guess, MAX_INPUT_LENGTH, stdin); // Get input
            
            // Ensure user's guess has 5 characters
            if(getInputLength(guess) == WORD_LENGTH)
            {
                
                // Ensure the user's guess is valid
                if(arrContainsWord(guess_arr, guess))
                {
                    char colourArr[WORD_LENGTH] = {0};

                    // Initialise all entries to NORMAL
                    for(int i = 0; i < WORD_LENGTH; i++)
                    {
                        colourArr[i] = 'N';
                    }

                    charsCorrect = 0;

                    // Determine correctness of each character
                    for(int i = 0; i < WORD_LENGTH; i++)
                    {
                        int numCharAnswer = numChars(word, guess[i], WORD_LENGTH);
                        int currCharGuess = numChars(guess, guess[i], i + 1);
                        
                        if(guess[i] == word[i])
                        {
                            colourArr[i] = 'G';
                            charsCorrect++;

                            // If too many colour chars and have green
                            // then remove a yellow
                            if(currCharGuess > numCharAnswer)
                            {
                                bool removed_yellow = false;

                                for(int j = i - 1; j >= 0 && !removed_yellow; j--)
                                {
                                    if(guess[j] == guess[i] && colourArr[j] == 'Y')
                                    {
                                        colourArr[j] = 'N';
                                        removed_yellow = true;
                                    }
                                }
                            }
                        }
                        else if(numChars(word, guess[i], WORD_LENGTH))
                        {
                            // only colour yellow if we need more yellow.
                            if(currCharGuess <= numCharAnswer)
                            {
                                colourArr[i] = 'Y';
                            }
                        }
                    }
                    // Add attempt result to final result
                    for(int i = 0; i < WORD_LENGTH; i++)
                    {
                        final_result[attempts * WORD_LENGTH + i] = colourArr[i];
                    }

                    // Print Results of Colour Array and attempts thus far
                    printColouredArray(colourArr, WORD_LENGTH);
                    attempts++;
                    printf("\t%d/%d\n", attempts, MAX_ATTEMPTS);

                    // Check for win or fail
                    if(charsCorrect == WORD_LENGTH || attempts == MAX_ATTEMPTS)
                    {
                        gameOn = false;
                        int result_length = attempts * WORD_LENGTH;
                        final_result[result_length] = '\0';
                        
                        if(charsCorrect == WORD_LENGTH)
                        {
                            printf("YOU WON!\n");
                        }
                        else
                        {
                            printf("You Failed. The word was %s\n", word);
                        }

                        printf("Final Results:\n");
                        printColouredArray(final_result, result_length);
                        putchar('\n');
                    }

                }
                else
                {
                    printf("Guess is not in word list.\n");
                }
            }
            else
            {
                printf("Word entered must have %d characters\n", WORD_LENGTH);
            }
        }
        fclose(guesses);
    }
    else
    {
        printf("Failed to open one or more files.\n");
    }

    return EXIT_SUCCESS;
}

void printColouredArray(char *arr, int length)
{
    for(int i = 0; i < length; i++)
    {
        // Check if need to print new attempt
        if(i % WORD_LENGTH == 0 && i != 0)
        {
            printf("\n");
        }

        // Print coloured square for each attempt's characters
        if(arr[i] == 'N')
        {
            printf("■");
        }
        else if(arr[i] == 'Y')
        {
            printf("%s■%s", YELLOW, NORMAL);
        }
        else
        {
            printf("%s■%s", GREEN, NORMAL);
        }
    }
}

// Binary Search
bool arrContainsWord(char arr[NUM_GUESSES][WORD_LENGTH + 1], char *word)
{
    bool hasWord = false;
    int start = 0;
    int end = NUM_GUESSES;
    int middle;

    while(!hasWord && start <= end)
    {
        middle = (start + end) / 2;

        int compare = strcmp(word, arr[middle]);

        if(compare < 0)
        {
            end = middle - 1;
        }
        else if(compare > 0)
        {
            start = middle + 1;
        }
        else
        {
            hasWord = true;
        }
    }
    return hasWord;
}

int numChars(char *word, char c, int length)
{
    int numChars = 0;

    for(int i = 0; i < length; i++)
    {
        if(word[i] == c)
        {
            numChars++;
        }
    }

    return numChars;
}

int getInputLength(char line[])
{
    int length = 0;
    
    while(line[length] != '\n')
    {
        length++;
    }
    line[length] = '\0';

    return length;
}
