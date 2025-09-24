#pragma once

typedef struct _STRING
{
    USHORT Length;
    USHORT MaximumLength;
    _Field_size_bytes_part_opt_(MaximumLength, Length) PCHAR Buffer;
} STRING, * PSTRING, ANSI_STRING, * PANSI_STRING, OEM_STRING, * POEM_STRING;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    _Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef const ANSI_STRING* PCANSI_STRING;

typedef const UNICODE_STRING* PCUNICODE_STRING;
typedef VOID(NTAPI* PRtlInitAnsiString)(PANSI_STRING DestinationString, PCSTR SourceString);
typedef VOID(NTAPI* PRtlAnsiStringToUnicodeString)(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString);
typedef VOID(NTAPI* PRtlFreeUnicodeString)(PUNICODE_STRING UnicodeString);
typedef NTSTATUS(NTAPI* NtClose)(_In_ _Post_ptr_invalid_ HANDLE Handle);
typedef BOOL(NTAPI* NtUserDestroyWindow)(_In_ HWND WindowHandle);


typedef HWND (NTAPI* NtUserFindWindowEx)(
    _In_opt_ HWND hwndParent,
    _In_opt_ HWND hwndChild,
    _In_ PCUNICODE_STRING ClassName,
    _In_ PCUNICODE_STRING WindowName,
    _In_ ULONG Type
    );

typedef BOOL(NTAPI* NtUserSetWindowPos)(
        _In_ HWND WindowHandle,
        _In_opt_ HWND WindowHandleInsertAfter,
        _In_ LONG X,
        _In_ LONG Y,
        _In_ LONG cx,
        _In_ LONG cy,
        _In_ ULONG Flags
    );




