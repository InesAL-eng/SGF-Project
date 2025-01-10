#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Struct.h"
// Function to read and print the organization part of vdisk
void read_org_part(const char *filename,FileData *out0,unsigned char *out1,FileMetaData *out2,Student *out3) {
    int num_blocks;
    int block_factor;
    FILE *vdisk_file;
    FileData myfdata;
    unsigned char *block_status;
    FileMetaData *file_table;
    int i, max_files;

    // Open the vdisk binary file
    vdisk_file = fopen(filename, "rb");
    if (vdisk_file == NULL) {
        perror("Error opening the vdisk file");
        return;
    }
    fread(&myfdata,sizeof(myfdata),1,vdisk_file);

    out0=(FileData *)malloc(sizeof(FileData));
    out0->f_blockage=myfdata.f_blockage;
    out0->n_blocks=myfdata.n_blocks;

    num_blocks=myfdata.n_blocks;
    block_factor=myfdata.f_blockage;
    // Read the Binary Table (Organization Part): block status table
    block_status = (unsigned char *)malloc(num_blocks * sizeof(unsigned char)); // Allocate memory for block status table

    if (block_status == NULL) {
        perror("Memory allocation failed for block status table");
        fclose(vdisk_file);
        return;
    }

    // Read the block status table (first part of the vdisk)

    fread(block_status, sizeof(unsigned char), num_blocks , vdisk_file);
    out1=(unsigned char *)malloc(sizeof(unsigned char)*num_blocks);
    for (i = 0; i < num_blocks; i++) {

        *(out1+i)=block_status[i];
    }
    file_table=(FileMetaData *)malloc(num_blocks*sizeof(FileMetaData));
    fread(file_table,sizeof(FileMetaData),num_blocks,vdisk_file);
    out2=(FileMetaData *)malloc(num_blocks*sizeof(FileMetaData));
    // Print the menu table
    for (i = 0; i < num_blocks; i++) {

        strcpy((*(out2+i)).file_name,(*(file_table+i)).file_name) ;
        (*(out2+i)).first_block    = (*(file_table+i)).first_block    ;
        (*(out2+i)).num_blocks     = (*(file_table+i)).num_blocks     ;
        (*(out2+i)).block_count    = (*(file_table+i)).block_count    ;
        (*(out2+i)).org_mode_intern= (*(file_table+i)).org_mode_intern;
        (*(out2+i)).org_mode_extern= (*(file_table+i)).org_mode_extern;
    }

}
