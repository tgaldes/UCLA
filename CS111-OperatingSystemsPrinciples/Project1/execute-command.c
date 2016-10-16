// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DependencyGraph.h"
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
int debug0 = 0;
int debugDependencies = 0;
GraphNode* mostRecent;
void printDependencyList(command_t root);
void initGraphNode(GraphNode *self);
int checkForDependencies(GraphNode* current, GraphNode* old);
void initDependencyGraph(DependencyGraph* self);
void printDependencyGraph(DependencyGraph* self);
int executeGraph(DependencyGraph* g);
int executeNoDependencies(GraphNode** noDep, int num);
int executeDependencies(GraphNode** dep, int num);
int command_status (command_t c)
{
  return c->status;
}
int execute_simple(command_t c)
{
	pid_t child_pid;

	child_pid = fork();
	if(child_pid == 0)
	{
		//open file descriptors for input and output
		if(c->input)
		{
			if(debug0)
				printf("%s has input %s\n", c->u.word[0], c->input);

			freopen(c->input, "r", stdin);
		}
		if(c->output)
		{
			if(debug0)
				printf("%s has output %s\n", c->u.word[0], c->output);
			freopen(c->output, "w", stdout);
		}

		if(debug0)
		{
			printf("about to execvp simple command\n");
			printf("%s\n", c->u.word[0]);
		}
		execvp(c->u.word[0], c->u.word);
		//if execvp returns, it must have failed
		printf("Failed inside execute_simple function's child process\n");
		c->status = 0;

		//this is the error exit status
		_Exit(11);
	}
	else//this is run by the parent
	{
		int returnStatus;
		waitpid(child_pid, &returnStatus, 0);
		if(debug0)
			printf("Running parent of simple command\n");
		if(c->input)
		{
			if(debug0)
				printf("Closed stdin\n");
			fclose(stdin);
		}
		if(c->output)
			fclose(stdout);
		if(WIFEXITED(returnStatus) && WEXITSTATUS(returnStatus)==11)
		{
			printf("The child terminated with an error\n");
			c->status = 0;
		}
		else
		{
			if(debug0)
				printf("The child exited normally\n");
			c->status = 1;
		}
	}
	return 1;
}

void execute_pipe(command_t c, bool time_travel)
{
	int fd[2];
	pipe(fd);
	int firstPid = fork();
	if(firstPid == 0)//child
	{
		if(debug0)
			printf("First Child\n");
			close(fd[1]);
			dup2(fd[0], 0);
			execute_command(c->u.command[1], time_travel);
			if(c->u.command[1]->type != SIMPLE_COMMAND)
				execute_command(c->u.command[1], time_travel);
			else
			{
				execvp(c->u.command[1]->u.word[0], c->u.command[1]->u.word);
				_Exit(11);
			}
			if(debug0)
				printf("returned from executing right side of pipe\n");
			exit(0);
	}
	else
	{
		int secondPid = fork();
		if(secondPid == 0)
		{
			if(debug0)
				printf("Second child\n");
		close(fd[0]);
		dup2(fd[1], 1);
		if(c->u.command[0]->type != SIMPLE_COMMAND)
			execute_command(c->u.command[0], time_travel);
		else
		{
			execvp(c->u.command[0]->u.word[0], c->u.command[0]->u.word);
			_Exit(11);
		}
		if(debug0)
			printf("return from executing the left command of a pipe\n");
		exit(0);
		}
		else
		{//parent
			if(debug0)
				printf("Parent thread\n");
			close(fd[0]);
			close(fd[1]);
			int status;
			int returnedPid = waitpid(-1, &status, 0);
			if(returnedPid == secondPid)
			{
				if(debug0)
					printf("Waiting for first process\n");
				waitpid(firstPid, &status, 0);
			}
			if(returnedPid == firstPid)
			{
				if(debug0)
					printf("Waiting for second process\n");
				waitpid(secondPid, &status, 0);
			}
			c->status = 1;
		}
	}
}


