<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="13008000">
	<Property Name="CCSymbols" Type="Str"></Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Property Name="NI.Project.Description" Type="Str"></Property>
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
		<Item Name="LV Process" Type="Folder">
			<Item Name="LV Process.aliases" Type="Document" URL="../LV Process.aliases"/>
			<Item Name="LV Process.lvlps" Type="Document" URL="../LV Process.lvlps"/>
			<Item Name="lv_proc.dll" Type="Document" URL="../lv_proc.dll"/>
			<Item Name="proc_append_error.vi" Type="VI" URL="../proc_append_error.vi"/>
			<Item Name="proc_check_exit_code.vi" Type="VI" URL="../proc_check_exit_code.vi"/>
			<Item Name="proc_cleanup.vi" Type="VI" URL="../proc_cleanup.vi"/>
			<Item Name="proc_clr_error.vi" Type="VI" URL="../proc_clr_error.vi"/>
			<Item Name="proc_command.vi" Type="VI" URL="../proc_command.vi"/>
			<Item Name="proc_create.vi" Type="VI" URL="../proc_create.vi"/>
			<Item Name="proc_demo.vi" Type="VI" URL="../proc_demo.vi"/>
			<Item Name="proc_dll_path.vi" Type="VI" URL="../proc_dll_path.vi"/>
			<Item Name="proc_flush_stdout.vi" Type="VI" URL="../proc_flush_stdout.vi"/>
			<Item Name="proc_format_error.vi" Type="VI" URL="../proc_format_error.vi"/>
			<Item Name="proc_get_exit_code.vi" Type="VI" URL="../proc_get_exit_code.vi"/>
			<Item Name="proc_handle_size.vi" Type="VI" URL="../proc_handle_size.vi"/>
			<Item Name="proc_peek_stdout.vi" Type="VI" URL="../proc_peek_stdout.vi"/>
			<Item Name="proc_terminate.vi" Type="VI" URL="../proc_terminate.vi"/>
			<Item Name="proc_vi_tree.vi" Type="VI" URL="../proc_vi_tree.vi"/>
			<Item Name="proc_wait_exit.vi" Type="VI" URL="../proc_wait_exit.vi"/>
			<Item Name="proc_write_stdin.vi" Type="VI" URL="../proc_write_stdin.vi"/>
		</Item>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
