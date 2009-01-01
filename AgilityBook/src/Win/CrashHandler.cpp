/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Trap information about unhandled exceptions.
 * @author David Connet
 *
 * See Bugslayer by John Robbins in MSJ Aug 1998.
 *
 * Revision History
 * @li 2006-05-24 DRC Updated for 64 bit, dropped VC6 support
 * @li 2004-10-22 DRC Created
 */

#include "stdafx.h"
#include "CrashHandler.h"

#include "imagehlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#if !defined(UNICODE) && _MSC_VER >= 1300

static HKEY g_hAppKey = NULL;

// The static buffer returned by various functions. This avoids putting
// things on the stack.
#define BUFF_SIZE 1024
static TCHAR g_szBuff[BUFF_SIZE];
static TCHAR g_szBuff2[BUFF_SIZE];
// The static symbol lookup buffer. This gets casted to make it work.
#define SYM_BUFF_SIZE 512
static BYTE g_stSymbol[SYM_BUFF_SIZE];
// The static source and line structure.
static IMAGEHLP_LINE64 g_stLine64;
// The stack frame used in walking the stack.
static STACKFRAME64 g_stFrame64;

static bool g_bSymEngInit = false;
// From ImageHlp.dll
typedef BOOL (__stdcall *PFNSYMGETLINEFROMADDR64)(
		IN HANDLE hProcess,
		IN DWORD64 dwAddr,
		OUT PDWORD pdwDisplacement,
		OUT PIMAGEHLP_LINE64 Line);
static PFNSYMGETLINEFROMADDR64 g_pfnSymGetLineFromAddr = NULL;
// From DbgHelp.dll
typedef DWORD (__stdcall *PFNSYMGETOPTIONS)(VOID);
static PFNSYMGETOPTIONS g_SymGetOptions = NULL;
typedef DWORD (__stdcall *PFNSYMSETOPTIONS)(IN DWORD SymOptions);
static PFNSYMSETOPTIONS g_SymSetOptions = NULL;
typedef BOOL (__stdcall *PFNSYMINITIALIZE)(
		IN HANDLE hProcess,
		IN PSTR UserSearchPath,
		IN BOOL fInvadeProcess);
static PFNSYMINITIALIZE g_SymInitialize = NULL;
static PGET_MODULE_BASE_ROUTINE64 g_SymGetModuleBase = NULL;
typedef BOOL (__stdcall *PFNSYMGETSYMFROMADDR64)(
		IN HANDLE hProcess,
		IN DWORD64 dwAddr,
		OUT PDWORD64 pdwDisplacement,
		OUT PIMAGEHLP_SYMBOL64 Symbol);
static PFNSYMGETSYMFROMADDR64 g_SymGetSymFromAddr = NULL;
typedef PVOID (__stdcall *PFNSYMFUNCTIONTABLEACCESS64)(
		HANDLE hProcess,
		DWORD64 AddrBase);
static PFNSYMFUNCTIONTABLEACCESS64 g_SymFunctionTableAccess = NULL;
typedef BOOL (__stdcall *PFNSTACKWALK64)(
		DWORD                             MachineType,
		HANDLE                            hProcess,
		HANDLE                            hThread,
		LPSTACKFRAME64                    StackFrame,
		PVOID                             ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
		);
static PFNSTACKWALK64 g_StackWalk = NULL;


static DWORD GetModuleBaseNameA(
		HANDLE hProcess,
		HMODULE hModule,
		LPSTR lpBaseName,
		DWORD nSize)
{
	// The typedefs for the PSAPI.DLL functions used by this module.
	typedef DWORD (WINAPI *GETMODULEBASENAME)(HANDLE hProcess,
		HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);
	static GETMODULEBASENAME g_pGetModuleBaseName = NULL;
	static bool bInitialized = false;
	if (!bInitialized)
	{
		// Load up PSAPI.DLL.
		HINSTANCE hInst = LoadLibraryA("PSAPI.DLL");
		assert(NULL != hInst);
		if (NULL == hInst)
		{
			TRACE0("Unable to load PSAPI.DLL!\n");
			SetLastErrorEx(ERROR_DLL_INIT_FAILED, SLE_ERROR);
			return FALSE;
		}
		// Now do the GetProcAddress stuff.
		g_pGetModuleBaseName = reinterpret_cast<GETMODULEBASENAME>(GetProcAddress(hInst, "GetModuleBaseNameA"));
		assert(NULL != g_pGetModuleBaseName);
		if (NULL == g_pGetModuleBaseName)
		{
			TRACE0("GetProcAddress failed on GetModuleBaseNameA!\n");
			SetLastErrorEx(ERROR_DLL_INIT_FAILED, SLE_ERROR);
			return FALSE;
		}
		bInitialized = true;
	}
	return g_pGetModuleBaseName(hProcess, hModule, lpBaseName, nSize);
}


