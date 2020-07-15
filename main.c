#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"
#include "vm.h"
// HW2 ->  HW3  ->  HW1
// LEX    parser    VM  ?
// this might need another step somewhere

// COP 3402
// Project 3- Parse Codegen
// Due 7/14/2020
// Victor Torres
// Maya Awad

/*
For constants: you must store kind, name and value.
For variables,: you must store kind, name, L and M.
For procedures: you must store kind, name, L and M.
*/
struct symbol
{
    int kind; 		// const = 1, var = 2, proc = 3
    char name[10];	// name up to 11 chars
	int value; 		// number (ASCII value). was "val". idk what it means by it has to be an ascii value its already a number
	int level; 		// L level, in this project will basically always be 0
	int address; 		// M address. was "addr"
	int mark;		// to indicate unavailable or deleted
}symbol;
int currAddress=3; // addresses here start at 4 in this project because we have 3 things in the AR already
int sizeOfSymbolTable = 1; // size of the symbol table currently, not the max size that's gonna be like 100 or something
                           // it starts at 1 because symbolTable starts at 1, if we ever hit symbolTable[0] that means there
                           // is no match when we search
struct symbol symbolTable [100];

// really only prototyping them so that the warnings go shhhhhh
void emit(int op, int level, int address);
void error(int errorType);
void statement();
void expression();
void term();
void factor();
void condition();
int countTokens();
void getToken();
int checkTable(struct token token);
void markVar( struct token token );
void block();
void printSymbolTable();



void emit(int op, int level, int address)
{
    if (currentCodeIndex > MAX_CODE_LENGTH)
    {
        error(1); // too much code
    }
    else
    {
        Code[currentCodeIndex].OP = op;
        Code[currentCodeIndex].L = level;
        Code[currentCodeIndex].M = address;

        currentCodeIndex++;
    }
    //printf(" emit ( %d, %d, %d )  \n", op, level, address);
}
//---->end of stuff for code generation part of the project<----//


int numTokens; //probably need to run the token struct through a function that does counter++ until it hits ID=0 to get this number
int tokenIndex = 0; // this is the index for the token struct
//struct token tokens[100]; //100 for now... changed all referances to "token" to "tokenStorage" from lex.h
struct token currToken;

int countTokens()
{
    int count = 0;
    for ( int i = 0 ; tokenStorage[i].ID != 0; i++ )
    {
        count++;
    }
    return count;
}
void getToken()
{
    //printf("  getToken() %d\n", currToken.ID);
    if ( tokenIndex == numTokens )
        error(30); // no more tokens to get
    else
    {
        currToken.ID = tokenStorage[tokenIndex].ID;
        currToken.value = tokenStorage[tokenIndex].value;
        strcpy(currToken.name, tokenStorage[tokenIndex].name);

        tokenIndex++;
    }
}
// searches if an identifier is in the symbol table already, returns index if match found
int checkTable(struct token token)
{
    if (sizeOfSymbolTable == 1)
        return 0; // there isn't even anything in the table yet
    for (int i = sizeOfSymbolTable ; i > 0 ; i-- )
    {
        if ( strcmp(token.name, symbolTable[i].name) == 0 ) // if a match is found
        {
            //printf("the search function thinks the %dth identifier in the table is %s based on the token of %s\n", i, symbolTable[i].name, token.name);

            return i;
        }
    }
    return 0; // no match has been found
}

void insertNewSymbol(struct token token, int kind)
{
    /*int kind; 		// const = 1, var = 2, proc = 3
    char name[10];	// name up to 11 chars
	int value; 		// number (ASCII value). was "val". idk what it means by it has to be an ascii value its already a number
	int level; 		// L level, in this project will basically always be 0
	int address; 		// M address. was "addr"
	int mark;*/

    symbolTable[sizeOfSymbolTable].kind = kind;
    strcpy( symbolTable[sizeOfSymbolTable].name, token.name );
    symbolTable[sizeOfSymbolTable].value = -1; // -1 is only a defult value, the actual value gets added separately (const only!)
    symbolTable[sizeOfSymbolTable].level = currLevel; //pretty much always 0 so who cares
    symbolTable[sizeOfSymbolTable].mark = 0;

    if (kind == 2)
    {
        currAddress++;
        symbolTable[sizeOfSymbolTable].address = currAddress;
                                                              // the reason we don't just start currAddress at 4 is because if we need to access
                                                            // it somewhere else outside of this function it might be 1 address ahead
        //printf("  the inserted var's adress is: %d\n", symbolTable[sizeOfSymbolTable].address);
        //printf("  current address of symbol table is now: %d\n", currAddress);
    }
    else
    {
        symbolTable[sizeOfSymbolTable].address = -1; // -1 is only a defult value because non-vars don't get addresses
    }

   // sizeOfSymbolTable++; no we do this only after we add a value

}

