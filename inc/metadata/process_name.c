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
 * Strings.
 * 
 * Defines one variable type, one macro, and various functions for manipulating arrays of characters.
 * https://www.tutorialspoint.com/c_standard_library/string_h.htm
 */
#include <string.h>

/**
 * Retrieve the current process name.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @returns char* The process name string (must be freed by the caller).
 */
char* GetCurrentProcessName(struct Relocatable* context) {
    char* lpProcessName = NULL;
    char lpPath[MAX_PATH];

    if (context->functions.GetModuleFileNameA(NULL, lpPath, MAX_PATH) == 0) {
        goto FALLBACK_AND_RETURN;
    }

    char* lpFileName = context->functions.strrchr(lpPath, '\\');
    if (lpFileName != NULL) {
        lpFileName++;
    } else {
        lpFileName = lpPath;
    }

    lpProcessName = (char*) context->functions.calloc(context->functions.strlen(lpFileName) + 1, sizeof(char));
    if (lpProcessName == NULL) goto FALLBACK_AND_RETURN;

    context->functions.strcpy(lpProcessName, lpFileName);
    goto RETURN;

FALLBACK_AND_RETURN:
    DEFINE_STRING(sUnknown, "[UNKNOWN]");
    lpProcessName = (char*)context->functions.calloc(context->functions.strlen(sUnknown) + 1, sizeof(char));
    if (lpProcessName == NULL) goto RETURN;

    context->functions.strcpy(lpProcessName, sUnknown);

RETURN:
    return lpProcessName;
}
