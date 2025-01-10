#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Struct.h"
#define MAXFileName 15

// Fonction pour afficher les fichiers existants
// Had fonction t'affichi list des fichiers li kaynin
void display_files(FILE *vdisk_file, int max_files, FileMetaData *file_table) {
    printf("\nFichiers disponibles :\n");
    for (int i = 0; i < max_files; i++) {
        if (file_table[i].first_block != -1) {  // Valid file
            printf("Index: %d, Nom: %s, Premier bloc: %d, Nombre de blocs: %d\n",
                   i, file_table[i].file_name, file_table[i].first_block, file_table[i].num_blocks);
        }
    }
}

// Function to delete a file by first block
void delete_file(const char *filename, int first_block) {
    FILE *vdisk_file = fopen(filename, "rb+");
    if (!vdisk_file) {
        perror("Erreur lors de l'ouverture du fichier vdisk");
        return;
    }

    FileData file_data;
    fread(&file_data, sizeof(FileData), 1, vdisk_file);

    int num_blocks = file_data.n_blocks;
    int max_files = num_blocks;  // Max possible files is one per block

    unsigned char *block_status = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));
    fread(block_status, sizeof(unsigned char), num_blocks, vdisk_file);

    FileMetaData *file_table = (FileMetaData *)malloc(max_files * sizeof(FileMetaData));
    fread(file_table, sizeof(FileMetaData), max_files, vdisk_file);

    display_files(vdisk_file, max_files, file_table);

    int file_index = -1;

    // Find file by first_block
    for (int i = 0; i < max_files; i++) {
        if (file_table[i].first_block == first_block) {
            file_index = i;
            break;
        }
    }

    if (file_index < 0 || file_table[file_index].first_block == -1) {
        printf("Fichier non trouvé.\n");
        free(block_status);
        free(file_table);
        fclose(vdisk_file);
        return;
    }

    FileMetaData *file_to_delete = &file_table[file_index];
    int num_blocks_to_delete = file_to_delete->num_blocks;

    // Clear the blocks by marking them as free and writing zeroes over the block data
    for (int i = 0; i < num_blocks_to_delete; i++) {
        int block_index = file_to_delete->first_block + i;
        if (block_index >= 0 && block_index < num_blocks) {
            block_status[block_index] = 0;  // Mark block as free

            // Seek to the block's data location and write zeroes
            fseek(vdisk_file, sizeof(FileData) + num_blocks + max_files + block_index * file_data.f_blockage, SEEK_SET);
            unsigned char *empty_block = (unsigned char *)calloc(file_data.f_blockage, sizeof(unsigned char));
            fwrite(empty_block, sizeof(unsigned char), file_data.f_blockage, vdisk_file);
            free(empty_block);
        }
    }

    // Clear the file metadata entry
    strncpy(file_to_delete->file_name, "Unnamed", MAXFileName - 1);
    file_to_delete->first_block = -1;
    file_to_delete->num_blocks = 0;
    file_to_delete->block_count = 0;

    // Write updated metadata and block status
    fseek(vdisk_file, sizeof(FileData), SEEK_SET);
    fwrite(block_status, sizeof(unsigned char), num_blocks, vdisk_file);
    fwrite(file_table, sizeof(FileMetaData), max_files, vdisk_file);

    printf("\nLe fichier a été supprimé avec succès.\n");

    free(block_status);
    free(file_table);
    fclose(vdisk_file);
}
