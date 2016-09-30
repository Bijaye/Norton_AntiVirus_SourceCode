//-----------------------------------------------------------------------
//
//  Help for NORTON
//  Copyright 1992 by Symantec Corporation
//
//  $Log:   N:/NORTON/VCS/NORTON.HSV  $
//  
//     Rev 1.32   14 Feb 1995 09:06:40   TMAGILL
//  Update copyright
//  
//     Rev 1.31   03 Nov 1994 17:33:34   SCOTTP
//  FIXED TYPO
//  
//     Rev 1.30   01 Mar 1994 11:58:18   JOHN
//  Removed bogus EOF marker AGAIN!
//  
//     Rev 1.29   27 Feb 1994 01:42:06   TMAGILL
//  Fixed spelling error in credits
//  
//     Rev 1.28   26 Feb 1994 03:08:44   TMAGILL
//  Update credits
//  
//     Rev 1.27   20 Feb 1994 05:23:46   TMAGILL
//  Update credits
//  
//     Rev 1.26   16 Feb 1994 14:31:58   JOHN
//  Removed bogus EOF marker.
//  
//     Rev 1.25   01 Feb 1994 22:29:32   TMAGILL
//  Updated to version 8.0
//  
//     Rev 1.24   18 Mar 1993 13:18:10   JOEP
//  Added new programmers name
//  
//     Rev 1.23   17 Mar 1993 12:06:26   JOEP
//  Added new name
//  
//     Rev 1.22   12 Mar 1993 19:53:32   JOHN
//  Updated credits.
//  
//     Rev 1.21   10 Mar 1993 17:18:42   JOHN
//  Updated credits.
//  
//     Rev 1.20   08 Mar 1993 14:00:16   ROB
//  Entered edits.
//  
//     Rev 1.19   24 Feb 1993 14:48:00   JOHN
//  Updated credits.
//  
//     Rev 1.18   23 Feb 1993 15:46:00   JOHN
//  Updated credits.
//  
//     Rev 1.17   22 Feb 1993 19:21:42   JOHN
//  Added ES team to credits.
//  
//     Rev 1.16   22 Feb 1993 16:00:32   JOHN
//  Added QA team to credits.
//  
//     Rev 1.15   22 Feb 1993 13:32:16   JOHN
//  Added more names to credits.
//  
//     Rev 1.14   16 Feb 1993 21:40:52   JOHN
//  First pass at adding team credits.  More names to follow...
//  
//     Rev 1.13   29 Dec 1992 16:35:56   ROB
//  Scorpio upgrade.
//  
//     Rev 1.12   11 Nov 1992 10:31:52   SKURTZ
//  Added DUPDISK program
//
//     Rev 1.11   29 Oct 1992 17:54:58   JOHN
//  Added RESCUE program
//
//     Rev 1.10   19 Oct 1992 17:33:24   JOHN
//  Fixed comment prefix characters
//
//     Rev 1.9   16 Oct 1992 21:32:42   JOHN
//  Quickly converted to compile with Hyperlink Help; still needs work!
//
//     Rev 1.8   23 Sep 1991 17:14:44   JOHN
//  Corrected hotkeys of items on the "Menu" menu.
//
//     Rev 1.7   20 May 1991 16:15:32   JOHN
//  Fixed typo: changed "you,ve" to "you've".
//
//     Rev 1.6   18 May 1991 18:18:38   JOHN
//  Changed FileSave to Erase Protect
//
//     Rev 1.5   17 May 1991 22:48:48   JOHN
//  Changes for NU 6.0.
//
//     Rev 1.4   25 Apr 1991 10:40:58   ED
//  Added help frames for FA, FS, FL, FD, and TS
//
//     Rev 1.3   24 Apr 1991 12:16:14   ED
//  Added a help ID for Directory Sort
//
//     Rev 1.2   11 Feb 1991 13:03:06   JOHN
//  Added place holders for new help topics.
//  Tech Pubs still needs to write the actual help text.
//
//     Rev 1.1   17 Dec 1990 18:41:18   BRAD
//  Updated Help Text
//
//     Rev 1.0   04 Oct 1990 13:34:18   JOHN
//  Initial revision.
//
//-----------------------------------------------------------------------

#PRODUCTNAME "NORTON"
#COPYRIGHT   "Copyright 1991-1995 by Symantec Corporation"
#VERSION     0x0800
#OUTPUTFILE  "NORTON.HLP"
#HEADERFILE  "NORT-HLP.H"
#COMPRESS    OFF

#INCLUDE GLOBAL.IHS

#DEFINE H_NI_INDEX
#SETINDEX
#TITLE "Norton Utilities
^bAbout^b
 [ The Norton Utilities     :H_NI_MAIN]
^bMenu Commands^b
 [ Sort by Name       Alt+N :H_NI_MENU_SORT_NAME]
 [ Sort by Topic      Alt+T :H_NI_MENU_SORT_TOPIC]
 [ Add Menu Item...         :H_NI_MENU_ADD_MENU_ITEM]
 [ Edit Menu Item...        :H_NI_MENU_EDIT_MENU_ITEM]
 [ Delete Menu Item...      :H_NI_MENU_DEL_MENU_ITEM]
 [ Exit               Alt+X :H_NI_MENU_EXIT]
^bConfiguration Commands^b
 [ Video and Mouse...       :H_NI_CONF_VIDEO_MOUSE]
 [ Printer Configuration... :H_NI_CONF_PRINTER]
 [ Startup Programs...      :H_NI_CONF_STARTUP_PROGS]
 [ Alternate names...       :H_NI_CONF_ALT_NAMES]
 [ Passwords...             :H_NI_CONF_PASSWORDS]
 [ Menu editing...          :H_NI_CONF_MENU_EDIT]
^bHelp Commands^b
 [ The Norton Advisor       :H_NI_HELP_ADVISOR]
 [ Index                    :H_NI_HELP_INDEX]
 [ Using the Keyboard       :H_GLOBAL_USING_KEYB]
 [ Using a Mouse            :H_GLOBAL_USING_MOUSE]
 [ Using Dialog Boxes       :H_GLOBAL_USING_DIALOGS]
 [ Using Pull-down Menus    :H_GLOBAL_USING_MENUS]
 [ How to use Help          :H_GLOBAL_HELP_ON_HELP]
 [ About...                 :H_NI_HELP_ABOUT]
