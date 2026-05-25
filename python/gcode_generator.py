import cv2
import numpy as np
from PIL import Image, ImageEnhance, ImageOps
import os
import math
from svg_processor import SVGPathParser

class GCodeGenerator:
    def __init__(self):
        self.svg_parser = SVGPathParser()

    def preprocess_image(self, pil_img, contrast=1.0, brightness=1.0, invert=False, threshold=127, mode='Outline', dither_res=150):
        """
        Applies contrast, brightness, inversion, and either binary threshold or Floyd-Steinberg
        dithering to a PIL Image. Returns both the binary PIL Image and OpenCV-compatible numpy array.
        """
        # Convert to grayscale
        img = pil_img.convert('L')
        
        # Adjust brightness
        if brightness != 1.0:
            enhancer = ImageEnhance.Brightness(img)
            img = enhancer.enhance(brightness)
            
        # Adjust contrast
        if contrast != 1.0:
            enhancer = ImageEnhance.Contrast(img)
            img = enhancer.enhance(contrast)
            
        # Invert colors if selected
        if invert:
            img = ImageOps.invert(img)
            
        if mode == 'Dither':
            # Resize image for dither resolution
            aspect = img.height / img.width
            dither_w = int(dither_res)
            dither_h = int(dither_res * aspect)
            if dither_w <= 0 or dither_h <= 0:
                dither_w, dither_h = 100, 100
                
            img_resized = img.resize((dither_w, dither_h), Image.Resampling.LANCZOS)
            
            # Apply Floyd-Steinberg dither (Pillow convert('1'))
            dithered_pil = img_resized.convert('1')
            thresh_np = np.array(dithered_pil).astype(np.uint8) * 255
            
            thresh_pil = Image.fromarray(thresh_np)
            return thresh_pil, thresh_np
            
        if mode in ('Wave', 'Spiral'):
            return img, np.array(img)
            
        # Convert to numpy array for thresholding
        img_np = np.array(img)
        
        # Apply threshold (pixels above threshold become 255, below become 0)
        _, thresh_np = cv2.threshold(img_np, threshold, 255, cv2.THRESH_BINARY)
        
        # Convert back to PIL for GUI preview
        thresh_pil = Image.fromarray(thresh_np)
        
        return thresh_pil, thresh_np

    def optimize_paths(self, paths, start_pos=(0.0, 0.0)):
        """
        Sorts paths using a Nearest-Neighbor Traveling Salesperson (TSP) heuristic.
        Minimizes travel moves (pen-up G0).
        Uses highly optimized NumPy vectorization for instantaneous processing of large datasets (e.g. Dither).
        """
        if not paths:
            return []
            
        N = len(paths)
        if N <= 1:
            return list(paths)

        # Convert start and end coordinates of all paths to numpy arrays
        starts = np.empty((N, 2), dtype=np.float32)
        ends = np.empty((N, 2), dtype=np.float32)
        
        for idx, path in enumerate(paths):
            if len(path) > 0:
                starts[idx] = path[0]
                ends[idx] = path[-1]
            else:
                starts[idx] = (0.0, 0.0)
                ends[idx] = (0.0, 0.0)

        active = np.ones(N, dtype=bool)
        optimized = []
        curr_pos = np.array(start_pos, dtype=np.float32)
        
        orig_indices = np.arange(N, dtype=np.int32)
        
        for _ in range(N):
            active_mask = active
            active_indices = orig_indices[active_mask]
            if len(active_indices) == 0:
                break
                
            starts_active = starts[active_mask]
            ends_active = ends[active_mask]
            
            # Vectorized squared distance calculations
            dists_start = np.sum((starts_active - curr_pos) ** 2, axis=1)
            dists_end = np.sum((ends_active - curr_pos) ** 2, axis=1)
            
            min_start_idx = np.argmin(dists_start)
            min_end_idx = np.argmin(dists_end)
            
            min_start_val = dists_start[min_start_idx]
            min_end_val = dists_end[min_end_idx]
            
            if min_start_val <= min_end_val:
                best_sub_idx = min_start_idx
                reverse_path = False
            else:
                best_sub_idx = min_end_idx
                reverse_path = True
                
            # Map back to original index
            original_idx = active_indices[best_sub_idx]
            path = paths[original_idx]
            
            if reverse_path and len(path) > 1:
                path = path[::-1]
                
            optimized.append(path)
            
            # Update current position to the end of the chosen path
            if len(path) > 0:
                curr_pos[0] = path[-1][0]
                curr_pos[1] = path[-1][1]
                
            # Deactivate path
            active[original_idx] = False
            
        return optimized

    def generate_gcode(self, file_path, img_np, mode, settings):
        """
        Generates G-code from either an SVG (vector) or a numpy thresholded image (raster).
        
        settings is a dictionary containing:
            - width_mm (float)
            - height_mm (float)
            - scale_mode (str: 'fit' or 'stretch')
            - origin_x (float)
            - origin_y (float)
            - flip_y (bool)
            - feedrate_draw (float)
            - feedrate_travel (float)
            - pen_down_cmd (str)
            - pen_up_cmd (str)
            - pen_delay (float)
            - contour_epsilon (float)
            - min_contour_len (float)
            - hatch_spacing (float)
            - hatch_angle (float)
            - cross_hatch (bool)
            - optimize (bool)
            - laser_mode (bool)
        """
        # 1. Extract paths based on mode
        paths = [] # list of lists of (x, y) coordinates
        
        target_w = settings.get('width_mm', 100.0)
        target_h = settings.get('height_mm', 100.0)
        scale_mode = settings.get('scale_mode', 'fit')
        
        if mode == 'SVG' and file_path and file_path.lower().endswith('.svg'):
            # Parse SVG vector paths
            raw_paths, svg_w, svg_h = self.svg_parser.parse_svg(file_path)
            
            # Scale vector paths to fit the target bounding box
            paths = self._scale_and_align_vector_paths(raw_paths, svg_w, svg_h, target_w, target_h, scale_mode)
            
        else: # Raster modes (requires image)
            if img_np is None:
                return "; No image loaded\n", [], {}
                
            img_h_px, img_w_px = img_np.shape
            
            if mode == 'Contour':
                # Trace outlines
                # Invert threshold image for contour detection (so black lines become white contours)
                inverted_np = cv2.bitwise_not(img_np)
                contours, hierarchy = cv2.findContours(inverted_np, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
                
                epsilon = settings.get('contour_epsilon', 1.0)
                min_len = settings.get('min_contour_len', 2.0)
                
                raw_paths = []
                for cnt in contours:
                    # Simplify contour using Douglas-Peucker
                    if epsilon > 0:
                        approx = cv2.approxPolyDP(cnt, epsilon * 0.05, False)
                    else:
                        approx = cnt
                        
                    # Filter out tiny noise contours
                    path_len = len(approx)
                    if path_len >= min_len:
                        path_pts = []
                        for pt in approx:
                            path_pts.append((float(pt[0][0]), float(pt[0][1])))
                        # If closed loop, append first point to close it
                        if len(path_pts) > 2:
                            path_pts.append(path_pts[0])
                        raw_paths.append(path_pts)
                
                # Scale pixel coordinates to physical dimensions
                paths = self._scale_and_align_pixel_paths(raw_paths, img_w_px, img_h_px, target_w, target_h, scale_mode)
                
            elif mode == 'Hatch':
                spacing_mm = settings.get('hatch_spacing', 1.0)
                angle_deg = settings.get('hatch_angle', 45.0)
                cross_hatch = settings.get('cross_hatch', False)
                
                scale_x, scale_y, offset_x, offset_y = self._get_scaling_factors(img_w_px, img_h_px, target_w, target_h, scale_mode)
                avg_scale = (scale_x + scale_y) / 2.0
                spacing_px = max(1.0, spacing_mm / avg_scale)
                
                # Generate main hatch paths
                raw_paths = self._generate_hatching_paths(img_np, spacing_px, angle_deg)
                
                # Generate perpendicular hatch paths if cross-hatching is checked
                if cross_hatch:
                    raw_paths2 = self._generate_hatching_paths(img_np, spacing_px, angle_deg + 90.0)
                    raw_paths.extend(raw_paths2)
                
                # Scale pixel coordinates to physical dimensions
                paths = self._scale_and_align_pixel_paths(raw_paths, img_w_px, img_h_px, target_w, target_h, scale_mode)
                
            elif mode == 'Dither':
                # Floyd-Steinberg dots: black pixels have value < 127
                ys, xs = np.where(img_np < 127)
                raw_paths = [[(float(x), float(y)), (float(x), float(y))] for x, y in zip(xs, ys)]
                            
                # Scale pixel coordinates to physical dimensions
                paths = self._scale_and_align_pixel_paths(raw_paths, img_w_px, img_h_px, target_w, target_h, scale_mode)
                
            elif mode == 'Wave':
                spacing_mm = settings.get('hatch_spacing', 1.0)
                freq = settings.get('wave_freq', 0.3)
                amp_mm = settings.get('wave_amp', 1.0)
                
                scale_x, scale_y, offset_x, offset_y = self._get_scaling_factors(img_w_px, img_h_px, target_w, target_h, scale_mode)
                
                # Line spacing in pixels
                spacing_px = max(1.0, spacing_mm / scale_y)
                amp_px = amp_mm / scale_y
                
                raw_paths = []
                x_coords = np.arange(0, img_w_px, 1.0)
                y_coords = np.arange(0, img_h_px, spacing_px)
                
                for y_center in y_coords:
                    path = []
                    y_idx = int(round(y_center))
                    if y_idx < 0 or y_idx >= img_h_px:
                        continue
                        
                    for x in x_coords:
                        x_idx = int(x)
                        px_val = img_np[y_idx, x_idx] / 255.0
                        
                        displacement = math.sin(x * freq) * amp_px * (1.0 - px_val)
                        y_val = y_center + displacement
                        
                        y_val = max(0.0, min(img_h_px - 1.0, y_val))
                        path.append((float(x), float(y_val)))
                        
                    if len(path) > 1:
                        raw_paths.append(path)
                        
                paths = self._scale_and_align_pixel_paths(raw_paths, img_w_px, img_h_px, target_w, target_h, scale_mode)
                
            elif mode == 'Spiral':
                spacing_mm = settings.get('hatch_spacing', 1.0)
                freq = settings.get('wave_freq', 0.3)
                amp_mm = settings.get('wave_amp', 1.0)
                
                scale_x, scale_y, offset_x, offset_y = self._get_scaling_factors(img_w_px, img_h_px, target_w, target_h, scale_mode)
                avg_scale = (scale_x + scale_y) / 2.0
                
                cx_px = img_w_px / 2.0
                cy_px = img_h_px / 2.0
                max_r = math.hypot(cx_px, cy_px)
                
                pitch_px = max(1.0, spacing_mm / avg_scale)
                amp_px = amp_mm / avg_scale
                
                raw_paths = []
                path = []
                
                theta = 0.0
                r = 0.0
                arc_step = 2.0
                
                while r < max_r:
                    x = cx_px + r * math.cos(theta)
                    y = cy_px + r * math.sin(theta)
                    
                    x_idx = int(round(x))
                    y_idx = int(round(y))
                    
                    if 0 <= x_idx < img_w_px and 0 <= y_idx < img_h_px:
                        px_val = img_np[y_idx, x_idx] / 255.0
                    else:
                        px_val = 1.0
                        
                    displacement = math.sin(theta * freq) * amp_px * (1.0 - px_val)
                    r_mod = r + displacement
                    
                    x_mod = cx_px + r_mod * math.cos(theta)
                    y_mod = cy_px + r_mod * math.sin(theta)
                    
                    x_mod = max(0.0, min(img_w_px - 1.0, x_mod))
                    y_mod = max(0.0, min(img_h_px - 1.0, y_mod))
                    
                    path.append((float(x_mod), float(y_mod)))
                    
                    if r > 0.1:
                        d_theta = arc_step / r
                    else:
                        d_theta = 0.5
                        
                    theta += d_theta
                    r = theta * pitch_px / (2.0 * math.pi)
                    
                if len(path) > 1:
                    raw_paths.append(path)
                    
                paths = self._scale_and_align_pixel_paths(raw_paths, img_w_px, img_h_px, target_w, target_h, scale_mode)

        # 2. Path Optimization (Nearest-Neighbor)
        origin_x = settings.get('origin_x', 0.0)
        origin_y = settings.get('origin_y', 0.0)
        optimize = settings.get('optimize', True)
        
        if optimize:
            # Optimize relative to coordinates before Y-flipping
            # Start search from bottom-left corner of the scaled canvas bounding box (scaled relative coordinates)
            paths = self.optimize_paths(paths, start_pos=(0.0, 0.0))

        # 3. Adjust coordinate orientation (Flipping Y axis for Cartesian CNC coordinates)
        flip_y = settings.get('flip_y', True)
        
        final_paths = []
        for path in paths:
            adj_path = []
            for x, y in path:
                fx = x + origin_x
                fy = (target_h - y) + origin_y if flip_y else y + origin_y
                adj_path.append((round(fx, 3), round(fy, 3)))
            if adj_path:
                final_paths.append(adj_path)

        # 4. Formulate G-code and Preview paths
        gcode_lines = []
        preview_paths = []
        
        # Header G-code
        gcode_lines.append("; G-code generated by Antigravity Image2Gcode")
        gcode_lines.append(f"; Mode: {mode}")
        gcode_lines.append(f"; Size: {target_w}mm x {target_h}mm")
        gcode_lines.append("G21 ; Set units to millimeters")
        gcode_lines.append("G90 ; Absolute positioning")
        
        feed_draw = settings.get('feedrate_draw', 1000.0)
        feed_travel = settings.get('feedrate_travel', 2000.0)
        
        # Check laser mode overrides
        laser_mode = settings.get('laser_mode', False)
        if laser_mode:
            pen_down = "M3 S1000"
            pen_up = "M5"
            pen_delay = 0.0
        else:
            pen_down = settings.get('pen_down_cmd', 'M3 S90').strip()
            pen_up = settings.get('pen_up_cmd', 'M5').strip()
            pen_delay = settings.get('pen_delay', 0.25)
            
        # Raise pen/turn off laser to start
        gcode_lines.append(f"{pen_up} ; Pen Up / Laser Off")
        if pen_delay > 0:
            gcode_lines.append(f"G4 P{pen_delay} ; Wait for pen up")
            
        gcode_lines.append(f"G0 F{feed_travel} ; Set travel feedrate")
        
        total_travel_dist = 0.0
        total_draw_dist = 0.0
        pen_actions = 0
        
        curr_pos = (origin_x, origin_y) # Start position
        
        for path in final_paths:
            if not path:
                continue
                
            start_pt = path[0]
            
            # Travel move to start of path
            travel_d = math.hypot(start_pt[0] - curr_pos[0], start_pt[1] - curr_pos[1])
            total_travel_dist += travel_d
            
            gcode_lines.append(f"G0 X{start_pt[0]:.3f} Y{start_pt[1]:.3f} ; Travel")
            preview_paths.append(('travel', [curr_pos, start_pt]))
            
            # Pen Down / Laser On
            gcode_lines.append(f"{pen_down} ; Pen Down / Laser On")
            pen_actions += 1
            if pen_delay > 0:
                gcode_lines.append(f"G4 P{pen_delay} ; Wait for pen down")
                
            # Draw moves
            gcode_lines.append(f"G1 F{feed_draw} ; Set drawing feedrate")
            
            draw_pts = [start_pt]
            for i in range(1, len(path)):
                pt = path[i]
                draw_d = math.hypot(pt[0] - path[i-1][0], pt[1] - path[i-1][1])
                total_draw_dist += draw_d
                
                gcode_lines.append(f"G1 X{pt[0]:.3f} Y{pt[1]:.3f} ; Draw")
                draw_pts.append(pt)
                
            preview_paths.append(('draw', draw_pts))
            
            curr_pos = path[-1]
            
            # Pen Up / Laser Off
            gcode_lines.append(f"{pen_up} ; Pen Up / Laser Off")
            pen_actions += 1
            if pen_delay > 0:
                gcode_lines.append(f"G4 P{pen_delay} ; Wait for pen up")

        # Go back to origin at travel speed
        travel_d = math.hypot(origin_x - curr_pos[0], origin_y - curr_pos[1])
        total_travel_dist += travel_d
        gcode_lines.append(f"G0 X{origin_x:.3f} Y{origin_y:.3f} ; Return to origin")
        preview_paths.append(('travel', [curr_pos, (origin_x, origin_y)]))
        
        gcode_lines.append("M30 ; End of program")
        
        # Calculate estimated run time
        travel_time_sec = total_travel_dist / (feed_travel / 60.0) if feed_travel > 0 else 0
        draw_time_sec = total_draw_dist / (feed_draw / 60.0) if feed_draw > 0 else 0
        delay_time_sec = pen_actions * pen_delay
        total_time_sec = travel_time_sec + draw_time_sec + delay_time_sec
        
        stats = {
            'travel_distance_mm': round(total_travel_dist, 1),
            'draw_distance_mm': round(total_draw_dist, 1),
            'pen_actions': pen_actions,
            'est_time_sec': int(total_time_sec),
        }
        
        gcode = "\n".join(gcode_lines) + "\n"
        return gcode, preview_paths, stats

    def _get_scaling_factors(self, w_px, h_px, target_w, target_h, scale_mode):
        """Calculates scaling factors and offsets to fit/stretch image coordinates."""
        if scale_mode == 'stretch':
            scale_x = target_w / w_px
            scale_y = target_h / h_px
            offset_x = 0.0
            offset_y = 0.0
        else: # 'fit' maintains aspect ratio
            img_aspect = w_px / h_px
            target_aspect = target_w / target_h
            
            if img_aspect > target_aspect:
                scale_x = scale_y = target_w / w_px
                offset_x = 0.0
                offset_y = (target_h - (h_px * scale_y)) / 2.0
            else:
                scale_x = scale_y = target_h / h_px
                offset_x = (target_w - (w_px * scale_x)) / 2.0
                offset_y = 0.0
                
        return scale_x, scale_y, offset_x, offset_y

    def _scale_and_align_pixel_paths(self, paths, w_px, h_px, target_w, target_h, scale_mode):
        """Scales pixel paths to target mm bounding box."""
        scale_x, scale_y, offset_x, offset_y = self._get_scaling_factors(w_px, h_px, target_w, target_h, scale_mode)
        
        scaled_paths = []
        for path in paths:
            sc_path = []
            for px, py in path:
                sx = px * scale_x + offset_x
                sy = py * scale_y + offset_y
                sc_path.append((sx, sy))
            scaled_paths.append(sc_path)
        return scaled_paths

    def _scale_and_align_vector_paths(self, paths, svg_w, svg_h, target_w, target_h, scale_mode):
        """Scales vector (SVG) paths to target mm bounding box."""
        scale_x, scale_y, offset_x, offset_y = self._get_scaling_factors(svg_w, svg_h, target_w, target_h, scale_mode)
        
        scaled_paths = []
        for path in paths:
            sc_path = []
            for px, py in path:
                sx = px * scale_x + offset_x
                sy = py * scale_y + offset_y
                sc_path.append((sx, sy))
            scaled_paths.append(sc_path)
        return scaled_paths

    def _generate_hatching_paths(self, thresh_img, spacing_px, angle_deg):
        """
        Generates parallel hatch lines inside the dark pixels (0) of thresh_img.
        Uses image rotation for easy scanline intersection.
        """
        h_px, w_px = thresh_img.shape
        
        diagonal = int(math.ceil(math.hypot(w_px, h_px)))
        
        pad_x = (diagonal - w_px) // 2
        pad_y = (diagonal - h_px) // 2
        
        padded = cv2.copyMakeBorder(thresh_img, pad_y, pad_y, pad_x, pad_x, cv2.BORDER_CONSTANT, value=255)
        pad_h, pad_w = padded.shape
        
        rot_matrix = cv2.getRotationMatrix2D((pad_w / 2.0, pad_h / 2.0), angle_deg, 1.0)
        rotated = cv2.warpAffine(padded, rot_matrix, (pad_w, pad_h), flags=cv2.INTER_NEAREST, borderMode=cv2.BORDER_CONSTANT, borderValue=255)
        
        inv_rot_matrix = cv2.getRotationMatrix2D((pad_w / 2.0, pad_h / 2.0), -angle_deg, 1.0)
        
        paths = []
        y_coords = np.arange(0, pad_h, spacing_px)
        
        for y in y_coords:
            r = int(round(y))
            if r < 0 or r >= pad_h:
                continue
                
            row = rotated[r, :]
            
            is_black = (row < 127).astype(np.int8)
            diff = np.diff(is_black)
            
            starts = np.where(diff == 1)[0] + 1
            ends = np.where(diff == -1)[0]
            
            if is_black[0]:
                starts = np.insert(starts, 0, 0)
            if is_black[-1]:
                ends = np.append(ends, pad_w - 1)
                
            for s, e in zip(starts, ends):
                if e - s < 1:
                    continue
                    
                pt1_rot = np.array([s, r, 1.0])
                pt2_rot = np.array([e, r, 1.0])
                
                pt1_orig = inv_rot_matrix.dot(pt1_rot)
                pt2_orig = inv_rot_matrix.dot(pt2_rot)
                
                x1_orig = pt1_orig[0] - pad_x
                y1_orig = pt1_orig[1] - pad_y
                x2_orig = pt2_orig[0] - pad_x
                y2_orig = pt2_orig[1] - pad_y
                
                x1_orig = max(0.0, min(w_px - 1.0, x1_orig))
                y1_orig = max(0.0, min(h_px - 1.0, y1_orig))
                x2_orig = max(0.0, min(w_px - 1.0, x2_orig))
                y2_orig = max(0.0, min(h_px - 1.0, y2_orig))
                
                paths.append([(x1_orig, y1_orig), (x2_orig, y2_orig)])
                
        return paths
