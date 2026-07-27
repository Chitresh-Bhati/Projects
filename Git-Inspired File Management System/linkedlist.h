template<typename Key, typename Val>//we have to store both key and val for hashmap implementation. to perform search in a chain.
class LinkedList{
public:
    struct LLNode{
    public:
        Key key;
        Val val;
        LLNode* next;
        LLNode(Key key, Val val){
            this->key= key;
            this->val=val;
            next=nullptr;
        }
    };
    LLNode* head;
   

    
    LinkedList(){
        head=nullptr;
    }
    ~LinkedList() {
        // Free all nodes
        LLNode* temp = head;
        while (temp != nullptr) {
            LLNode* toDel =  temp;
            temp = temp->next;
            delete toDel;
        }
    }

    // Copy Assignment Operator (deep copy with cleanup)
    LinkedList& operator=(const LinkedList& other) {//PADHNA BAAKI HAI!!!
        if (this == &other) return *this; // self-assignment check

        // 1. Free current list
        LLNode* temp = head;
        while (temp != nullptr) {
            LLNode* toDel = temp;
            temp = temp->next;
            delete toDel;
        }
        head = nullptr;

        // 2. Copy from other
        if (other.head == nullptr) return *this;

        head = new LLNode(other.head->key, other.head->val);
        LLNode* curr = head;
        LLNode* otherCurr = other.head->next;

        while (otherCurr != nullptr) {
            curr->next = new LLNode(otherCurr->key, otherCurr->val);
            curr = curr->next;
            otherCurr = otherCurr->next;
        }

        return *this;
    }


    LLNode* returnhead(){
        return head;
    }
    void addHead(Key key, Val val){//just add at the head.
        LLNode* newnode = new LLNode(key,val);

        
        if(head==nullptr){
            head=newnode;
        }
        else{
            newnode->next=head;
            head=newnode;

        }
    }

    LLNode* findNode(Key k){
        LLNode* temp=head;
        while(temp!=nullptr){
            if(temp->key==k)return temp;
            temp=temp->next;
        }
        return nullptr;
    }
    void DeleteNode(Key k){
        LLNode* temp=head;
        if(head==nullptr)return;
        if(head->key==k){
            LLNode* todel=head;
            head=head->next;
            delete head;
            return;
        }
        while(temp->next!=nullptr){
            if(temp->next->key==k){
                LLNode* todel=temp->next;
                temp=todel->next;
                delete todel;
            }
            temp=temp->next;
        }

    }
    void print_first_k_keys(int k){
        LLNode* temp=head;
        while(k-- && temp!=nullptr){
            cout<<temp->key<<endl;
            temp=temp->next;
        }
    }
};