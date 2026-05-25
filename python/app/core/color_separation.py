"""Color separation algorithms for DrawingBotV3."""

import cv2
import numpy as np
from typing import List

def split_grayscale(image: np.ndarray) -> List[np.ndarray]:
    """Return the image as a single grayscale channel."""
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        return [gray]
    return [image]

def split_rgb(image: np.ndarray) -> List[np.ndarray]:
    """Split an image into Red, Green, and Blue density maps (0-255)."""
    if len(image.shape) != 3:
        return [image, image, image] # Fallback if grayscale
        
    b, g, r = cv2.split(image)
    
    # We want density maps where 0 = no ink, 255 = max ink
    # If the channel is 255, we want 0 ink. If the channel is 0, we want 255 ink.
    # Therefore, we just invert the channels.
    r_density = 255 - r
    g_density = 255 - g
    b_density = 255 - b
    
    return [r_density, g_density, b_density]

def split_cmyk(image: np.ndarray) -> List[np.ndarray]:
    """Split an image into Cyan, Magenta, Yellow, and Key (Black) density maps."""
    if len(image.shape) != 3:
        return [image, image, image, image]
        
    b, g, r = cv2.split(image)
    
    # Convert BGR to CMYK manually
    # CMYK ranges from 0 to 1 mathematically
    r_norm = r.astype(np.float32) / 255.0
    g_norm = g.astype(np.float32) / 255.0
    b_norm = b.astype(np.float32) / 255.0
    
    k = 1.0 - np.maximum(np.maximum(r_norm, g_norm), b_norm)
    
    # Avoid division by zero
    k_mask = k < 1.0
    
    c = np.zeros_like(k)
    m = np.zeros_like(k)
    y = np.zeros_like(k)
    
    c[k_mask] = (1.0 - r_norm[k_mask] - k[k_mask]) / (1.0 - k[k_mask])
    m[k_mask] = (1.0 - g_norm[k_mask] - k[k_mask]) / (1.0 - k[k_mask])
    y[k_mask] = (1.0 - b_norm[k_mask] - k[k_mask]) / (1.0 - k[k_mask])
    
    # The output needs to be 0-255 where 255 is maximum density (darkness)
    c_density = (c * 255).astype(np.uint8)
    m_density = (m * 255).astype(np.uint8)
    y_density = (y * 255).astype(np.uint8)
    k_density = (k * 255).astype(np.uint8)
    
    # Return in CMYK order
    return [c_density, m_density, y_density, k_density]

