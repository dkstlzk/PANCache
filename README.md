# ⚡ PANCache – High-Performance In-Memory Cache Engine

**PANCache** is a modular, multi-file in-memory cache engine built in C++ (or C), inspired by Redis and Memcached.
It supports **fast key–value storage**, **LRU eviction**, **TTL expiration**, **Trie-based prefix search**, **Bloom filter optimization**, **dependency graphs**, and a **command-line interface (CLI)**.

---

## 📝 Features

### Key-Value Storage

```cpp
set("name", "ron");
get("name"); // → "ron"
```

### LRU Eviction

Automatically removes the **Least Recently Used** keys when the cache is full.

### TTL Expiration

Keys can expire after a specified duration:

```cpp
set("token", "xyz", 10); // Expires in 10 seconds
```

### Prefix Search

Efficient autocompletion and analytics:

```cpp
prefix("us"); // → ["user1", "user2", "username"]
```

### Bloom Filter Optimization

Quickly checks if a key *might exist* before performing a HashMap lookup — reducing unnecessary searches.

### Dependency Graph

Allows cascading updates or deletions:

```
A → B → C
```

Changing A affects B and C automatically.

### Top-K Analytics

Tracks the most frequently accessed keys.

### Command-Line Interface (CLI)

Supports commands like:

```
SET key value
GET key
DEL key
PREFIX ab
```

---

## 🔍 Components & Why They Exist

| Component              | Purpose                                                      |
| ---------------------- | ------------------------------------------------------------ |
| **HashMap**            | O(1) key lookup                                              |
| **LRU Cache**          | Keeps frequently used keys in memory; evicts least-used keys |
| **TTL Heap**           | Min-heap for fast expiration handling                        |
| **Trie**               | Prefix search & autocomplete                                 |
| **Bloom Filter**       | Fast negative lookup, reduces unnecessary HashMap searches   |
| **Dependency Graph**   | Cascading updates/deletes between keys                       |
| **Logger / Analytics** | Tracks usage patterns and assists debugging                  |
| **CLI Parser**         | Converts user commands into cache operations                 |

---

## ⚙ How It Works

### 1️⃣ SET(key, value)

1. Add key to Bloom Filter
2. Insert key-value into HashMap
3. Update LRU cache
4. Insert key into Trie for prefix search
5. Add to TTL Heap if expiration is provided

### 2️⃣ GET(key)

1. Check Bloom Filter

   * If "definitely not exists" → return null
   * If "might exist" → continue
2. Lookup in HashMap
3. Update LRU status

### 3️⃣ PREFIX(prefix)

* Traverse Trie from root
* Collect all matching keys
* Return as list

### 4️⃣ TTL Expiration

* Min-heap stores `(expiry_time, key)`
* Remove expired keys efficiently when their TTL passes

### 5️⃣ Dependency Graph

* Example:

```
depend(A, B) // B depends on A
```

* Updating or deleting A triggers updates/deletes to B

---

## 📁 Project Structure

```
PANCACHE/
│
├─ frontend/
│ ├─ index.html
│ ├─ script.js
│ └─ styles.css
│
├─ include/
│ ├─ analytics/
│ │ └─ topk.hpp
│ ├─ cli/
│ │ └─ command_parser.hpp
│ ├─ data/
│ │ ├─ bloom_filter.hpp
│ │ ├─ cache_engine.hpp
│ │ ├─ hashmap.hpp
│ │ ├─ lru.hpp
│ │ ├─ skiplist.hpp
│ │ ├─ trie.hpp
│ │ └─ ttl_heap.hpp
│ ├─ depend/
│ │ └─ graph.hpp
│ └─ utils/
│ ├─ http_server.hpp
│ ├─ httplib.hpp
│ ├─ logger.hpp
│ └─ types.hpp
│
├─ src/
│ ├─ analytics/
│ │ └─ topk.cpp
│ ├─ cli/
│ │ └─ command_parser.cpp
│ ├─ data/
│ │ ├─ bloom_filter.cpp
│ │ ├─ cache_engine.cpp
│ │ ├─ hashmap.cpp
│ │ ├─ lru.cpp
│ │ ├─ trie.cpp
│ │ └─ ttl_heap.cpp
│ ├─ depend/
│ │ └─ graph.cpp
│ ├─ utils/
│ │ ├─ http_server.cpp
│ │ └─ logger.cpp
│ └─ main.cpp
│
├─ tests/
│ ├─ test_cache.cpp
│ ├─ test_cli.cpp
│ ├─ test_graph.cpp
│ ├─ test_hashmap.cpp
│ ├─ test_heap.cpp
│ ├─ test_integration.cpp
│ ├─ test_logger.cpp
│ ├─ test_lru.cpp
│ ├─ test_skiplist.cpp
│ ├─ test_topk.cpp
│ └─ test_trie.cpp
│
├─ Makefile
├─ README.md
└─ .gitignore
```

---

## 💡 Real-World Applications

| Feature          | Example Use Case                        |
| ---------------- | --------------------------------------- |
| HashMap          | Fast key storage (Redis)                |
| LRU Cache        | OS page replacement, Redis caching      |
| Trie             | Autocomplete search                     |
| Bloom Filter     | Cassandra, Redis, Big Data              |
| TTL Heap         | Job scheduling                          |
| Dependency Graph | Build systems (Bazel), React UI updates |
| Top-K Analytics  | Dashboard for most accessed items       |
| CLI Parser       | Interactive cache interface             |

---

## 🚀 How to Run

1. Clone the repository:

```bash
git clone <repo_link>
```

2. Build the project:

```bash
make
```

3. Run the CLI:

```bash
./pancache
```

4. Use commands:

```
SET key value
GET key
DEL key
LINK A B 
EXPIRE key ttl
SIZE
PREFIX ab
TOPK k
HELP
SEARCH
CLEAR
EXIT
```

---


## Architecture Modules

```text
                             ┌───────────────────────────────┐
                             │           PANCache            │
                             │        (Architecture)         │
                             └───────────────┬───────────────┘
                                             │
                                             ▼
                            ┌───────────────────────────────────┐
                            │           CacheEngine             │
                            │  (Main Orchestrator / Facade)     │
                            └───────────────┬───────────────────┘
                                            │
           ┌───────────────┬────────────────┼───────────────┬───────────────┐
           ▼               ▼                ▼               ▼               ▼
       ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
       │  HashMap    │ │    LRU      │ │  TTL Heap   │ │  SkipList   │ │    Trie     │
       │ O(1) lookup │ │ O(1) evict  │ │ TTL expiry  │ │ Sorted keys │ │Prefix search│
       └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
              │               │               │               │               │
              └───────┬───────┴───────┬───────┴───────┬───────┴───────┬───────┘
                      ▼               ▼               ▼               ▼
              ┌─────────────────────────────────────────────────────────┐
              │                       Graph (DAG)                       │
              │ Dependency Manager: parent → child relationships        │
              │ Handles cascading updates/deletes                       │
              └─────────────────────────────────────────────────────────┘
                                            │
                                            ▼
                                  ┌──────────────────┐
                                  │      Logger      │
                                  │ Console / File   │
                                  │ Debug / Info     │
                                  └──────────────────┘
                                            │
                                            ▼
                                  ┌──────────────────┐
                                  │  CommandParser   │
                                  │  CLI Interface   │
                                  └──────────────────┘
                                            │
                                            ▼
                               User enters commands:
                       SET / GET / DEL / EXPIRE / LINK / PREFIX

```

