#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgtoolkit.h"   // your header file

// ---------------- GLOBAL IMAGE ----------------
unsigned char** currentImage = NULL;
int width = 0, height = 0, maxval = 255;

// ---------------- MENU HELPERS ----------------
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    printf("\nPress Enter to continue...");
    getchar();
}

void getInput(char* input) {
    fgets(input, 256, stdin);
    input[strcspn(input, "\n")] = '\0';  // remove newline
}

// ---------------- FORWARD DECLARATIONS ----------------
void zoomShrinkMenu();
void filtersMenu();
void edgeDetectionMenu();

// ---------------- ZOOM / SHRINK MENU ----------------
void zoomShrinkMenu() {
    char choice[10];
    while (1) {
        clearConsole();
        printf("Zoom/Shrink Menu:\n");
        printf("1. Zoom\n");
        printf("2. Shrink\n");
        printf("0. Back to main menu\n");
        printf("Select: ");
        getInput(choice);

        if (strcmp(choice, "0") == 0) break;

        float factor;
        unsigned char** out;
        int newW, newH;

        if (strcmp(choice, "1") == 0) {
            printf("Enter zoom factor (e.g., 1.5): ");
            scanf("%f", &factor);
            while (getchar() != '\n'); // clear buffer

            
            out = zoomNearest(currentImage, width, height, factor, &newW, &newH);
        }
        else if (strcmp(choice, "2") == 0) {
            printf("Enter shrink factor (e.g., 0.5): ");
            scanf("%f", &factor);
            while (getchar() != '\n');

            out = shrinkNearest(currentImage, width, height, factor, &newW, &newH);
        }
        else {
            printf("Invalid selection!\n");
            pause();
            continue;
        }

        // Replace current image
        free2D(currentImage, height);
        currentImage = out;
        width = newW;
        height = newH;

        printf("Operation complete. New size: %dx%d\n", width, height);
        pause();
    }
}

// ---------------- FILTERS MENU ----------------
void filtersMenu() {
    char choice[10];
    while (1) {
        clearConsole();
        printf("Filters Menu:\n");
        printf("1. Average Filter\n");
        printf("2. Mean Filter\n");
        printf("3. Median Filter\n");
        printf("0. Back to main menu\n");
        printf("Select: ");
        getInput(choice);

        if (strcmp(choice, "0") == 0) break;

        unsigned char** out = allocate2D(height, width);

        if (strcmp(choice, "1") == 0) averageFilter(currentImage, out, width, height);
        else if (strcmp(choice, "2") == 0) meanFilter(currentImage, out, width, height);
        else if (strcmp(choice, "3") == 0) medianFilter(currentImage, out, width, height);
        else {
            printf("Invalid selection!\n");
            free2D(out, height);
            pause();
            continue;
        }

        // Replace current image
        free2D(currentImage, height);
        currentImage = out;

        printf("Filter applied successfully.\n");
        pause();
    }
}

// ---------------- EDGE DETECTION MENU ----------------
void edgeDetectionMenu() {
    char choice[10];
    while (1) {
        clearConsole();
        printf("Edge Detection Menu:\n");
        printf("1. Sobel Edge Filter\n");
        printf("2. Canny Edge Filter\n");
        printf("3. Prewitt Edge Filter\n");
        printf("0. Back to main menu\n");
        printf("Select: ");
        getInput(choice);

        if (strcmp(choice, "0") == 0) break;

        unsigned char** out = allocate2D(height, width);

        if (strcmp(choice, "1") == 0) sobelFilter(currentImage, out, width, height);
        else if (strcmp(choice, "2") == 0) {
            int low, high;
            printf("Enter low threshold: ");
            scanf("%d", &low);
            printf("Enter high threshold: ");
            scanf("%d", &high);
            while (getchar() != '\n'); // clear buffer

            canny(currentImage, out, width, height, low, high);
        }
        else if (strcmp(choice, "3") == 0) prewittFilter(currentImage, out, width, height);
        

        else {
            printf("Invalid selection!\n");
            free2D(out, height);
            pause();
            continue;
        }

        free2D(currentImage, height);
        currentImage = out;

        printf("Edge detection complete.\n");
        pause();
    }
}

// --------------------------- MAIN ---------------------------
int main() {
    char selection[20];

    while (1) {
        clearConsole();

        printf("Please select from the options:\n\n");
        printf("1. Load a PGM image file\n");
        printf("2. Zoom or Shrink\n");
        printf("3. Apply Filters\n");
        printf("4. Edge Detection\n");
        printf("5. Compute and store LBP code for each pixel\n");
        printf("0. Save and exit\n\n");
        printf("Select: ");

        getInput(selection);

        // ---- EXIT & SAVE ----
        if (strcmp(selection, "0") == 0) {
            if (currentImage != NULL) {
                writePGM("output.pgm", currentImage, width, height, maxval);
                free2D(currentImage, height);
            }
            printf("\nSaved as output.pgm\nGoodbye!\n");
            break;
        }

        // ---- LOAD PGM ----
        else if (strcmp(selection, "1") == 0) {
            char path[260];

            printf("Enter PGM file path: ");
            getInput(path);

            // Free previous image
            if (currentImage != NULL) {
                free2D(currentImage, height);
                currentImage = NULL;
            }

            currentImage = loadPGM(path, &width, &height, &maxval);

            if (currentImage == NULL) {
                printf("\nFailed to load PGM image!\n");
            }
            else {
                printf("\nSuccessfully loaded PGM image!\n");
                printf("Width : %d\n", width);
                printf("Height: %d\n", height);
                printf("Maxval: %d\n", maxval);
            }

            pause();
        }

        // ---- ZOOM / SHRINK ----
        else if (strcmp(selection, "2") == 0) {
            if (!currentImage) {
                printf("Load an image first!\n");
                pause();
            }
            else {
                zoomShrinkMenu();
            }
        }

        // ---- FILTERS ----
        else if (strcmp(selection, "3") == 0) {
            if (!currentImage) {
                printf("Load an image first!\n");
                pause();
            }
            else {
                filtersMenu();
            }
        }

        // ---- EDGE DETECTION ----
        else if (strcmp(selection, "4") == 0) {
            if (!currentImage) {
                printf("Load an image first!\n");
                pause();
            }
            else {
                edgeDetectionMenu();
            }
        }

        // ---- LBP ----
        else if (strcmp(selection, "5") == 0) {
            if (!currentImage) {
                printf("Load an image first!\n");
                pause();
                continue;
            }

            unsigned char** out = allocate2D(height, width);

            computeLBP(currentImage, out, width, height);

            free2D(currentImage, height);
            currentImage = out;

            printf("LBP computed successfully.\n");
            pause();
        }

        else {
            printf("Invalid selection!\n");
            pause();
        }
    }

    return 0;
}
