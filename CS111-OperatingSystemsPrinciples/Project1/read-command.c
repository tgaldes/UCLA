// UCLA CS 111 Lab 1 command reading

#include "command-internals.h"
#include "command.h"
#include "command-stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#define WORD_SIZE 100
typedef struct command Command;
typedef struct stack Stack;
typedef struct elementNode elementNode;
typedef struct commandNode commandNode;
void sort(elementNode *input);
elementNode* createElementNodeLinkedList(Command *readin[], int length);
elementNode* linkCommands(elementNode *operator, elementNode *left, elementNode *right);
void linkCommandDebug(elementNode *input);
int tylerTest();
int debug = 0;
int linkCommandsDebug = 0;
int lineNo = 1;

//create two stacks to hold elementNode
//one for operators one for operands

Stack *operators;
Stack *operands;
void initializeStacks(Stack** operands, Stack** operators);
Command* mostRecent;


//WRITE FUNCTION THAT RETURNS OPERATOR PRECEDENCE
int determine_precedence(char* op)
{
    if (!strcmp(";", op))
        return 1;
    
    if ((!strcmp("&&", op)) || (!strcmp("||", op)))
        return 2;
    
    if (!strcmp("|", op))
        return 3;
    
    if ((!strcmp("(", op)) || (!strcmp(")", op)))
        return 4;
    
    else
        return -1;
}
//AUX FUNCTIONS
int isWord(char a)
{
    if (isalnum(a))
        return 1;
    switch(a)
    {
        case '!':
        case '%':
        case '+':
        case ',':
        case '-':
        case '.':
        case '/':
        case ':':
        case '@':
        case '^':
        case '_':
            return 1;
        default:
            return 0;
    }
}

int isOp(char a)
{
    switch(a)
    {
        case ';':
        case '(':
        case ')':
        case '|':
        case '&':
            return 1;
        default:
            return 0;
    }
}

//MODIFIED WHITE SPACE DETECTOR THAT RETURNS FALSE ON NEWLINE
int whiteSpaceNL(char a)
{
    if (a == '\n')
        return 0;
    else if (isspace(a))
        return 1;
    return 0;
}

const char* commandString(enum command_type type, char *a)
{
    if (type == AND_COMMAND)
        strcpy(a, "AND_COMMAND");
    
    else if (type == OR_COMMAND)
        strcpy(a, "OR_COMMAND");
    
    else if (type == SEQUENCE_COMMAND)
        strcpy(a, "SEQUENCE_COMMAND");
    
    else if (type == PIPE_COMMAND)
        strcpy(a, "PIPE_COMMAND");
    
    else if (type == OPEN_COMMAND)
        strcpy(a, "OPEN_COMMAND");
    
    else if (type == CLOSE_COMMAND)
        strcpy(a, "CLOSE_COMMAND");
    
    else if (type == SUBSHELL_COMMAND)
        strcpy(a, "SUBSHELL_COMMAND");
    
    else if (type == SIMPLE_COMMAND)
        strcpy(a, "SIMPLE_COMMAND");
    
    return a;
}

