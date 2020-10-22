#pragma once

#ifndef __TERMINALINFO_LHH__
#define __TERMINALINFO_LHH__
/******************************************************************************
* 文 件 名:terminalinfo.h  terminalinfo.cpp
* 模块名称：CTerminalinfo
* 功能说明：  
* 作    者：luohh
* 创建日期： 2015年10月14日 13:48:22
* 版 本 号：1.0
* 修改历史：
*
* 修改日期       姓名            内容
* ---------------------------------------------------------------------------
*******************************************************************************/

#include <winioctl.h>
//内网IP、公网IP、MAC地址、硬盘序列号、计算机名、CPU序列号、硬盘分区信息、备注信息。
//内网IP	LIP	LAN IP，取值内网IP
//公网IP	IIP	Internet IP，取值互联网IP
//MAC地址	MAC	
//硬盘序列号	HD	Hard Disk，取值硬盘序列号
//CPU序列号	CPU	取值CPU序列号
//计算机名	PCN	Personal Computer Name
//硬盘分区信息	PI	Partition Information，取值操作系统所在磁盘分区信息
//备注信息	NT	说明获取信息失败原因或信息不正确原因。格式为：
//NT:标识信息1[获取失败原因],标识信息2[获取失败原因],……
//IP	IPV4	15	192.168.123.125
//IPV6	39	FE80:0000:0000:0000:AAAA:0000:00C2:0002
//MAC地址	16	00000000000000E0
//计算机名	20	zhang.xc
//硬盘序列号	20	TF655AY91GHRVL
//CPU序列号	20	BFEBFBFF000306A9
//硬盘分区信息	20	C,FAT32,80

class CTerminalinfo
{
public:
	CTerminalinfo(void);
	~CTerminalinfo(void);

    /// 信息索引
    enum InfoIndex
    {
        eLIP = 0,       // 内网IP
        eIIP,           // 公网IP
        eMAC,           // MAC地址
        eHD,            // 硬盘序列号
        eCPU,           // CPU序列号
        ePCN,           // 计算机名
        ePI,            // 硬盘分区信息
        eAPP,           // 应用程序
        eHDCAP,         // 硬盘型号
        eIPORT,         // 外网端口
        eVS,            // 分区序号
        eIdxEnd
    } EInfoIdx;

	// 100 获取所有的mac和域名 101 获取所有的mac和ip 102 获取对应mac的ip
	bool GetTerminfo(int Idx,char* szRetInf,int nRetbufSize)
	{
        memset(szRetInf, 0, nRetbufSize);

		if((Idx>= eIdxEnd || eLIP> Idx) && (103< Idx || 100> Idx)) 
			return false;

        if (Idx == eIIP)
        {
            if (strlen(m_szTerinf[Idx]) == 0)
            {
                if (!get_Romeaddr())
                {
                    memset(m_szTerinf[Idx], 0, sizeof(m_szTerinf[Idx]));
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
		else if(Idx < 100 )
		{
			Idx %= eIdxEnd;
			if(0x00 == m_szTerinf[Idx][0]) 
				return false;
		}
		char szMacIp[512] = {0};
		if(szRetInf)
		{
			if (Idx == 102)
				strcpy(szMacIp,szRetInf);
			*szRetInf = 0;
		}
		if (Idx == 100)
			strncpy(szRetInf,m_szAllmac,nRetbufSize-1);
		else if (Idx == 101)
			strncpy(szRetInf,m_szAllmacIp,nRetbufSize-1);
		else if (Idx == 102)
		{
			char szAllmacIp[512] = {0};
            // 不能用nRetbufSize，可能会>512,从而导致“stack around the varialbe "szAllmacIp" was corrupted”的异常
			strncpy(szAllmacIp, m_szAllmacIp, sizeof(szAllmacIp) - 1);
            szAllmacIp[sizeof(szAllmacIp) - 1] = 0;

			char* szRetIp = strstr(szAllmacIp, szMacIp);
			if (szRetIp != NULL)
			{
				strcpy(szRetIp, szRetIp+strlen(szMacIp)+1);
			}
			char* szIp = strstr(szRetIp,";");
			if (szIp != NULL)
			{
				size_t p = szIp - szRetIp;
				strncpy(szRetInf, szRetIp, p);
			}
			else
                strncpy(szRetInf,szRetIp,nRetbufSize-1);
        }
		else
			strncpy(szRetInf,m_szTerinf[Idx],nRetbufSize-1);

        szRetInf[nRetbufSize - 1] = 0;
		return true;
	}

	bool GetTermErrinfo(int Idx,char* szRetInf,int nRetbufSize)
	{
		if(Idx>= eIdxEnd || eLIP> Idx) 
		{
			strncpy(szRetInf,"属性不支持",nRetbufSize-1);
			return false;
		}

		Idx %= eIdxEnd;
		
		if(szRetInf)
			*szRetInf = 0;
		
		if(0x00 == m_szTerErrinf[Idx][0]) 
			return false;
		
		strncpy(szRetInf,m_szTerErrinf[Idx],nRetbufSize-1);
		return true;
	}
	bool get_addrbyConn(const char * szRomeAddr,int nRomePort,bool bRealtime);

    bool CheckVirtualMachine();
private:
	bool get_locaddr();
	bool get_Romeaddr();
	
	USHORT  checksum(USHORT *buffer,int size);
	
	char *ConvertToString (DWORD diskdata [256],int firstIndex,	int lastIndex,char* buf);
	
	void PrintIdeInfo (int drive, DWORD diskdata [256]);
	
	BOOL DoIDENTIFY (HANDLE, PSENDCMDINPARAMS, PSENDCMDOUTPARAMS, BYTE, BYTE,PDWORD);		
	
	int  ReadPhysicalDriveInNTWithAdminRights (char cSysDrive);
	
	int  ReadPhysicalDriveInNTUsingSmart (char cSysDrive);
	
	char *  flipAndCodeBytes (const char * str,int pos,int flip,char * buf);
	
	int  ReadPhysicalDriveInNTWithZeroRights (char cSysDrive);
	
	int  ReadDrivePortsInWin9X (void);
	
	int  ReadIdeDriveAsScsiDriveInNT (char cSysDrive);
	
	long getHardDriveComputerID (char cSysDrive);
	
	int  DecodeIPHeader(char *buf,int bytes,struct sockaddr_in *from);  

    /// 应用程序名称
    bool getAppName();
    /// CPU信息
    bool getCpuInfo();
    /// 取磁盘信息
    bool getDiskInfo();
    /// 格式化分区序号
    const char* fmtVolSerial(DWORD nVolSerial);

private:
	char m_szTerinf[eIdxEnd][50];
	char m_szTerErrinf[eIdxEnd][50];
	char m_szAllmac[4096];
	char m_szAllmacIp[4096];
    char m_szBuffer[1024];
};
#endif
