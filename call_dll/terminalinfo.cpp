#include "StdAfx.h"
#include "terminalinfo.h"
#include "checkvm.h"
#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>                     //进行IPPROTO_IP级别设置时用到  
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <Urlmon.h>
#include <cstdlib>
#include <ctime>
#include <set>
#pragma comment (lib, "Iphlpapi.lib" )
#pragma comment(lib,"netapi32.lib")
#pragma comment(lib, "WS2_32")
#pragma comment(lib,"urlmon.lib")

#else
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#define PRINT_DEBUG     (0)

typedef struct _icmphdr                   //ICMP头部定义，被封装在IP包中  
{  
	BYTE   i_type;                        //报文类型  
	BYTE   i_code;                        //代码  
	USHORT i_cksum;                       //校验和  
	USHORT i_id;                          //标识符  
	USHORT i_seq;                         //序号    
}IcmpHeader;  

#define ICMP_ECHO       8                 //发送Ping请求时的ICMP报文类型  
#define ICMP_ECHOREPLY  0                 //接收Ping回复时的ICMP报文类型  
#define ICMP_TIMEOUT    11                //ICMP超时报文类型  
#define ICMP_MIN        8                 //Minimum 8-byte ICMP packet (header)  
#pragma pack(1)

#define  IDENTIFY_BUFFER_SIZE  512

//  IOCTL commands
#define  DFP_GET_VERSION                0x00074080
#define  DFP_SEND_DRIVE_COMMAND         0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA         0x0007c088

#define  FILE_DEVICE_SCSI               0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY   ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT            0x0004D008  //  see NTDDSCSI.H for definition

#define SMART_GET_VERSION               CTL_CODE(IOCTL_DISK_BASE, 0x0020, METHOD_BUFFERED, FILE_READ_ACCESS)
#define SMART_SEND_DRIVE_COMMAND        CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)


//typedef struct _GETVERSIONINPARAMS {
//	UCHAR    bVersion;               // Binary driver version.
//	UCHAR    bRevision;              // Binary driver revision.
//	UCHAR    bReserved;              // Not used.
//	UCHAR    bIDEDeviceMap;          // Bit map of IDE devices.
//	ULONG   fCapabilities;          // Bit mask of driver capabilities.
//	ULONG   dwReserved[4];          // For future use.
//} GETVERSIONINPARAMS, *PGETVERSIONINPARAMS, *LPGETVERSIONINPARAMS;


//  GETVERSIONOUTPARAMS contains the data returned from the 
//  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;


//  Bits returned in the fCapabilities member of GETVERSIONOUTPARAMS 
#define  CAP_IDE_ID_FUNCTION             1  // ATA ID command supported
#define  CAP_IDE_ATAPI_ID                2  // ATAPI ID command supported
#define  CAP_IDE_EXECUTE_SMART_FUNCTION  4  // SMART commannds supported


////  IDE registers
//typedef struct _IDEREGS
//{
//	BYTE bFeaturesReg;       // Used for specifying SMART "commands".
//	BYTE bSectorCountReg;    // IDE sector count register
//	BYTE bSectorNumberReg;   // IDE sector number register
//	BYTE bCylLowReg;         // IDE low order cylinder value
//	BYTE bCylHighReg;        // IDE high order cylinder value
//	BYTE bDriveHeadReg;      // IDE drive/head register
//	BYTE bCommandReg;        // Actual IDE command.
//	BYTE bReserved;          // reserved for future use.  Must be zero.
//} IDEREGS, *PIDEREGS, *LPIDEREGS;


//  SENDCMDINPARAMS contains the input parameters for the 
////  Send Command to Drive function.
//typedef struct _SENDCMDINPARAMS
//{
//	DWORD     cBufferSize;   //  Buffer size in bytes
//	IDEREGS   irDriveRegs;   //  Structure with drive register values.
//	BYTE bDriveNumber;       //  Physical drive number to send 
//	//  command to (0,1,2,3).
//	BYTE bReserved[3];       //  Reserved for future expansion.
//	DWORD     dwReserved[4]; //  For future use.
//	BYTE      bBuffer[1];    //  Input buffer.
//} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;


//  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.


//// Status returned from driver
//typedef struct _DRIVERSTATUS
//{
//	BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
//	BYTE  bIDEStatus;    //  Contents of IDE Error register.
//	//  Only valid when bDriverError is SMART_IDE_ERROR.
//	BYTE  bReserved[2];  //  Reserved for future expansion.
//	DWORD  dwReserved[2];  //  Reserved for future expansion.
//} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;


//// Structure returned by PhysicalDrive IOCTL for several commands
//typedef struct _SENDCMDOUTPARAMS
//{
//	DWORD         cBufferSize;   //  Size of bBuffer in bytes
//	DRIVERSTATUS  DriverStatus;  //  Driver status structure.
//	BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
//} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;


// The following struct defines the interesting part of the IDENTIFY
// buffer:
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;


typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;


// Define global buffers.
BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];


//  Max number of drives assuming primary/secondary, master/slave topology
#define  MAX_IDE_DRIVES  16


int  CTerminalinfo::ReadPhysicalDriveInNTWithAdminRights (char cSysDrive)
{
	int done = FALSE;
	int drive = 0;

    //for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		//sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);
		sprintf (driveName, "\\\\.\\%c:", cSysDrive);

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFileA (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE , NULL,
			OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
			strcpy(m_szTerErrinf[eHD],"permission denied");
#ifdef PRINTING_TO_CONSOLE_ALLOWED
			if (PRINT_DEBUG) 
				printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
				"\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n",
				__LINE__, driveName);
#endif
		}
		else
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) &VersionParams, 0, sizeof(VersionParams));

			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL) )
			{         
				strcpy(m_szTerErrinf[eHD],"unknown instruction");
#ifdef PRINTING_TO_CONSOLE_ALLOWED
				if (PRINT_DEBUG)
				{
					DWORD err = GetLastError ();
					printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
						"\nDeviceIoControl(%d, DFP_GET_VERSION) returned 0, error is %d\n",
						__LINE__, (int) hPhysicalDriveIOCTL, (int) err);
				}
#endif
			}

			// If there is a IDE device at number "i" issue commands
			// to the device
			if (VersionParams.bIDEDeviceMap <= 0)
			{
#ifdef PRINTING_TO_CONSOLE_ALLOWED
				if (PRINT_DEBUG)
					printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
					"\nNo device found at position %d (%d)\n",
					__LINE__, (int) drive, (int) VersionParams.bIDEDeviceMap);
#endif
			}
			else
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;
				//SENDCMDOUTPARAMS OutCmd;

				// Now, get the ID sector for all IDE devices in the system.
				// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
				// otherwise use the IDE_ATA_IDENTIFY command
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset (&scip, 0, sizeof(scip));
				memset (IdOutCmd, 0, sizeof(IdOutCmd));

				if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
					&scip, 
					(PSENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					DWORD diskdata [256];
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)
						((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

					for (ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					PrintIdeInfo (drive, diskdata);

					done = (drive / 2==0 && drive % 2==0);
					
				}
			}

			CloseHandle (hPhysicalDriveIOCTL);
		}
		//if(done) break;
	}

	return done;
}


//
// IDENTIFY data (from ATAPI driver source)
//

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid:1;        // 6A  53
	USHORT Reserved3:15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;


#pragma pack()


int CTerminalinfo::ReadPhysicalDriveInNTUsingSmart (char cSysDrive)
{
	int done = FALSE;
	int drive = 0;

    //for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		sprintf (driveName, "\\\\.\\%c:", cSysDrive);
		

		//  Windows NT, Windows 2000, Windows Server 2003, Vista
		hPhysicalDriveIOCTL = CreateFileA (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
#ifdef PRINTING_TO_CONSOLE_ALLOWED
			if (PRINT_DEBUG) 
				printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
				"\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n"
				"Error Code %d\n",
				__LINE__, driveName, GetLastError ());
#endif
		}
		else
		{
			GETVERSIONINPARAMS GetVersionParams;
			DWORD cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));

			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
				NULL, 
				0,
				&GetVersionParams, sizeof (GETVERSIONINPARAMS),
				&cbBytesReturned, NULL) )
			{         
#ifdef PRINTING_TO_CONSOLE_ALLOWED
				if (PRINT_DEBUG)
				{
					DWORD err = GetLastError ();
					printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
						"\nDeviceIoControl(%d, SMART_GET_VERSION) returned 0, error is %d\n",
						__LINE__, (int) hPhysicalDriveIOCTL, (int) err);
				}
#endif
			}
			else
			{
				// Print the SMART version
				// PrintVersion (& GetVersionParams);
				// Allocate the command buffer
				ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
				PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) malloc (CommandSize);
				// Retrieve the IDENTIFY data
				// Prepare the command

#define ID_CMD          0xEC            // Returns ID sector for ATA

				Command -> irDriveRegs.bCommandReg = ID_CMD;
				DWORD BytesReturned = 0;
				if (!DeviceIoControl (hPhysicalDriveIOCTL, 
					SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
					Command, CommandSize,
					&BytesReturned, NULL) )
                {
                    printf("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
                        "\nDeviceIoControl() Error Code %d\n", __LINE__, GetLastError());
				} 
				else
				{
					// Print the IDENTIFY data
					DWORD diskdata [256];
					USHORT *pIdSector = (USHORT *)
						(PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;

					for (int ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					PrintIdeInfo (drive, diskdata);
					done = (drive / 2==0 && drive % 2==0);
				}
				// Done
				CloseHandle (hPhysicalDriveIOCTL);
				free (Command);
			}
		}
		//if(done) break;
	}

	return done;
}


//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(4)


