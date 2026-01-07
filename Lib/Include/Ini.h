#pragma once
#include "File.h"
#include "Json.h"

enum class IniType{
	NORMALINI,
	ERRORINI,
};

//Ini -> Json 변환 Dict 구조로
class IniToDict{
private:
	DynamicStr* IniStr;
	char CurChar;
	char PrvChar;
	char NxtChar;
	bool isIni;		//Ini가 맞는지 확인
	bool ParseFin;
	int glb_csr;
	//예외처리
	inline bool isFileCk(FILE* ckFile){
		if (!ckFile){
			printf("File Error: Please check the file path\n");
			return false;
		}
		return true;
	}
	//처음 시작 초기화
	void InitStr(int StrSize){
		IniStr = new DynamicStr(StrSize);
		CurChar = ' ';
		PrvChar = ' ';
		NxtChar = ' ';
		glb_csr = 0;
		IniNode = new JNode();
		ParseFin = false;
	}
	void ReadFileIni(FILE* rFile){
		char c = ' ';

		while (1){
			c = fgetc(rFile);
			if (c == EOF) break;
			IniStr->Append_Char(&c);
		}
	}
	//보조도구
	void AsciiToVal(JNode* KeyValNode, DynamicStr* KeyStr, DynamicStr* ValStr){
		//ValStr 기준으로 가는거임 이걸 기준으로 실값인지 아닌지 판단해서 넣어주기
		//숫자, 논리, 문자열만 파싱해주고 {} []타입은 그냥 Node에 넣어주면 알아서 해줄거임
		int RstInt = 0;
		double RstDbl = 0.0;
		bool IsDbl = false;      //false -> int, true -> double
		bool RstBl = false;

		int ValStrCsr = 0;	//Value 문자열용 커서

		enum CkWrd{
			DFT,
			STR,
			M_NUM,
			P_NUM,
			OBJ,   //Str로 넣어버리기
			ARR,   //Str로 넣어버리기
			BL,
			END
		};

		CkWrd CrntVal = DFT;
		char PrvWrd, CurWrd, NxtWrd = ' ';
		char* SectKey = KeyStr->Get_Str();


		for (;; ValStrCsr++){
			PrvWrd = ValStr->Char_Get_Str(ValStrCsr - 1);
			CurWrd = ValStr->Char_Get_Str(ValStrCsr);
			NxtWrd = ValStr->Char_Get_Str(ValStrCsr + 1);

			//시작 판별
			if (CrntVal == DFT){
				//음의 숫자일 경우(음의 정수, 음의 실수)
				if (CurWrd == '-' && (NxtWrd >= '0' && NxtWrd <= '9')) {
					CrntVal = M_NUM;
				}
				//양의 숫자일 경우(양의 정수, 양의 실수)
				else if (CurWrd >= '0' && CurWrd <= '9') {
					CrntVal = P_NUM;
					ValStrCsr--;
				}
				//bool일 경우
				else if (CurWrd == 't' || CurWrd == 'f') {
					CrntVal = BL;
					ValStrCsr--;
				}
				else if (CurWrd == '{'){
					if (ValStr->Char_Get_Str(ValStr->current_size - 1) == '}'){
						CrntVal = OBJ;
						ValStrCsr--;
					}
				}
				else if (CurWrd == '['){
					if (ValStr->Char_Get_Str(ValStr->current_size - 1) == ']'){
						CrntVal = ARR;
						ValStrCsr--;
					}
				}
				else{	//위에거 제외하면 그냥 문자열로 처리하셈 귀찮으니까
					CrntVal = STR;
					ValStrCsr--;
				}
				continue;
			}
			//종료 판별
			else if (CrntVal == END){
				break;
			}
			//값 처리
			else{
				switch (CrntVal){
				case STR:
				case OBJ:
				case ARR:

					//OBJ든 ARR이든 그냥 문자열로 JNode에 넣어주면 알아서 객체라고 판단해서 결과 내보내줄거임
					(*KeyValNode)[static_cast<const char*>(SectKey)] = ValStr->Get_Str();
					CrntVal = END;
					break;
				case M_NUM:
				case P_NUM:
				{
					if (CurWrd < '0' || CurWrd > '9') {
						//값자기 값이  123T 이러면 문자열이니까 문자열로 처리해주기
						CrntVal = STR;
						break;
					}
					//값 넣기
					RstInt = RstInt * 10 + (CurWrd - '0');

					//소수 판별
					if (NxtWrd == '.'){
						IsDbl = true;
						ValStrCsr += 2;
						double decimal_part = 0;
						int divisor = 10;
						bool isStr = false;
						for (;; ValStrCsr++){
							PrvWrd = ValStr->Char_Get_Str(ValStrCsr - 1);
							CurWrd = ValStr->Char_Get_Str(ValStrCsr);
							NxtWrd = ValStr->Char_Get_Str(ValStrCsr + 1);
							if (CurWrd < '0' || CurWrd > '9') {
								//값자기 값이  123.12T 이러면 문자열이니까 문자열로 처리
								CrntVal = STR;
								isStr = true;
								break;
							}
							decimal_part = decimal_part + (CurWrd - '0') / (double)divisor;
							divisor *= 10;
							if (NxtWrd == '\0') break;
						}
						if (isStr) break;
						RstDbl = RstInt + decimal_part;
					}

					//정수 밸류 완성 종료 판별
					if (NxtWrd == '\0'){
						if (CrntVal == M_NUM){
							RstInt = RstInt * -1;
							RstDbl = RstDbl * -1;
						}
						if (IsDbl)
							(*KeyValNode)[static_cast<const char*>(SectKey)] = RstDbl;
						else (*KeyValNode)
							[static_cast<const char*>(SectKey)] = RstInt;
						//(*KeyValNode)[static_cast<const char*>(SectKey)] = IsDbl==true ? RstDbl : RstInt;
						CrntVal = END;
					}
				}
					break;
				case BL:
				{
					char FstWrd = ValStr->Char_Get_Str(0);
					char ScdWrd = ValStr->Char_Get_Str(1);
					char ThdWrd = ValStr->Char_Get_Str(2);
					char FrWrd = ValStr->Char_Get_Str(3);
					char FivWrd = ValStr->Char_Get_Str(4);
					char SixWrd = ValStr->Char_Get_Str(5);
					if (FstWrd == 't' || FstWrd == 'T'){
						if (ScdWrd == 'r' && ThdWrd == 'u' && FrWrd == 'e' && FivWrd == '\0'){
							RstBl = true;
						}
						else{
							CrntVal = STR;
							break;
						}
					}
					else if (FstWrd == 'f' || FstWrd == 'F'){
						if (ScdWrd == 'a' && ThdWrd == 'l' && FrWrd == 's' && FivWrd == 'e' && SixWrd == '\0'){
							RstBl = false;
						}
						else{
							CrntVal = STR;
							break;
						}
					}
				}
					(*KeyValNode)[static_cast<const char*>(SectKey)] = RstBl ? true : false;
					CrntVal = END;
					break;
				}

			}

		}




		(*KeyValNode)[static_cast<const char*>(SectKey)];	// = 실제 타입 값 넣어주기 Ascii 파싱해서

	}
protected:
	//메인 파싱부
	void Parse_Ini(){
		short BrkCnt = 0;       // '['의 개수 추적
		bool isKey = false;		// KEY= 임을 확인 Section내부에 '\n'다음이면 무조건 true
		bool isSecArea = false;		// 특정 영역의 Section임을 확인 이를 통해 키인지 밸류인지 구분가능
		bool bfrVal = false;
		bool isVal = false;		// Value임을 확인

		enum IniStt{
			DFT,
			SEC,
			KEY,
			VAL,
			CMT,	//주석
		};

		DynamicStr* Section = new DynamicStr(128);
		DynamicStr* Key = new DynamicStr(128);
		DynamicStr* Val = new DynamicStr(128);

		JNode* KeyVal = new JNode();
		JObj* debugObj;



		IniStt stt = DFT;
		while (1){
			PrvChar = IniStr->Char_Get_Str(glb_csr - 1);
			CurChar = IniStr->Char_Get_Str(glb_csr);
			NxtChar = IniStr->Char_Get_Str(++glb_csr);

			if (CurChar == EOF || CurChar == '\0'){
				if (isSecArea){
					if (stt == VAL){
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;
					}
					isSecArea = false;
					isKey = false;
					isVal = false;
					//전에 있던거 넣어주기
					char* Sect = Section->Get_Str();

					(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;


					//debug
					debugObj = static_cast<JObj*>(IniNode->P_Type);
					JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);
				}

				debugObj = static_cast<JObj*>(IniNode->P_Type);
				if (Section != nullptr){
					delete Section;
				}
				if (Key != nullptr){
					delete Key;
				}
				if (Val != nullptr){
					delete Val;
				}
				if (KeyVal != nullptr){
					delete KeyVal;
				}
				ParseFin = true;
				break;
			}

			//Status 확인
			if (stt == DFT){
				if (CurChar == '['){
					if (isSecArea){
						isSecArea = false;
						isKey = false;
						isVal = false;
						//전에 있던거 넣어주기
						char* Sect = Section->Get_Str();
						(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

						//debug
						debugObj = static_cast<JObj*>(IniNode->P_Type);
						JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);

						delete Section;
						Section = new DynamicStr(128);
					}
					BrkCnt++;
					isSecArea = true;
					stt = SEC;
					continue;
				}
				else if (CurChar == ']'){
					BrkCnt--;
					isKey = true;
					stt = KEY;
					continue;
				}
				else if (CurChar == '='){
					isVal = true;
					stt = VAL;
					continue;
				}
				else if (CurChar == '#' || CurChar == ';'){
					stt = CMT;
					continue;
				}
				else if (CurChar == '\n' || CurChar == '\t' || CurChar == ' '){
					continue;
				}
				else if (isSecArea){
					glb_csr--;
					stt = KEY;
					continue;
				}
			}

			switch (stt){
			case SEC:
			{
				Section->Append_Char(&CurChar);
				if (NxtChar == ']'){
					Section->Str_Trim_All();
					stt = DFT;
				}
				break;
			}
			case KEY:
			{
				Key->Append_Char(&CurChar);
				if (NxtChar == '='){
					Key->Str_Trim_All();
					stt = DFT;
					isKey = false;
				}
				break;
			}
			case VAL:
			{
				//NULL처리해줘야함
				if (CurChar == '\n'){
					//NULL값을 의미
					stt = DFT;
					isVal = false;

					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);
					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
					break;
				}
				Val->Append_Char(&CurChar);
				if (NxtChar == '\n' || NxtChar == EOF || NxtChar == '\0'){
					Val->Str_Trim_All();
					//Ascii 처리해주기 문자열에서 -> 실제값으로 처리해주기
					AsciiToVal(KeyVal, Key, Val);
					stt = DFT;
					isVal = false;

					//char* SectKey = Key->Get_Str();
					//char* SectVal = Val->Get_Str();

					//(*KeyVal)[static_cast<const char*>(SectKey)] = SectVal;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);

					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
				}

				break;
			}
			case CMT:
			{
				if (CurChar == '\n'){
					stt = DFT;
				}
				break;
			}
			}
		}

	}
	//TSHOOT 들어가있는 에러 전용 .ini 파싱
	void Parse_ErrIni(){
		short BrkCnt = 0;       // '['의 개수 추적
		bool isKey = false;      // KEY= 임을 확인 Section내부에 '\n'다음이면 무조건 true
		bool isSecArea = false;      // 특정 영역의 Section임을 확인 이를 통해 키인지 밸류인지 구분가능
		bool bfrVal = false;
		bool isVal = false;      // Value임을 확인

		enum IniStt{
			DFT,
			SEC,
			KEY,
			VAL,
			CMT,   //주석
			TSH,   //TShoot 전용(ErrLst전용)
		};

		DynamicStr* Section = new DynamicStr(128);
		DynamicStr* Key = new DynamicStr(128);
		DynamicStr* Val = new DynamicStr(128);
		DynamicStr* Tsh = new DynamicStr(128);

		bool isTsh = false;   //Tsh값 유무체크
		short TshCnt = -1;	  //Tsh 배열 크기 체크

		JNode* KeyVal = new JNode();
		JNode* TshVal = new JNode();

		JObj* debugObj;



		IniStt stt = DFT;
		while (1){
			PrvChar = IniStr->Char_Get_Str(glb_csr - 1);
			CurChar = IniStr->Char_Get_Str(glb_csr);
			NxtChar = IniStr->Char_Get_Str(++glb_csr);

			if (CurChar == EOF || CurChar == '\0'){
				if (isSecArea){
					if (stt == VAL){
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;
					}
					else if (stt == TSH){
						(*TshVal) = JNode::JType::NULLTYPE;
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기
					}
					isSecArea = false;
					isKey = false;
					isVal = false;
					isTsh = false;
					//전에 있던거 넣어주기
					char* Sect = Section->Get_Str();
					(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

					//debug
					debugObj = static_cast<JObj*>(IniNode->P_Type);
					JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);
				}

				debugObj = static_cast<JObj*>(IniNode->P_Type);
				if (Section != nullptr){
					delete Section;
				}
				if (Key != nullptr){
					delete Key;
				}
				if (Val != nullptr){
					delete Val;
				}
				if (KeyVal != nullptr){
					delete KeyVal;
				}
				ParseFin = true;
				break;
			}

			//Status 확인
			if (stt == DFT){
				if (CurChar == '['){
					if (isSecArea){
						isSecArea = false;
						isKey = false;
						isVal = false;
						//전에 있던거 넣어주기
						char* Sect = Section->Get_Str();

						(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

						//debug
						debugObj = static_cast<JObj*>(IniNode->P_Type);
						JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);


						delete Section;
						Section = new DynamicStr(128);
					}
					BrkCnt++;
					isSecArea = true;
					stt = SEC;
					continue;
				}
				else if (CurChar == ']'){
					BrkCnt--;
					isKey = true;
					stt = KEY;
					continue;
				}
				else if (CurChar == '='){
					if (Key->StrCmp("TSHOOT")){
						stt = TSH;
						if (NxtChar == '\n') glb_csr++;
					}
					else{
						isVal = true;
						stt = VAL;
					}
					continue;
				}
				else if (CurChar == '#' || CurChar == ';'){
					stt = CMT;
					continue;
				}
				else if (CurChar == '\n' || CurChar == '\t' || CurChar == ' '){
					continue;
				}
				else if (isSecArea){
					glb_csr--;
					stt = KEY;
					continue;
				}
			}

			switch (stt){
			case SEC:
			{
				Section->Append_Char(&CurChar);
				if (NxtChar == ']'){
					Section->Str_Trim_All();
					stt = DFT;
				}
				break;
			}
			case KEY:
			{
				Key->Append_Char(&CurChar);
				if (NxtChar == '='){
					Key->Str_Trim_All();
					stt = DFT;
					isKey = false;
				}
				break;
			}
			case VAL:
			{
				//NULL처리해줘야함
				if (CurChar == '\n'){
					//NULL값을 의미
					stt = DFT;
					isVal = false;

					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);
					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
					break;
				}
				Val->Append_Char(&CurChar);
				if (NxtChar == '\n' || NxtChar == EOF || NxtChar == '\0'){
					Val->Str_Trim_All();
					AsciiToVal(KeyVal, Key, Val);
					stt = DFT;
					isVal = false;

					//char* SectKey = Key->Get_Str();
					//char* SectVal = Val->Get_Str();
					//
					//(*KeyVal)[static_cast<const char*>(SectKey)] = SectVal;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);

					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
				}
				break;
			}
			case CMT:
			{
				if (CurChar == '\n'){
					stt = DFT;
				}
				break;
			}
			case TSH:
			{
				//다음이 바로 끝일때
				if (NxtChar == EOF || NxtChar == '[' || NxtChar == '\0'){
					//종료 처리
					if (isTsh){
						Tsh->Append_Char(&CurChar);
						Tsh->Str_Trim_All();
						(*TshVal)[TshCnt] = Tsh->Get_Str();
					}


					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기

					delete Key;
					Key = new DynamicStr(128);

					isTsh = false;
					stt = DFT;
					TshCnt = -1;
					delete Tsh;
					Tsh = new DynamicStr(128);
					break;
				}

				//CurChar == '[' (Tshoot내용이 끝났음을 의미함)
				//CurChar == '\n' (Tshoot 내용에서 멀어짐을 의미함

				else if (CurChar == '['){
					//값이 NULL이거나 값이 있거나
					if (isTsh){
						Tsh->Str_Trim_All();
						(*TshVal)[TshCnt] = Tsh->Get_Str();

						isTsh = false;
						delete Tsh;
						Tsh = new DynamicStr(128);
					}
					else{
						//값이 NULL인 경우임
						(*TshVal) = JNode::JType::NULLTYPE;
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기

						delete Key;
						Key = new DynamicStr(128);

						isTsh = false;
						stt = DFT;
						TshCnt = -1;
						glb_csr--;
						break;
					}
				}
				else if (CurChar == '\n'){
					//새로운 배열이 있거나 혹은 값이 Null인 경우
					if (isTsh){
						Tsh->Str_Trim_All();
						(*TshVal)[TshCnt] = Tsh->Get_Str();

						isTsh = false;
						delete Tsh;
						Tsh = new DynamicStr(128);
					}
					else{
						//값이 NULL이거나 혹은 뉴라인이 과하게 되어 있거나
						if (TshVal->ArrCnt < 0){
							//이러면 진짜 값이 없는거지
							(*TshVal) = JNode::JType::NULLTYPE;
						}
						continue;

					}
				}
				else if (CurChar != '\n'){
					//값이 있는 경우
					if (!isTsh){
						isTsh = true;
						TshCnt++;
					}
					Tsh->Append_Char(&CurChar);
				}

				break;
			}
			}
		}

	}

	JNode* IniNode;
	IniToDict(const char* Str){
		InitStr(1024);
		char c = ' ';
		for (int i = 0;; i++){
			if (c == '\0') break;

			c = Str[i];

			IniStr->Append_Char(&c);
		}
	}
	IniToDict(FILE* rFile){
		if (!isFileCk(rFile))
			return;
		InitStr(1024);
		ReadFileIni(rFile);
	}
