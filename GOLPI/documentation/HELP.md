# How to use Gnu Octave to Labview Pipes Interface

## Installation
Add 'GOLPI Library.lvlibp' to your project. Put 'lv_proc.dll' to your project directory (otherwise
you have to wire path to the dll into 'Start GNU Octave.vi'). Install GNU Octave. If you want
transfer data from LabVIEW to GNU Octave and back very quickly, install package 'golpi-X.X.X.tar.gz'
into GNU Octave.

## Use
Take a look at examples included in the library.

1. Start GNU Octave by 'Start GNU Octave.vi'. You have to wire in path to the directory containing
GNU Octave binary, i.e. octave-cli.exe (or octave.exe in versions prior 4.0). Wire in your working
directory, i.e. the working directory of GNU Octave. Usually it is the one with your data files or
script files.

2. You can send commands to GNU Octave using 'Write Command.vi'.

3. You can read output of GNU Octave using:
  1. 'Read Once.vi' - Reads selected number of bytes from available output.
  2. 'Read Full Output.vi' - Reads whole available output.
  3. 'Read Output Till Settle.vi' - Reads output and stops only if the output is not changing
  anymore. This is useful when waiting for the finish of some script, however the script
  have to periodically report its status.
  4. 'Read Output Till Keyword.vi' - Reads output and stops if some keyword is found. This is
  useful when waiting for end of some script and some keyword is shown after the end.

4. You can set value of some variable in GNU Octave user space using 'Set Variable.vi'. It is easier
than using 'Write Command.vi'. Two modes of transfer are available:
  1. Through console (default).
  2. Through temporary file. This is useful in the case of large data.

5. You can get value of some variable in GNU Octave user space using 'Get Variable.vi'. It is easier
than using e.g. 'Read Full Output.vi'. Three modes of transfer are available:
  1. Through console (default).
  2. Through temporary file. This is useful in the case of large data.
  3. Through console using bitstream mode. It is faster method of transfer than normal console,
  however you have to install pacakge 'golpi-X.X.X.tar.gz' into GNU Octave or place script
  'golpi_data2bits.m' into GNU Octave path. This mode is set by 'Set Bitstream Mode.vi'.

6. Bitstream mode of transfer data through console can be set by 'Set Bitstream Mode.vi'. See above
for details.

7. Output of GNU Octave can be checked for errors using 'Check Output for Octave Errors.vi'.

8. Running status of GNU Octave can be checked by 'Check Status.vi'.

9. Debugging can be switched on by 'Set Debug Mode.vi'. All data transfered from and to Gnu Octave
are saved in the GOLPI reference wire.

10. GNU Octave can be quitted using 'Quit GNU Octave.vi'.

## Examples

See included examples to see basic and advanced use.
