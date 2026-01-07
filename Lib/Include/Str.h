#pragma once

#define DEEP_COPY_STR(for_init, for_if, for_In_De, lvalue, rvalue) \
	for(for_init; for_if; for_In_De){\
		lvalue[i] = rvalue[i]; \
											}

namespace Dynamic {
	class DynamicStr{
	private:
		unsigned char* Str;
		//내부 메소드 안에 내용을 지우기
		void Mem_Set(void* Data, char Val, size_t block_size){
			unsigned char* tmpData = static_cast<unsigned char*>(Data);
			for (size_t i = 0; i < block_size; i++){
				tmpData[i] = Val;
			}
		}

	public:
		//음수표현 제거
		inline static void ToUChar(unsigned const char*& lVal, const char* rVal){
			lVal = reinterpret_cast<const unsigned char*>(rVal);
		}
		//음수표현사용?(안쓸듯?)
		inline static void ToChar(const char*& lVal, const unsigned char* rVal){
			lVal = reinterpret_cast<const char*>(rVal);
		}

		int capacity_size;
		int current_size;	//마지막 '\0'까지의 값 ex) New Word\0 => 8
		int str_last_focus;	//마지막 문자까지의 값 ex) New Word]0 => 7
		//기본생성자로 호출하면 16비트정도만 가지게 됨
		DynamicStr() : capacity_size(16), current_size(-1), str_last_focus(-1) {
			Str = new unsigned char[capacity_size];
			Mem_Set(Str, '\0', capacity_size);
		}
		DynamicStr(int _size) : capacity_size(_size), current_size(-1), str_last_focus(-1) {
			Str = new unsigned char[capacity_size];
			Mem_Set(Str, '\0', capacity_size);
		}
		~DynamicStr() {
			if (Str != nullptr) {
				delete[] Str;
			}
		}

		//복사생성자 호출 (깊은복사)
		DynamicStr(const DynamicStr& other) : capacity_size(other.capacity_size),
			current_size(other.current_size), str_last_focus(other.str_last_focus) {
			Str = new unsigned char[capacity_size];
		}

		//이동생성자 호출 
		DynamicStr(const DynamicStr&& other) : capacity_size(other.capacity_size), current_size(other.current_size),
			str_last_focus(other.str_last_focus), Str(other.Str) {
		}

		bool StrCmp(const char* Subject) {
			const unsigned char* uSubject = nullptr;
			ToUChar(uSubject, Subject);
			for (int i = 0;; i++) {
				if (Str[i] != uSubject[i]) return false;
				if (i == current_size) {
					if (uSubject[i] != '\0') return false;
					return true;
				}
			}

			return true;
		}
		bool StrCmp(const unsigned char* Subject) {
			for (int i = 0;; i++) {
				if (Str[i] != Subject[i]) return false;
				if (i == current_size) {
					if (Subject[i] != '\0') return false;
					return true;
				}
			}

			return true;
		}
		static bool Raw_StrCpy(char* lVal, size_t lSize, char* rVal, size_t rSize, size_t cpy_size){
			if (cpy_size <= lSize || cpy_size <= rSize){
				return false;	//복사불가능
			}
			size_t i = 0;
			for (; i < cpy_size; i++){
				lVal[i] = rVal[i];
			}
			return true;
		}
		static bool Raw_StrCpy(unsigned char* lVal, size_t lSize, unsigned char* rVal, size_t rSize, size_t cpy_size){
			if (cpy_size > lSize || cpy_size > rSize){
				return false;	//복사불가능
			}
			size_t i = 0;
			for (; i < cpy_size; i++){
				lVal[i] = rVal[i];
			}
			return true;
		}
		static bool StrCpy(char* lVal, size_t lSize, char* rVal){
			if (!lVal || !rVal || lSize == 0)
				return false;

			size_t len = 0;

			while (rVal[len] != '\0')len++;

			if (len + 1 > lSize) return false;

			//실제복사
			for (size_t i = 0; i < len; i++){
				lVal[i] = rVal[i];
			}
			lVal[len] = '\0';

			return true;
		}
		static bool StrCpy(unsigned char* lVal, size_t lSize, unsigned char* rVal){
			if (!lVal || !rVal || lSize == 0)
				return false;

			size_t len = 0;

			while (rVal[len] != '\0')len++;

			if (len + 1 > lSize) return false;

			//실제복사
			for (size_t i = 0; i < len; i++){
				lVal[i] = rVal[i];
			}
			lVal[len] = '\0';

			return true;
		}