void wordend(int *input, int *output, int *wordStart, char *temp_word, elementNode *parsed, char **word_array, int curWords, int wordlen)
{
        //MESSY REPETITIVE CODE UGH...
        //---------------------------------
        temp_word[wordlen] = '\0';
        *wordStart = 0;
    
        //STORE IN INPUT IF INPUT FLAG ON
        if (*input)
        {
            parsed->command->input = temp_word;
            *input = 0;
        }
        
        //STORE IN OUTPUT IF OUTPUT FLAG ON
        else if (*output)
        {
            parsed->command->output = temp_word;
            *output = 0;
        }
        
        else
            word_array[curWords-1] = temp_word;
        //-------------------------------------
}




        //-------------------------------------
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{

	//create command stream to store command nodes
	command_stream* myStream = malloc(sizeof(command_stream));
	myStream->head = NULL;
	myStream->tail = NULL;

    char temp_char = '\0';
    char* temp_word = NULL;
    int index = 0;
    int n = 0; //USED FOR WORD SIZE
    
    //CURRENT WORD COUNT IN ELEMENT NODE
    int curWords = 0, wordlen = 0;
    
    //OPERATOR COUNTS
    int AND_COUNT = 0, OR_COUNT = 0, INPUT_COUNT = 0, OUTPUT_COUNT = 0, mostRecentNL = 0, tempNLCount = 0; //<-----FOR OPERAND SEQUENCE
    /*TEMPNLCOUNT COUNTS THE NUMBER OF NEWLINES WITHIN AN OPERAND SEQUENCE WHILE MOST RECENTNL SAVES THE NUMBER OF NEW LINES AFTER A WORD IS FORMED. WHEN WE SUBTRACT TEMPNLCOUNT - MOSTRECENTNL, WE OBTAIN THE NUMBER OF NEWLINES IN BETWEEN THE CURRENT WORD AND PREVIOUS MOST RECENT WORD. IF THIS NUMBER IS EQUAL TO 2, WE KNOW WE HAVE TO FORM A NEW COMMAND TREE*/
    
    //SET FLAGS
    int input = 0, output = 0, isOperator = 0, isOperand = 0, isWhiteSpace = 0, isNL = 0, isComment = 0, wordStart = 0, endParen = 0, addSequence = 0;
    
    //CREATE ELEMENT NODE
    elementNode *parsed = NULL;
    elementNode *head = NULL;
    
    //ARRAY OF C-STRINGS
    char** word_array = NULL;
    
    
    //ITERATE THROUGH INPUT
    while (temp_char != EOF)
    {
        temp_char = (*get_next_byte)(get_next_byte_argument);
	if (temp_char == '`')
	  {
	    fprintf(stderr, "Syntax error on line %i\n", lineNo);
            printf("ERROR, LINE %i: CANNOT HAVE SINGLE QUOTE!", lineNo);
            exit(-1);
	  }
        
        //----------------EVENTS TRIGGERED FROM OLD FLAGS ----------------//
        //IF COMMENT, IGNORE ALL UNTIL NEWLINE
        if (isComment && temp_char != '\n')
        {
            index++;
            continue;
        }
        
        //OPERATOR ERROR (INCORRECT AND COUNT)
        if ((temp_char != '&') && AND_COUNT==1)
        {
			fprintf(stderr, "Syntax error on line %i\n", lineNo);
            printf("ERROR, LINE %i: CANNOT HAVE 1 '&'!", lineNo);
            exit(-1);
        }
        
        //IF WORD ENDS
        if (isOperand && wordStart && !isWord(temp_char))
            wordend(&input, &output, &wordStart, temp_word, parsed, word_array, curWords, wordlen);
        
        //---------------------------------------------------------------//
        
        //SET I/O FLAGS
        if (temp_char == '>')
        {
            OUTPUT_COUNT++;
            
            //I/O MUST BE PRECEDED BY OPERAND ON SAME LINE
            if (tempNLCount > mostRecentNL)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: I/O Redirection must be preceded by operand.", lineNo);
                exit(-1);
            }
            
            //OPERATOR PRECEDING
            if (isOperator && !endParen)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Cannot have operator precede output redirection.", lineNo);
                exit(-1);
            }
            
            //CAN ONLY HAVE ONE OUTPUT
            if (OUTPUT_COUNT > 1)
			{
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Cannot have more than one output.", lineNo);
                exit(-1);
            }
            
            //SET FLAGS
            output = 1;
            isWhiteSpace = 0;
            isNL = 0;
        }

        
        else if (temp_char == '<')
        {
            INPUT_COUNT++;
          
            //I/O MUST BE PRECEDED BY OPERAND ON SAME LINE
            if (tempNLCount > mostRecentNL)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: I/O Redirection must be preceded by operand.", lineNo);
                exit(-1);
            }
            
            //OPERATOR PRECEDING ERROR
            if (isOperator && !endParen)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Cannot have operator precede output redirection.", lineNo);
                exit(-1);
            }
            
            //OUTPUT MUST BE AFTER INPUT
            if (OUTPUT_COUNT > 0)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Cannot have output before input.", lineNo);
                exit(-1);
            }
            
            
            //CAN ONLY HAVE ONE INPUT
            if (INPUT_COUNT > 1)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Cannot have more than one input.", lineNo);
                exit(-1);
            }
            
            //SET FLAGS
            input = 1;
            isWhiteSpace = 0;
            isNL = 0;
        }
        
        //SET COMMENT FLAG
        else if (temp_char == '#')
        {
            if ((!isWhiteSpace && !isNL) && index)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                printf("ERROR, LINE %i: Ordinary token cannot precede comment.", lineNo);
                exit(-1);
            }
            isComment = 1;
        }
        
        else if (isWord(temp_char))
        {
            //END ELEMENT NODE IF CHAR ADDED AFTER I/O REDIRECTION
            if  ((OUTPUT_COUNT > output)  || ((INPUT_COUNT > input) && !output))
            {

                if (!endParen)
                {
                    parsed->command->u.word = word_array;
                    parsed->numWords = curWords;
                }
                
                else
                    endParen = 0;
            }


            if ((tempNLCount - mostRecentNL) >= 2 && (!isOperator || endParen))
			{
				if(debug)
					printf("NEW Command TREE\n\n");
				//This is where the magic happens
				//First create a new pointer to the head of the linked list
				elementNode *toSort = head;
				//printElementNodeLinkedList(head);
				sort(toSort);
				//create command pointer to the root of the tree
				Command* rootCommand = operands->head->command;
				//print_command(rootCommand);
				//clear stacks and make new linked list
				operators->init(operators);
				operands->init(operands);
				head = NULL;
				parsed = NULL;
				
				//create a new CommandNode that points to this tree
				commandNode* newCommandNode = malloc(sizeof(commandNode));
				newCommandNode->command = rootCommand;
				newCommandNode->next = NULL;
				
				//place node into the command stream
				if(myStream->head == NULL)
				{
					myStream->head = newCommandNode;
					myStream->tail = newCommandNode;
				}
				//stream has one element
				else if(myStream->head == myStream->tail)
				{
					myStream->head->next = newCommandNode;
					myStream->tail = newCommandNode;
				}
				//stream has more than one element
				else
				{
					myStream->tail->next = newCommandNode;
					myStream->tail = newCommandNode;
				}
			}    
            
            //ADD SEQUENCE NODE IF OPERAND SEPARATED BY NEWLINE
            if (addSequence)
            {
                //IF NODE DOESN'T EXIST, MAKE IT
                if (!parsed)
                {
                    head = parsed = (elementNode*) malloc(sizeof(struct elementNode));
                    parsed->prev = NULL;
                    parsed->next = NULL;
                }
                
                //IF NODE EXISTS, MAKE NEXT NODE
                else
                {
                    parsed->next = (elementNode*) malloc(sizeof(struct elementNode));
                    parsed->next->prev = parsed;
                    parsed->next->next = NULL;
                    parsed = parsed->next;
                }
                
                //CREATE COMMAND STRUCT
                parsed->command = (struct command*) malloc(sizeof(struct command));
                parsed->command->input = NULL;
                parsed->command->output = NULL;
                parsed->numWords = 0;
                parsed->command->type = SEQUENCE_COMMAND;
                
                //SET ADD SEQUENCE FLAG TO 0
                addSequence = 0;
            }
            
            
            
/*            //END ELEMENT NODE IF CHAR ADDED AFTER I/O REDIRECTION
            if  ((OUTPUT_COUNT > output)  || ((INPUT_COUNT > input) && !output))
            {

                if (!endParen)
                {
                    parsed->command->u.word = word_array;
                    parsed->numWords = curWords;
                }
                
                else
                    endParen = 0;
            }*/
            
            if (!wordStart)
            {
                n=1;
                
                //ONLY INCREASE WORD COUNT IF NOT PART OF INPUT OR OUTPUT
                //END SUBSHELL NODE IF NOT I/O REDIRECTION AFTER
                if (!input && !output)
                {
                    curWords++;
                    endParen = 0;
                }
                
                wordlen = 0;
                
                //SET FLAG
                wordStart = 1;
                
                //INIT TEMP WORD
                temp_word = (char*) malloc(sizeof(char)*WORD_SIZE);
                
                //IF WORDSTART OFF AND PREVIOUS CHAR NOT OPERAND, CREATE NEW ELEMENT NODE
                //ALSO CREATE NEW ELEMENT NODE IF WORD FOLLOWS INPUT OR OUTPUT OR NEWLINE
                if (!isOperand || ((OUTPUT_COUNT > output)  || ((INPUT_COUNT > input) && !output)))
                {
                    curWords = 1;
                    
                    if (!endParen)
                    {
                        //IF NODE DOESN'T EXIST, MAKE IT
                        if (!parsed)
                        {
                            head = parsed = (elementNode*) malloc(sizeof(struct elementNode));
                            parsed->prev = NULL;
                            parsed->next = NULL;
                        }
                        
                        //IF NODE EXISTS, MAKE NEXT NODE
                        else
                        {
                            parsed->next = (elementNode*) malloc(sizeof(struct elementNode));
                            parsed->next->prev = parsed;
                            parsed->next->next = NULL;
                            parsed = parsed->next;
                        }
                        
                        //CREATE COMMAND STRUCT
                        parsed->command = (struct command*) malloc(sizeof(struct command));
                        parsed->command->type = SIMPLE_COMMAND;
                        parsed->command->input = NULL;
                        parsed->command->output = NULL;
                        
                        //INITIALIZE TEMP WORD_ARRAY
                        word_array = (char**) malloc(sizeof(char*)*2);
						word_array[1] = NULL;
                        
                        //SET FLAGS FOR BEGINNING OF OPERAND SEQUENCE
                        OUTPUT_COUNT = 0;
                        INPUT_COUNT = 0;
                        tempNLCount = 0;
                    }
                }
                
                //REALLOCATE WORD_ARRAY IF ALREADY IN OPERAND SEQUENCE
                else if (curWords > 1)
				{
                    word_array = (char**) realloc(word_array, (curWords+1) * sizeof(char*));
					word_array[curWords] = NULL;
				}
                
            }
            
            if (wordlen+2 >= WORD_SIZE) //LAST BYTE HAS TO BE '\0', so index < WORD_SIZE - 1
            {
                n++;
                //RESIZE TEMP_WORD BUFFER
                temp_word = (char*) realloc(temp_word, sizeof(char)*n*WORD_SIZE);
            }
            
            //SET FLAGS
            isWhiteSpace = 0;
            isNL = 0;
            isOperand = 1;
            isOperator = 0;
            mostRecentNL = tempNLCount;
            temp_word[wordlen] = temp_char;
            wordlen++;
        }
        
        else if (isOp(temp_char))
        {
            //DO NOT ADDSEQUENCE IF OPERATOR COMES FIRST
            if (addSequence)
            {
                if (temp_char == '(')
                {
                    //IF NODE DOESN'T EXIST, MAKE IT
                    if (!parsed)
                    {
                        head = parsed = (elementNode*) malloc(sizeof(struct elementNode));
                        parsed->prev = NULL;
                        parsed->next = NULL;
                    }
                    
                    //IF NODE EXISTS, MAKE NEXT NODE
                    else
                    {
                        parsed->next = (elementNode*) malloc(sizeof(struct elementNode));
                        parsed->next->prev = parsed;
                        parsed->next->next = NULL;
                        parsed = parsed->next;
                    }
                    
                    //CREATE COMMAND STRUCT
                    parsed->command = (struct command*) malloc(sizeof(struct command));
                    parsed->command->input = NULL;
                    parsed->command->output = NULL;
                    parsed->numWords = 0;
                    parsed->command->type = SEQUENCE_COMMAND;
                }
                
                addSequence = 0;
            }
            
            if (((tempNLCount - mostRecentNL) >= 2) && temp_char=='(')
            {
                if(debug)
                    printf("NEW Command TREE\n\n");
                //This is where the magic happens
                //First create a new pointer to the head of the linked list
                elementNode *toSort = head;
                //printElementNodeLinkedList(head);
                sort(toSort);
                //create command pointer to the root of the tree
                Command* rootCommand = operands->head->command;
                //print_command(rootCommand);
                //clear stacks and make new linked list
                operators->init(operators);
                operands->init(operands);
                head = NULL;
                parsed = NULL;
                
                //create a new CommandNode that points to this tree
                commandNode* newCommandNode = malloc(sizeof(commandNode));
                newCommandNode->command = rootCommand;
                newCommandNode->next = NULL;
                
                //place node into the command stream
                if(myStream->head == NULL)
                {
                    myStream->head = newCommandNode;
                    myStream->tail = newCommandNode;
                }
                //stream has one element
                else if(myStream->head == myStream->tail)
                {
                    myStream->head->next = newCommandNode;
                    myStream->tail = newCommandNode;
                }
                //stream has more than one element
                else
                {
                    myStream->tail->next = newCommandNode;
                    myStream->tail = newCommandNode;
                }
            }

            
            if (tempNLCount > 0)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                //printf("ERROR, LINE %i: Operator must be preceded by operand.", lineNo);
                exit(-1);
            }
            
            //ERROR IF INPUT OR OUTPUT PRIOR TO OPERATOR
            if (output || input)
            {
				fprintf(stderr, "Syntax error on line %i\n", lineNo);
                //printf("ERROR, LINE %i: Cannot have I/O redirection without operand.", lineNo);
                exit (-1);
            }
            
            //IF WORD ENDS, OPERATOR IS DELIMINTER FOR ELEMENTNODE
            if (isOperand)
            {
                if (!endParen)
                {
                    parsed->command->u.word = word_array;
                    parsed->numWords = curWords;
                }
                
                else
                    endParen = 0;
                
                //SET I/O COUNTS
                OUTPUT_COUNT = 0;
                INPUT_COUNT = 0;
            }
            
            //FIRST DETERMINE WHETHER CURRENT CHAR IS 'AND' OR 'OR'
            if (temp_char == '&')
                AND_COUNT++;
            
            if (temp_char == '|')
                OR_COUNT++;
            
            //ONLY MAKE ELEMENT NODE ON FIRST INSTANCE OF SEEING OPERATOR
            if (AND_COUNT < 2 && OR_COUNT < 2)
            {
                //IF NODE DOESN'T EXIST, MAKE IT
                if (!parsed)
                {
                    head = parsed = (elementNode*) malloc(sizeof(struct elementNode));
                    parsed->prev = NULL;
                    parsed->next = NULL;
                }
                
                //IF NODE EXISTS, MAKE NEXT NODE
                else
                {
                    parsed->next = (elementNode*) malloc(sizeof(struct elementNode));
                    parsed->next->prev = parsed;
                    parsed->next->next = NULL;
                    parsed = parsed->next;
                }
                
                //CREATE COMMAND STRUCT
                parsed->command = (struct command*) malloc(sizeof(struct command));
                parsed->command->input = NULL;
                parsed->command->output = NULL;
                parsed->numWords = 0;
                
                if (temp_char == '(')
                {
                    //NEWLINES CAN ONLY BEGIN WITH SIMPLE COMMMAND OR OP
                    tempNLCount = 0;
                    parsed->command->type = OPEN_COMMAND;
                }
                else if (temp_char == ')')
                {
                    //NEWLINES CAN ONLY BEGIN WITH SIMPLE COMMMAND OR OP
                    tempNLCount = 0;
                    parsed->command->type = CLOSE_COMMAND;
                    endParen = 1;
                }
                else if (temp_char == '|')
                    parsed->command->type = PIPE_COMMAND;
                else if (temp_char == ';')
                    parsed->command->type = SEQUENCE_COMMAND;            }
            
            else if (AND_COUNT == 2)
            {
                parsed->command->type = AND_COMMAND;
                AND_COUNT = 0;
            }
            
            else if (OR_COUNT == 2)
            {
                parsed->command->type = OR_COMMAND;
                OR_COUNT = 0;
            }
            
            if (temp_char != ')')
                endParen = 0;
            
            isOperand = 0;
            isOperator = 1;
            isWhiteSpace = 0;
            isNL = 0;
        }
        
        else if (whiteSpaceNL(temp_char))
        {
            isWhiteSpace = 1;
            isNL = 0;
        }
        
        else if (temp_char == '\n')
        {
            if (isComment)
                isComment = 0;
            
            tempNLCount++;
            
            //END OPERAND NODE IF NOT INPUT/OUTPUT
            if (isOperand && !input && !output)
            {
                if (!endParen)
                {
                    parsed->command->u.word = word_array;
                    parsed->numWords = curWords;
                }
                
                else
                    endParen = 0;
                
                //IF ONLY ONE NEWLINE IN BETWEEN OPERAND, ADD SEQUENCE NODE IN BETWEEN
                if (tempNLCount == 1)
                    addSequence = 1;
                
                isOperand = 0;
            }
            if (endParen && tempNLCount == 1)
                addSequence = 1;
            
            if (tempNLCount > 1)
                addSequence = 0;
            
            isNL = 1;
            isWhiteSpace = 0;
            isOperand = 0;
            lineNo++;
        }
        
        
        //EVENTS THAT CHANGE FLAGS
        //---IF CHAR NOT OPERATOR, MAKE 'AND' and 'OR' COUNTS 0
        if (!isOp(temp_char))
        {
            AND_COUNT = 0;
            OR_COUNT = 0;
        }
        
        index++;
        continue;
    }
    
    //IF WORD ENDS BECAUSE OF 'EOF'
    if (isOperand)
    {
        if (wordStart)
            wordend(&input, &output, &wordStart, temp_word, parsed, word_array, curWords, wordlen);
        
        //NODE ALSO ENDS BECAUSE 'EOF' IS ELEMENTNODE DELIM
        if (!endParen)
        {
            parsed->command->u.word = word_array;
            parsed->numWords = curWords;
        }
        
        else
            endParen = 0;
    }
    
    if(debug) 
		printf("*****FINAL COMMAND TREE*******\n\n\n");
	//This is where the magic happens
	//First create a new pointer to the head of the linked list
	elementNode *toSort = head;
	sort(toSort);
	//print_command(operands->head->command);
	//create command pointer to the root of the tree
	Command* rootCommand = operands->head->command;
	//clear stacks and make new linked list
	operators->init(operators);
	operands->init(operands);
	head = NULL;
	
	//create a new CommandNode that points to this tree
	commandNode* newCommandNode = malloc(sizeof(commandNode));
	newCommandNode->command = rootCommand;
	newCommandNode->next = NULL;
	
	//place node into the command stream
	if(myStream->head == NULL)
	{
		myStream->head = newCommandNode;
		myStream->tail = newCommandNode;
	}
	//stream has one element
	else if(myStream->head == myStream->tail)
	{
		myStream->head->next = newCommandNode;
		myStream->tail = newCommandNode;
	}
	//stream has more than one element
	else
	{
		myStream->tail->next = newCommandNode;
		myStream->tail = newCommandNode;
	}
    //printf("\n");

  return myStream;
        //printf("p->numWords = %i\n", p->numWords);
    /*int j = 0;
    while (0)
    {
        char a[30];
        int i = 0;
        while (i < p->numWords)
        {
            printf("NODE%i: %s\n",j,p->command->u.word[i]);
            i++;
        }
        
        //PRINT INTPUT
        printf("NODE%i Input: %s\n",j,p->command->input);
        
        //PRINT OUTPUT
        printf("NODE%i Output: %s\n",j,p->command->output);
        
        //PRINT TYPE
        printf("NODE%i Type: %s\n\n", j, commandString(p->command->type, a));
        
        j++;
        p = p->next;
    }*/

}

