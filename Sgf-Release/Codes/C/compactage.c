#include<Struct.h>
#include <stdlib.h>
#include <stdio.h>

void compact_disk(const char *vdisk_name) {
    FILE *vdisk_file = fopen(vdisk_name, "r+b");
    if (vdisk_file == NULL) {
        perror("Error opening the virtual disk");
        return;
    }

    // Step 1: Read global disk data
    FileData myfdata;
    fread(&myfdata, sizeof(FileData), 1, vdisk_file);

    int num_blocks = myfdata.n_blocks;      // Total number of blocks
    int block_factor = myfdata.f_blockage; // Records per block

    // Read the block status table
    unsigned char *block_status = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));
    if (block_status == NULL) {
        perror("Memory allocation failed for block status table");
        fclose(vdisk_file);
        return;
    }
    fread(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    // Read the metadata table
    FileMetaData *file_table = (FileMetaData *)malloc(num_blocks * sizeof(FileMetaData));
    if (file_table == NULL) {
        perror("Memory allocation failed for file table");
        free(block_status);
        fclose(vdisk_file);
        return;
    }
    fread(file_table, sizeof(FileMetaData), num_blocks, vdisk_file);

    // Step 2: Begin compaction
    int next_free_block = 0; // Start looking from the zeroth block
    for (int i = 0; i < num_blocks; i++) {
        FileMetaData *current_file = &file_table[i];
        if (current_file->first_block == -1 || current_file->org_mode_intern != 0) {
            continue; // Skip invalid or non-contiguous files
        }

        int file_start = current_file->first_block;
        int file_size = current_file->num_blocks;

        // Find the next available contiguous space before the current file
        while (next_free_block < file_start) {
            int gap_size = 0;

            // Check for a sufficient gap
            for (int j = next_free_block; j < num_blocks && block_status[j] == 0; j++) {
                gap_size++;
                if (gap_size >= file_size) {
                    break;
                }
            }

            // If gap is large enough, shift the file
            if (gap_size >= file_size) {
                // Move file blocks to the new location
                for (int j = 0; j < file_size; j++) {
                    // Read block data from the current position
                    Student block[block_factor];
                    fseek(vdisk_file,
                          (file_start + j) * block_factor * sizeof(Student) +
                              sizeof(FileData) + num_blocks * sizeof(unsigned char) + num_blocks * sizeof(FileMetaData),
                          SEEK_SET);
                    fread(block, sizeof(Student), block_factor, vdisk_file);

                    // Write block data to the new position
                    fseek(vdisk_file,
                          (next_free_block + j) * block_factor * sizeof(Student) +
                              sizeof(FileData) + num_blocks * sizeof(unsigned char) + num_blocks * sizeof(FileMetaData),
                          SEEK_SET);
                    fwrite(block, sizeof(Student), block_factor, vdisk_file);
                }

                // Update the block status table
                for (int j = 0; j < file_size; j++) {
                    block_status[file_start + j] = 0;            // Mark old blocks as free
                    block_status[next_free_block + j] = 1;      // Mark new blocks as occupied
                }

                // Update the metadata for the file
                current_file->first_block = next_free_block;

                // Move to the next file
                next_free_block += file_size;
                break;
            } else {
                // No sufficient gap; continue looking
                next_free_block += gap_size + 1;
            }
        }
    }

    // Step 3: Rewrite metadata and block status tables to the disk
    fseek(vdisk_file, sizeof(FileData), SEEK_SET);
    fwrite(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    fseek(vdisk_file, sizeof(FileData) + num_blocks * sizeof(unsigned char), SEEK_SET);
    fwrite(file_table, sizeof(FileMetaData), num_blocks, vdisk_file);

    // Cleanup
    free(block_status);
    free(file_table);
    fclose(vdisk_file);

    printf("Disk compacted successfully.\n");
}
