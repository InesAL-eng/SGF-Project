#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Struct.h"

void write_data_generic(const char *vdisk_name, const char *file_name, Student *data, size_t record_size, int record_count) {
    FILE *vdisk_file = fopen(vdisk_name, "r+b");
    if (vdisk_file == NULL) {
        perror("Error opening the vdisk");
        return;
    }

    FileData myfdata;
    fread(&myfdata, sizeof(FileData), 1, vdisk_file);

    int num_blocks = myfdata.n_blocks;
    int block_factor = myfdata.f_blockage;

    unsigned char *block_status = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));
    if (block_status == NULL) {
        perror("Memory allocation failed for block status table");
        fclose(vdisk_file);
        return;
    }

    fread(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    FileMetaData *file_table = (FileMetaData *)malloc(num_blocks * sizeof(FileMetaData));
    if (file_table == NULL) {
        perror("Memory allocation failed for file table");
        free(block_status);
        fclose(vdisk_file);
        return;
    }

    fread(file_table, sizeof(FileMetaData), num_blocks, vdisk_file);

    int blocks_required = ceil((float)record_count / block_factor);
    int *free_blocks = (int *)malloc(blocks_required * sizeof(int));
    if (free_blocks == NULL) {
        perror("Memory allocation failed for free block list");
        free(block_status);
        free(file_table);
        fclose(vdisk_file);
        return;
    }

    int found_blocks = 0;

    // Find free blocks (scattered)
    for (int i = 0; i < num_blocks; i++) {
        if (block_status[i] == 0) {
            free_blocks[found_blocks++] = i;
            if (found_blocks >= blocks_required) {
                break;
            }
        }
    }

    // Check if we have enough blocks
    if (found_blocks < blocks_required) {
        printf("Not enough free blocks available on the vdisk.\n");
        free(block_status);
        free(file_table);
        free(free_blocks);
        fclose(vdisk_file);
        return;
    }

    int record_index = 0;

    // Write data into the found blocks
    for (int i = 0; i < blocks_required; i++) {
        int records_in_block = 0;
        Student block[block_factor];
        memset(block, 0, block_factor * sizeof(Student));

        // Fill the current block with data
        while (records_in_block < block_factor && record_index < record_count) {
            block[records_in_block] = data[record_index];
            block[records_in_block].nextBlock = (i < blocks_required - 1) ? free_blocks[i + 1] : -1; // Link to the next block
            record_index++;
            records_in_block++;
        }

        // Seek to the block position and write data
        fseek(vdisk_file, free_blocks[i] * block_factor * sizeof(Student) +
                              sizeof(FileData) +
                              num_blocks * sizeof(unsigned char) +
                              num_blocks * sizeof(FileMetaData), SEEK_SET);
        fwrite(block, block_factor * sizeof(Student), 1, vdisk_file);

        block_status[free_blocks[i]] = 1; // Mark block as used
    }

    // Create metadata for the new file
    FileMetaData file_meta;
    strncpy(file_meta.file_name, file_name, MAXFileName - 1);
    file_meta.file_name[MAXFileName - 1] = '\0';
    file_meta.first_block = free_blocks[0];
    file_meta.num_blocks = blocks_required;
    file_meta.block_count = record_count;
    file_meta.org_mode_intern = 1; // Chained allocation mode
    file_meta.org_mode_extern = 0;

    // Find a free slot in the file metadata table
    int file_index = -1;
    for (int i = 0; i < num_blocks; i++) {
        if (file_table[i].first_block == -1) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("No available space for new files in the file metadata table.\n");
        free(block_status);
        free(file_table);
        free(free_blocks);
        fclose(vdisk_file);
        return;
    }

    // Write the file metadata to the file table
    fseek(vdisk_file, sizeof(FileData) + num_blocks * sizeof(unsigned char) + file_index * sizeof(FileMetaData), SEEK_SET);
    fwrite(&file_meta, sizeof(FileMetaData), 1, vdisk_file);

    // Update the block status table
    fseek(vdisk_file, sizeof(FileData), SEEK_SET);
    fwrite(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    // Free allocated memory
    free(block_status);
    free(file_table);
    free(free_blocks);
    fclose(vdisk_file);

    printf("File '%s' written successfully, spanning %d blocks starting at block %d.\n", file_name, blocks_required, free_blocks[0]);
}

void ChainedAdd(char *file_name, Student *students, int count) {
    const char *vdisk_name = "vdisk.bin";
    write_data_generic(vdisk_name, file_name, students, sizeof(Student), count);
}
