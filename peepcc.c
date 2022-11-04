#include <stdlib.h>
#include <stdio.h>
#include "infToPstf.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "peepcc.h"


#define ADD     0x10    // Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define SUB     0x11    // Subtract a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MUL     0x12    // Multiple a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define DIV     0x13    // Divide a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MOD     0x14    // Compute the integer remainder when dividing a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)

// ACC-MEM Logical Instructions
#define AND     0x20    // Compute the binary AND of a specific memory location and the accumulator (leave the result in the accumulator)
#define ORR     0x21    // Compute the binary OR of a specific memory location and the accumulator (leave the result in the accumulator)
#define NOT     0x22    // Compute the logical NOT of the accumulator (leave the result in the accumulator)
#define XOR     0x23    // Compute the binary XOR of a specific memory location and the accumulator (leave the result in the accumulator)
#define LSR     0x24    // Compute the binary logical shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define ASR     0x25    // Compute the binary arithmetic shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define LSL     0x26    // Compute the binary logical shift left of the accumulator (leave the result in the accumulator)

// Branch Instructions
#define B       0X30    // Branch to a specific location in memory
#define BNEG    0X31    // Branch to a specific location in memory if the accumulator is negative
#define BPOS    0X32    // Branch to a specific location in memory if the accumulator is positive
#define BZRO    0X33    // Branch to a specific location in memory if the accumulator is zero

// ACC-MEM Load/Store Instructions
#define LOAD    0X40    // Load a word from a specific location in memory into the accumulator
#define STOR    0X41    // Store a word from the accumulator into a specific memory location

// I/O (Standard) Instructions
#define READ    0X50    // Read a word from the terminal into a specific location in memory
#define WRTE    0X51    // Write a word from a specific location in memory to the terminal

// HALT Instruction
#define HALT    0XFF    // Halt, i.e. the program has completed its task


/*
*Method declarations
*/
void initCompiler(PeepCompiler *compiler);
void firstPass(PeepCompiler *compiler, char file[]);
void printSymTab(TableEntry* tab, int size);
int* ifConverter(char *inst);
int* letConverter(char *inst);
int printConverter(PeepCompiler *comp, int var);
int gotoConverter(PeepCompiler *comp, int lineNumber);
int endConverter(PeepCompiler *compiler);
int inputConverter(PeepCompiler *comp, int var);

/*
*Method Implementations
*/

void initCompiler(PeepCompiler *compiler)
{
    TableEntry symbols[256];
    int flags[256] = {-1};
    unsigned short int hml[256] = {0};
    compiler->datacount = 255;
    compiler->hml = hml;
    compiler->inscount = 0;
    compiler->ln = 0;
    compiler->symSize = 0;
    compiler->symTab = symbols;
    compiler->flag = flags;
    compiler->memsize = 0;
}


bool lookUpSymbol(int symbol, TableEntry *table)
{
    for(int i = 0; i<MEMSIZE; ++i)
    {
        if(table[i].symbol == symbol)
        {
            return true;
        }
    }
    return false;
}

TableEntry getSymbol(int symbol, TableEntry *table)
{
    for(int i = 0; i<MEMSIZE; ++i)
    {
        if(table[i].symbol == symbol)
        {
            return table[i];
        }
    }
}


void printSymTab(TableEntry* tab, int size)
{
    printf("Symbol  Table  Location \n");
    for (int i = 0; i < size; ++i)
    {
        if(tab[i].type == 'V')
        {
            printf("  %c        %c      0x%02hX \n", tab[i].symbol, tab[i].type, tab[i].location);
        }
        else
        {
            printf("  %d       %c      0x%02hX \n", tab[i].symbol, tab[i].type, tab[i].location);
        }
        
    }
}

int endConverter(PeepCompiler *compiler)
{
    compiler->hml[compiler->inscount++] = HALT * MEMSIZE + 0;
    return HALT * MEMSIZE + 0;
}

int gotoConverter(PeepCompiler *compiler, int lineNumber)
{
    int result[1];
    if(lookUpSymbol(lineNumber, compiler->symTab))
    {
        printf("%d \n", lineNumber);
        result[0] = B * MEMSIZE + getSymbol(lineNumber, compiler->symTab).location;
    }
    else
    {
        compiler->flag[compiler->inscount] = lineNumber;
        result[0] = B * MEMSIZE + 0;
    }
    compiler->hml[compiler->inscount++] = result[0]; //add hml instr to the hml memory
    return result[0];
}

int printConverter(PeepCompiler *compiler, int var)
{
    TableEntry symbol;
    
    if(lookUpSymbol(var, compiler->symTab))
    {
        symbol = getSymbol(var, compiler->symTab);
    }
    
    compiler->hml[compiler->inscount++] = WRTE * MEMSIZE + symbol.location; //add hml instr to the hml memory
    
    return WRTE * MEMSIZE + symbol.location;
}


