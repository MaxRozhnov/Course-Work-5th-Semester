#pragma once
#include "stdafx.h"
#include <dinput.h>
#include <sstream>

struct Node {
	GUID deviceInstance;
	Node *next;
};

class GuidList {
public:
	GuidList() {
		head = NULL;
		tail = NULL;
	}
	
	void Add(GUID data) {
		Node *temp = new Node;
		temp->deviceInstance = data;
		temp->next = NULL;
		if (head == NULL) {
			head = temp;
			tail = head;
			temp = NULL;
		}
		else {
			tail->next = temp;
			tail = temp;
		}
	}

	bool CheckPresence(GUID guid) {
		Node *current = new Node;
		current = head;

		while (current != NULL) {
			if (current->deviceInstance == guid) {
				return true;
			}
			current = current->next;
		}
		return false;
	}

	void Remove(GUID guid) {
		Node *current = new Node;
		current = head;
		Node *previous = NULL;
		while (current != NULL) {
			if (current->deviceInstance == guid) {
				if(previous != NULL){
					previous->next = current->next;
					current = NULL;
				}
				else {
					head = current->next;
					current = NULL;
				}
				return;
				
			}
			previous = current;
			current = current->next;
		}
	}

private:
	Node *head;
	Node *tail;
};

