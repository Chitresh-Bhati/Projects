// Do NOT add any other includes
#include <string> 
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;
/*.................................*/
class uwnode{
    public:
    bool endofword;
    uwnode* ary[53];
    uwnode(){
        endofword=false;
        for (int i=0;i<53;i++){
            ary[i]=nullptr;
        }
    }
    ~uwnode(){
    for (int i=0;i<53;i++){
            if (ary[i]) delete ary[i];
        }
    }
};

class uwtrie{
    public:
    uwnode* root;
    uwtrie(){
        root=new uwnode();
    }
    int asciii(char c){
        int k=c;
        if (k>=35 && k<=38) return k-35;
        else if (k==42 || k==43) return k-38;
        else if (k>=47 && k<=57) return k-41;
        else if (k>=60 && k<=63) return k-43;
        else if (k==92) return 21;
        else if (k>=94 && k<=122) return k-72;
        else if (k==124) return 51;
        else return 52;
    }
    int num(int k){
        if (k>=0 && k<=3) return k+35;
        else if (k==4 || k==5) return k+38;
        else if (k>=6 && k<=16) return k+41;
        else if (k>=17 && k<=20) return k+43;
        else if (k==21) return 92;
        else if (k>=22 && k<=50) return k+72;
        else if (k==51) return 124;
        else return 126;

    }
    void insert(string s) {
        uwnode* temp = root;
        for (int c=0;c<s.size();c++) {
            int k = asciii(s[c]);
            if (!temp->ary[k]) {
                temp->ary[k] = new uwnode();
            }
            temp = temp->ary[k];
        }
        temp->endofword = true;
    }
    bool search(string s) {
        uwnode* temp = root;
        for (char c : s) {
            int k = asciii(c);
            if (temp==nullptr || !temp->ary[k]) return false;
            temp = temp->ary[k];
        }
        return temp->endofword;
    }

    // long long stringToInt(const string& str) {
    //     long long result = 0;
    //     size_t i = 0;
    //     while (i < str.size() && isdigit(str[i])) {
    //         result = result * 10 + (str[i] - '0');
    //         ++i;
    //     }
    //     return result;
// }

    void read_file(string file_name){
        ifstream file(file_name);
        string word;
        // string f;
        // getline(file,f);
        while (file>>word) {
            // istringstream iss(line);
            // string word, countStr;
            // file>>f;
        // Tokenize the line using comma as a delimiter
            // getline(iss, word);
            // getline(iss, countStr, ',');

        // Convert count from string to integer
            // long long count = stringToInt(countStr);
            insert(word);
        }
        file.close();
    }
    uwnode* khtm(uwnode* &root){
        if (!root) return nullptr;
        for (int i=0;i<53;i++){
            root->ary[i]=khtm(root->ary[i]);
        }
        delete root;
        root=nullptr;
        return nullptr;
    }

    ~uwtrie(){
       root=khtm(root);
    }
};


class csvtrienode {
public:
    bool endofword;
    long long count;
    csvtrienode* arr[53];

    csvtrienode() : endofword(false) ,count(0) {
        for (int i=0;i<53;i++){
            arr[i]=nullptr;
        }
    }
    ~csvtrienode(){
        for (int i=0;i<53;i++){
            if (arr[i]) delete arr[i];
        }
    }
};
class csvtrie {
public:
    csvtrienode* root;
    csvtrie() : root(new csvtrienode()) {}

    int asciii(char c){
        int k=c;
        if (k>=35 && k<=38) return k-35;
        else if (k==42 || k==43) return k-38;
        else if (k>=47 && k<=57) return k-41;
        else if (k>=60 && k<=63) return k-43;
        else if (k==92) return 21;
        else if (k>=94 && k<=122) return k-72;
        else if (k==124) return 51;
        else return 52;
    }

    int num(int k){
        if (k>=0 && k<=3) return k+35;
        else if (k==4 || k==5) return k+38;
        else if (k>=6 && k<=16) return k+41;
        else if (k>=17 && k<=20) return k+43;
        else if (k==21) return 92;
        else if (k>=22 && k<=50) return k+72;
        else if (k==51) return 124;
        else return 126;

    }

    void insert(string s,long long count) {
        csvtrienode* temp = root;
        for (int c=0;c<s.size();c++) {
            int k = asciii(s[c]);
            if (!temp->arr[k]) {
                temp->arr[k] = new csvtrienode();
            }
            temp = temp->arr[k];
        }
        temp->endofword = true;
        temp->count = count;
    }
    long long search(string s) {
        csvtrienode* temp = root;
        for (char c : s) {
            int k = asciii(c);
            if (temp==nullptr || !temp->arr[k]) return 0;
            temp = temp->arr[k];
        }
        if (!temp->endofword) return 0;
        return temp->count;
    }

    long long stringToInt(const string& str) {
        long long result = 0;
        size_t i = 0;
        while (i < str.size() && isdigit(str[i])) {
            result = result * 10 + (str[i] - '0');
            ++i;
        }
        return result;
}