// instead of "deleting" things from the table, we mark them
void markVar( struct token token )
{
    int index = checkTable( token );
    if ( index == 0 )
        error(12); //undeclared identifier
    else
    {
        symbolTable[index].mark = 1;
    }

}
void error(int errorType) // this should probably be the last thing we fill out
{
    switch (errorType)
    {
        case 1:
            printf("error 1 there was an error\n");
            break;
        case 2:
            printf("Use = instead of :=\n");
            break;
        case 3:
            printf("= must be followed by a number.\n");
            break;
        case 4:
            printf("Identifier must be followed by =\n");
            break;
        case 5:
            printf("const, var, -procedure- must be followed by identifier.\n");
            break;
        case 6:
            printf("Semicolon or comma missing\n");
            break;
        case 7:
            printf("Incorrect symbol after procedure declaration\n");
            break;
        case 8:
            printf("Statement expected\n");
            break;
        case 9:
            printf("Incorrect symbol after statement part in block\n");
            break;
        case 10:
            printf("Period expected\n");
            break;
        case 11:
            printf("Semicolon between statements missing.\n");
            break;
        case 12:
            printf("Undeclared identifier.\n");
            break;
        case 13:
            printf("Assignment to constant or procedure is not allowed.\n");
            break;
        case 14:
            printf("Assignment operator expected.\n");
            break;
        case 15:
            printf("call must be followed by an identifier.\n");
            break;
        case 16:
            printf("Call of a constant or variable is meaningless.\n");
            break;
        case 17:
            printf("then	 expected.\n");
            break;
        case 18:
            printf("Semicolon or end expected.\n");
            break;
        case 19:
            printf("do expected.\n");
            break;
        case 20:
            printf("Incorrect symbol following statement.\n");
            break;
        case 21:
            printf("Relational operator expected.\n");
            break;
        case 22:
            printf("Expression must not contain a procedure identifier.\n");
            break;
        case 23:
            printf("Right parenthesis missing.\n");
            break;
        case 24:
            printf("The preceding factor cannot begin with this symbol.\n");
            break;
        case 25:
            printf("An expression cannot begin with this symbol.\n");
            break;
        case 26:
            printf("This number is too large.\n");
            break;
        case 27:
            printf("duplicate identifier name\n");
            break;
        case 28:
            printf("read must be followed by identifier\n");
            break;
        case 29:
            printf("write must be followed by identifier\n");
            break;
        case 30:
            printf("there are no more tokens to read\n");
            break;
        case 31:
            printf("cannot declare and initialize a var at the same time\n");
            break;
        case 32:
            printf("use := instead of =\n");
            break;
        case 33: printf("expected + or -\n");
            break;
        default:
            printf("default error\n");
            break;
    }
    exit(0);
}
void block()
{
    // for some reason block increases current level
    //currLevel++;

    //printf("in block\n");
    // all the inputs from HW1 start with Jump to instruction 0
    emit(JMP, 0, 1);

    // TA says "keep track of the number of variables"
    int numVars = 0;

    if ( currToken.ID == constsym )// check for a constant declaration
    {
        //printf("in constsym\n");
        do  // we have at lest 1 constant declaration, so do it at lest once
        {
            getToken(); // *if theres a problem around here we, I think could try moving getToken outside of the do/while loop*
            // this next token must be an identifier
            if ( currToken.ID != identsym )
            {
                error(5); //"const, var, -procedure- must be followed by identifier"
            }
            // we check if this identifier is in the symbol table already

            // checkTable will take in the current token and check it against the symbol table, returns index if it is found,
            // returns 0 if no match found
            if ( checkTable(currToken) != 0 )
            {
                 error(27); // duplicate identifier name
            }
            else
            {
                // this is a new identifier!
                /* this block is all handled by insertNewSymbol()
                strcpy( symbolTable[sizeOfSymbolTable].name, currToken.name );
                symbolTable[sizeOfSymbolTable].kind = 1; // (kind 1 = const)
                symbolTable[sizeOfSymbolTable].level = 0;
                symbolTable[sizeOfSymbolTable].mark = 0;
                symbolTable[sizeOfSymbolTable].address = 0;
                */
                insertNewSymbol(currToken, 1);
            }

            // ok now that the table knows the name of this identifier, we update token again
            getToken();

            // the next token *has* to be an equals sign
            if ( currToken.ID != eqsym )
            {
                error(4); // Identifier must be followed by =
            }

            // continue if it was an equal sign
            getToken();

            // this next token *has* to be a digit
            if ( currToken.ID != numbersym )
            {
                error(3); // "= must be followed by a number."
            }
            // if it is a digit, we can input our new const's value into the symbol table
            symbolTable[sizeOfSymbolTable].value = currToken.value;

            // we can just go ahead and officially say the symbol table is bigger
             sizeOfSymbolTable++;


            // ok, next token
            getToken();
            // if its a comma it'll start this loop again, if not we must check for a semicolon
        }
        while (currToken.ID == commasym);//there could be multiple declared
        if ( currToken.ID != semicolonsym )// constant declarations *have* to end with a semicolon
        {
            error(6); // Semicolon or comma missing
        }
        // if it does end in a semicolon, we can move on
        getToken();
    }//end of constsym

    if ( currToken.ID == varsym ) // check for a variable declaration
    {
        //printf("in varsym\n");
        do
        {
            getToken();
            // following a var, we must have an identifier symbol
            if ( currToken.ID != identsym )
            {
                error(5); // const, var, -procedure- must be followed by identifier
            }
            // if it is an identifier, we check if one such exists in the symbol table already
            if ( checkTable(currToken) != 0 )
            {
                error(27); // duplicate identifier name
            }
            // if no variable with that name exists, we add it to the table:

            insertNewSymbol(currToken, 2);
            // we can officially grow the symbol table
            sizeOfSymbolTable++;

            // we can move on
            getToken();
            // also we cannot have a = after the identifier because we do not declare and initialize variables at the same time
            if (currToken.ID == eqsym)
            {
                error(31);// cannot initialize var at this time
            }

            // we can increment the number of variables now
            numVars++;
        }
        while (currToken.ID == commasym);// similarly to above, there could be multiple variables declared
        if ( currToken.ID != semicolonsym )// variable declarations *have* to end with a semicolon
        {
            error(6); // Semicolon or comma missing
        }
        // if it does end in a semicolon, we can move on
        getToken();
    }//end of varsym

    // after const and vars, we increment the stack pointer depending on how many vars we put i think?
    emit(INC, 0, 4 + numVars);// TA: "emit(INC, , 4+numVars")

    statement();
    //getToken();
    //printf("end of block statement\n");
}

