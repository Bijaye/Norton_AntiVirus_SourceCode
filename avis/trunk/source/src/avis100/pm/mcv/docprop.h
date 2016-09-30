//DBCS country codes
#define TW 1
#define JP 2
#define KR 3
#define CH 4


int IsWfw(char *szFileName);
int IsVBA5(char *szFileName);
int IsXl(char *szFileName);
int GetWfwCs(char *szFileName);
int GetWordDocVersion(char *szFileName);
int GetXlDocVersion(char *szFileName);
int IsWordDoc(char *szFileName);
int IsXlDoc(char *szFileName);
int WfwGetTemplateByte(char *szFileName);
int WfwSetTemplateByte(char *szFileName, int value);
int GetWfwDBCSCountryCode(char *szFileName);
