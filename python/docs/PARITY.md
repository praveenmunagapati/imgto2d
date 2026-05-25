# DrawingBot V3 parity checklist

Target: **DrawingBot V3 1.6.x** ([documentation](https://docs.drawingbotv3.com/en/latest/))

## C++ / Qt parity status

| Area | Status |
|------|--------|
| PFMs | Done: 103 C++ PFMs registered in the Qt app |
| Image filters | Done: 65 C++ filters registered |
| Masks | Done |
| Batch processing | Done |
| SVG/PDF/HPGL/G-code export | Done |
| Project save/load | Done: `.dbv3` state for image, PFM, PFM settings, filters, mask, preview/export/pen settings |
| Pen editor | Done: pen colors and stroke width affect preview, SVG, and PDF export |
| Video frame import | Done |
| vpype SVG optimization | Done: optional external `vpype` command |
| Serial plotter | Source implemented behind optional `Qt6SerialPort`; enabled when that Qt component is installed |

## Implemented PFM families

| Family | Modules | Notes |
|--------|---------|-------|
| **Sketch** | Lines, Squares, Curves, Catmull-Roms, Shapes, Sobel Edges, Beziers, Waves, Spirals, Flow Fields, Superformula, Radial, Scribble, Abstract, Voronoi, Delaunay | Core + premium-style extras |
| **Hatch** | Lines + multi-angle variants + Sawtooth + Circular Scribbles | |
| **Stipple** | Dots + shape variants + variable variants | |
| **Adaptive** | Stippling, Circular Scribbles, Shapes, Triangulation, Tree, Dashes, Diagram, TSP, Letters | |
| **LBG** | Circular Scribbles, Shapes, Triangulation, Tree, Stippling, Dashes, Diagram, TSP, Letters | Lloyd iterations default to 5 |
| **Voronoi** | Stippling, Circles, Triangulation, Tree, Dashes, Diagram, Shapes, TSP, Letters | |
| **Streamline** | Flow Field, Edge Field, Superformula | |
| **Spiral** | Sketch Spirals + Sawtooth + Circular Scribbles | |
| **Mosaic** | Rectangles, Voronoi, Custom | |
| **Grid** | Shapes, Dashes, Letters | |
| **Composite** | Layers PFM, Stipple Layers | |
| **Maze / TSP** | Maze/labyrinth variants + TSP variants | |
| **Special** | Contour Paths, Ambient Flow, Edge Shading | |

## Caveats

- Bit-identical output vs official DrawingBot V3 is not claimed.
- Serial streaming requires a Qt installation that includes `Qt6SerialPort`.
- vpype optimization requires `vpype` on `PATH`.

## Run C++ app

```powershell
cmake --build cpp\build --config Release
.\cpp\build\Release\imgto2d_qt.exe
```