static bool IsNT()
{
	static bool bHasVersion = false;
	static bool bIsNT = true;
	if (bHasVersion)
		return bIsNT;
	OSVERSIONINFO stOSVI;
	memset(&stOSVI, NULL, sizeof(OSVERSIONINFO));
	stOSVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bRet = GetVersionEx(&stOSVI);
	assert(TRUE == bRet);
	if (!bRet)
	{
		TRACE0("GetVersionEx failed!\n");
		return false;
	}
	// Check the version and call the appropriate thing.
	if (VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId)
		bIsNT = true;
	else
		bIsNT = false;
	bHasVersion = true;
	return bIsNT;
}


static DWORD BSUGetModuleBaseName(
		HANDLE hProcess,
		HMODULE hModule,
		LPTSTR lpBaseName,
		DWORD nSize)
{
	if (IsNT())
	{
		// Call the NT version. It is in NT4ProcessInfo because that is
		// where all the PSAPI wrappers are kept.
		// NTGetModuleBaseName(hProcess, hModule, lpBaseName, nSize);
		return GetModuleBaseNameA(hProcess, hModule, lpBaseName, nSize);
	}
	else
	{
		// Win95GetModuleBaseName(hProcess, hModule, lpBaseName, nSize);
		assert(!IsBadWritePtr(lpBaseName, nSize));
		if (IsBadWritePtr(lpBaseName, nSize))
		{
			TRACE0("Win95GetModuleBaseName Invalid string buffer\n");
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
		}

		// This could blow the stack...
		char szBuff[MAX_PATH + 1];
		DWORD dwRet = GetModuleFileName(hModule, szBuff, MAX_PATH);
		assert(0 != dwRet);
		if (0 == dwRet)
			return 0;

		// Find the last '\' mark.
		char* pStart = strrchr(szBuff, '\\');
		int iMin ;
		if (NULL != pStart)
		{
			// Move up one character.
			++pStart;
			//lint -e666
			iMin = min(static_cast<int>(nSize), lstrlen(pStart) + 1);
			//lint +e666
			lstrcpyn(lpBaseName, pStart, iMin);
		}
		else
		{
			// Copy the szBuff buffer in.
			//lint -e666
			iMin = min(static_cast<int>(nSize), lstrlen(szBuff) + 1);
			//lint +e666
			lstrcpyn(lpBaseName, szBuff, iMin);
		}
		// Always NULL terminate.
		lpBaseName[iMin] = '\0';
		return (DWORD)(iMin - 1);
	}
}