void
execute_command (command_t c, bool time_travel)
{
	switch(c->type)
	{
		case AND_COMMAND:
			execute_command(c->u.command[0], time_travel);
			if(debug0)
				printf("Just returned from the left child of and command\n");
			//if previous command was successful, execute the right side as well
			if(c->u.command[0]->status == 1)
			{
				if(debug0)
					printf("Doing the right side of an and command\n");
				execute_command(c->u.command[1], time_travel);
				c->status = c->u.command[1]->status;
			}
			else//bad first command, don't need to do the second one
			{
				c->status = 0;
				if(debug0)
					printf("only executed left side of and command\n");
			}
			if(debug0)
				printf("The status of the and command at exit is: %i\n", c->status);

			break;
		case OR_COMMAND:
			execute_command(c->u.command[0], time_travel);

			//if previous command was successful,we are done 
			if(c->u.command[0]->status == 1)
			{
				c->status = 1;
			}
			else//bad first command, do the second and make that our status 
			{
				execute_command(c->u.command[1], time_travel);
				c->status = c->u.command[1]->status;
				if(debug0)
					printf("executed both sides an or command\n");
			}
			if(debug0)
				printf("Finished or command\n");
			break;
		case PIPE_COMMAND:
			execute_pipe(c, time_travel);
			break;
		case SEQUENCE_COMMAND:
			execute_command(c->u.command[0], time_travel);
			if(debug0)
				printf("just finished the left side of a sequence command\n");
			execute_command(c->u.command[1], time_travel);
			c->status = c->u.command[1]->status;
			if(debug0)
				printf("just finished the right side of a sequence command\n");
			break;
		case SIMPLE_COMMAND:
			if(debug0)
				printf("simple command\n");
			execute_simple(c);
			break;
		case SUBSHELL_COMMAND:
			execute_command(c->u.subshell_command, time_travel);
			c->status = c->u.subshell_command->status;
			break;
		case OPEN_COMMAND:
		case CLOSE_COMMAND:
			break;
	}
}


void addToDependencyList(command_t root, command_t current, int* readNum, int* writeNum)
{
	if(debugDependencies)
		printf("made call to add a dependency\n");

	switch(current->type)
	{
		case PIPE_COMMAND:
		case AND_COMMAND:
		case OR_COMMAND:
		case SEQUENCE_COMMAND:
			//make recursive calls to the left, then the right
			addToDependencyList(root, current->u.command[0], readNum, writeNum);
			addToDependencyList(root, current->u.command[1], readNum, writeNum);
			break;
		case SUBSHELL_COMMAND:
			//here we only need one recursive call
			addToDependencyList(root, current->u.subshell_command, readNum, writeNum);
			break;
		case SIMPLE_COMMAND:
		if(debugDependencies)
			printf("adding dependencies from %s\n", current->u.word[0]);
			//here we need to add every option and the input to readList
			//and every output to writeList
			if(current->input != NULL)
			{
				root->readList[*readNum] = current->input;
				(*readNum)++;
			}
			char* option = current->u.word[1];
			int loopCount = 1;
			while(option != NULL)
			{
				if(debugDependencies)
				{
					printf("the option we are adding is %s\n", option);
					printf("we are adding it to this spot in the list %d\n", *readNum);
				}
				root->readList[*readNum] = option;
				//make sure we terminate the list with a null pointer
				root->readList[(*readNum) + 1] = NULL;
				(*readNum)++;
				loopCount++;
				option = current->u.word[loopCount];
			}
			if(current->output != NULL)
			{
				root->writeList[*writeNum] = current->output;
				root->writeList[(*writeNum) + 1] = NULL;
				(*writeNum)++;
			}
			break;
		case OPEN_COMMAND:
		case CLOSE_COMMAND:
			break;
	}
}

