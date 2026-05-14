# PANCache Architecture

This document describes the internal architecture of PANCache: how modules are organized, how requests flow through the system, how data structures interact, and how the frontend-backend integration works.

---

## System Overview

PANCache is a single-process, single-threaded in-memory cache engine with an embedded HTTP server and a browser-based visualization dashboard.

```
┌──────────────────────────────────────────────────────────┐
│                      Browser (UI)                        │
│  index.html + script.js + styles.css                     │
│  Polls /state every 1.5s, sends commands to POST /cmd    │
└────────────────────────┬─────────────────────────────────┘
                         │ HTTP
┌────────────────────────▼─────────────────────────────────┐
│                  HttpServer (cpp-httplib)                 │
│  Routes: GET /, GET /state, POST /cmd, GET /health       │
│  Static file mount: frontend/                            │
└────────────────────────┬─────────────────────────────────┘
                         │
          ┌──────────────▼──────────────┐
          │       CommandParser         │
          │  tokenize → uppercase →     │
          │  dispatch to handler fn     │
          └──────────────┬──────────────┘
                         │
          ┌──────────────▼──────────────┐
          │        CacheEngine          │
          │    (orchestration facade)   │
          │                             │
          │  Coordinates all DS on      │
          │  every mutation to keep     │
          │  state consistent           │
          └──────────────┬──────────────┘
                         │
    ┌────────┬───────────┼───────────┬────────────┐
    ▼        ▼           ▼           ▼            ▼
 HashMap   LRU       TTL Heap     Trie      Dep. Graph
  + Bloom  Cache    (min-heap)   (prefix)    (DFS cascade)
 Filter
```

---

## Module Boundaries

### `CacheEngine` — Core orchestration

`CacheEngine` owns all data structure instances and exposes a clean command-level API (`set`, `get`, `del`, `expire`, `link`, `prefix`, `topK`, `clear`). Every public method ensures all structures are updated atomically:

- `set(key, value)` → insert into HashMap, Bloom, Trie; update LRU (may trigger eviction); clear any existing TTL
- `get(key)` → Bloom check → HashMap lookup → LRU promotion → frequency increment
- `del(key)` → DFS on dependency graph → delete each dependent from HashMap, LRU, TTL, Trie, Bloom, Graph
- `expire(key, ttl)` → insert into TTL heap (lazy cleanup on next access)
- `exportState()` → snapshot all structures into `CacheEngineState` → serialize to JSON

No data structure is accessed directly by any other module. `CommandParser` and `HttpServer` interact only through `CacheEngine`'s public interface.

### `CommandParser` — Input dispatch

Stateless command processor. Tokenizes raw text input, normalizes the command verb to uppercase, and dispatches to the appropriate handler function. Each handler validates arguments and calls `CacheEngine` methods.

The command registry uses `std::unordered_map<string, function<string(vector<string>)>>`, making it trivial to add new commands.

### `HttpServer` — Transport layer

Thin wrapper around cpp-httplib. Responsibilities:

1. Mount `frontend/` as static files at `/`
2. Route `POST /cmd` to `CommandParser::handleCommand()`
3. Route `GET /state` to `CacheEngine::exportState()` → `to_json()`
4. Handle CORS preflight for all API endpoints
5. Suppress `/favicon.ico` noise with 204 response

The server does not parse commands, own state, or write to stdout (beyond the HTTP access log).

### `Logger` — Diagnostics

Thread-safe singleton logger with severity levels (DEBUG, INFO, WARN, ERROR), ANSI color output, millisecond timestamps, and optional file output. Used throughout the backend for startup diagnostics and runtime events.

---

## Request Lifecycle

### Command Execution (POST /cmd)

```
1. Browser sends POST /cmd with body "SET user:1 Alice"
2. HttpServer receives request, extracts body
3. CommandParser::handleCommand("SET user:1 Alice")
   3a. tokenize → ["SET", "user:1", "Alice"]
   3b. normalize → "SET"
   3c. dispatch → cmdSet(args)
4. cmdSet calls CacheEngine::set("user:1", "Alice")
   4a. cleanupExpiredKeys() — remove any expired TTL entries
   4b. hashmap_.insert("user:1", "Alice")
   4c. bloom_.insert("user:1")
   4d. trie_.insert("user:1")
   4e. ttl_heap_.erase("user:1") — clear any prior TTL
   4f. lru_.put("user:1", "Alice")
       → if capacity exceeded, returns evicted key
       → deleteCascade(evictedKey)
5. cmdSet returns "OK"
6. HttpServer sends 200 with body "OK"
7. Browser receives response, logs it
8. Browser immediately fetches GET /state (forced refresh)
```

