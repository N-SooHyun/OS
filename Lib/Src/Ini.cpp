#include "../Include/Json.h"
#include "../Include/Ini.h"

namespace INI_FUNC{
	FILE* RdFile(const char* rPath){
		FILE* rf;
		fopen_s(&rf, rPath, "r");
		return rf;
	}

	FILE* WtFile(const char* wPath){
		FILE* wf;
		fopen_s(&wf, wPath, "w");
		return wf;
	}

	//Ini -> Parse -> Json
	void Ini_To_Js(){
		const char* ReadPath = "Ini/In/TestIni.ini";
		const char* WritePath = "Ini/Out/IniToRstJs.json";

		//File_Kor_Test(ReadPath, WritePath);

		FILE* rF = RdFile(ReadPath);
		FILE* wF = WtFile(WritePath);

		//ErrLst_Ini test(rF);
		IniParser test(rF);
		test.ParseMain();
		JNode* node = test.getNode();

		Json_Func::PrtNode(*node);
		//PrtNode_File(wF, *node);

		fclose(rF);
		fclose(wF);
	}

	//Json -> Parse -> Ini
	void Js_To_Ini(){
		const char* ReadPath = "Ini/In/TestJs.json";
		const char* WritePath = "Ini/Out/JsToRstIni.ini";

		FILE* rF = RdFile(ReadPath);
		FILE* wF = WtFile(WritePath);

		JsonToIni Ini(rF);
		Ini.IniPrt();
		//JNode* node = Ini.getNode();

		//PrtNode(*node);
		//PrtNode_File(wF, *node);

		fclose(rF);
		fclose(wF);
		//delete node;
	}

	//Ini 읽고 Json화 해서 -> Ini로 다시 파일 만들어내기
	void Ini_To_Js_To_Ini(){
		const char* ReadPath = "Ini/In/TshootTest.ini";
		const char* WritePath = "Ini/Out/IniToJsMid.json";
		const char* RstPath = "Ini/Out/TshootRstIni.ini";

		FILE* rF = RdFile(ReadPath);
		FILE* wF = WtFile(WritePath);
		FILE* rstF = WtFile(RstPath);

		ErrLst_Ini TsIni(rF);
		TsIni.ParseMain();

		JNode* node = TsIni.getNode();
		//PrtNode_File(wF, *node);

		JsonToIni RstIni(node);

		RstIni.IniFPrt(rstF);



		delete node;

		fclose(rF);
		fclose(wF);
		fclose(rstF);
	}

	//.ini 파일 읽어서 json에 추가하기
	void Ini_AddTo_Js(){
		const char* Read1Path = "Ini/In/ERROR_0000.ini";
		const char* Read2Path = "Ini/In/ERROR_0001.ini";

		FILE* r1F = RdFile(Read1Path);
		FILE* r2F = RdFile(Read2Path);

		ErrLst_Ini r1(r1F);
		ErrLst_Ini r2(r2F);

		r1.ParseMain();
		r2.ParseMain();

		JNode* jr1 = r1.getNode();
		JNode* jr2 = r2.getNode();

		JObj* or1 = static_cast<JObj*>(jr1->P_Type);
		JObj* or2 = static_cast<JObj*>(jr2->P_Type);
		or1->next = or2;





		//소멸
		fclose(r1F);
		fclose(r2F);
		delete jr1;
		jr2->P_Type = nullptr;
		delete jr2;
	}



	/* ------------------------------------------------------------------------------------
	*  테스트 용
	--------------------------------------------------------------------------------------*/



	int getObjCnt(JObj* RootObj){
		if (RootObj == nullptr) return -1;
		for (int i = 0;; i++){
			if (RootObj->next == nullptr){
				return ++i;
			}
			RootObj = RootObj->next;
		}
	}

	union MyUnion {
		int num;
		bool bl;
		double dnum;
	};
	MyUnion Val;

