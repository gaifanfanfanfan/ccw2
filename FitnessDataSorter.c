#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the struct for the fitness data
typedef struct
{
    char date[11];
    char time[6];
    int steps;
} FitnessData;

// Function to tokenize a record
void tokeniseRecord(char *record, char delimiter, char *date, char *time, int *steps)
{
    char *ptr = strtok(record, &delimiter);
    if (ptr != NULL)
    {
        strcpy(date, ptr);
        ptr = strtok(NULL, &delimiter);
        if (ptr != NULL)
        {
            strcpy(time, ptr);
            ptr = strtok(NULL, &delimiter);
            if (ptr != NULL)
            {
                *steps = atoi(ptr);
            }
        }
    }
}
int loadDataFromFile(const char *filename, FitnessData **data)
{
    if (strlen(filename) == 0)
    {
        printf("Incorrectly named data file\n");
        return 1;
    }

    // Check if the filename matches the expected file name
    if (strcmp(filename, "FitnessData_2024.csv") != 0)
    {
        printf("Error: Could not find or open the file.\n");
        return 1;
    }
    if (strstr(filename, ".csv") == NULL)
    {
        printf("Error: The file must have a '.csv' extension.\n");
        return 1;
    }

    // Check if the file exists
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not find or open the file.\n");
        return 0;
    }

    int count = 0, capacity = 1;
    *data = malloc(capacity * sizeof(FitnessData)); // Allocate memory for records
    if (*data == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 0;
    }

    char line[60], date[11], time[6], stepsStr[10];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (count >= capacity)
        {
            capacity *= 2;
            *data = realloc(*data, capacity * sizeof(FitnessData)); // Reallocate memory
            if (*data == NULL)
            {
                printf("Memory reallocation failed.\n");
                fclose(file);
                return 0;
            }
        }

        tokeniseRecord(line, ",", date, time, stepsStr);
        (*data)[count].steps = atoi(stepsStr); // Convert steps to integer
        strcpy((*data)[count].date, date);
        strcpy((*data)[count].time, time);

        count++;
    }

    fclose(file);
    return count; // Return the number of records loaded
}
void sortDataBySteps(FitnessData *data, int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (data[i].steps < data[j].steps)
            {
                // Swap records if the first one has fewer steps
                FitnessData temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}
void saveDataToFile(const char *inputFilename, FitnessData *data, int count)
{

    char outputFilename[100];
    strcpy(outputFilename, inputFilename);
    strcat(outputFilename, ".tsv");

    FILE *file = fopen(outputFilename, "w");
    if (file == NULL)
    {
        printf("Error: Could not open the output file for writing.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s\t%s\t%d\n", data[i].date, data[i].time, data[i].steps);
    }

    fclose(file);
    printf("Data sorted and written to  '%s'.\n", outputFilename);
}

int main()
{
    char filename[100];
    FitnessData *data = NULL;

    // Get the filename from the user
    printf("Enter Filename: ");
    scanf("%s", filename);

    // Load the data from the file
    int recordCount = loadDataFromFile(filename, &data);
    if (recordCount == 0)
    {
        return 0; // Exit if there was an error loading the file
    }

    // Sort the data by steps in descending order
    sortDataBySteps(data, recordCount);

    // Save the sorted data to a .tsv file
    saveDataToFile(filename, data, recordCount);

    // Free allocated memory
    free(data);

    return 0;
}