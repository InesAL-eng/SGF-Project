
#ifndef INITVDISK_H
#define INITVDISK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Struct.h"
#include "initVdisk.c"
// Structures from your "Struct.h"

                                                                // Function declarations
                                                                void create_vdisk();               // Function to create the vdisk and organization part
int check_vdisk(const char *filename);  // Function to check if vdisk exists
void init();                       // Function to initialize vdisk if not present

#endif // INITVDISK_H
