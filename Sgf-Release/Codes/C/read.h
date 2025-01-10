#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.c"
#include "Struct.h"
// Function prototype for reading and printing the organization part of vdisk
void read_org_part(const char *filename, FileData *out0, unsigned char *out1, FileMetaData *out2, Student *out3);

#endif // READ_H
