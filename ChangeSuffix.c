/**********************************************************************
* 版权所有 (C)2014, ZhouZhaoxiong。
*
* 文件名称： ChangeSuffix.c
* 文件标识：无
* 内容摘要：对文件的后缀进行转换
* 其它说明：无
* 当前版本： V1.0
* 作    者：周兆熊
* 完成日期： 20141117
**********************************************************************/
 
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <io.h>
 
 
#defineMAX_FILESUFFIX_NUM   10  // 能够配置的文件后缀最大个数
#defineMAX_FILESUFFIX_LEN   10  // 单个文件后缀的最大长度
 
 
#define _MAXPATH 1024
int GetAppConfig(void);
voidParseFileSuffix(char *pSzSuffixBuf, int iType);
int GetFileExt(char*pFilePath, char *pOutFileExt);
void Check(char*pPath);
voidModifyFileName(char* pFilePath, char *pFileName);
 
 
// 全局配置
typedef struct
{
    int iOld2NewOrNew2Old;                                         // 新老后缀的转换
    charszOldSuffix[MAX_FILESUFFIX_NUM][MAX_FILESUFFIX_LEN];       // 旧的文件后缀
    charszNewSuffix[MAX_FILESUFFIX_NUM][MAX_FILESUFFIX_LEN];       // 新的文件后缀
} Config_T;
 
 
Config_T g_tConfig;
 
 
/****************************************************************
 * 功能描述: 获取全局配置
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 0-执行成功    -1-执行失败
 * 其它说明：无
 * 修改日期        版本号        修改人      修改内容
 *--------------------------------------------------------------
 * 20141117       V1.0           zzx         新建
 ****************************************************************/
int GetAppConfig(void)
{
    char szIniFile[128]   = {0};
    char szOldSuffix[256] = {0};
    char szNewSuffix[256] = {0};
 
 
    memset(&g_tConfig, 0x00,sizeof(Config_T));
 
 
    GetCurrentDirectory(sizeof(szIniFile)-1,szIniFile);
    strcat(szIniFile, "\\");
    strcat(szIniFile, "Config.ini");
 
 
    // 读取各个配置的值
    // 获取新旧后缀的转换标志
    g_tConfig.iOld2NewOrNew2Old =GetPrivateProfileInt("General", "Old2NewOrNew2Old", 1,szIniFile);
 
 
    // 获取旧的后缀并解析
    GetPrivateProfileString("General", "OldSuffix","", szOldSuffix, sizeof(szOldSuffix), szIniFile);
    if (strlen(szOldSuffix) == 0)
    {
        return -1;
    }
    ParseFileSuffix(szOldSuffix, 1);
 
 
    // 获取新的后缀并解析
    GetPrivateProfileString("General", "NewSuffix","", szNewSuffix, sizeof(szNewSuffix), szIniFile);
    if (strlen(szNewSuffix) == 0)
    {
        return -1;
    }
    ParseFileSuffix(szNewSuffix, 2);
 
    return 0;
}
 
 
/****************************************************************
 * 功能描述: 对读取到的后缀配置进行解析
 * 输入参数: pSzSuffixBuf: 包含所有配置值的缓存
             iType:解析新旧后缀的标志
 * 输出参数: 无
 * 返 回 值: 无
 * 其它说明：无
 * 修改日期       版本号         修改人      修改内容
 *--------------------------------------------------------------
 * 20141117       V1.0           zzx          新建
 ****************************************************************/
void ParseFileSuffix(char*pSzSuffixBuf, int iType)
{
    char szFileSuffix[128] = {0};
    char *t      = NULL;
    char *p      = NULL;
    int   h      = 0;
    int   iLen   = 0;
    int   iCpLen = 0;
 
    if (pSzSuffixBuf == NULL)
    {
        return;
    }
 
    memcpy(szFileSuffix, pSzSuffixBuf,sizeof(szFileSuffix));
 
 
    iLen = strlen(szFileSuffix);
    if (iLen >= sizeof(szFileSuffix))
    {
        iLen = sizeof(szFileSuffix);
    }
 
    if (szFileSuffix[iLen-1] != ';')
    {
        strcat(szFileSuffix, ";");
    }
 
 
    t = szFileSuffix;
 
    while ((p=strchr(t, ';')) != NULL)
    {
        if ((p-t) >= MAX_FILESUFFIX_LEN)
        {
            iCpLen = MAX_FILESUFFIX_LEN;
        }
        else
        {
            iCpLen = p-t;
        }
 
 
        switch (iType)
        {
            case 1:      // 解析旧的后缀
            {
               strncpy(g_tConfig.szOldSuffix[h], t, iCpLen);
                break;
            }
 
 
            case 2:      // 解析新的后缀
            {
               strncpy(g_tConfig.szNewSuffix[h], t, iCpLen);
                break;
            }
 
 
            default:
            {
                break;
            }
        }
 
        t = p + 1;
 
        h ++;
 
        if (h >= MAX_FILESUFFIX_NUM)
        {
            break;
        }
    }    
 
    return;
} 
 
 
/****************************************************************
 * 功能描述: 获取文件后缀
 * 输入参数: pFilePath-包含路径的文件名
 * 输出参数: pOutFileExt-文件后缀
 * 返  回值: 0-执行成功    -1-执行失败
 * 其它说明：无
 * 修改日期       版本号        修改人      修改内容
 *--------------------------------------------------------------
 * 20141117       V1.0           zzx         新建
 ****************************************************************/