command_t
read_command_stream (command_stream_t s)
{
	if(s != NULL)
	{
		commandNode* currentTree = s->head;
		if(s->head == NULL)
			return NULL;
		s->head=s->head->next;
		return currentTree->command;
	}
  return NULL;
}

int tylerTest()
{
	Command *test = malloc (sizeof(struct command));
	test->input = "Tyler";
	test->type = SIMPLE_COMMAND;

	struct command *test1 = malloc (sizeof(struct command));
	test1->input = "Tyler1";
	test1->type = PIPE_COMMAND;
	Command *script[9];
	struct command *test2 = malloc (sizeof(struct command));
	test2->input = "Tyler2";
	test2->type = SIMPLE_COMMAND;
	struct command *test3 = malloc (sizeof(struct command));
	test3->input = "Tyler3";
	test3->type = AND_COMMAND;
	struct command *test4 = malloc (sizeof(struct command));
	test4->input = "Tyler4";
	test4->type = OPEN_COMMAND;
	struct command *test5 = malloc (sizeof(struct command));
	test5->input = "Tyler5";
	test5->type = SIMPLE_COMMAND;
	struct command *test6 = malloc (sizeof(struct command));
	test6->input = "Tyler6";
	test6->type = CLOSE_COMMAND;
	struct command *test7 = malloc (sizeof(struct command));
	test7->input = "Tyler7";
	test7->type = AND_COMMAND;
	struct command *test8 = malloc (sizeof(struct command));
	test8->input = "Tyler8";
	test8->type = SIMPLE_COMMAND;


	script[0] = test;
	script[1] = test1;
	script[2] = test2;
	script[3] = test3;
	script[4] = test4;
	script[5] = test5;
	script[6] = test6;
	script[7] = test7;
	script[8] = test8;
	//printElementNodeLinkedList(ll);
	//printElementNodeLinkedList(ll);

	return 1;
}



