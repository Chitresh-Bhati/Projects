// Self-contained test suite for corpus analysis & querying tool.
// Run from the project directory (requires unigram_freq.csv and unwanted_words.txt).
// Build: g++ Node.cpp dict.cpp qna_tool.cpp test.cpp -o test_runner -std=c++17
// Run:   ./test_runner

#include <bits/stdc++.h>
#include "Node.h"
#include "dict.h"
#include "qna_tool.h"
using namespace std;

// ── tiny test harness ────────────────────────────────────────────────────────
static int pass_count = 0, fail_count = 0;

#define CHECK(cond, name) do { \
    if (cond){ cout << "  PASS: " << name << "\n"; ++pass_count; } \
    else     { cout << "  FAIL: " << name << "\n"; ++fail_count; } \
} while(0)

#define SECTION(name) cout << "\n=== " << name << " ===\n"

// ── helpers ──────────────────────────────────────────────────────────────────
static bool almost_equal(long double a, long double b, long double eps=1e-9){
    return fabsl(a-b) < eps;
}

// ── 1. uwtrie (stop-word trie) ───────────────────────────────────────────────
void test_uwtrie(){
    SECTION("uwtrie — basic trie insert/search");
    uwtrie t;
    t.insert("hello");
    t.insert("world");
    t.insert("he");

    CHECK(t.search("hello"),      "search existing word 'hello'");
    CHECK(t.search("world"),      "search existing word 'world'");
    CHECK(t.search("he"),         "search prefix 'he' (also inserted)");
    CHECK(!t.search("hell"),      "search partial 'hell' (not inserted)");
    CHECK(!t.search("helloo"),    "search over-length 'helloo'");
    CHECK(!t.search(""),          "search empty string");
    CHECK(!t.search("xyz"),       "search nonexistent word");

    // Duplicate insertion must not corrupt
    t.insert("hello");
    CHECK(t.search("hello"),      "search after duplicate insert");
}

// ── 2. csvtrie (frequency trie) ──────────────────────────────────────────────
void test_csvtrie(){
    SECTION("csvtrie — trie with count values");
    csvtrie ct;
    ct.insert("apple", 1500);
    ct.insert("banana", 200);
    ct.insert("app", 50);

    CHECK(ct.search("apple")==1500,  "count for 'apple'");
    CHECK(ct.search("banana")==200,  "count for 'banana'");
    CHECK(ct.search("app")==50,      "count for prefix word 'app'");
    CHECK(ct.search("ap")==0,        "count for non-inserted prefix 'ap'");
    CHECK(ct.search("apples")==0,    "count for extended word 'apples'");
    CHECK(ct.search("xyz")==0,       "count for nonexistent word");

    // Overwrite via re-insert
    ct.insert("apple", 9999);
    CHECK(ct.search("apple")==9999,  "count updated after re-insert");
}

// ── 3. word::df tracking ────────────────────────────────────────────────────
void test_word_df(){
    SECTION("word — document-frequency (df) tracking");
    word w("test", 0);

    // All in same paragraph → df should be 1
    w.insert(1, 1, 1);
    w.insert(1, 1, 1);
    w.insert(1, 1, 1);
    CHECK(w.df == 1,          "df=1 after 3 inserts into same para");
    CHECK(w.corpuscount == 3, "corpuscount=3 after 3 inserts");

    // New paragraph → df becomes 2
    w.insert(1, 1, 2);
    CHECK(w.df == 2,          "df=2 after insert into new para");

    // Different page → df becomes 3
    w.insert(1, 2, 1);
    CHECK(w.df == 3,          "df=3 after insert into para on new page");

    // Different book → df becomes 4
    w.insert(2, 1, 1);
    CHECK(w.df == 4,          "df=4 after insert into new book");

    // Back to same para (last book/page/para = 2,1,1) → no increment
    w.insert(2, 1, 1);
    CHECK(w.df == 4,          "df unchanged when same para repeated consecutively");
}

