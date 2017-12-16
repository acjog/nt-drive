#include "driver.h"
extern "C" VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject,PUNICODE_STRING pRegistryPath)//�������
{
	NTSTATUS status;  
	DbgPrint("DriverEntry: %s \r\n", pRegistryPath);

	pDriverObject->DriverUnload = DriverUnload;//ע��ж�غ���
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchRoutine;//ע����ǲ����
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

	//�����豸����  
	UNICODE_STRING devName;  

	RtlInitUnicodeString(&devName,L"\\Device\\MyDDKDevice");

	//�����豸  
	status = IoCreateDevice( pDriverObject,  

		sizeof(DEVICE_EXTENSION),  

		&devName,  

		FILE_DEVICE_UNKNOWN,//�����豸Ϊ��ռ�豸FILE_DEVICE_DISK

		FILE_DEVICE_SECURE_OPEN | FILE_READ_ONLY_DEVICE , TRUE,  

		&pDevObj );  

	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO; 

	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;  

	pDevExt->pDevice = pDevObj;  

	pDevExt->ustrDeviceName = devName;  

	// ������������  
	// ���������ķ������ӣ�����������GLOBAL??��ַ�� ��Ȼntname��device���棬���ǲ�û����ʾ�÷�������
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
	//����ϵͳ�����еĴ����豸����ɾ���豸�������������  

	PDEVICE_OBJECT    pNextObj;  

	KdPrint(("Enter DriverUnload\n"));  

	pNextObj = pDriverObject->DeviceObject;  

	while (pNextObj != NULL)   

	{  

		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;  

		//ɾ����������  

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

			//���IRP
			pIrp->IoStatus.Status = status;

			//����IRP�����˶����ֽ�

			pIrp->IoStatus.Information = ulReadLength;

			//�����ں�ģʽ�µĻ�����

			memset(pIrp->AssociatedIrp.SystemBuffer,0xAA,ulReadLength);

			//����IRP
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

	IoCompleteRequest(pIrp, IO_NO_INCREMENT); //���IRP����
	return status;
}
