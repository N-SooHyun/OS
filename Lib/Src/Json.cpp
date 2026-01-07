#include <stdlib.h>
//#include "Str.h"
#include "../Include/Json.h"

using namespace std;
using namespace Dynamic;
using namespace JSON;


//<JNode class>--------------------------------------------------------------------------------------

void JNode::delType(){
	//이중소멸을 막기 위한 예외
	if (isTypeNull()){
		return;
	}

	JType Del_Node_Type = this->Cur_Type;
	switch (Del_Node_Type){
	case JType::OBJ:
	{
		JObj* obj = static_cast<JObj*>(P_Type);
		delete obj;
		break;
	}
	case JType::ARR:
	{
		JArr* arr = static_cast<JArr*>(P_Type);
		delete arr;
		break;
	}
	case JType::STRING:
	{
		DynamicStr* str = static_cast<DynamicStr*>(P_Type);
		delete str;
		break;
	}
	case JType::NUMBER:
	{
		int* num = static_cast<int*>(P_Type);
		delete num;
		break;
	}
	case JType::BOOL:
	{
		bool* bl = static_cast<bool*>(P_Type);
		delete bl;
		break;
	}
	case JType::DOUBLE:
	{
		double* dnum = static_cast<double*>(P_Type);
		delete dnum;
		break;
	}
	default:
	case JType::NULLTYPE:
	{
		break;
	}
	}
	P_Type = nullptr;
	Cur_Type = JType::NULLTYPE;

	//if (prev_Node != nullptr){
	//	//prev_Node한테 del된 상태를 알려줘야함 
	//	//안그러면 이중 소멸 발생함
	//	//어떤 객체가 소멸했는지 알아야 할것
	//	//어떤 배열이 소멸했는지 알아야 할것
	//	if (prev_obj != nullptr){
	//		
	//	} 
	//	if (prev_arr != nullptr){

	//	}

	//	
	//}
}

void JNode::setType(JType Set_Node_Type){
	if (!isTypeNull()){
		//P_Type이 존재하기 문에 소멸하고 해줘야 하기에 반환
		return;
	}
	ObjCnt = -2;
	ArrCnt = -2;
	Cur_Type = Set_Node_Type;
	switch (Cur_Type) {
	case JType::OBJ:
	{
		P_Type = new JObj();
		ObjCnt = -1;
		break;
	}
	case JType::ARR:
	{
		P_Type = new JArr();
		ArrCnt = -1;
		break;
	}
	case JType::STRING:
	{
		P_Type = new Dynamic::DynamicStr(128);
		Dynamic::DynamicStr* init_str = static_cast<Dynamic::DynamicStr*>(P_Type);
		init_str->Set_Str("0\0");
		break;
	}
	case JType::NUMBER:
	{
		P_Type = new int(); // 기본적으로 int로 초기화
		int* init_num = static_cast<int*>(P_Type);
		*init_num = 0;
		break;
	}
	case JType::BOOL:
	{
		P_Type = new bool();
		bool* init_bool = static_cast<bool*>(P_Type);
		*init_bool = false;
		break;
	}
	case JType::DOUBLE:
	{
		P_Type = new double();
		double* init_dnum = static_cast<double*>(P_Type);
		*init_dnum = 0.0;
		break;
	}
	case JType::NULLTYPE:
	{
		P_Type = nullptr; // null 타입은 포인터를 사용하지 않음
		break;
	}
	default:
	{
		P_Type = nullptr; // 기본적으로 nullptr로 초기화
		break;
	}
	}
}

void* JNode::getPType(){
	if (isTypeNull()){
		return P_Type;
	}
	return nullptr;
}

JNode::JType JNode::getType(){
	return this->Cur_Type;
}


