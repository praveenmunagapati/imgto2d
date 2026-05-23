"""
Minimal Tkinter GUI for imgto2d — prototype migration from PyQt6 to Tkinter.
Features:
- Load image
- Select PFM
- Run processing in background
- Show preview of generated geometries

Run: .venv\Scripts\python.exe tk_gui.py
"""

import threading
import sys
from pathlib import Path
from tkinter import Tk, Frame, Button, Label, Canvas, filedialog, ttk, StringVar
from PIL import Image, ImageTk, ImageDraw
import numpy as np
import cv2

# Ensure repo root on path
ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT))

# Import PFMs (safe, no Qt)
from app.pfm.sketch_lines import SketchLinesPFM
from app.pfm.sketch_squares import SketchSquaresPFM
from app.pfm.sketch_curves import SketchCurvesPFM
from app.pfm.sketch_shapes import SketchShapesPFM
from app.pfm.sketch_beziers import SketchQuadBeziersPFM
from app.pfm.sketch_waves import SketchWavesPFM
from app.pfm.sketch_spirals import SketchSpiralsPFM
from app.pfm.hatch_lines import HatchLinesPFM
from app.pfm.stipple_dots import StippleDotsPFM
from app.pfm.adaptive_pfms import AdaptiveStipplingPFM

AVAILABLE_PFMS = [
    SketchLinesPFM,
    SketchSquaresPFM,
    SketchCurvesPFM,
    SketchShapesPFM,
    SketchQuadBeziersPFM,
    SketchWavesPFM,
    SketchSpiralsPFM,
    HatchLinesPFM,
    StippleDotsPFM,
    AdaptiveStipplingPFM,
]

class TkApp:
    def __init__(self, root):
        self.root = root
        root.title('imgto2d - Tkinter Preview')
        root.geometry('1000x700')

        # Left controls
        ctrl = Frame(root, width=280)
        ctrl.pack(side='left', fill='y', padx=8, pady=8)

        self.load_btn = Button(ctrl, text='Load Image', command=self.load_image)
        self.load_btn.pack(fill='x', pady=4)

        Label(ctrl, text='Select PFM:').pack(anchor='w', pady=(8,0))
        self.pfm_var = StringVar()
        self.pfm_combo = ttk.Combobox(ctrl, textvariable=self.pfm_var, state='readonly')
        names = [cls().name for cls in AVAILABLE_PFMS]
        self.pfm_combo['values'] = names
        if names:
            self.pfm_combo.current(0)
        self.pfm_combo.pack(fill='x', pady=4)

        self.start_btn = Button(ctrl, text='Start Processing', command=self.start_processing)
        self.start_btn.pack(fill='x', pady=4)

        self.reset_btn = Button(ctrl, text='Reset', command=self.reset)
        self.reset_btn.pack(fill='x', pady=4)

        self.progress_lbl = Label(ctrl, text='Idle')
        self.progress_lbl.pack(anchor='w', pady=(12,0))

        # Right canvas
        canvas_frame = Frame(root)
        canvas_frame.pack(side='right', fill='both', expand=True, padx=8, pady=8)
        self.canvas = Canvas(canvas_frame, bg='#222222')
        self.canvas.pack(fill='both', expand=True)

        self.image = None
        self.processed_image = None
        self.display_imgtk = None
        self.processing_thread = None

    def load_image(self):
        path = filedialog.askopenfilename(filetypes=[('Image files', '*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.webp')])
        if not path:
            return
        img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            self.progress_lbl.config(text='Failed to load image')
            return
        self.image = img
        self.processed_image = img.copy()
        self._show_image(img)
        self.progress_lbl.config(text=f'Loaded: {Path(path).name} ({img.shape[1]}x{img.shape[0]})')

    def _show_image(self, img_np):
        # Convert grayscale numpy to PIL Image and fit to canvas size
        h, w = img_np.shape[:2]
        pil = Image.fromarray(img_np).convert('RGB')
        cw = max(200, self.canvas.winfo_width() or 800)
        ch = max(200, self.canvas.winfo_height() or 600)
        pil = pil.resize((cw, ch), Image.LANCZOS)
        self.display_imgtk = ImageTk.PhotoImage(pil)
        self.canvas.delete('all')
        self.canvas.create_image(0, 0, anchor='nw', image=self.display_imgtk)

    def start_processing(self):
        if self.image is None:
            self.progress_lbl.config(text='Load an image first')
            return
        if self.processing_thread and self.processing_thread.is_alive():
            self.progress_lbl.config(text='Already processing')
            return
        idx = self.pfm_combo.current()
        if idx < 0:
            self.progress_lbl.config(text='Select a PFM')
            return
        pfm_cls = AVAILABLE_PFMS[idx]
        self.processing_thread = threading.Thread(target=self._run_pfm, args=(pfm_cls,))
        self.processing_thread.start()
        self.progress_lbl.config(text='Processing...')

    def _run_pfm(self, pfm_cls):
        try:
            pfm = pfm_cls()
            geoms = pfm.process(self.processed_image)
            # Render geometries to image
            h, w = self.processed_image.shape[:2]
            out = Image.new('RGB', (w, h), (255, 255, 255))
            draw = ImageDraw.Draw(out)
            for geom in geoms:
                path = geom.path
                if len(path) >= 2:
                    pts = [(x, y) for x, y in path]
                    draw.line(pts, fill=(0,0,0), width=1)
            # Resize for display
            cw = max(200, self.canvas.winfo_width() or 800)
            ch = max(200, self.canvas.winfo_height() or 600)
            disp = out.resize((cw, ch), Image.LANCZOS)
            self.display_imgtk = ImageTk.PhotoImage(disp)
            # Update UI on main thread
            self.root.after(0, lambda: self._update_canvas_with_image(self.display_imgtk))
            self.root.after(0, lambda: self.progress_lbl.config(text=f'Processed: {len(geoms)} shapes'))
        except Exception as e:
            import traceback
            tb = traceback.format_exc()
            self.root.after(0, lambda: self.progress_lbl.config(text=f'Error: {e}'))
            print(tb)

    def _update_canvas_with_image(self, imgtk):
        self.canvas.delete('all')
        self.canvas.create_image(0, 0, anchor='nw', image=imgtk)
        # keep a reference
        self.canvas.image = imgtk

    def reset(self):
        self.image = None
        self.processed_image = None
        self.canvas.delete('all')
        self.progress_lbl.config(text='Reset')


if __name__ == '__main__':
    root = Tk()
    app = TkApp(root)
    root.mainloop()
