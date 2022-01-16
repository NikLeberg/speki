# Profiling

## Poor man's profiling

Die Idee ist simpel: Mit dem Debugger in einem fixen oder zufälligen Intervall das Programm pausieren und anschauen was gerade ausgeführt wurde, resp. den Backtrace ausgeben lassen. (Halb-)Automatisiert mit GDB Skripts kann aus den Logdaten ein simples Profiling des Programms erstellt werden. Folgend wird der fixe Intervall des SysTick-Interrupts verwendet um mit einem Breakpoint bei diesem den Backtrace auszugeben.

1. Systick Interruptfrequenz anpassen, stellt Frequenz der "Abtastung" ein.
1. folgende Zeilen als `profiling.gdb` speichern:
```bash
set logging on
break SysTick_Handler
commands 1
backtrace
end
```
2. GDB Debugger starten in VSCode und in der Debug Konsole `-exev source profiling.gdb` eingeben
3. Debugging starten und ggf. immer wieder neustarten bis genügend Abtastungen aufgezeichnet wurden.
4. GDB hat während der Ausführung eine Logdatei `gdb.txt` angelegt. Mit folgendem regulären Ausdruck können die interessanten Daten aus dieser Logdatei extrahiert werden. Als RegEx-Parser eignet sich z.B. [regex101](https://www.regex101.com).
```regex
^~"#(\d+)  (?:0x.+in |)(.+) \(.+$
```
5. Die Daten mit Excel o.ä. analysieren z.B. die "Hits" per Funktion.


## Profiling mit ITM und Orbuculum

In Cortex-M basierten STM32 uC's ist eine Instrumentation Trace Macrocell (ITM) integriert. Mit dieser lässt sich unter anderem der Programmcode Live analysieren. Dazu wird mit OpenOCD eine debugging session gestartet, die ITM konfiguriert und die ITM Pakete an tcp Port 3443 bereitgestellt. Auf diesen Port kann sich dann [Orbuculum](https://github.com/orbcode/orbuculum) verbinden und die Pakete analysieren.

1. OpenOCD starten und konfigurieren: `openocd -f openocd_profiling.cfg`
2. Orbtop ausführen: `orbtop -e <elf_file>.elf -v 0`
- ggf. mit der `-I <ms>` Option die Aktualisierungsrate verringern um über einen längeren Zeitraum zu analysieren
- Die Verbindung mit OpenOCD ist aus unbekanntem Grund sehr instabil und bricht öfters nach einigen Sekunden mit folgendem Fehler ab: `Error: jtag status contains invalid mode value - communication failure`. OpenOCD einfach neustarten. Orbtop verbindet sich automatisch erneut.

## Quellen
- https://interrupt.memfault.com/blog/profiling-firmware-on-cortex-m
- https://github.com/orbcode/orbuculum
- https://github.com/japaric/itm-tools
- https://openocd.org/doc/html/Architecture-and-Core-Commands.html
