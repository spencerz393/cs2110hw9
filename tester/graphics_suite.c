#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "graphics.h"
#include "qdbmp.h"
#include "azul.h"

#define RED ((Pixel) 0x1F)
#define GREEN ((Pixel) 0x3E0)
#define BLUE ((Pixel) 0x7C00)
#define YELLOW ((Pixel) 0x3FF)
#define BLACK ((Pixel) 0x0)
#define WHITE ((Pixel) 0x7FFF)

#define DARKRED ((Pixel) 0xF)
#define DARKGREEN ((Pixel) 0x1E0)
#define DARKBLUE ((Pixel) 0x3C00)

#define WIDTH 240
#define HEIGHT 160

#define MARGIN 10
#define BUFFER_WIDTH (WIDTH)
#define BUFFER_HEIGHT (HEIGHT + (2 * MARGIN))
#define BUFFER_SIZE (BUFFER_WIDTH * BUFFER_HEIGHT)

#define add_tcase(suite, setup_fixture, teardown_fixture, func) { \
    TCase *tc = tcase_create(#func); \
    tcase_set_timeout(tc, 10); \
    tcase_add_checked_fixture(tc, setup_fixture, teardown_fixture); \
    tcase_add_test(tc, func); \
    suite_add_tcase(s, tc); \
}

#define ck_assert_buffer_eq(b1, b2) {\
    for (unsigned int i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++) {\
        ck_assert_uint_eq(b1[i], b2[i]);\
    }\
}

static Pixel *paddedBuffer;
static Screen *screen;

static Pixel *compareBuffer;

void setup_screen() {
    paddedBuffer = malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));

    // Wipe the buffer
    for (unsigned int i = 0; i < BUFFER_SIZE; i++) {
    paddedBuffer[i] = WHITE;
    }

    // Draw a frame outside the actual display
    for (unsigned int i = 0; i < BUFFER_WIDTH; i++) {
    paddedBuffer[(MARGIN - 1) * BUFFER_WIDTH + i] = BLACK;
        paddedBuffer[(MARGIN + HEIGHT) * BUFFER_WIDTH + i] = BLACK;
    }

    // Get a pointer to the actual location of the display
    Pixel* buffer = paddedBuffer + (MARGIN * BUFFER_WIDTH);
    screen = malloc(sizeof(Screen));
    screen->size = (Vector){WIDTH, HEIGHT};
    screen->buffer = buffer;

    compareBuffer = malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));
}

void teardown_screen() {
    free(paddedBuffer);
    free(screen);

    free(compareBuffer);
}

void load_buffer(char* filename, Pixel *buffer) {
    BMP* bmp = BMP_ReadFile(filename);
    uint8_t r, g, b;

    for (unsigned int y = 0; y < BUFFER_HEIGHT; y++) {
    for (unsigned int x = 0; x < BUFFER_WIDTH; x++) {
        BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
            buffer[y * BUFFER_WIDTH + x] = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3);
        }
    }

    BMP_Free(bmp);
}

void save_buffer(char* filename, Pixel *buffer) {
    BMP* bmp = BMP_Create(BUFFER_WIDTH, BUFFER_HEIGHT, 24);
    Pixel pixel;
    uint8_t r, g, b;

    for (unsigned int y = 0; y < BUFFER_HEIGHT; y++) {
        for (unsigned int x = 0; x < BUFFER_WIDTH; x++) {
            pixel = buffer[y * BUFFER_WIDTH + x];
            r = ((pixel) & 0x1F) * 255 / 31;
            g = ((pixel >> 5) & 0x1F) * 255 / 31;
            b = ((pixel >> 10) & 0x1F) * 255 / 31;
            BMP_SetPixelRGB(bmp, x, y, r, g, b);
        }
    }

    BMP_WriteFile(bmp, filename);
    BMP_Free(bmp);
}

