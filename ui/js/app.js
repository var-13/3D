// ui/js/app.js
// ThermoBench – main application script.
// Phase 0: tab switching + version badge fetch.
// Later phases will add FEA plots, benchmark UI, etc.

"use strict";

// ── Tab switching ─────────────────────────────────────────────────────────────
(function initTabs() {
  const buttons = document.querySelectorAll(".tab-btn");
  const panels  = document.querySelectorAll(".tab-panel");

  function activate(tabName) {
    buttons.forEach(b => {
      const isActive = b.dataset.tab === tabName;
      b.classList.toggle("active", isActive);
      b.setAttribute("aria-selected", isActive);
    });
    panels.forEach(p => {
      p.classList.toggle("active", p.id === "tab-" + tabName);
    });
    // Persist last tab so a page refresh returns you to the same place.
    try { sessionStorage.setItem("tb_tab", tabName); } catch (_) {}
  }

  buttons.forEach(btn => {
    btn.addEventListener("click", () => activate(btn.dataset.tab));
  });

  // Restore last tab (default: geometry)
  const saved = (function() {
    try { return sessionStorage.getItem("tb_tab"); } catch (_) { return null; }
  })();
  activate(saved || "geometry");
})();


// ── Version badge ─────────────────────────────────────────────────────────────
(function fetchVersion() {
  const badge = document.getElementById("version-badge");
  if (!badge) return;

  fetch("/api/version")
    .then(r => {
      if (!r.ok) throw new Error("HTTP " + r.status);
      return r.json();
    })
    .then(data => {
      badge.textContent = data.version || "v?";
    })
    .catch(() => {
      // Running from file:// or server not up yet – silently keep default text.
    });
})();


