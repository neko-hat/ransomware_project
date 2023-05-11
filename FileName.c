#include <stdio.h>
#include <windows.h>

typedef unsigned __int64 QWORD;

int main(void)
{
    HKEY key;
    DWORD dwDisposition;
    DWORD dwData;
    BYTE rgbData[sizeof(QWORD)];

    srand((unsigned int)time(NULL));
    for (int i = 0; i < sizeof(QWORD); i++) {
        rgbData[i] = (BYTE)rand();
    }
    
    
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\Background", 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS)
    {
        printf("Key location open successful \n");
        dwData = sizeof(QWORD);
        if (RegSetValueExW(key, L"TEST", 0, REG_QWORD, rgbData, dwData) == ERROR_SUCCESS)
        {
            printf("Key changed in registry \n");
        }
        else {
            printf("Key not changed in registry \n");
            printf("Error %u ", (unsigned int)GetLastError());
        }
        RegCloseKey(key);
    }
    else
    {
        printf("Unsuccessful in opening key  \n");
        printf("Cannot find key value in registry \n");
        printf("Error: %u ", (unsigned int)GetLastError());
    }

    return 0;
}