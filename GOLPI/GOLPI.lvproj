﻿<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="13008000">
	<Property Name="CCSymbols" Type="Str"></Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Property Name="NI.Project.Description" Type="Str"></Property>
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Globals.vi" Type="VI" URL="../Globals.vi"/>
		<Item Name="GOLPI library.lvlib" Type="Library" URL="../GOLPI library.lvlib"/>
		<Item Name="LV Process.lvlibp" Type="LVLibp" URL="../LV Process.lvlibp">
			<Item Name="Application Directory.vi" Type="VI" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/file.llb/Application Directory.vi"/>
			<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
			<Item Name="Clean Up.vi" Type="VI" URL="../LV Process.lvlibp/Clean Up.vi"/>
			<Item Name="Create Process.vi" Type="VI" URL="../LV Process.lvlibp/Create Process.vi"/>
			<Item Name="Demo.vi" Type="VI" URL="../LV Process.lvlibp/Demo.vi"/>
			<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/error.llb/Error Cluster From Error Code.vi"/>
			<Item Name="Generate Error.vi" Type="VI" URL="../LV Process.lvlibp/Generate Error.vi"/>
			<Item Name="Get Exit Code.vi" Type="VI" URL="../LV Process.lvlibp/Get Exit Code.vi"/>
			<Item Name="Kill.vi" Type="VI" URL="../LV Process.lvlibp/Kill.vi"/>
			<Item Name="lv_proc.dll" Type="Document" URL="../lv_proc.dll"/>
			<Item Name="NI_FileType.lvlib" Type="Library" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/lvfile.llb/NI_FileType.lvlib"/>
			<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
			<Item Name="Pipe Handle.ctl" Type="VI" URL="../LV Process.lvlibp/Pipe Handle.ctl"/>
			<Item Name="Read Stdout.vi" Type="VI" URL="../LV Process.lvlibp/Read Stdout.vi"/>
			<Item Name="Trim Whitespace.vi" Type="VI" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/error.llb/Trim Whitespace.vi"/>
			<Item Name="Vi Tree.vi" Type="VI" URL="../LV Process.lvlibp/Vi Tree.vi"/>
			<Item Name="Wait For Exit.vi" Type="VI" URL="../LV Process.lvlibp/Wait For Exit.vi"/>
			<Item Name="whitespace.ctl" Type="VI" URL="../LV Process.lvlibp/1abvi3w/vi.lib/Utility/error.llb/whitespace.ctl"/>
			<Item Name="Write Stdin.vi" Type="VI" URL="../LV Process.lvlibp/Write Stdin.vi"/>
		</Item>
		<Item Name="Test RE.vi" Type="VI" URL="../Test RE.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="Packed library" Type="Packed Library">
				<Property Name="Bld_buildCacheID" Type="Str">{7DE983DA-D699-4748-8190-D73AFF3CD1BC}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">Packed library</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/GOLPI [VersionNumber]</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{369EFAE7-7719-4A46-B6B4-E54D725C2DB5}</Property>
				<Property Name="Bld_version.minor" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">GOLPI Library.lvlibp</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/GOLPI [VersionNumber]/GOLPI Library.lvlibp</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/GOLPI [VersionNumber]</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="PackedLib_callersAdapt" Type="Bool">true</Property>
				<Property Name="Source[0].itemID" Type="Str">{3D2C123D-847C-462C-9729-0D62C2E5A6F4}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/GOLPI library.lvlib</Property>
				<Property Name="Source[1].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[1].Library.atomicCopy" Type="Bool">true</Property>
				<Property Name="Source[1].Library.LVLIBPtopLevel" Type="Bool">true</Property>
				<Property Name="Source[1].preventRename" Type="Bool">true</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">Library</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/LV Process.lvlibp</Property>
				<Property Name="Source[2].preventRename" Type="Bool">true</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[3].itemID" Type="Ref">/My Computer/LV Process.lvlibp/lv_proc.dll</Property>
				<Property Name="Source[3].sourceInclusion" Type="Str">Include</Property>
				<Property Name="SourceCount" Type="Int">4</Property>
				<Property Name="TgtF_companyName" Type="Str">CMI</Property>
				<Property Name="TgtF_fileDescription" Type="Str">GNU Octave Labview Pipes Interface</Property>
				<Property Name="TgtF_internalName" Type="Str">GOLPI</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2013 CMI</Property>
				<Property Name="TgtF_productName" Type="Str">GOLPI</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{2F2E515A-2FD4-4DD9-8DED-E33A0170DF60}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">GOLPI Library.lvlibp</Property>
			</Item>
		</Item>
	</Item>
</Project>