//#include "interface.hh"
#include <stdint.h>

typedef uint64_t Addre;

struct Node {
    Addre miss_addr;
    Node *next;
};

class List {
    private:
    Node *head, *tail;
    int length, max_length;
    public:
    List(int max){
        head=NULL;
	tail=NULL;
	length=0;
	max_length=max;
    };
    void create_node(Addre addr); // insert new node at end
    Addre next_miss(Addre miss);
};

void List::create_node(Addre addr){
    if(length<max_length){
        length++;
        Node *temp=new Node;
        temp->miss_addr=addr;
        temp->next=NULL;
        if(head==NULL){
            head=temp;
            tail=temp;
            temp=NULL;
        } else {
            tail->next=temp;
            tail=temp;
        }
    }
}

Addre List::next_miss(Addre miss){
    Node *temp=new Node;
    temp=head;
    while(temp->miss_addr!=miss){
        temp=temp->next;
	if(temp==NULL){
            return 0;
	}
    }
    temp=temp->next;
    return temp->miss_addr;
}
