----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
Generador de archivos ODT - Algunas aclaraciones
----------------------------------------------------------------------------------
----------------------------------------------------------------------------------


*********************Que hace este proyecto?*********************


El proyecto tiene como finalidad crear una API para escribir archivos ".odt" desde un código de Qt en C++.


*********************Como funciona el proyecto?*********************


Para que todo funcione lo que se hace es escribir un archivo llamado genOdt.xml desde Qt donde se detalla todo lo que se desea escribir en el .odt y la el path de salida

Para esta versión podemos hacer uso de: 
-texto con formato y alineacion al párrafo
-imágenes con alineacion al parrafo
-tablas
-configuración de márgenes y tamaño de página
-listas de items
-header
-footer

Luego este archivo XML es consumido desde un script de python que utiliza el módulo "odfpy" para poder cumplir con todo lo detallado.


*********************Que hay en cada carpeta?*********************


En la carpeta EjecutableWindows y EjecutableLinux se encuentran los archivos ejecutables que debemos llamar desde Qt. Dependiendo el sistema operativo en el que estemos trabajando.
En la carpeta Sources Python y Sources Qt están:
Codigos fuente de C++ con un ejemplo de uso main.cpp
Codigos fuente de python (no deberian ser necesario su uso).
En la carpeta Imagenes hay una imagen para correr el ejemplo del main.cpp


*********************Como se usa el proyecto?*********************


Para poder usar el proyecto tendremos las siguientes clases en Qt:

-> textOdt:
es un objeto texto al que podremos setearle el contenido, alineación de párrafo, tamaño de letra, fuente, negrita, cursiva, tachado, subrayado, color de fondo y color de texto

-> tableOdt:
Es un objeto tabla al que debemos indicarle numero de filas y columnas.
Luego podremos ir agregándole objetos "tableItem" indicando en que posicion de la tabla irán

-> tableElement:
son objetos tipo texto que además tienen como atributos el numero de fila y columna donde deben ser ubicados

-> pageOdt:
es un objeto que contiene informacion acerca de margenes y tamaño de hoja

-> imageOdt:
es un objeto que contiene el path de la imagen y tamaño de la misma

-> writerOdt: 
Es la clase encargada de escribir el archivo genOdt.xml con sus métodos writeText(), writeImage, writeTable()
Cada uno de estos métodos recibe como parámetro su respectivo objeto
En el constructor debemos indicarle:
Ruta donde guardemos el ejecutable de python
Ruta donde vamos a generar elarchivo odt
Objeto page setup (sirve para indicar tamaño de pagina y margenes)
Finalmente para poder guardar el XML y consumirlo desde python debemos llamar al método generateOdt()


*********************Cómo generar un ejecutable de python*********************


Para hacer el ejecutable hago uso de la funcion pyinstaller
Para más info, me ayudó mucho este video https://www.youtube.com/watch?v=UZX5kH72Yx4



*********************Consejo en caso de querer usar odfpy en el futuro*********************


En caso de querer modificar algo desde python deben instalar la biblioteca odfpy. Para ello visiten su web y lean el instructivo README https://github.com/eea/odfpy
odfpy tiene el potencial de escribir todos los tipos de objetos que vemos en el Writer de Libre Office.
Para ahorrarles tiempo es bastante útil que sepan la estructura de un archivo .odt

Este tipo de archivos están basados en la estructura XML. En líneas generales, el módulo odfpy lo que hace es hacer uso de funciones que completan los tags y atributos de dicha estructura.
Generalmente todos los items que podemos usar en los archivos de Libre Office (ya sean tablas, texto, imagenes, parrafos, titulos, etc) tienen dos partes principales: el objeto en sí y el style que usa.
En el objeto podremos ver atributos pertinentes como pueden ser por ejemplo, la cadena de caracteres a escribir en un párrafo.
Por otro lado el style tendrá atributos relacionados al formato, por ejemplo alineacion de parrafo, negrita, italica, subrayado, etc y todo esto con sus variantes.

Para lograr usar estas herramientas en primer lugar se escribe el Style a usar.
Al hacer esto, la clase nos pedirá que le demos un nombre personalizado nuestro y le indiquemos qué tipo de estilo será (Por ejemplo, estilo de párrafo, de texto, de imágnes, etc).
Dependiendo la familia indicada debemos configurar diferentes atributos. Esta informacion irá escrita en el archivo .odt usando la funcion addElement().
NOTA: Varias clases tienen este método, siempre que es llamado escribe informacion en el .odt

Luego, debemos indicarle los atributos que necesitemos.
En general para indicar dichos atributos se usan metodos que al final digan "Properties". Por ejemplo, GraphicProperties para indicarle atributos a las imágenes.

Acto seguido, podremos invocar objetos texto por ejemplo, donde se le indicará el contenido del mismo pero debemos indicarle también el Style tanto para el párrafo como para el texto.
En general, las tablas, imágenes y demás cuestiones funcionan de manera similar.

Por otro lado, les aconsejo que para hacer uso de dicho módulo primero busquen en el user-manual, pero muchas cosas no van a estar alli (por ejemplo, como debemos completar los atributos de las funcioens setProperties).
Lo mas rapido es generar un .odt con lo que sea que se desee programar, guardarlo como XML y abrirlo con un editor de texto para analizar los atributos y tags necesarios.
Las "properties" están en tags que llevan justamente esta palabra.
En lineas generales lo que hacen los archivos .odt es crear en primer lugar los Styles al principio y luego los llaman con el nombre guardado (generalmente los styles de párrafos son llamados P1, P2, P3, etc).
Los atributos están escritos con palabras y guiones, por ejemplo, una tributo podría ser "page-width":"22cm" pero cuando lo querramos completar desde la clase de python el atributo debemos ponerlo todo en mínuscula y sin guiones
quedando: "pagewidth":"22cm".

NOTA: el user manual nos dirá que hay un atributo llamado "pagewidth" pero no sabremos con qué completarlo. Por eso es importante crear los .odt de prueba para ver cómo completar dicho atributo.

En este link hay un ejemplo muy completo de como hacer diferentes cosas en nuestro odt
https://mashupguide.net/1.0/html/ch17s04.xhtml


*********************Contacto*********************


Nicolás Almaraz :)
UTN - FRBA
uModelFactory
Cualquier consulta:
nnico.almaraz@gmail.com
nalmaraz@frba.utn.edu.ar