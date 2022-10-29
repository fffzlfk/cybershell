build: configure
	cmake --build build

configure:
	cmake -B build

clean:
	rm -r build

run: build
	./build/bin/cybershell
