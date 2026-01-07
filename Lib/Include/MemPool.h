#pragma once

//고정 크기 메모리 풀
namespace Fxd_Mem{
	/*****************************************************************
	* API 클래스들
	*****************************************************************/
	class Fx_Mem_Func{
	protected:
		//메모리 내부 값을 초기화 해주는 영역
		virtual void Mem_Set(void* Data, size_t Val, size_t block_size){
			char* tmpData = static_cast<char*>(Data);
			for (size_t i = 0; i < block_size; i++){
				tmpData[i] = Val;
			}
		}

		virtual void Cst_Mem_Set(void* const Data, size_t Val, size_t block_size){
			char* tmpData = static_cast<char*>(Data);
			for (size_t i = 0; i < block_size; i++){
				tmpData[i] = Val;
			}
		}
	};

	class Fx_Mem_Size{
	protected:
		size_t block_size;
		size_t block_cnt;

		Fx_Mem_Size() : block_size(0), block_cnt(0){}
		Fx_Mem_Size(size_t block_size, size_t block_cnt) : block_size(block_size), block_cnt(block_cnt){}
	};

	class Fx_MemVal{
	public:
		size_t index;
		void* Data;
		bool is_free;

		Fx_MemVal(size_t block_size) : is_free(false){ Data = new char[block_size]; }
		Fx_MemVal() : Data(nullptr), is_free(false){}
		~Fx_MemVal(){
			if (Data != nullptr) {
				delete[] static_cast<char*>(Data);
				Data = nullptr;
			}
		}
	};



	/*****************************************************************
	* 실제 구현부 클래스들
	*****************************************************************/

	// ===============================
	// 기본적인 고정크기 메모리풀
	// 물리적인 하나의 큰 메모리영역을 여러명이 공유하는 구조
	// 주소만 알면 접근자체는 빠르지만(O(1)) 경계부분이 없어 침범의 여지가 존재
	// 주소모를시에는 여전히 O(n) 속도를 가짐 -> Free List 사용하면 해결되긴 함
	// ===============================

