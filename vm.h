#ifndef __VM_H
#define __VM_H
// COP 3402
// Project 1- P Machine
// Due 6/5/2020
// Victor Torez
// Maya Awad

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"

// given default values
#define MAX_DATA_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

struct instruct{
    int OP; // opcode
    int  L; // L lexigraphical level
    int  M; // M
}instruct;
struct instruct Code[MAX_CODE_LENGTH];
int currentCodeIndex=0;
int currLevel = 0;

typedef enum {
    LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8,
    SIO1 = 9, SIO2 = 10, SIO3 = 11
}opcodes;
typedef enum{ // for when op=2
    RET = 0, NEG = 1, ADD = 2, SUB = 3, MUL = 4, DIV = 5, ODD = 6,
    MOD = 7, EQL = 8, NEQ = 9, LSS = 10, LEQ = 11, GTR = 12, GEQ = 13
}OPRCodes;

int v=0; // this determines what v is
// opens the file
/*
FILE *openFile(char fileName[], char mode[], FILE *fp)
{
    fp=fopen(fileName, mode);
    if(fp==NULL)
    {
        printf("Could not open %s\n", fileName);
        exit(0);
    }
    //printf("file opened\n");
    return fp;
}
*/
// returns the number of lines in the input file,
int countLines(FILE *fp)
{
    int count = 0;
    int temp;

    while(!feof(fp))
    {
    // stores entire row made up of 3 integers into a temp variable to keep track of position
      fscanf(fp, "%d %d %d", &temp, &temp, &temp);
      count++;
    }
    // send file pointer back at the start so we can read it again
    rewind(fp);
    return count;
}

// returns a string based on OP
char *interpertOP(int op)
{
    // where a, b, and c are the read integers
    // line, OP, L, M
    char *OP = (char *)malloc(3);
    switch (op)
    {
        case 1:
            strcpy(OP, "LIT");
            break;
        case 2:
            strcpy(OP, "OPR");
            break;
        case 3:
            strcpy(OP, "LOD");
            break;
        case 4:
            strcpy(OP, "STO");
            break;
        case 5:
            strcpy(OP, "CAL");
            break;
        case 6:
            strcpy(OP, "INC");
            break;
        case 7:
            strcpy(OP, "JMP");
            break;
        case 8:
            strcpy(OP, "JPC");
            break;
        case 9:
            strcpy(OP, "SIO");
            break;
        case 10:
            strcpy(OP, "SIO");
            break;
        case 11:
            strcpy(OP, "SIO");
            break;
        default:
            // some default/err case
            break;
    }
    return OP;
}

void printOutput1(struct instruct ir[], int size, FILE *fpw)
{
    //printf("printOutput1\n");

    fprintf(fpw, "Line, OP, L, M \n");

    for (int i = 0; i<size; i++)
    {
        fprintf(fpw, "%3d ", i);
        fprintf(fpw, "  %s ", interpertOP(ir[i].OP));
        fprintf(fpw, "%d ", ir[i].L);
        fprintf(fpw, "%d \n", ir[i].M);
    }
    fprintf(fpw, "\n\n\n\n");

}
int base(int L, int base, int stack[]){
  int b1 = base;
  while(L > 0){
    b1 = stack[b1 - 1];
    L--;
  }
  return b1;
}

void printStack(int stack[], int SP, int BP, FILE *fp)
{
    fprintf(fp, "\t");
    for (int i = MAX_DATA_STACK_HEIGHT-1; i>=SP ; i--)
    {
        if (i == BP && (BP != ( MAX_DATA_STACK_HEIGHT-1) ) )
        {
            fprintf(fp, "| ");
            //printf("| ");
            BP = BP-7; //assumes each Activation Record can hold 6 inputs
        }
        fprintf(fp, "%d ", stack[i]);
        //printf("%d ", stack[i]);
    }
    //printf("\n");
    fprintf(fp, "\n ");
}

