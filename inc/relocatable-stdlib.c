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
 * Data type limits.
 * 
 * The macros defined in this header, limits the values of various variable types like char, int and long.
 * https://www.tutorialspoint.com/c_standard_library/limits_h.htm
 */
#include <limits.h>

/**
 * Windows API.
 * 
 * Contains declarations for all of the functions, macro's & data types in the Windows API.
 * https://docs.microsoft.com/en-us/previous-versions//aa383749(v=vs.85)?redirectedfrom=MSDN
 */
#include <windows.h>

/**
 * Convert char* to wchar_t*.
 * 
 * @param struct Relocatable* context A struct with all loaded modules & functions.
 * @param char* input The input char*
 * @returns wchar_t* The result
 */
wchar_t* RelocatableCharToWchar(struct Relocatable* context, char* input) {
    if (!input) return NULL;

    int size_needed = context->functions.MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
    if (size_needed == 0) return NULL;

    wchar_t* wide_string = (wchar_t*)context->functions.calloc(size_needed, sizeof(wchar_t));
    if (!wide_string) return NULL;

    if (context->functions.MultiByteToWideChar(CP_UTF8, 0, input, -1, wide_string, size_needed) == 0) {
        context->functions.free(wide_string);
        return NULL;
    }

    return wide_string;
}

/**
 * Check if a character is a decimal digit (0-9).
 * 
 * @param char c Character to check.
 * @return int Positive if character is a decimal digit, 0 otherwise.
 */
int RelocatableIsDigit(char c) {
    return (c >= '0' && c <= '9');
}

/**
 * Check if a character is a hexadecimal digit (0-9, a-f, A-F).
 * 
 * @param char c Character to check.
 * @return int Positive if character is a hexadecimal digit, 0 otherwise.
 */
int RelocatableIsXDigit(char c) {
    return ( (c >= '0' && c <= '9') || 
             (c >= 'a' && c <= 'f') || 
             (c >= 'A' && c <= 'F') );
}

/**
 * Convert a string to a long long integer.
 * 
 * @param const char* str The string to convert.
 * @param char** endptr Pointer to character after the number (optional).
 * @param int base The base to use for conversion (0 for auto-detection).
 * @return long long The converted long long integer.
 */
long long RelocatableStringToLong(const char* str, char** endptr, int base) {
    if (!str) return 0;

    const char *ptr = str;
    long long result = 0;
    int sign = 1;

    // Skip white spaces
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r' || *ptr == '\f' || *ptr == '\v') {
        ptr++;
    }

    // Handle sign
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // Handle base
    if (base == 0) {
        if (*ptr == '0') {
            ptr++;
            if (*ptr == 'x' || *ptr == 'X') {
                base = 16;
                ptr++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }

    // Convert the string to an integer
    while ((*ptr >= '0' && *ptr <= '9') || 
           (base == 16 && ((*ptr >= 'a' && *ptr <= 'f') || (*ptr >= 'A' && *ptr <= 'F')))) {

        int digit = (*ptr >= '0' && *ptr <= '9') ? *ptr - '0' :
                    (*ptr >= 'a' && *ptr <= 'f') ? *ptr - 'a' + 10 :
                    (*ptr >= 'A' && *ptr <= 'F') ? *ptr - 'A' + 10 : 0;

        if (digit >= base) break;

        result = result * base + digit;
        ptr++;
    }

    // Return result with sign
    if (endptr) *endptr = (char*)ptr;
    return sign * result;
}

/**
 * Compute the power of a number (base^exponent).
 * 
 * @param int base The base value.
 * @param int exponent The exponent value.
 * @return double The result of base raised to the exponent.
 */
double RelocatablePow(double base, int exponent) {
    // Special case: any number raised to the power of 0 is 1
    if (exponent == 0) {
        return 1.0;
    }

    double result = 1.0;
    int abs_exponent = (exponent > 0) ? exponent : -exponent;

    // Multiply base by itself abs_exponent times
    for (int i = 0; i < abs_exponent; i++) {
        result *= base;
    }

    // If the exponent is negative, invert the result
    if (exponent < 0) {
        result = 1.0 / result;
    }

    return result;
}

/**
 * Convert a string to a double.
 * 
 * @param const char* str The string to convert.
 * @param char** endptr Pointer to character after the number (optional).
 * @return double The converted double.
 */
double RelocatableStringToDouble(const char* str, char** endptr) {
    if (!str) return 0.0;

    const char *ptr = str;
    double result = 0.0;
    int sign = 1;

    // Skip white spaces
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r' || *ptr == '\f' || *ptr == '\v') {
        ptr++;
    }

    // Handle sign
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // Convert the integer part
    while (*ptr >= '0' && *ptr <= '9') {
        result = result * 10.0 + (*ptr - '0');
        ptr++;
    }

    // Convert the fractional part
    if (*ptr == '.') {
        ptr++;
        double factor = 0.1;
        while (*ptr >= '0' && *ptr <= '9') {
            result += (*ptr - '0') * factor;
            factor /= 10.0;
            ptr++;
        }
    }

    // Handle exponent part
    if (*ptr == 'e' || *ptr == 'E') {
        ptr++;
        int exp_sign = 1;
        int exponent = 0;

        if (*ptr == '-') {
            exp_sign = -1;
            ptr++;
        } else if (*ptr == '+') {
            ptr++;
        }

        while (*ptr >= '0' && *ptr <= '9') {
            exponent = exponent * 10 + (*ptr - '0');
            ptr++;
        }

        result *= RelocatablePow(10.0, exp_sign * exponent);
    }

    if (endptr) *endptr = (char*)ptr;
    return sign * result;
}


/**
 * Resolve a relative path to an absolute path based on the current working directory.
 * 
 * @note This function was generated by ChatGPT and thus likely not (memory) safe.
 * 
 * @param char* relativePath The relative path to resolve.
 * @returns char* A buffer containing the absolute path, or NULL on failure.
 */
char* ResolveRelativePath(struct Relocatable* context, const char* relativePath) {
    char* absPath = NULL;
    DWORD dwBufferSize = MAX_PATH;

    absPath = context->functions.calloc(dwBufferSize, sizeof(char));
    if (absPath == NULL) return NULL;

    // Convert relative path to absolute path
    if (context->functions.GetFullPathNameA(relativePath, dwBufferSize, absPath, NULL) == 0) {
        context->functions.free(absPath);
        return NULL;
    }

    return absPath;
}