^bThe Utilities^b
 [ Batch Enhancer           :H_NI_BE]    BE
 [ Calibrate                :H_NI_CALIBRATE]    CALIBRAT
 [ Configuration            :H_NI_NUCONFIG]    NUCONFIG
 [ Control Center           :H_NI_CONTROL_CENTER]    NCC
 [ Diagnostics              :H_NI_NDIAGS]    NDIAGS
 [ Directory Sort           :H_NI_DIRECTORY_SORT]    DS
 [ Disk Doctor              :H_NI_NDD]    NDD
 [ Disk Editor              :H_NI_DISKEDITOR]    DISKEDIT or DE
 [ Disk Monitor             :H_NI_DISKMON]    DISKMON
 [ Diskreet                 :H_NI_DISKREET]    DISKREET.SYS
 [ Disk Tools               :H_NI_DISKTOOL]    DISKTOOL
 [ Duplicate Disk           :H_NI_DD]    DUPDISK or DD (or DISKCOPY)
 [ File Attributes          :H_NI_FA]    FA
 [ File Date                :H_NI_FD]    FD
 [ File Find                :H_NI_FILEFIND]    FILEFIND or FF
 [ File Fix                 :H_NI_FILEFIX]    FILEFIX
 [ File Locate              :H_NI_FL]    FL
 [ File Size                :H_NI_FS]    FS
 [ Image                    :H_NI_IMAGE]    IMAGE
 [ Line Print               :H_NI_LP]    LP
// [ MEM-MAX                  :H_NI_MM]
 [ NDOS                     :H_NI_NDOS]    NDOS
 [ Norton Cache             :H_NI_CACHE]    NCACHE2
 [ Norton Change Directory  :H_NI_NCD]    NCD
 [ Rescue                   :H_NI_RESCUE]    RESCUE
 [ Speed Disk               :H_NI_SD]    SPEEDISK or SD
 [ Safe Format              :H_NI_SF]    SFORMAT or SF (or FORMAT)
 [ SmartCan                 :H_NI_SC]    SMARTCAN
 [ System Info              :H_NI_SI]    SYSINFO or SI
 [ Text Search              :H_NI_TS]    TS
 [ UnErase                  :H_NI_UNERASE]    UNERASE or UE
 [ UnFormat                 :H_NI_UNFORMAT]    UNFORMAT
 [ Wipe Information         :H_NI_WIPEINFO]    WIPEINFO
^bOther^b
 [ The Norton Team          :H_NI_CREDITS]
   

#DEFINE H_NI_HELP_ADVISOR
#TITLE "Norton Advisor
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
The Norton Advisor is designed to help you understand computer
usage problems and then correct them. The Advisor, drawing
from the ample armament of the Norton Utilities, determines if
a diagnostic or remedial program is required, and runs it for
you.

Assistance is divided into four general areas:

^BCommon Disk Problems^B
   These are situations that frequently occur. Browse through the
   listing to see if any match your dilemma.

^BDOS Error Messages^B
   A listing of error messages that result from file and disk 
   operations. If you ever receive a DOS error message that you
   do not understand, find it here and follow the recommended
   action.

^BCHKDSK Error Messages^B
   CHKDSK (Check Disk) is a program provided by DOS to test the
   condition of your disks; however, it often generates cryptic
   messages when run. If you use CHKDSK, find the message and 
   again follow the recommended action.

^bApplication Error Messages^b
   Help is here for users of Lotus 1-2-3, Symphony, dBASE,
   Excel, Quattro Pro, and WordPerfect (and other programs that
   use identical file formats). Choose from a listing of error
   messages generated by these applications to correct file
   problems.



#DEFINE H_NI_HELP_INDEX
#TITLE "Index
Get help for any component of the Norton Utilities. Choose
from the list of topics for explanations of Norton commands or
individual utilities, and procedures for using them.

^bTip:^b It's always easiest to press ^bF1^b for context-sensitive
     help whenever you need a hand, rather than using the Index. 

#DEFINE H_NI_HELP_ABOUT
#TITLE "About...
This command displays the Norton Utilities version number, the
DOS version number, and available memory on your computer.


#DEFINE H_NI_MAIN
#TITLE "Welcome to the Norton Utilities!
This program, ^bNORTON^b, gives you easy access to all of the
Norton Utilities. Use ^b^C18^b and ^b^C19^b to move the highlight bar and
scroll the selections in the ^b컴Commands컴^b list. The line
at the bottom of the window shows the DOS command that will be
executed when you press ^bEnter^b (or ^bdouble-click^b with the mouse).

The ^b컴Description컴^b box describes the utility and shows some
or all of the options and switches (parameters) you can use. 
To add parameters to a command, just type them in before you
press ^bEnter^b.

^bNote:^b Virtually ALL the utilities are interactive; you rarely
      need command-line options except when using a batch file.
      For a list of options for any of the Norton Utilities, use
      ^b/?^b as the only parameter. You can do this from the DOS
      command line as well, for example:

      ^bFILEFIND /?^b

NORTON is also a First Aid center for disk ailments and a
consultant with answers to many questions. The [Norton Advisor:H_NI_HELP_ADVISOR]
in the Help menu (^bAlt^b+^bH^b,^bV^b) provides general help for common
computer problems and will run the right utility to help fix
them.

Finally, NORTON is an interactive "shell" or menu program.
You can execute any DOS command by typing it into the input
box near the bottom of the screen. And you can create custom
commands and descriptions, making it easy to execute often-
used application programs and DOS commands. In fact, you can
even customize the default commands and descriptions displayed
for the Norton Utilities. See the [Add Menu Item:H_NI_MENU_ADD_MENU_ITEM] and
[Edit Menu Item:H_NI_MENU_EDIT_MENU_ITEM] help topics for more information on this
feature.



