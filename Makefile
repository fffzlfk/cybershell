build: configure
	cmake --build build

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug

clean:
	rm -rf build

run: build
	./build/bin/cybershell
