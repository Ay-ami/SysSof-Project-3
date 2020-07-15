#ifndef __LEX_H
#define __LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// COP 3402
// Project 2- Lexical Analyzer
// Due 6/19/2020
// Victor Torres
// Maya Awad

//This is the final version at 7:35pm


/*
Based on Wirth’s definition for EBNF we have the following rule:
[ ] means an optional item,
{ } means repeat 0 or more times.
Terminal symbols are enclosed in quote marks.
A period is used to indicate the end of the definition of a syntactic class.
*/

#define MAX_DIGITS 5
#define MAX_ID_LENGTH 11

//                            0       1         2          3      4     5       6        7      8       9      10      11       12      13
char * RESERVED_WORDS[] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd", "begin", "end"};
char SPECIAL_SYMBOLS[] = {'+', '-', '*', '/', '(' , ')', '=', ',' , '.', '<', '>',  ';' , ':' };
char *LITERAL_ID[] = {"", " ", "id", "number", "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=",
"(", ")", ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure",
"write", "read", "else", ":"};

typedef enum {
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomessym = 20, beginsym = 21, endsym=22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33, colonsym = 34
}token_type;

struct token{
    int ID;
    int value;
    char name[MAX_ID_LENGTH];
}token;
struct token tokenStorage[500];
int globalTestFire = 3;
// opens the file

FILE *openFile(char fileName[], char mode[], FILE *fp)
{
    fp=fopen(fileName, mode);
    if(fp==NULL)
    {
        printf("Could not open %s\n", fileName);
        exit(0);
    }
    return fp;
}

 //print character by character and ignores comments
void removeComments (FILE *fp, FILE *fpw, char *arr)
{
    char c, lc = 0; //"character" and "last character"
    int commentFlag = 0, i = 0;
    while(!feof(fp))
    {
        fscanf(fp, "%c", &c);
        // checks to see if it's the beginning of a comment
        if ( c == '*' )
            if ( lc == '/' )
                commentFlag = 1;
        // prints if it's not inside of a comment and if it's not the very first thing scanned
        if(commentFlag == 0 && lc != NULL)
        {
            fprintf(fpw, "%c", lc); // yes we print "last character" and not the current one
            arr[i] = lc;
            i++;
        }
        // checks if a comment has ended
        if( c == '/')
            if ( lc == '*' )
            {
                commentFlag = 0;
                c = NULL; // no this doesn't have anything to do with that NULL up there, it's just a patch for a dumb quirk
            }
        // assigns the current character to "last character"
        lc = c;
    }
    //arr[i] = '\0';
}

// returns 1 if true, returns 0 if false, unused as of right now
int isTrueString (char* bigStr, int place, int whichWord)
{
    int i = 0;
    char word[strlen(RESERVED_WORDS[whichWord])]; // word is as long as the reserved word we want to compare it to
    strcpy(word, RESERVED_WORDS[whichWord]);// copy the reserved word into the empty string "word"

    // if the string would have been too long for bigStr then it is false automatically
    if (strlen(bigStr) < place + strlen(word))
        return 0;
    // check letter by letter
    for (i = 0 ; i < strlen(word) ; i++)
    {
        if (word[i] != bigStr[place + i])
            return 0;
    }
    // check if it is the whole word or simply a substring
    if(bigStr[place + i] != ' ')
        return 0;

    // return the word's length if it truly is the same word
    return 1;
}

int symbolToToken(char symbol){ //exchanges char for token ident tag
    switch((int) symbol){
        case (int) '+':
            return plussym;
        case (int) '-':
            return minussym;
        case (int) '*':
            return multsym;
        case (int) '/':
            return slashsym;
        case (int) '=':
            return eqsym;
        case (int) '<':
            return lessym;
        case (int) '>':
            return gtrsym;
        case (int) '(':
            return lparentsym;
        case (int) ')':
            return rparentsym;
        case (int) ',':
            return commasym;
        case (int) ';':
            return semicolonsym;
        case (int) '.':
            return periodsym;
        case (int) ':':
            return colonsym;
        default:
            return 0;
    }
    return 0;
}

