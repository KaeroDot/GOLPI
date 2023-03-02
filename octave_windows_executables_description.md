# Reason
GNU Octave for windows is somewhat erratic. The system of executables differs
through the versions. This document tries to map it. This is important for VI
"Check and Generate Path to Octave Executable.vi".

# Table
Table below shows typical installation paths, executables and what happens
after running.

Actions are:
- console: Windows console with GNU Octave is started.
- gui: GUI of GNU Octave is started.
- console->gui: Windows console is started and immediately a GUI is started.
- black console: Windows console with nothing in it is started.

| version | path | executable | action |
|---------|------|------------|--------|
4.0.0 | c:\Octave\Octave-4.0.0\ | octave.bat  | console->gui
4.0.0 | c:\Octave\Octave-4.0.0\ | octave.vbs  | gui
4.0.0 | c:\Octave\Octave-4.0.0\bin\ | octave.exe | console->gui
4.0.0 | c:\Octave\Octave-4.0.0\bin\ | octave-4.0.0.exe | console->gui
4.0.0 | c:\Octave\Octave-4.0.0\bin\ | octave-cli.exe | console
4.0.0 | c:\Octave\Octave-4.0.0\bin\ | octave-cli-4.0.0.exe | console
4.0.0 | c:\Octave\Octave-4.0.0\bin\ | octave-gui.exe | gui
4.2.2 | c:\Octave\Octave-4.2.2\ | octave.bat | nothing
4.2.2 | c:\Octave\Octave-4.2.2\ | octave.vbs | gui
4.2.2 | c:\Octave\Octave-4.2.2\bin\
4.2.2 | c:\Octave\Octave-4.2.2\bin\ | octave.exe | nothing
4.2.2 | c:\Octave\Octave-4.2.2\bin\ | octave-4.2.2.exe | nothing
4.2.2 | c:\Octave\Octave-4.2.2\bin\ | octave-cli.exe | cli
4.2.2 | c:\Octave\Octave-4.2.2\bin\ | octave-cli-4.2.2.exe | clin
4.2.2 | c:\Octave\Octave-4.2.2\bin\ | octave-gui.exe | black console
5.1.0.0 | c:\Octave\Octave-5.1.0.0\ | octave.vbs | gui
5.1.0.0 | c:\Octave\Octave-5.1.0.0\clang32\bin\ | empty dir | 
5.1.0.0 | c:\Octave\Octave-5.1.0.0\clang64\bin\ | empty dir | 
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw32\bin\ | empty dir | 
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw64\bin\ | octave.bat | cli
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw64\bin\ | octave-5.1.0.bat | cli
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw64\bin\ | octave-cli.exe | cli
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw64\bin\ | octave-cli-5.1.0.exe | cli
5.1.0.0 | c:\Octave\Octave-5.1.0.0\mingw64\bin\ | octave-gui.exe | black console
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\ | octave.vbs | gui
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\clang32\bin\ | empty dir |
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\clang64\bin\ | empty dir |
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw32\bin\ | empty dir |
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw64\bin\ |  octave.bat | cli
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw64\bin\ |  octave-6.2.0.bat | cli
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw64\bin\ |  octave-cli.exe | cli
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw64\bin\ |  octave-cli-6.2.0.exe | cli
6.2.0 | c:\Program Files\GNU Octave\Octave-6.2.0\mingw64\bin\ |  octave-gui.exe | cli (not a mistake)
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\ | octave.vbs | gui
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\ | octave-launch.exe | gui
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\clang32\bin\ | empty dir |
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\clang64\bin\ | empty dir |
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw32\bin\ | empty dir |
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave.bat | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave.exe | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave-7.1.0.bat | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave-7.1.0.exe | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave-cli.exe | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave-cli-7.1.0.exe | cli
7.1.0 | c:\Program Files\GNU Octave\Octave-7.1.0\mingw64\bin\ | octave-gui.exe | cli (not a mistake)
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\ | octave.vbs | gui
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\ | octave-launch.exe | gui
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\clang32\bin\ | empty dir |
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\clang64\bin\ | empty dir | 
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw32\bin\ | empty dir |
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | octave.bat | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | octave.exe | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | ctave-7.2.0.bat | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | ctave-7.2.0.exe | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | ctave-cli.exe | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | ctave-cli-7.2.0.exe | cli
7.2.0 | c:\Program Files\GNU Octave\Octave-7.2.0\mingw64\bin\ | ctave-gui.exe | cli (not a mistake)
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\ | octave.vbs | gui
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\ | octave-launch.exe | gui
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\clang32\bin\ | empty dir |
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\clang64\bin\ | empty dir | 
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw32\bin\ | empty dir |
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | octave.bat | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | octave.exe | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | ctave-7.3.0.bat | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | ctave-7.3.0.exe | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | ctave-cli.exe | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | ctave-cli-7.2.0.exe | cli
7.3.0 | c:\Program Files\GNU Octave\Octave-7.3.0\mingw64\bin\ | ctave-gui.exe | cli (not a mistake)
