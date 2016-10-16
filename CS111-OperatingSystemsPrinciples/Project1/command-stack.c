#include <stdio.h>
#include <stdlib.h>
#include "command-internals.h"
typedef struct command Command;
int debugCommandStack = 0;
struct elementNode
{
	Command *command;
	struct elementNode *next;
	struct elementNode *prev;
	void (*elementNodeInit)();
};

typedef struct elementNode elementNode;
void printElementNodeLinkedList(elementNode *head)
{
	if(head == NULL)
		printf("The linked list is NULL\n");
	else
	{
		struct elementNode* temp = head;
		printf("The list begins:\n");
		while(temp != NULL)
		{
			if(temp->command == NULL)
				printf("The pointer has not been linked to a command\n");
			else
				printf("%s\n", temp->command->input);
			temp = temp->next;
		}
		printf("The list ends\n\n");
	}
}

void elementNodeInit(struct elementNode *self)
{
	self->next = NULL;
	self->prev = NULL;
	self->command = NULL;
}

struct stack
{
	elementNode *head;
	elementNode *(*pop)(struct stack *self);
	void (*push)(struct stack *self, elementNode* action);
	void (*init)(struct stack *self);
	void (*printStack)(struct stack *self);
	int size;
};
void init(struct stack *self)
{
	self->size = 0;
	self->head = NULL;
	if(debugCommandStack)
		printf("Initialization has been called\n");
}
void printStack(struct stack *self);
elementNode* pop(struct stack *self)
{
	if(debugCommandStack)	
		printf("Pop has been called\n");
	if(self->head == NULL)
	{
		return NULL;
	}
	elementNode *returnVal = self->head;
	self->head = self->head->next;
	self->size = self->size -1;
	if(self->size == 0)
		self->head = NULL;
	return returnVal;
}

void push(struct stack *self, elementNode* action)
{
	self->size = self->size + 1;
	if(self->head == NULL)
	{
		if(debugCommandStack)
			printf("Added a elementNode to an empty stack\n");
		action->next = NULL;
		action->prev = NULL;
		self->head = action;
	}
	else
	{
		if(debugCommandStack)
			printf("Added a elementNode to a non-empty stack\n");
		action->next = self->head;
		action->prev = NULL;
		self->head->prev = action;
		self->head = action;
	}
}


int testStack()
{
	typedef struct stack Stack;
	Stack *mine = malloc(sizeof(Stack));
	mine->init = &init;
	mine->pop = &pop;
	mine->push = &push;
	mine->init(mine);
	
	elementNode one;
	mine->push(mine, &one);
	elementNode two;
	mine->push(mine, &two);
	if(mine->size == 2)
		printf("Yay!!! two successful pushes");
	elementNode *twos = &two;
	if(twos == mine->pop(mine))
		printf("popping preserved the pointer!!\n");
	if(mine->size == 1)
		printf("keeping track of size correctly so far...\n");
	mine->pop(mine);
	printf("The size is %d\n", mine->size);
	if(mine->pop(mine) == NULL)
		printf("Correctly popped a null pointer\n");
	mine->pop(mine);
	
	printf("The size is %d\n", mine->size);
	
	
	
	mine->pop(mine);
	return 0;
}

void printStack(struct stack *self)
{
	if(self->head == NULL)
	{
		printf("The stack is empty.\n");
	}
	else
	{
		printf("TOP:\n");
		elementNode *cursor = self->head;
		while(cursor != NULL)
		{
			if(cursor->command != NULL)
			{
				switch(cursor->command->type)
				{
					case AND_COMMAND:
						printf("AND\n");	
						cursor = cursor->next;
						break;
					case OR_COMMAND:
						printf("OR\n");
						cursor = cursor->next;
						break;
					case SEQUENCE_COMMAND:
						printf("SEQUENCE\n");
						cursor = cursor->next;
						break;
					case PIPE_COMMAND:
						printf("PIPE\n");
						cursor = cursor->next;
						break;
					case SUBSHELL_COMMAND:
						printf("SUBSHELL\n");
						cursor = cursor->next;
						break;
					case SIMPLE_COMMAND:
						printf("SIMPLE\n");
						cursor = cursor->next;
						break;
					default:
						printf("UNINTIALIZED COMMAND TYPE\n");
						cursor=cursor->next;
						break;
				}
			}
		}
	printf("BOTTOM\n\n");
	}
}
