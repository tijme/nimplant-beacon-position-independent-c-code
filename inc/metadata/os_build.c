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
 * Retrieve the OS build number.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @returns char* The OS build number string (must be freed by the caller).
 */
char* GetOSBuildNumber(struct Relocatable* context) {
    char* lpBuildNumber = NULL;
    OSVERSIONINFOW osVersionInfo;

    /** Retrieve the OS version. */
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    if (context->functions.RtlGetVersion((PRTL_OSVERSIONINFOW) &osVersionInfo) != 0) {
        goto FALLBACK_AND_RETURN;
    }

    /** Allocate memory for the build number string. */
    lpBuildNumber = (char*) context->functions.calloc(32 + 1, sizeof(char));
    if (lpBuildNumber == NULL) goto FALLBACK_AND_RETURN;

    // /** Convert the build number to a string. */
    DEFINE_STRING(lpFormatter, "%lu");
    context->functions.snprintf(lpBuildNumber, 32, lpFormatter, osVersionInfo.dwBuildNumber);
    goto RETURN;

FALLBACK_AND_RETURN:
    DEFINE_STRING(sUnknown, "[UNKNOWN]");

    lpBuildNumber = (char*)context->functions.calloc(sizeof(sUnknown) + 1, sizeof(char));
    if (lpBuildNumber == NULL) goto RETURN;

    context->functions.strcpy(lpBuildNumber, sUnknown);

RETURN:
    return lpBuildNumber;
}