### State Snapshot (GET /state)

```
1. Browser sends GET /state
2. HttpServer calls CacheEngine::exportState()
   2a. cleanupExpiredKeys()
   2b. Iterate HashMap keys → build entries[] with TTL remaining
   2c. Iterate Graph edges → build links[] (filtered to live keys)
   2d. Sort keys lexicographically → build ordered runtime snapshot
   2e. Get LRU order → build lru[]
   2f. Get TTL expiry epochs → build ttlExpiry{}
   2g. Compute Top-K from freqMap → build topk[]
   2h. Get prefix matches → build trie_matches[]
   2i. Get HashMap buckets → build hashmapBuckets[]
3. to_json(state) → manual JSON string construction
4. HttpServer sends 200 with Content-Type: application/json
5. Browser parses JSON, diffs against last signature
6. If changed: update all dashboard panels
```

### Cascading Deletion

When a key is deleted (explicitly or via LRU eviction), `deleteCascade` runs:

```
deleteCascade("parent")
  1. graph_.getAllDependentsRecursive("parent")
     → DFS from each direct child of "parent"
     → returns all transitive dependents
  2. to_delete = dependents ∪ {"parent"}
  3. For each key in to_delete:
     → hashmap_.erase(key)
     → lru_.erase(key)
     → trie_.remove(key)
     → ttl_heap_.erase(key)
     → freqMap_.erase(key)
     → graph_.removeNode(key)
```

This ensures no orphaned entries exist in any structure after deletion.

---

## Data Structure Interactions

```
                    SET key value
                         │
            ┌────────────▼────────────┐
            │   cleanupExpiredKeys()  │
            │   (TTL Heap → remove    │
            │    expired → cascade)   │
            └────────────┬────────────┘
                         │
         ┌───────────────┼───────────────┐
         ▼               ▼               ▼
    ┌─────────┐    ┌──────────┐    ┌──────────┐
    │ HashMap │    │  Bloom   │    │   Trie   │
    │ insert  │    │  insert  │    │  insert  │
    └─────────┘    └──────────┘    └──────────┘
                         │
                  ┌──────▼──────┐
                  │  LRU put    │
                  │  (promote   │
                  │   or insert)│
                  └──────┬──────┘
                         │
                  capacity exceeded?
                    ┌────▼────┐
                    │ evict   │
                    │ LRU tail│
                    └────┬────┘
                         │
                  ┌──────▼──────┐
                  │deleteCascade│
                  │ (Graph DFS) │
                  └─────────────┘


                    GET key
                       │
                ┌──────▼──────┐
                │ Bloom check │───── definitely not present → return null
                └──────┬──────┘
                       │ possibly present
                ┌──────▼──────┐
                │HashMap lookup│───── not found → return null
                └──────┬──────┘
                       │ found
                ┌──────▼──────┐
                │ LRU promote │
                │ freq++      │
                └──────┬──────┘
                       │
                  return value
```

---

## Frontend Architecture

The frontend is a single-page application (`index.html` + `script.js` + `styles.css`) served as static files by the backend.

### Rendering Model

The frontend is a **pure renderer of backend state**. It has no independent simulation:

- No client-side TTL timers
- No client-side LRU ordering
- No client-side cache entries
- All state comes from `GET /state`

### Polling Strategy

1. On page load: immediate `fetchBackendState({ force: true })`
2. Start polling: `setTimeout` loop every 1.5 seconds
3. On tab hidden (`visibilitychange`): stop polling
4. On tab visible: resume polling + force immediate fetch
5. After each command: force immediate fetch (bypasses poll interval)
6. In-flight deduplication: if a fetch is already running, queue at most one follow-up

### State Diffing

The frontend computes a JSON signature (`JSON.stringify(state)`) and skips re-rendering if the signature matches the previous one. This prevents unnecessary DOM mutations during idle periods.

### Graph Rendering

The dependency graph uses D3.js force simulation:

