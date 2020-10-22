// test2.cpp : �������̨Ӧ�ó������ڵ㡣
//�����ǲ��� tgw_op_site_format.h �ļ���ʹ��
 
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "kdencodecli.h"
#include "tgw_op_site_format.h"


using namespace std;

class Point {
	int m_x, m_y;
public:
	Point(int x, int y) : m_x(x), m_y(y) {}
	void display() {
		cout << "(x1=" << m_x << ", x2=" << m_y << ")" << endl;
	}
	// ��һ��const��ʾ����ֵ�Ǹ�������������ֹ (p1+p2)=Point(10,20); ���ָ�ֵ
	// �����ڵڶ���const��ʾ����Ĳ������Խ���constҲ���Խ��շ�const
	// ������const��ʾ�������Ե��øú���	
	// ���������ķ�����
	const Point operator+(const Point &point) const {
		return Point(m_x + point.m_x, m_y + point.m_y);
	}
};

int main()
{
	CTGWOpSiteFormat format;
	string strSrc = "PC;IIP=123.112.154.118;MAC=3065EC93E537;HD=KN2560L012549005F5HC;PCN=ZJS-PC;CPU=BFEBFBFF000306D4;PI=C^0004-2CC4^NTFS^78G;VOL=0004-2CC4;VER=666;IPORT=50361@KINGPB;WINDOWS.6.1;PBCLIENT;3.3.1.19";

	//string strFormat = "PC;IIP=$IIP;IPORT=$PORT;LIP=$LIP;MAC=$MAC;HD=$HD;PCN=$PCN;CPU=$CPU;PI=$PI;VOL=$VOL@SFPT;$VER";
	string strFormat = "$IIP@";

	string s = format.SeparateFormat(strSrc.c_str(), strFormat.c_str(), ';', true);
	cout << s << endl;

	system("pause");
	return 0;
}