public:
	bool IniMainParse(){
		Parse_Ini();
		if (isIni){
			return true;
		}
		else
			return false;
	}

	virtual ~IniToDict(){
		if (IniNode != nullptr) delete IniNode;
		if (IniStr != nullptr) delete IniStr;
	}

	JNode* getNode(){
		if (IniNode != nullptr){
			JNode* rtnNode = IniNode;
			IniNode = nullptr;
			return rtnNode;
		}
		return nullptr;
	}
};

//Ini 파일 파싱 -> JNode를 활용해 Dict 구조로 변환 {"SECTION" : {"KEY" : VALUE }}
class IniParser{
protected:
	DynamicStr* IniStr;
	char CurChar;
	char PrvChar;
	char NxtChar;
	int glb_csr;
	JNode *IniNode;
	bool ParseFin;

	inline bool isFileCk(FILE* ckFile){
		if (!ckFile){
			printf("File Error: Please check the file path\n");
			return false;
		}
		return true;
	}

	void InitStr(int StrSize){
		IniStr = new DynamicStr(StrSize);
		CurChar = ' ';
		PrvChar = ' ';
		NxtChar = ' ';
		glb_csr = 0;
		IniNode = new JNode();
		ParseFin = false;
	}

	void AsciiToVal(JNode* KeyValNode, DynamicStr* KeyStr, DynamicStr* ValStr){
		//ValStr 기준으로 가는거임 이걸 기준으로 실값인지 아닌지 판단해서 넣어주기
		//숫자, 논리, 문자열만 파싱해주고 {} []타입은 그냥 Node에 넣어주면 알아서 해줄거임
		int RstInt = 0;
		double RstDbl = 0.0;
		bool IsDbl = false;      //false -> int, true -> double
		bool RstBl = false;

		int ValStrCsr = 0;	//Value 문자열용 커서

		enum CkWrd{
			DFT,
			STR,
			M_NUM,
			P_NUM,
			OBJ,   //Str로 넣어버리기
			ARR,   //Str로 넣어버리기
			BL,
			END
		};

		CkWrd CrntVal = DFT;
		char PrvWrd, CurWrd, NxtWrd = ' ';
		char* SectKey = KeyStr->Get_Str();


		for (;; ValStrCsr++){
			PrvWrd = ValStr->Char_Get_Str(ValStrCsr - 1);
			CurWrd = ValStr->Char_Get_Str(ValStrCsr);
			NxtWrd = ValStr->Char_Get_Str(ValStrCsr + 1);

			//시작 판별
			if (CrntVal == DFT){
				//음의 숫자일 경우(음의 정수, 음의 실수)
				if (CurWrd == '-' && (NxtWrd >= '0' && NxtWrd <= '9')) {
					CrntVal = M_NUM;
				}
				//양의 숫자일 경우(양의 정수, 양의 실수)
				else if (CurWrd >= '0' && CurWrd <= '9') {
					CrntVal = P_NUM;
					ValStrCsr--;
				}
				//bool일 경우
				else if (CurWrd == 't' || CurWrd == 'f') {
					CrntVal = BL;
					ValStrCsr--;
				}
				else if (CurWrd == '{'){
					if (ValStr->Char_Get_Str(ValStr->current_size - 1) == '}'){
						CrntVal = OBJ;
						ValStrCsr--;
					}
				}
				else if (CurWrd == '['){
					if (ValStr->Char_Get_Str(ValStr->current_size - 1) == ']'){
						CrntVal = ARR;
						ValStrCsr--;
					}
				}
				else{	//위에거 제외하면 그냥 문자열로 처리하셈 귀찮으니까
					CrntVal = STR;
					ValStrCsr--;
				}
				continue;
			}
			//종료 판별
			else if (CrntVal == END){
				break;
			}
			//값 처리
			else{
				switch (CrntVal){
				case STR:
				case OBJ:
				case ARR:

					//OBJ든 ARR이든 그냥 문자열로 JNode에 넣어주면 알아서 객체라고 판단해서 결과 내보내줄거임
					(*KeyValNode)[static_cast<const char*>(SectKey)] = ValStr->Get_Str();
					CrntVal = END;
					break;
				case M_NUM:
				case P_NUM:
				{
					if (CurWrd < '0' || CurWrd > '9') {
						//값자기 값이  123T 이러면 문자열이니까 문자열로 처리해주기
						CrntVal = STR;
						break;
					}
					//값 넣기
					RstInt = RstInt * 10 + (CurWrd - '0');

					//소수 판별
					if (NxtWrd == '.'){
						IsDbl = true;
						ValStrCsr += 2;
						double decimal_part = 0;
						int divisor = 10;
						bool isStr = false;
						for (;; ValStrCsr++){
							PrvWrd = ValStr->Char_Get_Str(ValStrCsr - 1);
							CurWrd = ValStr->Char_Get_Str(ValStrCsr);
							NxtWrd = ValStr->Char_Get_Str(ValStrCsr + 1);
							if (CurWrd < '0' || CurWrd > '9') {
								//값자기 값이  123.12T 이러면 문자열이니까 문자열로 처리
								CrntVal = STR;
								isStr = true;
								break;
							}
							decimal_part = decimal_part + (CurWrd - '0') / (double)divisor;
							divisor *= 10;
							if (NxtWrd == '\0') break;
						}
						if (isStr) break;
						RstDbl = RstInt + decimal_part;
					}

					//정수 밸류 완성 종료 판별
					if (NxtWrd == '\0'){
						if (CrntVal == M_NUM){
							RstInt = RstInt * -1;
							RstDbl = RstDbl * -1;
						}
						if (IsDbl)
							(*KeyValNode)[static_cast<const char*>(SectKey)] = RstDbl;
						else (*KeyValNode)
							[static_cast<const char*>(SectKey)] = RstInt;
						//(*KeyValNode)[static_cast<const char*>(SectKey)] = IsDbl==true ? RstDbl : RstInt;
						CrntVal = END;
					}
				}
					break;
				case BL:
				{
					char FstWrd = ValStr->Char_Get_Str(0);
					char ScdWrd = ValStr->Char_Get_Str(1);
					char ThdWrd = ValStr->Char_Get_Str(2);
					char FrWrd = ValStr->Char_Get_Str(3);
					char FivWrd = ValStr->Char_Get_Str(4);
					char SixWrd = ValStr->Char_Get_Str(5);
					if (FstWrd == 't' || FstWrd == 'T'){
						if (ScdWrd == 'r' && ThdWrd == 'u' && FrWrd == 'e' && FivWrd == '\0'){
							RstBl = true;
						}
						else{
							CrntVal = STR;
							break;
						}
					}
					else if (FstWrd == 'f' || FstWrd == 'F'){
						if (ScdWrd == 'a' && ThdWrd == 'l' && FrWrd == 's' && FivWrd == 'e' && SixWrd == '\0'){
							RstBl = false;
						}
						else{
							CrntVal = STR;
							break;
						}
					}
				}
					(*KeyValNode)[static_cast<const char*>(SectKey)] = RstBl ? true : false;
					CrntVal = END;
					break;
				}

			}

		}




		(*KeyValNode)[static_cast<const char*>(SectKey)];	// = 실제 타입 값 넣어주기 Ascii 파싱해서

	}