//
// IOCTL_STORAGE_QUERY_PROPERTY
//
// Input Buffer:
//      a STORAGE_PROPERTY_QUERY structure which describes what type of query
//      is being done, what property is being queried for, and any additional
//      parameters which a particular property query requires.
//
//  Output Buffer:
//      Contains a buffer to place the results of the query into.  Since all
//      property descriptors can be cast into a STORAGE_DESCRIPTOR_HEADER,
//      the IOCTL can be called once with a small buffer then again using
//      a buffer as large as the header reports is necessary.
//


//
// Types of queries
//
//
//typedef enum _STORAGE_QUERY_TYPE {
//	PropertyStandardQuery = 0,          // Retrieves the descriptor
//	PropertyExistsQuery,                // Used to test whether the descriptor is supported
//	PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor
//	PropertyQueryMaxDefined     // use to validate the value
//} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

//
// define some initial property id's
//
//
//typedef enum _STORAGE_PROPERTY_ID {
//	StorageDeviceProperty = 0,
//	StorageAdapterProperty
//} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

//
// Query structure - additional parameters for specific queries can follow
// the header
//

//typedef struct _STORAGE_PROPERTY_QUERY {
//
//	//
//	// ID of the property being retrieved
//	//
//
//	STORAGE_PROPERTY_ID PropertyId;
//
//	//
//	// Flags indicating the type of query being performed
//	//
//
//	STORAGE_QUERY_TYPE QueryType;
//
//	//
//	// Space for additional parameters if necessary
//	//
//
//	UCHAR AdditionalParameters[1];
//
//} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;


#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Device property descriptor - this is really just a rehash of the inquiry
// data retrieved from a scsi device
//
// This may only be retrieved from a target device.  Sending this to the bus
// will result in an error
//

#pragma pack(4)

//typedef struct _STORAGE_DEVICE_DESCRIPTOR {
//
//	//
//	// Sizeof(STORAGE_DEVICE_DESCRIPTOR)
//	//
//
//	ULONG Version;
//
//	//
//	// Total size of the descriptor, including the space for additional
//	// data and id strings
//	//
//
//	ULONG Size;
//
//	//
//	// The SCSI-2 device type
//	//
//
//	UCHAR DeviceType;
//
//	//
//	// The SCSI-2 device type modifier (if any) - this may be zero
//	//
//
//	UCHAR DeviceTypeModifier;
//
//	//
//	// Flag indicating whether the device's media (if any) is removable.  This
//	// field should be ignored for media-less devices
//	//
//
//	BOOLEAN RemovableMedia;
//
//	//
//	// Flag indicating whether the device can support mulitple outstanding
//	// commands.  The actual synchronization in this case is the responsibility
//	// of the port driver.
//	//
//
//	BOOLEAN CommandQueueing;
//
//	//
//	// Byte offset to the zero-terminated ascii string containing the device's
//	// vendor id string.  For devices with no such ID this will be zero
//	//
//
//	ULONG VendorIdOffset;
//
//	//
//	// Byte offset to the zero-terminated ascii string containing the device's
//	// product id string.  For devices with no such ID this will be zero
//	//
//
//	ULONG ProductIdOffset;
//
//	//
//	// Byte offset to the zero-terminated ascii string containing the device's
//	// product revision string.  For devices with no such string this will be
//	// zero
//	//
//
//	ULONG ProductRevisionOffset;
//
//	//
//	// Byte offset to the zero-terminated ascii string containing the device's
//	// serial number.  For devices with no serial number this will be zero
//	//
//
//	ULONG SerialNumberOffset;
//
//	//
//	// Contains the bus type (as defined above) of the device.  It should be
//	// used to interpret the raw device properties at the end of this structure
//	// (if any)
//	//
//
//	STORAGE_BUS_TYPE BusType;
//
//	//
//	// The number of bytes of bus-specific data which have been appended to
//	// this descriptor
//	//
//
//	ULONG RawPropertiesLength;
//
//	//
//	// Place holder for the first byte of the bus specific property data
//	//
//
//	UCHAR RawDeviceProperties[1];
//
//} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;


//  function to decode the serial numbers of IDE hard drives
//  using the IOCTL_STORAGE_QUERY_PROPERTY command 
char * CTerminalinfo::flipAndCodeBytes (const char * str,
						 int pos,
						 int flip,
						 char * buf)
{
	int i;
	int j = 0;
	int k = 0;

	buf [0] = '\0';
	if (pos <= 0)
		return buf;

	if ( ! j)
	{
		char p = 0;

		// First try to gather all characters representing hex digits only.
		j = 1;
		k = 0;
		buf[k] = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = tolower(str[i]);

			if (isspace(c))
				c = '0';

			++p;
			buf[k] <<= 4;

			if (c >= '0' && c <= '9')
				buf[k] |= (unsigned char) (c - '0');
			else if (c >= 'a' && c <= 'f')
				buf[k] |= (unsigned char) (c - 'a' + 10);
			else
			{
				j = 0;
				break;
			}

			if (p == 2)
			{
				if (buf[k] != '\0' && ! isprint(buf[k]))
				{
					j = 0;
					break;
				}
				++k;
				p = 0;
				buf[k] = 0;
			}

		}
	}

	if ( ! j)
	{
		// There are non-digit characters, gather them as is.
		j = 1;
		k = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = str[i];

			if ( ! isprint(c))
			{
				j = 0;
				break;
			}

			buf[k++] = c;
		}
	}

	if ( ! j)
	{
		// The characters are not there or are not printable.
		k = 0;
	}

	buf[k] = '\0';

	if (flip)
		// Flip adjacent characters
		for (j = 0; j < k; j += 2)
		{
			char t = buf[j];
			buf[j] = buf[j + 1];
			buf[j + 1] = t;
		}

		// Trim any beginning and end space
		i = j = -1;
		for (k = 0; buf[k] != '\0'; ++k)
		{
			if (! isspace(buf[k]))
			{
				if (i < 0)
					i = k;
				j = k;
			}
		}

		if ((i >= 0) && (j >= 0))
		{
			for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
				buf[k - i] = buf[k];
			buf[k - i] = '\0';
		}

		return buf;
}



#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)
//
//typedef struct _DISK_GEOMETRY_EX {
//	DISK_GEOMETRY  Geometry;
//	LARGE_INTEGER  DiskSize;
//	UCHAR  Data[1];
//} DISK_GEOMETRY_EX, *PDISK_GEOMETRY_EX;


int CTerminalinfo::ReadPhysicalDriveInNTWithZeroRights (char cSysDrive)
{
	int done = FALSE;
	int drive = 0;

// 	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

//		sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);
		sprintf (driveName, "\\\\.\\%c:",cSysDrive);

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = CreateFileA (driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
			strcpy(m_szTerErrinf[eHD],"权限不足");
#ifdef PRINTING_TO_CONSOLE_ALLOWED
			if (PRINT_DEBUG)
				printf ("\n%d ReadPhysicalDriveInNTWithZeroRights ERROR"
				"\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n",
				__LINE__, driveName);
#endif
		}
		else
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];

			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			memset (buffer, 0, sizeof (buffer));

			if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query),
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				char serialNumber [1000] = {0x00};
				char modelNumber [1000] = {0x00};
				char vendorId [1000] = {0x00};
				char productRevision [1000] = {0x00};

//#ifdef PRINTING_TO_CONSOLE_ALLOWED
//				if (PRINT_DEBUG)
//				{
//					printf ("\n%d STORAGE_DEVICE_DESCRIPTOR contents for drive %d\n"
//						"                Version: %ld\n"
//						"                   Size: %ld\n"
//						"             DeviceType: %02x\n"
//						"     DeviceTypeModifier: %02x\n"
//						"         RemovableMedia: %d\n"
//						"        CommandQueueing: %d\n"
//						"         VendorIdOffset: %4ld (0x%02lx)\n"
//						"        ProductIdOffset: %4ld (0x%02lx)\n"
//						"  ProductRevisionOffset: %4ld (0x%02lx)\n"
//						"     SerialNumberOffset: %4ld (0x%02lx)\n"
//						"                BusType: %d\n"
//						"    RawPropertiesLength: %ld\n",
//						__LINE__, drive,
//						(unsigned long) descrip->Version,
//						(unsigned long) descrip->Size,
//						(int) descrip->DeviceType,
//						(int) descrip->DeviceTypeModifier,
//						(int) descrip->RemovableMedia,
//						(int) descrip->CommandQueueing,
//						(unsigned long) descrip->VendorIdOffset,
//						(unsigned long) descrip->VendorIdOffset,
//						(unsigned long) descrip->ProductIdOffset,
//						(unsigned long) descrip->ProductIdOffset,
//						(unsigned long) descrip->ProductRevisionOffset,
//						(unsigned long) descrip->ProductRevisionOffset,
//						(unsigned long) descrip->SerialNumberOffset,
//						(unsigned long) descrip->SerialNumberOffset,
//						(int) descrip->BusType,
//						(unsigned long) descrip->RawPropertiesLength);
//
//					dump_buffer ("Contents of RawDeviceProperties",
//						(unsigned char*) descrip->RawDeviceProperties,
//						descrip->RawPropertiesLength);
//
//					dump_buffer ("Contents of first 256 bytes in buffer",
//						(unsigned char*) buffer, 256);
//				}
//#endif
				flipAndCodeBytes (buffer,
					descrip -> VendorIdOffset,
					0, vendorId );
				flipAndCodeBytes (buffer,
					descrip -> ProductIdOffset,
					0, modelNumber );
				flipAndCodeBytes (buffer,
					descrip -> ProductRevisionOffset,
					0, productRevision );
				/*flipAndCodeBytes (buffer,
					descrip -> SerialNumberOffset,
					1, serialNumber );*/

				strncpy_s(serialNumber, sizeof(serialNumber), 
					buffer + descrip->SerialNumberOffset, sizeof(serialNumber) - 1);

				if (0 == m_szTerinf[eHD][0] && serialNumber[0] &&
					//  serial number must be alphanumeric
					//  (but there can be leading spaces on IBM drives)
					(isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
				{
					strcpy (m_szTerinf[eHD], serialNumber);
					m_szTerErrinf[eHD][0]= 0x00;
					done = TRUE;
				}
				strcpy (m_szTerinf[eHDCAP], modelNumber);
				// Get the disk drive geometry.
				/*memset (buffer, 0, sizeof(buffer));
				if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
					IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
					NULL,
					0,
					&buffer,
					sizeof(buffer),
					&cbBytesReturned,
					NULL))
				{
#ifdef PRINTING_TO_CONSOLE_ALLOWED
					if (PRINT_DEBUG)
						printf ("\n%d ReadPhysicalDriveInNTWithZeroRights ERROR"
						"|nDeviceIoControl(%s, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX) returned 0",
						driveName);
#endif
				}
				else
				{         
					DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
					int fixed = (geom->Geometry.MediaType == FixedMedia);
					__int64 size = geom->DiskSize.QuadPart;

#ifdef PRINTING_TO_CONSOLE_ALLOWED
					printf ("\n**** DISK_GEOMETRY_EX for drive %d ****\n"
						"Disk is%s fixed\n"
						"DiskSize = %I64d\n",
						drive,
						fixed ? "" : " NOT",
						size);
#endif
				}*/
			}
			else
			{
				DWORD err = GetLastError ();
#ifdef PRINTING_TO_CONSOLE_ALLOWED
				printf ("\nDeviceIOControl IOCTL_STORAGE_QUERY_PROPERTY error = %d\n", err);
#endif
			}

			CloseHandle (hPhysicalDriveIOCTL);
		}
		//if(done) break;
	}

	return done;
}


