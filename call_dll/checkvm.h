// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string>
#include <algorithm>

#include <DXGI.h>
#define _WIN32_DCOM 
#include <objbase.h>
#include <comdef.h>
#include <wbemidl.h>

using namespace std;
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "dxgi.lib")
// 具体可以参考 http://www.hackdig.com/08/hack-48126.htm#15_CPUID


std::string WStringToString(const std::wstring &wstr)
{
    std::string str;
    int nLen = (int)wstr.length();
    str.resize(nLen, ' ');
    int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
    if (nResult == 0)
    {
        return "";
    }
    return str;
}

/// 检查VMWare_BIOS
bool chkVMW_BiosUuid()
{
    bool bRet = FALSE;
    HRESULT hr = S_OK;
    BSTR bstrNamespace =(BSTR)  L"root\\cimv2";
    BSTR bstrQuery = (BSTR)L"SELECT * FROM Win32_ComputerSystemProduct";
    VARIANT vVal;
    ULONG uEnumRet = 0;
    IWbemLocator* pWbemLocator = NULL;
    IWbemServices* pServices = NULL;
    IEnumWbemClassObject* pEnum = NULL;
    IWbemClassObject* pObj = NULL;
    
    do
    {
        hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
            RPC_C_AUTHN_LEVEL_CONNECT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL, EOAC_NONE, 0);
        if (FAILED(hr))
        {
            break;
        }

        hr = CoCreateInstance(CLSID_WbemLocator, NULL,
            CLSCTX_INPROC_SERVER, IID_IWbemLocator,
            (void**)&pWbemLocator);
        if (FAILED(hr))
        {
 
            break;
        }

        hr = pWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, NULL,
            0, NULL, NULL, &pServices);
        if (FAILED(hr))
        {
             break;
        }

        pWbemLocator->Release();
        pWbemLocator = NULL;
 
        hr = pServices->ExecQuery((const BSTR)L"WQL", bstrQuery, 0, NULL, &pEnum);
        if (FAILED(hr))
        {
             break;
        }

        hr = pEnum->Reset();
        hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &uEnumRet);
        while (S_OK == hr)
        {
            hr = pObj->Get(L"Vendor", 0, &vVal, NULL, NULL);
            hr = pObj->Get(L"Manufacturer", 0, &vVal, NULL, NULL);
            wprintf(L"Manufacturer is: [%s]\n", vVal.bstrVal);
            string a = WStringToString(vVal.bstrVal);
            transform(a.begin(), a.end(), a.begin(), ::toupper);
            bRet = true;
            if (a.find("BOCHS ") != string::npos)  break;
            if (a.find("XEN") != string::npos)  break;
            if (a.find("SEABIOS ") != string::npos)  break;
            if (a.find("PARALLELS ") != string::npos)  break;
            if (a.find("INNOTEK ") != string::npos)  break;
            bRet = false;
            break;
        }
    } while (0);
 
    if (pObj)
        pObj->Release();

    if (pEnum)
        pEnum->Release();

    if (pServices)
        pServices->Release();

    if (pWbemLocator)
        pWbemLocator->Release();

    CoUninitialize();
    return bRet;
}

/// 检查VMWare_CUP特权指令
bool chkVMW_SpecInstruct()
{
#ifndef _X64_
    bool rc = true;
    __try
    {
        __asm

        {
            push   edx
            push   ecx
            push   ebx
            mov    eax, 'VMXh'
            mov    ebx, 0           // 将ebx设置为非幻数’VMXH’的其它值
            mov    ecx, 10          // 指定功能号，用于获取VMWare版本，当它为0x14时用于获取VMware内存大小
            mov    edx, 'VX'        // 端口号
            in     eax, dx          // 从端口dx读取VMware版本到eax

            //若上面指定功能号为0x14时，可通过判断eax中的值是否大于0，若是则说明处于虚拟机中
            cmp    ebx, 'VMXh'      // 判断ebx中是否包含VMware版本’VMXh’，若是则在虚拟机中
            setz[rc]                // 设置返回值
            pop    ebx
            pop    ecx
            pop    edx
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)  //如果未处于VMware中，则触发此异常
    {
        rc = false;
    }

    return rc;
#else
    return false;
#endif
}

/// 检查虚拟机显卡名称（其中有虚拟机标识）
bool chkVM_GPU()
{
    // 参数定义  
    IDXGIFactory* pFactory;
    IDXGIAdapter* pAdapter;
    int iAdapterNum = 0;

    // 创建一个DXGI工厂  
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

    if (FAILED(hr))
        return  false;

    // 枚举适配器  
    while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC adapterDesc;
        pAdapter->GetDesc(&adapterDesc);
        wstring aa(adapterDesc.Description);
        std::string bb = WStringToString(aa);
        std::transform(bb.begin(), bb.end(), bb.begin(), ::toupper);
        if (bb.find("VMWARE ") != string::npos) return true;
        if (bb.find("VIRTUAL ") != string::npos) return true;
        if (bb.find("VIRTUALBOX ") != string::npos) return true;
        if (bb.find("PARALLELS ") != string::npos) return true;
        ++iAdapterNum;
    }

    return  false;
}

/// 查找指定虚拟机进程  
bool chkVM_Process(const CHAR* strProcessName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    //遍历进程快照。轮流显示每个进程的信息  
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        if (0 == _tcsicmp(pe32.szExeFile, strProcessName))
            break;
        bMore = ::Process32Next(hProcessSnap, &pe32);
    }
    //清除snapshot对象  
    ::CloseHandle(hProcessSnap);
    return bMore == TRUE;
}

bool chkVM_Disk(const char* szDiskInfo)
{
    char szDiskModel[256] = { 0 };
    strcpy(szDiskModel, szDiskInfo);

    // 转换为小些
    strlwr(szDiskModel);

    if (strnicmp(szDiskModel, "vmware ", sizeof("vmware")) == 0 || strnicmp(szDiskModel, "vbox ", sizeof("vbox")) == 0 ||
        strnicmp(szDiskModel, "virtual ", sizeof("virtual")) == 0 || strnicmp(szDiskModel, "qemu ", sizeof("qemu")) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool chkVM(const char* szDiskInfo)
{
    if (chkVM_Disk(szDiskInfo)) return true;
    if (chkVMW_BiosUuid()) return true;
    if (chkVMW_SpecInstruct()) return true;
    if (chkVM_Process("prl-tool-service.exe")) return true;
    if (chkVM_Process("VBoxService.exe")) return true;
    if (chkVM_Process("Vmtoolsd.exe")) return true;

    return false;
}
 