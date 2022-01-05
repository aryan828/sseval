#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *inputFile = "input.txt";
const char *intermediateFile = "estab.txt";
const char *outputFile = "output.txt";

int PROGADDR = 0x5000;
int CSADDR = 0x5000;
int CSLTH;
int EXECADDR = 0x5000;
int INDEX = 0;

// Struct to store ESTAB
struct ESLINE {
    char CSNAME[50];
    char SYMBOL[50];
    int SADDR;
    int CSLEN;
} ESTAB[100];

struct TEXTREC {
    char SYMBOL[50];
    int SADDR;
    int LEN;
};

// Function to check duplicates control section name
int checkExistProg(char *key) {
    for (int i = 0; i < INDEX; i++) {
        if(strcmp(ESTAB[i].CSNAME, key) == 0)
            return 1;
    }
    return 0;
}

// Function to check duplicates symbol name
int checkExistSym(char *key) {
    for (int i = 0; i < INDEX; i++) {
        if(strcmp(ESTAB[i].SYMBOL, key) == 0)
            return 1;
    }
    return 0;
}

// Function to write Program Name to ESTAB
void writeProgram(char *progName) {
    strcpy(ESTAB[INDEX].CSNAME, progName);
    ESTAB[INDEX].SADDR = CSADDR;
    ESTAB[INDEX].CSLEN = CSLTH;
    INDEX++;
}

// Function to write Definition to ESTAB
void writeDefine(char *symbol, int addr) {
    strcpy(ESTAB[INDEX].SYMBOL, symbol);
    ESTAB[INDEX].SADDR = CSADDR + addr;
    INDEX++;
}

// Function to parse Header Records
void parseHeader(char *line) {
    line[strlen(line)-1] = '\0';
    // printf("Header Record: %s\n", line);
    char *delim = " \t\n";
    char *token = strtok(line, delim);
    char *progName = strtok(NULL, delim);
    token = strtok(NULL, delim);
    char *progLength = strtok(NULL, delim);
    CSLTH = strtol(progLength, NULL, 16);
    // printf("Program Length: %X\t%X\n", CSLTH, CSADDR);
    if (checkExistProg(progName)) {
        printf("Error: Program %s already exists\n", progName);
    } else {
        writeProgram(progName);
    }
}

// Function to parse Define Records
void parseDefine(char *line) {
    char *symbol = NULL;
    int addr = 0;
    line[strlen(line)-1] = '\0';
    // printf("Define Record: %s\n", line);
    char *delim = " \t\n";
    char *token = strtok(line, delim);
    while(token != NULL) {
        symbol = strtok(NULL, delim);
        token = strtok(NULL, delim);
        if (token != NULL) {
            addr = strtol(token, NULL, 16);
            if (checkExistSym(symbol)) {
                printf("Error: Symbol %s already exists\n", symbol);
            } else {
                writeDefine(symbol, addr);
            }
            // writeDefine(symbol, addr);
            // printf("%s\t%X\n", symbol, addr);
        }
    }
}

// Input file parser for linking loader pass 1
void parseInput() {
    FILE *file = fopen(inputFile, "r");
    char *line = NULL;
    int first = 1;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    char *delim = " \t\n";
    while((read = getline(&line, &len, file)) != -1) {
        if (line[0] == 'H') {
            parseHeader(line);
        } else if (line[0] == 'D') {
            parseDefine(line);
        } else if (line[0] == 'E') {
            CSADDR += CSLTH;
        }
    }
    fclose(file);
}

// Function to clear intermediate file
void clearIntermediate() {
    FILE *fp = fopen(intermediateFile, "w");
    fclose(fp);
}

// Function to print ESTAB to output file
void printESTAB() {
    FILE *fp = fopen(intermediateFile, "w");
    for(int i = 0; i < INDEX; i++) {
        if (strlen(ESTAB[i].CSNAME) != 0) {
            fprintf(fp, "%s\t%s\t%X\t%X\n", ESTAB[i].CSNAME, ESTAB[i].SYMBOL, ESTAB[i].SADDR, ESTAB[i].CSLEN);
        } else {
            fprintf(fp, "%s\t%s\t%X\n", ESTAB[i].CSNAME, ESTAB[i].SYMBOL, ESTAB[i].SADDR);
        }
    }
    fclose(fp);
}

