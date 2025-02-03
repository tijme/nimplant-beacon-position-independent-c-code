/**
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###################                                         ###################
 * ################### THIS IS AN EXTERNAL LIBRARY FROM GITHUB ###################
 * ###################                                         ###################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * 
 * The author and owner of this specific Base64 implementation is @realapire: 
 * https://github.com/realapire. Copyright may apply. Please refer to the original 
 * source.
 * 
 * Original source: https://github.com/realapire/base64-encode-decode
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

/**
 * Encode the given string using base64.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param uint8_t* in The given string to encode.
 * @param uint64_t inlen Input length.
 * @param size_t* outlen Output length.
 * @return char* A newly allocated base64-encoded string.
 */
char* Base64Encode(struct Relocatable* context, uint8_t* in, uint64_t inlen, size_t* outlen) {
    const char base64en[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/',
    };

    size_t output_length = 4 * ((inlen + 2) / 3);
    char *out = (char *)context->functions.calloc(output_length + 1, sizeof(uint8_t));
    if (out == NULL) return NULL;

    int s;
    unsigned int i;
    unsigned int j;
    unsigned char c;
    unsigned char l;

    s = 0;
    l = 0;
    for (i = j = 0; i < inlen; i++) {
        c = in[i];

        switch (s) {
        case 0:
            s = 1;
            out[j++] = base64en[(c >> 2) & 0x3F];
            break;
        case 1:
            s = 2;
            out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
            break;
        case 2:
            s = 0;
            out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
            out[j++] = base64en[c & 0x3F];
            break;
        }
        l = c;
    }

    switch (s) {
    case 1:
        out[j++] = base64en[(l & 0x3) << 4];
        out[j++] = BASE64_PAD;
        out[j++] = BASE64_PAD;
        break;
    case 2:
        out[j++] = base64en[(l & 0xF) << 2];
        out[j++] = BASE64_PAD;
        break;
    }

    out[j] = 0;

    if (outlen != NULL) *outlen = j;

    return out;
}

/**
 * Decode the given string using base64.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param char* input The given string to decode.
 * @param uint64_t input_length Input length.
 * @param size_t* output_length Output length.
 * @return char* A newly allocated base64-decoded string.
 */
uint8_t* Base64Decode(struct Relocatable* context, char* input, size_t input_length, size_t* output_length) {
    DEFINE_STRING(base64_chars, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

    // Check for valid Base64 length
    if (input_length % 4 != 0) return NULL;
    
    // Calculate the expected output length
    *output_length = (3 * input_length) / 4;
    if (input[input_length - 1] == '=') {
        (*output_length)--;
    }
    if (input[input_length - 2] == '=') {
        (*output_length)--;
    }

    // Allocate memory for the decoded data
    uint8_t* decoded_data = (uint8_t*) context->functions.calloc(*output_length + 1, sizeof(char));
    if (decoded_data == NULL) return NULL;

    // Initialize variables for decoding process
    size_t j = 0;
    uint32_t sextet_bits = 0;
    int sextet_count = 0;

    // Loop through the Base64 input and decode it
    for (size_t i = 0; i < input_length; i++) {
        // Convert Base64 character to a 6-bit value
        uint32_t base64_value = 0;
        if (input[i] == '=') {
            base64_value = 0;
        } else {
            char *char_pointer = context->functions.strchr(base64_chars, input[i]);
            if (char_pointer == NULL) {
                context->functions.free(decoded_data);
                return NULL; // Invalid Base64 character
            }
            base64_value = char_pointer - base64_chars;
        }

        // Combine 6-bit values into a 24-bit sextet
        sextet_bits = (sextet_bits << 6) | base64_value;
        sextet_count++;

        // When a sextet is complete, decode it into three bytes
        if (sextet_count == 4) {
            decoded_data[j++] = (sextet_bits >> 16) & 0xFF;
            decoded_data[j++] = (sextet_bits >> 8) & 0xFF;
            decoded_data[j++] = sextet_bits & 0xFF;
            sextet_bits = 0;
            sextet_count = 0;
        }
    }

    return decoded_data;
}