# GNSS PLANNING

This project was carried out as part of the **Satellite Navigation** class taught at Warsaw University of Technology in the summer semester 2023.

### Introduction and installation

<!-- Zadaniem programu jest wspomaganie planowania satelitarnych misji pomiarowych poprzez wyliczenie warunkow geometrycznych panujacych w wybranym czasie. Warunki te mają bezpośredni wpływ na jakość oraz precyzję pomiaru. W celu skompilowania programu należy zainstalować GCC (Gnu Compiler Collection). Następnie wystarczy w terminalu wywołać compilator: -->
The programme's task is to support the planning of satellite measurement missions by calculating the geometric conditions prevailing at the selected time. These conditions have a direct impact on the quality and precision of the measurement. To compile the programme, GCC (Gnu Compiler Collection) must be installed. Then simply call the compiler in the terminal:

```
g++ source/main.cpp -O3 -o gnssplanning
```

<!-- Zostanie utworzony plik o nazwie `gnssplanning` który jest docelowym programem. Żeby z niego skorzystać wywołać go w terminalu z dodaniem niezbędnych argumentów: -->
A file named `gnssplanning` will be created which is the target program. To use it call it in a terminal with all the necessary arguments:

<!-- - almanach (ścieżka do pliku z danymi o orbitach satelitów)
- epoka 0 (czas rozpoczęcia misji pomiarowej w formacie YYYY-MM-DD-hh-mm-ss)
- epoka 1 (czas zakończenia misji pomiarowej w formacie YYYY-MM-DD-hh-mm-ss)
- interwał (odstęp czasu co jaki program ma wyznaczyć warunki geometryczne)
- $\phi$ odbiornika (szerokość geograficzna w której znajduje się odbiornik)
- $\lambda$ odbiornika (długość geograficzna w której znajduje się odbiornik)
- $h$ odbiornika (wysokość nad poziomem morza na jakiej znajduje się odbiornik)
- maska (kąt opisujący ograniczenie widoczności odbiornika) -->
- almanac (path to file with data on satellites orbits)
- epoch 0 (start time of the survey mission in the format YYYY-MM-DD-hh-mm-ss)
- epoch 1 (end time of measurement mission in the format YYYY-MM-DD-hh-mm-ss)
- interval (time interval for the program to determine geometric conditions)
- $\phi$ of the receiver (latitude in which the receiver is located)
- $\lambda$ of the receiver (the longitude in which the receiver is located)
- $h$ of the receiver (the height above sea level at which the receiver is located)
- mask of the receiver (angle describing the visibility limitation of the receiver)

<!-- Dodatkowo użytkownik może wskazać które z satelitów biorą udział w pomiarach. Wystarczy zakończyć komendę znakami `--` a następnie wymienić numery ID konkretnych satelitów (w formacie Trimble) lub nazwy systemów GNSS. Poniżej można zobaczyć podział numerów ID na odpowiadające im systemy: -->
In addition, the user can indicate which satellites are involved in the measurements. Simply end the command with `--` and then list the ID numbers of the specific satellites (in Trimble format) or the names of the GNSS systems. Below you can see the breakdown of the ID numbers into their corresponding systems:

<center>

|ID|System|
|:--:|:--:|
|1..37|GPS|
|38..64|GLONASS|
|111..118|QZSS|
|201..263|GALILEO|
|264..283|BEIDOU|

</center>

An example program call may look like this:

```
gnssplanning data/Almanac.alm 2023-02-23-00-00-00 2023-02-23-00-30-00 60 52 21 100 10 -- 1 2 3 QZSS
```

### Global satellite position



### Azimuth and satellite elevation



### Dilution of precision



### Visualization