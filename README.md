# PGM Image Toolkit

A simple C program to **load, process, and save PGM images** with basic image processing features such as zoom, shrink, filters, edge detection, and Local Binary Patterns (LBP).

---

## Features

- **Load PGM images** (ASCII `P2` and Binary `P5`) with support for comments.
- **Save PGM images** (ASCII format only).
- **Zoom & Shrink** images using nearest-neighbor interpolation (camera like zoom).
- **Apply filters**:
  - Average filter  
  - Mean filter  
  - Median filter
- **Edge detection**:
  - Sobel filter  
  - Canny filter
  - Prewitt Filter
- **Compute LBP codes** for each pixel.

---

## Requirements

- C compiler (GCC, MSVC, etc.)
- Cross-platform: works on **Windows, Linux, and Mac** via console.

---

## How to Run

1. **Compile the project**:

```bash
gcc main.c imgtoolkit.c -o pgmtool
```

2. **Run the Program**:

```bash
./pgmtool
```

3. **Use the menu to**:

- Load a PGM image 

- Apply zoom/shrink operations

- Apply filters or edge detection

- Compute LBP

- Save the final result (automatically saved as output.pgm)

## File Structure

- main.c – Main program with menu interface

- imgtoolkit.h – Header file with function declarations

- imgtoolkit.c – Implementation of image operations: PGM loading, filters, edges, zoom/shrink, LBP

## Notes

- Supports 8-bit grayscale images (maxval ≤ 255).

- Handles comments (# ...) in PGM files.

- Dynamically allocates memory to support large images.

