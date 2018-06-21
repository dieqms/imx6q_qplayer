#ifndef _LIST_UTIL_H
#define _LIST_UTIL_H

#include<iostream>

namespace Base {

template<class T>
struct NodeList {
	T _data;
	NodeList<T>* _next;
	NodeList<T>* _prev;

	NodeList(const T& a)
	: _data(a) , _next(NULL), _prev(NULL) {
	}
};

template<class T, class ref, class ptr>
struct ListIterator
{
	typedef NodeList<T> Node;
	Node* _node;
	ListIterator(Node* node) : _node(node) {
	}
	ref operator*() {
		return _node->_data;
	}
	ptr operator->() {
		return &_node->_data;
	}
	ListIterator<T, ref, ptr>& operator++() {
		_node = _node->_next;
		return *this;
	}
	ListIterator<T, ref, ptr> operator++(int) {
		ListIterator<T, ref, ptr> tmp = (*this);
		_node = _node->_next;
		return tmp;
	}
	bool operator!=(const ListIterator<T, ref, ptr>& l1) {
		return this->_node != l1._node;
	}
	bool operator==(const ListIterator<T, ref, ptr>& l1) {
		return this->_node == l1._node;
	}
};

template<class T>
class List {
	typedef NodeList<T> Node;
private:
	Node* _head;
public:
	typedef ListIterator<T, T&, T*> Iterator;
	typedef ListIterator<T, const T&, const T*> ConstIterator;
public:
	List() : _head(new Node(T())) {
		_head->_next = _head;
		_head->_prev = _head;
	}
	void PushBack(const T& d = T()) {
		Node* tmp = new Node(d);
		Node* cur = _head->_prev;
		cur->_next = tmp;
		tmp->_next = _head;
		tmp->_prev = cur;
		_head->_prev = tmp;
	}

	void PopBack() {
		Node* tail = _head->_prev;
		Node* prev = tail->_prev;

		delete tail;
		prev->_next = _head;
		_head->_prev = prev;
	}

	Iterator Find(const T& d) {
		Iterator it = Begin();
		while (it != End()) {
			if ((*it) == d)
				return it;
			it++;
		}
		return End();
	}

	void Insert(Iterator pos, const T& d) {
		assert(pos._node);

		Node* Prev = pos._node->_prev;
		Node* Next = pos._node;
		Node* New = new Node(d);

		Prev->_next = New;
		New->_prev = Prev;
		New->_next = Next;
		Next->_prev = New;
	}

	void Erase(Iterator& pos) {
		assert(pos._node && pos != End());

		Node* Prev = pos._node->_prev;
		Node* Next = pos._node->_next;

		Prev->_next = Next;
		Next->_prev = Prev;

		delete pos._node;
		pos = Prev;
	}

	ConstIterator Begin() const {
		ConstIterator Bit(_head->_next);
		return Bit;
	}

	Iterator Begin() {
		Iterator Bit(_head->_next);
		return Bit;
	}

	ConstIterator End() const {
		ConstIterator Eit(_head);
		return Eit;
	}

	Iterator End() {
		Iterator Eit(_head);
		return Eit;
	}

	void Destory() {
		Iterator it = Begin();
		while (it != End()) {
			Node* tmp = it._node;
			it++;
			delete tmp;
		}
		_head->_next = _head;
		_head->_prev = _head;
	}

	~List() {
		Destory();
		_head = NULL;
	}
};

}

#endif
