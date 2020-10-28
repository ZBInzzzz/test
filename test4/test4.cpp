// test4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <iostream>
#include <string>
#include "xlnt.hpp"
#include <windows.h>
//#include "Ansi2Utf8.h"
using namespace std;
using namespace xlnt;

workbook*			m_pExcel;
worksheet				m_pCurrWorkSheet;
int					m_nCount;

bool				m_bSheet1;
// int nResult = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nDesSize, NULL, NULL);


int preNUm(unsigned char byte) {
	unsigned char mask = 0x80;
	int num = 0;
	for (int i = 0; i < 8; i++) {
		if ((byte & mask) == mask) {
			mask = mask >> 1;
			num++;
		}
		else {
			break;
		}
	}
	return num;
}


bool isUtf8(unsigned char* data, int len) {
	int num = 0;
	int i = 0;
	while (i < len) {
		if ((data[i] & 0x80) == 0x00) {
			// 0XXX_XXXX
			i++;
			continue;
		}
		else if ((num = preNUm(data[i])) > 2) {
			// 110X_XXXX 10XX_XXXX
			// 1110_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// preNUm() 返回首个字节8个bits中首�?0bit前面1bit的个数，该数量也是该字符所使用的字节数        
			i++;
			for (int j = 0; j < num - 1; j++) {
				//判断后面num - 1 个字节是不是都是10开
				if ((data[i] & 0xc0) != 0x80) {
					return false;
				}
				i++;
			}
		}
		else {
			//其他情况说明不是utf-8
			return false;
		}
	}
	return true;
}

bool isGBK(unsigned char* data, int len) {
	int i = 0;
	while (i < len) {
		if (data[i] <= 0x7f) {
			//编码小于等于127,只有一个字节的编码，兼容ASCII
			i++;
			continue;
		}
		else {
			//大于127的使用双字节编码
			if (data[i] >= 0x81 &&
				data[i] <= 0xfe &&
				data[i + 1] >= 0x40 &&
				data[i + 1] <= 0xfe &&
				data[i + 1] != 0xf7) {
				i += 2;
				continue;
			}
			else {
				return false;
			}
		}
	}
	return true;
}
typedef enum {
	GBK,
	UTF8,
	UNKOWN
} CODING;
//需要说明的是，isGBK()是通过双字节是否落在gbk的编码范围内实现的，
//而utf-8编码格式的每个字节都是落在gbk的编码范围内�?
//所以只有先调用isUtf8()先判断不是utf-8编码，再调用isGBK()才有意义
CODING GetCoding(unsigned char* data, int len) {
	CODING coding;
	if (isUtf8(data, len) == true) {
		coding = UTF8;
	}
	else if (isGBK(data, len) == true) {
		coding = GBK;
	}
	else {
		coding = UNKOWN;
	}
	return coding;
}

//Unicode 转 utf-8
bool WStringToString(const std::wstring &wstr, std::string &str)
{
	int nLen = (int)wstr.length();
	int nDesSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), nLen, NULL, 0, NULL, NULL);
	str.resize(nDesSize, '\0');

	int nResult = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nDesSize, NULL, NULL);

	if (nResult == 0)
	{
		//DWORD dwErrorCode = GetLastError();
		return FALSE;
	}

	return TRUE;
}


void doReader(string s)
{


	//m_pExcel->load("C:\\Users\\zheng\\Desktop\\123.xlsx");

	//m_pCurrWorkSheet = m_pExcel->active_sheet();
	//m_pCurrWorkSheet = m_pExcel->create_sheet();

	//m_pCurrWorkSheet.title("abc");

	//m_pExcel->save("test.xlsx");

	if (!strcmp(s.c_str(), "Sheet1"))
		m_bSheet1 = true;

	if (!m_pExcel->contains(s))
	{
		worksheet pWk = m_pExcel->active_sheet();
		if (  m_pExcel->sheet_count() == 1 && pWk.title() == "Sheet1" && !m_bSheet1)
		{
			m_pCurrWorkSheet = pWk;
		}
		else
		{
			m_pCurrWorkSheet = m_pExcel->create_sheet();
		}

		m_pCurrWorkSheet.title(s);
	}
	else
		m_pCurrWorkSheet = m_pExcel->sheet_by_title(s);


	m_pCurrWorkSheet.merge_cells(range_reference(1,1,1,3));

	// 

	//m_pCurrWorkSheet.cell(xlnt::cell_reference(1, 1)).value("ttttt");;

	//


	
	//worksheet ws = m_pExcel->active_sheet();

	//auto cols = ws.columns();		auto rows = ws.rows();

	//int ColLength = cols.length();	int RowLength = rows.length();

	//std::cout << "demo01有效列数为: " << ColLength << std::endl;
	//std::cout << "demo01有效行数为: " << RowLength << std::endl;

	//for (int i = 0; i < RowLength; i++)
	//{
	//	for (int j = 0; j < ColLength; j++)
	//	{
	//		//取得指定一行一列的值 下标从0开始 例如 0，0 表示的就是A1    0,1表示B1 表示第1列第0行
	//		std::string A1Value = rows[i][j].value<std::string>();

	//		std::cout << CAnsi2Utf8::Utf8ToAnsi(A1Value) << std::endl;
	//	}

	//	std::cout << "*********************************************" << std::endl;
	//}

	std::cout << "*****************************" << m_nCount++ << "*****************************" << std::endl;
}