void statement()
{
    //printf("in statement\n");

    // here we use a switch statement instead of a bunch of if statements because the grammar
    // separates types of statements with an OR symbol ("|"). Unlike block() which can enter
    // constant declarations, variable declarations, and a statement at the same time
    int ID = currToken.ID;
    int saveIndex1, saveIndex2;
    int checkedTableIndex; //when we use checkTable() save the index

    switch (ID)
    {
        case identsym:; // this semicolon is here because the immediate next line is a declaration which makes it funky for some reason
            // if it is an identifier symbol, check if one with this name exists
            //printf("in identsym (in statement) %s\n", currToken.name);

            // check if the identifier has been declared already
            checkedTableIndex = checkTable(currToken);
            if (checkedTableIndex == 0) // if checkTable returned a 0, if so, then the identifier doesn't exist
            {
                error(5); //"const, var, -procedure- must be followed by identifier"
            }
            // cannot update the contents of a constant (kind=1) or a procedure (kind=3)
            if ( symbolTable[checkedTableIndex].kind == 1 || symbolTable[checkedTableIndex].kind == 3 )
            {
                error(13); // Assignment to constant or procedure is not allowed.
            }

            // the identifier exists, we can move on
            getToken();

            // the following token must be the becomes symbol (":=")
            if (currToken.ID == eqsym)
            {
                error(32); //oh no, should use := here not just =
            }
            if ( currToken.ID != becomessym )
            {
                error(14); // Assignment operator expected.
            }

            //symbolTable[checkTable(currToken, 2)].value = currToken.value;//?/////????//////////??////////??//////

            // if it is the becomes symbol, we can move on
            getToken();

            // the following *must* be an expression
            expression();

            emit(STO, 0, symbolTable[checkedTableIndex].address);

            break;

        case beginsym: // if "begin"
            //printf("in beginsym\n");
            do
            {
                getToken();
                // after "begin" must be a statement
                statement();
                //printf("after the first statement() %d\n", currToken.ID);
            }while( currToken.ID == semicolonsym ); //we can have multiple statements in a row so long as they are separated by semicolons
            if ( currToken.ID != periodsym )
            {
                error(9); // all these statements must eventually end with "end"
                          // "Incorrect symbol after statement part in block"
            }
            //getToken();
            break;

        case ifsym:
            //printf("in ifsym\n");
            getToken();
            // following "if" we need the condition
            condition();

            // the next token has to be "then"
            if (currToken.ID != thensym)
            {
                error(17); // condition must be followed by "then"
            }
            // if there is a "then", we can move on
            getToken();

            // we gotta stostore the current Code index
            saveIndex1 = currentCodeIndex;
            emit(JPC, 0, 0); // JPC = "Jump to instruction M if top stack element is 0"

            // do the statement following "then"
            statement();

            // save current code index again because who knows how much the statement() moved it
            saveIndex2 = currentCodeIndex;

            emit(JMP, 0, 0); // jump to instruction 0

            // hmmmmmmm these ones are weird, you're supposed to do something with the saved indexes of course but
            // is this it?
            Code[saveIndex1].M = currentCodeIndex;
            // there is no "else" for this project
            Code[saveIndex2].M = currentCodeIndex;

            break;

        case whilesym:
            //printf("in whilesym \n");
            getToken();
            // save jump location for the top
            saveIndex1 = currentCodeIndex;
            condition();
            // save jump location for the end
            saveIndex2 = currentCodeIndex;
            emit(JPC, 0,0);

            // the next token must be do
            if (currToken.ID != dosym)
            {
                error(19);// do expected
            }

            getToken();
            statement();
            emit(JMP, 0, saveIndex1); // perhaps emit saveIndex1?
            Code[saveIndex2].M = currentCodeIndex; //still not sure about this
            break;

        case readsym:
            //printf("in readsym\n");
            getToken();

            // next token must be dentsym
            if (currToken.ID != identsym)
            {
                error(28); // "read must be followed by identifier"
            }

            checkedTableIndex = checkTable(currToken);
            // check if the identifier exists a a const or as a var
            if ( checkedTableIndex == 0)
            {
                error(12); // "undeclared identifier!"
            }

            // read emit
            emit(SIO2, 0, 2); // there are 3 STOs and they're basically seperated by their  M  so that's why there's just a 2 here
            emit(STO, 0, symbolTable[checkedTableIndex].address); // i think this one's ok? again, L might not have to be 0

            // move on
            getToken();

            break;

        case writesym:
            //printf("in writesym\n");
            getToken();

           // next token *must* be identsym
            if (currToken.ID != identsym)
            {
                error(29); // "write must be followed by identifier"
            }

            // check if the identifier exists
            checkedTableIndex = checkTable(currToken);
            if (checkedTableIndex == 0)
            {
                error(12); // "undeclared identifier!"
            }

            // checks if a const identifier with this name exists
            if (symbolTable[checkedTableIndex].kind == 1) // if it is indeed a const
            {
                emit(LIT, 0, currToken.value); // then we can emit it as a literal
            }
            else{ // or else it is a var not a cnost
                emit(LOD, 0, symbolTable[checkedTableIndex].address); // maybe the L isnt really a 0 but eeeee
            }
            // STO1 is write
            emit(SIO1, 0, 1);
            // finally update token
            getToken();

            break;

        case endsym:
            //printf("in endsym\n");
            //printf("currtoken id: %d\n", currToken.ID);
            getToken();
            //printf("currtoken id: %d\n", currToken.ID);
            if (currToken.ID != periodsym)
            {
                error(10); // expected a period
            }
            if (currToken.ID == periodsym) // yay! the end
            {
                emit(SIO3, 0, 3); // emit halt
                return;
            }
            getToken();
            if ( tokenIndex == numTokens )
                error(10); // period expected
            break;
    }//end of switch
    //statement();

}//end statement

