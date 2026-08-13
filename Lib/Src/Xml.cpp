#define _CRT_SECURE_NO_WARNINGS
#include "../Include/Xml.h"

using namespace std;
using namespace Dynamic;

/* ================================================
* Xml Print - 트리를 문자열로 출력하는 유틸리티
==================================================*/
static void XmlAttrPrint(XmlObj* Obj){
	int AttrsIdx = Obj->getAttrsIdx();
	AttrsIdx != -1 ? printf(" ") : 0;
	for (int i = 0; i <= AttrsIdx; i++){
		if (i == AttrsIdx) printf("%s = \"%s\"", Obj->getAttr(i)->getName(), Obj->getAttr(i)->getValue());
		else printf("%s = \"%s\" ", Obj->getAttr(i)->getName(), Obj->getAttr(i)->getValue());
	}
}

static void XmlValuePrint(XmlObj* Obj, int floor){
	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("<%s", Obj->getName());
	XmlAttrPrint(Obj);

	if (Obj->isNul()){
		printf("/>\n");
	}
	else{
		printf(">\n");
		for (int i = 0; i <= floor; i++){
			printf("\t");
		}
		printf("%s\n", Obj->getVal()->Val.getName());
		for (int i = 0; i < floor; i++){
			printf("\t");
		}
		printf("</%s>\n", Obj->getName());
	}
}

static void XmlObjPrint(XmlObj* Obj, int floor){
	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("<%s", Obj->getName());
	XmlAttrPrint(Obj);
	printf(">\n");

	XmlObj* pObj = nullptr;

	for (int i = 0; i <= Obj->getObjIdx(); i++){
		pObj = Obj->getObj(i);

		if (pObj->isVal()){
			XmlValuePrint(pObj, floor + 1);
		}
		else if(pObj->isObj()){
			XmlObjPrint(pObj, floor + 1);
		}
		else if(pObj->isNul()){
			XmlValuePrint(pObj, floor + 1);
		}
	}

	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("</%s>\n", Obj->getName());
}

void XmlPrint(XmlObj* Root){
	XmlVal* cursor = Root;
	XmlObj* Obj = dynamic_cast<XmlObj*>(cursor);

	if (Obj->isVal()){
		XmlValuePrint(Obj, 0);
	}
	else if(Obj->isObj()){
		XmlObjPrint(Obj, 0);
	}
	else if(Obj->isNul()){
		XmlValuePrint(Obj, 0);
	}
}


/* ================================================
* Xml Value (XmlObj의 내부 값 노드)
==================================================*/
XmlValue::XmlValue(char* newValue) : Val(newValue){}
XmlValue::XmlValue(DynamicStr* newValue) : Val(newValue){}
char* XmlValue::c_toString() {
	return Val.Name.Get_Str();
}
DynamicStr* XmlValue::d_toString()  {
	return &Val.Name;
}


/* ================================================
* Xml Attribute Class
==================================================*/
XmlAttrObj::XmlAttrObj(){}
XmlAttrObj::XmlAttrObj(DynamicStr* Name, DynamicStr* Value){
	this->Name.Set_Str(Name->Get_Str());
	this->Value.Set_Str(Value->Get_Str());
}
XmlAttrObj::XmlAttrObj(char* Name, char* Value){
	this->Name.Set_Str(Name);
	this->Value.Set_Str(Value);
}
XmlAttrObj::~XmlAttrObj(){}
char* XmlAttrObj::getName(){
	return Name.Get_Str();
}
char* XmlAttrObj::getValue(){
	return Value.Get_Str();
}
void XmlAttrObj::setValue(char* rVal){
	Value.Set_Str(rVal);
}
void XmlAttrObj::setValue(DynamicStr* rVal){
	Value.Set_Str(rVal->Get_Str());
}


