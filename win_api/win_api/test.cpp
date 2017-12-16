#include <windows.h>
#include <stdio.h>

/*
测试创建文件，写文件
*/
int test()
{
	printf("hello world\n");

	HANDLE hFILE = CreateFile("I:/1.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 
	//HANDLE hFILE = CreateFile("\\\\.\\HelloDDK", GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
	if(hFILE==INVALID_HANDLE_VALUE)  
	{  
		printf("CreateFile error\n");  
		return 0;  
	} 


	if(SetFilePointer(hFILE,0,NULL,FILE_END)==-1)  
	{  
		printf("SetFilePointer error\n");  
		return 0;  
	}  

	char buff[256]="hello"; 
	DWORD dwWrite;  
	if(!WriteFile(hFILE,&buff,strlen(buff),&dwWrite,NULL))  
	{  
		printf("WriteFile error\n");  
		return 0;  
	}  
	printf("write %d.\n",dwWrite);  
	printf("done.\n");  

	CloseHandle(hFILE);  

	return 0;
}

int test2()
{
	HANDLE hFILE = CreateFile("\\\\.\\HelloDDK",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
	if(hFILE==INVALID_HANDLE_VALUE)  
	{  
		printf("CreateFile error: %d \n", GetLastError());  
		return 0;  
	}

	UCHAR buffer[10];

	ULONG ulRead;

	//对设备读写

	BOOL bRet = ReadFile(hFILE,buffer,10,&ulRead,NULL);

	if(bRet)

	{

		printf("Read %d bytes:",ulRead);
		for(int i=0;i<(int)ulRead;i++)
		{
			printf("%02X ",buffer[i]);
		}
	}

	printf("\n success \n");
	CloseHandle(hFILE);
	return 0;
}


int main()
{
	test2();
	return 0;
}