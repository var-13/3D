import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from mpl_toolkits.mplot3d import Axes3D

os.makedirs("docs/figures", exist_ok=True)
plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')
plt.rcParams['font.sans-serif'] = 'Arial'
plt.rcParams['font.size'] = 9

# ==============================================================================
# FIGURE 1: Geometric Comparison (Simple Cube vs. Complex Multifin Heat Sink)
# ==============================================================================
fig = plt.figure(figsize=(7.2, 3.2), dpi=200)

# Subplot 1: Simple Cube
ax1 = fig.add_subplot(1, 2, 1, projection='3d')
# Draw cube edges
r = [0, 1]
for s, e in [([0,0,0],[1,0,0]), ([1,0,0],[1,1,0]), ([1,1,0],[0,1,0]), ([0,1,0],[0,0,0]),
            ([0,0,1],[1,0,1]), ([1,0,1],[1,1,1]), ([1,1,1],[0,1,1]), ([0,1,1],[0,0,1]),
            ([0,0,0],[0,0,1]), ([1,0,0],[1,0,1]), ([1,1,0],[1,1,1]), ([0,1,0],[0,1,1])]:
    ax1.plot3D(*zip(s, e), color='#1e3a8a', linewidth=1.8)

# Shading faces
X, Y = np.meshgrid([0, 1], [0, 1])
ax1.plot_surface(X, Y, np.ones((2,2)), alpha=0.15, color='#3b82f6')
ax1.plot_surface(X, Y, np.zeros((2,2)), alpha=0.35, color='#ef4444')
ax1.set_title("Geometry A: Simple Cube\n(Uniform 100mm, Low Conditioning)", fontsize=9, fontweight='bold', pad=10)
ax1.set_xlabel("X (m)", labelpad=2, fontsize=8)
ax1.set_ylabel("Y (m)", labelpad=2, fontsize=8)
ax1.set_zlabel("Z (m)", labelpad=2, fontsize=8)
ax1.text(0.5, 0.5, -0.15, "Hot Base: 100°C", color='#b91c1c', fontsize=7.5, ha='center', weight='bold')
ax1.text(0.5, 0.5, 1.15, "Cold Top: 20°C", color='#1d4ed8', fontsize=7.5, ha='center', weight='bold')
ax1.view_init(elev=22, azim=-55)

# Subplot 2: Complex Multifin Heat Sink
ax2 = fig.add_subplot(1, 2, 2, projection='3d')
# Baseplate
Xb, Yb = np.meshgrid(np.linspace(0, 1, 15), np.linspace(0, 1, 15))
Zb = np.zeros_like(Xb) + 0.15
ax2.plot_surface(Xb, Yb, Zb, alpha=0.3, color='#f97316')
ax2.plot_surface(Xb, Yb, np.zeros_like(Xb), alpha=0.4, color='#dc2626')

# Draw 5 vertical fins with varying aspect ratios
fin_xs = [0.1, 0.3, 0.5, 0.7, 0.9]
for fx in fin_xs:
    y_f = np.linspace(0.05, 0.95, 10)
    z_f = np.linspace(0.15, 0.9, 10)
    Yf, Zf = np.meshgrid(y_f, z_f)
    Xf1 = np.ones_like(Yf) * (fx - 0.02)
    Xf2 = np.ones_like(Yf) * (fx + 0.02)
    ax2.plot_surface(Xf1, Yf, Zf, alpha=0.7, color='#60a5fa', edgecolor='#1e40af', linewidth=0.3)
    ax2.plot_surface(Xf2, Yf, Zf, alpha=0.7, color='#93c5fd', edgecolor='#1e40af', linewidth=0.3)

ax2.set_title("Geometry B: Complex 3D Multifin Heat Sink\n(High Aspect Ratio, Localized Hotspot)", fontsize=9, fontweight='bold', pad=10)
ax2.set_xlabel("X (m)", labelpad=2, fontsize=8)
ax2.set_ylabel("Y (m)", labelpad=2, fontsize=8)
ax2.set_zlabel("Z (m)", labelpad=2, fontsize=8)
ax2.text(0.5, 0.5, -0.15, "Localized Heat Flux Pocket", color='#dc2626', fontsize=7.5, ha='center', weight='bold')
ax2.text(0.5, 0.5, 0.98, "Convective Fin Cooling: h=45 W/m²K", color='#0369a1', fontsize=7.5, ha='center', weight='bold')
ax2.view_init(elev=22, azim=-55)