// DoIDENTIFY
// FUNCTION: Send an IDENTIFY command to the drive
// bDriveNum = 0-3
// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL CTerminalinfo::DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				 PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	//pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;

	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL) );
}


//  ---------------------------------------------------

// (* Output Bbuffer for the VxD (rt_IdeDinfo record) *)
typedef struct _rt_IdeDInfo_
{
	BYTE IDEExists[4];
	BYTE DiskExists[8];
	WORD DisksRawInfo[8*256];
} rt_IdeDInfo, *pt_IdeDInfo;


// (* IdeDinfo "data fields" *)
typedef struct _rt_DiskInfo_
{
	BOOL DiskExists;
	BOOL ATAdevice;
	BOOL RemovableDevice;
	WORD TotLogCyl;
	WORD TotLogHeads;
	WORD TotLogSPT;
	char SerialNumber[20];
	char FirmwareRevision[8];
	char ModelNumber[40];
	WORD CurLogCyl;
	WORD CurLogHeads;
	WORD CurLogSPT;
} rt_DiskInfo;


#define  m_cVxDFunctionIdesDInfo  1


//  ---------------------------------------------------


int CTerminalinfo::ReadDrivePortsInWin9X (void)
{
	int done = FALSE;
	unsigned long int i = 0;

	HANDLE VxDHandle = 0;
	pt_IdeDInfo pOutBufVxD = 0;
	DWORD lpBytesReturned = 0;

	//  set the thread priority high so that we get exclusive access to the disk
	BOOL status =
		// SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		SetPriorityClass (GetCurrentProcess (), REALTIME_PRIORITY_CLASS);
	// SetPriorityClass (GetCurrentProcess (), HIGH_PRIORITY_CLASS);

#ifdef PRINTING_TO_CONSOLE_ALLOWED

	if (0 == status) 
		// printf ("\nERROR: Could not SetThreadPriority, LastError: %d\n", GetLastError ());
		printf ("\nERROR: Could not SetPriorityClass, LastError: %d\n", GetLastError ());

#endif

	// 1. Make an output buffer for the VxD
	rt_IdeDInfo info;
	pOutBufVxD = &info;

	// *****************
	// KLUDGE WARNING!!!
	// HAVE to zero out the buffer space for the IDE information!
	// If this is NOT done then garbage could be in the memory
	// locations indicating if a disk exists or not.
	ZeroMemory (&info, sizeof(info));

	// 1. Try to load the VxD
	//  must use the short file name path to open a VXD file
	//char StartupDirectory [2048];
	//char shortFileNamePath [2048];
	//char *p = NULL;
	//char vxd [2048];
	//  get the directory that the exe was started from
	//GetModuleFileName (hInst, (LPSTR) StartupDirectory, sizeof (StartupDirectory));
	//  cut the exe name from string
	//p = &(StartupDirectory [strlen (StartupDirectory) - 1]);
	//while (p >= StartupDirectory && *p && '\\' != *p) p--;
	//*p = '\0';   
	//GetShortPathName (StartupDirectory, shortFileNamePath, 2048);
	//sprintf (vxd, "\\\\.\\%s\\IDE21201.VXD", shortFileNamePath);
	//VxDHandle = CreateFile (vxd, 0, 0, 0,
	//               0, FILE_FLAG_DELETE_ON_CLOSE, 0);   
	VxDHandle = CreateFileA ("\\\\.\\IDE21201.VXD", 0, 0, 0,
		0, FILE_FLAG_DELETE_ON_CLOSE, 0);

	if (VxDHandle != INVALID_HANDLE_VALUE)
	{
		// 2. Run VxD function
		DeviceIoControl (VxDHandle, m_cVxDFunctionIdesDInfo,
			0, 0, pOutBufVxD, sizeof(pt_IdeDInfo), &lpBytesReturned, 0);

		// 3. Unload VxD
		CloseHandle (VxDHandle);
	}
	/*else
	MessageBox (NULL, "ERROR: Could not open IDE21201.VXD file", 
	TITLE, MB_ICONSTOP);*/

	// 4. Translate and store data
	for (i=0; i<8; i++)
	{
		if((pOutBufVxD->DiskExists[i]) && (pOutBufVxD->IDEExists[i/2]))
		{
			DWORD diskinfo [256];
			for (int j = 0; j < 256; j++) 
				diskinfo [j] = pOutBufVxD -> DisksRawInfo [i * 256 + j];

			// process the information for this buffer
			PrintIdeInfo (i, diskinfo);
			done = (i / 2==0 && i % 2==0);
		}
		if(done) break;
	}

	//  reset the thread priority back to normal
	// SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	SetPriorityClass (GetCurrentProcess (), NORMAL_PRIORITY_CLASS);

	return done;
}


#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE


int CTerminalinfo::ReadIdeDriveAsScsiDriveInNT (char cSysDrive)
{
	int done = FALSE;
	int controller = 0;

// 	for (controller = 0; controller < 16; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   driveName [256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		//sprintf (driveName, "\\\\.\\Scsi%d:", controller);
		sprintf (driveName, "\\\\.\\%c:", cSysDrive);

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFileA (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
		//            controller, GetLastError ());

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int drive = 0;

			for (drive = 0; drive < 2; drive++)
			{
				char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
				SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
				SENDCMDINPARAMS *pin =
					(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;

				memset (buffer, 0, sizeof (buffer));
				p -> HeaderLength = sizeof (SRB_IO_CONTROL);
				p -> Timeout = 10000;
				p -> Length = SENDIDLENGTH;
				p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy ((char *) p -> Signature, "SCSIDISK", 8);

				pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin -> bDriveNumber = drive;

				if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					buffer,
					sizeof (SRB_IO_CONTROL) +
					sizeof (SENDCMDINPARAMS) - 1,
					buffer,
					sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
					&dummy, NULL))
				{
					SENDCMDOUTPARAMS *pOut =
						(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
					if (pId -> sModelNumber [0])
					{
						DWORD diskdata [256];
						int ijk = 0;
						USHORT *pIdSector = (USHORT *) pId;

						for (ijk = 0; ijk < 256; ijk++)
							diskdata [ijk] = pIdSector [ijk];

						PrintIdeInfo (controller * 2 + drive, diskdata);

						done = ((controller * 2 + drive) / 2==0 && (controller * 2 + drive) % 2==0);
					}
				}
			}
			CloseHandle (hScsiDriveIOCTL);
		}
		//if(done) break;
	}

	return done;
}