	virtual void Parse(){
		short BrkCnt = 0;       // '['의 개수 추적
		bool isKey = false;		// KEY= 임을 확인 Section내부에 '\n'다음이면 무조건 true
		bool isSecArea = false;		// 특정 영역의 Section임을 확인 이를 통해 키인지 밸류인지 구분가능
		bool bfrVal = false;
		bool isVal = false;		// Value임을 확인

		enum IniStt{
			DFT,
			SEC,
			KEY,
			VAL,
			CMT,	//주석
		};

		DynamicStr* Section = new DynamicStr(128);
		DynamicStr* Key = new DynamicStr(128);
		DynamicStr* Val = new DynamicStr(128);

		JNode* KeyVal = new JNode();
		JObj* debugObj;



		IniStt stt = DFT;
		while (1){
			PrvChar = IniStr->Char_Get_Str(glb_csr - 1);
			CurChar = IniStr->Char_Get_Str(glb_csr);
			NxtChar = IniStr->Char_Get_Str(++glb_csr);

			if (CurChar == EOF || CurChar == '\0'){
				if (isSecArea){
					if (stt == VAL){
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;
					}
					isSecArea = false;
					isKey = false;
					isVal = false;
					//전에 있던거 넣어주기
					char* Sect = Section->Get_Str();

					(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;


					//debug
					debugObj = static_cast<JObj*>(IniNode->P_Type);
					JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);
				}

				debugObj = static_cast<JObj*>(IniNode->P_Type);
				if (Section != nullptr){
					delete Section;
				}
				if (Key != nullptr){
					delete Key;
				}
				if (Val != nullptr){
					delete Val;
				}
				if (KeyVal != nullptr){
					delete KeyVal;
				}
				ParseFin = true;
				break;
			}

			//Status 확인
			if (stt == DFT){
				if (CurChar == '['){
					if (isSecArea){
						isSecArea = false;
						isKey = false;
						isVal = false;
						//전에 있던거 넣어주기
						char* Sect = Section->Get_Str();
						(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

						//debug
						debugObj = static_cast<JObj*>(IniNode->P_Type);
						JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);

						delete Section;
						Section = new DynamicStr(128);
					}
					BrkCnt++;
					isSecArea = true;
					stt = SEC;
					continue;
				}
				else if (CurChar == ']'){
					BrkCnt--;
					isKey = true;
					stt = KEY;
					continue;
				}
				else if (CurChar == '='){
					isVal = true;
					stt = VAL;
					continue;
				}
				else if (CurChar == '#' || CurChar == ';'){
					stt = CMT;
					continue;
				}
				else if (CurChar == '\n' || CurChar == '\t' || CurChar == ' '){
					continue;
				}
				else if (isSecArea){
					glb_csr--;
					stt = KEY;
					continue;
				}
			}

			switch (stt){
			case SEC:
			{
				Section->Append_Char(&CurChar);
				if (NxtChar == ']'){
					Section->Str_Trim_All();
					stt = DFT;
				}
				break;
			}
			case KEY:
			{
				Key->Append_Char(&CurChar);
				if (NxtChar == '='){
					Key->Str_Trim_All();
					stt = DFT;
					isKey = false;
				}
				break;
			}
			case VAL:
			{
				//NULL처리해줘야함
				if (CurChar == '\n'){
					//NULL값을 의미
					stt = DFT;
					isVal = false;

					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);
					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
					break;
				}
				Val->Append_Char(&CurChar);
				if (NxtChar == '\n' || NxtChar == EOF || NxtChar == '\0'){
					Val->Str_Trim_All();
					//Ascii 처리해주기 문자열에서 -> 실제값으로 처리해주기
					AsciiToVal(KeyVal, Key, Val);
					stt = DFT;
					isVal = false;

					//char* SectKey = Key->Get_Str();
					//char* SectVal = Val->Get_Str();

					//(*KeyVal)[static_cast<const char*>(SectKey)] = SectVal;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);

					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
				}

				break;
			}
			case CMT:
			{
				if (CurChar == '\n'){
					stt = DFT;
				}
				break;
			}
			}
		}

	}

public:

	IniParser(){
		InitStr(1024);
	}

	IniParser(const char* Str){
		InitStr(1024);
		if (Str != nullptr) return;
		char c = ' ';
		for (int i = 0;; i++){
			if (c == '\0') break;

			c = Str[i];

			IniStr->Append_Char(&c);

		}
	}

	IniParser(FILE* rFile){
		if (!isFileCk(rFile))
			return;
		InitStr(1024);
		ReadFileIni(rFile);
	}

	~IniParser(){
		if (IniStr != nullptr) delete IniStr;
		//if (IniNode != nullptr) delete IniNode;
	}


	void ReadFileIni(FILE* rFile){
		char c = ' ';

		while (1){
			c = fgetc(rFile);
			if (c == EOF) break;
			IniStr->Append_Char(&c);
		}
	}

	void ParseMain(){
		if (IniStr != nullptr){
			Parse();
		}
		else
			printf("파일이 없거나 읽어온 문자열이 없습니다.\n");
	}

	JNode* getNode(){
		if (ParseFin){
			return IniNode;
		}
		return nullptr;
	}
};

//Error List TSHOOT 전용 ErrorList Ini Parser
class ErrLst_Ini : public IniParser{
public:
	ErrLst_Ini(){}

	ErrLst_Ini(const char* Str) : IniParser(Str){}

	ErrLst_Ini(FILE* rFile) : IniParser(rFile){}

