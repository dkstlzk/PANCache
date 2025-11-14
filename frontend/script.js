
const cache = new Map();
const links = [];
const svg = d3.select("#graph");
let ttlTimers = {};           
let ttlExpiry = {};    
let lruList = [];           


function getSvgSize() {
  const node = svg.node();
  if (!node) return { width: 600, height: 400 };
  const r = node.getBoundingClientRect();
  const w = Math.max(300, Math.round(r.width || +svg.attr("width") || 600));
  const h = Math.max(200, Math.round(r.height || +svg.attr("height") || 400));
  return { width: w, height: h };
}

function log(message) {
  const logDiv = document.getElementById("log");
  if (!logDiv) return;
  const time = new Date().toLocaleTimeString();
  logDiv.innerHTML += `[${time}] ${message}<br>`;
  logDiv.scrollTop = logDiv.scrollHeight;
}

function updateCacheTable() {
  const tbody = document.querySelector("#cache-table tbody");
  if (!tbody) return;
  tbody.innerHTML = "";
  for (const [key, obj] of cache.entries()) {
    const row = document.createElement("tr");
    row.innerHTML = `
      <td>${key}</td>
      <td>${obj.value}</td>
      <td>${obj.ttl ?? "-"}</td>
      <td class="status" style="color:${obj.statusColor}">${obj.status}</td>
    `;
    tbody.appendChild(row);
  }

  updateHashmapBuckets();
  updateLRU();
  updateTTLHeap();
}


function updateHashmapBuckets() {

  const buckets = 6;
  const arr = Array.from({ length: buckets }, () => []);
  for (const [key, obj] of cache.entries()) {
    let idx;
    if (!key || typeof key !== "string") {
      idx = Math.abs(String(key).length) % buckets;
    } else {
      let sum = 0;
      for (let i = 0; i < key.length; ++i) sum = (sum + key.charCodeAt(i)) | 0;
      idx = Math.abs(sum) % buckets;
    }
    arr[idx].push(`${key}:${obj.value}`);
  }

  const el = document.getElementById("hashmap-buckets");
  if (!el) return;
  el.textContent = arr.map((b, i) =>
    `Bucket ${i}: ${b.length ? b.join(", ") : "(empty)"}`
  ).join("\n");
}

function updateLRU() {
  const el = document.getElementById("lru-cache");
  if (!el) return;
  if (lruList.length === 0) {
    el.textContent = "(empty)";
    return;
  }
  el.textContent = "MRU → LRU\n" + lruList.join("  ");
}
function updateTTLHeap() {
  const el = document.getElementById("ttl-heap");
  if (!el) return;
  const now = Date.now();
  const arr = Object.keys(ttlExpiry)
    .filter(k => cache.has(k))
    .map(k => ({ key: k, left: Math.max(0, Math.round((ttlExpiry[k] - now) / 1000)) }))
    .sort((a, b) => a.left - b.left);

  if (!arr.length) {
    el.textContent = "(no TTL keys)";
    return;
  }
  el.textContent = arr.map(x => `${x.key} → ${x.left}s`).join("\n");
}

