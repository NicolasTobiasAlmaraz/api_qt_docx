# API Qt Docx
En este repositorio encotnrarán una API que desarrollé para tener soporte en la escritura de archivos .docx desde Qt utilizando en lenguaje C++.

## Ejemplo de documento que podemos generar con la API
![image](https://github.com/user-attachments/assets/b3961003-72db-4751-a190-d2448c4e7072)


## Funcionalidades Ofrecidas
### Formato de hoja
- Tamaño
- Márgenes

### Texto con formato:
- Color de fondo
- Color de relleno
- Negrita
- Cursiva
- Alineación
- Fuente
- Tamaño de letra
- Subrayado
- Tachado

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

Este script de Python consumirá el XML que generamos previamente y hace uso del módulo llamado python-docx ([más info aquí](https://python-docx.readthedocs.io/en/latest/)) para generar el archivo .docx con todo lo que le indicamos.

Obviamente para hacer todo esto tenemos un conjunto de clases que nos permite hacer una capa de abstracción y se adjunta también un ejemplo demostrativo para generar el documento de prueba de la imagen anterior.

Cabe destacar que en este repositorio solamente se encuentra el ejecutable de Python hecho para Windows.

En caso de que estén trabajando en otro sistema operativo, deberían generar el ejecutable de los archivos del proyecto de python que se encuentra en la carpeta src_python/

Para esto pueden utilizar herramientas como py-installer ([Más info aquí](https://pyinstaller.org/en/stable/))

## API Reference
La documentación de las clases y métodos está en la "api_reference/index.html"

Allí encontrarán la documentación generada con Doxygen.

Cabe destacar que la mejor referencia que pueden tener es seguir el ejemplo de uso ubicado en la "carpeta example/"

Donde está el paso a paso de cómo generar el documento de ejemplo

## Contexto
Esta API la desarrollé en un marco académico dentro el PID (Proyecto de investigación y desarrollo) "uModelFactory".
Este proyecto se desarrolla en la Universidad Tecnológica Nacional Facultad Regional Buenos AIRES (UTN FRBA).
Consiste en una App para diseño, simulación y depuración de máquinas de estados.
Particularmente esta API la desarrollé para que la app ofrezca la posibilidad de generar documentación automáticamente acerca del proyecto que estamos desarrollando.

## Video introductorio a uModelFactory y demostración de uso de la API:
[![Presentacion CACiC 2023](https://img.youtube.com/vi/wtGmZHFcD08/0.jpg)](https://www.youtube.com/watch?v=wtGmZHFcD08&t=138s&ab_channel=NicolasAlmaraz)

## Más Información sobre uModelFactory
[UTN FRBA - uModeLFactory](https://www.frba.utn.edu.ar/electronica/umodelfactory-2/)
