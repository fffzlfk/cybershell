build: configure
	cmake --build build

configure:
	cmake -B build -GNinja

clean:
	rm -r build

run: build
	./build/bin/cybershell
