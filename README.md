# API Qt Docx
En este repositorio encotnrarán una API que desarrollé para tener soporte en la escritura de archivos .docx desde Qt utilizando en lenguaje C++.

## Funcionalidades Ofrecidas
### Formato de hoja
- Tamaño
- Márgenes

### Texto con formato:
- Color
- Negrita
- Cursiva
- Alineación
- Fuente
- Tamaño de letra
- Subrayado

### Encabezado y pie de página
- Dentro podemos poner texto y una imágen a modo de rótulo

### Imágenes
- Tamaño
- Alineación
- Anclaje

### Listas de Items
- Dentro podemos poner texto con formato

### Tablas
- Dentro podemos poner texto con formato

## Qt Version
v5.12

## Funcionamiento
El esquema de funcionamiento es el que se ve en la figura:
![image](https://github.com/user-attachments/assets/86203528-292c-4886-853e-b251a8953259)

La idea sería desde Qt escribir un archivo XML auxiliar indicando todo lo que queremos escribir y una vez que hayamos terminado de escribir todo lo deseado, desde Qt lanzamos un thread que ejecuta un script de Python.

Este script de Python consumirá el XML que generamos previamente y hace uso del módulo llamado python-docx ([más info aquí](https://python-docx.readthedocs.io/en/latest/) para generar el archivo .docx con todo lo que le indicamos.

Obviamente para hacer todo esto tenemos un conjunto de clases que nos permite hacer una capa de abstracción y se adjunta también un ejemplo demostrativo para generar este documento de prueba:

TODO: (adjuntar foto)

## API Reference
La documentación de las clases y métodos está en la carpeta doc/index.html
Allí encontrarán la documentación generada con Doxygen

## Contexto
Esta API la desarrollé en un marco académico dentro el PID (Proyecto de investigación y desarrollo) "uModelFactory".
Este proyecto se desarrolla en la Universidad Tecnológica Nacional Facultad Regional Buenos AIRES (UTN FRBA).
Consiste en una App para diseño, simulación y depuración de máquinas de estados.
Particularmente esta API la desarrollé para que la app ofrezca la posibilidad de generar documentación automáticamente acerca del proyecto que estamos desarrollando.

## Video introductorio a uModelFactory y demostración de uso de la API:
[![Presentacion CACiC 2023](https://img.youtube.com/vi/wtGmZHFcD08/0.jpg)](https://www.youtube.com/watch?v=wtGmZHFcD08&t=138s&ab_channel=NicolasAlmaraz)

## Más Información sobre uModelFactory
[UTN FRBA - uModeLFactory](https://www.frba.utn.edu.ar/electronica/umodelfactory-2/)