// Function to implement pass 2 of linking loader
// void pass2() {
//     FILE *file = fopen(inputFile, "r");
//     FILE *fp = fopen(outputFile, "w");
//     char *line = NULL;
//     int first = 1;
//     size_t len = 0;
//     ssize_t read;
//     int i = 0;
//     char *delim = " \t\n";
//     struct TEXTREC textRec[100];
//     while((read = getline(&line, &len, file)) != -1) {
//         if (line[0] == 'H') {
//             line[strlen(line)-1] = '\0';
//             char *delim = " \t\n";
//             char *token = strtok(line, delim);
//             char *progName = strtok(NULL, delim);
//             token = strtok(NULL, delim);
//             char *progLength = strtok(NULL, delim);
//             for (int i = 0; i < INDEX; i++) {
//                 if(strcmp(ESTAB[i].CSNAME, progName) == 0) {
//                     CSADDR = ESTAB[i].SADDR;
//                     fprintf(fp, "H %s\t%X %s\n", progName, ESTAB[i].SADDR, progLength);
//                     break;
//                 }
//             }
//         } else if (line[0] == 'T') {
//             char *addrress = NULL;
//             int addr = 0;
//             int counter = 5;
//             line[strlen(line)-1] = '\0';
//             char *delim = " \t\n";
//             char *token = strtok(line, delim);
//             addrress = strtok(NULL, delim);
//             addr = strtol(addrress, NULL, 16);
//             addr += CSADDR;
//             fprintf(fp, "T %.6X ", addr);
//             // printf("T %.6X ", addr);
//             while(counter--) {
//                 addrress = strtok(NULL, delim);
//                 strcpy(textRec[i].SYMBOL, addrress);
//                 textRec[i].SADDR = addr+i*3;
//                 // printf("%s\t%X\n", textRec[i].SYMBOL, textRec[i].SADDR);
//                 i++;
//             }
//             while((read = getline(&line, &len, file)) != -1) {
//                 if (line[0] == 'E')
//                     break;
//                 if (line[0] == 'M') {
//                     line[strlen(line)-1] = '\0';
//                     char *delim = " \t\n";
//                     char *token = strtok(line, delim);
//                     char *modaddr = strtok(NULL, delim);
//                     char *modlen = strtok(NULL, delim);
//                     char *spec = strtok(NULL, delim);
//                     int addr = strtol(modaddr, NULL, 16);
//                     int newaddr = 0;
//                     char symbolname[10];
//                     for (int j = 1; j < strlen(spec); j++) {
//                         symbolname[j-1] = spec[j];
//                     }
//                     for (int j = 0; j < INDEX; j++) {
//                         if(strcmp(ESTAB[j].SYMBOL, symbolname) == 0) {
//                             newaddr = ESTAB[j].SADDR;
//                             break;
//                         }
//                     }
//                     for (int j = 0; j < 100; j++) {
//                         if(textRec[j].SADDR+CSADDR == addr) {
//                             if (spec[0] == '+')
//                                 textRec[j].SADDR += newaddr;
//                             else
//                                 textRec[j].SADDR -= newaddr;
//                         }
//                     }
//                     for (int j=0; j<5; j++) {
//                         printf("%s\t%X\n", textRec[j].SYMBOL, textRec[j].SADDR);
//                     }
//                 }
//             }
//             printf("\n");
//             fprintf(fp, "\n");
//         } else if (line[0] == 'E') {
//             fprintf(fp, line);
//         }
//     }
//     printf("Execution starts at %X\n", EXECADDR);
//     fclose(file);
//     fclose(fp);
// }

// Main Function
int main(int argc, char *argv[])
{
    clearIntermediate();
    parseInput();
    printESTAB();
    // pass2();
    return 0;
}