START_TEST (noFilter_untouched)
{
  ck_assert_uint_eq(noFilter(WHITE), WHITE);
  ck_assert_uint_eq(noFilter(BLACK), BLACK);
  ck_assert_uint_eq(noFilter(GREEN), GREEN);
  ck_assert_uint_eq(noFilter(GREEN | BLUE), GREEN | BLUE);
  ck_assert_uint_eq(noFilter(DARKRED | DARKBLUE), DARKRED | DARKBLUE);
}
END_TEST

START_TEST (greyscaleFilter_white)
{
  ck_assert_uint_eq(greyscaleFilter(WHITE), WHITE);
}
END_TEST

START_TEST (greyscaleFilter_black)
{
  ck_assert_uint_eq(greyscaleFilter(BLACK), BLACK);
}
END_TEST

START_TEST (greyscaleFilter_green)
{
  ck_assert_uint_eq(greyscaleFilter(GREEN), 19026);
}
END_TEST

START_TEST (greyscaleFilter_cyan)
{
  ck_assert_uint_eq(greyscaleFilter(GREEN | BLUE), 22197);
}
END_TEST

START_TEST (greyscaleFilter_darkpurple)
{
  ck_assert_uint_eq(greyscaleFilter(DARKRED | DARKBLUE), 6342);
}
END_TEST

START_TEST (redOnlyFilter_white)
{
  ck_assert_uint_eq(redOnlyFilter(WHITE), RED);
}
END_TEST

START_TEST (redOnlyFilter_black)
{
  ck_assert_uint_eq(redOnlyFilter(BLACK), BLACK);
}
END_TEST

START_TEST (redOnlyFilter_green)
{
  ck_assert_uint_eq(redOnlyFilter(GREEN), 0);
}
END_TEST

START_TEST (redOnlyFilter_cyan)
{
  ck_assert_uint_eq(redOnlyFilter(GREEN | BLUE), 0);
}
END_TEST

START_TEST (redOnlyFilter_darkpurple)
{
  ck_assert_uint_eq(redOnlyFilter(DARKRED | DARKBLUE), 15);
}
END_TEST

START_TEST (brighterFilter_white)
{
  ck_assert_uint_eq(brighterFilter(WHITE), WHITE);
}
END_TEST

START_TEST (brighterFilter_black)
{
  ck_assert_uint_eq(brighterFilter(BLACK), 15855);
}
END_TEST

START_TEST (brighterFilter_green)
{
  ck_assert_uint_eq(brighterFilter(GREEN), 16367);
}
END_TEST

START_TEST (brighterFilter_darkgreen)
{
  ck_assert_uint_eq(brighterFilter(DARKGREEN), 16111);
}
END_TEST

START_TEST (brighterFilter_darkpurple)
{
  ck_assert_uint_eq(brighterFilter(DARKRED | DARKBLUE), 24055);
}
END_TEST