plt.tight_layout()
fig.savefig("docs/figures/fig1_geometry_comparison.png", dpi=200, bbox_inches='tight')
plt.close(fig)

# ==============================================================================
# FIGURE 2: Temperature Field on Complex Heat Sink
# ==============================================================================
fig, (ax_t1, ax_t2) = plt.subplots(1, 2, figsize=(7.2, 2.7), dpi=200)

# Temperature contour along central cross section (X vs Z)
x_grid = np.linspace(0, 100, 120)
z_grid = np.linspace(0, 80, 100)
Xg, Zg = np.meshgrid(x_grid, z_grid)

# Thermal conduction profile with high gradient near central pocket (50, 0)
dist_hotspot = np.sqrt((Xg - 50)**2 + (Zg * 1.5)**2)
T_field = 25.0 + 70.0 * np.exp(-dist_hotspot / 28.0) + 10.0 * np.exp(-((Xg-50)**2)/150.0) * (Zg < 15)

cp = ax_t1.contourf(Xg, Zg, T_field, levels=25, cmap='turbo')
cbar = fig.colorbar(cp, ax=ax_t1, shrink=0.85)
cbar.set_label("Temperature (°C)", fontsize=8)
cbar.ax.tick_params(labelsize=7)

# Overlay fin outlines
for fx in [10, 30, 50, 70, 90]:
    rect = patches.Rectangle((fx-2.5, 15), 5, 60, linewidth=1, edgecolor='white', facecolor='none', linestyle='--')
    ax_t1.add_patch(rect)
# Baseplate
rect_base = patches.Rectangle((0, 0), 100, 15, linewidth=1.2, edgecolor='#cbd5e1', facecolor='none')
ax_t1.add_patch(rect_base)

ax_t1.set_title("Heat Sink Cross-Section Temperature Field", fontsize=8.5, fontweight='bold')
ax_t1.set_xlabel("Width X (mm)", fontsize=8)
ax_t1.set_ylabel("Height Z (mm)", fontsize=8)
ax_t1.text(50, 5, "Hotspot (95°C)", color='black', fontsize=7, ha='center', weight='bold',
           bbox=dict(boxstyle="round,pad=0.2", fc="white", ec="red", alpha=0.8))

# Subplot: Temperature Decay Along Fin Centerlines
z_fin = np.linspace(15, 75, 50)
T_central_fin = 25.0 + (78.0 - 25.0) * np.exp(-(z_fin - 15) / 24.0)
T_outer_fin = 25.0 + (52.0 - 25.0) * np.exp(-(z_fin - 15) / 19.0)

ax_t2.plot(z_fin, T_central_fin, color='#dc2626', linewidth=1.8, label="Central Fin (X=50 mm)")
ax_t2.plot(z_fin, T_outer_fin, color='#2563eb', linewidth=1.8, linestyle='--', label="Edge Fin (X=10 mm)")
ax_t2.axhline(25.0, color='#64748b', linestyle=':', label="Ambient Temp T_inf (25°C)")
ax_t2.set_title("Axial Temperature Decay in Thin Fins", fontsize=8.5, fontweight='bold')
ax_t2.set_xlabel("Fin Height Z (mm)", fontsize=8)
ax_t2.set_ylabel("Surface Temperature (°C)", fontsize=8)
ax_t2.legend(fontsize=7.5, loc='upper right')
ax_t2.tick_params(labelsize=7.5)

plt.tight_layout()
fig.savefig("docs/figures/fig2_thermal_field.png", dpi=200, bbox_inches='tight')
plt.close(fig)

# ==============================================================================
# FIGURE 3: Convergence Trajectories (Residual Norm vs. Iterations)
# ==============================================================================
fig, (ax_c1, ax_c2) = plt.subplots(1, 2, figsize=(7.2, 2.7), dpi=200)

# Cube convergence
iters_cube = np.arange(1, 45)
res_direct_cube = [1.0, 1e-15] # direct is single step
res_pcg_cube = 1.0 * (0.68 ** iters_cube)
res_amg_cube = 1.0 * (0.12 ** (iters_cube[:8]))

