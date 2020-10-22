#pragma once
/******************************************************************************
*																																							
*                                  交易平台 V1.0                      					
*																																							
*           Copyright (C) 2009-2020, SHENZHEN KINGDOM Co., Ltd.								
*																																							
*                             All Rights Reserved.															
*																																							
* ===========================================================================	
*																																							
* 文 件 名: tgw_op_site_format.h  tgw_op_site_format.cpp																			
* 模块名称：CTGWOpSiteFormat
* 功能说明：  																																	
* 作    者：技术研究部																													
* 创建日期：2017年6月21日
* 版 本 号：1.0																																
* 修改历史：																																		
*																																							
* 修改日期       姓名            内容																					
* ---------------------------------------------------------------------------	
* 2017年6月21日  技术研究部      初始版本						
*																																							
*******************************************************************************/
#include <string>

#define TGW_VAR_LIP		"$LIP"		//内网IP		LIP	LAN IP，取值内网IP
#define TGW_VAR_IP		"$IP"		//为了兼容旧版，等价于$LIP
#define TGW_VAR_IIP		"$IIP"		//公网IP		IIP	Internet IP，取值互联网IP
#define TGW_VAR_MAC		"$MAC"		//MAC地址		MAC
#define TGW_VAR_HD		"$HD"		//硬盘序列号	HD	Hard Disk，取值硬盘序列号
#define TGW_VAR_CPU		"$CPU"		//CPU序列号		CPU	取值CPU序列号
#define TGW_VAR_PCN		"$PCN"		//计算机名		PCN	Personal Computer Name
#define TGW_VAR_PI		"$PI"		//硬盘分区信息	PI	Partition Information，取值操作系统所在磁盘分区信息
#define TGW_VAR_NT		"$NT"		//备注信息		NT	说明获取信息失败原因或信息不正确原因。格式为：NT : 标识信息1[获取失败原因], 标识信息2[获取失败原因], ……
#define TGW_VAR_VOL     "$VOL"      //系统盘卷标号  VOL
#define TGW_VAR_OSV     "$OSV"      //操作系统版本  OSV
#define TGW_VAR_VER     "$VER"      //pb系统版本    VER
#define TGW_VAR_ICN     "$ICN"      //操作渠道      ICN
#define TGW_VAR_DID     "$DID"      //系统卷盘标号  DID
#define TGW_VAR_PORT	"$PORT"		//端口号		  IPORT

/* 终端信息解析规则
	以;分隔，按标志信息(如IIP: 或 IIP=)提取后台送过来的站点信息值，替换格式串中的内部变量var 如($IP)。
	@后的值为扩充值，扩展值约定以@开头，放在末尾。提取后组装到信息字符串末尾。
	
举例:
   后台送的格式串:PC;IIP=127.0.0.1;IPORT=50361;MAC:123@KINGPB;WINDOWS.6.1;PBCLIENT;3.3.1.19
   IIP:$IIP;MAC:MAC:$MAC； --> IIP:127.0.0.1;MAC:123@KINGPB;WINDOWS.6.1;PBCLIENT;3.3.1.19
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
			//如果已经转义则跳过
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
			char * var;		//内部变量
			char * tag;		//标识信息
			char * end;		//分隔符
		};

		//老版站点信息格式
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

		//新版本站点信息格式
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

		// 提取扩展值，扩展值约定以@开头，放在末尾
		size_t nExtraIndex = strSrc.rfind('@');
		size_t nForMatIndex = m_strOpSite.rfind('@');

		// 如果配置里有@，认为使用自定义的扩展信息，不再使用后台扩展信息
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

			//格式信息小写字母转换成大写字母
			transform(strNew.begin(), strNew.end(), strNew.begin(), ::toupper);

			if(bIsTranMean)
			{
				tranferMeaning(strNew);
			}

			m_strOpSite.replace(inder, strlen(pInfo->var), strNew.c_str());
		}

		// 扩展值放在末尾
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
			char * var;		//内部变量
			char * tag;		//标识信息
			char * end;		//分隔符
		};

		//老版站点信息格式
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

		//新版本站点信息格式
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

		// 提取扩展值，扩展值约定以@开头，放在末尾
		size_t nExtraIndex = strSrc.rfind('@');
		size_t nForMatIndex = m_strOpSite.rfind('@');

		// 如果配置里有@，认为使用自定义的扩展信息，不再使用后台扩展信息
		bool bUseGivenExtra = (nForMatIndex == std::string::npos && nExtraIndex != std::string::npos);

		// 使用后台传来的扩展信息
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
		//在端口号和'@'之间加上分号';'
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

			//格式信息小写字母转换成大写字母
			transform(strNew.begin(), strNew.end(), strNew.begin(), ::toupper);
			
			//对站点信息值进行转义
			if (bIsTranMean)
			{
				tranferMeaning(strNew);
			}

			Replace(m_strOpSite, pInfo->var, strNew.c_str(), cSep);
		}

		// 扩展值放在末尾
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