void doWriter()
{
	try
	{
		workbook objWorkBook;

		worksheet pCurrWorkSheet = objWorkBook.active_sheet();
		pCurrWorkSheet.title(("ABC+测试+345"));

		int i = 1;

		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("整形测试*Interger"));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("字符串测试#String"));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("字符$Char"));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("浮点型%Float"));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("高精度&Double"));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value((""));
		pCurrWorkSheet.cell(cell_reference(i++, 1)).value(("空后面"));

		for (int nRow = 2; nRow <= 3; nRow++)
		{
			for (int nCol = 1; nCol <= i; nCol++)
			{
				switch (nCol)
				{
				case 1:
					pCurrWorkSheet.cell(cell_reference(nCol, nRow)).value(12345678 + nCol);
					break;
				case 2:
					pCurrWorkSheet.cell(cell_reference(nCol, nRow)).value(("我是一个字符串"));
					break;
				case 3:
					pCurrWorkSheet.cell(cell_reference(nCol, nRow)).value('A');
					break;
				case 4:
					pCurrWorkSheet.cell(cell_reference(nCol, nRow)).value(float(123.456));
					break;
				case 5:
					pCurrWorkSheet.cell(cell_reference(nCol, nRow)).value(double(123.456789));
					break;
				default:
					break;
				}
			}
		}

		objWorkBook.save("C:\\Users\\zheng\\Desktop\\Test.xlsx");
	}
	catch (xlnt::exception& exp)
	{
		cout << exp.what() << endl;
	}
	catch (...)
	{
	}
}

void main()
{
	m_pExcel = new workbook();

//	while (1)
	{
		char src[20] = { 0 };
		cin >> src;

		//char src[512] = "你好";
		int len = strlen(src);
		//printf("%s, len:%d\n",src, len);
		char dstgbk[512] = { 0 };
		char dstutf8[512] = { 0 };
		int nCoding = GetCoding((unsigned char*)src, len);

		cout << "coding:" << nCoding << endl;   //判断是否是utf-8


		//wchar_t * pUnicodeBuff = NULL;
		//int rlen = 0;
		//rlen = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, NULL);
		//pUnicodeBuff = new WCHAR[rlen + 1];  //为Unicode字符串空间
		//rlen = MultiByteToWideChar(CP_UTF8, 0, src, -1, pUnicodeBuff, rlen);
		//rlen = WideCharToMultiByte(936, 0, pUnicodeBuff, -1, NULL, NULL, NULL, NULL); //936 为windows gb2312代码页码
		//WideCharToMultiByte(936, 0, pUnicodeBuff, -1, dstgbk, rlen, NULL, NULL);
		//delete[] pUnicodeBuff;


		wchar_t * pUnicodeBuff = NULL;
		int rlen = 0;
		rlen = MultiByteToWideChar(936, 0, src, -1, NULL, NULL);
		pUnicodeBuff = new WCHAR[rlen + 1];  //为Unicode字符串空间
		rlen = MultiByteToWideChar(936, 0, src, -1, pUnicodeBuff, rlen);
		rlen = WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuff, -1, NULL, NULL, NULL, NULL); //936 为windows gb2312代码页码
		WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuff, -1, dstgbk, rlen, NULL, NULL);
		delete[] pUnicodeBuff;


		//中文处理
		std::string strDes;
		WStringToString(L"测试", strDes);

		doReader(dstgbk);
		doReader(dstgbk);
		//doReader(sCin);
		//doReader("Sheet1");
		doReader("Sheet3");
		//doReader("Sheet2");
	

		//doReader("Sheet4");
		m_pExcel->save("E:\\workSpace\\VS2015\\test\\Debug\\1.xlsx");
		Sleep(500);
	}
}