//문자열 파싱 부분
void JNode::isObjArrCk(DynamicStr* str){
	unsigned const char* Str = str->Get_uStr();
	short first_focus = 0;
	short last_focus = str->str_last_focus;

	//문자열이 들어올때 확인하는 용도
	/*
	if (Str[first_focus] == '\"' && Str[last_focus] == '\"') {
	first_focus++;
	last_focus--;
	}
	*/



	while (Str[first_focus] == ' ' || Str[first_focus] == '\n') {
		first_focus++;
	}

	while (Str[last_focus] == ' ' || Str[last_focus] == '\n') {
		last_focus--;
	}


	if (Str[first_focus] == '{' && Str[last_focus] == '}'){
		//객체 Mode
		//Parsing 시작 함수 호출 하든가 하셈
		//StrParser obj_parser(this, str, JNode::JType::OBJ);
		StrParser obj_parser(this, str, JNode::JType::OBJ);
		//StrParser *obj_parser = new StrParser(this, str, JNode::JType::OBJ);
		//delete obj_parser;
	}
	else if (Str[first_focus] == '[' && Str[last_focus] == ']'){
		//배열 Mode
		//Parsing 시작 함수 호출 하든가 하셈
		StrParser arr_parser(this, str, JNode::JType::ARR);
	}
}


//예외처리목록
bool JNode::isTypeNull(){
	if (P_Type != nullptr) return false;
	return true;
}
bool JNode::isTypeMatch(JType match_type){
	if (Cur_Type == match_type) return true;
	return false;
}



