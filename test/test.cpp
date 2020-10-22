// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
using namespace std;


int smallLarge(vector<int> arr)
{
	int n = arr.size();

	int *rightMin = new int[n];  //记录原始数组arr[i]右边（包括自己）的最小值
	int leftMax;  //记录arr[i]左边（包括自己）的最大值
	rightMin[n - 1] = arr[n - 1];
	for (int i = n - 2; i >= 0; i--)
	{
		if (rightMin[i + 1] > arr[i])
			rightMin[i] = arr[i];
		else
			rightMin[i] = rightMin[i + 1];
	}
	leftMax = arr[0];
	for (int i = 0; i < n; i++)
	{
		if (arr[i] >= leftMax)
			leftMax = arr[i];
		if (leftMax == rightMin[i])   //该数是左边数中的最大数，是右边数中的最小数
			return i;
	}
	return -1;
}

int main()
{
	vector<int> v = { 1,2,3,1,2,0,5,6 };

	int t = smallLarge(v);
	if (t == -1)
		cout << "false";
	else
		cout << t << endl;
	system("pause");
    return 0;
}

