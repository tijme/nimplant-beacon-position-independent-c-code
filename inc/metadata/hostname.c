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
 * Windows API.
 * 
 * Contains declarations for all of the functions, macro's & data types in the Windows API.
 * https://docs.microsoft.com/en-us/previous-versions//aa383749(v=vs.85)?redirectedfrom=MSDN
 */
#include <windows.h>

/**
 * LAN Manager Constants.
 * 
 * Defines constants for network-related identifiers.
 * https://github.com/tpn/winsdk-10/blob/master/Include/10.0.14393.0/shared/lmcons.h
 */
#include <lmcons.h>

/**
 * Strings.
 * 
 * Defines one variable type, one macro, and various functions for manipulating arrays of characters.
 * https://www.tutorialspoint.com/c_standard_library/string_h.htm
 */
#include <string.h>

/**
 * Standard Library.
 * 
 * Defines four variable types, several macros, and various functions for performing general functions.
 * https://www.tutorialspoint.com/c_standard_library/stdlib_h.htm
 */
#include <stdlib.h>

/**
 * Retrieve the current hostname.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @returns char* The hostname string (must be freed by the caller).
 */
char* GetCurrentHostName(struct Relocatable* context) {
    DWORD dwHostNameLen = MAX_COMPUTERNAME_LENGTH + 1;
    char szHostName[MAX_COMPUTERNAME_LENGTH + 1];
    char* lpHostName = NULL;

    if (!context->functions.GetComputerNameA(szHostName, &dwHostNameLen)) {
        goto FALLBACK_AND_RETURN;
    }

    lpHostName = (char*) context->functions.calloc(dwHostNameLen + 1, sizeof(char));
    if (lpHostName == NULL) goto FALLBACK_AND_RETURN;

    context->functions.strcpy(lpHostName, szHostName);
    goto RETURN;

FALLBACK_AND_RETURN:
    DEFINE_STRING(sUnknown, "[UNKNOWN]");

    lpHostName = (char*) context->functions.calloc(sizeof(sUnknown) + 1, sizeof(char));
    if (lpHostName == NULL) goto RETURN;

    context->functions.strcpy(lpHostName, sUnknown);

RETURN:
    return lpHostName;
}
