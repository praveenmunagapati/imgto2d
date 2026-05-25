Summary
-------
This document maps the repository's Path Finding Modules (PFMs) and related export features against the official DrawingBot V3 docs (source: docs.drawingbotv3.com/_sources/pfms.rst.txt).

High-level findings
- The repo contains comprehensive Python and C++ PFM implementations spanning Sketch, Streamline, Spiral, Hatch, Adaptive, LBG, Voronoi, Grid, Composite, Mosaic and Special PFMs.
- The C++/Qt app now registers the documented LBG family, 65 image filters, project save/load, pen editing, video frame import, vpype SVG optimization, masks, batch processing, and SVG/PDF/HPGL/G-code export.
- Qt serial plotter source is implemented behind optional `Qt6SerialPort`; it is enabled when that Qt component is installed.
- Bit-identical output vs. official DrawingBot V3 is not claimed or required.

Repository PFM modules -> Official PFM coverage (summary)
- app/pfm/adaptive_pfms.py -> Adaptive Circular Scribbles, Adaptive Shapes, Adaptive Triangulation, Adaptive Tree, Adaptive Stippling, Adaptive Dashes, Adaptive Letters, Adaptive Diagram, Adaptive TSP
- app/pfm/composite_pfms.py -> Composite PFMs, Mosaic Rectangles, Mosaic Voronoi, Mosaic Triangulation, Mosaic Segments, Layers PFM
- app/pfm/grid_pfms.py -> Grid Shapes, Grid Dashes, Grid Letters
- app/pfm/hatch_extras.py, app/pfm/hatch_lines.py, app/pfm/hatch_premium.py -> Hatch Sawtooth, Hatch Circular Scribbles, Hatch Fill, related hatch styles
- app/pfm/lbg_pfms.py -> LBG Circular Scribbles, LBG Shapes, LBG Triangulation, LBG Tree, LBG Stippling, LBG Dashes, LBG Letters, LBG Diagram, LBG TSP
- app/pfm/letters_pfms.py -> Adaptive Letters / Letter-style PFMs
- app/pfm/maze_tsp_pfms.py -> Maze / TSP style PFMs (TSP variants)
- app/pfm/mosaic_pfms.py -> Mosaic variants (rectangles, voronoi, triangulation, segments)
- app/pfm/pfm_utils.py -> Utility functions used across PFMs
- app/pfm/sketch_beziers.py -> Sketch Quad Beziers, Sketch Cubic Beziers
- app/pfm/sketch_curves.py -> Sketch Curves, Catmull-Rom variants
- app/pfm/sketch_extras.py -> Sketch Sweeping Curves, Flow Field helpers, Superformula utilities
- app/pfm/sketch_lines.py -> Sketch Lines
- app/pfm/sketch_shapes.py -> Sketch Shapes
- app/pfm/sketch_special.py -> Sketch Sobel Edges and related special sketch variants
- app/pfm/sketch_spirals.py & app/pfm/spiral_dbv3.py -> Spiral PFMs (Spiral Sawtooth, Spiral Circular Scribbles, Spiral family)
- app/pfm/sketch_squares.py -> Sketch Squares
- app/pfm/sketch_waves.py -> Sketch Waves
- app/pfm/special_pfms.py -> ECS Drawing, SVG Converter, Pen Calibration, and other Special PFMs
- app/pfm/stipple_dots.py, app/pfm/stipple_extras.py -> Stippling PFMs
- app/pfm/streamline_pfms.py -> Streamlines (Edge Field, Flow Field, Superformula, other streamline variants)
- app/pfm/voronoi_pfms.py -> Voronoi Shapes, Voronoi Triangulation, Voronoi Tree, Voronoi Stippling, Voronoi Dashes, Voronoi Diagram, Voronoi TSP

Notable gaps & caveats
- At the module level the repo covers nearly all PFMs documented in the official source text. Some named variants in the docs (e.g., small preset names, tutorial-specific presets) may be implemented as parameter sets rather than distinct modules.
- Exact one-to-one name parity (literal names) requires an automated textual comparison of doc-listed PFM names vs. an extracted module-based registry; this file provides a human-reviewed mapping.
- Runtime validation (processing outputs) has not been performed: behaviour/perceptual parity should be validated by running the PFMs on sample images and comparing outputs.

Export & UI parity
- VPype exporter: implemented in Python and C++/Qt. C++ uses a temporary SVG and external `vpype` command.
- Qt serial plotter: implemented in source behind optional `Qt6SerialPort`.
- G-code export is implemented in both Python and C++.
- C++ project save/load, pen editor, video frame import, masks, batch processing, PDF/HPGL/SVG/G-code export, and filter stack are implemented.

Recommended next steps
1) Perform qualitative runtime checks for several PFMs on sample images.
2) Test serial streaming on a machine with `Qt6SerialPort` installed and a GRBL device or simulator.
3) Test vpype export on a machine with `vpype` installed on `PATH`.
