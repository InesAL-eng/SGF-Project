#ifndef SUPPRESSION_H
#define SUPPRESSION_H
#include "Struct.h"
#include "suppression.c"
void display_files(FILE *vdisk_file, int max_files, FileMetaData *file_table);

// Function to delete a file
void delete_file(const char *filename, int first_block);

#endif // SUPPRESSION_H
