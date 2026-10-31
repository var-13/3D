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