START_TEST (drawPixel_simple)
{
    drawPixel(screen, (Vector){10, 10}, RED);
    drawPixel(screen, (Vector){20, 10}, YELLOW);
    drawPixel(screen, (Vector){18, 27}, BLUE);
    drawPixel(screen, (Vector){190, 140}, GREEN);

    save_buffer("actual/drawPixel_simple.bmp", paddedBuffer);

    load_buffer("expected/drawPixel_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawPixel_outofbounds)
{
    drawPixel(screen, (Vector){290, 0}, RED);
    drawPixel(screen, (Vector){290, 180}, GREEN);
    drawPixel(screen, (Vector){0, 180}, BLUE);

    save_buffer("actual/drawPixel_outofbounds.bmp", paddedBuffer);

    load_buffer("expected/drawPixel_outofbounds.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawFilledRectangle_simple)
{
    drawFilledRectangle(screen, &(Rectangle){(Vector){0, 0}, (Vector){240, 160}, RED});
    drawFilledRectangle(screen, &(Rectangle){(Vector){15, 15}, (Vector){30, 90}, GREEN});
    drawFilledRectangle(screen, &(Rectangle){(Vector){110, 150}, (Vector){120, 10}, BLUE});

    save_buffer("actual/drawFilledRectangle_simple.bmp", paddedBuffer);

    load_buffer("expected/drawFilledRectangle_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawFilledRectangle_outofbounds)
{
    drawFilledRectangle(screen, &(Rectangle){(Vector){10, 10}, (Vector){240, 20}, RED});
    drawFilledRectangle(screen, &(Rectangle){(Vector){15, 15}, (Vector){20, 200}, GREEN});

    save_buffer("actual/drawFilledRectangle_outofbounds.bmp", paddedBuffer);

    load_buffer("expected/drawFilledRectangle_outofbounds.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST


START_TEST (drawCircle_simple)
{
    drawCircle(screen, &(Circle){(Vector){145, 80}, 75, RED | BLUE});
    drawCircle(screen, &(Circle){(Vector){120, 80}, 50, BLACK});
    drawCircle(screen, &(Circle){(Vector){100, 80}, 30, RED});
    drawCircle(screen, &(Circle){(Vector){90, 80}, 20, BLUE});
    drawCircle(screen, &(Circle){(Vector){80, 80}, 10, GREEN});

    save_buffer("actual/drawCircle_simple.bmp", paddedBuffer);

    load_buffer("expected/drawCircle_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawCircle_outofbounds)
{
    drawCircle(screen, &(Circle){(Vector){0, 0}, 75, RED | BLUE});
    drawCircle(screen, &(Circle){(Vector){0, 159}, 50, BLACK});
    drawCircle(screen, &(Circle){(Vector){239, 0}, 30, RED});
    drawCircle(screen, &(Circle){(Vector){239, 159}, 20, BLUE});

    save_buffer("actual/drawCircle_outofbounds.bmp", paddedBuffer);

    load_buffer("expected/drawCircle_outofbounds.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_nocrop_simple)
{
    Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    drawImage(screen, &i, NULL, noFilter);

    save_buffer("actual/drawImage_nocrop_simple.bmp", paddedBuffer);

    load_buffer("expected/drawImage_nocrop_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_nocrop_outofbounds)
{
    Image i = {(Vector){120, 80}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    drawImage(screen, &i, NULL, noFilter);

    save_buffer("actual/drawImage_nocrop_outofbounds.bmp", paddedBuffer);

    load_buffer("expected/drawImage_nocrop_outofbounds.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_nocrop_withfilter)
{
    Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    drawImage(screen, &i, NULL, brighterFilter);

    save_buffer("actual/drawImage_nocrop_withfilter.bmp", paddedBuffer);

    load_buffer("expected/drawImage_nocrop_withfilter.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_withcrop_simple)
{
    Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    CropWindow w = {(Vector){120, 80}, (Vector){120, 80}};
    drawImage(screen, &i, &w, noFilter);

    save_buffer("actual/drawImage_withcrop_simple.bmp", paddedBuffer);

    load_buffer("expected/drawImage_withcrop_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_withcrop_outofbounds)
{
    Image i = {(Vector){10, 60}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    CropWindow w = {(Vector){120, 80}, (Vector){400, 800}};
    drawImage(screen, &i, &w, noFilter);

    w.top_left.x = 260;
    drawImage(screen, &i, &w, noFilter);

    save_buffer("actual/drawImage_withcrop_outofbounds.bmp", paddedBuffer);

    load_buffer("expected/drawImage_withcrop_outofbounds.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (drawImage_withcrop_withfilter)
{
    Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    CropWindow w = {(Vector){0, 0}, (Vector){120, 80}};

    // top left
    drawImage(screen, &i, &w, noFilter);

    // top right
    i.top_left.x = 120;
    w.top_left.x = 120;
    drawImage(screen, &i, &w, brighterFilter);

    // bottom right
    i.top_left.y = 80;
    w.top_left.y = 80;
    drawImage(screen, &i, &w, greyscaleFilter);

    // bottom left
    i.top_left.x = 0;
    w.top_left.x = 0;
    drawImage(screen, &i, &w, redOnlyFilter);

    save_buffer("actual/drawImage_withcrop_withfilter.bmp", paddedBuffer);

    load_buffer("expected/drawImage_withcrop_withfilter.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (rotateImage_simple) {
	Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    rotateImage(screen, &i, noFilter);

    save_buffer("actual/rotateImage_simple.bmp", paddedBuffer);

    load_buffer("expected/rotateImage_simple.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

START_TEST (rotateImage_withfilter) {
	Image i = {(Vector){0,0}, (Vector){AZUL_WIDTH, AZUL_HEIGHT}, azul};
    rotateImage(screen, &i, brighterFilter);

    save_buffer("actual/rotateImage_withfilter.bmp", paddedBuffer);

    load_buffer("expected/rotateImage_withfilter.bmp", compareBuffer);
    ck_assert_buffer_eq(paddedBuffer, compareBuffer);
}
END_TEST

Suite *graphics_suite(void) {
    Suite *s = suite_create("graphics");

    add_tcase(s, setup_screen, teardown_screen, noFilter_untouched);

    add_tcase(s, setup_screen, teardown_screen, greyscaleFilter_white);
    add_tcase(s, setup_screen, teardown_screen, greyscaleFilter_black);
    add_tcase(s, setup_screen, teardown_screen, greyscaleFilter_green);
    add_tcase(s, setup_screen, teardown_screen, greyscaleFilter_cyan);
    add_tcase(s, setup_screen, teardown_screen, greyscaleFilter_darkpurple);

    add_tcase(s, setup_screen, teardown_screen, redOnlyFilter_white);
    add_tcase(s, setup_screen, teardown_screen, redOnlyFilter_black);
    add_tcase(s, setup_screen, teardown_screen, redOnlyFilter_green);
    add_tcase(s, setup_screen, teardown_screen, redOnlyFilter_cyan);
    add_tcase(s, setup_screen, teardown_screen, redOnlyFilter_darkpurple);

    add_tcase(s, setup_screen, teardown_screen, brighterFilter_white);
    add_tcase(s, setup_screen, teardown_screen, brighterFilter_black);
    add_tcase(s, setup_screen, teardown_screen, brighterFilter_green);
    add_tcase(s, setup_screen, teardown_screen, brighterFilter_darkgreen);
    add_tcase(s, setup_screen, teardown_screen, brighterFilter_darkpurple);

    add_tcase(s, setup_screen, teardown_screen, drawPixel_simple);
    add_tcase(s, setup_screen, teardown_screen, drawPixel_outofbounds);

    add_tcase(s, setup_screen, teardown_screen, drawFilledRectangle_simple);
    add_tcase(s, setup_screen, teardown_screen, drawFilledRectangle_outofbounds);

    add_tcase(s, setup_screen, teardown_screen, drawCircle_simple);
    add_tcase(s, setup_screen, teardown_screen, drawCircle_outofbounds);

    add_tcase(s, setup_screen, teardown_screen, drawImage_nocrop_simple);
    add_tcase(s, setup_screen, teardown_screen, drawImage_nocrop_outofbounds);
    add_tcase(s, setup_screen, teardown_screen, drawImage_nocrop_withfilter);

    add_tcase(s, setup_screen, teardown_screen, drawImage_withcrop_simple);
    add_tcase(s, setup_screen, teardown_screen, drawImage_withcrop_outofbounds);
    add_tcase(s, setup_screen, teardown_screen, drawImage_withcrop_withfilter);
    
    add_tcase(s, setup_screen, teardown_screen, rotateImage_simple);
    add_tcase(s, setup_screen, teardown_screen, rotateImage_withfilter);

    return s;
}
