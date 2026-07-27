#include <assert.h>
#include <sstream>
#include "qna_tool.h"


class minheapnode{
public:
    int bookcode=0;
    int pagno=0;
    int parno=0;
    long double frac=0;
    minheapnode(int b,int p,int pr,long double f){
        bookcode=b;
        pagno=p;
        parno=pr;
        frac=f;
    }
    minheapnode(){}
};

class minheap{
public:
    // vector<minheapnode> arr;
    vector<minheapnode> heap;
    int k;
    minheap(int k){
        heap.resize(k);
        this->k=k;
    }
    minheap(){}

    void swap(minheapnode &a, minheapnode &b) {
        minheapnode temp = a;
        a = b;
        b = temp;
    }

    int getParentIndex(int i) {
    return (i - 1) / 2;
    }
  
    int getLeftChildIndex(int i) {
        return 2 * i + 1;
    }
    
    int getRightChildIndex(int i) {
        return 2 * i + 2;
    }

    void heapifyUp(int index) {
    if (index == 0) return; // base condition for termination of a recursive invocation of the fnc
    
    int parentIndex = getParentIndex(index);
    
    if (heap[parentIndex].frac > heap[index].frac) {
      swap(heap[parentIndex], heap[index]);
      heapifyUp(parentIndex);
    }
  }
  void heapifyDown(int index) {
    int leftChild = getLeftChildIndex(index);
    int rightChild = getRightChildIndex(index);
    
    if (leftChild >= heap.size()) return; // No children
    
    int minindex = index;
    
    if (heap[minindex].frac > heap[leftChild].frac) {
      minindex = leftChild;
    }
    
    if (rightChild < heap.size() && heap[minindex].frac > heap[rightChild].frac) {
      minindex = rightChild;
    }
    
    if (minindex != index) {
      swap(heap[minindex], heap[index]);
      heapifyDown(minindex);
    }
  }
    minheapnode minElem(){
    return heap[0];
  }

  minheapnode deleteMin() {
    // if (heap.empty()) {
    //   cout << "Heap is empty!" << endl;
    //   return 0;
    // }
    minheapnode temp=heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    heapifyDown(0);
    return temp;

  }

  void insert(minheapnode val) {
    // if (heap.size()>k){
        if (heap[0].frac<val.frac){
        heap[0]=val;
        heapifyDown(0);
    }
    // }
    // else{
    // heap.push_back(val); //satisfies the structural prop
    // heapifyUp(heap.size() - 1);
    // }
  }

// void printHeap (){
//     for (const auto &elem : heap) {
//       cout << elem.frac << " ";
//     }
//     cout << endl;
//   }

  void makeheapempty(){
    heap.clear();
  }
//   void insertt(minheapnode val){
//     if (heap[0].frac<val.frac){
//         heap[0]=val;
//         heapifyDown(0);
//     }
//   }

};

using namespace std;
class paranode{
    public:
    long double frac;
    paranode(){
        frac=0;
    }
};

class pagenode{
    public:
    vector<paranode*> vec;
    pagenode(int paramax){
        vec.resize(paramax,nullptr);
    }
};

class booknode{
    public:
    vector<pagenode*> vec;
    booknode(int pagamax){
        vec.resize(pagamax,nullptr);
    }
};

class hashnode{
    public:
    vector<booknode*> vec;
    hashnode(int bookmax){
        vec.resize(bookmax,nullptr);
    }
};

class triehash{
    public:
    hashnode* root;
    minheap minhp;
    int bkmax;
    int pgmax;
    int prmax;
    triehash(int bookmax,int pagemax,int paramax,int k){
        root=new hashnode(bookmax);
        bkmax=bookmax;
        pgmax=pagemax;
        prmax=paramax;
        minhp=minheap(k);
    }
    void insert(word* wordlist){
        if (wordlist==nullptr) return;
        if (wordlist->head==nullptr) return;
        wordnode* temp=wordlist->head->next;
        long double frac=(static_cast<long double>(wordlist->corpuscount+1))/(wordlist->csvcount+1);
        while (temp!=wordlist->tail){
            int bkcode=temp->bookcode;
            int pgno=temp->pageno;
            int parno=temp->parano;
            if (!root->vec[bkcode]) root->vec[bkcode]=new booknode(pgmax);
            if (!root->vec[bkcode]->vec[pgno]) root->vec[bkcode]->vec[pgno]=new pagenode(prmax);
            if (!root->vec[bkcode]->vec[pgno]->vec[parno]) root->vec[bkcode]->vec[pgno]->vec[parno]=new paranode();
            root->vec[bkcode]->vec[pgno]->vec[parno]->frac+=frac;
            temp=temp->next;
        }
    }

    void heapinsert(){
        for (int i=0;i<bkmax;i++){
            if (root->vec[i]){
                for (int j=0;j<pgmax;j++){
                    if (root->vec[i]->vec[j]){
                        for (int k=0;k<prmax;k++){
                            if (root->vec[i]->vec[j]->vec[k]){
                                minhp.insert(minheapnode(i,j,k,root->vec[i]->vec[j]->vec[k]->frac));
                            }
                        }
                    }
                }
            }
        }
    }
    Node* givelargestk(int k){
        Node* temp=nullptr;
        // Node*head=nullptr;
        for (int i=0;i<k;i++){
            minheapnode m=minhp.deleteMin();
            if (temp==nullptr){
                temp=new Node(m.bookcode,m.pagno,m.parno,0,0);
                // temp=head;
            }
            else {
                Node* n=new Node(m.bookcode,m.pagno,m.parno,0,0);
                temp->left=n;
                n->right=temp;
                temp=n;
            }
        }
        // temp->right=nullptr;
        // head->left=nullptr;
        return temp;
    }


};

