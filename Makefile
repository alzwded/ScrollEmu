ScrollEmu.exe: ScrollEmu.cpp Makefile
	cl.exe /EHsc /Zi ScrollEmu.cpp /FeScrollEmu.exe /link Shcore.lib User32.lib Shell32.lib /SUBSYSTEM:WINDOWS

clean:
	del/q/s ScrollEmu*.exe
