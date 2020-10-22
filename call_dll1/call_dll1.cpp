// call_dll1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "HdTerminal.h"
//#include "terminalinfo.h"

#include <iostream>


using namespace std;

int main()
{
	char t[1024] = "";
	//if (0 != GetaddrbyConn("192.168.20.125", 8500))
	//	cout << "error1";
	if (0 != GetTerminfo(1,t,1024))
		cout << "error";

	cout <<"t: "<< t << endl;
	system("pause");
    return 0;
}