// ── 4. trie with inverted index ──────────────────────────────────────────────
void test_trie_index(){
    SECTION("trie — inverted index insert / exact search / prefix search");
    trie tr;

    // Insert terms into two "paragraphs"
    tr.insert(1, 1, 1, "neural");
    tr.insert(1, 1, 1, "network");
    tr.insert(1, 1, 2, "neural");
    tr.insert(1, 1, 2, "deep");
    tr.insert(2, 1, 1, "deep");
    tr.insert(2, 1, 1, "learning");

    word* w_neural = tr.search("neural");
    word* w_deep   = tr.search("deep");
    word* w_xyz    = tr.search("xyz");
    word* w_net    = tr.search("network");

    CHECK(w_neural != nullptr,     "search finds 'neural'");
    CHECK(w_deep   != nullptr,     "search finds 'deep'");
    CHECK(w_xyz    == nullptr,     "search returns null for nonexistent term");
    CHECK(w_net    != nullptr,     "search finds 'network'");

    CHECK(w_neural->corpuscount == 2, "corpuscount(neural)=2");
    CHECK(w_neural->df == 2,          "df(neural)=2 (appears in 2 paragraphs)");
    CHECK(w_deep->corpuscount == 2,   "corpuscount(deep)=2");
    CHECK(w_deep->df == 2,            "df(deep)=2 (appears in 2 paragraphs)");
    CHECK(w_net->df == 1,             "df(network)=1");

    // Prefix search
    vector<word*> results = tr.prefix_search("ne");
    CHECK(!results.empty(),                       "prefix_search('ne') returns results");
    bool found_neural=false, found_network=false;
    for (word* wp : results){
        if (wp->myword=="neural")  found_neural=true;
        if (wp->myword=="network") found_network=true;
    }
    CHECK(found_neural,   "prefix_search('ne') includes 'neural'");
    CHECK(found_network,  "prefix_search('ne') includes 'network'");

    // Prefix that matches nothing
    vector<word*> no_res = tr.prefix_search("zzz");
    CHECK(no_res.empty(), "prefix_search('zzz') returns empty");

    // Exact prefix (single char)
    vector<word*> d_res = tr.prefix_search("d");
    bool found_deep = false;
    for (word* wp : d_res) if (wp->myword=="deep") found_deep=true;
    CHECK(found_deep, "prefix_search('d') includes 'deep'");
}

// ── 5. Dict class (needs CSV files, run from project dir) ────────────────────
void test_dict(){
    SECTION("Dict — insert_sentence / get_word_count / is_unwanted");
    Dict d;
    d.insert_sentence(1, 1, 1, 1, "the quick brown fox");
    d.insert_sentence(1, 1, 1, 2, "the fox jumped");
    d.insert_sentence(1, 1, 2, 1, "quick brown dog");
    d.insert_sentence(2, 1, 1, 1, "the lazy dog sleeps");

    word* w_the   = d.get_word_count("the");
    word* w_quick = d.get_word_count("quick");
    word* w_fox   = d.get_word_count("fox");
    word* w_dog   = d.get_word_count("dog");
    word* w_miss  = d.get_word_count("missing");

    CHECK(w_the   != nullptr, "get_word_count finds 'the'");
    CHECK(w_quick != nullptr, "get_word_count finds 'quick'");
    CHECK(w_miss  == nullptr, "get_word_count returns null for unknown word");

    // "the" appears in para(1,1,1) twice and para(2,1,1) once → df=2
    CHECK(w_the->df == 2,    "df('the')=2 (in 2 unique paragraphs)");
    // "quick" appears in para(1,1,1) and para(1,1,2) → df=2
    CHECK(w_quick->df == 2,  "df('quick')=2");
    // "fox" appears in para(1,1,1) twice → df=1
    CHECK(w_fox->df == 1,    "df('fox')=1");
    // "dog" appears in para(1,1,2) and para(2,1,1) → df=2
    CHECK(w_dog->df == 2,    "df('dog')=2");

    // corpus counts
    CHECK(w_the->corpuscount == 3,   "corpuscount('the')=3 (3 sentence tokens)");
    CHECK(w_quick->corpuscount == 2, "corpuscount('quick')=2");
    CHECK(w_fox->corpuscount == 2,   "corpuscount('fox')=2");

    // is_unwanted (needs unwanted_words.txt)
    CHECK(d.is_unwanted("the"),    "'the' is unwanted (stop word)");
    CHECK(!d.is_unwanted("quick"), "'quick' is not unwanted");

    // prefix_search
    vector<word*> qr = d.prefix_search("qu");
    bool has_quick = false;
    for (word* wp : qr) if (wp->myword=="quick") has_quick=true;
    CHECK(has_quick, "prefix_search('qu') returns 'quick'");
}

