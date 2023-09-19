# Filtro Laplaciano

En este directorio se encuentra la implementación del filtro Laplaciano 
utilizado en el proyecto. 

Además existe una aplicación de prueba para verificar su funcionamiento, cuenta
con las siguientes características para simular el comportamiento:

- Aloca un bloque de 60MB para simular la SDRAM.
- Simular el header compartido y aplicación del filtro mientras se desencripta la imagen periódicamente seteando que hay pixeles nuevos por procesar.
- Se emula el comportamiento de los 2 procesadores a través de threads.

## Consideraciones del filtro

- Se implementa a través de una convolución con el kernel de la [plantilla de nueve puntos](https://en.wikipedia.org/wiki/Discrete_Laplace_operator#Implementation_via_operator_discretization).
- Para la aplicación del filtro en los bordes de la imagen se [extiende el borde](https://en.wikipedia.org/wiki/Kernel_(image_processing)#Edge_handling).

## Compilación

```
make
```

## Ejecución

```
./verificar_filtro
```

## Compilación para LSP (clangd)
Si utiliza el language server `clangd` puede compilar el `compile_commands.json` utilizando [bear](https://github.com/rizsotto/Bear):

```
make clean; bear -- make
```
