#include <stdlib.h>
#include <stdio.h>

#include "exec.h"

int exec(char *command)
{
    // TODO: Make this safer to call
    system(command);
}