function renderGraph() {
  svg.selectAll("*").remove();
  const { width, height } = getSvgSize();
  svg.attr("viewBox", `0 0 ${width} ${height}`);

  const nodes = Array.from(cache.keys()).map(k => ({ id: k }));
  const linkData = links.map(l => ({ source: l.parent, target: l.child }));

  if (nodes.length === 0) {
    svg.append("text")
      .attr("x", width / 2)
      .attr("y", height / 2)
      .attr("text-anchor", "middle")
      .attr("fill", "#99a")
      .text("No nodes");
    return;
  }

  const simulation = d3.forceSimulation(nodes)
    .force("link", d3.forceLink(linkData).id(d => d.id).distance(70).strength(0.9))
    .force("charge", d3.forceManyBody().strength(-120))
    .force("center", d3.forceCenter(width / 2, height / 2))
    .force("collide", d3.forceCollide(28))
    .force("x", d3.forceX(width / 2).strength(0.05))
    .force("y", d3.forceY(height / 2).strength(0.05))
    .velocityDecay(0.35)
    .alphaDecay(0.04);

  const link = svg.selectAll(".link")
    .data(linkData, d => d.source + "-" + d.target)
    .enter()
    .append("line")
    .attr("class", "link")
    .attr("stroke", "#45a29e")
    .attr("stroke-width", 2)
    .attr("stroke-linecap", "round")
    .attr("opacity", 0.95);

  const node = svg.selectAll(".node")
    .data(nodes, d => d.id)
    .enter()
    .append("g")
    .attr("class", "node");

  node.append("circle")
    .attr("r", 20)
    .attr("fill", "#66fcf1")
    .attr("stroke", "#0b0c10")
    .attr("stroke-width", 1.5);

  node.append("text")
    .text(d => d.id)
    .attr("font-size", 12)
    .attr("text-anchor", "middle")
    .attr("dy", ".35em")
    .attr("fill", "#0b0c10")
    .style("pointer-events", "none");

  const radius = 20;
  const clampX = (x) => Math.max(radius, Math.min(width - radius, x));
  const clampY = (y) => Math.max(radius, Math.min(height - radius, y));

  const dragHandler = d3.drag()
    .on("start", (event, d) => {
      if (!event.active) simulation.alphaTarget(0.3).restart();
      d.fx = clampX(event.x ?? d.x);
      d.fy = clampY(event.y ?? d.y);
    })
    .on("drag", (event, d) => {
      d.fx = clampX(event.x);
      d.fy = clampY(event.y);
    })
    .on("end", (event, d) => {
      if (!event.active) simulation.alphaTarget(0);
      d.fx = null;
      d.fy = null;
    });

  node.call(dragHandler);

  simulation.on("tick", () => {
    nodes.forEach(n => {
      n.x = clampX(n.x);
      n.y = clampY(n.y);
    });

    node.attr("transform", d => `translate(${d.x},${d.y})`);

    link
      .attr("x1", d => (d.source.x !== undefined ? d.source.x : d.source))
      .attr("y1", d => (d.source.y !== undefined ? d.source.y : d.source))
      .attr("x2", d => (d.target.x !== undefined ? d.target.x : d.target))
      .attr("y2", d => (d.target.y !== undefined ? d.target.y : d.target));
  });

  const stopAfter = setInterval(() => {
    if (simulation.alpha() < 0.02) {
      simulation.stop();
      clearInterval(stopAfter);
    }
  }, 200);

  setTimeout(() => {
    if (simulation.running()) simulation.stop();
  }, 2500);
}


// LRU Helpers
function touchLRUOnAccess(key) {
  lruList = lruList.filter(k => k !== key);
  lruList.unshift(key); 
}

function removeFromLRU(key) {
  lruList = lruList.filter(k => k !== key);
}


// TTL Logic
function scheduleTTL(key, seconds) {
  if (ttlTimers[key]) {
    clearTimeout(ttlTimers[key]);
  }
  if (seconds <= 0) {
    cache.delete(key);
    delete ttlTimers[key];
    delete ttlExpiry[key];
    log(`EXPIRE: ${key} removed immediately`);
    updateCacheTable();
    renderGraph();
    return;
  }
  const expireAt = Date.now() + seconds * 1000;
  ttlExpiry[key] = expireAt;
  ttlTimers[key] = setTimeout(() => {
    cache.delete(key);
    delete ttlTimers[key];
    delete ttlExpiry[key];
    removeFromLRU(key);
    log(`EXPIRE: ${key} removed after ${seconds}s`);
    updateCacheTable();
    renderGraph();
  }, seconds * 1000);
}