void CTerminalinfo::PrintIdeInfo (int drive, DWORD diskdata [256])
{
	char serialNumber [1024];
	char modelNumber [1024];
	char revisionNumber [1024];
	char bufferSize [32];

	__int64 sectors = 0;
	__int64 bytes = 0;

	//  copy the hard drive serial number to the buffer
	ConvertToString (diskdata, 10, 19, serialNumber);
	ConvertToString (diskdata, 27, 46, modelNumber);
	ConvertToString (diskdata, 23, 26, revisionNumber);
	sprintf (bufferSize, "%u", diskdata [21] * 512);

	if (0 == m_szTerinf[eHD] [0] && serialNumber [0]&&
		//  serial number must be alphanumeric
		//  (but there can be leading spaces on IBM drives)
		(isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
	{
		strcpy (m_szTerinf[eHD], serialNumber);
		m_szTerErrinf[eHD][0]= 0x00;
	}
	strcpy (m_szTerinf[eHDCAP], modelNumber);
//
//#ifdef PRINTING_TO_CONSOLE_ALLOWED
//
//	printf ("\nDrive %d - ", drive);
//
//	switch (drive / 2)
//	{
//	case 0: printf ("Primary Controller - ");
//		break;
//	case 1: printf ("Secondary Controller - ");
//		break;
//	case 2: printf ("Tertiary Controller - ");
//		break;
//	case 3: printf ("Quaternary Controller - ");
//		break;
//	}
//
//	switch (drive % 2)
//	{
//	case 0: printf (" - Master drive\n\n");
//		break;
//	case 1: printf (" - Slave drive\n\n");
//		break;
//	}
//
//	printf ("Drive Model Number________________: [%s]\n",
//		modelNumber);
//	printf ("Drive Serial Number_______________: [%s]\n",
//		serialNumber);
//	printf ("Drive Controller Revision Number__: [%s]\n",
//		revisionNumber);
//
//	printf ("Controller Buffer Size on Drive___: %s bytes\n",
//		bufferSize);
//
//	printf ("Drive Type________________________: ");
//	if (diskdata [0] & 0x0080)
//		printf ("Removable\n");
//	else if (diskdata [0] & 0x0040)
//		printf ("Fixed\n");
//	else printf ("Unknown\n");
//
//	//  calculate size based on 28 bit or 48 bit addressing
//	//  48 bit addressing is reflected by bit 10 of word 83
//	if (diskdata [83] & 0x400) 
//		sectors = diskdata [103] * 65536I64 * 65536I64 * 65536I64 + 
//		diskdata [102] * 65536I64 * 65536I64 + 
//		diskdata [101] * 65536I64 + 
//		diskdata [100];
//	else
//		sectors = diskdata [61] * 65536 + diskdata [60];
//	//  there are 512 bytes in a sector
//	bytes = sectors * 512;
//	printf ("Drive Size________________________: %I64d bytes\n",
//		bytes);
//
//#endif  // PRINTING_TO_CONSOLE_ALLOWED
//
//	char string1 [1000];
//	sprintf (string1, "Drive%dModelNumber", drive);
//	WriteConstantString (string1, modelNumber);
//
//	sprintf (string1, "Drive%dSerialNumber", drive);
//	WriteConstantString (string1, serialNumber);
//
//	sprintf (string1, "Drive%dControllerRevisionNumber", drive);
//	WriteConstantString (string1, revisionNumber);
//
//	sprintf (string1, "Drive%dControllerBufferSize", drive);
//	WriteConstantString (string1, bufferSize);
//
//	sprintf (string1, "Drive%dType", drive);
//	if (diskdata [0] & 0x0080)
//		WriteConstantString (string1, "Removable");
//	else if (diskdata [0] & 0x0040)
//		WriteConstantString (string1, "Fixed");
//	else
//		WriteConstantString (string1, "Unknown");
}



char *CTerminalinfo::ConvertToString (DWORD diskdata [256],
					   int firstIndex,
					   int lastIndex,
					   char* buf)
{
	int index = 0;
	int position = 0;

	//  each integer has two characters stored in it backwards
	for (index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character
		buf [position++] = (char) (diskdata [index] / 256);

		//  get low byte for 2nd character
		buf [position++] = (char) (diskdata [index] % 256);
	}

	//  end the string 
	buf[position] = '\0';

	//  cut off the trailing blanks
	for (index = position - 1; index > 0 && isspace(buf [index]); index--)
		buf [index] = '\0';

	return buf;
}



long CTerminalinfo::getHardDriveComputerID (char cSysDrive)
{
	int done = FALSE;
	// char string [1024];
	__int64 id = 0;
	OSVERSIONINFO version;
	strcpy (m_szTerinf[eHD], "");
	memset (&version, 0, sizeof (version));
	version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&version);
	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		//  this works under WinNT4 or Win2K if you have admin rights
#ifdef PRINTING_TO_CONSOLE_ALLOWED
		printf ("\nTrying to read the drive IDs using physical access with admin rights\n");
#endif
		done = ReadPhysicalDriveInNTWithAdminRights (cSysDrive);

		//  this should work in WinNT or Win2K if previous did not work
		//  this is kind of a backdoor via the SCSI mini port driver into
		//     the IDE drives
#ifdef PRINTING_TO_CONSOLE_ALLOWED
		printf ("\nTrying to read the drive IDs using the SCSI back door\n");
#endif
		if ( !done) 
			done = ReadIdeDriveAsScsiDriveInNT (cSysDrive);

		//  this works under WinNT4 or Win2K or WinXP if you have any rights
#ifdef PRINTING_TO_CONSOLE_ALLOWED
		printf ("\nTrying to read the drive IDs using physical access with zero rights\n");
#endif
		if ( ! done)
			done = ReadPhysicalDriveInNTWithZeroRights (cSysDrive);

		//  this works under WinNT4 or Win2K or WinXP or Windows Server 2003 or Vista if you have any rights
#ifdef PRINTING_TO_CONSOLE_ALLOWED
		printf ("\nTrying to read the drive IDs using Smart\n");
#endif
		if ( ! done)
			done = ReadPhysicalDriveInNTUsingSmart (cSysDrive);
	}
	else
	{
		//  this works under Win9X and calls a VXD
		int attempt = 0;

		//  try this up to 10 times to get a hard drive serial number
		for (attempt = 0;
			attempt < 10 && ! done && 0 == m_szTerinf[eHD] [0];
			attempt++)
			done = ReadDrivePortsInWin9X ();
	}

	if (m_szTerinf[eHD] [0] > 0)
	{
		char *p = m_szTerinf[eHD];

		//WriteConstantString ("m_szTerinf[eHD]", m_szTerinf[eHD]);

		//  ignore first 5 characters from western digital hard drives if
		//  the first four characters are WD-W
		if ( ! strncmp (m_szTerinf[eHD], "WD-W", 4)) 
			p += 5;
		for ( ; p && *p; p++)
		{
			if ('-' == *p) 
				continue;
			id *= 10;
			switch (*p)
			{
			case '0': id += 0; break;
			case '1': id += 1; break;
			case '2': id += 2; break;
			case '3': id += 3; break;
			case '4': id += 4; break;
			case '5': id += 5; break;
			case '6': id += 6; break;
			case '7': id += 7; break;
			case '8': id += 8; break;
			case '9': id += 9; break;
			case 'a': case 'A': id += 10; break;
			case 'b': case 'B': id += 11; break;
			case 'c': case 'C': id += 12; break;
			case 'd': case 'D': id += 13; break;
			case 'e': case 'E': id += 14; break;
			case 'f': case 'F': id += 15; break;
			case 'g': case 'G': id += 16; break;
			case 'h': case 'H': id += 17; break;
			case 'i': case 'I': id += 18; break;
			case 'j': case 'J': id += 19; break;
			case 'k': case 'K': id += 20; break;
			case 'l': case 'L': id += 21; break;
			case 'm': case 'M': id += 22; break;
			case 'n': case 'N': id += 23; break;
			case 'o': case 'O': id += 24; break;
			case 'p': case 'P': id += 25; break;
			case 'q': case 'Q': id += 26; break;
			case 'r': case 'R': id += 27; break;
			case 's': case 'S': id += 28; break;
			case 't': case 'T': id += 29; break;
			case 'u': case 'U': id += 30; break;
			case 'v': case 'V': id += 31; break;
			case 'w': case 'W': id += 32; break;
			case 'x': case 'X': id += 33; break;
			case 'y': case 'Y': id += 34; break;
			case 'z': case 'Z': id += 35; break;
			}                            
		}
	}else if(m_szTerErrinf[eHD][0] == 0x00)
	{
		strcpy(m_szTerErrinf[eHD], "UnKnown1");
	}
	if (strnicmp (m_szTerinf[eHDCAP], "VMWare ",sizeof("VMWare")) == 0 || strnicmp (m_szTerinf[eHDCAP], "VBOX ",sizeof("VBOX")) == 0 ||
		strnicmp (m_szTerinf[eHDCAP], "Virtual ",sizeof("Virtual")) == 0 ||  strnicmp (m_szTerinf[eHDCAP], "QEMU ",sizeof("QEMU")) == 0  )
	{
		strcpy(m_szTerErrinf[eHD],"UnKnown2");
	}
	id %= 100000000;
	if (strstr (m_szTerinf[eHDCAP], "IBM-"))
		id += 300000000;
	else if (strstr (m_szTerinf[eHDCAP], "MAXTOR") ||
		strstr (m_szTerinf[eHDCAP], "Maxtor"))
		id += 400000000;
	else if (strstr (m_szTerinf[eHDCAP], "WDC "))
		id += 500000000;
	else
		id += 600000000;

//#ifdef PRINTING_TO_CONSOLE_ALLOWED
//
//	printf ("\nHard Drive Serial Number__________: %s\n", 
//		m_szTerinf[eHD]);
//	printf ("\nHard Drive Model Number___________: %s\n", 
//		m_szTerinf[eHDCAP]);
//	printf ("\nComputer ID_______________________: %I64d\n", id);
//
//#endif

	return (long) id;
}



// GetMACAdapters.cpp : Defines the entry point for the console application.
//
// Author:  Khalid Shaikh [Shake@ShakeNet.com]
// Date:    April 5th, 2002
//
// This program fetches the MAC address of the localhost by fetching the 
// information through GetAdapatersInfo.  It does not rely on the NETBIOS
// protocol and the ethernet adapter need not be connect to a network.
//
// Supported in Windows NT/2000/XP
// Supported in Windows 95/98/Me
//
// Supports multiple NIC cards on a PC.

#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")



// Prints the MAC address stored in a 6 byte array to stdout
//static void CTerminalinfo::PrintMACaddress(unsigned char MACData[])
//{
//
//#ifdef PRINTING_TO_CONSOLE_ALLOWED
//
//	printf("\nMAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n", 
//		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
//
//#endif
//
//	char string [256];
//	sprintf (string, "%02X-%02X-%02X-%02X-%02X-%02X", MACData[0], MACData[1], 
//		MACData[2], MACData[3], MACData[4], MACData[5]);
//	WriteConstantString ("MACaddress", string);
//}
//
//
//
//// Fetches the MAC address and prints it
//DWORD GetMACaddress(void)
//{
//	DWORD MACaddress = 0;
//	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
//	// for up to 16 NICs
//	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
//
//	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
//		AdapterInfo,                 // [out] buffer to receive data
//		&dwBufLen);                  // [in] size of receive data buffer
//	assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
//	// valid, no buffer overflow
//
//	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
//	// current adapter info
//	do {
//		if (MACaddress == 0)
//			MACaddress = pAdapterInfo->Address [5] + pAdapterInfo->Address [4] * 256 + 
//			pAdapterInfo->Address [3] * 256 * 256 + 
//			pAdapterInfo->Address [2] * 256 * 256 * 256;
//		//if(pAdapterInfo->IpAddressList.IpAddress)
//
//		PrintMACaddress(pAdapterInfo->Address); // Print MAC address
//		WriteConstantString ("MA Caddress", pAdapterInfo->IpAddressList.IpAddress.String);
//		pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
//	}
//	while(pAdapterInfo);                    // Terminate if last adapter
//
//	return MACaddress;
//}
//
//

