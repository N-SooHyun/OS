#pragma once

#define SHMSIZE 1024

namespace Test{

	class Shm{
	protected:
		void* shm;
		Shm(){
			shm = new char[SHMSIZE];
			char* initMem = static_cast<char*>(shm);
			for (size_t i = 0; i < SHMSIZE; i++){
				initMem[i] = '\0';
			}
		}
		~Shm(){ if (shm != nullptr) delete[] shm; }

		//데이터 넣기?
		bool setData(DynamicStr* Data, size_t strAddr){
			unsigned char* Mem = static_cast<unsigned char*>(shm);
			size_t j = 0;
			for (size_t i = strAddr; i < (size_t)Data->capacity_size+strAddr; i++, j++){
				Mem[i] = Data->Char_Get_Str(j);
			}
			return true;
		}

		//데이터 빼기
		void* getData(size_t strAddr, size_t capacity){
			unsigned char* Mem = static_cast<unsigned char*>(shm);
			unsigned char* lMem = new unsigned char[capacity];			
			size_t j = 0;
			for (size_t i = strAddr; i < capacity+strAddr; i++,j++){
				lMem[j] = Mem[i];
			}
			void* rst = lMem;
			return rst;
		}
	};

	//원형큐에 대한 접근 방식
	class FstLayer : public Shm{
		size_t HeadRd;		//나갈차례 읽어내는 부분
		size_t TailWt;		//들어갈차례 쓰는 부분
		int capacity;
		DynamicStr DataStr;
		size_t BufAddr;
		/* ================================================
		* 주소계산
		==================================================*/
		size_t getAddrMem(size_t tgCsr){
			//32문자열에 1024바이트공간이면 총 32개의 메모리 공간이 생김
			//0, 32, 64, 96, 128 ... 
			//HeadRd : 0~31 이어야함
			return tgCsr*capacity;
		}

	public:
		FstLayer() : Shm(), capacity(32), DataStr(capacity), HeadRd(0), TailWt(0){}
		~FstLayer() {}

		//TailWt는 쓰고 Tail++ 쓰고 Tail++ 순서이며 Tail++전에 Head-1 == Tail확인 후 ++하기
		//HeadRd는 읽고 Head++ 읽고 Head++ 순서이며 Head++전에 Head==Tail 확인 후 ++하기
		//Head==Tail(비었음)
		//Head-1 == Tail(찼음)

		//데이터 넣기(TailWt++)
		void SetStr(const char* Str, size_t strSize){
			if (strSize > 31){
				std::cout << "Str size over Failed" << std::endl;
				return;
			}
			//다 찼음 데이터 못넣음
			if (HeadRd - 1 == TailWt || (HeadRd == 0 && TailWt == capacity - 1)){
				std::cout << "Queue Full Failed" << std::endl;
				return;
			}
			TailWt = TailWt == capacity - 1 ? 0 : TailWt+1;

			DataStr.Set_Str(Str);

			BufAddr = getAddrMem(TailWt);	//공유메모리 주소 계산하기

			setData(&DataStr, BufAddr);		//공유메모리에 써주기
		}

		//데이터 빼기
		unsigned char* getStr(){
			//비었음
			if (HeadRd == TailWt){
				std::cout << "Queue Empty Failed" << std::endl;
				return '\0';
			}
			DynamicStr Str(capacity);
			void* pStr = nullptr;
			HeadRd = HeadRd == capacity - 1 ? 0 : HeadRd + 1;

			BufAddr = getAddrMem(HeadRd);	//공유메모리 주소 계산하기

			pStr = getData(BufAddr, capacity);

			//빼낸 데이터 복사해주기
			Str.Set_Str(static_cast<char*>(pStr));
			delete pStr;
			return Str.Get_uStr();
		}
	};
	




	//메인 함수 역할을 하는 녀석
	class TestMain{
	public:
		TestMain(){}
		~TestMain(){}

		//이중큐사용예정
		
		//참조방식, 복사방식 둘다 해볼것

		void TestFuncMain(){
			FstLayer queue;
			DynamicStr Str(32);
			Str.Set_Str("Test");
			queue.SetStr(Str.Get_Str(), (unsigned int)Str.current_size-1);
			unsigned char* a = queue.getStr();
			if (a != nullptr) Str.Set_Str(a);
			printf("%s\n", Str.Get_Str());
			//Str.Set_Str(queue.getStr());

			int debug = 10;
		}
	};
}