#DEFINE H_NI_MENU_SORT_NAME
#TITLE "Sort by Name            ^bAlt^b+^bN^b or ^bAlt^b+^bM^b,^bN^b
This menu command redisplays the items in the ^b컴Commands컴^b
list in alphabetical order. The Topics entries are hidden
from view.

^bTip:^b Use ^bAlt^b+^bT^b to get back to the "Sort by Topics" display.



#DEFINE H_NI_MENU_SORT_TOPIC
#TITLE "Sort by Topic           ^bAlt^b+^bT^b or ^bAlt^b+^bM^b,^bT^b
This menu command redisplays the items in the ^b컴Commands컴^b
list, grouped by Topic.

In this format, the menu items shown as ^b  TOPIC  ^b have no
associated command displayed in the command input box; they
are subheadings for groups of commands.

^bTip:^b Use ^bAlt^b+^bN^b to remove the TOPIC lines and display only the
     command names, in alphabetical order.


#DEFINE H_NI_MENU_ADD_MENU_ITEM
#TITLE "Add Menu Item                            ^bAlt^b+^bM^b,^bA^b
This menu command lets you add a new command or topic to the
^b컴Commands컴^b list. Use this feature to customize Norton,
and use it as a general-purpose menu utility for easy access
to applications and often-used DOS commands.

In the initial dialog, select the ^b\[Command]^b button to add a
command or ^b\[Topic]^b to create a new topic. Topics are displayed
in the form, ^b  TOPIC  ^b, in the Commands list box; they are
passive headings and are not associated with a DOS command.

^bNote:^b To add a topic, the ^b컴Commands컴^b list must be
      [Sorted by Topic:H_NI_MENU_SORT_TOPIC].

#DEFINE H_NI_MENU_ADD_MENU_ITEM_TOPIC
#TITLE "Adding or Editing a Topic
  Type the name you want displayed. It will be centered
  within the ^b컴Commands컴^b list box.

  Use ^b^C18^b and ^b^C19^b to set its position relative to the other
  topics. Press ^bTab^b to access the controls on the right side
  and select [Description...:H_NI_DLG_ADD_EDIT_ITEM_DESC] to add some descriptive
  text to be displayed when the topic is highlighted. 

  Upon return, select ^b\[OK]^b to save your work and return to
  the main screen.


#DEFINE H_NI_DLG_ADD_EDIT_MENU_COMM
#TITLE "Adding or Editing a Command
  Type the text you want displayed in the Commands list. For
  instance,

     Name in menu: ^bFormat B: 720K^b

  and press ^bTab^b or ^b^C19^b.

  Next, type in any command that DOS will execute 컴 a program
  name, batch file name, or a DOS command, including specific
  parameters if you want. Later, this command will show up in
  the text area near the bottom of the main screen. For example,

     DOS command: ^bSFORMAT B: /F:720 /A^b

  and press ^bTab^b or ^b^C19^b.

  In the ^b컴Topic컴^b box, pick the topic under which you want
  this command to be placed. For instance, move down to the
  option button next to TOOLS and press ^bSpacebar^b.

  Finally, press ^bTab^b to get to the control group at the bottom
  and select [Description...:H_NI_DLG_ADD_EDIT_ITEM_DESC] to add some descriptive
  text to be displayed when the command is highlighted. 

  Upon return, select ^b\[OK]^b to save your work and return to
  the main screen.

#DEFINE H_NI_DLG_ADD_EDIT_ITEM_DESC
#TITLE "Editing a Command or Topic Description
In this screen, you can create or modify the text displayed in
the ^b컴Description컴^b box for a command or topic.

Use ^b^C18^b ^b^C19^b ^b^C1b^b ^b^C1a^b to move around in the Description box.
^bHome^b goes to the start of a line and ^bEnd^b to the end.

Insert a line by pressing ^bEnter^b. Pull up a lower line by
moving to the end of the line above it and pressing ^bDel^b.

You can dress up your text by using display attributes. Press
^bF2^b to cycle between Normal, Reverse, Bold, and Underline.
Subsequently typed text is displayed in that attribute.

Press ^bTab^b to move to the ^b\[OK]^b box and press ^bEnter^b to select
it.

^bPower-User Note^b: All commands, topics, and descriptions get
   saved into a standard ASCII text file named ^bNORTON.CMD^b in
   the Norton directory. You may prefer to edit this file
   with a text editor.


#DEFINE H_NI_MENU_EDIT_MENU_ITEM
#TITLE "Edit Menu Item                           ^bAlt^b+^bM^b,^bE^b
This lets you modify any item in the ^b컴Commands컴^b list as
well as change its associated DOS command line and description
text. It also lets you rearrange the order in which topic
groups are displayed. 

^bNote:^b The initial dialog box varies, depending upon whether
      you are changing a topic or a command.

This feature lets system administrators customize the text
shown by Norton. For instance, you can keep a novice user
out of hot water by adding special warnings to a command
description.

^bNote:^b To remove an item altogether, choose [Delete menu item...:H_NI_MENU_DEL_MENU_ITEM]
      in the Menu Menu (^bAlt^b+^bM^b,^bD^b).





#DEFINE H_NI_MENU_DEL_MENU_ITEM
#TITLE "Delete Menu Item                     Alt+^bM D^b
This menu command lets you delete a command or topic from the
^b컴Commands컴^b list.

^bNote:^b To delete a topic, the ^b컴Commands컴^b list must be
      [Sorted by Topic:H_NI_MENU_SORT_TOPIC].

When you delete a menu item, its name, DOS command, and
description are permanently removed from the ^bNORTON.CMD^b file. 
You may wish to make a backup of that file before doing any
deletions. If you accidentally delete items from the Norton
Utilities, you can start over with the default setup by
running the Install program.

This feature lets system administrators control the text shown
by Norton. For instance, you can delete some of the default
commands to keep novice users from trying things they
shouldn't.

^bTip:^b If you don't want anyone to delete or modify commands,
     you may wish to make the NORTON.CMD file a read-only
     file.

#DEFINE H_NI_MENU_EXIT
#TITLE "Exit                                           ^bAlt^b+^bX^b


  Best regards from Peter Norton and the rest of the gang.