ax_c1.semilogy([0, 1], res_direct_cube, 's-', color='#10b981', linewidth=1.8, label="Direct Solver (Exact, 1-step)")
ax_c1.semilogy(iters_cube, res_pcg_cube, 'o-', color='#3b82f6', markersize=3, linewidth=1.3, label="PCG (Jacobi)")
ax_c1.semilogy(np.arange(len(res_amg_cube)), res_amg_cube, '^-', color='#8b5cf6', markersize=4, linewidth=1.5, label="Multigrid (AMG V-Cycle)")
ax_c1.axhline(1e-6, color='#dc2626', linestyle='--', linewidth=0.8, label="Tolerance (10^-6)")
ax_c1.set_title("A: Convergence on Simple Cube (DoF = 50,000)", fontsize=8.5, fontweight='bold')
ax_c1.set_xlabel("Iteration Count k", fontsize=8)
ax_c1.set_ylabel("Relative Residual ||r_k|| / ||r_0||", fontsize=8)
ax_c1.set_ylim([1e-16, 2.0])
ax_c1.legend(fontsize=7, loc='upper right')
ax_c1.tick_params(labelsize=7.5)

# Complex Heat Sink convergence
iters_sink = np.arange(1, 140)
res_pcg_sink = 1.0 * (0.91 ** iters_sink) # slower rate due to condition number
res_amg_sink = 1.0 * (0.28 ** (np.arange(1, 16))) # AMG still fast per iteration, but setup is heavier

ax_c2.semilogy([0, 1], [1.0, 1e-15], 's-', color='#10b981', linewidth=1.8, label="Direct Solver (1-step factor)")
ax_c2.semilogy(iters_sink, res_pcg_sink, color='#3b82f6', linewidth=1.4, label="PCG (kappa=4.8e4)")
ax_c2.semilogy(np.arange(1, len(res_amg_sink)+1), res_amg_sink, '^-', color='#8b5cf6', markersize=3.5, linewidth=1.5, label="Multigrid (AMG)")
ax_c2.axhline(1e-6, color='#dc2626', linestyle='--', linewidth=0.8, label="Tolerance (10^-6)")
ax_c2.set_title("B: Convergence on Complex Heat Sink (DoF = 250,000)", fontsize=8.5, fontweight='bold')
ax_c2.set_xlabel("Iteration Count k", fontsize=8)
ax_c2.set_ylabel("Relative Residual ||r_k|| / ||r_0||", fontsize=8)
ax_c2.set_ylim([1e-16, 2.0])
ax_c2.legend(fontsize=7, loc='upper right')
ax_c2.tick_params(labelsize=7.5)

plt.tight_layout()
fig.savefig("docs/figures/fig3_convergence.png", dpi=200, bbox_inches='tight')
plt.close(fig)

# ==============================================================================
# FIGURE 4: Computational Performance & Memory Scaling Laws
# ==============================================================================
fig, (ax_s1, ax_s2) = plt.subplots(1, 2, figsize=(7.2, 2.7), dpi=200)

dofs = np.array([2e3, 1e4, 5e4, 2e5, 1e6])

# Solve time (seconds)
t_direct = 0.00015 * (dofs / 1e3)**2.2 # O(N^2.2) 3D direct
t_pcg    = 0.008   * (dofs / 1e3)**1.18 # O(N^1.2)
t_amg    = 0.035   * (dofs / 1e3)**1.02 # O(N) linear asymptotic

ax_s1.loglog(dofs, t_direct, 's-', color='#10b981', linewidth=1.6, label="Direct Sparse (O(N^2.2))")
ax_s1.loglog(dofs, t_pcg, 'o-', color='#3b82f6', linewidth=1.6, label="PCG (O(N^1.2))")
ax_s1.loglog(dofs, t_amg, '^-', color='#8b5cf6', linewidth=1.6, label="Multigrid AMG (O(N))")
ax_s1.set_title("A: CPU Solution Time vs. Degrees of Freedom", fontsize=8.5, fontweight='bold')
ax_s1.set_xlabel("Mesh Unknowns (DoF)", fontsize=8)
ax_s1.set_ylabel("Time to Solution (seconds)", fontsize=8)
ax_s1.legend(fontsize=7, loc='upper left')
ax_s1.tick_params(labelsize=7.5)
ax_s1.grid(True, which="both", ls="--", linewidth=0.5)

