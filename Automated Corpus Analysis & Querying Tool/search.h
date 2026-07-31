// Do NOT add any other includes
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Node.h"
#include "dict.h"
using namespace std;

class SearchEngine {
private:
    Dict dict;  // inverted index backend

public:
    /* Please do not touch the attributes and
    functions within the guard lines placed below  */
    /* ------------------------------------------- */
    SearchEngine();

    ~SearchEngine();

    void insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence);

    // Returns a linked list of Nodes for paragraphs containing pattern terms.
    // Paragraphs are ranked by how many query terms they contain.
    // n_matches is set to the total number of matching paragraphs.
    Node* search(string pattern, int& n_matches);

    /* -----------------------------------------*/
};