	class Fx_Mem_Mng_Dft : Fx_Mem_Func, Fx_Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			Values = new char[block_size * block_cnt];
			is_free = new bool[block_cnt];
			for (size_t i = 0; i < block_cnt; i++){
				is_free[i] = false;
			}
			Mem_Set(Values, 0, block_size*block_cnt);
		}
	public:
		void* Values;
		bool* is_free;	//사용여부

		//기본 타입 4,4
		Fx_Mem_Mng_Dft() : Fx_Mem_Size(4, 4){
			Init_Mem(this->block_size, this->block_cnt);
		}

		Fx_Mem_Mng_Dft(size_t block_size, size_t block_cnt) : Fx_Mem_Size(block_size, block_cnt){
			Init_Mem(this->block_size, this->block_cnt);
		}
		~Fx_Mem_Mng_Dft(){
			if (Values != nullptr){
				delete[] Values;
			}
			if (is_free != nullptr){
				delete[] is_free;
			}
		}

		//할당
		void* allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (is_free[i] == false){
					is_free[i] = true;
					return static_cast<void*>(static_cast<char*>(Values)+(i * block_size));
				}
			}
			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		//전체반환
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				is_free[i] = false;
				Mem_Set(static_cast<void*>(static_cast<char*>(Values)+(i * this->block_size)), 0, this->block_size);
			}
		}

		//한개만 반환
		void de_allocate(void* Data){
			if (Data < Values || Data >= static_cast<char*>(Values)+block_size * block_cnt){
				std::cout << "Invalid pointer" << std::endl;
				return;
			}
			//(반환데이터주소 - 첫번째주소) / 주소개수 = 반환데이터주소의 index를 알 수 있음
			size_t index = (static_cast<char*>(Data)-static_cast<char*>(Values)) / this->block_size;
			is_free[index] = false;
			Mem_Set(static_cast<void*>(static_cast<char*>(Values)+(index * this->block_size)), 0, this->block_size);
		}

	};


	// ===============================
	// 메모리 영역을 나누어서 객체로 관리하기 때문에 보안에 용이
	// 사용자는 자신의 메모리만 신경쓸 수 있어서 좋음
	// 메모리를 반환 혹은 할당해야하는 관리시점에 (개별객체) 오버헤드 발생가능성 있으며, 탐색시 O(n)을 가짐
	// ===============================
	class Fx_Mem_Mng_Sec : Fx_Mem_Func, Fx_Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			for (size_t i = 0; i < block_cnt; i++){
				Values[i].Data = new char[block_size];
				Values[i].is_free = false;
				Mem_Set(Values[i].Data, 0, block_size);
			}
		}
	public:
		Fx_MemVal* Values;



		//기본 블럭 4바이트
		Fx_Mem_Mng_Sec() : Fx_Mem_Size(4, 4){
			Values = new Fx_MemVal[block_cnt];
			Init_Mem(this->block_size, this->block_cnt);
		}

		Fx_Mem_Mng_Sec(size_t block_size, size_t block_cnt) : Fx_Mem_Size(block_size, block_cnt){
			Values = new Fx_MemVal[this->block_cnt];
			Init_Mem(this->block_size, this->block_cnt);
		}

		~Fx_Mem_Mng_Sec(){
			if (Values != nullptr) delete Values;
		}

		//할당
		void* allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (Values[i].is_free == false){
					Values[i].is_free = true;
					return Values[i].Data;
				}
			}

			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		//전체반환 free애들 전부 반환
		void de_allocate(){
			for (size_t i = 0; i < this->block_cnt; i++){
				Values[i].is_free = false;
				Mem_Set(Values[i].Data, 0, this->block_size);
			}
		}

		//한개만 반환
		void de_allocate(void* Data){
			for (size_t i = 0; i < this->block_cnt; i++){
				if (Values[i].Data == Data){
					Values[i].is_free = false;
					Mem_Set(Values[i].Data, 0, this->block_size);
					//delete로 운영체제를 호출해서 소멸하면 메모리 풀 쓰는 의미가 없어짐(속도가 떨어진다는 의미)
					//delete Values[i].Data;
					//Values[i].Data = nullptr;
					//Values[i].Data = new char[block_size];
					return;
				}
			}
		}


	};




	// ===============================
	// Free List 기반 O(1) 속도를 따르는 메모리 풀
	// bool 방식을 사용하지 않음
	// 메모리크기는 무조건 8바이트 이상이어야함(64비트 OS에서 주소가 8바이트 이상이기에)
	// Values를 next처럼 사용해야함
	// ===============================
	class Fx_Mem_Mng_Spd : public Fx_Mem_Func, Fx_Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			Values = new char[block_size*block_cnt];
			HeadPtr = Values;	//처음에는 시작점 다음에는 Values[0]이 가리키는 녀석

			for (size_t i = 0; i < block_cnt; i++){
				void* crnt_blk = (char*)Values + (i * block_size);

				void* nxt_blk = (char*)Values + ((i + 1) * block_size);

				*(void**)crnt_blk = nxt_blk;	//crnt_blk가 가리키는 녀석 Values[0]에 값에 nxt_blk를 넣겠다는 뜻
				//쉽게 풀어쓰기
				//void** test = &crnt_blk;
				//*test = nxt_blk;
				//*test -> *crnt_blk -> Values[0]을 가리킴
				//*test = nxt_blk;  -> Values[0] = nxt_blk(Values[1]) 이런개념
			}
			void* lst_blk = (char*)Values + (block_cnt * block_size);
			*(void**)lst_blk = nullptr;	//마지막값은 null이 되기
		}
	public:
		void* Values;
		void* HeadPtr;	//다음 비어있는 녀석을 가리키는 포인터

		Fx_Mem_Mng_Spd() :
			Fx_Mem_Size(8, 4){
			Init_Mem(block_size, block_cnt);
			Mem_Set(Values, 0, block_size);
		}
		Fx_Mem_Mng_Spd(size_t block_size, size_t block_cnt) :
			Fx_Mem_Size(block_size, block_cnt){
			Init_Mem(block_size, block_cnt);
			Mem_Set(Values, 0, block_size);
		}
		~Fx_Mem_Mng_Spd(){
			if (Values != nullptr) delete[] Values;
		}

		void* allocate(){
			if (HeadPtr != nullptr){
				void* all_blk = HeadPtr;

				HeadPtr = *(void**)HeadPtr;		//HeadPtr의 다음을 가리키기 내부적으로 연결되어 있어서 괜찮음
				*(void**)all_blk = nullptr;		//좀더 안전함 주소 유출이 없으니
				return all_blk;
			}

			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		void de_allocate(void* Data){
			if (Data == nullptr){
				std::cout << "Invalid pointer" << std::endl;
				return;
			}

			if (Data < Values || Data >= static_cast<char*>(Values)+block_size * block_cnt){
				std::cout << "Invalid pointer" << std::endl;
				return;
			}

			*(void**)Data = HeadPtr;

			HeadPtr = Data;
			//&Data = *Data = **Data  실주소->주소->값
			//*(void**)Data = 곧 값만 대입하기를 의미
			//next를 연결했다고 이해하면 됨
		}

		//전체 반환 (의미가 있나 싶긴 함)
		void de_allocate_all(){
			for (size_t i = 0; i < block_cnt; i++){
				void* crnt_blk = (char*)Values + (i * block_size);
				void* nxt_blk = (char*)Values + ((i + 1) * block_size);

				*(void**)crnt_blk = nxt_blk;
			}
			void* lst_blk = (char*)Values + (block_size*block_cnt);
			*(void**)lst_blk = nullptr;
		}

	};




	// ===============================
	// Stack 기반 고정크기 메모리 풀 (FILO)
	// 특수목적용이며 먼저 할당한 애를 해제하려면 맨뒤에 할당메모리 전부 해제해줘야함
	// ===============================
	class Fx_Mem_Mng_Stck : public Fx_Mem_Func, Fx_Mem_Size{
	private:
		void Init_Mem(size_t block_size, size_t block_cnt){
			//Values = new char[block_size * block_cnt];
			Stk_Ptr = Values;
			crnt_size = 0;
			Cst_Mem_Set(Values, 0, block_size);
		}
	public:
		void* const Values;
		void* Stk_Ptr;
		size_t crnt_size;	//현재 사이즈
		//size_t Head;  //기존 스택에 필요한 것들이 주소로써 컨트롤 되기에
		//size_t Tail;  //만들지 않아도 됨
		Fx_Mem_Mng_Stck() : Fx_Mem_Size(4, 4), Values(new char[block_size*block_cnt]){
			Init_Mem(block_size, block_cnt);
		}
		Fx_Mem_Mng_Stck(size_t blk_size, size_t blk_cnt) :
			Fx_Mem_Size(blk_size, blk_cnt), Values(new char[block_size*block_cnt]){
			Init_Mem(block_size, block_cnt);
		}
		~Fx_Mem_Mng_Stck(){
			if (Values != nullptr){
				delete[] Values;
			}
		}

		//할당
		void* allocate(){
			if (crnt_size < block_cnt){
				void* Ptr = Stk_Ptr;
				Stk_Ptr = (char*)Stk_Ptr + block_size;
				++crnt_size;
				return Ptr;
			}

			std::cout << "Memory Full" << std::endl;

			return nullptr;
		}

		//해제(단순히 마지막을 해제하기만 함)
		void de_allocate(){
			if (crnt_size == 0){
				std::cout << "Memory Empty" << std::endl;
				return;
			}

			Stk_Ptr = (char*)Stk_Ptr - block_size;
			--crnt_size;
			Mem_Set(Stk_Ptr, 0, block_size);
		}

	};


	// ===============================

	// ===============================

}