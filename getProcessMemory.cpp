#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
//BOOL CALLBACK EnumChildWindowProc(HWND hWnd,LPARAM lParam);//枚举记事本中的子窗口
const int N=50*1024*1024;
//提升debug权限
void EnableDebugPriv()
{
	HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 
    
    OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES  | TOKEN_QUERY, &hToken); 
    
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid); 
    
    tkp.PrivilegeCount = 1; 
    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
    
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0); 
    
    CloseHandle( hToken ); 	
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
    //HWND nphWnd=::FindWindow("notepad",NULL);
    //if(nphWnd)
		FILE *fp ;
		//FILE *gp ;
		fp = fopen ("hProcess.txt","w");
		//gp = fopen ("hProcess.txt","w");
        char *temp=new char[N];
		//char mess[999999];
        PROCESSENTRY32 pe32;
        pe32.dwSize=sizeof(pe32);
        HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//获得进程列表的快照，第一个参数可以有其他选项，详细请参考MSDN
        if(hProcessSnap==INVALID_HANDLE_VALUE)
        {
            ::MessageBox(NULL,"CreateToolhelp32Snapshot error","error",MB_OK);
            return 0;
        }
        HANDLE hProcess;
        BOOL bMore=::Process32First(hProcessSnap,&pe32);//获得第一个进程的信息
        while(bMore)
        {
            ::wsprintf(temp,"%s",pe32.szExeFile);
            if(!::strcmp(temp,"iexplore.exe"))
            {
                hProcess=::OpenProcess(PROCESS_ALL_ACCESS,false,(DWORD)pe32.th32ProcessID);
                if(hProcess==NULL)
                {
                    ::wsprintf(temp,"%s","打开进程失败!\n");
					fprintf (fp,"打开进程失败!");
                    //::strcat(mess,temp);
                }
                else
                {
                    ::wsprintf(temp,"%s","打开进程成功!");
					fprintf (fp,"打开进程成功!\n");
                    //::strcat(mess,temp);
                    //读取内存中内容
                    int tmp;
                    DWORD dwNumberOfBytesRead;
					
					EnableDebugPriv();//提权

					HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,(DWORD)pe32.th32ProcessID); 
					if (hSnapshot == INVALID_HANDLE_VALUE)
					{
						CloseHandle(hSnapshot);
					}
					MODULEENTRY32 me;
					me.dwSize = sizeof(MODULEENTRY32);
					Module32First(hSnapshot,&me);

                    if( !::ReadProcessMemory(hProcess,(LPCVOID)me.modBaseAddr,temp,me.modBaseSize,&dwNumberOfBytesRead) )//me.modBaseAddr
                    {
						int a = GetLastError();
                        ::wsprintf(temp,"%s","读取失败");
						fprintf (fp,"读取失败!\n");
                        //::strcat(mess,temp);
                    }
                    else
                    {
						int i = 0;
						while (i < me.modBaseSize) {
							fprintf (fp,"%c",temp[i]);
							i++;
						}
						fprintf (fp,"\n");
                        //::strcat(mess,temp);
                    }
					while(Module32Next(hSnapshot,&me))//继续读该进程模块中的其他模块
					{
						
						if( !::ReadProcessMemory(hProcess,(LPCVOID)me.modBaseAddr,temp,me.modBaseSize,&dwNumberOfBytesRead) )//me.modBaseAddr
						{
							int a = GetLastError();
							::wsprintf(temp,"%s","读取失败");
							fprintf (fp,"读取失败!\n");
							//::strcat(mess,temp);
						}
						else
						{
							int i = 0;
							while (i < me.modBaseSize) {
								fprintf (fp,"%c",temp[i]);
								i++;
							}
							//::strcat(mess,temp);
							fprintf (fp,"\n");
						}
					}
					CloseHandle(hSnapshot);
					/*if( !::ReadProcessMemory(hProcess,(LPCVOID)hProcess,mess,999999,&dwNumberOfBytesRead) )
                    {
						int a = GetLastError();
                        ::wsprintf(temp,"%s","读取失败");
                        ::strcat(mess,temp);
                    }
                    else
                    {
						int i = 0;
						while (i <= 999999) {
							fprintf (gp,"%c",mess[i]);
							i++;
						}
                        ::strcat(mess,temp);
                    }*/

                }
                break;
            }
            bMore=::Process32Next(hProcessSnap,&pe32);//获得其他进程信息
        }
		fclose(fp);
        //::EnumChildWindows(nphWnd,EnumChildWindowProc,0);//获得记事本的edit窗口，打印进程信息
        return 0;
    
    /*else
    {
        ::MessageBox(NULL,"please open notepad","error",MB_OK);
        return 0;
    }*/
}
/*BOOL CALLBACK EnumChildWindowProc(HWND hWnd,LPARAM lParam)
{
    char temp1[256];
    if(hWnd)
    {
        ::GetClassName(hWnd,temp1,255);
        if(!::strcmp(temp1,"Edit"))//得到edit子窗口句柄
        {
            ::SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)mess);
            return 0;
        }
    }
    return true;
}*/