///////// config menu

#DEFINE H_NI_CONF_VIDEO_MOUSE
#TITLE "Video and Mouse...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This command lets you modify the configuration that was set up
when you installed the Norton Utilities. You can paint the
screen with any colors, change the number of screen lines,
specify graphical effects, or train your mouse to behave.

^bNote:^b Changes made here affect colors and options for ALL
      programs that comprise the Norton Desktop.

#DEFINE H_NI_CONF_PRINTER
#TITLE "Printer Configuration...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This command lets you create or modify printer configuration
files, used with the [Line Print:H_NI_LP] (LP) utility. If you don't use
LP, you don't have to concern yourself further. 

If you use LP on more than one printer, you may want to create
separate configuration files for each printer. Or, you may want
to set up different configurations for a single printer, such
as one for double-spaced printing and one for single-spaced
printing.

^bTip:^b For basic printing chores, you don't really need a
     printer configuration file as the defaults are appropriate
     for most situations. You can also control LP options from
     the command line. Type ^bLP /?^b for a summary of
     command-line options.

#DEFINE H_NI_CONF_STARTUP_PROGS
#TITLE "Startup Programs...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This command lets you specify which programs are run from your
AUTOEXEC.BAT and CONFIG.SYS files at system startup. You have
the following options:

  [Use NDOS command processor      :H_NI_NDOS]
  [Diagnose Disk Problems (NDD /Q) :H_NI_NDD]
  [Save Disk Format Data (IMAGE)   :H_NI_IMAGE]
  [Start SmartCan                  :H_NI_SC]
  [Start Norton Cache              :H_NI_CACHE]
  [Start Disk Monitor              :H_NI_DISKMON]
  [Start Norton Cache              :H_NI_CACHE]
  [Start DISKREET Encryption Driver:H_NI_DISKREET]
  [Start Keystack (requires NDOS)  :H_NI_KEYSTACK]

If you don't know what any of these options mean, make sure
you at least choose ^bSave Disk Format Data^b to protect against
an accidental format of you hard disk. Another wise choice is
^bStart SmartCan^b which guarantees that deleted files can be
recovered.


#DEFINE H_NI_CONF_ALT_NAMES
#TITLE "Alternate names...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This command lets you shorten the command-line names for
several of the most frequently used utilities to save a few
keystrokes. 

^bTip:^b You also can rename Safe Format to make sure it's always
     used instead of the DOS FORMAT command.



#DEFINE H_NI_CONF_PASSWORDS
#TITLE "Passwords...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
The Norton Utilities have the power to do great good, but
if not used properly, great harm. With this option, you can
prevent the utilities that could possibly do damage from being
used.

This may be of particular importance if other people will be
using your computer, or if you oversee other people's computer
use.

^bNote:^b Only one password can be set for all protected
      programs.


#DEFINE H_NI_CONF_MENU_EDIT
#TITLE "Menu Editing...
If you need general help on how to use this command, choose
it from the menu, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This command allows you to control editing of the Norton 
program menu. 

A powerful capability of the Norton program is that the menu
can be edited to add new programs or delete ones that might
be dangerous for non-technical users. After you've set up
the menu to your liking, you can prevent any further changes.

/// leave this for help??
#DEFINE H_NI_USER_CMD
#TITLE "User-Customized Command or Topic
The highlighted Command or Topic is a custom menu item. It was
added to the ^b컴Commands컴^b list after the Norton Utilities
were installed.



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxx                                        xxxxxxxxxx
//xxxxxxx  Utilities, alphabetically by #DEFINE  xxxxxxxxxx
//xxxxxxx                                        xxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



#DEFINE H_NI_BE
#TITLE "Batch Enhancer
This is a toolkit of useful commands that you can use to make
batch files more flexible and more interesting.

Normally, BE commands are used only in batch files, but you
can experiment by typing them directly at the DOS command
prompt. For instance:

  ^bBE WINDOW 1 1 24 78 bright green ON red ZOOM^b

Or, you can execute a series of BE commands by placing them in
a text file and telling BE where to find them; for example:

  ^bBE MYFILE.TXT^b