	~ErrLst_Ini(){}

private:
	virtual void Parse(){
		short BrkCnt = 0;       // '['의 개수 추적
		bool isKey = false;      // KEY= 임을 확인 Section내부에 '\n'다음이면 무조건 true
		bool isSecArea = false;      // 특정 영역의 Section임을 확인 이를 통해 키인지 밸류인지 구분가능
		bool bfrVal = false;
		bool isVal = false;      // Value임을 확인

		enum IniStt{
			DFT,
			SEC,
			KEY,
			VAL,
			CMT,   //주석
			TSH,   //TShoot 전용(ErrLst전용)
		};

		DynamicStr* Section = new DynamicStr(128);
		DynamicStr* Key = new DynamicStr(128);
		DynamicStr* Val = new DynamicStr(128);
		DynamicStr* Tsh = new DynamicStr(128);

		bool isTsh = false;   //Tsh값 유무체크
		short TshCnt = -1;	  //Tsh 배열 크기 체크

		JNode* KeyVal = new JNode();
		JNode* TshVal = new JNode();

		JObj* debugObj;



		IniStt stt = DFT;
		while (1){
			PrvChar = IniStr->Char_Get_Str(glb_csr - 1);
			CurChar = IniStr->Char_Get_Str(glb_csr);
			NxtChar = IniStr->Char_Get_Str(++glb_csr);

			if (CurChar == EOF || CurChar == '\0'){
				if (isSecArea){
					if (stt == VAL){
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;
					}
					else if (stt == TSH){
						(*TshVal) = JNode::JType::NULLTYPE;
						char* SectKey = Key->Get_Str();
						(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기
					}
					isSecArea = false;
					isKey = false;
					isVal = false;
					isTsh = false;
					//전에 있던거 넣어주기
					char* Sect = Section->Get_Str();
					(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

					//debug
					debugObj = static_cast<JObj*>(IniNode->P_Type);
					JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);
				}

				debugObj = static_cast<JObj*>(IniNode->P_Type);
				if (Section != nullptr){
					delete Section;
				}
				if (Key != nullptr){
					delete Key;
				}
				if (Val != nullptr){
					delete Val;
				}
				if (KeyVal != nullptr){
					delete KeyVal;
				}
				ParseFin = true;
				break;
			}

			//Status 확인
			if (stt == DFT){
				if (CurChar == '['){
					if (isSecArea){
						isSecArea = false;
						isKey = false;
						isVal = false;
						//전에 있던거 넣어주기
						char* Sect = Section->Get_Str();

						(*IniNode)[static_cast<const char*>(Sect)] = KeyVal;

						//debug
						debugObj = static_cast<JObj*>(IniNode->P_Type);
						JObj* debugObjVal = static_cast<JObj*>(debugObj->Value->P_Type);


						delete Section;
						Section = new DynamicStr(128);
					}
					BrkCnt++;
					isSecArea = true;
					stt = SEC;
					continue;
				}
				else if (CurChar == ']'){
					BrkCnt--;
					isKey = true;
					stt = KEY;
					continue;
				}
				else if (CurChar == '='){
					if (Key->StrCmp("TSHOOT")){
						stt = TSH;
						if (NxtChar == '\n') glb_csr++;
					}
					else{
						isVal = true;
						stt = VAL;
					}
					continue;
				}
				else if (CurChar == '#' || CurChar == ';'){
					stt = CMT;
					continue;
				}
				else if (CurChar == '\n' || CurChar == '\t' || CurChar == ' '){
					continue;
				}
				else if (isSecArea){
					glb_csr--;
					stt = KEY;
					continue;
				}
			}

			switch (stt){
			case SEC:
			{
				Section->Append_Char(&CurChar);
				if (NxtChar == ']'){
					Section->Str_Trim_All();
					stt = DFT;
				}
				break;
			}
			case KEY:
			{
				Key->Append_Char(&CurChar);
				if (NxtChar == '='){
					Key->Str_Trim_All();
					stt = DFT;
					isKey = false;
				}
				break;
			}
			case VAL:
			{
				//NULL처리해줘야함
				if (CurChar == '\n'){
					//NULL값을 의미
					stt = DFT;
					isVal = false;

					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = JNode::JType::NULLTYPE;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);
					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
					break;
				}
				Val->Append_Char(&CurChar);
				if (NxtChar == '\n' || NxtChar == EOF || NxtChar == '\0'){
					Val->Str_Trim_All();
					AsciiToVal(KeyVal, Key, Val);
					stt = DFT;
					isVal = false;

					//char* SectKey = Key->Get_Str();
					//char* SectVal = Val->Get_Str();
					//
					//(*KeyVal)[static_cast<const char*>(SectKey)] = SectVal;

					//debug 용
					debugObj = static_cast<JObj*>(KeyVal->P_Type);

					delete Key;
					delete Val;

					Key = new DynamicStr(128);
					Val = new DynamicStr(128);
				}
				break;
			}
			case CMT:
			{
				if (CurChar == '\n'){
					stt = DFT;
				}
				break;
			}
			case TSH:
			{
				if (NxtChar == EOF || NxtChar == '[' || NxtChar == '\0'){
					//종료 처리
					if (isTsh){
						Tsh->Append_Char(&CurChar);
						Tsh->Str_Trim_All();
						(*TshVal)[TshCnt] = Tsh->Get_Str();
					}


					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기

					delete Key;
					Key = new DynamicStr(128);

					isTsh = false;
					stt = DFT;
					TshCnt = -1;
					break;
				}
				else if (CurChar == '['){
					//값이 NULL인 경우임
					(*TshVal) = JNode::JType::NULLTYPE;
					char* SectKey = Key->Get_Str();
					(*KeyVal)[static_cast<const char*>(SectKey)] = TshVal;	//배열을 넣기

					delete Key;
					Key = new DynamicStr(128);

					isTsh = false;
					stt = DFT;
					TshCnt = -1;
					glb_csr--;
					break;
				}
				else if (CurChar == '\n'){
					//새로운 배열이 있거나 혹은 값이 Null인 경우
					if (isTsh){
						Tsh->Str_Trim_All();
						(*TshVal)[TshCnt] = Tsh->Get_Str();

						isTsh = false;
						delete Tsh;
						Tsh = new DynamicStr(128);
					}
					else{
						//값이 NULL인 경우임
						(*TshVal) = JNode::JType::NULLTYPE;
						continue;
					}
				}
				else if (CurChar != '\n'){
					//값이 있는 경우
					if (!isTsh){
						isTsh = true;
						TshCnt++;
					}
					Tsh->Append_Char(&CurChar);
				}

				break;
			}
			}
		}

	}


};





//{Sect : {Key : Value}} to
//[Sect]
//Key=Value
class JsonToIni : public IniParser{
private:
	union MyUnion {
		int num;
		bool bl;
		double dnum;
	};
	MyUnion Val;
	void PrtNode(JNode& node){
		DynamicStr* Str;
		switch (node.Cur_Type){
		case JNode::JType::NULLTYPE:   //종료
			printf("null");
			return;
		case JNode::JType::NUMBER:
			Val.num = node;
			printf("%d", Val.num);
			break;
		case JNode::JType::STRING:
			Str = static_cast<DynamicStr*>(node.P_Type);
			Str->Str_Trim_Quote();
			printf("%s", Str->Get_Str());
			break;
		case JNode::JType::BOOL:
			Val.bl = node;
			printf("%s", Val.bl == true ? "true" : "false");
			break;
		case JNode::JType::DOUBLE:
			Val.dnum = node;
			printf("%.3f", Val.dnum);
			break;
		case JNode::JType::OBJ:
			printf("{");
			JObj* obj;
			obj = static_cast<JObj*>(node.P_Type);

			for (int i = 0; i <= node.ObjCnt; i++, obj = obj->next){
				printf("\"%s\" : ", obj->Key.Get_Str());
				PrtNode(*obj->Value);

				if (i != node.ObjCnt) printf(", ");
			}

			printf("}");
			break;
		case JNode::JType::ARR:
			printf("[");
			JArr* arr;
			arr = static_cast<JArr*>(node.P_Type);
			for (int i = 0; i <= node.ArrCnt; i++, arr = arr->next){
				PrtNode(*arr->Value);

				if (i != node.ArrCnt) printf(",");
			}
			printf("]");
			break;
		default:
			break;
		}
	}

	void FPrtNode(FILE* fp, JNode& node) {
		DynamicStr* Str;

		switch (node.Cur_Type) {
		case JNode::JType::NULLTYPE:   // 종료
			//fprintf(fp, "null");
			return;

		case JNode::JType::NUMBER:
			Val.num = node;
			fprintf(fp, "%d", Val.num);
			break;

		case JNode::JType::STRING:
			Str = static_cast<DynamicStr*>(node.P_Type);
			Str->Str_Trim_Quote();
			fprintf(fp, "%s", Str->Get_Str());
			break;

		case JNode::JType::BOOL:
			Val.bl = node;
			fprintf(fp, "%s", Val.bl == true ? "true" : "false");
			break;

		case JNode::JType::DOUBLE:
			Val.dnum = node;
			fprintf(fp, "%.3f", Val.dnum);
			break;

		case JNode::JType::OBJ: {
			fprintf(fp, "{");
			JObj* obj = static_cast<JObj*>(node.P_Type);

			for (int i = 0; i <= node.ObjCnt; i++, obj = obj->next) {
				fprintf(fp, "\"%s\" : ", obj->Key.Get_Str());
				FPrtNode(fp, *obj->Value);

				if (i != node.ObjCnt)
					fprintf(fp, ", ");
			}

			fprintf(fp, "}");
			break;
		}

		case JNode::JType::ARR: {
			fprintf(fp, "[");
			JArr* arr = static_cast<JArr*>(node.P_Type);

			for (int i = 0; i <= node.ArrCnt; i++, arr = arr->next) {
				FPrtNode(fp, *arr->Value);

				if (i != node.ArrCnt)
					fprintf(fp, ",");
			}

			fprintf(fp, "]");
			break;
		}

		default:
			break;
		}
	}

	virtual void Parse(){
		enum NdStt{
			DFT,
			SCT,
			KEY,
			VAL,
		};

		//IniNode가 Root임
		(*IniNode) = IniStr->Get_Str();
		IniStr = nullptr;
		ParseFin = true;
	}
public:
	JsonToIni(){ InitStr(1024); }
	JsonToIni(const char* Str){
		InitStr(1024);
		if (Str != nullptr) return;
		char c = ' ';
		for (int i = 0;; i++){
			if (c == '\0') break;

			c = Str[i];
			IniStr->Append_Char(&c);
		}
	}
	JsonToIni(FILE* rFile){
		if (!isFileCk(rFile))
			return;
		InitStr(1024);
		ReadFileIni(rFile);
	}
	JsonToIni(JNode* node){
		IniNode = node;
		ParseFin = true;
	}
	~JsonToIni(){}

	void IniPrt(){
		Parse();
		if (ParseFin == false) return;

		JObj* RootObj = static_cast<JObj*>(IniNode->P_Type);
		JObj* JmpObj = RootObj;
		char* pSectKey;
		for (;;){
			if (JmpObj == nullptr)
				break;

			pSectKey = JmpObj->Key.Get_Str();
			JObj* KeyVal = static_cast<JObj*>(JmpObj->Value->P_Type);
			printf("[%s]\n", pSectKey);
			char* pKey = nullptr;
			char* pVal = nullptr;
			for (;;){
				if (KeyVal == nullptr) break;
				//Sect모드
				KeyVal->Key.Str_Trim_Quote();
				printf("%s=", KeyVal->Key.Get_Str());
				PrtNode(*KeyVal->Value);
				printf("\n");
				KeyVal = KeyVal->next;
			}
			JmpObj = JmpObj->next;

		}

	}

	void IniFPrt(FILE* pf){
		//Parse();
		if (ParseFin == false) return;

		JObj* RootObj = static_cast<JObj*>(IniNode->P_Type);
		JObj* JmpObj = RootObj;
		char* pSectKey;
		for (;;){
			if (JmpObj == nullptr)
				break;

			pSectKey = JmpObj->Key.Get_Str();
			JObj* KeyVal = static_cast<JObj*>(JmpObj->Value->P_Type);
			fprintf(pf, "[%s]\n", pSectKey);
			char* pKey = nullptr;
			char* pVal = nullptr;
			for (;;){
				if (KeyVal == nullptr) break;
				//Sect모드
				KeyVal->Key.Str_Trim_Quote();
				fprintf(pf, "%s=", KeyVal->Key.Get_Str());
				FPrtNode(pf, *KeyVal->Value);
				fprintf(pf, "\n");
				KeyVal = KeyVal->next;
			}
			JmpObj = JmpObj->next;

		}

	}
};



namespace INI_FUNC{
	FILE* RdFile(const char* rPath);

	FILE* WtFile(const char* wPath);

	//Ini -> Parse -> Json
	void Ini_To_Js();
	//Json -> Parse -> Ini
	void Js_To_Ini();

