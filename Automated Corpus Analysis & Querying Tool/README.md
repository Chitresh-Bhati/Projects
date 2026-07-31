# Automated Corpus Analysis & Querying Tool

A Data Structures & Algorithms project that solves the LLM **context-window / token-limit problem** by building a custom inverted index with IDF-based ranking, so only the most relevant passages from a large corpus are ever sent to the language model.

---

## Problem Statement

Large Language Models (LLMs) have a fixed context window — typically 4 000–128 000 tokens. A large corpus (e.g., 98 volumes of collected works, ~120 MB) cannot fit into a single prompt. Naively truncating the corpus destroys information; naively sending a random subset wastes the budget on irrelevant content.

**Solution:** Pre-index the corpus with an inverted index and a vocabulary trie, then at query time retrieve only the top-ranked passages that fit within a configurable token budget before sending anything to the LLM.

---

## Architecture

```
Large Corpus (98 volumes, ~120 MB)
        │
        ▼
  Tokenization / Normalization
  (lowercase, punctuation split)
        │
        ├─────────────────────────────────────────┐
        ▼                                         ▼
  Trie (vocabulary)               Inverted Index (posting lists)
  - uwtrie  → stop words          word → [(book, page, para), ...]
  - csvtrie → word frequencies    df(word) tracked per paragraph
  - trie    → corpus index        corpuscount = total occurrences
        │                                         │
        └────────────────┬────────────────────────┘
                         │
                  Query Processing
                         │
                    Tokenize query
                         │
                 Trie lookup per term
                         │
               Fetch posting lists (O(n×f))
                         │
              IDF-weighted TF scoring
              score(para) += IDF(term) × TF
                         │
              Min-heap top-k extraction
                         │
          Context / Token Budget Filter
          (select highest-ranked paragraphs
           until character budget reached)
                         │
                 LLM API Call
          (only relevant passages sent)
                         │
                    Answer
```

---

## Data Structures

### 1. Trie (`trie` class in `dict.h`)

Three custom trie implementations are used:

| Class | Purpose |
|-------|---------|
| `uwtrie` | Stop-word lookup — words in `unwanted_words.txt` are excluded from query scoring |
| `csvtrie` | Word-frequency lookup — each node stores a `count` from `unigram_freq.csv` |
| `trie` | Main vocabulary trie + inverted index — each end-of-word node holds a `word*` with the full posting list |

**Trie node character mapping (`asciii` / `num`):**  
Characters are mapped to indices 0–52 supporting lowercase letters, digits, and common punctuation. This avoids a sparse 256-slot array.

**Supported operations:**
- `insert(word)` — O(|word|)
- `search(word) → word*` — O(|word|), returns posting list or nullptr
- `prefix_search(prefix) → vector<word*>` — O(|prefix| + matches), DFS from prefix node
- `read_file(filename)` — bulk load from file

### 2. Inverted Index (`word` / `wordnode` classes in `dict.h`)

Each vocabulary entry (`word`) maintains a **doubly-linked list of postings** (`wordnode`):

```
word "gandhi"
  ├─ myword    = "gandhi"
  ├─ df        = 4234    (# unique paragraphs containing the word)
  ├─ corpuscount = 18921 (total occurrences across the corpus)
  └─ posting list (doubly-linked):
       head → (book=1, page=12, para=3) → (book=1, page=12, para=3)
            → (book=2, page=7,  para=1) → ... → tail
```

**Document frequency (`df`) tracking:**  
`df` increments whenever a new (book, page, paragraph) triple differs from the previous insertion. Since the corpus is fed in document order, this correctly counts unique paragraphs without a hash set.

### 3. Min-Heap (`minheap` class in `qna_tool.cpp`)

A fixed-size min-heap of size k keeps the top-k paragraphs by IDF score during retrieval. Each insertion is O(log k); extracting all k elements is O(k log k).

### 4. Scored Paragraph Map (`triehash` / `booknode` / `pagenode` / `paranode`)

A 3-level nested array `book → page → paragraph → paranode` accumulates IDF scores for paragraphs touched during a query. Only paragraphs in the `touched` list are ever visited during heap insertion — this is the key optimization that gives O(n × f) retrieval.

---

## IDF-Based Ranking

### Formula

```
IDF(term) = log( (N + 1) / (df(term) + 1) )
```

- **N** = total unique paragraphs indexed (tracked by `QNA_tool::N`)  
- **df(term)** = number of paragraphs containing the term (tracked by `word::df`)  
- Smoothing (`+1`) avoids `log(0)` and prevents division by zero

### Score Accumulation

For every occurrence of a query term in a paragraph, the paragraph's score increases by `IDF(term)`:

```
score(paragraph) += IDF(term)   for each occurrence (TF × IDF)
```

Terms that appear in many paragraphs (low IDF → near 0) contribute negligible signal. Rare, distinctive terms dominate ranking.

