# PANCache — Portable Advanced IN-Memory Cache

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


## Architecture Modules

PANCache/
├── include/
│   ├── data/
│   │   ├── hashmap.hpp
│   │   ├── lru.hpp
│   │   ├── ttl_heap.hpp
│   │   └── skiplist.hpp
│   ├── depend/
│   │   └── graph.hpp
│   ├── analytics/
│   │   └── topk.hpp
│   ├── cli/
│   │   └── command_parser.hpp
│   └── utils/
│       └── types.hpp
│
├── src/
│   ├── data/
│   │   ├── hashmap.cpp
│   │   ├── lru.cpp
│   │   ├── ttl_heap.cpp
│   │   └── skiplist.cpp
│   ├── depend/
│   │   └── graph.cpp
│   ├── analytics/
│   │   └── topk.cpp
│   ├── cli/
│   │   └── command_parser.cpp
│   ├── main.cpp
│   └── utils/
│       └── time_utils.cpp
│
├── tests/
│   ├── test_hashmap.cpp
│   ├── test_lru.cpp
│   ├── test_heap.cpp
│   ├── test_skiplist.cpp
│   ├── test_graph.cpp
│   ├── test_topk.cpp
│   └── test_integration.cpp
│
├── docs/
│   └── architecture_diagram.png
│
├── Makefile
└── README.md

## Testing
Unit tests and integration tests should cover:
- HashMap: insert/get/delete, collision handling
- LRU: set/get sequences, eviction correctness
- TTL manager: expiry simulation, correct removals
- SkipList: ordered insertion, range query
- Graph: dependency add/delete, cascade invalidation
- Integration: combined commands (SET/GET/EXPIRE/RANGE/DEPEND)

## Milestones / Timeline (example 3-week sprint)
### Week 1
- Day 1: Project scaffold, README, initial commit.
- Days 2-4: Implement HashMap with unit tests.
- Days 5-7: Implement LRU + integrate with HashMap, tests.
### Week 2
- Days 8-10: Implement TTL Min-Heap & expiry loop, tests.
- Days 11-13: Implement SkipList for sorted access & integrate, tests.
- Day 14: CLI skeleton + integration test.
### Week 3
- Days 15-17: Dependency Graph module + tests.
- Days 18-19: Top-K analytics + INFO command.
- Days 20-21: Documentation, report draft, demo prep.

## Team Roles
- Parneeka (CSE): HashMap + LRU integration + CLI main loop.
- Cherry (DSAI): SkipList, Top-K analytics, Bloom filter (optional).
- Nupur (EE): TTL manager (heap) + performance measurement & tests.
- Anshika (CSE): Graph dependencies + integration + docs + demo scripting.

## Grading Checklist
- Novelty: graph dependencies + integrated DS modules
- Demonstration of code: CLI demo, unit tests, performance measurement
- Correctness: modules pass tests, integration works
- Efficiency: complexity analysis + basic benchmarking
- Documentation: README + architecture diagrams + report

## README Short Starter
Please see above sections for README content.

## Bonus Enhancements (if time permits)
- Concurrency / thread safety for multi-client access
- Persistence snapshot (AOF or periodic dump)
- Networked REPL (simple TCP server)
- Consistent hashing + simulated shards for distributed mapping
- Performance graphs: compare naive map-only vs full system

## Risks & Mitigation
- Scope too large → focus minimal viable product first: HashMap + LRU + TTL.
- Bug-prone low-level code → write unit tests, use memory tools (Valgrind).
- Time crunch → prototype in higher-level language if needed then port to C.

## Final Checklist Before Submission
- [ ] All core modules implemented & unit tested.
- [ ] README + architecture diagram in docs/.
- [ ] Demo script prepared (6-8 min). Slide deck ready.
- [ ] Report draft complete.
- [ ] Clean commit history.


