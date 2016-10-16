#include "command.h"
#include "GraphNode.h"
typedef struct{
	GraphNode** dependencies;
	GraphNode** noDependencies;
	int numDependencies;
	int numNoDependencies;
} DependencyGraph;
