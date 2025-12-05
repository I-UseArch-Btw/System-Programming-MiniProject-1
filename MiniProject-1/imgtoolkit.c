#define _CRT_SECURE_NO_WARNINGS

#include "imgtoolkit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

/*

 __  __                                 
|  \/  | ___ _ __ ___   ___  _ __ _   _ 
| |\/| |/ _ \ '_ ` _ \ / _ \| '__| | | |
| |  | |  __/ | | | | | (_) | |  | |_| |
|_|  |_|\___|_| |_| |_|\___/|_|   \__, |
                                  |___/ 


*/
unsigned char** allocate2D(int height, int width) {
    unsigned char** arr = malloc(height * sizeof(unsigned char*));
    for (int i = 0; i < height; i++)
        arr[i] = malloc(width * sizeof(unsigned char));
    return arr;
}

void free2D(unsigned char** arr, int height) {
    for (int i = 0; i < height; i++)
        free(arr[i]);
    free(arr);
}

/*

 ____   ____ __  __   _                    _    ____        __    _ _       
|  _ \ / ___|  \/  | | |    ___   __ _  __| |  / /\ \      / / __(_) |_ ___ 
| |_) | |  _| |\/| | | |   / _ \ / _` |/ _` | / /  \ \ /\ / / '__| | __/ _ \
|  __/| |_| | |  | | | |__| (_) | (_| | (_| |/ /    \ V  V /| |  | | ||  __/
|_|    \____|_|  |_| |_____\___/ \__,_|\__,_/_/      \_/\_/ |_|  |_|\__\___|


*/
// Helper to skip comments and whitespace
void skipComments(FILE* f) {
    int c = fgetc(f);
    while (c != EOF) {
        if (c == '#') {            // comment line
            while ((c = fgetc(f)) != '\n' && c != EOF);
        }
        else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { // whitespace
            c = fgetc(f);
        }
        else {
            ungetc(c, f);          // a number starts here
            break;
        }
    }
}

unsigned char** loadPGM(const char* filename, int* width, int* height, int* maxval) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    char magic[3];
    if (fscanf(f, "%2s", magic) != 1) { fclose(f); return NULL; }

    skipComments(f);
    if (fscanf(f, "%d", width) != 1) { fclose(f); return NULL; }

    skipComments(f);
    if (fscanf(f, "%d", height) != 1) { fclose(f); return NULL; }

    skipComments(f);
    if (fscanf(f, "%d", maxval) != 1) { fclose(f); return NULL; }

    fgetc(f); // skip one whitespace/newline after maxval

    unsigned char** img = allocate2D(*height, *width);

    if (strcmp(magic, "P5") == 0) { // binary
        for (int i = 0; i < *height; i++)
            fread(img[i], sizeof(unsigned char), *width, f);
    }
    else if (strcmp(magic, "P2") == 0) { // ASCII
        for (int i = 0; i < *height; i++)
            for (int j = 0; j < *width; j++) {
                skipComments(f);
                int val;
                if (fscanf(f, "%d", &val) != 1) { free2D(img, *height); fclose(f); return NULL; }
                img[i][j] = (unsigned char)val;
            }
    }
    else {
        free2D(img, *height);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return img;
}



int writePGM(const char* filename, unsigned char** img, int w, int h, int maxval) {
    FILE* f = fopen(filename, "w"); // text mode for ASCII
    if (!f) return 0;

    // ASCII PGM header
    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "%d\n", maxval);

    // Write pixel values
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            fprintf(f, "%d ", img[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}


/*

 _____ _ _ _                
|  ___(_) | |_ ___ _ __ ___ 
| |_  | | | __/ _ \ '__/ __|
|  _| | | | ||  __/ |  \__ \
|_|   |_|_|\__\___|_|  |___/


*/
void averageFilter(unsigned char** input, unsigned char** output, int w, int h) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int sum = 0, count = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++) {
                    int ny = y + dy, nx = x + dx;
                    if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                        sum += input[ny][nx];
                        count++;
                    }
                }
            output[y][x] = sum / count;
        }
}

void meanFilter(unsigned char** input, unsigned char** output, int w, int h) {
    averageFilter(input, output, w, h); // identical to average for simplicity
}

void medianFilter(unsigned char** input, unsigned char** output, int w, int h) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            unsigned char vals[9]; int count = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++) {
                    int ny = y + dy, nx = x + dx;
                    if (nx >= 0 && nx < w && ny >= 0 && ny < h)
                        vals[count++] = input[ny][nx];
                }
            // simple bubble sort
            for (int i = 0; i < count - 1; i++)
                for (int j = i + 1; j < count; j++)
                    if (vals[i] > vals[j]) { unsigned char t = vals[i]; vals[i] = vals[j]; vals[j] = t; }
            output[y][x] = vals[count / 2];
        }
}

