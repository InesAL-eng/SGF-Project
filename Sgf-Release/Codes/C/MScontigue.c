#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Struct.h"

void write_data_genericMS(const char *vdisk_name, const char *file_name, Student *data, size_t record_size, int record_count) {
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
    int start_block = -1;

    // Find contiguous free blocks
    for (int i = 0; i <= num_blocks - blocks_required; i++) {
        int j;
        for (j = 0; j < blocks_required; j++) {
            if (block_status[i + j] != 0) break;
        }
        if (j == blocks_required) { // Found enough contiguous blocks
            start_block = i;
            break;
        }
    }

    if (start_block == -1) {
        printf("Not enough contiguous free blocks available on the vdisk.\n");
        free(block_status);
        free(file_table);
        fclose(vdisk_file);
        return;
    }

    int record_index = 0;

    // Write data into the contiguous blocks
    for (int i = 0; i < blocks_required; i++) {
        int records_in_block = 0;
        Student block[block_factor];
        memset(block, 0, block_factor * sizeof(Student));

        while (records_in_block < block_factor && record_index < record_count) {
            block[records_in_block] = data[record_index];
            record_index++;
            records_in_block++;
        }

        fseek(vdisk_file, (start_block + i) * block_factor * sizeof(Student) +
                              sizeof(FileData) +
                              num_blocks * sizeof(unsigned char) +
                              num_blocks * sizeof(FileMetaData), SEEK_SET);
        fwrite(block, block_factor * sizeof(Student), 1, vdisk_file);

        block_status[start_block + i] = 1;
    }

    FileMetaData file_meta;
    strncpy(file_meta.file_name, file_name, MAXFileName - 1);
    file_meta.file_name[MAXFileName - 1] = '\0';
    file_meta.first_block = start_block;
    file_meta.num_blocks = blocks_required;
    file_meta.block_count = record_count;
    file_meta.org_mode_intern = 0;
    file_meta.org_mode_extern = 0;

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
        fclose(vdisk_file);
        return;
    }

    fseek(vdisk_file, sizeof(FileData) + num_blocks * sizeof(unsigned char) + file_index * sizeof(FileMetaData), SEEK_SET);
    fwrite(&file_meta, sizeof(FileMetaData), 1, vdisk_file);

    fseek(vdisk_file, sizeof(FileData), SEEK_SET);
    fwrite(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    free(block_status);
    free(file_table);
    fclose(vdisk_file);

    printf("File '%s' written successfully, spanning %d contiguous blocks starting at block %d.\n", file_name, blocks_required, start_block);

}

void ContigueAdd(char *file_name, Student *students, int count) {
    const char *vdisk_name = "vdisk.bin";
    write_data_genericMS(vdisk_name, file_name, students, sizeof(Student), count);
}