int GetFileExt(char*pFilePath, char *pOutFileExt)
{
    char *pDest      = NULL;
    int iResult      = 0;
    int iFullPathLen = 0;
    int iFileExtLen  = 0;
    char szFullPathName[_MAXPATH] = {0};
 
 
    if ((NULL == pFilePath) || (NULL == pOutFileExt))
    {
        return -1;
    }
 
 
    _snprintf(szFullPathName,sizeof(szFullPathName)-1, "%s", pFilePath);
    iFullPathLen = strlen(szFullPathName);
 
 
    pDest = strrchr(szFullPathName, '.');
    if (NULL == pDest)
    {
        return -1;
    }
 
 
    iResult = pDest - szFullPathName + 1;
 
 
    iFileExtLen = iFullPathLen - iResult;
    if (iFileExtLen > 0)
    {
        memcpy(pOutFileExt, szFullPathName+iResult,iFileExtLen);
    }
    else
    {
        *pOutFileExt = 0;
    }
 
    return 0;
}
 
 
/****************************************************************
 * 功能描述: 修改文件后缀
 * 输入参数: pFilePath-文件路径
 * 输出参数: pOutFileExt-文件后缀
 * 返 回 值: TRUE-执行成功    FALSE-执行失败
 * 其它说明：无
 * 修改日期        版本号        修改人      修改内容
 *--------------------------------------------------------------
 *  20141117       V1.0           zzx         新建
 ****************************************************************/
voidModifyFileName(char* pFilePath, char *pFileName)
{
    HANDLE hFile                 = NULL;
    int  iLoopFlag               = 0;
    int  iResult                 = 0;
    int  iNameLen                = 0;
    int  iTotalExtType           = 0;
    char szFileTitle[_MAXPATH]   = {0};
    char szFilename[_MAXPATH]    = {0};
    char szNewFilename[_MAXPATH] = {0};
    char szFileExt[_MAXPATH]     = {0};
 
 
    if((NULL == pFilePath) || (NULL == pFileName))
    {
        return;
    }
 
 
    if (-1 == GetFileExt(pFileName, szFileExt))
    {
        return;
    }
 
    printf("ext:[%s] file:[%s]\n", szFileExt,pFileName);
 
 
    iNameLen = strlen(pFileName)-strlen(szFileExt);
    memcpy(szFileTitle, pFileName, iNameLen);
 
 
    iTotalExtType = sizeof(g_tConfig.szOldSuffix) /sizeof(g_tConfig.szOldSuffix[0]);
    for (iLoopFlag = 0; iLoopFlag < iTotalExtType; iLoopFlag++)
    {
        if (g_tConfig.iOld2NewOrNew2Old == 1)   // 变为新的后缀
        {
            if (0 == strcmp(g_tConfig.szOldSuffix[iLoopFlag],szFileExt))
            {
                break;
            }
        }
        else
        {
            if (0 ==strcmp(g_tConfig.szNewSuffix[iLoopFlag], szFileExt))
            {
                break;
            }
        }
    }
 
    if (iLoopFlag >= iTotalExtType)
    {
        printf("非修改文件类型: %s\n", szFileExt);
        return;
    }
 
 
    _snprintf(szFilename, sizeof(szFilename)-1,"%s\\%s", pFilePath, pFileName);
 
 
    if (1 == g_tConfig.iOld2NewOrNew2Old) // 变为新的后缀
    {
        _snprintf(szNewFilename, sizeof(szNewFilename)-1,"%s\\%s%s", pFilePath, szFileTitle, g_tConfig.szNewSuffix[iLoopFlag]);
    }
    else
    {
        _snprintf(szNewFilename, sizeof(szNewFilename)-1,"%s\\%s%s", pFilePath, szFileTitle,g_tConfig.szOldSuffix[iLoopFlag]);
    }
 
 
    iResult = rename(szFilename,szNewFilename);
    if (iResult != 0)
    {
       printf("Could not rename'%s'\n", szFilename);
    }
    else
    {
       printf("File '%s' renamed to'%s'\n", szFilename, szNewFilename);
    }
 
 
    return;
}
 
 
/****************************************************************
 * 功能描述: 扫描文件目录
 * 输入参数: pPath-文件路径
 * 输出参数: 无
 * 返回值: TRUE-执行成功    FALSE-执行失败
 * 其它说明：无
 * 修改日期        版本号        修改人      修改内容
 *--------------------------------------------------------------
 * 20141117        V1.0           zzx         新建
 ****************************************************************/
