# Interpretador de imágenes binarias

En este directorio se encuentra un programa que toma cualquier archivo y lo
reinterpreta como una imagen de grises donde cada byte es un pixel y la guarda
en formato png.

Se proporciona una imagen de prueba `test_image.bin`.

## Uso

El interpretador toma parámetros de `<ancho> <altura> <imagen_binaria>` donde ancho y altura son el ancho y altura de la imagen.
Con la imagen de prueba se puede utilizar de la siguiente manera:

```
make
./interpretar 800 503 test_image.bin
```

El interpretador guardará el resultado en un archivo `resultado.png`.
