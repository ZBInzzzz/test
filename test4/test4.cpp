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

int					m_nCount;
//int nResult = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nDesSize, NULL, NULL);
void doReader(string s)
{
	worksheet				m_pCurrWorkSheet;

	//m_pExcel->load("C:\\Users\\zheng\\Desktop\\123.xlsx");

	//m_pCurrWorkSheet = m_pExcel->active_sheet();
	//m_pCurrWorkSheet = m_pExcel->create_sheet();

	//m_pCurrWorkSheet.title("abc");

	//m_pExcel->save("test.xlsx");
	if (m_pExcel->contains(s))
	{
		worksheet pWk = m_pExcel->active_sheet();
		if (!strcmp(s.c_str(), "Sheet1") && m_pExcel->sheet_count() == 1 && pWk.title() == "Sheet1")
		{
			m_pCurrWorkSheet = pWk;
		}
		else
		{
			m_pCurrWorkSheet = m_pExcel->create_sheet();
		}

		m_pCurrWorkSheet.title(s);
	}


	m_pExcel->save("C:\\Users\\zheng\\Desktop\\1234.xlsx");

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
		doReader("Sheet1");
		doReader("Sheet2");
		doReader("Sheet3");

		Sleep(500);
	}
}
