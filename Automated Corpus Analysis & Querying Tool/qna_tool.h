#pragma once
#include <iostream>
#include <fstream>
#include "Node.h"
#include "dict.h"

using namespace std;

class QNA_tool {

private:

    // You are free to change the implementation of this function
    void query_llm(string filename, Node* root, int k, string question);
    // filename is the python file which will call ChatGPT API
    // root is the head of the linked list of paragraphs
    // k is the number of paragraphs (or the number of nodes in linked list)
    // question is the question asked by the user

    // You can add attributes/helper functions here
    Dict dict;
    int bmax;
    int pgmaxx;
    int prmaxx;
    int N;              // total unique paragraphs (denominator for IDF)
    int token_budget;   // max characters to send to LLM (context window budget)
    int lastBook, lastPage, lastPara; // for counting unique paragraphs

public:

    /* Please do not touch the attributes and
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    
    QNA_tool(); // Constructor
    ~QNA_tool(); // Destructor

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);
    // This function is similar to the functions in dict and search 
    // The corpus will be provided to you via this function
    // It will be called for each sentence in the corpus

    Node* get_top_k_para(string question, int k);
    // This function takes in a question, preprocess it
    // And returns a list of paragraphs which contain the question
    // In each Node, you must set: book_code, page, paragraph (other parameters won't be checked)

    std::string get_paragraph(int book_code, int page, int paragraph);
    // Given the book_code, page number, and the paragraph number, returns the string paragraph.
    // Searches through the corpus.

    void query(string question, string filename);
    // This function takes in a question and a filename.
    // It should write the final answer to the specified filename.

    /* -----------------------------------------*/
    /* Please do not touch the code above this line */
    Node* get_top_k_modified_para(string question, int k);

    // Configure the character budget sent to the LLM (default: 4000 chars)
    void set_token_budget(int budget);

    // Returns total unique paragraphs indexed (IDF denominator N)
    int get_N() const { return N; }
};