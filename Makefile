build: configure
	cmake --build build

configure:
	cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build

clean:
	rm -rf build

run: build
	./build/bin/cybershell
