// test5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include<iostream>
#include <string>
#include <vector>
#include <fstream>  //文件流库函数
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

	    //第二种删除
		first = s.begin();
		second = s.begin();
		//second += 4;
	     s.erase(first, second);
	     //第三种删除
		    // s.erase(8);
	     cout << "删除后 set 中元素是 ：";
	     for (iter = s.begin(); iter != s.end(); ++iter)
		     {
		         cout << *iter << " ";
		     }
	     cout << endl;
	

		 system("pause");
    return 0;
}

