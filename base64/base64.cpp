// base64.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

typedef struct sa {

	int a;
	int b;
}SA;

class MyClass
{
public:
	MyClass();
	~MyClass();

	public:
	int a;
	SA s;
	SA* ps;
};

MyClass::MyClass()
{
	ps = &s;
}

MyClass::~MyClass()
{
}

void fun2(char *p)
{
	cout << "2" << endl;
}



int main()
{


	/*char day1[9] = "20200812";
	char day2[9] = "20200813";

	uint32_t lday1 = atoi(day1);
	uint32_t lday2 = atoi(day2);
	uint64_t lday = 0;
	lday = lday1;
	lday = (lday << 32);
	lday |= lday2;

	cout << lday1 << lday2<<endl;
	cout << lday << endl;*/


	int32_t my_int32_left = 20200812;
	int32_t my_int32_right = 20200813;
	int64_t my_int64 = 0;
	//拼凑
	my_int64 = ((int64_t)my_int32_left << 32)+ my_int32_right;
	//my_int64 = (my_int64 << 32);
	//my_int64 |= my_int32_right;

	cout << my_int64<<endl;

	//拆分
	my_int32_right = my_int64;
	my_int32_left = my_int64 >> 32;

	cout << my_int32_left << endl;
	cout << my_int32_right << endl;
	system("pause");
    return 0;
}

