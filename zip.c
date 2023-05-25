#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unzip.h>
#include <sys/stat.h>
#include <windows.h>
#include <libxml/parser.h>

int create_directory_for_file(const char* filepath)
{
    int result = 0;
    char* path = NULL;
    char* slash_pos = strrchr(filepath, '/');
    if (slash_pos != NULL)
    {
        int path_len = slash_pos - filepath + 1;
        path = (char*)malloc(path_len + 1);
        if (path != NULL)
        {
            strncpy(path, filepath, path_len);
            path[path_len] = '\0';
            while (path_len > 1 && path[path_len - 2] == '/')
            {
                path[path_len - 2] = '\0';
                path_len--;
            }
            if (CreateDirectoryA(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
            {
                result = -1;
            }
        }
        free(path);
    }
    return result;
}

int main() {

    unzFile zip_file;
    unz_file_info zip_info;
    char filename[256];
    int ret;
    int flag = 0;

    // open zip file
    zip_file = unzOpen("example.docx");
    if (zip_file == NULL) {
        printf("failed to open archive.zip\n");
        exit(1);
    }

    // create directory for extracted files
    CreateDirectoryA("example", NULL);

    // read zip file contents
    ret = unzGoToFirstFile(zip_file);
    while (ret == UNZ_OK) {
        // get file information
        ret = unzGetCurrentFileInfo(zip_file, &zip_info, filename, sizeof(filename), NULL, 0, NULL, 0);
        if (ret != UNZ_OK) {
            printf("failed to get file info\n");
            exit(1);
        }

        // extract file
        ret = unzOpenCurrentFile(zip_file);
        if (ret != UNZ_OK) {
            printf("failed to open file\n");
            exit(1);
        }

        printf("[+]%s\n", filename);

        // _rels/.rels
        flag = create_directory_for_file(filename);
             
   
        FILE* out_file = fopen(filename, "wb");
        if (out_file == NULL) {
            printf("failed to create file %s\n",filename);
            exit(1);
        }

        char buffer[4096];
        int read_len;
        do {
            read_len = unzReadCurrentFile(zip_file, buffer, sizeof(buffer));
            if (read_len < 0) {
                printf("failed to read file\n");
                exit(1);
            }
            if (read_len > 0) {
                fwrite(buffer, read_len, 1, out_file);
            }
        } while (read_len > 0);

        fclose(out_file);
        unzCloseCurrentFile(zip_file);

        // go to next file
        ret = unzGoToNextFile(zip_file);
    }

    // close zip file
    unzClose(zip_file);
}