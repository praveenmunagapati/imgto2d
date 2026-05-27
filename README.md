# imgto2d

imgto2d is a tool for converting images to 2D vector graphics like SVG, GCode, and HPGL. It features a standalone command-line interface for processing single images or batch directories based on project configurations.

## Prerequisites

- CMake (version 3.20 or higher)
- A C++17 compatible compiler
- OpenCV (core, imgcodecs, imgproc, photo)
- OpenMP
- *Optional*: [vpype](https://github.com/abey79/vpype) for SVG optimization

## Building the Program

You can build the project using CMake. Open your terminal or command prompt and run the following commands from the `cli` directory:

```bash
cd cli
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The compiled executable `imgto2d_standalone_cli` will be located in the `build` directory (or `build/Release` depending on your generator).

## Usage

The CLI operates primarily on project configuration files (`.dbv3`).

### Single Image Mode

Process an image using the settings defined in your project file. You can override the image path stored in the project by providing `[image_override]`.

```bash
imgto2d_standalone_cli <project.dbv3> <output_dir_or_file> [image_override] [--vpype]
```

- `<project.dbv3>`: Path to your project configuration database.
- `<output_dir_or_file>`: The output destination. The file extension determines the output format:
  - `.svg`: Exports as Scalable Vector Graphics.
  - `.gcode`: Exports as GCode for CNC/plotters.
  - `.hpgl`: Exports as HPGL commands.
- `[image_override]`: (Optional) Provide a path to a specific image to process instead of the one saved in the project.
- `--vpype`: (Optional) Add this flag at the end to optimize the output SVG using `vpype` (requires `vpype` to be installed and in your system PATH).

**Example:**
```bash
imgto2d_standalone_cli my_settings.dbv3 output.svg input.jpg
```

### Batch Mode

Process an entire directory of images (`.jpg`, `.jpeg`, `.png`) using the same project settings.

```bash
imgto2d_standalone_cli <project.dbv3> <output_dir/> --batch <input_dir/>
```

- `<project.dbv3>`: Path to your project configuration database.
- `<output_dir/>`: The directory where the generated files will be saved.
- `--batch`: Flag to enable batch processing.
- `<input_dir/>`: The directory containing the source images.

**Example:**
```bash
imgto2d_standalone_cli my_settings.dbv3 ./output_folder --batch ./input_folder
```

## Output Formats
- **SVG**: Default vector format.
- **GCode**: Used for plotters and CNC machines. Output is triggered by using a `.gcode` extension for the output file.
- **HPGL**: Used for vintage pen plotters. Output is triggered by using an `.hpgl` extension for the output file.
