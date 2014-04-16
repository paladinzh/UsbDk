#include "stdafx.h"
#include "DriverFile.h"

//------------------------------------------------------------------------------------------------

UsbDkDriverFile::UsbDkDriverFile(LPCTSTR lpFileName)
{
    m_hDriver = CreateFile(lpFileName,
                           GENERIC_READ | GENERIC_WRITE,
                           0,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);

    if (m_hDriver == INVALID_HANDLE_VALUE)
    {
        throw UsbDkDriverFileException(tstring(TEXT("Failed to open device symlink ")) + lpFileName);
    }
}
//------------------------------------------------------------------------------------------------

bool UsbDkDriverFile::Ioctl(DWORD Code,
                            bool ShortBufferOk,
                            LPVOID InBuffer,
                            DWORD InBufferSize,
                            LPVOID OutBuffer,
                            DWORD OutBufferSize,
                            LPDWORD BytesReturned,
                            LPOVERLAPPED Overlapped)
{
    DWORD InternalBytesReturned;
    LPDWORD InternalBytesReturnedPtr = (BytesReturned != nullptr) ? BytesReturned : &InternalBytesReturned;
    if (!DeviceIoControl(m_hDriver, Code,
                         InBuffer, InBufferSize,
                         OutBuffer, OutBufferSize,
                         InternalBytesReturnedPtr, Overlapped))
    {
        if (ShortBufferOk)
        {
            DWORD err = GetLastError();
            if (err == ERROR_INSUFFICIENT_BUFFER || err == ERROR_MORE_DATA)
            {
                return false;
            }
        }

        throw UsbDkDriverFileException(TEXT("IOCTL failed"));
    }

    return true;
}