void expression() // expression are ["+" | "-"] term() {("+" | "-") term()}.
{
    //printf("in expression\n");
    int storeSign = plussym; // you probably don't need to initialize this but I am just in case
    int checkedSymbolTable; // stores the result of checkTable()

    if (currToken.ID == plussym || currToken.ID == minussym) // "does this expression have that optional + or - at the beginning?"
    {
        storeSign = currToken.ID;
        getToken();
        // following a starting sign, or just at the beginning, there must be a term
        term();
        // if we found that this term has a negative sign before it, we need to tell the VM to negate the term it just read
        if (storeSign == minussym)
            emit(OPR, 0, NEG); //if it has a minus sign out front, negate it
    }
    else // if theres no + or - then it's a regular term
        term();

    // moving on, there can be another +/-
    //printf("this print statement is right before it checks if there is a plus or minus. the third time this shows up the current ID should be 4 for plussym.......    currToken.ID: %d\n", currToken.ID);
    while (currToken.ID == plussym || currToken.ID == minussym)
    {
        //getToken();
        // if the next term is an identifier
        if (currToken.ID == identsym)
        {
            // does it exist
            checkedSymbolTable = checkTable(currToken);
            if (checkedSymbolTable == 0)
            {
                error(12); //undeclared identifier
            }
            // we could check for kind here but there will never be a kind 3 so, moving on

            // if it is a const we emit LIT, if it's var we emit LOD
            if (symbolTable[checkedSymbolTable].kind == 1)
            {
                emit(LIT, 0, symbolTable[checkedSymbolTable].value);
            }
            else
            {
                emit(LOD, 0, symbolTable[checkedSymbolTable].address);
            }

            // move on
            getToken();
        }// end of if


        // save if its + or - again
        storeSign = currToken.ID;
        getToken();
        term();
        if (storeSign == plussym)
        {
            emit(OPR, 0, ADD);
        }
        else{
            emit(OPR, 0, SUB);
        }
    }// end of while

}

