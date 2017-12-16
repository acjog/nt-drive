#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <ntddk.h>

#ifdef __cplusplus  
  
extern "C"  
  
{  
  
#endif  
  


#include <NTDDK.h>  
  
#ifdef __cplusplus  
  
}  
  
#endif   
  
  
  
#define PAGEDCODE code_seg("PAGE")  
  
#define LOCKEDCODE code_seg()  
  
#define INITCODE code_seg("INIT")  
  
  
  
#define PAGEDDATA data_seg("PAGE")  
  
#define LOCKEDDATA data_seg()  
  
#define INITDATA data_seg("INIT")  
  

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))  
  
  
typedef struct _DEVICE_EXTENSION {  
    PDEVICE_OBJECT pDevice;  
    UNICODE_STRING ustrDeviceName;    //设备名称  
    UNICODE_STRING ustrSymLinkName;    //符号链接名  
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;  
 

NTSTATUS CreateDevice (  IN PDRIVER_OBJECT    pDriverObject ); 



#define LOCATION __FILE__,__LINE__, __FUNCTION__
#define zb_debug() Dump("zb (%s:%d %s) \n",LOCATION)

#endif