function executeCommand(cmd) {
  const parts = cmd.trim().split(/\s+/);
  const op = parts[0]?.toUpperCase();
  const key = parts[1];
  const val = (op === "SET" ? parts.slice(2).join(" ") : parts[2]);
  const now = performance.now();
  if (!op) return;

  if (op === "SET") {
    if (!key) {
      log("SET requires a key (usage: SET key value)");
    } else {
      const valueToSet = (val === undefined) ? "" : val;
      cache.set(key, {
        value: valueToSet,
        ttl: null,
        status: "Inserted",
        statusColor: "#66fcf1"
      });
      touchLRUOnAccess(key);

      updateSkipList();

      log(`SET ${key}=${valueToSet} (latency: ${((performance.now() - now) * 1000).toFixed(2)} µs)`);
    }

  } else if (op === "GET") {
    if (!key) log("GET requires a key");
    else if (cache.has(key)) {
      const entry = cache.get(key);
      entry.status = "Accessed";
      entry.statusColor = "#45a29e";
      touchLRUOnAccess(key);

      accessCount[key] = (accessCount[key] || 0) + 1;
      updateTopK();

      log(`GET ${key} -> ${entry.value} (latency: ${((performance.now() - now) * 1000).toFixed(2)} µs)`);
    } else {
      log(`GET ${key} -> [MISS]`);
    }
  } else if (op === "DEL") {
    if (!key) log("DEL requires key");
    else {
      cascadeDelete(key);
      cache.delete(key);
      if (ttlTimers[key]) {
        clearTimeout(ttlTimers[key]);
        delete ttlTimers[key];
        delete ttlExpiry[key];
      }
      removeFromLRU(key);

      updateSkipList();

      log(`DEL ${key} (latency: ${((performance.now() - now) * 1000).toFixed(2)} µs)`);
    }
  } else if (op === "LINK") {
    if (!key || !val) log("LINK requires A B");
    else {
      links.push({ parent: key, child: val });
      log(`LINK ${key} -> ${val}`);
    }
  } else if (op === "EXPIRE") {
    const ttl = parseInt(val, 10);
    if (!key || isNaN(ttl)) log("EXPIRE key ttl");
    else if (cache.has(key)) {
      const entry = cache.get(key);
      entry.ttl = ttl;
      entry.status = "Expiring";
      entry.statusColor = "#ffd166";
      scheduleTTL(key, ttl);
      log(`EXPIRE ${key} set for ${ttl}s`);
    } else log(`EXPIRE: ${key} not present`);

  } else if (op === "SIZE") {
    log(`Cache size: ${cache.size}`);
  } else if (op === "CLEAR") {
    cache.clear();
    links.length = 0;
    for (const k of Object.keys(ttlTimers)) {
      clearTimeout(ttlTimers[k]);
      delete ttlTimers[k];
      delete ttlExpiry[k];
    }
    lruList = [];
    log("Cache cleared");

    accessCount = {};
    updateTopK();
    updateSkipList();

  } else if (op === "PREFIX") {
    if (!key) log("PREFIX p");
    else updateTrie(key);

  } else if (op === "TOPK") {
    const k = parseInt(key);
    if (isNaN(k)) log("TOPK k");
    else {
      const arr = Object.entries(accessCount)
        .sort((a,b)=>b[1]-a[1])
        .slice(0,k);
      const el = document.getElementById("topk-box");
      if (el) el.textContent =
        arr.length ? arr.map(x => `${x[0]} (${x[1]})`).join("\n") : "(empty)";
    }

  } else {
    log(`Unknown command: ${cmd}`);
  }

  updateCacheTable();
  renderGraph();
  updateHashmapBuckets();
  updateLRU();
  updateTTLHeap();
}

const inputEl = document.getElementById("command-input");
if (inputEl) {
  inputEl.addEventListener("keydown", e => {
    if (e.key === "Enter") {
      const val = e.target.value.trim();
      if (val.length) {
        executeCommand(val);
        e.target.value = "";
      }
    }
  });
}

updateCacheTable();
renderGraph();
updateHashmapBuckets();
updateLRU();
updateTTLHeap();


function updateSkipList() {
  const keys = Array.from(cache.keys()).sort();
  const el = document.getElementById("skiplist-box");
  if (!el) return;
  el.textContent = keys.length ? keys.join(" → ") : "(empty)";
}

function updateTrie(prefix) {
  const keys = Array.from(cache.keys());
  const matches = keys.filter(k => k.startsWith(prefix));
  const el = document.getElementById("trie-box");
  if (!el) return;
  el.textContent = matches.length ? matches.join(" ") : "(no match)";
}

let accessCount = {};
function updateTopK() {
  const arr = Object.entries(accessCount).sort((a,b)=>b[1]-a[1]);
  const el = document.getElementById("topk-box");
  if (!el) return;
  el.textContent = arr.length ?
    arr.map(x => `${x[0]} (${x[1]})`).join("\n") :
    "(empty)";
}

function cascadeDelete(key) {
  const children = links.filter(l => l.parent === key).map(l => l.child);
  for (const c of children) {
    cache.delete(c);
    removeFromLRU(c);
    delete accessCount[c];
    cascadeDelete(c);
  }
  for (let i = links.length - 1; i >= 0; i--) {
    if (links[i].parent === key || links[i].child === key)
      links.splice(i, 1);
  }
}