//static void dump_buffer (const char* title,
//						 const unsigned char* buffer,
//						 int len)
//{
//	int i = 0;
//	int j;
//
//	printf ("\n-- %s --\n", title);
//	if (len > 0)
//	{
//		printf ("%8.8s ", " ");
//		for (j = 0; j < 16; ++j)
//		{
//			printf (" %2X", j);
//		}
//		printf ("  ");
//		for (j = 0; j < 16; ++j)
//		{
//			printf ("%1X", j);
//		}
//		printf ("\n");
//	}
//	while (i < len)
//	{
//		printf("%08x ", i);
//		for (j = 0; j < 16; ++j)
//		{
//			if ((i + j) < len)
//				printf (" %02x", (int) buffer[i +j]);
//			else
//				printf ("   ");
//		}
//		printf ("  ");
//		for (j = 0; j < 16; ++j)
//		{
//			if ((i + j) < len)
//				printf ("%c", isprint (buffer[i + j]) ? buffer [i + j] : '.');
//			else
//				printf (" ");
//		}
//		printf ("\n");
//		i += 16;
//	}
//	printf ("-- DONE --\n");
//}
//
//滤除字符串起始位置的空格
void TrimStart(char* pBuf)
{
	if(*pBuf != 0x20)
		return;

	char* pDest = pBuf;
	char* pSrc = pBuf + 1;
	while(*pSrc == 0x20)
		++pSrc;

	while(*pSrc)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
	*pDest = 0;
}

std::set<unsigned short> GetAllUsedTcpPort()
{
    std::set<unsigned short> result;

    ULONG size = 0;
    GetTcpTable(NULL, &size, TRUE);

    char *pBuffer = new char[size];
    PMIB_TCPTABLE tcpTable = reinterpret_cast<PMIB_TCPTABLE>(pBuffer);

    if (GetTcpTable(tcpTable, &size, FALSE) == NO_ERROR)
    {
        for (size_t i = 0; i < tcpTable->dwNumEntries; i++)
        {
            result.insert(ntohs((unsigned short)tcpTable->table[i].dwLocalPort));
        }
    }
    delete pBuffer;
    pBuffer = nullptr;

    return result;
}

CTerminalinfo::CTerminalinfo(void)
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	memset(m_szTerinf,0x00,sizeof(m_szTerinf));
	memset(m_szTerErrinf,0x00,sizeof(m_szTerErrinf));
	memset(m_szAllmac,0x00,sizeof(m_szAllmac));
	memset(m_szAllmacIp,0x00,sizeof(m_szAllmacIp));
	strcpy(m_szTerErrinf[eIIP],"未调用连接初始");
	strcpy(m_szTerErrinf[eLIP],"未调用连接初始");

    getCpuInfo();

	memset(m_szTerinf[ePCN],0,sizeof(m_szTerinf[ePCN]));
	memset(m_szTerinf[eLIP],0,sizeof(m_szTerinf[eLIP]));
	gethostname(m_szTerinf[ePCN],sizeof(m_szTerinf[ePCN]));                
	struct hostent FAR* lpHostEnt = gethostbyname(m_szTerinf[ePCN]);
	if( lpHostEnt )
	{
		// 取得IP地址列表中的第一个为返回的IP(因为一台主机可能会绑定多个IP)
		LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
		struct in_addr inAddr;
		// 将IP地址转化成字符串形式
		char   **pptr;// *ptr, 
		//char   str[32];
		pptr=lpHostEnt->h_addr_list;
		
		for(; *pptr!=NULL; pptr++)
		{
			memmove(&inAddr,pptr[0],4);
			printf( " address:%s\n", inet_ntoa(inAddr));//IPV6/IPV4内网IP
			//printf(" address:%s\n",
			//	inet_ntop(lpHostEnt->h_addrtype, *pptr, str, sizeof(str)));
			//printf(" first address: %s\n",
			//	inet_ntop(lpHostEnt->h_addrtype, lpHostEnt->h_addr, str, sizeof(str)));
		}

		int i = 0;
		while(lpAddr)
		{
			memmove(&inAddr,lpAddr,4);
			sprintf( m_szTerinf[eLIP],"%s", inet_ntoa(inAddr));//IPV6/IPV4内网IP
			m_szTerErrinf[eLIP][0]=0x00;
			if (strstr(m_szTerinf[eLIP],"127.0.0.1")==0)
			{
				break;
			}
			lpAddr = lpHostEnt->h_addr_list[i+1];
			i++;
			
		}
	}

    std::set<unsigned short> setUsedPort = GetAllUsedTcpPort();
    srand(time(NULL));
    while (true)
    {
        unsigned short nPort = rand() % 10000;
        if (nPort >= 1000)
        {
            if (setUsedPort.find(nPort) == setUsedPort.end())
            {
                sprintf(m_szTerinf[eIPORT], "%d", nPort);
                break;
            }
        }
    }

    getAppName();
    getDiskInfo();

	get_locaddr();
	//get_Romeaddr();
}
CTerminalinfo::~CTerminalinfo(void)
{
	WSACleanup(); 
}

#ifndef _MSC_VER
bool CTerminalinfo::get_locaddr()
{
	do
	{
		char szhostname[1024]={0};
		int rslt = readlink("/proc/self/exe", szhostname, sizeof(szhostname) - 1);
		if (rslt < 0 || (rslt >= sizeof(szhostname) - 1))
		{
			sprintf(m_szTerinf[eAPP],"%s","kpmscli");
			break;
		}
		szhostname[rslt] = '\0';
		for (int i = rslt; i >= 0; i--)
		{
			//printf("buf[%d] %c\n", i, buf[i]);
			if (szhostname[i] == '/')
			{
				sprintf(m_szTerinf[eAPP],"%s",szhostname + i+1);
				break;
			}
		}
	} while (0);

	struct ifreq tmpifeq[10];
	char mac_addr[30];
	struct ifconf get_info;
	get_info.ifc_len = sizeof(tmpifeq);
	get_info.ifc_ifcu.ifcu_buf = (char*)tmpifeq;
	int sockfd1;
	int sockfd;
	sockfd1 = socket(PF_INET,SOCK_STREAM,0);
	ioctl(sockfd1,SIOCGIFCONF,&get_info);
	int count;
	//获取网络接口数
	count = get_info.ifc_len / sizeof(struct ifreq);
	if(count)
	{
		sprintf(m_szTerinf[eMAC],"%s","001122334455");
		close(sockfd1);
		return false;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0)
	{
		perror("create socket fail\n");
		close(sockfd1);
		sprintf(m_szTerinf[eMAC],"%s","001122334455");
		return false;
	}
	if( (ioctl(sockfd,SIOCGIFHWADDR,tmpifeq)) < 0 )
	{
		printf("mac ioctl error\n");
		close(sockfd);
		close(sockfd1);
		sprintf(m_szTerinf[eMAC],"%s","001122334455");
		//m_szLocAdrr = "001122334455";
		return false;
	}

	sprintf(m_szTerinf[eMAC], "%02x%02x%02x%02x%02x%02x",
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[0],
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[1],
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[2],
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[3],
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[4],
		(unsigned char)tmpifeq[0].ifr_hwaddr.sa_data[5]
	);
	printf("local mac:%s\n", mac_addr);
	close(sockfd);
	close(sockfd1);
	return true;
}
#else
bool locmac(char* szip, char* szTerinf,int nSize)
{
	unsigned char macAddress[10]; 
	ULONG macAddLen = 6; 
	int iRet = SendARP((IPAddr)inet_addr(szip), (unsigned long)NULL,(PULONG)&macAddress, &macAddLen);
	if ( iRet == NO_ERROR ) 
	{ 
		sprintf_s(szTerinf,nSize,"%02X%02X%02X%02X%02X%02X",macAddress[0],macAddress[1],macAddress[2],macAddress[3],macAddress[4],macAddress[5]);
		//m_szTerErrinf[eMAC][0]=0x00;
		return true;
	}
	return false;
}

bool IsLocalAdapter( const char *pAdapterName )
{
	BOOL ret_value = FALSE;

	char szDataBuf[MAX_PATH+1];
	DWORD dwDataLen = MAX_PATH;
	DWORD dwType = REG_SZ;
	HKEY hNetKey = NULL;
	HKEY hLocalNet = NULL;
    LONG retVal = RegOpenKeyExA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_WOW64_64KEY|KEY_READ, &hNetKey);
	if(ERROR_SUCCESS != retVal)
		return FALSE;
	sprintf(szDataBuf, "%s\\Connection", pAdapterName);
	retVal  = RegOpenKeyEx(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet);
	if(ERROR_SUCCESS != retVal)
	{
			RegCloseKey(hNetKey);
			return FALSE;
	}
	dwDataLen = MAX_PATH;
	retVal  = RegQueryValueEx(hLocalNet, "PnpInstanceID", 0, &dwType, (BYTE *)szDataBuf, &dwDataLen);
	if (ERROR_SUCCESS != retVal)
	{
			goto ret;
	}
	if (strncmp(szDataBuf, "PCI", strlen("PCI")))
	{
		goto ret;
	}
	ret_value = TRUE;
ret:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);

	return ret_value;
}

void ReplaceChar(char *str, int nCount,char findChar, char replaceChar)
{
	if (str==NULL) return;
	for (int i = 0; i<nCount; i++)
	{
		if (str[i] == findChar)
		{
			str[i] = replaceChar;
		}
	}
}

