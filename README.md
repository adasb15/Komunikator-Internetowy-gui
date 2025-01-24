# Komunikator-Internetowy-gui
Aleksandra Bamberska 156021  
Adam Biernacki 155934  

## Proces kompilacji i uruchomienia projektu:

### 1. Serwer

a) Ustawienie w kodzie odpowiedniego ip komputera, który będzie chciał połączyć się do serwera. (w celu przesłania nowo przychodzących wiadomości)

b) Przejście do folderu w którym znajdują sie pliki źródłowe serwer:  
   `cd ./Komuniaktor-Internetowy-gui/server`

c) Kompilacja:  
   `g++ server.cpp -o "nazwa pliku wykonawczego np. server" -pthread`  
   *(ostatnia flaga dla g++/c++ w wersji 13+ nie jest konieczna)*

d) Uruchomienie:  
   `./server`

### 2. Klient

a) Ustawienie w kodzie odpowiedzniego ip server w plikach nagłówkowych `client.h` oraz `message.h`

b) Przy użyciu programu Qt Creator zbudowanie kodu, po którym nastąpi samo czynne uruchomienie

b2) Jeśli projekt został już wcześniej poprawnie zbudowany wystarczy uruchomić plik wykonawczy z folderu `build`
