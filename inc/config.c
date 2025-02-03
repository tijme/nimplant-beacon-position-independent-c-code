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
 * Config struct
 */
struct NimPlantConfig {
	int ListenerType;                  // 0 = HTTP, 1 = HTTPS
    wchar_t* ListenerHostname;
    wchar_t* ListenerIP;
    int ListenerPort;
    wchar_t* ListenerRegisterPath;
    wchar_t* ListenerTaskPath;
    wchar_t* ListenerResultPath;
    bool NimPlantRiskyMode;
    bool NimPlantSleepMask;
    int NimPlantSleepTime;
    int NimPlantSleepJitter;
    wchar_t* NimPlantKillDate;
    wchar_t* NimPlantUserAgent;
    uint64_t XorKey;
};

/**
 * Populate the NimPlant beacon config
 * 
 * @param struct NimPlantConfig* config A struct holding (connection) settings for the current beacon.
 * @param int ListenerType Zero for HTTP or one for HTTPS.
 * @param wchar_t* ListenerHostname The hostname to be used in HTTP requests.
 * @param wchar_t* ListenerIP The IP of the listener to connect to.
 * @param int ListenerPort The TCP port of the listener to connect to.
 * @param wchar_t* ListenerRegisterPath An HTTP path (such as `/register`) to be used for implant registration.
 * @param wchar_t* ListenerTaskPath An HTTP path (such as `/task`) to retrieve a new task from.
 * @param wchar_t* ListenerResultPath An HTTP path (such as `/result`) to send a task result to.
 * @param int NimPlantRiskyMode One if risky mode is enabled, zero otherwise.
 * @param int NimPlantSleepMask The type of sleep-mask (currently not implemented).
 * @param int NimPlantSleepTime The amount of seconds to sleep between callbacks.
 * @param int NimPlantSleepJitter The percentage of sleep jitter (variation in the amount of seconds).
 * @param wchar_t* NimPlantKillDate On which date and time the implant should stop working.
 * @param wchar_t* NimPlantUserAgent The HTTP user agent to be used in HTTP requests.
 * @param uint64_t XorKey The XOR key to use to generate the encryption key.
 */
void PopulateNimPlantConfig(struct NimPlantConfig* config, int ListenerType, wchar_t* ListenerHostname, wchar_t* ListenerIP, int ListenerPort, wchar_t* ListenerRegisterPath, wchar_t* ListenerTaskPath, wchar_t* ListenerResultPath, bool NimPlantRiskyMode, bool NimPlantSleepMask, int NimPlantSleepTime, int NimPlantSleepJitter, wchar_t* NimPlantKillDate, wchar_t* NimPlantUserAgent, uint64_t XorKey) {
    config->ListenerType = ListenerType;
    config->ListenerHostname = ListenerHostname;
    config->ListenerIP = ListenerIP;
    config->ListenerPort = ListenerPort;
    config->ListenerRegisterPath = ListenerRegisterPath;
    config->ListenerTaskPath = ListenerTaskPath;
    config->ListenerResultPath = ListenerResultPath;
    config->NimPlantRiskyMode = NimPlantRiskyMode;
    config->NimPlantSleepMask = NimPlantSleepMask;
    config->NimPlantSleepTime = NimPlantSleepTime;
    config->NimPlantSleepJitter = NimPlantSleepJitter;
    config->NimPlantKillDate = NimPlantKillDate;
    config->NimPlantUserAgent = NimPlantUserAgent;
    config->XorKey = XorKey;
}