static LPCTSTR ConvertSimpleException(DWORD dwExcept)
{
	switch (dwExcept)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return _T("EXCEPTION_ACCESS_VIOLATION");
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
	case EXCEPTION_BREAKPOINT:
		return _T("EXCEPTION_BREAKPOINT");
	case EXCEPTION_SINGLE_STEP:
		return _T("EXCEPTION_SINGLE_STEP");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT:
		return _T("EXCEPTION_FLT_INEXACT_RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION:
		return _T("EXCEPTION_FLT_INVALID_OPERATION");
	case EXCEPTION_FLT_OVERFLOW:
		return _T("EXCEPTION_FLT_OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK:
		return _T("EXCEPTION_FLT_STACK_CHECK");
	case EXCEPTION_FLT_UNDERFLOW:
		return _T("EXCEPTION_FLT_UNDERFLOW");
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
	case EXCEPTION_INT_OVERFLOW:
		return _T("EXCEPTION_INT_OVERFLOW");
	case EXCEPTION_PRIV_INSTRUCTION:
		return _T("EXCEPTION_PRIV_INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR:
		return _T("EXCEPTION_IN_PAGE_ERROR");
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
	case EXCEPTION_STACK_OVERFLOW:
		return _T("EXCEPTION_STACK_OVERFLOW");
	case EXCEPTION_INVALID_DISPOSITION:
		return _T("EXCEPTION_INVALID_DISPOSITION");
	case EXCEPTION_GUARD_PAGE:
		return _T("EXCEPTION_GUARD_PAGE");
	case EXCEPTION_INVALID_HANDLE:
		return _T("EXCEPTION_INVALID_HANDLE");
	default:
		wsprintf(g_szBuff2, "Unknown Exception (%08x)", dwExcept);
		return g_szBuff2;
	}
}


static BOOL InternalSymGetLineFromAddr(
		HANDLE hProcess,
		DWORD64 dwAddr,
		PDWORD pdwDisplacement,
		PIMAGEHLP_LINE64 Line)
{
	if (NULL != g_pfnSymGetLineFromAddr)
	{
#ifdef WORK_AROUND_SRCLINE_BUG
		// The problem is that the symbol engine only finds those source
		// line addresses (after the first lookup) that fall exactly on
		// a zero displacement. I will walk backwards 100 bytes to
		// find the line and return the proper displacement.
		DWORD dwTempDis = 0;
		while (!g_pfnSymGetLineFromAddr(hProcess, dwAddr - dwTempDis, pdwDisplacement, Line))
		{
			dwTempDis += 1;
			if (100 == dwTempDis)
				return FALSE;
		}
		// It was found and the source line information is correct so
		// change the displacement if it was looked up multiple times.
		if (0 != dwTempDis)
			*pdwDisplacement = dwTempDis;
		return TRUE;
#else // WORK_AROUND_SRCLINE_BUG
		return g_pfnSymGetLineFromAddr(hProcess, dwAddr, pdwDisplacement, Line);
#endif
	}
	return FALSE;
}


static LPCTSTR GetFaultReason(LPEXCEPTION_POINTERS pExPtrs)
{
	assert(FALSE == IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)));
	if (IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)))
	{
		TRACE0("Bad parameter to GetFaultReasonA\n");
		return NULL;
	}

	// The value that holds the return.
	LPCTSTR szRet = NULL;
	__try
	{
		// The current position in the buffer.
		int iCurr = 0;
		// A temp value holder. This is to keep the stack usage to a
		// minimum.
		DWORD64 dwTemp;
		iCurr += BSUGetModuleBaseName(GetCurrentProcess(), NULL, g_szBuff, BUFF_SIZE);
		iCurr += wsprintf(g_szBuff + iCurr , _T(" caused a "));

		dwTemp = reinterpret_cast<DWORD64>(ConvertSimpleException(pExPtrs->ExceptionRecord->ExceptionCode));

		if (NULL != dwTemp)
		{
			iCurr += wsprintf(g_szBuff + iCurr, _T("%s"), dwTemp);
		}
		else
		{
			iCurr += (FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS
				| FORMAT_MESSAGE_FROM_HMODULE,
				GetModuleHandle(_T("NTDLL.DLL")),
				pExPtrs->ExceptionRecord->ExceptionCode,
				0,
				g_szBuff + iCurr,
				BUFF_SIZE,
				0) * sizeof(TCHAR));
		}

		assert(iCurr < BUFF_SIZE);

		iCurr += wsprintf(g_szBuff + iCurr, _T(" in module "));

		dwTemp = g_SymGetModuleBase(GetCurrentProcess(),
			reinterpret_cast<DWORD64>(pExPtrs->ExceptionRecord->ExceptionAddress));

		assert(NULL != dwTemp);

		if (NULL == dwTemp)
		{
			iCurr += wsprintf(g_szBuff + iCurr, _T("<UNKNOWN>"));
		}
		else
		{
			iCurr += BSUGetModuleBaseName(GetCurrentProcess(),
				reinterpret_cast<HINSTANCE>(dwTemp), g_szBuff + iCurr, BUFF_SIZE - iCurr);
		}

#if defined(_M_IX86)
		iCurr += wsprintf(g_szBuff + iCurr,
			_T(" at %04X:%08X"),
			pExPtrs->ContextRecord->SegCs,
			pExPtrs->ExceptionRecord->ExceptionAddress);
#elif defined(_M_AMD64)
		iCurr += wsprintf(g_szBuff + iCurr,
			_T(" at %04X:%08X"),
			pExPtrs->ContextRecord->SegCs,
			pExPtrs->ExceptionRecord->ExceptionAddress);
