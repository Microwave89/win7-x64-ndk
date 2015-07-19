/*++ NDK Version: 0098

Copyright (c) Alex Ionescu.  All rights reserved.

Header Name:

    dbgkfuncs.h

Abstract:

    Function definitions for the User Mode Debugging Facility.

Author:

    Alex Ionescu (alexi@tinykrnl.org) - Updated - 27-Feb-2006

--*/

#ifndef _DBGKFUNCS_H
#define _DBGKFUNCS_H

//
// Dependencies
//
#include <..\ndk\umtypes.h>
#include <..\ndk\dbgktypes.h>
#include <..\ndk\exfuncs.h>
#include <..\ndk\rtlfuncs.h>
#include <ntstrsafe.h>

//
// Native calls
//
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDebugActiveProcess(
    _In_ HANDLE Process,
    _In_ HANDLE DebugObject
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateDebugObject(
    _Out_ PHANDLE DebugHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG Flags
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDebugContinue(
    _In_ HANDLE DebugObject,
    _In_ PCLIENT_ID AppClientId,
    _In_ NTSTATUS ContinueStatus
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForDebugEvent(
    _In_ HANDLE DebugObject,
    _In_ BOOLEAN Alertable,
    _In_opt_ PLARGE_INTEGER Timeout,
    _Out_ PDBGUI_WAIT_STATE_CHANGE StateChange
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRemoveProcessDebug(
    _In_ HANDLE Process,
    _In_ HANDLE DebugObject
);

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationDebugObject(
    _In_ HANDLE DebugObject,
    _In_ DEBUGOBJECTINFOCLASS InformationClass,
    _In_ PVOID Information,
    _In_ ULONG InformationLength,
    _Out_opt_ PULONG ReturnLength
);

NTSYSAPI
NTSTATUS
NTAPI
ZwDebugActiveProcess(
    _In_ HANDLE Process,
    _In_ HANDLE DebugObject
);

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateDebugObject(
    _Out_ PHANDLE DebugHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG Flags
);

NTSYSAPI
NTSTATUS
NTAPI
ZwDebugContinue(
    _In_ HANDLE DebugObject,
    _In_ PCLIENT_ID AppClientId,
    _In_ NTSTATUS ContinueStatus
);

NTSYSAPI
NTSTATUS
NTAPI
ZwRemoveProcessDebug(
    _In_ HANDLE Process,
    _In_ HANDLE DebugObject
);

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForDebugEvent(
    _In_ HANDLE DebugObject,
    _In_ BOOLEAN Alertable,
    _In_opt_ PLARGE_INTEGER Timeout,
    _Out_ PDBGUI_WAIT_STATE_CHANGE StateChange
);

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationDebugObject(
    _In_ HANDLE DebugObject,
    _In_ DEBUGOBJECTINFOCLASS InformationClass,
    _In_ PVOID Information,
    _In_ ULONG InformationLength,
    _Out_opt_ PULONG ReturnLength
);

//#if defined(NTOS_MODE_USER) && !defined(BOOT_APP)
//int _fltused = 0;
#if defined(NTOS_MODE_USER)
//#if !defined(BOOT_APP)
//extern void __cdecl __va_start(va_list *, ...);
//#define my_va_start(ap, x) (__va_start(&ap, x))
//typedef int(*P_VSNPRINTF_S)(char *buffer, unsigned long long sizeOfBuffer, unsigned long long count, const char *format, va_list argptr);
//static P_VSNPRINTF_S sg_fpVsnprintf_s;
//
//static void DebugPrint2A(const char* pFormatstring, ...){	///LEGACY!!! Use myWPrintf in new designs!
//	void* hMod = (void*)0;
//	char pBuf[1024];
//	if (!pFormatstring)
//		return;
//
//	pBuf[sizeof(pBuf) - 1] = 0;
//	pBuf[sizeof(pBuf) - 2] = 0;
//	//RtlZeroMemory(pBuf, sizeof(pBuf));
//	//SecureZeroMemory()
//
//	if (!sg_fpVsnprintf_s){
//		hMod = LoadLibraryA("msvcrt.dll");
//		sg_fpVsnprintf_s = (P_VSNPRINTF_S)GetProcAddress(hMod, "_vsnprintf_s");
//		if (!sg_fpVsnprintf_s)
//			return;
//	}
//
//	va_list args;
//	my_va_start(args, pFormatstring);
//
//	sg_fpVsnprintf_s(pBuf, sizeof(pBuf), (unsigned long long)(-1), pFormatstring, args); // C4996
//	OutputDebugStringA(pBuf);
//}
//#endif ///!BOOT_APP
//#elif defined(BOOT_APP)
//extern void __cdecl __va_start(va_list *, ...);
//#define my_va_start(ap, x) (__va_start(&ap, x))
//typedef int(*P_VSNPRINTF_S)(char *buffer, unsigned long long sizeOfBuffer, unsigned long long count, const char *format, va_list argptr);
//static P_VSNPRINTF_S sg_fpVsnprintf_s;

static void myWPrintf(const WCHAR* pFormatString, ...){
	WCHAR szPrintBuf[512];
	va_list args;
	NTSTATUS status;

#if defined (CONSOLE_APP)
#if defined(CONSOLE_OUTPUT)
#error STATUS_NOT_IMPLEMENTED!
#elif defined(DBGVIEW_OUTPUT)

	if (!pFormatString)
		return;

	RtlSecureZeroMemory(szPrintBuf, sizeof(szPrintBuf));

#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringValidateDestW(szPrintBuf, sizeof(szPrintBuf)/sizeof(WCHAR), NTSTRSAFE_MAX_CCH);
#pragma warning(pop)
	if(status){
		OutputDebugStringW(L"RtlStringValidateDestW error!");
		return;
	}

	va_start(args, pFormatString);
#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringVPrintfWorkerW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NULL, pFormatString, args);
#pragma warning(pop)
	va_end(args);
	if (status){
		OutputDebugStringW(L"RtlStringVPrintfWorkerW error!");
		return;
	}

	OutputDebugStringW(szPrintBuf);
#else
#error With CONSOLE_APP, either CONSOLE_OUTPUT or DBGVIEW_OUTPUT must be defined!
#endif

#elif defined (WIN32_APP)
#if defined(DBGVIEW_OUTPUT)
	
	if (!pFormatString)
		return;

	RtlSecureZeroMemory(szPrintBuf, sizeof(szPrintBuf));

#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringValidateDestW(szPrintBuf, sizeof(szPrintBuf)/sizeof(WCHAR), NTSTRSAFE_MAX_CCH);
#pragma warning(pop)
	if(status){
		OutputDebugStringW(L"RtlStringValidateDestW error!");
		return;
	}

	va_start(args, pFormatString);
#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringVPrintfWorkerW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NULL, pFormatString, args);
#pragma warning(pop)
	va_end(args);
	if (status){
		OutputDebugStringW(L"RtlStringVPrintfWorkerW error!");
		return;
	}

	OutputDebugStringW(szPrintBuf);
#elif defined (HARDERROR_OUTPUT)
	ULONG harderrorResponse;
	ULONGLONG harderrorParams[4];
	UNICODE_STRING uPrintBuf;
	UNICODE_STRING uTitle;
	WCHAR szTitle[] = L"Bla,undso !!!!!";

	if (!pFormatString)
		return;

	RtlSecureZeroMemory(szPrintBuf, sizeof(szPrintBuf));

#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringValidateDestW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NTSTRSAFE_MAX_CCH);
#pragma warning(pop)
	if (status){
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
		return;
	}

	va_start(args, pFormatString);
#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringVPrintfWorkerW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NULL, pFormatString, args);
#pragma warning(pop)
	va_end(args);
	if (status){
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
		return;
	}

	RtlInitUnicodeString(&uPrintBuf, szPrintBuf);
	RtlInitUnicodeString(&uTitle, szTitle);

	harderrorParams[0] = (ULONGLONG)&uPrintBuf;
	harderrorParams[1] = (ULONGLONG)&uTitle;
	harderrorParams[2] = (ULONGLONG)((ULONG)OptionOk | (ULONG)MB_ICONINFORMATION | (ULONG)0 | (ULONG)MB_DEFBUTTON1);
	harderrorParams[3] = (ULONGLONG)INFINITE;

	status = NtRaiseHardError(STATUS_SERVICE_NOTIFICATION, 4, 0x3, harderrorParams, 0, &harderrorResponse);
	if (status)
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
#else
#error With WIN32_APP, either DBGVIEW_OUTPUT or HARDERROR_OUTPUT must be defined!
#endif

#elif defined (BOOT_APP)
#if defined (BOOTSCR_OUTPUT)
	UNICODE_STRING uPrintBuf;

	if (!pFormatString)
		return;

	RtlSecureZeroMemory(szPrintBuf, sizeof(szPrintBuf));

#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringValidateDestW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NTSTRSAFE_MAX_CCH);
#pragma warning(pop)
	if (status){
		RtlInitUnicodeString(&uPrintBuf, L"RtlStringCbPrintfW error!");
		NtDisplayString(&uPrintBuf);
		return;
	}
	
	va_start(args, pFormatString);
#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringVPrintfWorkerW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NULL, pFormatString, args);
#pragma warning(pop)
	va_end(args);
	if (status){
		RtlInitUnicodeString(&uPrintBuf, L"RtlStringVPrintfWorkerW error!");
		NtDisplayString(&uPrintBuf);
		return;
	}

	RtlInitUnicodeString(&uPrintBuf, szPrintBuf);
	NtDisplayString(&uPrintBuf);
#elif defined (HARDERROR_OUTPUT)
	ULONG harderrorResponse;
	ULONGLONG harderrorParams[4];
	UNICODE_STRING uPrintBuf;
	UNICODE_STRING uTitle;
	WCHAR szTitle[] = L"Bla,undso !!!!!";

	if (!pFormatString)
		return;

	RtlSecureZeroMemory(szPrintBuf, sizeof(szPrintBuf));

#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringValidateDestW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NTSTRSAFE_MAX_CCH);
#pragma warning(pop)
	if (status){
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
		return;
	}

	va_start(args, pFormatString);
#pragma warning(push)
#pragma warning(disable:4995) ///name was marked as #pragma deprecated
	status = RtlStringVPrintfWorkerW(szPrintBuf, sizeof(szPrintBuf) / sizeof(WCHAR), NULL, pFormatString, args);
#pragma warning(pop)
	va_end(args);
	if (status){
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
		return;
	}

	RtlInitUnicodeString(&uPrintBuf, szPrintBuf);
	RtlInitUnicodeString(&uTitle, szTitle);
	
	harderrorParams[0] = (ULONGLONG)&uPrintBuf;
	harderrorParams[1] = (ULONGLONG)&uTitle;
	harderrorParams[2] = (ULONGLONG)((ULONG)OptionOk | (ULONG)MB_ICONINFORMATION | (ULONG)0 | (ULONG)MB_DEFBUTTON1);
	harderrorParams[3] = (ULONGLONG)INFINITE;

	status = NtRaiseHardError(STATUS_SERVICE_NOTIFICATION, 4, 0x3, harderrorParams, 0, &harderrorResponse);
	if(status)
		NtRaiseHardError(status, 0, 0, NULL, 0, (PULONG)status);
#else
#error With BOOT_APP, either BOOTSCR_OUTPUT or HARDERROR_OUTPUT must be defined!
#endif

#else
#error Either CONSOLE_APP, WIN32_APP or BOOT_APP must be defined!
#endif
}
#endif ///UM
#endif ///DBGKFUNCS_H
