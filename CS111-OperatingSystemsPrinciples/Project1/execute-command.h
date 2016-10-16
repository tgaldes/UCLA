#include "command.h"
#include "DependencyGraph.h"
DependencyGraph* createGraph(command_stream_t stream);
void execute(command_t command, bool time_travel);
int executeGraph(DependencyGraph* g);
