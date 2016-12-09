## Copyright 2016 Stanislav Mašláň, Martin Šíra
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation version 3 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

## -*- texinfo -*-
## @deftypefn {Function file} golpi_data2bits (@var{Data}, @var{ExpDataType}, @var{Dim} )
## Function codes variable content into a series of 7 bit characters and stream it to stdout.
## This is usefull for very fast transfer of data through stdout and is used in project GOLPI 
## (Gnu Octave to Labview Pipes Interface). Stream is in big endian coding. The stream starts
## with a header of following format: 
## @code{_GpBiT_(SS)(T)(NNNNNNNN)[(SSSSSSS1)[(SSSSSSS2)[...]]]},
## where (SS) is hexadecimal size of one element of array,
## (T) is determined hex. type of variable in coding of @var{ExpDataType},
## (NNNNNNNN) is hex. number of maximum dimensions in coding of @var{Dim},
## (SSSSSSS1) is size of first dimension,
## (SSSSSSS2) is size of second dimension, etc.
##
## If variable cannot be converted into @var{ExpDataType}, (T) in header is set to
## zero. If variable @var{Data} has more dimensions than is set in @var{Dim}, (T) in header is set
## to zero to mark error.
##
## Inputs:
## @table @samp
## @item @var{Data} - Variable to encode
## @item @var{ExpDataType} - What is expected data type of the variable.
##      1: signed integer,
##      2: unsigned integer,
##      3: real float,
##      4: complex float,
##      5: string,
##      6: real numeric (integer or float).
## @item @var{Dim} - Maximum expected dimensions of @var{Data}
##      0: scalar,
##      1: vector,
##      2: matrix,
##      etc.
##      
## @end table
##
## Example:
## @example
## @group
## golpi_data2bits([1:5], 6, 1)
## @result{} _GpBiT_083000000020000000100000005o�30000000100000000r0000000042000000085000000000
## @end group
## @end example
## 
## @seealso{GOLPI}
## @end deftypefn

function golpi_data2bits(Data,ExpDataType,Dim) 

  % object size
  sz=size(Data);
  % elements count
  nn=numel(Data);
  % dimensions
  dn=sum(sz>1);
  % element byte size
  bsz=0;
  if(nn)
    bsz=sizeof(Data)/nn;
  endif
  % idnetify data type
  tok=0;
  if(((isa(Data,'int8')||isa(Data,'int16')||isa(Data,'int32')||isa(Data,'int64'))&&(ExpDataType==1))||((isa(Data,'uint8')||isa(Data,'uint16')||isa(Data,'uint32')||isa(Data,'uint64'))&&(ExpDataType==2))||(isfloat(Data)&&isreal(Data)&&ExpDataType==3)||(iscomplex(Data)&&ExpDataType==4)||(ischar(Data)&&ExpDataType==5))
        tok=ExpDataType;
  endif

  if(~tok && (ExpDataType==6) && isnumeric(Data) && isreal(Data))
    % generic real numeric transfer - identify actual type
    if(isa(Data,'int8')||isa(Data,'int16')||isa(Data,'int32')||isa(Data,'int64'))
      tok = 1;
    elseif(isa(Data,'uint8')||isa(Data,'uint16')||isa(Data,'uint32')||isa(Data,'uint64'))
      tok = 2;
    elseif(isfloat(Data))
      tok = 3;
    endif
  endif

  % check dimensions
  if(dn>Dim)
    tok=0;
  endif;

  % send stream header
  printf('_GpBiT_%02X%01X',bsz,tok);
  printf('%08X',[numel(sz),sz]);

  if(tok&&nn)
    % convert data to bitstream
    str=reshape(reshape(typecast(Data,'uint8'),[bsz,nn])(bsz:-1:1,:),[1,bsz*nn]);
    str=[str zeros(1,rem(7-rem(numel(str),7),7))];
    str=[bitand(str(1:7:end),0x7F);bitshift(bitand(str(1:7:end),0x80),-7)+bitshift(bitand(str(2:7:end),0x3F),1);bitshift(bitand(str(2:7:end),0xC0),-6) + bitshift(bitand(str(3:7:end),0x1F),2);bitshift(bitand(str(3:7:end),0xE0),-5)+bitshift(bitand(str(4:7:end),0x0F),3);bitshift(bitand(str(4:7:end),0xF0),-4)+bitshift(bitand(str(5:7:end),0x07),4);bitshift(bitand(str(5:7:end),0xF8),-3)+bitshift(bitand(str(6:7:end),0x03),5);bitshift(bitand(str(6:7:end),0xFC),-2)+bitshift(bitand(str(7:7:end),0x01),6);bitshift(bitand(str(7:7:end),0xFE),-1)];
    str=char(reshape(str,[1,numel(str)])+'0');

    % transfer stream
    nn=numel(str);
    bs=4096;
    n=1;
    while(n<=nn)
      printf('%s',str(n:min(n+bs-1,nn)));n+=bs;
    endwhile;
  endif;

  % mark end of stream
  printf('\n');

endfunction
