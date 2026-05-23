# DrawingBot V3 parity checklist

Target: **DrawingBot V3 1.6.x** ([documentation](https://docs.drawingbotv3.com/en/latest/))

## Implemented (premium styles)

| Family | Modules | Notes |
|--------|---------|--------|
| **Sketch** | Lines, Squares, Curves, Catmull-Roms, Shapes, Sobel Edges, Beziers, Waves, Spirals, Flow Fields, Superformula, Radial, Scribble, Abstract, Voronoi, Delaunay, … | Core + extras marked premium |
| **Hatch** | Lines + 10 multi-angle + **Sawtooth**, **Circular Scribbles** | Premium hatch uses zigzag / ring scribbles |
| **Stipple** | Dots + 10 shape variants (variable size from brightness) | All stipple extras premium |
| **Adaptive** | 8 styles + **Letters** | Delaunay/MST tree via OpenCV; letters A–Z per cell |
| **Voronoi** | 8 styles + **Letters** | Distinct renderers (not clones); facet dashes, variable stipple |
| **LBG** | 8 Lloyd variants + **Letters** | Default 5 Lloyd iterations |
| **Streamline** | Edge Field, Flow Field, Superformula | Sobel flow tracing |
| **Spiral** | Sketch Spirals + Sawtooth + Circular Scribbles | |
| **Mosaic** | Rectangles, Voronoi, Custom | |
| **Grid** | Shapes, Dashes, **Letters** | Grid letter glyphs |
| **Composite** | Layers PFM, **Stipple Layers** | Multi-band processing |
| **Maze / TSP** | 10 mazes + 6 TSP variants | All premium |
| **Special** | Contour Paths, Ambient Flow, Edge Shading | |

**Total: ~105 PFMs** (with `PREMIUM_UNLOCKED = True` in `app/config.py`)

## Product features

| Feature | Status |
|---------|--------|
| 63 image filters | Done |
| Masks, batch, PDF/HPGL/G-code export | Done |
| Project save/load | Done |
| Pen editor | Done |
| Video frame import | Done |

## Algorithm note

Premium PFMs use faithful **Python implementations** (OpenCV Voronoi/Delaunay, Lloyd relaxation, flow fields, stroke letters). Output will differ from the Java DrawingBot reference builds but follows the same visual families.

## Not implemented

- vpype integration  
- Serial plotter in Qt app  
- Bit-identical output vs official DBV3  

## Run

```bash
pip install -r requirements.txt
python run.py
```
