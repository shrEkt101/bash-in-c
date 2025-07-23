#include <stdlib.h>
#include <stdio.h>

typedef struct Header{
  uint16_t bfType;
  int bfSize;
  uint32_t bfReserved;
  uint32_t bfOffBits;
} Header;

typedef struct InfoHeader{
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} InfoHeader;


/**
 * Prints metadata of bitmap file (Task 1).
 * input: Pointer to a bitmap file opened for reading.
 */
void print_metadata(FILE *input){
  // TODO: Implement function.

  struct Header header;
  struct InfoHeader infoHeader;

  fread(&header, 1, sizeof(Header), input);
  printf("%d\n", header.bfSize);
}

/**
 * Inverts image of a bitmap file and saves the inverted image to a new bitmap
 * file named "output.bmp" (Task 2).
 * input: Pointer to a bitmap file opened for reading
 */
void invert_bitmap(FILE *input){
  // TODO: Implement function.
}

int main(int argc, char **argv) {
  // open input file for binary reading
  FILE *input;
  input = fopen(argv[1],"rb");
  
  // call function to print metadata (Task 1).
  print_metadata(input);
  
  // call function to invert image (Task 2).
  invert_bitmap(input);

  // close input file
  fclose(input);
    
  return 0;
}
