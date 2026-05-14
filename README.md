# PANCache

A Redis-inspired in-memory cache engine built from scratch in C++17 with an integrated real-time visualization dashboard.

PANCache combines multiple classical data structures — HashMap, LRU linked list, TTL min-heap, Trie, Bloom filter, dependency graph, SkipList, and Top-K analytics — into a single coherent cache system. The backend exposes an HTTP API; the frontend renders live snapshots of every internal data structure as the cache mutates.

---

## Why PANCache Exists

Most cache implementations are black boxes. PANCache makes the internals visible.

The goal is to demonstrate how real systems compose multiple data structures to solve overlapping concerns (fast lookup, eviction, expiration, dependency tracking, prefix search, frequency analysis) — and how those structures interact under mutation, eviction, and cascading deletion.

This is not a production cache. It is a systems engineering project designed for clarity, correctness, and architectural demonstration.

---

## Architecture at a Glance

```
Browser (frontend/)
  ├── GET  /           → index.html (dashboard)
  ├── GET  /styles.css → stylesheet
  ├── GET  /script.js  → D3.js visualization logic
  ├── GET  /state      → full JSON snapshot of all DS state
  └── POST /cmd        → execute cache command (plain text body)
        ↓
  C++ HTTP Server (cpp-httplib)
        ↓
  CommandParser (tokenize → dispatch)
        ↓
  CacheEngine (orchestration facade)
        ↓
  ┌─────────────┬──────────────┬──────────────┬────────────┐
  │  HashMap    │  LRU Cache   │  TTL Heap    │  Trie      │
  │  O(1) KV    │  eviction    │  expiration  │  prefix    │
  ├─────────────┼──────────────┼──────────────┼────────────┤
  │ Bloom Filter│  Dep. Graph  │  SkipList    │  Top-K     │
  │  neg. lookup│  cascade del │  sorted keys │  frequency │
  └─────────────┴──────────────┴──────────────┴────────────┘
```

The frontend is stateless — it polls `/state`, receives a JSON snapshot, and renders every data structure panel from that snapshot. No client-side cache simulation or TTL logic exists.

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed runtime flow and module interaction diagrams.

---

## Data Structures

| Structure | Role | Complexity |
|---|---|---|
| **HashMap** (chained, dynamic resize) | Primary key-value storage | O(1) avg lookup/insert |
| **LRU Cache** (doubly-linked list + HashMap) | Eviction of least recently used entries | O(1) access/eviction |
| **TTL Heap** (min-heap + map) | Time-based expiration scheduling | O(log n) insert, O(1) peek |
| **Trie** | Prefix search across the keyspace | O(L) search, L = key length |
| **Bloom Filter** | Fast negative lookups before HashMap access | O(k) per query, k = hash count |
| **Dependency Graph** (adjacency list, DFS) | Cascading deletion across linked keys | O(V+E) recursive delete |
| **SkipList** (probabilistic, experimental) | Prototype implementation for ordered access experimentation | O(log n) avg search/insert |
| **Top-K** (min-heap over frequency map) | Access frequency analytics | O(n log k) per query |

### Why Multiple Structures?

Each data structure addresses a distinct operational concern. A real cache system needs all of these working together:

- **HashMap** alone gives O(1) lookup but no eviction, expiration, or search.
- **LRU** provides eviction policy — but needs the HashMap for O(1) node access.
- **TTL Heap** handles time-based expiry independently of access patterns.
- **Trie** enables `PREFIX` queries without scanning the entire keyspace.
- **Bloom Filter** short-circuits misses before touching the HashMap (useful at scale).
- **Dependency Graph** models key relationships — deleting a parent cascades to children.
- **SkipList** was implemented to explore probabilistic ordered indexing and range-query style traversal. The current runtime visualization uses lexicographically sorted snapshots, while deeper SkipList-backed integration remains a planned architectural enhancement.
- **Top-K** surfaces frequently accessed keys using a bounded min-heap.

The `CacheEngine` class orchestrates all of these, ensuring consistency across structures during mutations.

---

## Supported Commands

