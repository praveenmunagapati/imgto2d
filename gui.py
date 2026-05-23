import os
import tkinter as tk
from tkinter import filedialog, messagebox
import customtkinter as ctk
from PIL import Image, ImageTk, ImageDraw
import math
import numpy as np
import queue

from gcode_generator import GCodeGenerator
from serial_manager import SerialManager

# ====================================================
# MATERIAL DESIGN 3 STYLE TOKENS (Dark Theme)
# ====================================================
MD3_BG = "#1C1B1F"                # Slate / deep charcoal background
MD3_SURFACE = "#25232A"           # Sidebar / tabview base
MD3_SURFACE_HIGH = "#2D2B33"      # Cards / elevated containers
MD3_PRIMARY = "#D0BCFF"           # Lavender (primary highlight)
MD3_PRIMARY_CONTAINER = "#4F378B" # Dark violet (primary container fill)
MD3_ON_PRIMARY = "#381E72"        # Dark text color on primary lavender
MD3_SECONDARY = "#006A6A"         # Accent teal
MD3_ON_SECONDARY = "#FFFFFF"      # Text color on secondary
MD3_TEXT_PRIMARY = "#E6E1E5"      # High emphasis body text
MD3_TEXT_SECONDARY = "#CAC4D0"    # Medium emphasis subtitle text
MD3_BORDER = "#49454F"            # Outline border color
MD3_ERROR = "#F2B8B5"             # Error color
MD3_ERROR_CONTAINER = "#8C1D18"   # Dark red container for aborts

class PlotterApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        # Dynamic screen resolution setup
        screen_w = self.winfo_screenwidth()
        screen_h = self.winfo_screenheight()
        start_w = min(1250, int(screen_w * 0.85))
        start_h = min(850, int(screen_h * 0.85))
        x_pos = (screen_w - start_w) // 2
        y_pos = (screen_h - start_h) // 2
        
        self.title("Antigravity 2D Plotter G-Code Tool")
        self.geometry(f"{start_w}x{start_h}+{x_pos}+{y_pos}")
        self.minsize(480, 400)
        
        # Set appearance
        ctk.set_appearance_mode("Dark")
        self.configure(fg_color=MD3_BG)

        # Initialize backend
        self.generator = GCodeGenerator()
        self.serial_mgr = SerialManager()
        
        self.raw_image = None         # PIL Image
        self.processed_image = None   # PIL Image (thresholded/dithered)
        self.processed_np = None      # OpenCV thresholded image
        self.file_path = ""
        self.generated_gcode = ""
        self.preview_paths = []
        self.stats = {}
        self._current_job_id = 0
        
        # Thread-safe queue for serial events
        self.gui_queue = queue.Queue()

        # Configure grid layout (1 row, 2 columns)
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=0, minsize=380) # Left panel fixed width
        self.grid_columnconfigure(1, weight=1)              # Right panel takes remaining

        self._create_widgets()
        self._setup_defaults()
        self._update_ui_states()
        
        # Start GUI queue polling
        self.after(50, self._process_gui_queue)

        # Bind window resize event for responsive layouts
        self.bind("<Configure>", self._on_window_resize)

    def _create_widgets(self):
        # ----------------------------------------------------
        # LEFT PANEL: SETTINGS (Scrollable, Material Surface)
        # ----------------------------------------------------
        self.left_frame = ctk.CTkScrollableFrame(self, corner_radius=16, fg_color=MD3_SURFACE, border_width=1, border_color=MD3_BORDER)
        self.left_frame.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)
        
        # Title
        title_label = ctk.CTkLabel(
            self.left_frame, 
            text="2D Plotter Settings", 
            font=ctk.CTkFont(family="Segoe UI", size=22, weight="normal"),
            text_color=MD3_TEXT_PRIMARY
        )
        title_label.pack(pady=(15, 10), padx=10, anchor="w")

        # --- Section 1: File Loading ---
        self._create_section_header("1. Input File")
        
        self.file_btn = ctk.CTkButton(
            self.left_frame, 
            text="Load Image or SVG", 
            command=self._load_file, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold")
        )
        self.file_btn.pack(pady=5, padx=10, fill="x")
        
        self.file_lbl = ctk.CTkLabel(
            self.left_frame, 
            text="No file selected", 
            text_color=MD3_TEXT_SECONDARY, 
            font=ctk.CTkFont(family="Segoe UI", size=12),
            wraplength=340, 
            justify="left"
        )
        self.file_lbl.pack(pady=2, padx=10, anchor="w")

        # --- Section 2: Mode Selection ---
        self._create_section_header("2. Processing Mode")
        
        self.mode_var = ctk.StringVar(value="Outline")
        self.mode_selector = ctk.CTkSegmentedButton(
            self.left_frame, 
            values=["Outline", "Hatch", "Dither", "Wave", "Spiral", "SVG"],
            command=self._on_mode_change,
            variable=self.mode_var,
            corner_radius=20,
            selected_color=MD3_PRIMARY_CONTAINER,
            selected_hover_color="#5D45A0",
            text_color=MD3_TEXT_PRIMARY
        )
        self.mode_selector.pack(pady=5, padx=10, fill="x")

        # --- Section 3: Image Filters (Disabled for SVG) ---
        self.filter_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        self.filter_frame.pack(pady=5, padx=10, fill="x")
        
        self._create_section_header("3. Image Adjustments", parent=self.filter_frame)

        # Brightness
        self.bright_lbl = ctk.CTkLabel(self.filter_frame, text="Brightness: 1.00", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.bright_lbl.pack(anchor="w")
        self.bright_slider = ctk.CTkSlider(self.filter_frame, from_=0.1, to=3.0, number_of_steps=29, command=self._on_filter_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.bright_slider.set(1.0)
        self.bright_slider.pack(fill="x", pady=(0, 10))
        self.bright_slider.bind("<ButtonRelease-1>", lambda e: self._on_filter_changed())

        # Contrast
        self.contrast_lbl = ctk.CTkLabel(self.filter_frame, text="Contrast: 1.00", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.contrast_lbl.pack(anchor="w")
        self.contrast_slider = ctk.CTkSlider(self.filter_frame, from_=0.1, to=3.0, number_of_steps=29, command=self._on_filter_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.contrast_slider.set(1.0)
        self.contrast_slider.pack(fill="x", pady=(0, 10))
        self.contrast_slider.bind("<ButtonRelease-1>", lambda e: self._on_filter_changed())

        # Threshold slider
        self.thresh_frame = ctk.CTkFrame(self.filter_frame, fg_color="transparent")
        self.thresh_frame.pack(fill="x")
        self.thresh_lbl = ctk.CTkLabel(self.thresh_frame, text="Threshold: 127", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.thresh_lbl.pack(anchor="w")
        self.thresh_slider = ctk.CTkSlider(self.thresh_frame, from_=0, to=255, number_of_steps=255, command=self._on_filter_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.thresh_slider.set(127)
        self.thresh_slider.pack(fill="x", pady=(0, 10))
        self.thresh_slider.bind("<ButtonRelease-1>", lambda e: self._on_filter_changed())

        # Dither resolution slider
        self.dither_res_frame = ctk.CTkFrame(self.filter_frame, fg_color="transparent")
        self.dither_res_lbl = ctk.CTkLabel(self.dither_res_frame, text="Dither Dots (Width): 150 px", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.dither_res_lbl.pack(anchor="w")
        self.dither_res_slider = ctk.CTkSlider(self.dither_res_frame, from_=50, to=350, number_of_steps=30, command=self._on_filter_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.dither_res_slider.set(150)
        self.dither_res_slider.pack(fill="x", pady=(0, 10))
        self.dither_res_slider.bind("<ButtonRelease-1>", lambda e: self._on_filter_changed())

        # Invert Colors
        self.invert_var = ctk.BooleanVar(value=False)
        self.invert_cb = ctk.CTkCheckBox(
            self.filter_frame, 
            text="Invert Colors", 
            variable=self.invert_var, 
            command=self._on_filter_changed, 
            text_color=MD3_TEXT_PRIMARY, 
            font=ctk.CTkFont(family="Segoe UI", size=13), 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            border_color=MD3_BORDER
        )
        self.invert_cb.pack(anchor="w", pady=5)

        # --- Section 4: Mode Parameters ---
        self.param_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        self.param_frame.pack(pady=5, padx=10, fill="x")
        
        self.param_header = self._create_section_header("4. Mode Parameters", parent=self.param_frame)
        
        # Contour Frame
        self.contour_param_frame = ctk.CTkFrame(self.param_frame, fg_color="transparent")
        self.contour_param_frame.pack(fill="x")
        
        self.epsilon_lbl = ctk.CTkLabel(self.contour_param_frame, text="Simplification (Epsilon): 1.0", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.epsilon_lbl.pack(anchor="w")
        self.epsilon_slider = ctk.CTkSlider(self.contour_param_frame, from_=0.0, to=5.0, number_of_steps=50, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.epsilon_slider.set(1.0)
        self.epsilon_slider.pack(fill="x", pady=(0, 10))
        self.epsilon_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())

        self.min_len_lbl = ctk.CTkLabel(self.contour_param_frame, text="Noise Filter (Min px): 3", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.min_len_lbl.pack(anchor="w")
        self.min_len_slider = ctk.CTkSlider(self.contour_param_frame, from_=1, to=20, number_of_steps=19, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.min_len_slider.set(3)
        self.min_len_slider.pack(fill="x", pady=(0, 10))
        self.min_len_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())

        # Hatch Frame
        self.hatch_param_frame = ctk.CTkFrame(self.param_frame, fg_color="transparent")
        self.hatch_param_frame.pack(fill="x")
        
        self.spacing_lbl = ctk.CTkLabel(self.hatch_param_frame, text="Hatch Spacing (mm): 1.0", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.spacing_lbl.pack(anchor="w")
        self.spacing_slider = ctk.CTkSlider(self.hatch_param_frame, from_=0.2, to=5.0, number_of_steps=48, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.spacing_slider.set(1.0)
        self.spacing_slider.pack(fill="x", pady=(0, 10))
        self.spacing_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())

        self.angle_lbl = ctk.CTkLabel(self.hatch_param_frame, text="Hatch Angle (deg): 45", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.angle_lbl.pack(anchor="w")
        self.angle_slider = ctk.CTkSlider(self.hatch_param_frame, from_=-90, to=90, number_of_steps=180, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.angle_slider.set(45)
        self.angle_slider.pack(fill="x", pady=(0, 10))
        self.angle_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())

        # Cross-hatching checkbox
        self.cross_hatch_var = ctk.BooleanVar(value=False)
        self.cross_hatch_cb = ctk.CTkCheckBox(
            self.hatch_param_frame, 
            text="Cross-Hatch Pattern", 
            variable=self.cross_hatch_var, 
            command=self.generate_gcode_preview, 
            text_color=MD3_TEXT_PRIMARY, 
            font=ctk.CTkFont(family="Segoe UI", size=13), 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            border_color=MD3_BORDER
        )
        self.cross_hatch_cb.pack(anchor="w", pady=5)

        # Wave & Spiral Parameter Frame
        self.wave_param_frame = ctk.CTkFrame(self.param_frame, fg_color="transparent")
        
        self.wave_spacing_lbl = ctk.CTkLabel(self.wave_param_frame, text="Line Spacing (mm): 1.50", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.wave_spacing_lbl.pack(anchor="w")
        self.wave_spacing_slider = ctk.CTkSlider(self.wave_param_frame, from_=0.2, to=5.0, number_of_steps=48, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.wave_spacing_slider.set(1.50)
        self.wave_spacing_slider.pack(fill="x", pady=(0, 10))
        self.wave_spacing_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())
        
        self.wave_freq_lbl = ctk.CTkLabel(self.wave_param_frame, text="Wave Frequency: 0.30", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.wave_freq_lbl.pack(anchor="w")
        self.wave_freq_slider = ctk.CTkSlider(self.wave_param_frame, from_=0.05, to=2.0, number_of_steps=39, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.wave_freq_slider.set(0.30)
        self.wave_freq_slider.pack(fill="x", pady=(0, 10))
        self.wave_freq_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())
        
        self.wave_amp_lbl = ctk.CTkLabel(self.wave_param_frame, text="Wave Amplitude (mm): 1.00", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        self.wave_amp_lbl.pack(anchor="w")
        self.wave_amp_slider = ctk.CTkSlider(self.wave_param_frame, from_=0.0, to=5.0, number_of_steps=50, command=self._on_param_slider_move, button_color=MD3_PRIMARY, button_hover_color="#EADDFF", progress_color=MD3_PRIMARY_CONTAINER)
        self.wave_amp_slider.set(1.00)
        self.wave_amp_slider.pack(fill="x", pady=(0, 10))
        self.wave_amp_slider.bind("<ButtonRelease-1>", lambda e: self.generate_gcode_preview())

        # --- Section 5: Target Dimensions & Alignment ---
        self._create_section_header("5. Target Dimensions & Routing")
        
        size_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        size_frame.pack(pady=5, padx=10, fill="x")
        size_frame.grid_columnconfigure((0, 1), weight=1)
        
        # Input helper to quickly configure MD3 style Entry boxes
        def md3_entry(parent, width=80):
            return ctk.CTkEntry(parent, width=width, fg_color=MD3_BG, border_color=MD3_BORDER, text_color=MD3_TEXT_PRIMARY, corner_radius=8)

        # Width
        w_lbl = ctk.CTkLabel(size_frame, text="Width (mm):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        w_lbl.grid(row=0, column=0, sticky="w", pady=2)
        self.width_entry = md3_entry(size_frame)
        self.width_entry.grid(row=0, column=1, sticky="e", pady=2)
        self.width_entry.bind("<FocusOut>", lambda e: self._on_dimension_changed())
        self.width_entry.bind("<Return>", lambda e: self._on_dimension_changed())
        
        # Height
        h_lbl = ctk.CTkLabel(size_frame, text="Height (mm):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        h_lbl.grid(row=1, column=0, sticky="w", pady=2)
        self.height_entry = md3_entry(size_frame)
        self.height_entry.grid(row=1, column=1, sticky="e", pady=2)
        self.height_entry.bind("<FocusOut>", lambda e: self._on_dimension_changed())
        self.height_entry.bind("<Return>", lambda e: self._on_dimension_changed())
        
        # Scale mode
        scale_lbl = ctk.CTkLabel(size_frame, text="Scale Mode:", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        scale_lbl.grid(row=2, column=0, sticky="w", pady=2)
        self.scale_mode_var = ctk.StringVar(value="fit")
        self.scale_mode_menu = ctk.CTkOptionMenu(
            size_frame, 
            values=["fit", "stretch"], 
            variable=self.scale_mode_var,
            width=85,
            command=lambda v: self.generate_gcode_preview(),
            fg_color=MD3_PRIMARY_CONTAINER,
            button_color=MD3_PRIMARY_CONTAINER,
            button_hover_color="#5D45A0",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=12
        )
        self.scale_mode_menu.grid(row=2, column=1, sticky="e", pady=2)

        # Offset X, Y
        offset_lbl = ctk.CTkLabel(size_frame, text="Offset X,Y (mm):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        offset_lbl.grid(row=3, column=0, sticky="w", pady=2)
        offset_xy_frame = ctk.CTkFrame(size_frame, fg_color="transparent")
        offset_xy_frame.grid(row=3, column=1, sticky="e", pady=2)
        self.offset_x_entry = md3_entry(offset_xy_frame, width=45)
        self.offset_x_entry.pack(side="left", padx=(0,2))
        self.offset_x_entry.bind("<FocusOut>", lambda e: self._on_dimension_changed())
        self.offset_x_entry.bind("<Return>", lambda e: self._on_dimension_changed())
        
        self.offset_y_entry = md3_entry(offset_xy_frame, width=45)
        self.offset_y_entry.pack(side="left")
        self.offset_y_entry.bind("<FocusOut>", lambda e: self._on_dimension_changed())
        self.offset_y_entry.bind("<Return>", lambda e: self._on_dimension_changed())

        # Optimization & axis checkboxes
        self.optimize_var = ctk.BooleanVar(value=True)
        self.optimize_cb = ctk.CTkCheckBox(
            self.left_frame, 
            text="Optimize Path Order (TSP)", 
            variable=self.optimize_var, 
            command=self.generate_gcode_preview, 
            text_color=MD3_TEXT_PRIMARY, 
            font=ctk.CTkFont(family="Segoe UI", size=13), 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            border_color=MD3_BORDER
        )
        self.optimize_cb.pack(anchor="w", pady=5, padx=10)

        self.flip_y_var = ctk.BooleanVar(value=True)
        self.flip_y_cb = ctk.CTkCheckBox(
            self.left_frame, 
            text="Flip Y Axis (Standard CNC)", 
            variable=self.flip_y_var, 
            command=self.generate_gcode_preview, 
            text_color=MD3_TEXT_PRIMARY, 
            font=ctk.CTkFont(family="Segoe UI", size=13), 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            border_color=MD3_BORDER
        )
        self.flip_y_cb.pack(anchor="w", pady=5, padx=10)

        # --- Section 6: Hardware & Commands ---
        self._create_section_header("6. G-Code & Hardware Settings")
        
        self.hw_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        self.hw_frame.pack(pady=5, padx=10, fill="x")
        self.hw_frame.grid_columnconfigure((0, 1), weight=1)

        # Feedrate Draw
        f_draw_lbl = ctk.CTkLabel(self.hw_frame, text="Draw Feed (mm/m):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        f_draw_lbl.grid(row=0, column=0, sticky="w", pady=2)
        self.f_draw_entry = md3_entry(self.hw_frame)
        self.f_draw_entry.grid(row=0, column=1, sticky="e", pady=2)
        self.f_draw_entry.bind("<FocusOut>", lambda e: self.generate_gcode_preview())

        # Feedrate Travel
        f_trav_lbl = ctk.CTkLabel(self.hw_frame, text="Travel Feed (mm/m):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        f_trav_lbl.grid(row=1, column=0, sticky="w", pady=2)
        self.f_travel_entry = md3_entry(self.hw_frame)
        self.f_travel_entry.grid(row=1, column=1, sticky="e", pady=2)
        self.f_travel_entry.bind("<FocusOut>", lambda e: self.generate_gcode_preview())

        # Laser Mode Toggle
        self.laser_mode_var = ctk.BooleanVar(value=False)
        self.laser_mode_cb = ctk.CTkCheckBox(
            self.left_frame, 
            text="Laser Mode (PWM control)", 
            variable=self.laser_mode_var, 
            command=self._on_laser_mode_changed, 
            text_color=MD3_TEXT_PRIMARY, 
            font=ctk.CTkFont(family="Segoe UI", size=13), 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            border_color=MD3_BORDER
        )
        self.laser_mode_cb.pack(anchor="w", pady=5, padx=10)

        # Pen Command entries (only shown when Laser Mode is inactive)
        self.pen_cmds_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        self.pen_cmds_frame.pack(pady=5, padx=10, fill="x")
        self.pen_cmds_frame.grid_columnconfigure((0, 1), weight=1)

        p_down_lbl = ctk.CTkLabel(self.pen_cmds_frame, text="Pen Down Cmd:", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        p_down_lbl.grid(row=0, column=0, sticky="w", pady=2)
        self.p_down_entry = md3_entry(self.pen_cmds_frame)
        self.p_down_entry.grid(row=0, column=1, sticky="e", pady=2)
        self.p_down_entry.bind("<FocusOut>", lambda e: self.generate_gcode_preview())

        p_up_lbl = ctk.CTkLabel(self.pen_cmds_frame, text="Pen Up Cmd:", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        p_up_lbl.grid(row=1, column=0, sticky="w", pady=2)
        self.p_up_entry = md3_entry(self.pen_cmds_frame)
        self.p_up_entry.grid(row=1, column=1, sticky="e", pady=2)
        self.p_up_entry.bind("<FocusOut>", lambda e: self.generate_gcode_preview())

        p_delay_lbl = ctk.CTkLabel(self.pen_cmds_frame, text="Pen Delay (sec):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        p_delay_lbl.grid(row=2, column=0, sticky="w", pady=2)
        self.p_delay_entry = md3_entry(self.pen_cmds_frame)
        self.p_delay_entry.grid(row=2, column=1, sticky="e", pady=2)
        self.p_delay_entry.bind("<FocusOut>", lambda e: self.generate_gcode_preview())

        # --- Action Buttons (MD3 Filled & Accent styling) ---
        self.generate_btn = ctk.CTkButton(
            self.left_frame, 
            text="Generate G-Code", 
            command=self.generate_gcode_preview, 
            fg_color=MD3_PRIMARY, 
            hover_color="#EADDFF",
            text_color=MD3_ON_PRIMARY,
            corner_radius=20,
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold")
        )
        self.generate_btn.pack(pady=(15, 5), padx=10, fill="x")
        
        self.save_btn = ctk.CTkButton(
            self.left_frame, 
            text="Save G-Code File", 
            command=self._save_file, 
            fg_color=MD3_SECONDARY, 
            hover_color="#004D40",
            text_color=MD3_ON_SECONDARY,
            corner_radius=20,
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold")
        )
        self.save_btn.pack(pady=5, padx=10, fill="x")
        
        ctk.CTkLabel(self.left_frame, text="", height=20).pack()

        # ----------------------------------------------------
        # RIGHT PANEL: VISUAL PREVIEWS, TABS, & MACHINE CONTROL
        # ----------------------------------------------------
        self.right_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.right_frame.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        
        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_rowconfigure(1, weight=0, minsize=40) # Status bar
        self.right_frame.grid_columnconfigure(0, weight=1)

        # Tabview
        self.tabview = ctk.CTkTabview(
            self.right_frame, 
            fg_color=MD3_SURFACE, 
            segmented_button_selected_color=MD3_PRIMARY_CONTAINER,
            segmented_button_selected_hover_color="#5D45A0",
            segmented_button_unselected_color=MD3_BG,
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=16,
            command=self._on_tab_changed
        )
        self.tabview.grid(row=0, column=0, sticky="nsew", padx=0, pady=5)
        
        self.img_tab = self.tabview.add("Processed Image")
        self.gcode_tab = self.tabview.add("G-Code Toolpath")
        self.control_tab = self.tabview.add("Plotter Control")

        # Tab 1: Image Preview
        self.img_tab.grid_rowconfigure(0, weight=1)
        self.img_tab.grid_columnconfigure(0, weight=1)
        self.img_preview_lbl = ctk.CTkLabel(self.img_tab, text="No Image Loaded\nImport a JPG/PNG/WebP image to adjust threshold and filters.", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=13))
        self.img_preview_lbl.grid(row=0, column=0, sticky="nsew")

        # Tab 2: Canvas Toolpath
        self.gcode_tab.grid_rowconfigure(0, weight=1)
        self.gcode_tab.grid_columnconfigure(0, weight=1)
        
        self.preview_canvas = tk.Canvas(self.gcode_tab, bg=MD3_BG, highlightthickness=1, highlightbackground=MD3_BORDER)
        self.preview_canvas.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
        self.preview_canvas.bind("<Configure>", lambda e: self._draw_toolpath())

        # Tab 3: Plotter Control (Serial Jogger / Streamer)
        self._setup_control_tab()

        # --- Status Bar ---
        self.status_frame = ctk.CTkFrame(self.right_frame, height=35, corner_radius=12, fg_color=MD3_SURFACE, border_width=1, border_color=MD3_BORDER)
        self.status_frame.grid(row=1, column=0, sticky="nsew", padx=0, pady=(10, 0))
        self.status_frame.grid_rowconfigure(0, weight=1)
        self.status_frame.grid_columnconfigure((0, 1, 2, 3), weight=1)

        self.status_scale = ctk.CTkLabel(self.status_frame, text="Dimensions: 0 x 0 mm", font=ctk.CTkFont(family="Segoe UI", size=12), text_color=MD3_TEXT_SECONDARY)
        self.status_scale.grid(row=0, column=0, sticky="w", padx=15)

        self.status_distance = ctk.CTkLabel(self.status_frame, text="Distance: Travel: 0mm | Draw: 0mm", font=ctk.CTkFont(family="Segoe UI", size=12), text_color=MD3_TEXT_SECONDARY)
        self.status_distance.grid(row=0, column=1, sticky="w", padx=15)

        self.status_time = ctk.CTkLabel(self.status_frame, text="Estimated Time: 00:00:00", font=ctk.CTkFont(family="Segoe UI", size=12), text_color=MD3_TEXT_SECONDARY)
        self.status_time.grid(row=0, column=2, sticky="w", padx=15)

        self.status_pen = ctk.CTkLabel(self.status_frame, text="Pen Actions: 0", font=ctk.CTkFont(family="Segoe UI", size=12), text_color=MD3_TEXT_SECONDARY)
        self.status_pen.grid(row=0, column=3, sticky="w", padx=15)

    def _setup_control_tab(self):
        """Builds connection dropdowns, circular jog pad, progress bars, and streaming console."""
        tab = self.control_tab
        tab.grid_rowconfigure(0, weight=1)
        tab.grid_columnconfigure(0, weight=1, minsize=400) # Left Column (Streaming / Connect)
        tab.grid_columnconfigure(1, weight=1, minsize=350) # Right Column (Jogging)

        # Left Column Frame
        self.left_ctrl_frame = ctk.CTkFrame(tab, fg_color="transparent")
        self.left_ctrl_frame.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)
        self.left_ctrl_frame.grid_columnconfigure(0, weight=1)
        
        # --- Serial Connection Panel ---
        conn_box = ctk.CTkFrame(self.left_ctrl_frame, border_width=1, border_color=MD3_BORDER, fg_color=MD3_SURFACE_HIGH, corner_radius=16)
        conn_box.pack(fill="x", pady=(0, 15), padx=5)
        
        conn_lbl = ctk.CTkLabel(conn_box, text="Serial Connection (GRBL)", font=ctk.CTkFont(family="Segoe UI", size=14, weight="bold"), text_color=MD3_TEXT_PRIMARY)
        conn_lbl.pack(anchor="w", padx=15, pady=(10, 5))
        
        conn_setup = ctk.CTkFrame(conn_box, fg_color="transparent")
        conn_setup.pack(fill="x", padx=15, pady=(5, 10))
        
        self.port_var = ctk.StringVar(value="Select Port")
        self.port_menu = ctk.CTkOptionMenu(
            conn_setup, 
            variable=self.port_var, 
            values=["Select Port"], 
            width=130, 
            fg_color=MD3_BG, 
            button_color=MD3_BG, 
            button_hover_color="#2D2B33", 
            text_color=MD3_TEXT_PRIMARY, 
            corner_radius=12
        )
        self.port_menu.pack(side="left", padx=(0,5))
        
        self.port_refresh_btn = ctk.CTkButton(
            conn_setup, 
            text="🔄", 
            width=30, 
            command=self._refresh_ports,
            fg_color=MD3_BG,
            hover_color="#2D2B33",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=12
        )
        self.port_refresh_btn.pack(side="left", padx=(0,10))
        
        self.baud_var = ctk.StringVar(value="115200")
        self.baud_menu = ctk.CTkOptionMenu(
            conn_setup, 
            variable=self.baud_var, 
            values=["9600", "115200"], 
            width=90,
            fg_color=MD3_BG,
            button_color=MD3_BG,
            button_hover_color="#2D2B33",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=12
        )
        self.baud_menu.pack(side="left", padx=(0,10))
        
        self.connect_btn = ctk.CTkButton(
            conn_setup, 
            text="Connect", 
            command=self._toggle_connection, 
            fg_color=MD3_SECONDARY, 
            hover_color="#004D40", 
            text_color=MD3_ON_SECONDARY,
            corner_radius=20,
            font=ctk.CTkFont(family="Segoe UI", weight="bold"),
            width=90
        )
        self.connect_btn.pack(side="right", fill="x", expand=True)

        # --- G-Code Streamer Card ---
        stream_box = ctk.CTkFrame(self.left_ctrl_frame, border_width=1, border_color=MD3_BORDER, fg_color=MD3_SURFACE_HIGH, corner_radius=16)
        stream_box.pack(fill="both", expand=True, padx=5)
        
        stream_lbl = ctk.CTkLabel(stream_box, text="G-Code Streamer", font=ctk.CTkFont(family="Segoe UI", size=14, weight="bold"), text_color=MD3_TEXT_PRIMARY)
        stream_lbl.pack(anchor="w", padx=15, pady=(10, 5))
        
        # Progress elements
        self.stream_progress = ctk.CTkProgressBar(stream_box, progress_color=MD3_PRIMARY, fg_color=MD3_BG)
        self.stream_progress.pack(fill="x", padx=15, pady=5)
        self.stream_progress.set(0.0)
        
        self.stream_status_lbl = ctk.CTkLabel(stream_box, text="Status: Disconnected", font=ctk.CTkFont(family="Segoe UI", size=12), text_color=MD3_TEXT_SECONDARY)
        self.stream_status_lbl.pack(anchor="w", padx=15, pady=2)
        
        # Stream buttons
        stream_buttons_frame = ctk.CTkFrame(stream_box, fg_color="transparent")
        stream_buttons_frame.pack(fill="x", padx=10, pady=(10, 15))
        stream_buttons_frame.grid_columnconfigure((0, 1, 2, 3), weight=1)
        
        self.stream_send_btn = ctk.CTkButton(
            stream_buttons_frame, 
            text="Send", 
            command=self._start_stream, 
            fg_color=MD3_PRIMARY, 
            hover_color="#EADDFF", 
            text_color=MD3_ON_PRIMARY,
            corner_radius=20,
            font=ctk.CTkFont(family="Segoe UI", weight="bold"),
            state="disabled"
        )
        self.stream_send_btn.grid(row=0, column=0, padx=2)
        
        self.stream_pause_btn = ctk.CTkButton(
            stream_buttons_frame, 
            text="Pause", 
            command=self._pause_stream, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            state="disabled"
        )
        self.stream_pause_btn.grid(row=0, column=1, padx=2)
        
        self.stream_resume_btn = ctk.CTkButton(
            stream_buttons_frame, 
            text="Resume", 
            command=self._resume_stream, 
            fg_color=MD3_SECONDARY, 
            hover_color="#004D40", 
            text_color=MD3_ON_SECONDARY,
            corner_radius=20,
            state="disabled"
        )
        self.stream_resume_btn.grid(row=0, column=2, padx=2)
        
        self.stream_abort_btn = ctk.CTkButton(
            stream_buttons_frame, 
            text="Abort", 
            command=self._abort_stream, 
            fg_color=MD3_ERROR_CONTAINER, 
            hover_color="#B13A3A", 
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            state="disabled"
        )
        self.stream_abort_btn.grid(row=0, column=3, padx=2)

        # Right Column Frame (Jogger Pad)
        self.right_ctrl_frame = ctk.CTkFrame(tab, fg_color="transparent")
        self.right_ctrl_frame.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        self.right_ctrl_frame.grid_columnconfigure(0, weight=1)
        
        # --- Jogging Card ---
        jog_box = ctk.CTkFrame(self.right_ctrl_frame, border_width=1, border_color=MD3_BORDER, fg_color=MD3_SURFACE_HIGH, corner_radius=16)
        jog_box.pack(fill="both", expand=True, padx=5)
        
        jog_lbl = ctk.CTkLabel(jog_box, text="Manual Jog Controller", font=ctk.CTkFont(family="Segoe UI", size=14, weight="bold"), text_color=MD3_TEXT_PRIMARY)
        jog_lbl.pack(anchor="w", padx=15, pady=(10, 5))
        
        # Jog Cross Frame
        jog_pad_frame = ctk.CTkFrame(jog_box, fg_color="transparent", width=190, height=190)
        jog_pad_frame.pack(pady=10)
        
        # 5-way D-Pad Jog layout using perfectly circular buttons (width=55, height=55, corner_radius=27)
        self.jog_y_up = ctk.CTkButton(
            jog_pad_frame, 
            text="Y+", 
            width=55, 
            height=55, 
            corner_radius=27, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            text_color=MD3_TEXT_PRIMARY,
            command=lambda: self._jog(0, 1), 
            state="disabled"
        )
        self.jog_y_up.grid(row=0, column=1, pady=3, padx=3)
        
        self.jog_x_left = ctk.CTkButton(
            jog_pad_frame, 
            text="X-", 
            width=55, 
            height=55, 
            corner_radius=27, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            text_color=MD3_TEXT_PRIMARY,
            command=lambda: self._jog(-1, 0), 
            state="disabled"
        )
        self.jog_x_left.grid(row=1, column=0, pady=3, padx=3)
        
        # Center Home/Zero button (Accent Colored Circular)
        self.jog_zero = ctk.CTkButton(
            jog_pad_frame, 
            text="Zero", 
            width=55, 
            height=55, 
            corner_radius=27, 
            fg_color=MD3_SECONDARY, 
            hover_color="#004D40", 
            text_color=MD3_ON_SECONDARY,
            font=ctk.CTkFont(family="Segoe UI", size=12, weight="bold"),
            command=self._zero_axes, 
            state="disabled"
        )
        self.jog_zero.grid(row=1, column=1, pady=3, padx=3)
        
        self.jog_x_right = ctk.CTkButton(
            jog_pad_frame, 
            text="X+", 
            width=55, 
            height=55, 
            corner_radius=27, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            text_color=MD3_TEXT_PRIMARY,
            command=lambda: self._jog(1, 0), 
            state="disabled"
        )
        self.jog_x_right.grid(row=1, column=2, pady=3, padx=3)
        
        self.jog_y_down = ctk.CTkButton(
            jog_pad_frame, 
            text="Y-", 
            width=55, 
            height=55, 
            corner_radius=27, 
            fg_color=MD3_PRIMARY_CONTAINER, 
            hover_color="#5D45A0", 
            text_color=MD3_TEXT_PRIMARY,
            command=lambda: self._jog(0, -1), 
            state="disabled"
        )
        self.jog_y_down.grid(row=2, column=1, pady=3, padx=3)
        
        # Jog Step Selection
        step_lbl = ctk.CTkLabel(jog_box, text="Jog Step Size (mm):", text_color=MD3_TEXT_SECONDARY, font=ctk.CTkFont(family="Segoe UI", size=12))
        step_lbl.pack(pady=(5,2))
        
        self.step_var = ctk.StringVar(value="10")
        self.step_selector = ctk.CTkSegmentedButton(
            jog_box, 
            values=["0.1", "1", "10", "50"], 
            variable=self.step_var,
            corner_radius=20,
            selected_color=MD3_PRIMARY_CONTAINER,
            selected_hover_color="#5D45A0",
            text_color=MD3_TEXT_PRIMARY
        )
        self.step_selector.pack(fill="x", padx=30, pady=5)
        
        # Manual Pen / GRBL Command actions
        action_frame = ctk.CTkFrame(jog_box, fg_color="transparent")
        action_frame.pack(fill="x", padx=15, pady=(10, 15))
        action_frame.grid_columnconfigure((0, 1, 2), weight=1)
        
        self.jog_pen_down = ctk.CTkButton(
            action_frame, 
            text="Pen Down", 
            command=self._manual_pen_down, 
            fg_color=MD3_BG, 
            hover_color="#2D2B33",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            state="disabled"
        )
        self.jog_pen_down.grid(row=0, column=0, padx=2)
        
        self.jog_pen_up = ctk.CTkButton(
            action_frame, 
            text="Pen Up", 
            command=self._manual_pen_up, 
            fg_color=MD3_BG, 
            hover_color="#2D2B33",
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            state="disabled"
        )
        self.jog_pen_up.grid(row=0, column=1, padx=2)
        
        self.jog_unlock = ctk.CTkButton(
            action_frame, 
            text="Unlock", 
            command=self._unlock_grbl, 
            fg_color=MD3_ERROR_CONTAINER, 
            hover_color="#B13A3A", 
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=20,
            state="disabled"
        )
        self.jog_unlock.grid(row=0, column=2, padx=2)

        # --- Console Log Row (Spans Bottom of Control Tab) ---
        self.console_box = ctk.CTkFrame(tab, border_width=1, border_color=MD3_BORDER, fg_color=MD3_SURFACE_HIGH, corner_radius=16)
        self.console_box.grid(row=1, column=0, columnspan=2, sticky="nsew", padx=10, pady=(5, 10))
        tab.grid_rowconfigure(1, weight=1) # Let console frame expand vertically
        
        self.console_box.grid_rowconfigure(0, weight=1)
        self.console_box.grid_rowconfigure(1, weight=0)
        self.console_box.grid_columnconfigure(0, weight=1)
        
        # Console text area
        self.console_txt = ctk.CTkTextbox(self.console_box, fg_color=MD3_BG, text_color="#00FF00", font=("Consolas", 10))
        self.console_txt.grid(row=0, column=0, sticky="nsew", padx=10, pady=(10, 5))
        self.console_txt.insert("end", "--- Serial Console Log Initiated ---\n")
        self.console_txt.configure(state="disabled")
        
        # Direct Command Input
        cmd_input_frame = ctk.CTkFrame(self.console_box, fg_color="transparent")
        cmd_input_frame.grid(row=1, column=0, sticky="nsew", padx=10, pady=(2, 10))
        
        cmd_input_lbl = ctk.CTkLabel(cmd_input_frame, text="CMD: ", font=ctk.CTkFont(family="Segoe UI", size=12, weight="bold"), text_color=MD3_PRIMARY)
        cmd_input_lbl.pack(side="left", padx=(0,5))
        
        self.console_input = ctk.CTkEntry(
            cmd_input_frame, 
            placeholder_text="Type raw G-code (e.g. $$ or G0 X50 Y50) and press Enter...", 
            height=30,
            fg_color=MD3_BG,
            border_color=MD3_BORDER,
            text_color=MD3_TEXT_PRIMARY,
            corner_radius=8
        )
        self.console_input.pack(side="left", fill="x", expand=True)
        self.console_input.bind("<Return>", self._send_manual_input_line)
        self.console_input.configure(state="disabled")
        
        # Scan ports on startup
        self._refresh_ports()

    def _create_section_header(self, text, parent=None):
        if parent is None:
            parent = self.left_frame
        lbl = ctk.CTkLabel(
            parent, 
            text=text, 
            font=ctk.CTkFont(family="Segoe UI", size=13, weight="bold"), 
            text_color=MD3_PRIMARY
        )
        lbl.pack(pady=(15, 5), anchor="w")
        return lbl

    def _setup_defaults(self):
        # Set default values for G-code options
        self.width_entry.insert(0, "100")
        self.height_entry.insert(0, "100")
        self.offset_x_entry.insert(0, "0")
        self.offset_y_entry.insert(0, "0")
        self.f_draw_entry.insert(0, "1000")
        self.f_travel_entry.insert(0, "2500")
        self.p_down_entry.insert(0, "M3 S90")
        self.p_up_entry.insert(0, "M5")
        self.p_delay_entry.insert(0, "0.25")

    def _update_ui_states(self):
        """Enables/disables parameters in the left panel based on current mode."""
        mode = self.mode_var.get()
        
        # Hide all conditional parameter frames
        self.contour_param_frame.pack_forget()
        self.hatch_param_frame.pack_forget()
        self.wave_param_frame.pack_forget()
        self.thresh_frame.pack_forget()
        self.dither_res_frame.pack_forget()
        
        if mode == "SVG":
            # Disable filter adjustments
            self._set_frame_state(self.filter_frame, "disabled")
            self.bright_slider.configure(state="disabled")
            self.contrast_slider.configure(state="disabled")
            self.invert_cb.configure(state="disabled")
            
            # Hide parameters
            self.param_frame.pack_forget()
            
            # Change tab view to Toolpath
            self.tabview.set("G-Code Toolpath")
        else:
            # Enable filter adjustments
            self.param_frame.pack(after=self.filter_frame, pady=5, padx=10, fill="x")
            self._set_frame_state(self.filter_frame, "normal")
            self.bright_slider.configure(state="normal")
            self.contrast_slider.configure(state="normal")
            self.invert_cb.configure(state="normal")
            
            if mode == "Outline":
                self.contour_param_frame.pack(fill="x")
                self.thresh_frame.pack(fill="x", before=self.invert_cb)
                self.tabview.set("Processed Image")
            elif mode == "Hatch":
                self.hatch_param_frame.pack(fill="x")
                self.thresh_frame.pack(fill="x", before=self.invert_cb)
                self.tabview.set("Processed Image")
            elif mode == "Dither":
                self.dither_res_frame.pack(fill="x", before=self.invert_cb)
                self.tabview.set("Processed Image")
            elif mode in ("Wave", "Spiral"):
                self.wave_param_frame.pack(fill="x")
                self.tabview.set("Processed Image")

    def _set_frame_state(self, frame, state):
        for child in frame.winfo_children():
            try:
                child.configure(state=state)
            except:
                pass

    # --- Image Slider Events ---
    def _on_mode_change(self, mode):
        self._update_ui_states()
        
        if mode == "SVG" and self.file_path and not self.file_path.lower().endswith(".svg"):
            messagebox.showwarning("Incompatible Mode", "The loaded file is not a vector SVG. SVG mode can only run on SVG files.")
            self.mode_var.set("Outline")
            self._update_ui_states()
            return
            
        if mode in ("Outline", "Hatch", "Dither", "Wave", "Spiral") and self.file_path.lower().endswith(".svg"):
            self._rasterize_svg()

        self.apply_filters()
        self.generate_gcode_preview()

    def _on_filter_slider_move(self, value):
        self.bright_lbl.configure(text=f"Brightness: {self.bright_slider.get():.2f}")
        self.contrast_lbl.configure(text=f"Contrast: {self.contrast_slider.get():.2f}")
        self.thresh_lbl.configure(text=f"Threshold: {int(self.thresh_slider.get())}")
        self.dither_res_lbl.configure(text=f"Dither Dots (Width): {int(self.dither_res_slider.get())} px")

    def _on_param_slider_move(self, value):
        self.epsilon_lbl.configure(text=f"Simplification (Epsilon): {self.epsilon_slider.get():.1f}")
        self.min_len_lbl.configure(text=f"Noise Filter (Min px): {int(self.min_len_slider.get())}")
        self.spacing_lbl.configure(text=f"Hatch Spacing (mm): {self.spacing_slider.get():.2f}")
        self.angle_lbl.configure(text=f"Hatch Angle (deg): {int(self.angle_slider.get())}")
        self.wave_spacing_lbl.configure(text=f"Line Spacing (mm): {self.wave_spacing_slider.get():.2f}")
        self.wave_freq_lbl.configure(text=f"Wave Frequency: {self.wave_freq_slider.get():.2f}")
        self.wave_amp_lbl.configure(text=f"Wave Amplitude (mm): {self.wave_amp_slider.get():.2f}")

    def _on_filter_changed(self):
        self.apply_filters()
        self.generate_gcode_preview()

    def _on_dimension_changed(self):
        self.generate_gcode_preview()

    def _on_laser_mode_changed(self):
        # Toggle pen parameter entry widgets state
        is_laser = self.laser_mode_var.get()
        state = "disabled" if is_laser else "normal"
        self._set_frame_state(self.pen_cmds_frame, state)
        for child in self.pen_cmds_frame.winfo_children():
            try:
                child.configure(state=state)
            except:
                pass
        self.generate_gcode_preview()

    # --- Loading & Preprocessing ---
    def _load_file(self):
        filetypes = [
            ("All Supported Files", "*.svg *.png *.jpg *.jpeg *.webp"),
            ("Vector SVG Files", "*.svg"),
            ("Raster Image Files", "*.png *.jpg *.jpeg *.webp"),
        ]
        
        path = filedialog.askopenfilename(title="Select Image or SVG", filetypes=filetypes)
        if not path:
            return
            
        self.file_path = path
        self.file_lbl.configure(text=os.path.basename(path))
        
        is_svg = path.lower().endswith(".svg")
        
        if is_svg:
            self.mode_var.set("SVG")
            self.raw_image = None
            self.processed_image = None
            self.processed_np = None
        else:
            try:
                self.raw_image = Image.open(path)
                if self.mode_var.get() == "SVG":
                    self.mode_var.set("Outline")
            except Exception as e:
                messagebox.showerror("Error", f"Could not load image: {e}")
                return
                
        self._update_ui_states()
        
        if is_svg and self.mode_var.get() in ("Outline", "Hatch", "Dither"):
            self._rasterize_svg()
        else:
            self.apply_filters()
            
        self.generate_gcode_preview()

    def _rasterize_svg(self):
        if not self.file_path or not self.file_path.lower().endswith(".svg"):
            return
            
        try:
            raw_paths, svg_w, svg_h = self.generator.svg_parser.parse_svg(self.file_path)
            if not raw_paths:
                self.raw_image = Image.new("L", (500, 500), 255)
                self.apply_filters()
                return

            canvas_w = 1000
            aspect = svg_h / svg_w if svg_w > 0 else 1.0
            canvas_h = int(canvas_w * aspect)
            
            img = Image.new("L", (canvas_w, canvas_h), 255)
            from PIL import ImageDraw
            draw = ImageDraw.Draw(img)
            
            scale_x = canvas_w / svg_w
            scale_y = canvas_h / svg_h
            
            for path in raw_paths:
                scaled_path = [(pt[0] * scale_x, pt[1] * scale_y) for pt in path]
                for i in range(1, len(scaled_path)):
                    draw.line([scaled_path[i-1], scaled_path[i]], fill=0, width=2)
                    
            self.raw_image = img
            self.apply_filters()
        except Exception as e:
            messagebox.showerror("SVG Rasterization Error", f"Could not rasterize SVG: {e}")

    def apply_filters(self):
        if self.mode_var.get() == "SVG" or self.raw_image is None:
            self.img_preview_lbl.configure(image=None, text="Processed Image View not available in SVG Mode.\nSVG paths are loaded directly as vectors.")
            return
            
        mode = self.mode_var.get()
        self.processed_image, self.processed_np = self.generator.preprocess_image(
            self.raw_image,
            contrast=self.contrast_slider.get(),
            brightness=self.bright_slider.get(),
            invert=self.invert_var.get(),
            threshold=int(self.thresh_slider.get()),
            mode=mode,
            dither_res=int(self.dither_res_slider.get())
        )
        
        self._display_processed_image()

    def _display_processed_image(self):
        if self.processed_image is None:
            return
            
        frame_w = self.img_tab.winfo_width()
        frame_h = self.img_tab.winfo_height()
        
        if frame_w < 50 or frame_h < 50:
            frame_w, frame_h = 600, 500
            
        img_w, img_h = self.processed_image.size
        
        scale = min(frame_w / img_w, frame_h / img_h) * 0.95
        new_w = int(img_w * scale)
        new_h = int(img_h * scale)
        
        if new_w <= 0 or new_h <= 0:
            return
            
        self.tk_image = ctk.CTkImage(light_image=self.processed_image, dark_image=self.processed_image, size=(new_w, new_h))
        self.img_preview_lbl.configure(image=self.tk_image, text="")

    # --- Toolpath Render ---
    def generate_gcode_preview(self, event=None):
        try:
            settings = {
                'width_mm': float(self.width_entry.get()),
                'height_mm': float(self.height_entry.get()),
                'scale_mode': self.scale_mode_var.get(),
                'origin_x': float(self.offset_x_entry.get()),
                'origin_y': float(self.offset_y_entry.get()),
                'flip_y': self.flip_y_var.get(),
                'feedrate_draw': float(self.f_draw_entry.get()),
                'feedrate_travel': float(self.f_travel_entry.get()),
                'pen_down_cmd': self.p_down_entry.get(),
                'pen_up_cmd': self.p_up_entry.get(),
                'pen_delay': float(self.p_delay_entry.get()),
                'contour_epsilon': self.epsilon_slider.get(),
                'min_contour_len': int(self.min_len_slider.get()),
                'hatch_spacing': self.wave_spacing_slider.get() if self.mode_var.get() in ("Wave", "Spiral") else self.spacing_slider.get(),
                'hatch_angle': self.angle_slider.get(),
                'cross_hatch': self.cross_hatch_var.get(),
                'wave_freq': self.wave_freq_slider.get(),
                'wave_amp': self.wave_amp_slider.get(),
                'optimize': self.optimize_var.get(),
                'laser_mode': self.laser_mode_var.get(),
            }
        except ValueError:
            return

        mode = self.mode_var.get()
        
        # Increment job ID
        self._current_job_id += 1
        job_id = self._current_job_id
        
        # Show generating status
        self.status_distance.configure(text="Generating G-Code in background thread...")
        
        import threading
        def worker(jid, f_path, img_np, m, s):
            try:
                gcode, paths, stats = self.generator.generate_gcode(f_path, img_np, m, s)
                self.after(0, self._on_gcode_generated, jid, gcode, paths, stats, s)
            except Exception as e:
                self.after(0, lambda: self._log_to_console(f"G-code Thread Error: {e}\n"))
                
        # Start thread
        t = threading.Thread(target=worker, args=(job_id, self.file_path, self.processed_np, mode, settings), daemon=True)
        t.start()

    def _on_gcode_generated(self, job_id, gcode, paths, stats, settings):
        """Generational callback for background G-code generation."""
        if job_id != self._current_job_id:
            # Outdated job result, discard
            return
            
        self.generated_gcode = gcode
        self.preview_paths = paths
        self.stats = stats
        
        self._update_status_bar(settings)
        self._draw_toolpath()

    def _update_status_bar(self, settings):
        if not self.stats:
            self.status_scale.configure(text="Dimensions: 0 x 0 mm")
            self.status_distance.configure(text="Distance: Travel: 0mm | Draw: 0mm")
            self.status_time.configure(text="Estimated Time: 00:00:00")
            self.status_pen.configure(text="Pen Actions: 0")
            return
            
        w = settings['width_mm']
        h = settings['height_mm']
        self.status_scale.configure(text=f"Dimensions: {w} x {h} mm")
        
        trav = self.stats['travel_distance_mm']
        draw = self.stats['draw_distance_mm']
        self.status_distance.configure(text=f"Distance: Travel: {trav:.0f}mm | Draw: {draw:.0f}mm")
        
        time_sec = self.stats['est_time_sec']
        h_est = time_sec // 3600
        m_est = (time_sec % 3600) // 60
        s_est = time_sec % 60
        self.status_time.configure(text=f"Est. Time: {h_est:02d}:{m_est:02d}:{s_est:02d}")
        
        self.status_pen.configure(text=f"Pen Actions: {self.stats['pen_actions']}")

    def _draw_toolpath(self):
        canvas = self.preview_canvas
        canvas.delete("all")
        
        if not self.preview_paths:
            canvas.create_text(
                canvas.winfo_width() / 2, 
                canvas.winfo_height() / 2, 
                text="No Toolpath Generated\nLoad a file and click 'Generate G-Code'.", 
                fill="gray",
                font=("Segoe UI", 12),
                justify="center"
            )
            return

        try:
            target_w = float(self.width_entry.get())
            target_h = float(self.height_entry.get())
            origin_x = float(self.offset_x_entry.get())
            origin_y = float(self.offset_y_entry.get())
        except ValueError:
            return

        c_w = canvas.winfo_width()
        c_h = canvas.winfo_height()
        
        if c_w < 50 or c_h < 50:
            return
            
        min_x = min(0.0, origin_x)
        max_x = max(target_w, target_w + origin_x)
        min_y = min(0.0, origin_y)
        max_y = max(target_h, target_h + origin_y)
        
        margin = 35
        
        scale_x = (c_w - 2 * margin) / (max_x - min_x) if max_x != min_x else 1.0
        scale_y = (c_h - 2 * margin) / (max_y - min_y) if max_y != min_y else 1.0
        scale = min(scale_x, scale_y)
        
        draw_w = (max_x - min_x) * scale
        draw_h = (max_y - min_y) * scale
        offset_canvas_x = margin + (c_w - 2 * margin - draw_w) / 2.0
        offset_canvas_y = margin + (c_h - 2 * margin - draw_h) / 2.0
        
        def to_canvas(x, y):
            cx_val = offset_canvas_x + (x - min_x) * scale
            cy_val = offset_canvas_y + (max_y - y) * scale
            return cx_val, cy_val

        # --- 1. Draw Grid Lines ---
        grid_spacing = 10.0
        if target_w > 200 or target_h > 200:
            grid_spacing = 50.0
        elif target_w < 20 or target_h < 20:
            grid_spacing = 5.0
            
        for y_mm in np.arange(math.floor(min_y), math.ceil(max_y) + 1, grid_spacing):
            cx1, cy1 = to_canvas(min_x, y_mm)
            cx2, cy2 = to_canvas(max_x, y_mm)
            canvas.create_line(cx1, cy1, cx2, cy2, fill="#252429", width=1)
            canvas.create_text(cx1 - 15, cy1, text=f"{int(y_mm)}", fill=MD3_TEXT_SECONDARY, font=("Arial", 8))

        for x_mm in np.arange(math.floor(min_x), math.ceil(max_x) + 1, grid_spacing):
            cx1, cy1 = to_canvas(x_mm, min_y)
            cx2, cy2 = to_canvas(x_mm, max_y)
            canvas.create_line(cx1, cy1, cx2, cy2, fill="#252429", width=1)
            canvas.create_text(cx1, cy2 + 10, text=f"{int(x_mm)}", fill=MD3_TEXT_SECONDARY, font=("Arial", 8))

        # --- 2. Draw Printable Frame (MD3 Secondary Accent Color) ---
        tcx1, tcy1 = to_canvas(origin_x, origin_y)
        tcx2, tcy2 = to_canvas(origin_x + target_w, origin_y + target_h)
        canvas.create_rectangle(tcx1, tcy1, tcx2, tcy2, outline=MD3_PRIMARY, width=2, dash=(4, 4))
        
        canvas.create_text(
            (tcx1 + tcx2)/2, 
            tcy1 - 10, 
            text=f"Print Bed Boundary: {target_w}x{target_h}mm", 
            fill=MD3_PRIMARY, 
            font=("Segoe UI", 9, "bold")
        )

        # --- 3. Draw Axis Origin Indicator ---
        ocx, ocy = to_canvas(0.0, 0.0)
        canvas.create_oval(ocx-5, ocy-5, ocx+5, ocy+5, fill="#FFA500", outline="white", width=1.5)
        canvas.create_text(ocx+15, ocy-10, text="Origin (0,0)", fill="#FFA500", font=("Segoe UI", 8, "bold"))
        
        canvas.create_line(ocx, ocy, ocx + 35, ocy, fill="#FF5252", width=2, arrow="last") # X Axis (Red)
        canvas.create_line(ocx, ocy, ocx, ocy - 35, fill="#66BB6A", width=2, arrow="last") # Y Axis (Green)

        # --- 4. Draw Toolpaths ---
        # Create a transparent PIL image for high-performance path rendering
        tp_img = Image.new("RGBA", (c_w, c_h), (0, 0, 0, 0))
        tp_draw = ImageDraw.Draw(tp_img)
        
        for path_type, path_pts in self.preview_paths:
            if not path_pts:
                continue
            if len(path_pts) < 2:
                # Handle single-dot paths (Dither Mode stippling)
                cx, cy = to_canvas(path_pts[0][0], path_pts[0][1])
                tp_draw.ellipse([cx-1.5, cy-1.5, cx+1.5, cy+1.5], fill=(0, 240, 255, 255))
                continue
                
            canvas_pts = [to_canvas(x, y) for x, y in path_pts]
            
            if path_type == 'travel':
                tp_draw.line(canvas_pts, fill=(141, 110, 99, 150), width=1) # Semi-transparent brown
            else:
                tp_draw.line(canvas_pts, fill=(0, 240, 255, 255), width=2) # Cyan draw lines
                
        # Convert to PhotoImage and display on canvas in a single step
        self.toolpath_photo = ImageTk.PhotoImage(tp_img)
        canvas.create_image(0, 0, image=self.toolpath_photo, anchor="nw")

    def _save_file(self):
        """Opens a file dialog to save G-code text."""
        if not self.generated_gcode:
            messagebox.showwarning("Warning", "No G-code has been generated yet. Please load a file and adjust settings first.")
            return
            
        filetypes = [
            ("G-Code Files", "*.gcode *.nc *.g *.tap"),
            ("Text Files", "*.txt"),
            ("All Files", "*.*")
        ]
        
        default_ext = ".gcode"
        suggested_name = "output"
        if self.file_path:
            suggested_name = os.path.splitext(os.path.basename(self.file_path))[0]
            
        save_path = filedialog.asksaveasfilename(
            title="Save G-Code File",
            initialfile=f"{suggested_name}_plotter",
            defaultextension=default_ext,
            filetypes=filetypes
        )
        
        if not save_path:
            return
            
        try:
            with open(save_path, 'w') as f:
                f.write(self.generated_gcode)
            messagebox.showinfo("Success", f"G-code successfully saved to:\n{save_path}")
        except Exception as e:
            messagebox.showerror("Error", f"Could not save file: {e}")

    # ====================================================
    # SERIAL PORT & JOGGING METHODS (Plotter Control Tab)
    # ====================================================
    def _refresh_ports(self):
        ports = self.serial_mgr.get_ports()
        if not ports:
            ports = ["No Ports Found"]
            self.port_var.set("No Ports Found")
        else:
            self.port_var.set(ports[0])
            
        self.port_menu.configure(values=ports)
        self._log_to_console(f"Port scan completed. Found ports: {ports}")

    def _toggle_connection(self):
        if self.serial_mgr.is_connected:
            self.serial_mgr.disconnect()
        else:
            port = self.port_var.get()
            if port == "Select Port" or port == "No Ports Found":
                messagebox.showerror("Connection Error", "Please select a valid COM port.")
                return
            
            baud = int(self.baud_var.get())
            self.serial_mgr.connect(port, baud, self._serial_event_callback)

    def _serial_event_callback(self, status_type, message, progress_pct):
        """Thread-safe serial callback dropping messages in the queue."""
        self.gui_queue.put((status_type, message, progress_pct))

    def _process_gui_queue(self):
        """Polled every 50ms to execute status changes safely on the main GUI thread."""
        while not self.gui_queue.empty():
            try:
                status_type, message, progress_pct = self.gui_queue.get_nowait()
                self._handle_serial_event(status_type, message, progress_pct)
            except queue.Empty:
                break
        self.after(50, self._process_gui_queue)

    def _handle_serial_event(self, status_type, message, progress):
        """Processes events and updates sliders, consoles, and progress bars."""
        if status_type == "connect":
            self._log_to_console(f"SYSTEM: {message}")
            self.connect_btn.configure(text="Disconnect", fg_color=MD3_ERROR_CONTAINER, hover_color="#A12A2A")
            self._set_control_widgets_state("normal")
            self.stream_status_lbl.configure(text="Status: Connected / Idle")
            
        elif status_type == "disconnect":
            self._log_to_console("SYSTEM: Connection closed.")
            self.connect_btn.configure(text="Connect", fg_color=MD3_SECONDARY, hover_color="#004D40")
            self._set_control_widgets_state("disabled")
            self.stream_status_lbl.configure(text="Status: Disconnected")
            self.stream_progress.set(0.0)
            
        elif status_type == "console_rx":
            self._log_to_console(f"RX: {message}")
            
        elif status_type == "console_tx":
            self._log_to_console(f"TX: {message}")
            
        elif status_type == "error":
            self._log_to_console(f"ERROR: {message}")
            messagebox.showerror("Plotter Error", message)
            
        elif status_type == "warning":
            self._log_to_console(f"WARNING: {message}")
            
        elif status_type == "stream_start":
            self._log_to_console(f"SYSTEM: {message}")
            self.stream_status_lbl.configure(text="Status: Streaming G-code...")
            self.stream_send_btn.configure(state="disabled")
            self.stream_pause_btn.configure(state="normal")
            self.stream_resume_btn.configure(state="disabled")
            self.stream_abort_btn.configure(state="normal")
            
        elif status_type == "stream_line":
            self._log_to_console(f"TX: {message}")
            self.stream_progress.set(progress / 100.0)
            self.stream_status_lbl.configure(text=f"Status: Streaming ({progress}%)")
            
        elif status_type == "stream_paused":
            self._log_to_console(f"SYSTEM: {message}")
            self.stream_status_lbl.configure(text=f"Status: Paused ({progress}%)")
            self.stream_pause_btn.configure(state="disabled")
            self.stream_resume_btn.configure(state="normal")
            
        elif status_type == "stream_resumed":
            self._log_to_console(f"SYSTEM: {message}")
            self.stream_status_lbl.configure(text=f"Status: Streaming ({progress}%)")
            self.stream_pause_btn.configure(state="normal")
            self.stream_resume_btn.configure(state="disabled")
            
        elif status_type == "stream_stopped":
            self._log_to_console(f"SYSTEM: {message}")
            self.stream_status_lbl.configure(text="Status: Aborted / Reset")
            self.stream_progress.set(0.0)
            self.stream_send_btn.configure(state="normal")
            self.stream_pause_btn.configure(state="disabled")
            self.stream_resume_btn.configure(state="disabled")
            self.stream_abort_btn.configure(state="disabled")
            
        elif status_type == "stream_finish":
            self._log_to_console(f"SYSTEM: {message}")
            self.stream_status_lbl.configure(text="Status: Completed Successfully")
            self.stream_progress.set(1.0)
            self.stream_send_btn.configure(state="normal")
            self.stream_pause_btn.configure(state="disabled")
            self.stream_resume_btn.configure(state="disabled")
            self.stream_abort_btn.configure(state="disabled")
            messagebox.showinfo("Success", "G-code streaming completed successfully!")

    def _set_control_widgets_state(self, state):
        """Toggle buttons availability depending on connection status."""
        self.stream_send_btn.configure(state=state)
        self.jog_y_up.configure(state=state)
        self.jog_x_left.configure(state=state)
        self.jog_zero.configure(state=state)
        self.jog_x_right.configure(state=state)
        self.jog_y_down.configure(state=state)
        self.jog_pen_down.configure(state=state)
        self.jog_pen_up.configure(state=state)
        self.jog_unlock.configure(state=state)
        self.console_input.configure(state=state)

    def _log_to_console(self, text):
        self.console_txt.configure(state="normal")
        self.console_txt.insert("end", f"{text}\n")
        self.console_txt.see("end")
        self.console_txt.configure(state="disabled")

    def _send_manual_input_line(self, event=None):
        cmd = self.console_input.get().strip()
        if not cmd:
            return
            
        self.console_input.delete(0, "end")
        self.serial_mgr.write_command(cmd)

    def _jog(self, dx, dy):
        """Sends GRBL jog command ($J) based on selected step size."""
        if not self.serial_mgr.is_connected:
            return
            
        try:
            step = float(self.step_var.get())
        except ValueError:
            step = 10.0
            
        jog_feed = 2000
        
        cmd = "$J=G91 G21 "
        if dx != 0:
            cmd += f"X{dx * step:.3f} "
        if dy != 0:
            cmd += f"Y{dy * step:.3f} "
        cmd += f"F{jog_feed}"
        
        self.serial_mgr.write_command(cmd)

    def _zero_axes(self):
        """Resets coordinate system offset to current location (G92 X0 Y0)."""
        if messagebox.askyesno("Zero Coordinates", "Set current location as coordinate origin X0 Y0?"):
            self.serial_mgr.write_command("G92 X0 Y0")

    def _unlock_grbl(self):
        """Clears ALARM lock ($X) on GRBL board."""
        self.serial_mgr.write_command("$X")

    def _manual_pen_down(self):
        """Executes current Pen Down instruction manually."""
        is_laser = self.laser_mode_var.get()
        cmd = "M3 S1000" if is_laser else self.p_down_entry.get().strip()
        self.serial_mgr.write_command(cmd)

    def _manual_pen_up(self):
        """Executes current Pen Up instruction manually."""
        is_laser = self.laser_mode_var.get()
        cmd = "M5" if is_laser else self.p_up_entry.get().strip()
        self.serial_mgr.write_command(cmd)

    # --- Streaming Playback Actions ---
    def _start_stream(self):
        if not self.generated_gcode:
            messagebox.showwarning("No G-Code", "Please load a file and generate G-code first.")
            return
            
        if messagebox.askyesno("Start Plot", "Confirm that your plotter bed is clear and start drawing?"):
            self.serial_mgr.start_stream(self.generated_gcode)

    def _pause_stream(self):
        self.serial_mgr.pause_stream()

    def _resume_stream(self):
        self.serial_mgr.resume_stream()

    def _abort_stream(self):
        if messagebox.askyesnocancel("Abort Plot", "Are you sure you want to stop the machine immediately and cancel current work?"):
            self.serial_mgr.stop_stream()

    def _on_tab_changed(self):
        """Triggers scaling/redraw logic when switching between tabs to guarantee correct bounding layouts."""
        current_tab = self.tabview.get()
        if current_tab == "Processed Image":
            self._display_processed_image()
        elif current_tab == "G-Code Toolpath":
            self._draw_toolpath()

    def _on_window_resize(self, event):
        """Responsive screen adjustment. Adapts panels and stacking structures to size changes."""
        if event.widget != self:
            return
            
        w = event.width
        h = event.height
        
        # 1. Main frame responsiveness (Sidebar layout vs. Top-Down Stack)
        if w < 900:
            # Narrow layout: stack controls vertically
            self.grid_columnconfigure(0, weight=1, minsize=0)
            self.grid_columnconfigure(1, weight=1)
            self.left_frame.grid(row=0, column=0, columnspan=2, sticky="nsew", padx=10, pady=5)
            self.right_frame.grid(row=1, column=0, columnspan=2, sticky="nsew", padx=10, pady=5)
            self.grid_rowconfigure(0, weight=0) # Sidebar fits its parameters
            self.grid_rowconfigure(1, weight=1) # Visual area takes priority
        else:
            # Wide layout: standard split columns
            self.grid_columnconfigure(0, weight=0, minsize=380)
            self.grid_columnconfigure(1, weight=1)
            self.left_frame.grid(row=0, column=0, columnspan=1, sticky="nsew", padx=10, pady=10)
            self.right_frame.grid(row=0, column=1, columnspan=1, sticky="nsew", padx=10, pady=10)
            self.grid_rowconfigure(0, weight=1)
            self.grid_rowconfigure(1, weight=0)

        # 2. Plotter Control tab column stacking
        rf_w = self.right_frame.winfo_width()
        
        # If right frame width is small, stack left/right serial and jog pads vertically
        if rf_w < 780:
            self.control_tab.grid_columnconfigure(0, weight=1, minsize=0)
            self.control_tab.grid_columnconfigure(1, weight=1, minsize=0)
            
            self.left_ctrl_frame.grid(row=0, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
            self.right_ctrl_frame.grid(row=1, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
            self.console_box.grid(row=2, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
            
            self.control_tab.grid_rowconfigure(0, weight=0)
            self.control_tab.grid_rowconfigure(1, weight=0)
            self.control_tab.grid_rowconfigure(2, weight=1)
        else:
            self.control_tab.grid_columnconfigure(0, weight=1, minsize=400)
            self.control_tab.grid_columnconfigure(1, weight=1, minsize=350)
            
            self.left_ctrl_frame.grid(row=0, column=0, columnspan=1, sticky="nsew", padx=5, pady=5)
            self.right_ctrl_frame.grid(row=0, column=1, columnspan=1, sticky="nsew", padx=5, pady=5)
            self.console_box.grid(row=1, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
            
            self.control_tab.grid_rowconfigure(0, weight=1)
            self.control_tab.grid_rowconfigure(1, weight=1)
            self.control_tab.grid_rowconfigure(2, weight=0)

        # 3. Trigger image resize if Processed Image tab is active
        if self.tabview.get() == "Processed Image" and self.processed_image is not None:
            self._display_processed_image()
