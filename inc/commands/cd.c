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
 * Run the `cd` command and update the working directory.
 * 
 * The `cd` command changes the current working directory to the specified path.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* lpTask The task that has to be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
bool CommandCd(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    BOOL bResultStatus = false;
    
    // There must be exactly one argument
    if (lpTask->argumentsCount != 1) goto INVALID_ARGUMENT_AND_RETURN;

    // Change directory
    if (context->functions.chdir(lpTask->arguments[0]) != 0) goto CANNOT_CHANGE_DIRECTORY_AND_RETURN;
    
    // Store success message
    DEFINE_STRING(lpSuccessMessage, "[directory changed]");
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

CANNOT_CHANGE_DIRECTORY_AND_RETURN:
    DEFINE_STRING(lpCannotChangeDir, "[cannot change directory]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpCannotChangeDir) + 1, sizeof(char));
    if (lpTask->result == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpTask->result, lpCannotChangeDir);
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