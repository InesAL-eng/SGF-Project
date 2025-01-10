#ifndef STRUCT_H
#define STRUCT_H

#define MAXFileName 15

// Structure to represent a file in the menu files table
typedef struct {
    int n_blocks;      // Number of blocks
    int f_blockage;    // Blockage factor
} FileData;

typedef struct {
    char file_name[MAXFileName];  // File name (max length 14, plus null terminator)
    int first_block;              // First block of the file
    int num_blocks;               // Number of blocks per file
    int block_count;              // Number of records in the file
    int org_mode_intern;          // Internal organization mode
    int org_mode_extern;          // External organization mode
} FileMetaData;

typedef struct {
    long id;
    char full_name[40];
    char mat[20];
    char section;
    int nextBlock;
} Student;

#endif // STRUCT_H