void printStackCons(int stack[], int SP, int BP)
{
    printf("\t");
    for (int i = MAX_DATA_STACK_HEIGHT-1; i>=SP ; i--)
    {
        if (i == BP && (BP != ( MAX_DATA_STACK_HEIGHT-1) ) )
        {
            printf("| ");
            //printf("| ");
            BP = BP-7; //assumes each Activation Record can hold 6 inputs
        }
        printf("%d ", stack[i]);
        //printf("%d ", stack[i]);
    }
    //printf("\n");
    printf("\n ");
}
int vm()
{
    //printf("\ninside vm.h\n");
    // CPU Register
    int SP = MAX_DATA_STACK_HEIGHT;
    int BP = SP - 1;
    int PC = 0;
    int halt = 1;
    struct instruct IR; // holds a signular struct from Code

    /*
    FILE *fp;
    fp=openFile("input.txt", "r", fp);// remember to keep "input.txt" in the same file as the main c file so that the compiler can find it


    // determining the total number of instructions
    int num = countLines(fp); //num = number of instructions

    // Building and filling out the struct array
    struct instruct Code[num];
    int count = 0;
    while(!feof(fp)) // "while not at the end of the file"
    {
      fscanf(fp, "%d %d %d", &Code[count].OP, &Code[count].L, &Code[count].M); // "scan in 3 numbers per line"
      count++;
    }
*/
    // print the first half of the output based on the now built Code array
    FILE *fpw = openFile("output.txt", "w", fpw);
    printOutput1(Code, currentCodeIndex, fpw); //num, fpw);
    //fclose(fp);

    int *stack = calloc(MAX_DATA_STACK_HEIGHT, sizeof(int));


    fprintf(fpw, "\t\t pc  bp  sp \tstack\n");
    fprintf(fpw, "Initial Values:  %2d %3d %4d\n ", 0, 999, 1000);


    int i = 0;
    while (halt != 0)
    {

        //printf("inside while loop \n");
        IR = Code[PC];
        int lastPC = PC;
        PC++;
        switch(IR.OP)
        {

          //Load Constant aka LIT
          case 1:
              //printf("case %s\n", interpertOP(IR.OP));
            SP--;
            stack[SP] = IR.M;
            break;

          //OPR
          case 2:
              //printf("case %s\n", interpertOP(IR.OP));
            switch(IR.M){
              //RTN
              case 0:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "RTN");
                SP = BP + 1;
                BP = stack[(SP - 3)];
                PC = stack[(SP - 4)];
                break;

              //NEG
              case 1:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "NEG");
                stack[SP] = -1 * stack[SP];
                break;

              //ADD
              case 2:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "ADD");
                SP = SP + 1;
                stack[SP] = stack[SP] + stack[SP - 1];
                break;


              //SUB
              case 3:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "SUB");
                SP = SP + 1;
                stack[SP] = stack[SP] - stack[SP - 1];
                break;

              //MUL
              case 4:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "MULT");
                SP = SP + 1;
                stack[SP] = stack[SP] * stack[SP - 1];
                break;

              //DIV
              case 5:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "DIV");
                SP = SP + 1;
                stack[SP] = stack[SP] / stack[SP - 1];
                break;

              //ODD
              case 6:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "ODD");
                stack[SP] = stack[SP] % 2;
                break;

              //MOD
              case 7:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "MOD");
                SP = SP + 1;
                stack[SP] = stack[SP] % stack[SP - 1];
                break;

              //EQL
              case 8:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "EQL");
              SP = SP + 1;
                stack[SP] = (stack[SP] == stack[SP - 1]);
                break;

              //NEQ
              case 9:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "NEQ");
              SP = SP + 1;
                stack[SP] = (stack[SP] != stack[SP - 1]);
                break;

              //LSS
              case 10:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "LSS");
                SP = SP + 1;
                stack[SP] = (stack[SP] < stack[SP - 1]);
                break;

              //LEQ
              case 11:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "LEQ");
                SP = SP + 1;
                stack[SP] = (stack[SP] <= stack[SP - 1]);
                break;

              //GTR
              case 12:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "GTR");
                SP = SP + 1;
                stack[SP] = (stack[SP] > stack[SP - 1]);
                break;

              //GEQ
              case 13:
                  //printf("case %s, %s\n", interpertOP(IR.OP), "GEQ");
                SP = SP + 1;
                stack[SP] = (stack[SP] >= stack[SP - 1]);
                break;

              default:
                fprintf(stderr, "Invalid M Entered.");
              break;
            } // end of second switch
            break;
          //Load to stackister
          case 3:
              //printf("case %s\n", interpertOP(IR.OP));
            SP--;
            stack[SP] = stack[(base(IR.L, BP, stack) - IR.M)];
            break;

          //Store to Stack
          case 4:
              //printf("case %s\n", interpertOP(IR.OP));
            stack[base(IR.L, BP, stack) - IR.M] = stack[SP];
            SP++;
            break;

          //Call func
          case 5:
              //printf("case %s\n", interpertOP(IR.OP));
            stack[SP - 1] = 0;
            stack[SP - 2] = base(IR.L, BP, stack);
            stack[SP - 3] = BP;
            stack[SP - 4] = PC;
            BP = SP - 1;
            PC = IR.M;
            break;

          //Increment
          case 6:
              //printf("case %s\n", interpertOP(IR.OP));
            SP = SP - IR.M;
            break;

          //Jump
            case 7:
                //printf("case %s\n", interpertOP(IR.OP));
            PC = IR.M;
            break;

          //Jump of Zero
          case 8:
              //printf("case %s\n", interpertOP(IR.OP));
            if (stack[SP] == 0){
                PC = IR.M;
            }
            SP++;
            break;

          //SIO Print stack
          case 9:
              //printf("case %s\n", interpertOP(IR.OP));
            //printf("%i\n", stack[SP]);
            SP++;
          break;

          //SIO Read into stack
          case 10:
              //printf("case %s\n", interpertOP(IR.OP));
            SP--;
            scanf("%d", &stack[SP]);
            //printf("\n");
            break;

          //Set Stop
          case 11:
              //printf("case %s\n", interpertOP(IR.OP));
            halt = 0;
            break;

          default:
            fprintf(stderr, "Invalid OP Code Entered.");
        } // end of switch

        //fprintf(fpw, "Initial Values: 0  999  1000");
        fprintf(fpw, "%2d %s %d %d", lastPC, interpertOP(IR.OP), IR.L, IR.M);
        if (v==1)
        {
            printf("%2d %s %d %d", lastPC, interpertOP(IR.OP), IR.L, IR.M);
        }
        fprintf(fpw, "\t%3d %3d %4d ", PC, BP, SP);
        if (v==1)
        {
            fprintf(fpw, "\t%3d %3d %4d ", PC, BP, SP);
        }

        printStack(stack, SP, BP, fpw);
        if (v==1)
        {
            printStackCons(stack, SP, BP);
        }


    }//end of while


    return 0;
}
#endif // VM_H
