// src/server/main.cpp
// Thin HTTP server for ThermoBench.
// ALL maths is in src/core – this file does routing and JSON only.
//
// Phase 0: serves ui/index.html + static assets; one /api/version endpoint.

// Pull in the full httplib implementation exactly once here.
// CPPHTTPLIB_OPENSSL_SUPPORT=0 is set via CMake target_compile_definitions.
#include "httplib.h"


#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "core/fem1d.hpp"

namespace fs = std::filesystem;

// ── Helpers ─────────────────────────────────────────────────────────────────

// Read a whole file into a string. Returns empty string on failure.
static std::string read_file(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) return {};
    return {std::istreambuf_iterator<char>(f),
            std::istreambuf_iterator<char>()};
}

// Guess MIME type from file extension.
static std::string mime(const std::string& ext) {
    if (ext == ".html") return "text/html; charset=utf-8";
    if (ext == ".css")  return "text/css";
    if (ext == ".js")   return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".svg")  return "image/svg+xml";
    if (ext == ".png")  return "image/png";
    if (ext == ".ico")  return "image/x-icon";
    return "application/octet-stream";
}

// ── Main ─────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // The server expects to be run from the project root so that ui/ is reachable.
    // argv[1] can override the UI directory for packaging.
    fs::path ui_dir = (argc > 1) ? argv[1] : "ui";
    int port = (argc > 2) ? std::stoi(argv[2]) : 8080;

    if (!fs::exists(ui_dir)) {
        std::cerr << "[ThermoBench] ERROR: UI directory not found: "
                  << fs::absolute(ui_dir) << "\n";
        return 1;
    }

    httplib::Server svr;

    // ── /api/version ─────────────────────────────────────────────────────
    svr.Get("/api/version", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            "{\"version\":\"" + thermo::version() + "\"}",
            "application/json"
        );
    });

    // ── /health  (used by launcher to detect readiness) ──────────────────
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    // ── Static file serving ───────────────────────────────────────────────
    svr.Get("/.*", [&ui_dir](const httplib::Request& req, httplib::Response& res) {
        // Strip leading '/'
        std::string rel = req.path.substr(1);
        if (rel.empty()) rel = "index.html";

        fs::path file = ui_dir / rel;
        std::string body = read_file(file);
        if (body.empty()) {
            res.status = 404;
            res.set_content("404 Not Found: " + rel, "text/plain");
            return;
        }
        std::string ext = file.extension().string();
        res.set_content(body, mime(ext));
    });

    std::cout << "[ThermoBench] Serving " << fs::absolute(ui_dir)
              << " on http://localhost:" << port << "\n";
    std::cout << "[ThermoBench] Press Ctrl+C to stop.\n";

    if (!svr.listen("localhost", port)) {
        std::cerr << "[ThermoBench] ERROR: Could not bind to port " << port
                  << ". Is something else running there?\n";
        return 1;
    }
    return 0;
}