/*

 _____    _                 
| ____|__| | __ _  ___  ___ 
|  _| / _` |/ _` |/ _ \/ __|
| |__| (_| | (_| |  __/\__ \
|_____\__,_|\__, |\___||___/
            |___/           


*/
void sobelFilter(unsigned char** input, unsigned char** output, int w, int h) {
    
    //Sobel Kernels
    int Gx[3][3] = { 
        {-1,0,1},
        {-2,0,2},
        {-1,0,1}
    };
    int Gy[3][3] = { 
        {1,2,1},
        {0,0,0},
        {-1,-2,-1} 
    };

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int sumX = 0, sumY = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++) {
                    int ny = y + dy, nx = x + dx;
                    unsigned char val = (nx >= 0 && nx < w && ny >= 0 && ny < h) ? input[ny][nx] : 0;
                    sumX += val * Gx[dy + 1][dx + 1];
                    sumY += val * Gy[dy + 1][dx + 1];
                }
            int mag = (int)sqrt(sumX * sumX + sumY * sumY);
            if (mag > 255) mag = 255;
            output[y][x] = (unsigned char)mag;
        }
}

void prewittFilter(unsigned char** input, unsigned char** output, int w, int h) {
    
    //Prewitt Kernels
    int Gx[3][3] = {
    {-1, 0, 1},
    {-1, 0, 1},
    {-1, 0, 1}
    };

    int Gy[3][3] = {
        { 1,  1,  1},
        { 0,  0,  0},
        {-1, -1, -1}
    };

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sumX = 0, sumY = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;
                    unsigned char val = 0;
                    if (nx >= 0 && nx < w && ny >= 0 && ny < h)
                        val = input[ny][nx];

                    sumX += val * Gx[dy + 1][dx + 1];
                    sumY += val * Gy[dy + 1][dx + 1];
                }
            }

            int mag = (int)sqrt(sumX * sumX + sumY * sumY);
            if (mag > 255) mag = 255;
            output[y][x] = (unsigned char)mag;
        }
    }
}


/*-----------------------------
  ____                        
 / ___|__ _ _ __  _ __  _   _ 
| |   / _` | '_ \| '_ \| | | |
| |__| (_| | | | | | | | |_| |
 \____\__,_|_| |_|_| |_|\__, |
                        |___/ 
------------------------------*/

// Gaussian filter (3x3 kernel)
void gaussianBlur(unsigned char** input, unsigned char** output, int w, int h) {
    int kernel[3][3] = { {1,2,1},{2,4,2},{1,2,1} };
    int sumKernel = 16;
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += input[y + ky][x + kx] * kernel[ky + 1][kx + 1];
                }
            }
            output[y][x] = sum / sumKernel;
        }
    }
}
// Non-maximum suppression
void nonMaxSuppression(double** grad, double** direction, unsigned char** output, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            double angle = direction[y][x];
            double g = grad[y][x];
            double g1 = 0, g2 = 0;

            // Quantize the angle
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                g1 = grad[y][x - 1]; g2 = grad[y][x + 1];
            }
            else if (angle >= 22.5 && angle < 67.5) {
                g1 = grad[y - 1][x + 1]; g2 = grad[y + 1][x - 1];
            }
            else if (angle >= 67.5 && angle < 112.5) {
                g1 = grad[y - 1][x]; g2 = grad[y + 1][x];
            }
            else if (angle >= 112.5 && angle < 157.5) {
                g1 = grad[y - 1][x - 1]; g2 = grad[y + 1][x + 1];
            }

            if (g >= g1 && g >= g2)
                output[y][x] = (unsigned char)g;
            else
                output[y][x] = 0;
        }
    }
}

// Double threshold
void doubleThreshold(unsigned char** input, unsigned char** output, int w, int h, int low, int high) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (input[y][x] >= high)
                output[y][x] = 255;
            else if (input[y][x] >= low)
                output[y][x] = 128; // weak edge
            else
                output[y][x] = 0;
        }
    }
}

