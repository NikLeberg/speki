# Speki

Ein Audioplayer mit "Speki" auf dem CARME-M4-Kit unter Zuhilfenahme der diskreten Fourier Transformation. Erstellt als Projekt für das Modul BTE5053 Hardwarenahe Softwareentwicklung.

## Dokumentation
Infos über das `Was?` finden sich im [Pflichtenheft](./doc/Pflichtenheft.md).
Über das `Wie?` informiert das [DFD](./doc/DFD/DFD.md) und der Doxygen annotierte Quellcode.
Die Dokumentation lässt sich per Doxygen mit `doxygen doc/Doxyfile` in den Ordner `doc/doxygen/html` generieren.
Alternativ findet sich das aktuellste Exemplar auf [GitLab-Pages](http://leuen4.pages.ti.bfh.ch/speki).

## Nutzung ohne Windows WSL2
- `make` - Kompilieren mit [arm-none-eabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) Toolchain
- (optional) `make test` - statische Tests ausführen
- `/bin/speki.bin` mit [ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html) oder [STM32Cube](https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stm32cubeprog.html) auf das CARME-M4-Kit flashen
- Geeignete Songs gemäss [Anleitung](./songs/README.md) erstellen und auf SD-Karte laden
- Kopfhörer oder Lautsprecher an der HEAD Buchse des CARMEs anschliessen
- Per Menü ein Song auswählen:
    - Im Hauptmenü:
        - Button T3: Auswahl um eine Zeile nach unten verschieben
        - Button T2: Auswahl um eine Zeile nach oben verschieben
        - Button T0: Aktuell ausgewählter Song abspielen
    - Währendem ein Song abspielt:
        - Button T1: Song stoppen und zum Hauptmenü zurück
        - Potentiometer (Analog In 0): Lautstärkeregulierung
- Musik & Speki geniessen!

## Nutzung unter Windows WSL2
Nach Einrichten des Devcontainers unter WSL2 lässt sich per [usbipd](https://github.com/dorssel/usbipd-win) den USB-Port des physischen STM32-Debugger an WSL2 weiterleiten. Dazu in einem Windows Terminal mit Administratorrechten folgende Befehle verwenden:
- `usbipd wsl list` - Auflisten aller verfügbaren USB Geräte
- `usbipd wsl attach --busid <X-Y>` - Weiterleiten des per busid ausgewählten Geräts
- (optional) `sc start usbipd` - bei Fehlermeldung hiermit den Service starten
- Das Flashen und das Debugging funktioniert nun direkt aus VSCode heraus: Dazu die vordefinierten Aufgaben `flash` oder `openocd` verwenden.
- Weiteres Vorgehen wie im vorherigen Abschnitt.

## Projektstand
Die grundlegenden Funktionen gemäss [Pflichtenheft](./doc/Pflichtenheft.md) wurden alle umgesetzt. Einige der genannten Erweiterungen wurden implementiert:
- *mehrere Audiosignale auswählbar*: indem ab SD-Karte belibige Songs abspielbar sind
- *schöne Darstellung*: Albumcover, Vortschrittsanzeige und Song-Metatinformationen werden übersichtlich dargestellt

Das neuschreiben *sinnvoller Teile in Assembler* sollte gemäss Profiling (siehe [hier](./doc/Profiling.md)) ermittelt werden um die kritischen und langsamen Teile des C Codes zu optimieren. Das Profiling ergab aber, dass die meiste Zeit > 60% in der BSP und sGUI Library verweilt wird. Hauptsächlich wurde auf die SD-Karte gewartet oder Pixel an das LCD gesendet. Der von uns als langsam vermutete Teil des DFT-Algorithmus beanspruchte lediglich ~ 20% der CPU. Da wir nicht zu fest mit den Librarys kämpfen wollten, optimierten wir dennoch den DFT-Algorithmus. In folgender Tabelle ist die relativ beansprchte CPU Zeit mit C oder ASM Implementierung und O0 oder O4 Optimisierungslevel vermerkt.

|  Optimisierungslevel  | C   | ASM |
|----|-----|-----|
| O0 | 22% | 5%  |
| O4 | 6%  | 5%  |

Somit wurde das von uns gesetzte Ziel erreicht! Die ASM Implementierung ist schneller als der optimierte Code des Compilers.

Welche Implementierung die DFT verwendet, lässt sich per `DFT_USE_ASM` Makro in [dft.h](./inc/dft.h) auswählen.

## Verwandte Projekte
[carme-template](https://gitlab.ti.bfh.ch/jeken1/carme-template) - Vorlage für STM32CubeIDE unabhängige CARME Projekte.

## Lizenz
[MIT](LICENSE) © [A. Reusser](mailto:reusa1@bfh.ch), [N. Leuenberger](mailto:leuen4@bfh.ch).