will cause BE to read MYFILE.TXT and execute each of its
lines as a BE command (Note: Don't start the lines with "BE").

Use ^bBE /?^b to get a quick summary of BE command names. Use
^bBE command /?^b to see the syntax of a specific command.

^bNote:^b BE's color-setting capabilities depend on having the
      ANSI.SYS device driver installed.

Refer to the User's Guide for details about each BE command.



#DEFINE H_NI_CACHE
#TITLE "Norton Cache
Most disk access time is spent waiting for data to come from
the disk. If you have recently read that same data, Norton
Cache eliminates the delay entirely. The basic idea is to use
part of your system memory as a disk buffer, that is, as a
temporary holding pen for disk data. In fact, Norton Cache
anticipates your future data needs and has the data ready,
even before you ask for it! The speed increase is quite
dramatic.

If you have ^bno extended or expanded memory^b, you probably should
not use a disk cache. Many applications require 500K to run,
while the minimum size for an effective cache is at least 64K
(the more, the better). Exceptions include performing disk
intensive activities or using a laptop with an auto-power-down
hard disk.

Refer to the User's Guide for more information about using the
Norton Cache.


#DEFINE H_NI_CALIBRATE
#TITLE "Calibrate
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility provides three very useful hard disk functions:

^bCorrects (calibrates) interleave inefficiencies:^b
  Many hard disks have been "low-level" formatted with an
  inefficient interleave setting. This means that you must
  wait longer for data to be read from the disk than is
  necessary.

  Calibrate performs sophisticated tests to find out the best
  interleave for your hard disk when attached to your
  computer. It then fixes the interleave, often with a data
  transfer rate increase of 66% or more.

^bNote:^b The online help for Calibrate provides details of
      what disk interleave is and why it's important.

^bPerforms "deep" surface testing:^b
  This feature scans the disk for surface defects which might
  go undetected in a less thorough testing. It executes a
  scientifically accurate analysis of the ability of the disk
  surface to accept various magnetic flux combinations.

  If you've ever thought that some part of your hard disk was
  "a little shaky," use Calibrate's test to flush-out the bad
  and nearly-bad parts.

^bFirms up sector ID marks:^b
  As a disk ages, the read/write heads may tend to drift
  slightly from their original positions. Calibrate's
  non-destructive, low-level formatting ensures that the head
  will be positioned exactly, directly over each track, and
  that the low-level track-and-sector markings are easily
  read by the hard disk controller.



#DEFINE H_NI_CONTROL_CENTER
#TITLE "Control Center
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
Norton Control Center (NCC) provides a potpourri of useful
hardware configuration services. It is an interactive program
in which you can configure system devices the way you like
them, then save the settings for quick setup later. The optional
command-line "Quick Switches" provide a convenient way to set
the screen mode or keyboard rate.

^bNote:^b Some NCC services are not available on some computers.

NCC provides a variety of ^bscreen control^b services. It can set
your cursor size, customize the colors at the DOS prompt, 
select a 25-, 40-, or 50-line video mode, and lets you set up
a custom color palette to force uncooperative programs to use
your favorite colors.

NCC can speed up your ^bkeyboard^b and make your ^bmouse^b more
responsive. It also provides an interactive way to set the
configuration of your ^bserial ports^b.

NCC also provides several ^btimer^b functions. It has up to four
timers which you can start and stop to help you track your
daily activities or to make a record of how long some
operation took. It can print a "^bTime Mark^b" to the standard
output device for later reference.


#DEFINE H_NI_DD
#TITLE "Duplicate Disk 
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
Dup Disk makes an exact copy of a floppy disk to another
floppy disk of the same capacity 컴 including the DOS system
files and other hidden files. If the target disk is not
formatted, Dup Disk can format the target disk during the copy
operation.

Dup Disk uses extended or expanded memory, if available, so
that you swap disks only once when using a single floppy
drive. For multiple copies of the same disk, Dup Disk saves
you even more time because it holds the disk contents in
memory rather than rereading the disk for each copy made.



#DEFINE H_NI_DIRECTORY_SORT
#TITLE "Directory Sort
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility arranges the order in which files and directories
are listed. Directory Sort can be used either from the command
line or in full-screen mode.

Sort by ^bname^b, ^bextension^b, ^btime^b, ^bdate^b, or ^bsize^b. You can combine
any number of ^bsort keys^b, so that files that are the same by
one key can be arranged in order by the next key. When a
directory is sorted, all subdirectories are grouped first, 
followed by all files. 

In full-screen mode, a file can be placed at any location in
the sort order. You can make finding frequently accessed files
easier by placing them first in their directories.

^bNote:^b If you use NDOS, you won't see any changes in the sort
      order. NDOS displays the listings according to its own
      settings.

Use ^bDS /?^b for help with the command-line options.



#DEFINE H_NI_DISKEDITOR
#TITLE "Disk Editor
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility lets you see and modify any part of your disk,
including the highly sensitive partition table, file
allocation tables, and directories. It lets you examine any
part of any file and make changes.

Disk Editor is safe for even non-technical users. It starts out
in a "read-only" mode which lets you explore any disk without
fear of making a data-threatening mistake.



#DEFINE H_NI_DISKMON
#TITLE "Disk Monitor
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility provides three disk security features:

^bDisk Protection^b installs a TSR (RAM-resident program) that
  keeps an eye on all attempts to write to any of your disks. 
  It provides ^bsafety from viruses^b and it can ^bavoid accidental^b
  ^bdeletion^b or overwriting of specific types of files. When
  Disk Protect is ON, you will be warned whenever the disk is
  about to be modified. You can choose to block the attempted
  write or let it continue.

^bDisk Light^b also installs a TSR that keeps an eye on your disk
  activity. It displays a tiny flashing "light" in the top
  right corner of your screen whenever any disk is read or
  written to. This is handy for monitoring RAM-disk activities,
  to give you indication of disk activity if your system
  unit is on the floor and out of sight, or if a network
  drive is being accessed.

^bDisk Park^b is an important safety measure you should take
  whenever you turn off your computer. It moves the read/write
  heads of all your hard disk drives into a safe location. It
  is especially important when you intend to move your system.



#DEFINE H_NI_DISKREET
#TITLE "Diskreet
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility protects confidential files by encrypting them so
that only you can read them.

You can encrypt files individually or you can create something
called an ^bNDisk^b. It has a drive ID just like a regular disk,
but is actually a designated area on a physical disk.

The operation of an NDisk is transparent. Every file gets
encrypted as it put down on the disk. And it is decrypted
when it is read back into memory (or copied, etc.). Along the
way, you will be asked to enter a password, so only you can
have "clear text" access to the file.



#DEFINE H_NI_DISKTOOL
#TITLE "Disk Tools
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility provides a set of automated procedures which
protect disk data, simplify some technical operations, and
recover from certain types of data disasters. Since these are
safe, totally automated procedures, there is no need for an
novice user to use the complex and powerful Disk Editor
utility for these problems.

^bMake a Disk Bootable^b takes all the necessary steps to make it
  possible to use a disk as a DOS system or "boot" disk. The
  DOS SYS command is supposed to do this, but it often fails
  with a message, "No room for system files." Well, Disk Tools
  makes room, then copies the files correctly onto the disk
  (including COMMAND.COM). It can even adjust a hard disk
  partition table, in case that got scrambled somehow.

^bRecover from DOS's RECOVER^b fixes the mess that the DOS RECOVER
  utility makes of a disk. As a proud owner of the Norton
  Utilities, you have no need for RECOVER and should never use
  it. If someone does accidentally "recover" a disk, use
  Disk Tools to put the disk back into a state where a real
  utility can recover data from it.

^bRevive a Defective Diskette^b: When you see "Bad sector reading
  drive A: Abort, Retry...," the problem is often related to
  the "sector ID" marks BETWEEN the sectors. This tool
  rewrites the sector marks, without wiping the data.

^bMark a Cluster^b provides a simple way to tell DOS not to
  allocate data on a particular part of a disk OR you can
  perform the reverse operation (tell DOS to go ahead and use
  an area which is currently marked as "bad").

^bNote:^b The "Create Rescue Disk" and "Restore Rescue Disk"
      procedures from previous versions of the Norton Utilities
      Disk Tools are now part of a separate utility called
      [Rescue Disk:H_NI_RESCUE].


#DEFINE H_NI_SC
#TITLE "SmartCan
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This is one of the smartest safety inventions since strings on
mittens. SmartCan is a small TSR program that installs 
into memory and watches for commands that delete files. 
Instead of deleting a file, it moves it into a special hidden
directory named ^bSMARTCAN^b.

When the SMARTCAN gets full or a specified number of days have
passed, SmartCan automatically purges files to make room
for other "delayed deletions." Use the UnErase utility to 
recover (with 100% reliability) one or more of the deleted 
files.

This is one of those very-low-cost safety nets; it's a great
addition to your AUTOEXEC.BAT file.

^bTip:^b Choose [Startup Programs:H_NI_CONF_STARTUP_PROGS] from the Configuration menu to
     automatically place the command to start SmartCan in your
     AUTOEXEC.BAT file.


#DEFINE H_NI_FA
#TITLE "File Attributes
This utility lets you display, set, or clear any of the four
file attributes: ^bhidden^b, ^bsystem^b, ^bread-only^b, and ^barchive^b. 

You can protect a file from accidental erasure or overwriting,
or hide it from prying eyes. You can even hide a subdirectory.

Use ^bFA /?^b for help with the command-line options.



#DEFINE H_NI_FD
#TITLE "File Date
This utility lets you change the date or time stamps on a file
or set of files.

File Date is useful if you use backup software that backs up
files based on their date and time, or the MAKE program that
comes with many compilers. You could also give a group of
files a "generic" look by giving them all the same date and
time 컴 or removing the date and time completely.

Use ^bFD /?^b for help with the command line options.



#DEFINE H_NI_FILEFIND
#TITLE "File Find
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
As hard disk capacities multiply, it can be a certifiable
nuisance to find a file lost in a spaghetti of directories. 
File Find uses a super-fast technique to search ^ball directories^b
of ^ball drives^b to unearth a buried file.

But this versatile utility does a lot more than just look
for filenames. If you can't remember the filename, all you
need to know is ^bsome text in the file^b. File Find lists the
files that contain the text as it scans the disk, and you can
browse the contents of any file while File Find continues
searching. You can even search for text or Hex values and
replace it with something else wherever found. 

File Find can output a list of the matching files (with all
kinds of options), and it can format that list into a batch
file. ^bProgrammers love this^b: you can easily create a batch
file which does something (edit, copy, delete, you name it)
with every file that contains any string of text, such as
"printf" or "itemCount" or "DosOpen(".

You can limit the search by file date, size, or attribute (for
instance, you can list all hidden files). Moreover, you can
set any file attribute, or change the date and time of the
matching files.



#DEFINE H_NI_FILEFIX
#TITLE "File Fix
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility is handy for users of Lotus 1-2-3, Symphony, 
dBASE, Excel, Quattro Pro, and WordPerfect (and other
programs that use identical file formats).

File Fix knows all about the file headers and data layout
in these types of files and can repair files that the
application is unable to use. For instance, if a single
sector is lost, File Fix performs a "smart" recovery, so
that only the missing data is lost (rather than losing the
entire file).

File Fix also provides a convenient "UnZap" procedure to
restore data lost by an accidental use of the dBASE Zap
command.



#DEFINE H_NI_FL
#TITLE "File Locate
This utility is used from the command line to locate files or
directories quickly 컴 and requires very little memory to run.

File Locate is ideal for those times when you can't find a 
file or remember where it is. You can locate lost or missing
files, and even find buried directories, by searching through
all of the directories of one or more disks.

You can also search just the DOS path, which is useful when
you're trying to find multiple filenames or programs that you
know are somewhere in the path.

Use ^bFL /?^b for help with the command-line options.



#DEFINE H_NI_FS
#TITLE "File Size
This utility lists the size of a file or group of files,
reporting the total size of the group as well as the 
percentage of slack space the files occupy (unused space
in the last disk cluster of a file). 

File Size is very handy when copying files to another drive,
especially a floppy disk. It will determine if the group of 
files will fit on the other disk drive 컴 before you try to 
copy them.

You can also determine the total size of files for a branch
of the directory tree.

Use ^bFS /?^b for help with the command-line options.



#DEFINE H_NI_IMAGE
#TITLE "Image
How do you prevent the total disaster that occurs when a
novice user formats the hard disk?  One way is to remove the
DOS FORMAT command and use Norton's Safe Format utility. But
even that doesn't ensure that someone won't later use FORMAT
(say, from a floppy disk) to wipe out the disk. Also, Norton's
UnFormat utility will usually recover most of the files
without any help. But how to be 100% sure?

Image takes a "snapshot" of the data that would get wiped
during a DOS FORMAT, including the boot record, file
allocation table, and root directory. Then, if the worst
happens, the UnFormat utility will provide a 100% recovery of
your data.

Image works quickly and without fanfare and uses up only a
small amount of disk space. It is an excellent addition to
your AUTOEXEC.BAT file. 

^bTip:^b Choose [Startup Programs:H_NI_CONF_STARTUP_PROGS] from the Configuration menu to
     automatically place the command to run Image in your
     AUTOEXEC.BAT file.


#DEFINE H_NI_LP
#TITLE "Line Print
Print a text file directly from the DOS command line without
having to call up your word processor or editor. Use Line Print
for a quick print of AUTOEXEC.BAT, CONFIG.SYS, or any batch
file.

Line Print is a print formatter capable of printing text 
files to a file, printer, or device (such as COM1:). It is 
especially useful when you need files printed with line 
numbers (such as program listings or legal references).

Line Print has flexible options to set up your output page
with headers and footers, margins, line spacing columns, and
so forth. You can even print a file to a PostScript printer.

^bTip:^b Set the height of the page to 60 lines (/H60) for
     a laser printer; the default is 66 lines.

Use ^bLP /?^b for a list of the command-line format options.

If you use multiple printers or require several standard
print formats for the same printer, you can create a series
of printer description files for use with Line Print. Choose
Printer Configuration from the Configuration menu or the
NUCONFIG program to create the description files. Then use
^bLP filename /CONFIG:description^b to print. For example, 

   LP c:\\autoexec.bat /CONFIG:wideprt


//#DEFINE H_NI_MM
//#TITLE "MEM-MAX
//If you need general help on how to use this program, choose
//it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
//^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
//TK next version ...

#DEFINE H_NI_NDOS
#TITLE "NDOS
NDOS is a replacement for COMMAND.COM, the default command
processor supplied with DOS. Power users have come to rely on
its capabilities. NDOS provides all functions of COMMAND.COM,
and supplies numerous enhancements and additional commands.  

Refer to the NDOS.DOC file for details about NDOS usage.


#DEFINE H_NI_KEYSTACK
#TITLE "Keystack
Keystack is a special component of NDOS. When installed, it
lets you send keystrokes to another program from a batch file,
as if they were entered from the keyboard.


#DEFINE H_NI_NCD
#TITLE "Norton Change Directory
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
One of the all-time favorite utilities, this will save you
a lot of wear-and-tear on the finger muscles. It's a
replacement for the DOS CD command, and a whole lot more.

The interactive program (when you just type ^bNCD^b) is a great
visual way to find your way around a complex directory
structure. It lets you create and ^brename directories^b and
change a disk's volume label. It also provides a Print Tree
function to give you a printed map of your directories.

But the real value of Norton Change Directory is the time it
saves at the keyboard. For instance, rather than typing:

  C:\\> ^bD:^b                                      (change drive)
  D:\\> ^bCD \\wordproc\\daisy\\ltrs\\may^b             (change directory)
  D:\\WORDPROC\\DAISY\\LTRS\\MAY>_

you can simply type the first part of the endpoint name:

  C:\\> ^bNCD D:MAY^b               (change default drive & directory)
  D:\\WORDPROC\\DAISY\\LTRS\\MAY>_

to get to the same place. This example saves about 20 
keystrokes. Multiply that by the thousands of times you'll
use Norton Change Directory and you see just how handy it is.

#DEFINE H_NI_NDIAGS
#TITLE "Norton Diagnostics
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
Norton Diagnostics is a comprehensive hardware diagnostics
utility. Use Norton Diagnostics to pinpoint hardware problems
when they occur, or as a routine diagnostics tool to locate
marginally defective components.

MIS personnel can use Norton Diagnostics to diagnose computer
problems and generate reports of each computer's operating
status. 



#DEFINE H_NI_NUCONFIG
#TITLE "Norton Utilities Configuration
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
After you have the Norton Utilities up and running, there may
come a time when you want to change your configuration. This 
utility is the tool to do it. All in one program, you can:
 
    ^b ^C07^b  Set or remove passwords for the utilities that 
        can do damage if used carelessly or improperly.
    ^b ^C07^b  Control editing of the Norton program menu.
    ^b ^C07^b  Remove utilities that can be dangerous for 
        non-technical users.
    ^b ^C07^b  Set screen and graphics options.
    ^b ^C07^b  Set mouse options.
    ^b ^C07^b  Customize screen colors.
    ^b ^C07^b  Install, remove, or configure the Norton Cache.
    ^b ^C07^b  Install or remove device drivers from your 
        CONFIG.SYS file.
    ^b ^C07^b  Install or remove programs from your 
        AUTOEXEC.BAT file.
    ^b ^C07^b  Select alternate names for certain of the utilites.

^bNote:^b You can also access these options directly from the Norton
      program Configuration menu.

#DEFINE H_NI_NDD
#TITLE "Norton Disk Doctor
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility performs a comprehensive analysis of the logical
structure of a floppy or hard disk.

The Doctor diagnoses disk problems and, with impeccable
bedside manner, tells you what's wrong and offers to correct
the problem immediately.

And unlike your neighborhood sawbones, the Doctor can ^bUndo^b any
operation. This keeps the malpractice insurance to a minimum
and lets you get plenty of untroubled bed rest.


#DEFINE H_NI_RESCUE
#TITLE "Rescue Disk
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
Rescue provides the best possible insurance policy for your
system. ^bCreate Rescue Disk^b reads critical system information
from a healthy system and copies it to a floppy disk (your
CMOS configuration settings, hard disk partition tables, and
so forth).

^bRestore Rescue Disk^b can be used to reset your computer back to
those known-to-be-good settings.

^bTip:^b If you haven't yet created a rescue disk, do it now!

#DEFINE H_NI_SD
#TITLE "Speed Disk
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility optimizes the way data is arranged on a floppy
disk or hard disk. It eliminates file fragmentation which can
degrade disk performance. See the online help in Speed Disk
for a detailed explanation of fragmentation.

In this version of Norton Utilities, Speed Disk now gives you
finer control over how things are organized. For instance,
you can specify the physical ordering of directories, so
certain often-used directories are close together and near the
front of the disk (for fastest access).

As a bonus, Speed Disk can sort filenames as they are listed
in the directory (so you can more-easily find something when
you use the DOS DIR command).



#DEFINE H_NI_SF
#TITLE "Safe Format
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility is a fast-working replacement for the DOS FORMAT
command. In fact, when you installed the Norton Utilities, you
may have chosen to have Safe Format substituted in place of
the DOS FORMAT command (which gets renamed as XXFORMAT.COM).

The most important features of Safe Format are its all-around
^bsafety^b and its reformatting ^bspeed^b.

Safe Format will never format a hard disk unless you expressly
ask it to (several times!). And when it does, it still saves
a copy of all the information it will need to restore the
disk entirely. Even on a floppy disk, Safe Format warns you
if the disk contains readable data (in case you put the wrong
disk in the drive by accident).

The speed increase is seen when you reformat a floppy disk which
was already formatted. Safe Format skips the time-consuming
part of a format and just clears out the system area (saving
recovery data, of course) and then tests to make sure that the
disk is fully usable. This is much faster than a standard
format. There is even a quicker version, ^b/Q^b, which just
cleans out the system area without checking the media.



#DEFINE H_NI_SI
#TITLE "System Information
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility is an exceptionally comprehensive source of
information about your computer's configuration and it
provides disk and system benchmarks that you can use in
comparing the performance of one computer to another.

In addition to the expected disk and CPU speed tests, you'll
get a current ^bmemory-usage summary^b (including precise details
on TSRs, device drivers, and memory blocks), a complete up-to-
the-minute list of ^binterrupt vectors^b and ^bIRQ^b values, a ^bCMOS^b
configuration settings and status report, ^bnetwork^b information
and performance test. Your ^bdisk partition table^b information
is listed and you can see the contents of your ^bstartup files^b.

System Information can generate printed reports (a full report
can run to 15 pages) or it can send the report to a text file.

^bTip:^b Don't overlook the ^bSYSINFO /TSR^b Quick Switch. It's a
     convenient way to get a list of currently-installed RAM-
     resident software without leaving the comfort of the DOS
     command line.



#DEFINE H_NI_TS
#TITLE "Text Search
This utility searches for text in one or more files anywhere
on the disk, including erased space.

Text Search is a lifesaver when you can't remember a file's 
name, but do remember a unique word or some text from the 
file. Use it also to locate that part of an erased file that
contains the text (if it hasn't been overwritten in the 
meantime). You might even get lucky with ^bUnErase^b.

Use ^bTS /?^b for help with the command-line for options.



#DEFINE H_NI_UNERASE
#TITLE "UnErase
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
This utility makes it simple to recover a deleted or erased
file.

For quick recovery, just type ^bUnErase filename.ext^b as soon as
possible after the file gets deleted. Or type ^bUnErase^b
(without a filename) to use the interactive mode.

UnErase, by itself, is as reliable as is technically possible.
And when it's impossible to know where part of a deleted file
is, UnErase provides powerful tools to help you find and
recover the entire file or whatever parts have not been
overwritten.

^bTip:^b If you install the [SmartCan:H_NI_SC] utility as a memory-resident
     "safety net," UnErase is 100% reliable. UnErase will use
     information provided by SmartCan to achieve a flawless
     recovery, even if you deleted the file several days ago!



#DEFINE H_NI_UNFORMAT
#TITLE "UnFormat
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
When you use the DOS FORMAT command to format a hard disk or
use Norton Safe Format to re-initialize a floppy disk, only the
system area (the bookkeeping information) gets wiped 컴 the data
areas of the disk remain intact.

Since the data is still there, UnFormat is able to recover
most or all of it by re-creating the bookkeeping information. 
If you occasionally use the Image utility to save a snapshot
of your system area, UnFormat can recover everything from a
formatted hard disk.

^bNote:^b The DOS FORMAT command (prior to DOS 5.0), when used
      on a floppy disk, literally wipes everything off of the
      disk. You should always use Norton [Safe Format:H_NI_SF] for all
      formatting operations instead.



#DEFINE H_NI_WIPEINFO
#TITLE "Wipe Information
If you need general help on how to use this program, choose
it from the ^b컴Commands컴^b list, then press ^bF1^b for help.
^b컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b
When a file gets deleted or a hard disk gets formatted, the
data on the disk is not actually erased. Anyone with UnErase
or a disk editor can snoop around and locate data which you
may prefer to keep from prying eyes. Wipe Information's job
is to cleanse data totally from a file or disk.

One place not to overlook is the "slack space" at the end of
each file. It may contain old information that you are not
even aware is there (some word processors, for instance,
create temporary files which never appear in the directory).
Wipe Information can carefully scrub this area without damaging
the file data.

^bWarning^b: Whatever data you wipe will be ^blost forever^b. For
         instance, be aware that wiping "unused" space will
         eliminate any chance of ever recovering any erased 
         file data that's in there (except for currently
         SmartCan-protected files).