#elif defined(_ALPHA_)
		iCurr += wsprintf(g_szBuff + iCurr,
			_T(" at %08X"),
			pExPtrs->ExceptionRecord->ExceptionAddress);
#else
#error Undefined architecture
#endif
		assert(iCurr < BUFF_SIZE);

		// Start looking up the exception address.
		//lint -e545
		PIMAGEHLP_SYMBOL64 pSym = (PIMAGEHLP_SYMBOL64)&g_stSymbol;
		//lint +e545
		ZeroMemory(pSym, SYM_BUFF_SIZE);
		pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		pSym->MaxNameLength = SYM_BUFF_SIZE - sizeof(IMAGEHLP_SYMBOL64);

		DWORD64 dwDisp64;
		if (g_SymGetSymFromAddr(GetCurrentProcess(),
			reinterpret_cast<DWORD64>(pExPtrs->ExceptionRecord->ExceptionAddress),
			&dwDisp64, pSym))
		{
			iCurr += wsprintf(g_szBuff + iCurr, _T(", "));

			// Copy no more than there is room for.
			dwTemp = lstrlen(pSym->Name);
			if (static_cast<int>(dwTemp) > (BUFF_SIZE - iCurr - 20))
			{
				lstrcpyn(g_szBuff + iCurr, pSym->Name, BUFF_SIZE - iCurr - 1);
				// Gotta leave now.
				szRet = g_szBuff;
				__leave;
			}
			else
			{
				if (dwDisp64 > 0)
				{
					iCurr += wsprintf(g_szBuff + iCurr,
						_T("%s()+%d byte(s)"), pSym->Name, (int)dwDisp64);
				}
				else
				{
					iCurr += wsprintf(g_szBuff + iCurr,
						_T("%s "), pSym->Name);
				}
			}
		}
		else
		{
			// If the symbol was not found, the source and line will not
			// be found either so leave now.
			szRet = g_szBuff;
			__leave;
		}

		assert(iCurr < BUFF_SIZE);

		// Do the source and line lookup.
		ZeroMemory(&g_stLine64, sizeof(IMAGEHLP_LINE64));
		g_stLine64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwDisp;
		if (InternalSymGetLineFromAddr(GetCurrentProcess(),
			reinterpret_cast<DWORD64>(pExPtrs->ExceptionRecord->ExceptionAddress),
			&dwDisp, &g_stLine64))
		{
			iCurr += wsprintf(g_szBuff + iCurr, _T(", "));

			// Copy no more than there is room for.
			dwTemp = lstrlen(g_stLine64.FileName);
			if (static_cast<int>(dwTemp) > (BUFF_SIZE - iCurr - 25))
			{
				lstrcpyn(g_szBuff + iCurr,
					g_stLine64.FileName,
					BUFF_SIZE - iCurr - 1);
				// Gotta leave now.
				szRet = g_szBuff;
				__leave;
			}
			else
			{
				if (dwDisp > 0)
				{
					iCurr += wsprintf(g_szBuff + iCurr,
						_T("%s, line %d+%d byte(s)"),
						g_stLine64.FileName,
						g_stLine64.LineNumber,
						dwDisp);
				}
				else
				{
					iCurr += wsprintf(g_szBuff + iCurr,
						_T("%s, line %d"),
						g_stLine64.FileName,
						g_stLine64.LineNumber);
				}
			}
		}
		szRet = g_szBuff;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		assert(FALSE);
		szRet = NULL;
	}
	return szRet;
}


