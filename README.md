![GOLPI logo](GOLPI/GOLPI.png?raw=true "GOLPI logo")
# Gnu Octave to Labview Pipes Interface

Try [GNU Octave](https://www.gnu.org/software/octave/) in [LabVIEW](http://www.ni.com/labview/)
with a new shiny interface!

LabVIEW can do some very limited mathematics. Therefore one has to use an external software. LabVIEW
provides easy connection to e.g. [Matlab](https://www.mathworks.com/products/matlab/), however
Matlab is not free, so if you want to make a measurement program and use it on many computers, you
need multiple licences or expensive server licence. 

Or you can use GNU Octave - open source and free software with almost the same
syntax as Matlab, and similar capabilities and speed. Unfortunately GNU Octave is not directly
supported by LabVIEW. In Linux, you can use comfortable pipes to interface GNU Octave. For unknown
reasons Windows pipes are still missing in LabVIEW. Some old pipes implementation exist but not
really working in new LabVIEW versions. So GOLPI (Gnu Octave to Labview Pipes Interace) was created.

GOLPI works as a device. You initialize it, sends values or commands into, reads variables or
outputs, repeat as you like, if you are finished you quit it.

##Features:

- Pipes (stdin and stdout) are used for communication to and from GNU Octave.
- Polymorphic VI for reading a value of a variable (by means of stdout or temporary file for large amount of data).
- Polymorphic VI for seting a value to a variable (by means of stdout or temporary file for large amount of data).
- Can check if GNU Octave is still running.
- Debug mode can store all communication going through pipes.
- Debug console can be shown to view ongoing communication.
- 3 examples included.
- Tested on several versions of GNU Octave in Windows.

##Three components:

- **lv_process**: C++ library which handles pipes.
- **LV Process**: LabVIEW interface for lv_process
- **GOLPI**: starts GNU Octave, send commands, read commands etc.

## LV Process, lv_process
Yet another implementation of pipes in LabVIEW for Windows operating system. (OpenG implementation
seems to be abandoned). VIs call lv_process.dll to start a process, send and receive data through
stdin and stdout, closes process. Pipe is in fact cluster containing handlers and setting. User can
change the cluster values as required (be careful). All traffic can be logged if debug mode is
started. Debug console can be shown on the background.

It can be simply used as standalone (without GOLPI).

## GOLPI Examples 

Block diagram of the VI Tree: 

![GOLPI VI Tree](GOLPI_VI_Tree.png?raw=true "GOLPI VI Tree")

Block diagram of very simple demonstration: 

![GOLPI block diagram](GOLPI_Demo_-_Simple_Use.png?raw=true "GOLPI block diagram")

## Download
- [source codes - GOLPI GitHub repository](https://github.com/KaeroDot/GOLPI)
- [GOLPI 0.5.0.1 packed library](https://github.com/KaeroDot/GOLPI/tree/master/builds/GOLPI%200.5.0.1)
- [GOLPI 0.5.0.1 VI Package](https://github.com/KaeroDot/GOLPI/blob/master/builds/golpi_lvlib-0.5.0.1.vip?raw=true)
- [LV Process 0.6.0.1 packed library](https://github.com/KaeroDot/GOLPI/tree/master/builds/LV%20Process%200.6.0.1)
- [LV Process 0.6.0.1 VI Package](https://github.com/KaeroDot/GOLPI/blob/master/builds/lv_process_lvlib-0.6.0.1.vip?raw=true)

## Installation & use
Download packed library or all source codes. Copy into your LabVIEW project. Put lv_proc.dll into
the root directory of your project.

## License
GOLPI, LV Process and LV Process dll is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

GOLPI, LV Process and LV Process dll  is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with GOLPI, LV Process and LV Process dll.  If not, see <http://www.gnu.org/licenses/>.
