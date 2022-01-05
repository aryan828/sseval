#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *InputFile = "input.txt";
char *OutputFile = "output.txt";
char *SymTab = "symtab.txt";
char *OpTab = "optab.txt";

int oind = 0;
int sind = 0;
int ind = 0;

struct optab {
    char opcode[10];
    int mnemonic;
} optab[100];

struct symtab {
    char label[10];
    int address;
} symtab[100];

struct opline {
    int address;
    char label[10];
    char opcode[10];
    char operand[10];
} lines[100];

void load_optab() {
    FILE *fp = fopen(OpTab, "r");
    char line[100];
    while(fgets(line, sizeof(line), fp) != NULL) {
        strcpy(optab[oind].opcode, strtok(line, " \n\t"));
        optab[oind].mnemonic = strtol(strtok(NULL, " \n\t"), NULL, 16);
        oind++;
    }
    fclose(fp);
}

void load_symtab() {
    FILE *fp = fopen(SymTab, "r");
    char line[100];
    while(fgets(line, sizeof(line), fp) != NULL) {
        strcpy(symtab[sind].label, strtok(line, " \n\t"));
        symtab[sind].address = strtol(strtok(NULL, " \n\t"), NULL, 16);
        sind++;
    }
    fclose(fp);
}

int get_len(char *line) {
    int count = 0;
    for(int i = 1; i < strlen(line) - 1; i++) {
        if((line[i] == 32 || line[i] == 9) && line[i+1] > 32) {
            count++;
        }
    }
    return count;
}

void load_file() {
    FILE *fp = fopen(InputFile, "r");
    char line[100];
    while(fgets(line, sizeof(line), fp) != NULL) {
        int len = get_len(line);
        if(len == 1) {
            lines[ind].address = strtol(strtok(line, " \t\n"), NULL, 16);
            strcpy(lines[ind].opcode, strtok(NULL, " \t\n"));
        } else if(len == 2) {
            lines[ind].address = strtol(strtok(line, " \t\n"), NULL, 16);
            strcpy(lines[ind].opcode, strtok(NULL, " \t\n"));
            strcpy(lines[ind].operand, strtok(NULL, " \t\n"));
        } else if(len == 3) {
            lines[ind].address = strtol(strtok(line, " \t\n"), NULL, 16);
            strcpy(lines[ind].label, strtok(NULL, " \t\n"));
            strcpy(lines[ind].opcode, strtok(NULL, " \t\n"));
            strcpy(lines[ind].operand, strtok(NULL, " \t\n"));
        }
        ind++;
    }
    fclose(fp);
}

void write_header(int i) {
    FILE *fp = fopen(OutputFile, "w");
    fprintf(fp, "H %-6s %06X %06X\n", lines[i].label, lines[i].address, lines[ind-1].address-lines[i].address);
    fclose(fp);
}

char text_buffer[61];

int in_optab(char *opcode) {
    for(int i = 0; i < oind; i++) {
        if(strcmp(opcode, optab[i].opcode) == 0) {
            return optab[i].mnemonic;
        }
    }
    return -1;
}

int in_symtab(char *label) {
    for(int i = 0; i < sind; i++) {
        if(strcmp(label, symtab[i].label) == 0) {
            return symtab[i].address;
        }
    }
    return -1;
}

void write_text(int i) {
    int opcode = in_optab(lines[i].opcode);
    opcode = opcode == -1 ? 0 : opcode;
    int operand = in_symtab(lines[i].operand);
    operand = operand == -1 ? 0 : operand;
    FILE *fp = fopen(OutputFile, "a");
    fprintf(fp, "T %06X %02X%04X\n", lines[i].address, opcode, operand);
    fclose(fp);
}

void write_end() {
    FILE *fp = fopen(OutputFile, "a");
    fprintf(fp, "E 000000\n");
    fclose(fp);
}

void write_record() {
    for(int i = 0; i < ind; i++) {
        if(strcmp(lines[i].opcode, "START") == 0) {
            write_header(i);
        } else if(strcmp(lines[i].opcode, "END") == 0) {
            write_end();
        } else {
            write_text(i);
        }
    }
}

void show_code() {
    for(int i = 0; i < ind; i++) {
        printf("%.4X\t%s\t%s\t%s\n", lines[i].address, lines[i].label, lines[i].opcode, lines[i].operand);
    }
}

int main() {
    load_optab();
    load_symtab();
    load_file();
    show_code();
    write_record();
    return 0;
}