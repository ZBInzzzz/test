// test6.cpp : �������̨Ӧ�ó������ڵ㡣
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
    in.open(SourceFile);//��Դ�ļ�
    if (in.fail())//��Դ�ļ�ʧ��
    {
        cout << "Error 1: Fail to open the source file." << endl;
        in.close();
        out.close();
        return false;
    }
    out.open(NewFile, ios::binary);//����Ŀ���ļ� 
    if (out.fail())//�����ļ�ʧ��
    {
        cout << "Error 2: Fail to create the new file." << endl;
        out.close();
        in.close();
        return false;
    }
    else//�����ļ�
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
//    /* _access(char *,int) �ж��ļ��Ƿ����
//    ���� ����0;������ ����-1.
//    _access(const char *path,int mode)
//    mode��ֵ:
//    00 �Ƿ����
//    02 дȨ��
//    04 ��Ȩ��
//    06 ��дȨ��
//    */
//    if (!_access(SourceFile, 0))//����ļ�����:�ļ�Ϊֻ���޷�ɾ��
//    {
//        //ȥ���ļ�ֻ������
//        SetFileAttributes(wideSourceFile, 0);
//        if (DeleteFile(wideSourceFile))//ɾ���ɹ�
//        {
//            cout << wideSourceFile << " �ѳɹ�ɾ��." << endl;
//            return true;
//        }
//        else//�޷�ɾ��:�ļ�ֻ������Ȩ��ִ��ɾ��
//        {
//            cout << wideSourceFile << " �޷�ɾ��:�ļ�Ϊֻ�����Ի���ɾ��Ȩ��." << endl;
//            return false;
//        }
//    }
//    else//�ļ�������
//    {
//        cout << wideSourceFile << " ������,�޷�ɾ��." << endl;
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

    //��ȡ��ǰ��������·��
    char pszFileName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, pszFileName, MAX_PATH);
    //��ȡ��ǰ��������Ŀ¼
    (strrchr(pszFileName, '\\'))[0] = 0;

    while (1)
    {
        // ȡ��ǰ����

        time_t t;   struct tm *ptm;
        time(&t);
        ptm = localtime(&t);
        sprintf(szCurrDataTime, "%04d%02d%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
 

        if (strcmp(szCurrDataTime, szPrevInitDateTime) == 0)
        {
            //Sleep(1000 );
            Sleep(1000 * 60 * 5); //5���Ӽ��һ��
            continue;
        }

        cout << "��ǰʱ��: " << szCurrDataTime << endl;

        for (int i = 0; i < 3; i++)
        {
            sprintf(szSourceFilePath, "%s\\%s%s", pszFileName,szSourceFileName[i], szSourceFileSuffix[i]);
            sprintf(szDestFilePath, "%s/SZ/%s_%s%s", pszFileName,szSourceFileName[i], szCurrDataTime,szSourceFileSuffix[i]);

            // �����ļ���ָ��Ŀ¼
            bRetCopy = CopySourceFile(szSourceFilePath, szDestFilePath);
            if (!bRetCopy)
            {
                break;
            }
                
            // ɾ��ԴĿ¼�µ��ļ�
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

            cout << "���Ʋ�ɾ���ļ�["<< szSourceFileName[i]<<"]�ɹ���" << endl;

        }

      

        if (!bRetCopy || !bRetDelete)
        {
            cout << "�����ļ�[" << bRetCopy << "]��ɾ���ļ�[" << bRetDelete << "]ʧ�ܣ�" << endl;
            //Sleep(1000);
            Sleep(1000 * 60 * 5); //5���Ӽ��һ��
            continue;
        }
           

       
        cout << "------------------------------------" << endl << endl;
        strcpy(szPrevInitDateTime, szCurrDataTime);

       
    }
    return 0;
}