void Check(char *pPath)
{
    struct _finddata_t hFile; //定义结构体变量
    long handle           = 0;
    char szPath[_MAXPATH] = {0};
 
 
    memset(szPath, 0, sizeof(szPath));
    _snprintf(szPath, sizeof(szPath)-1, "%s\\*", pPath);
 
 
    handle =_findfirst(szPath, &hFile);     // 查找所有文件
    if (handle == -1)
    {
        printf("目录 %s 为空\n", pPath);
        return;
    }
    else
    {
        if (hFile.attrib &_A_SUBDIR) // 是目录
        {
            if (hFile.name[0] != '.') // 文件名不是'.'或'..'时
            {
                memset(szPath, 0, sizeof(szPath));
                _snprintf(szPath, sizeof(szPath)-1,"%s\\%s", pPath, hFile.name);
                printf("检查目录: %s\n", szPath);
 
                Check(szPath);   // 继续遍历
 
                Sleep(10);
              }
         }
         else    // 如果第一个实体不是目录,显示该文件
         {
             // 再获得文件的完整的路径名(包含文件的名称)
             memset(szPath,0, sizeof(szPath));
              _snprintf(szPath, sizeof(szPath)-1,"%s\\%s", pPath, hFile.name);
              ModifyFileName(pPath, hFile.name);
          }
 
 
          // 继续对当前目录中的下一个子目录或文件进行与上面同样的查找
          while (!(_findnext(handle, &hFile)))
          {
              if (hFile.attrib &_A_SUBDIR ) // 是目录                       
              {
                  if (hFile.name[0] != '.') // 文件名不是'.'或'..'时
                  {
                      memset(szPath, 0, sizeof(szPath));
                      _snprintf(szPath,sizeof(szPath)-1, "%s\\%s", pPath, hFile.name);
                      printf("检查目录: %s\n", szPath);
 
                      Check(szPath);       // 继续遍历
                      Sleep(10);
                  }
              }
              else    //如果第一个实体不是目录,显示该文件
              {
                  memset(szPath, 0, sizeof(szPath));
                  _snprintf(szPath, sizeof(szPath)-1,"%s\\%s", pPath, hFile.name);
                  ModifyFileName(pPath, hFile.name);
              }
          }
 
 
          _findclose(handle);
     }
}
 
 
/****************************************************************
 * 功能描述: 主函数
 * 输入参数: argc-输入的整型入参
             argv-输入的字符型入参
 * 输出参数: 无
 * 返回值: 0-执行成功    -1-执行失败
 * 其它说明：无
 * 修改日期        版本号        修改人      修改内容
 *--------------------------------------------------------------
 * 20141117        V1.0           zzx         新建
 ****************************************************************/
int main(int argc, char*argv[])
{
    char szPathName[1000] = {0};
    int  iPathLen = 0;
    int  iRetVal  = 0;
 
 
    printf("starting...\n");
 
 
    // 先读取全局配置
    iRetVal = GetAppConfig();
    if (iRetVal != 0)
    {
        return -1;
    }   
 
 
    if (4 != argc)
    {
        printf("argc=%d\n", argc);
        rintf("用法: touch.exe 目录名保持天数是否恢复\n例1: touch.exe D:\\work 60 1\n例2: touch.exe \"D:\\w o rk\" 60 0\n");
        eturn -1;
    }
    else
    {
        char szTempPath[1000] = {0};
 
 
        //目录名称
        strncpy(szTempPath, argv[1], sizeof(szTempPath)-1);
        iPathLen = strlen(szTempPath);
        if (iPathLen < 2)
        {
            printf("用法: touch.exe 目录名保持天数是否恢复\n例如: touch.exe D:\\work 600\n");
            return -1;
        }
 
 
        //第一个字符为引号
        if (szTempPath[0] == '"')
        {
            //最后一个不是引号判为非法
            if (szTempPath[iPathLen] != '"')
            {
                printf("第一个参数第一个字符是双引号, 最后一个字符不是双引号.\n");
                printf("用法: touch.exe 目录名保持天数\n例如: touch.exe \"D:\\w o r k\" 60\n");
                return -1;
            }
            else
            {
                //去掉前后引号
                memcpy(szPathName, szTempPath+1,iPathLen-2);
            }
        }
        //盘符
        else if ( ((szTempPath[0] >= 'A') &&(szTempPath[0] <= 'Z')) ||
               ((szTempPath[0] >= 'a') &&(szTempPath[0] <= 'z')) )
        {
            strncpy(szPathName, szTempPath,sizeof(szPathName)-1);
        }
        //第一个字符不是双引号或A~Z判为非法
        else
        {
            printf("第一个参数第一个字符不是双引号或A~Z或a~z\n");
            printf("用法: touch.exe 目录名保持天数\n例如: touch.exe D:\\work 60\n");
            return -1;
        }
    }
 
 
    Check(szPathName);
 
    printf("finished!...\n");
 
 
    return 0;  
}
