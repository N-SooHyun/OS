#pragma once

#include <stdio.h>
#include <iostream>
#include "Str.h"
#include "Timer.h"

using namespace std;
using namespace Dynamic;

namespace JSON {
	class JObj;
	class JArr;
	class JsonCallObjArr;

	/* ================================================
	* 기능 API(상위 기능)
	==================================================*/

	class JNode {
	public:
		enum class JType {
			OBJ,
			ARR,
			NUMBER,
			DOUBLE,
			BOOL,
			STRING,
			NULLTYPE,	//널
		};

		//내부 리소스 Ptype을 삭제시켜줌
		void delType();

		JNode() : Cur_Type(JType::NULLTYPE){
			setType(Cur_Type);
		}
		JNode(JType Set_Node_Type) : Cur_Type(Set_Node_Type){
			setType(Cur_Type);
		}
		//소멸자
		~JNode(){
			delType();
		}

		//복사 생성자
		JNode(const JNode& other) : Cur_Type(other.Cur_Type), P_Type(other.P_Type){

		}
		// 이동 생성자
		JNode(const JNode&& other) : Cur_Type(other.Cur_Type), P_Type(other.P_Type) {

		}

		JsonCallObjArr push();
	private:
		JsonCallObjArr obj_push(const char* key);	//추후 생성 예정 안쓰는게 좋을거 같기도 하고
		JsonCallObjArr obj_push(const unsigned char* key);
	public:

		JsonCallObjArr arr_del(int index);			//선택해서 삭제
		JsonCallObjArr* arr_del();					//끝에만 삭제
		JsonCallObjArr obj_del(const char* key);	//선택해서 삭제
		JsonCallObjArr obj_del(const unsigned char* key);
		JsonCallObjArr* obj_del();					//끝에만 삭제

		//push del 통합 메소드 호출용 메소드
		JsonCallObjArr* del();
		void all_del();		//모든 객체 삭제

		void operator=(JType);

		bool isTypeNull();	//true면 P_Type == nullptr
		bool isTypeMatch(JType); //true면 P_Type == Type 매치된상태
		bool isOverWrite();	//덮어쓰기 혹은 생성하기

		void setType(JType);
		void* getPType();
		JType getType();

		//문자열 파싱 부분
		void isObjArrCk(DynamicStr* str);

		//반환
		operator int();
		operator int*();
		operator double();
		operator double*();
		operator bool();
		operator bool*();
		operator char();
		operator char*();
		operator unsigned char();
		operator unsigned char*();
	private:
		operator JObj();			//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		operator JArr();			//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		operator JArr*();			//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		operator JObj*();			//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		operator JNode();			//JNode = JNode 이런경우 lValue만 사용되기에 반환은 호출이 안됨
		operator JNode*();			//고로 있으나 마나 반환은 쓰이지 않을예정
	public:

		//대입
		void operator=(int);
		void operator=(int*);
		void operator=(double);
		void operator=(double*);
		void operator=(bool);
		void operator=(bool*);
		void operator=(char);
		void operator=(const char*);
		void operator=(char*);//변수로 들어오는 경우를 위함
		void operator=(unsigned char);
		void operator=(const unsigned char*);
		void operator=(unsigned char*);
	private:
		void operator=(JObj);		//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		void operator=(JArr);		//안만들 예정 소멸할때 골치아픔(지역변수 동적변수 구분해야함)
		void operator=(JNode);		//rValue를 복사후 rValue.P_Type = nullptr로 만들어주어도 복사생성자로 생성된 객체가 되어서 결국 이중소멸됨
		void operator=(JObj*);
		void operator=(JArr*);
	public:
		//해당 대입들은 rValue가 무조건 nullptr로 변한된다고 가정 즉, lValue = rValue에서 lValue로 완전한 이동인셈
		void operator=(JNode*);

		//객체 배열 연산자 오버로딩
		JsonCallObjArr operator[](const char* key);
		JsonCallObjArr operator[](const unsigned char* key);
		JsonCallObjArr operator[](int index);

		JType Cur_Type;
		void* P_Type;
		//JNode* prev_Node;
		//JObj* prev_obj;
		//JArr* prev_arr;
		int ObjCnt;
		int ArrCnt;
	private:
		friend JsonCallObjArr;
		friend JObj;
		friend JArr;
	};

	class JObj{
	public:
		JObj() :Key(128), Value(new JNode()), next(nullptr){}
		JObj(const char* key) : Key(128), Value(new JNode()), next(nullptr){
			Key.Set_Str(key);
		}
		JObj(const unsigned char* key) : Key(128), Value(new JNode()), next(nullptr){
			Key.Set_Str(key);
		}
		~JObj() {
			if (Value != nullptr){
				delete Value;
				//Value = nullptr;
			}
			if (next != nullptr){
				delete next;
				next = nullptr;
			}
		}

		//자신의 Obj기준 next참조해서 key와 매치되는 Obj 반환
		JObj* searchKey(const char* key){
			//현재 위치를 기준으로 찾는거임
			JObj* cur_obj = this;

			do{
				if (cur_obj->Key.StrCmp(key)) return cur_obj;
				cur_obj = cur_obj->next;
			} while (cur_obj != nullptr);

			return nullptr;
		}

		//자신의 Obj기준 next참조해서 key와 매치되는 Obj 반환
		JObj* searchKey(const unsigned char* key){
			//현재 위치를 기준으로 찾는거임
			JObj* cur_obj = this;

			do{
				if (cur_obj->Key.StrCmp(key)) return cur_obj;
				cur_obj = cur_obj->next;
			} while (cur_obj != nullptr);

			return nullptr;
		}

		//자신의 Obj기준 Tail위치 반환
		JObj* getTailObj(){
			JObj* cur_obj = this;

			if (cur_obj == nullptr) return nullptr;

			for (; cur_obj->next != nullptr;){
				cur_obj = cur_obj->next;
			}

			return cur_obj;
		}

		JObj* getPrevObj(JObj* cur_obj, JNode* root_node){
			JObj* root_obj = static_cast<JObj*>(root_node->P_Type);

			if (cur_obj == nullptr) return nullptr;

			if (root_obj->next == nullptr)
				return nullptr;

			if (root_obj == cur_obj) return nullptr;

			for (;;){
				if (root_obj->next == cur_obj) break;
				root_obj = root_obj->next;
			}

			return root_obj;	//cur_obj == root_obj->next
		}

		//키값 생성
		void setKey(const char* key){
			if (Value->Cur_Type != JNode::JType::NULLTYPE)
				return;//만약에 이런경우면 기존키값이 변경됨 그래서 실패
			Key.Set_Str(key);
		}
		void setKey(const unsigned char* key){
			if (Value->Cur_Type != JNode::JType::NULLTYPE)
				return;//만약에 이런경우면 기존키값이 변경됨 그래서 실패
			Key.Set_Str(key);
		}

		void setValue(JNode::JType setNodeType){
			//덮어쓰는거임
			Value->delType();
			Value->setType(Value->Cur_Type);
		}

		//private:
		DynamicStr Key;		//키값
		JNode* const Value;		//이친구는 상수화 시켜야 하나
		JObj* next;
		friend JsonCallObjArr;
	};

	class JArr{
	public:
		JArr() : Value(new JNode()), next(nullptr){}

		~JArr(){
			if (Value != nullptr){
				delete Value;
				//Value = nullptr;
			}
			if (next != nullptr){
				delete next;
				next = nullptr;
			}
		}

