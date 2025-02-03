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
 * Free all allocated memory belonging to a NimPlant task
 * 
 * @param struct Relocatable* context A 'global' variable capturing Relocatable's entire context (loaded modules & functions)
 * @param struct NimPlantTask* The task to free.
 */
void FreeTask(struct Relocatable* context, struct NimPlantTask* lpTask) {
	if (lpTask == NULL) return;

    if (lpTask->guid != NULL) {
    	context->functions.free(lpTask->guid);
    }

    if (lpTask->command != NULL) {
    	context->functions.free(lpTask->command);
    }

    if (lpTask->result != NULL) {
    	context->functions.free(lpTask->result);
    }

    if (lpTask->arguments != NULL) {
        for (size_t i = 0; i < lpTask->argumentsCount; i++) {
            if (lpTask->arguments[i] != NULL) {
                context->functions.free(lpTask->arguments[i]);
            }
        }
        
        context->functions.free(lpTask->arguments);
    }

    context->functions.free(lpTask);
}