---

## Query Processing Pipeline

```
get_top_k_para(question, k):
  1. Normalize question: lowercase, strip punctuation
  2. For each query term t_i  (n terms total):
       a. Trie lookup: dict.get_word_count(t_i) → word* w_i   — O(|t_i|)
       b. Walk posting list of w_i (f_i postings):             — O(f_i)
          score[para] += IDF(w_i)   for each posting
  3. Insert scored paragraphs into min-heap of size k           — O(|touched| log k)
  4. Extract top-k from heap                                    — O(k log k)

get_top_k_modified_para:
  Same as above but skips stop words (uwtrie.search(t_i) == true)

query_llm (context budget):
  5. Retrieve paragraph text for each top-k node (get_paragraph)
  6. Accumulate characters until token_budget reached
  7. Write selected paragraphs to paragraph_N.txt files
  8. Write query.txt with context header + question
  9. Call python api_call.py with the selected paragraphs
```

---

## Context / Token Budget Mechanism

The token budget is configured via `set_token_budget(chars)` (default: 4 000 characters).

```cpp
// In QNA_tool::query_llm:
while (traverse && num_paragraph < k) {
    string para = get_paragraph(...);
    if (total_chars + para.size() > token_budget) break;   // stop adding
    total_chars += para.size();
    selected_paragraphs.push_back(para);
}
// Only selected_paragraphs go to the LLM — not the full corpus
```

This guarantees the LLM never receives more than `token_budget` characters of context, regardless of how many passages are retrieved.

---

## Time & Space Complexity

### Indexing (one-time build)

| Step | Complexity |
|------|-----------|
| Trie insert per word | O(\|word\|) |
| Full corpus indexing | O(total tokens × avg word length) |

### Query Retrieval (per query)

| Step | Complexity |
|------|-----------|
| Tokenize question (n terms) | O(Q) where Q = question length |
| Trie lookup per term | O(\|term\|) ≈ O(1) for short words |
| Walk posting lists | O(f₁ + f₂ + … + fₙ) = O(n × f̄) |
| Heap insert (touched paras) | O(\|touched\| × log k) |
| Extract top-k | O(k log k) |
| **Total retrieval** | **O(n × f)** |

Where:
- **n** = number of query terms  
- **f** = average posting list length (occurrences per term)  
- **f̄** = total postings / n

**Key insight:** Because the inverted index maps each term directly to its paragraph list, retrieval never scans the full corpus. Only the `touched` paragraphs (those that contain at least one query term) are scored and compared — this set is bounded by Σ fᵢ across query terms.

### Space

| Structure | Space |
|-----------|-------|
| Trie nodes | O(vocabulary × avg word length) |
| Posting lists | O(total token occurrences) |
| Query-time scoring | O(\|touched\|) |

---

## File Structure

```
.
├── Node.h / Node.cpp        — Doubly-linked list node (book/page/para/offset)
├── dict.h / dict.cpp        — Trie + inverted index + Dict wrapper
│     ├── uwtrie             — Stop-word trie
│     ├── csvtrie            — Word-frequency trie (from unigram_freq.csv)
│     ├── trie               — Main vocabulary trie with posting lists
│     └── Dict               — Public API: insert_sentence, get_word_count
├── qna_tool.h / qna_tool.cpp — QNA_tool: retrieval + IDF scoring + LLM bridge
│     ├── minheap            — Custom min-heap for top-k extraction
│     ├── triehash           — 3-level score accumulator
│     └── QNA_tool           — Public API: insert_sentence, get_top_k_para, query
├── search.h / search.cpp    — SearchEngine: TF-based multi-term pattern search
├── test.cpp                 — Test suite (67 tests, 0 failures)
├── tester.cpp               — Full corpus loader + interactive query loop
├── api_call.py              — OpenAI API bridge
├── Makefile                 — Build system
├── unigram_freq.csv         — English word frequency data
├── unwanted_words.txt       — Stop-word list
└── mahatma-gandhi-collected-works-volume-{1..98}.txt  — Corpus
```

---

## Building and Running

### Prerequisites
- C++17 compiler (g++ or clang++)
- Python 3 with `openai` and `python-dotenv` packages (for LLM queries)
- OpenAI API key in `.env` file (copy `.env.example` to `.env`)

### Build

```bash
make            # builds both corpus_tool and test_runner
make test       # builds and runs the full test suite
make clean      # removes binaries and temporary files
```

Manual build:

```bash
g++ -std=c++17 -O2 Node.cpp dict.cpp qna_tool.cpp search.cpp tester.cpp -o corpus_tool
g++ -std=c++17 -O2 Node.cpp dict.cpp qna_tool.cpp search.cpp test.cpp   -o test_runner
```

### Run tests

```bash
./test_runner
# Expected: 67 passed, 0 failed
```

