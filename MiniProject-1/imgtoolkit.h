#pragma once
// ---- Memory ----
unsigned char** allocate2D(int height, int width);
void free2D(unsigned char** arr, int height);

// ---- PGM ----
unsigned char** loadPGM(const char* filename, int* width, int* height, int* maxval);
int writePGM(const char* filename, unsigned char** img, int w, int h, int maxval);

// ---- Filters ----
void averageFilter(unsigned char** input, unsigned char** output, int w, int h);
void meanFilter(unsigned char** input, unsigned char** output, int w, int h);
void medianFilter(unsigned char** input, unsigned char** output, int w, int h);

// ---- Edges ----
void sobelFilter(unsigned char** input, unsigned char** output, int w, int h);
void canny(unsigned char** input, unsigned char** edges, int w, int h, int low, int high);
void prewittFilter(unsigned char** input, unsigned char** output, int w, int h);

// ---- LBP ----
void computeLBP(unsigned char** input, unsigned char** output, int w, int h);

// ---- Zoom / Shrink ----
unsigned char** zoomNearest(unsigned char** input, int w, int h, float factor, int* newW, int* newH);
unsigned char** shrinkNearest(unsigned char** input, int w, int h, float factor, int* newW, int* newH);