| Command | Description | Example |
|---|---|---|
| `SET key value` | Insert or update a key-value pair | `SET user:1 Alice` |
| `GET key` | Retrieve a value by key | `GET user:1` |
| `DEL key` | Delete a key (cascades to dependents) | `DEL user:1` |
| `LINK parent child` | Create a dependency edge | `LINK user:1 user:2` |
| `EXPIRE key seconds` | Set TTL expiration in seconds | `EXPIRE session:a 30` |
| `SIZE` | Report total number of cached entries | `SIZE` |
| `SEARCH key` | Exact key existence check | `SEARCH user:1` |
| `PREFIX prefix` | List all keys matching prefix | `PREFIX user:` |
| `TOPK k` | Show top-k most frequently accessed keys | `TOPK 5` |
| `CLEAR` | Remove all keys and reset state | `CLEAR` |
| `HELP` | Show command reference | `HELP` |

---

## HTTP API

| Method | Endpoint | Content-Type | Description |
|---|---|---|---|
| `GET` | `/` | `text/html` | Dashboard UI |
| `GET` | `/state` | `application/json` | Full cache state snapshot |
| `POST` | `/cmd` | `text/plain` (body) | Execute a cache command |
| `GET` | `/health` | `text/plain` | Liveness check (`"OK"`) |

### Example: API Usage

```bash
# Insert a key
curl -X POST http://127.0.0.1:8080/cmd -d "SET user:1 Alice"

# Retrieve a key
curl -X POST http://127.0.0.1:8080/cmd -d "GET user:1"
# → user:1 = Alice

# Create dependency and set TTL
curl -X POST http://127.0.0.1:8080/cmd -d "LINK user:1 user:2"
curl -X POST http://127.0.0.1:8080/cmd -d "EXPIRE user:2 30"

# Prefix search
curl -X POST http://127.0.0.1:8080/cmd -d "PREFIX user:"
# → Matches: user:1 user:2

# Full state snapshot
curl http://127.0.0.1:8080/state
# → {"entries":[...],"links":[...],"lru":[...],...}
```

---

## Build Instructions

### Prerequisites

- **Compiler:** g++ with C++17 support (GCC 9+ or MSYS2 MinGW-w64)
- **OS:** Windows (MSYS2/MinGW), Linux, or macOS
- **Make:** GNU Make

### Windows / MSYS2 Setup

