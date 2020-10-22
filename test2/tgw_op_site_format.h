#pragma once
/******************************************************************************
*																																							
*                                  ����ƽ̨ V1.0                      					
*																																							
*           Copyright (C) 2009-2020, SHENZHEN KINGDOM Co., Ltd.								
*																																							
*                             All Rights Reserved.															
*																																							
* ===========================================================================	
*																																							
* �� �� ��: tgw_op_site_format.h  tgw_op_site_format.cpp																			
* ģ�����ƣ�CTGWOpSiteFormat
* ����˵����  																																	
* ��    �ߣ������о���																													
* �������ڣ�2017��6��21��
* �� �� �ţ�1.0																																
* �޸���ʷ��																																		
*																																							
* �޸�����       ����            ����																					
* ---------------------------------------------------------------------------	
* 2017��6��21��  �����о���      ��ʼ�汾						
*																																							
*******************************************************************************/
#include <string>

#define TGW_VAR_LIP		"$LIP"		//����IP		LIP	LAN IP��ȡֵ����IP
#define TGW_VAR_IP		"$IP"		//Ϊ�˼��ݾɰ棬�ȼ���$LIP
#define TGW_VAR_IIP		"$IIP"		//����IP		IIP	Internet IP��ȡֵ������IP
#define TGW_VAR_MAC		"$MAC"		//MAC��ַ		MAC
#define TGW_VAR_HD		"$HD"		//Ӳ�����к�	HD	Hard Disk��ȡֵӲ�����к�
#define TGW_VAR_CPU		"$CPU"		//CPU���к�		CPU	ȡֵCPU���к�
#define TGW_VAR_PCN		"$PCN"		//�������		PCN	Personal Computer Name
#define TGW_VAR_PI		"$PI"		//Ӳ�̷�����Ϣ	PI	Partition Information��ȡֵ����ϵͳ���ڴ��̷�����Ϣ
#define TGW_VAR_NT		"$NT"		//��ע��Ϣ		NT	˵����ȡ��Ϣʧ��ԭ�����Ϣ����ȷԭ�򡣸�ʽΪ��NT : ��ʶ��Ϣ1[��ȡʧ��ԭ��], ��ʶ��Ϣ2[��ȡʧ��ԭ��], ����
#define TGW_VAR_VOL     "$VOL"      //ϵͳ�̾���  VOL
#define TGW_VAR_OSV     "$OSV"      //����ϵͳ�汾  OSV
#define TGW_VAR_VER     "$VER"      //pbϵͳ�汾    VER
#define TGW_VAR_ICN     "$ICN"      //��������      ICN
#define TGW_VAR_DID     "$DID"      //ϵͳ���̱��  DID
#define TGW_VAR_PORT	"$PORT"		//�˿ں�		  IPORT

/* �ն���Ϣ��������
	��;�ָ�������־��Ϣ(��IIP: �� IIP=)��ȡ��̨�͹�����վ����Ϣֵ���滻��ʽ���е��ڲ�����var ��($IP)��
	@���ֵΪ����ֵ����չֵԼ����@��ͷ������ĩβ����ȡ����װ����Ϣ�ַ���ĩβ��
	
����:
   ��̨�͵ĸ�ʽ��:PC;IIP=127.0.0.1;IPORT=50361;MAC:123@KINGPB;WINDOWS.6.1;PBCLIENT;3.3.1.19
   IIP:$IIP;MAC:MAC:$MAC�� --> IIP:127.0.0.1;MAC:123@KINGPB;WINDOWS.6.1;PBCLIENT;3.3.1.19
*/

class CTGWOpSiteFormat
{
public:
	CTGWOpSiteFormat(){}
	~CTGWOpSiteFormat(){}