	//Ini 읽고 Json화 해서 -> Ini로 다시 파일 만들어내기
	void Ini_To_Js_To_Ini();
	//.ini 파일 읽어서 json에 추가하기
	void Ini_AddTo_Js();

	void Api_Js();

	void ErrIniMake(JNode* js);
}




//IO, MotorPos, machiesetting.ini, errorlist.ini  4개정도 만들기 .json4개 필요
//Json To Ini
//Ini to Json
namespace INI_MAKER{
	class IniPrs : public IniToDict{
	public:
		IniPrs(const char* Str) : IniToDict(Str) {}
		~IniPrs(){}

		void SmpIniToJs(){
			this->Parse_Ini();
		}

		void ErrIniToJs(){
			this->Parse_ErrIni();
		}
	};


	class IniMain{
	private:
		enum FTYPE{	//읽어들일 파일 타입
			IO,
			MtPos,
			Mt,
			ErrIni_Kor,
			ErrIni_Eng,
			ErrIni_Ch,
			ErrIni_Tw,
			ErrMtVac_Kor,		//모터랑 버큠만 있는 에러(서보, 드라이버 등등)
			ErrMtVac_Eng,
			ErrMtVac_Ch,
			ErrMtVac_Tw,
			END
		};

		enum MTINI{
			SLWHOMZERO,	//느리게 홈잡을때
			HOMSPDSWT,	//홈잡고 Stn갈때
			HOMACC,		//홈 가속도

			PRFACC_DCE,		//속도 가감속
			PRFVLCMAX,		//PROFILE_VELOCITY, MAX_PROFILE_VELOCITY
			QCK_STOP,		//빠른 정지
			VLCPOSWIN,		//거리 속도 정확도
			MTEND,
		};
		/* ================================================
		* Init 함수들
		==================================================*/
		void Init_Path(DynamicStr**& Paths){
			Paths = new DynamicStr*[FTYPE::END];	//소멸자 필요
			for (int i = 0; i < FTYPE::END; i++){
				Paths[i] = new DynamicStr(32);
			}
			Paths[FTYPE::IO]->Set_Str(".\\Dir\\Input\\IOList.json");
			Paths[FTYPE::MtPos]->Set_Str(".\\Dir\\Input\\MtPos.json");
			Paths[FTYPE::Mt]->Set_Str(".\\Dir\\Input\\machine.json");
			Paths[FTYPE::ErrIni_Kor]->Set_Str(".\\Dir\\Input\\ErrLst_UTF-8\\ErrLst_Kor.ini");			//한국어
			Paths[FTYPE::ErrIni_Eng]->Set_Str(".\\Dir\\Input\\ErrLst_UTF-8\\ErrLst_Eng.ini");			//영어
			Paths[FTYPE::ErrIni_Ch]->Set_Str(".\\Dir\\Input\\ErrLst_UTF-8\\ErrLst_Ch.ini");		//중국어(간체)
			Paths[FTYPE::ErrIni_Tw]->Set_Str(".\\Dir\\Input\\ErrLst_UTF-8\\ErrLst_Tw.ini");			//중국어(대만어 번체)
			Paths[FTYPE::ErrMtVac_Kor]->Set_Str(".\\Dir\\Input\\MtVacErr_UTF-8\\MtVacErr_Kor.json");
			Paths[FTYPE::ErrMtVac_Eng]->Set_Str(".\\Dir\\Input\\MtVacErr_UTF-8\\MtVacErr_Eng.json");
			Paths[FTYPE::ErrMtVac_Ch]->Set_Str(".\\Dir\\Input\\MtVacErr_UTF-8\\MtVacErr_Ch.json");
			Paths[FTYPE::ErrMtVac_Tw]->Set_Str(".\\Dir\\Input\\MtVacErr_UTF-8\\MtVacErr_Tw.json");
		}
		void Init_File(FILE**& rFs, DynamicStr**& Paths){
			rFs = new FILE*[FTYPE::END];
			for (int i = 0; i < FTYPE::END; i++){
				rFs[i] = FileFunc::RdFile(Paths[i]->Get_Str());
			}
		}
		void Del_Path_File(DynamicStr**& Paths, FILE**& rFs){
			if (Paths != nullptr){
				for (int i = 0; i < FTYPE::END; i++){
					delete Paths[i];
				}
				delete[] Paths;
				Paths = nullptr;
			}

			if (rFs != nullptr){
				for (int i = 0; i < FTYPE::END; i++){
					fclose(rFs[i]);
					//delete rFs[i];
				}
				delete[] rFs;
				rFs = nullptr;
			}
		}

		void Init_IO(JObj* RootObj, JNode** I_Obj, JNode** O_Obj){
			JObj* i_obj = RootObj->searchKey("INPUT");
			JObj* o_obj = RootObj->searchKey("OUTPUT");
			*I_Obj = static_cast<JNode*>(i_obj->Value);
			*O_Obj = static_cast<JNode*>(o_obj->Value);
		}

		void Init_Mt(JNode* Root, JArr** mtKey, JArr** ctrlName, JArr** uiName, JArr** ldVal){
			JObj* root = static_cast<JObj*>(Root->P_Type);

			JObj* ctrlObj = nullptr;
			*mtKey = static_cast<JArr*>(root->searchKey("MtKeys")->Value->P_Type);
			*ctrlName = static_cast<JArr*>(root->searchKey("Ctrl Name")->Value->P_Type);
			*uiName = static_cast<JArr*>(root->searchKey("UI Name")->Value->P_Type);
			*ldVal = static_cast<JArr*>(root->searchKey("Lead Value")->Value->P_Type);
		}

		void Init_MtVac(JNode* Root, JObj** Mtr, JObj** Vac, JArr** MtrImg){
			JObj* root = static_cast<JObj*>(Root->P_Type);

			*Mtr = static_cast<JObj*>(root->searchKey("MOTOR")->Value->P_Type);
			*Vac = static_cast<JObj*>(root->searchKey("VAC")->Value->P_Type);
			*MtrImg = static_cast<JArr*>(root->searchKey("MOTORPNG")->Value->P_Type);
		}

		/*void Init_MtKorEng(JObj* Root, JObj** korObj, JObj** engObj){
		*korObj = static_cast<JObj*>(Root->searchKey("KOR")->Value->P_Type);
		*engObj = static_cast<JObj*>(Root->searchKey("ENG")->Value->P_Type);
		}*/

		void Init_Msg_Tsh(JObj* Root, JObj** MsgObj, JObj** TshObj){
			*MsgObj = static_cast<JObj*>(Root->searchKey("MSG")->Value->P_Type);
			*TshObj = static_cast<JObj*>(Root->searchKey("TSH")->Value->P_Type);
		}


		/* ================================================
		* 기능 API
		==================================================*/
		//File -> Str
		DynamicStr* FileToStr(FILE* rF){
			DynamicStr* newStr = new DynamicStr(128);

			if (rF == nullptr) {
				std::cout << "File -> Str 실패" << std::endl;
				return nullptr;
			}
			char c = ' ';

			while (1){
				c = fgetc(rF);
				if (c == EOF) break;
				newStr->Append_Char(&c);
			}
			return newStr;
		}

		//Str -> Json 화
		JNode* StrToJs(DynamicStr* Str){
			JNode* node = new JNode();
			*node = Str->Get_Str();

			return node;
		}

		//숫자 -> 여러자리수로 변형해서 문자열 반환
		void IntToStr(DynamicStr* numStr, int num, int cntNum){
			int i = 0;
			char temp[16]; // 충분히 큰 버퍼
			do {
				temp[i++] = '0' + (num % 10);
				num /= 10;
			} while (num > 0);
			// 필요한 만큼 앞에 0 추가
			while (i < cntNum) {
				temp[i++] = '0';
			}
			// 역순으로 numStr에 붙이기
			for (int j = i - 1; j >= 0; j--) {
				numStr->Append_Char(&temp[j]);  // DynamicStr에서 Append() 가 문자 하나씩 추가하는 함수라고 가정
			}
		}

		//Str로 접점확인
		void StrCkPol(DynamicStr* Str, char& Pol){
			//맨뒤에 글자 가져와서 A or B면 맨뒤에서 앞으로 가서 '|'인지 확인
			int len = Str->current_size - 1;

			char lastChar = Str->Char_Get_Str(len);

			//맨뒤에 접점이 있음을 의미
			if (lastChar == 'A' || lastChar == 'B'){
				Pol = lastChar;

				//그 앞에 있는게 "|" 인지 확인
				lastChar = Str->Char_Get_Str(len - 1);
				if (lastChar == '|'){
					//맨뒤에 있는거 지워줘야함
					Str->Str_Trim_Back();
					Str->Str_Trim_Back();
				}
				else{
					Pol = 'A';
				}
			}
			else{
				Pol = 'A';
				//안지워도 됨
			}

		}

		//MotorPos 값 계산해서 구하기
		void MtIniCal(double(&MtVals)[MTINI::MTEND], double LeadVal){
			if (LeadVal <= 0){
				//그냥 전부 0으로 해버리는게 좋을듯
				for (int i = 0; i < MTINI::MTEND; i++) MtVals[i] = 0;
				return;
			}
			double Fst_Lead = 8388608 / LeadVal;

			MtVals[MTINI::SLWHOMZERO] = 10 * Fst_Lead;		//홈잡을때 느린속도
			MtVals[MTINI::HOMSPDSWT] = 50 * Fst_Lead;		//홈잡고 Stand갈때 속도
			MtVals[MTINI::HOMACC] = 2000 * Fst_Lead;		//홈 가속도
			MtVals[MTINI::PRFACC_DCE] = 10000 * Fst_Lead;	//속도 가감속
			MtVals[MTINI::PRFVLCMAX] = 10000 * Fst_Lead;	//최대속도 상한(제한)(MAX_PROFILE_VELOCITY) 및 (PROFILE_VELOCITY) 
			//Profile Velocity 추가설명
			//목표 속도에 도달하기 위해서 내는 속도
			//목표에 도달하면 등속구간에서 유지해야하는 속도
			MtVals[MTINI::QCK_STOP] = 100000 * Fst_Lead;	//빠른 정지
			MtVals[MTINI::VLCPOSWIN] = Fst_Lead / 100;		//목표 정확도
		}

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

