import xml.etree.ElementTree as ET
import re
import math

class SVGPathParser:
    """
    A simple, robust SVG path parser that handles:
    - Commands: M, m, L, l, H, h, V, v, C, c, Q, q, Z, z
    - Group transforms (translate, scale, matrix)
    - Approximation of curves (cubic & quadratic Beziers) to polyline segments
    """
    
    def __init__(self, bezier_steps=10):
        self.bezier_steps = bezier_steps

    def parse_svg(self, filepath):
        """
        Parses an SVG file and returns a list of paths.
        Each path is a list of (x, y) coordinates representing lines/polylines.
        """
        try:
            tree = ET.parse(filepath)
            root = tree.getroot()
        except Exception as e:
            print(f"Error parsing SVG XML: {e}")
            return [], 0, 0, 100, 100 # empty result, bounds placeholders

        # Strip namespace prefixes if present to make querying easier
        for elem in root.iter():
            if '}' in elem.tag:
                elem.tag = elem.tag.split('}', 1)[1]

        # Get viewBox or width/height attributes
        viewbox_str = root.attrib.get('viewBox', '')
        width_str = root.attrib.get('width', '')
        height_str = root.attrib.get('height', '')

        # Parse width/height to get scale and aspect ratio
        def parse_length(val, default=100.0):
            if not val:
                return default
            # match numbers, ignore units (px, mm, pt, etc. for now)
            m = re.match(r'^\s*([0-9.-]+)', val)
            if m:
                return float(m.group(1))
            return default

        width = parse_length(width_str, 0)
        height = parse_length(height_str, 0)

        # Default viewport bounds if not specified
        min_x, min_y = 0.0, 0.0
        vb_w, vb_h = width, height

        if viewbox_str:
            vb_parts = re.split(r'[\s,]+', viewbox_str.strip())
            if len(vb_parts) == 4:
                try:
                    min_x, min_y, vb_w, vb_h = map(float, vb_parts)
                    if width == 0:
                        width = vb_w
                    if height == 0:
                        height = vb_h
                except ValueError:
                    pass

        if width == 0: width = 500
        if height == 0: height = 500
        if vb_w == 0: vb_w = width
        if vb_h == 0: vb_h = height

        # Traverse elements and extract paths
        paths = []
        # Current transformation matrix is identity
        # Matrix is [a, c, e]
        #           [b, d, f]
        # represented as a list [a, b, c, d, e, f]
        identity = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0]
        
        self._parse_element(root, identity, paths)
        
        # We also need to normalize all paths relative to the viewBox
        # and translate such that min_x, min_y is at 0,0.
        normalized_paths = []
        for path in paths:
            norm_path = []
            for x, y in path:
                # Map viewBox space to output scale
                # scale to 0-vb_w and 0-vb_h then to 0-width and 0-height
                nx = (x - min_x) * (width / vb_w)
                ny = (y - min_y) * (height / vb_h)
                norm_path.append((nx, ny))
            if norm_path:
                normalized_paths.append(norm_path)

        return normalized_paths, width, height

    def _parse_element(self, elem, parent_matrix, paths):
        """Recursively parses SVG elements and computes cumulative transforms."""
        # Check for transforms on the current element
        local_matrix = self._parse_transform(elem.attrib.get('transform', ''))
        current_matrix = self._multiply_matrices(parent_matrix, local_matrix)

        tag = elem.tag
        if tag == 'path':
            d = elem.attrib.get('d', '')
            if d:
                raw_paths = self._parse_path_d(d)
                for rp in raw_paths:
                    transformed = [self._apply_matrix(pt[0], pt[1], current_matrix) for pt in rp]
                    if len(transformed) > 1:
                        paths.append(transformed)
        elif tag == 'rect':
            try:
                x = float(elem.attrib.get('x', 0))
                y = float(elem.attrib.get('y', 0))
                w = float(elem.attrib.get('width', 0))
                h = float(elem.attrib.get('height', 0))
                # Create a rectangle path
                rect_pts = [(x, y), (x + w, y), (x + w, y + h), (x, y + h), (x, y)]
                transformed = [self._apply_matrix(pt[0], pt[1], current_matrix) for pt in rect_pts]
                paths.append(transformed)
            except ValueError:
                pass
        elif tag in ('line', 'polyline', 'polygon'):
            pts = []
            if tag == 'line':
                try:
                    x1 = float(elem.attrib.get('x1', 0))
                    y1 = float(elem.attrib.get('y1', 0))
                    x2 = float(elem.attrib.get('x2', 0))
                    y2 = float(elem.attrib.get('y2', 0))
                    pts = [(x1, y1), (x2, y2)]
                except ValueError:
                    pass
            else:
                pts_str = elem.attrib.get('points', '')
                # extract numbers
                num_list = list(map(float, re.findall(r'[-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?', pts_str)))
                pts = [(num_list[i], num_list[i+1]) for i in range(0, len(num_list)-1, 2)]
                if tag == 'polygon' and pts:
                    pts.append(pts[0])  # Close polygon
            
            if pts:
                transformed = [self._apply_matrix(pt[0], pt[1], current_matrix) for pt in pts]
                paths.append(transformed)
        elif tag in ('circle', 'ellipse'):
            try:
                cx = float(elem.attrib.get('cx', 0))
                cy = float(elem.attrib.get('cy', 0))
                if tag == 'circle':
                    rx = ry = float(elem.attrib.get('r', 0))
                else:
                    rx = float(elem.attrib.get('rx', 0))
                    ry = float(elem.attrib.get('ry', 0))
                
                # Approximate ellipse with 36 points
                pts = []
                steps = 36
                for i in range(steps + 1):
                    theta = 2.0 * math.pi * i / steps
                    pts.append((cx + rx * math.cos(theta), cy + ry * math.sin(theta)))
                transformed = [self._apply_matrix(pt[0], pt[1], current_matrix) for pt in pts]
                paths.append(transformed)
            except ValueError:
                pass

        # Recurse for children
        for child in elem:
            self._parse_element(child, current_matrix, paths)

    def _parse_transform(self, transform_str):
        """Parses transform attribute and returns a 2D affine matrix [a, b, c, d, e, f]."""
        if not transform_str:
            return [1.0, 0.0, 0.0, 1.0, 0.0, 0.0]

        # Identity
        matrix = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0]
        
        # Regex to find command(arguments)
        transform_re = re.compile(r'(\w+)\s*\(([^)]+)\)')
        for match in transform_re.finditer(transform_str):
            cmd = match.group(1).lower()
            args = list(map(float, re.split(r'[\s,]+', match.group(2).strip())))
            
            local = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0]
            
            if cmd == 'translate':
                tx = args[0]
                ty = args[1] if len(args) > 1 else 0.0
                local = [1.0, 0.0, 0.0, 1.0, tx, ty]
            elif cmd == 'scale':
                sx = args[0]
                sy = args[1] if len(args) > 1 else sx
                local = [sx, 0.0, 0.0, sy, 0.0, 0.0]
            elif cmd == 'rotate':
                angle = math.radians(args[0])
                cos_a = math.cos(angle)
                sin_a = math.sin(angle)
                if len(args) == 3: # rotate around cx, cy
                    cx, cy = args[1], args[2]
                    # T(cx,cy) * R(angle) * T(-cx,-cy)
                    m1 = [1.0, 0.0, 0.0, 1.0, cx, cy]
                    m2 = [cos_a, sin_a, -sin_a, cos_a, 0.0, 0.0]
                    m3 = [1.0, 0.0, 0.0, 1.0, -cx, -cy]
                    local = self._multiply_matrices(m1, self._multiply_matrices(m2, m3))
                else:
                    local = [cos_a, sin_a, -sin_a, cos_a, 0.0, 0.0]
            elif cmd == 'skewx':
                tan_a = math.tan(math.radians(args[0]))
                local = [1.0, 0.0, tan_a, 1.0, 0.0, 0.0]
            elif cmd == 'skewy':
                tan_a = math.tan(math.radians(args[0]))
                local = [1.0, tan_a, 0.0, 1.0, 0.0, 0.0]
            elif cmd == 'matrix' and len(args) == 6:
                local = args
            
            matrix = self._multiply_matrices(matrix, local)
            
        return matrix

    def _multiply_matrices(self, m1, m2):
        """Multiplies two 2D transform matrices: m1 * m2."""
        # m1 = [a1, b1, c1, d1, e1, f1]
        # m2 = [a2, b2, c2, d2, e2, f2]
        # Represented as:
        # [a1 c1 e1]   [a2 c2 e2]
        # [b1 d1 f1] * [b2 d2 f2]
        # [ 0  0  1]   [ 0  0  1]
        a1, b1, c1, d1, e1, f1 = m1
        a2, b2, c2, d2, e2, f2 = m2
        
        a = a1 * a2 + c1 * b2
        b = b1 * a2 + d1 * b2
        c = a1 * c2 + c1 * d2
        d = b1 * c2 + d1 * d2
        e = a1 * e2 + c1 * f2 + e1
        f = b1 * e2 + d1 * f2 + f1
        
        return [a, b, c, d, e, f]

    def _apply_matrix(self, x, y, m):
        """Applies transform matrix m to point (x, y)."""
        a, b, c, d, e, f = m
        nx = a * x + c * y + e
        ny = b * x + d * y + f
        return nx, ny

    def _parse_path_d(self, d):
        """
        Parses SVG path 'd' string into a list of list of (x,y) points.
        Handles relative/absolute commands and approximates beziers.
        """
        # Split d into commands and numerical arguments
        tokens = re.findall(r'([A-Za-z])|(-?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?)', d)
        # Parse tokens into clear command sequences
        commands = []
        for cmd, val in tokens:
            if cmd:
                commands.append((cmd, []))
            elif val:
                if not commands:
                    # Invalid, numbers before first command. Default to 'M'
                    commands.append(('M', []))
                commands[-1][1].append(float(val))

        paths = []
        curr_path = []
        
        curr_x, curr_y = 0.0, 0.0
        start_x, start_y = 0.0, 0.0 # start of subpath for 'Z'
        
        # Last control point for smooth curves S/s, T/t
        last_cx, last_cy = 0.0, 0.0
        last_cmd = ''

        for cmd, args in commands:
            # We process arguments in groups based on command type
            cmd_type = cmd.upper()
            is_relative = cmd.islower()
            
            i = 0
            while i < len(args) or (cmd_type == 'Z' and i == 0):
                if cmd_type == 'M': # Move to (2 parameters)
                    # If we have points in curr_path, save it
                    if len(curr_path) > 1:
                        paths.append(curr_path)
                    
                    dx = args[i]
                    dy = args[i+1]
                    i += 2
                    
                    if is_relative:
                        curr_x += dx
                        curr_y += dy
                    else:
                        curr_x = dx
                        curr_y = dy
                        
                    curr_path = [(curr_x, curr_y)]
                    start_x, start_y = curr_x, curr_y
                    last_cx, last_cy = curr_x, curr_y
                    # Note: subsequent pairs in M are treated as L
                    cmd_type = 'L'
                    
                elif cmd_type == 'L': # Line to (2 parameters)
                    dx = args[i]
                    dy = args[i+1]
                    i += 2
                    
                    if is_relative:
                        curr_x += dx
                        curr_y += dy
                    else:
                        curr_x = dx
                        curr_y = dy
                    curr_path.append((curr_x, curr_y))
                    last_cx, last_cy = curr_x, curr_y
                    
                elif cmd_type == 'H': # Horizontal line (1 parameter)
                    val = args[i]
                    i += 1
                    if is_relative:
                        curr_x += val
                    else:
                        curr_x = val
                    curr_path.append((curr_x, curr_y))
                    last_cx, last_cy = curr_x, curr_y
                    
                elif cmd_type == 'V': # Vertical line (1 parameter)
                    val = args[i]
                    i += 1
                    if is_relative:
                        curr_y += val
                    else:
                        curr_y = val
                    curr_path.append((curr_x, curr_y))
                    last_cx, last_cy = curr_x, curr_y
                    
                elif cmd_type == 'C': # Cubic Bezier (6 parameters: x1,y1, x2,y2, x,y)
                    x1, y1 = args[i], args[i+1]
                    x2, y2 = args[i+2], args[i+3]
                    x, y = args[i+4], args[i+5]
                    i += 6
                    
                    if is_relative:
                        x1 += curr_x; y1 += curr_y
                        x2 += curr_x; y2 += curr_y
                        x += curr_x; y += curr_y
                    
                    # Interpolate cubic bezier
                    for step in range(1, self.bezier_steps + 1):
                        t = step / self.bezier_steps
                        mt = 1 - t
                        # Bezier equation
                        px = mt**3 * curr_x + 3 * mt**2 * t * x1 + 3 * mt * t**2 * x2 + t**3 * x
                        py = mt**3 * curr_y + 3 * mt**2 * t * y1 + 3 * mt * t**2 * y2 + t**3 * y
                        curr_path.append((px, py))
                    
                    curr_x, curr_y = x, y
                    last_cx, last_cy = x2, y2
                    
                elif cmd_type == 'S': # Smooth Cubic Bezier (4 parameters: x2,y2, x,y)
                    # Control point 1 is reflection of last control point about current point
                    if last_cmd in ('C', 'S'):
                        x1 = 2 * curr_x - last_cx
                        y1 = 2 * curr_y - last_cy
                    else:
                        x1, y1 = curr_x, curr_y
                        
                    x2, y2 = args[i], args[i+1]
                    x, y = args[i+2], args[i+3]
                    i += 4
                    
                    if is_relative:
                        x2 += curr_x; y2 += curr_y
                        x += curr_x; y += curr_y
                        
                    # Interpolate cubic bezier
                    for step in range(1, self.bezier_steps + 1):
                        t = step / self.bezier_steps
                        mt = 1 - t
                        px = mt**3 * curr_x + 3 * mt**2 * t * x1 + 3 * mt * t**2 * x2 + t**3 * x
                        py = mt**3 * curr_y + 3 * mt**2 * t * y1 + 3 * mt * t**2 * y2 + t**3 * y
                        curr_path.append((px, py))
                        
                    curr_x, curr_y = x, y
                    last_cx, last_cy = x2, y2
                    
                elif cmd_type == 'Q': # Quadratic Bezier (4 parameters: x1,y1, x,y)
                    x1, y1 = args[i], args[i+1]
                    x, y = args[i+2], args[i+3]
                    i += 4
                    
                    if is_relative:
                        x1 += curr_x; y1 += curr_y
                        x += curr_x; y += curr_y
                        
                    # Interpolate quadratic bezier
                    for step in range(1, self.bezier_steps + 1):
                        t = step / self.bezier_steps
                        mt = 1 - t
                        px = mt**2 * curr_x + 2 * mt * t * x1 + t**2 * x
                        py = mt**2 * curr_y + 2 * mt * t * y1 + t**2 * y
                        curr_path.append((px, py))
                        
                    curr_x, curr_y = x, y
                    last_cx, last_cy = x1, y1
                    
                elif cmd_type == 'T': # Smooth Quadratic Bezier (2 parameters: x,y)
                    # Control point is reflection of last control point about current point
                    if last_cmd in ('Q', 'T'):
                        x1 = 2 * curr_x - last_cx
                        y1 = 2 * curr_y - last_cy
                    else:
                        x1, y1 = curr_x, curr_y
                        
                    x, y = args[i], args[i+1]
                    i += 2
                    
                    if is_relative:
                        x += curr_x; y += curr_y
                        
                    # Interpolate quadratic bezier
                    for step in range(1, self.bezier_steps + 1):
                        t = step / self.bezier_steps
                        mt = 1 - t
                        px = mt**2 * curr_x + 2 * mt * t * x1 + t**2 * x
                        py = mt**2 * curr_y + 2 * mt * t * y1 + t**2 * y
                        curr_path.append((px, py))
                        
                    curr_x, curr_y = x, y
                    last_cx, last_cy = x1, y1
                    
                elif cmd_type == 'Z': # Close Path
                    if curr_path and (curr_x != start_x or curr_y != start_y):
                        curr_path.append((start_x, start_y))
                    curr_x, curr_y = start_x, start_y
                    last_cx, last_cy = start_x, start_y
                    i += 1 # advance to break
                    break
                else:
                    # Ignore other less common commands like A/a (elliptical arc) for now to keep code clean,
                    # or skip arguments to prevent infinite loops.
                    break
            
            last_cmd = cmd_type
            
        if len(curr_path) > 1:
            paths.append(curr_path)
            
        return paths