		static bool StrCmp_Org(const char* lVal, const char* rVal){
			const unsigned char* ulVal, *urVal = nullptr;
			ToUChar(ulVal, lVal);
			ToUChar(urVal, rVal);

			while (*ulVal && *urVal){
				if (*ulVal != *urVal){
					return false;
				}
				++ulVal;
				++urVal;
			}

			return *ulVal == *urVal;
		}
		static bool StrCmp_Org(const unsigned char* lVal, const unsigned char* rVal){
			while (*lVal && *rVal){
				if (*lVal != *rVal){
					return false;
				}
				++lVal;
				++rVal;
			}

			return *lVal == *rVal;
		}

		unsigned char* Get_uStr() {
			return Str;
		}

		char* Get_Str() {
			return reinterpret_cast<char*>(Str);
		}

		unsigned char UChar_Get_Str(int focus) {
			if (focus < 0 || focus >= current_size) {
				//예외처리
				return '\0';	//범위 벗어나면 Null 반환
			}
			return Str[focus];
		}

		char Char_Get_Str(int focus) {
			if (focus < 0 || focus >= current_size) {
				//예외처리
				return '\0';	//범위 벗어나면 Null 반환
			}
			return Str[focus];
		}

		/*char operator[](int focus) {
		return Str[focus];
		}*/

		//문자열 통으로 삽입하기
		void Set_Str(const char* new_str) {
			const unsigned char* unew_str = nullptr;
			ToUChar(unew_str, new_str);
			int i = 0;
			for (; unew_str[i] != '\0'; i++) {
				if (i >= capacity_size) {
					SizeUpStr();
				}
				Str[i] = unew_str[i];
				if (unew_str[i + 1] == '\0') {
					if (i + 1 >= capacity_size) {
						SizeUpStr();
					}
				}
			}

			Str[i] = unew_str[i];	// \0삽입
			current_size = i;
			str_last_focus = i - 1;

			//FitSizeStr();
		}
		void Set_Str(const unsigned char* new_str) {
			int i = 0;
			for (; new_str[i] != '\0'; i++) {
				if (i >= capacity_size) {
					SizeUpStr();
				}
				Str[i] = new_str[i];
				if (new_str[i + 1] == '\0') {
					if (i + 1 >= capacity_size) {
						SizeUpStr();
					}
				}
			}

			Str[i] = new_str[i];	// \0삽입
			current_size = i;
			str_last_focus = i - 1;

			//FitSizeStr();
		}

		void Set_Char(const char* new_char){
			const unsigned char* unew_char = nullptr;
			ToUChar(unew_char, new_char);

			int i = 0;
			if (i >= capacity_size || (capacity_size <= 1)){
				SizeUpStr();
			}

			Str[i] = unew_char[i++];
			Str[i] = '\0';
			current_size = i;
			str_last_focus = i - 1;
		}
		void Set_Char(const unsigned char* new_char){
			int i = 0;
			if (i >= capacity_size || (capacity_size <= 1)){
				SizeUpStr();
			}

			Str[i] = new_char[i++];
			Str[i] = '\0';
			current_size = i;
			str_last_focus = i - 1;
		}

		//문자 1개씩 추가
		void Append_Char(const char* new_char) {
			const unsigned char* unew_char = nullptr;
			ToUChar(unew_char, new_char);
			if (current_size + 1 >= capacity_size) {
				SizeUpStr();
			}
			if (current_size == -1)
				current_size++;

			Str[current_size++] = unew_char[0];
			Str[current_size] = '\0';
		}
		void Append_Char(const unsigned char* new_char){
			if (current_size + 1 >= capacity_size) {
				SizeUpStr();
			}
			if (current_size == -1)
				current_size++;

			Str[current_size++] = new_char[0];
			Str[current_size] = '\0';
		}

		//문자열을 뒤에 추가해주기
		void Append_Str(const char* new_str){
			const unsigned char* unew_str = nullptr;
			ToUChar(unew_str, new_str);
			int len = 0;
			while (unew_str[len] != '\0') len++;

			while (current_size + len >= capacity_size){
				SizeUpStr();
			}

			if (current_size == -1)
				current_size++;


			for (int i = 0; i < len; i++){
				Str[current_size++] = unew_str[i];
			}
			Str[current_size] = unew_str[len];
			//Str[current_size] = '\0';
			int debug = 10;
		}
		void Append_Str(const unsigned char* new_str){
			int len = 0;
			while (new_str[len] != '\0') len++;

			while (current_size + len >= capacity_size){
				SizeUpStr();
			}

			if (current_size == -1)
				current_size++;


			for (int i = 0; i < len; i++){
				Str[current_size++] = new_str[i];
			}
			Str[current_size] = new_str[len];
			//Str[current_size] = '\0';
			int debug = 10;
		}


