#include "ArkTools.h"

typedef struct _OBJECT_CREATE_INFORMATION
{
    ULONG Attributes;
    HANDLE RootDirectory;
    PVOID ParseContext;
    KPROCESSOR_MODE ProbeMode;
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    ULONG SecurityDescriptorCharge;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;
} OBJECT_CREATE_INFORMATION, *POBJECT_CREATE_INFORMATION;

typedef struct _OBJECT_HEADER
{
    LONG PointerCount;
    union
    {
        LONG HandleCount;
        PSINGLE_LIST_ENTRY SEntry;
    };
    POBJECT_TYPE Type;
    UCHAR NameInfoOffset;
    UCHAR HandleInfoOffset;
    UCHAR QuotaInfoOffset;
    UCHAR Flags;
    union
    {
        POBJECT_CREATE_INFORMATION ObjectCreateInfo;
        PVOID QuotaBlockCharged;
    };

    PSECURITY_DESCRIPTOR SecurityDescriptor;
    QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

#define NUMBER_HASH_BUCKETS 37
typedef struct _OBJECT_DIRECTORY
{
    struct _OBJECT_DIRECTORY_ENTRY* HashBuckets[NUMBER_HASH_BUCKETS];
    struct _OBJECT_DIRECTORY_ENTRY** LookupBucket;
    BOOLEAN LookupFound;
    USHORT SymbolicLinkUsageCount;
    struct _DEVICE_MAP* DeviceMap;
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

typedef struct _OBJECT_HEADER_NAME_INFO
{
    POBJECT_DIRECTORY Directory;
    UNICODE_STRING Name;
    ULONG Reserved;
#if DBG
    ULONG Reserved2;
    LONG DbgDereferenceCount;
#endif
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;

#define OBJECT_TO_OBJECT_HEADER( o ) \
    CONTAINING_RECORD( (o), OBJECT_HEADER, Body )

#define OBJECT_HEADER_TO_NAME_INFO( oh ) ((POBJECT_HEADER_NAME_INFO) \
    ((oh)->NameInfoOffset == 0 ? NULL : ((PCHAR)(oh) - (oh)->NameInfoOffset)))

void GetDeviceName(PDEVICE_OBJECT pDeviceObj)
{
    POBJECT_HEADER ObjectHeader;
    POBJECT_HEADER_NAME_INFO ObjectNameInfo;

    DbgPrint("pDeviceObj:%p\n", pDeviceObj);

    if (pDeviceObj == NULL)
    {
        DbgPrint("pDeviceObj is NULL!\n");
        return;
    }
    // 得到对象头
    ObjectHeader = OBJECT_TO_OBJECT_HEADER(pDeviceObj);

    DbgPrint("OBJECT_TO_OBJECT_HEADER:%p\n", ObjectHeader);


    if (ObjectHeader)
    {
        // 查询设备名称并打印
        ObjectNameInfo = OBJECT_HEADER_TO_NAME_INFO(ObjectHeader);

        DbgPrint("ObjectNameInfo:%p\n", ObjectNameInfo);

        if (ObjectNameInfo && ObjectNameInfo->Name.Buffer)
        {
            DbgPrint("Driver Name:%wZ - Device Name:%wZ - Driver Address:0x%x - Device Address:0x%x\n",
                &pDeviceObj->DriverObject->DriverName,
                &ObjectNameInfo->Name,
                pDeviceObj->DriverObject,
                pDeviceObj);
        }

        // 对于没有名称的设备，则打印 NULL
        else if (pDeviceObj->DriverObject)
        {
            DbgPrint("Driver Name:%wZ - Device Name:%S - Driver Address:0x%x - Device Address:0x%x\n",
                &pDeviceObj->DriverObject->DriverName,
                L"NULL",
                pDeviceObj->DriverObject,
                pDeviceObj);
        }
    }
}