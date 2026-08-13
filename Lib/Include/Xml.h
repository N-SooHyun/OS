#pragma once

#include <stdio.h>
#include <iostream>
#include "Str.h"
#include "LinkedList.h"

using namespace Dynamic;

/* =================================================================================
* 전방 선언
===================================================================================*/
class XmlObj;
class XmlObjOper;
class XmlAttrOper;
class XmlVal;
class XmlStr;

void XmlTestFunc();
void XmlPrint(XmlObj* Root);


/* =================================================================================
* 값(Value) 계층
* 이름을 감싸는 보조 클래스 XmlStr -> XmlVal(추상) -> XmlValue(실제 값 노드)
*
* 주의: XmlValue가 XmlStr을 포인터가 아니라 값으로 직접 소유(XmlStr Val;)하기 때문에
* XmlStr은 반드시 XmlValue보다 앞에서 완전히 정의되어 있어야 한다.
* (상속 때와 똑같은 규칙 - 값으로 소유하는 멤버도 완전한 타입이 필요하다)
===================================================================================*/

class XmlStr{
	friend class XmlObj;
public:
	DynamicStr Name;	// 크기는 상황 봐가며 조정 예정
	XmlStr(char* newName, int StrSize = 32) : Name(StrSize){
		Name.Append_Str(newName);
	}
	XmlStr(DynamicStr* newName, int StrSize = 32) : Name(StrSize){
		Name.Set_Str(newName->Get_Str());
	}

	char* getName(){
		return Name.Get_Str();
	}

	void SetName(char* setName){
		Name.Str_Clear();
		Name.Append_Str(setName);
	}
	void SetName(DynamicStr* setName){
		Name.Str_Clear();
		Name.Set_Str(setName->Get_Str());
	}
};

class XmlVal{
	friend class XmlObj;
	friend class XmlStr;
	virtual char* c_toString() = 0;
	virtual DynamicStr* d_toString() = 0;
protected:
	XmlVal() = default;
public:
	virtual ~XmlVal() = default;
};

// <Obj>값</Obj> 형태에서 값(텍스트) 부분을 의미
class XmlValue : public XmlVal{
public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;
	XmlStr Val;
	XmlValue(char*);
	XmlValue(DynamicStr*);
	~XmlValue() = default;
};

class XmlNull : public XmlVal{
public:
	virtual char* c_toString() override { return nullptr; }
	virtual DynamicStr* d_toString() override { return nullptr; }
};

enum class XmlContentType : unsigned char{
	NUL = 0,
	VAL,
	OBJ,
};

//나중에 범용 Xml을 하기를 원한다면 사용하도록 남겨두기(현재는 사용 안함) TBD
//PI <?xml-stylesheet 
class XmlPI : public XmlVal{
public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;
	XmlStr Target;
	XmlStr Data;
	XmlPI(char* target, char* data);
};

// <!-- --> 주석
class XmlComment : public XmlVal{
public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;
	XmlStr Val;
	XmlComment(char*);
	XmlComment(DynamicStr*);
};

// <!DOCTYPE 
class XmlDocType : public XmlVal{

};



/* =================================================================================
* 대입 연산자 믹스인 계층
*
* TargetProvider가 최상위 인터페이스(GetTarget 순수가상)이고,
* AssignObjOper / AssignAttrOper가 이를 virtual 상속한다.
* 실제 조작 대상이 자기 자신(XmlObj)인지, 다른 곳을 가리키는 대리자(XmlObjOper)인지는
* 이 두 믹스인은 몰라도 되고, 각 자식이 GetTarget()으로만 알려주면 된다.
===================================================================================*/

class XmlParser{
	typedef struct{
		char PrvWord = 0;
		char CurWord = 0;
		char NxtWord = 0;
		XmlObj* CurXmlObj = nullptr;
		int Stack = 0;
		DynamicStr* ObjName = nullptr;
		DynamicStr* Value = nullptr;
		DynamicStr* AttrName = nullptr;
		DynamicStr* AttrValue = nullptr;
	}_PrsTol;