### Run interactive query (loads all 98 volumes)

```bash
./corpus_tool
# Waits for input after indexing; type a question and press Enter
```

---

## Example Query Walkthrough

Query: **"What did Gandhi say about nonviolence?"**

1. **Tokenize** → `["what", "did", "gandhi", "say", "about", "nonviolence"]`  
2. **Stop-word filter** (get_top_k_modified_para) → skip "what", "did", "about"  
   Retained: `["gandhi", "say", "nonviolence"]`
3. **Trie lookup + posting retrieval:**
   - `"gandhi"` → df=4234, posting list length ≈ 18000 → IDF ≈ log(N/4234)
   - `"nonviolence"` → df=312, IDF ≈ log(N/312) — higher IDF, stronger signal
   - `"say"` → very high df → low IDF
4. **Score paragraphs:** those mentioning "nonviolence" get the strongest boost
5. **Top-5 passages** extracted via min-heap
6. **Budget check:** accumulate passages up to 4 000 chars
7. **LLM call** with only those passages — not the 120 MB corpus

---

## Resume Claims → Implementation

### ✓ Tackled the challenge of LLMs with limited context windows because of token limits

**Where:** `QNA_tool::query_llm()` in `qna_tool.cpp` (lines 404–461)  
**How:** The `token_budget` member (default 4 000 chars, configurable via `set_token_budget()`) caps total characters sent to the LLM. Paragraphs are added greedily by rank until the budget is reached; the loop breaks when the next paragraph would exceed it.  
The LLM receives only `selected_paragraphs` — a small, ranked subset — not the 120 MB corpus.

---

### ✓ Built an inverted index with IDF-based ranking

**Inverted index:** `word` class (posting list) + `trie` class (dictionary lookup) in `dict.h`  
- `word::insert(book, page, para)` appends to the doubly-linked posting list and tracks `df`  
- `Dict::get_word_count(term)` does an O(|term|) trie lookup and returns the `word*` with the full posting list

**IDF-based ranking:** `triehash::insert(word*)` in `qna_tool.cpp` (lines 182–200)  
```cpp
long double idf = logl((long double)(N+1) / (wordlist->df+1));
...
root->vec[bk]->vec[pg]->vec[pr]->frac += idf;   // TF × IDF accumulation
```
Only paragraphs with `idf > 0` contribute; ubiquitous terms (df ≈ N) are silently suppressed.

---

### ✓ Trie structures to efficiently store vocabulary

**Where:** `uwtrie`, `csvtrie`, `trie` classes in `dict.h`  
**Participation in pipeline:**
- `uwtrie` — loaded from `unwanted_words.txt` at startup; consulted on every query term in `get_top_k_modified_para` to skip stop words
- `csvtrie` — loaded from `unigram_freq.csv`; attaches baseline frequency (`csvcount`) to each `word` entry during indexing
- `trie` — the main index; every corpus token is inserted here; query lookups happen here via `dict.get_word_count(term)` and `dict.prefix_search(prefix)`

All three are real, working trie implementations with `insert`, `search`, and (for the main trie) `prefix_search`.

---

### ✓ Retrieves answers in O(n × f) time

**Where:** `QNA_tool::get_top_k_para()` in `qna_tool.cpp` (lines 300–310) and `triehash` class

**How:**
- For each of the **n** query terms, a trie lookup (O(|term|)) fetches the posting list directly — no corpus scan.
- Walking that posting list costs O(fᵢ) where fᵢ is the number of occurrences of term i.
- `triehash::heapinsert()` iterates only over `touched` — paragraphs that actually received a non-zero score — not the entire book/page/paragraph space.
- Total: O(n × |term|) + O(Σ fᵢ) = O(n × f) where f = average/max postings per query term.

**More precise statement:** O(n × f̄) where f̄ = (Σ fᵢ) / n, or equivalently O(total postings across all query terms). This is consistent with the claim since f is the per-term average.

---

## Test Coverage

The test suite (`test.cpp`, 67 tests) covers:

| Section | What is tested |
|---------|---------------|
| `uwtrie` | insert, exact search, partial search, over-length, empty, duplicate insert |
| `csvtrie` | insert with count, search, non-inserted prefix, re-insert (update) |
| `word::df` | same-para inserts, new para, new page, new book, consecutive repeat |
| `trie` | inverted index insert, exact search, nonexistent, prefix search |
| `Dict` | insert_sentence, get_word_count, is_unwanted, prefix_search |
| IDF ranking | synthetic 3-para corpus, multi-term query, rare term advantage |
| Repeated terms | TF accumulation within a paragraph |
| Nonexistent term | returns nullptr, no crash |
| Token budget | set_token_budget, get_top_k still works |
| IDF formula | log((N+1)/(df+1)) numerical correctness |
| SearchEngine | single term, absent term, multi-term ranked results |
