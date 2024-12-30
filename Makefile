delete:
	rm -rf build/
run:
	cd build && cmake .. && make -j16
app:
	./build/AUDIO_APP_artefacts/Debug/Audio\ App.app/Contents/MacOS/Audio\ App