//---------------------------------------------------------------------------------------------------------------------
// LV Process DLL
//---------------------------------------------------------------------------------------------------------------------
// Author: Stanislav Maslan
// E-mail: s.maslan@seznam.cz, smaslan@cmi.cz
// www: https://forums.ni.com/t5/Community-Documents/LV-Process-Windows-pipes-LabVIEW/tac-p/3497843/highlight/true
// Revision: V4.0, 2016-12-05
//
//
// LICENSE:
// --------
// Copyright (c) 2014 - 2016, Stanislav Maslan <s.maslan@seznam.cz>
//
// This file is part of the LV Process DLL.
//
// LV Process DLL interface is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LV Process DLL interface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LV Process DLL interface. If not, see <http://www.gnu.org/licenses/>.
//
//
//
// BRIEF DESCRIPTION:
// ------------------
// This library was developed for pipe communication with console application. It is basically 
// just a wrapper for nasty WINAPIs dedicated for pipe handling and process execution. The wrapper
// simplifies the whole operation to just a several functions with simple data types. So it can be 
// easily linked also to LabVIEW (that is why it is called "LV Process").
// 
// Usage it quite straightforward:
// 1) Call "proc_create()" to create instance of desired process with some setup.
//    This function will also create stdin and stdout pipes which are then used for communication with the process.
// 2) Call "proc_write_stdin()" to write string to the process' stdin.
// 3) Call "proc_peek_stdout()" to read process stdout (eventual response to 2), etc.).
// 4) Call "proc_command()" to write and read response at once.
// 5) Repeat from 2) and profit???
// 6) When you are done, terminate the process somehow. For instance send "exit\n" to stdin when you want to
//    to terminate "cmd.exe". Then call "proc_wait_exit()" to wait for process end. If it does not do so,
//    kill it with fire ... i mean "proc_terminate()". Eventually retrieve exit code by "proc_get_exit_code()".
// 
// There are several other functions exported to DLL so follow the header file for details. 
//
// The DLL also enables to create debug console. It is just a read console where you can check the stdin/stdout
// traffic. Some day I will maybe add keyboard input too.
// 
//
// lv_proc.ini:
//
//  If 'lv_proc.ini' config file is found in the folder with the 'lv_proc.dll' it will be used as a config file.
//  Supported options of the *.ini file:
//   [DEBUG]
//   ;enables logging to a "debug.log" file located in DLL's folder
//   debug_enabled = 0
//
//   [PIPES]
//   ;read and write pipe sizes (0: system decides)
//   write_pipe_buffer_size = 0
//   read_pipe_buffer_size = 0
//   
//   [READ]
//   ;read thread priority (0: normal, <-15,15> range possible)
//   thread_priority = +1
//   ;read thread idle time if no data in read pipe (1 to 100 ms)
//   thread_idle_time = 1
//   
//   [CONSOLE]
//   ;always create console (1 - overides proc_create(..., hide) parameter)
//   no_hide = 0
//   
//   ;console buffer sizes (use -1 for default, max 32767)
//   buf_size_x = 100
//   buf_size_y = 8192
//   
//   ;=== console text color scheme ===
//   ;Use standard text attribute flags:
//   ; FOREGROUND_BLUE            Text color contains blue. 
//   ; FOREGROUND_GREEN           Text color contains green. 
//   ; FOREGROUND_RED             Text color contains red. 
//   ; FOREGROUND_INTENSITY       Text color is intensified. 
//   ; BACKGROUND_BLUE            Background color contains blue. 
//   ; BACKGROUND_GREEN           Background color contains green. 
//   ; BACKGROUND_RED             Background color contains red. 
//   ; BACKGROUND_INTENSITY       Background color is intensified. 
//   ; COMMON_LVB_LEADING_BYTE    Leading byte. 
//   ; COMMON_LVB_TRAILING_BYTE   Trailing byte. 
//   ; COMMON_LVB_GRID_HORIZONTAL Top horizontal. 
//   ; COMMON_LVB_GRID_LVERTICAL  Left vertical. 
//   ; COMMON_LVB_GRID_RVERTICAL  Right vertical. 
//   ; COMMON_LVB_REVERSE_VIDEO   Reverse foreground and background attributes. 
//   ; COMMON_LVB_UNDERSCORE      Underscore. 
//   ;standard input text format
//   color_stdin = FOREGROUND_RED|FOREGROUND_INTENSITY
//   ;standard output text format
//   color_stdout = FOREGROUND_GREEN
//
// Debug:
//  If the 'debug_enabled' option is enabled in 'lv_proc.ini' the calls of the particular functions will be
//  logged to the 'debug.log' that will be created in the folder with 'lv_proc.dll'.
//---------------------------------------------------------------------------------------------------------------------