void term()
{
    //printf("in term\n");
    int saveType; // save if it was multiply or divide
    // terms start with a factor
    factor();

    // terms can then have 0 or greater following factors separated by multiplication or division
    while (currToken.ID == slashsym || currToken.ID == multsym) // while multiply or divide
    {
        saveType = currToken.ID;
        getToken();

        factor();

        if (saveType == multsym)
            emit(OPR, 0, MUL);
        if (saveType == slashsym)
            emit(OPR, 0, DIV);
    }

}

void factor() // ident | number | "(" expression ")"
{
    //printf("in factor\n");
    int ID = currToken.ID;
    int checkedTableIndex;

    switch (ID)
    {
        case identsym:
            //printf("in identsym (in factor) %s\n", currToken.name);

            checkedTableIndex = checkTable(currToken);
            if (checkedTableIndex == 0) // if a variable identifier with that name doesn't exist, error
                error(12); // "Undeclared identifier"

            if (symbolTable[checkedTableIndex].kind == 2) // if it's a variable
            {
                emit(LOD, 0, symbolTable[checkedTableIndex].address); // assumes level is always 0, might have to fix this
            }
            if (symbolTable[checkedTableIndex].kind == 1) // if it's a constant
            {
                emit(LIT, 0, symbolTable[checkedTableIndex].value); // assumes level is always 0, might have to fix this
            }
            getToken();
            break;

        case numbersym:
            //printf("in numbersym\n");
            emit(LIT, 0, currToken.value);
            getToken();
            break;

        case lparentsym: // this is the "(" expression ")" part
            //printf("in lparentsym\n");
            getToken();
            expression();

            // there *has* to be a closing parenthesis after the expression
            if (currToken.ID != rparentsym)
            {
                error(23); // "right parenthesis missing"
            }
            getToken();
            break;

        default:
            //printf("default switch in factor\n");
            error(1); // oh oh it's not an identifier, number, or an expression enclosed in parenthesis
            break;
    }
    //getToken();

}

