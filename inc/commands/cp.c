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
#include <windows.h>

/**
 * Booleans.
 * 
 * Defines boolean types.
 * https://pubs.opengroup.org/onlinepubs/007904975/basedefs/stdbool.h.html
 */
#include <stdbool.h>

/**
 * Resolve a relative path to an absolute path based on the current working directory.
 * 
 * @param char* relativePath The relative path to resolve.
 * @returns char* A buffer containing the absolute path, or NULL on failure.
 */
char* ResolveRelativePath(struct Relocatable* context, const char* relativePath) {
    char* absPath = NULL;
    DWORD dwBufferSize = MAX_PATH;

    absPath = context->functions.calloc(dwBufferSize, sizeof(char));
    if (absPath == NULL) return NULL;

    // Convert relative path to absolute path
    if (context->functions.GetFullPathNameA(relativePath, dwBufferSize, absPath, NULL) == 0) {
        context->functions.free(absPath);
        return NULL;
    }

    return absPath;
}

/**
 * Copy a file from source to destination.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* lpTask The task that has to be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
bool CommandCp(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    BOOL bResultStatus = false;
    HANDLE hSourceFile = INVALID_HANDLE_VALUE;
    HANDLE hDestFile = INVALID_HANDLE_VALUE;
    DWORD dwBytesRead = 0, dwBytesWritten = 0;
    char* lpBuffer = NULL;
    DWORD dwBufferSize = 1024;  // Buffer size for reading/writing
    char* lpResolvedSourcePath = NULL;
    char* lpResolvedDestPath = NULL;


    // There must be exactly two arguments: source file and destination path
    if (lpTask->argumentsCount != 2) goto INVALID_ARGUMENT_AND_RETURN;

    // Resolve the source path
    DWORD lpResolvedSourcePathLen = context->functions.strlen(lpTask->arguments[0]);
    lpResolvedSourcePath = (lpTask->arguments[0][0] == '\\' || (lpResolvedSourcePathLen >= 2 && lpTask->arguments[0][1] == ':')) ? lpTask->arguments[0] : ResolveRelativePath(context, lpTask->arguments[0]);
    if (lpResolvedSourcePath == NULL) goto CANNOT_RESOLVE_SOURCE_PATH_AND_RETURN;

    // Resolve the destination path
    DWORD lpResolvedDestPathLen = context->functions.strlen(lpTask->arguments[1]);
    lpResolvedDestPath = (lpTask->arguments[1][0] == '\\' || (lpResolvedDestPathLen >= 2 && lpTask->arguments[1][1] == ':')) ? lpTask->arguments[1] : ResolveRelativePath(context, lpTask->arguments[1]);
    if (lpResolvedDestPath == NULL) goto CANNOT_RESOLVE_DEST_PATH_AND_RETURN;

    // Open the source file for reading
    hSourceFile = context->functions.CreateFileA(lpResolvedSourcePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSourceFile == INVALID_HANDLE_VALUE) goto CANNOT_OPEN_SOURCE_FILE_AND_RETURN;

    // Create the destination file for writing
    hDestFile = context->functions.CreateFileA(lpResolvedDestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDestFile == INVALID_HANDLE_VALUE) goto CANNOT_CREATE_DEST_FILE_AND_RETURN;

    // Allocate buffer for file copying
    lpBuffer = context->functions.calloc(dwBufferSize, sizeof(char));
    if (lpBuffer == NULL) goto FAILURE_AND_RETURN;

    // Copy data from source to destination file
    while (context->functions.ReadFile(hSourceFile, lpBuffer, dwBufferSize, &dwBytesRead, NULL) && dwBytesRead > 0) {
        if (!context->functions.WriteFile(hDestFile, lpBuffer, dwBytesRead, &dwBytesWritten, NULL) || dwBytesWritten != dwBytesRead) {
            goto FAILURE_AND_RETURN;
        }
    }

    // Close both files
    context->functions.CloseHandle(hSourceFile);
    context->functions.CloseHandle(hDestFile);

    // Store success message
    DEFINE_STRING(lpSuccessMessage, "[file copied successfully]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpSuccessMessage) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpSuccessMessage);

    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

INVALID_ARGUMENT_AND_RETURN:
    DEFINE_STRING(lpInvalidArgument, "[invalid argument]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpInvalidArgument) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpInvalidArgument);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CANNOT_RESOLVE_SOURCE_PATH_AND_RETURN:
    DEFINE_STRING(lpCannotResolveSourcePath, "[cannot resolve source path]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotResolveSourcePath) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotResolveSourcePath);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CANNOT_RESOLVE_DEST_PATH_AND_RETURN:
    DEFINE_STRING(lpCannotResolveDestPath, "[cannot resolve destination path]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotResolveDestPath) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotResolveDestPath);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CANNOT_OPEN_SOURCE_FILE_AND_RETURN:
    DEFINE_STRING(lpCannotOpenSourceFile, "[cannot open source file]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotOpenSourceFile) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotOpenSourceFile);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CANNOT_CREATE_DEST_FILE_AND_RETURN:
    DEFINE_STRING(lpCannotCreateDestFile, "[cannot create destination file]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotCreateDestFile) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotCreateDestFile);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    if (hSourceFile != INVALID_HANDLE_VALUE) context->functions.CloseHandle(hSourceFile);
    if (hDestFile != INVALID_HANDLE_VALUE) context->functions.CloseHandle(hDestFile);
    if (lpBuffer != NULL) context->functions.free(lpBuffer);
    if (lpTask->result != NULL) context->functions.free(lpTask->result);
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    goto RETURN;

RETURN:
    return bResultStatus;
}
