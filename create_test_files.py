import numpy as np
from PIL import Image, ImageDraw

def create_test_png(path):
    # Create a 400x400 white image
    img = Image.new("L", (400, 400), 255)
    draw = ImageDraw.Draw(img)
    # Draw a black circle in the center
    draw.ellipse([100, 100, 300, 300], fill=0, outline=0, width=5)
    # Draw a black square inside the circle
    draw.rectangle([160, 160, 240, 240], fill=0)
    img.save(path)
    print(f"Created raster test image: {path}")

def create_test_svg(path):
    svg_content = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100">
  <!-- A bounding border -->
  <rect x="5" y="5" width="90" height="90" fill="none" stroke="black" stroke-width="2" />
  <!-- A circle -->
  <circle cx="50" cy="50" r="25" fill="none" stroke="red" stroke-width="1.5" />
  <!-- A diagonal line -->
  <line x1="10" y1="10" x2="90" y2="90" stroke="blue" stroke-width="1" />
  <!-- A cubic bezier curve -->
  <path d="M 15 85 C 30 50, 70 50, 85 85" fill="none" stroke="green" stroke-width="2" />
</svg>
"""
    with open(path, "w") as f:
        f.write(svg_content)
    print(f"Created vector test SVG: {path}")

if __name__ == "__main__":
    create_test_png("test_shape.png")
    create_test_svg("test_shape.svg")
