#include "driver.h"
extern "C" VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject,PUNICODE_STRING pRegistryPath)//驱动入口
{
	NTSTATUS status;  
	DbgPrint("DriverEntry: %s \r\n", pRegistryPath);

	pDriverObject->DriverUnload = DriverUnload;//注册卸载函数
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchRoutine;//注册派遣函数
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRoutine;
	status = CreateDevice(pDriverObject);  
	return status;
}

NTSTATUS CreateDevice (IN PDRIVER_OBJECT    pDriverObject)   
{  

	NTSTATUS status;  
	PDEVICE_OBJECT pDevObj;  
	PDEVICE_EXTENSION pDevExt;  
	UNICODE_STRING symLinkName;     

	//创建设备名称  
	UNICODE_STRING devName;  

	RtlInitUnicodeString(&devName,L"\\Device\\MyDDKDevice");

	//创建设备  
	status = IoCreateDevice( pDriverObject,  

		sizeof(DEVICE_EXTENSION),  

		&devName,  

		FILE_DEVICE_UNKNOWN,//此种设备为独占设备FILE_DEVICE_DISK

		FILE_DEVICE_SECURE_OPEN | FILE_READ_ONLY_DEVICE , TRUE,  

		&pDevObj );  

	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO; 

	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;  

	pDevExt->pDevice = pDevObj;  

	pDevExt->ustrDeviceName = devName;  

	// 创建符号链接  
	// 这样创建的符号链接，符号链接在GLOBAL??地址， 虽然ntname在device里面，但是并没有显示该符号链接
	RtlInitUnicodeString(&symLinkName,L"\\??\\HelloDDK"); 
	//RtlInitUnicodeString(&symLinkName,L"\\DosDevices\\I:");

	pDevExt->ustrSymLinkName = symLinkName;

	status = IoCreateSymbolicLink( &symLinkName,&devName );  

	if (!NT_SUCCESS(status))   

	{  
		DbgPrint(" IoCreateSymbolicLink devname: %s  symname: %s error \r\n", devName, symLinkName, symLinkName );

		IoDeleteDevice( pDevObj );  

		return status;  

	}

	return STATUS_SUCCESS;
}  


extern "C" VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	//遍历系统中所有的此类设备对象，删除设备对象及其符号链接  

	PDEVICE_OBJECT    pNextObj;  

	KdPrint(("Enter DriverUnload\n"));  

	pNextObj = pDriverObject->DeviceObject;  

	while (pNextObj != NULL)   

	{  

		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;  

		//删除符号链接  

		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;  

		IoDeleteSymbolicLink(&pLinkName);  

		pNextObj = pNextObj->NextDevice;  

		IoDeleteDevice( pDevExt->pDevice );  

	}  
}

extern "C" NTSTATUS DispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	int  flag = 0;
	ULONG ulReadLength;
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation (pIrp);

	KdPrint((" DispatchRoutine \n")); 
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	//wchar_t majorfun[64];
	char majorfun[64];

	switch (irpSp->MajorFunction)
	{
	case IRP_MJ_CLOSE:
		{
			strcpy(majorfun, "IRP_MJ_CLOSE");
			break;
		}
	case IRP_MJ_CREATE:
		{
			strcpy(majorfun, "IRP_MJ_CREATE");
			break;
		}
	case IRP_MJ_SHUTDOWN:
		{
			strcpy(majorfun, "IRP_MJ_SHUTDOWN");
			break;
		}
	case IRP_MJ_READ:
		{
			strcpy(majorfun, "IRP_MJ_READ");
			ulReadLength = irpSp->Parameters.Read.Length;

			//完成IRP
			pIrp->IoStatus.Status = status;

			//设置IRP操作了多少字节

			pIrp->IoStatus.Information = ulReadLength;

			//设置内核模式下的缓冲区

			memset(pIrp->AssociatedIrp.SystemBuffer,0xAA,ulReadLength);

			//处理IRP
			break;
		}
	case IRP_MJ_WRITE:
		{
			//wcscpy(majorfun, "IRP_MJ_WRITE");
			strcpy(majorfun, "IRP_MJ_WRITE");
			break;
		}
	case IRP_MJ_DEVICE_CONTROL:
		{
			strcpy(majorfun, "IRP_MJ_DEVICE_CONTROL");
			break;
		}
	case IRP_MJ_FLUSH_BUFFERS:
		{
			strcpy(majorfun, "IRP_MJ_FLUSH_BUFFERS");
			break;
		}
	default:
		{
			flag = 1;
			break;
		}
	}
	if (0 == flag)
	{
		DbgPrint("irp %s \n", majorfun);
	}
	else
	{
		DbgPrint("irp not in debugprint, irp: %d \n", irpSp->MajorFunction);
	}

	IoCompleteRequest(pIrp, IO_NO_INCREMENT); //完成IRP请求
	return status;
}
