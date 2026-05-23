"""Mask loading and application (white = draw, black = skip)."""

from __future__ import annotations

from typing import Optional

import cv2
import numpy as np


def apply_mask_to_image(image: np.ndarray, mask: Optional[np.ndarray]) -> np.ndarray:
    """Multiply image by mask; masked-out areas become white (255)."""
    if mask is None:
        return image
    h, w = image.shape[:2]
    m = mask
    if m.shape[:2] != (h, w):
        m = cv2.resize(m, (w, h), interpolation=cv2.INTER_LINEAR)
    if len(m.shape) == 3:
        m = cv2.cvtColor(m, cv2.COLOR_BGR2GRAY)
    m = m.astype(np.float32) / 255.0
    out = image.astype(np.float32)
    out = out * m + 255.0 * (1.0 - m)
    return np.clip(out, 0, 255).astype(np.uint8)


class MaskManager:
    def __init__(self):
        self.mask: Optional[np.ndarray] = None
        self.mask_path: str = ""
        self.enabled: bool = False

    def load(self, path: str) -> bool:
        m = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
        if m is None:
            return False
        self.mask = m
        self.mask_path = path
        self.enabled = True
        return True

    def clear(self):
        self.mask = None
        self.mask_path = ""
        self.enabled = False

    def to_dict(self) -> dict:
        return {"mask_path": self.mask_path, "enabled": self.enabled}

    def from_dict(self, d: dict):
        self.enabled = d.get("enabled", False)
        path = d.get("mask_path", "")
        if path:
            self.load(path)