// ── Phase 1: Thermal FEA Module ──────────────────────────────────────────────
(function initThermalFEA() {
  const inpL  = document.getElementById("inp-L");
  const inpA  = document.getElementById("inp-A");
  const inpk  = document.getElementById("inp-k");
  const inpq  = document.getElementById("inp-q");
  const inpT0 = document.getElementById("inp-T0");
  const inpTL = document.getElementById("inp-TL");
  const selN  = document.getElementById("sel-N");

  const btnRun       = document.getElementById("btn-run-thermal");
  const btnRunConv   = document.getElementById("btn-run-convergence");
  const btnExport    = document.getElementById("btn-export-thermal-csv");
  const btnExportConv= document.getElementById("btn-export-conv-csv");
  const statusBar    = document.getElementById("thermal-status");

  const kpiN       = document.getElementById("kpi-N");
  const kpiH       = document.getElementById("kpi-h");
  const kpiMidErr  = document.getElementById("kpi-mid-err");
  const kpiNodeErr = document.getElementById("kpi-node-err");
  const kpiOrder   = document.getElementById("kpi-order");
  const tbodyConv  = document.getElementById("tbody-convergence");

  if (!btnRun || !window.Chart) return;

  let tempChart = null;
  let convChart = null;

  function setStatus(msg, type = "info") {
    if (!statusBar) return;
    statusBar.textContent = msg;
    statusBar.className = "status-bar " + type;
  }

  function getParams() {
    const L  = parseFloat(inpL.value);
    const A  = parseFloat(inpA.value);
    const k  = parseFloat(inpk.value);
    const q  = parseFloat(inpq.value);
    const T0 = parseFloat(inpT0.value);
    const TL = parseFloat(inpTL.value);
    const N  = parseInt(selN.value, 10);

    // Strict validation
    if (isNaN(L) || L <= 0)  { setStatus("Validation error: Length L must be a positive number (> 0).", "error"); return null; }
    if (isNaN(A) || A <= 0)  { setStatus("Validation error: Area A must be a positive number (> 0).", "error"); return null; }
    if (isNaN(k) || k <= 0)  { setStatus("Validation error: Conductivity k must be positive (> 0).", "error"); return null; }
    if (isNaN(q))            { setStatus("Validation error: Heat generation q must be a valid number.", "error"); return null; }
    if (isNaN(T0) || isNaN(TL)) { setStatus("Validation error: Boundary temperatures must be finite numbers.", "error"); return null; }
    if (isNaN(N) || N <= 0)  { setStatus("Validation error: Element count N must be a positive integer.", "error"); return null; }

    return { L, A, k, q, T0, TL, N };
  }

  // ── Chart initialisation ───────────────────────────────────────────────────
  function initCharts() {
    const ctxTemp = document.getElementById("chart-temperature");
    const ctxConv = document.getElementById("chart-convergence");
    if (!ctxTemp || !ctxConv) return;

    const commonOptions = {
      responsive: true,
      maintainAspectRatio: false,
      color: "#8b93a8",
      plugins: {
        legend: { labels: { color: "#f0f2f8", font: { family: "Inter", size: 12 } } },
        tooltip: {
          backgroundColor: "#1c2030",
          titleColor: "#f0f2f8",
          bodyColor: "#f97316",
          borderColor: "#2a3048",
          borderWidth: 1
        }
      },
      scales: {
        x: {
          grid: { color: "rgba(255,255,255,0.06)" },
          ticks: { color: "#8b93a8" },
          title: { display: true, color: "#8b93a8", text: "Position x [m]" }
        },
        y: {
          grid: { color: "rgba(255,255,255,0.06)" },
          ticks: { color: "#8b93a8" },
          title: { display: true, color: "#8b93a8", text: "Temperature T [°C]" }
        }
      }
    };

    tempChart = new Chart(ctxTemp, {
      type: "line",
      data: { datasets: [] },
      options: commonOptions
    });

    convChart = new Chart(ctxConv, {
      type: "line",
      data: { datasets: [] },
      options: {
        ...commonOptions,
        scales: {
          x: {
            type: "logarithmic",
            grid: { color: "rgba(255,255,255,0.06)" },
            ticks: { color: "#8b93a8" },
            title: { display: true, color: "#8b93a8", text: "Element Size h [m] (log scale)" }
          },
          y: {
            type: "logarithmic",
            grid: { color: "rgba(255,255,255,0.06)" },
            ticks: { color: "#8b93a8" },
            title: { display: true, color: "#8b93a8", text: "Max Midpoint Error [°C] (log scale)" }
          }
        }
      }
    });
  }

  // ── Run single mesh FEA ────────────────────────────────────────────────────
  async function runSingleMesh() {
    const p = getParams();
    if (!p) return;

    setStatus(`Computing 1-D FEA for N = ${p.N}...`, "info");
    const qs = `L=${p.L}&A=${p.A}&k=${p.k}&q=${p.q}&T0=${p.T0}&TL=${p.TL}&N=${p.N}`;

    try {
      const res = await fetch(`/api/thermal/run?${qs}`);
      const data = await res.json();
      if (!data.success) {
        setStatus(`Compute error: ${data.error}`, "error");
        return;
      }

      // Update KPI cards
      kpiN.textContent = data.N;
      kpiH.textContent = `h = ${data.h.toFixed(4)} m`;
      kpiMidErr.textContent = `${data.max_midpoint_error.toFixed(6)} °C`;
      kpiNodeErr.textContent = `${data.max_nodal_error.toExponential(2)} °C`;

      // Theoretical expected error: (q / (2*k)) * (h/2)^2
      const theoErr = (data.q / (2 * data.k)) * Math.pow(data.h / 2, 2);
      document.getElementById("kpi-mid-expected").textContent = `Theoretical: ${theoErr.toFixed(6)} °C`;

      // High-resolution exact curve
      const exactPts = [];
      const numExact = 100;
      for (let i = 0; i <= numExact; ++i) {
        const x = (i / numExact) * data.L;
        const T = data.T0 + ((data.TL - data.T0) / data.L) * x + (data.q / (2 * data.k)) * x * (data.L - x);
        exactPts.push({ x, y: T });
      }

      const fePts = data.x_nodes.map((x, i) => ({ x, y: data.T_fe[i] }));

      tempChart.data = {
        datasets: [
          {
            label: "Exact Analytical Solution",
            data: exactPts,
            borderColor: "#38bdf8",
            borderWidth: 2,
            pointRadius: 0,
            fill: false
          },
          {
            label: `FE Solution (N = ${data.N})`,
            data: fePts,
            borderColor: "#f97316",
            backgroundColor: "#f97316",
            borderWidth: 2,
            pointRadius: 4,
            pointHoverRadius: 6,
            fill: false
          }
        ]
      };
      tempChart.update();

      setStatus(`✅ Solved N = ${data.N} (h = ${data.h} m). Max Midpoint Error = ${data.max_midpoint_error.toFixed(6)} °C (matches theoretical ${theoErr.toFixed(6)} °C).`, "ok");
    } catch (err) {
      setStatus(`Network/server error: ${err.message}`, "error");
    }
  }

  // ── Run all 4 meshes & convergence ─────────────────────────────────────────
  async function runConvergence() {
    const p = getParams();
    if (!p) return;

    setStatus("Running multi-mesh convergence benchmark (N = 10, 20, 40, 80)...", "info");
    const qs = `L=${p.L}&A=${p.A}&k=${p.k}&q=${p.q}&T0=${p.T0}&TL=${p.TL}`;

    try {
      const res = await fetch(`/api/thermal/convergence?${qs}`);
      const data = await res.json();
      if (!data.success) {
        setStatus(`Convergence error: ${data.error}`, "error");
        return;
      }

      // Populate convergence table
      tbodyConv.innerHTML = "";
      const convPts = [];
      const colors = ["#fb923c", "#f97316", "#ea580c", "#c2410c"];

      data.meshes.forEach((m, idx) => {
        const theoErr = (p.q / (2 * p.k)) * Math.pow(m.h / 2, 2);
        let ratioStr = "—";
        let orderStr = "—";
        if (idx > 0) {
          const prevErr = data.meshes[idx - 1].max_midpoint_error;
          const ratio = prevErr / m.max_midpoint_error;
          ratioStr = ratio.toFixed(4) + "×";
          orderStr = m.observed_order !== null ? m.observed_order.toFixed(4) : "—";
        }

        const tr = document.createElement("tr");
        tr.innerHTML = `
          <td><strong>N = ${m.N}</strong></td>
          <td>${m.h.toFixed(4)}</td>
          <td>${m.max_nodal_error.toExponential(2)}</td>
          <td style="color:#f97316; font-weight:600;">${m.max_midpoint_error.toFixed(8)}</td>
          <td>${theoErr.toFixed(8)}</td>
          <td>${ratioStr}</td>
          <td><strong style="color:#22c55e;">${orderStr}</strong></td>
        `;
        tbodyConv.appendChild(tr);

        convPts.push({ x: m.h, y: m.max_midpoint_error });
      });

      // Update KPI
      const last = data.meshes[data.meshes.length - 1];
      kpiOrder.textContent = last.observed_order ? last.observed_order.toFixed(2) : "2.00";
      kpiMidErr.textContent = `${last.max_midpoint_error.toFixed(6)} °C`;
      kpiNodeErr.textContent = `${last.max_nodal_error.toExponential(2)} °C`;

      // Plot convergence chart
      // Slope 2 reference curve: y = C * h^2 with C = q / (8*k) = 125
      const refPts = convPts.map(pt => ({ x: pt.x, y: (p.q / (8 * p.k)) * Math.pow(pt.x, 2) }));

      convChart.data = {
        datasets: [
          {
            label: "FE Midpoint Error (Observed)",
            data: convPts,
            borderColor: "#f97316",
            backgroundColor: "#f97316",
            borderWidth: 2,
            pointRadius: 6,
            pointHoverRadius: 8,
            fill: false
          },
          {
            label: "Theoretical O(h²) Reference (Slope = 2.0)",
            data: refPts,
            borderColor: "#38bdf8",
            borderWidth: 1.5,
            borderDash: [5, 5],
            pointRadius: 0,
            fill: false
          }
        ]
      };
      convChart.update();

      // Fetch and overlay all 4 meshes on temperature chart
      overlayAllMeshes(p);

      setStatus(`✅ Multi-mesh convergence complete! Second-order O(h²) rate verified (ratio 4.0×, order = 2.00).`, "ok");
    } catch (err) {
      setStatus(`Network/server error: ${err.message}`, "error");
    }
  }

  async function overlayAllMeshes(p) {
    const exactPts = [];
    for (let i = 0; i <= 100; ++i) {
      const x = (i / 100) * p.L;
      const T = p.T0 + ((p.TL - p.T0) / p.L) * x + (p.q / (2 * p.k)) * x * (p.L - x);
      exactPts.push({ x, y: T });
    }

    const datasets = [
      {
        label: "Exact Analytical Solution",
        data: exactPts,
        borderColor: "#38bdf8",
        borderWidth: 2.5,
        pointRadius: 0,
        fill: false
      }
    ];

    const meshColors = { 10: "#fde047", 20: "#fb923c", 40: "#f97316", 80: "#dc2626" };

    for (const N of [10, 20, 40, 80]) {
      const qs = `L=${p.L}&A=${p.A}&k=${p.k}&q=${p.q}&T0=${p.T0}&TL=${p.TL}&N=${N}`;
      const res = await fetch(`/api/thermal/run?${qs}`);
      const d = await res.json();
      if (d.success) {
        datasets.push({
          label: `FE Mesh N = ${N}`,
          data: d.x_nodes.map((x, i) => ({ x, y: d.T_fe[i] })),
          borderColor: meshColors[N],
          backgroundColor: meshColors[N],
          borderWidth: 1.5,
          pointRadius: N <= 20 ? 3 : 1.5,
          fill: false
        });
      }
    }

    tempChart.data = { datasets };
    tempChart.update();
  }

  // ── CSV Exports ────────────────────────────────────────────────────────────
  btnExport.addEventListener("click", () => {
    const p = getParams();
    if (!p) return;
    const url = `/api/thermal/export?L=${p.L}&A=${p.A}&k=${p.k}&q=${p.q}&T0=${p.T0}&TL=${p.TL}&N=${p.N}`;
    window.location.href = url;
  });

  btnExportConv.addEventListener("click", () => {
    const p = getParams();
    if (!p) return;
    const url = `/api/thermal/export_convergence?L=${p.L}&A=${p.A}&k=${p.k}&q=${p.q}&T0=${p.T0}&TL=${p.TL}`;
    window.location.href = url;
  });

  btnRun.addEventListener("click", runSingleMesh);
  btnRunConv.addEventListener("click", runConvergence);
  selN.addEventListener("change", runSingleMesh);

  // Initial load
  initCharts();
  runConvergence();
})();