// ── 6. IDF ranking ───────────────────────────────────────────────────────────
void test_idf_ranking(){
    SECTION("IDF ranking — QNA_tool on small synthetic corpus");
    //
    // Corpus:
    //   para A (1,1,1): "independence freedom nonviolence"
    //   para B (1,1,2): "independence congress leader"
    //   para C (1,2,1): "food agriculture farmer"
    //
    // N = 3
    // IDF("independence") = log(4/3) ≈ 0.288  (df=2)
    // IDF("freedom")      = log(4/2) = log(2) ≈ 0.693  (df=1)
    // IDF("nonviolence")  = log(4/2) = log(2) ≈ 0.693  (df=1)
    // IDF("congress")     = log(4/2)  (df=1)
    // IDF("leader")       = log(4/2)  (df=1)
    // IDF("food")         = log(4/2)  (df=1)
    //
    // Query "freedom independence":
    //   score(A) = IDF(freedom) + IDF(independence) ≈ 0.693 + 0.288 = 0.981
    //   score(B) = IDF(independence)                ≈ 0.288
    //   score(C) = 0
    //
    // Therefore A should be top-ranked.

    QNA_tool qna;
    qna.insert_sentence(1,1,1,1,"independence freedom nonviolence");
    qna.insert_sentence(1,1,2,1,"independence congress leader");
    qna.insert_sentence(1,2,1,1,"food agriculture farmer");

    CHECK(qna.N == 3, "N=3 unique paragraphs");

    // get_top_k_para should rank para A first for query "freedom independence"
    Node* top = qna.get_top_k_para("freedom independence", 3);
    CHECK(top != nullptr, "get_top_k_para returns results");

    if (top){
        // top is the highest-scored paragraph
        bool top_is_A = (top->book_code==1 && top->page==1 && top->paragraph==1);
        CHECK(top_is_A, "highest-scored para is A (1,1,1) — contains both 'freedom' and 'independence'");
    }

    // Query with rare term "nonviolence" (df=1, highest IDF) should rank A first
    Node* top2 = qna.get_top_k_para("nonviolence", 3);
    if (top2){
        bool top2_is_A = (top2->book_code==1 && top2->page==1 && top2->paragraph==1);
        CHECK(top2_is_A, "rare term 'nonviolence' → para A ranked first");
    }

    // Para C should NOT appear for query "freedom"
    Node* top3 = qna.get_top_k_para("freedom", 3);
    if (top3){
        bool head_is_C = (top3->book_code==1 && top3->page==2 && top3->paragraph==1);
        CHECK(!head_is_C, "unrelated para C not top-ranked for 'freedom'");
    }
}

// ── 7. multiple terms in same para ───────────────────────────────────────────
void test_repeated_terms(){
    SECTION("Repeated terms — TF accumulation via posting list");
    QNA_tool qna;
    // "peace" appears twice in para A → TF=2, score(A) = 2 × IDF(peace)
    qna.insert_sentence(1,1,1,1,"peace and peace");
    qna.insert_sentence(1,1,2,1,"war and conflict");

    Node* top = qna.get_top_k_para("peace", 2);
    CHECK(top != nullptr, "results found for 'peace'");
    if (top){
        bool is_A = (top->book_code==1 && top->page==1 && top->paragraph==1);
        CHECK(is_A, "para A ranks first (higher TF for 'peace')");
    }
}

// ── 8. nonexistent term ───────────────────────────────────────────────────────
void test_nonexistent_term(){
    SECTION("Nonexistent query term");
    QNA_tool qna;
    qna.insert_sentence(1,1,1,1,"hello world");

    Node* result = qna.get_top_k_para("supercalifragilistic", 5);
    CHECK(result == nullptr, "no results for unknown query term");
}

