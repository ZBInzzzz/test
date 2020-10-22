#pragma once

#ifndef __TERMINALINFO_LHH__
#define __TERMINALINFO_LHH__
/******************************************************************************
* �� �� ��:terminalinfo.h  terminalinfo.cpp
* ģ�����ƣ�CTerminalinfo
* ����˵����  
* ��    �ߣ�luohh
* �������ڣ� 2015��10��14�� 13:48:22
* �� �� �ţ�1.0
* �޸���ʷ��
*
* �޸�����       ����            ����
* ---------------------------------------------------------------------------
*******************************************************************************/

#include <winioctl.h>
//����IP������IP��MAC��ַ��Ӳ�����кš����������CPU���кš�Ӳ�̷�����Ϣ����ע��Ϣ��
//����IP	LIP	LAN IP��ȡֵ����IP
//����IP	IIP	Internet IP��ȡֵ������IP
//MAC��ַ	MAC	
//Ӳ�����к�	HD	Hard Disk��ȡֵӲ�����к�
//CPU���к�	CPU	ȡֵCPU���к�
//�������	PCN	Personal Computer Name
//Ӳ�̷�����Ϣ	PI	Partition Information��ȡֵ����ϵͳ���ڴ��̷�����Ϣ
//��ע��Ϣ	NT	˵����ȡ��Ϣʧ��ԭ�����Ϣ����ȷԭ�򡣸�ʽΪ��
//NT:��ʶ��Ϣ1[��ȡʧ��ԭ��],��ʶ��Ϣ2[��ȡʧ��ԭ��],����
//IP	IPV4	15	192.168.123.125
//IPV6	39	FE80:0000:0000:0000:AAAA:0000:00C2:0002
//MAC��ַ	16	00000000000000E0
//�������	20	zhang.xc
//Ӳ�����к�	20	TF655AY91GHRVL
//CPU���к�	20	BFEBFBFF000306A9
//Ӳ�̷�����Ϣ	20	C,FAT32,80

class CTerminalinfo
{
public:
	CTerminalinfo(void);
	~CTerminalinfo(void);

    /// ��Ϣ����
    enum InfoIndex
    {
        eLIP = 0,       // ����IP
        eIIP,           // ����IP
        eMAC,           // MAC��ַ
        eHD,            // Ӳ�����к�
        eCPU,           // CPU���к�
        ePCN,           // �������
        ePI,            // Ӳ�̷�����Ϣ
        eAPP,           // Ӧ�ó���
        eHDCAP,         // Ӳ���ͺ�
        eIPORT,         // �����˿�
        eVS,            // �������
        eIdxEnd
    } EInfoIdx;

	// 100 ��ȡ���е�mac������ 101 ��ȡ���е�mac��ip 102 ��ȡ��Ӧmac��ip
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
            // ������nRetbufSize�����ܻ�>512,�Ӷ����¡�stack around the varialbe "szAllmacIp" was corrupted�����쳣
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
			strncpy(szRetInf,"���Բ�֧��",nRetbufSize-1);
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

    /// Ӧ�ó�������
    bool getAppName();
    /// CPU��Ϣ
    bool getCpuInfo();
    /// ȡ������Ϣ
    bool getDiskInfo();
    /// ��ʽ���������
    const char* fmtVolSerial(DWORD nVolSerial);

private:
	char m_szTerinf[eIdxEnd][50];
	char m_szTerErrinf[eIdxEnd][50];
	char m_szAllmac[4096];
	char m_szAllmacIp[4096];
    char m_szBuffer[1024];
};
#endif