//반환 연산자 = P_Type을 반환
JNode::operator int(){
	if (!isTypeMatch(JType::NUMBER)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return -1;
	}

	int* num = static_cast<int*>(P_Type);

	return *num;
}
JNode::operator int*(){
	if (!isTypeMatch(JType::NUMBER)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return nullptr;
	}

	int* num = static_cast<int*>(P_Type);

	return num;
}
JNode::operator double(){
	if (!isTypeMatch(JType::DOUBLE)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return -1.0;
	}
	double* dnum = static_cast<double*>(P_Type);

	return *dnum;
}
JNode::operator double*(){
	if (!isTypeMatch(JType::DOUBLE)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return nullptr;
	}
	double* dnum = static_cast<double*>(P_Type);
	return dnum;
}
JNode::operator bool(){
	if (!isTypeMatch(JType::BOOL)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return false;
	}
	bool* bl = static_cast<bool*>(P_Type);
	return *bl;
}
JNode::operator bool*(){
	if (!isTypeMatch(JType::BOOL)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return nullptr;
	}
	bool* pbl = static_cast<bool*>(P_Type);
	return pbl;
}
JNode::operator char(){
	if (!isTypeMatch(JType::STRING)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return '\0';
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	char* c = str->Get_Str();
	return c[0];
}
JNode::operator char*(){
	if (!isTypeMatch(JType::STRING)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return nullptr;
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	return str->Get_Str();
}
JNode::operator unsigned char(){
	if (!isTypeMatch(JType::STRING)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return '\0';
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	unsigned char* c = str->Get_uStr();
	return c[0];
}
JNode::operator unsigned char*(){
	if (!isTypeMatch(JType::STRING)){
		//실패하는 경우 타입이 Null이거나 타입매치가 안맞는경우
		return nullptr;
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	return str->Get_uStr();
}
//Node를 반화할때 처리

//대입 연산자 P_Type = 대입
void JNode::operator=(JType Set_Node_Type){
	if (!isTypeMatch(Set_Node_Type)) {
		//타입이 안맞으면 덮어쓸꺼니까 지워주셈
		delType();
		Cur_Type = Set_Node_Type;
		setType(Cur_Type);
	}
}
void JNode::operator=(int _num){
	if (!isTypeMatch(JType::NUMBER)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::NUMBER;
		setType(Cur_Type);
	}

	int* num = static_cast<int*>(P_Type);
	*num = _num; //복사
}
void JNode::operator=(int* _pnum){
	if (!isTypeMatch(JType::NUMBER)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::NUMBER;
		setType(Cur_Type);
	}
	int* pnum = static_cast<int*>(P_Type);
	*pnum = *_pnum; //복사 참조X
}
void JNode::operator=(double _dnum){
	if (!isTypeMatch(JType::DOUBLE)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::DOUBLE;
		setType(Cur_Type);
	}
	double* dnum = static_cast<double*>(P_Type);
	*dnum = _dnum;	//복사
}
void JNode::operator=(double* _pdnum){
	if (!isTypeMatch(JType::DOUBLE)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::DOUBLE;
		setType(Cur_Type);
	}
	double* dnum = static_cast<double*>(P_Type);
	*dnum = *_pdnum; //복사 참조 X
}
void JNode::operator=(bool _bl){
	if (!isTypeMatch(JType::BOOL)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::BOOL;
		setType(Cur_Type);
	}
	bool* bl = static_cast<bool*>(P_Type);
	*bl = _bl;	//복사
}
void JNode::operator=(bool* _pbl){
	if (!isTypeMatch(JType::BOOL)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::BOOL;
		setType(Cur_Type);
	}
	bool* bl = static_cast<bool*>(P_Type);
	*bl = *_pbl;	//복사 참조X
}
void JNode::operator=(char _c){
	if (!isTypeMatch(JType::STRING)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* c = static_cast<DynamicStr*>(P_Type);
	char* pc = &_c;
	c->Set_Char(pc);	//복사
}
void JNode::operator=(const char* _str){
	if (!isTypeMatch(JType::STRING)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	str->Set_Str(_str);	//복사

	//문자열 파싱 부분
	isObjArrCk(str);
}
void JNode::operator=(char* _str) {
	if (!isTypeMatch(JType::STRING)) {
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	str->Set_Str(_str);	//복사

	//문자열 파싱 부분
	isObjArrCk(str);
}
void JNode::operator=(unsigned char _uc){
	if (!isTypeMatch(JType::STRING)){
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* uc = static_cast<DynamicStr*>(P_Type);
	unsigned char* puc = &_uc;
	uc->Set_Char(puc);
}
void JNode::operator=(const unsigned char* _ustr){
	if (!isTypeMatch(JType::STRING)){
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	str->Set_Str(_ustr);	//복사

	//문자열 파싱 부분
	isObjArrCk(str);
}
void JNode::operator=(unsigned char *_ustr){
	if (!isTypeMatch(JType::STRING)) {
		//타입이 안맞으면 덮어쓸거니까 지워주셈
		delType();
		Cur_Type = JType::STRING;
		setType(Cur_Type);
	}
	DynamicStr* str = static_cast<DynamicStr*>(P_Type);
	str->Set_Str(_ustr);	//복사

	//문자열 파싱 부분
	isObjArrCk(str);
}

//Node를 받을때 처리
//rNode를 lNode로 대입하는 상황인데
//이런경우 rNode->P_Type을 lNode->P_Type에 대입하게 되는데
//P_Type을 2개의 인스턴스가 공유하기 때문에 rNode와 lNode가 소멸할때
//이중 소멸이 발생할 수 있으므로
//rNode의 P_Type은 nullptr로 만들어주어야 한다.
//즉, 이걸 사용한다는건 rValue의 값을 복사후 delType()으로 삭제함을 의미
//void JNode::operator=(JNode rNode) {
//	//즉 해당 연산자는 복사가 아닌 이동이라고 생각하면 된다.
//	//해당 rNode는 실제 지역변수가 아닌 복사생성으로 생성된 임시객체이므로
//	//rNode.P_Type = nullptr을 해줘도
//	//실제 호출된 매개변수 rNode.P_Type은 nullptr이 되지 않는다.
//	//고로 이 연산은 사용할 수 없도록 막았다.
//	operator=(&rNode);
//}
void JNode::operator=(JNode* rNode) {
	//즉 해당 연산자는 복사가 아닌 이동이라고 생각하면 된다.
	this->delType(); //기존 타입 삭제
	if (rNode->isTypeNull()) {
		//rNode가 Null타입인경우
		this->setType(JType::NULLTYPE);
		return;
	}
	this->Cur_Type = rNode->Cur_Type;
	this->P_Type = rNode->P_Type; //P_Type을 참조

	this->ObjCnt = rNode->ObjCnt; //객체 카운트
	this->ArrCnt = rNode->ArrCnt; //배열 카운트

	//rNode의 P_Type을 nullptr로 만들어서 이중소멸 방지
	rNode->P_Type = nullptr;
	rNode->Cur_Type = JType::NULLTYPE; //rNode의 타입을 Null로 변경
}

//객체와 배열에 대한 연산자
//Node["Key"]
JsonCallObjArr JNode::operator[](const char* key){
	//Obj타입이 아닌경우? 1. 덮어쓰기, 2. 실패하기 1번 선택
	if (!isTypeMatch(JType::OBJ)){
		//타입이 안맞는경우 덮어쓸꺼니까 지워주셈
		delType();
		setType(JType::OBJ);
		JsonCallObjArr Call(this, key);
		return Call;
	}
	JsonCallObjArr Call(this, key);
	return Call;
}
JsonCallObjArr JNode::operator[](const unsigned char *key){
	//Obj타입이 아닌경우? 1. 덮어쓰기, 2. 실패하기 1번 선택
	if (!isTypeMatch(JType::OBJ)){
		//타입이 안맞는경우 덮어쓸꺼니까 지워주셈
		delType();
		setType(JType::OBJ);
		JsonCallObjArr Call(this, key);
		return Call;
	}
	JsonCallObjArr Call(this, key);
	return Call;
}

//Node[1]
JsonCallObjArr JNode::operator[](int index){
	//Arr타입이 아닌경우? 1. 덮어쓰기 2. 실패하기 1번 선택
	if (!isTypeMatch(JType::ARR)){
		delType();
		setType(JType::ARR);
		JsonCallObjArr Call(this, index);
		return Call;
	}
	JsonCallObjArr Call(this, index);
	return Call;
}

//가장 끝 index에 값 넣기
JsonCallObjArr JNode::push(){
	if (this->Cur_Type == JNode::JType::OBJ){
		JsonCallObjArr fail;
		return fail;
	}
	//Arr타입이 아닌경우 덮어쓰기
	if (!isTypeMatch(JType::ARR)){
		delType();
		setType(JType::ARR);
		JsonCallObjArr call(this, JNode::JType::ARR);
		return call;
	}
	JsonCallObjArr call(this, JNode::JType::ARR);
	return call;
}

//가장 끝 객체에 값 넣기 안쓰는게 좋을거 같기도 함 key중복생길 수 있어서
JsonCallObjArr JNode::obj_push(const char* key){
	JsonCallObjArr j;
	return j;
}
JsonCallObjArr JNode::obj_push(const unsigned char* key){
	JsonCallObjArr j;
	return j;
}


//해당 인덱스 삭제하기
JsonCallObjArr JNode::arr_del(int index){
	//Arr타입이 아닌경우? 그냥 삭제 실패해버리셈
	if (!isTypeMatch(JType::ARR)){
		JsonCallObjArr failcall;
		return failcall;
	}
	JsonCallObjArr call(this, index, true);
	return call;
}

//해당 키가진 객체 삭제하기
JsonCallObjArr JNode::obj_del(const char* key){
	//Obj 타입이 아닌 경우? 그냥 삭제 실패해버리셈
	if (!isTypeMatch(JType::OBJ)){
		JsonCallObjArr failcall;
		return failcall;
	}
	JsonCallObjArr call(this, key, true);
	return call;
}
JsonCallObjArr JNode::obj_del(const unsigned char* key){
	//Obj 타입이 아닌 경우? 그냥 삭제 실패해버리셈
	if (!isTypeMatch(JType::OBJ)){
		JsonCallObjArr failcall;
		return failcall;
	}
	JsonCallObjArr call(this, key, true);
	return call;
}

//끝에만 삭제
JsonCallObjArr* JNode::arr_del(){
	//Arr타입이 아닌경우? 그냥 삭제 실패해버리셈
	if (!isTypeMatch(JType::ARR)){
		return nullptr;
	}
	return new JsonCallObjArr(this, JNode::JType::ARR, true);
}

//끝에만 삭제(이거는 모호해서 안쓰는게 좋을거 같음)
JsonCallObjArr* JNode::obj_del(){
	//Obj 타입이 아닌 경우? 그냥 삭제 실패해버리셈
	if (!isTypeMatch(JType::OBJ)){
		return nullptr;
	}
	return new JsonCallObjArr(this, JNode::JType::OBJ, true);
}

JsonCallObjArr* JNode::del(){
	if (this->Cur_Type == JNode::JType::ARR){
		JsonCallObjArr* arr = arr_del();
		//return &arr_del();
		return arr;
	}
	else if (this->Cur_Type == JNode::JType::OBJ){
		JsonCallObjArr* obj = obj_del();
		//return &obj_del();
		return obj;
	}
	return nullptr;
}

void JNode::all_del() {
	if (this->Cur_Type == JNode::JType::ARR) {
		delType();
		setType(JType::ARR);
	}
	else if (this->Cur_Type == JNode::JType::OBJ) {
		delType();
		setType(JType::OBJ);
	}
}


//<JObj class>--------------------------------------------------------------------------------------


//<JArr class>--------------------------------------------------------------------------------------


//<JsonCtrl class>----------------------------------------------------------------------------------


//<JsonCallObjArr class>-------------------------------------------------------------------------------
//반환연산자
// = JNode["Key"]
// = JNode[1]
// = Cur_Obj->Value->P_Type(???)
JsonCallObjArr::operator int(){
	if (!getOper_Ctrl(JNode::JType::NUMBER)){	//실패하는경우
		return -1;
	}
	int* num = nullptr;

	if (Cur_Obj != nullptr){
		num = static_cast<int*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		num = static_cast<int*>(Cur_Arr->Value->P_Type);
	}

	return *num;
}
JsonCallObjArr::operator int*(){
	if (!getOper_Ctrl(JNode::JType::NUMBER)){	//실패하는경우
		return nullptr;
	}

	int* num = nullptr;

	if (Cur_Obj != nullptr){
		num = static_cast<int*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		num = static_cast<int*>(Cur_Arr->Value->P_Type);
	}

	return num;
}
JsonCallObjArr::operator double(){
	if (!getOper_Ctrl(JNode::JType::DOUBLE)){	//실패하는경우
		return -1.0;
	}

	double* dnum = nullptr;

	if (Cur_Obj != nullptr){
		dnum = static_cast<double*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		dnum = static_cast<double*>(Cur_Arr->Value->P_Type);
	}

	return *dnum;
}
JsonCallObjArr::operator double*(){
	if (!getOper_Ctrl(JNode::JType::DOUBLE)){	//실패하는경우
		return nullptr;
	}

	double* dnum = nullptr;

	if (Cur_Obj != nullptr){
		dnum = static_cast<double*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		dnum = static_cast<double*>(Cur_Arr->Value->P_Type);
	}

	return dnum;
}
JsonCallObjArr::operator bool(){
	if (!getOper_Ctrl(JNode::JType::BOOL)){	//실패하는경우
		return false;
	}

	bool* bl = nullptr;

	if (Cur_Obj != nullptr){
		bl = static_cast<bool*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		bl = static_cast<bool*>(Cur_Arr->Value->P_Type);
	}

	return *bl;
}
JsonCallObjArr::operator bool*(){
	if (!getOper_Ctrl(JNode::JType::BOOL)){	//실패하는경우
		return nullptr;
	}

	bool* bl = nullptr;

	if (Cur_Obj != nullptr){
		bl = static_cast<bool*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		bl = static_cast<bool*>(Cur_Arr->Value->P_Type);
	}

	return bl;
}
JsonCallObjArr::operator char(){
	if (!getOper_Ctrl(JNode::JType::STRING)){	//실패하는경우
		return '\0';
	}

	DynamicStr* str = nullptr;

	if (Cur_Obj != nullptr){
		str = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		str = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
	}

	char* c = str->Get_Str();
	return c[0];
}
JsonCallObjArr::operator char*(){
	if (!getOper_Ctrl(JNode::JType::STRING)){	//실패하는경우
		return nullptr;
	}

	DynamicStr* str = nullptr;

	if (Cur_Obj != nullptr){
		str = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		str = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
	}

	return str->Get_Str();
}
JsonCallObjArr::operator unsigned char(){
	if (!getOper_Ctrl(JNode::JType::STRING)){	//실패하는경우
		return '\0';
	}

	DynamicStr* str = nullptr;

	if (Cur_Obj != nullptr){
		str = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		str = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
	}

	unsigned char* c = str->Get_uStr();
	return c[0];
}
JsonCallObjArr::operator unsigned char*(){
	if (!getOper_Ctrl(JNode::JType::STRING)){	//실패하는경우
		return nullptr;
	}

	DynamicStr* str = nullptr;

	if (Cur_Obj != nullptr){
		str = static_cast<DynamicStr*>(Cur_Obj->Value->P_Type);
	}

	if (Cur_Arr != nullptr){
		str = static_cast<DynamicStr*>(Cur_Arr->Value->P_Type);
	}

	return str->Get_uStr();
}
JsonCallObjArr::operator JNode*(){
	//*JNode = Cur_Obj->Value;  Value를 반환해야함
	if (Cur_Obj == nullptr && Cur_Arr == nullptr){
		return nullptr;
	}

	if (Cur_Node == nullptr){
		return nullptr;
	}

	JNode* lNode = new JNode();


	//복사가아닌 이동을 해주어야함 기존 lNode <- rNode(JNode[..])
	//lNode = rNode[4] 는 곧
	//lNode = rNode->P_Type->4번째 Value(Jnode)를 참조하는거임
	//즉, 복사가 아닌 참조용도이기에 lNode는 delete하면 안됨

	if (Cur_Obj != nullptr){
		delete lNode;
		// lNode = Cur_Obj -> Value 참조
		/*lNode->Cur_Type = Cur_Obj->Value->Cur_Type;
		lNode->P_Type = Cur_Obj->Value->P_Type;
		lNode->ArrCnt = Cur_Obj->Value->ArrCnt;
		lNode->ObjCnt = Cur_Obj->Value->ObjCnt;*/
		lNode = Cur_Obj->Value;
	}
	if (Cur_Arr != nullptr){
		delete lNode;
		// lNode = Cur_Arr -> Value 참조
		/*lNode->Cur_Type = Cur_Arr->Value->Cur_Type;
		lNode->P_Type = Cur_Arr->Value->P_Type;
		lNode->ArrCnt = Cur_Arr->Value->ArrCnt;
		lNode->ObjCnt = Cur_Arr->Value->ObjCnt;*/
		lNode = Cur_Arr->Value;
	}

	/*lNode->prev_Node = Cur_Node;
	lNode->prev_obj = Cur_Obj;
	lNode->prev_arr = Cur_Arr;*/
	return lNode;
}


//대입연산자
// JNode["Key"] = 
// JNode[1] = 
// Cur_Obj->Value->P_Type(???) = 
void JsonCallObjArr::operator=(int _num){
	//예외 처리 및 덮어쓰기용도
	setOper_Ctrl(JNode::JType::NUMBER, &_num);
}
void JsonCallObjArr::operator=(int* _num){
	setOper_Ctrl(JNode::JType::NUMBER, _num);
}
void JsonCallObjArr::operator=(double _dnum){
	setOper_Ctrl(JNode::JType::DOUBLE, &_dnum);
}
void JsonCallObjArr::operator=(double* _dnum){
	setOper_Ctrl(JNode::JType::DOUBLE, _dnum);
}
void JsonCallObjArr::operator=(bool _bl){
	setOper_Ctrl(JNode::JType::BOOL, &_bl);
}
void JsonCallObjArr::operator=(bool* _bl){
	setOper_Ctrl(JNode::JType::BOOL, _bl);
}
void JsonCallObjArr::operator=(char _c){
	setOper_Ctrl(JNode::JType::STRING, &_c);
}
void JsonCallObjArr::operator=(char* _str){
	setOper_Ctrl(JNode::JType::STRING, _str, true);
}
void JsonCallObjArr::operator=(const char* _str) {
	setOper_Ctrl(JNode::JType::STRING, (void*)_str, true);
}
void JsonCallObjArr::operator=(unsigned char _c){
	setOper_Ctrl(JNode::JType::STRING, &_c);
}
void JsonCallObjArr::operator=(unsigned char* _str){
	setOper_Ctrl(JNode::JType::STRING, _str, true);
}
void JsonCallObjArr::operator=(const unsigned char* _str) {
	setOper_Ctrl(JNode::JType::STRING, (void*)_str, true);
}

void JsonCallObjArr::operator=(JNode* jnode){
	//Cur_Arr->Value = JNode;  Value에 대입하는건데
	//Value를 덮어씌우고 하는거라고 가정하면 될까?

	if (Cur_Obj == nullptr && Cur_Arr == nullptr){
		return;
	}

	//2가지로 나누기

	if (Cur_Obj != nullptr){
		if (jnode->isTypeNull()){
			//jnode->P_Type == nullptr인 경우
			Cur_Obj->Value->delType();
			Cur_Obj->Value->setType(JNode::JType::NULLTYPE);
		}
		else{
			Cur_Obj->Value->delType();
			Cur_Obj->Value->Cur_Type = jnode->Cur_Type;
			Cur_Obj->Value->P_Type = jnode->P_Type;
			Cur_Obj->Value->ObjCnt = jnode->ObjCnt;
			Cur_Obj->Value->ArrCnt = jnode->ArrCnt;

			//이중 소멸을 방지하기 위한 jnode의 P_Type을 참조해제
			jnode->P_Type = nullptr;
			jnode->delType();
			jnode->setType(JNode::JType::NULLTYPE);
		}
	}

	if (Cur_Arr != nullptr){
		if (jnode->isTypeNull()){
			//jnode->P_Type == nullptr인 경우
			Cur_Arr->Value->delType();
			Cur_Arr->Value->setType(JNode::JType::NULLTYPE);
		}
		else{
			Cur_Arr->Value->delType();
			Cur_Arr->Value->Cur_Type = jnode->Cur_Type;
			Cur_Arr->Value->P_Type = jnode->P_Type;
			Cur_Arr->Value->ObjCnt = jnode->ObjCnt;
			Cur_Arr->Value->ArrCnt = jnode->ArrCnt;

			//이중 소멸을 방지하기 위한 jnode의 P_Type을 참조해제
			jnode->P_Type = nullptr;
			jnode->delType();
			jnode->setType(JNode::JType::NULLTYPE);
		}
	}
}

void JsonCallObjArr::operator=(JNode::JType curType) {
	if (!Cur_Obj->Value->isTypeMatch(curType)) {
		//타입이 안맞으면 덮어쓸꺼니까 지워주셈
		Cur_Obj->Value->delType();
		Cur_Obj->Value->Cur_Type = curType;
		Cur_Obj->Value->setType(curType);
	}
}






//생성자 부분
JsonCallObjArr::JsonCallObjArr(JNode* node, JNode::JType curType, bool trg_del){
	if (trg_del){
		//del
		//1. Obj인지 Arr인지 판단
		if (curType == JNode::JType::ARR){
			arr_del(node);
		}
		else if (curType == JNode::JType::OBJ){
			obj_del(node);
		}
	}
	else{
		//push
		//1. Obj인지 Arr인지 판단
		if (curType == JNode::JType::ARR){
			arr_push(node);
		}
		else if (curType == JNode::JType::OBJ){
			obj_push(node);
		}
	}
}




// StrParser class ----------------------------------------------------------------------------------
Json_Func::MyUnion Json_Func::Val = { 0 };