bool CTerminalinfo::get_locaddr()
{
	if (0x00 != m_szTerinf[eMAC][0] )  
		return true;

	IP_ADAPTER_INFO *IPInfo;
	DWORD dwBufferSize = 0;
	bool IsFindLocalAdapter = false; 
	IPInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

	if(GetAdaptersInfo(IPInfo, &dwBufferSize)!=ERROR_BUFFER_OVERFLOW)
	{
		free(IPInfo);
		strcpy(m_szTerErrinf[eMAC],"权限不足");
	}
	else
	{
		free(IPInfo);
		IPInfo = (IP_ADAPTER_INFO *)malloc(dwBufferSize);
		GetAdaptersInfo(IPInfo, &dwBufferSize);
		IP_ADAPTER_INFO* IPInfofree =  IPInfo;
		
		bool bAppendAtHeader = false;

		while(IPInfo!=NULL)
		{			
			if (strstr(IPInfo->Description, "VPN") ||
				strstr(IPInfo->Description, "SSl") ||
				strstr(IPInfo->Description, "Virtual"))
			{
				// 这些肯定是虚拟的网卡
				bAppendAtHeader = false;
			}
			else
			{
                // Type=6位以太网卡，71为无线网卡
				if((IPInfo->Type == 6 || IPInfo->Type == 71) && IsLocalAdapter(IPInfo->AdapterName))
				{
					sprintf_s(m_szTerinf[eMAC],sizeof(m_szTerinf[eMAC]),"%02X%02X%02X%02X%02X%02X", IPInfo->Address[0], IPInfo->Address[1], IPInfo->Address[2],
						IPInfo->Address[3], IPInfo->Address[4], IPInfo->Address[5]);

					if(strstr(IPInfo->IpAddressList.IpAddress.String,"0.0.0.0")==0) {
						sprintf( m_szTerinf[eLIP],"%s", IPInfo->IpAddressList.IpAddress.String);
					}

					m_szTerErrinf[eMAC][0]=0x00;
					bAppendAtHeader = IsFindLocalAdapter = true;
				}
				else
				{
					bAppendAtHeader = false;
				}
			}
			
			// 临时保存
			sprintf_s(m_szTerErrinf[eMAC], sizeof(m_szTerErrinf[eMAC]), "%02X%02X%02X%02X%02X%02X", IPInfo->Address[0], IPInfo->Address[1], IPInfo->Address[2],
				IPInfo->Address[3], IPInfo->Address[4], IPInfo->Address[5] );

            ReplaceChar(IPInfo->Description, sizeof(IPInfo->Description), '|', ' ');

			if (bAppendAtHeader)
			{
				char szTemp[4096];

				// 保存本机所有的mac,
                // 注意：拼接是向前拼接，保证本地网卡在列表前面，但一定要添加分隔符(;)，
                // 否则如果有本地网卡出现在链表的最后，就会出现缺少分隔符的问题
				sprintf(szTemp, "%s|%s;", m_szTerErrinf[eMAC], IPInfo->Description);
				strcat(szTemp, m_szAllmac);
				strcpy(m_szAllmac, szTemp);

				// 保存本机所有的mac和ip
				sprintf(szTemp, "%s|%s;", m_szTerErrinf[eMAC], IPInfo->IpAddressList.IpAddress.String);
				strcat(szTemp, m_szAllmacIp);
				strcpy(m_szAllmacIp, szTemp);
			}
			else
			{			
				// 保存本机所有的mac
				strcat(m_szAllmac, m_szTerErrinf[eMAC]);
				strcat(m_szAllmac, "|");
				strcat(m_szAllmac, IPInfo->Description);
                if (IPInfo->Next != NULL)
                {
                    strcat(m_szAllmac, ";");
                }

				// 保存本机所有的mac和ip
				strcat(m_szAllmacIp, m_szTerErrinf[eMAC]);
				strcat(m_szAllmacIp, "|");
				strcat(m_szAllmacIp,  IPInfo->IpAddressList.IpAddress.String);
				if (IPInfo->Next != NULL)
                {
					strcat(m_szAllmacIp, ";");
				}
			}
			IPInfo = IPInfo->Next;
		}

		free(IPInfofree);
	}

	if (IsFindLocalAdapter)
	{
		return true;
	}

	unsigned char macAddress[10]; 
	ULONG macAddLen = 6; 
	int iRet = SendARP((IPAddr)inet_addr(m_szTerinf[eLIP]), (unsigned long)NULL,(PULONG)&macAddress, &macAddLen);
	if ( iRet == NO_ERROR ) 
	{ 
		sprintf_s(m_szTerinf[eMAC],sizeof(m_szTerinf[eMAC]),"%02X%02X%02X%02X%02X%02X",macAddress[0],macAddress[1],macAddress[2],macAddress[3],macAddress[4],macAddress[5]);
		m_szTerErrinf[eMAC][0]=0x00;
		return true;
	}
	return false;
}

#endif
//校验和函数  
USHORT CTerminalinfo::checksum(USHORT *buffer,int size)  
{  
	unsigned long cksum=0;  
	while(size>1)  
	{  
		cksum+=*buffer++;  
		size-=sizeof(USHORT);  
	}  
	if(size)  
		cksum+=*(UCHAR *)buffer;  
	cksum=(cksum>>16)+(cksum & 0xffff);  
	cksum+=(cksum>>16);  
	return (USHORT)(~cksum);;  
}  
int CTerminalinfo::DecodeIPHeader(char *buf,int bytes,struct sockaddr_in *from)  
{  
	IcmpHeader      *icmphdr=NULL;  
// 	DWORD           tick;  
// 	static int      icmpcount=1;  
	unsigned short  iphdrlen;  
	//判断接收操作是否超时  
	if(!buf)  
	{  
// 		printf("%2d:        ***.***.***.***        Request timed out.\n",icmpcount++);  
		return 1;  
	}  
// 	tick=GetTickCount();  
	iphdrlen=(buf[0] & 0x0f)*4;  
	icmphdr=(IcmpHeader *)(buf+iphdrlen);  
	if(bytes<iphdrlen+ICMP_MIN)  
	{  
		if(from->sin_addr.s_net ==10 || from->sin_addr.s_net == 192 || from->sin_addr.s_net == 172 )
		{  // 内网 地址
			return 0;
		}
		m_szTerErrinf[eIIP][0]=0x00;
		sprintf_s(m_szTerinf[eIIP],sizeof(m_szTerinf[eIIP]),"%s",inet_ntoa(from->sin_addr));
		//printf("Too few bytes from %s\n",inet_ntoa(from->sin_addr));  
		return 0;  
	}  
	//判断接收的ICMP报文是否为超时报文  
	if(icmphdr->i_type==ICMP_TIMEOUT&&icmphdr->i_code==0)  
	{  //A 类：10.0.0.0～10.255.255.255	B 类：172.16.0.0～172.31.255.255	C 类：192.168.0.0～192.168.255.255
		sprintf_s(m_szTerinf[eIIP],sizeof(m_szTerinf[eIIP]),"%s",inet_ntoa(from->sin_addr));
		m_szTerErrinf[eIIP][0]=0x00;
		//printf(" ICMP_TIMEOUT    %-15s \n", inet_ntoa(from->sin_addr) );  
		if(from->sin_addr.s_net ==10 || from->sin_addr.s_net == 192 || from->sin_addr.s_net == 172 )
		{// 内网 地址
			return 0;
		}
		from->sin_port = 2;
		return 1;  
	}  
	//判断接收的ICMP报文是否为回复报文  
	else if(icmphdr->i_type==ICMP_ECHOREPLY&&icmphdr->i_id==GetCurrentProcessId())  
	{  
		//if(m_szTerinf[eIIP][0] == 0)
		m_szTerErrinf[eIIP][0]=0x00;
		sprintf_s(m_szTerinf[eIIP],sizeof(m_szTerinf[eIIP]),"%s",inet_ntoa(from->sin_addr));
		//printf("ICMP_ECHOREPLY     %-15s \n", inet_ntoa(from->sin_addr) );  
		from->sin_port = 1;
		//printf("Trace complete!\n");  
		return 1;  
	}  
	//其他类型,表示不可达  
	else  
	{  
		if(m_szTerinf[eIIP][0] == 0)
		{
			sprintf_s(m_szTerinf[eIIP],sizeof(m_szTerinf[eIIP]),"%s",m_szTerinf[eLIP]);
			m_szTerErrinf[eIIP][0]=0x00;
		}
		//printf("Destination host is unreachable     %-15s \n", inet_ntoa(from->sin_addr) );  
		//printf("%2d:        Destination host is unreachable! %s",inet_ntoa(from->sin_addr));
		from->sin_port = 0;
		return 1;  
	}  
}

