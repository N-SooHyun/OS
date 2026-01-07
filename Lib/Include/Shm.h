//#pragma once
//#include <Windows.h>
//
//class CShmemory
//{
//public:
//	enum {
//		NO_EXIST_NODE = 0x80,
//		NO_EXIST_ROOT,
//		NO_EXIST_APP,
//		NO_EXIST_SHM,
//		EXIST_SHM
//	};
//
//	CShmemory(char * Appname, char *NodeName = "NodeA");
//
//
//	~CShmemory();
//
//	//메소드 4개임
//	bool			Catalog(void* hProcess, void* hObject, LPSTR lpszName);
//	unsigned char	SearchShm(char * InTimeNodename, char * Appname, char * Shmname, void** hShareMem);
//	bool			CreateSMem(char *Shmname, void **p = 0, unsigned int shmsize = 0, char * AppName = "KPAmotion");
//	static bool		SearchKPAmotionInNodeA(void);
//
//private:
//	char * pNodeName;
//};