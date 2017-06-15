#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#define WIN32_LEAN_AND_MEAN

#define DEBUG printf("DEBUG - %ld\r\n", GetLastError());

DWORD dwError;
#define DEFAULT_EXIT "Press any key to continue..\r\n"
#define SW(x) SysWait(x);
VOID SysWait(LPTSTR lpString);

#define SET_ERROR_HANDLER dwError = GetLastError();
#define RESET_ERROR_HANDLER dwError = ERROR_SUCCESS;

CHAR tVmProcesses[0x07][0x18] = {"vmsrvc.exe", "vmusrv.exe", "vmtoolsd.exe"
                                 "df5serv.exe", "vboxtray.exe", "vboxservice.exe"};

BOOL NtVmProcessSignature(PPROCESSENTRY32 Pe32);

INT __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
     HANDLE hProcessImage;
     PROCESSENTRY32 Pe32;
     BOOL bFlag;
     RESET_ERROR_HANDLER
     
     RtlZeroMemory(&Pe32, sizeof(PROCESSENTRY32));
     
     hProcessImage = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
     if(hProcessImage == INVALID_HANDLE_VALUE)
          goto FAILURE;
     else
          Pe32.dwSize = sizeof(PROCESSENTRY32);
    
     if(!Process32First(hProcessImage, &Pe32))
          goto FAILURE;
          
     while(Process32Next(hProcessImage, &Pe32))
     {
          bFlag = TRUE;
          if(NtVmProcessSignature(&Pe32))
              break;
          else
               bFlag = FALSE;      
     }
     
     if(bFlag)
          printf("Process %s detected.\r\n", Pe32.szExeFile);
     else
          printf("No VM detected.\r\n");
    
     if(hProcessImage)
          CloseHandle(hProcessImage);

     SW(DEFAULT_EXIT)
     return ERROR_SUCCESS;
    
    
FAILURE:

     SET_ERROR_HANDLER
     printf("Failed to enumerate process snap shot %ld\r\n", dwError);
     
     if(hProcessImage)
          CloseHandle(hProcessImage);
     
     SW(DEFAULT_EXIT)
     return GetLastError();
}

VOID SysWait(LPTSTR lpString)
{
     printf("%s", lpString);
     dwError = getch();
}

BOOL NtVmProcessSignature(PPROCESSENTRY32 Pe32)
{
     RESET_ERROR_HANDLER
     
     for(;dwError < 0x07; dwError++)
     {
          if((_stricmp(Pe32->szExeFile, tVmProcesses[dwError]) == ERROR_SUCCESS))
               return TRUE; 
     }
     
     return FALSE; 
}
