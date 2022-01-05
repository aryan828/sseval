#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int LOCCTR = 0;

char *InputFile = "input.txt";
char *OpTabFile = "optab.txt";
char *OutputFile = "output.txt";
char *SymTabFile = "symtab.txt";

struct opline {
    int addr;
    char label[50];
    char opcode[50];
    char operand[50];
} lines[100];

struct symtab {
    char label[50];
    int address;
} symtab[100];

struct optab {
    char opcode[50];
    int code;
} optab[100];

int ind = 0;
int sind = 0;
int oind = 0;

int in_symtab(char *label) {
    for (int i = 0; i < sind; i++) {
        if (strcmp(label, symtab[i].label) == 0) {
            return 1;
        }
    }
    return 0;
}

int in_optab(char *opcode) {
    for (int i = 0; i < oind; i++) {
        if (strcmp(opcode, optab[i].opcode) == 0) {
            return 1;
        }
    }
    return 0;
}

void check_label(int i) {
    if (strlen(lines[i].label) > 0) {
        if (in_symtab(lines[i].label)) {
            printf("Error: Duplicate label %s\n", lines[i].label);
        } else {
            strcpy(symtab[sind].label, lines[i].label);
            symtab[sind].address = LOCCTR;
            sind++;
        }
    }
}

// Function to parse input records
void start_parsing() {
    for (int i = 0; i < ind; i++) {
        lines[i].addr = LOCCTR;
        if (strcmp(lines[i].opcode, "START") == 0) {
            LOCCTR = (int)strtol(lines[i].operand, NULL, 16);
            lines[i].addr = LOCCTR;
        } else if (in_optab(lines[i].opcode)) {
            check_label(i);
            LOCCTR += 3;
        } else if (strcmp(lines[i].opcode, "WORD") == 0) {
            check_label(i);
            LOCCTR += 3;
        } else if (strcmp(lines[i].opcode, "RESW") == 0) {
            check_label(i);
            LOCCTR += 3 * (int)strtol(lines[i].operand, NULL, 10);
        } else if (strcmp(lines[i].opcode, "RESB") == 0) {
            check_label(i);
            LOCCTR += (int)strtol(lines[i].operand, NULL, 10);
        } else if (strcmp(lines[i].opcode, "BYTE") == 0) {
            check_label(i);
            if (lines[i].operand[0] == 'X') {
                LOCCTR += (int)(strlen(lines[i].operand) - 3) / 2;
            } else if (lines[i].operand[0] == 'C') {
                LOCCTR += strlen(lines[i].operand) - 3;
            }
        } else if (strcmp(lines[i].opcode, "END") == 0) {
            break;
        } else {
            printf("Error: Invalid opcode %s\n", lines[i].opcode);
        }
    }
}

// Function to get the number of words in a line
int get_length(char *line) {
    int count = 1;
    for(int i = 0; i < strlen(line) - 1; i++) {
        if((line[i] == ' ' || line[i] == '\t') && line[i+1] > 32) {
            count++;
        }
    }
    return count;
}

// Function to read the file and store the lines in the array
void read_file() {
    FILE *fp = fopen(InputFile, "r");
    char line[100];
    int i = 0;
    while(fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '.') {
            continue;
        }
        lines[i].label[0] = '\0';
        lines[i].opcode[0] = '\0';
        lines[i].operand[0] = '\0';
        int length = get_length(line);
        if (length == 1) {
            strcpy(lines[i].opcode, strtok(line, " \t\n"));
        } else if (length == 2) {
            strcpy(lines[i].opcode, strtok(line, " \t\n"));
            strcpy(lines[i].operand, strtok(NULL, " \t\n"));
        } else if (length == 3) {
            strcpy(lines[i].label, strtok(line, " \t\n"));
            strcpy(lines[i].opcode, strtok(NULL, " \t\n"));
            strcpy(lines[i].operand, strtok(NULL, " \t\n"));
        }
        i++; ind++;
    }
    fclose(fp);
}

// Function to print lines
void print_lines() {
    FILE *fp = fopen(OutputFile, "w");
    for(int i = 0; i < ind; i++) {
        printf("%.4X\t%s\t%s\t%s\n", lines[i].addr, lines[i].label, lines[i].opcode, lines[i].operand);
        fprintf(fp, "%.4X\t%s\t%s\t%s\n", lines[i].addr, lines[i].label, lines[i].opcode, lines[i].operand);
    }
    fclose(fp);
}

void load_optab() {
    FILE *fp = fopen(OpTabFile, "r");
    char line[100];
    int i = 0;
    while(fgets(line, sizeof(line), fp) != NULL) {
        strcpy(optab[i].opcode, strtok(line, " \t\n"));
        optab[i].code = (int)strtol(strtok(NULL, " \t\n"), NULL, 16);
        i++; oind++;
    }
    fclose(fp);
}

void write_symtab() {
    FILE *fp = fopen(SymTabFile, "w");
    for(int i = 0; i < sind; i++) {
        fprintf(fp, "%s\t%04X\n", symtab[i].label, symtab[i].address);
    }
    fclose(fp);
}

int main() {
    read_file();
    load_optab();
    start_parsing();
    print_lines();
    write_symtab();
    return 0;
}