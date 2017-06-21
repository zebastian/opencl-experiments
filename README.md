# opencl-experiments
Analyse von OpenCL im Rahmen der Seminararbeit des Fachs IT-Infrastruktur

## Kompilieren
Dieses Programm lässt sich unter Linux mit folgendem Befehl kompilieren:

```g++ -lOpenCL vecAdd.c -o vecAdd```

Voraussetzung ist ein für das OpenCL Gerät korrekt installierter Treiber und
die Verfügbarkeit der nötigen Header Dateien.

## Ausgabe

Hier eine Beispielausgabe:

```
[sebastian@sebs-fedora opencl-experiments]$ ./vecAdd 
Benutze Platform: Intel(R) OpenCL
Benutze Gerät:        Intel(R) Core(TM) i3-3227U CPU @ 1.90GHz

Ergebnis des vecAdd Kernels (Berechnet A + B = [C]): 
3 + 1 = 4
3 + 2 = 5
3 + 3 = 6
3 + 4 = 7
3 + 5 = 8
3 + 6 = 9
4 + 7 = 11
4 + 8 = 12
```