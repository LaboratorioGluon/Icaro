# Guía de estilo

El objetivo de esta guía es intentar mantener un estilo homogéneo en cuestión de forma de escribir código y ficheros.

- [Ficheros](#ficheros)
- [Software](#Software)
- [Kicad](#Kicad)

## Ficheros

Los ficheros de código fuente siempre se escribirán en `lowerCamelCase.ext`.

Los ficheros de documentación se escribirán en `UpperCamelCase`

## Software
 
- **Naming**: usaremos `lowerCamelCase` para los nombres de variables, y `UpperCamelCase` para tipos/enumerados/clases/funciones.
```c 
class Test{ // UpperCamelCase
    
    void runTest(); // lowerCamelCase

    uint8_t internalVariable; //lowerCamelCase
};

Test testClass;

```
---
- **stdint.h**: En la parte de SW embarcado usaremos siempre los tipos definidos en `stdint.h`: `uint32_t`, `int8_t`,...[Más Info](https://cplusplus.com/reference/cstdint/)
---
- **Llaves**: Las llaves tendrán una nueva línea después del bloque que abren. Ejemplo:
```c
void f()
{
    if (...)
    {

    }
    else
    {
        
    }
}

```
---
## Kicad

- **Labels**: 
    - Siempre se usará la label más restrictiva que se pueda.
    - Se nombrarán con `UPPER_SNAKE_CASE`