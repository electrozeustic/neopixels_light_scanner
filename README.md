neopixels_light_scanner
=============================
Efecto de luces moviéndose de un lado para otro en una barra de 8 neopixels al estilo de el coche (KITT) de la serie El Coche Fantástico (Knight Rider).
Según el modo de funcionamiento, configurable mediante el código fuente antes de compilarlo, se puede usar: con la barra de neopixels (8 por defecto),
con la barra de neopixels más un botón que sirve para pausar/reanudar el movimiento de las luces con una pulsación corta o encenderlas haciendo un
fade-in o apagarlas haciendo un fade-out, esto con una pulsación larga. O con la barra de neopixels, más el botón y 3 potenciómetros que controlan la
velocidad del movimiento de las luces, el tono del color (hue) y el brillo global de las mismas dentro de unos límites. Y el último modo de
funcionamiento, que es usando la barra de neopixels junto a estos 3 potenciómetros solamente.

## Esquema de montaje (hardware)
Aquí se pueden ver los componentes usados para el proyecto:
![MONTAJE](images/assembly.png)

## Código fuente (software)
Usar la versión correspondiente según el entorno de programación que se vaya a utilizar.
Hace falta instalar en el entorno de programación las siguientes librerías antes de compilar el código fuente:
- [FastLED by Daniel Garcia](https://github.com/FastLED/FastLED)
- [FireTimer by PowerBroker2](https://github.com/PowerBroker2/FireTimer)

### Versión para el IDE de Arduino:
[Código fuente para el IDE de Arduino](ArduinoIDE/)

### Versión para PlatformIO:
[Código fuente para PlatformIO](PlatformIO/)

## Licencia
Mirar el archivo [LICENSE.md](./LICENSE.md)

## Renuncia de responsabilidad
Yo, electrozeustic (@ElectroZeusTIC) no me hago resposable del uso que se le pueda dar al software y al hardware usado en este proyecto por parte
de otras personas, empresas, instituciones, etc. Se ofrece tal como es, para experimentar y probar. Puede tener errores y por lo tanto, no usar en
entornos y situaciones críticas, ya que se puedan producir ciertos peligros con graves consecuencias. Las personas, empresas, entidades, etc que lo
usen serán los únicos responsables de las consecuencias que puedan ocurrir.