		void SetWrtEF(FILE* WrtEF, JObj* IniNode){
			//SECT[]
			//NAME=
			//TYPE=E
			//NEXTACT=C
			//POSITION=0
			//MESSAGE=
			//IMG_PATH= 비워두셈 그냥
			//TSHOOT=
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

			_SrtStt StrStt = NAME;

			JNode** Vals = new JNode*[_SrtStt::END];				//포인터 배열(참조만 함)
			DynamicStr** Keys = new DynamicStr*[_SrtStt::END];		//포인터 배열(참조만 함)

			for (int i = 0; i < _SrtStt::END; i++){
				Vals[i] = nullptr;
				Keys[i] = nullptr;
			}

			bool isKeys[_SrtStt::END];
			for (int i = 0; i < _SrtStt::END; i++) isKeys[i] = false;

			JObj* obj = static_cast<JObj*>(IniNode->Value->P_Type);
			JObj* SlctObj = obj;

			fprintf(WrtEF, "[INFO]\n");
			//값을 넣어주는 역할만 함
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


			//실제 파일을 만들어내는 역할을 함
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
						fprintf(WrtEF, "IMG_PATH=\n");
						/*int mtNum = 0;
						if (StrToInt(IniNode->Key.Get_Str(), mtNum)){
						fprintf(WrtEF, "IMG_PATH=%s\n", mtPngName[mtNum]->Get_Str());
						}
						else
						fprintf(WrtEF, "IMG_PATH=\n");*/
					}
					else if (i == _SrtStt::TSH){
						fprintf(WrtEF, "TSHOOT=\n\n");
					}
				}
				else{
					fprintf(WrtEF, "%s=", Keys[i]->Get_Str());
					Json_Func::ErrFilePrt(WrtEF, static_cast<JNode*>(Vals[i]));
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
		}


		//MtVacErr 에 대한 출력 함수(스레드 사용예정)
		void SetMtErr(FILE* wf, JObj* RootMt, JObj* MtMsg, JObj* MtTsh, JArr* MtImgs, int MtStr, FTYPE ftype){
			DynamicStr ErrName(64);
			DynamicStr WrtPath(64);
			DynamicStr NumToStr(16);
			JArr* RootMtName = static_cast<JArr*>(RootMt->Value->P_Type);
			JArr* RootErrName = static_cast<JArr*>(RootMt->next->Value->P_Type);
			JObj* ctrlObjMsg = MtMsg;
			JObj* ctrlObjTsh = MtTsh;

			JArr* MtNames = nullptr;		//모터이름
			JArr* MtErrs = nullptr;			//모터에러명

			JArr* MtTshs = nullptr;			//모터 Tsh
			JArr* ctrlMtImg = nullptr;		//모터 Img

			int MtCnts = RootMt->Value->ArrCnt;
			int ErrCnts = RootMt->next->Value->ArrCnt;

			MtErrs = RootErrName;
			for (int ErrCnt = 0; ErrCnt <= ErrCnts; ErrCnt++){
				//초기화			
				MtNames = RootMtName;
				ctrlMtImg = MtImgs;

				//모터개수 만큼
				for (int Mtcnt = 0; Mtcnt <= MtCnts; Mtcnt++, MtStr++){
					//파일또다시 만들어야함
					WrtPath.Str_Clear();
					if (ftype == FTYPE::ErrMtVac_Kor){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\ko-KR\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Eng){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\en-US\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Ch){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\zh-CN\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Tw){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\zh-TW\\ERROR");
					}

					IntToStr(&NumToStr, MtStr, 4);
					WrtPath.Append_Str(NumToStr.Get_Str());
					WrtPath.Append_Str(".ini");
					FILE* sgl_WF = FileFunc::WtFile(WrtPath.Get_Str());
					NumToStr.Str_Clear();

					MtTshs = static_cast<JArr*>(ctrlObjTsh->Value->P_Type);

					//[MtStr] 섹션 숫자
					fprintf(wf, "[%d]\n", MtStr);
					fprintf(sgl_WF, "[INFO]\n");

					//NAME=
					static_cast<DynamicStr*>(MtNames->Value->P_Type)->Str_Trim_Quote();
					static_cast<DynamicStr*>(MtErrs->Value->P_Type)->Str_Trim_Quote();
					ErrName.Set_Str(static_cast<DynamicStr*>(MtNames->Value->P_Type)->Get_Str());
					ErrName.Append_Char(" ");
					ErrName.Append_Str(static_cast<DynamicStr*>(MtErrs->Value->P_Type)->Get_Str());
					fprintf(wf, "NAME=%s\n", ErrName.Get_Str());
					fprintf(sgl_WF, "NAME=%s\n", ErrName.Get_Str());
					ErrName.Str_Clear();
					//TYPE=E, NEXTACT=C, POSITION=,
					fprintf(wf, "TYPE=E\nNEXTACT=C\nPOSITION=\n");
					fprintf(sgl_WF, "TYPE=E\nNEXTACT=C\nPOSITION=\n");

					//MESSAGE=
					static_cast<DynamicStr*>(ctrlObjMsg->Value->P_Type)->Str_Trim_Quote();
					//static_cast<DynamicStr*>(MtMsg->searchKey("-Lmt")->Value->P_Type)->Str_Trim_Quote();
					ErrName.Set_Str(static_cast<DynamicStr*>(ctrlObjMsg->Value->P_Type)->Get_Str());
					fprintf(wf, "MESSAGE=%s\n", ErrName.Get_Str());
					fprintf(sgl_WF, "MESSAGE=%s\n", ErrName.Get_Str());
					ErrName.Str_Clear();

					//IMG_PATH=
					static_cast<DynamicStr*>(ctrlMtImg->Value->P_Type)->Str_Trim_Quote();
					ErrName.Set_Str(static_cast<DynamicStr*>(ctrlMtImg->Value->P_Type)->Get_Str());
					fprintf(wf, "IMG_PATH=%s\n", ErrName.Get_Str());
					fprintf(sgl_WF, "IMG_PATH=%s\n", ErrName.Get_Str());
					ErrName.Str_Clear();

					//TSHOOT=
					fprintf(wf, "TSHOOT=\n");
					fprintf(sgl_WF, "TSHOOT=\n");
					for (; MtTshs != nullptr; MtTshs = MtTshs->next){
						static_cast<DynamicStr*>(MtTshs->Value->P_Type)->Str_Trim_Quote();
						ErrName.Set_Str(static_cast<DynamicStr*>(MtTshs->Value->P_Type)->Get_Str());
						fprintf(wf, "%s\n", ErrName.Get_Str());
						fprintf(sgl_WF, "%s\n", ErrName.Get_Str());
					}
					fprintf(wf, "\n");
					fprintf(sgl_WF, "\n");
					ErrName.Str_Clear();

					ctrlMtImg = ctrlMtImg->next;
					MtNames = MtNames->next;
					fclose(sgl_WF);
				}

				MtStr = (MtStr / 100) * 100;
				MtStr += 100;

				ctrlObjMsg = ctrlObjMsg->next;
				ctrlObjTsh = ctrlObjTsh->next;
				MtErrs = MtErrs->next;
			}
		}

		void SetVacErr(FILE* wf, JObj* RootVac, JObj* VacMsg, JObj* VacTsh, int VacStr, FTYPE ftype){
			DynamicStr ErrName(64);
			DynamicStr WrtPath(64);
			DynamicStr NumToStr(16);
			JArr* RootVacName = static_cast<JArr*>(RootVac->Value->P_Type);
			JArr* RootErrName = static_cast<JArr*>(RootVac->next->Value->P_Type);
			JObj* ctrlObjMsg = VacMsg;
			JObj* ctrlObjTsh = VacTsh;
			JArr* VacNames = nullptr;		//버큠 이름
			JArr* VacErrs = nullptr;		//버큠에러명
			JArr* VacTshs = nullptr;		//버큠 Tsh

			int VacCnts = RootVac->Value->ArrCnt;
			int ErrCnts = RootVac->next->Value->ArrCnt;

			VacErrs = RootErrName;
			for (int ErrCnt = 0; ErrCnt <= ErrCnts; ErrCnt++){
				//초기화
				VacNames = RootVacName;

				for (int VacCnt = 0; VacCnt <= VacCnts; VacCnt++, VacStr++){
					WrtPath.Str_Clear();
					if (ftype == FTYPE::ErrMtVac_Kor){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\ko-KR\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Eng){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\en-US\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Ch){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\zh-CN\\ERROR");
					}
					else if (ftype == FTYPE::ErrMtVac_Tw){
						WrtPath.Set_Str(".\\Dir\\Output\\ErrMtVac_UTF-8\\zh-TW\\ERROR");
					}

					IntToStr(&NumToStr, VacStr, 4);
					WrtPath.Append_Str(NumToStr.Get_Str());
					WrtPath.Append_Str(".ini");
					FILE* sgl_WF = FileFunc::WtFile(WrtPath.Get_Str());
					NumToStr.Str_Clear();

					VacTshs = static_cast<JArr*>(ctrlObjTsh->Value->P_Type);

					//[VacStr] 섹션 숫자
					fprintf(wf, "[%d]\n", VacStr);
					fprintf(sgl_WF, "[INFO]\n");

					//NAME=
					static_cast<DynamicStr*>(VacNames->Value->P_Type)->Str_Trim_Quote();
					static_cast<DynamicStr*>(VacErrs->Value->P_Type)->Str_Trim_Quote();
					ErrName.Set_Str(static_cast<DynamicStr*>(VacNames->Value->P_Type)->Get_Str());
					ErrName.Append_Str(" ");
					ErrName.Append_Str(static_cast<DynamicStr*>(VacErrs->Value->P_Type)->Get_Str());
					fprintf(wf, "NAME=%s\n", ErrName.Get_Str());
					fprintf(sgl_WF, "NAME=%s\n", ErrName.Get_Str());
					ErrName.Str_Clear();

					//TYPE=E, NEXTACT=C, POSITION=,
					fprintf(wf, "TYPE=E\nNEXTACT=C\nPOSITION=\n");
					fprintf(sgl_WF, "TYPE=E\nNEXTACT=C\nPOSITION=\n");

					//MESSAGE=
					static_cast<DynamicStr*>(ctrlObjMsg->Value->P_Type)->Str_Trim_Quote();
					//static_cast<DynamicStr*>(MtMsg->searchKey("-Lmt")->Value->P_Type)->Str_Trim_Quote();
					ErrName.Set_Str(static_cast<DynamicStr*>(ctrlObjMsg->Value->P_Type)->Get_Str());
					fprintf(wf, "MESSAGE=%s\n", ErrName.Get_Str());
					fprintf(sgl_WF, "MESSAGE=%s\n", ErrName.Get_Str());
					ErrName.Str_Clear();

					//IMG_PATH=
					fprintf(wf, "IMG_PATH=\n");
					fprintf(sgl_WF, "IMG_PATH=\n");

					//TSHOOT=
					fprintf(wf, "TSHOOT=\n");
					fprintf(sgl_WF, "TSHOOT=\n");
					for (; VacTshs != nullptr; VacTshs = VacTshs->next){
						static_cast<DynamicStr*>(VacTshs->Value->P_Type)->Str_Trim_Quote();
						ErrName.Set_Str(static_cast<DynamicStr*>(VacTshs->Value->P_Type)->Get_Str());
						fprintf(wf, "%s\n", ErrName.Get_Str());
						fprintf(sgl_WF, "%s\n", ErrName.Get_Str());
					}
					fprintf(wf, "\n");
					fprintf(sgl_WF, "\n");
					ErrName.Str_Clear();

					VacNames = VacNames->next;
					fclose(sgl_WF);

				}
				VacStr = (VacStr / 100) * 100;
				VacStr += 100;

				ctrlObjMsg = ctrlObjMsg->next;
				ctrlObjTsh = ctrlObjTsh->next;
				VacErrs = VacErrs->next;
			}
		}

