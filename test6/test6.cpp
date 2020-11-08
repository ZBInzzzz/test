// test6.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <time.h>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <io.h>

using namespace std;


bool CopySourceFile(char *SourceFile, char *NewFile)
{
    ifstream in;
    ofstream out;
    in.open(SourceFile);//打开源文件
    if (in.fail())//打开源文件失败
    {
        cout << "Error 1: Fail to open the source file." << endl;
        in.close();
        out.close();
        return false;
    }
    out.open(NewFile, ios::binary);//创建目标文件 
    if (out.fail())//创建文件失败
    {
        cout << "Error 2: Fail to create the new file." << endl;
        out.close();
        in.close();
        return false;
    }
    else//复制文件
    {
        out << in.rdbuf();
        out.close();
        in.close();
        return true;
    }
}

//bool DeleteSourceFile(char *SourceFile)
//{
//    int num = MultiByteToWideChar(0, 0, SourceFile, -1, NULL, 0);
//    wchar_t *wideSourceFile = new wchar_t[num];
//    MultiByteToWideChar(0, 0, SourceFile, -1, wideSourceFile, num);
//
//    /* _access(char *,int) 判断文件是否存在
//    存在 返回0;不存在 返回-1.
//    _access(const char *path,int mode)
//    mode的值:
//    00 是否存在
//    02 写权限
//    04 读权限
//    06 读写权限
//    */
//    if (!_access(SourceFile, 0))//如果文件存在:文件为只读无法删除
//    {
//        //去掉文件只读属性
//        SetFileAttributes(wideSourceFile, 0);
//        if (DeleteFile(wideSourceFile))//删除成功
//        {
//            cout << wideSourceFile << " 已成功删除." << endl;
//            return true;
//        }
//        else//无法删除:文件只读或无权限执行删除
//        {
//            cout << wideSourceFile << " 无法删除:文件为只读属性或无删除权限." << endl;
//            return false;
//        }
//    }
//    else//文件不存在
//    {
//        cout << wideSourceFile << " 不存在,无法删除." << endl;
//        return false;
//    }
//
//}

int main()
{
    char szPrevInitDateTime[64] = { 0x00 }, szCurrDataTime[64] = { 0x00 };

    char szSourceFilePath[64] = { 0x00 }, szDestFilePath[64] = { 0x00 };

    char szSourceFileName[3][64] = { "securities","indexinfo","hkexreff04" };
    char szSourceFileSuffix[3][10] = { ".xml",".xml",".txt" };

    bool bRetCopy = false, bRetDelete = false;

    //获取当前程序所在路径
    char pszFileName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, pszFileName, MAX_PATH);
    //获取当前程序所在目录
    (strrchr(pszFileName, '\\'))[0] = 0;

    while (1)
    {
        // 取当前日期

        time_t t;   struct tm *ptm;
        time(&t);
        ptm = localtime(&t);
        sprintf(szCurrDataTime, "%04d%02d%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
 

        if (strcmp(szCurrDataTime, szPrevInitDateTime) == 0)
        {
            //Sleep(1000 );
            Sleep(1000 * 60 * 5); //5分钟检测一次
            continue;
        }

        cout << "当前时间: " << szCurrDataTime << endl;

        for (int i = 0; i < 3; i++)
        {
            sprintf(szSourceFilePath, "%s\\%s%s", pszFileName,szSourceFileName[i], szSourceFileSuffix[i]);
            sprintf(szDestFilePath, "%s/SZ/%s_%s%s", pszFileName,szSourceFileName[i], szCurrDataTime,szSourceFileSuffix[i]);

            // 复制文件到指定目录
            bRetCopy = CopySourceFile(szSourceFilePath, szDestFilePath);
            if (!bRetCopy)
            {
                break;
            }
                
            // 删除源目录下的文件
            if (remove(szSourceFilePath) != 0)
            {
                bRetDelete = false;
                break;
            }
            bRetDelete = true;
           // bRetDelete = DeleteSourceFile(szSourceFilePath);
            //if (!bRetDelete)
            //{
            //    break;
            //}

            cout << "复制并删除文件["<< szSourceFileName[i]<<"]成功！" << endl;

        }

      

        if (!bRetCopy || !bRetDelete)
        {
            cout << "复制文件[" << bRetCopy << "]或删除文件[" << bRetDelete << "]失败！" << endl;
            //Sleep(1000);
            Sleep(1000 * 60 * 5); //5分钟检测一次
            continue;
        }
           

       
        cout << "------------------------------------" << endl << endl;
        strcpy(szPrevInitDateTime, szCurrDataTime);

       
    }
    return 0;
}

