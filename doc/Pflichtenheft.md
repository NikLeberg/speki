# Pflichtenheft «Speki»
Das funktionale Ziel dieses Projekts ist die Anwendung der im Modul «Mathematik 1» erlernten Fouriertransformation, mit der ein zeitvariantes Signal in ein frequenzvariantes Signal umgewandelt werden kann. Von einem Audiosignal (im geeigneten Dateiformat) soll mit dem FFT Algorithmus (Fast Fourier Transformation) als Spektrum mit Balken dargestellt werden (Spektralbalkendiagramm), fortan Speki genannt.

[<img src="equalizer.jpg" width="50%"/>](equalizer.jpg)
<p style="font-size:0px">@image latex doc/equalizer.jpg Exemplarisches "Speki"</p>

Dieses Speki soll auf dem LCD des CARME-M4-Kits dargestellt werden. Zusätzlich soll das originale Audiosignal auch auf der Audiobuchse des CARMEs ausgegeben werden. Das CARME fungiert als einfacher Audioplayer.

Als Lernziel des Projekts ist vorgesehen, dass es hardwarenah in der Programmiersprache C entwickelt wird und aber sinnvolle Teile davon in Assembler implementiert werden. Als sinnvoll gelten in diesem Kontext Teile des Programms welche besonders viel Rechenleistung benötigen resp. viel Rechenzeit in Anspruch nehmen. Dies wird vermutlich der FFT-Algorithmus sein.

## Zusätze
Als Zusatz könnten folgende Ideen noch umgesetzt werden:
-	mehrere Audiosignale auswählbar per «Menü» z.B. ab SD-Karte oder USB-Stick
-	eine Art Equalizer (z.B. per Kippschalter) mit dessen Hilfe Frequenzbänder ausgeschaltet / abgeschwächt werden können
-	«Umkehrung» des Systems: anstelle, dass ein Signal an den Audiobuchsen ausgegeben wird, wird eines eingelesen und dieses am Speki dargestellt
-	ansprechende / schöne Darstellung: unterteilte Balken (Vierecke), höchster Wert bleibt für X Sekunden bestehen, Spotify-Like mit Albumcover und Fortschrittsanzeige