		//Arr기준 Root라고 가정 next참조해서 index를 반환
		JArr* RootSearchIndex(int index){
			if (index <= -1) return nullptr;	//예외처리 -1

			JArr* cur_arr = this;

			int idx = 0;

			while (cur_arr != nullptr){
				if (idx == index){
					return cur_arr;
				}

				cur_arr = cur_arr->next;
				idx++;
			}

			return nullptr;
		}

		//자신의 arr기준 tail위치 반환
		JArr* getTailArr(){
			JArr* cur_arr = this;

			if (cur_arr == nullptr) return nullptr;

			for (; cur_arr->next != nullptr;){
				cur_arr = cur_arr->next;
			}

			return cur_arr;
		}

		//자신의 arr기준 tail위치 반환 + idx값도 반환해줄 수 있음 포인터로
		JArr* getTailArr(int *idx){
			JArr* cur_arr = this;
			JArr* prev_arr = nullptr;

			*idx = -1;

			if (cur_arr == nullptr) return nullptr;

			for (; cur_arr != nullptr;){
				prev_arr = cur_arr;
				cur_arr = cur_arr->next;
				*idx += 1;
			}

			return prev_arr;
		}

		JArr* getPrevArr(JArr* cur_arr, JNode* root_node){
			JArr* root_arr = static_cast<JArr*>(root_node->P_Type);

			if (cur_arr == nullptr) return nullptr;

			if (root_arr->next == nullptr)
				return nullptr;

			if (root_arr == cur_arr) return nullptr;

			for (;;){
				if (root_arr->next == cur_arr) break;
				root_arr = root_arr->next;
			}

			return root_arr;
		}

		void setValue(JNode::JType setNodeType){
			//덮어쓰기
			Value->delType();
			Value->setType(Value->Cur_Type);
		}


		//private:
		JNode* const Value;
		JArr* next;
		friend JsonCallObjArr;
	};

