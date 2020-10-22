// test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <vector>
using namespace std;


int smallLarge(vector<int> arr)
{
	int n = arr.size();

	int *rightMin = new int[n];  //��¼ԭʼ����arr[i]�ұߣ������Լ�������Сֵ
	int leftMax;  //��¼arr[i]��ߣ������Լ��������ֵ
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
		if (leftMax == rightMin[i])   //������������е�����������ұ����е���С��
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

