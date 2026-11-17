# HapticMelody

El proyecto consiste en un traductor que traduce un lenguaje de descripción musical a código ejecutable para microcontroladores (Arduino/ESP32), con el objetivo de controlar actuadores hápticos (motores de vibración) que permitan "sentir" la melodía de una canción a través del tacto. El traductor recibe un archivo con extensión .mus que contiene notas musicales, duraciones, tempo e intensidades, y genera un archivo .ino que puede ser compilado y cargado en un Arduino para producir vibraciones sincronizadas con la melodía. El lenguaje resuelve el problema de traducir notación musical a señales de control háptico, eliminando la necesidad de programar manualmente frecuencias, duraciones e intensidades. 

---

## ¿Cómo funciona el Lexer de HapticMelody?

El `LexicalAnalyzer` se encarga de leer el código fuente del archivo de partitura y descomponerlo en una secuencia ordenada de unidades léxicas (`HapticToken`). Su flujo de funcionamiento es el siguiente:

1. **Lectura y Validación del Archivo (`.mus`):**
   - El método `scanFile(filePath)` valida que el archivo tenga la extensión correcta (`.mus`), lo abre de forma segura utilizando flujos de C++ (`std::ifstream`) y carga todo su contenido en un búfer de texto.
   
2. **Análisis Secuencial y Seguimiento de Posición:**
   - La función `scanString()` procesa el texto carácter por carácter y token por token.
   - Mantiene un control estricto del número de **línea** y **columna** (`currentLine`, `currentColumn`), actualizándolas dinámicamente cada vez que encuentra saltos de línea (`\n`) o caracteres, lo que permite ubicar con total precisión cualquier elemento o error en el código fuente.

3. **Estructura del Token (`HapticToken`):**
   - Cada token generado almacena su tipo (`TokenType`), su texto literal o lexema (`lexeme`), y su coordenada exacta en el archivo (Línea y Columna), empaquetándose en una lista enlazada lista para la siguiente fase del compilador.

---

## Catálogo de Tokens Soportados

El lexer reconoce y clasifica los elementos del lenguaje en las siguientes categorías:

* **Palabras Clave de Declaración (`KW_DECLARATION`):** `TEMPO`, `COMPAS`, `TONALIDAD`, `MOTOR`, `PIN`.
* **Notas Musicales (`KW_NOTE`):** Soporta notación latina e inglesa (`DO`, `RE`, `MI`, `FA`, `SOL`, `LA`, `SI`, `C`, `D`, `E`, `F`, `G`, `A`, `B`), con opciones para alteraciones y número de octava (ej. `DO4`, `RE#3`).
* **Duraciones (`KW_DURATION`):** `REDONDA`, `BLANCA`, `NEGRA`, `CORCHEA`, `SEMICORCHEA`.
* **Eventos y Modificadores:**
  - Eventos (`KW_EVENT`): `SILENCIO`
  - Modificadores (`KW_MODIFIER`): `INTENSIDAD`
  - Modos (`KW_MODE`): `MAYOR`, `MENOR`
  - Control (`KW_CONTROL`): `REPETIR`, `VECES`
  - Accidentales (`KW_ACCIDENTAL` / `ACCIDENTAL_SYMBOL`): `SOSTENIDO`, `BEMOL`, `#`, `b`.
* **Literales y Símbolos:**
  - `IDENTIFIER`: Nombres de variables o identificadores personalizados.
  - `INTEGER` / `REAL`: Números enteros y decimales (ej. `120`, `3.14`).
  - `TIME_SIGNATURE`: Firmas de compás (ej. `4/4`, `3/4`).
  - `PERCENTAGE`: Valores porcentuales (ej. `80%`).
* **Delimitadores y Operadores:**
  - Asignación (`ASSIGN_OPERATOR`): `:`
  - Delimitadores de bloques y grupos (`BRACE_DELIMITER`, `PAREN_DELIMITER`): `{`, `}`, `(`, `)`
  - Separador (`COMMA_DELIMITER`): `,`
* **Control y Formato:**
  - `WHITESPACE`: Espacios y tabulaciones.
  - `NEWLINE`: Saltos de línea.
  - `LINE_COMMENT`: Comentarios de línea (iniciados con `//`).
  - `END_OF_FILE`: Marca de fin de archivo (`EOF`).

---

## Estructura del Proyecto

```text
HapticMelody/
├── include/
│   ├── haptic_lexer.hpp     # Interfaz de la clase LexicalAnalyzer
│   └── haptic_token.hpp     # Definición de TokenType, HapticToken y conversiones
├── src/
│   ├── lexer.cpp            # Lógica de escaneo, reglas léxicas y manejo de posiciones
│   └── main.cpp             # Programa principal para pruebas de consola
└── test/
    └── melodia_prueba.mus   # Archivo de partitura de prueba en formato .mus