	void tranferMeaning(std::string& strValue)
	{
		//  : , ; '\'
		std::string strTranChar = ":;,\\";
		std::string strTransedArr[4] = { "\\:","\\;","\\,","\\\\" };
		size_t nIndex = -1;

		for (size_t i = 0; i < strValue.size(); i++)
		{
			//����Ѿ�ת��������
			if (strValue[i] == '\\' &&  i < strValue.size() - 1)
			{
				if (strTranChar.find(strValue[i + 1]) != std::string::npos)
				{
					i++;
					continue;
				}
			}

			nIndex = strTranChar.find(strValue[i]);

			if (nIndex >= 0 && nIndex <= 3)
			{
				strValue.replace(i, 1, strTransedArr[nIndex]);
				i += strTransedArr[nIndex].size() - 1;
			}
		}
	}

	inline const char * Format(const char * _szSrc, const char * _szFormat,bool bIsTranMean = true)
	{
		struct info
		{
			char * var;		//�ڲ�����
			char * tag;		//��ʶ��Ϣ
			char * end;		//�ָ���
		};

		//�ϰ�վ����Ϣ��ʽ
		static const info oldRelationTable[] = {
			{ TGW_VAR_LIP,	"LIP:",		";" },
			{ TGW_VAR_IP,	"LIP:",		";" },
			{ TGW_VAR_IIP,	"IIP:",		";" },
			{ TGW_VAR_MAC,	"MAC:",		";" },
			{ TGW_VAR_HD,	"HD:",		";" },
			{ TGW_VAR_CPU,	"CPU:",		";" },
			{ TGW_VAR_PCN,	"PCN:",		";" },
			{ TGW_VAR_PI,	"PI:",		";" },
			{ TGW_VAR_NT,	"NT:",		";" },
			{ TGW_VAR_VOL,	"VOL:",		";" },
			{ TGW_VAR_OSV,	"OSV:",		";" },
			{ TGW_VAR_VER,	"VER:",		";" },
			{ TGW_VAR_ICN,	"ICN:",		";" },
			{ TGW_VAR_DID,	"DID:",		";" },
			{ TGW_VAR_PORT,	"IPORT:",	";" }
		};

		//�°汾վ����Ϣ��ʽ
		static const info newRelationTable[] = {
			{ TGW_VAR_LIP,	"LIP=",		";" },
			{ TGW_VAR_IP,	"LIP=",		";" },
			{ TGW_VAR_IIP,	"IIP=",		";" },
			{ TGW_VAR_MAC,	"MAC=",		";" },
			{ TGW_VAR_HD,	"HD=",		";" },
			{ TGW_VAR_CPU,	"CPU=",		";" },
			{ TGW_VAR_PCN,	"PCN=",		";" },
			{ TGW_VAR_PI,	"PI=",		";" },
			{ TGW_VAR_NT,	"NT=",		";" },
			{ TGW_VAR_VOL,	"VOL=",		";" },
			{ TGW_VAR_OSV,	"OSV=",		";" },
			{ TGW_VAR_VER,	"VER=",		";" },
			{ TGW_VAR_ICN,	"ICN=",		";" },
			{ TGW_VAR_DID,	"DID=",		";" },
			{ TGW_VAR_PORT,	"IPORT=",	";" }
		};

		std::string strSrc	= _szSrc;
		m_strOpSite			= _szFormat;
		std::string strExtra;

		// ��ȡ��չֵ����չֵԼ����@��ͷ������ĩβ
		size_t nExtraIndex = strSrc.rfind('@');
		size_t nForMatIndex = m_strOpSite.rfind('@');

		// �����������@����Ϊʹ���Զ������չ��Ϣ������ʹ�ú�̨��չ��Ϣ
		bool bUseGivenExtra = (nForMatIndex == std::string::npos && nExtraIndex != std::string::npos);

		if (bUseGivenExtra)
		{
			strExtra = strSrc.substr(nExtraIndex, strSrc.size() - nExtraIndex);
			strSrc = strSrc.substr(0, nExtraIndex);
		}
		
		info * arrInfo		= (info*)newRelationTable;
		int    nArrSize		= _countof(newRelationTable);
		
		if (strSrc.find(oldRelationTable[0].tag) != std::string::npos 
			&& strSrc.find(oldRelationTable[0].end) != std::string::npos)
		{
			arrInfo = (info *)oldRelationTable;
			nArrSize = _countof(oldRelationTable);
		}

		if (strSrc.size() > 0 && strSrc[strSrc.size() - 1] != ';')
		{
			strSrc.append(";");
		}

		for (int i = 0; i < nArrSize; ++i)
		{
			const info * pInfo = &arrInfo[i];

			std::string::size_type inder = m_strOpSite.find(pInfo->var);
			if (inder == std::string::npos)
				continue;

			std::string::size_type inder_tag = strSrc.find(pInfo->tag);
			if (std::string::npos == inder_tag)
			{
				m_strOpSite.replace(inder, strlen(pInfo->var), "");
				continue;
			}

			size_t uTagLen = strlen(pInfo->tag);
			std::string::size_type inder_end = strSrc.find(pInfo->end, inder_tag + uTagLen);
			if (std::string::npos == inder_end)
				continue;

			std::string strNew(strSrc, inder_tag + uTagLen, inder_end - inder_tag - uTagLen);

			//��ʽ��ϢСд��ĸת���ɴ�д��ĸ
			transform(strNew.begin(), strNew.end(), strNew.begin(), ::toupper);

			if(bIsTranMean)
			{
				tranferMeaning(strNew);
			}

			m_strOpSite.replace(inder, strlen(pInfo->var), strNew.c_str());
		}

		// ��չֵ����ĩβ
		if (bUseGivenExtra)
			m_strOpSite.replace(m_strOpSite.size() - 1, 1, strExtra);

		return m_strOpSite.c_str();
	}

