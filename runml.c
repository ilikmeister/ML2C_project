//  CITS2002 Project 1 2024
//  Student1:   24038357   ILIYAS AKHMET
//  Student2:   24069389   DMITRY PRYTKOV
//  Platform:   Linux

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void compileCFile (char *cFileName, char *outputFileName) {
    char command[BUFSIZ];

    // Constructing the cc compile command
    sprintf(command, "cc -std=c11 -Wall -Werror -o %s %s", outputFileName, cFileName);

    // Executing the command
    int status = system(command);

    // Checking if compilation was successful
    if (status != 0) {
        fprintf(stderr, "Compilation failed\n");
        exit(EXIT_FAILURE);
    }
}

void runProgram (char *outputFileName, int argc, char *argv[]) {
    char command[BUFSIZ] = "";
    strcat(command, "./");
    strcat(command, outputFileName);

    // Runing the executable file
    int status = system(command);

    // Checking if the execution was successful
    if (status != 0) {
        fprintf(stderr, "Execution failed\n");
        exit(EXIT_FAILURE);
    }
}

void cleanUp(char *cFileName, char *outputFileName) {
    // Removing the generated C file
    remove(cFileName);

    // Removing the compiled executable
    remove(outputFileName);
}

void prependTab (char* newLine) {
    char tab[2] = "\t";
    char temp[BUFSIZ];

    // Adding the tab character at the beginning of newLine
    strncpy(temp, tab, sizeof(temp) - 1); // Copying the tab character into temp
    temp[sizeof(temp) - 1] = '\0'; // Adding the null byte
    strncat(temp, newLine, sizeof(temp) - strlen(temp) - 1); // Appending a newLine to temp
    strncpy(newLine, temp, BUFSIZ - 1); // Copying result back to newLine
    newLine[BUFSIZ - 1] = '\0'; // Adding the null byte
}

void prependDouble (char* newLine) {
    char doub[8] = "double ";
    char temp[BUFSIZ];

    // Adding the double at the beginning of newLine
    strncpy(temp, doub, sizeof(temp) - 1); // Copying the double into temp
    temp[sizeof(temp) - 1] = '\0'; // Adding the null byte
    strncat(temp, newLine, sizeof(temp) - strlen(temp) - 1); // Appending a newLine to temp
    strncpy(newLine, temp, BUFSIZ - 1); // Copying result back to newLine
    newLine[BUFSIZ - 1] = '\0'; // Adding the null byte
}

void prependMain (char*newLine) {
    char funcMain[36] = "int main(int argc, char *argv[]) {\n";
    char temp[BUFSIZ];

    // Adding the main function
    strncpy(temp, funcMain, sizeof(temp) - 1); // Copying the main into temp
    temp[sizeof(temp) - 1] = '\0'; // Adding the null byte
    strncat(temp, newLine, sizeof(temp) - strlen(temp) - 1); // Appending a newLine to temp
    strncpy(newLine, temp, BUFSIZ - 1); // Copying result back to newLine
    newLine[BUFSIZ - 1] = '\0'; // Adding the null byte
}

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
    strncpy(newLine, line, BUFSIZ - 1);
    newLine[BUFSIZ - 1] = '\0'; // Adding the null byte
}

