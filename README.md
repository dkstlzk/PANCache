# PANCache (Portable Advanced IN-Memory Cache)

PANCache is a **high-performance in-memory key-value store** that demonstrates practical data-structures engineering: O(1) HashMap storage, TTL expiry via Min-Heap, LRU eviction via a doubly linked list, sorted queries using a SkipList/BST, and dependency invalidation via a Directed Acyclic Graph (DAG).

## Features
- O(1) key lookup (HashMap)
- TTL expiry (Min-Heap)
- LRU eviction (HashMap + Doubly Linked List)
- Sorted access (SkipList or balanced BST)
- Dependency DAG for cascading invalidation
- Top-K analytics via Heap/BST (optional)
- (Optional) Bloom filter for fast existence checks; persistence snapshot (AOF-like) for extra credit

## Core Commands / API
```text
SET key value [TTL_seconds]
GET key
DEL key
EXPIRE key seconds
RANGE key_start key_end
TOPK N
DEPEND keyA keyB
INFO
SAVE / LOAD
QUIT
```

## Architecture Modules
```text

```
