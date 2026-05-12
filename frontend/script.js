const BACKEND_URL =
  (window.location.origin && window.location.origin !== "null")
    ? window.location.origin
    : "http://127.0.0.1:8080";
const POLL_INTERVAL_MS = 1500;
let backendOnline = false;

const cache = new Map();
const links = [];
const svg = d3.select("#graph");
let ttlExpiry = {};
let lruList = [];
let hashmapBuckets = [];
let fetchInFlight = false;
let refreshQueued = false;
let pollTimer = null;
let pollActive = false;
let lastStateSignature = "";

// send raw command to backend
async function sendCommandToBackend(cmd) {
  try {
    const res = await fetch(`${BACKEND_URL}/cmd`, {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: cmd
    });

    backendOnline = true;

    const text = await res.text();
    log(text.trim() || `(executed ${cmd})`);

    await fetchBackendState({ force: true });

  } catch (err) {
    backendOnline = false;
    log("⚠ Backend offline. Command not executed.");
  }
}

// fetch backend full state object
async function fetchBackendState(options = {}) {
  const force = options.force === true;
  if (fetchInFlight) {
    if (force) refreshQueued = true;
    return;
  }
  fetchInFlight = true;
  try {
    const r = await fetch(`${BACKEND_URL}/state`);
    if (!r.ok) throw new Error("bad");

    backendOnline = true;

    const state = await r.json();
    const signature = JSON.stringify(state);
    if (signature !== lastStateSignature) {
      lastStateSignature = signature;
      applyBackendState(state);
    }

  } catch (err) {
    backendOnline = false;
  } finally {
    fetchInFlight = false;
    if (refreshQueued) {
      refreshQueued = false;
      fetchBackendState();
    }
  }
}

function scheduleNextPoll() {
  if (!pollActive) return;
  pollTimer = setTimeout(async () => {
    if (!document.hidden) await fetchBackendState();
    scheduleNextPoll();
  }, POLL_INTERVAL_MS);
}

function startPolling() {
  if (pollActive) return;
  pollActive = true;
  scheduleNextPoll();
}

function stopPolling() {
  pollActive = false;
  if (pollTimer) {
    clearTimeout(pollTimer);
    pollTimer = null;
  }
}

// apply backend → frontend state mapping
function applyBackendState(s) {

  // cache
  if (s.entries) {
    cache.clear();
    for (const e of s.entries) {
      cache.set(e.key, {
        value: e.value,
        ttl: e.ttl >= 0 ? e.ttl : null,
        status: e.status || "Synced",
        statusColor: "#66fcf1"
      });
    }
  }

  if (s.links) {
    links.length = 0;
    for (const l of s.links) links.push(l);
  }

  if (s.lru) lruList = [...s.lru];

  if (s.ttlExpiry) ttlExpiry = { ...s.ttlExpiry };

  if (s.hashmapBuckets) hashmapBuckets = [...s.hashmapBuckets];

  if (s.topk) {
    const el = document.getElementById("topk-box");
    el.textContent = s.topk.length
      ? s.topk.map(x => `${x.key} (${x.count})`).join("\n")
      : "(empty)";
  }

  if (s.skiplist) {
    document.getElementById("skiplist-box").textContent =
      s.skiplist.length ? s.skiplist.join(" → ") : "(empty)";
  }

  if (s.trie_matches) {
    document.getElementById("trie-box").textContent =
      s.trie_matches.length ? s.trie_matches.join(" ") : "(no match)";
  }

  updateCacheTable();
  renderGraph();
}

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
  const el = document.getElementById("hashmap-buckets");
  if (!el) return;
  if (!hashmapBuckets.length) {
    el.textContent = "(empty)";
    return;
  }
  el.textContent = hashmapBuckets.map((b, i) =>
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
    .map(k => ({
      key: k,
      left: Math.max(0, Math.round((ttlExpiry[k] - now) / 1000))
    }))
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
  const clampX = x => Math.max(radius, Math.min(width - radius, x));
  const clampY = y => Math.max(radius, Math.min(height - radius, y));

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


const inputEl = document.getElementById("command-input");
if (inputEl) {
  inputEl.addEventListener("keydown", e => {
    if (e.key === "Enter") {
      const cmd = e.target.value.trim();
      if (!cmd.length) return;

      sendCommandToBackend(cmd);

      e.target.value = "";
    }
  });
}

document.addEventListener("visibilitychange", () => {
  if (document.hidden) {
    stopPolling();
  } else {
    startPolling();
    fetchBackendState({ force: true });
  }
});

fetchBackendState({ force: true });
startPolling();
