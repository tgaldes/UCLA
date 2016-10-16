// UCLA CS 111 Lab 1 command internals

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
	OPEN_COMMAND,
	CLOSE_COMMAND,
  };
//create the subshell commands as OPEN and CLOSE, then push them on
//the stack. When you pop them, you'll create a subshell command
//when you finish the popoping down the the OPEN_COMMAND and push
//that subshell onto the operand stack after linking it

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  //set to 1 if command has exited succesfully
  int status;

  // I/O redirections, or null if none.
  char *input;
  char *output;

  //only initialized for the root command of a tree
  char** readList;
  char** writeList;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};

//STRUCTURES FOR COMMAND_STREAM AND NODES AS MENTIONED IN CLASS :: EDIT 4/4/15 3:19PM
struct commandNode{
    struct command *command; //Store root of command tree
    struct commandNode *next;
};

//POINTER TO FIRST AND LAST COMMAND TREE HEAD
typedef struct command_stream{
    struct commandNode *head;
    struct commandNode *tail;
}  command_stream;
/*
   char **word = (char** word)malloc(sizeof(char*)*20);
   char* w = (char*)malloc(20*sizeof(char));
   w[0] = 'A';
   w[1] = 'u';
   w[2] = 's';
   w[3] = 't';

   word[0] = w;
*/
