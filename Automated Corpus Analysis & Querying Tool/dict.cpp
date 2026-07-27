// Do NOT add any other includes
#include "dict.h"

string tolowerr(string s){
    string d="";
    for (char i:s){
        int k=i-'A';
        if (k>=0 && k<=25) i=i+32;
        d+=i;
    }
    return d;
}

vector<string> getword(string sentence){
    int n=sentence.length();
    vector<string> v;
    string s="";
    string sp=" .,-:!\"\'()?[];@";
    for (char i:sentence){
        bool flag=false;
        for (char c:sp){
            if (c==i){
                flag=true;
                if (s!="") v.push_back(s);
                s="";
                break;
            }
        }
        if (!flag)  s+=i;
    }
    if (s!="") v.push_back(s);
    return v;

}



Dict::Dict(){}

Dict::~Dict(){
    sr.~trie();    
}

void Dict::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    vector<string> v=getword(sentence);
    for (string i:v){
        i=tolowerr(i);
        sr.insert(book_code,page,paragraph,i);
    }
    return;
}

// int Dict::get_word_count(string wd){
//     word* temp=sr.search(wd);
//     return temp->corpuscount;
// }

word* Dict::get_word_count(string wd){
    return sr.search(wd);
}
void Dict::dump_dictionary(string filename){
    // sr.dump(filename); 
    return;
}
bool Dict::is_unwanted(string i){
    return sr.uwords->search(i);
}