#include <assert.h>
#include <sstream>
#include <cmath>
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
    paranode(){ frac=0; }
};

class pagenode{
public:
    vector<paranode*> vec;
    pagenode(int paramax){ vec.resize(paramax,nullptr); }
    ~pagenode(){ for (auto p : vec) delete p; }
};

class booknode{
public:
    vector<pagenode*> vec;
    booknode(int pagemax){ vec.resize(pagemax,nullptr); }
    ~booknode(){ for (auto p : vec) delete p; }
};

class hashnode{
public:
    vector<booknode*> vec;
    hashnode(int bookmax){ vec.resize(bookmax,nullptr); }
    ~hashnode(){ for (auto p : vec) delete p; }
};

// Tracks a scored paragraph reference
struct ParagraphKey { int book, page, para; };

class triehash{
public:
    hashnode* root;
    minheap minhp;
    int bkmax, pgmax, prmax;
    int N;                       // total unique paragraphs in corpus (for IDF)
    vector<ParagraphKey> touched; // paragraphs that received a score this query

    triehash(int bookmax, int pagemax, int paramax, int k, int n){
        root=new hashnode(bookmax);
        bkmax=bookmax; pgmax=pagemax; prmax=paramax;
        minhp=minheap(k);
        N=n;
    }
    ~triehash(){ delete root; }

    // Accumulate IDF-weighted TF scores for each posting of wordlist.
    // IDF(t) = log((N+1) / (df(t)+1))  — smoothed to avoid log(0)
    // Score for a paragraph accumulates one IDF unit per occurrence (= TF × IDF).
    void insert(word* wordlist){
        if (!wordlist || !wordlist->head || wordlist->df==0) return;

        long double idf = logl((long double)(N+1) / (wordlist->df+1));
        if (idf <= 0) return; // term too common to contribute signal

        wordnode* temp=wordlist->head->next;
        while (temp!=wordlist->tail){
            int bk=temp->bookcode, pg=temp->pageno, pr=temp->parano;
            if (!root->vec[bk]) root->vec[bk]=new booknode(pgmax);
            if (!root->vec[bk]->vec[pg]) root->vec[bk]->vec[pg]=new pagenode(prmax);
            bool is_new = (root->vec[bk]->vec[pg]->vec[pr]==nullptr);
            if (is_new){
                root->vec[bk]->vec[pg]->vec[pr]=new paranode();
                touched.push_back({bk,pg,pr});
            }
            root->vec[bk]->vec[pg]->vec[pr]->frac += idf; // one IDF unit per occurrence = TF×IDF
            temp=temp->next;
        }
    }

    // O(|touched|) — only visits paragraphs that were actually scored,
    // not the full B×P×Pa space. This is the key O(n×f) optimization.
    void heapinsert(){
        for (auto& pk : touched){
            long double sc = root->vec[pk.book]->vec[pk.page]->vec[pk.para]->frac;
            minhp.insert(minheapnode(pk.book,pk.page,pk.para,sc));
        }
    }

    // Extract top-k, skipping zero-scored placeholder nodes.
    // Results are returned head-first in descending score order.
    Node* givelargestk(int k){
        Node* head_node=nullptr;
        int extracted=0;
        while (extracted<k && !minhp.heap.empty()){
            minheapnode m=minhp.deleteMin();
            if (m.frac<=0) continue;
            Node* n=new Node(m.bookcode,m.pagno,m.parno,0,0);
            n->right=head_node;
            if (head_node) head_node->left=n;
            head_node=n;
            extracted++;
        }
        return head_node;
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
    bmax=0; pgmaxx=0; prmaxx=0;
    N=0; token_budget=4000;
    lastBook=-1; lastPage=-1; lastPara=-1;
}

QNA_tool::~QNA_tool(){
    // Implement your function here
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    dict.insert_sentence(book_code, page, paragraph, sentence_no, sentence);
    pgmaxx=max(page,pgmaxx);
    bmax=max(bmax,book_code);
    prmaxx=max(prmaxx,paragraph);
    // Count unique paragraphs for IDF denominator N
    if (book_code!=lastBook || page!=lastPage || paragraph!=lastPara){
        N++;
        lastBook=book_code; lastPage=page; lastPara=paragraph;
    }
    return;
}

void QNA_tool::set_token_budget(int budget){ token_budget=budget; }

// Retrieval complexity: O(n × |term| + Σ f_i) where n = query terms,
// |term| = trie lookup depth, f_i = posting list length for term i.
// The heapinsert step is O(Σ f_i) (visits only scored paragraphs via
// the touched list), NOT O(B × P × Pa) as in a naive scan.
// This is O(n × f) where f = average/max postings per query term.
Node* QNA_tool::get_top_k_para(string question, int k) {
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k,N);
    for (string& i:qvec){
        updatehash->insert(dict.get_word_count(i));
    }
    updatehash->heapinsert();
    Node* result=updatehash->givelargestk(k);
    delete updatehash;
    return result;
}

Node* QNA_tool::get_top_k_modified_para(string question,int k){
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k,N);
    for (string& i:qvec){
        if (!dict.is_unwanted(i)){
            updatehash->insert(dict.get_word_count(i));
        }
    }
    updatehash->heapinsert();
    Node* result=updatehash->givelargestk(k);
    delete updatehash;
    return result;
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
    // Select highest-ranked paragraphs that fit within the token budget.
    // This is the core LLM context-window problem: instead of dumping the
    // entire corpus into the prompt, we send only the top-ranked passages
    // that fit within token_budget characters.

    Node* traverse = root;
    int num_paragraph = 0;
    int total_chars = 0;

    vector<string> selected_paragraphs;

    while(traverse != nullptr && num_paragraph < k){
        string paragraph = get_paragraph(traverse->book_code, traverse->page, traverse->paragraph);
        if (paragraph.empty()){
            traverse = traverse->right;
            num_paragraph++;
            continue;
        }
        // Stop adding paragraphs if budget would be exceeded
        if (total_chars + (int)paragraph.size() > token_budget) break;
        total_chars += (int)paragraph.size();
        selected_paragraphs.push_back(paragraph);
        traverse = traverse->right;
        num_paragraph++;
    }

    int actual_k = (int)selected_paragraphs.size();
    if (actual_k == 0){
        cout << "No relevant paragraphs found within token budget." << endl;
        return;
    }

    // Write selected paragraphs to files
    for(int i = 0; i < actual_k; i++){
        string p_file = "paragraph_" + to_string(i) + ".txt";
        remove(p_file.c_str());
        ofstream outfile(p_file);
        outfile << selected_paragraphs[i];
        outfile.close();
    }

    // Write query with context header
    ofstream outfile("query.txt");
    outfile << "These are the excerpts from Mahatma Gandhi's books.\nOn the basis of this, ";
    outfile << question;
    outfile.close();

    cout << "[Context budget: " << total_chars << "/" << token_budget
         << " chars, " << actual_k << " paragraphs selected]" << endl;

    string command = "python ";
    command += filename;
    command += " " + to_string(actual_k);
    command += " query.txt";
    system(command.c_str());
    return;
}
// #undef int