/* ================================================
* XmlObj
==================================================*/
XmlObj::XmlObj(DynamicStr *Name, bool isNotNull) : Name(Name, 128){
	XmlObj* Obj = XmlParser::Parse(Name);
	if (Obj == nullptr && isNotNull){
		InitValSet();
	}
	else if (Obj != nullptr){
		DepCpy(this, Obj);
		delete Obj;
	}
}
XmlObj::XmlObj(char *Name, bool isNotNull) : Name(Name, 128){
	XmlObj* Obj = XmlParser::Parse(Name);
	if (Obj == nullptr && isNotNull){
		InitValSet();
	}
	else if (Obj != nullptr){
		DepCpy(this, Obj);
		delete Obj;
	}
}
XmlObj* XmlObj::getParentXml(){	return Parent; }

char* XmlObj::c_toString() {
	auto* toStr = new DynamicStr(128);

	Node<XmlVal>* Cur = Vals.getHead();

	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}

	char* ptoStr = toStr->Get_Str();

	toStr->str_free();
	delete toStr;

	return ptoStr;
}
DynamicStr* XmlObj::d_toString()  {	// 호출자가 delete 해줘야 함 (이건 안 좋은 규칙이라 나중에 개선 필요)
	auto* toStr = new DynamicStr(128);

	Node<XmlVal>* Cur = Vals.getHead();

	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}

	return toStr;
}

void XmlObj::InitValSet(){
	XmlVal* InitVal = new XmlValue("");
	Vals.AddList(InitVal);
}

XmlContentType XmlObj::getContentType(){
	Node<XmlVal>* head = Vals.getHead();
	if (head == nullptr) return XmlContentType::NUL;

	XmlVal* v = head->getData();

	if (dynamic_cast<XmlNull*>(v)) return XmlContentType::NUL;
	if (dynamic_cast<XmlValue*>(v)) return XmlContentType::VAL;
	return XmlContentType::OBJ;   // 그 외엔 XmlObj 자식들
}

bool XmlObj::isObj(){
	if (getContentType() == XmlContentType::OBJ) return true;
	else return false;
}

bool XmlObj::isVal(){
	if (getContentType() == XmlContentType::VAL) return true;
	else return false;
}

bool XmlObj::isNul(){
	if (getContentType() == XmlContentType::NUL) return true;
	else return false;
}

void XmlObj::DelAttrs(){ Attrs.AllDelList(); }
void XmlObj::DelVals(){
	Vals.AllDelList();
	XmlVal* NullVal = new XmlValue("");
	Vals.setNode(NullVal);
}
void XmlObj::DelObjVal(int idx){	//0이 가장 앞임
	if (idx == -1 || idx > Vals.Index){
		Vals.DelList(Vals.Index+1);
	}
	else{
		Vals.DelList(idx+1);
	}
}
void XmlObj::DelAttrVal(int idx){
	if (idx == -1 || idx > Vals.Index){
		Attrs.DelList(Attrs.Index+1);
	}
	else{
		Attrs.DelList(idx+1);
	}
}

char* XmlObj::getName(){
	return Name.getName();
}
void XmlObj::setName(char* Name){
	this->Name.SetName(Name);
}
void XmlObj::setName(DynamicStr* Name){
	this->Name.SetName(Name);
}
int XmlObj::getObjIdx(){
	if (isVal() || isNul()) return -1;
	return Vals.Index;
}
int XmlObj::getAttrsIdx(){
	return Attrs.Index;
}