void printDependencyList(command_t root)
{
	printf("Here is the read list:\n");
	int count = 0;
	while(root->readList[count] != NULL)
	{
		printf("%d: %s\n", count, root->readList[count]);
		count++;
	}
	printf("Here is the write list:\n");
	count = 0;
	while(root->writeList[count] != NULL)
	{
		printf("%d: %s\n", count,  root->writeList[count]);
		count++;
	}
}
DependencyGraph* createGraph(command_stream_t stream)
{
	mostRecent = NULL;
	//first we need to set up the read and write list for the root command
	//then we check its dependencies with all the previous trees
	//then we add it to the dependency graph

	//intialize the dependency graph
	DependencyGraph* myGraph = malloc(sizeof(DependencyGraph));
	initDependencyGraph(myGraph);
	//loop through every root command in the stream
	do
	{
		if(debugDependencies)
			printf("entered loop while creating the graph\n");
		if(stream->head == NULL)
			break;
		//first call has empty read and write lists
		int read = 0;
		int write = 0;
		//allocate memory for the lists
		stream->head->command->readList = (char**)malloc(sizeof(char*)*30);
		stream->head->command->writeList = (char **)malloc(sizeof(char*)*10);
		stream->head->command->writeList[0] = NULL;
		stream->head->command->readList[0] = NULL;
		addToDependencyList(stream->head->command,stream->head->command, &read, &write);
		//printDependencyList(stream->head->command);
		if(debugDependencies)
			printf("Creating a graph node for the most recent command tree\n");
		//now we need to create a graph node and fill it out for the command we just processed
		GraphNode* myNode = malloc(sizeof(GraphNode));
		//intialize it
		initGraphNode(myNode);
		//the next pointer points to the most recent Graph node we created, so if we follow 
		//these to the end of the list we can go through all the previously created nodes
		//to find all the dependencies
		myNode->command = stream->head->command;
		//set up next
		myNode->next = mostRecent;
		GraphNode* next = mostRecent;
		mostRecent = myNode;

		//now we'll loop through all the previous graph nodes and check each for dependencies with this one
		while(next != NULL)
		{
			if(checkForDependencies(myNode, next) == 1)
			{
				//we need to add next to the before array of myNode
				int currentDependencies = myNode->numDependencies;
				myNode->before[currentDependencies] = next;
				//increment the number of dependencies
				myNode->numDependencies = currentDependencies + 1;
			}
			next = next->next;
		}
		if(debugDependencies)
			printf("The total number of dependencies is: %d\n", myNode->numDependencies);

		//now we need to add our graph node to the dependency graph
		if(myNode->numDependencies ==0)
		{
			if(debugDependencies)
				printf("About to add a node with no dependencies\n");
			myGraph->noDependencies[myGraph->numNoDependencies] = myNode;
			myGraph->noDependencies[myGraph->numNoDependencies + 1] = NULL;
			myGraph->numNoDependencies = myGraph->numNoDependencies + 1;
		}
		else
		{
			if(debugDependencies)
				printf("About to add a node with dependency\n");
			myGraph->dependencies[myGraph->numDependencies] = myNode;
			myGraph->dependencies[myGraph->numDependencies + 1] = NULL;
			myGraph->numDependencies = myGraph->numDependencies + 1;
		}
		//and hopefully now we're done with using this node

		stream->head = stream->head->next;
	}while (stream->head != NULL);
	//printDependencyGraph(myGraph);
	return myGraph;
}

int checkForDependencies(GraphNode* current, GraphNode* old)
{
	//if current reads anything old writes, or writes anything old reads or writes
	//then we'll create a depency edge
	int dependency = 0;
	int moduleDebug = 0;
	if(moduleDebug)
		printf("checking for dependencies\n");
	//RAW check
	//loop through all the things current reads
	char** currentRead = current->command->readList;
	int i = 0;
	while(currentRead[i] != NULL && dependency == 0)
	{
		//now we loop through everything that old writes to
		char** oldWrite = old->command->writeList;
		int j = 0;
		while(oldWrite[j] != NULL)
		{
			if(strcmp(currentRead[i], oldWrite[j]) == 0)
			{
				if(moduleDebug)
					printf("We found a RAW dependency\n");
				dependency = 1;
				break;
			}
			j++;
		}
		i++;
	}


	//WAW check
	//loop through all the things current writes 
	char** currentWrite = current->command->writeList;
	i = 0;
	while(currentWrite[i] != NULL && dependency == 0)
	{
		//now we loop through everything that old writes to
		char** oldWrite = old->command->writeList;
		int j = 0;
		while(oldWrite[j] != NULL)
		{
			if(strcmp(currentWrite[i], oldWrite[j]) == 0)
			{
				if(moduleDebug)
					printf("We found a WAW dependency\n");
				dependency = 1;
				break;
			}
			j++;
		}
		i++;
	}


	//WAR check
	//loop through all the things current writes 
	i = 0;
	while(currentWrite[i] != NULL && dependency == 0)
	{
		//now we loop through everything that old writes to
		char** oldRead = old->command->readList;
		int j = 0;
		while(oldRead[j] != NULL)
		{
			if(strcmp(currentWrite[i], oldRead[j]) == 0)
			{
				if(moduleDebug)
					printf("We found a WAR dependency\n");
				dependency = 1;
				break;
			}
			j++;
		}
		i++;
	}


	return dependency;
}

