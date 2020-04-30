ScrollEmu.exe: ScrollEmu.obj Makefile
	link.exe /Out:ScrollEmu.exe ScrollEmu.obj Shcore.lib User32.lib Shell32.lib /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level=requireAdministrator uiAccess=true"
	ping -n 2 localhost>NUL
	mt.exe -manifest ScrollEmu.exe.manifest -outputresource:ScrollEmu.exe;1
	FCIV -md5 -sha1 ScrollEmu.exe

ScrollEmu.obj: ScrollEmu.cpp Makefile
	cl.exe /c /EHsc /Zi ScrollEmu.cpp 

clean:
	del/q/s ScrollEmu*.exe *.obj