static LPCTSTR InternalGetStackTraceString(EXCEPTION_POINTERS* pExPtrs)
{
	assert(!IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)));
	if (IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)))
	{
		TRACE0("GetStackTraceString - invalid pExPtrs!\n");
		return NULL;
	}

	// The value that is returned.
	LPCTSTR szRet = NULL;
	// A temporary for all to use. This saves stack space.
	DWORD64 dwTemp = 0;

	__try
	{
#if defined(_M_IX86)
#define CH_MACHINE IMAGE_FILE_MACHINE_I386
#elif defined(_M_AMD64)
#define CH_MACHINE IMAGE_FILE_MACHINE_AMD64
#elif defined(_ALPHA_)
#define CH_MACHINE IMAGE_FILE_MACHINE_ALPHA
#else
#error Undefined architecture
#endif
		// Note: If the source and line functions are used, then
		// StackWalk can access violate.
		BOOL bSWRet = g_StackWalk(CH_MACHINE,
			GetCurrentProcess(),
			GetCurrentThread(),
			&g_stFrame64,
			pExPtrs->ContextRecord,
			NULL,
			g_SymFunctionTableAccess,
			g_SymGetModuleBase,
			NULL) ;
		if (!bSWRet || 0 == g_stFrame64.AddrFrame.Offset)
		{
			szRet = NULL;
			__leave;
		}

		int iCurr = 0;
		// At a minimum, put the address in.
#if defined(_M_IX86)
		iCurr += wsprintf(g_szBuff + iCurr, _T("%04X:%08X"),
			pExPtrs->ContextRecord->SegCs,
			g_stFrame64.AddrPC.Offset);
#elif defined(_M_AMD64)
		iCurr += wsprintf(g_szBuff + iCurr, _T("%04X:%08X"),
			pExPtrs->ContextRecord->SegCs,
			g_stFrame64.AddrPC.Offset);
#elif defined(_ALPHA_)
		iCurr += wsprintf(g_szBuff + iCurr, _T("0x%08X"),
			g_stFrame64.AddrPC.Offset);
#else
#error Undefined architecture
#endif
		// Do the parameters?
		//if (GSTSO_PARAMS == (dwOpts & GSTSO_PARAMS))
		{
			iCurr += wsprintf(g_szBuff + iCurr,
				_T(" (0x%08X 0x%08X 0x%08X 0x%08X)"),
				g_stFrame64.Params[0],
				g_stFrame64.Params[1],
				g_stFrame64.Params[2],
				g_stFrame64.Params[3]);
		}

		//if (GSTSO_MODULE == (dwOpts & GSTSO_MODULE))
		{
			iCurr += wsprintf(g_szBuff + iCurr, _T(" "));
			dwTemp = g_SymGetModuleBase(GetCurrentProcess(), g_stFrame64.AddrPC.Offset);
			assert(NULL != dwTemp);
			if (NULL == dwTemp)
			{
				iCurr += wsprintf(g_szBuff + iCurr, _T("<UNKNOWN>"));
			}
			else
			{
				iCurr += BSUGetModuleBaseName(GetCurrentProcess(),
					reinterpret_cast<HINSTANCE>(dwTemp),
					g_szBuff + iCurr,
					BUFF_SIZE - iCurr);
			}
		}

		assert (iCurr < BUFF_SIZE);
		DWORD64 dwDisp64;

		//if (GSTSO_SYMBOL == (dwOpts & GSTSO_SYMBOL))
		{
			// Start looking up the exception address.
			//lint -e545
			PIMAGEHLP_SYMBOL64 pSym = (PIMAGEHLP_SYMBOL64)&g_stSymbol;
			//lint +e545
			ZeroMemory(pSym, SYM_BUFF_SIZE);
			pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
			pSym->MaxNameLength = SYM_BUFF_SIZE - sizeof(IMAGEHLP_SYMBOL64);

			if (g_SymGetSymFromAddr(GetCurrentProcess(),
				g_stFrame64.AddrPC.Offset, &dwDisp64, pSym))
			{
				iCurr += wsprintf(g_szBuff + iCurr, _T(", "));

				// Copy no more than there is room for.
				dwTemp = lstrlen(pSym->Name);
				if (dwTemp > (DWORD)(BUFF_SIZE - iCurr - 20))
				{
					lstrcpyn(g_szBuff + iCurr, pSym->Name, BUFF_SIZE - iCurr - 1);
					// Gotta leave now.
					szRet = g_szBuff;
					__leave;
				}
				else
				{
					if (dwDisp64 > 0)
					{
						iCurr += wsprintf(g_szBuff + iCurr,
							_T("%s()+%d byte(s)"),
							pSym->Name, (int)dwDisp64);
					}
					else
					{
						iCurr += wsprintf(g_szBuff + iCurr,
							_T("%s"), pSym->Name);
					}
				}
			}
			else
			{
				// If the symbol was not found, the source and line will
				// not be found either so leave now.
				szRet = g_szBuff;
				__leave;
			}

		}

		//if (GSTSO_SRCLINE == (dwOpts & GSTSO_SRCLINE))
		{
			ZeroMemory(&g_stLine64, sizeof(IMAGEHLP_LINE64));
			g_stLine64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			DWORD dwDisp;
			if (InternalSymGetLineFromAddr(GetCurrentProcess(),
				g_stFrame64.AddrPC.Offset, &dwDisp, &g_stLine64))
			{
				iCurr += wsprintf(g_szBuff + iCurr, _T(", "));

				// Copy no more than there is room for.
				dwTemp = lstrlen(g_stLine64.FileName);
				if (dwTemp > (DWORD)(BUFF_SIZE - iCurr - 25))
				{
					lstrcpyn(g_szBuff + iCurr, g_stLine64.FileName, BUFF_SIZE - iCurr - 1);
					// Gotta leave now.
					szRet = g_szBuff;
					__leave;
				}
				else
				{
					if (dwDisp > 0)
					{
						iCurr += wsprintf(g_szBuff + iCurr,
							_T ("%s, line %d+%d byte(s)"),
							g_stLine64.FileName,
							g_stLine64.LineNumber,
							(int)dwDisp);
					}
					else
					{
						iCurr += wsprintf(g_szBuff + iCurr,
							_T("%s, line %d"),
							g_stLine64.FileName,
							g_stLine64.LineNumber);
					}
				}
			}
		}

		szRet = g_szBuff;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		assert(FALSE);
		szRet = NULL;
	}
	return szRet;
}