int executeGraph(DependencyGraph* g)
{
	int returnVal = executeNoDependencies(g->noDependencies, g->numNoDependencies);
	executeDependencies(g->dependencies, g->numDependencies);
	return returnVal;
}

int executeNoDependencies(GraphNode** noDep, int num)
{
	int moduleDebug = 0;
	if(moduleDebug)
		printf("Executing non dependent trees\n");
	int i = 0;
	while(i < num)
	{
		GraphNode* current = noDep[i];
		pid_t pid = fork();
		if(pid == 0)
		{
			//child process executes tree
			execute_command(current->command, true);
			_exit(0);
		}
		else
		{
			//parent thread
			current->pid = pid;
			if(moduleDebug)
				printf("The pid of this is: %d\n", current->pid);
		}
		i++;
	}
	return 1;
}
int executeDependencies(GraphNode** dep, int num)
{
	//here we need to wait on every graphnode in the before list
	int moduleDebug = 0;
	if(moduleDebug)
		printf("Executing dependent trees\n");
	int i = 0;
	while(i < num)
	{
		GraphNode* current = dep[i];
		//loop through everything in the before list
		int j = 0;
		while(j < current->numDependencies)
		{
			GraphNode* execBefore = current->before[j];

			//the node hasn't beed executed yet, so we need to spawn a thread 
			//and wait on this pid
			if(execBefore->pid == -1)
			{
				pid_t pid = fork();
				if(pid == 0)
				{
					execute_command(execBefore->command, true);
					_exit(0);
				}
				else
				{
					execBefore->pid = pid;
					if(moduleDebug)
						printf("The pid of this is: %d\n", execBefore->pid);
				}
			}
			
			//now we wait for every before node to finish
			int status;
			waitpid(execBefore->pid, &status, 0);
			j++;
		}
		pid_t pid = fork();
		if(pid == 0)
		{
			//child process executes tree
			execute_command(current->command, true);
			_exit(0);
		}
		else
		{
			//parent thread
			current->pid = pid;
			if(moduleDebug)
				printf("The pid of this is: %d\n", current->pid);
		}
		i++;
	}
	return 1;
}

void execute(command_t command, bool time_travel)
{
	execute_command(command, time_travel);
}
void initGraphNode(GraphNode *self)
{
	self->before = malloc(sizeof(GraphNode*)*20);
	self->numDependencies = 0;
	self->next = NULL;
	self->pid = -1;
}

void initDependencyGraph(DependencyGraph* self)
{
	self->numDependencies = 0;
	self->numNoDependencies = 0;

	self->dependencies = malloc(sizeof(GraphNode*) * 20);
	self->noDependencies = malloc(sizeof(GraphNode*) * 20);

	self->dependencies[0] = NULL;
	self->noDependencies[0] = NULL;
}

void printDependencyGraph(DependencyGraph* self)
{
	printf("The nodes with no dependency are:\n");
	GraphNode* current = self->noDependencies[0];
	int count = 0;
	while(current != NULL)
	{
		printf("%d\n", count);
		count++;
		current = self->noDependencies[count];
	}
	count = 0;
	current = self->dependencies[0];
	printf("The nodes with dependencies are:\n");
	while(current != NULL)
	{
		printf("%d\n", count);
		count++;
		current = self->dependencies[count];
	}
}
