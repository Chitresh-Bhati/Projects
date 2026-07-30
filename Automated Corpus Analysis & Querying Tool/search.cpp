// Do NOT add any other includes
#include "search.h"
#include <map>

static string se_tolower(string s){
    for (char& c : s){
        if (c>='A' && c<='Z') c += 32;
    }
    return s;
}

static vector<string> se_tokenize(const string& sentence){
    vector<string> v;
    string word;
    string sep = " .,-:!\"\'()?[];@";
    for (char ch : sentence){
        bool is_sep = false;
        for (char s : sep){ if (ch==s){ is_sep=true; break; } }
        if (is_sep){
            if (!word.empty()){ v.push_back(word); word=""; }
        } else {
            word += ch;
        }
    }
    if (!word.empty()) v.push_back(word);
    return v;
}

SearchEngine::SearchEngine(){}
SearchEngine::~SearchEngine(){}

void SearchEngine::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    dict.insert_sentence(book_code, page, paragraph, sentence_no, sentence);
}

// Retrieves paragraphs containing any of the pattern's terms.
// Score per paragraph = number of distinct query terms found in it.
// Returns linked list head (highest-scored first), sets n_matches.
Node* SearchEngine::search(string pattern, int& n_matches){
    vector<string> terms = se_tokenize(pattern);

    // Map (book,page,para) → hit count across query terms
    map<tuple<int,int,int>, int> para_score;

    for (string& t : terms){
        t = se_tolower(t);
        word* w = dict.get_word_count(t);
        if (!w) continue;

        // Walk posting list; count distinct paragraphs per term
        // (use a local set so repeated occurrences of same term in same para count once)
        map<tuple<int,int,int>,bool> seen;
        wordnode* node = w->head->next;
        while (node != w->tail){
            auto key = make_tuple(node->bookcode, node->pageno, node->parano);
            if (!seen[key]){
                para_score[key]++;
                seen[key] = true;
            }
            node = node->next;
        }
    }

    n_matches = (int)para_score.size();
    if (n_matches == 0) return nullptr;

    // Build linked list ordered by descending score (simple insertion sort into list)
    // Collect into vector, sort, then link
    vector<pair<int, tuple<int,int,int>>> scored;
    for (auto& kv : para_score)
        scored.push_back({kv.second, kv.first});
    sort(scored.begin(), scored.end(), [](auto& a, auto& b){ return a.first > b.first; });

    Node* head_node = nullptr;
    for (auto& sv : scored){
        auto [b,p,pa] = sv.second;
        Node* n = new Node(b, p, pa, 0, 0);
        n->right = head_node;
        if (head_node) head_node->left = n;
        head_node = n;
    }
    return head_node;
}
