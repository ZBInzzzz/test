// test3.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <limits.h>
#include <Windows.h>

#define  _CRT_SECURE_NO_WARNINGS

using namespace std;
int main()
{
//	time_t t;
//	struct tm *ptminfo;
//	time(&t);
//	ptminfo = localtime(&t);
//	printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
//		ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
//		ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
//
//
//	time_t t;
//	struct tm tmlocal;
//	t = time(&t);
//#ifdef _MSC_VER
//	localtime_s(&tmlocal, &t);
//#else
//	localtime_r(&t, &tmlocal);
//


	//printf("%s  %d\n", __FILE__, __LINE__);

	//long    i = 10000000L;
	//clock_t start, finish;
	//double duration;
	///* ����һ���¼�������ʱ��*/
	//printf("Time to do %ld empty loops is ", i);
	//start = clock();
	//while (i--);
	//finish = clock();
	//duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//duration = (double)(finish - start) ;
	//printf("%f seconds\n", duration);



	//unsigned int data, *point;
	//    point = &data;//��ȡ�������ݵĵ�ַ��������point��
	//     data = 0;
	// * (int *)point = 0x22;
	//    if (data == 0x22)
	//	     {
	//	       cout << "�˴�������С�ˣ�" << endl;
	//	     }
	//    else  if (data == 0x22000000)
	//	    {
	//        cout << "�˴������Ǵ�ˣ�" << endl;
	//	  }
 //   else
	//    {
	//         cout << "��ʱ�޷��жϻ������ͣ�" << endl;
	//    }



	union
	{
		  char str;
		  int data;
	};
	data = 0x01020304;
	if (str == 0x01)
	{
		cout << "�˻����Ǵ�ˣ�" << endl;
	}     
	else if (str == 0x04) 
	{
		cout << "�˻�����С�ˣ�" << endl;	
	}
    else 
	{
		cout << " ���޷��жϴ˻������ͣ�" << endl;
	}

	system("pause");
    return 0;
}