	//파일 이름 생성
	void ErrNumMak(DynamicStr* ErrNumStr, char* KeyNum){
		int len = 0;
		while (KeyNum[len] != '\0') len++;

		char buf[5];
		for (int i = 0; i < 4; i++) buf[i] = '0';
		buf[4] = '\0';

		int start = 4 - len;

		for (int i = 0; i < len; i++){
			buf[start + i] = KeyNum[i];
		}

		ErrNumStr->Append_Str(buf);
	}

	//파일로 출력
	void ErrFPrt(FILE* WrtEF, JNode* node){
		int * num = nullptr;
		DynamicStr* Str;
		switch (node->Cur_Type){
		case JNode::JType::NULLTYPE:
			return;

		case JNode::JType::NUMBER:
			num = static_cast<int*>(node->P_Type);
			fprintf(WrtEF, "%d", *num);
			break;

		case JNode::JType::STRING:
			Str = static_cast<DynamicStr*>(node->P_Type);
			fprintf(WrtEF, "%s", Str->Get_Str());
			break;

		case JNode::JType::BOOL:
			//안쓰이는게 정상
			break;

		case JNode::JType::DOUBLE:
			//안쓰이는게 정상
			break;

		case JNode::JType::OBJ:
			//안쓰이는게 정상
			break;

		case JNode::JType::ARR:
			//TSHOOT이니까 뉴라인만 잘 신경써서 해주면 될듯
			JArr* arr = static_cast<JArr*>(node->P_Type);
			fprintf(WrtEF, "\n");
			for (int i = 0; i <= node->ArrCnt; i++, arr = arr->next){
				ErrFPrt(WrtEF, arr->Value);

				if (i != node->ArrCnt) fprintf(WrtEF, "\n");
			}
			break;
		}

	}

	void SetmtPngName(DynamicStr** mtPngName){
		mtPngName[0]->Set_Str("mtLdMzZ.png");
		mtPngName[1]->Set_Str("mtLdGrpY.png");
		mtPngName[2]->Set_Str("mtWfrStgX.png");
		mtPngName[3]->Set_Str("mtWfrStgY.png");
		mtPngName[4]->Set_Str("mtWfrStgZ.png");
		mtPngName[5]->Set_Str("mtWfrStgT.png");
		mtPngName[6]->Set_Str("mtPlgY.png");
		mtPngName[7]->Set_Str("mtPlgZ.png");
		mtPngName[8]->Set_Str("mtPlgPinZ.png");
		mtPngName[9]->Set_Str("mtUnLdPkrX.png");
		mtPngName[10]->Set_Str("mtUnLdMzZ.png");
		mtPngName[11]->Set_Str("mtUnLdRalX.png");
		mtPngName[12]->Set_Str("mtUnLdGrpY.png");
		mtPngName[13]->Set_Str("mtLdTblY.png");
		mtPngName[14]->Set_Str("mtLblTblY.png");
		mtPngName[15]->Set_Str("mtLblTblT.png");
		mtPngName[16]->Set_Str("mtLdMapVsnX.png");
		mtPngName[17]->Set_Str("mtUnLdMapVsnX.png");
		mtPngName[18]->Set_Str("mtPkrX.png");
		mtPngName[19]->Set_Str("mtPkrZ.png");
		mtPngName[20]->Set_Str("mtPkrT.png");
		return;
	}

	bool StrToInt(const char* str, int& mtNum)
	{
		if (str == nullptr) return false;


		mtNum = std::atoi(str);


		if (mtNum >= 1700){
			mtNum %= 100;


			return true;
		}
		else
			return false;
	}