	inline bool  Replace(std::string & szRepstr, const char* chOld, const char* chNew, char cSep)
	{
		if (chOld != nullptr   &&   chNew != nullptr)
		{
			std::string::size_type index = szRepstr.find(chOld);
			if (index != std::string::npos)
			{
				if (chNew[0] == 0)
				{
					std::string::size_type nBegin = 0;
					std::string::size_type nEnd = szRepstr.size() - 1;
					for (int i = int(index); i > 0; i--)
					{
						if (szRepstr[i] == cSep)
						{
							nBegin = i;
							break;
						}
					}

					for (size_t j = index; j < szRepstr.size(); j++)
					{
						if (szRepstr[j] == cSep)
						{
							nEnd = j;
							break;
						}
					}
					if (nBegin == 0 || nEnd == szRepstr.size() - 1)
					{
						szRepstr.erase(nBegin, nEnd - nBegin + 1);
					}
					else
					{
						szRepstr.erase(nBegin, nEnd - nBegin);
					}
				}
				else
				{
					szRepstr.erase(index, strlen(chOld));
					szRepstr.insert(index, chNew);
				}
				return true;
			}
		}
		return false;
	}

	inline const char * SeparateFormat(const char * _szSrc, const char * _szFormat, char cSep, bool bIsTranMean = true)
	{
		struct info
		{
			char * var;		//�ڲ�����
			char * tag;		//��ʶ��Ϣ
			char * end;		//�ָ���
		};

		//�ϰ�վ����Ϣ��ʽ
		static const info oldRelationTable[] = {
			{ TGW_VAR_LIP,	"LIP:",		";" },
			{ TGW_VAR_IP,	"LIP:",		";" },
			{ TGW_VAR_IIP,	"IIP:",		";" },
			{ TGW_VAR_MAC,	"MAC:",		";" },
			{ TGW_VAR_HD,	"HD:",		";" },
			{ TGW_VAR_CPU,	"CPU:",		";" },
			{ TGW_VAR_PCN,	"PCN:",		";" },
			{ TGW_VAR_PI,	"PI:",		";" },
			{ TGW_VAR_NT,	"NT:",		";" },
			{ TGW_VAR_VOL,	"VOL:",		";" },
			{ TGW_VAR_OSV,	"OSV:",		";" },
			{ TGW_VAR_VER,	"VER:",		";" },
			{ TGW_VAR_ICN,	"ICN:",		";" },
			{ TGW_VAR_DID,	"DID:",		";" },
			{ TGW_VAR_PORT,	"IPORT:",	";" }
		};

		//�°汾վ����Ϣ��ʽ
		static const info newRelationTable[] = {
			{ TGW_VAR_LIP,	"LIP=",		";" },
			{ TGW_VAR_IP,	"LIP=",		";" },
			{ TGW_VAR_IIP,	"IIP=",		";" },
			{ TGW_VAR_MAC,	"MAC=",		";" },
			{ TGW_VAR_HD,	"HD=",		";" },
			{ TGW_VAR_CPU,	"CPU=",		";" },
			{ TGW_VAR_PCN,	"PCN=",		";" },
			{ TGW_VAR_PI,	"PI=",		";" },
			{ TGW_VAR_NT,	"NT=",		";" },
			{ TGW_VAR_VOL,	"VOL=",		";" },
			{ TGW_VAR_OSV,	"OSV=",		";" },
			{ TGW_VAR_VER,	"VER=",		";" },
			{ TGW_VAR_ICN,	"ICN=",		";" },
			{ TGW_VAR_DID,	"DID=",		";" },
			{ TGW_VAR_PORT,	"PORT=",	";" }
		};

		std::string strSrc = _szSrc;
		m_strOpSite = _szFormat;
		std::string strExtra;

		// ��ȡ��չֵ����չֵԼ����@��ͷ������ĩβ
		size_t nExtraIndex = strSrc.rfind('@');
		size_t nForMatIndex = m_strOpSite.rfind('@');

		// �����������@����Ϊʹ���Զ������չ��Ϣ������ʹ�ú�̨��չ��Ϣ
		bool bUseGivenExtra = (nForMatIndex == std::string::npos && nExtraIndex != std::string::npos);

		// ʹ�ú�̨��������չ��Ϣ
		if (bUseGivenExtra)
		{
			strExtra = strSrc.substr(nExtraIndex, strSrc.size() - nExtraIndex);
			strSrc = strSrc.substr(0, nExtraIndex);
		}
		else if ((m_strOpSite.size() - 1) == nForMatIndex)
			m_strOpSite[nForMatIndex] = '\0';

		
		info * arrInfo = (info*)newRelationTable;
		int    nArrSize = _countof(newRelationTable);

		if (strSrc.find(oldRelationTable[0].tag) != std::string::npos
			&& strSrc.find(oldRelationTable[0].end) != std::string::npos)
		{
			arrInfo = (info *)oldRelationTable;
			nArrSize = _countof(oldRelationTable);
		}
		//�ڶ˿ںź�'@'֮����Ϸֺ�';'
		if(nExtraIndex != std::string::npos)
			strSrc.insert(nExtraIndex, ";");

		if (strSrc.size() > 0 && strSrc[strSrc.size() - 1] != ';')
		{
			strSrc.append(";");
		}

		for (int i = 0; i < nArrSize; ++i)
		{
			const info * pInfo = &arrInfo[i];

			std::string::size_type inder = m_strOpSite.find(pInfo->var);
			if (inder == std::string::npos)
				continue;

			std::string::size_type inder_tag = strSrc.find(pInfo->tag);
			if (std::string::npos == inder_tag)
			{
				Replace(m_strOpSite, pInfo->var, "", cSep);
				continue;
			}

			size_t uTagLen = strlen(pInfo->tag);
			std::string::size_type inder_end = strSrc.find(pInfo->end, inder_tag + uTagLen);
			if (std::string::npos == inder_end)
				continue;

			std::string strNew(strSrc, inder_tag + uTagLen, inder_end - inder_tag - uTagLen);

			//��ʽ��ϢСд��ĸת���ɴ�д��ĸ
			transform(strNew.begin(), strNew.end(), strNew.begin(), ::toupper);
			
			//��վ����Ϣֵ����ת��
			if (bIsTranMean)
			{
				tranferMeaning(strNew);
			}

			Replace(m_strOpSite, pInfo->var, strNew.c_str(), cSep);
		}

		// ��չֵ����ĩβ
		if(bUseGivenExtra)
			m_strOpSite.replace(m_strOpSite.size() - 1, 1, strExtra);
		
		return m_strOpSite.c_str();
	}

	inline const char * GetOpSite()
	{
		return m_strOpSite.c_str();
	}

private:
	std::string m_strOpSite;
};