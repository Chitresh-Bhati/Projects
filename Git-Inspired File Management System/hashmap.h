#include <iostream>
#include <vector>
using namespace std;
#include "linkedlist.h"


template<typename Key,typename Val>
class hashmap{
  private:
    size_t initial_bucket_size=10;
    size_t bucketSize;
    size_t n_of_elements=0;
    vector<LinkedList<Key,Val>> buckets;
    size_t hashKey(const string& k) {//based on a bit-mixing trick (Thomas Wang’s integer hash)
        size_t hash = 5381; //large prime
        for(char c:k) {
            hash=((hash<<5)+hash)+c;
        }
        return hash;
    }
    size_t hashKey(int k) {//for integer
        // Mix the bits a little
        size_t hash=k;
        hash^=(hash>>20)^(hash>>12);
        hash^=(hash>>7)^(hash>>4);
        return hash;
    }



    size_t compress(size_t hashVal) {//compression function.
        return hashVal%bucketSize;
    }
    
    
  public:
    hashmap(){
        
        bucketSize=initial_bucket_size;
        buckets = vector<LinkedList<Key,Val>>(bucketSize);
        
    }
    ~hashmap(){}

    Val& operator[](const Key& k){//no copy, no modification of key!!
        size_t index=compress(hashKey(k));
        auto node=buckets[index].findNode(k);
        if(node!=nullptr){
            return (node->val);
        }
        else{//key not found
            if(4*n_of_elements>=3*bucketSize){//rehash (if load factor >= 0.75)
                
                vector<LinkedList<Key,Val>> oldBuckets=buckets;
                bucketSize=bucketSize*2;
                buckets=vector<LinkedList<Key,Val>>(bucketSize);
                n_of_elements=0;
                for(auto chain:oldBuckets){
                    auto temp=chain.returnhead();
                    while(temp != nullptr){
                        size_t newidx=compress(hashKey(temp->key));
                        buckets[newidx].addHead(temp->key,temp->val);
                        n_of_elements++;
                        temp=temp->next;
                    }
                }
                //~oldBuckets;//delete oldBuckets.
            }
            buckets[index].addHead(k,Val{}); //assign default value to key.(Val{} even works for pointer but Val() doesn't)
            n_of_elements++;
            return (buckets[index].findNode(k)->val); //return the value

        }
    }//we returned the value by refrence "Val&" so it can be modified.
    void erase(Key k){
        size_t index=compress(hashKey(k));
        auto node=buckets[index].DeleteNode(k);
    }
    vector<pair<Key,Val>> getAll(){
        vector<pair<Key,Val>> all;
        for(LinkedList<Key,Val>& ll:buckets){
            auto temp=ll.head;
            while(temp!=nullptr){
                all.push_back({temp->key,temp->val});
                temp=temp->next;
            }

        }
        return all;
    }
};

