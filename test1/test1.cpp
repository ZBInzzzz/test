// test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "InitSock.h" 
#include <iostream>
#include <string>

using namespace std;
CInitSock initSock;     // 初始化Winsock库 

/*!
* \brief 根据域名获取ip
* WIN32
*
* \param char * domain 需要解析的域名
* \return const char * 解析结果
*/
void getDomainIP(const char * domain, char * ip)
{
	char ** pptr = NULL;
	int i = 0;
	char szHostName[256] = {};
	struct hostent * pHostEntry = gethostbyname(domain);
	if (pHostEntry != NULL)
	{
		char szIpBuff[32] = { 0 };
		for (i = 1, pptr = pHostEntry->h_addr_list; *pptr != NULL; ++pptr)
		{
			memset(szIpBuff, 0, sizeof(szIpBuff));
			const char* szIpRet = inet_ntop(pHostEntry->h_addrtype, *pptr, szIpBuff, sizeof(szIpBuff));
			// inet_ntop的返回值为NULL，则表示失败，否则返回相应的IP地址（此时szIpRet指向的是szIpBuff）
			if (szIpBuff != NULL)
			{
				memcpy(ip, szIpRet, sizeof(char[16]));
			}
		}
	}
	else
	{
		printf("gethostbyname error for host: %d\n", WSAGetLastError());
	}
}

/*!
* \brief 从http响应中获取ip
*
* \param std::string response
* \return std::string
*/
std::string getIpFromHttpResponse(std::string response)
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
			cout<<( "%s", response.c_str());
		}
	}
	return response;
}

/*!
* \brief 获取IP请求结果
* WIN32
*
* \return std::string 请求相应
*/
std::string httpRequestIp(const char * domain)
{
	char ipAddress[16];
	memset(ipAddress, 0, sizeof(char[16]));
	getDomainIP(domain, ipAddress);
	char tmpReq[200];
	sprintf(tmpReq, "GET http://%s/ HTTP/1.1\r\nHOST: %s\r\n\r\n", domain, domain);
	std::string request = (std::string)tmpReq;
	int port = 80;

	std::string response;
	int resp_leng;
	char buffer[1024];
	int sock;

#ifdef WIN32

	struct sockaddr_in serveraddr;
	WSADATA wsaData;

	//init winsock
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		cout<<("WSAStartup() failed");

	//open socket
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		cout<<("socket() failed");

	//connect
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipAddress);
	serveraddr.sin_port = htons((unsigned short)port);
	if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		cout<<( "connect() failed");

	//send request
	if (send(sock, request.c_str(), request.length(), 0) != request.length())
		cout<<( "send() sent a different number of bytes than expected");


#else
	int error;
	struct sockaddr_in addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		HW_WRITE_LOG(log_error, "Error 01: creating socket failed!");
		return "";
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ipAddress, &addr.sin_addr); //this should be the ip of the server I'm contacting

	error = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (error != 0)
	{
		HW_WRITE_LOG(log_error, "Error 02: conecting to server failed!");
		return "";
	}

	send(sock, request.c_str(), request.length(), 0);

#endif

	//get response
	response = "";
	resp_leng = 1024;
	int revlen = 0;
	while (resp_leng == 1024)
	{
		resp_leng = recv(sock, (char*)&buffer, 1024, 0);
		revlen += resp_leng;
		if (resp_leng > 0)
			response += std::string(buffer).substr(0, resp_leng);
	}

	std::string answer(response);

#if _WIN32
	WSACleanup();
#endif
	answer = getIpFromHttpResponse(answer);
	return answer;
}
/*!
* \brief 获取本机公网IP
*
* \param char * outBuf 保存结果
* \return void
*/
std::string GetPublicIP() {
	//char * domain = "checkip.amazonaws.com";
	//std::string answer = httpRequestIp(domain);
	// 从多个http域名获取机器使用的外网ip，若获取失败则使用后面的其他域名
	//std::string domains[3] = { "bot.whatismyipaddress.com", "api.ipify.org", "icanhazip.com" };
	std::string domains[3] = { "bot.whatismyipaddress.com", "api.ipify.org", "www.baidu.com" };
	std::string answer = "";
	int num = sizeof(domains) / sizeof(domains[0]);
	for (int i = 0; i <= num - 1; i++)
	{
		const char * domain = domains[i].c_str();
		answer = httpRequestIp(domain);
		if (!answer.empty())
		{
			break;
		}
	}

	return answer;
}


int main()
{
	
	string ip = GetPublicIP();
	cout << ip << endl;
	system("pause");
    return 0;
}