		/* ================================================
		* 예외처리부
		==================================================*/
		bool Exc_File_Ck(FILE* rF){

		}

		bool Exc_IO_Obj(JObj* input, JObj* output){
			if (input != nullptr && output != nullptr){
				return true;
			}
			return false;
		}

		/* ================================================
		* UI부
		==================================================*/
		int UI(){
			int select = -1;
			printf("선택하세요\n(1. IO.ini, 2. MotorPos.ini, 3. motor.ini, 4. ErrorList(Kor)");
			printf(", 5.ErrorList(Eng), 6.ErrorList(Ch-간체), 7.ErrorList(Tw-번체), 8.ErrMtVac(Kor), 9.ErrMtVac(Eng), ");
			printf("10.ErrMtVac(Ch-간체), 11.ErrMtVac(Tw-번체)\n");
			scanf_s("%d", &select);
			//select = 5;

			if (select <= 0 || select > 11) return -1;
			return select - 1;
		}

		/* ================================================
		* 프로그램 핵심부
		==================================================*/
		//IO.json -> input.ini, output.ini
		void IO_Make(FILE* rFs){
			if (rFs == nullptr) return;
			DynamicStr* IoStr = FileToStr(rFs);	//소멸 필요
			JNode* RootNode = nullptr;
			RootNode = StrToJs(IoStr);	//소멸 필요
			JNode* I_Obj = nullptr;
			JNode* O_Obj = nullptr;

			Init_IO(static_cast<JObj*>(RootNode->P_Type), &I_Obj, &O_Obj);

			//파일 2개 만들어야함
			const char* W_Input = R"(.\\Dir\\Output\\IO\\Input.ini)";
			const char* W_Output = R"(.\\Dir\\Output\\IO\\Output.ini)";

			//테스트용이었음
			//Json_Func::PrtNode(*I_Obj);

			DynamicStr* secStr = new DynamicStr(16);	//섹션용		소멸필요
			DynamicStr* keyStr = new DynamicStr(32);	//키전용	 	소멸필요

			//Input 먼저 진행
			if (I_Obj == nullptr){
				std::cout << "Input.ini 실패" << std::endl;
			}
			else{
				FILE* wf_input = FileFunc::WtFile(W_Input);
				JNode* CurNode = I_Obj;
				JObj* CurObj = static_cast<JObj*>(CurNode->P_Type);
				JArr* CurArr = nullptr;

				for (int ch = 0; ch < CurNode->ObjCnt; ch++){
					fprintf(wf_input, ";Input Channel %d\n", ch);
					secStr->Str_Clear();
					IntToStr(secStr, ch, 2);
					char Pol = 'A';
					CurArr = static_cast<JArr*>(CurObj->Value->P_Type);
					for (int bit = 0; bit < 16; bit++){
						Pol = 'A';
						char c = ' ';
						//섹션 만들어주기
						if (bit <= 9){
							c = '0' + bit;
						}
						else{
							c = 'A' + (bit - 10);
						}
						fprintf(wf_input, "[%s_%c]\n", secStr->Get_Str(), c);
						//키값 만들어주기
						DynamicStr* debug = static_cast<DynamicStr*>(CurArr->Value->P_Type);
						keyStr->Set_Str(static_cast<DynamicStr*>(CurArr->Value->P_Type)->Get_Str());
						keyStr->Str_Trim_Quote();

						StrCkPol(keyStr, Pol);

						fprintf(wf_input, "ctrlname=\n");
						fprintf(wf_input, "uiname=%s\n", keyStr->Get_Str());
						fprintf(wf_input, "comment=\n");
						fprintf(wf_input, "polar=%c\n", Pol);
						CurArr = CurArr->next;
					}
					CurObj = CurObj->next;
				}
				fclose(wf_input);
			}


			//Output 먼저 진행
			if (O_Obj == nullptr){
				std::cout << "Output.ini 실패" << std::endl;
			}
			else{
				FILE* wf_output = FileFunc::WtFile(W_Output);
				JNode* CurNode = O_Obj;
				JObj* CurObj = static_cast<JObj*>(CurNode->P_Type);
				JArr* CurArr = nullptr;

				for (int ch = 0; ch < CurNode->ObjCnt; ch++){
					fprintf(wf_output, ";Output Channel %d\n", ch);
					secStr->Str_Clear();
					IntToStr(secStr, ch, 2);
					char Pol = 'A';
					CurArr = static_cast<JArr*>(CurObj->Value->P_Type);
					for (int bit = 0; bit < 16; bit++){
						Pol = 'A';
						char c = ' ';
						//섹션 만들어주기
						if (bit <= 9){
							c = '0' + bit;
						}
						else{
							c = 'A' + (bit - 10);
						}
						fprintf(wf_output, "[%s_%c]\n", secStr->Get_Str(), c);
						//키값 만들어주기
						DynamicStr* debug = static_cast<DynamicStr*>(CurArr->Value->P_Type);
						keyStr->Set_Str(static_cast<DynamicStr*>(CurArr->Value->P_Type)->Get_Str());
						keyStr->Str_Trim_Quote();

						StrCkPol(keyStr, Pol);

						fprintf(wf_output, "ctrlname=\n");
						fprintf(wf_output, "uiname=%s\n", keyStr->Get_Str());
						fprintf(wf_output, "comment=\n");
						fprintf(wf_output, "polar=%c\n", Pol);
						fprintf(wf_output, "value=1\n");
						CurArr = CurArr->next;
					}
					CurObj = CurObj->next;
				}


				fclose(wf_output);
			}

			delete RootNode;
			delete IoStr;
			if (secStr != nullptr) delete secStr;
			if (keyStr != nullptr) delete keyStr;
		}


		//Motor.json -> motorpos.ini
		void MtPos_Make(FILE* rFs){
			if (rFs == nullptr) return;

			DynamicStr* mtPosStr = FileToStr(rFs);	//소멸필요
			JNode* RootNode = nullptr;	//소멸필요
			RootNode = StrToJs(mtPosStr);

			JObj* MtObj = static_cast<JObj*>(RootNode->P_Type);
			JNode* ArrNode = static_cast<JNode*>(MtObj->Value);
			JArr* MtArr = static_cast<JArr*>(MtObj->Value->P_Type);

			//파일 여러개 만들어야함
			DynamicStr* AddPath = new DynamicStr(64);	//소멸 필요
			DynamicStr* BasePath = new DynamicStr(64);	//소멸 필요
			DynamicStr* NamePath = new DynamicStr(32);	//소멸 필요
			DynamicStr* tmpStr = new DynamicStr(16);	//소멸 필요

			BasePath->Set_Str(".\\Dir\\Output\\MotorPos\\");


			for (int MtCnt = 0; MtCnt <= RootNode->ObjCnt; MtCnt++){
				NamePath->Str_Clear();
				tmpStr->Str_Clear();
				AddPath->Str_Clear();
				NamePath->Set_Str("motor_");
				IntToStr(tmpStr, MtCnt, 0);
				NamePath->Append_Str(tmpStr->Get_Str());
				NamePath->Append_Str(".ini");

				AddPath->Append_Str(BasePath->Get_Str());
				AddPath->Append_Str(NamePath->Get_Str());

				MtArr = static_cast<JArr*>(MtObj->Value->P_Type);
				ArrNode = static_cast<JNode*>(MtObj->Value);
				FILE* wt_mt = FileFunc::WtFile(AddPath->Get_Str());
				fprintf(wt_mt, ";%s\n", MtObj->Key.Get_Str());
				for (int PosCnt = 0; PosCnt <= ArrNode->ArrCnt; PosCnt++){
					(static_cast<DynamicStr*>(MtArr->Value->P_Type))->Str_Trim_Quote();
					fprintf(wt_mt, "[POS_%d]\n", PosCnt);
					fprintf(wt_mt, "NAME=%s\n", (static_cast<DynamicStr*>(MtArr->Value->P_Type))->Get_Str());
					fprintf(wt_mt, "USE=POS|SPD\n");
					fprintf(wt_mt, "ALLSAVE=0\n");
					fprintf(wt_mt, "COMMENT=\n");

					MtArr = MtArr->next;

				}
				fclose(wt_mt);
				MtObj = MtObj->next;
			}


			delete tmpStr;
			delete AddPath;
			delete BasePath;
			delete NamePath;
			delete RootNode;
			delete mtPosStr;
		}