    void read_file(string file_name){
        ifstream file(file_name);
        string line;
        string f;
        getline(file,f);
        while (getline(file,line)) {
            istringstream iss(line);
            string word, countStr;

        // Tokenize the line using comma as a delimiter
            getline(iss, word, ',');
            getline(iss, countStr, ',');

        // Convert count from string to integer
            long long count = stringToInt(countStr);
            insert(word,count);
        }
        file.close();
    }
    csvtrienode* khtm(csvtrienode* &root){
        if (!root) return nullptr;
        for (int i=0;i<53;i++){
            root->arr[i]=khtm(root->arr[i]);
        }
        delete root;
        root=nullptr;
        return nullptr;
    }

    ~csvtrie(){
       root=khtm(root);
    }
};

/*.....................................*/



/*..........................................*/
class wordnode{
    public:
    int bookcode;
    int pageno;
    int parano;
    wordnode* next;
    wordnode* prev;
    wordnode(int book,int page,int par){
        bookcode=book;
        pageno=page;
        parano=par;
        next=nullptr;
        prev=nullptr;
    }
    wordnode(){
        bookcode=-1;
        parano=-1;
        pageno=-1;
        next=nullptr;
        prev=nullptr;
    }
};

class word{
    public:
    string myword;
    long long corpuscount;
    long long csvcount;
    wordnode* head;
    wordnode* tail;
    word(string mword,long long csvcou){
        corpuscount=0;
        csvcount=csvcou;
        myword=mword;
        head=new wordnode();
        tail=new wordnode();
        head->next=tail;
        tail->prev=head;
    }
    void insert(int book,int pg,int par){
        wordnode* n=new wordnode(book,pg,par);
        tail->prev->next=n;
        n->prev=tail->prev;
        tail->prev=n;
        n->next=tail;
        corpuscount++;
    }
    ~word(){
        wordnode* temp=head->next;
        while (temp!=tail){
            wordnode* newtemp=temp;
            temp=temp->next;
            delete newtemp;
        }
        delete tail;
        delete head;
    }

};

class trienode{
    public:
    bool endofword;
    word* wordlist;
    trienode* arr[53];
    trienode() : endofword(false) {
        wordlist=nullptr;
        for (int i=0;i<53;i++){
            arr[i]=nullptr;
        }
    }
    ~trienode(){
        for (int i=0;i<53;i++){
            if (arr[i]!=nullptr){
                if (wordlist!=nullptr){
                    delete wordlist;
                }
                delete arr[i];
            }
        }
    }
};

class trie{
    public:
    trienode* root;
    csvtrie* csv;
    uwtrie* uwords;
    trie(){
        root=new trienode();
        csv=new csvtrie();
        uwords=new uwtrie();
        csv->read_file("unigram_freq.csv");
        uwords->read_file("unwanted_words.txt");
    }

    int asciii(char c){
        int k=c;
        if (k>=35 && k<=38) return k-35;
        else if (k==42 || k==43) return k-38;
        else if (k>=47 && k<=57) return k-41;
        else if (k>=60 && k<=63) return k-43;
        else if (k==92) return 21;
        else if (k>=94 && k<=122) return k-72;
        else if (k==124) return 51;
        else return 52;
    }

    int num(int k){
        if (k>=0 && k<=3) return k+35;
        else if (k==4 || k==5) return k+38;
        else if (k>=6 && k<=16) return k+41;
        else if (k>=17 && k<=20) return k+43;
        else if (k==21) return 92;
        else if (k>=22 && k<=50) return k+72;
        else if (k==51) return 124;
        else return 126;

    }

    void insert(int book_code, int page, int paragraph,string s){
        trienode* temp=root;
        for (int c=0;c<s.size();c++) {
            int k = asciii(s[c]);
            if (!temp->arr[k]) {
                temp->arr[k] = new trienode();
            }
            temp = temp->arr[k];
        }
        if (temp->endofword){
            // temp->wordlist->corpuscount++;
            temp->wordlist->insert(book_code,page,paragraph);
        }
        else{
            temp->endofword=true;
            temp->wordlist=new word(s,csv->search(s));
            // temp->wordlist->corpuscount++;
            temp->wordlist->insert(book_code,page,paragraph);
        }
    }
    word* search(string s){
        trienode* temp = root;
        for (char c : s) {
            int k = asciii(c);
            if (temp==nullptr || !temp->arr[k]) return 0;
            temp = temp->arr[k];
        }
        if (!temp->endofword) return nullptr;
        return temp->wordlist;
    }
    trienode* khtm(trienode* &root){
        if (!root) return nullptr;
        for (int i=0;i<53;i++){
            root->arr[i]=khtm(root->arr[i]);
        }
        delete root;
        root=nullptr;
        return nullptr;
    }
    ~trie(){
       root=khtm(root);
    //    csv->~csvtrie();
    }
};

/*............................................*/



class Dict {
private:
    // You can add attributes/helper functions here
    trie sr;

public: 
    /* Please do not touch the attributes and 
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    Dict();

    ~Dict();

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);

    word* get_word_count(string wd);

    void dump_dictionary(string filename);

    /* -----------------------------------------*/
    bool is_unwanted(string i);
};