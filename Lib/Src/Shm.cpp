//#include "../../Lib/Include/Shm.h"
//#include <stdio.h>
//#include <string.h>
//
//CShmemory::CShmemory(char * Appname, char *NodeName)
//{
//	pNodeName = NodeName;
//}
//
//
//CShmemory::~CShmemory()
//{
//}
//
//
//bool CShmemory::Catalog(void* hProcess, void* hObject, LPSTR lpszName)
//{
//
//	return false;
//}
//
//
//bool CShmemory::SearchKPAmotionInNodeA(void)
//{
//	return true;
//}
//
//BYTE CShmemory::SearchShm(char * InTimeNodename, char * Appname, char * Shmname, void* * hShareMem)
//{
//
//
//	void* hnode = 0;
//	void* m_Root = 0;
//	void* m_App = 0;
//
//	hnode = ntxGetLocationByName(InTimeNodename);
//	if (hnode == ((void*)(UINT_PTR)(-1L))) return NO_EXIST_NODE;
//
//	m_Root = ntxGetRootRtProcess(hnode);
//	if (m_Root == ((void*)0)) return NO_EXIST_ROOT;
//
//	m_App = ntxLookupvoid*(m_Root, Appname, 10);
//	if (m_App == (void*)((void*)(UINT_PTR)(-1))) return NO_EXIST_APP;
//
//	(*hShareMem) = ntxLookupvoid*(m_App, Shmname, 10);
//	if ((*hShareMem) == ((void*)(UINT_PTR)(-1))) return NO_EXIST_SHM;
//
//	return EXIST_SHM;
//}
//
//
//bool CShmemory::CreateSMem(char *Shmname, void **p, unsigned int shmsize, char *AppName)
//{
//	bool ret = true;
//
//	printf("[CREATE SHARED MEMORY %s = %d ] \n", Shmname, shmsize);
//	void* hnode = 0;
//	void*	hShm;
//	HANDLE hMapFile;
//	TCHAR szUniCode[256] = { 0, };
//	MultiByteToWideChar(0, 0x00000001, Shmname, strlen(Shmname), szUniCode, 256);
//
//	if (strcmp(pNodeName, "NodeA") == false) {
//
//		if (SearchShm(pNodeName, AppName, Shmname, &hShm) == EXIST_SHM) {
//			(*p) = ntxMapRtSharedMemoryEx(hShm, 0x10000);
//			printf("hShm : %x  \n", hShm);
//			printf("ntxGetLastRtError : %x  \n", ntxGetLastRtError());
//			printf("MapRtSharedMemory : %x  \n", (*p));
//			if (p == 0) {
//				ret = false;
//			}
//			return ret;
//		}
//		else if (shmsize == 0){
//			return false;
//		}
//
//		hMapFile = CreateFileMappingW(
//			((HANDLE)(LONG_PTR)-1),
//			0,
//			0x04,
//			0,
//			shmsize,
//			szUniCode);
//
//		if (hMapFile == 0) {
//			printf("Could not create file mapping object (%d).\n", GetLastError());
//			return false;
//		}
//
//		*p = (char *)MapViewOfFile(hMapFile,
//			((0x000F0000L) | 0x0001 | 0x0002 | 0x0004 | 0x0008 | 0x0010),
//			0,
//			0,
//			shmsize);
//
//		if (*p == 0) {
//			printf("Could not map view of file (%d).\n", GetLastError());
//			CloseHandle(hMapFile);
//			return false;
//		}
//	}
//	else {
//		hMapFile = CreateFileMappingW(
//			((HANDLE)(LONG_PTR)-1),
//			0,
//			0x04,
//			0,
//			shmsize,
//			szUniCode);
//
//		if (hMapFile == 0) {
//			printf("Could not create file mapping object (%d).\n", GetLastError());
//			return false;
//		}
//
//		printf("hMapFile: %x\n", hMapFile);
//
//		*p = (char *)MapViewOfFile(hMapFile,
//			((0x000F0000L) | 0x0001 | 0x0002 | 0x0004 | 0x0008 | 0x0010),
//			0,
//			0,
//			shmsize);
//
//		if (*p == 0) {
//			printf("Could not map view of file (%d).\n", GetLastError());
//			CloseHandle(hMapFile);
//			return false;
//		}
//		printf("buf: %s\n", *p);
//	}
//
//
//	if (*p != 0) {
//		memset(*p, 0, shmsize);
//	}
//	return ret;
//};
//
