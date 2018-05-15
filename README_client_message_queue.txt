Instrukcja do prymitywnego, szybkiego przetestowania feature/client_message_queue
3 terminale:
cd cental/build
cd sensor/build x2

w sensor/build:
./rosa_sensor 9000
./rosa_sensor 9001

w central/build:
./rosa_central localhost 9000 9001

odstępy przy uruchamianiu - max kilka sekund (zgodnie z timeoutem ustowionym w client.getSocket().setReceiveTimeout
