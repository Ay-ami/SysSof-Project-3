#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// HW2 ->  HW3  ->  HW1
// LEX    parser    VM  ?
// this might need another step somewhere

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
	int mark;		// to indicate unavailable or delete d
}symbol;
int currAddress=4; // addresses here start at 4 in this project because we have 3 things in the AR already
int sizeOfSymbolTable = 1; // size of the symbol table currently, not the max size that's gonna be like 100 or something
                           // it starts at 1 because symbolTable starts at 1, if we ever hit symbolTable[0] that means there
                           // is no match when we search
struct symbol symbolTable [100];


//----->here is some stuff for the code generation part of the project<-----//
typedef enum {
    LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8,
    SIO1 = 9, SIO2 = 10, SIO3 = 11
}opcodes;
typedef enum{ // for when op=2
    RET = 0, NEG = 1, ADD = 2, SUB = 3, MUL = 4, DIV = 5, ODD = 6,
    MOD = 7, EQL = 8, NEQ = 9, LSS = 10, LEQ = 11, GTR = 12, GEQ = 13
}OPRCodes;
// given default values from HW1
#define MAX_DATA_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500
//struct of instructions
struct instruct{
    int OP; // opcode
    int  L; // L lexigraphical level
    int  M; // M
}instruct;
struct instruct Code[MAX_CODE_LENGTH];
int currentCodeIndex=0;
int currLevel = 0;
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
}
//---->end of stuff for code generation part of the project<----//

//---->stuff from project 2<----//
#define MAX_DIGITS 5
#define MAX_ID_LENGTH 11
struct token{
    int ID;
    int value;
    char name[MAX_ID_LENGTH];
}token;

char * RESERVED_WORDS[] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd"};
char SPECIAL_SYMBOLS[] = {'+', '-', '*', '/', '(' , ')', '=', ',' , '.', '<', '>',  ';' , ':' };
char *LITERAL_ID[] = {"", " ", "id", "number", "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=",
"(", ")", ",", ";", ".", ":=", "{", "}", "if", "then", "while", "do", "call", "const", "var", "procedure",
"write", "read", "else", ":"};

typedef enum {
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomessym = 20, lbracesym = 21, beginsym = 21, rbracesym = 22, endsym=22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32, elsesym = 33, colonsym = 34
}token_type;

//----->end of stuff from project 2<-----//

int numTokens; //probably need to run the token struct through a function that does counter++ until it hits ID=0 to get this number
int tokenIndex = 0; // this is the index for the token struct
struct token tokens[100]; //100 for now...
struct token currToken;

