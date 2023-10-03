#include <FreeImage.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Uso: %s [ancho] [altura] [imagen_binaria]\n", argv[0]);
    exit(1);
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  char *input_file = argv[3];

  if (width == 0 || height == 0) {
    fprintf(stderr, "Valor inválido de ancho o altura\n");
    exit(1);
  }

  FILE *fp;
  long filelen;

  fp = fopen(input_file, "rb");

  if (fp == NULL) {
    fprintf(stderr, "No se pudo abrir el archivo '%s'\n", input_file);
    exit(1);
  }

  fseek(fp, 0, SEEK_END); // Jump to the end of the file
  filelen = ftell(fp);    // Get the current byte offset in the file
  rewind(fp);             // Jump back to the beginning of the file

  uint8_t *buffer;

  buffer = (uint8_t *)malloc(filelen * sizeof(uint8_t));
  fread(buffer, filelen, 1, fp);
  fclose(fp);

  if (filelen != width * height) {
    fprintf(stderr,
            "El ancho*altura (%d*%d = %d) no calza con el tamaño del archivo "
            "(%ld bytes)\n",
            width, height, width * height, filelen);
    exit(1);
  }

  BYTE *pixels = (BYTE *)buffer;

  FIBITMAP *dib = FreeImage_ConvertFromRawBits(pixels, width, height, width, 8,
                                               0, 0, 0, FALSE);

  FreeImage_Save(FIF_PNG, dib, "resultado.png", 0);
}
