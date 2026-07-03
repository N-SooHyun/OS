#pragma once

/* =================================================================================
* LinkedList - 범용 이중 연결 리스트
* Xml 도메인과 무관한 순수 자료구조라서 별도 파일로 분리했다.
* 다른 프로젝트에서도 그대로 재사용할 수 있다.
===================================================================================*/

template<typename T>
class Node{
	T* Data;
	Node* next;
	Node* prev;
public:
	Node() : Data(nullptr), next(nullptr), prev(nullptr){}
	~Node() = default;
	template <typename U> friend class LinkedList;

	T* getData(){ return Data; }
	Node* getNextNode(){ return next; }
	Node* getPrevNode(){ return prev; }
};

template <typename T>
class LinkedList{
	Node<T>* Head;
	Node<T>* Tail;
public:
	int Index;

	LinkedList() : Head(nullptr), Tail(nullptr), Index(-1){}

	LinkedList(T* Root) : Index(0), Head(nullptr), Tail(nullptr){
		// 첫 노드를 만들면서 시작하는 경우
		Head = new Node < T > ;
		Head->Data = Root;
		Head->next = nullptr;
		Head->prev = nullptr;
		Tail = Head;
	}

	~LinkedList(){
		AllDelList();
	}

	void setNode(T* SetNode){
		if (SetNode == nullptr) return;
		AllDelList();
		AddList(SetNode);
	}

	void AllDelList(){
		Node<T>* cur = Head;
		while (cur != nullptr){
			Node<T>* next = cur->next;
			cur->next = nullptr;
			delete cur->Data;
			delete cur;
			cur = next;
		}
		Head = Tail = nullptr;
		Index = -1;
	}

	void AddList(T* AddNode){
		if (AddNode == nullptr) return;

		Node<T>* newNode = new Node<T>();
		newNode->Data = AddNode;
		newNode->next = nullptr;
		if (Tail != nullptr){
			newNode->prev = Tail;
			Tail->next = newNode;
			Tail = newNode;
		}
		else{
			newNode->prev = nullptr;
			Head = Tail = newNode;
		}
		Index++;
	}

	void AddList(T* AddNode, int idx){ // 사용자 인덱스 기준 1부터 시작 (0 없음)
		if (AddNode == nullptr || idx <= 0 || idx > Index + 1) return;

		// 맨 뒤에 추가하는 경우는 기존 로직 재활용
		if (idx == Index + 1 || Index == 0){
			AddList(AddNode);
			return;
		}
		Node<T>* newNode = new Node<T>();
		newNode->Data = AddNode;

		// 맨 앞에 삽입하는 경우
		if (idx == 1){
			newNode->next = Head;
			newNode->prev = nullptr;
			if (Head) Head->prev = newNode;
			Head = newNode;
		}
		// 중간에 삽입하는 경우
		else{
			Node<T>* target = Head;
			for (int i = 1; i < idx; i++){
				target = target->next;
			}
			newNode->prev = target->prev;
			newNode->next = target;
			if (target->prev) target->prev->next = newNode;
			target->prev = newNode;
		}
		Index++;
	}

	void DelList(){
		if (Tail == nullptr) return; // 리스트가 비어있음
		Node<T>* target = Tail;
		if (Tail->prev != nullptr){
			if (Tail->prev) Tail->prev->next = nullptr;
			Tail = Tail->prev;
			target->prev = nullptr;
			delete target;
			target = nullptr;
		}
		else{ // 노드가 1개인 경우
			delete Tail;
			Head = Tail = nullptr;
		}
		Index--;
	}

	void DelList(int idx){
		if (Tail == nullptr || idx <= 0 || idx > Index + 1) return;

		// 맨 뒤에서 제거하는 경우
		if (idx == Index + 1 || Index == 0){
			DelList();
			return;
		}
		Node<T>* target = Head;

		// 맨 앞에서 삭제하는 경우
		if (idx == 1){
			Head = Head->next;
			Head->prev = nullptr;
			target->next = nullptr;
			delete target;
			target = nullptr;
		}
		// 중간에서 삭제하는 경우
		else{
			for (int i = 1; i < idx; i++){
				target = target->next;
			}
			if (target->prev) target->prev->next = target->next;
			if (target->next) target->next->prev = target->prev;
			delete target;
			target = nullptr;
		}
		Index--;
	}

	Node<T>* getHead(){ return Head; }
	Node<T>* getTail(){ return Tail; }

	Node<T>* get(int idx){
		if (idx > Index || idx < 0){
			return nullptr;
		}
		if (Index == idx){
			return getTail();
		}
		Node<T>* target = Head;
		for (int i = 0; i < idx; i++){
			target = target->next;
		}
		return target;
	}
};
