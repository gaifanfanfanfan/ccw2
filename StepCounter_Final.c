#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FitnessDataStruct.h"

// Struct moved to header file

// Define any additional variables here
// Global variables for filename and FITNESS_DATA array
int recordCount = 0;
FITNESS_DATA *data = NULL;

// This is your helper function. Do not change it in any way.
// Inputs: character array representing a row; the delimiter character
// Outputs: date character array; time character array; steps character array
void tokeniseRecord(const char *input, const char *delimiter,
                    char *date, char *time, char *steps)
{
    // Create a copy of the input string as strtok modifies the string
    char *inputCopy = strdup(input);

    // Tokenize the copied string
    char *token = strtok(inputCopy, delimiter);
    if (token != NULL)
    {
        strcpy(date, token);
    }

    token = strtok(NULL, delimiter);
    if (token != NULL)
    {
        strcpy(time, token);
    }

    token = strtok(NULL, delimiter);
    if (token != NULL)
    {
        strcpy(steps, token);
    }

    // Free the duplicated string
    free(inputCopy);
}
#include <stdbool.h>

bool loadDataFromFile(const char *filename)
{
    // Check if the filename is empty
    if (strlen(filename) == 0)
    {
        printf("Incorrectly named data file\n");
        return false;
    }

    // Check if the filename matches the expected file name
    if (strcmp(filename, "FitnessData_2024.csv") != 0)
    {
        printf("Error: Could not find or open the file.\n");
        return false;
    }

    // Check if the file exists
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not find or open the file.\n");
        return false;
    }

    // Proceed with loading the data if file is found
    printf("File '%s' successfully loaded.\n", filename);

    int count = 0, capacity = 1;
    recordCount = 0;
    data = malloc(capacity * sizeof(FITNESS_DATA)); // Allocate memory
    if (data == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return false;
    }

    char line[60];
    char date[11], time[6], stepsStr[10];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (count >= capacity)
        {
            capacity *= 2;
            data = realloc(data, capacity * sizeof(FITNESS_DATA)); // Reallocate memory
            if (data == NULL)
            {
                printf("Memory reallocation failed.\n");
                fclose(file);
                return false;
            }
        }

        tokeniseRecord(line, ",", date, time, stepsStr);
        data[count].steps = atoi(stepsStr); // Convert steps to integer
        strcpy(data[count].date, date);
        strcpy(data[count].time, time);

        count++;
    }

    recordCount = count; // Update record count
    fclose(file);

    return true; // Return success status
}
void FewestSteps()
{
    if (recordCount == 0)
    {
        printf("No data available.\n");
        return;
    }

    int minSteps = data[0].steps;
    int minIndex = 0;

    for (int i = 1; i < recordCount; i++)
    {
        if (data[i].steps < minSteps)
        {
            minSteps = data[i].steps;
            minIndex = i;
        }
    }

    printf("Fewest steps: %s %s\n", data[minIndex].date, data[minIndex].time);
}

void LargestSteps()
{
    if (recordCount == 0)
    {
        printf("No data available.\n");
        return;
    }

    int maxSteps = data[0].steps;
    int maxIndex = 0;

    for (int i = 1; i < recordCount; i++)
    {
        if (data[i].steps > maxSteps)
        {
            maxSteps = data[i].steps;
            maxIndex = i;
        }
    }

    printf("Largest steps: %s %s\n", data[maxIndex].date, data[maxIndex].time);
}

void calculateMeanSteps()
{
    if (recordCount == 0)
    {
        printf("No data available.\n");
        return;
    }

    long totalSteps = 0;
    for (int i = 0; i < recordCount; i++)
    {
        totalSteps += data[i].steps;
    }

    printf("Mean step count: %ld\n", totalSteps / recordCount);
}

void findLongestPeriodAbove500()
{
    if (recordCount == 0)
    {
        printf("No data available.\n");
        return;
    }

    int maxStart = 0, maxEnd = 0;
    int currentStart = -1;

    for (int i = 0; i < recordCount; i++)
    {
        if (data[i].steps > 500)
        {
            if (currentStart == -1)
            {
                currentStart = i;
            }
        }
        else
        {
            if (currentStart != -1)
            {
                if ((i - 1 - currentStart) > (maxEnd - maxStart))
                {
                    maxStart = currentStart;
                    maxEnd = i - 1;
                }
                currentStart = -1;
            }
        }
    }

    if (currentStart != -1 && (recordCount - 1 - currentStart) > (maxEnd - maxStart))
    {
        maxStart = currentStart;
        maxEnd = recordCount - 1;
    }

    printf("Longest period start: %s %s\n", data[maxStart].date, data[maxStart].time);
    printf("Longest period end: %s %s\n", data[maxEnd].date, data[maxEnd].time);
}

int main()
{
    char filename[100];
    char select;

    while (1)
    {
        printf("\nMenu:\n");
        printf("Please select from the following options\n");
        printf("A: Specify the filename to be imported\n");
        printf("B: Display the total number of records in the file\n");
        printf("C: Find the date and time of the timeslot with the fewest steps\n");
        printf("D: Find the date and time of the timeslot with the largest number of steps\n");
        printf("E: Find the mean step count of all the records in the file\n");
        printf("F: Find the longest continuous period where the step count is above 500 steps\n");
        printf("Q: Quit\n");

        // Get user choice
        scanf(" %c", &select);

        switch (select)
        {
        case 'A':
        case 'a':
            printf("Input filename: ");
            scanf("%s", filename);
            loadDataFromFile(filename); // Load the data
            break;
        case 'B':
        case 'b':
            printf("Total records: %d\n", recordCount);
            break;
        case 'C':
        case 'c':
            FewestSteps();
            break;
        case 'D':
        case 'd':
            LargestSteps();
            break;
        case 'E':
        case 'e':
            calculateMeanSteps();
            break;
        case 'F':
        case 'f':
            findLongestPeriodAbove500();
            break;
        case 'Q':
        case 'q':
            printf("Exiting program.\n");
            free(data); // Free memory
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
}