#DEFINE H_NI_CREDITS
#TITLE "The Norton Utilities 8.0 Team
^b컴 Development 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�^b

   Reese Anschultz
   Leo Cohen
   Basil Gabriel
   Rick Glenn
   David Hertel
   Meijen Huang
   Henri Isenberg
   Mark Kennedy
   Dan'l Leviton
   Tony Magill
   John McNamee

^b컴 Quality Assurance 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�^b

   Rebecca Black
   David Buches
   Fernando Chapa
   Sam Doe
   Dave Hicks
   James Kazanegras
   Daniel Neuwirth
   Tiffany Phan
   Julian Rozentur
   Lorne Steiner
   Rowan Trollope
   Harry Wan

^b컴 Documentation and Online Help 컴컴컴컴컴컴컴컴컴컴컴컴컴컴�^b

   Ken Baker
   Renee Gentry
   Ken Hodges
   Dan Norton
   Christopher Ratner
   Karen Torimaru
   Greg Vogel

^b컴 Product Management and Marketing 컴컴컴컴컴컴컴컴컴컴컴컴컴^b

   Pat Kennedy
   Richard Yim

^b컴 Technical Support 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�^b
        
   Vernon Balbert
   Mike Brown
   Jeffrey Bryan
   Kris Bugbee
   Richard Coutinho
   Eli Glass
   Curtis Hale
   Adam Karol
   David Kerendian
   Jill Pengra
   Valerie Rice
   Wes Santee
   Mike Shkolnik
   Larry Stone
   Russel Tiller
   Michael Williams
   
^b컴 Additional Acknowledgments 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴^b

   Frank Arjasbi
   Ardeshir Babak
   Karen Black
   Suzanne Bray
   Noreen Butler
   Alena Cespivova
   Cam Cotrill
   John Eusebi
   John Fawcett
   Brian Foster
   Barry Gerhardt
   Alfred Ghadimi
   Bruce Hellstrom
   Kim Johnston
   Steve Kurtz
   George Lawrence
   Patrick Martin
   John McIllwain
   David Oldfield
   Scott Pasnikowski
   Valerie Rice
   Vicki Routs
   Enrique Salem
   Ana Shannon
   Ellen Taylor
   Gary Ulaner
   Vickie VonBergen
   Laura Weatherford

