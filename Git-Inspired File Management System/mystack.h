template<typename val>
class mystack{
private:
    class Node{
    public:
        val data;
        Node* next;

    public:
        Node(val data1, Node* next1){
            data=data1;
            next=next1;
        }
    public:
        Node(val data1){
            //it next not given, automatically takes it as NULL.
            data=data1;
            next=nullptr;
        }
    };
    Node* topnode=nullptr;
    int stacksize=0;

public:
    void push(val n){
        //new element should be added at the start.
        Node* temp=new Node(n,topnode);
        topnode=temp;
        stacksize++;
    }
    val top(){
        if(stacksize==0)return val{};
        return topnode->data;
    }
    void pop(){
        if(stacksize==0)return;
        Node* temp=topnode;
        topnode=topnode->next;
        delete temp;
        stacksize--;

    }
    int size(){
        return stacksize;
    }
    bool empty(){
        return stacksize==0;
    }


};