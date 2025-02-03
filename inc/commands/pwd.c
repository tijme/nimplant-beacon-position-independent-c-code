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
 * Run the pwd command and save the result.
 * 
 * The pwd command retrieves the current working directory.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* The task that has to be performed and in which the result can be saved.
 * @returns bool Positive if successfully executed.
 */
bool CommandPwd(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    BOOL bResultStatus = false;
    char* lpDirectoryBuffer = NULL;
    DWORD dwBufferSize = MAX_PATH;

    // Allocate memory for directory buffer
    lpDirectoryBuffer = context->functions.calloc(dwBufferSize, sizeof(char));
    if (lpDirectoryBuffer == NULL) goto FAILURE_AND_RETURN;

    // Get current working directory
    if (context->functions.GetCurrentDirectoryA(dwBufferSize, lpDirectoryBuffer) == 0) goto FAILURE_AND_RETURN;

    // Store result
    lpTask->result = lpDirectoryBuffer;
    bResultStatus = true;

    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    if (lpDirectoryBuffer != NULL) context->functions.free(lpDirectoryBuffer);
    if (lpTask->result != NULL) context->functions.free(lpTask->result);
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    goto RETURN;

RETURN:
    return bResultStatus;
}