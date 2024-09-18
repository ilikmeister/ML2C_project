//  CITS2002 Project 1 2024
//  Student1:   24038357   ILIYAS AKHMET
//  Student2:   24069389   DMITRY PRYTKOV
//  Platform:   Linux

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void translateComment (char *line, char *newLine, int len, int position) {
    // Shifting the characters 1 position to the right from '#' index
    for (int i = len; i > position; --i) {
        line[i] = line[i - 1];
    }

    // Replacing the first 2 characters from '#' index with '//'
    line[position] = '/';
    line[position + 1] = '/';

    // Adding a null byte
    line[len + 1] = '\0';

    // Copying the translated line to a new line
    strcpy(newLine, line);
}

void translateArrow (char *line, char *newLine, int len, int position) {
    // Shifting the characters 1 position to the left from '-' index
    for (int i = position; i < len; ++i) {
        line[i] = line[i + 1];
    }

    // Replacing the '<' with '='
    line[position - 1] = '=';

    len--;

    // Adding new line
    line[len] = '\n';

    // Adding a null byte
    line[len + 1] = '\0';

    // Copying the translated line to a new line
    strcpy(newLine, line);
}

void translateFunction (char *line, char *newLine, int len, int position) {
    char funcName[BUFSIZ];
    char parameters[BUFSIZ] = "";
    char param[BUFSIZ]; 
    // Skipping the keyword 'function'
    int ind = position + 9;
    // Extracting the function's name
    sscanf(line + ind, "%s", funcName);

    // Storing the first part of the function
    sprintf(newLine, "double %s (", funcName);

    ind += strlen(funcName) + 1; // Skipping function's name
    // Extracting parameters
    while (sscanf(line + ind, "%s", param) == 1) {
        // Adding comma separator if the parameter is not the first
        if (strlen(parameters) > 0) {
            strcat(parameters, ", ");
        }
        // Appending parameters to the list
        strcat(parameters, "double "); // Setting parameter type to double
        strcat(parameters, param); // Appending the parameter's name
        // Skipping the appended parameter
        ind += strlen(param) + 1;

    }
    // Completing the function's line
    strcat(newLine, parameters);
    strcat(newLine, ") {\n");

}

void processLine (char *line, char *newLine) {
    // Calculating the length of the line
    int len = strlen(line);
    // Indicator for processed line
    int flag = 0;
    // Looping through each character
    for (int i = 0; i < len; ++i) {
        // Translating a comment line
        if (line[i] == '#') {
            translateComment(line, newLine, len, i);
            flag = 1;
            break;
        }
        // Translating an assignment sign
        else if (line[i] == '<' && line[i + 1] == '-') {
            translateArrow(line, newLine, len, i + 1);
            flag = 1;
        }
        // Translating the function
        else if (strncmp(line + i, "function", 8) == 0) { // Comparing first 8 characters for 'function' keyword
            translateFunction(line, newLine, len, i);
            flag = 1;
        }
    }

    if (flag != 1) {
        strcpy(newLine, line);
    }

}

void readFile (char mlFileName[]) {
    // Opening the passed ML file
    FILE *mlfile = fopen(mlFileName, "r");
    if (mlfile == NULL) {
        fprintf(stderr, "Failed to open ML file");
        exit(EXIT_FAILURE);
    }

    // Generating the name for the new C file
    char cFileName[BUFSIZ];
    sprintf(cFileName, "ml-%d.c", getpid());

    // Creating the new C file
    FILE *cfile = fopen(cFileName, "w");
    if (cfile == NULL) {
        fprintf(stderr, "Failed to create C file");
        fclose(mlfile);
        exit(EXIT_FAILURE);
    }

    // Initializing buffers to store lines
    char line[BUFSIZ];
    char newLine[BUFSIZ];

    // Reading each line
    while (fgets(line, sizeof line, mlfile) != NULL) {
       processLine(line, newLine); // Processing the line
       fputs(newLine, cfile); // Writing the processed line to the new C file
    }

    // Closing both files
    fclose(mlfile);
    fclose(cfile);
}

int main (int argc, char *argv[]) {
    // Checking the number of arguments passed
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename.ml (arg1 arg2 ...)\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Checking the file's format
    else if (!strstr(argv[1], ".ml")) {
        fprintf(stderr, "The file should be in '.ml' format\n");
        exit(EXIT_FAILURE);
    }
    // Passing the file to read
    else {
        readFile(argv[1]);
        return EXIT_SUCCESS;
    }
    return 0;
}