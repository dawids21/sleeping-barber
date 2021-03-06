# Problem śpiącego golibrody

## Wymagane programy
- gcc
- make

## Opis uruchomienia
- Ustaw parametry programu w pliku `fryzjerzy.c` za pomocą dyrektyw `#define`
- Skompiluj program poleceniem `make`
- Uruchom program `./fryzjerzy`

## Koncepcja realizacji
Program zawiera jeden plik wykonywalny z funkcją `main`, która odpowiada za utworzenie struktur początkowych
oraz uruchomienie procesów fryzjerów i klientów za pomocą instrukcji `fork`.

Rozwiązanie problemów związanych z synchronizacją wykonane jest przy pomocy mechanizmów IPC.

### Poczekalnia
Wykorzystana została kolejka komunikatów. Na początku wypełniamy całą kolejkę
komuniaktami `pusty`. Następnie, gdy klient chce wejść do kolejki musi odczytać komunikat `pusty`
i wpisać komunikat `pełny`, dając siebie jako ciało komunikatu. Gdy fryzjer odczytuje klient to
musi odczytać komunikat `pełny` i zapisac komunikat `pusty`. Przy próbie wejścia klienta do
poczekalni użyta została flaga `IPC_NOWAIT`, która powoduje, że jeżeli w danym momencie nie ma
żadnego komunikatu `pusty` to klient wychodzi z poczekalni i wraca do zarabiania.

### Fotele
Wykorzystany został pojedynczy semafor liczący o początkowej wartości równej liczbie
foteli w salonie. Gdy fryzjer chce zająć fotel musi opuścić ten semafor, a gdy go zwolnić - unieść.

### Kasa
Wykorzystana została współdzielona pamięć oraz semafor, który powoduje wzajemne wykluczanie
w dostępie do pieniędzy w kasie. Dodatkowo tworzona jest tablica semaforów, która ma za zadanie
powiadomić innych fryzjerów o zmianie ilości monet. Wykorzystane zostały semafory binarne.
Działają one w ten sposób, że każdy fryzjer ma przypisany jeden semafor i w momencie jakiejś
zmiany w kasie wszystkie te semafory są ustawiane na 1. Wtedy inni fryzjerzy wiedzą, że mogą
sprawdzić czy jest możliwość wydania reszty.

### Wydawanie reszty
Wykorzystana została kolejka komunikatów. Każdy komunikat jako swój typ podaje
id klienta. Po tym jak klient zajmie miejsce w poczekalni jego następną akcją jest oczekiwanie
na zwrot reszty. Czeka on więc na komunikat na tej kolejce, z jego id, który wysyłany jest przez
fryzjera, a w którego ciele znajduje się zwrócona reszta pomniejszona o monety wydane na usługę.
W ten sposób proces klienta modyfikuje posiadane przez siebie pieniądze. W celu obliczenia monet
do wydania został wykrozystany algorytm programowania dynamicznego, który zwraca najmniejszą
liczbę monet potrzebnych do wydania reszty.

### Płacenie
W celu zwiększenia losowości klient może albo zapłacić wydając najpierw monety
o największym nominale, albo o najmniejszym (patrz fryzjerzy.c:57).

### Logowanie
Przy implementacji programu został wykorzstany logger. Odnoszą się do niego funkcje
z `log` w nazwie. Funkcje zaczynające się od `i` dotyczą głównej części programu (poziom INFO).
Przy implementacji programu pomocny był niższy poziom logowania - DEBUG. Zmiana poziomu następuje
w pliku `fryzjerzy_logger.h` za pomocą `LOG_LEVEL`. W logach jest umieszczone PID procesu, to
czy dotyczny fryzjera (H) czy klienta (C). Przy zapisie monet są one wypisane od najmniejszego
nominału.