		//맨앞글자를 지우기 앞으로 이동복사는 O(n) 시간복잡도 때문에 그냥 새롭게 버퍼 하나 파서 복사해줄 예정
		void Str_Trim_Front(){
			unsigned char* new_Str = new unsigned char[capacity_size];

			for (int idx = 0;; idx++){
				if (idx == capacity_size - 1){
					new_Str[idx] = Str[idx];		//그냥 마지막은 마지막 넣어주기 '\0' 이게 있을수도 있어서 해줘야할듯
					break;
				}
				new_Str[idx] = Str[idx + 1];		//앞글자 제외하고 넣기
			}
			current_size = current_size > -1 ? (current_size - 1) : -1;
			delete Str;
			Str = new_Str;
			new_Str = nullptr;
		}


		//맨뒤에 글자 지우기 이거는 그냥 앞뒤랑 복사붙여넣기 하면 될듯
		void Str_Trim_Back(){
			if (current_size >= capacity_size){
				SizeUpStr();
			}

			if (current_size <= 0){
				//지울게 없음 문자가 없으니까
				return;
			}
			current_size--;
			Str[current_size] = '\0';	//이론상 원래 '\0'일테지만 한번더 안전하게 넣어주기	
		}

		//앞뒤로 공백 \n \t 지워주기 전부
		void Str_Trim_All(){
			int fstIdx = 0;
			int lstIdx = current_size - 1;

			while ((Str[fstIdx] == ' ' || Str[fstIdx] == '\n' || Str[fstIdx] == '\t')
				|| (Str[lstIdx] == ' ' || Str[lstIdx] == '\n' || Str[lstIdx] == '\t')){
				if ((Str[fstIdx] == ' ' || Str[fstIdx] == '\n' || Str[fstIdx] == '\t')){
					//앞글자 지우기
					Str_Trim_Front();
					lstIdx = current_size - 1;
				}

				if ((Str[lstIdx] == ' ' || Str[lstIdx] == '\n' || Str[lstIdx] == '\t')){
					//뒤글자 지우기
					Str_Trim_Back();
					lstIdx = current_size - 1;
				}
			}
		}

		//앞뒤로 공백만 지워주기
		void Str_Trim(){
			int fstIdx = 0;
			int lstIdx = current_size - 1;

			while ((Str[fstIdx] == ' ')
				|| (Str[lstIdx] == ' ')){
				if ((Str[fstIdx] == ' ')){
					//앞글자 지우기
					Str_Trim_Front();
					lstIdx = current_size - 1;
				}

				if ((Str[lstIdx] == ' ')){
					//뒤글자 지우기
					Str_Trim_Back();
					lstIdx = current_size - 1;
				}
			}
		}

		//앞뒤로 " ' 지워주기
		void Str_Trim_Quote(){
			int fstIdx = 0;
			int lstIdx = current_size - 1;

			while ((Str[fstIdx] == '\'') || (Str[lstIdx] == '\'')
				|| (Str[fstIdx] == '\"') || (Str[fstIdx] == '\"')){
				if ((Str[fstIdx] == '\'') || (Str[fstIdx] == '\"')){
					//앞글자 지우기
					Str_Trim_Front();
					lstIdx = current_size - 1;
				}

				if ((Str[lstIdx] == '\'') || (Str[lstIdx] == '\"')){
					//뒤글자 지우기
					Str_Trim_Back();
					lstIdx = current_size - 1;
				}
			}
		}


		//내용자체를 지워버리기 Clear
		void Str_Clear(){
			current_size = -1;
			str_last_focus = 0;
			Mem_Set(Str, '\0', capacity_size);
		}
	private:
		//<문자열 처리 로직에 대한 API>

		//문자열 크기 증가 로직
		void SizeUpStr() {
			int old_capacity = capacity_size;
			capacity_size = (capacity_size / 2) > 2 ? (capacity_size / 2) + capacity_size : capacity_size + 2;
			unsigned char* new_size_str = new unsigned char[capacity_size + 1];		//Null포함위한 +1

			//Deep Copy (new_size_str <- Str)
			DEEP_COPY_STR(int i = 0, i < old_capacity, i++, new_size_str, Str);
			delete Str;

			Str = new_size_str;
		}

		//문자열 빈공간 제거 거의 사용하지 말것 오버헤드 발생
		void FitSizeStr() {
			int null_current_size = current_size + 1;			//Null 포함해서 넣어줘야함 +1
			unsigned char* new_size_str = new unsigned char[null_current_size];
			DEEP_COPY_STR(int i = 0, i < null_current_size, i++, new_size_str, Str);
			capacity_size = null_current_size;
			delete Str;
			Str = new_size_str;
		}
	};



}