	//파일에 값 넣기
	void SetWrtEF(FILE* WrtEF, JObj* IniNode){
		//SECT[]
		//NAME=
		//TYPE=E
		//NEXTACT=C
		//POSITION=0
		//MESSAGE=
		//IMG_PATH= 비워두셈 그냥
		//TSHOOT=
		int mtCnt = 21;
		//const char** mtPngName = new const char*[mtCnt];	//소멸자 필요
		DynamicStr** mtPngName = new DynamicStr*[mtCnt];

		for (int i = 0; i < mtCnt; i++){
			mtPngName[i] = new DynamicStr(32);
		}

		SetmtPngName(mtPngName);

		enum _SrtStt{
			NAME,
			TYPE,
			NXTACT,
			POS,
			MSG,
			IMG,
			TSH,
			END, // 7
		};

		_SrtStt SrtStt = NAME;

		JNode** Vals = new JNode*[_SrtStt::END];	//포인터 배열(참조만 할것)
		DynamicStr** Keys = new DynamicStr*[_SrtStt::END]; //포인터 배열(참조만 할것)
		for (int i = 0; i < _SrtStt::END; i++) {
			Vals[i] = nullptr;
			Keys[i] = nullptr;
		}

		bool isKeys[_SrtStt::END];
		for (int i = 0; i < _SrtStt::END; i++) isKeys[i] = false;

		JObj* obj = static_cast<JObj*>(IniNode->Value->P_Type);
		JObj* SlctObj = obj;

		fprintf(WrtEF, "[INFO]\n");
		for (; SlctObj != nullptr; SlctObj = SlctObj->next){
			if (SlctObj->Key.StrCmp("NAME")){
				isKeys[NAME] = true;
				Vals[NAME] = SlctObj->Value;
				Keys[NAME] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("TYPE")){
				isKeys[TYPE] = true;
				Vals[TYPE] = SlctObj->Value;
				Keys[TYPE] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("NEXTACT")){
				isKeys[NXTACT] = true;
				Vals[NXTACT] = SlctObj->Value;
				Keys[NXTACT] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("POSITION")){
				isKeys[POS] = true;
				Vals[POS] = SlctObj->Value;
				Keys[POS] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("MESSAGE")){
				isKeys[MSG] = true;
				Vals[MSG] = SlctObj->Value;
				Keys[MSG] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("IMG_PATH")){
				isKeys[IMG] = true;
				Vals[IMG] = SlctObj->Value;
				Keys[IMG] = &SlctObj->Key;
			}
			else if (SlctObj->Key.StrCmp("TSHOOT")){
				isKeys[TSH] = true;
				Keys[TSH] = &SlctObj->Key;
				Vals[TSH] = SlctObj->Value;
			}
		}

		for (int i = 0; i < _SrtStt::END; i++){
			if (isKeys[i] == false){
				if (i == _SrtStt::NAME){
					fprintf(WrtEF, "NAME=\n");
				}
				else if (i == _SrtStt::TYPE){
					fprintf(WrtEF, "TYPE=E\n");
				}
				else if (i == _SrtStt::NXTACT){
					fprintf(WrtEF, "NEXTACT=C\n");
				}
				else if (i == _SrtStt::POS){
					fprintf(WrtEF, "POSITION=0\n");
				}
				else if (i == _SrtStt::MSG){
					fprintf(WrtEF, "MESSAGE=\n");
				}
				else if (i == _SrtStt::IMG){
					//IniNode->
					int mtNum = 0;
					if (StrToInt(IniNode->Key.Get_Str(), mtNum)){
						fprintf(WrtEF, "IMG_PATH=%s\n", mtPngName[mtNum]->Get_Str());
					}
					else
						fprintf(WrtEF, "IMG_PATH=\n");
				}
				else if (i == _SrtStt::TSH){
					fprintf(WrtEF, "TSHOOT=\n\n");
				}
			}
			else{
				if (Keys[i]->StrCmp("IMG_PATH")){
					int mtNum = 0;
					if (StrToInt(IniNode->Key.Get_Str(), mtNum)){
						fprintf(WrtEF, "IMG_PATH=%s\n", mtPngName[mtNum]->Get_Str());
					}
					else{
						//fprintf(WrtEF, "IMG_PATH=\n");
						fprintf(WrtEF, "%s=", Keys[i]->Get_Str());
						ErrFPrt(WrtEF, static_cast<JNode*>(Vals[i]));
					}
				}
				else{
					fprintf(WrtEF, "%s=", Keys[i]->Get_Str());
					ErrFPrt(WrtEF, static_cast<JNode*>(Vals[i]));
				}
				fprintf(WrtEF, "\n");

			}
		}

		//소멸자
		for (int i = 0; i < _SrtStt::END; i++){
			Vals[i] = nullptr;
			Keys[i] = nullptr;
		}
		delete[] Keys;
		delete[] Vals;


		for (int i = 0; i < mtCnt; i++){
			delete mtPngName[i];
		}
		delete[] mtPngName;

	}



