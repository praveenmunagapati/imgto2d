"""DrawingBotV3 Python Edition — Entry Point.

Launches the PyQt6-based main application window.
"""

import sys
import os

# Ensure the project root is on the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from PyQt6.QtWidgets import QApplication
from PyQt6.QtGui import QFont, QIcon
from PyQt6.QtCore import Qt


def main():
    # High DPI support
    os.environ.setdefault("QT_ENABLE_HIGHDPI_SCALING", "1")

    app = QApplication(sys.argv)

    # Set global font
    font = QFont("Segoe UI", 10)
    app.setFont(font)

    # Set app metadata
    app.setApplicationName("DrawingBot V3")
    app.setOrganizationName("Antigravity")
    app.setApplicationVersion("1.6.10")

    # Dark mode palette override (ensures system widgets match theme)
    from PyQt6.QtGui import QPalette, QColor
    palette = QPalette()
    palette.setColor(QPalette.ColorRole.Window, QColor(28, 27, 31))
    palette.setColor(QPalette.ColorRole.WindowText, QColor(230, 225, 229))
    palette.setColor(QPalette.ColorRole.Base, QColor(37, 35, 42))
    palette.setColor(QPalette.ColorRole.AlternateBase, QColor(45, 43, 51))
    palette.setColor(QPalette.ColorRole.Text, QColor(230, 225, 229))
    palette.setColor(QPalette.ColorRole.Button, QColor(37, 35, 42))
    palette.setColor(QPalette.ColorRole.ButtonText, QColor(230, 225, 229))
    palette.setColor(QPalette.ColorRole.Highlight, QColor(79, 55, 139))
    palette.setColor(QPalette.ColorRole.HighlightedText, QColor(255, 255, 255))
    palette.setColor(QPalette.ColorRole.PlaceholderText, QColor(150, 150, 160))
    app.setPalette(palette)

    # Create and show main window
    from app.ui.main_window import MainWindow
    window = MainWindow()
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
