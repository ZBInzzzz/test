#ifndef __KDENCODECLI_H__
#define __KDENCODECLI_H__

/*************************************
Version : 1.0.0.3
*************************************/

// ���ܷ�ʽ����
#define NOT_ENCODE        0			// ������
#define DES_ENCODE        1			// DES
#define MD5_ENCODE        2			// MD5(������)
#define RIJNDAEL_ENCODE   3			// AES
#define BASE64_ENCODE     4			// BASE64
#define KDSIMPLE_ENCODE   5			// ��֤�򵥼���
#define KDCOMPLEX_ENCODE  6			// ��֤���ϼ���
#define GMSM4_ENCODE	  7			// �����㷨(SM4)

// KD2HsReEncode���ܷ�ʽ����
#define KD2HS_NORMAL      0			// ��׼��Blowfish���ܷ�ʽ
#define KD2HS_HS		  1			// �����Blowfish���ܷ�ʽ


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
KDEncode                   ����
����:
int nEncode_Level          ���ܼ��� = KDCOMPLEX_ENCODE
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
void *pKey                 ��Կ
int nKeyLen                ��Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ�������ĵĳ���

ע: ���ܼ��� = KDCOMPLEX_ENCODE ʱ
���Ļ������Ĵ�С           nDestDataBufLen = 2*nSrcDataLen(����Դ�ĵĳ���)����nDestDataBufLen>= 32����������Ϊ0x00��βASCII�ַ���
***********************************************************/
int KDEncode(int nEncode_Level, 
			 unsigned char* pSrcData,  int nSrcDataLen, 
			 unsigned char* pDestData, int nDestDataBufLen, 
			 void*          pKey,      int nKeyLen);



/***********************************************************
KDReEncode                 ���¼���
����:
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
int nOldEncode_Level       �ɼ��ܼ��� = KDCOMPLEX_ENCODE
void *pOldKey              ����Կ
int nOldKeyLen             ����Կ�ĳ���
int nOldEncode_Level       �¼��ܼ��� = KDCOMPLEX_ENCODE
void *pNewKey              ����Կ
int nNewKeyLen             ����Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ���������ĵĳ���

ע: ���ܼ��� = KDCOMPLEX_ENCODE ʱ
���Ļ������Ĵ�С           nDestDataBufLen = 2*nSrcDataLen(����Դ�ĵĳ���)����nDestDataBufLen>= 32����������Ϊ0x00��βASCII�ַ���
***********************************************************/
int KDReEncode(unsigned char* pSrcData,  int   nSrcDataLen, 
			   unsigned char* pDestData, int   nDestDataBufLen, 
			   int nOldEncode_Level,     void* pOldKey, int nOldKeyLen, 
			   int nNewEncode_Level,     void* pNewKey, int nNewKeyLen);



/***********************************************************
KDEncrypt                  ����
����:
int nEncode_Level          ���ܼ��� = DES_ENCODE, RIJNDAEL_ENCODE, BASE64_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
void *pKey                 ��Կ
int nKeyLen                ��Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ�������ĵĳ���
***********************************************************/
int KDEncrypt(int nEncode_Level, 
			  unsigned char* pSrcData,  int nSrcDataLen, 
			  unsigned char* pDestData, int nDestDataBufLen, 
			  void*          pKey,      int nKeyLen);


/***********************************************************
KDDecrypt                  ����(��KDEncryptΪһ��)
����:
int nEncode_Level          ���ܼ��� = DES_ENCODE, RIJNDAEL_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    ��Ҫ���ܵ�����
int nSrcDataLen            ��Ҫ�������ĵĳ���
unsigned char *pDestData   Դ�Ļ�����
int nDestDataBufLen        Դ�Ļ������Ĵ�С
void *pKey                 ��Կ
int nKeyLen                ��Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�ΪԴ�ĵĳ���
***********************************************************/
int KDDecrypt(int nEncode_Level, 
			  unsigned char* pSrcData,  int nSrcDataLen, 
			  unsigned char* pDestData, int nDestDataBufLen, 
			  void*          pKey,      int nKeyLen);


/***********************************************************
KDEncryptText              ����(����Ϊ�ɼ��ַ�)
����:
int nEncode_Level          ���ܼ��� = DES_ENCODE, RIJNDAEL_ENCODE, BASE64_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
void *pKey                 ��Կ
int nKeyLen                ��Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ�������ĵĳ���
***********************************************************/
int KDEncryptText(int nEncode_Level, 
			      unsigned char* pSrcData,  int nSrcDataLen, 
			      unsigned char* pDestData, int nDestDataBufLen, 
			      void*          pKey,      int nKeyLen);


/***********************************************************
KDDecryptText              ����(��KDEncryptTextΪһ��)
����:
int nEncode_Level          ���ܼ��� = DES_ENCODE, RIJNDAEL_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    ��Ҫ���ܵ�����
int nSrcDataLen            ��Ҫ�������ĵĳ���
unsigned char *pDestData   Դ�Ļ�����
int nDestDataBufLen        Դ�Ļ������Ĵ�С
void *pKey                 ��Կ
int nKeyLen                ��Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�ΪԴ�ĵĳ���
***********************************************************/
int KDDecryptText(int nEncode_Level, 
			      unsigned char* pSrcData,  int nSrcDataLen, 
			      unsigned char* pDestData, int nDestDataBufLen, 
			      void*          pKey,      int nKeyLen);

/***********************************************************
KD2HsReEncode              ���¼���(ʹ��Blowfish��ʽ)
����:
int nEncodeType            ���ܷ�ʽ��KD2HS_NORMALΪ��׼Blowfish��KD2HS_HSΪ����Blowfish
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
int nOldEncode_Level       �ɼ��ܼ��� = KDCOMPLEX_ENCODE
void *pOldKey              ����Կ
int nOldKeyLen             ����Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ���������ĵĳ���
***********************************************************/
int KD2HsReEncode(int nEncodeType,
				  unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen);

int KD2HsReEncodeExt(int nEncodeType,
				  unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen, void *pHsKey = NULL);


/***********************************************************
KD2XnReEncode              ���¼���(ʹ��Я���Զ�����ܷ�ʽ)
����:
unsigned char *pSrcData    ��Ҫ���ܵ�Դ��
int nSrcDataLen            ��Ҫ����Դ�ĵĳ���
unsigned char *pDestData   ���Ļ�����
int nDestDataBufLen        ���Ļ������Ĵ�С
int nOldEncode_Level       �ɼ��ܼ��� = KDCOMPLEX_ENCODE
void *pOldKey              ����Կ
int nOldKeyLen             ����Կ�ĳ���

����ֵ:                    ��1Ϊ���󣬳ɹ�Ϊ���������ĵĳ���
***********************************************************/
int KD2XnReEncode(unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen);

#ifdef __cplusplus
}
#endif

#endif