void sort(elementNode *input){

	//set up stacks
	initializeStacks(&operands, &operators);
	//trach most recent type
	mostRecent = malloc(sizeof(Command));
	mostRecent->type = SUBSHELL_COMMAND;
	while(input != NULL)
	{
		elementNode* nextNode = input->next;
		switch(input->command->type){
			case AND_COMMAND:
			case OR_COMMAND:
				if(debug)
					printf("Encountered AND/OR command in sort\n");
				if(mostRecent->type == AND_COMMAND || mostRecent->type == OR_COMMAND || mostRecent->type == PIPE_COMMAND || mostRecent->type == SEQUENCE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				if(operators->size == 0 || operators->head->command->type == SEQUENCE_COMMAND || operators->head->command->type == OPEN_COMMAND)
				{
					operators->push(operators, input); 
					break;
				}
				//all ops of >= precedenct (&& || |)
				if(operators->head->command->type == PIPE_COMMAND || operators->head->command->type == AND_COMMAND || operators->head->command->type == OR_COMMAND)
				{
				//loop unitl strictly lower precedence
				while(operators->size != 0 && (operators->head->command->type == PIPE_COMMAND || operators->head->command->type == AND_COMMAND || operators->head->command->type == OR_COMMAND))
					{
						elementNode* tempelementNode = operators->pop(operators);
						if(debug)
							printf("Popped operator with input %s while pushing AND/OR\n", tempelementNode->command->input);
						//pop off our two highest operands
						//link operands to the operator we popped off
						//then we push that onto the operand stack
						elementNode* tempRight = NULL;
						elementNode* tempLeft = NULL;
						if(operands->size > 0)
							tempRight = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
						if(operands->size > 0)
							tempLeft = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
						elementNode* newOperand = linkCommands(tempelementNode, tempLeft, tempRight);
						operands->push(operands, newOperand);	
						if(linkCommandsDebug)
						{
							linkCommandDebug(newOperand);
						}

					}

				//after you are done popping push the AND/OR 
				operators->push(operators,input);
				}
				if(debug)
				{
					printf("\nStacks & LL after pushing AND/OR\n\n");
					operators->printStack(operators);
					operands->printStack(operands);
				}
				break;
			case PIPE_COMMAND:
				if(debug)
					printf("Encountered PIPE command in sort\n");
				if(mostRecent->type == AND_COMMAND || mostRecent->type == OR_COMMAND || mostRecent->type == PIPE_COMMAND || mostRecent->type == SEQUENCE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				if(operators->size == 0 || operators->head->command->type != PIPE_COMMAND)
				{
					operators->push(operators, input);
					break;
				}
				
				//all ops of >= precedenct (|)
				if(operators->head->command->type == PIPE_COMMAND)
				{
				//loop until strictly lower precedence
				while(operators->size != 0 && operators->head->command->type == PIPE_COMMAND)
					{
						elementNode* tempelementNode = operators->pop(operators);
						if(debug)
							printf("Popped operator with input %s while pushing PIPE\n", tempelementNode->command->input);
						//pop off our two highest operands
						//link operands to the operator we popped off
						//then we push that onto the operand stack
						elementNode* tempRight = NULL;
						elementNode* tempLeft = NULL;
						if(operands->size > 0)
							tempRight = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
						if(operands->size > 0)
							tempLeft = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
						elementNode* newOperand = linkCommands(tempelementNode, tempLeft, tempRight);
						operands->push(operands, newOperand);	
						if(linkCommandsDebug)
						{
							linkCommandDebug(newOperand);
						}

					}

				//after you are done popping push the PIPE 
				operators->push(operators,input);
				}
				if(debug)
				{
					printf("\nStacks & LL after pushing PIPE\n\n");
					operators->printStack(operators);
					operands->printStack(operands);
				}
				break;
			case SIMPLE_COMMAND:
				if(debug)
					printf("Encountered SIMPLE command in sort\n");
				if(mostRecent->type == SIMPLE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				if(mostRecent->type == CLOSE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				operands->push(operands, input);
				break;
			case OPEN_COMMAND:
				if(debug)
					printf("Encountered OPEN command in sort\n");
				if(mostRecent->type == CLOSE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				if(mostRecent->type == SIMPLE_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				//push all open parens onto the stack
				operators->push(operators, input);
				break;
			case CLOSE_COMMAND:
				if(debug)
					printf("Encountered a CLOSE command in sort\n");
				if(mostRecent->type == OPEN_COMMAND)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				
				//until we find an open paren, we pop operators off the stack
				//and link them with the top two ops, then push them
				//on the operand stack
				
				while(operators->size != 0 && operators->head->command->type != OPEN_COMMAND)
				{
					elementNode* tempelementNode = operators->pop(operators);
					if(debug)
						printf("Popped operator with input %s while pushing CLOSE\n", tempelementNode->command->input);
					//pop off our two highest operands
					//link operands to the operator we popped off
					//then we push that onto the operand stack
						elementNode* tempRight = NULL;
						elementNode* tempLeft = NULL;
						if(operands->size > 0)
							tempRight = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
						if(operands->size > 0)
							tempLeft = operands->pop(operands);
						else{ 
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
							exit(-1);}
					elementNode* newOperand = linkCommands(tempelementNode, tempLeft, tempRight);

					//push our newly linked command and print it if needed
					operands->push(operands, newOperand);
					if(linkCommandsDebug)
						linkCommandDebug(newOperand);
				}
				
				//now the top operator is an OPEN or we have an error
				//we'll create a subshell command that will point to the top
				//operand on the stack, and push the subshell on the stack
				if(operators->size == 0)
				{
					fprintf(stderr, "Syntax error on line %i\n", lineNo);
					exit(-1);
				}
				Command *newSubshell = malloc(sizeof(Command));
				elementNode* newSubshellNode = malloc(sizeof(elementNode));
				newSubshellNode->command = newSubshell;

				//if we are debugging, give the new subshell input
				if(linkCommandsDebug)
					newSubshell->input = "Newly created subshell";
				newSubshell->type = SUBSHELL_COMMAND;
				elementNode* tempTop = operands->pop(operands);
				newSubshellNode->command->u.subshell_command = tempTop->command;

				//link the input and output from CLOSE to the new SUBSHELL
				newSubshell->input = input->command->input;
				newSubshell->output = input->command->output;
				operands->push(operands, newSubshellNode);
				
				//the last thing we need is to pop the OPEN off of the stack
				operators->pop(operators);
				//if we are debugging linkCommands function, print specs
				if(linkCommandsDebug)
					linkCommandDebug(newSubshellNode);
				break;
			case SEQUENCE_COMMAND:
				if(debug)
					printf("Encountered SEQUENCE_COMMAND command in sort\n");
				if(mostRecent->type == AND_COMMAND || mostRecent->type == OR_COMMAND || mostRecent->type == PIPE_COMMAND || mostRecent->type == SEQUENCE_COMMAND)
				{
					fprintf(stderr, "Incorrect # of parentheses\n");
					exit(-1);
				}
				if(operators->size == 0)
				{
					operators->push(operators, input);
					break;
				}
				
				//loop until the operator stack is empty becuase ; is the lowest precedent
				while(operators->size != 0 && operators->head->command->type != OPEN_COMMAND)
					{
						elementNode* tempelementNode = operators->pop(operators);
						if(debug)
							printf("Popped operator with input %s while pushing PIPE\n", tempelementNode->command->input);
						//pop off our two highest operands
						//link operands to the operator we popped off
						//then we push that onto the operand stack
						elementNode* tempRight = NULL;
						elementNode* tempLeft = NULL;
						if(operands->size > 0)
							tempRight = operands->pop(operands);
						else{ 
					fprintf(stderr, "Incorrect # of parentheses\n");
							exit(-1);}
						if(operands->size > 0)
							tempLeft = operands->pop(operands);
						else{ 
					fprintf(stderr, "Incorrect # of parentheses\n");
							exit(-1);}
						
						elementNode* newOperand = linkCommands(tempelementNode, tempLeft, tempRight);
						operands->push(operands, newOperand);	
						if(linkCommandsDebug)
						{
							linkCommandDebug(newOperand);
						}

					}

				//after you are done popping push the SEQUENCE_COMMAND 
				operators->push(operators,input);
				
				if(debug)
				{
					printf("\nStacks & LL after pushing SEQUENCE\n\n");
					operators->printStack(operators);
					operands->printStack(operands);
				}
				break;
			case SUBSHELL_COMMAND:
				if(debug)
					printf("Encountered SUBHSELL command in sort\n");
				break;
			default:
				break;
			}
	//update most recent command
	mostRecent = input->command;
	input = nextNode;
	}
	if(debug)
	{
		printStack(operators);
		printStack(operands);
	}
	while(operators->size != 0)
	{
		elementNode* tempelementNode = operators->pop(operators);
		if(debug)
			printf("Popped operator with input %s while in final loop\n", tempelementNode->command->input);
		//pop off our two highest operands
		//link operands to the operator we popped off
		//then we push that onto the operand stack
		elementNode* tempRight = NULL;
		elementNode* tempLeft = NULL;
		if(operands->size > 0)
			tempRight = operands->pop(operands);
		else{ 
					fprintf(stderr, "Incorrect # of operands\n");
			exit(-1);}

		//SEQUENCE COMMANDS DONT NEED TWO OPERANDS
		//We'll handle those first
		/*if(operators->size == 0 && tempelementNode->command->type == SEQUENCE_COMMAND)
		{
			//switch the left and the right since we
			//popped the "right" first
			tempLeft = tempRight;
			tempRight->command = NULL;
		}*/
		if(operands->size > 0)
			tempLeft = operands->pop(operands);
		else{ 
					fprintf(stderr, "Incorrect # of operands\n");
			exit(-1);}
		elementNode* newOperand = linkCommands(tempelementNode, tempLeft, tempRight);
		operands->push(operands, newOperand);	
		if(linkCommandsDebug)
		{
			linkCommandDebug(newOperand);
		}
	}
}		

		


elementNode* createElementNodeLinkedList(Command *readin[], int length)
{
	//create linked list of command nodes that will be
	//used in the stack (read in order)
	elementNode *head= malloc(sizeof(elementNode));
	elementNode *tail= malloc(sizeof(elementNode));
	int i;
	for(i = 0; i < length; i++)
	{
		if(i == 0)
		{
			head->command = readin[i];
			tail->command = readin[i];
			if(debug)
				printf("The head's input is: %s\n", head->command->input);
		}
		else if(i == 1)
		{
			elementNode *temp = malloc(sizeof(elementNode));
			temp->command = readin[i];
			temp->prev = head;
			temp->next = NULL;
			head->next = temp;
			tail= temp;
		}
		else
		{
			elementNode *temp = malloc(sizeof(struct elementNode));
			temp->command = readin[i];
			temp->prev = tail;
			temp->next = NULL;
			tail->next = temp;
			tail = temp;
			if(debug)
				printf("The new tail's input is: %s\n", tail->command->input);
		}
	}
	return head;
}
//only use this function when the head of the tree 
//returned will be AND SEQUENCE_COMMAND PIPE OR
elementNode* linkCommands(elementNode *operator, elementNode *left, elementNode *right)
{
	//assign the two children of the new head
	//of the sub-tree we are creating here
	operator->command->u.command[0] = left->command;
	operator->command->u.command[1] = right->command;

	//patch up memory leaks
	free(left);
	free(right);

	return operator;
}

void linkCommandDebug(elementNode *input)
{
	if(input->command->type != SUBSHELL_COMMAND)
	{
		printf("\nSpecs for the thing we just linked:\n\n");
		printf("It's input: %s\n", input->command->input);
		printf("It's left child: %s\n", input->command->u.command[0]->input);
		printf("It's right child: %s\n", input->command->u.command[1]->input);
	}
	else
	{
		printf("\nSpecs for the subshell we just linked\n");
		printf("Its child: %s\n", input->command->u.subshell_command->input);
	}
}


void initializeStacks(Stack** operands, Stack** operators)
{
	//allocate space
	(*operators) = malloc(sizeof(Stack));
	(*operands) = malloc(sizeof(Stack));

	//initialize the functions of both stacks
	(*operators)->init = &init;
	(*operators)->push = &push;
	(*operators)->pop = &pop;
	(*operators)->printStack = &printStack;

	(*operands)->init = &init;
	(*operands)->push = &push;
	(*operands)->pop = &pop;
	(*operands)->printStack = &printStack;

	//initialize both stacks
	(*operators)->init(*operators);
	(*operands)->init(*operands);
}	