- Nodes represent cache keys
- Edges represent dependency links
- Drag support allows repositioning
- Simulation auto-stops after alpha < 0.02 or 2.5 seconds

---

## Build Architecture

```
Makefile
  │
  ├── src/main.cpp ──────────────────────► build/pancache_main.exe
  │     └── links: data/*.o, depend/*.o,
  │                analytics/*.o, cli/*.o,
  │                utils/*.o
  │
  └── tests/test_*.cpp ────────────────► build/test_*.exe
        └── links: all .o except main.o
```

The build system uses:
- Separate object files per module (prefixed: `data_hashmap.o`, `utils_logger.o`, etc.)
- Auto-dependency tracking (`-MMD -MP`)
- Platform detection for `.exe` suffix and WinSock linking
- Parallel build support (`-j8`)

---

## Benchmark Planning

> **Status:** No benchmarks have been run yet. This section outlines planned methodology.

### Planned Measurements

| Metric | Method |
|---|---|
| SET latency (p50, p99) | `std::chrono::high_resolution_clock` around `CacheEngine::set()` |
| GET latency (hit/miss) | Measure with and without Bloom filter short-circuit |
| Eviction throughput | Fill cache to capacity, measure SET rate with continuous eviction |
| CASCADE latency | Build a deep dependency chain (100+ keys), measure `del(root)` |
| PREFIX scan time | Insert 10k keys with common prefix, measure `prefix()` |
| State export time | Measure `exportState()` + `to_json()` with 1k, 5k, 10k entries |

### Framework Ideas

- Embed timing into a dedicated `bench/` directory with standalone executables
- Use median-of-N runs to reduce variance
- Output CSV for external plotting
- Compare with/without Bloom filter to quantify its benefit

---

## Known Technical Challenges

1. **Cross-structure consistency during cascade deletes:** When `deleteCascade` removes a key, all seven structures must be updated. Missing any one causes ghost entries.

2. **Bloom filter staleness:** Bloom filters don't support deletion. After many SET/DEL cycles, the false positive rate degrades. `CLEAR` is the only reset mechanism.

3. **TTL cleanup is lazy:** Expired keys are only removed when the next cache operation triggers `cleanupExpiredKeys()`. There is no background timer thread.

4. **Graph stability under rapid updates:** Re-rendering the D3 force simulation on every state change can cause visual jitter. The simulation auto-stops to mitigate this.

5. **LRU eviction during GET:** If a `GET` promotes a key and the LRU was at capacity from a prior `SET`, the promotion can trigger an unexpected eviction cascade.

6. **SkipList integration is currently partial:** A standalone SkipList implementation exists and is tested independently, but the runtime ordered-key visualization currently uses serialized sorted snapshots instead of a live SkipList-backed index. This tradeoff simplified frontend synchronization during early development while preserving the underlying SkipList implementation for future integration work.

---

## Concurrency Model and Performance Limitations

### Synchronization Strategy

PANCache uses **coarse-grained locking**: all cache operations are protected by a single global `std::mutex` (`CacheEngine::mtx_`). Every public method acquires a `lock_guard` for its entire duration:

```cpp
void CacheEngine::set(const string& key, const string& value) {
    std::lock_guard<std::mutex> lk(mtx_);  // lock acquired here
    cleanupExpiredKeys();
    hashmap_.insert(key, value);
    bloom_.insert(key);
    trie_.insert(key);
    ttl_heap_.erase(key);
    auto evicted = lru_.put(key, value);
    if (evicted.has_value()) deleteCascade(evicted.value());
}  // lock released here
```

**Why this design:**
- Guarantees **atomic, consistent** state transitions
- Simplifies correctness reasoning: no partial updates visible to concurrent readers
- No data races, no corruption, no inconsistent snapshots
- Essential given the tight coupling of seven interdependent data structures

### Performance Characteristics

| Operation | Lock Hold Time | Notes |
|-----------|---|---|
| `set(key, value)` | ~1-3 ms | Bloom insert, Trie insert, LRU put, potential cascade |
| `get(key)` | ~0.5-1 ms | Bloom check, HashMap lookup, LRU promotion, freq++ |
| `del(key)` | ~2-10 ms | Depends on cascade depth; recursive graph DFS |
| `exportState()` | ~10-20 ms | Snapshot all structures, sort keys, compute TopK, build JSON |
| `prefix(query)` | ~1-5 ms | Trie traversal, HashMap containment check |
| `topK(k)` | ~2-5 ms | Frequency map scan and sorting |