int countTokens()
{
    int count = 0;
    for ( int i = 0 ; tokens[i].ID != 0; i++ )
    {
        count++;
    }
    return count;
}
void getToken()
{
    printf("  getToken() %d\n", currToken.ID);
    if ( tokenIndex == numTokens )
        error(1); // no more tokens to get
    else
    {
        currToken.ID = tokens[tokenIndex].ID;
        currToken.value = tokens[tokenIndex].value;
        strcpy(currToken.name, tokens[tokenIndex].name);

        tokenIndex++;
    }
}
// searches if an identifier is in the symbol table already, returns index if match found
int checkTable(struct token token, int kind)
{
    if (sizeOfSymbolTable == 1)
        return 0; // there isn't even anything in the table yet

    if (kind == 2) // if var, the level must also at lest be equal or less than the current level
    {
         for (int i = sizeOfSymbolTable ; i > 0 ; i-- )
        {
            if ( strcmp(token.name, symbolTable[i].name) == 0 && symbolTable[i].level <= currLevel) // if a match is found
            {
                return i;
            }
        }
    }
    for (int i = sizeOfSymbolTable ; i > 0 ; i-- )
    {
        if ( strcmp(token.name, symbolTable[i].name) == 0 ) // if a match is found
        {
            return i;
        }
    }
    return 0; // no match has been found
}
void error(int errorType) // this should probably be the last thing we fill out
{
    switch (errorType)
    {
        case 1:
            printf("there was an error\n");
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

    printf("in block\n");
    // all the inputs from HW1 start with Jump to instruction 0
    emit(JMP, 0, 0);

    // TA says "keep track of the number of variables"
    int numVars = 0;

    if ( currToken.ID == constsym )// check for a constant declaration
    {
        printf("in constsym\n");
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
            if ( checkTable(currToken, 1) != 0 )
            {
                 error(27); // duplicate identifier name
            }
            else
            {
                // this is a new identifier!
                strcpy( symbolTable[sizeOfSymbolTable].name, currToken.name );
                symbolTable[sizeOfSymbolTable].kind = 1; // (kind 1 = const)
                symbolTable[sizeOfSymbolTable].level = 0;
                symbolTable[sizeOfSymbolTable].mark = 0;
                symbolTable[sizeOfSymbolTable].address = 0;
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
            // there is no level, address, or mark to store in the symbol table for this one so
            symbolTable[sizeOfSymbolTable].level = 0;
            symbolTable[sizeOfSymbolTable].mark = 0;
            symbolTable[sizeOfSymbolTable].address = 0;

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
        printf("in varsym\n");
        numVars++;
        do
        {
            getToken();
            // following a var, we must have an identifier symbol
            if ( currToken.ID != identsym )
            {
                error(5); // const, var, -procedure- must be followed by identifier
            }
            // if it is an identifier, we check if one such exists in the symbol table already
            if ( checkTable(currToken, 2) != 0 )
            {
                error(27); // duplicate identifier name
            }
            // if no variable with that name exists, we add it to the table:
            strcpy( symbolTable[sizeOfSymbolTable].name, currToken.name );
            symbolTable[sizeOfSymbolTable].kind = 2; // (kind 2 = variable)
            symbolTable[sizeOfSymbolTable].address = currAddress;
            currAddress++; // increment current address
            // there is no value to input just yet, there is no level to input, no mark either so
            symbolTable[sizeOfSymbolTable].level = 0;
            symbolTable[sizeOfSymbolTable].mark = 0;

            // we can officially grow the symbol table
            sizeOfSymbolTable++;
            // we can move on
            getToken();

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
    emit(INC, 0, currAddress);

    statement();
}

void statement()
{
    printf("in statement\n");

    // here we use a switch statement instead of a bunch of if statements because the grammar
    // separates types of statements with an OR symbol ("|"). Unlike block() which can enter
    // constant declarations, variable declarations, and a statement at the same time
    int ID = currToken.ID;
    int cx1, cx2;
    int checkedTableIndex;

    switch (ID)
    {
        case identsym:; // this semicolon is here because the immediate next line is a declaration which makes it funky for some reason
            // if it is an identifier symbol, check if one with this name exists
            printf("in identsym (in statement) %s\n", currToken.name);

            checkedTableIndex = checkTable(currToken, 2);
            if (checkedTableIndex == 0) // if checkTable returned a 0, then the identifier doesn't exist
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
            if ( currToken.ID != becomessym )
            {
                error(14); // Assignment operator expected.
            }

            // if it is the becomes symbol, we can move on
            getToken();

            // the following *must* be an expression
            expression();

            // emit(STO, ,) here, not sure what goes into it

            break;

        case beginsym: // if "begin"
            printf("in beginsym\n");
            do
            {
                getToken();
                // after "begin" must be a statement
                statement();
            }while( currToken.ID == semicolonsym ); //we can have multiple statements in a row so long as they are separated by semicolons
            if ( currToken.ID != endsym )
            {
                error(9); // all these statements must eventually end with "end"
                          // "Incorrect symbol after statement part in block"
            }
            break;

        case ifsym:
            printf("in ifsym\n");
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

            // we gotta store the current Code index
            cx1 = currentCodeIndex; // i *think* "code_index" corresponds with our "currentCodeIndex"?
            emit(JPC, 0, 0); // JPC = "Jump to instruction M if top stack element is 0"

            // do the statement following "then"
            statement();

            //save current code index again because who knows how much the statement() moved it
            cx2 = currentCodeIndex;

            emit(JMP, 0, 0); // jump to instruction 0
            Code[cx1].M = currentCodeIndex;
            // there is no "else" for this project
            Code[cx2].M = currentCodeIndex;

            break;

        case whilesym:
            printf("in whilesym \n");
            // -->save jump location for the top<--
            getToken();
            condition();
            // -->save jump location for the end <--
            emit(JPC, 0,0);

            // the next token must be do
            if (token.ID != dosym)
            {
                error(19);// do expected
            }

            getToken();
            statement();
            emit(JMP, 0, cx1);
            //code[cx2].m;// = whatever the hell "code_index" is;
            break;

        case readsym:
            printf("in readsym\n");
            getToken();

            // next token must be dentsym
            if (currToken.ID != identsym)
            {
                error(28); // "read must be followed by identifier"
            }

            // check if the identifier exists a a const or as a var
            if (checkTable(currToken, 1) == 0 || checkTable(currToken, 2) == 0)
            {
                error(12); // "undeclared identifier!"
            }
            emit(SIO2, 0, 2); // there are 3 STOs and they're basically seperated by their  M  so that's why there's just a 2 here

            // checks if a const identifier with this name exists
            checkedTableIndex = checkTable(token, 1);
            if (checkedTableIndex == 0) // if no const with that name exists, try var
            {
                checkedTableIndex = checkTable(token, 2);
            }
            // emit(STO, );

            getToken();
            break;

        case writesym:
            printf("in writesym\n");
            getToken();

           // next token *must* be identsym
            if (currToken.ID != identsym)
            {
                error(29); // "write must be followed by identifier"
            }

            // check if the identifier exists a a const or as a var
            if (checkTable(currToken, 1) == 0 || checkTable(currToken, 2) == 0)
            {
                error(12); // "undeclared identifier!"
            }

            // checks if a const identifier with this name exists
            checkedTableIndex = checkTable(token, 1);
            if (checkedTableIndex != 0) // if it is indeed a const
            {
                emit(LIT, 0, currToken.value); // then we can emit it as a literal
            }
            else{ // or else it is a var not a cnost
                emit(LOD, 0, symbolTable[checkTable(token, 2)].address); // maybe the L isnt really a 0 but eeeee
            }

            // finally update token
            getToken();

            break;

        case endsym:
            printf("in endsym\n");
            break;


    }


}//end statement

void expression() // expression are ["+" | "-"] term() {("+" | "-") term()}.
{
    printf("in expression\n");

    int storeSign = plussym; // you probably don't need to initialize this but I am just in case
    if (currToken.ID == plussym || currToken.ID == minussym) // "does this expression have that optional + or - at the beginning?"
    {
        storeSign = currToken.ID;
        getToken();
    }

    // following a starting sign, or just at the beginning, there must be a term
    term();
    // if we found that this term has a negative sign before it, we need to tell the VM to negate the term it just read
    if (storeSign == minussym)
    {
        emit(OPR, 0, NEG);
    }

    // moving on, there can be another +/-
    //getToken();

    while (currToken.ID == plussym || currToken.ID == minussym)
    {
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

    }


}

void term()
{
     printf("in term\n");
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
    printf("in factor\n");
    int ID = currToken.ID;

    switch (ID)
    {
        case identsym://; // there is a ; here because the following line is a declaration and that makes it funky
            printf("in identsym (in factor) %s\n", currToken.name);
            int check = checkTable(currToken, 2); // check using kind=2 first
            if (check == 0) // if a variable identifier with that name doesn't exist, check if a constant does
                check = checkTable(currToken, 1);
            if (check == 0) // if it *still* isn't in the table
                error(12); // "Undeclared identifier"

            if (symbolTable[check].kind == 2) // if it's a variable
            {
                emit(LOD, 0, symbolTable[check].address); // assumes level is always 0, might have to fix this
            }
            if (symbolTable[check].kind == 1) // if it's a constant
            {
                emit(LIT, 0, symbolTable[check].value); // assumes level is always 0, might have to fix this
            }
            break;

        case numbersym:
            printf("in numsym\n");
            emit(LIT, 0, currToken.value);
            break;

        case lparentsym: // this is the "(" expression ")" part
            printf("in lparentsym\n");
            getToken();
            expression();

            // there *has* to be a closing parenthesis after the expression
            if (currToken.ID != rparentsym)
            {
                error(23); // "right parenthesis missing"
            }
            break;

        default:
            error(1); // oh oh it's not an identifier, number, or an expression enclosed in parenthesis
            break;
    }
    getToken();

}

void condition()
{
    printf("in condition\n");
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
    printf("Symbol Table: \n");
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
int main()
{
    /*
    notes from looking over the instructions and watching the TA's video:

        *a big difference between project 2 and project 3 is that there are no pros in
            project 3 (thank goodness! basically, no functions)

        *another difference is that the open brackets and closing brackets+period becoming
            begin and end

        *for project 1 and 2 we really only made 1 c file, but here I think we should make a driver and also a parser
            c files. like this might not continue to be called "main.c"
        *the driver will need main(argc, **argv) where argv is and array of strings that gets input in the command
            line and argc is the number of strings
        *maybe the parser should be stuck into the end of the lexical analyzer so we don't have to pass the tokens??


    more notes from reviewing the 7/6 lecture:
        *Inside const-declaration  and/or var-declaration is where we populate the symbol table
        *Once the symbol table has been populated, you can move on to the "statement" part of the grammar
        *Once again: no procedures! 50 pts off if we have procedures! even if commented out?
        *It's "if and then" not "if and else"
        *He says to have an error function that takes in an int that corresponds to which error it is
        *One of the test cases they're gonna use is   procedure := else * call   to check that our program must
            take these are identifiers not actual procedure calls!
        *Every time you search a symbol table, keep the thing you're searching for in i=0
        *I think the addresses in the symbol table start at 4 because we had 3 things in the AR
        *We don't do any arithmetic in the symbol table
        *math is done in the code generation ie
            a:=b+c ==> LOD 0,5
                       LOD 0,6
                       ADD
                       STO 0,4
        *in this project, L is always 0 because we have no procedures/
        *
    */


    /* TEST:
    var a, b;
    {
        a:=b+1;
    }.

    Lexeme List
    29  2 a  17  2 b  18  21  2 a  20  2 b  4  3  18  22  19

    length: 15 (14 tokens + 1 ID=0)
    */

    tokens[0].ID = 29;
    tokens[1].ID = 2;
    strcpy(tokens[1].name, "a");
    tokens[2].ID = 17;
    tokens[3].ID = 2;
    strcpy(tokens[3].name, "b");
    tokens[4].ID = 18;
    tokens[5].ID = 21;
    tokens[6].ID = 2;
    strcpy(tokens[6].name, "a");
    tokens[7].ID = 20;
    tokens[8].ID = 2;
    strcpy(tokens[8].name, "b");
    tokens[9].ID = 4;
    tokens[10].ID = 3;
    tokens[11].ID = 18;
    tokens[12].ID = 22;
    //tokens[13].ID = 19;
    tokens[13].ID = 0;

    numTokens = countTokens();
    printf("numtokens: %d\n", numTokens);
    getToken();
    printf("starting token ID: %d\n", currToken.ID);
    block();

    printSymbolTable();
}
