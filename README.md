# TP1 - Inter Process Communication
Implementado por Tomas Becerra, Valentina Marti Reta y Agustin Alonso.
## Instrucciones de compilación y ejecución
### Compilación
Para compilar, ejecutar el Makefile con las variantes `clean` y `all` como indica el siguiente ejemplo. Utilizamos unicamente los flags `Wall` y `fsanitize=address`.
```
make clean all
```

### Ejecución - proceso aplicación
Para ejecutar el proceso aplicación sin conectar una vista:
```
./appx <files>
```

### Ejecución - proceso aplicación con vista conectada
Para ejecutar el proceso aplicación y luego conectarle una vista:
```
./appx <files> &
```
Y luego:
```
./viewx <pid> <amount_of_files>
```
En donde `<files>` referecencia a los archivos que se desean procesar, `<pid>` es el ID del proceso aplicación y `<amount_of_files>` es la cantidad de archivos procesados. Estos ultimos dos parametros se imprimen en salida estadar a la hora de ejecutar el proceso aplicación.

### Ejecución - ambos procesos conectados por un pipe
Simplemente ejecutar ambos procesos mediante un pipe como se muestra a continuación.
```
./appx <files> | ./viewx
```