	void ObjMake(char* rVal, int& Csr, _PrsTol& PrsTol);
	void ValMake(char* rVal, int& Csr, _PrsTol& PrsTol, bool isCdata = false);

	void CurXmlObjParse(char* rVal, int& Csr, _PrsTol& PrsTol);

	XmlObj* RealParser(char*);
public:
	XmlParser() = default;
	static XmlObj* Parse(char*);
	static XmlObj* Parse(DynamicStr*);
};

class TargetProvider{
protected:
	virtual XmlObj* GetTarget() = 0;
public:
	virtual ~TargetProvider(){}
};

class AssignObjOper : public virtual TargetProvider{
	bool ValSet(char*);
	bool ValSet(DynamicStr*);
public:
	/*
	좌변은 부모든 자식이든 상관없이 그냥 하나의 XmlObj다.
	<<는 항상 "자식으로 추가"라는 뜻으로 통일한다.
	<Root></Root> -> lVal
	<Child></Child> -> rVal
	lVal << rVal;
	-> lVal 안에 <Child></Child>가 자식으로 추가됨
	*/
	void operator<<(char*);				// Value Set or Obj Add
	void operator<<(DynamicStr*);			// Value Set or Obj Add
	void operator<<(XmlObj*);				// Obj Add (Deep Copy)
	void operator<<(XmlObj&);				// Obj Add (Deep Copy)

	void operator=(char*);					// Only Value Set
	void operator=(DynamicStr*);			// Only Value Set
	void operator=(XmlObj*);				// Obj Set (Deep Copy)
	void operator=(XmlObj&);				// Obj Set (Deep Copy)

	void DepCpy(XmlObj*, XmlObj*);
};

class AssignAttrOper : public virtual TargetProvider{
protected:
	enum class TYPE{
		OBJ,
		ATTR,
	};
public:
	template<typename T>
	T SearchObjAttr(int idx, TYPE);
	template<typename T>
	T SearchObjAttr(char*, TYPE);

	XmlAttrOper operator[](int);		// AttrObj 검색
	XmlAttrOper operator[](char*);
};


/* =================================================================================
* 속성 데이터 클래스 - 이름+값만 갖는 독립적인 저장소
===================================================================================*/

class XmlAttrObj{
	friend class XmlObj;
	DynamicStr Name = DynamicStr(128);		// 속성의 이름
	DynamicStr Value = DynamicStr(128);	// 속성의 값
public:
	XmlAttrObj();
	XmlAttrObj(DynamicStr*, DynamicStr*);
	XmlAttrObj(char*, char*);
	~XmlAttrObj();

	char* getName();
	char* getValue();
	void setValue(char*);
	void setValue(DynamicStr*);
};


/* =================================================================================
* XmlObj - 메인 클래스
* 노드 하나가 값(Value)이거나, 자식들을 담는 컨테이너이거나 둘 중 하나다.
===================================================================================*/

class XmlObj : public XmlVal, public AssignObjOper, public AssignAttrOper{
	friend class XmlObjOper;
	friend class XmlStr;
	friend class XmlAttrObj;
	friend class AssignObjOper;

	XmlObj* Parent;
	XmlStr Name;
	LinkedList<XmlAttrObj> Attrs;
	LinkedList<XmlVal> Vals;		// 자식 객체이거나 값이거나

	void InitValSet();
	void setaddObjValCtrl(char*, bool isNull = false);

protected:
	XmlObj* GetTarget();

public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;

	XmlObj(DynamicStr* Name, bool isNotNull = true);
	XmlObj(char* Name, bool isNotNull = true);
	~XmlObj() = default;
	XmlObj* getParentXml();
	void DelAttrs();
	void DelVals();
	void DelObjVal(int = -1);
	void DelAttrVal(int = -1);

