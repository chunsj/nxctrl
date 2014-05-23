#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libnsbmp.h>

#include <NXCTRL_bitArray.h>

void *createBitmap (int nWidth, int nHeight, unsigned int nState);
void destroyBitmap (void *pBitmap);
unsigned char *getBitmapBuffer (void *pBitmap);
size_t getBitmapBPP (void *pBitmap);

unsigned char *loadBitmapFile (const char *pszPath, size_t *pnSize);
void warning (const char *pszCtx, bmp_result nCode);

int
main (int argc, const char *argv[]) {
  bmp_bitmap_callback_vt bitmapCBs = {
    createBitmap,
    destroyBitmap,
    getBitmapBuffer,
    getBitmapBPP
  };
  bmp_result nCode;
  bmp_image bmp;
  size_t nSize;
  unsigned short nRes = 0;
  unsigned char *pData;
  NXCTRLBITARRAY arrBits;
  FILE *pFile;
  int nRow, nCol, i;
  unsigned char *pImage;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s black_and_white.bmp output.h\n", argv[0]);
    return 1;
  }

  pData = loadBitmapFile(argv[1], &nSize);

  bmp_create(&bmp, &bitmapCBs);
  nCode = bmp_analyse(&bmp, nSize, pData);
  if (nCode != BMP_OK) {
    warning("bmp_analyse", nCode);
    nRes = 1;
    goto CLEANUP;
  }

  nCode = bmp_decode(&bmp);
  if (nCode != BMP_OK) {
    warning("bmp_code", nCode);
    if (nCode != BMP_INSUFFICIENT_DATA) {
      nRes = 1;
      goto CLEANUP;
    }
  }

  pFile = fopen(argv[2], "w");
  if (!pFile) {
    fprintf(stderr, "cannot create file %s\n", argv[2]);
    nRes = 1;
    goto CLEANUP;
  }

  fprintf(pFile, "// NAME:   %s\n", argv[1]);
  fprintf(pFile, "// WIDTH:  %u\n", bmp.width);
  fprintf(pFile, "// HEIGHT: %u\n", bmp.height);

  NXCTRLBITARRAYInit(&arrBits, bmp.width * bmp.height);
  pImage = (unsigned char *)bmp.bitmap;
  for (nRow = 0; nRow != bmp.height; nRow++) {
    for (nCol = 0; nCol != bmp.width; nCol++) {
      size_t z = (nRow * bmp.width + nCol) * 4; // bytes per pixel
      if (pImage[z] > 0) // only B & W image
        NXCTRLBITARRAYSet(&arrBits, (nRow * bmp.width + nCol));
      else
        NXCTRLBITARRAYClear(&arrBits, (nRow * bmp.width + nCol));
    }
  }

  for (i = 0; i < arrBits.nSize; i++) {
    fprintf(pFile, "%u, ", arrBits.arr[i]);
    if (((i + 1) % 4) == 0)
        fprintf(pFile, "\n");
  }

  fclose(pFile);

CLEANUP:
  bmp_finalise(&bmp);
  NXCTRLBITARRAYFree(&arrBits);

  return nRes;
}

unsigned char *
loadBitmapFile (const char *pszPath, size_t *pnDataSize) {
  FILE *pFile;
  struct stat st;
  unsigned char *pBuf;
  size_t nSize;
  size_t n;

  pFile = fopen(pszPath, "rb");
  if (!pFile) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  if (stat(pszPath, &st)) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  nSize = st.st_size;

  pBuf = malloc(nSize);
  if (!pBuf) {
    fprintf(stderr, "cannot allocate memory of size %lld\n",
            nSize);
    exit(EXIT_FAILURE);
  }

  n = fread(pBuf, 1, nSize, pFile);
  if (n != nSize) {
    perror(pszPath);
    exit(EXIT_FAILURE);
  }

  fclose(pFile);

  *pnDataSize = nSize;

  return pBuf;
}

void
warning (const char *pszCtx, bmp_result nCode) {
  fprintf(stderr, "%s: ", pszCtx);
  switch (nCode) {
  case BMP_INSUFFICIENT_MEMORY:
    fprintf(stderr, "Insufficiant memory");
    break;
  case BMP_INSUFFICIENT_DATA:
    fprintf(stderr, "Insufficient data");
    break;
  case BMP_DATA_ERROR:
    fprintf(stderr, "Data error");
    break;
  default:
    fprintf(stderr, "Unknown code %d", nCode);
  }
  fprintf(stderr, "\n");
}

void *
createBitmap (int nW, int nH, unsigned int nState) {
  return calloc(nW * nH, 4); // bytes per pixel
}

unsigned char *
getBitmapBuffer (void *pBitmap) {
  assert(pBitmap);
  return pBitmap;
}

size_t
getBitmapBPP (void *pBitmap) {
  return 4; // bytes per pixel
}

void
destroyBitmap (void *pBitmap) {
  assert(pBitmap);
  free(pBitmap);
}