std::string getIpFromHttpResponse(std::string& response)
{
    if (!response.empty())
    {
        // 去掉尾部空白行
        response.erase(response.find_last_not_of("\r\n\r\n") + 1);
        // 获取最后一行
        std::string::size_type pos = response.find_last_of("\r\n\r\n");
        bool isGetIp = pos > 0 && pos < response.length();
        response = isGetIp ? response.substr(pos, response.length()) : "";
        // 判断是否是合法的ip
        int tmp1, tmp2, tmp3, tmp4;
        int i = sscanf(response.c_str(), "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
        // 若ip非法则返回空
        if (i != 4 || tmp1 > 255 || tmp2 > 255 || tmp3 > 255 || tmp4 > 255)
        {
            response = "";
        }
        else
        {
            char tmp[20];
            snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d", tmp1, tmp2, tmp3, tmp4);
            response = tmp;
        }
    }
    return response;
}

bool  CTerminalinfo::get_Romeaddr()
{
	SOCKET hSocket = INVALID_SOCKET;
	do 
	{
        string url[3] = { "bot.whatismyipaddress.com", "api.ipify.org", "icanhazip.com" }; //  备用：icanhazip.com // api.ipify.org // bot.whatismyipaddress.com
        std::string strNetIp = "";
        
        for (int i = 0; i < 3; i++)
        {
            sockaddr_in  sockAddr;
            hSocket = socket(AF_INET, SOCK_STREAM, 0);

            //socket设置为非阻塞 
            unsigned long iMode = 1;
            if (ioctlsocket(hSocket, FIONBIO, &iMode) < 0) {
                printf("ioctlsocket failed: %d\n", WSAGetLastError());
                shutdown(hSocket, 2 /*SD_BOTH*/);
                closesocket(hSocket);
                hSocket = INVALID_SOCKET;
                continue;
            }   
        
		    memset(&sockAddr,0,sizeof(sockAddr));
		    sockAddr.sin_family = AF_INET;
		    sockAddr.sin_port = htons(80);

		    hostent* lphost;
		    lphost = gethostbyname(url[i].c_str());
		    if (lphost == NULL)
		    {
			    shutdown(hSocket,2 /*SD_BOTH*/ );
			    closesocket(hSocket);
			    hSocket = INVALID_SOCKET;
                continue;
		    }
		    sockAddr.sin_addr.s_addr = ((in_addr*)lphost->h_addr)->s_addr;

		    int nRet = connect(hSocket, (struct sockaddr FAR *) &sockAddr, sizeof(sockAddr));
		    if(nRet)
		    {
                if (nRet < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    int err = WSAGetLastError();
                    shutdown(hSocket, 2 /*SD_BOTH*/);
                    closesocket(hSocket);
                    hSocket = INVALID_SOCKET;
                    continue;
                }
                else
                {
                    fd_set readfds, writefds;
                    struct timeval timeout;
                    timeout.tv_sec = 10;
                    timeout.tv_usec = 0;  /* 10 seconds AND 0 microseconds */

                    FD_ZERO(&readfds);
                    FD_ZERO(&writefds);

                    FD_SET(hSocket, &readfds);
                    FD_SET(hSocket, &writefds);

                    nRet = select(FD_SETSIZE, &readfds, &writefds, NULL, &timeout);
                    if (nRet == SOCKET_ERROR)
                    {
                        int err = WSAGetLastError();
                        printf("select(): %d\n", err);
                        shutdown(hSocket, 2 /*SD_BOTH*/);
                        closesocket(hSocket);
                        hSocket = INVALID_SOCKET;
                        continue;
                    }
                    else
                    {
                        int error = -1, ret = 0;
                        int optLen = sizeof(int);
                        getsockopt(hSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);

                        if (0 != error)
                        {
                            ret = -1; // 有错误  
                            shutdown(hSocket, 2 /*SD_BOTH*/);
                            closesocket(hSocket);
                            hSocket = INVALID_SOCKET;
                            continue;
                        }
                    }
                }
		    }

            // 设回为阻塞socket  
            iMode = 0;
            ioctlsocket(hSocket, FIONBIO, (u_long FAR*)&iMode); //设置为阻塞模式  

		    char buffer[1024]={0};
		
            sprintf(buffer,"GET / HTTP/1.0\r\n"
						    "Host:%s\r\n"
			    		    "Connection:keep-alive\r\n"
						    "Cache-Control:max-age=0\r\n"
						    "Accept:*/*\r\n"
						    "User-Agent: Mozilla/5.0 (Windows NT 5.2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36\r\n"
						    "Accept-Encoding:gzip,deflate,sdch\r\n"
						    "Accept-Language:zh-CN,zh;q=0.8\r\n\r\n", url[i].c_str()
						    );

		    send(hSocket, buffer,strlen(buffer), 0);

		    memset(buffer,0,sizeof(buffer));

		    int nReceiveLen = recv(hSocket, buffer,sizeof(buffer),0);
		    if (nReceiveLen == 0) break;
        
            // printf("recv: %s\n%s\n", url[i].c_str(), buffer);
            strNetIp = getIpFromHttpResponse(std::string(buffer)); 

            shutdown(hSocket, 2 /*SD_BOTH*/);
            closesocket(hSocket);
            hSocket = INVALID_SOCKET;

            if (strNetIp.length() > 0)
            {
                break;
            }
        }

        if (strNetIp.length() > 0)
        {
            strcpy(m_szTerinf[eIIP], strNetIp.c_str());
            return true;
        }
        else
        {
            strcpy(m_szTerErrinf[eIIP], "外网错误");
            return false;
        }
	} while (0);

	if( hSocket != INVALID_SOCKET )
	{
		shutdown(hSocket,2 /*SD_BOTH*/ );
		closesocket(hSocket);
	}

	hSocket = INVALID_SOCKET;
	return false;
}


bool  CTerminalinfo::get_addrbyConn(const char * szRomeAddr,int nRomePort,bool bRealtime)
{
	
	SOCKET hSocket = INVALID_SOCKET;
	do 
	{
		sockaddr_in  sockAddr;
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		// 	const char* szAddr ;int nPort;
		memset(&sockAddr,0,sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(szRomeAddr);
		sockAddr.sin_port = htons(nRomePort);
		do 
		{
			if (sockAddr.sin_addr.s_addr != INADDR_NONE) break;
			hostent* lphost;
			lphost = gethostbyname(szRomeAddr);
			if (lphost == NULL)
			{
				shutdown(hSocket,2 /*SD_BOTH*/ );
				closesocket(hSocket);
				hSocket = INVALID_SOCKET;
				strcpy(m_szTerErrinf[eIIP],"外网错误");
				break;
			}
			sockAddr.sin_addr.s_addr = ((in_addr*)lphost->h_addr)->s_addr;
		} while (0);
		if(hSocket == INVALID_SOCKET) break;
		#define SO_CONNECT_TIME 0x700C
		int timeout=3000;
		int nRet=setsockopt(hSocket,SOL_SOCKET,SO_CONNECT_TIME,(char *)&timeout,sizeof(timeout));  
// 		if(nRet==SOCKET_ERROR)  
// 		{  
// 			strcpy(m_szTerErrinf[eIIP],"外网错误");
// 			break;
// 		}
		nRet = connect(hSocket, (struct sockaddr FAR *) &sockAddr, sizeof(sockAddr));
		if(nRet)
		{
			strcpy(m_szTerErrinf[eIIP],"外网错误");
			sockAddr.sin_addr.s_addr = inet_addr(m_szTerinf[eIIP]);
			if(sockAddr.sin_addr.s_net == 10 || sockAddr.sin_addr.s_net == 192 || sockAddr.sin_addr.s_net == 172 )
			{
				m_szTerinf[eIIP][0] = 0x00;
			}
			break;
			//shutdown(hSocket,2 /*SD_BOTH*/ );
			//closesocket(hSocket);
			//hSocket = INVALID_SOCKET;
			//return false;
		}
		struct sockaddr_in sa;
		int len = sizeof(sa);
		// 内网地址
		if(!getsockname(hSocket, (struct sockaddr *)&sa, &len))
		{
			m_szTerErrinf[eLIP][0]=0x00;
			sprintf_s(m_szTerErrinf[eLIP],sizeof(m_szTerErrinf[eLIP]),"%d.%d.%d.%d",sa.sin_addr.s_net, sa.sin_addr.s_host,sa.sin_addr.s_lh,  sa.sin_addr.s_impno);
			if(stricmp(m_szTerErrinf[eLIP],m_szTerinf[eLIP]))
			{
				sprintf_s(m_szTerinf[eLIP],sizeof(m_szTerinf[eLIP]),"%d.%d.%d.%d",sa.sin_addr.s_net, sa.sin_addr.s_host,sa.sin_addr.s_lh,  sa.sin_addr.s_impno);
				m_szTerErrinf[eLIP][0]=0x00;
				m_szTerinf[eMAC][0] = 0x00;
				get_locaddr();
			}
			m_szTerErrinf[eLIP][0]=0x00;
		}
		if(sockAddr.sin_addr.s_net == 10 || sockAddr.sin_addr.s_net == 192 || sockAddr.sin_addr.s_net == 172 )
		{// 内网 地址
			if( hSocket != INVALID_SOCKET )
			{
				shutdown(hSocket,2 /*SD_BOTH*/ );
				closesocket(hSocket);
			}
			hSocket = INVALID_SOCKET;
			m_szTerErrinf[eIIP][0]=0x00;
			sprintf_s(m_szTerinf[eIIP],sizeof(m_szTerinf[eIIP]),"%d.%d.%d.%d",sa.sin_addr.s_net, sa.sin_addr.s_host,sa.sin_addr.s_lh,  sa.sin_addr.s_impno);
			return true;
		}
		if(m_szTerinf[eIIP][0] == 0) break;
		sockAddr.sin_addr.s_addr = inet_addr(m_szTerinf[eIIP]);
		if(sockAddr.sin_addr.s_net == 10 || sockAddr.sin_addr.s_net == 192 || sockAddr.sin_addr.s_net == 172 )
		{
			m_szTerinf[eIIP][0] = 0x00;
		}
	} while (0);

	if( hSocket != INVALID_SOCKET )
	{
		shutdown(hSocket,2 /*SD_BOTH*/ );
		closesocket(hSocket);
	}
	hSocket = INVALID_SOCKET;
	if( m_szTerinf[eIIP][0] && !bRealtime ) return true;
	//strcpy(m_szTerErrinf[eIIP],"不支持");
	
	if( get_Romeaddr() ) return true;
	return true;
	//SOCKET             sockRaw=INVALID_SOCKET;  
	struct sockaddr_in dest;   
	int bread, timeout=1000,ret;     
	USHORT     seq_no=0;  
	//创建套接字  
	hSocket=WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL,0,WSA_FLAG_OVERLAPPED);  
	if(hSocket==INVALID_SOCKET)  
	{  
		strcpy(m_szTerErrinf[eIIP],"套接字错");
		//printf("WSASocket() failed:%d\n",WSAGetLastError());  
		return false;
	}  
	//对锁定套接字设置超时  
	bread=setsockopt(hSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));  
	if(bread==SOCKET_ERROR)  
	{  
		//printf("setsockopt(SO_RCVTIMEO) failed:%d\n",WSAGetLastError());  
		closesocket(hSocket);
		strcpy(m_szTerErrinf[eIIP],"套接字错");
		return false;
	}  
	timeout=1000;  
	bread=setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout));  
	if(bread==SOCKET_ERROR)  
	{  
		//printf("setsockopt(SO_SNDTIMEO) failed:%d\n",WSAGetLastError()); 
		closesocket(hSocket);
		strcpy(m_szTerErrinf[eIIP],"套接字错");
		return false;
	}  
	//解析目标地址，将主机名转化为IP地址  
	memset(&dest,0,sizeof(dest));  
	dest.sin_family=AF_INET;  
	if((dest.sin_addr.S_un.S_addr=inet_addr(szRomeAddr))==INADDR_NONE)  
	{  
		struct hostent    *hp=NULL; 
		if((hp=gethostbyname(szRomeAddr))!=NULL)  
		{  
			memcpy(&(dest.sin_addr.S_un.S_addr),hp->h_addr_list[0],hp->h_length);  
			dest.sin_family=hp->h_addrtype;  
			//printf("dest.sin_addr=%s\n",inet_ntoa(dest.sin_addr));  
		}  
		else  
		{  
			//printf("gethostbyname() failed:%d\n",WSAGetLastError()); 
			strcpy(m_szTerErrinf[eIIP],"连接外地址错误");
			closesocket(hSocket);
			return false;
		}  
	}  
	//Create the ICMP pakcet  
	char  icmp_data[44]={0};
	char  recvbuf[1024]={0};
	/*icmp_data= (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,MAX_PACKET);  
	recvbuf  = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,MAX_PACKET);  
	if(!icmp_data)  
	{  
		printf("HeapAlloc() failed: %d\n",GetLastError());  
		return -1;  
	}  
	memset(icmp_data,0,MAX_PACKET);  */
	IcmpHeader *icmp_hdr=NULL;  
	char       *datapart=NULL;  
	icmp_hdr=(IcmpHeader *)icmp_data;  
	icmp_hdr->i_type=ICMP_ECHO;     //request an ICMP echo  
	icmp_hdr->i_code=0;  
	icmp_hdr->i_id=(USHORT)GetCurrentProcessId();  
	icmp_hdr->i_cksum=0;  
	icmp_hdr->i_seq=0;  
	datapart=icmp_data+sizeof(IcmpHeader);  
	memset(datapart,'E',sizeof(icmp_data)-sizeof(IcmpHeader)); 
	int ns_impno = dest.sin_addr.s_impno;// 默认的 
	//开始发送/接收ICMP报文  
	struct sockaddr_in from;
	//memset(from,0x00,sizeof(from));
	int fromlen =sizeof(from);
	int nTTL = 0;
	int nCount = 0;
	for(int i=1;i<=255;i++)  
	{  
		int bwrote;  
		//设置IP包的生存期  
		ret=setsockopt(hSocket,IPPROTO_IP,IP_TTL,(char *)&i,sizeof(int));  
		//if(ret==SOCKET_ERROR)  
		//{  
		//	printf("setsockopt(IP_TTL) failed:%d\n",WSAGetLastError());  
		//}  
		((IcmpHeader *)icmp_data)->i_cksum =0;         
		((IcmpHeader *)icmp_data)->i_seq=seq_no++;     //Sequence number of ICMP packets  
		((IcmpHeader *)icmp_data)->i_cksum=checksum((USHORT *)icmp_data,sizeof(icmp_data));  
		//发送ICMP包请求查询  
		//cStartTickCount=GetTickCount();  
		bwrote=sendto(hSocket,icmp_data,sizeof(icmp_data),0,(struct sockaddr *)&dest,sizeof(dest));  
		if(bwrote==SOCKET_ERROR)  
		{  
			if(WSAGetLastError()==WSAETIMEDOUT)  
			{  
				printf("timed out\n");  
				continue;  
			}  
			//printf("sendto() failed:%d\n",WSAGetLastError());  
			closesocket(hSocket);
			strcpy(m_szTerErrinf[eIIP],"连接外地址错误");
			return false;  
		}  
		//if(bwrote<sizeof(icmp_data))  
		//{  
		//	printf("Wrote %d bytes\n",bwrote);  
		//}  
		//接收ICMP回复包  
		bread=recvfrom(hSocket,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&from,&fromlen);  
		if(bread==SOCKET_ERROR)  
		{  
			if(WSAGetLastError()==WSAETIMEDOUT)  
			{  
				/*DecodeIPHeader(NULL,0,NULL);*/  
				continue;  
			}  
			//printf("recvfrom() failed:%d\n",WSAGetLastError());  
			closesocket(hSocket);
			strcpy(m_szTerErrinf[eIIP],"ICMP不支持");
			return false; 
		}  
		nCount++;
		if(DecodeIPHeader(recvbuf,bread, &from))  
		{
			if( nTTL == 0 )
			{
				if(nCount< 3 ) break;
				nTTL = nCount;
				if(from.sin_port == 2)
				{
					memcpy(&dest.sin_addr,&from.sin_addr,sizeof(from.sin_addr));
					ns_impno = dest.sin_addr.s_impno;
					
				}
				dest.sin_addr.s_impno--;
				i = 1;
				seq_no = 0;
				nCount = 0;
				//printf(" 重新目标    %-15s \n", inet_ntoa(dest.sin_addr) );  
			}else
			{
				if( nCount < nTTL && from.sin_port != 0) break;  
				dest.sin_addr.s_impno--;
				if( dest.sin_addr.s_impno == ns_impno  ) break;
				if( dest.sin_addr.s_impno == 0 )
				{
					dest.sin_addr.s_impno= 254;
				}
				i = 1;
				//printf(" 重新目标    %-15s \n", inet_ntoa(dest.sin_addr) );  
				seq_no = 0;
				nCount = 0;
			}
		}else if( nTTL)
		{
			if((nTTL-nCount) == 1)
			{
				dest.sin_addr.s_impno--;
				if( dest.sin_addr.s_impno == ns_impno  ) break;
				if( dest.sin_addr.s_impno == 0 )
				{
					dest.sin_addr.s_impno= 254;
				}
				i = 1;
				//printf(" 重新目标    %-15s \n", inet_ntoa(dest.sin_addr) );  
				seq_no = 0;
				nCount = 0;
			}
		}
		Sleep(10);  
	}  
	//system("pause");  
	m_szTerErrinf[eIIP][0]=0x00;
	if(hSocket!=INVALID_SOCKET)  
		closesocket(hSocket);  
	return true;
}