void translateArrow (char *line, char *newLine, int len, int position) {
    // Indicator for the comment
    int comment = 0;
    
    // Shifting the characters 1 position to the left from '-' index
    for (int i = position; i < len; ++i) {
        line[i] = line[i + 1];
    }

    // Replacing the '<' with '='
    line[position - 1] = '=';

    // Adding a semicolon if comment was found
    for (int i = 0; i < len; ++i) {
        // Checking for comment
        if (line[i] == '#') {
            comment = 1;
            // Moving 1 space to the right
            for (int j = len; j > i; --j) {
                line[j] = line[j - 1];
            }
            // Semicolon before comment
            line[i] = ';';
            len++;
            break;
        }
    }

    // Adding a semicolon if comment was not found
    if (comment != 1) {
        line[len - 2] = ';';
    }

    // Adding new line
    line[len - 1] = '\n';

    // Adding a null byte
    line[len] = '\0';

    // Copying the translated line to a new line
    strncpy(newLine, line, BUFSIZ - 1);
    newLine[BUFSIZ - 1] = '\0'; // Adding the null byte

    // Prepending double
    prependDouble(newLine);

    // Handling indentation
    if (line[0] == '\t') {
        prependTab(newLine);
    }
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

void translatePrint (char *line, char *newLine, int len, int position) {
    char expression[BUFSIZ];
    char temp[BUFSIZ];

    // Skipping the keyword print
    int ind = position + 6;

    // Extracting the expression
    strncpy(temp, line + ind, len - ind);
    temp[len - ind] = '\0'; // Adding the null byte 

    // Removing the \n
    size_t tempLen = strlen(temp);
    if (temp[tempLen - 1] == '\n') {
        temp[tempLen - 1] = '\0';
    }

    // Copying the processed expression
    strncpy(expression, temp, BUFSIZ - 1);
    expression[BUFSIZ - 1] = '\0';  // Adding null byte

    // Formatting new line by checking for whole number results
    sprintf(newLine, 
            "double tempResult = %s;\n"
            "if ((int)tempResult == tempResult) printf(\"%%d\\n\", (int)tempResult); else printf(\"%%.6f\\n\", tempResult);\n", 
            expression);

    // Handling tab indentation
    if (line[0] == '\t') {
        prependTab(newLine);
    }
    // Starting the main function if no tab
    else {
        prependTab(newLine);
        prependMain(newLine);
    }
}

void translateReturn (char *line, char *newLine, int len, int position) {
    char expression[BUFSIZ];
    char temp[BUFSIZ];
    // Skipping the keyword return
    int ind = position + 7;

    // Extracting the expression
    strncpy(temp, line + ind, len - ind);
    temp[len - ind] = '\0';  // Adding the null byte

    // Removing the \n
    size_t tempLen = strlen(temp);
    if (temp[tempLen - 1] == '\n') {
        temp[tempLen - 1] = '\0';
    }

    // Copying the processed expression
    strncpy(expression, temp, BUFSIZ - 1);
    expression[BUFSIZ - 1] = '\0';  // Adding null byte

    // Formatting new line
    sprintf(newLine, "return %s;\n", expression);

    // Handling tab indentation
    if (line[0] == '\t') {
        prependTab(newLine);
    }
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
        // Translating the print statement
        else if (strncmp(line + i, "print ", 6) == 0) { // Comparing 6 characters for 'print ' keyword
            translatePrint(line, newLine, len, i);
            flag = 1;
        }
        else if (strncmp(line + i, "return", 6) == 0) { // Comparing 6 characters for 'return' keyword
            translateReturn(line, newLine, len, i);
            flag = 1;
        }
    }

    if (!flag) {
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

    // Including the libraries
    fprintf(cfile, "#include <stdlib.h>\n");
    fprintf(cfile, "#include <stdio.h>\n\n");

    // Flag for function
    int funcFlag = 0;

    // Initializing buffers to store lines
    char line[BUFSIZ];
    char newLine[BUFSIZ];

    // Reading each line
    while (fgets(line, sizeof line, mlfile) != NULL) {
        // Checking for function keyword
        if (strncmp(line, "function", 8) == 0) { // Comparing 8 characters for 'function' keyword
            // Checking whether already in a function
            if (funcFlag) { 
                fputs("}\n", cfile); // Closing the previous function
            }
            translateFunction(line, newLine, strlen(line), 0);
            fputs(newLine, cfile); // Entering the function header into file
            funcFlag = 1;
        }
        // Processing the body of function
        else if (funcFlag) {
            // Checking for tab
            if (line[0] == '\t') {
                processLine(line, newLine); // Processing the body line
                fputs(newLine, cfile); // Entering it to the file
            }
            else {
                funcFlag = 0; // Resetting the flag
                fputs("}\n", cfile); // Closing the previous function
                processLine(line, newLine); // Reprocessing the current line as non-function
                fputs(newLine, cfile); // Writing it to the file
            }
        }
        // Processing the non function lines
        else {
            processLine(line, newLine);
            fputs(newLine, cfile);
        }
    }

    // Finishing the main function
    fprintf(cfile, "    return 0;\n");
    fprintf(cfile, "}\n");

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
    // Passing the file to read, compiling, and executing
    else {
        // Reading and translating
        readFile(argv[1]);

        // Compiling
        char cFileName[BUFSIZ];
        sprintf(cFileName, "ml-%d.c", getpid()); // Assuming the same filename pattern
        char outputFileName[BUFSIZ];
        sprintf(outputFileName, "ml-%d", getpid()); // Outputing the program name
        compileCFile(cFileName, outputFileName);

        // Running the program
        runProgram(outputFileName, argc, argv);

        // Cleaning up
        cleanUp(cFileName, outputFileName);

        return EXIT_SUCCESS;
    }
    return 0;
}