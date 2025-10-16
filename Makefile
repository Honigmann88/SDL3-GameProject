run:
	./build/main

edit:
	helix src/main.cpp

com:
	sh bit.sh
	# g++ src/main.cpp -o bubu/main 'pkg-config --cflags --libs sdl3'

cop:
	cp src/main.cpp src/main1.cpp

back:
	rm -rf src/main.cpp
	mv src/main1.cpp src/main.cpp