int inputConverter(PeepCompiler *compiler, int var)
{
    
    TableEntry symbol;
    
    if(lookUpSymbol(var, compiler->symTab))
    {
        symbol = getSymbol(var, compiler->symTab);
        
    }
    
    compiler->hml[compiler->inscount++] = READ * MEMSIZE + symbol.location; //add hml instr to the hml memory
    
    return READ * MEMSIZE + symbol.location;
}


void printMem(short int mem[256])
{
    printf("\n");
    printf("      ");
    for (short int i = 0; i < 16; ++i)
    {
        printf("%4hX ", i);
    }
    printf("\n");
    printf("    0");
    printf(" %04hX ", mem[0]);
    short int j = 1;
    for (int i = 1; i < 256; ++i)
    {
        if (i % 16 == 0)
        {
            printf("\n");
            printf("%4hX0 ", j);
            ++j;
        }
        printf("%04hX ", mem[i]);
    }
    printf("\n");
}

void generateInstructions(char *command, char vars[], PeepCompiler *compiler)
{
    
    if(strcmp(command, "print") == 0)
    {
        printConverter(compiler, vars[0]);
    }
    else if(strcmp(command, "goto") == 0)
    {
        gotoConverter(compiler, atoi(vars));
    }
    else if(strcmp(command, "input") == 0)
    {
        inputConverter(compiler, vars[0]);
    }
    else if(strcmp(command, "end") == 0)
    {
        endConverter(compiler);
    }
    
}


void  firstPass(PeepCompiler *compiler, char *fileName)
{

    char mem[256][256];

    FILE* f; 
        
    f = fopen(fileName,"r");
    int ii = 0;
    // If file opened successfully, then add it to the memory array of peep.
    if ( f )
    {
        
        char word[256];
        while ((fgets(word, 256, f)))
        {
            
            strcpy(mem[ii], word);
            ii++;
        }
        printf("\n");
    }
    else
    {
        printf("Failed to open the file\n");
    }

    for(int i = 0; i < ii; ++i)
    {
        
        // Declaration of delimiter
        const char s[4] = " ";
        char* token;

        // Use of strtok
        // get first token
        token = strtok(mem[i], s);
        
        TableEntry entry;
        

        //Inserting line number in symbol table---------
        int line = atoi(token);
        entry.type = 'L';
        entry.location = compiler->inscount;
        entry.symbol = atoi(token);
        compiler->symTab[compiler->symSize++] = entry;
        //-----------------------------------------------

        //Getting command token--------------------------
        token = strtok(NULL, s);
        char *command;
        command = token;
        //-----------------------------------------------

        token = strtok(NULL, s);
        
        //Control flow based on the command of the line
        if (strcmp(command,"rem") != 0 && strcmp(command, "end") != 0 && strcmp(command, "goto") != 0)
        {
                //Table entry struct incase a symbol needs to be inserted in the symbol table
                TableEntry entry;
                

                //If token is a variable
                if(isalpha((unsigned int) token[0]))
                {
                    //If symbol is in the symbol table
                    if(lookUpSymbol(token[0], compiler->symTab))
                    {
                        entry = getSymbol(token[0], compiler->symTab);
                    }
                    //If symbol is not in the symbol table then insert it
                    else
                    {
                        entry.type = 'V';
                        entry.location = compiler->datacount--;
                        entry.symbol = token[0];
                        compiler->symTab[compiler->symSize++] = entry;
                    }
                }

                //If token is a constant 
                else
                {
                    //If symbol is in the symbol table
                    if(lookUpSymbol(atoi(token), compiler->symTab))
                    {
                        entry = getSymbol(atoi(token), compiler->symTab);
                    }
                    //If symbol is not in the symbol table then insert it
                    else
                    {
                        entry.type = 'C';
                        entry.location = compiler->datacount--;
                        entry.symbol = atoi(token);
                        compiler->symTab[compiler->symSize++] = entry;
                    }
                }
            generateInstructions(command, token, compiler);
        }
        else if (strcmp(command, "end") == 0)
        {
            generateInstructions(command, "", compiler);
        }
        else if (strcmp(command, "goto") == 0)
        {
            generateInstructions(command, token, compiler);
        }
        
    }
}

int main(int argc, char *argv[]){

    /*
    *Do not delete the commented code below. Might need it incase we need to read file from the terminal.
    */
    // FILE *fp;
    // char *filename;
    // Check if a filename has been specified in the command
    // if (argc < 2)
    // {
    //     printf("Missing Filename\n");
    //     //return(1);
    // }
    // else
    // {
    //     filename = argv[1];
    //     printf("Filename : %s\n", filename);
    // }
    
    PeepCompiler compiler;
    initCompiler(&compiler);
    firstPass(&compiler, "myFile.peep");
    printMem(compiler.hml);
    printSymTab(compiler.symTab, compiler.symSize);
    return(0);
}