static LPCTSTR GetFirstStackTraceString(LPEXCEPTION_POINTERS pExPtrs)
{
	// All of the error checking is in the InternalGetStackTraceString
	// function.

	// Initialize the STACKFRAME structure.
	ZeroMemory(&g_stFrame64, sizeof(STACKFRAME64));
#if defined(_M_IX86)
	g_stFrame64.AddrPC.Offset = pExPtrs->ContextRecord->Eip;
	g_stFrame64.AddrPC.Mode = AddrModeFlat;
	g_stFrame64.AddrStack.Offset = pExPtrs->ContextRecord->Esp;
	g_stFrame64.AddrStack.Mode = AddrModeFlat;
	g_stFrame64.AddrFrame.Offset = pExPtrs->ContextRecord->Ebp;
	g_stFrame64.AddrFrame.Mode = AddrModeFlat;
#elif defined(_M_AMD64)
	g_stFrame64.AddrPC.Offset = pExPtrs->ContextRecord->Rip;
	g_stFrame64.AddrPC.Mode = AddrModeFlat;
	g_stFrame64.AddrStack.Offset = pExPtrs->ContextRecord->Rsp;
	g_stFrame64.AddrStack.Mode = AddrModeFlat;
	g_stFrame64.AddrFrame.Offset = pExPtrs->ContextRecord->Rbp;
	g_stFrame64.AddrFrame.Mode = AddrModeFlat;
#elif defined(_ALPHA_)
	g_stFrame64.AddrPC.Offset = (DWORD)pExPtrs->ContextRecord->Fir;
	g_stFrame64.AddrPC.Mode = AddrModeFlat;
	g_stFrame64.AddrReturn.Offset = (DWORD)pExPtrs->ContextRecord->IntRa;
	g_stFrame64.AddrReturn.Mode = AddrModeFlat;
	g_stFrame64.AddrStack.Offset = (DWORD)pExPtrs->ContextRecord->IntSp;
	g_stFrame64.AddrStack.Mode = AddrModeFlat;
	g_stFrame64.AddrFrame.Offset = (DWORD)pExPtrs->ContextRecord->IntFp;
	g_stFrame64.AddrFrame.Mode = AddrModeFlat;
#else
#error Undefined architecture
#endif

	return InternalGetStackTraceString(pExPtrs);
}


static LPCTSTR GetNextStackTraceString(LPEXCEPTION_POINTERS pExPtrs)
{
	// All error checking is in InternalGetStackTraceString.
	// Assume that GetFirstStackTraceString has already initialized the
	// stack frame information.
	return InternalGetStackTraceString(pExPtrs);
}


#define PRINT_INDENT	for (int indent = 0; indent < inIndent; ++indent) fputc(' ', output);


