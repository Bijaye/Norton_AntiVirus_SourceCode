[Application]

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; SAMPLE .ZAP FILE FOR SYMANTEC ANTIVIRUS CORPORATE EDITION
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

; Only FriendlyName and SetupCommand are required,
; everything else is optional.

; FriendlyName is the name of the program that 
; will appear in the software installation snap-in 
; and the Add/Remove Programs tool.

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; REQUIRED
FriendlyName = "<Insert application name you wish to appear here>"

;SAMPLE FriendlyName = "Symantec AntiVirus Client"
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


; SetupCommand is the command line used to 
; Run the program's Setup. If it is a relative 
; path, it is assumed to be relative to the 
; location of the .zap file.
; Long file name paths need to be quoted. For example:
; SetupCommand = "long folder\setup.exe" /unattend
; or
; SetupCommand = "\\server\share\long _
; folder\setup.exe" /unattend

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; REQUIRED
SetupCommand = "<\\servername\share\path\application executable>"

;SAMPLE  SetupCommand = "\\dpadvsvr\Managed\WIN32\SETUP.EXE"
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


; Version of the program that will appear 
; in the software installation snap-in and the 
; Add/Remove Programs tool. 

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; OPTIONAL
DisplayVersion = <version number you wish displayed>

;SAMPLE  DisplayVersion = 8.0
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

; Version of the program that will appear 
; in the software installation snap-in and the 
; Add/Remove Programs tool. 

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; OPTIONAL 
Publisher = Symantec Corporation
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~