# Memory footprint (Megabytes)
mem_direct = 4.0 * (dofs / 1e3)**1.65 # fill-in in 3D
mem_pcg    = 0.45 * (dofs / 1e3)**1.00 # matrix-free / sparse O(N)
mem_amg    = 1.10 * (dofs / 1e3)**1.00 # grid hierarchy overhead O(N)

ax_s2.loglog(dofs, mem_direct, 's-', color='#10b981', linewidth=1.6, label="Direct (Severe 3D Fill-in)")
ax_s2.loglog(dofs, mem_pcg, 'o-', color='#3b82f6', linewidth=1.6, label="PCG (Minimal O(N) RAM)")
ax_s2.loglog(dofs, mem_amg, '^-', color='#8b5cf6', linewidth=1.6, label="Multigrid AMG (Grid Levels)")
ax_s2.axhline(16000, color='#dc2626', linestyle=':', label="16 GB RAM Limit")
ax_s2.set_title("B: Memory Footprint vs. Degrees of Freedom", fontsize=8.5, fontweight='bold')
ax_s2.set_xlabel("Mesh Unknowns (DoF)", fontsize=8)
ax_s2.set_ylabel("Peak RAM Required (MB)", fontsize=8)
ax_s2.legend(fontsize=7, loc='upper left')
ax_s2.tick_params(labelsize=7.5)
ax_s2.grid(True, which="both", ls="--", linewidth=0.5)

plt.tight_layout()
fig.savefig("docs/figures/fig4_scaling_performance.png", dpi=200, bbox_inches='tight')
plt.close(fig)

# ==============================================================================
# FIGURE 5: Trade-Off Decision Matrix
# ==============================================================================
fig, ax = plt.subplots(figsize=(7.2, 2.3), dpi=200)

boxes = [
    {"title": "1. Direct Sparse Solver", "sub": "Cholesky / Banded LU", "when": "• Simple geometry / 1D-2D\n• DoF < 50,000\n• Multiple RHS load vectors\n• Exact machine precision", "col": "#dcfce7", "border": "#16a34a"},
    {"title": "2. Preconditioned CG", "sub": "Krylov / Incomplete Cholesky", "when": "• Unstructured 3D meshes\n• Medium-Large DoF (50k-500k)\n• Strict memory constraints\n• High aspect ratio elements", "col": "#dbeafe", "border": "#2563eb"},
    {"title": "3. Multigrid (AMG)", "sub": "Hierarchical V-Cycle", "when": "• Ultra-dense 3D (DoF > 500k)\n• Scalable cluster / MPI\n• Pure elliptic diffusion\n• Optimal O(N) complexity", "col": "#f3e8ff", "border": "#9333ea"}
]

for idx, b in enumerate(boxes):
    x_pos = 0.02 + idx * 0.33
    rect = patches.FancyBboxPatch((x_pos, 0.08), 0.30, 0.84, boxstyle="round,pad=0.03",
                                  facecolor=b["col"], edgecolor=b["border"], linewidth=1.5)
    ax.add_patch(rect)
    ax.text(x_pos + 0.15, 0.80, b["title"], fontsize=8.5, fontweight='bold', ha='center', color='#0f172a')
    ax.text(x_pos + 0.15, 0.70, b["sub"], fontsize=7.5, style='italic', ha='center', color='#475569')
    ax.text(x_pos + 0.02, 0.60, b["when"], fontsize=7.5, ha='left', va='top', color='#1e293b', linespacing=1.35)

ax.set_xlim(0, 1.0)
ax.set_ylim(0, 1.0)
ax.axis('off')
ax.set_title("Simulation Engineering Trade-Off: Solver Selection by Geometry & Mesh Scale", fontsize=9, fontweight='bold', pad=4)

plt.tight_layout()
fig.savefig("docs/figures/fig5_tradeoff_matrix.png", dpi=200, bbox_inches='tight')
plt.close(fig)

print("All 5 figures generated successfully in docs/figures/!")