		//motor.ini
		void Mt_Make(FILE* rFs){
			if (rFs == nullptr) return;

			DynamicStr* MtStr = FileToStr(rFs);	//소멸필요
			JNode* RootNode = nullptr;	//소멸필요
			RootNode = StrToJs(MtStr); //소멸필요

			JArr* mtKeys = nullptr;
			JArr* RtMtKeys = nullptr;
			JArr* mtCtrName = nullptr;
			JArr* mtUiName = nullptr;
			JArr* mtLdVal = nullptr;
			JObj* ctrlObj = nullptr;
			JNode* ctrlNode = nullptr;


			Init_Mt(RootNode, &mtKeys, &mtCtrName, &mtUiName, &mtLdVal);
			RtMtKeys = mtKeys;
			ctrlObj = static_cast<JObj*>(RootNode->P_Type);				//Arr 개수 용도외에는 아무것도 아님 무시하셈
			ctrlNode = static_cast<JNode*>(ctrlObj->Value);				//Arr 개수 용도외에는 아무것도 아님 무시하셈
			int KeyCnt = ctrlNode->ArrCnt;
			ctrlObj = ctrlObj->searchKey("Ctrl Name");
			ctrlNode = static_cast<JNode*>(ctrlObj->Value);
			int MtCnt = ctrlNode->ArrCnt;


			//파일 관련
			const char* wt_mtPath = ".\\Dir\\Output\\motor.ini";
			FILE* wt_mt = FileFunc::WtFile(wt_mtPath);


			//Lead값 관련
			double MtVals[MTINI::MTEND] = { 0.0 };
			double LeadVal = 0;
			int* tmpNum = nullptr;

			//이름 관련
			DynamicStr* mtName = new DynamicStr(64);		//소멸자 필요
			DynamicStr* UiName = new DynamicStr(64);		//소멸자 필요
			DynamicStr* KeyName = new DynamicStr(64);		//소멸자 필요


			for (int mtCnt = 0; mtCnt <= MtCnt; mtCnt++){

				mtName->Str_Clear();
				UiName->Str_Clear();

				//Lead값 알아내기
				fprintf(wt_mt, "\n[MOTOR_%d]\n", mtCnt);
				tmpNum = static_cast<int*>(mtLdVal->Value->P_Type);
				LeadVal = (double)*tmpNum;
				MtIniCal(MtVals, LeadVal);	//계산해주기

				(static_cast<DynamicStr*>(mtCtrName->Value->P_Type))->Str_Trim_Quote();
				(static_cast<DynamicStr*>(mtUiName->Value->P_Type))->Str_Trim_Quote();
				mtKeys = RtMtKeys;
				for (int key = 0; key <= KeyCnt; key++){
					KeyName->Str_Clear();
					KeyName->Set_Str((static_cast<DynamicStr*>(mtKeys->Value->P_Type))->Get_Str());
					KeyName->Str_Trim_Quote();

					//파일쓰기 시작

					if (KeyName->StrCmp("CTRLNAME")){
						fprintf(wt_mt, "%-38s = %s\n", KeyName->Get_Str(), (static_cast<DynamicStr*>(mtCtrName->Value->P_Type))->Get_Str());
					}
					else if (KeyName->StrCmp("UINAME")){
						fprintf(wt_mt, "%-38s = %s\n", KeyName->Get_Str(), (static_cast<DynamicStr*>(mtUiName->Value->P_Type))->Get_Str());
					}
					else if (KeyName->StrCmp("COMMENT")){
						fprintf(wt_mt, "%-38s = [P%02d]\n", KeyName->Get_Str(), mtCnt);
					}
					else if (KeyName->StrCmp("PULSE_PER_TURN")
						|| KeyName->StrCmp("MTENCCNT_PER_TURN")){
						fprintf(wt_mt, "%-38s = %d\n", KeyName->Get_Str(), 8388608);
					}
					else if (KeyName->StrCmp("DISTANCE_PER_TURN")){	//Lead값
						fprintf(wt_mt, "%-38s = %d\n", KeyName->Get_Str(), (int)LeadVal);
					}

					//수식넣는 부분
					else if (KeyName->StrCmp("CIA402_HOMING_SPEED_ZERO")){	//느리게 홈잡을때
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::SLWHOMZERO]);
					}
					else if (KeyName->StrCmp("CIA402_HOMING_SPEED_SWITCH")){
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::HOMSPDSWT]);
					}
					else if (KeyName->StrCmp("CIA402_HOMING_ACCELERATION")){
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::HOMACC]);
					}
					else if (KeyName->StrCmp("CIA402_PROFILE_ACCELERATION")
						|| KeyName->StrCmp("CIA402_PROFILE_DECELERATION")
						|| KeyName->StrCmp("CIA402_MAX_PROFILE_VELOCITY")
						|| KeyName->StrCmp("CIA402_PROFILE_VELOCITY")){
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::PRFVLCMAX]);	//PRF_ACC_DEC써도됨
					}
					else if (KeyName->StrCmp("CIA402_QUICK_STOP_DECELERATION")){
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::QCK_STOP]);
					}
					else if (KeyName->StrCmp("CIA402_VELOCITY_WINDOW")
						|| KeyName->StrCmp("CIA402_POSITION_WINDOW")){
						fprintf(wt_mt, "%-38s = %lld\n", KeyName->Get_Str(), (long long)MtVals[MTINI::VLCPOSWIN]);
					}
					else{
						fprintf(wt_mt, "%-38s = \n", KeyName->Get_Str());
					}
					mtKeys = mtKeys->next;
				}
				mtCtrName = mtCtrName->next;
				mtUiName = mtUiName->next;
				mtLdVal = mtLdVal->next;
			}



			fclose(wt_mt);
			delete RootNode;
			delete MtStr;
			delete mtName;
			delete UiName;
		}


		//Error List (유일하게 .ini를 받아서 나눠줌 나머지는 .json인데 예만 ini임)
		void Err_Make(FILE* rFs, FTYPE lng){
			if (rFs == nullptr) return;

			DynamicStr* ErrStr = FileToStr(rFs);	//소멸 필요
			IniPrs ini(ErrStr->Get_Str());			//소멸 불필요(지역 인스턴스라 알아서 지워줄거임)
			ini.ErrIniToJs();						//Ini To Json화 시작
			JNode* RootNode = ini.getNode();		//소멸 필요	넘겨지는 순간부터 손을 떠난거로 만들어서 필요함

			//Json_Func::PrtNode(*RootNode);

			JObj* RootObj = static_cast<JObj*>(RootNode->P_Type);
			JObj* CurObj = RootObj;

			DynamicStr* WrtPath;	//소멸필요
			FILE* WrtEF;			//소멸필요

			for (int i = 0; CurObj != nullptr; i++){
				//파일 이름 생성
				WrtPath = new DynamicStr(128);

				if (lng == FTYPE::ErrIni_Kor){
					WrtPath->Set_Str(".\\Dir\\Output\\ErrorList_UTF-8\\ko-KR\\ERROR");			//한국어
				}
				else if (lng == FTYPE::ErrIni_Eng){
					WrtPath->Set_Str(".\\Dir\\Output\\ErrorList_UTF-8\\en-US\\ERROR");			//영어
				}
				else if (lng == FTYPE::ErrIni_Ch){
					WrtPath->Set_Str(".\\Dir\\Output\\ErrorList_UTF-8\\zh-CN\\ERROR");			//중국어(간체)
				}
				else if (lng == FTYPE::ErrIni_Tw){
					WrtPath->Set_Str(".\\Dir\\Output\\ErrorList_UTF-8\\zh-TW\\ERROR");			//중국어(대만어 번체)
				}
				ErrNumMak(WrtPath, CurObj->Key.Get_Str());
				WrtPath->Append_Str(".ini");

				//실제 파일 생성
				WrtEF = FileFunc::WtFile(WrtPath->Get_Str());

				//파일에 값 넣기
				SetWrtEF(WrtEF, CurObj);

				CurObj = CurObj->next;
				delete WrtPath;
				WrtPath = nullptr;
				fclose(WrtEF);
			}


			delete RootNode;
			delete ErrStr;
		}


		//Error List (Motor Vac만)	.json을 ini로 여러개랑 1개짜리 2개 만들어야함
		void Err_Make_MtVac(FILE* rFs, FTYPE ftype){
			if (rFs == nullptr) return;
			DynamicStr* ErrStr = FileToStr(rFs);		//소멸필요
			JNode* RootNode = nullptr;					//소멸필요
			RootNode = StrToJs(ErrStr);

			JObj* MtObj, *VacObj = nullptr;
			JArr *MtImgs = nullptr;
			Init_MtVac(RootNode, &MtObj, &VacObj, &MtImgs);

			//모터버큠 메시지 tsh 얻기
			JObj* MtMsg, *MtTsh = nullptr;
			JObj* VacMsg, *VacTsh = nullptr;
			Init_Msg_Tsh(MtObj, &MtMsg, &MtTsh);
			Init_Msg_Tsh(VacObj, &VacMsg, &VacTsh);

			FILE* WrtMt = nullptr;		//파일 클로즈 필요
			FILE* WrtVac = nullptr;		//파일 클로즈 필요
			int VacStr = 1500;
			int MtStr = VacStr + 200;
			if (ftype == FTYPE::ErrMtVac_Kor){
				WrtMt = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Kor_mt.ini");
				WrtVac = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Kor_vac.ini");
				SetMtErr(WrtMt, MtObj, MtMsg, MtTsh, MtImgs, MtStr, FTYPE::ErrMtVac_Kor);
				SetVacErr(WrtVac, VacObj, VacMsg, VacTsh, VacStr, FTYPE::ErrMtVac_Kor);
			}
			else if (ftype == FTYPE::ErrMtVac_Eng){
				WrtMt = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Eng_mt.ini");
				WrtVac = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Eng_vac.ini");
				SetMtErr(WrtMt, MtObj, MtMsg, MtTsh, MtImgs, MtStr, FTYPE::ErrMtVac_Eng);
				SetVacErr(WrtVac, VacObj, VacMsg, VacTsh, VacStr, FTYPE::ErrMtVac_Eng);
			}
			else if (ftype == FTYPE::ErrMtVac_Ch){
				WrtMt = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Ch_mt.ini");
				WrtVac = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Ch_vac.ini");
				SetMtErr(WrtMt, MtObj, MtMsg, MtTsh, MtImgs, MtStr, FTYPE::ErrMtVac_Ch);
				SetVacErr(WrtVac, VacObj, VacMsg, VacTsh, VacStr, FTYPE::ErrMtVac_Ch);
			}
			else if (ftype == FTYPE::ErrMtVac_Tw){
				WrtMt = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Tw_mt.ini");
				WrtVac = FileFunc::WtFile(".\\Dir\\Output\\ErrMtVac_UTF-8\\ErrMtVac\\ErrMtVac_Tw_vac.ini");
				SetMtErr(WrtMt, MtObj, MtMsg, MtTsh, MtImgs, MtStr, FTYPE::ErrMtVac_Tw);
				SetVacErr(WrtVac, VacObj, VacMsg, VacTsh, VacStr, FTYPE::ErrMtVac_Tw);
			}

			fclose(WrtMt);
			fclose(WrtVac);
			delete RootNode;
			delete ErrStr;
		}

	public:
		IniMain(){}
		~IniMain(){}
		void Main(){
			//초기화 작업 파일읽어 들이기
			FILE** rFs = nullptr;
			DynamicStr** Paths = nullptr;
			Init_Path(Paths);//소멸자 필요
			Init_File(rFs, Paths);	//소멸자 필요			

			//UI
			int select = UI();
			switch (select){
			case FTYPE::IO:
				IO_Make(rFs[FTYPE::IO]);
				break;
			case FTYPE::MtPos:
				MtPos_Make(rFs[FTYPE::MtPos]);
				break;
			case FTYPE::Mt:
				Mt_Make(rFs[FTYPE::Mt]);
				break;
			case FTYPE::ErrIni_Kor:
				Err_Make(rFs[FTYPE::ErrIni_Kor], FTYPE::ErrIni_Kor);
				break;
			case FTYPE::ErrIni_Eng:
				Err_Make(rFs[FTYPE::ErrIni_Eng], FTYPE::ErrIni_Eng);
				break;
			case FTYPE::ErrIni_Ch:
				Err_Make(rFs[FTYPE::ErrIni_Ch], FTYPE::ErrIni_Ch);
				break;
			case FTYPE::ErrIni_Tw:
				Err_Make(rFs[FTYPE::ErrIni_Tw], FTYPE::ErrIni_Tw);
				break;
			case FTYPE::ErrMtVac_Kor:
				Err_Make_MtVac(rFs[FTYPE::ErrMtVac_Kor], FTYPE::ErrMtVac_Kor);
				break;
			case FTYPE::ErrMtVac_Eng:
				Err_Make_MtVac(rFs[FTYPE::ErrMtVac_Eng], FTYPE::ErrMtVac_Eng);
				break;
			case FTYPE::ErrMtVac_Ch:
				Err_Make_MtVac(rFs[FTYPE::ErrMtVac_Ch], FTYPE::ErrMtVac_Ch);
				break;
			case FTYPE::ErrMtVac_Tw:
				Err_Make_MtVac(rFs[FTYPE::ErrMtVac_Tw], FTYPE::ErrMtVac_Tw);
				break;
			default:
				printf("선택 실패\n");
				printf("종료\n");
				break;
			}


			Del_Path_File(Paths, rFs);
		}
	};


}