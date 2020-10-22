// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HDTERMINAL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HDTERMINAL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#if defined(WIN32)
	#ifdef HDTERMINAL_EXPORTS
		#define HDTERMINAL_API __declspec(dllexport)
	#else
		#define HDTERMINAL_API __declspec(dllimport)
	#endif
		#define  HDTERMINAL_STD_CALL 
#else
	#define  HDTERMINAL_API 
	#define  HDTERMINAL_STD_CALL /*__stdcall*/
#endif
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _MSC_VER

/*************************************************
功能描述：取终端信息前需要做的动作，忒别是外网地址的 时候。
功能函数: GetaddrbyConn
参数说明：
输入：(const char * szRomeAddr,int nRomePort)
参数1 连接的远端地址
参数2 连接的远端端口
输出：
0 成功 其他 失败
历史变更:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL  GetaddrbyConn(const char * szRomeAddr,int nRomePort,bool bRealtime=false);

/*************************************************
功能描述：取终端信息
功能函数: GetTerminfo
参数说明：
输入：(int Idx,char* szRetInf,int nRetbufSize)
参数1 eLIP	= 0,//内网IP
eIIP ,//公网IP
eMAC ,//MAC地址
eHD	 ,//硬盘序列号
eCPU ,//CPU序列号
ePCN ,//计算机名
ePI ,//硬盘分区信息
eAPP,// 应用程序
eHDCAP,// 硬盘型号
参数2 信息值buf区
参数3 信息buf的size
输出：
0 成功 其他 失败
历史变更:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL GetTerminfo(int Idx,char* szRetInf,int nRetbufSize);

/*************************************************
功能描述：取终端对应 信息错误原因
功能函数: GetTermErrinfo
参数说明：
输入：(int Idx,char* szRetInf,int nRetbufSize)
参数1 eLIP	= 0,//内网IP
eIIP ,//公网IP
eMAC ,//MAC地址
eHD	 ,//硬盘序列号
eCPU ,//CPU序列号
ePCN ,//计算机名
ePI ,//硬盘分区信息
eAPP,// 应用程序
eHDCAP,// 硬盘型号
参数2 错误buf区
参数3 错误buf的size
输出：
0 成功 其他 失败
历史变更:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL GetTermErrinfo(int Idx,char* szRetInf,int nRetbufSize);

/*************************************************
功能描述：检查当前操作系统是否是虚拟机环境
功能函数: CheckVirtualMachine
参数说明：无
输出：
0 非虚拟机环境 其他 虚拟机环境
历史变更:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL CheckVirtualMachine();

#ifdef __cplusplus
}
#endif
#endif
