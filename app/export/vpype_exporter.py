"""VPype integration for optimized SVG export."""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
from typing import Optional

from app.export.svg_exporter import export_svg


def export_svg_with_vpype(
    filepath: str,
    geometries,
    drawing_area,
    active_pens,
    path_opt: dict,
    vpype_pipeline: str = "linemerge linesimplify",
) -> None:
    """Export SVG and optionally optimize it with vpype."""
    # Create the base SVG first
    temp_svg = filepath + ".tmp.svg"
    export_svg(temp_svg, geometries, drawing_area, active_pens, path_opt)

    pipeline_tokens = [tok for tok in vpype_pipeline.strip().split() if tok]
    if not pipeline_tokens:
        os.replace(temp_svg, filepath)
        return

    vpype_cmd = shutil.which("vpype")
    if vpype_cmd is None:
        vpype_cmd = sys.executable
        command = [vpype_cmd, "-m", "vpype", "read", temp_svg, *pipeline_tokens, "write", filepath]
    else:
        command = [vpype_cmd, "read", temp_svg, *pipeline_tokens, "write", filepath]

    try:
        subprocess.run(command, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        raise RuntimeError(
            f"VPype optimization failed: {exc.stderr.strip() or exc.stdout.strip()}"
        ) from exc
    finally:
        if os.path.exists(temp_svg):
            try:
                os.remove(temp_svg)
            except OSError:
                pass
