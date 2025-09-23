#pragma once


typedef const UNICODE_STRING* PCUNICODE_STRING;

typedef HWND (NTAPI* NtUserFindWindowEx)(
    _In_opt_ HWND hwndParent,
    _In_opt_ HWND hwndChild,
    _In_ PCUNICODE_STRING ClassName,
    _In_ PCUNICODE_STRING WindowName,
    _In_ ULONG Type
    );

