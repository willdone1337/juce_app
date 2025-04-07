delete:
	rm -rf build/AUDIO_APP_artefacts
run:
	cd build && cmake .. && make -j16
app:
	./build/AUDIO_APP_artefacts/Debug/Audio\ App.app/Contents/MacOS/Audio\ App 
