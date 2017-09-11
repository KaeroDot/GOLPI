## Copyright 2017 Stanislav Maöl·Ú
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
## @deftypefn {Function file} golpi_conv_struct (@var{Data}} )
## Function codes structure content into binary stream of 'uint8'.
## This is useful for simple transfer of structures to LabVIEW via GOLPI 
## (Gnu Octave to Labview Pipes Interface). It is recoursive function
## so it is possible to transfer nested structures. 
##
## Format of the output stream:
##  uint32(number of items in the structure),
##  for each item:
##   uint8(length of name of the item)
##   string(name of the item with no '\0' at the end, if @var{Data} was char
##          string this string will be empty)
##   uint8(item data type)
##   uint32(rows count)
##   uint32(columns count)
##   optional for array/cell array of items - for each cell/array item:
##    uint32(relative byte offset from item record start to binary item data)
##   uint8(item data converted to uint8, little-endian)
##
## The output stream is returned converted to vector of uint8.
##
## Item data type format 0bC000TTTT:
##  C    - is complex
##  TTTT - data type:
##   0 - int32
##   2 - single real
##   3 - double real
##   4 - single string
##   5 - array or cell array of items
##
## When structure item is another structure or array of structures or 
## cell array of anything the function is called recoursively, i.e.
## the binary item data will be output of the nested call of this function.
## Unlimited nested structures are supported.     

function [bin] = golpi_conv_struct(data)  
  
  % initialize compressed data output variable
  bin = [typecast(uint32(0),'uint8')(:)];
  
  % items count
  ic = 0;
  
  if ischar(data)
    % convert string to fake struct with single string item
    str = data;
    data = struct();
    data.str = str;
    fake_str = 1;    
  else
    fake_str = 0;
  endif
  
  if isstruct(data)
  
    % get struct item names
    items = fieldnames(data);
    
    % items count
    I = numel(items);
  
    for k = 1:I
      
      % get item
      it = getfield(data,items{k});
         
      if ismatrix(it) && isstruct(it)
        % --- array of structs
        
        % convert to cell array of structs
        it = num2cell(it);
        
      endif
        
      if isnumeric(it) && numel(size(it)) <= 2
        % --- numeric items up to 2D matrix
        
        % identify type
        if isa(it,'int32')
          ty = 0;
        elseif isa(it,'single')
          ty = 2;
        elseif isa(it,'double')
          ty = 3;
        else
          ty = -1;
        endif
        
        % is complex?
        if ty >=0 && iscomplex(it)
          ty += 128;
        endif
        
        if ty >= 0
          % supported type
      
          % matrix size
          sz = size(it);
          
          % convert the data to stream
          itd = [uint8(numel(items{k}))(:);
            typecast(items{k},'uint8')(:);
            uint8(ty);
            typecast(uint32(sz),'uint8')(:);
            typecast(it(:),'uint8')(:)];
            
          % add the data to output stream
          bin = [bin;typecast(uint32(numel(itd)),'uint8')(:);itd];
          
          % items count in the stream
          ic++;
          
        endif
      
      elseif ischar(it) && isvector(it)
        % --- string (single vector of char)
        
        varname = items{k};
        if fake_str
          varname = ''; % store no var name if it is fake structure containing only string 
        endif
                
        % matrix size
        sz = size(it);
        
        % convert the data to stream
        itd = [uint8(numel(varname))(:);
          typecast(varname,'uint8')(:);
          uint8(4); % type id
          typecast(uint32(sz),'uint8')(:);
          typecast(it,'uint8')(:)];
        
        % add the data to output stream
        bin = [bin;typecast(uint32(numel(itd)),'uint8')(:);itd];

        % items count in the stream
        ic++;
      
      elseif iscell(it) && numel(size(it)) <= 2
        % --- cell array up to 2D
        
        % array size
        sz = size(it);
        
        % convert data to 1D
        cdata = {it{:}};
        
        % data items count
        C = numel(cdata);
        
        % build this item header
        sub = [typecast(uint32(0),'uint8')(:); % array item size in bytes
          uint8(numel(items{k}))(:); % var name string len
          typecast(items{k},'uint8')(:); % var name ascii data
          uint8(5); % id of cell array of something complex (structs or whatever)
          typecast(uint32(sz),'uint8')(:); % array dimensions
          typecast(uint32(zeros(C,1)),'uint8')(:)]; % list of byte offsets of each cell in the cell array data
  
        % cell data offsets
        cell_00 = numel(sub);
        cell_offsets = zeros(C,1);
  
        % convert each item of the cell array 
        for c = 1:C
          
          % store data offset of this cell
          cell_offsets(c) = numel(sub) - cell_00;          

          % convert item data
          idata = golpi_conv_struct(cdata{c});
          
          % append it to the 
          sub = [sub;idata];
           
        endfor
        
        % store cell array total size in bytes
        sub(1:4) = typecast(uint32(numel(sub)-4),'uint8')(:);
        
        % store cell offsets list
        sub(cell_00 - C*4 + 1:cell_00) = typecast(uint32(cell_offsets),'uint8')(:);
       
        % append to data stream
        bin = [bin;sub];
        
        % items count in the stream
        ic++;
      
      endif
         
    endfor
    
  endif
  
  % store items count
  bin(1:4) = typecast(uint32(ic),'uint8')(:);

endfunction