void XmlObj::setVal(DynamicStr* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::setVal(char* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::setObj(DynamicStr* newObj, bool isNotNull){
	XmlVal* pnewObj = nullptr;
	isNotNull ? pnewObj = new XmlObj(newObj) : pnewObj = new XmlObj(newObj, NULL);
	static_cast<XmlObj*>(pnewObj)->Parent = this;
	Vals.setNode(pnewObj);
}
void XmlObj::setObj(char* newObj, bool isNotNull){
	XmlVal* pnewObj = nullptr;
	isNotNull ? pnewObj = new XmlObj(newObj) : pnewObj = new XmlObj(newObj, NULL);
	static_cast<XmlObj*>(pnewObj)->Parent = this;
	Vals.setNode(pnewObj);
}
void XmlObj::addObj(DynamicStr* newObj, bool isNotNull){
	XmlVal* pnewObj = nullptr;
	isNotNull ? pnewObj = new XmlObj(newObj) : pnewObj = new XmlObj(newObj, NULL);
	static_cast<XmlObj*>(pnewObj)->Parent = this;
	if (isVal() || isNul()){
		Vals.setNode(pnewObj);
	}
	else if(isObj()){
		Vals.AddList(pnewObj);
	}
}
void XmlObj::addObj(char* newObj, bool isNotNull){
	XmlVal* pnewObj = nullptr;
	isNotNull ? pnewObj = new XmlObj(newObj) : pnewObj = new XmlObj(newObj, NULL);
	static_cast<XmlObj*>(pnewObj)->Parent = this;
	if (isVal() || isNul()){
		Vals.setNode(pnewObj);
	}
	else if(isObj()){
		Vals.AddList(pnewObj);
	}
}

void XmlObj::setAttr(DynamicStr* AttrsName, DynamicStr* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.setNode(newAttr);
}
void XmlObj::setAttr(char* AttrsName, char* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.setNode(newAttr);
}
void XmlObj::addAttr(DynamicStr* AttrsName, DynamicStr* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.AddList(newAttr);
}
void XmlObj::addAttr(char* AttrsName, char* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.AddList(newAttr);
}

XmlValue* XmlObj::getVal(){
	if (isObj()){
		// Obj가 들어있는데 Val을 꺼내려고 해서 실패하는 경우
		return nullptr;
	}
	return dynamic_cast<XmlValue*>(Vals.getHead()->getData());
}
XmlObj* XmlObj::getObj(int idx){
	if (isVal() || isNul()){
		// Value면 Obj가 아니기 때문에 실패
		return nullptr;
	}
	Node<XmlVal>* pNode = nullptr;
	if (idx == -1)
		pNode = Vals.getTail();
	else
		pNode = Vals.get(idx);

	if (pNode == nullptr){
		return nullptr;
	}
	XmlVal* data = pNode->getData();
	return dynamic_cast<XmlObj*>(data);
}
XmlObj* XmlObj::getObj(char* name){
	if (isVal() || isNul()){
		return nullptr;
	}
	Node<XmlVal>* pNode = nullptr;
	XmlObj* pObj = nullptr;
	pNode = Vals.getHead();
	for (int i = 0; i <= Vals.Index; i++){
		pObj = dynamic_cast<XmlObj*>(pNode->getData());
		if (DynamicStr::StrCmp_Org(pObj->getName(), name)){
			return pObj;
		}
		pNode = pNode->getNextNode();
	}
	return nullptr;	// Search Failed
}

XmlAttrObj* XmlObj::getAttr(int idx){
	if (Attrs.Index <= -1) return nullptr;	// 속성이 아예 없는 경우
	Node<XmlAttrObj>* pNode = nullptr;

	if (idx == -1)
		pNode = Attrs.getTail();
	else
		pNode = Attrs.get(idx);

	if (pNode == nullptr)
		return nullptr;

	XmlAttrObj* data = pNode->getData();
	return data;
}
XmlAttrObj* XmlObj::getAttr(char* name){
	if (Attrs.Index <= -1) return nullptr;	// 속성이 아예 없는 경우
	Node<XmlAttrObj>* pNode = nullptr;
	XmlAttrObj* pAttr = nullptr;
	pNode = Attrs.getHead();

	for (int i = 0; i <= Attrs.Index; i++){
		pAttr = dynamic_cast<XmlAttrObj*>(pNode->getData());
		if (DynamicStr::StrCmp_Org(pAttr->getName(), name)){
			return pAttr;
		}
		pNode = pNode->getNextNode();
	}
	return nullptr; // Search Failed
}

XmlObj* XmlObj::GetTarget(){
	return this;
}

// 자식 객체를 인덱스로 찾는 연산
XmlObjOper XmlObj::operator()(int idx){
	XmlObj* obj = this->SearchObjAttr<XmlObj*>(idx, TYPE::OBJ);
	XmlObjOper ObjOper(obj);
	return ObjOper;
}

// 자식 객체를 이름으로 찾는 연산
XmlObjOper XmlObj::operator()(char* ObjName){
	XmlObj* obj = this->SearchObjAttr<XmlObj*>(ObjName, TYPE::OBJ);
	if (obj == nullptr){
		XmlObj* root = GetTarget();
		root->addObj(ObjName);
		obj = root->getObj(ObjName);
	}
	XmlObjOper ObjOper(obj);
	return ObjOper;
}

XmlObjOper XmlObj::Insert(int idx){		//0 이 가장 맨앞으로 할것 LinkedList는 1이 가장 맨앞이지만 API는 0으로 시작한게 많으니..
	LinkedList<XmlVal> *pNode = &this->Vals;
	int NodeIndex = pNode->Index;

	//idx값이 터무니없이 크다던가 혹은 -1이라던가 하면 그냥 맨끝에 붙이는걸로 합의
	if (NodeIndex <= -1 || NodeIndex < idx){
		XmlObj* obj = this->SearchObjAttr<XmlObj*>(idx, TYPE::OBJ);
		XmlObjOper ObjOper(obj);	//어차피 이게 마지막 그거임
		return ObjOper;
	}
	else{
		XmlVal* InsertVal = new XmlObj("__placeholder__");
		pNode->AddList(InsertVal, idx+1);
		XmlObj* obj = static_cast<XmlObj*>(this->Vals.getHead()->getData());
		XmlObjOper ObjOper(obj, true);
		return ObjOper;
	}
}

/* ================================================
* AssignObjOper - 노드 대입/추가(<<, =) 담당
==================================================*/
void AssignObjOper::DepCpy(XmlObj* lVal, XmlObj* rVal) {
	// 1단계: 이름 대입
	lVal->setName(rVal->getName());

	int rValIdx = rVal->getObjIdx();
	int rValAttrIdx = rVal->getAttrsIdx();

	// 2단계: 속성 넣어주기
	for (int AttrIdx = 0; AttrIdx <= rValAttrIdx; AttrIdx++) {
		char* AttrName = rVal->getAttr(AttrIdx)->getName();
		char* AttrVal = rVal->getAttr(AttrIdx)->getValue();
		lVal->addAttr(AttrName, AttrVal);
	}

	// 2.5단계: Parent 대입
	lVal->Parent = rVal->Parent;

	// 3단계: 내부 객체 재귀 복사
	// 주의: 이 리스트는 항상 XmlObj만 담는다는 전제(혼합 콘텐츠 미지원) 하에 null 체크를 생략함
	for (int ObjIdx = 0; ObjIdx <= rValIdx; ObjIdx++) {
		XmlObj* rValObj = rVal->getObj(ObjIdx);

		char* rValObjName = rValObj->getName();
		lVal->addObj(rValObjName, NULL);
		DepCpy(lVal->getObj(ObjIdx), rValObj);
	}

	//4단계: 내부 Value 복사
	if (rVal->isVal()){
		XmlValue* pVal = rVal->getVal();
		lVal->setVal(rVal->getVal()->c_toString());
	}
}
bool AssignObjOper::ValSet(char* rVal) {
	XmlObj* pTarget = GetTarget();
	if (pTarget->isVal() || pTarget->isNul()) {
		pTarget->setVal(rVal);
		return true;
	}
	return false;   // Value가 아님, Obj임
}
bool AssignObjOper::ValSet(DynamicStr* rVal) {
	XmlObj* pTarget = GetTarget();
	if (pTarget->isVal() || pTarget->isNul()) {
		pTarget->setVal(rVal);
		return true;
	}
	return false;   // Value가 아님, Obj임
}
void AssignObjOper::operator<<(char* rVal) {
	XmlObj* rValXml = XmlParser::Parse(rVal);
	if (rValXml == nullptr){
		if (ValSet(rVal)) return;
		GetTarget()->addObj(rVal, NULL);
		rValXml->Parent = GetTarget();
	}
	else{
		this->operator<<(rValXml);
	}
}
void AssignObjOper::operator<<(DynamicStr* rVal) {
	XmlObj* rValXml = XmlParser::Parse(rVal);
	if (rValXml == nullptr){
		if (ValSet(rVal)) return;
		GetTarget()->addObj(rVal, NULL);
		rValXml->Parent = GetTarget();
	}
	else{
		this->operator<<(rValXml);
	}
}

void AssignObjOper::operator<<(XmlObj* rVal) {
	XmlObj* RootObj = GetTarget();
	if (RootObj == nullptr){ return; }
	RootObj->addObj("", NULL);
	XmlObj* addLVal = RootObj->getObj();	// 가장 마지막 객체 가져오기
	rVal->Parent = addLVal->Parent;
	DepCpy(addLVal, rVal);
}

void AssignObjOper::operator<<(XmlObj& rVal) {
	XmlObj* RootObj = GetTarget();
	if (RootObj == nullptr){ return; }
	RootObj->addObj("", NULL);
	XmlObj* addLVal = RootObj->getObj();	// 가장 마지막 객체 가져오기
	rVal.Parent = addLVal->Parent;
	DepCpy(addLVal, &rVal);
}

void AssignObjOper::operator=(char* rVal) {
	XmlObj* rValXml = XmlParser::Parse(rVal);
	if (rValXml == nullptr){
		//if (ValSet(rVal)) return;
		GetTarget()->setVal(rVal);
		//GetTarget()->setObj(rVal);
	}
	else{
		this->operator=(rValXml);
	}
}

void AssignObjOper::operator=(DynamicStr* rVal) {
	XmlObj* rValXml = XmlParser::Parse(rVal);
	if (rValXml == nullptr){
		//if (ValSet(rVal)) return;
		GetTarget()->setObj(rVal);
	}
	else{
		this->operator=(rValXml);
	}
}

void AssignObjOper::operator=(XmlObj* rVal) {
	XmlObj* lVal = GetTarget();
	if (lVal == rVal) return;
	lVal->DelVals();
	lVal->DelAttrs();
	DepCpy(lVal, rVal);
}

void AssignObjOper::operator=(XmlObj& rVal) {
	XmlObj* lVal = GetTarget();
	if (lVal == &rVal) return;
	lVal->DelVals();
	lVal->DelAttrs();
	DepCpy(lVal, &rVal);
}


/* ================================================
* AssignAttrOper - 속성 검색/대입([]) 담당
==================================================*/
template<typename T>
T AssignAttrOper::SearchObjAttr(int idx, TYPE type){	// XmlObj* 또는 XmlAttrObj*만 반환
	if (type == TYPE::OBJ){
		void* raw = GetTarget()->getObj(idx);
		return static_cast<T>(raw);
	}
	else{
		void* raw = GetTarget()->getAttr(idx);
		return static_cast<T>(raw);
	}
}
template<typename T>
T AssignAttrOper::SearchObjAttr(char* Name, TYPE type){	// XmlObj* 또는 XmlAttrObj*만 반환
	if (type == TYPE::OBJ){
		void* raw = GetTarget()->getObj(Name);
		return static_cast<T>(raw);
	}
	else{
		void* raw = GetTarget()->getAttr(Name);
		return static_cast<T>(raw);
	}
}

XmlAttrOper AssignAttrOper::operator[](int idx){
	XmlAttrObj* attr = this->SearchObjAttr<XmlAttrObj*>(idx, TYPE::ATTR);

	XmlAttrOper AttrOper(attr);
	return AttrOper;
}
XmlAttrOper AssignAttrOper::operator[](char* AttrName){
	XmlAttrObj* attr = this->SearchObjAttr<XmlAttrObj*>(AttrName, TYPE::ATTR);
	if (attr == nullptr){	// 속성이 없는 경우, 이름으로 대입을 한 것이기 때문에 새로 만들어줌
		XmlObj* Obj = this->GetTarget();
		Obj->addAttr(AttrName, "");
		attr = Obj->getAttr(AttrName);
	}
	XmlAttrOper AttrOper(attr);
	return AttrOper;
}


/* ================================================
* XmlObjOper - 다른 XmlObj를 가리키는 대리 객체
==================================================*/
XmlObjOper::XmlObjOper(XmlObj* ObjRoot, bool NewSlot) : ObjRoot(ObjRoot), isNewSlot(NewSlot){}

XmlObj* XmlObjOper::GetTarget(){
	return ObjRoot;
}

void XmlObjOper::operator=(const XmlObjOper& rhs){
	XmlObj* lVal = GetTarget();
	//XmlObj* rVal = rhs.GetTarget();
	XmlObj* rVal = const_cast<XmlObjOper&>(rhs).GetTarget();
	if (lVal == rVal) return;
	lVal->DelVals();
	lVal->DelAttrs();
	DepCpy(lVal, rVal);
}

void XmlObjOper::operator<<(XmlObj* rVal){
	if (this->isNewSlot){
		AssignObjOper::operator=(rVal);
		isNewSlot = false;
	}
	else{
		AssignObjOper::operator<<(rVal);
	}
}
void XmlObjOper::operator<<(XmlObj& rVal){
	if (this->isNewSlot){
		AssignObjOper::operator=(rVal);
		isNewSlot = false;
	}
	else{
		AssignObjOper::operator<<(rVal);
	}
}

// 반환 연산자 - 아직 stub 상태
XmlObjOper::operator char*(){
	XmlObj* ObjRoot = this->ObjRoot;
	if (ObjRoot == nullptr) return nullptr;
	if (ObjRoot->isVal()){
		return ObjRoot->getVal()->c_toString();
	}
	else{
		return nullptr;
	}
}
XmlObjOper::operator XmlObj*(){
	return this->ObjRoot;
}

void XmlObjOper::SetObjRoot(XmlObj* xmlobj){}


/* ================================================
* XmlAttrOper - 속성 하나만 다루는 독립 클래스
==================================================*/
XmlAttrOper::XmlAttrOper(XmlAttrObj* AttrRoot) : AttrRoot(AttrRoot){}

void XmlAttrOper::operator=(char* rStrVal){
	if (this->AttrRoot == nullptr) return;	// 속성이 없는 경우 실패
	this->AttrRoot->setValue(rStrVal);
}
void XmlAttrOper::operator=(DynamicStr* rStrVal){
	if (this->AttrRoot == nullptr) return;	// 속성이 없는 경우 실패
	this->AttrRoot->setValue(rStrVal);
}

// 변환 연산자 - 아직 stub 상태
XmlAttrOper::operator char*(){
	if (this->AttrRoot == nullptr) return nullptr;
	XmlAttrObj* AttrRoot = this->AttrRoot;
	return AttrRoot->getValue();
}

/* ================================================
* XmlParser - 파싱을 진행해주는 정적 함수 전용 클래스
==================================================*/
void XmlParser::ValMake(char* rVal, int& Csr, _PrsTol& PrsTol, bool isCdata){	//여기서 Xml과 더불어 Value만 만들어준다
	if (PrsTol.CurXmlObj == nullptr){
		return;	//그냥 계속 반환하면서 넘기셈 의미가 없음
	}
	PrsTol.Value = new DynamicStr(16);
	if (isCdata){
		Csr += 7;
		for (; ; Csr++){
			if (rVal[Csr] == ']' && rVal[Csr + 1] == ']' && rVal[Csr + 2] == '>'){ 
				Csr += 3; break; 
			}
			PrsTol.Value->Append_Char(&rVal[Csr]);
		}
	}
	else{
		for (; rVal[Csr] != '<'; Csr++){
			PrsTol.Value->Append_Char(&rVal[Csr]);
		}
		Csr--;	//CurCsr가 '<'가 되게끔 유도
	}

	if (PrsTol.CurXmlObj != nullptr){
		PrsTol.CurXmlObj->setVal(PrsTol.Value);
	}

	delete PrsTol.Value;
	PrsTol.Value = nullptr;
}

void XmlParser::ObjMake(char* rVal, int& Csr, _PrsTol& PrsTol){	//여기서 Xml을 만들어준다 Value까지는 안만듬
	//Obj만들기(ObjName), Attr만들기(AttrName, AttrValue)
	PrsTol.Stack++;
	PrsTol.ObjName = new DynamicStr(16);
	PrsTol.AttrName = new DynamicStr(16);
	PrsTol.AttrValue = new DynamicStr(16);		

	bool ObjNameFns = false;	//공백으로 구분
	bool AttrNameFns = false;	//=로 구분
	bool AttrValStr = false;
	bool AttrValFns = false;	//" 로 구분
	bool isObjNull = false;		// false로 계속 유지가 된다면 CurXmlObj에 공백 Value를 반드시 넣어줄것

	for (; rVal[Csr] != '>'; Csr++){
		PrsTol.PrvWord = rVal[Csr - 1];
		PrsTol.CurWord = rVal[Csr];
		PrsTol.NxtWord = rVal[Csr + 1];

		//ObjName Mode 띄어쓰기 미허용
		if (PrsTol.CurWord == ' '){	//이름이 끝났음을 의미
			if (ObjNameFns == false){
				if (PrsTol.CurXmlObj != nullptr){
					auto* rVal = new XmlObj(PrsTol.ObjName, NULL);
					PrsTol.CurXmlObj->operator<<(rVal);
					PrsTol.CurXmlObj = PrsTol.CurXmlObj->getObj();
				}
				else{
					PrsTol.CurXmlObj = new XmlObj(PrsTol.ObjName, NULL);
				}
				ObjNameFns = true; 			
			}
			else if (AttrNameFns == false) { AttrNameFns = true; }
			else if (AttrNameFns && AttrValFns){
				AttrNameFns = false; AttrValFns = false; AttrValStr = false;
				delete PrsTol.AttrName; delete PrsTol.AttrValue;
				PrsTol.AttrName = new DynamicStr(16); PrsTol.AttrValue = new DynamicStr(16);
			}
			else continue;
		}
		//else if (PrsTol.CurWord == ','){	//속성이 또 있다는 이야기
		//	if (AttrNameFns && AttrValFns){
		//		AttrNameFns = false; AttrValFns = false;
		//		delete PrsTol.AttrName; delete PrsTol.AttrValue;
		//		PrsTol.AttrName = new DynamicStr(16); PrsTol.AttrValue = new DynamicStr(16);
		//	}
		//}
		else if (PrsTol.CurWord == '='){	//속성명이 끝났음을 의미
			if (AttrNameFns == false) AttrNameFns = true;
		}
		else if (PrsTol.CurWord == '\"'){
			if (AttrValStr == false) AttrValStr = true;
			else if (AttrValStr && AttrValFns == false) { 
				AttrValFns = true; 
				PrsTol.CurXmlObj->addAttr(PrsTol.AttrName, PrsTol.AttrValue);
				//PrsTol.CurXmlObj가 nullptr인 경우는 없는게 맞긴하다만.. (<"이런식으로> 쓴 xml이라면 크래쉬가 날것)
			}
		}
		else if (PrsTol.CurWord == '/' && PrsTol.NxtWord == '>'){ 
			isObjNull = true; 
		}
		else{
			if (ObjNameFns == false){
				PrsTol.ObjName->Append_Char(&PrsTol.CurWord);
			}
			else if (AttrNameFns == false){
				PrsTol.AttrName->Append_Char(&PrsTol.CurWord);
			}
			else if (AttrValStr && AttrValFns == false){
				PrsTol.AttrValue->Append_Char(&PrsTol.CurWord);
			}
		}
	}
	if (ObjNameFns == false){ //<Name> 이렇게 끝나는 경우
		if (PrsTol.CurXmlObj != nullptr){
			auto* rVal = new XmlObj(PrsTol.ObjName, NULL);
			PrsTol.CurXmlObj->operator<<(rVal);
			PrsTol.CurXmlObj = PrsTol.CurXmlObj->getObj();

			//Debug
			XmlObj* pRoot = PrsTol.CurXmlObj->getParentXml();
			for (;;){
				if (pRoot->getParentXml() == nullptr) break;
				pRoot = pRoot->getParentXml();
			}
		}
		else{
			PrsTol.CurXmlObj = new XmlObj(PrsTol.ObjName, NULL);
		}
		ObjNameFns = true;
	}
	
	if (isObjNull == false){
		PrsTol.CurXmlObj->setVal("");
	}
	Csr--;	//rVal[Csr] == '>' 인 시점에서 끝이 났기에 한번 빼주기
	delete PrsTol.ObjName; 	delete PrsTol.AttrName;	delete PrsTol.AttrValue;
	PrsTol.ObjName = PrsTol.AttrName = PrsTol.AttrValue = nullptr;
}

void XmlParser::CurXmlObjParse(char* rVal, int& Csr, _PrsTol& PrsTol){
	Csr++;
	for (; rVal[Csr] != '>'; Csr++){
		PrsTol.PrvWord = rVal[Csr - 1];
		PrsTol.CurWord = rVal[Csr];
		PrsTol.NxtWord = rVal[Csr + 1];
			
		if (PrsTol.CurWord == '/'){ //종료 태그
			if (PrsTol.CurXmlObj != nullptr && PrsTol.Stack > 1){
				PrsTol.CurXmlObj = PrsTol.CurXmlObj->getParentXml();
			}
			PrsTol.Stack--;
			for (;; Csr++){ if (rVal[Csr] == '>'){ return; } }
		}
		//특수 선언 부 CDATA와 주석
		else if (PrsTol.CurWord == '!'){
			if (PrsTol.NxtWord == '-' && rVal[Csr+2] == '-'){//주석
				for (;; Csr++){ if (rVal[Csr] == '>'){ Csr++; return; } }
			}
			//Value가 되어야함 CDATA의 경우에는
			//[CDATA[ 고정 리터럴
			if (DynamicStr::StrCmp_rValSize(rVal, "[CDATA[", Csr+1)){
				ValMake(rVal, ++Csr, PrsTol, true);
			}
			else{//DOCTYPE같은 경우 현재는 Skip
				for (;; Csr++){ if (rVal[Csr] == '>'){ Csr++; return; } }
			}
		}
		//처리 명령어 나중에 만들거나 자료구조 하나 만들어야함 현재는 Skip
		else if (PrsTol.CurWord == '?'){
			for (;; Csr++){ if (rVal[Csr] == '>'){ Csr++; return; } }
		}
		else{	//그냥 파싱 진행하면 됨			
			ObjMake(rVal, Csr, PrsTol);
		}
	}
	return;
}

XmlObj* XmlParser::RealParser(char* rVal){
	//실제 파싱을 해주는 파서부
	_PrsTol PrsTol{};
	for (int Csr = 0; rVal[Csr] != '\0'; Csr++){
		PrsTol.PrvWord = rVal[Csr - 1];
		PrsTol.CurWord = rVal[Csr];
		PrsTol.NxtWord = rVal[Csr + 1];

		if (Csr == 0 && PrsTol.CurWord != '<') break;

		if (PrsTol.CurWord == '<'){	//Obj Mode
			CurXmlObjParse(rVal, Csr, PrsTol);
		}
		else{	//Value
			//현재 나의 자료구조는 Mixed Content구현이 안되어있음 고로 공백 및 탭 이런것들 제외할것
			if (PrsTol.CurWord == ' '
				|| PrsTol.CurWord == '\n'
				|| PrsTol.CurWord == '\t') continue;
			ValMake(rVal, Csr, PrsTol);
		}
	}
	auto* rtnXml = PrsTol.CurXmlObj;
	PrsTol.CurXmlObj = nullptr;
	return rtnXml;
}

XmlObj* XmlParser::Parse(char* rVal){	//Static 메소드
	XmlParser Parser;
	return Parser.RealParser(rVal);
}
XmlObj* XmlParser::Parse(DynamicStr* rVal){	//Static 메소드
	XmlParser Parser;
	return Parser.RealParser(rVal->Get_Str());
}