void condition()
{
    //printf("in condition\n");
    if (currToken.ID == oddsym) // "odd" expression
    {
        getToken();
        expression();
        //emit(ODD); // some sort of emit here
    }
    else // expression  rel-op  expression
    {
        expression();

        if (currToken.ID < 9 && currToken.ID > 14) // relational operators are 9 - 14
        {
            error(21); // relational operator expected
        }

        // save operator
        int saveOP = currToken.ID;

        // move on
        getToken();

        // second expression
        expression();

        switch (saveOP)
        {
            case eqsym:
                emit(OPR, 0, EQL);
                break;
            case neqsym:
                emit(OPR, 0, NEQ);
                break;
            case lessym:
                emit(OPR, 0, LSS);
                break;
            case leqsym:
                emit(OPR, 0, LEQ);
                break;
            case gtrsym:
                emit(OPR, 0, GTR);
                break;
            case geqsym:
                emit(OPR, 0, GEQ);
                break;
        }

    }
}
void printSymbolTable()
{
    FILE *fpw = openFile("output.txt", "a", fpw);
    fprintf(fpw, "\nSymbol Table: \n");
    for (int i = 1; i < sizeOfSymbolTable; i++)
    {
        fprintf(fpw, "kind : %d\n", symbolTable[i].kind);
        fprintf(fpw,"name : %s\n", symbolTable[i].name);
        fprintf(fpw,"value : %d\n", symbolTable[i].value);
        fprintf(fpw,"level : %d\n", symbolTable[i].level);
        fprintf(fpw,"address : %d\n", symbolTable[i].address);
        fprintf(fpw,"mark : %d\n", symbolTable[i].mark);
        fprintf(fpw, "\n");

    }
}
void printSymbolTableCons()
{
    printf("\nSymbol Table: \n");
    for (int i = 1; i < sizeOfSymbolTable; i++)
    {
        printf("kind : %d\n", symbolTable[i].kind);
        printf("name : %s\n", symbolTable[i].name);
        printf("value : %d\n", symbolTable[i].value);
        printf("level : %d\n", symbolTable[i].level);
        printf("address : %d\n", symbolTable[i].address);
        printf("mark : %d\n", symbolTable[i].mark);
        printf("\n");

    }
}
void printCodeArray()
{
    printf("\n");
    for (int i = 0; i<currentCodeIndex ; i++)
    {
        /*
        printf("OP: %d\n", Code[i].OP);
        printf("L: %d\n", Code[i].L);
        printf("M: %d\n\n", Code[i].M);
        */

        printf("%d %d %d\n", Code[i].OP, Code[i].L, Code[i].M);
    }

}

void printLexemeList()
{
    FILE *fpw = openFile("output.txt", "a", fpw);
    fprintf(fpw, "\n\nLexeme List\n");

    for(int i = 0 ; tokenStorage[i].ID != 0 ; i++)
    {
        fprintf(fpw, "%d ",tokenStorage[i].ID);
        if (tokenStorage[i].ID == 2 || tokenStorage[i].ID == 3)
        {
            fprintf(fpw, "%s ", tokenStorage[i].name);
        }
        fprintf(fpw, " ");
    }
}
void printLexemeListCons()
{
    printf("\n\nLexeme List\n");

    for(int i = 0 ; tokenStorage[i].ID != 0 ; i++)
    {
        printf("%d ",tokenStorage[i].ID);
        if (tokenStorage[i].ID == 2 || tokenStorage[i].ID == 3)
        {
            printf("%s ", tokenStorage[i].name);
        }
        printf(" ");
    }
}

int main(int argc, char **argv)
{
    int l = 0, a=0;
    // v is declared as a global in vm.h


     if (argc<2)
    {
        printf("error : please include file name\n");
    }
    if (argc == 5)
    {
        l = 1;
        a = 1;
        v = 1;
    }
    else if (argc == 3)
    {
        if (argv[2][1]==a)
        {
            a = 1;
        }
        else if (argv[2][1]==v)
        {
            v=1;
        }
        else{
            l = 1;
        }
    }
    else if (argc == 4)
    {
        if (argv[2][1]==a)
        {
            a = 1;
        }
        else if (argv[2][1]==v)
        {
            v=1;
        }
        else{
            l = 1;
        }

        if (argv[3][1]==a)
        {
            a = 1;
        }
        else if (argv[3][1]==v)
        {
            v=1;
        }
        else{
            l = 1;
        }
    }

    /*
    a = 1;
    v=1;
    l =1;
    */

    // step 1: HW2
    lex();

    // HW3
    numTokens = countTokens(); // this is so we know how big the lexeme list is
    getToken();

    block();

    //printCodeArray(); // shows the instructions we have generated, this was only useful when we didn't have vm.h

    //HW1
    vm(); // vm does its own printing

    //FILE *fpw = openFile("output.txt", "w", fpw);

    // printStack () is handled inside
    printSymbolTable();
    printLexemeList();

    if (a == 1)
    {
        printf("assembly code \n");
        printCodeArray();
    }

    if (l == 1)
    {
        printLexemeListCons();
    }


}
