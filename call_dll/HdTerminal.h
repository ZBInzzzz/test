// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HDTERMINAL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HDTERMINAL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

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
����������ȡ�ն���Ϣǰ��Ҫ���Ķ�����߯����������ַ�� ʱ��
���ܺ���: GetaddrbyConn
����˵����
���룺(const char * szRomeAddr,int nRomePort)
����1 ���ӵ�Զ�˵�ַ
����2 ���ӵ�Զ�˶˿�
�����
0 �ɹ� ���� ʧ��
��ʷ���:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL  GetaddrbyConn(const char * szRomeAddr,int nRomePort,bool bRealtime=false);

/*************************************************
����������ȡ�ն���Ϣ
���ܺ���: GetTerminfo
����˵����
���룺(int Idx,char* szRetInf,int nRetbufSize)
����1 eLIP	= 0,//����IP
eIIP ,//����IP
eMAC ,//MAC��ַ
eHD	 ,//Ӳ�����к�
eCPU ,//CPU���к�
ePCN ,//�������
ePI ,//Ӳ�̷�����Ϣ
eAPP,// Ӧ�ó���
eHDCAP,// Ӳ���ͺ�
����2 ��Ϣֵbuf��
����3 ��Ϣbuf��size
�����
0 �ɹ� ���� ʧ��
��ʷ���:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL GetTerminfo(int Idx,char* szRetInf,int nRetbufSize);

/*************************************************
����������ȡ�ն˶�Ӧ ��Ϣ����ԭ��
���ܺ���: GetTermErrinfo
����˵����
���룺(int Idx,char* szRetInf,int nRetbufSize)
����1 eLIP	= 0,//����IP
eIIP ,//����IP
eMAC ,//MAC��ַ
eHD	 ,//Ӳ�����к�
eCPU ,//CPU���к�
ePCN ,//�������
ePI ,//Ӳ�̷�����Ϣ
eAPP,// Ӧ�ó���
eHDCAP,// Ӳ���ͺ�
����2 ����buf��
����3 ����buf��size
�����
0 �ɹ� ���� ʧ��
��ʷ���:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL GetTermErrinfo(int Idx,char* szRetInf,int nRetbufSize);

/*************************************************
������������鵱ǰ����ϵͳ�Ƿ������������
���ܺ���: CheckVirtualMachine
����˵������
�����
0 ����������� ���� ���������
��ʷ���:
*************************************************/
HDTERMINAL_API int HDTERMINAL_STD_CALL CheckVirtualMachine();

#ifdef __cplusplus
}
#endif
#endif
