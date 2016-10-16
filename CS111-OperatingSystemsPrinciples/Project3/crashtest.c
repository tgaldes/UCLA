#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ospfs.h"
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char** argv)
{
    int inputCrash = atoi(argv[1]);
    //create new file desciptor to make the sys call
    int fd = open("./test/world.txt", "r");
    void* new = NULL;
    int exitStatus = ioctl(fd, OSPFS_SET_NWRITES_TO_CRASH, inputCrash);
    close(fd);
    exit(exitStatus);
}
    