// This function does put a bit of data on the stack and heap.
// Hopefully it won't cause more trouble... So we log this data last.
static void QueryKey(
		FILE* output,
		HKEY hKey,
		int inIndent)
{
	if (!hKey)
		return;
	// Prevent recursion from going too deep.
	if (3 < inIndent)
		return;

	DWORD i;
	DWORD dwLen;
	// Enumerate the subkeys, recursively.
	for (i = 0; ; ++i)
	{
		dwLen = BUFF_SIZE;
		// A key name max size is 255.
		if (ERROR_SUCCESS != RegEnumKeyEx(hKey, i, g_szBuff, &dwLen, NULL, NULL, NULL, NULL))
			break;
		HKEY hSubKey;
		if (ERROR_SUCCESS == RegOpenKeyEx(hKey, g_szBuff, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hSubKey))
		{
			PRINT_INDENT
			fputs(g_szBuff, output);
			fputs("\n", output);
			QueryKey(output, hSubKey, inIndent + 1);
			RegCloseKey(hSubKey);
		}
	}

	// Enumerate the key values.
	for (i = 0; ; ++i)
	{
		dwLen = BUFF_SIZE;
		DWORD type;
		if (ERROR_SUCCESS != RegEnumValue(hKey, i,
			g_szBuff, &dwLen, NULL,
			&type, NULL, NULL))
		{
			break;
		}
		bool bOk = false;
		TCHAR* pType = "??";
		switch (type)
		{
		default:
			break;
		case REG_BINARY:
			pType = "BINARY";
			break;
		//REG_DWORD_LITTLE_ENDIAN == REG_DWORD
		case REG_DWORD_BIG_ENDIAN:
			pType = "DWORD_BIG_ENDIAN";
			break;
		case REG_LINK:
			pType = "LINK";
			break;
		case REG_MULTI_SZ:
			pType = "MULTI_SZ";
			break;
		case REG_NONE:
			pType = "NONE";
			break;
#if _MSC_VER >= 1300
		case REG_QWORD:
		//REG_QWORD_LITTLE_ENDIAN == REG_QWORD
			pType = "QWORD";
			break;
#endif
		case REG_DWORD:
			pType = "DWORD";
			{
				DWORD dwVal;
				dwLen = sizeof(dwVal);
				if (ERROR_SUCCESS == RegQueryValueEx(hKey, g_szBuff, NULL, &type, (LPBYTE)&dwVal, &dwLen))
				{
					bOk = true;
					PRINT_INDENT
					fputs(g_szBuff, output);
					fputs("(", output);
					fputs(pType, output);
					fputs("): ", output);
					fprintf(output, "%08x\n", dwVal);
				}
			}
			break;
		case REG_EXPAND_SZ:
		case REG_SZ:
			if (REG_EXPAND_SZ == type)
				pType = "EXPAND_SZ";
			else
				pType = "SZ";
			{
				dwLen = BUFF_SIZE;
				DWORD dwRet = RegQueryValueEx(hKey, g_szBuff, NULL, &type, reinterpret_cast<LPBYTE>(g_szBuff2), &dwLen);
				if (ERROR_SUCCESS == dwRet || ERROR_MORE_DATA == dwRet)
				{
					bOk = true;
					PRINT_INDENT
					fputs(g_szBuff, output);
					fputs("(", output);
					fputs(pType, output);
					fputs("): ", output);
					fputs(g_szBuff2, output);
					if (ERROR_MORE_DATA == dwRet)
						fputs("...", output);
					fputs("\n", output);
				}
			}
			break;
		}
		if (!bOk)
		{
			PRINT_INDENT
			fputs(g_szBuff, output);
			fputs("(", output);
			fputs(pType, output);
			fputs(")\n", output);
		}
	}
}


/**
 * This function is passed to SetUnhandledExceptionFilter().
 * Note, this function will NOT be called if you are running under a debugger.
 */
