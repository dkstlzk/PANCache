# PANCache

PANCache is a Redis-inspired, high-performance in-memory cache engine with a live visualization dashboard. It is built from scratch in C++17 and designed to showcase systems-level architecture, data structures, and end-to-end integration between a backend cache engine and a frontend observability layer.

---

## Project Overview

PANCache implements a modular cache engine with TTL, LRU eviction, dependency graphs, prefix search, and top-k analytics. The backend exposes a simple HTTP API and a frontend dashboard renders the cache state in real time. The backend is the single source of truth; the frontend is a pure renderer of backend snapshots.

---

## Features

- Key-value storage with custom HashMap
- LRU eviction with eviction-aware cascade deletion
- TTL expiration via a min-heap
- Trie-based prefix search
- Bloom filter optimization for fast negative lookups
- Dependency graph with cascading deletes
- Top-K access analytics
- HTTP API with JSON state snapshots
- D3.js visualization dashboard

---

## System Architecture

```
                           +-------------------------------+
                           |           CacheEngine         |
                           |   orchestration / facade      |
                           +---------------+---------------+
                                           |
        +---------------+  +---------------+---------------+  +----------------+
        | HashMap       |  | LRU Cache     |               |  | TTL Heap       |
        | O(1) lookup   |  | MRU ordering  |               |  | expiry control |
        +---------------+  +---------------+               +----------------+
                |                    |                             |
                +--------------------+-----------------------------+
                                    |
                              +-----+-----+
                              | Trie      |
                              | Prefix    |
                              +-----+-----+
                                    |
                              +-----+-----+
                              | Graph     |
                              | Cascade   |
                              +-----------+

HTTP server: /cmd + /state + /health
Frontend: renders /state snapshots only
```

---

## Data Structures Used

| Data Structure | Role |
| --- | --- |
| HashMap | O(1) key-value storage for cache entries |
| LRU Cache | Tracks most/least recently used keys for eviction |
| TTL Heap | Expiration scheduling using a min-heap |
| Trie | Prefix search and autocomplete |
| Bloom Filter | Fast negative lookup to reduce HashMap misses |
| Dependency Graph | Cascading deletes across dependent keys |
| SkipList | Standalone sorted structure with tests |
| Top-K | Access frequency analytics |

---

## Why Each Data Structure Was Chosen

- HashMap provides O(1) average lookup and is the foundation of the cache.
- LRU ordering ensures capacity-based eviction is predictable and fast.
- TTL Heap is a natural fit for ordered expiration by time.
- Trie enables prefix search without scanning the entire keyspace.
- Bloom Filter reduces negative lookups when the key definitely does not exist.
- Dependency Graph models cascading key dependencies without hard coupling.
- SkipList is included as a standalone sorted structure (with tests) to demonstrate probabilistic indexing.
- Top-K analytics highlight frequently accessed keys with minimal overhead.

---

## Backend Architecture

- CacheEngine is the orchestration layer for all data structures.
- CommandParser parses and dispatches commands only; it does not own state.
- HttpServer is transport-only; no stdout hijacking or state logic.
- Serialization is centralized in a single export path.

---

## Frontend Visualization Architecture

- Frontend renders a single backend snapshot from `/state`.
- No independent cache simulation or TTL/LRU logic exists on the client.
- Rendering functions are isolated and reusable.
- Graph rendering uses D3.js with drag support.

---

## HTTP API

| Method | Endpoint | Description |
| --- | --- | --- |
| GET | /health | Liveness check |
| POST | /cmd | Execute a cache command (plain text body) |
| GET | /state | Full JSON snapshot for UI rendering |

Example:
```
POST /cmd
SET a 10
```

---

## Command Reference

| Command | Description |
| --- | --- |
| SET key value | Insert/update a key |
| GET key | Retrieve a value |
| DEL key | Delete a key (cascades) |
| LINK A B | Create dependency A -> B |
| EXPIRE key ttl | Set TTL in seconds |
| SIZE | Report size |
| PREFIX p | Prefix search |
| TOPK k | Top-K access analytics |
| CLEAR | Clear all keys |

---

## Build Instructions

```
make clean
make all -j8
```

## Run Instructions

```
./build/pancache_main
```

---

## Example Usage

```
SET user:1 Alice
SET user:2 Bob
GET user:1
LINK user:1 user:2
EXPIRE user:2 10
PREFIX user:
TOPK 3
CLEAR
```

---

## Screenshots / GIFs

- Dashboard screenshot: (add image here)
- Dependency graph demo: (add GIF here)

---

## Performance Notes

- HashMap and LRU operations are O(1) average.
- TTL heap uses a min-heap; expiry removal is ordered.
- Bloom filter can produce false positives but no false negatives.

---

## Future Improvements

- SSE/WebSocket updates instead of polling
- Background TTL cleanup tick
- Concurrency safety and lock-free paths
- Metrics and tracing for cache hit/miss and eviction

---

## Scalability Discussion

The current design is single-process and optimized for clarity. With a sharded HashMap, background expiration workers, and async event handling, the architecture can scale horizontally with minimal changes to the core abstractions.

---

## Project Structure

```
frontend/
include/
src/
tests/
Makefile
README.md
```

---

## Design Decisions

- Centralized CacheEngine orchestration prevents data structure drift.
- Stateless frontend rendering avoids client-side desynchronization.
- Explicit JSON export keeps observability consistent and debuggable.

---

## Technical Challenges

- Avoiding stale LRU/TTL state across cascading deletes
- Ensuring trie + bloom filter consistency on updates and deletes
- Rendering stable dependency graphs under rapid updates

---

## Resume / Interview Value

PANCache demonstrates systems design, data structure implementation, and full-stack integration. It is suitable for interviews that test architecture, performance, and correctness tradeoffs.

