# Speki

Ein Audioplayer mit "Speki" auf dem CARME-M4-Kit unter Zuhilfenahme des Fast Fourier Transform Algorithmus. Erstellt als Projekt für das Modul BTE5053 Hardwarenahe Softwareentwicklung.

## Dokumentation
Infos über das `Was?` finden sich im [Pflichtenheft](doc/Pflichtenheft.md).
Über das `Wie?` informiert das [DFD](doc/DFD/DFD.md) und der Doxygen annotierte Quellcode.
Die Dokumentation lässt sich per Doxygen mit `doxygen doc/Doxyfile` in den Ordner `doc/doxygen/html` generieren.
Alternativ findet sich das aktuellste Exemplar auf [GitLab-Pages](http://leuen4.pages.ti.bfh.ch/speki).

## Installation und Nutzung
- `make` - Kompilieren mit [arm-none-eabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) Toolchain
- (optional) `make test` - statische Tests ausführen
- `/bin/speki.bin` mit [ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html) oder [STM32Cube](https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stm32cubeprog.html) auf das CARME-M4-Kit flashen
- Kopfhörer (HEAD) oder Lautsprecher (LINE) an der entsprechenden Buchse des CARMEs anschliessen
- Musik & Speki geniessen!

## Lizenz
[MIT](LICENSE) © [A. Reusser](mailto:reusa1@bfh.ch), [N. Leuenberger](mailto:leuen4@bfh.ch).
