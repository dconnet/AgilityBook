' Windows Installer utility to print productcode
' For use with Windows Scripting Host, CScript.exe or WScript.exe
' Copyright (c) David Connet. All rights reserved.
' (Based on WiLangId.vbs)
'
Option Explicit

Const msiOpenDatabaseModeReadOnly     = 0

Dim argCount:argCount = Wscript.Arguments.Count
If argCount > 0 Then If InStr(1, Wscript.Arguments(0), "?", vbTextCompare) > 0 Then argCount = 0
If (argCount = 0) Then
	message = "Windows Installer utility to print the ProductCode for a package."
	Wscript.Echo message
	Wscript.Quit 1
End If

' Connect to Windows Installer object
On Error Resume Next
Dim installer : Set installer = Nothing
Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError

' Open database
Dim databasePath:databasePath = Wscript.Arguments(0)
Dim database : Set database = installer.OpenDatabase(databasePath, msiOpenDatabaseModeReadOnly) : CheckError

' Get ProductCode property from Property table
Dim view : Set view = database.OpenView("SELECT `Value` FROM `Property` WHERE `Property` = 'ProductCode'")
view.Execute : CheckError
Dim record : Set record = view.Fetch : CheckError
Dim result : Set result = ""
If record Is Nothing Then result = "ERROR!" Else result = record.StringData(1)

Set database = nothing
Wscript.Echo result
Wscript.Quit 0

Sub CheckError
	Dim message, errRec
	If Err = 0 Then Exit Sub
	message = Err.Source & " " & Hex(Err) & ": " & Err.Description
	If Not installer Is Nothing Then
		Set errRec = installer.LastErrorRecord
		If Not errRec Is Nothing Then message = message & vbNewLine & errRec.FormatText
	End If
	Fail message
End Sub

Sub Fail(message)
	Wscript.Echo message
	Wscript.Quit 2
End Sub
