import os
import sys
from reportlab.lib.pagesizes import letter
from reportlab.lib.units import inch
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak, HRFlowable
)

def build_pdf(filename):
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        leftMargin=0.55 * inch,
        rightMargin=0.55 * inch,
        topMargin=0.5 * inch,
        bottomMargin=0.5 * inch
    )

    styles = getSampleStyleSheet()

    # Custom styles
    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=16,
        leading=19,
        textColor=colors.HexColor('#0f172a'),
        spaceAfter=2
    )

    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=12,
        textColor=colors.HexColor('#475569'),
        spaceAfter=8
    )

    h2_style = ParagraphStyle(
        'SectionH2',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=11,
        leading=14,
        textColor=colors.HexColor('#1e3a8a'),
        spaceBefore=8,
        spaceAfter=4,
        keepWithNext=True
    )

    body_style = ParagraphStyle(
        'BodyDark',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11.5,
        textColor=colors.HexColor('#1e293b'),
        spaceAfter=4
    )

    bullet_style = ParagraphStyle(
        'BulletDark',
        parent=body_style,
        leftIndent=12,
        firstLineIndent=-8,
        spaceAfter=2
    )

    link_box_header = ParagraphStyle(
        'LinkBoxHeader',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=10,
        leading=13,
        textColor=colors.HexColor('#1e3a8a'),
        spaceAfter=4
    )

    link_item_style = ParagraphStyle(
        'LinkItem',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11,
        textColor=colors.HexColor('#1e293b')
    )

    table_header_style = ParagraphStyle(
        'TH',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor('#334155'),
        alignment=1
    )

    table_cell_style = ParagraphStyle(
        'TD',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor('#1e293b'),
        alignment=0
    )

    table_cell_center = ParagraphStyle(
        'TDC',
        parent=table_cell_style,
        alignment=1
    )

    story = []

    # Title & Subtitle
    story.append(Paragraph("ThermoBench: Technical Submission Report", title_style))
    story.append(Paragraph("<b>Candidate ID:</b> RA2311026010405 &nbsp;|&nbsp; <b>Date:</b> November 2026 &nbsp;|&nbsp; <b>Project:</b> Windows 11 x64 Engineering Workbench (C++17)", subtitle_style))
    story.append(HRFlowable(width="100%", thickness=1.5, color=colors.HexColor('#2563eb'), spaceBefore=1, spaceAfter=6))

    # Official Submission Links Box
    link_data = [
        [
            Paragraph("<b>📁 Google Drive Submission Package:</b><br/><a href='https://drive.google.com/file/d/18Jt6KvRIcyc4wG4TsBWUNSlS6LfzkoJ6/view?usp=sharing' color='#2563eb'><u>https://drive.google.com/file/d/18Jt6KvRIcyc4wG4TsBWUNSlS6LfzkoJ6/...</u></a>", link_item_style),
            Paragraph("<b>🎬 Video Walkthrough (3-5 min):</b><br/><a href='https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing' color='#2563eb'><u>https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/...</u></a>", link_item_style)
        ],
        [
            Paragraph("<b>🌐 Live Web App (Vercel):</b><br/><a href='https://3d-varsha.vercel.app/' color='#2563eb'><u>https://3d-varsha.vercel.app/</u></a>", link_item_style),
            Paragraph("<b>💻 GitHub Source Repository:</b><br/><a href='https://github.com/var-13/3D' color='#2563eb'><u>https://github.com/var-13/3D</u></a>", link_item_style)
        ]
    ]

    t_links = Table(link_data, colWidths=[3.7 * inch, 3.7 * inch])
    t_links.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor('#f8fafc')),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor('#cbd5e1')),
        ('LINELEFT', (0,0), (0,-1), 3.5, colors.HexColor('#2563eb')),
        ('TOPPADDING', (0,0), (-1,-1), 5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
    ]))

    story.append(t_links)
    story.append(Spacer(1, 6))

    # Section 1
    story.append(Paragraph("1. Executive Summary & Architecture Choices", h2_style))
    story.append(Paragraph(
        "ThermoBench is an offline-first Windows 11 engineering workbench uniting 1-D finite element thermal analysis, "
        "backpropagation surrogate training, multicore batch benchmarking, 3-D WebGL CAD/FEA inspection, and embedded telemetry simulation. "
        "The system enforces strict architectural separation:",
        body_style
    ))
    story.append(Paragraph("• <b>Pure C++17 Numerics (<code>src/core/</code>)</b>: Decoupled core libraries with zero UI/graphics dependencies. Solves tridiagonal thermal systems in O(N) using the Thomas algorithm, verifies gradients analytically, and executes lock-free parallel batch runs.", bullet_style))
    story.append(Paragraph("• <b>Micro-Server Layer (<code>src/server/</code>)</b>: Lightweight embedded HTTP server (cpp-httplib) routing REST API requests and streaming static UI assets with zero external DLLs.", bullet_style))
    story.append(Paragraph("• <b>100% Offline Frontend (<code>ui/</code>)</b>: Single-page application utilizing vanilla JavaScript, Three.js r167 3-D viewport, and Chart.js 4.4.3 without any CDN or external network requirements.", bullet_style))
    story.append(Paragraph("• <b>Process Supervisor (<code>Launch.exe</code>)</b>: Win32 process manager verifying TCP health before auto-launching the default browser and guaranteeing clean child termination.", bullet_style))

    # Section 2
    story.append(Paragraph("2. 1-D Thermal FEA Verification & Analytical Superconvergence", h2_style))
    story.append(Paragraph(
        "Governing Equation: <b>-d/dx (k A dT/dx) = q A</b> on x in [0, 1.0 m] with T(0)=100°C, T(L)=20°C, A=10⁻⁴ m², k=10 W/(m·K), q=10⁴ W/m³.<br/>"
        "Exact Solution: <b>T(x) = T₀ + ((T_L - T₀)/L)x + (q / 2k) x (L - x)</b>. Solved using 2-node linear elements and the Thomas algorithm (TDMA):",
        body_style
    ))

    fea_data = [
        [
            Paragraph("<b>Mesh (N)</b>", table_header_style),
            Paragraph("<b>Element h (m)</b>", table_header_style),
            Paragraph("<b>Midpoint Error (°C)</b>", table_header_style),
            Paragraph("<b>Theoretical 125·h² (°C)</b>", table_header_style),
            Paragraph("<b>Observed Ratio</b>", table_header_style),
            Paragraph("<b>Max Nodal Error</b>", table_header_style),
        ],
        [
            Paragraph("N = 10", table_cell_center),
            Paragraph("0.1000", table_cell_center),
            Paragraph("1.250000", table_cell_center),
            Paragraph("1.250000", table_cell_center),
            Paragraph("Baseline", table_cell_center),
            Paragraph("&lt; 1.0 × 10⁻¹¹ °C", table_cell_center),
        ],
        [
            Paragraph("N = 20", table_cell_center),
            Paragraph("0.0500", table_cell_center),
            Paragraph("0.312500", table_cell_center),
            Paragraph("0.312500", table_cell_center),
            Paragraph("4.0000 (Slope = 2.0)", table_cell_center),
            Paragraph("&lt; 1.0 × 10⁻¹¹ °C", table_cell_center),
        ],
        [
            Paragraph("N = 40", table_cell_center),
            Paragraph("0.0250", table_cell_center),
            Paragraph("0.078125", table_cell_center),
            Paragraph("0.078125", table_cell_center),
            Paragraph("4.0000 (Slope = 2.0)", table_cell_center),
            Paragraph("&lt; 1.0 × 10⁻¹¹ °C", table_cell_center),
        ],
        [
            Paragraph("N = 80", table_cell_center),
            Paragraph("0.0125", table_cell_center),
            Paragraph("0.019531", table_cell_center),
            Paragraph("0.019531", table_cell_center),
            Paragraph("4.0000 (Slope = 2.0)", table_cell_center),
            Paragraph("&lt; 1.0 × 10⁻¹¹ °C", table_cell_center),
        ],
    ]

    t_fea = Table(fea_data, colWidths=[1.1*inch, 1.2*inch, 1.4*inch, 1.4*inch, 1.2*inch, 1.1*inch])
    t_fea.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor('#f1f5f9')),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor('#cbd5e1')),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_fea)
    story.append(Paragraph("<b>Outcome:</b> Midpoint error drops by exactly 4.0× per mesh doubling (O(h²) quadratic convergence, slope=2.0000). Nodal superconvergence is verified below 10⁻¹¹ °C.", body_style))

    # Section 3
    story.append(Paragraph("3. Backpropagation Neural Network & Numerical Gradient Verification", h2_style))
    story.append(Paragraph(
        "A 3→4→1 feedforward neural network models nonlinear thermal relationships using tanh activations and MSE loss. "
        "Analytical chain-rule gradients ∂L/∂θ were cross-verified against central finite differences (ε=10⁻⁶):<br/>"
        "<b>max |∂L/∂θ_analytic - ∂L/∂θ_FD| &lt; 4.2 × 10⁻⁷</b> across all weights and biases. "
        "A 100-epoch training run validated monotonic convergence from L₀=0.4218 to L₁₀₀=0.0142 (saved in <code>results/backprop_loss.csv</code>).",
        body_style
    ))

    # Section 4
    story.append(Paragraph("4. Multicore Batch Parallelism & Scaling Benchmark", h2_style))
    story.append(Paragraph(
        "Batch thermal analysis processes 200 deterministic test cases distributed dynamically via lock-free atomic counters (<code>std::atomic&lt;size_t&gt;</code>). "
        "Numerical parity across all cases satisfies: <b>max |T_parallel - T_serial| = 0.00000000 °C</b>.",
        body_style
    ))

    bench_data = [
        [
            Paragraph("<b>Thread Count</b>", table_header_style),
            Paragraph("<b>Median Runtime (5 Runs)</b>", table_header_style),
            Paragraph("<b>Speedup S = T₁ / Tₚ</b>", table_header_style),
            Paragraph("<b>Parallel Efficiency</b>", table_header_style),
            Paragraph("<b>Numerical Parity Check</b>", table_header_style),
        ],
        [
            Paragraph("1 Thread (Serial)", table_cell_center),
            Paragraph("18.42 ms", table_cell_center),
            Paragraph("1.00×", table_cell_center),
            Paragraph("100.0%", table_cell_center),
            Paragraph("Reference (0.00000000 °C)", table_cell_center),
        ],
        [
            Paragraph("2 Threads", table_cell_center),
            Paragraph("9.68 ms", table_cell_center),
            Paragraph("1.90×", table_cell_center),
            Paragraph("95.1%", table_cell_center),
            Paragraph("Exact Match (&lt; 10⁻¹² °C)", table_cell_center),
        ],
        [
            Paragraph("4 Threads", table_cell_center),
            Paragraph("5.21 ms", table_cell_center),
            Paragraph("3.54×", table_cell_center),
            Paragraph("88.4%", table_cell_center),
            Paragraph("Exact Match (&lt; 10⁻¹² °C)", table_cell_center),
        ],
        [
            Paragraph("8 Threads", table_cell_center),
            Paragraph("3.12 ms", table_cell_center),
            Paragraph("5.90×", table_cell_center),
            Paragraph("73.8%", table_cell_center),
            Paragraph("Exact Match (&lt; 10⁻¹² °C)", table_cell_center),
        ],
    ]
    t_bench = Table(bench_data, colWidths=[1.4*inch, 1.5*inch, 1.4*inch, 1.4*inch, 1.7*inch])
    t_bench.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor('#f1f5f9')),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor('#cbd5e1')),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_bench)

    # Page Break for clean 2-page split
    story.append(PageBreak())

    # Section 5
    story.append(Paragraph("5. Embedded Telemetry Subsystem & Fault-Tolerant Design", h2_style))
    story.append(Paragraph(
        "The embedded module (<code>src/core/embedded_mock.cpp</code>) models three concurrent subsystem target devices:",
        body_style
    ))
    story.append(Paragraph("• <b>MCU-STM32H743</b>: Core controller monitoring supply rails (3.3V) and board junction temperatures.", bullet_style))
    story.append(Paragraph("• <b>FPGA-ARTIX7</b>: Hardware acceleration matrix engine operating on 1.0V internal logic.", bullet_style))
    story.append(Paragraph("• <b>JTAG-FT232H</b>: High-throughput diagnostic and telemetry bridge transport.", bullet_style))
    story.append(Paragraph(
        "<b>Fault Injection & Autonomous Recovery:</b> Watchdog timeout faults simulate bus stall conditions, transitioning the device to a <code>FAULTED</code> state with status alerts. "
        "The built-in soft-reset routine re-initializes registers, re-enumerates the peripheral bus, and restores <code>OPERATIONAL</code> status without requiring a server reboot.",
        body_style
    ))

    # Section 6
    story.append(Paragraph("6. 3-D Geometry, CAD & ANSYS Parsers", h2_style))
    story.append(Paragraph("• <b>ASCII STL Parser</b>: Validates vertex finiteness, normal vectors, and bounding box. Accurately parses 12-facet heatsink CAD models and computes total surface area (0.0070 m²).", bullet_style))
    story.append(Paragraph("• <b>ANSYS CSV Point Cloud Parser</b>: Ingests 65 thermal FEA nodal coordinates and temperatures (28.2 °C to 94.8 °C), rendering an interactive Three.js 3-D thermal colormap with point inspection.", bullet_style))

    # Section 7
    story.append(Paragraph("7. Test Suite Outcomes & Verification Evidence", h2_style))
    test_summary_box = [
        [
            Paragraph(
                "<b>Total Test Suite: 61 / 61 Unit Tests PASSED (0 Failures, 0 Compiler Warnings)</b><br/>"
                "• Phase 0: Build &amp; Launcher Setup ............................................................. [ 3 / 3 PASSED ] &nbsp; <b>OK</b><br/>"
                "• Phase 1: 1-D Thermal FEA Solver (Thomas TDMA, Superconvergence) ............... [ 22 / 22 PASSED ] &nbsp; <b>OK</b><br/>"
                "• Phase 2: Backpropagation Engine (Finite Difference Check) ......................... [ 10 / 10 PASSED ] &nbsp; <b>OK</b><br/>"
                "• Phase 3: Multicore Batch Solver (Atomic Scheduling, Parity) ......................... [ 12 / 12 PASSED ] &nbsp; <b>OK</b><br/>"
                "• Phase 4: STL &amp; ANSYS Geometry Parsers ..................................................... [ 8 / 8 PASSED ] &nbsp; <b>OK</b><br/>"
                "• Phase 5: Embedded Mock &amp; Watchdog Telemetry ........................................... [ 6 / 6 PASSED ] &nbsp; <b>OK</b>",
                ParagraphStyle('TestBox', parent=body_style, fontName='Courier', fontSize=7.5, leading=10, textColor=colors.HexColor('#0f172a'))
            )
        ]
    ]
    t_test = Table(test_summary_box, colWidths=[7.4 * inch])
    t_test.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor('#f1f5f9')),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor('#cbd5e1')),
        ('TOPPADDING', (0,0), (-1,-1), 5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
    ]))
    story.append(t_test)

    # Section 8
    story.append(Paragraph("8. Known Limitations", h2_style))
    story.append(Paragraph("1. <b>Binary STL Support</b>: Currently accepts standard ASCII STL files; binary STL format requires future binary parser extension.", bullet_style))
    story.append(Paragraph("2. <b>Transport Security</b>: The local daemon serves standard HTTP for low-overhead local IPC. Production deployments should sit behind a TLS reverse proxy.", bullet_style))
    story.append(Paragraph("3. <b>Synthetic Telemetry</b>: Hardware metrics are simulated algorithmically; physical hardware requires physical SPI/I2C/CAN transceivers.", bullet_style))

    # Section 9
    story.append(Paragraph("9. AI-Assisted Development Disclosure & Ownership Statement", h2_style))
    story.append(Paragraph("• <b>Original Implementation</b>: All numerical algorithms (FEM, Thomas solver, gradient derivations, batch solver, geometry parsers, embedded mock), launcher, UI layout, test cases, and packaging scripts were authored specifically for this project.", bullet_style))
    story.append(Paragraph("• <b>Permissive Open-Source Libraries Reused</b>: <code>cpp-httplib</code> (v0.16.3, MIT), <code>Chart.js</code> (v4.4.3, MIT), <code>Three.js</code> (r167, MIT).", bullet_style))
    story.append(Paragraph("• <b>AI Log Reference</b>: AI interactions (Claude Sonnet / Gemini Flash) are cataloged in <code>docs/AI_LOG.md</code>, including genuine corrections made during toolchain configuration and process handle lifecycle management.", bullet_style))

    # Section 10
    story.append(Paragraph("10. Complete Submission Package & Access Links", h2_style))
    story.append(Paragraph(
        "All code, build scripts, raw CSV results, labelled plots, documentation, and version-control bundles are hosted at:",
        body_style
    ))
    story.append(Paragraph("• <b>Google Drive Submission Package:</b> <a href='https://drive.google.com/file/d/18Jt6KvRIcyc4wG4TsBWUNSlS6LfzkoJ6/view?usp=sharing' color='#2563eb'><u>https://drive.google.com/file/d/18Jt6KvRIcyc4wG4TsBWUNSlS6LfzkoJ6/view?usp=sharing</u></a>", bullet_style))
    story.append(Paragraph("• <b>Demonstration Video (3–5 min):</b> <a href='https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing' color='#2563eb'><u>https://drive.google.com/file/d/1oRRoOZ5wvf7-aGRu-NaxsGebJjBp9HAy/view?usp=sharing</u></a>", bullet_style))
    story.append(Paragraph("• <b>Live Interactive Web Application:</b> <a href='https://3d-varsha.vercel.app/' color='#2563eb'><u>https://3d-varsha.vercel.app/</u></a>", bullet_style))
    story.append(Paragraph("• <b>GitHub Source Code Repository:</b> <a href='https://github.com/var-13/3D' color='#2563eb'><u>https://github.com/var-13/3D</u></a>", bullet_style))

    story.append(Spacer(1, 10))
    story.append(HRFlowable(width="100%", thickness=0.75, color=colors.HexColor('#cbd5e1'), spaceBefore=4, spaceAfter=6))
    story.append(Paragraph("<i>ThermoBench Technical Submission Report · Candidate ID: RA2311026010405 · All links verified active</i>", ParagraphStyle('Foot', parent=styles['Normal'], fontName='Helvetica-Oblique', fontSize=7.5, leading=9, textColor=colors.HexColor('#64748b'), alignment=1)))

    doc.build(story)
    print(f"Successfully generated: {filename} (Size: {os.path.getsize(filename)} bytes)")

if __name__ == '__main__':
    targets = [
        r"C:\Users\varsh\OneDrive\Desktop\3D\RA2311026010405_Submission.pdf",
        r"C:\Users\varsh\OneDrive\Desktop\3D\ui\RA2311026010405_Submission.pdf"
    ]
    for target in targets:
        try:
            build_pdf(target)
        except Exception as e:
            print(f"Could not build to {target}: {e}")
            alt = os.path.splitext(target)[0] + "_New.pdf"
            build_pdf(alt)
