<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="lv_process" Type="Folder">
			<Item Name="proc_demo.vi" Type="VI" URL="../lv_process/proc_demo.vi"/>
			<Item Name="proc_vi_tree.vi" Type="VI" URL="../lv_process/proc_vi_tree.vi"/>
			<Item Name="proc_clr_error.vi" Type="VI" URL="../lv_process/proc_clr_error.vi"/>
			<Item Name="proc_format_error.vi" Type="VI" URL="../lv_process/proc_format_error.vi"/>
			<Item Name="proc_append_error.vi" Type="VI" URL="../lv_process/proc_append_error.vi"/>
			<Item Name="proc_handle_size.vi" Type="VI" URL="../lv_process/proc_handle_size.vi"/>
			<Item Name="proc_DLL_path.vi" Type="VI" URL="../lv_process/proc_DLL_path.vi"/>
			<Item Name="proc_create.vi" Type="VI" URL="../lv_process/proc_create.vi"/>
			<Item Name="proc_cleanup.vi" Type="VI" URL="../lv_process/proc_cleanup.vi"/>
			<Item Name="proc_terminate.vi" Type="VI" URL="../lv_process/proc_terminate.vi"/>
			<Item Name="proc_get_exit_code.vi" Type="VI" URL="../lv_process/proc_get_exit_code.vi"/>
			<Item Name="proc_check_exit_code.vi" Type="VI" URL="../lv_process/proc_check_exit_code.vi"/>
			<Item Name="proc_wait_exit.vi" Type="VI" URL="../lv_process/proc_wait_exit.vi"/>
			<Item Name="proc_write_stdin.vi" Type="VI" URL="../lv_process/proc_write_stdin.vi"/>
			<Item Name="proc_flush_stdout.vi" Type="VI" URL="../lv_process/proc_flush_stdout.vi"/>
			<Item Name="proc_peek_stdout.vi" Type="VI" URL="../lv_process/proc_peek_stdout.vi"/>
			<Item Name="proc_command.vi" Type="VI" URL="../lv_process/proc_command.vi"/>
		</Item>
		<Item Name="lv_proc.dll" Type="Document" URL="../lv_proc.dll"/>
		<Item Name="octave_demo.vi" Type="VI" URL="../octave_demo.vi"/>
		<Item Name="octave_var.vi" Type="VI" URL="../octave_var.vi"/>
		<Item Name="octave_vi_tree.vi" Type="VI" URL="../octave_vi_tree.vi"/>
		<Item Name="octave_open.vi" Type="VI" URL="../octave_open.vi"/>
		<Item Name="octave_close.vi" Type="VI" URL="../octave_close.vi"/>
		<Item Name="octave_command.vi" Type="VI" URL="../octave_command.vi"/>
		<Item Name="octave_cmd_get_version.vi" Type="VI" URL="../octave_cmd_get_version.vi"/>
		<Item Name="octave_check_exit_code.vi" Type="VI" URL="../octave_check_exit_code.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="octave_demo" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{65B67AEA-6663-43C9-903D-7A9121F855C1}</Property>
				<Property Name="App_INI_GUID" Type="Str">{764B1F88-88DD-420A-B98F-3C9B324E4A70}</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{24AD5B18-8F46-449D-B2AA-7F3E0199C360}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">octave_demo</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/NI_AB_PROJECTNAME/octave_demo</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{E5159307-D6F5-4B48-9DF4-EF958B07B91F}</Property>
				<Property Name="Destination[0].destName" Type="Str">octave_demo.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/NI_AB_PROJECTNAME/octave_demo/octave_demo.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/NI_AB_PROJECTNAME/octave_demo</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{D4873213-5C8B-4B9B-A8EB-07860BCE9117}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/octave_demo.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/lv_proc.dll</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">Include</Property>
				<Property Name="SourceCount" Type="Int">3</Property>
				<Property Name="TgtF_companyName" Type="Str">CMI</Property>
				<Property Name="TgtF_fileDescription" Type="Str">octave_demo</Property>
				<Property Name="TgtF_fileVersion.major" Type="Int">1</Property>
				<Property Name="TgtF_internalName" Type="Str">octave_demo</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2013 - Stanislav Maslan.</Property>
				<Property Name="TgtF_productName" Type="Str">octave_demo</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{A5068D92-B4D3-42D3-8381-BC9A9F985960}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">octave_demo.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
