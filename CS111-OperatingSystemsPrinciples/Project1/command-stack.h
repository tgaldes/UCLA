struct elementNode{
    struct command *command;
    struct elementNode *next;
    struct elementNode *prev;
    int numWords;
	void (*elementNodeInit)();
};

struct stack
{
	struct elementNode *head;
//	struct elementNode *(*pop)(struct strack *self);
	struct elementNode *(*pop)();
//	void (*push)(struct stack *self, struct elementNode* action);
	void (*push)();
//	void (*init)(struct stack *self);
	void (*init)();
//  void (*printStack)(struct stack *self);
	void (*printStack)();
	int size;
};

void init(struct stack *self);
struct elementNode* pop(struct stack *self);
void push (struct stack *self, struct elementNode* action);
int testStack();
void printStack(struct stack *self);
void printElementNodeLinkedList(struct elementNode *head);          
void elementNodeInit(struct elementNode *self);
