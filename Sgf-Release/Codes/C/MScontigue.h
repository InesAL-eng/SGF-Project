#ifndef MSCONTIGUE_H
#define MSCONTIGUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Struct.h"
#include "MScontigue.c"
// Function to write data to the vdisk based on organization and file structure
void write_data_genericMS(const char *vdisk_name, const char *file_name, Student *data, size_t record_size, int record_count);

// Function to append data contiguously to the virtual disk
void ContigueAdd(char *file_name, Student *students, int count);

#endif // MSCONTIGUE_H
