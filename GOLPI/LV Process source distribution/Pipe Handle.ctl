RSRC
 LVCCLBVW  )0  8      )      LV Process library.lvlib      � �  0           < � @�      ����            �
FQC�s�u�xxX          %G��N�#g����ُ ��	���B~       [��]��yE�2� r���   K�r���Xl_p�V   M��T�}	a�t��~�          l LVCC(LV Process library.lvlib:Pipe Handle.ctl       VILB    PTH0      LV Process library.lvlib             &   ,x�c�g`j`�� Č ��������!�' �nE     M  $x�c`��� H1200��lh�`Ʀ6@6��셊31��~xc���{���
@7�J�@ �J)X      8 VIDS(LV Process library.lvlib:Pipe Handle.ctl          ,  �x��U_h[U>�=7�Y3����hqiG`���Р�&Kog���)8:A���?)%��͕d�K_��@�Q�8�f�T������A�B/+ll�_��;7��t����������|�dGg������Ɔ5�dU��;o�=�0g�s5���B�6�
Oj��݋ɢe�+9$jO`E:˝�<٤��V�h4r��²�k˲�h�-�]Ń�?_|IY�#@{1��8���wTmJP6��^>����
|��yQ��^Sx�
/�w8L�Xt[\���B��)Ǩ�W����ݎ��� Z������b����	ڴ��9��=ͻ#j	j�1�����mF�Vm;�]jb�
,s��\ŲE-���� 3��{}�Y����s"���`��4��%gQ��1UL���8yK�?Ǹ%�s��`M��	c
D�������m�>c�6�o�!ik������)��z����᰾��v�6���ҙ6��t@'nTWc�{��S�{��+ަ=�4p������b�B܍��3| :&��p}[�HoQ�&�S�ZJ�ťf�
{�(�ʲ\:����0R�1���ˁP}�qt.�_
' Yd�$K	B>�~ç+VF�t����Vf���NG�hK��l��E�㣹q���u4rD�u������L1�����
*��;��PR��q&>iUW�������?��h���sjKT���7(w;JG�+8�+xB|',8-#:���"Y>N��q;|�^�A�S�bL�(��Eu�m��%�I<%#7k���%������Ƒ܈���/�����F�����	Z�o�[��_0NY��k���)�r�1Q��)�x�W7R)�0&�������zB'���X�"l�)��&)Xo���ҙW۷���D�w.�&�[�Joh	Ǔ�b<E��~R�w��B�c�z��� #{��c'>S�\�	Vj���f��r���6+���?��������n �fi�+�$�M�8�
8<<L|�<"h,�6�p�U�! %:����_�������ߺ�D���z
�������?I���      	x�c```dd         �G  13.0     �   13.0     �G  13.0     �   13.0     �G  13.0                      6  2Pipe handle of the LV Process library. Contains path to lv_proc.dll library and handle of the process according to windows API. If you wish to access to newly craeted proces instance by your own VI's use process instance handle which has following structure (C/C++ style):

typedef struct{
  HANDLE proc_handle;
  HANDLE main_thread_handle;
  DWORD process_id;
  DWORD thread_id;
  HANDLE stdout_pipe[2];
  HANDLE stdin_pipe[2];
}TPHndl;

where [1] ends of the pipes are assigned to the new process instance and [0] ends are accessible from the caller (use these to communicate).

Handle also can store debug data.

---
LV Process works only with lv_proc.dll library, which is part of vipm package or should be delivered together with packed library. Placement of this dll is selected in terminal of Create Process.vi.     ������  �  ���������䡡�����  �  �  �  �  �  �  �  �  �����(��������  �  �s�ݥJ%�s%��B%�C�р  �  ����   ���������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������   8 FPHP(LV Process library.lvlib:Pipe Handle.ctl          	�  !�x��Z}lS�?�yN�??'8�4Ď���F�(+���1xMW�Ҕ
�V�CR�	"���Ԣ�J�nhݐ�F��[��mt��n[ɚ�n��MSG�� ���&�*�}y;�]?��>�g�jĕ���=�w>� M���=���x	�ܣ�?�� fZ�Z�A��>n^�S`X����r
S��]��H�Ϊ?�� i�Ca~B�l4�Vʉ��i«K��^nֳI�(�͌�B�,"g���Nn��L��w8��ȯ�V!���) ���R�&������*�<W�������0ej��XNxΣX��,�S+g�J�%�_�A=��G�Ͼ@l'LV�:9+"+��֯|^�%ǕU�ٳg��<߀��j!&�I��Vߟy8�2'�0[����q��בXh�)�5���n�g���d=[A�s��_��Ń����uj���"YQ: 	�M2?��ܑN�����*g}��ŉxP<�	��:z&''��x�m��r��~�Spv�p6�j8�ftZ�fh~����5����5Ю��{�sD;3�;�a�j�	Ʒ\/Et=e�1��i�X��R&���uΕ�Y�<O�`<�zN�zJ�x��Ӻ�����o�È�V�3�q0�~L�|T}�p���1(f�h�z^�w����_ty��?�޶k{"�;�k�G�Ϧ�A�N9_-n�&n!�E�Y�5;���
L���k���x���uϨa�3W�\A��I�4�ޓ��/���Xs[{^P���R�B����P���%9r%���)1��V$f��5H��H��)&�a�&$�x;)�X9ͤ��a�W�5͚���k�nM_�����A~�y�)P0.��a/[ii�h���G�H�F�h>�iЗ��Cr|9Gͥ.
����-0p�u�
��ӓ?
'^
e��
���'�ϑ盅��哿Ǭ�R��}�����à���}���� �9�2^���XT���L�ۤҮ3e���Z�u���|�*��x���٘�?��g-v�O���O����C����B�F?��#�9ct��:�n��3�|�;���:�+�	�)�e~`�L�ȵ^)�3i�L��g���Tg:d�L���R��%:�Ǵ3=��H=���T��ݏA��I��z\=O�cp̡3�hQ���l&� րo#�VH�+P��X�$��_N�/�k��Z�Ie%W��Ze��o3ߏ1��b�+�y��y��9J�xPO��5\=�?Ц0%�3��8O�:�4N'�������)D����e3�-���X���a*���P���F����.-�=����Y���c��aKı���7�7@D���{���_@Q�4�`4�0����&r鐽�RҔJH�t'�v��\q���<��7�[������x�DP!i"$i�5&R�|�s'-��5͠k�<��*���?��rX�SYh(��zB���ʪt����-멖I�>j-�8�&�k�y~�q������=g�ͷ7�}f��y���J��yr�F�cn�c��N���mrDk~N�a��Z�8wk��b�l�α���� ��K��r4��VƂ�8�6�ab0��ƣ_�5�p-SA�'�.�]�D7����#�-�tk��[���K&�"���%��Bݯ���·t�>�|�Lܫ�t��GJ��W���$�ү�ɏ:�1���ޱ]��2��W�n�����b�����_�k��ר�����_�Ka�<;<��KX�}�I���uIY����ۍL��ÍX�:��I B���{�(���%�v�l~?�w�g���?`�-�mͤ�1���~�cr%�v���L�f&��q��-��2jV�[�HJ�_��eo�$��&�H �7����1֚��#�h-�Ӛ���ӭW�^�T}�}��N�wP�9�d?`��{��AEw��K��&�G�z����R��rXH�O�u!�������D-�6|�R=s]HmtXHL�Ͷ���B�A�B�
Zq��s_H=d_HUA���\��m!U��s���m!Uc�St!��e!U�!	��}��a!�R�h�p��X]f��SƖ����@��u�"%�>��1gc�!���^���֜��ߪ�^�vʹeI���;X���i�К�ܭq� Ŭ1W���m�Q��~�6ūo0.�ی�73�ۘ���㤹N:��p�;fa;V�b��DKZ�Ԗ$\���+SY/��ߴ����u\c��Co��ֈ=�������P�p(���;6`A?jZ��FE[�B�}N�V��OSÃD^%I"�3@z�sB�*:�6��A�Qƒ��i��V�Y/N3�eA����:$`I��e��a?� Ϙ_����04������q�/�M�3�?�'�G��笓�]L�WѼ��w/�Z�'��t!�	���9>�'2M�`&����v4�,t����|����ݒ��"�D�V "��=.ލ�
ww�=��@�pP�4���������9����I/ƅ���w{       �   8 BDHP(LV Process library.lvlib:Pipe Handle.ctl           b   rx�c``��`��P���I�+�!���YЏ�7���a �( 	����.��>��� �l���9�2-�����z�\�8Se�<� b           �      NI_IconEditor �     @0����Data      h13008047    Load & Unload.lvclass       	  6ddPTH0                 Layer.lvclass         �          � (  �                 ���                                                                                                     ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������      ������������������������������������������������������������������������������������������                                                                                                   ��������������������������������������������������������������������������������������������������������������������������������   Filledd    LV PROC       pipe   typedef                   Small Fonts 	                 �   (                                       �  yx�uR�N�@=�-`A����ĸ����c�je
4�Bt���qo�gJ�co���s�gN�!v������G�VSi-ڑ�0��p�R�2$X���"���
E7�+�$�
�'��(�kd�8�q�ɺ;�U�GQ�>�
� �J6ɤ���|^�W�o�ϧ/ �7�O���~��z�ղCq��j�b�������M%�xP��οK�l�P���C�yev[w��O��*�%�A�v�����U{���!q�}��d�:2�MD�R�Tm����&V-�(�����ְ���.3�K���w��*��=RM�������7RObf���Ĕ=,"�%��m��`n��C"E��>C9f�a|+�'�[w�   _       H       � �   O       � �   V       � �   ]� � �   � �TahomaTahomaTahoma0 RSRC
 LVCCLBVW  )0  8      )               4  (   LIBN      `LVSR      tRTSG      �OBSG      �CCST      �LIvi      �CONP      �TM80      �DFDS       LIds      VICD      (GCDI      <vers     PGCPR      �STRG      �ICON      �icl8      �LIfp      FPHb      FPSE      ,LIbd      @BDHb      TBDSE      hVITS      |DTHP      �MUID      �HIST      �VCTP      �FTAB      �    ����                           $        ����       �        ����       �        ����       �        ����       �        ����      P        ����      X        ����      �        ����      �        ����              ����      D       ����      \       ����      l       ����      |       	����      �       
����      �        ����      �        ����      �        ����      	�        ����      
�        ����      �        ����      �        ����      �        ����      �        ����      �        ����      <        ����      D        ����      &�        ����      &�        ����      &�        ����      '$       �����      (�    Pipe Handle.ctl