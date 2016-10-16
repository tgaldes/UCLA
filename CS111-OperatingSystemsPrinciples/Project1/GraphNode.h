#include "command.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
typedef struct{
	command_t command;
	struct GraphNode** before;
	struct GraphNode* next;
	pid_t pid;
	int numDependencies;
} GraphNode;