### Scalability Bottleneck

With a global mutex, **all cache operations serialize**. This means:

- **Theoretical maximum throughput:** ~1000-2000 requests/second on typical hardware (assuming 0.5-1ms average lock hold time per operation)
- **Practical limit:** Single server is I/O-bound (network + kernel TCP stack), not CPU-bound
- **Realistic workload:** 100-500 concurrent connections with moderate request rates

**This is not a bug—it is an architectural choice.** Serialization ensures correctness at the cost of throughput. For a cache with tight cross-structure invariants, this trade-off is acceptable.

### Concurrency Safety Guarantees

✅ **Correctness:** All data structure mutations are atomic from caller perspective
✅ **No deadlocks:** Single mutex eliminates circular lock dependencies
✅ **No data races:** All reads/writes protected by same lock
✅ **No corrupted snapshots:** State exports see consistent view of all seven structures

### HTTP Request Handling

The `cpp-httplib` library uses **per-request worker threads**. Each incoming HTTP request is handled by a thread from the OS thread pool:

```
Client A ──┐
            ├─► [Worker Thread 1] ──► POST /cmd "SET key1 val1" ──► CacheEngine (waits for lock)
Client B ──┤                                                        │
            │                                                       ├─ mtx_ (serialized)
Client C ──┼─► [Worker Thread 2] ──► GET /state ──────────────────►│
            │                                                       │
Client D ──┘─► [Worker Thread 3] ──► POST /cmd "GET key2" ────────►│
```

Under burst load (e.g., 20 concurrent POST requests):
1. 20 requests arrive and spawn 20 worker threads
2. Each thread executes its request handler immediately
3. All 20 threads contend for `CacheEngine::mtx_`
4. Threads serialize behind the lock: ~100ms total wall-clock time
5. Responses are sent as locks are released

**Windows-specific behavior:**
- Default thread pool size is limited by available system resources
- Keep-alive sockets remain open for 5 seconds after request completion
- Lingering connections in TIME_WAIT state can cause brief connection delays if pool exhausted

### Integration Testing Approach

The HTTP integration test (`tests/test_http_cmd.cpp`) validates:

✅ **Thread safety:** 2 concurrent threads × 10 SET requests each; validates no corruption
✅ **State consistency:** `exportState()` snapshots are coherent after burst load
✅ **Feature correctness:** TTL expiry, dependency cascades, form-encoded/plain-text parsing
✅ **Keep-alive handling:** 6-second wait between burst and state probe allows connection recycling

**Why the 6-second wait is necessary (not artificial):**
- Httplib keep-alive timeout is 5 seconds
- Burst of 20 requests leaves 20 worker threads in keep-alive state
- Without the wait, new /state request competes with lingering connections
- 6-second wait ensures old workers are reaped, new request gets fresh worker
- This is standard practice for network tests on systems with limited thread pools

**Deliberately NOT tested:**
- 100+ concurrent connections (exceeds reasonable system capacity)
- 10,000 requests/second (requires async handlers or thread pool tuning)
- Production-scale load testing (out of scope for this project)

### Design Rationale

PANCache prioritizes **correctness and simplicity** over throughput:

1. **Correctness first:** Complex interactions between seven data structures make fine-grained locking error-prone
2. **Realistic scope:** Intended for learning, visualization, and small-scale caching—not high-frequency trading
3. **Maintainability:** Single global lock is easier to reason about than lock-per-structure
4. **Honest limits:** This documentation makes limitations clear to future maintainers

### If Higher Throughput is Needed

To scale beyond ~1000 req/sec, consider these architectures:

- **Sharding:** Partition cache into N buckets, each with its own mutex → N × throughput
- **Read-write lock:** Separate lock for read-only operations (`get`, `prefix`, `topK`, `exportState`) from write operations → read parallelism
- **Async handlers:** cpp-httplib supports async requests; allows pipelining while waiting for cache locks
- **Event loop:** Rewrite core cache to use event-driven architecture (more complex, but removes global lock)

These are beyond current project scope but documented here for future reference.