LONG WINAPI CrashHandler(LPEXCEPTION_POINTERS pExPtrs)
{
	if (!g_bSymEngInit)
		return EXCEPTION_CONTINUE_SEARCH;

	LPCTSTR pReason = GetFaultReason(pExPtrs);

	// Create the log file.
	{
		GetModuleFileName(NULL, g_szBuff2, BUFF_SIZE);
		int len = lstrlen(g_szBuff2);
		if (5 > len)
		{
			AfxMessageBox(pReason, MB_ICONSTOP);
			return EXCEPTION_CONTINUE_SEARCH;
		}
		lstrcpy(&g_szBuff2[len-3], "log");
	}
#if _MSC_VER >= 1400
	FILE* output = NULL;
	if (0 != fopen_s(&output, g_szBuff2, "a+"))
	{
		if (output)
		{
			fclose(output);
			output = NULL;
		}
	}
#else
	FILE* output = fopen(g_szBuff2, "a+");
#endif

	// Need to copy pReason since it's sitting in a global buffer.
	// We don't display it until after we've written the log.
	static TCHAR reasonBuffer[BUFF_SIZE+MAX_PATH+30];
	lstrcpy(reasonBuffer, pReason);

	// Now dump the stack.
	if (output)
	{
		// Add some info to display.
		lstrcat(reasonBuffer, "\n\nLog written at ");
		lstrcat(reasonBuffer, g_szBuff2);

		time_t t;
		time(&t);
#if _MSC_VER < 1400
		struct tm* local = localtime(&t);
#else
		struct tm l;
		_localtime64_s(&l, &t);
		struct tm* local = &l;
#endif
		_tcsftime(g_szBuff2, MAX_PATH, _T("%#c"), local);
		fputs("\n============================================================\n", output);
		fputs(g_szBuff2, output);
		fputs("\n\n", output);
		// Write the reason
		fputs(pReason, output);
		fputs("\n\n", output);
		// Now re-use the variable
		for (pReason = GetFirstStackTraceString(pExPtrs);
			pReason;
			pReason = GetNextStackTraceString(pExPtrs))
		{
			fputs(pReason, output);
			fputs("\n", output);
		}
		// Since we are handling an unhandled exception, there is a very
		// real danger that the application is in an extremely unstable
		// state. So dump the registry settings last just in case we die.
		fputs("\nCurrent registry settings:\n", output);
		QueryKey(output, g_hAppKey, 1);
		fclose(output);
	}

	AfxMessageBox(reasonBuffer, MB_ICONSTOP);
	return EXCEPTION_EXECUTE_HANDLER;
}


bool InitCrashHandler(HKEY hAppKey)
{
	if (!g_bSymEngInit)
	{
		HINSTANCE hInst = LoadLibraryA("DBGHELP.DLL");
		assert(NULL != hInst);
		if (NULL == hInst)
		{
			TRACE0("Unable to load DBGHELP.DLL!\n");
			SetLastErrorEx(ERROR_DLL_INIT_FAILED, SLE_ERROR);
			return false;
		}
		g_pfnSymGetLineFromAddr = reinterpret_cast<PFNSYMGETLINEFROMADDR64>(GetProcAddress(GetModuleHandle(_T("IMAGEHLP.DLL")), "SymGetLineFromAddr64"));
		// Now do the GetProcAddress stuff.
		g_SymGetOptions = reinterpret_cast<PFNSYMGETOPTIONS>(GetProcAddress(hInst, "SymGetOptions"));
		g_SymSetOptions = reinterpret_cast<PFNSYMSETOPTIONS>(GetProcAddress(hInst, "SymSetOptions"));
		g_SymInitialize = reinterpret_cast<PFNSYMINITIALIZE>(GetProcAddress(hInst, "SymInitialize"));
		g_SymGetModuleBase = reinterpret_cast<PGET_MODULE_BASE_ROUTINE64>(GetProcAddress(hInst, "SymGetModuleBase64"));
		g_SymGetSymFromAddr = reinterpret_cast<PFNSYMGETSYMFROMADDR64>(GetProcAddress(hInst, "SymGetSymFromAddr64"));
		g_SymFunctionTableAccess = reinterpret_cast<PFNSYMFUNCTIONTABLEACCESS64>(GetProcAddress(hInst, "SymFunctionTableAccess64"));
		g_StackWalk = reinterpret_cast<PFNSTACKWALK64>(GetProcAddress(hInst, "StackWalk64"));

		// Set up the symbol engine.
		DWORD dwOpts = g_SymGetOptions();
		// Always defer loading to make life faster.
		g_SymSetOptions(dwOpts | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
		// Initialize the symbol engine.
		VERIFY(g_SymInitialize(GetCurrentProcess(), NULL, TRUE));
		g_bSymEngInit = true;
	}
	g_hAppKey = hAppKey;
	SetUnhandledExceptionFilter(CrashHandler);
	return true;
}


bool CleanupCrashHandler()
{
	if (g_hAppKey != NULL)
	{
		RegCloseKey(g_hAppKey);
		g_hAppKey = NULL;
	}
	return true;
}


#else
bool InitCrashHandler(HKEY hAppKey)
{
	return false;
}
bool CleanupCrashHandler()
{
	return true;
}
#endif