	char* getName();
	void setName(char*);
	void setName(DynamicStr*);
	int getObjIdx();	// 자식이 있으면 마지막 인덱스, 값(Value) 노드면 -1
	int getAttrsIdx();	// 속성 없으면 -1, 있으면 0부터

	XmlContentType getContentType();
	bool isObj();
	bool isVal();
	bool isNul();

	void setVal(DynamicStr*);
	void setVal(char*);
	void setObj(DynamicStr*, bool isNotNull = true);
	void setObj(char*, bool isNotNull = true);
	void addObj(DynamicStr*, bool isNotNull = true);
	void addObj(char*, bool isNotNull = true);

	void setAttr(DynamicStr*, DynamicStr*);
	void setAttr(char*, char*);
	void addAttr(DynamicStr*, DynamicStr*);
	void addAttr(char*, char*);

	XmlValue* getVal();
	XmlObj* getObj(int idx = -1);		// idx 생략 시 마지막 자식, 범위초과면 nullptr
	XmlObj* getObj(char*);
	XmlAttrObj* getAttr(int idx = -1);	// idx 생략 시 마지막 속성, 범위초과면 nullptr
	XmlAttrObj* getAttr(char*);


	// 자식 인덱싱 연산자 - 반환된 XmlObjOper를 통해 대입/체이닝/속성접근이 이어짐
	XmlObjOper operator()(int = -1);
	XmlObjOper operator()(char*);

	// AssignObjOper의 대입/체이닝 연산자를 그대로 재사용
	using AssignObjOper::operator=;
	using AssignObjOper::operator<<;

	// XmlObj&를 그대로 받는 시그니처는 컴파일러가 암시적으로 만드는
	// 복사 대입 연산자와 충돌하므로 직접 재정의해서 부모 로직으로 위임한다
	void operator=(const XmlObj& rhs){
		XmlObj* lVal = GetTarget();
		XmlObj* rVal = const_cast<XmlObj&>(rhs).GetTarget();	// rhs도 자기 GetTarget()으로 진짜 대상을 얻는다
		if (lVal == rVal) return;
		lVal->DelVals();
		lVal->DelAttrs();
		DepCpy(lVal, rVal);
	}
	XmlObjOper Insert(int idx);

};


/* =================================================================================
* 대리 객체(Proxy) 계층
*
* XmlObjOper - 다른 XmlObj를 가리키며 XmlObj와 동일한 API를 그대로 노출한다.
* XmlAttrOper - 속성 하나만 다루는 완전히 독립적인 클래스다.
*               진입 경로가 하나뿐이라 Assign 계열을 상속할 이유가 없다.
===================================================================================*/

class XmlObjOper : public AssignObjOper, public AssignAttrOper{
	XmlObj* ObjRoot;
	bool isNewSlot;	//삽입시
protected:
	XmlObj* GetTarget();
public:
	XmlObjOper(XmlObj* ObjRoot, bool NewSlot = false);

	using AssignObjOper::operator=;
	using AssignObjOper::operator<<;

	// XmlObjOper&끼리의 대입도 각자 GetTarget()으로 진짜 대상을 구해서 위임한다.
	// (explicit 변환연산자를 우회하지 않는 이식성 있는 방식)
	void operator=(const XmlObjOper& rhs);
	void operator<<(XmlObj*);
	void operator<<(XmlObj&);

	XmlObjOper Insert(int idx);

	// 변환 연산자 - 아직 stub 상태, 실제 구현 채워야 함
	explicit operator char*();
	explicit operator XmlObj*();

	void SetObjRoot(XmlObj*);
};

class XmlAttrOper{
	XmlAttrObj* AttrRoot;
public:
	XmlAttrOper(XmlAttrObj* AttrRoot);

	void operator=(char*);
	void operator=(DynamicStr*);

	// 변환 연산자 - 아직 stub 상태, 실제 구현 채워야 함
	operator char*();
};