string tolower(string s){
    string d="";
    for (char i:s){
        int k=i-'A';
        if (k>=0 && k<=25) i=i+32;
        d+=i;
    }
    return d;
}

vector<string> getmyword(string sentence){
    int n=sentence.length();
    vector<string> v;
    string s="";
    string sp=" .,-:!\"\'()?[];@";
    for (char i:sentence){
        bool flag=false;
        for (char c:sp){
            if (c==i){
                flag=true;
                if (s!="") {
                    s=tolower(s);
                    v.push_back(s);
                }
                s="";
                break;
            }
        }
        if (!flag)  s+=i;
    }
    if (s!=""){
        s=tolower(s);
        v.push_back(s);
    }
    return v;

}



QNA_tool::QNA_tool(){
    // Implement your function here
    Dict dict;
    bmax=0;
    pgmaxx=0;
    prmaxx=0;
    
}

QNA_tool::~QNA_tool(){
    // Implement your function here
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    // Implement your function here
    dict.insert_sentence(book_code, page, paragraph, sentence_no,sentence);
    pgmaxx=max(page,pgmaxx);
    bmax=max(bmax,book_code);
    prmaxx=max(prmaxx,paragraph);
    // searchengine.insert_sentence(book_code,page,paragraph,sentence_no,sentence);
    return;
}

Node* QNA_tool::get_top_k_para(string question, int k) {
    // Implement your function here
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k);
    for (string i:qvec){
        updatehash->insert(dict.get_word_count(i));
    }
    updatehash->heapinsert();
    return updatehash->givelargestk(k);
}

Node* QNA_tool::get_top_k_modified_para(string question,int k){
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k);
    for (string i:qvec){
        if (!dict.is_unwanted(i)){
        updatehash->insert(dict.get_word_count(i));
        }
    }
    updatehash->heapinsert();
    return updatehash->givelargestk(k);
}

void QNA_tool::query(string question, string filename){
    // Implement your function here
    std::cout << "Q: " << question << std::endl;
    std::cout << "A: " << "Studying COL106 :)" << std::endl;
    Node* root=get_top_k_modified_para(question,5);
    query_llm(filename,root,5,question);
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph){

    cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << endl;
    
    std::string filename = "mahatma-gandhi-collected-works-volume-";
    filename += to_string(book_code);
    filename += ".txt";

    std::ifstream inputFile(filename);

    std::string tuple;
    std::string sentence;

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }

    std::string res = "";

    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence)) {
        // Get a line in the sentence
        tuple += ')';

        int metadata[5];
        std::istringstream iss(tuple);

        // Temporary variables for parsing
        std::string token;

        // Ignore the first character (the opening parenthesis)
        iss.ignore(1);

        // Parse and convert the elements to integers
        int idx = 0;
        while (std::getline(iss, token, ',')) {
            // Trim leading and trailing white spaces
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
            }
            
            // Check if the element is a number or a string
            if (token[0] == '\'') {
                // Remove the single quotes and convert to integer
                int num = std::stoi(token.substr(1, token.length() - 2));
                metadata[idx] = num;
            } else {
                // Convert the element to integer
                int num = std::stoi(token);
                metadata[idx] = num;
            }
            idx++;
        }

        if(
            (metadata[0] == book_code) &&
            (metadata[1] == page) &&
            (metadata[2] == paragraph)
        ){
            res += sentence;
        }
    }

    inputFile.close();
    return res;
}

void QNA_tool::query_llm(string filename, Node* root, int k, string question){

    // first write the k paragraphs into different files

    Node* traverse = root;
    int num_paragraph = 0;

    while(num_paragraph < k){
        assert(traverse != nullptr);
        string p_file = "paragraph_";
        p_file += to_string(num_paragraph);
        p_file += ".txt";
        // delete the file if it exists
        remove(p_file.c_str());
        ofstream outfile(p_file);
        string paragraph = get_paragraph(traverse->book_code, traverse->page, traverse->paragraph);
        assert(paragraph != "$I$N$V$A$L$I$D$");
        outfile << paragraph;
        outfile.close();
        traverse = traverse->right;
        num_paragraph++;
    }

    // write the query to query.txt
    ofstream outfile("query.txt");
    outfile << "These are the excerpts from Mahatma Gandhi's books.\nOn the basis of this, ";
    outfile << question;
    // You can add anything here - show all your creativity and skills of using ChatGPT
    outfile.close();
 
    // you do not need to necessarily provide k paragraphs - can configure yourself

    // python3 <filename> num_paragraphs query.txt
    string command = "python ";
    command += filename;
    command += " ";
    command += to_string(k);
    command += " ";
    command += "query.txt";

    system(command.c_str());
    return;
}
// #undef int