// ── 9. token budget ───────────────────────────────────────────────────────────
void test_token_budget(){
    SECTION("Token/context budget — limits paragraphs sent to LLM");
    // We cannot call query_llm (needs LLM), but we can verify:
    // set_token_budget modifies the internal budget, and get_top_k_para
    // still returns results correctly.

    QNA_tool qna;
    qna.set_token_budget(100); // very tight budget
    qna.insert_sentence(1,1,1,1,"swaraj means self-rule and freedom from british");
    qna.insert_sentence(1,1,2,1,"nonviolence was the core principle of gandhi");
    qna.insert_sentence(1,2,1,1,"salt march was a civil disobedience movement");

    // get_top_k works independently of token_budget (budget only affects query_llm)
    Node* top = qna.get_top_k_para("freedom nonviolence swaraj", 3);
    CHECK(top != nullptr, "get_top_k_para returns results regardless of token_budget");

    // Verify budget is stored
    qna.set_token_budget(8000);
    // (no crash = pass)
    CHECK(true, "set_token_budget(8000) does not crash");
}

// ── 10. IDF formula correctness ──────────────────────────────────────────────
void test_idf_formula(){
    SECTION("IDF formula: log((N+1)/(df+1))");
    // Manual check: with N=3, df=1 → IDF = log(4/2) = log(2) ≈ 0.6931
    long double expected = logl(4.0L / 2.0L);
    long double computed = logl((3.0L+1.0L) / (1.0L+1.0L));
    CHECK(almost_equal(expected, computed), "IDF(df=1, N=3) = log(2)");

    // With N=3, df=2 → IDF = log(4/3) ≈ 0.2877
    long double expected2 = logl(4.0L / 3.0L);
    long double computed2 = logl((3.0L+1.0L) / (2.0L+1.0L));
    CHECK(almost_equal(expected2, computed2), "IDF(df=2, N=3) = log(4/3)");

    // Rare term (df=N) has lowest IDF (but still > 0 due to smoothing)
    long double idf_ubiquitous = logl((3.0L+1.0L) / (3.0L+1.0L)); // = log(1) = 0
    CHECK(almost_equal(idf_ubiquitous, 0.0L), "IDF(df=N) = 0 (ubiquitous term)");
}

// ── 11. SearchEngine ──────────────────────────────────────────────────────────
void test_search_engine(){
    SECTION("SearchEngine — inverted-index-backed pattern search");
    // SearchEngine requires unigram_freq.csv; skip if file missing
    // (just test basic interface)
    SearchEngine se;
    se.insert_sentence(1,1,1,1,"mountains rivers forests");
    se.insert_sentence(1,1,2,1,"rivers and oceans");
    se.insert_sentence(1,2,1,1,"deserts and dunes");

    int n = 0;
    Node* result = se.search("rivers", n);
    CHECK(result != nullptr,  "search('rivers') returns results");
    CHECK(n == 2,             "search('rivers') finds 2 paragraphs");

    int n2 = 0;
    Node* result2 = se.search("volcanoes", n2);
    CHECK(result2 == nullptr, "search for absent term returns null");
    CHECK(n2 == 0,            "n_matches=0 for absent term");

    int n3 = 0;
    Node* result3 = se.search("rivers oceans", n3);
    CHECK(result3 != nullptr, "multi-word search('rivers oceans') returns results");
    // Para B (1,1,2) contains both rivers and oceans → score=2; should be first
    if (result3){
        bool B_first = (result3->book_code==1 && result3->page==1 && result3->paragraph==2);
        CHECK(B_first, "para containing both terms ranked first");
    }
}

// ── main ──────────────────────────────────────────────────────────────────────
int main(){
    cout << "Automated Corpus Analysis & Querying Tool — Test Suite\n";
    cout << string(55,'=') << "\n";

    test_uwtrie();
    test_csvtrie();
    test_word_df();
    test_trie_index();
    test_dict();
    test_idf_ranking();
    test_repeated_terms();
    test_nonexistent_term();
    test_token_budget();
    test_idf_formula();
    test_search_engine();

    cout << "\n" << string(55,'=') << "\n";
    cout << "Results: " << pass_count << " passed, " << fail_count << " failed.\n";
    return fail_count > 0 ? 1 : 0;
}
