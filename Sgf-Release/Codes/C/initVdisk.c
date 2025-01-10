#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Struct.h"

// Function to create the vdisk and organisation part
void create_vdisk(int num_blocks,int block_factor) {
    int max_files, i;
    FileData myfdata;
    FILE *vdisk_file;
    // In worst case, each file contains 1 block, so max_files = num_blocks
    max_files = num_blocks;  // 1 file per block
    myfdata.n_blocks=num_blocks;
    myfdata.f_blockage=block_factor;
    // Create and open the binary file
    vdisk_file = fopen("vdisk.bin", "wb");
    if (vdisk_file == NULL) {
        perror("Error opening the vdisk file");
        return;
    }

    //Write the Binary Table (Organisation Part)
    unsigned char *block_status = (unsigned char *)calloc(num_blocks, sizeof(unsigned char));
    if (block_status == NULL) {
        perror("Memory allocation failed for block status table");
        fclose(vdisk_file);
        return;
    }

    // Fill the binary table: All blocks are initially empty
    for (i = 0; i < num_blocks; i++) {
        // 0 means the block is empty
        block_status[i] = 0;
    }

    // Write table to vdisk
    fwrite(&myfdata,sizeof(myfdata),1,vdisk_file);
    fwrite(block_status, sizeof(unsigned char), num_blocks, vdisk_file);
    free(block_status);

    //Write the Menu Files Table (Metadata for Files)
    FileMetaData *file_table = (FileMetaData *)malloc(max_files * sizeof(FileMetaData));
    if (file_table == NULL) {
        perror("Memory allocation failed for file table");
        fclose(vdisk_file);
        return;
    }

    // Initially, all file metadata will be empty (0 values)
    for (i = 0; i < max_files; i++) {
        strncpy(file_table[i].file_name, "Unnamed", MAXFileName - 1);
        file_table[i].first_block = -1;
        file_table[i].num_blocks = 0;
        file_table[i].block_count = 0;
        file_table[i].org_mode_extern = 0;
        file_table[i].org_mode_intern = 0;
    }

    //Write the file metadata (menu table) to the file
    fwrite(file_table, sizeof(FileMetaData), max_files, vdisk_file);
    free(file_table);
    // Close the binary file
    fclose(vdisk_file);
}

int check_vdisk(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fclose(file);
        return 1;
    } else {
        return 0;
    }
}
void init(int nb,int nf){
    if(!check_vdisk("vdisk.bin")){
        create_vdisk(nb,nf);
    }
}