	//API 사용은 안하지만 뭔지 모르겠네
	void Api_Js(){
		//const char* ReadPath = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\Input_Kor.ini)";
		//const char* ReadPath = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\Input_Eng.ini)";
		const char* ReadPath = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\Input_Ch.ini)";
		//const char* WrtMsg = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\.csv)";
		//const char* WrtTsh = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\.csv)";

		FILE* rF = RdFile(ReadPath);

		ErrLst_Ini IniNode(rF);
		IniNode.ParseMain();

		JNode* RootNode = IniNode.getNode();		//소멸 필요
		JObj* RootObj = static_cast<JObj*>(RootNode->P_Type);
		JObj* CurObj = RootObj;

		int ObjCnt = getObjCnt(RootObj);

		DynamicStr* WrtPath;	//소멸 필요
		FILE* WrtEF;			//소멸 필요

		//모터 버큠에 대한 오류 반복성이 많아서 정리좀 해주기


		//File 만들어내기
		for (int i = 0; CurObj != nullptr; i++){
			//파일이름 생성
			WrtPath = new DynamicStr(128);
			//WrtPath->Set_Str("C:/Users/Admin/Downloads/장비_제어/공부 및 정리/CodeStruct/MakeProject/INI_To_Dict/Err_Dir/Eng/ERROR");
			//WrtPath->Set_Str("C:/Users/Admin/Downloads/장비_제어/공부 및 정리/CodeStruct/MakeProject/INI_To_Dict/Err_Dir/Kor/ERROR");
			WrtPath->Set_Str("C:/Users/Admin/Downloads/장비_제어/공부 및 정리/CodeStruct/MakeProject/INI_To_Dict/Err_Dir/Ch/ERROR");
			ErrNumMak(WrtPath, CurObj->Key.Get_Str());
			WrtPath->Append_Str(".ini");

			//실제 파일 생성
			WrtEF = WtFile(WrtPath->Get_Str());

			//파일에 값 넣기
			SetWrtEF(WrtEF, CurObj);


			CurObj = CurObj->next;
			delete WrtPath;
			WrtPath = nullptr;
			fclose(WrtEF);
		}



		delete RootNode;
		fclose(rF);
	}

	//ErrorList.ini만들어주기
	//2개의 파일이 필요 MtVacErr.json, Err.ini(확장)
	void ErrIniMake(JNode* MtVacErrJs = nullptr){
		//경로부
		const char* ReadPath = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\Input_Ch.ini)";
		int MtVacErrStr = 0;
		bool bMtVacErr = MtVacErrJs == nullptr ? false : true;

		FILE* rF = RdFile(ReadPath);

		ErrLst_Ini IniNode(rF);
		IniNode.ParseMain();

		JNode* RootNode = IniNode.getNode();	//소멸 필요
		JObj* RootObj = static_cast<JObj*>(RootNode->P_Type);
		JObj* CurObj = RootObj;

		int ObjCnt = getObjCnt(RootObj);

		DynamicStr* WrtPath = nullptr;	//소멸 필요
		FILE* WrtEF = nullptr;			//소멸 필요

		//모터 버큠에 대한 오류 반복성 정리해주기


		for (int i = 0; CurObj != nullptr; i++){

		}

		delete RootNode;
		fclose(rF);
	}

	//파일 순서대로 읽어와서 Json에 붙여넣기
	void AddToJs(){
		const char* ReadPath = R"(C:\Users\Admin\Downloads\장비_제어\공부 및 정리\CodeStruct\MakeProject\INI_To_Dict\Err_Dir\Vac_Mt_Err\Input_Kor.ini)";
		FILE* rF = RdFile(ReadPath);

	}



}