// prints out lexeme table
void lexemeTable (FILE *fpw, struct token *tokens)
{
    fprintf(fpw, "\n\nLexeme Table: \n");
    fprintf(fpw, "lexeme \t\t token type\n");

    for(int i = 0 ; tokens[i].ID != 0 ; i++)
    {
        if ( tokens[i].ID == 2 ) // if var
        {
            fprintf(fpw, "%-11s   %6d\n", tokens[i].name, tokens[i].ID);
        }
        else if (tokens[i].ID == 3)
        {
             fprintf(fpw, "%-11d   %6d\n", tokens[i].value, tokens[i].ID);
        }
        else{
            fprintf(fpw, "%-11s   %6d\n", LITERAL_ID[tokens[i].ID], tokens[i].ID);
        }
    }

}
// prints out lexeme list
void lexemeList (FILE *fpw, struct token *tokens)
{
    fprintf(fpw, "\n\nLexeme List\n");

    for(int i = 0 ; tokens[i].ID != 0 ; i++)
    {
        fprintf(fpw, "%d ",tokens[i].ID);
        if (tokens[i].ID == 2)
        {
            fprintf(fpw, "%s ", tokens[i].name);
        }
        fprintf(fpw, " ");
    }
}

void tokenizer(char *codeArr)
{

  int i = 0;
  int j = 0;
  int lineCount = 0;
  struct token currToken;
  //struct token tokenStorage [strlen(codeArr)];



  while(codeArr[i] != '\0'){

    //Skips any white space in the string
    if (codeArr[i] == ' '){
      i++;
      continue;
    }

    if (codeArr[i] == '\n' || codeArr[i] == '\r'){
      lineCount++;
      i++;
      continue;
    }

    if(codeArr[i] == '\t'){
      i++;
      continue;
    }

    //Checks for reserved letters--------------------------------------//
    if(codeArr[i] >= 'a' && codeArr[i] <= 'z'){

      switch((int) codeArr[i]){
      case (int) 'b':
        if(codeArr[i+1] == 'e' && codeArr[i+2] == 'g' && codeArr[i+3] == 'i' && codeArr[i+4] == 'n' && !(codeArr[i+5]>='a' && codeArr[i+5]<='z') && !(codeArr[i+5]>='A' && codeArr[i+5]<='Z'))
        {
            i = i + 5;
            currToken.ID = beginsym;
            tokenStorage[j]=currToken;
            j++;
            continue;
        }

        case (int) 'e':

          /*if(codeArr[i+1] == 'l' && codeArr[i+2] == 's' && codeArr[i+3] == 'e' && !(codeArr[i+4]>='a' && codeArr[i+4]<='z') && !(codeArr[i+4]>='A' && codeArr[i+4]<='Z')){
            i = i + 4;
            currToken.ID = elsesym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
          else */if(codeArr[i+1]=='n' && codeArr[i+2]=='d' && !(codeArr[i+3]>='a' && codeArr[i+3]<='z') && !(codeArr[i+3]>='A' && codeArr[i+3]<='Z'))
          {
              i = i + 3;
              currToken.ID = endsym;
              tokenStorage[j] = currToken;
              j++;
              continue;
          }
        case (int) 'i':

          if(codeArr[i+1] == 'f' && !(codeArr[i+2]>='a' && codeArr[i+2]<='z') && !(codeArr[i+2]>='A' && codeArr[i+2]<='Z')){
            i = i + 2;
            currToken.ID = ifsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 't':

          if(codeArr[i+1]=='h' && codeArr[i+2]=='e' && codeArr[i+3]=='n' && !(codeArr[i+4]>='a' && codeArr[i+4]<='z') && !(codeArr[i+4]>='A'&&codeArr[i+4]<='Z')){

            i = i + 4;
            currToken.ID = thensym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 'w':

          if(codeArr[i+1] == 'h' && codeArr[i+2] == 'i' && codeArr[i+3] == 'l'  && codeArr[i+4] =='e'&& !(codeArr[i+5]>='a' && codeArr[i+5]<='z') && !(codeArr[i+5]>='A'&&codeArr[i+5]<='Z')){

            i = i + 5;
            currToken.ID = whilesym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }else if(codeArr[i+1] == 'r' && codeArr[i+2] == 'i' && codeArr[i+3] == 't'  && codeArr[i+4] =='e'&& !(codeArr[i+5]>='a' && codeArr[i+5]<='z') && !(codeArr[i+5]>='A'&&codeArr[i+5]<='Z')){

            i = i + 5;
            currToken.ID = writesym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 'd':

          if(codeArr[i+1]=='o' && !(codeArr[i+2]>='a' && codeArr[i+2]<='z') && !(codeArr[i+2]>='A'&&codeArr[i+2]<='Z')){

            i = i + 2;
            currToken.ID = dosym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 'c':

          /*if(codeArr[i+1]=='a'&&codeArr[i+2]=='l' && codeArr[i+3]=='l' && !(codeArr[i+4]>='a' && codeArr[i+4]<='z') && !(codeArr[i+4]>='A'&&codeArr[i+4]<='Z')){

            i = i + 4;
            currToken.ID = callsym;
            tokenStorage[j] = currToken;
            j++;
            continue;

          }else*/ if(codeArr[i+1] == 'o' && codeArr[i+2] == 'n' && codeArr[i+3] == 's'  && codeArr[i+4] =='t'&& !(codeArr[i+5]>='a' && codeArr[i+5]<='z') && !(codeArr[i+5]>='A'&&codeArr[i+5]<='Z')){

            i = i + 5;
            currToken.ID = constsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 'v':
          if(codeArr[i+1]=='a'&&codeArr[i+2]=='r' && !(codeArr[i+3]>='a' && codeArr[i+3]<='z') && !(codeArr[i+3]>='A'&&codeArr[i+3]<='Z')){

            i = i + 3;
            currToken.ID = varsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        case (int) 'o':
           if(codeArr[i+1]=='d'&&codeArr[i+2]=='d' && !(codeArr[i+3]>='a' && codeArr[i+3]<='z') && !(codeArr[i+3]>='A'&&codeArr[i+3]<='Z')){

            i = i + 3;
            currToken.ID = oddsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        /*case (int) 'p':

          if(codeArr[i+1]=='r' &&codeArr[i+2] == 'o' &&codeArr[i+3] == 'c' &&codeArr[i+4] == 'e' &&codeArr[i+5] == 'd' &&codeArr[i+6] == 'u' &&codeArr[i+7] == 'r' &&codeArr[i+8] == 'e' && !(codeArr[i+9]>='a' && codeArr[i+9]<='z') && !(codeArr[i+9]>='A'&&codeArr[i+9]<='Z')){

            i = i + 9;
            currToken.ID = procsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }*/
        case (int) 'r':

          if(codeArr[i+1] == 'e' &&codeArr[i+2] == 'a' &&codeArr[i+3] == 'd' && !(codeArr[i+4]>='a' && codeArr[i+4]<='z') && !(codeArr[i+4]>='A'&&codeArr[i+4]<='Z')){

            i= i + 4;
            currToken.ID = readsym;
            tokenStorage[j] = currToken;
            j++;
            continue;
          }
        default:
          break;

      }

    }//end of checking for reserved letters / words---------------------------//

    //Will check for reserved symbols----------------//

    //This will check if the current token is going to match with a character
    currToken.ID = symbolToToken(codeArr[i]);
    if(currToken.ID != 0){

      int nextSym;

      if(currToken.ID == lessym){

        nextSym = symbolToToken(codeArr[i+1]);
        if(nextSym == gtrsym){

            currToken.ID = neqsym;
            tokenStorage[j] = currToken;
            j++;
            i = i + 2;
            continue;

        }else if(nextSym == eqsym){

            currToken.ID = leqsym;
            tokenStorage[j] = currToken;
            j++;
            i = i + 2;
            continue;

        }

      }
      if(currToken.ID == gtrsym){

        nextSym = symbolToToken(codeArr[i+1]);
        if(nextSym == eqsym){
            currToken.ID = geqsym;
            tokenStorage[j] = currToken;
            j++;
            i = i + 2;
            continue;
        }

      }
      if(currToken.ID == colonsym){

        nextSym = symbolToToken(codeArr[i+1]);
        if(nextSym == eqsym){

          currToken.ID = becomessym;
          tokenStorage[j] = currToken;
          j++;
          i = i + 2;
          continue;

        }else{
          //there is a invalid symbol instead, this kills the lexical analyzer
          printf("ERROR: INVALID SYMBOL AT LINE NUMBER %d", lineCount);
          exit(0);
        }

      }

      nextSym = symbolToToken(codeArr[i+1]);
      tokenStorage[j] = currToken; //add current token to tokens array if a single symbol
      j++;
      i++;
      continue;

    }//---------------------end of reserved symbols-----------------------//


    //Will check numbers-------------------------------------------------//
    if(codeArr[i]>='0' && codeArr[i]<='9'){
      int k;
      int tempSum;
      int digitCount;

      for(k=i; k < MAX_DIGITS + i; k++){
        if((codeArr[k] == ' ') || (codeArr[k] == '\t') || (codeArr[k] == '\n') || (codeArr[k] >= '!' && codeArr[k] <= '/') || (codeArr[k] >= ':' && codeArr[k] <= '@')){

          break;

        }
        if((codeArr[k] >= 'A' && codeArr[k] <= 'Z') || (codeArr[k] >= 'a' && codeArr[k] <= 'z')){

          printf("ERROR: VARIABLE NAME STARTS WITH NUMBER ON LINE %d\n", lineCount);
          exit(0);

        }
      }

      k = i;
      tempSum = 0;

      digitCount = 1;
      while(codeArr[k] >= '0' && codeArr[k] <= '9'){
        if(digitCount > 5){
          printf("ERROR: NUMBER EXCEEDS MAX LENGTH ON LINE %d\n", lineCount);
          exit(0);
        }else{
          k++;
          digitCount++;
        }
      }

      digitCount--;
      int m;

      for( m=k-digitCount; m < k ; m ++){
        tempSum += (codeArr[m]- '0') * pow(10,digitCount-1);
        digitCount--;
      }

      currToken.ID = numbersym;
      currToken.value = tempSum;
      tokenStorage[j] = currToken;
      j++;

      i = k;//This needs to be tested
      continue;
    }//end of checking numbers-----------------------------------------------------------------------------------//

    //This is the last case where it must be an identifier-------------------------------------------------------//
    if((codeArr[i]>='a' && codeArr[i]<='z') || (codeArr[i]>='A'&& codeArr[i]<='Z') || (codeArr[i]>='0' && codeArr[i]<='9')|| (codeArr[i] == '_')){

      int identifier_length = 0;
      char identifier[MAX_ID_LENGTH];
      int p, n;
      n = i;

      for(p = 0; p < MAX_ID_LENGTH; p++){
          identifier[p] = '\0';
      }

      while(((codeArr[n]>='a' && codeArr[n]<='z')|| (codeArr[n]>='A'&& codeArr[n]<='Z') || (codeArr[n]>='0'&& codeArr[n]<='9')) || (codeArr[n] == '_')){

          if(identifier_length > MAX_ID_LENGTH-1){
              //there is an identifier with a length too long, this kills the lexical analyzer
              printf("ERROR: IDENTIFIER LENGTH TOO LONG ON LINE %d\n", lineCount);
              exit(0);
          }
          identifier[identifier_length] = codeArr[n];
          identifier_length++;
          n++;

      }

      //generate the identifier token
      currToken.ID=identsym;
      strcpy(currToken.name, identifier);
      tokenStorage[j] = currToken;
      j++;
      i+=identifier_length;
      continue;

      }else{
          printf("ERROR: INVALID SYMBOL ON LINE %d\n", lineCount);
          exit(0);
    }//-------------------------------------end of checking identifier------------------------------------------------//

  }
  tokenStorage[j].ID = 0; //ID 0 signals the end of the struct

}
int lex()
{
    FILE *fp, *fpw;
    fp=openFile("input.txt", "r", fp);
    fpw=openFile("output.txt", "w", fpw);

    char *codeArr = calloc(1000, sizeof(char));
    removeComments(fp, fpw, codeArr);
    //printf("code arr length: %d", strlen(codeArr));

    // tokinizer function:
    tokenizer(codeArr);

    // print lexeme table
    lexemeTable(fpw, tokenStorage);

    // print lexeme list
    lexemeList(fpw, tokenStorage);


    fclose(fp);
    fclose(fpw);
    //free(tokenStorage);


    return 0;
}
#endif
