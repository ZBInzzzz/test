// test5.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include<iostream>
#include <string>
#include <vector>
#include <fstream>  //�ļ����⺯��
#include <cstdlib>
#include <ctime>
#include <set>

using namespace std;


int main()
{

	set<int> s;
 set<int>::const_iterator iter;
	     set<int>::iterator first;
	     set<int>::iterator second;

	    for (int i = 1; i <= 10; ++i)
		     {
		        s.insert(i);

				
		     }

	    //�ڶ���ɾ��
		first = s.begin();
		second = s.begin();
		//second += 4;
	     s.erase(first, second);
	     //������ɾ��
		    // s.erase(8);
	     cout << "ɾ���� set ��Ԫ���� ��";
	     for (iter = s.begin(); iter != s.end(); ++iter)
		     {
		         cout << *iter << " ";
		     }
	     cout << endl;
	

		 system("pause");
    return 0;
}

