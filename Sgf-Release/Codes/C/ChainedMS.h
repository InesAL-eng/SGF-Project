#ifndef CHAINEDMS_H
#define CHAINEDMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ChainedMS.c"
#include "Struct.h"
// Function to write data to the vdisk based on organization and file structure
void write_data_generic(const char *vdisk_name, const char *file_name, Student *data, size_t record_size, int record_count);

// Function to append data in a chained (linked list) manner
void ChainedAdd(char *file_name, Student *students, int count);


#endif // CHAINEDMS_H