	//연산자오버로딩을 위한 클래스 JNode node; node["Key"] or node[3] 이런식의 접근을 허용해주기 위함
	class JsonCallObjArr {
	public:
		JsonCallObjArr() : Root_Node(nullptr), Root_Obj(nullptr), Root_Arr(nullptr){}
		JsonCallObjArr(JNode* node, const unsigned char* key, bool trg_del = false){
			Root_Node = node;
			Root_Obj = static_cast<JObj*>(Root_Node->P_Type);
			JObj* search_key_obj = Root_Obj->searchKey(key);
			JObj* tail_obj = Root_Obj->getTailObj();

			//1. Root_Node의 상태가 "{}" 이런 상태일때 아무 객체도 없을때
			if (Root_Node->ObjCnt == -1){
				if (trg_del == true){
					//삭제 할 객체가 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}
				Root_Obj->setKey(key);			//키값 설정
				Root_Obj->setValue(JNode::JType::NULLTYPE);	//일단 초기화니까 NullType넣어주기
				Root_Obj->next = nullptr;
				Root_Node->ObjCnt++;

				//연산자 오버로딩에서 사용할 리소스 초기화 해주기
				Cur_Obj = Root_Obj;
			}

			//2. Root_Node의 상태가 "{...}"일때 객체가 존재할때 해당키인 객체를 찾기

			else if (search_key_obj != nullptr){	//이미 해당 키를 가진 객체가 존재함
				if (trg_del == true){
					//삭제할 객체가 존재할때
					JObj* prev_obj = search_key_obj->getPrevObj(search_key_obj, Root_Node);
					//1. 1개 혹은 여러개 있으나 0번 객체일때
					if (prev_obj == nullptr){
						//1. 1개만 있을때
						if (search_key_obj->next == nullptr){
							Root_Node->delType();
							Root_Node->setType(JNode::JType::OBJ);
						}
						//2. 여러개 있으나 0번 객체를 지우려고 할때
						else{
							JObj* next_root_obj = search_key_obj->next;
							search_key_obj->next = nullptr;
							Root_Node->delType();
							Root_Node->Cur_Type = JNode::JType::OBJ;
							Root_Node->P_Type = next_root_obj;
							Root_Node->ObjCnt--;
						}
					}
					//2. 여러개 있을때
					else{
						prev_obj->next = search_key_obj->next;
						search_key_obj->next = nullptr;
						delete search_key_obj;
						Root_Node->ObjCnt--;
					}

					return;
				}
				//연산자 오버로딩에서 처리하도록 해당 키를 가진 객체를 넘겨주기
				Cur_Obj = search_key_obj;
			}
			else{//해당 키를 가진 객체가 존재하지 않음
				if (trg_del == true){
					//삭제할 객체가 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}

				//새롭게 키를 만들어줘야함
				JObj* new_obj = new JObj(key);
				new_obj->setValue(JNode::JType::NULLTYPE);
				new_obj->next = nullptr;
				Root_Node->ObjCnt++;

				tail_obj->next = new_obj;

				//연산자 오버로딩에서 처리하도록 해당 키를 가진 객체를 넘겨주기
				Cur_Obj = new_obj;
			}


			//연산자 오버로딩에서 사용할 리소스 초기화 해주기
			Cur_Node = Root_Node;
			Cur_Arr = Root_Arr;			//nullptr
		}
		JsonCallObjArr(JNode* node, const char* key, bool trg_del = false) : Root_Arr(nullptr){
			Root_Node = node;
			Root_Obj = static_cast<JObj*>(Root_Node->P_Type);
			JObj* search_key_obj = Root_Obj->searchKey(key);
			JObj* tail_obj = Root_Obj->getTailObj();

			//1. Root_Node의 상태가 "{}" 이런 상태일때 아무 객체도 없을때
			if (Root_Node->ObjCnt == -1){
				if (trg_del == true){
					//삭제 할 객체가 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}
				Root_Obj->setKey(key);			//키값 설정
				Root_Obj->setValue(JNode::JType::NULLTYPE);	//일단 초기화니까 NullType넣어주기
				Root_Obj->next = nullptr;
				Root_Node->ObjCnt++;

				//연산자 오버로딩에서 사용할 리소스 초기화 해주기
				Cur_Obj = Root_Obj;
			}

			//2. Root_Node의 상태가 "{...}"일때 객체가 존재할때 해당키인 객체를 찾기

			else if (search_key_obj != nullptr){	//이미 해당 키를 가진 객체가 존재함
				if (trg_del == true){
					//삭제할 객체가 존재할때
					JObj* prev_obj = search_key_obj->getPrevObj(search_key_obj, Root_Node);
					//1. 1개 혹은 여러개 있으나 0번 객체일때
					if (prev_obj == nullptr){
						//1. 1개만 있을때
						if (search_key_obj->next == nullptr){
							Root_Node->delType();
							Root_Node->setType(JNode::JType::OBJ);
						}
						//2. 여러개 있으나 0번 객체를 지우려고 할때
						else{
							JObj* next_root_obj = search_key_obj->next;
							search_key_obj->next = nullptr;
							Root_Node->delType();
							Root_Node->Cur_Type = JNode::JType::OBJ;
							Root_Node->P_Type = next_root_obj;
							Root_Node->ObjCnt--;
						}
					}
					//2. 여러개 있을때
					else{
						prev_obj->next = search_key_obj->next;
						search_key_obj->next = nullptr;
						delete search_key_obj;
						Root_Node->ObjCnt--;
					}

					return;
				}
				//연산자 오버로딩에서 처리하도록 해당 키를 가진 객체를 넘겨주기
				Cur_Obj = search_key_obj;
			}
			else{//해당 키를 가진 객체가 존재하지 않음
				if (trg_del == true){
					//삭제할 객체가 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}

				//새롭게 키를 만들어줘야함
				JObj* new_obj = new JObj(key);
				new_obj->setValue(JNode::JType::NULLTYPE);
				new_obj->next = nullptr;
				Root_Node->ObjCnt++;

				tail_obj->next = new_obj;

				//연산자 오버로딩에서 처리하도록 해당 키를 가진 객체를 넘겨주기
				Cur_Obj = new_obj;
			}


			//연산자 오버로딩에서 사용할 리소스 초기화 해주기
			Cur_Node = Root_Node;
			Cur_Arr = Root_Arr;			//nullptr
		}
		JsonCallObjArr(JNode* node, int index, bool trg_del = false) : Root_Obj(nullptr){
			Root_Node = node;
			int max_idx = -1;
			Root_Arr = static_cast<JArr*>(Root_Node->P_Type);
			JArr* search_idx_arr = Root_Arr->RootSearchIndex(index);
			JArr* tail_arr = Root_Arr->getTailArr(&max_idx);

			if (index == -1) index = max_idx + 1;		//-1이면 그냥 뒤에 추가

			//1. Root_Node의 상태가 "[]" 이런 상태일때 아무 배열에 값도 없을때 
			if (Root_Node->ArrCnt == -1){
				if (trg_del == true){
					//삭제할 배열이 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}

				if (index != 0){//실패
					Cur_Arr = nullptr;
				}
				else{
					Root_Arr->setValue(JNode::JType::NULLTYPE);
					Root_Arr->next = nullptr;
					Root_Node->ArrCnt++;

					Cur_Arr = Root_Arr;
				}
			}

			//2. Root_Node의 상태가 "[...]"이런 상태일때 배열의 값이 존재할때

			// 해당 idx를 가진 값이 존재할때
			else if (search_idx_arr != nullptr){
				if (trg_del == true){
					//삭제할 인덱스의 요소가 존재할때
					JArr* prev_arr = search_idx_arr->getPrevArr(search_idx_arr, Root_Node);
					//1. 1개 혹은 여러개 있으나 0번 객체일때
					if (prev_arr == nullptr){
						//1. 1개만 있을때
						if (search_idx_arr->next == nullptr){
							Root_Node->delType();
							Root_Node->setType(JNode::JType::ARR);
						}

						//2. 여러개 있으나 0번 객체를 지우려고 할때
						else{
							JArr* next_root_arr = search_idx_arr->next;
							search_idx_arr->next = nullptr;
							Root_Node->delType();
							Root_Node->Cur_Type = JNode::JType::ARR;
							Root_Node->P_Type = next_root_arr;
							Root_Node->ArrCnt--;
						}
					}
					//2. 여러개 있을때
					else{
						prev_arr->next = search_idx_arr->next;
						search_idx_arr->next = nullptr;
						delete search_idx_arr;
						Root_Node->ArrCnt--;
					}

					return;
				}
				Cur_Arr = search_idx_arr;
			}
			// 해당 idx를 가진 값이 존재하지 않을때

			else{
				if (trg_del == true){
					//삭제할 인덱스의 요소가 존재하지 않을때
					Cur_Obj = nullptr;
					Cur_Arr = nullptr;
					Cur_Node = nullptr;
					return;
				}

				if (index > max_idx + 1){		//배열의 크기보다 클때 실패
					//연산자오버로딩에서 실패하도록 만들기
					Cur_Arr = nullptr;
				}
				else{
					JArr* new_arr = new JArr();
					new_arr->setValue(JNode::JType::NULLTYPE);
					new_arr->next = nullptr;
					Root_Node->ArrCnt++;

					tail_arr->next = new_arr;

					Cur_Arr = new_arr;
				}
			}

			//연산자 오버로딩에서 사용할 리소스 초기화 해주기
			Cur_Node = Root_Node;
			Cur_Obj = Root_Obj;		//nullptr
		}

		//그냥 맨뒤에 push 혹은 del 하기 위한 생성자
		JsonCallObjArr(JNode* node, JNode::JType curType, bool trg_del = false);




		//끝에만 push or del 하기
		//obj는 안쓸예정
		void obj_push(JNode* node){
			Root_Arr = nullptr;
			//1. obj만 처리해주기
			Root_Node = node;
			Root_Obj = static_cast<JObj*>(Root_Node->P_Type);
			JObj* tail_obj = Root_Obj->getTailObj();
			//2. 객체에 아무값도 존재하지 않을때 그냥 새롭게 하나 만들기
			if (Root_Node->ObjCnt == -1){

			}

			//3. 값이 존재해서 맨뒤까지 도달해야할때
		}

		void arr_push(JNode* node){
			Root_Obj = nullptr;
			//1. arr만 처리해주기
			Root_Node = node;
			int max_idx = -1;
			Root_Arr = static_cast<JArr*>(Root_Node->P_Type);
			JArr* tail_arr = Root_Arr->getTailArr(&max_idx);
			//2. 배열에 아무값도 존재하지 않을때 그냥 새롭게 하나 만들기
			if (Root_Node->ArrCnt == -1){
				Root_Arr->setValue(JNode::JType::NULLTYPE);
				Root_Arr->next = nullptr;
				Root_Node->ArrCnt++;

				Cur_Arr = Root_Arr;
			}

			//3. 값이 존재해서 맨뒤까지 도달해야할때
			else if (Root_Node->ArrCnt >= 0){
				JArr* new_arr = new JArr();
				new_arr->setValue(JNode::JType::NULLTYPE);
				new_arr->next = nullptr;
				Root_Node->ArrCnt++;

				tail_arr->next = new_arr;

				Cur_Arr = new_arr;
			}
			//연산자 오버로딩에서 사용할 리소스 초기화 해주기
			Cur_Node = Root_Node;
			Cur_Obj = Root_Obj;		//nullptr
		}

		void obj_del(JNode* node){
			Root_Arr = nullptr;
			//1. obj만 처리해주기
			Root_Node = node;
			Root_Obj = static_cast<JObj*>(Root_Node->P_Type);

			//2. 객체 상태가 "{}"일때
			if (Root_Node->ObjCnt == -1){
				return;
				//삭제못함 그냥 반환
			}

			//3. 객체 상태가 "{..}"일때
			else{
				JObj* tail_obj = Root_Obj->getTailObj();
				JObj* prev_obj = Root_Obj->getPrevObj(tail_obj, Root_Node);

				if (prev_obj == nullptr){
					//1개가 남아있을때
					Root_Node->delType();
					Root_Node->setType(JNode::JType::OBJ);
				}
				else{
					//여러개가 남아있을때
					prev_obj->next = nullptr;
					Root_Node->ObjCnt--;
					delete tail_obj;
				}
			}
		}

		void arr_del(JNode* node){
			Root_Obj = nullptr;
			//1. Arr만 처리해주기
			Root_Node = node;
			Root_Arr = static_cast<JArr*>(Root_Node->P_Type);

			//2. 배열 상태가 "[]"일때
			if (Root_Node->ArrCnt == -1) {
				return;
				//삭제못함 그냥 반환
			}

			//3. 배열 상태가 "[..]"일때
			else{
				JArr* tail_arr = Root_Arr->getTailArr();
				JArr* prev_arr = Root_Arr->getPrevArr(tail_arr, Root_Node);

				if (prev_arr == nullptr){
					//1개남아있을때
					Root_Node->delType();
					Root_Node->setType(JNode::JType::ARR);
				}
				else{
					//여러개 남아있을때
					prev_arr->next = nullptr;
					Root_Node->ArrCnt--;
					delete tail_arr;
				}


			}
		}


		//반환 연산자 = ObjArr["Key"] 값을 반환
		operator int();
		operator int*();
		operator double();
		operator double*();
		operator bool();
		operator bool*();
		operator char();
		operator char*();
		operator unsigned char();
		operator unsigned char*();
		operator JNode*();

	private:
		operator JNode();
		operator JObj();
		operator JObj*();
		operator JArr();
		void operator=(JNode);
		void operator=(JObj);
		void operator=(JObj*);
		void operator=(JArr);
		void operator=(JArr*);
	public:

		//대입 연산자 ObjArr["Key"] = 값을 대입
		void operator=(int);
		void operator=(int*);
		void operator=(double);
		void operator=(double*);
		void operator=(bool);
		void operator=(bool*);
		void operator=(char);
		void operator= (const char*);
		void operator=(char*);
		void operator=(unsigned char);
		void operator= (unsigned const char*);
		void operator=(unsigned char*);
		void operator=(JNode*);
		void operator=(JNode::JType curType);

		//반환용 제어 (True 성공, False 실패)
		bool getOper_Ctrl(JNode::JType curType){
			//예외처리용도
			if (Cur_Obj == nullptr && Cur_Arr == nullptr){
				return false;
			}

			if (Cur_Obj != nullptr){
				switch (curType){
				case JNode::JType::NUMBER:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::NUMBER))
						return false;
					break;
				case JNode::JType::DOUBLE:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::DOUBLE))
						return false;
					break;
				case JNode::JType::BOOL:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::BOOL))
						return false;
					break;
				case JNode::JType::STRING:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::STRING))
						return false;
					break;
				case JNode::JType::OBJ:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::OBJ))
						return false;
					break;
				case JNode::JType::ARR:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::ARR))
						return false;
					break;
				case JNode::JType::NULLTYPE:
					if (Cur_Obj->Value->isTypeNull() && !Cur_Obj->Value->isTypeMatch(JNode::JType::NULLTYPE))
						return false;
					break;

				}
			}

			if (Cur_Arr != nullptr){
				switch (curType){
				case JNode::JType::NUMBER:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::NUMBER))
						return false;
					break;
				case JNode::JType::DOUBLE:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::DOUBLE))
						return false;
					break;
				case JNode::JType::BOOL:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::BOOL))
						return false;
					break;
				case JNode::JType::STRING:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::STRING))
						return false;
					break;
				case JNode::JType::OBJ:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::OBJ))
						return false;
					break;
				case JNode::JType::ARR:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::ARR))
						return false;
					break;
				case JNode::JType::NULLTYPE:
					if (Cur_Arr->Value->isTypeNull() && !Cur_Arr->Value->isTypeMatch(JNode::JType::NULLTYPE))
						return false;
					break;

				}
			}

			//성공
			return true;
		}

		//대입용 제어 (True 성공, False 실패)
		void setOper_Ctrl(JNode::JType curType, void* rValue, bool str = false){
			//예외처리용도
			if (Cur_Obj == nullptr && Cur_Arr == nullptr){
				return;
			}

			if (Cur_Obj != nullptr){
				if (curType == JNode::JType::NUMBER){
					if (!Cur_Obj->Value->isTypeMatch(JNode::JType::NUMBER)){
						Cur_Obj->Value->delType();
						Cur_Obj->Value->setType(JNode::JType::NUMBER);
					}
					int* lval = static_cast<int*>(Cur_Obj->Value->P_Type);
					int* rval = static_cast<int*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::DOUBLE){
					if (!Cur_Obj->Value->isTypeMatch(JNode::JType::DOUBLE)){
						Cur_Obj->Value->delType();
						Cur_Obj->Value->setType(JNode::JType::DOUBLE);
					}
					double* lval = static_cast<double*>(Cur_Obj->Value->P_Type);
					double* rval = static_cast<double*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::BOOL){
					if (!Cur_Obj->Value->isTypeMatch(JNode::JType::BOOL)){
						Cur_Obj->Value->delType();
						Cur_Obj->Value->setType(JNode::JType::BOOL);
					}
					bool* lval = static_cast<bool*>(Cur_Obj->Value->P_Type);
					bool* rval = static_cast<bool*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::STRING){
					if (!Cur_Obj->Value->isTypeMatch(JNode::JType::STRING)){
						Cur_Obj->Value->delType();
						Cur_Obj->Value->setType(JNode::JType::STRING);
					}
					if (str){//문자열
						DynamicStr* lval = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
						unsigned char* rval = static_cast<unsigned char*>(rValue);
						lval->Set_Str(rval);

						//문자열 파싱 해주는거 필요
						Cur_Obj->Value->isObjArrCk(lval);

					}
					else{//문자
						DynamicStr* lval = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
						unsigned char* rval = static_cast<unsigned char*>(rValue);
						lval->Set_Char(&rval[0]);
					}
				}
				else if (curType == JNode::JType::OBJ){

				}
				else if (curType == JNode::JType::ARR){

				}
				else if (curType == JNode::JType::NULLTYPE){

				}
			}

			if (Cur_Arr != nullptr){
				if (curType == JNode::JType::NUMBER){
					if (!Cur_Arr->Value->isTypeMatch(JNode::JType::NUMBER)){
						Cur_Arr->Value->delType();
						Cur_Arr->Value->setType(JNode::JType::NUMBER);
					}
					int* lval = static_cast<int*>(Cur_Arr->Value->P_Type);
					int* rval = static_cast<int*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::DOUBLE){
					if (!Cur_Arr->Value->isTypeMatch(JNode::JType::DOUBLE)){
						Cur_Arr->Value->delType();
						Cur_Arr->Value->setType(JNode::JType::DOUBLE);
					}
					double* lval = static_cast<double*>(Cur_Arr->Value->P_Type);
					double* rval = static_cast<double*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::BOOL){
					if (!Cur_Arr->Value->isTypeMatch(JNode::JType::BOOL)){
						Cur_Arr->Value->delType();
						Cur_Arr->Value->setType(JNode::JType::BOOL);
					}
					bool* lval = static_cast<bool*>(Cur_Arr->Value->P_Type);
					bool* rval = static_cast<bool*>(rValue);

					*lval = *rval;
				}
				else if (curType == JNode::JType::STRING){
					if (!Cur_Arr->Value->isTypeMatch(JNode::JType::STRING)){
						Cur_Arr->Value->delType();
						Cur_Arr->Value->setType(JNode::JType::STRING);
					}
					if (str){//문자열
						DynamicStr* lval = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
						unsigned char* rval = static_cast<unsigned char*>(rValue);
						lval->Set_Str(rval);

						//문자열 파싱 해주는거 필요
						Cur_Arr->Value->isObjArrCk(lval);
					}
					else{//문자
						DynamicStr* lval = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
						unsigned char* rval = static_cast<unsigned char*>(rValue);
						lval->Set_Char(&rval[0]);
					}
				}
				else if (curType == JNode::JType::OBJ){
					//필요없을 확률 99
					//대입시 JNode["Key"] = JArr로 넘기는건 금지할거기 때문 무조건 대입은 JNode만 넘길 수 있음
				}
				else if (curType == JNode::JType::ARR){
					//필요없을 확률 99
					//대입시 JNode["Key"] = JArr로 넘기는건 금지할거기 때문 무조건 대입은 JNode만 넘길 수 있음
				}
				else if (curType == JNode::JType::NULLTYPE){

				}
			}


		}

	private:
		JNode* Root_Node;	//node->P_Type == Obj; 인거임 obj의 Value node 아님
		//node->P_Type == Arr; 인거임 arr의 Value node 아님
		JObj* Root_Obj;		//node->P_Type 이 가리키는 key의 Obj임
		JArr* Root_Arr;		//node->P_Type 이 가리키는 index의 Arr임

		//operator에서 사용될 포인터 변수들
		JNode* Cur_Node;
		JObj* Cur_Obj;
		JArr* Cur_Arr;
	};


	/* ================================================
	* 파싱부
	==================================================*/


	class ValRss{
	public:
		ValRss() : /*FullVal(128),*/ Key(128), Value(128), curType(JNode::JType::NULLTYPE){}
		~ValRss(){}
		//Dynamic::DynamicStr FullVal;		//없어도 될듯 여기서 관리하진 말자
		Dynamic::DynamicStr Key;
		Dynamic::DynamicStr Value;
		JNode::JType curType;
	};

	//동적 배열
	class ValArr{
	public:
		ValArr(){
			//Val = new ValRss[128];
			Vals = new ValRss*[128];
			InitVals(Vals, 128);
			curCsr = -1;
		}
		ValArr(int _capacity) {
			capacity = _capacity;
			//Val = new ValRss[capacity];
			Vals = new ValRss*[capacity];
			InitVals(Vals, capacity);
			curCsr = -1;
		}
		~ValArr(){
			/*if (Val != nullptr){
			delete[] Val;
			}*/
			DelVals(Vals, capacity);
		}

		void DelVals(ValRss** _Vals, int _capacity){
			if (_Vals != nullptr){
				for (int i = 0; i < _capacity; i++){
					if (_Vals[i] != nullptr){
						delete _Vals[i];
						_Vals[i] = nullptr;
					}
				}
				delete[] _Vals;
			}
		}

		void InitVals(ValRss** _Vals, int _capacity){
			for (int i = 0; i < _capacity; i++){
				_Vals[i] = nullptr;
			}
		}

	public:
		int getIdxCnt(){
			return curCsr;
		}
		//사이즈 늘리기
		void UpBuf(int _capacity){
			if (capacity >= _capacity){
				//기존 용량보다 작아서 불가능
				return;
			}

			ValRss** newVals = new ValRss*[_capacity];
			InitVals(newVals, _capacity);
			for (int i = 0; i < capacity; i++){
				newVals[i] = Vals[i];
				Vals[i] = nullptr;
			}
			DelVals(Vals, capacity);
			Vals = newVals;
			newVals = nullptr;
			capacity = _capacity;


			//ValRss* newVal = new ValRss[_capacity];
			//for (int i = 0; i < capacity; i++){
			//   //newVal[i].FullVal.Set_Str(Val[i].FullVal.Get_Str());
			//   newVal[i].Key.Set_Str(Val[i].Key.Get_Str());
			//   newVal[i].Value.Set_Str(Val[i].Value.Get_Str());
			//}
			//delete[] Val;
			//Val = nullptr;
			//Val = newVal;
			//capacity = _capacity;
		}

		//메모리 얕은복사로 불필요한 배열 크기 줄이기
		void setCmpValArr(){
			int newCsr = curCsr + 1;
			if (curCsr == capacity){
				return;
				//굳이 안해줘도 될듯
			}
			ValRss** newVals = new ValRss*[newCsr];
			InitVals(newVals, newCsr);

			for (int i = 0; i <= curCsr; i++){
				newVals[i] = Vals[i];
				Vals[i] = nullptr;
			}
			DelVals(Vals, capacity);
			Vals = newVals;
			newVals = nullptr;
			capacity = newCsr;
		}

		//맨뒤에 삽입
		void setObjRss(Dynamic::DynamicStr* Key, Dynamic::DynamicStr* Value){
			if (curCsr >= capacity - 1){
				//마지막 커서 UpBuf해줘야함
				UpBuf(capacity * 2);
			}
			curCsr++;

			Key->Str_Trim_All();
			Value->Str_Trim_All();

			Vals[curCsr] = new ValRss();

			Vals[curCsr]->Key.Set_Str(Key->Get_Str());
			Vals[curCsr]->Value.Set_Str(Value->Get_Str());
			Vals[curCsr]->curType = JNode::JType::OBJ;

			//Val[curCsr].Key.Set_Str(Key->Get_Str());
			//Val[curCsr].Value.Set_Str(Value->Get_Str());
			////Val[curCsr].FullVal.Set_Str(FullVal->Get_Str());
			//Val[curCsr].curType = JNode::JType::OBJ;

			//혹시 모르니까 정리해주는 메소드 사용
			//setTrim();
		}

		//배열전용 값 넣기 배열일때는 키값이 필요없으니까 넣은거임
		void setArrRss(Dynamic::DynamicStr* Value){
			if (curCsr >= capacity - 1){
				//마지막 커서 UpBuf해줘야함
				UpBuf(capacity * 2);
			}
			curCsr++;

			Value->Str_Trim_All();

			Vals[curCsr] = new ValRss();
			Vals[curCsr]->Value.Set_Str(Value->Get_Str());
			Vals[curCsr]->curType = JNode::JType::ARR;

			//Val[curCsr].Value.Set_Str(Value->Get_Str());
			////Val[curCsr].FullVal.Set_Str(FullVal->Get_Str());
			//Val[curCsr].curType = JNode::JType::ARR;
		}

		//큰따옴표 제거
		void RemoveQuotes(DynamicStr& Str){
			Str.Str_Trim_Front();
			//Str.Str_Trim_Front();
			Str.Str_Trim_Back();
			//Str.Str_Trim_Back();
			//이거 두번 하는건 생각해봐야할듯?  \"
		}


	protected:
		int capacity;      //용량   동적으로 늘릴예정
		int curCsr;         //index의 크기?
		//ValRss* Val;
		ValRss** Vals;
		//FullVal 굳이 만들어야 할까?
	};


	//문자열 파싱 -> JNode로 변환
	class StrParser : public ValArr {
	public:
		StrParser(JNode* RootNode, Dynamic::DynamicStr* RootStr, JNode::JType RootNodeType) : ValArr(2),
			RootNode(RootNode), RootStr(RootStr), RootNodeType(RootNodeType), gCsr(0),
			Key(nullptr), Value(nullptr), FullVal(nullptr){
			ParserMain();
		}

		~StrParser(){
			int debug = 10;
		}

		enum class WrdInfo{
			Init,
			KEY,
			STR,
			VALUE,
			IDLE,      //아직 어떠한 확정도 없는 상태
		};

		void ParserMain(){
			Parser();
			//Parser_MltByte();
		}

		void Value_Ascii_Parser(ValRss* Vals, JNode::JType JsStt, JNode* CurNode){
			int result_int = 0;
			double result_double = 0.0;
			bool is_double = false;      //false -> int, true -> double
			bool result_bool = false;

			int stack_glb_csr = 0;



			enum ck_word{
				DEF,
				STR,
				M_NUM,
				P_NUM,
				OBJ,   //Str로 넣어버리기
				ARR,   //Str로 넣어버리기
				BL,
				NULLVAL,		//Value가 null일때
				NULLTYPE,		//빈객체[] 혹은 빈배열[] 일때
				END
			};

			ck_word crnt_value = DEF;	//Value의 타입임

			JObj* obj = nullptr;   //이거는 단순히 디버깅 용도임
			JArr* arr = nullptr;   //이거는 단순히 디버깅 용도임

			//디버깅 용도임 실제는 CurNode 사용할거임
			if (JsStt == JNode::JType::OBJ){
				obj = static_cast<JObj*>(CurNode->P_Type);
				if (Vals->Key.str_last_focus == -1 && Vals->Value.str_last_focus == -1){
					//빈객체임
					crnt_value = NULLTYPE;
				}
			}
			else if (JsStt == JNode::JType::ARR){
				arr = static_cast<JArr*>(CurNode->P_Type);
				if (Vals->Value.str_last_focus == -1){
					//빈 배열임
					crnt_value = NULLTYPE;
				}
			}

			for (;; stack_glb_csr++){//Key는 차피 문자열이라 안해도 됨 Value만 하면 됨				
				PrvWrd = Vals->Value.Char_Get_Str(stack_glb_csr - 1);
				CurWrd = Vals->Value.Char_Get_Str(stack_glb_csr);
				NxtWrd = Vals->Value.Char_Get_Str(stack_glb_csr + 1);



				//시작판별
				if (crnt_value == DEF){
					if (CurWrd == '\"'){
						crnt_value = STR;
						stack_glb_csr--;
					}
					//음의 숫자일 경우(음의 정수, 음의 실수)
					else if (CurWrd == '-' && (NxtWrd >= '0' && NxtWrd <= '9')) {
						crnt_value = M_NUM;
					}
					//양의 숫자일 경우(양의 정수, 양의 실수)
					else if (CurWrd >= '0' && CurWrd <= '9') {
						crnt_value = P_NUM;
						stack_glb_csr--;
					}
					//bool일 경우
					else if (CurWrd == 't' || CurWrd == 'f') {
						crnt_value = BL;
						stack_glb_csr--;
					}
					else if (CurWrd == 'n' && NxtWrd == 'u'
						&&Vals->Value.Char_Get_Str(stack_glb_csr + 2) == 'l' && Vals->Value.Char_Get_Str(stack_glb_csr + 3 == 'l')){
						crnt_value = NULLVAL;
						stack_glb_csr--;
					}
					else if (CurWrd == '{') {
						if (Vals->Value.Char_Get_Str(Vals->Value.str_last_focus) == '}'){
							crnt_value = OBJ;
							stack_glb_csr--;
						}
					}
					else if (CurWrd == '[') {
						if (Vals->Value.Char_Get_Str(Vals->Value.str_last_focus) == ']'){
							crnt_value = ARR;
							stack_glb_csr--;
						}
					}
					continue;
				}
				//종료판별
				else if (crnt_value == END){
					break;
				}
				else{
					if (crnt_value == STR) {
						//RemoveQuotes(Vals->Value);
						unsigned char* chKeys = Vals->Key.Get_uStr();
						unsigned char* chVals = Vals->Value.Get_uStr();

						if (obj != nullptr){
							(*CurNode)[static_cast<const unsigned char*>(chKeys)] = chVals;
							//DynamicStr* deb = static_cast<DynamicStr*>(CurNode->P_Type);
						}
						else if (arr != nullptr){
							int arrCnt = CurNode->ArrCnt + 1;
							(*CurNode)[arrCnt] = chVals;
						}


						//디버깅 확인용
						//JNode* debug = static_cast<JNode*>(obj->Value);
						//DynamicStr* db = static_cast<DynamicStr*>(debug->P_Type);
						//디버깅 확인용

						crnt_value = END;
					}
					else if (crnt_value == M_NUM || crnt_value == P_NUM) {
						result_int = result_int * 10 + (CurWrd - '0');

						//소수판별
						if (NxtWrd == '.') {
							is_double = true;
							stack_glb_csr += 2;
							double decimal_part = 0;
							int divisor = 10;		//소수점 뒤 숫자를 10으로 나누는데 사용할 변수
							for (;; stack_glb_csr++) {
								PrvWrd = Vals->Value.Char_Get_Str(stack_glb_csr - 1);
								CurWrd = Vals->Value.Char_Get_Str(stack_glb_csr);
								NxtWrd = Vals->Value.Char_Get_Str(stack_glb_csr + 1);
								decimal_part = decimal_part + (CurWrd - '0') / (double)divisor;
								divisor *= 10;
								if (NxtWrd == '\0') break;
							}
							result_double = result_int + decimal_part;
						}

						//종료 판별
						if (NxtWrd == '\0') {
							if (crnt_value == M_NUM) {
								result_int = result_int * -1;
								result_double = result_double * -1;
							}
							//값 넣어주기
							if (obj != nullptr) {
								char* chKeys = Vals->Key.Get_Str();
								if (is_double){
									(*CurNode)[static_cast<const char*>(chKeys)] = result_double;
								}
								else{
									(*CurNode)[static_cast<const char*>(chKeys)] = result_int;
								}

							}
							else if (arr != nullptr) {
								int arrCnt = CurNode->ArrCnt + 1;
								if (is_double)
									(*CurNode)[arrCnt] = result_double;
								else
									(*CurNode)[arrCnt] = result_int;
							}
							else {
								return;
							}

							//디버깅용
							//JNode* debug = static_cast<JNode*>(obj->next->Value);
							//double* db = static_cast<double*>(debug->P_Type);
							//디버깅용

							crnt_value = END;
						}
					}
					else if (crnt_value == BL) {
						char first_word = Vals->Value.Char_Get_Str(0);
						char second_word = Vals->Value.Char_Get_Str(1);
						char third_word = Vals->Value.Char_Get_Str(2);
						char four_word = Vals->Value.Char_Get_Str(3);
						char five_word = Vals->Value.Char_Get_Str(4);	//true면 '\0'
						char six_word = Vals->Value.Char_Get_Str(5);	//false면 '\0'
						if (first_word == 't') {
							if (second_word == 'r' &&
								third_word == 'u' &&
								four_word == 'e') {
								result_bool = true;
							}
						}
						else if (first_word == 'f') {
							if (second_word == 'a' &&
								third_word == 'l' &&
								four_word == 's' &&
								five_word == 'e') {
								result_bool = false;
							}
						}

						//종료판별
						if (five_word == '\0' || six_word == '\0') {
							//값 넣어주기
							if (obj != nullptr) {
								char* chKeys = Vals->Key.Get_Str();
								(*CurNode)[static_cast<const char*>(chKeys)] = result_bool;
							}
							else if (arr != nullptr) {
								int arrCnt = CurNode->ArrCnt + 1;
								(*CurNode)[arrCnt] = result_bool;
							}
							else {
								return;
							}
							//디버깅용
							//JNode* debug = static_cast<JNode*>(obj->next->Value);
							//bool* db = static_cast<bool*>(debug->P_Type);
							//디버깅용
							crnt_value = END;
						}
					}
					else if (crnt_value == OBJ) {
						char* chVals = Vals->Value.Get_Str();
						JNode* newObjNode = new JNode(JNode::JType::STRING);
						*newObjNode = chVals;
						if (obj != nullptr){
							char* chKeys = Vals->Key.Get_Str();
							(*CurNode)[static_cast<const char*>(chKeys)] = newObjNode;
						}
						else if (arr != nullptr){
							int arrCnt = CurNode->ArrCnt + 1;
							(*CurNode)[arrCnt] = newObjNode;
						}
						//종료판별
						crnt_value = END;
					}
					else if (crnt_value == ARR) {
						char* chVals = Vals->Value.Get_Str();
						JNode* newArrNode = new JNode(JNode::JType::STRING);
						*newArrNode = chVals;
						if (obj != nullptr){
							char* chKeys = Vals->Key.Get_Str();
							(*CurNode)[static_cast<const char*>(chKeys)] = newArrNode;
						}
						else if (arr != nullptr){
							int arrCnt = CurNode->ArrCnt + 1;
							(*CurNode)[arrCnt] = newArrNode;
						}
						//종료판별
						crnt_value = END;
					}
					else if (crnt_value == NULLVAL){
						//Value가 null일때
						char* chKeys = Vals->Key.Get_Str();
						if (obj != nullptr){
							(*CurNode)[static_cast<const char*>(chKeys)] = JNode::JType::NULLTYPE;
						}
						else if (arr != nullptr){
							int arrCnt = CurNode->ArrCnt + 1;
							(*CurNode)[arrCnt] = JNode::JType::NULLTYPE;
						}
						crnt_value = END;
					}
					else if (crnt_value == NULLTYPE){

						//빈 객체
						//char* chKeys = Vals->Key.Get_Str();
						if (obj != nullptr){
							//(*CurNode)[static_cast<const char*>(chKeys)] = JNode::JType::NULLTYPE;
						}
						//빈배열
						if (arr != nullptr){
							//int arrCnt = CurNode->ArrCnt + 1;
							//(*CurNode)[arrCnt] = JNode::JType::NULLTYPE;
						}
						//빈객체 빈배열
						crnt_value = END;
					}
				}
			}


		}

		// FullVal - (Key:Value), FullVal - (Value)
		void ValueParser(JNode::JType curType){
			//배열 정리 한번만 하기
			setCmpValArr();

			RootNode->delType();

			//배열의 개수를 파악하기
			int ValIdxCnt = getIdxCnt();

			//객체 타입일때
			if (curType == JNode::JType::OBJ)
				*RootNode = JNode::JType::OBJ;
			//배열타입일때
			else if (curType == JNode::JType::ARR)
				*RootNode = JNode::JType::ARR;




			if (ValIdxCnt > -1){
				for (int i = 0; i <= ValIdxCnt; i++){
					Value_Ascii_Parser(Vals[i], curType, RootNode);
				}

			}

		}



		//1Byte 구조
		void Parser() {
			short BrcCnt = 0;      //'{'의 개수 추적
			short BrkCnt = 0;      //'['의 개수 추적
			bool qtStt = false;      //'"'상태 추적

			FullVal = new DynamicStr(128);
			Key = new DynamicStr(128);
			Value = new DynamicStr(128);

			JNode::JType curType = RootNodeType;
			WrdInfo curWrdType = WrdInfo::IDLE;
			bool isValStr = true;

			while (1) {
				PrvWrd = RootStr->Char_Get_Str(gCsr - 1);
				CurWrd = RootStr->Char_Get_Str(gCsr);
				NxtWrd = RootStr->Char_Get_Str(gCsr + 1);


				if (CurWrd == '{') {
					BrcCnt++;
				}
				else if (CurWrd == '}') {
					BrcCnt--;
				}
				if (CurWrd == '[') {
					BrkCnt++;
				}
				else if (CurWrd == ']') {
					BrkCnt--;
				}

				if (CurWrd == '"' && PrvWrd != '\\') {
					if (qtStt == true) qtStt = false;
					else if (qtStt == false) qtStt = true;
				}


				if (curType == JNode::JType::OBJ) {//Key와 Value만 생각하기
					if (CurWrd == ',' && (BrcCnt == 1 && BrkCnt == 0) && (curWrdType != WrdInfo::KEY) && isValStr) {//이때만 FullVal가 추가됨
						//FullVal->Append_Char(&CurWrd);

						setObjRss(Key, Value);

						//delete FullVal;
						delete Key;
						delete Value;

						//FullVal = new DynamicStr(128);
						Key = new DynamicStr(128);
						Value = new DynamicStr(128);

						curWrdType = WrdInfo::IDLE;
					}


					if (CurWrd == '}' && (BrcCnt == 0 && BrkCnt == 0)) {//종료 판별
						setObjRss(Key, Value);
						FullVal->Append_Char(&CurWrd);

						ValueParser(curType);

						//여기서 계층 구조를 파싱하기 위한 메소드가 필요할거 같음 
						break;
					}


					//FullVal 처리
					FullVal->Append_Char(&CurWrd);

					//Key 찾기
					if (curWrdType == WrdInfo::IDLE && CurWrd == '"' && qtStt == true) {
						curWrdType = WrdInfo::KEY;
					}
					else if (curWrdType == WrdInfo::KEY) {
						if (CurWrd == '"' && qtStt == false) {   //Key끝
							curWrdType = WrdInfo::IDLE;
						}
						else
							Key->Append_Char(&CurWrd);
					}

					//Value 찾기
					else if (curWrdType == WrdInfo::IDLE && CurWrd == ':') {
						curWrdType = WrdInfo::VALUE;
					}
					else if (curWrdType == WrdInfo::VALUE) {
						if (CurWrd == '\"'){
							isValStr = !isValStr;      //true 가 되면 문제가 없는거임
						}
						Value->Append_Char(&CurWrd);
					}

				}
				else if (curType == JNode::JType::ARR) {//Value만 생각하기
					if (CurWrd == ',' && (BrkCnt == 1 && BrcCnt == 0) && isValStr) { //이때만 FullVal가 추가됨
						//setArrRss();

						setArrRss(Value);

						//delete FullVal;
						//delete Key;
						delete Value;

						//FullVal = new DynamicStr(128);
						//Key = new DynamicStr(128);
						Value = new DynamicStr(128);
						curWrdType = WrdInfo::IDLE;
					}

					if (CurWrd == ']' && (BrkCnt == 0 && BrcCnt == 0)) { //종료판별
						setArrRss(Value);
						FullVal->Append_Char(&CurWrd);

						ValueParser(curType);
						break;
					}


					//FullVal 처리
					FullVal->Append_Char(&CurWrd);

					//Value 처리
					if (curWrdType == WrdInfo::IDLE){
						curWrdType = WrdInfo::VALUE;
					}
					else if (curWrdType == WrdInfo::VALUE){
						if (CurWrd == '\"'){
							isValStr = !isValStr;      //true 가 되면 문제가 없는거임
						}
						Value->Append_Char(&CurWrd);
					}

				}
				gCsr++;
			}
			delete FullVal;
			delete Key;
			delete Value;
		}

		//Multy Byte 구조(최대 4바이트) UTF-8 파일만 넣어야 함(다른 인코딩 값 넣으면 바이트 잘못읽어서 문제생김)
		void Parser_MltByte(){
			short BrcCnt = 0;      //'{'의 개수 추적
			short BrkCnt = 0;      //'['의 개수 추적
			bool qtStt = false;      //'"'상태 추적

			FullVal = new DynamicStr(128);
			Key = new DynamicStr(128);
			Value = new DynamicStr(128);

			JNode::JType curType = RootNodeType;
			WrdInfo curWrdType = WrdInfo::IDLE;
			bool isvalStr = true;
			unsigned char MltBytCk = ' ';
			M_PrvWrd[0] = '\0';
			for (/*gCsr = -1*/;; gCsr++,
				DynamicStr::Raw_StrCpy(M_PrvWrd, sizeof(M_PrvWrd), M_CurWrd, sizeof(M_CurWrd), 5)){
				MltBytCk = RootStr->Char_Get_Str(gCsr);
				//M_CurWrd[MltCsr] = RootStr->Char_Get_Str(gCsr);

				//멀티바이트인지 판별
				int numBytes = 1;
				if ((MltBytCk & 0x80) == 0x00) numBytes = 1;
				else if ((MltBytCk & 0xE0) == 0xC0) numBytes = 2;
				else if ((MltBytCk & 0xF0) == 0xE0) numBytes = 3;
				else if ((MltBytCk & 0xF8) == 0xF0) numBytes = 4;
				else {
					printf("MultyByte를 초과한 문자범위가 발견되었습니다.\n");
					break;
				}
				for (int k = 0; k < numBytes; k++) {
					M_CurWrd[k] = RootStr->Char_Get_Str(gCsr + k);
				}
				gCsr += numBytes - 1; // for루프의 증가 보정

				/*printf("과거문자 1개 :  %c%c%c%c%c\n", M_PrvWrd[0], M_PrvWrd[1], M_PrvWrd[2], M_PrvWrd[3], M_PrvWrd[4]);
				printf("현재문자 1개 :  %c%c%c%c%c\n", M_CurWrd[0], M_CurWrd[1], M_CurWrd[2], M_CurWrd[3], M_CurWrd[4]);*/

				if (numBytes == 1){	//Ascii임
					if (M_CurWrd[numBytes - 1] == '{') {
						BrcCnt++;
					}
					else if (M_CurWrd[numBytes - 1] == '}') {
						BrcCnt--;
					}
					if (M_CurWrd[numBytes - 1] == '[') {
						BrkCnt++;
					}
					else if (M_CurWrd[numBytes - 1] == ']') {
						BrkCnt--;
					}
					//"의 Stack 정보
					if (M_CurWrd[numBytes - 1] == '"'  && M_PrvWrd[numBytes - 1] != '\\') {
						if (qtStt == true) qtStt = false;
						else if (qtStt == false) qtStt = true;
					}
				}




			}

			int debug = 10;

		}



	private:
		JNode* RootNode;      //자체적으로 절대로 변질되어선 안됨 내부 구조를 바꾸는건 가능하지만 이미 자체 참조가 된상태임
		DynamicStr* RootStr;      //변질되도 의미는 없음 RootNode가 제일 중요한거라

		DynamicStr* FullVal;      //밸류뽑기 위한 문자들   디버깅용이라고 생각하고 추후 지울것
		DynamicStr* Key;
		DynamicStr* Value;

		JNode::JType RootNodeType;
		int gCsr;         //문자 컨트롤을 위한 값들
		unsigned char CurWrd;      //
		unsigned char NxtWrd;      //
		unsigned char PrvWrd;      //
		unsigned char M_CurWrd[5];  //멀티바이트 전용 문자
		//unsigned char M_NxtWrd[5];  //멀티바이트 전용 문자
		unsigned char M_PrvWrd[5];  //멀티바이트 전용 문자
		//int MltCsr;				   //멀티바이트 전용 커서
	};
}

