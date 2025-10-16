run:
	./build/main

edit:
	helix src/main.cpp

com:
	sh bit.sh

cop:
	cp src/main.cpp src/main1.cpp

back:
	rm -rf src/main.cpp
	mv src/main1.cpp src/main.cpp