// Edge tracking by hysteresis
void hysteresis(unsigned char** input, unsigned char** output, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            if (input[y][x] == 128) {
                if (input[y - 1][x - 1] == 255 || input[y - 1][x] == 255 || input[y - 1][x + 1] == 255 ||
                    input[y][x - 1] == 255 || input[y][x + 1] == 255 ||
                    input[y + 1][x - 1] == 255 || input[y + 1][x] == 255 || input[y + 1][x + 1] == 255)
                    output[y][x] = 255;
                else
                    output[y][x] = 0;
            }
            else if (input[y][x] == 255)
                output[y][x] = 255;
            else
                output[y][x] = 0;
        }
    }
}
// Compute gradient using Sobel
void sobelGradient(unsigned char** input, double** grad, double** direction, int w, int h) {
    
    int gx, gy;
    //Sobel Kernels
    int sobelX[3][3] = { 
        {-1,0,1},
        {-2,0,2},
        {-1,0,1} };
    
    int sobelY[3][3] = { 
        {1,2,1},
        {0,0,0},
        {-1,-2,-1} };

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            gx = 0; gy = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    gx += input[y + ky][x + kx] * sobelX[ky + 1][kx + 1];
                    gy += input[y + ky][x + kx] * sobelY[ky + 1][kx + 1];
                }
            }
            grad[y][x] = sqrt(gx * gx + gy * gy);
            direction[y][x] = atan2(gy, gx) * 180 / PI;
            if (direction[y][x] < 0) direction[y][x] += 180;
        }
    }
}


// Full Canny function
void canny(unsigned char** input, unsigned char** edges, int w, int h, int low, int high) {
    unsigned char** blurred = allocate2D(h, w);
    gaussianBlur(input, blurred, w, h);

    double** grad = (double**)malloc(h * sizeof(double*));
    double** dir = (double**)malloc(h * sizeof(double*));
    for (int i = 0;i < h;i++) {
        grad[i] = (double*)malloc(w * sizeof(double));
        dir[i] = (double*)malloc(w * sizeof(double));
    }

    sobelGradient(blurred, grad, dir, w, h);

    unsigned char** nms = allocate2D(h, w);
    nonMaxSuppression(grad, dir, nms, w, h);

    unsigned char** dt = allocate2D(h, w);
    doubleThreshold(nms, dt, w, h, low, high);

    hysteresis(dt, edges, w, h);

    free2D(blurred, h);
    free2D(nms, h);
    free2D(dt, h);
    for (int i = 0;i < h;i++) { free(grad[i]); free(dir[i]); }
    free(grad); free(dir);
}


//--------------------------------------

/*

 _     ____  ____  
| |   | __ )|  _ \ 
| |   |  _ \| |_) |
| |___| |_) |  __/ 
|_____|____/|_|    


*/
void computeLBP(unsigned char** input, unsigned char** output, int w, int h) {
    for (int y = 1; y < h - 1; y++)
        for (int x = 1; x < w - 1; x++) {
            unsigned char center = input[y][x];
            unsigned char code = 0;
            code |= (input[y - 1][x - 1] >= center) << 7;
            code |= (input[y - 1][x] >= center) << 6;
            code |= (input[y - 1][x + 1] >= center) << 5;
            code |= (input[y][x + 1] >= center) << 4;
            code |= (input[y + 1][x + 1] >= center) << 3;
            code |= (input[y + 1][x] >= center) << 2;
            code |= (input[y + 1][x - 1] >= center) << 1;
            code |= (input[y][x - 1] >= center) << 0;
            output[y][x] = code;
        }
}

/*

 _____                        ______  _          _       _    
|__  /___   ___  _ __ ___    / / ___|| |__  _ __(_)_ __ | | __
  / // _ \ / _ \| '_ ` _ \  / /\___ \| '_ \| '__| | '_ \| |/ /
 / /| (_) | (_) | | | | | |/ /  ___) | | | | |  | | | | |   < 
/____\___/ \___/|_| |_| |_/_/  |____/|_| |_|_|  |_|_| |_|_|\_\

*/
unsigned char** zoomNearest(unsigned char** input, int w, int h, float factor, int* newW, int* newH) {
    *newW = w; // keep resolution
    *newH = h;

    unsigned char** out = allocate2D(*newH, *newW);
    int cx = w / 2;
    int cy = h / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int srcX = (int)((x - cx) / factor + cx);
            int srcY = (int)((y - cy) / factor + cy);

            // Clamp coordinates
            if (srcX < 0) srcX = 0;
            if (srcX >= w) srcX = w - 1;
            if (srcY < 0) srcY = 0;
            if (srcY >= h) srcY = h - 1;

            out[y][x] = input[srcY][srcX];
        }
    }
    return out;
}

unsigned char** shrinkNearest(unsigned char** input, int w, int h, float factor, int* newW, int* newH) {
    // Shrink works the same way, just factor < 1
    return zoomNearest(input, w, h, factor, newW, newH);
}

