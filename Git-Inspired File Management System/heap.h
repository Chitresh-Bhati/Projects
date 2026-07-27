#include<iostream>
#include<vector>
#include<string>
using namespace std;

class min_heap{
private:
    vector<pair<time_t,string>> v={{0,""}};//comparison using time_t.

public:
    void push(pair<time_t,string> entry){
        v.push_back(entry);
        int i=v.size()-1;
        while(i/2>0 && v[i/2]>v[i]){
            swap(v[i],v[i/2]);
            i=i/2;
        }
    }
    bool empty(){
        return v.size()<2;
    }
    void pop(){
        if(empty())return;
        v[1]=v[v.size()-1];
        v.pop_back();
        int i=1;
        while(i<v.size()){
            int largest=i;
            if(2*i<v.size() && v[2*i]<v[i])largest=2*i;
            if(2*i+1<v.size() && v[2*i+1]<v[i])largest=2*i+1;
            if(largest==i)break;
            swap(v[i],v[largest]);
            i=largest;
        }
    }
    string top(){
        if(empty())return "";
        return v[1].second;
    }
    int size(){
        return v.size()-1;
    }
    
};