1. Install [MSYS2](https://www.msys2.org/)
2. Open **MSYS2 MinGW 64-bit** terminal
3. Install toolchain:
   ```bash
   pacman -S mingw-w64-x86_64-gcc make
   ```
4. Ensure `g++` and `make` are in PATH

### Build

```bash
make clean
make all -j8
```

This produces `build/pancache_main.exe` (Windows) or `build/pancache_main` (Linux/macOS).

### Run

```bash
./build/pancache_main.exe
```

The server starts on `http://127.0.0.1:8080`. A browser window opens automatically (disable with `PANCACHE_OPEN_BROWSER=0`).

### Tests

```bash
# Build and run all tests
make test_all

# Run a specific test
make test_hashmap
make test_lru
make test_cache
make test_integration
```

### Makefile Reference

| Command | Description |
|---|---|
| `make all` | Build main executable |
| `make test_all` | Build and run all test suites |
| `make test_<name>` | Build and run a specific test (e.g., `test_lru`) |
| `make clean` | Remove the `build/` directory |
| `make help` | Show available commands |

Available tests: `test_hashmap`, `test_lru`, `test_heap`, `test_skiplist`, `test_graph`, `test_cache`, `test_integration`, `test_cli`, `test_logger`, `test_trie`, `test_topk`

---

## Frontend Dashboard

The visualization dashboard renders all internal data structures in real-time:

| Panel | What it shows |
|---|---|
| **Cache Table** | All key-value entries with TTL and status |
| **HashMap Buckets** | Internal bucket layout showing hash distribution |
| **LRU Cache** | MRU → LRU ordering of keys |
| **TTL Min-Heap** | Keys with active TTL, sorted by time remaining |
| **Dependency Graph** | Interactive D3.js force-directed graph with drag support |
| **Ordered Index View** | Lexicographically sorted runtime keys |
| **Trie (Prefix Search)** | Results of the most recent `PREFIX` query |
| **Top-K** | Most frequently accessed keys |
| **Command Console** | Interactive terminal-style console with history navigation (Up/Down arrows) and output styling |

The frontend polls `/state` every 1.5 seconds (skipped when the tab is hidden). After each command, the frontend forces an immediate refresh.

### Screenshots

<img width="2217" height="1189" alt="Screenshot 2026-05-12 232723" src="https://github.com/user-attachments/assets/14f46225-2748-4d38-aeb6-8d429aa39b95" />
<img width="2239" height="1189" alt="Screenshot 2026-05-12 232813" src="https://github.com/user-attachments/assets/b3ff9f41-3937-4239-baa1-dd44f6803545" />

---

## Example Workflow

```
SET user:1 Alice
SET user:2 Bob
SET user:3 Charlie

LINK user:1 user:2        # user:2 depends on user:1
LINK user:1 user:3        # user:3 depends on user:1

EXPIRE user:3 15          # user:3 expires in 15 seconds

GET user:1                # → user:1 = Alice (access count incremented)
GET user:1                # → user:1 = Alice (access count: 2)
GET user:2                # → user:2 = Bob

TOPK 3                    # → user:1 => 2, user:2 => 1
PREFIX user:              # → user:1 user:2 user:3

DEL user:1                # cascades: user:2 and user:3 also deleted
GET user:2                # → Key not found: user:2
```

---

## Runtime Log Example

```
2026-05-12 17:40:00.123 [INFO] Initializing PANCache...
2026-05-12 17:40:00.124 [INFO] Cache engine initialized with capacity: 50
2026-05-12 17:40:00.125 [INFO] HTTP routes configured
2026-05-12 17:40:00.126 [INFO] PANCache server starting on http://127.0.0.1:8080
2026-05-12 17:40:00.127 [INFO] Available endpoints:
2026-05-12 17:40:00.127 [INFO]   - GET  /health
2026-05-12 17:40:00.127 [INFO]   - POST /cmd
2026-05-12 17:40:00.128 [INFO] PANCache UI available at: http://127.0.0.1:8080
2026-05-12 17:40:00.128 [INFO] Press Ctrl+C to stop the server
[HTTP] Binding to 127.0.0.1:8080
[HTTP] Listening on 127.0.0.1:8080
```

---

## Project Structure

```
PANCache/
├── frontend/
│   ├── index.html              # dashboard UI
│   ├── styles.css              # dark-theme styling
│   └── script.js               # D3.js rendering + backend polling
├── include/
│   ├── analytics/
│   │   └── topk.hpp            # Top-K frequency analysis (min-heap)
│   ├── cli/
│   │   └── command_parser.hpp  # command tokenization + dispatch
│   ├── data/
│   │   ├── cache_engine.hpp    # CacheEngine orchestration facade
│   │   ├── cache_engine_state.hpp  # state snapshot + JSON serialization
│   │   ├── hashmap.hpp         # chained HashMap with dynamic resize
│   │   ├── lru.hpp             # LRU cache (doubly-linked list)
│   │   ├── skiplist.hpp        # experimental probabilistic ordered-index structure
│   │   ├── trie.hpp            # prefix Trie
│   │   ├── ttl_heap.hpp        # TTL expiration min-heap
│   │   └── bloom_filter.hpp    # Bloom filter for negative lookups
│   ├── depend/
│   │   └── graph.hpp           # dependency graph (DFS cascade)
│   └── utils/
│       ├── http_server.hpp     # HTTP server wrapper
│       ├── httplib.hpp         # cpp-httplib (vendored, single-header)
│       └── logger.hpp          # thread-safe logger with color output
├── src/
│   ├── main.cpp                # entry point, server setup, signal handling
│   ├── analytics/topk.cpp
│   ├── cli/command_parser.cpp
│   ├── data/
│   │   ├── cache_engine.cpp    # CacheEngine implementation
│   │   ├── hashmap.cpp
│   │   ├── lru.cpp
│   │   ├── trie.cpp
│   │   ├── ttl_heap.cpp
│   │   └── bloom_filter.cpp
│   ├── depend/graph.cpp
│   └── utils/
│       ├── http_server.cpp
│       └── logger.cpp
├── tests/
│   ├── test_cache.cpp          # CacheEngine integration tests
│   ├── test_hashmap.cpp
│   ├── test_lru.cpp
│   ├── test_heap.cpp           # TTL heap tests
│   ├── test_skiplist.cpp
│   ├── test_graph.cpp          # dependency graph tests
│   ├── test_trie.cpp
│   ├── test_topk.cpp
│   ├── test_integration.cpp    # full cross-module test
│   ├── test_cli.cpp
│   └── test_logger.cpp
├── Makefile                    # unified build system
├── ARCHITECTURE.md             # detailed architecture documentation
└── README.md
```

---

## Design Decisions

| Decision | Rationale |
|---|---|
| **Centralized CacheEngine** | Single orchestration point prevents data structure drift and ensures consistency across HashMap, LRU, TTL, Trie, Bloom, and Graph on every mutation. |
| **Stateless frontend** | The browser renders backend snapshots only — no client-side TTL timers or cache simulation. Eliminates desynchronization. |
| **Manual JSON serialization** | Avoids heavy dependencies. The `to_json()` function in `cache_engine_state.hpp` serializes the full state in a single pass. |
| **Polling over WebSocket** | Simpler to implement and debug. 1.5s poll interval with visibility-aware throttling is sufficient for a demo tool. |
| **cpp-httplib (vendored)** | Single-header HTTP library. Zero build dependencies beyond a C++17 compiler. |
| **Template explicit instantiation** | HashMap, LRU, and TTL are templates instantiated for `<string, string>` at link time. Keeps headers clean while supporting type safety. |
| **Ordered-key visualization** | The frontend currently renders ordered runtime keys using serialized sorted snapshots. A standalone SkipList implementation exists and is tested independently, with future plans for deeper runtime integration. |

---

## Performance Characteristics

| Operation | Time Complexity | Notes |
|---|---|---|
| `SET` | O(1) amortized | HashMap insert + LRU promotion + Trie insert + Bloom insert |
| `GET` | O(1) amortized | Bloom check → HashMap lookup → LRU promotion |
| `DEL` | O(V+E) worst case | DFS over dependency graph, then O(1) per structure per key |
| `EXPIRE` | O(log n) | TTL heap insertion |
| `PREFIX` | O(L + m) | L = prefix length, m = number of matches |
| `TOPK` | O(n log k) | n = unique keys accessed, k = requested count |
| TTL cleanup | O(k log n) | k = expired keys removed per sweep |
| LRU eviction | O(1) | Tail removal + HashMap delete |

> **Note:** These are algorithmic complexities of the custom implementations. No synthetic benchmarks have been run. See [ARCHITECTURE.md](ARCHITECTURE.md) for benchmark planning notes.

---

## Limitations and Tradeoffs

- **Single-threaded:** All cache operations and HTTP handling run on a single thread (cpp-httplib's internal thread pool handles connections, but CacheEngine is not thread-safe).
- **No persistence:** All data is in-memory. Server restart clears the cache.
- **Bloom filter does not shrink:** Deletions do not remove bits from the Bloom filter. Over time, false positive rate increases until `CLEAR` resets it.
- **Polling latency:** Frontend updates are delayed by up to 1.5 seconds (the poll interval).
- **No authentication:** The HTTP API is open. Not intended for production use.

---

## Troubleshooting

| Problem | Solution |
|---|---|
| `make` fails with "g++ not found" | Install g++ via MSYS2: `pacman -S mingw-w64-x86_64-gcc` |
| Port 8080 already in use | Kill the existing process or change the port in `main.cpp` |
| Browser doesn't open automatically | Navigate manually to `http://127.0.0.1:8080` |
| Frontend shows "Backend offline" | Ensure the server is running. Check firewall settings. |
| Tests fail with linker errors | Run `make clean` then `make test_all` |
| MSYS2 path issues | Use the **MinGW 64-bit** terminal, not the MSYS2 default shell |

---

## Roadmap

- [ ] SSE or WebSocket for push-based frontend updates
- [ ] Background TTL cleanup thread (currently cleanup is lazy, triggered on access)
- [ ] Thread-safe CacheEngine with reader-writer locks
- [ ] Benchmarking harness with latency histograms
- [ ] Persistence layer (AOF or snapshot-based)
- [ ] CLI mode (interactive terminal without HTTP server)

---

## License

This project is intended for educational and portfolio purposes.