using namespace JSON;

class Json_Func{
private:
	union MyUnion {
		int num;
		bool bl;
		double dnum;
	};
	static MyUnion Val;
public:

	static void PrtNode(JNode& node){
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
			printf("\"%s\"", Str->Get_Str());
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

	static void PrtNode_Trim(JNode& node){
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
			Str->Str_Trim_All();
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
				obj->Key.Str_Trim_All();
				printf("\"%s\" : ", obj->Key.Get_Str());
				PrtNode_Trim(*obj->Value);

				if (i != node.ObjCnt) printf(", ");
			}

			printf("}");
			break;
		case JNode::JType::ARR:
			printf("[");
			JArr* arr;
			arr = static_cast<JArr*>(node.P_Type);
			for (int i = 0; i <= node.ArrCnt; i++, arr = arr->next){
				PrtNode_Trim(*arr->Value);

				if (i != node.ArrCnt) printf(",");
			}
			printf("]");
			break;
		default:
			break;
		}
	}



	static void FileNode(JNode& node, FILE* oFile) {
		DynamicStr* Str;
		switch (node.Cur_Type) {
		case JNode::JType::NULLTYPE:
			fprintf(oFile, "null");
			return;
		case JNode::JType::NUMBER:
			Val.num = node;
			fprintf(oFile, "%d", Val.num);
			break;
		case JNode::JType::STRING:
			Str = static_cast<DynamicStr*>(node.P_Type);
			fprintf(oFile, "\"%s\"", Str->Get_Str());
			break;
		case JNode::JType::BOOL:
			Val.bl = node;
			fprintf(oFile, "%s", Val.bl ? "true" : "false");
			break;
		case JNode::JType::DOUBLE:
			Val.dnum = node;
			fprintf(oFile, "%.3f", Val.dnum);
			break;
		case JNode::JType::OBJ:
		{
			fprintf(oFile, "{");
			JObj* obj = static_cast<JObj*>(node.P_Type);

			for (int i = 0; i <= node.ObjCnt; i++, obj = obj->next) {
				fprintf(oFile, "\"%s\" : ", obj->Key.Get_Str());
				FileNode(*obj->Value, oFile);

				if (i != node.ObjCnt) fprintf(oFile, ", ");
			}

			fprintf(oFile, "}");
			break;
		}
		case JNode::JType::ARR:
		{
			fprintf(oFile, "[");
			JArr* arr = static_cast<JArr*>(node.P_Type);
			for (int i = 0; i <= node.ArrCnt; i++, arr = arr->next) {
				FileNode(*arr->Value, oFile);

				if (i != node.ArrCnt) fprintf(oFile, ",");
			}
			fprintf(oFile, "]");
			break;
		}
		default:
			break;
		}
	}


	static void PrtNode_OnlySec(JNode& node, int& SectCnt){
		DynamicStr* Str;
		switch (node.Cur_Type){
		case JNode::JType::NULLTYPE:   //종료
			//printf("null");
			return;
		case JNode::JType::NUMBER:
			Val.num = node;
			//printf("%d", Val.num);
			break;
		case JNode::JType::STRING:
			Str = static_cast<DynamicStr*>(node.P_Type);
			//printf("\"%s\"", Str->Get_Str());
			break;
		case JNode::JType::BOOL:
			Val.bl = node;
			//printf("%s", Val.bl == true ? "true" : "false");
			break;
		case JNode::JType::DOUBLE:
			Val.dnum = node;
			//printf("%.3f", Val.dnum);
			break;
		case JNode::JType::OBJ:
			//printf("{");
			JObj* obj;
			obj = static_cast<JObj*>(node.P_Type);

			for (int i = 0; i <= node.ObjCnt; i++, obj = obj->next){
				//printf("\"%s\" : ", obj->Key.Get_Str());
				printf("%s\n", obj->Key.Get_Str());
				SectCnt++;
				//PrtNode(*obj->Value);

				//if (i != node.ObjCnt) printf(", ");
			}

			//printf("}");
			break;
		case JNode::JType::ARR:
			//printf("[");
			JArr* arr;
			arr = static_cast<JArr*>(node.P_Type);
			for (int i = 0; i <= node.ArrCnt; i++, arr = arr->next){
				//PrtNode(*arr->Value);

				//if (i != node.ArrCnt) printf(",");
			}
			//printf("]");
			break;
		default:
			break;
		}
	}


	static void ErrFilePrt(FILE* WrtEF, JNode* node){
		int *num = nullptr;
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

		case JNode::JType::BOOL:{
			bool* d = static_cast<bool*>(node->P_Type);
			if (*d == true){
				fprintf(WrtEF, "true");
			}
			else{
				fprintf(WrtEF, "false");
			}
			break;
		}

		case JNode::JType::DOUBLE:{
			double *d = static_cast<double*>(node->P_Type);
			fprintf(WrtEF, "%.3f", *d);
			break;
		}
		case JNode::JType::OBJ:
			//안씀
			break;
		case JNode::JType::ARR:
			JArr* arr = static_cast<JArr*>(node->P_Type);
			fprintf(WrtEF, "\n");
			for (int i = 0; i <= node->ArrCnt; i++, arr = arr->next){
				ErrFilePrt(WrtEF, arr->Value);
				if (i != node->ArrCnt) fprintf(WrtEF, "\n");
			}
			break;
		}

	}
};