/// 应用程序名称
bool CTerminalinfo::getAppName()
{
    int nTryTime = 0;
    while (0x00 == m_szTerinf[eAPP][0] && nTryTime < 10)
    {
        memset(m_szBuffer, 0x00, sizeof(m_szBuffer));
        ::GetModuleFileNameA((HMODULE)::GetModuleHandle(NULL), m_szBuffer, sizeof(m_szBuffer) - 1);

        strrev(m_szBuffer);

        unsigned char* szPos;
        szPos = (UCHAR*)strstr(m_szBuffer, "\\");
        if (szPos)
        {
            *szPos = 0x00;
            szPos = NULL;
        }

        strrev(m_szBuffer);
        sprintf_s(m_szTerinf[eAPP], sizeof(m_szTerinf[eAPP]), "%s", m_szBuffer);

        ++nTryTime;
    }

    return m_szTerinf[eAPP][0];
}

/// CPU序列号 20    BFEBFBFF000306A9
bool CTerminalinfo::getCpuInfo()
{
    INT32 dwBuf[4];
    __cpuidex(dwBuf, 1, 1);
    sprintf_s(m_szTerinf[eCPU], sizeof(m_szTerinf[eCPU]), "%08X%08X", dwBuf[3], dwBuf[0]);

    return true;
}

/// 取磁盘信息
bool CTerminalinfo::getDiskInfo()
{
    // 获取系统目录
    char szDir[512] = { 0 };
    GetSystemDirectoryA(szDir, sizeof(szDir));
    szDir[3] = 0x00;

    // 分区卷标信息
    DWORD  nVolSerial = 0;
    char   szVolName[256] = { 0 };
    char   szFileSysName[256] = { 0 };
    BOOL bRet = GetVolumeInformationA(szDir, szVolName, sizeof(szVolName), &nVolSerial, NULL, NULL, szFileSysName, sizeof(szFileSysName));
    if (!bRet)
    {
        return false;
    }

    // 磁盘空间：磁盘根路径,可用空间，总空间，剩余空间
    ULARGE_INTEGER nTotalFreeSpaceOfBytes;
    GetDiskFreeSpaceExA(szDir, NULL, &nTotalFreeSpaceOfBytes, NULL);

    // 分区序列号
    sprintf(m_szTerinf[eVS], "%s", fmtVolSerial(nVolSerial));

    // 分区分区信息
    szDir[1] = 0x00;
    sprintf(m_szTerinf[ePI], "%c^%s^%s^%dG", szDir[0], m_szTerinf[eVS], szFileSysName, nTotalFreeSpaceOfBytes.QuadPart >> 30);

    // 磁盘信息
    getHardDriveComputerID(szDir[0]);
    TrimStart(m_szTerinf[eHD]);
    return true;
}

/// 格式化分区序号
const char* CTerminalinfo::fmtVolSerial(DWORD nVolSerial)
{
    memset(m_szBuffer, 0, sizeof(m_szBuffer));

    sprintf(m_szBuffer, "%08X", nVolSerial);
    for (int i = 7; i > 3; --i)
        m_szBuffer[i + 1] = m_szBuffer[i];
    m_szBuffer[4] = '-';

    return m_szBuffer;
}

/// 检查当前系统是否是虚拟机
bool CTerminalinfo::CheckVirtualMachine()
{
    return chkVM(m_szTerinf[eHDCAP]);
}


