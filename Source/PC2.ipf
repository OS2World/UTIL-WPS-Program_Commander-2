:userdoc.
:title.PC/2 Help Program
:body.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help with F1, the mouse, the smarticon or the keyboard.            *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=306.PC/2 Help for Help
:lines align=center.
:color fc=red.
PC/2 - Program Commander/2 Version 2.20
for IBM OS/2 2.x, WARP, WARP Srv. SMP, Aurora Presentation Manager
Copyright (C) by Roman Stangl December, 2000
:color fc=default.
:elines.
:p.Please select one of the following topics for further information:
:ul compact.
:li.:link reftype=hd res=303.<About the utility PC/2>:elink.
:li.:link reftype=hd res=4012.<Installing PC/2>:elink.
:li.:link reftype=hd res=4008.<Using PC/2 as a WPS addon>:elink.
:li.:link reftype=hd res=4009.<Concepts of the Popup Menu>:elink.
:li.:link reftype=hd res=4010.<Starting a Program from PC/2>:elink.
:li.:link reftype=hd res=301.<Setup dialog>:elink. (Setting up the Popup Menu)
:ul compact.
:li.:link reftype=hd res=2700.<Menu Installation dialog>:elink. (Adding a Submenu)
:li.:link reftype=hd res=1200.<Program Installation dialog>:elink. (Adding a Program)
:li.:link reftype=hd res=2900.<Control Addition dialog>:elink. (Adding Menucontrols)
:li.:link reftype=hd res=1650.<Control Configuration dialog>:elink. (Changing Popup Menucontrols)
:li.:link reftype=hd res=2600.<Resort dialog>:elink. (Resorting a (Sub)menu)
:li.:link reftype=hd res=3200.<Startup Environment Spaces dialog>:elink. (Runtime Environment Spaces)
:li.:link reftype=hd res=3100.<Startup Parameters dialog>:elink. (Runtime (Commandline) Parameters)
:eul.
:li.:link reftype=hd res=302.<Desktop Configuration dialog>:elink. (Setting up the Desktop)
:li.:link reftype=hd res=4013.<PC/2's Overview Window>:elink.
:li.:link reftype=hd res=258.<PC/2's Spooler Control Window>:elink.
:li.:link reftype=hd res=262.<PC/2's QuickSwitch Window>:elink.
:li.:link reftype=hd res=263.<PC/2's SessionBar Window>:elink.
:li.:link reftype=hd res=4014.<PC/2's compatibility to other applications>:elink.
:li.:link reftype=hd res=4018.<PC/2's known problems>:elink.
:li.:link reftype=hd res=4011.<Error Messages>:elink.
:li.:link reftype=hd res=4003.<Questions &amp. Answers (part 1)>:elink.
:li.:link reftype=hd res=4004.<Questions &amp. Answers (part 2)>:elink.
:li.:link reftype=hd res=4005.<Questions &amp. Answers (part 3)>:elink.
:li.:link reftype=hd res=4015.<PC/2 hang/trap/bug diagnostics>:elink.
:li.:link reftype=hd res=4016.<Technical background information>:elink.
.* :li.:link reftype=hd res=4017.<PC/2 Survey>:elink.
:eul.
:note.
PC/2 contains :hp8."WPS 2 PC/2":ehp8. WorkPlace Shelll migration tool in its package.
For further information about WPS 2 PC/2 select :hp9.Help:ehp9. from
the WPS Object WPS 2 PC/2 itself.
WPS 2 PC/2 is installed on your Desktop during installation of the
PC/2 package.

.* :p.:link reftype=hd res=4017.<Backward>:elink.
:p.:link reftype=hd res=4016.<Backward>:elink.
                                :link reftype=hd res=303.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the About PC/2 dialog box.                               *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=303.Help for About PC/2 dialog
:lines align=center.
:color fc=red.
PC/2 - Program Commander/2 Version 2.20
for IBM OS/2 2.x, WARP, WARP Srv. SMP, Aurora Presentation Manager
Copyright (C) by Roman Stangl December, 2000
:color fc=default.
:elines.
:p.E-Mail:
:ul compact.
:li.InterNet
:ul compact.
:li.Roman_Stangl@at.ibm.com (preferred) (or alias 61821799@at.ibm.com)
:li.warpguru@geocities.com
:li.rstangl@vnet.ibm.com
:eul.
:li.Lotus Notes: Roman Stangl/Austria/IBM@IBMAT (IBM Internal)
:eul.
:p.WWW:
:ul compact.
:li.http&colon.//www.geocities.com/SiliconValley/Pines/7885/
:li.http&colon.//www.geocities.com/warpguru/
:eul.
:p.The PC/2 package (consisting of the OS/2 PM application PC/2 and
the WPS program Object WPS 2 PC/2) is classified as Freeware.
Under the copyright, you are allowed and encouraged to freely use, distribute or modify
this program under the following conditions:
:ul compact.
:li.The complete source code and all executables must be included. This rule also applies
for any modifications/recompilations you've made.
Just drop me a short note describing the changes.
:li.You may only charge for the costs of duplicating. :hp4.Distribution on InterNet, FidoNet
or similar is preferred.:ehp4. Therefore, this program must not be sold by immoral "Public
Domain &amp. ShareWare" dealers that charge a price way over the actual duplication costs.
:li.:hp4.Any:ehp4. commercial use is prohibited without contacting me beforehand, except
for incorporating the concept or code in any future release of OS/2 by IBM.  This however this
clause doesn't apply to individual private use.
:li.You don't want to port this utility to MS Windows, Windows NT (Nice Try), Windows 95,
Windows97 oops Windows98 (it's late again and still sucks ;-) or Windows 2000 (with its
30 millions new lines of code you can calculate the numbers of bugs yourself).
:eul.
:p.You are encouraged to upload this utility to your favorite file-server or
BBS. Please doublecheck that no file from this archive is missing, if you
change the archive to a different compression scheme.
Please retain the same name, regardless of the compression method you use.
PC/2's archive is named PC2vxyy.ZIP, where x is the major and yy the minor version,
e.g. 220 for version 2.20 (PC2v220.ZIP).
:p.If you find this program useful and want to honor many many months of work writing
this utility, you are welcome to send me a small donation of about 50 to 150 Austrian
Schillings (about 5 to 15 dollars) to my home address.
.* I'm interested in any feedback (goodies or badies) of PC/2, so I would ask you
.* to fill in the :link reftype=hd res=4017.<PC/2 Survey>:elink..
At least I would ask you to do your best to convince, as many as possible, Windows
users and vendors to support OS/2.
Of course you don't need to pay again, when a new release of PC/2 comes out.
Please send any donations in cash - it is a lot of trouble to work with foreign
money orders or equivalent, here in Austria, plus the banks charge an arm and a leg!
:p.Suggestions are welcome.
Because of the Freeware status of PC/2 I am :hp4.not:ehp4. going to send new
versions or anything else by mail (which is likely to become a never ending story),
however I try to do my best to do support via E-Mail.  As my time is limited, contributors'
get priority treatment.
:p.Thanks to all those people who sent me proposals and bug reports to improve this
utility!
Special thanks to
Alex Stewart,
Carrick von Schoultz and Markus Hof,
Frank Schroeder,
Ingo Pflanz,
Jason Shannon,
Joe Hartman
Mario Semo,
Max Mikhanosha
Michael Wakunda,
Monte Copeland,
Peter Fitzsimmons,
Robert Mahoney,
Roland Emmerich,
Rolf Knebel,
Roman Fischer,
Stefan Hora,
who gave me advice, source code, valuable hints, or other support to improve PC/2.
Special thanks also to
Adam Todd Bradley,
Alessandro Cantatore,
Alexander Men'shchikov,
Andrea Resmini,
Bj”rn-Erik Wenz,
Chris Green,
Christian Kraemer,
David Grasner,
Don Eitner,
Gianluca Della Vedova,
Gordon Barnard,
Hung Kang,
Jean-Bernard Clerin,
Jeff Blakley,
Jim Read,
Joaquim Homrighausen,
Joel Swartz,
Joerg Tiemann,
John Bacas,
Kenn Yuill,
Larry Seah,
Marcus Sommer,
Martin Cooney,
Michael Grigutsch,
Michael Taylor,
Paul Allen,
Paul Gazo,
Paul Marwick,
Peter Moylan,
Petr Mikulik,
Radek Pavelka,
Reinhard Giesen,
Ron Klein,
Sorin Srbu,
Steve Winter,
Tero Kilkanen,
Terry Bayne,
Zephyr Q,
and others for their invalueable feedback during beta testing 2.xx.
:p.Special thanks to Andrew Zabolotny for giving me the source code the greatly
enhance the Overview window (and some bug fixes and corrected type conversions).
:p.Also, thanks to anyone who makes "undocumented" information available, too!
:p.My home address is:
:sl compact.
:li.    Roman Stangl
:li.    Strebersdorferstrasse 46
:li.    A-2103  Langenzersdorf, Austria
:esl.
:p.:link reftype=hd res=306.<Backward>:elink.
                                :link reftype=hd res=4012.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for PC/2 installation.                                        *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4012.Installing PC/2
:p.You have obviously started PC/2 once, because you are reading this help panel.
The installation is straight forward, with a REXX script that installs PC/2.
However here are some points to take note of:
:ul.
:li.If you are upgrading from a previous version, I strongly recommend to backup
:hp9.PC2.CFG:ehp9., :hp9.PC2.INI:ehp9. and :hp9.PC2.ENV:ehp9.!
Additionally, it is a great idea to make such backups regularily anyway, due to
intensive beta testing PC/2 likely will not crash on, but you know the devil
never sleeps ;-).
:li.The installation installs an icon on the Desktop and starts the help panels
of PC/2. To ensure that OS/2 starts PC/2 automatically the next time you boot your
system, you must start PC/2 again (you then can notice that the background behind
the icon changes).
:p.If you have added :hp4.SET RESTARTOBJECTS=STARTUPFOLDERSONLY:ehp4.
in your :hp9.CONFIG.SYS:ehp9. then you :hp2.must:ehp2. add a shadow of PC/2
to your startup folder to ensure PC/2 is started the next time you boot OS/2.
:li.PC/2 requires access to the :hp4.PC2HOOK.DLL:ehp4. and :hp4.PC2SPOOL.DLL:ehp4. files.
PC/2 tries to find these files in the current directory PC/2's executable was started
from.
PC/2's installation will copy all required files to the installation directory of
your choice, moving or deleting files may prevent PC/2 from loading.
:p.PC/2 will not be able to run if :hp4.PC2HOOK.DLL:ehp4. can't be loaded, and won't be
able to provide the PC/2 Spooler Control Window, if :hp4.PC2SPOOL.DLL:ehp4. can't
be loaded.
:p.The file :hp4.PC2.HLP:ehp4. may be either accessible in the directory,
where PC/2 is started from, or in a path that is included in the environment
variable :hp4.HELP:ehp4..
:p. If you have problems running PC/2, ensure that the required files are accessible in
the working directory of PC/2.
:li.The installation program assumes that you are using the default profile :hp9.PC2.CFG:ehp9.
containing a standard pre-defined Popup Menu. If you want to use another Popup Menu,
you have to restart PC/2 and add your preferred profile as a commandline option.
Click to the next help panel to get more information about commandline parameters.
You may also use your favorite editor to import one profile into another, but
you have to ensure that you don't violate the syntax rules of the profile.
:li.PC/2 looks for its initialization file, :hp9.PC2.INI:ehp9. in the directory
where PC/2 was started from.
:li.You can only run one copy of PC/2 concurrently.
:li.If you select a DOS or WIN-OS2 Program to be started via PC/2, you may want to
increase their time-slice (by changing the DOS-Settings to IDLE_SECONDS=5,
IDLE_SENSITIVITY=100 and SESSION_PRIORITY=32) to get better performance.
This is useful for most DOS games, but slows down multitasking of other
applications.
:li.You may :hp2.use PC/2 with or without the WPS installed:ehp2.,
:hp2.you may even use PC/2 as a replacement for the WPS:ehp2..
To do so, you must install PC/2 first.
Then you have to ensure that :hp4.PC2HOOK.DLL:ehp4. and :hp4.PC2SPOOL.DLL:ehp4.
are accessible by PC/2, if :hp4.PC2SPOOL.DLL:ehp4. can't be found then you have
no way to control the spooler then.
You also have to ensure that :hp4.PC2.HLP:ehp4. is accessible in the PC/2 directory, or
in a path pointed to by the :hp4.HELP:ehp4. environment
variable in your :hp9.CONFIG.SYS:ehp9. file. Then replace the statement
:p.:hp4.SET RUNWORKPLACE=C&colon.\OS2\PMSHELL.EXE:ehp4. with
:p.:hp4.SET RUNWORKPLACE=[path]PC2.EXE:ehp4. or
:p.:hp4.SET RUNWORKPLACE=[directory]PC2.EXE:ehp4.
:p.where where :hp4.[path]:ehp4. is the full pathname of the directory (including
the drive letter, e.g. C&colon.\PMAPPS\PC2\PC2.EXE) and :hp4.[directory]:ehp4. is the
pathname of the directory (excluding the drive letter, e.g. \PMAPPS\PC2\PC2.EXE)
you installed PC/2.
:warning.
If you specify only the directory you installed PC/2 into, then be sure that this
directory is found on your boot drive.
:ewarning.
This feature is useful when using PC/2 as the shell process for an OS/2 maintenance
setup created by :hp4.BOOTOS2:ehp4..
See the :link reftype=hd res=4014.<Compatibility section>:elink.
for more information.
:p.The next time you boot, PC/2 will be started as a replacement for the WPS.
Be careful, OS/2 seems to dislike any parameters added to the program
started as a replacement for the WPS!
Of course, you can start the WPS from a command prompt by typing
:hp4.START PMSHELL:ehp4. or add a corresponding entry to PC/2.
:warning.
When running PC/2 as the Workplace Shell process, you can't terminate
PC/2, because the WPS process should not terminate, neither by closing
PC/2 from the Window List, nor by closing it from the :hp9.Exit:ehp9.
smarticon or the :hp9.Exit OS/2:ehp9. Menuentry.
Any WPS process will automatically be restarted by OS/2 immediately following its
termination (you may notice this fact when the WPS traps, it will restart
immediately, and rebuild your desktop).
:ewarning.
:li.You may :hp2.use PC/2 as the protected mode (PROTSHELL) process:ehp2..
To do so, you must install PC/2 first.
Then you have to ensure that :hp4.PC2HOOK.DLL:ehp4. (and :hp4.PC2SPOOL.DLL:ehp4.
if you are not running :hp4.PMSHELL.EXE:ehp4. as the WPS) are accessible by PC/2.
You also have to ensure that :hp4.PC2.HLP:ehp4. is accessible in the PC/2 directory, or
in a path pointed to by the :hp4.HELP:ehp4. environment
variable in your :hp9.CONFIG.SYS:ehp9. file. Then replace the statement
:p.:hp4.PROTSHELL=C&colon.\OS2\PMSHELL.EXE:ehp4. with
:p.:hp4.PROTSHELL=[path]PC2.EXE:ehp4. or
:p.:hp4.PROTSHELL=[directory]PC2.EXE:ehp4.
:p.where where :hp4.[path]:ehp4. is the full pathname of the directory (including
the drive letter, e.g. C&colon.\PMAPPS\PC2\PC2.EXE) and :hp4.[directory]:ehp4. is the
pathname of the directory (excluding the drive letter, e.g. \PMAPPS\PC2\PC2.EXE)
you installed PC/2.
:warning.
If you specify only the directory you installed PC/2 into, then be sure that this
directory is found on your boot drive.
:ewarning.
:p.The next time you boot, PC/2 will be started as the PROTSHELL process.
:warning.
:hp2.PC/2 can't be run as the PROTSHELL and WPS process simultaneously:ehp2., so
please ensure that PC/2 is either the PROTSHELL or WPS process, but not both!
:p.
When running PC/2 as the protected mode (PROTSHELL) process, you can't terminate
PC/2, because the PROTSHELL process can should not terminate, neither by closing
PC/2 from the Window List, nor by closing it from the :hp9.Exit:ehp9.
smarticon or the :hp9.Exit OS/2:ehp9. Menuentry.
If the PROTSHELL process exits, PM will hang as OS/2 adds additional threads to
a process started as the PROTSHELL (e.g. to display the Window List).
Unfortunately the same applies when the PROTSHELL process terminates due to a
program error.
:p.Running PC/2 as the PROTSHELL process prevents :hp4.CTRL+ALT+DEL:ehp4. to
cause a reboot!
:ewarning.
:warning text='Note:'.
:hp2.PC/2 has not been tested on all OS/2 versions or installed software
and hardware :ehp2.(simply because I don't have them):hp2., so there is no guarantee
of PC/2 being bugfree!
On the other hand I can think of no situation where a PC/2 bug would cause a loss of data
or system destruction more than the effects of an unplanned reboot.:ehp2..
:ewarning.
:eul.
:p.:link reftype=hd res=303.<Backward>:elink.
                                :link reftype=hd res=4008.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected general help from the PC/2 main help panel.                        *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4008.General Help for PC/2
:p.PC/2 - Program Commander is an addon or replacement for the WPS (Workplace Shell).
Invoking PC/2 uses options loaded from configuration files, which can be replaced by
using the following commandline syntax:
:hp4.:xmp.
PC2 [/Profile Filename.ext]
    [/Environment Filename.ext]
    [/INI Filename.ext]
    [/[Double|Single]Click]
    [/Button1 | /Button3]
    [/NoAutoStart]
    [/NoScheduler]
    [/NoCleanup]
    [/NoPrompt]
    [/ReserveDriveLetter Drives]
:exmp.:ehp4.
instead of :hp4./:ehp4. you can invoke the options also by :hp4.-:ehp4..
Of course, the options can be specified case insensitive but are case preserving.
:p.
:warning text='Note!'.
PC/2 will immediately terminate with a message box when
you press and hold the :hp4.ALT:ehp4.+Key while PC/2 starts (that allows
you to prevent PC/2 from loading even if PC/2 is started
automatically from STARTUP.COM or the Startup Folder).
:ewarning.
:p.You can configure a Popup Menu that will appear when mouse button 1 or mouse
button 3 is clicked anywhere on the Desktop (or when having selected even anywhere).
The Popup Menu may contain only text entries, as you may know from other
OS/2 menus, or it may contain in addition to the text an icon representing
that Menuentry.
:p.Alternatively you may activate the Popup Menu by keyboard keystrokes,
that is pressing the :hp4.LEFT SHIFT:ehp4. and the :hp4.RIGHT SHIFT:ehp4.-keys
simultaneously, the :hp4.WIN:ehp4.-keys, the :hp4.Caps Lock:ehp4., the
:hp4.Scroll Lock:ehp4., or the left and right :hp4.CTRL:ehp4.-keys,
all combinations configurable.
Those keys also activate the Popup Menu
next to the mouse pointer, or centered on the screen if no mouse is available.
This feature allows you to use PC/2 on workstations where no mouse is attached.
From this freely configurable Popup Menu, you can select
an entry - which is launched immediately.
:p.PC/2 lets you define (up to 180) Hotkeys too.
Pressing the Hotkey defined for a program the program is switched into the foreground
or, if it is not already running, launched immediately.
:p.PC/2 also allows you to expand the Desktop up to an 81-fold Virtual
Desktop, you may define Hotkeys to switch to an application on any Virtual Desktop
by a simple keypress.
:p.You may also switch to any Virtual Desktop directly by doubleclicking with mouse
button 1 or mouse button 3 on PC/2's Overview Window.
:p.PC/2 also enables you to define a UNIX-style behavior of switching the input focus
from one window to another. This allows you to have the input focus in a window which
is not the topmost window. (Sounds complicated, but try it; you may like it!)
:p.PC/2 additionally provides a simple Spooler interface. You can manage the printer
queues and jobs on your local machine - a necessity when using PC/2 as a WPS
replacement (because without the WPS you have no spooler available unless you use
PC/2's spooler interface).
:p.PC/2 allows you to customize the fonts and colors used by the Popup Menu, the Overview
Window the SessionBar and the Spooler window, just by dragging fonts from the Font
Palette and color from the Color Palette to PC/2's windows.
:warning text='Warning'.
On some display drivers you may not be able to set up every font
available in the Font Palette exactly. Instead, you may just get a similar font.
:ewarning.
:p.For example. the Popup Menu might look like:
:artwork name='Source\Resource\PopupMenu.bmp' align=left.
:p.You have selected the item :hp4.Prompts:ehp4., which is a Submenu containing
five more items. E.g., the item :hp4.OS/2 Window:ehp4. starts a windowed OS/2
command prompt, when selected. This Popup Menu is fully configurable - you may add,
change or remove items. An item can either be a Submenu containing more items,
the name of a program, or a Menucontrol.
:p.The complete Popup Menu can be modified, but the Menucontrol :hp9.Desktop Menu:ehp9.
must be defined before the Popup Menu will be displayed after mouse button 1 or
mouse button 3 clicks.
If you have not defined this Menucontrol, the :hp9.Setup:ehp9. dialog appears after
mouse button 1 or mouse button 3 clicks enabling you to add this control.
:p.Your Popup Menu configuration is read from the default profile :hp9.PC2.CFG:ehp9. each time
PC/2 is started and saved every time you :hp9.save:ehp9. your changes.
You can of course overwrite the default
configuration filename to a file named :hp4.Filename.ext:ehp4. by invoking PC/2 with
the following commandline option (the square brackest mean that the option is just
optional but will overwrite the default when specified):
:p.:hp4.[-Profile Filename.ext]:ehp4. or
:p.:hp4.[/Profile Filename.ext]:ehp4.
:p.where :hp4.Filename.ext:ehp4. conforms to a filename of a PC/2 profile. Both FAT
and HPFS filename naming conventions are accepted. A few
sample configuration files are provided in this package.
:p.The Environment Space configuration is read from the default profile
:hp9.PC2.ENV:ehp9. each time PC/2 is started and saved every time you
:hp9.Save:ehp9. your changes.
You can of course overwrite the default configuration filename to a file named
:hp4.Filename.ext:ehp4. by invoking PC/2 with the following commandline option:
:p.:hp4.[-Environment Filename.ext]:ehp4. or
:p.:hp4.[/Environment Filename.ext]:ehp4.
:p.where :hp4.Filename.ext:ehp4. conforms to a filename of a PC/2 profile. Both FAT
and HPFS filename naming conventions are accepted. A few
sample configuration files are provided in this package.
:p.The Configuration data is read from the default initialization profile
:hp9.PC2.INI:ehp9. each time PC/2 is started and saved every time you
:hp9.Save:ehp9. your changes.
You can too overwrite the default configuration filename to a file named
:hp4.Filename.ext:ehp4. by invoking PC/2 with the following commandline option:
:p.:hp4.[-INI Filename.ext]:ehp4. or
:p.:hp4.[/INI Filename.ext]:ehp4.
:p.where :hp4.Filename.ext:ehp4. conforms to a filename of a PC/2 initialization profile.
Both FAT and HPFS filename naming conventions are accepted. A few
sample configuration files are provided in this package.
:warning text='Hint!'.
For above 3 commandline options, the argument specified by :hp4.Filename.ext:ehp4. will
interpreted as the name of a file to be loaded from the directory PC/2 is launched from
unless the specified argument looks like a fully qualified path, that is contains at least
the character :hp4.&colon.:ehp4. or :hp4.\:ehp4..
Thus to request PC/2 to load the configuration data from a arbitrary file, the filename
has to be specified containing a fully qualified path.
:ewarning.
:p.As the default, the Popup Menu appears if you double-click once on the Desktop,
if no option, or the following option is specified:
:p.:hp4.[-DoubleClick]:ehp4. or
:p.:hp4.[/DoubleClick]:ehp4.
:p.You may select a single-click instead, by invoking PC/2 with the following
commandline option:
:p.:hp4.[-SingleClick]:ehp4. or
:p.:hp4.[/SingleClick]:ehp4.
:p.Please be sure not to combine both click behavior options.
, and note that your
selection will also be saved into :hp9.PC2.INI:ehp9...
:p.By default mouse button 1 is used to display the Popup Menu and to switch between
Virtual Desktops, you can request to overwrite this though:
:p.:hp4.[-Button1 | -Button3]:ehp4. or
:p.:hp4.[/Button1 | /Button3]:ehp4.
:p.If you don't specify these options, a user-modifiable default behavior is taken
from :hp9.PC2.INI:ehp9..
Of course the options changing the click behavior and the configuration
filename options may be combined.
:warning text="Note!".
A change of the click behaviour and the mouse button will also be saved
into :hp9.PC2.INI:ehp9. (as if you had changed it in the Desktop Dialog).
:ewarning.
:p.You may disable that PC/2 automatically starts the sessions having
the style :hp9.Autostart:ehp9., but PC/2 does preserve this style in
the configuration file saved.
This may prove useful when customizing the Popup Menu.
To disable the :hp9.Autostart:ehp9. style for the current invocation of PC/2,
invoke PC/2 with the following commandline option:
:p.:hp4.[-NoAutostart]:ehp4. or
:p.:hp4.[/NoAutostart]:ehp4.
:p.You may also select the behavior to ignore the :hp9.Autostart:ehp9. style, by
selection of the :hp9.Ignore Autostart:ehp9. checkbox on the
:link reftype=hd res=1800.<Virtual Desktop Settings>:elink. dialog.
:warning text="Note!".
If you autostart more than about 10 sessions simultaneously, that is at exactly the
same time (e.g. at x seconds after PC/2 was launched, where PC/2 allows you
to choose x between 0 and 3600 seconds in steps of a few seconds) then PC/2
may run out of stack space, causing PC/2 to crash.
Thus it is strongly recommended not to autostart more then 10 items
simultaneously!
:ewarning.
:p.You may disable that PC/2 automatically schedules the sessions having
the defined an scheduling event, but PC/2 does preserve this style in
the configuration file saved.
This may prove useful when customizing the Popup Menu.
To disable the Scheduler for the current invocation of PC/2,
invoke PC/2 with the following commandline option:
:p.:hp4.[-NoScheduler]:ehp4. or
:p.:hp4.[/NoScheduler]:ehp4.
:p.You may also select the behavior to ignore scheduling requests, by
selection of the :hp9.Ignore Scheduler:ehp9. checkbox on the
:warning text="Note!".
A change of the Autostart and Scheduler behaviour will not be saved
into :hp9.PC2.INI:ehp9. (that is the next time you launch PC/2 you have
to specify them again or the values from :hp9.PC2.INI:ehp9. will be taken).
:ewarning.
:p.You may also reduce the time required for PC/2 to terminate (either when
exiting it or as a result of an OS/2 Shutdown) by specifying the following
commandline option:
:p.:hp4.[-NoCleanup]:ehp4. or
:p.:hp4.[/NoCleanup]:ehp4.
During its termination, PC/2 will then not clean up the resources used by
its Popup Menu which significantly can reduce the time required, as no memory
deallocation is performed and PM SIQ is not blocked shortly during the
descruction of the Popup Menu (OS/2 fill clean up both when PC/2's process
has terminated).
:p.You may also want the user to be presented questions or informational
messages by the following commandline option:
:p.:hp4.[-NoPrompt]:ehp4. or
:p.:hp4.[/NoPrompt]:ehp4.
Instead of asking or informing the user default values will be taken
automatically, thus technical details of the PC/2 installation may be hidden
from the user that way which might be usefuly in a centrally managed
LAN/WSOD environment.
:p.To prevent PC/2 changing to the root directory of some drives before
launching a session you can use the commandline option:
:p.:hp4.[-ReserveDriveLetter Drives]:ehp4. or
:p.:hp4.[/ReserveDriveLetter Drives]:ehp4.
Specifying this option (e.g. -ReserverDriveLetter FG) allows you to
specify Drives (e.g. F&colon. and G&colon.) to be excluded by PC/2 when
changing into the root directory of all drives before a session
gets launched. This should help to workaround buggy driver like
e.g. the "OAD" parallel port ZIP driver (which will not return
an error if the drive is empty as e.g. CD-ROM drivers do, so
PC/2 will try to change directories causing nasty Popup Messages
similar to doing a DIR A: to an empty A: drive.
:p.Because some people use PC/2 as a WPS replacement, PC/2 contains a
Menuitem to ShutDown OS/2. By selecting this entry, it is safe to power off your PS/2 or
PC after all disk activity has been stopped, although no windows are closed.
And with the PowerDown OS/2 option you can even suspend or turn off your
workstation.
If you run PC/2 as a WPS replacement, you may want to start the WPS either by
starting :hp4.PMSHELL.EXE:ehp4. from a command prompt or by starting a
:hp4.PMSHELL:ehp4. Menuitem with PC/2.
You then can follow the WPS initialization by heavy disk access and
buildup of your Desktop with objects.
PC/2 requires to "see" the Desktop (in technical terms the window handle
of the Desktop) to be able to catch mouse button 1 or mouse button 3 clicks.
If you start the WPS, the Desktop also undergoes changes too.
Therefore, PC/2 checks regularly if the
Desktop has changed, so you may have to wait a little bit
after the WPS was started, before PC/2 recognizes mouse clicks again..
:p.I would strongly recommend starting PC/2 during OS/2 boot. If you don't
want to use the default profile, don't forget to add :hp4./Profile Filename.exe:ehp4.
to the :hp4.Parameters:ehp4. entryfield in the Settings of PC/2.
:p.The reason why I wrote this utility was primarily that many programs
installed on a system require many folders and icons on the Desktop. But to
start a program, I don't want to open and close folders, so I made shadows on
the Desktop messing it up, and adding overhead, thereby slowing things down.
:p.Using PC/2 as a WPS replacement, you will get:
:ul compact.
:li.a remarkable performance increase and boot time decrease, especially on systems
equipped with less than 8 MB memory,
:li.improved overall system performance and response times due to less
swapping.
:li.the ability to run multimedia or network software with acceptable
performance, even with 8 MB or less,
:li.or the ability to create a maintenance partition (for example by using
:link reftype=hd res=4014.<BOOTOS2>:elink.) without losing immediate
access to your favorite programs.
:eul.
:p.The solution I prefer, is to click on the Desktop - this gives me an
immediate Popup Menu, from which I can easily select an application to be launched from
this menu, or alternatively launch (or switch) to an application by simply pressing
a Hotkey.
So PC/2 will help you to start your programs more quickly, without needing to open
and close folders, and all the overhead of the WPS. Of course, to gain this speed, and
and overhead savings, you lose some of the "niceties" of the WPS, for instance, you can't
start applications from icons.
:p.:link reftype=hd res=4012.<Backward>:elink.
                                :link reftype=hd res=4009.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected concepts from the PC/2 main help panel.                            *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4009.Concepts of PC/2
:p.The basic idea behind PC/2 is to have a configurable Popup Menu available
to start programs quickly, without having to open and close folders.
:p.The Popup Menu, popped up when you click with mouse button 1 or mouse button 3 anywhere or
anywhere on the Desktop (just like button 2 for the Desktop's Context Menu), consists
of :hp4.Menu items:ehp4. (sometimes called :hp4.Menuentries:ehp4.).
An item may be a Submenu, which contains further items, or the title of
an Application, or Menucontrol items. A Submenu contains of a little arrow right
beside the title of the Submenu. (Mouse button 1 is on most systems the left
mouse button.)
:p.To change the font of the Popup Menu, drag a font from the Font Palette to
the smarticon bar of PC/2's Overview Window. This font will be saved when you
exit PC/2.
:p.You can imagine this Popup Menu as a tree, with Submenus as branches,
and applications as leaves on
a particular branch. PC/2 allows you to freely build your own tree. To do so
you have to click on the Desktop with the mouse button 1 or mouse button 3 and select
:hp4.Setup&rarrow.Config Menu:ehp4. from the Popup Menu. A dialog box appears, where you
can add, modify or remove Submenus and Applications. The listbox displays all
Submenus and Applications of one level (like a cut through the tree). Of
course you may change this level up and down.
:p.Note: You must have defined the Menu :hp4.Configure Menu:ehp4. before PC/2 will display
its Popup Menu on mouse button 1 or mouse button 3 clicks. If you don't define it, PC/2
displays its Setup dialog after each click until you have added this control.
:p.:link reftype=hd res=4008.<Backward>:elink.
                                :link reftype=hd res=4010.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for starting a program from PC/2.                             *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4010.Help for starting a program via PC/2
:p.After you have configured the PC/2 Popup Menu, you click on the Desktop
with mouse button 1 or mouse button 3, select the Application you want to start and click on it.
The Application will be started as if you were clicking on an icon.
:p.One advantage starting an Application this way is that you can manipulate
many parameters governing how the Application is started.  This is one thing you can't do with
many icons. E.g., you can start it invisibly (useful for a backup program), and making
it only visible by selecting it from the window list. You may also specify the size
of the Application or that you want to start it in the background. You can even set its priority
or define a Hotkey to switch to it.
:p.You can also start WPS Objects that are not Folder (WPFolder class) or Program
(WPProgram class) type WPS Objects, if they have an ObjectID defined.
Most OS/2 WPS Objects get installed with a predefined ObjectID by default during
installation (see :hp4.OS2_20.RC:ehp4. for a more detailed information), and many
applications (e.g. IBM C Set++, Toolkit, ...) defined an ObjectID for their WPS
Objects too.
:p.Folder and Program WPS Objects and any other WPS Object that has a defined
ObjectID can be dumped automatically by WPS 2 PC/2, to create corresponding
Popup Menu entries.
:p.You can also open any WPS Folder by PC/2 after dragging the folder onto PC/2
to create a corresponding Menuentry.
:warning.
You can start WPS Objects via their ObjectID and open folders only if the WPS is running,
because only the WPS can process the request made by PC/2 to open them.
:ewarning.
:p.To close an Application, select the corresponding option from the Application
or terminate it from the Window List.
Alternatively, you may use PC/2's :hp9.Kill:ehp9. smarticon to close the application's window.
:p.:link reftype=hd res=4009.<Backward>:elink.
                                :link reftype=hd res=301.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Configuration/Setup PC/2 dialog box.                 *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=301.Help for PC/2 Setup dialog
:p.The PC/2 Setup dialog is the entry to configure the Popup Menu.
:artwork name='Source\Resource\SetupDialog.bmp' align=left.
:p.In the listbox, all Submenus and Applications of one level (imagine a cut
through the tree) are displayed. A Submenu has an arrow right beside it.
:p. For instance, assume
the listbox contains 5 Submenus and 2 Applications resulting from the following Popup Menu:
:artwork name='Source\Resource\PopupMenu.bmp' align=left.
:p.Select the item :hp4.Config Menu:ehp4. in :hp4.PC/2 Setup:ehp4. to configure the
items appearing in the Popup Menu. The :hp9.Setup:ehp9. dialog appears, allowing you
to adjust all items of the current level.
:p.You may want to get into the Submenu :hp4.Prompts:ehp4.. Just select the Submenu
:hp4.Prompts:ehp4. and click on the button :hp4.Level Down >>:ehp4.. The listbox
will now be filled with all Submenus and Applications of the Submenu :hp4.Prompts:ehp4..
To get one level up, simply press :hp4.<< Level Up:ehp4.. If nothing seems to
change, you may be either at the highest level (the root of the tree) or at
the deepest level (a leaf of the tree) of a Submenu.
:p.To configure the Popup Menu, just select an item and/or invoke functions by
selecting the following pushbuttons.
:dl tsize=12 break=fit.
:dthd.:hp4.Pushbutton&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Level Up:ehp9.
:dd.Selecting :hp9.Level Up:ehp9. chains up into the next upper Submenu level, unless
the root level is reached.
:dt.:hp9.Level Down:ehp9.
:dd.After selecting a Submenu, click on :hp9.Level Down:ehp9.. The listbox contents
are replaced with the Menuentries of the Submenu.
:p.You may alternatively doubleclick on a Submenu to change to.
:dt.:hp9.Add Program:ehp9.
:dd.To add an application, press the button :hp9.Add Program:ehp9..
The :hp4.Program Installation:ehp4. dialog is displayed, allowing you to add a new
Application. After pressing :hp4.Ok:ehp4. in that dialog, the
Application will be inserted :hp2.after:ehp2. the selected item, or at
the end of the list, if no item was selected.
:dt.:hp9.Add Control:ehp9.
:dd.To add a Control press the button :hp4.Add Control:ehp4., and a dialog
appears, allowing you to add Menucontrols predefined by PC/2.
:dt.:hp9.Add Submenu:ehp9.
:dd.To add a Submenu, press the button :hp9.Add Submenu:ehp9..
The :hp4.Menu Installation:ehp4. dialog is displayed, allowing you to add a new
Submenu. After pressing :hp4.Ok:ehp4. in that dialog, the Submenu
will be inserted either :hp4.after:ehp4. the selected item, or at
the end of the list, if no item was selected.
:dt.:hp9.Clone Program:ehp9.
:dd.This pushbutton allows you to easily clone the selected Menuentry, if the
Menuentry represents an application.
The clones item will be inserted just after the item selected before selecting
this pushbutton.
:p.The selected Menuentry is duplicated, except for the Hotkey, as a single
Hotkey is not valid for multiple menuentries.
:dt.:hp9.Change Entry:ehp9.
:dd.To change a Menuentry that corresponds to an executable application, select
the application and then invoke :hp9.Change Entry:ehp9. to display the
:hp4.Program Installation:ehp4. dialog.
:p.Alternatively, you may just doubleclick on a Menuentry to display the
:hp4.Program Installation:ehp4. dialog.
:p.Only for the Lockup PC/2, Exit PC/2, PowerDown OS/2 and ShutDown OS/2 Menuentries, the
:link reftype=hd res=1650.<Control Configuration>:elink..
dialog is displayed, where you can use the PC/2 Scheduler to automatically
invoke these Menuentries.
:dt.:hp9.Remove Entry:ehp9.
:dd.To remove an application or an empty Submenu, select the Menuitem and click
on the button :hp9.Remove Entry:ehp9..
:warning text='Note:'.
You can not cut a :hp9.Break Separator:ehp9. nor can you cut or remove the
menuentry before a Break Separator, you have to remove the Break Separator first.
This limitation (due to the current implementation within PC/2) comes from the
fact that though a Break Separator is handled as a menuentry, but is actually a
style of the previous menuentry (I was thinking about an implementation in that
way, but decided against that for consistency with the :hp9.Menu Separator:ehp9.).
:ewarning.
:dt.:hp9.Resort Menu:ehp9.
:dd.To resort a Submenu press the button :hp4.Resort Menu:ehp4..
A new dialog is displayed allowing you to resort the currently listed Submenu.
Press here to get more information about the
:link reftype=hd res=2600.<Resort>:elink. dialog.
:dt.:hp9.Cut/Paste:ehp9.
:dd.This pushbutton is the interface between the Popup Menu Setup dialog and
a PC/2 internal clipboard to cut and paste a Menuentry.
Using this clipboard allows you to move a Menuentry (Program, Control or
Submenu) from the current level of the Popup Menu to any other level.
:p.To move a Menuentry, use the following procedure:
:ol compact.
:li.Select the Menuentry you want to remove from the current level of the
Popup Menu.
The Menuentry may be a Program, a Control (except a Break Separator) or a
Submenu (which includes all Menuentries within this Submenu).
:li.When the Clipboard is empty, the :hp9.Cut:ehp9. pushbutton is available,
and you can select it to cut your selection into PC/2's Popup
Menu clipboard.
:li.If you really want to cut your selection, select :hp4.Ok:ehp4. from
the verification message box.
:li.Your selection will then be removed from the Popup Menu.
Remember that your Popup Menu must contain the
:link reftype=hd res=301.<PC/2 Setup>:elink. dialog (which
you are reading the help for).
If you remove its Menuentry :hp9.Configure Popup Menu Settings:ehp9.,
the Popup Menu will not be displayed therefore.
:li.The :hp9.Cut:ehp9. pushbutton will change to a :hp9.Paste:ehp9.
pushbutton, because the PC/2 clipboard does now contain the Menuentry
you have cut.
:li.Use the PC/2 Setup dialog to change to the level of the Popup Menu
you want to paste the cut selection into.
:li.Select the :hp9.Paste:ehp9. pushbutton to paste the PC/2 clipboard
after the selected Menuentry, or at the end of the Submenu if no Menuentry
was selected.
:li.The PC/2 clipboard contents will be added to the Popup Menu, and
the :hp9.Paste:ehp9. pushbutton changes back to :hp9.Cut:ehp9., as the
PC/2 clipboard is now empty again.
Now you can repeat all above steps for another Menuentry.
:eol.
:p.You may ask why you can only cut and paste a single selection at once?
The answer is that this ensures that you don't accidentally lose your
previous clipboard contents.
:warning text='Caution!'.
If you close PC/2 while the PC/2 clipboard contains
a Menuentry, this Menuentry will not be saved and is lost.
Ensure that you paste the PC/2 clipboard contents before exiting PC/2
to avoid data losses.
:ewarning.
:warning text='Note:'.
You may of course close the PC/2 Setup dialog
after having cut a Menuentry into the clipboard.
Display the PC/2 Setup dialog again later on, to paste the Menuentry from
the clipboard.
:ewarning.
:warning text='Note:'.
You can not cut a :hp9.Break Separator:ehp9. nor can you cut or remove the
menuentry before a Break Separator, you have to remove the Break Separator first.
This limitation (due to the current implementation within PC/2) comes from the
fact that though a Break Separator is handled as a menuentry, but is actually a
style of the previous menuentry (I was thinking about an implementation in that
way, but decided against that for consistency with the :hp9.Menu Separator:ehp9.).
:ewarning.
:dt.:hp9.Import:ehp9.
:dd.Use this pushbutton to import a Menuentry (Program, Control or
Submenu) from a file.
You may get such a file from using the :hp9.Export:ehp9. option or download
it from e.g. the PC/2's homepage on the Internet.
:dt.:hp9.Export:ehp9.
:dd.Use this pushbutton to export a Menuentry (Program, Control or
Submenu) into a file.
You may find this option useful to backup a part of your Popup Menu, or if
you use multiple Popup Menus (on e.g. multiple machines) to share common
parts.
:p.
Of course, you can still use a standard editor like e.g. EPM to make such
manipulations to the Popup Menu, but you could easily damage your Popup
Menu then, that's why I never have recommended to do that (another
reason why you should keep a recent backup, which PC/2 will offer to do
after some modifications anyway).
Using the :hp9.Import:ehp9. and :hp9.Export:ehp9. functionality greatly
reduces the risk damaging your Popup Menu.
:edl.
:p.Sometimes it is comfortable to add or remove one or more (by holding
down the :hp4.CTRL:ehp4. key while making your selections)
:link reftype=hd res=1550.<Environment Spaces>:elink. for more than
one item simultaneously (the
:link reftype=hd res=1200.<Program Installation Dialog>:elink.
allows you to edit only the selected Menuentry).
To do so, select one ore more Environment Spaces from the
:hp9.Environment Spaces Management:ehp9. listbox additional to a Menuentry
in the :hp9.Submenu/Program Configuration:ehp9. listbox.
Then press :hp9.Add:ehp9. or :hp9.Remove:ehp9. to add or remove the
selected Environment Spaces from the selected Menuentry.
:p.As Environment Spaces are only valid for OS/2 executables, applying
Environment Spaces to all other types of Menuentries is ignored without any
error message.
If the selected Menuentry is a Submenu, then the Environment Spaces operation
will recursively be applied to all Menuentries (which are OS/2 executables)
within that Submenu.
This lets you configure Environment Spaces for a group of Menuentries with
just a few mouse clicks!
You may consider applying Environment Spaces to a Submenu after extracting
a folder with WPS 2 PC/2 from your OS/2 Desktop.
:p.For more information about defining Environment Spaces select the topic
:link reftype=hd res=2200.<Managing Environment Spaces>:elink..
:dl tsize=12 break=fit.
:dthd.:hp4.Pushbutton&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Add:ehp9.
:dd.Select this pushbutton to add the selected Environment Space(s) to the
selected Menuentry.
If an Environment Space is already defined for a Menuentry, this single request
will just be ignored.
:dt.:hp9.Remove:ehp9.
:dd.Select this pushbutton to remove the selected Environment Space(s) from
the selected Menuentry.
If an Environment Space is not defined for a Menuentry, the request removing
this Environment Space will just be ignored, without effecting the other Environment
Spaces to be removed.
:edl.
:p.If the listbox is empty, you can't remove or change anything. You also can't
select any item, but in this case, you can add a Submenu or an Application by
just pressing the buttons, without needing to select an item.
:dl tsize=12 break=fit.
:dt.:artwork name='Source\Resource\Wps2Pc2.bmp' runin align=left.(WPS 2 PC/2's icon)
:dd.By using the listbox on this dialog page together with WPS 2 PC/2, you can
easily convert WPS Objects into Menuitems for PC/2's Popup Menu.
Assuming that you have extracted details about WPS Objects by dropping them
onto WPS 2 PC/2's icon on the Desktop, just drag them from WPS 2 PC/2's
:hp9.Status:ehp9. bitmap and drop it onto the :hp9.Submenu/Program Configuration:ehp9.
listbox.
PC/2 then generates Popup Menu items automatically and adds them to your Popup Menu.
This lets you copy the settings of WPS Objects easily, without entering them
manually.
:p.When importing the parameters of WPS Objects, PC/2 does remove parameters which
make only sense withing the WPS (due to the WPS's object oriented design which does
not apply to PC/2's Popup Menu).
These parameters are in detail&colon. :hp2.%:ehp2., :hp2.%*:ehp2., :hp2.%**P:ehp2., :hp2.%**D:ehp2.,
:hp2.%**N:ehp2., :hp2.%**F:ehp2. and  :hp2.%**F:ehp2., however :hp2.[...]:ehp2.,
used to query commandline parameters at run-time, will be migrated.
You can find more information about the WPS specific parameters in the context
help for :hp2.Parameters:ehp2. of WPS Objects, just open the settings of a WPS Program Object,
position the input focus at the Parameters entryfield and press :hp4.F1:ehp4. key or
the :hp4.Help:ehp4. pushbutton to display the context help.
:p.For more information about WPS 2 PC/2 just select help from the
WPS 2 PC/2 WPS Object, which you can install by PC/2's
installation program, if it is missing.
:edl.
:p.Once you have configured your tree of Submenus and Applications, just press
:hp4.Save:ehp4. to bring them into effect and save them to the profile, :hp2.or:ehp2.
:hp4.Cancel:ehp4. to make them temporarily effective, but without saving them
into the profile :hp9.PC2.CFG:ehp9. to disk.
If your :hp9.PC2.INI:ehp9. file is corrupted, PC/2 assumes default values, which
you then can edit and save to a new INI file, if :hp9.PC2.CFG:ehp9. is corrupted,
you may lose the defined Popup Menu.
To prevent possible data loss (unlikely that this will happen, but no software really
is perfect ;-) you should regularily backup your configuration by selecting the
:hp4.Backup:ehp4. pushbutton, which will backup your :hp9.PC2.INI:ehp9. to
:hp9.PC2-INI.BAK:ehp9., :hp9.PC2.CFG:ehp9. to :hp9.PC2-CFG.BAK:ehp9. and
:hp9.PC2.ENV:ehp9. to :hp9.PC2-ENV.BAK:ehp9..
:p.:link reftype=hd res=4051.<Example 1>:elink.
     :link reftype=hd res=4052.<Example 2>:elink.
:p.:link reftype=hd res=4009.<Backward>:elink.
                                :link reftype=hd res=2700.<Forward>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected example 1 from the Setup PC/2 help.                                *
.*--------------------------------------------------------------------------------------*

:h2 res=4051.Setup example 1
:p.Assume the listbox contains the following items resulting in the following Popup Menu:
:artwork name='Source\Resource\PopupMenu.bmp' align=left.
:p.You want to remove the Submenu :hp4.Communication:ehp4., rename the Submenu
:hp4.Tools:ehp4. to :hp4.Utilities:ehp4. and add a Submenu :hp4.Demos:ehp4..
:ol compact.
:li.Select the item :hp4.Communication:ehp4.. You can now press the button
:hp4.Remove Entry:ehp4. to remove the Submenu, but only if this Submenu is empty.
You will be prompted if you are really sure to remove the selected Menuentry.
If you confirm the deletion, the while Menuentry will be deleted, even if
it is a non empty Submenu.
:li.To rename :hp4.Tools:ehp4. to :hp4.Utilities:ehp4. select the item
:hp4.Tools:ehp4. and press the button :hp4.Change Entry:ehp4.. A dialog box
appears allowing you to rename this item.
:li.Select the item, after which you want to insert the new Submenu. Then press
the button :hp4.Add Submenu:ehp4. to add one. A dialog box appears allowing you
to enter the title of a new Submenu.
:eol.

.*--------------------------------------------------------------------------------------*
.* The user selected example 2 from the Setup PC/2 help.                                *
.*--------------------------------------------------------------------------------------*

:h2 res=4052.Setup example 2
:p.Assume that you have remove all items, leading to an empty listbox. Now
you want to add the Applications :hp4.Backup:ehp4. and :hp4.Terminal:ehp4..
:ol compact.
:li.The listbox is empty, so you can't select any item. Just press the
button :hp4.Add Program:ehp4. and a dialog box appears, allowing you to
enter all the data required for the new Application.
:li.Now the listbox contains the item :hp4.Backup:ehp4., select it and
press the button :hp4.Add Program:ehp4. a second time. Again you get
a dialog box, where you enter the data for the Application :hp4.Terminal:ehp4..
:eol.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Menu installation PC/2 dialog box.                   *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=2700.Help for PC/2 Menu Installation dialog
:p.This dialog is used to get a name for a Submenu, which is then added to the
Popup Menu.
:artwork name='Source\Resource\MenuInstallationDialog.bmp' align=left.
:p.This dialog is a notebook consisting of the following pages:
:dl tsize=12 break=fit.
:dthd.:hp4.Page&colon.
:ddhd.Tab title&colon.:ehp4.
:dt.:hp9.:link reftype=hd res=2800.<Submenu>:elink.:ehp9.
:dd.This page is used to define a Submenu's name and a file its icon is loaded from.
(As shown in the graphics, you may use Drag &amp. Drop to define the Submenu's icon,
by default a standard OS/2 folder icon will be used.)
:edl.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered.
:p.:link reftype=hd res=301.<Backward>:elink.
                                :link reftype=hd res=1200.<Forward>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected examples from the Menu installation PC/2 help.                     *
.*--------------------------------------------------------------------------------------*

:h2 res=2800.Submenu
:p.You can enter whatever you like for the name of a Submenu, for example to create a
Submenu that contains OS/2 utilities you might enter :hp4.Utilities:ehp4..
:dl tsize=12 break=fit.
:dthd.:hp4.Option&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Submenu Title:ehp9.
:dd.This entryfield contains the name of the current Submenu.
:dt.:hp9.Icon File:ehp9.
:dd.If you specify the filename of an icon file, the icon representing this Submenu
in the Popup Menu (when having activated
:link reftype=hd res=1800.<icons in the Popup Menu>:elink.) is loaded from
that file.
If not specified, or the icon could not be loaded, a default Submenu icon will be loaded.
:dt.:hp9.Drag &amp. Drop support:ehp9.
:dd.You can either enter the fully qualified path where to find the icon file, or insert
a filename (that must contain the extension :hp4..ICO:ehp4. then) via Drag &amp. Drop
from your favourite file manager.
:edl.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation PC/2 dialog box.                *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=1200.Help for PC/2 Program Installation dialog
:p.The PC/2 Program Installation dialog is used to configure all Menuentries that
are not Submenus or Menucontrols.
:artwork name='Source\Resource\ProgramInstallationDialog.bmp' align=left.
:p.This dialog is a notebook consisting of the following pages:
:dl tsize=12 break=fit.
:dthd.:hp4.Page&colon.
:ddhd.Tab title&colon.:ehp4.
:dt.:hp9.:link reftype=hd res=1250.<Application>:elink.:ehp9.
:dd.This page is used to define an application's title, path, filename,
working directory, command-line parameters, an optional file to load
an icon from and DOS settings (for DOS sessions).
:dt.:hp9.:link reftype=hd res=1300.<Session>:elink.:ehp9.
:dd.This page determines the type of session you want to add. Select
between PM, OS/2, DOS and WIN-OS2 sessions.
:dt.:hp9.:link reftype=hd res=1400.<Style>:elink.:ehp9.
:dd.This page can be used to define the appearance of an application. If the
application doesn't overwrite attributes, you may define the style, size and
position.
(The notebook page supports configuration via Drag &amp. Drop, that is, you may drag an
executable to configure the application data and an icon to configure an icon file
onto this menuentry.)
Or you can open the standard OS/2 file dialog to locate the application you want
to configure
:dt.:hp9.:link reftype=hd res=1450.<Hotkey>:elink.:ehp9.
:dd.This page is used to define a Hotkey which will switch to the application
and bring it into the foreground.
:warning.
If you don't have filled in the :hp9.Window Name:ehp9. entryfield correctly
on this notebook page, you will not be able to use PC/2 features successfully,
including Hotkeys, specifying the sessions startup position or keeping the
session on the active Desktop.
I strongly recommend to read the online help, including the Frequently
asked chapter, carefully.
:ewarning.
:dt.:hp9.:link reftype=hd res=1500.<Priority>:elink.:ehp9.
:dd.This page can be used to overwrite the default priority assigned to an
application.
:warning text='Caution:'.
Changing the priority may cause serious problems, because
the application may be set to consume the full CPU!
:ewarning.
:dt.:hp9.:link reftype=hd res=1550.<Environment>:elink.:ehp9.
:dd.This page is used to add environment variables to, or to replace the
environment inherited from PC/2 to the application started from
PC/2's Popup Menu.
:dt.:hp9.:link reftype=hd res=1600.<Scheduler>:elink.:ehp9.
:dd.This page is used to define the time settings the scheduler built into PC/2
automatically launches the application.
:edl.
:p.For more information about one of the above notebook pages, select:
:ul compact.
:li.:link reftype=hd res=1250.<Application Settings>:elink.
:li.:link reftype=hd res=1300.<Session Settings>:elink.
:li.:link reftype=hd res=1400.<Style Settings>:elink.
:li.:link reftype=hd res=1450.<Hotkey Settings>:elink.
:li.:link reftype=hd res=1500.<Priority Settings>:elink.
:li.:link reftype=hd res=1550.<Environment Settings>:elink.
:li.:link reftype=hd res=1600.<Scheduler Settings>:elink.
:eul.
:p.:link reftype=hd res=4050.<Example 1>:elink.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=2700.<Backward>:elink.
                                :link reftype=hd res=2900.<Forward>:elink.


.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Application notebook page.      *
.*--------------------------------------------------------------------------------------*
:h2 res=1250.Application Settings
:p.This dialog is used to get the title, the executable (with or without its fully
qualified path), the working directory and parameters of a Menuitem, which is then
added to the Popup Menu.
:warning text='Hint!'.
PC/2 support the use of Makros in the fields :hp9.Executable:ehp9., :hp9.Workdirectory:ehp9.
and :hp9.Parameters:ehp9..
Those Makros, in the form of :hp4.%MAKRO%:ehp4. will be replaced by the Value of a
corresponding Environment Variable, e.g. assume you have specified :hp4.%STAROFFICE%\SOFFICE.EXE:ehp4.
and the Menuentry's environment contains the Environment Variable :hp4.STAROFFICE=T&colon.\SO51-US:ehp4.
then it will be converted to :hp4.T&colon.\SO51-US\SOFFICE.EXE:ehp4. before passing to OS/2.
:p.
The Environment Variables checks may come from the environment of PC/2 (if you haven't specified
an Environment for the Menuentry yourself) or from the specified Environment.
Please read about :link reftype=hd res=2200.<Managing Environment Spaces>:elink..
and :link reftype=hd res=1550.<specifying an application's Environment>:elink.
for more information about that very powerful feature.
:ewarning.
:p.For a normal user it is the easiest way to use the following guidelines.
According to the type of application you add, some fields may not be editable
or visible:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.Here you enter the name which should appear on the application's titlebar,
which may be overwritten by some applications.
For OS/2 and DOS windows, this will also be the title in the window list.
You may also name the Objects here, you want to add.
:p.For better navigation through the Popup Menu, you may add a tilde (~).
When a Menuentry has a tilde before a letter, the letter is treated as a
mnemonic. After activating the Popup Menu, you can type a mnemonic defined
for a session to start it, instead of selecting it with the mouse or
keyboard.
:p.When defining a mnemonic, ensure that the entryfield :hp9.Window Name:ehp9.
on the :link reftype=hd res=1450.<:hp4.Hotkey Settings:ehp4.>:elink.
page is filled in correctly.
:dt.:hp9.Executable&colon.:ehp9.
:dd.Here you enter executable's filename, optionally including the fully
qualified path - which is only required when the executable's working directory
can't be found in the PATH environment variable - including any modification
due to the Environment and Environment Spaces - and no working directory is
specified. E.g., you might enter
:hp4.cmd.exe:ehp4. to obtain an OS/2 command prompt (as c&colon.\os2\ usually
is in the PATH environment variable in your CONFIG.SYS), or
:hp4.c&colon.\os2\mdos\qbasic.exe:ehp4. to start the basic interpreter.
:p.If the executable is located in the path specified in the
:hp9.Workdirectory:ehp9. entryfield, or locateable through the Environment
or Environment Spaces, you may supply the executable's name excluding path
information.
:p.Here you also enter the name of WPS Objects.
WPS Objects are referred either by their defined Object ID (Identity) e.g
:hp4.<WP_DRIVES>:ehp4. or :hp4.<WP_INST>:ehp4. for preinstalled Objects,
of e.g. :hp4.<WP_USER_CALC>:ehp4. or :hp4.<WP_WIN_PBRUSH>:ehp4. which
can be extracted by the WPS 2 PC/2 WPS Object, unless they
represent Folder or Program objects (then their settings are taken instead
of their ObjectID, which may be available too).
Preinstalled WPS Objects usually have a defined name; you may need to take a
look into the file :hp4.OS2_20.RC:ehp4. to get further information.
:p.PC/2 can only launch WPS Objects named by their defined ID, so be careful
to get either the correct ID from :hp4.OS2_20.RC:ehp4. or by extracting the
WPS Object in question by using WPS 2 PC/2.
You can't use the Program Template to create Objects if you want them to
add to PC/2, because PC/2 can't define the Object ID of such Objects,
so try to use any installation routine that probably creates a Program object
with an ObjectID.
:p.Here you would also enter the path and filename of a seamless WIN-OS2
application. WIN-OS2 applications prefixed by a :hp4.!:ehp4. (exclamation mark)
immediately before the filename get started minimized, applications
prefixed by a :hp4.^:ehp4. (caret) get started maximized.
To start more than one application, separate applications by a
:hp4.,:ehp4. (comma). (By the way, this is a feature of OS/2 and should be therefore
also available for your WIN-OS2 WPS Objects!)
:p.Here you can also enter the application to be started as a detached session.
As detached session have some limitations I recommend to take a short look at
:link reftype=hd res=1300.<Session Settings>:elink. about detached
sessions.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.Here you enter the directory where at least the executable (usually and EXE
file), and usually also the HLP (online help) and DLL (load library) files of your
application are located. Before the application is started, a change drive
and change directory call is made to make this directory the working directory.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.Here you enter the parameters passed to the application. PC/2 allows you
to be asked for commandline parameters before the application is invoked.
Press on :link reftype=hd res=3100.<Commandline Parameters>:elink.
to find out more about this feature. Assume that you want
to use VIEW.EXE to view the command reference.  You would enter:
:p.:hp4.c&colon.\os2\book\cmdref.inf:ehp4..
:p.If you have selected the WIN-OS2 Program Manager :hp4.PROGMAN.EXE:ehp4.
to be started as a seamless WIN-OS2 application, you may enter 1 and only
1 additional WIN-OS2 application.  This application will be started by the Program
Manager in the same WIN-OS2 session. The Program Manager itself will be
started minimized. You may also change the application's style by using
:hp4.!:ehp4., :hp4.^:ehp4. and :hp4.,:ehp4. characters as described in
:p.When importing the parameters of WPS Objects, PC/2 does remove parameters which
make only sense withing the WPS (due to the WPS's object oriented design which does
not apply to PC/2's Popup Menu).
These parameters are in detail&colon. :hp2.%:ehp2., :hp2.%*:ehp2., :hp2.%**P:ehp2., :hp2.%**D:ehp2.,
:hp2.%**N:ehp2., :hp2.%**F:ehp2. and  :hp2.%**F:ehp2., however :hp2.[...]:ehp2.,
used to query commandline parameters at run-time, will be migrated.
You can find more information about the WPS specific parameters in the context
help for :hp2.Parameters:ehp2. of WPS Objects, just open the settings of a WPS Program Object,
position the input focus at the Parameters entryfield and press :hp4.F1:ehp4. key or
the :hp4.Help:ehp4. pushbutton to display the context help.
:hp9.Executable:ehp9..
:dt.:hp9.Icon File:ehp9.
:dd.If you specify the filename of an icon file, the icon representing this Submenu
in the Popup Menu (when having activated
:link reftype=hd res=1800.<icons in the Popup Menu>:elink.) is loaded from
that file.
If not specified, or the icon could not be loaded, a default Submenu icon will be loaded.
:dt.:hp9.DOS Settings&colon.:ehp9.
:dd.Selecting this pushbutton opens the standard OS/2 DOS settings dialog
(OS/2 DOS support must be installed of course). You can then customize the
DOS settings exactly as you would do with a WPS program Object. The settings
are then set into the multiline entryfield. You can of course write them by
hand, but this is discouraged, because of spelling mistakes easily made.
DOS settings are valid for DOS applications, and WIN-OS2 is just a special
DOS application. By default, for a new DOS application the DOS Settings
:hp4.IDLE_SECONDS=5:ehp4. and :hp4.IDLE_SENSITIVITY=100:ehp4. are defined.
:p.As seamless WIN-OS2 sessions are also DOS type sessions, you may strongly
want to adjust the Settings of WIN-OS2 sessions for better WIN-OS2 performance
than the default settings, e.g you may want to increase the memory by
adding DPMI_MEMORY_LIMIT=8.
:dt.:hp4.File Find:ehp4.&colon.
:dd.If you click on this button, the standard file dialog of OS/2 is displayed
where you can select the application you want to install. The application
parameters (e.g path) will then be inserted automatically in the
corresponding entryfields.
:dt.:hp9.Drag &amp. Drop support:ehp9.
:dd.You can alternatively use the Drag &amp. Drop operations from your
favourite file manager or the WPS drives object to create a Program
Popup Menu entry for both the Executable and File icon.
There are 3 basic kind of objects you may want to drop onto this notebook page:
:ul compact.
:li.1. Program objects, which are *.EXE or *.COM files of PM, OS/2 windowed, DOS
or WIN-OS2 applications.
Just select one application from, say, a Drives object or from
a filemanager, drag it into the dialog over any area that accepts it and
drop it.
:li.2. Batch file objects, which are *.CMD or *.BAT type batch files for OS/2 and
DOS command processors (CMD.EXE and COMMAND.COM).
These batch files are automatically installed to be run through a copy of their
command processors (see the online OS/2 command reference for more information
about CMD.EXE and COMMAND.COM).
:li.3. WPS Folder Objects, which are opened by the WPS on request of PC/2 when
selected from the Popup Menu.
If the WPS is not running, PC/2 can't open Menuentries of this type.
:p.You can't create a Menuentry for any WPS object not being a folder object
by dropping it onto this notebook page, you have to use WPS 2 PC/2 in that case
as only WPS 2 PC/2 is able to extract the required data from the WPS.
Data extracted from the WPS by WPS 2 PC/2 can then be dropped onto
PC/2's :link reftype=hd res=301.<Setup dialog>:elink..
:li.4. Icon files, which are files that contain the extension :hp4..ICO:ehp4.,
created e.g. with the icon editor.
:eul.
:p.The notebook settings will be adjusted according to the application (by
analysing the filename) you dragged into.
:edl.
:p.PC/2 of course also supports the use of UNC names.
Additionally you may use the :hp4.%BOOTDRIVE%:ehp4. macro, which gets
replaced by :hp4.d&colon.:ehp4., where :hp4.d:ehp4. is the drive letter
of the drive OS/2 was booted from, the :hp4.%PC2%:ehp4. macro, which
gets replaced by the path PC/2 was launched from or the :hp4.%CDDRIVE:ehp4.
macro, which gets replaced by the drive of the first CD-ROM drive.
For example, using %BOOTDRIVE% allows you to share a single Popup Menu of PC/2 even
when booting from different OS/2 partitions, as you can request PC/2 to
adjust the Popup Menu sessions automagically.
For example, using %PC2% allows you to install applications under PC/2's
directory or to place icons for use in PC/2's Popup Menu in a way you can
easily backup and restore a working environment (e.g. when reinstalling OS/2,
or copying your installation to a different PC).
:p.For a power user, the data entered may be less. Because the OS/2 API
DosStartSession() is used to start the application, all rules for this
API apply as described in the toolkit. Hence you may leave the entryfield
Executable blank and then select from the
:link reftype=hd res=1300.<Session Settings>:elink. group an
OS/2 or DOS fullscreen or window, to get a command prompt.
:p.You may also start an application indirectly through a secondary command
processor by specifying :hp4.c&colon.\os2\cmd.exe:ehp4. as the Executable and
use then the :hp4./C:ehp4. or :hp4./K:ehp4. option of CMD.EXE in the
Parameters entryfield.
(For further information, type :hp4.start view cmdref cmd:ehp4. in an OS/2
command prompt, to get online information about the command processor.)
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Session notebook page.          *
.*--------------------------------------------------------------------------------------*
:h2 res=1300.Session Settings
:p.Here you can select the type of application you want to add.
By default, PC/2 tries to determine the type of an application you installed
via the standard file dialog or via Drag &amp. Drop.
:dl tsize=12 break=fit.
:dthd.:hp4.Option&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Determined by WPS:ehp9.
:dd.This default selection lets the WPS choose the type of application when
started.
:dt.:hp9.OS/2 Full-Screen:ehp9.
:dd.The session is a command session, started in full-screen.
:dt.:hp9.OS/2 Window:ehp9.
:dd.The session is a command session, started in a window on PM.
:dt.:hp9.Presentation Manager:ehp9.
:dd.The session is graphical PM application.
:dt.:hp9.DOS Full-Screen:ehp9.
:dd.The session is a DOS command session, started in full-screen.
:dt.:hp9.DOS Window:ehp9.
:dd.The session is a DOS command session, started in a window on PM.
:dt.:hp9.WPS Object:ehp9.
:dd.The Object-ID of the WPS Object to be started is contained in the entryfield
:hp9.Executable&colon.:ehp9.. The session can only be started when a
Object-ID is assigned to the WPS Object, or if it is the name of a folder object.
:dt.:hp9.Detached:ehp9.
:dd.The session will be started into a detached session.
A detached session has no standard input and standard output, in other words
you can't see the data it writes to the screen, nor can you enter input data.
The session will not even have a visible window, however you can verify that
the session is running by using e.g. the :hp9.PSTAT:ehp9. command.
You may get some more information about detached sessions by looking for
the term :hp9.DETACH:ehp9. in the OS/2 command reference.
:p.To take full advantage of detached sessions the following details about how
detached sessions are launched may be useful:
Technically, only the OS/2 API DosExecPgm() can start detached sessions, however
DosExecPgm() can only start sessions of the same type.
As PC/2 is a PM program it could only detach PM programs, but unfortunately PM
programs fit worst as detached programs, because they usually require user
interaction.
Programs not requireing user interaction are usually windowed command sessions,
e.g. the RC5DES cracking client makes perfectly sense to be detached.
:p.
So, to allow windows sessions to be detached, PC/2 starts a temporary invisible
command processor instead of the program you requested to be detached.
The program you requested to be detached and of course its optional arguments
are added as an argument to that temporary command processor.
Additionally, the argument tells the command processor to execute it detached.
:p.
Complicated? Well consider the following example:
Assume you want to detach a Rexx batch job named :hp4.C&colon.\OS2\Beep.cmd:ehp4..
In the :hp9.Workdirectory:ehp9. you have for sure correctly assumed to enter
:hp4.C&colon.\OS2:ehp4., as batch jobs are executed through command processors
you have of course assumed to enter :hp4./c Beep.cmd:ehp4. in the :hp9.Parameter:ehp9.
and likely also :hp4.C&colon.\OS2\CMD.EXE:ehp4. in the :hp9.Executable:ehp9.
entryfields.
That will finally cause the temporary invisible command processor to be started with the following
argument :hp4./c "detach C&colon.\OS2\CMD.EXE /c Beep.cmd":ehp4., as the sessions
started by PC/2 will inherit the working directory :hp4.Beep.cmd:ehp4. doesn't need
to include a fully qualified path.
:p.
Still complicated? Well consider how the temporarily invoked invisible command processor
will interprete that argument.
:hp4./c:ehp4. tells it to run a command and then silently exit, and that command just says
to detach a program, which in this example is just another command processor, and pass
that program a argument (again, in this example it's a command for the detached command
processor, that is, to run the Rexx batch :hp4.Beep.cmd:ehp4. and then exit silently too).
:p.
Huh, you find that can be improved?
Good! Sure, you would have cleared the :hp9.Parameters:ehp9. entryfield empty and just
have entered :hp4.Beep.cmd:ehp4. in the :hp9.Executable:ehp9. entryfield (yes, detached
sessions are an exception that the :hp9.Executable:ehp9. entryfield can only contain
executables, that is files having the extensions :hp4..COM:ehp4. or :hp4..EXE:ehp4.).
Now it's no surprise that :hp4./c "detach Beep.cmd":ehp4. has been passed as an
argument to the temporary invisible command processor invoked by PC/2.
:p.
Of course, you could have replaced the Rexx batch job for the later method by a
executable like e.g. :hp9.CHKDSK.COM:ehp9..
A funny, but explainable, side effect is (at least to me), that you can also
detach PM programs that way, e.g. just replace the Rexx batch job in the later
example by :hp4.FDISKPM.EXE:ehp4., yes the fixed disk manager will really open
and is fully operable.
:dt.:hp9.Seamless WIN-OS2:ehp9.
:dd.The session is a seamless WIN-OS2 session. Be sure that your graphics
adapter supports such sessions.
You may select between common and separate seamless WIN-OS2 sessions.
:p.Common WIN-OS2 sessions share a single address space, which that only a slight
memory overhead is added for each session, but a single failing WIN-OS2
application can bring down the whole common WIN-OS2 session (causing loss of data
in the non-failing applications).
:p.Separate WIN-OS2 sessions have unique address spaces, which causes the complete
WIN-OS2 environment to be loaded for each session, but a failing WIN-OS2 application
don't bring down other separate WIN-OS2 sessions.
:dt.:hp9.Fullscreen WIN-OS2:ehp9.
:dd.The session is a fullscreen WIN-OS2 session.
You may want to use a fullscreen WIN-OS2 session, if you want to run WIN-OS2
programs under a different resolution than PM programs.
:dt.:hp9.Real Mode:ehp9.
:dd.The seamless WIN-OS2 session is started in Real Mode. Real Mode compatibility
is only provided by OS/2 2.0.
:dt.:hp9.Standard Mode:ehp9.
:dd.The seamless WIN-OS2 session is started in Standard Mode.
:dt.:hp9.386 Enhanced Mode:ehp9.
:dd.The seamless WIN-OS2 session is started in 386 Enhanced Mode. 386 Enhanced
mode compatibility is only provided by OS/2 2.1.
:dt.:hp9.Separate Session:ehp9.
:dd.When checked, the seamless WIN-OS2 session in :hp9.Standard Mode:ehp9.
or :hp9.386 Enhanced Mode:ehp9. is started in a separate session (loading
another copy of the WIN-OS2 environment) otherwise the session is started
into a common WIN-OS2 session (loading the WIN-OS2 environment only once).
:edl.
:p.You generally have to verify that the Program Type setting corresponds
to the application you installed, for most programs you can use
:hp9.Determined by WPS:ehp9. if you aren't sure.
:p.According to the Program Type that is selected, some settings may not
be editable or visible.
:p.OS/2 and DOS Fullscreen or Windows are only used if you want to start
a command prompt as the application. In this case, you would usually enter in the
the entryfield Executable of the group Program Data, the path of
the corresponding command processor. An advanced technique to select the
type of command processor only with these radiobuttons is described under
the group :hp9.Program Type:ehp9..
:warning.
Under OS/2 2.0 GA, seamless WIN-OS2 sessions may only be started with
the WPS running, otherwise the system hangs.
There also seems to be a problem with a real mode session, because
such a WIN-OS2 session starts to load, but doesn't finish successfully.
Under OS/2 2.1 GA you may start seamless WIN-OS2 sessions with or without
the WPS installed.
:ewarning.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Style notebook page.            *
.*--------------------------------------------------------------------------------------*
:h2 res=1400.Style Settings
:p.You can select one of the radiobuttons and any of the checkboxes to
define the style the application should be started with.
:warning.
PC/2 can only request that an application is started by OS/2 with the
selected style, but it can not force the application not to overwrite
these recommendations.
:p.Some applications overwrite the default style settings, for example by
reading them from a profile, and adjust themselves immediately before
becoming visible.
PC/2 won't be able to prevent applications from doing this, for example,
no-one will be able to move PC/2's :hp9.Overview Window:ehp9. outside
of the Desktop, because PC/2 itself rejects such manipulations.
:ewarning.
PC/2 can't force such applications to accept the styles you selected.
:dl tsize=12 break=fit.
:dthd.:hp4.Option&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Restored:ehp9.
:dd.The default style defined by the session itself is used.
:dt.:hp9.Maximized:ehp9.
:dd.The session's default style is modified to maximized.
:dt.:hp9.Minimized:ehp9.
:dd.The session's default style is modified to minimized.
:dt.:hp9.Invisible:ehp9.
:dd.The window created by the application is invisible.
:dt.:hp9.No Autoclose:ehp9.
:dd.The window created by the application is not terminated when the application
shuts down.
:dt.:hp9.Background:ehp9.
:dd.The window is created in the background.
This option may be useful for sessions that get started by a :hp9.Hotkey:ehp9. on a
:hp9.Virtual Desktop:ehp9., to avoid switching the focus from the current Desktop.
:dt.:hp9.Password Protected:ehp9.
:dd.A session marked with this setting will be protected by the :hp9.Access
Password:ehp9. which you have defined on the
:link reftype=hd res=2150.<Security>:elink. notebook page.
When you select to launch this Menuentry from the Popup Menu, you will be
asked the Access Password.
The application will only be launched when that password is entered successfully.
:dt.:hp9.Autostart:ehp9.
:dd.A session marked with this setting will be started automatically by
PC/2 during PC/2's invocation. This feature works similar as the
:hp4.startup folder:ehp4. of the WPS.
:warning text="Note!".
If you autostart more than about 10 sessions simultaneously, that is at exactly the
same time (e.g. at x seconds after PC/2 was launched, where PC/2 allows you
to choose x between 0 and 3600 seconds in steps of a few seconds) then PC/2
may run out of stack space, causing PC/2 to crash.
Thus it is strongly recommended not to autostart more then 10 items
simultaneously!
:ewarning.
:dt.:hp9.Delay:ehp9.
:dd.This scrollbar allows you to select the delay in seconds which, when expired, causes
PC/2 to :hp9.Autostart:ehp9. the application
(when PC/2 is launched the delay begins to expire, however Autostart will not be
invoked for a Menuentry you have changed).
You can select the delay between 0 seconds (which means the application will be launched
when PC/2 has started up) or 1 hour in steps of 5 seconds.
When all Menuentries marked to be autostarted had been autostarted, then no further
checking will be done to avoid unnecessary CPU load.
You have to restart PC/2 I you make changes regarding Autostart for a Menuentry to
get that change effective.
:p.
This option may be useful if you want to e.g.start an application from a mounted drive
(that is a drive which is only available when a control program runs like e.g. LAN Server
or NFS, but not a boot time),
but mounting of that drive takes takes some time to finish.
Then you may want PC/2 to autostart the program that mounts the drive immediately while
the application to be loaded from the mounted drive should be started after some time
to allow the mount to complete (to avoid race conditions, e.g. even though applications more upwards
in the Popup-Menu are launched earlier than applications more downwards in the Popup-Menu
you can't tell exactly which one will run first as this depends on factors like the
applications startup time, OS/2 Scheduler, system load,...).
:dt.:hp9.User defined Position &amp. Size:ehp9.
:dd.The session's default size and position is modified to the values entered.
PC/2 forces the window to the entered position, even when the application overwrites
these values. You may specify a position relative to the origin of the Virtual
Desktops (the "middle" Desktop of the 9 Virtual Desktops), useful when the.
:link reftype=hd res=302.<Virtual Desktop>:elink. feature is
enabled.
:p.
To allow PC/2 to detect the window, ensure that the :hp9.Window Name:ehp9. entryfield
at the :link reftype=hd res=1450.<Hotkey>:elink. page is filled correctly.
:edl.
:p.E.g. if you selected :hp9.Minimized:ehp9., :hp9.Invisible:ehp9. and
:hp9.Background:ehp9. buttons, the application will be started minimized, in the
background and invisible (this may make sense for e.g. a backup program).
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Hotkey notebook page.           *
.*--------------------------------------------------------------------------------------*
:h2 res=1450.Hotkey Settings
:p.PC/2 supports the definition of Hotkeys to switch from the current
application to other applications, or to launch the application if it not already
running, just by pressing a Hotkey.
If the Hotkeyed session is on another Virtual Desktop, PC/2 switches to this Desktop
and switches the application into the foreground.
:warning.
Hotkeys defined are detected by PC/2 systemwide, that is they may be stolen from
any PM application (including OS/2 and DOS windows), unless you have selected :hp9.No
systemwide Hotkeys:ehp9. on the :link reftype=hd res=2050.<General PC/2
Settings>:elink. notebook page (or when OS/2, a device driver like WatchCat or another
application detecting systemwide key presses captures that key before).
:ewarning.
PC/2 supports :hp4.0:ehp4. ... :hp4.9:ehp4. and :hp4.A:ehp4. ... :hp4.Z:ehp4.
in combination with either the :hp4.right CTRL:ehp4.-Key (:hp4.RCTRL:ehp4.),
the :hp4.left and right CTRL:ehp4.-Key, the :hp4.ALT:ehp4.-Key,
the :hp4.left WIN:ehp4.-Key or the :hp4.CTRL:ehp4.+:hp4.ALT:ehp4.-Keys as Hotkeys,
giving you 180 different Hotkeys at maximum.
This set of keys is the optimum compromise between CPU requirements and
keys available on all possible keyboard layouts.
:p.If PC/2 can't detect the application running the pressed Hotkey is defined for,
it starts the application as if it was invoked by PC/2's Popup Menu.
Starting applications by pressing a Hotkey is another alternative delivered by PC/2,
when invoking programs.
:p.PC/2 allows you to mark the application as non-movable around the Virtual Desktop.
By defining this style, the application is always kept on the active Desktop, regardless
what :link reftype=hd res=302.<Virtual Desktop>:elink. you are
switching to.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Window Name:ehp9.
:dd.If you have either marked the application to be kept on active Desktop, defined a Hotkey
or want to start an application at a user defined size and position
you must fill this entryfield with a string that equals or is part of
either the application's name on its titlebar, the application's name in the Window List
pr the application window's class name (though you will only be able to get a window's
class name by using PC/2's :link reftype=hd res=2350.<Spy>:elink. notebook
page or utilities like PMTREE or TWCP).
If you use one of the two features and you leave the entryfield empty, PC/2 copies the
string from the :hp9.Program Title:ehp9. entryfield into the :hp9.Window Name:ehp9.
entryfield, when you press :hp4.Ok.:ehp4. after automatically removing the tilde
defined for the mnemonic.
:warning text='Note!'.
Be sure that the string you entered matches only with the current application, because
the first match of this string with any application's titlebar or its entry in the
window list will be used.
You may want to start your application without these options first, to get its titlebar
name or its entry in the window list.
:ewarning.
:warning text='Hint!'.
To find out a window's name, class and Window List entry most easily,
just use the :link reftype=hd res=2350.<Spy>:elink. notebook page.
:ewarning.
:warning text='Hint:'.
I strongly recommend you fill in this entryfield if you
experience unexpected results after marking the window to be kept on active
Desktop or defining a Hotkey.
However, if you haven't marked the window to be kept on active Desktop and haven't
defined a Hotkey, PC/2 will ignore the entryfield's contents (and when you are not using
both features, this field wouldn't be used anyway).
PC/2 uses the :hp9.Window Name:ehp9. to identify the current sessions for all
running sessions, so a unique identifier is a requirement if you select at least
one feature.
:ewarning.
:dt.:hp9.Sticky Window (Keep on active Desktop):ehp9.
:dd.Checking this flag keeps the window always on the current Desktop regardless
to which :hp9.Virtual Desktop:ehp9. you move.
:warning text='Note:'.
PC/2 will match the :hp9.Window Name:ehp9. with the first window
containing that (sub)string, even if it has not been launched by PC/2 or multiple
instances of that window exist.
If some windows are kept on every Virtual Desktop you can't explain why, most likely
these windows matches with Menuentries where this checkbox has been selected.
:ewarning.
:warning text='Note!'.
Due to user requests, starting with PC/2 version 2.10, windows entered here will also
be added into the :hp9.Sticky Windows:ehp9. feature at the
:link reftype=hd res=1900.<Overview Window Settings>:elink.
notebook page, but the setting here is still supported (as searching the
:hp9.Window Name:ehp9. is the most effort and this option is just another little
comparism).
:p.
Likely, in a future version the :hp9.Sticky Window:ehp9. checkbox may finally disappear
from thist page (when I feel that existing configurations have been migrated).
:ewarning.
:warning text='Warning!'.
As said, since version 2.10 a :hp9.Sticky Window:ehp9. will be added
added into the :hp9.Sticky Windows:ehp9. feature at the
:link reftype=hd res=1900.<Overview Window Settings>:elink. during initialization
of PC/2.
If you delete the entry there, but do not unselect the checkbox here, the deletion will
only be effective while running PC/2, the next time you launch PC/2 it will be added again
as long as the :hp9.Sticky Window:ehp9. checkbox stays checked.
:p.
While I admit this may be confusing, it is a liability of the design changes that had to
be introduced in the more than 6 years history of PC/2.
:ewarning.
:dt.:hp9.Enable Hotkey:ehp9.
:dd.This flag says you want to define a Hotkey for this application. The controls
necessary to define the Hotkey will be enabled.
You may want to select the session started in background, if it will get started
on a :hp9.Virtual Desktop:ehp9., to avoid losing focus from the current Desktop.
:dt.:hp9.Default:ehp9.
:dd.Once you press the Hotkey, the Desktop is switched to the :hp9.Virtual Desktop:ehp9. the
application is located on and activates it.
:dt.:hp9.Maximized:ehp9.
:dd.The same as for :hp9.Default:ehp9. is done, but the applications window is maximized
too.
:dt.:hp9.Restore:ehp9.
:dd.The same as for :hp9.Default:ehp9. is done, but the application's window is restored
too.
:dt.:hp9.RCTRL-Key:ehp9.
:dd.You define a Hotkey which ranges from :hp4.0:ehp4. ... :hp4.9:ehp4. and
:hp4.A:ehp4. ... :hp4.Z:ehp4. in conjunction with the :hp4.right CTRL:ehp4.-Key only.
:dt.:hp9.CTRL-Key:ehp9.
:dd.You define a Hotkey which ranges from :hp4.0:ehp4. ... :hp4.9:ehp4. and
:hp4.A:ehp4. ... :hp4.Z:ehp4. in conjunction with one of the :hp4.CTRL:ehp4.-Keys.
:warning text='Note!'.
You have to be cautious to differentiate the :hp4.CTRL-Key:ehp4. from the
:hp4.RCTRL-Key:ehp4., they are not completely exclusive, because the :hp4.CTRL:ehp4.-Key
consists of both :hp4.CTRL:ehp4.-Keys, the :hp4.left:ehp4. and the :hp4.right:ehp4. one.
The selection :hp4.CTRL:ehp4. means both :hp4.CTRL:ehp4.-Keys, while the selection
:hp4.RCTRL:ehp4. only means the :hp4.right CTRL:ehp4.-Key.
For example, if you have defined a Hotkey for a key :hp4.*:ehp4. together with both
:hp4.RCTRL:ehp4. and :hp4.CTRL:ehp4. keys, don't be surprised when pressing
:hp4.RCTRL+*:ehp4. triggers the Hotkey for :hp4.RCTRL+*:ehp4. and :hp4.CTRL+*:ehp4. simultaneously.
:ewarning.
:dt.:hp9.ALT-Key:ehp9.
:dd.You define a Hotkey which ranges from :hp4.0:ehp4. ... :hp4.9:ehp4. and
:hp4.A:ehp4. ... :hp4.Z:ehp4. in conjunction with the :hp4.ALT:ehp4.-Key.
:dt.:hp9.LWIN-Key:ehp9.
:dd.You define a Hotkey which ranges from :hp4.0:ehp4. ... :hp4.9:ehp4. and
:hp4.A:ehp4. ... :hp4.Z:ehp4. in conjunction with the :hp4.left WIN:ehp4.-Key.
:p.You must select a key in the range between :hp4.0:ehp4. and :hp4.9:ehp4.
or :hp4.A:ehp4. ... :hp4.Z:ehp4. from the dropdown box, which in
conjunction with the :hp4.CTRL:ehp4.-key or the :hp4.ALT:ehp4.-key or
the :hp4.left WIN:ehp4.-key, or the combination of the
:hp4.CTRL:ehp4.+:hp4.ALT:ehp4.-keys, is used to switch to the application.
You can define 144 different keys this way as the maximum.
:dt.:hp9.Current Window List:ehp9.
:dd.To define the style non-movable or the Hotkey, you may start the application first
without these options and grab its titlebar name from its window or its window
list name from the :hp9.Current Window List:ehp9..
:edl.
:p.PC/2 prior to version 1.90, PC/2 could not detect Hotkeys pressed in OS/2
or DOS windows, but for other windows, when you have defined a Hotkey,
you had just to press the key on the keyboard that has the defined Hotkey
character labeled on.
:p.Said otherwise, this approach was NLS-independent. Version 1.90 could detect
Hotkeys pressed in OS/2 or DOS window, but broke the NLS-independence (and could
not detect accelerator keys).
Version 2.00 fixed both problems and is now NLS-independent again besides being
able to detect accelerator keys.
You just select a Hotkey on your keyboard, and exactly that key will be used.
:p.:warning text='Caution!'.
All keys defined as systemwide Hotkeys in PC/2
are no longer available for other applications, because once a key is
defined as systemwide Hotkey no other application except PC/2 gets this key.
This is a limitation of the Hotkey feature, because only this way PC/2
can ensure that if a Hotkey is defined, the Desktop switches to this application.
The Hotkey launches the application, if the application, the Hotkey is defined for,
is not running.
If you need a certain key, which is defined as a Hotkey in PC/2,
change the Hotkey to an unused key.
:ewarning.
:p.:warning text='Hint!'.
Even if a :hp4.Systemwide Hotkey:ehp4. is defined, you may pass
this key to a application, when the :hp4.SHIFT:ehp4.-key is pressed and hold before
the Hotkey.
When PC/2 detects :hp4.SHIFT:ehp4. is pressed together with the Hotkey, it ignores the
Hotkey by passing it to the application that currently has the focus.
:ewarning.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Priority notebook page.         *
.*--------------------------------------------------------------------------------------*
:h2 res=1500.Priority Settings
:p.PC/2 allows you to specify the priority of the applications you start via the
Popup Menu. OS/2 has the following priority classes and deltas to select from:
:dl tsize=12 break=fit.
:dthd.:hp4.Priority Class&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Idle time:ehp9.
:dd.:hp4.Idle-time:ehp4. is the lowest priority class a session or thread can run under OS/2.
You may use this option for task that should only be performed if enough CPU-resource is
available, i.e. for a background NFS (Network File System) Server or BBS.
:dt.:hp9.Regular:ehp9.
:dd.If not specified otherwise, the priority class :hp4.Regular:ehp4. is the default class of all
sessions and threads. For this default class, the priority delta of 0 is the default.
:dt.:hp9.No Change:ehp9.
:dd.This option just doesn't change the priority. The application is started with
priority class Regular and priority delta 0.
:dt.:hp9.Time Critical:ehp9.
:dd.:hp4.Time-critical:ehp4. priority class is used for time-critical sessions. Examples
may be real-time data acquisition.
:dt.:hp9.Foreground Server:ehp9.
:dd.:hp4.Foreground Server:ehp4. is the highest priority class available and should be only
used for small assembly language applications because they really eat up the CPU time.
:dt.:hp9.Priority Delta:ehp9.
:dd.The priority delta selectable between 0 and 31 is defined for each priority class.
I.e., a session with Regular priority class and priority delta 25 has a lower priority
than a session with Time-critical priority class and priority delta 3.
:edl.
:p.:warning text='Extreme caution!'.
Changing the priority from the default selection
may cause serious performance impacts on your system. Be very carefully when
selecting higher priorities, because the higher the priority, the higher the CPU time-slice
a session consumes deteriorating overall system performance.
:ewarning.
:warning text='Note:'.
You can't change the priority of DOS session (and WIN-OS2 sessions are DOS sessions)
because allowing this would lock PM for unknown reasons.
Beginning with Warp 3 you can use the DOS setting DOS_PRIORITY instead.
:ewarning.
:p.For example, if you set an OS/2 command prompt to a Time-critical priority, you may
not be able to switch to other sessions until the command prompt is waiting for input.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Environment notebook page.      *
.*--------------------------------------------------------------------------------------*
:h2 res=1550.Environment Settings
:p.PC/2 allows you to specify the Environment of the applications you start.
If you don't specify any special Environment settings, the
the started application inherits the Environment of PC/2.
First, let me explain how an application's environment is built.
:p.
By default, when a program is started (regardless if started from the WPS, PC/2,
a commandline,...) the started application inherits the environment from the
parent (starting) application.
For the first process started by PM (the so-called :hp2.shell process:ehp2.,
which likely is PMSHELL.EXE or PC2.EXE) the inherited environment is derived
from the environment settings defined in :hp4.CONFIG.SYS:ehp4..
If only the default inheritance is used, all running processes will get
inherited the environment settings defined in CONFIG.SYS, or said otherwise,
what you change in your CONFIG.SYS will be reflected for all processes.
:p.
However the parent application may change the environment the started application
gets inherited (which is, to no surprise, exactly what PC/2 does ;-).
This may be done either by changing its own environment, and these changes will
then get inherited, or by passing the to be started application's environment
to the OS/2 API that actually can launch a new process (which PC/2 does).
:p.
If you have defined such changes in the :hp9.Environment Spaces:ehp9. and/or
the :hp9.Leading:ehp9. and/or :hp9.Trailing:ehp9., PC/2 will merge its own
environment with the environment(s) defined for those changes and will then
pass that merged environment to the OS/2 API launching the application.
The net effect is, that the newly started application will then have an
environment that is modified to the default one defined in CONFIG.SYS.
:p.
PC/2 therefore allows you to modify - that is adding, changing and deleting -
the environment defined in CONFIG.SYS for any number of applications without
having to make modifications to CONFIG.SYS.
You can even run applications with conflicting environments (e.g. different
versions of the JAVA runtime) or even conflicting versions of an applications
(e.g. different versions of IBM's C++ compiler, though this would also
require that DLLs are not shared which is likely only if the names differ)
side by side, without needing changes to or even different CONFIG.SYS files,
as the following examples show.
:p.You may either add settings to the Environment settings that already get inherited
from PC/2, or you may replace settings defined in PC/2's Environment
with a different value, or you may modify existing settings with additional values.
An entry in the Environment specifies the value of a environment variable, and a entry
has therefore the following form:
:p.:hp4.ENVIRONMENTVARIABLE=[[VALUE][%ENVIRONMENTVARIABLE%][VALUE]]:ehp4.
:p.where :hp4.ENVIRONMENTVARIABLE:ehp4. is the name of a environment variable,
:hp4.VALUE:ehp4. is the value it gets assigned and :hp4.[...]:ehp4. specifies,
that this item may be specified 0...n times.
You may replace :hp4.VALUE:ehp4. by anything you want, but ensure that you use
the correct spelling.
:p.There are special predefined environment variables too, that is:
:ul compact.
:li.:hp4.%PC2%:ehp4., which expands to the complete path to where PC2's
executable resides in, for example into :hp4.D&colon.\Tools\PC2:ehp4..
:li.:hp4.%BOOTDRIVE%:ehp4., which expands the the drive OS/2 was booted
from, for example into :hp4.D&colon.:ehp4..
:li.:hp4.%CDDRIVE%:ehp4., which expands the the first CD-DRIVE drive,
for example into :hp4.L&colon.:ehp4..
:eul.
:p.The left side of the environment entry (left to the = sign) specifies
the name of the environment variable the user wants to set.
The right side of the environment entry (right to the = sign) specifies
the value to assign.
:hp4.%ENVIRONMENTVARIABLE%:ehp4. is a macro, which gets expanded to
the value of the same named environment variable of PC/2's environment.
There is a special environment variable named :hp4.PC2:ehp4. defined automatically
by PC/2.
This environment variable points to the path PC/2 was lanched from, e.g. if you
have invoked PC/2 with :hp4.Start D&colon.\Tools\PC2\Pc2:ehp4. then :hp4.%PC%:ehp4.
will contain :hp4.D&colon.\Tools\PC2:ehp4..
This may be useful if you install applications under PC/2's directory, and configure
the e.g. PATH or BEGINLIBPATH/ENDLIBPATH (that is the dynamic LIBPATH) relative to
PC/2's directory, allowing you easily to backup and restore your environment due to
reinstalling OS/2 or copying your environment to other PCs.
:p.If you need to include the token % itself into a environment, just replace
it with the token :hp4.%%:ehp4..
PC/2 will not treat the :hp4.%%:ehp4. token as part of an Environment Variable's name, and
will keep a single :hp4.%:ehp4. token in the Environment.
:p.For example, assume PC/2 inherits the following Environment:
:ul compact.
:li.INCLUDE=C&colon.\IBMCPP\INCLUDE&semi.C&colon.\IBMCPP\IBMCLASS&semi.
:li.CINCLUDE=C&colon.\TOOLKT\C\OS2H&semi.C&colon.\TOOLKT\C\OS2H\VDD&semi.
:li.CPLUSINCLUDE=C&colon.\TOOLKT\CPLUS\OS2H&semi.C&colon.\TOOLKT\CPLUS\OS2H\VDD&semi.
:li.TEMP=C&colon.\TEMP
:li.TMP=C&colon.\TEMP
:eul.
:p.and you make to following modifications in the environment settings page
for a application (using the Leading option):
:ul compact.
:li.INCLUDE=.&semi.%INCLUDE%%CPLUSINCLUDE%C&colon.\TCPIP\INCLUDE
:li.TEMP=D&colon.\TEMP&semi.
:li.TMP=%TMP%&semi.
:eul.
:p.the INCLUDE environment variable of the application will be:
:ul compact.
:li.INCLUDE=.&semi.C&colon.\IBMCPP\INCLUDE&semi.C&colon.\IBMCPP\IBMCLASS&semi.C&colon.\TOOLKT\
CPLUS\OS2H&semi.C&colon.\TOOLKT\CPLUS\OS2H\VDD&semi.C&colon.\TCPIP\INCLUDE
:li.TEMP=D&colon.\TEMP&semi.
:li.TMP=C&colon.\TEMP&semi.
:eul.
:p.Both :hp4.ENVIRONMENTVARIABLE:ehp4. and :hp4.VALUE:ehp4. are case sensitive,
though :hp4.ENVIRONMENTVARIABLE:ehp4. matches a certain environment variable
case insensitive (e.g. InClude matches INCLude).
All environment settings get inherited to the started process with case
sensitivity preserved.
You may notice that the SET command of the OS/2 command processor CMD.EXE
displays all environment variables uppercase, so I recommend to specify
any environment settings to be entered uppercase.
Of course you can use UNC Names in your environment settings too.
:p.For example, to add the environment variable MyProg with the value
MyValue you'd write:
:ul compact.
:li.MyProg=MyValue&semi.
:eul.
:p.or to replace the PATH environment variable of PC/2's
Environment you could write (using the leading option):
:ul compact.
:li.PATH=C&colon.\&semi.D&colon.\MYPROG&semi.D&colon.\MYPROG\DLL&semi.,
:eul.
:p.or to append the path X&colon.\TOOLS&semi. before and Z&colon.\NETWORK\&semi.
after the existing DPATH environment variable of PC/2's Environment, you may write:
:ul compact.
:li.DPATH=Z&colon.\NETWORD\&semi.%DPATH%X&colon.\TOOLS&semi.
:eul.
:p.
You may also include one or more
:link reftype=hd res=2200.<Environment Spaces>:elink. in your Environment.
The following example demonstrates the power of PC/2 setting a session's Environment.
When you launch a Menuentry, PC/2's Environment is taken as the basis for all
modifications.
All the following steps are optional, but the sequence itself can't be changed.
:ol.
:li.If you have defined Leading Environment settings, these environment settings are
merged with the Environment inherited by PC/2 first.
:li.Then all Environment Spaces selected are merged into this Environment one by one.
:li.If you have defined Trailing Environment settings, these environment settings are
merged with the environment merged so far.
:eol.
:p.Let's demonstrate this step by step:
:ul compact.
:li.Environment of PC/2&colon.
.br
&dot.&dot.&dot.
.br
PATH=C&colon.\;C&colon.\OS2;C&colon.\MMOS2;
.br
HELP=C&colon.\;C&colon.\OS2\HELP;
.br
&dot.&dot.&dot.
:li.Environment Space "Compiler"&colon.
.br
&dot.&dot.&dot.
.br
PATH=D&colon.\IBMCPP\BIN;%PATH%;
.br
HELP=D&colon.\IBMCPP\BOOK;%HELP%;
.br
&dot.&dot.&dot.
:li.Environment Space "Toolkit"&colon.
.br
&dot.&dot.&dot.
.br
PATH=D&colon.\TOOLKIT\OS2BIN;%PATH%;
.br
HELP=D&colon.\TOOLKIT\OS2HELP;%HELP%;
.br
&dot.&dot.&dot.
:li.Session's Environment Modifications&colon.
.br
&dot.&dot.&dot.
.br
PATH=E&colon.\TOOLS;%PATH%;
.br
HELP=E&colon.\TOOLS;
.br
&dot.&dot.&dot.
:li.Having the settings of the session's Environment modifications entered as
Trailing Environment settings, the session's Environment will be constructed by:
:ol compact.
:li.The Environment inherited to the session by PC/2 will be taken as the basis.
:li.The Environment Space "Compiler" will be merged, because it is the first
one available.
:li.The Environment Space "Toolkit" will be merged afterwards.
:li.Finally the session's Environment modifications will be merged.
:eol.
:p.The session's Environment will result in:
.br
&dot.&dot.&dot.
.br
PATH=E&colon.\TOOLS;D&colon.\TOOLKIT\OS2BIN;D&colon.\IBMCPP\BIN;C&colon.\;C&colon.\OS2;C&colon.\MMOS2;
.br
HELP=E&colon.\TOOLS;
.br
&dot.&dot.&dot.
:li.Having the settings of the session's Environment modifications entered as
Leading Environment settings, the session's Environment will be constructed by:
:ol compact.
:li.The Environment inherited to the session by PC/2 will be taken as the basis.
:li.The session's Environment modifications will be merged next.
:li.The Environment Space "Compiler" will be merged, because it is the first
one available.
:li.The Environment Space "Toolkit" will be merged afterwards.
:eol.
:p.The session's Environment will result in:
.br
&dot.&dot.&dot.
.br
PATH=D&colon.\TOOLKIT\OS2BIN;D&colon.\IBMCPP\BIN;E&colon.\TOOLS;C&colon.\;C&colon.\OS2;C&colon.\MMOS2;
.br
HELP=D&colon.\TOOLKIT\OS2HELP;D&colon.\IBMCPP\BOOK;E&colon.\TOOLS;
.br
&dot.&dot.&dot.
:eul.
:p.The example above shows the power of defining Environment Spaces to be used
for more than one Menuentry and combining them with additions/modifications to
the Environment that is unique to a single session.
Depending on merging the additions/modifications before or after merging
with any Environment Space, you may virtually be able to create environments
for all programs of even conflicting products.
:p.In addition to the Environment Spaces selected on this notebook page statically,
you may select the :hp9.Verify on Startup:ehp9. checkbox to allow dynamic changes at run-time.
PC/2 will then invoke the :link reftype=hd res=3200.<Startup Environment
Spaces>:elink. dialog immediately before launching the application.
This dialog allows you to dynamically change the Environment Spaces used for
a launch at run-time without changing anything in the Program Installation dialog.
:p.For example, this option may also be useful if you have upgraded to a new version of
a product - let's assume IBM's Internet Connection Server - and you have defined
an Environment Space for the current and previous version.
To see if you can compile your CGI-BIN programs under the new version, you
use the Startup Environment Spaces dialog to switch your compiler environment
from the previous to the current version of the ICS' header files.
:p.When migrating (e.g. by WPS 2 PC/2) a product that contains more than one executable,
you may create a Environment Space for this product and inherit it to all executables.
This avoids you to enter the same environment settings multiple times and allows you
to modify the settings for all executables of the product at a single place:
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Environment Spaces&colon.:ehp9.
:dd.This listbox contains the names of all available Environment Spaces.
You may select exactly the Environment Spaces that are required, however you may
ignore any Environment Spaces not applicable.
:dt.:hp9.Environment Modifications&colon.:ehp9.
:dd.This multiline entryfield contains the environment settings, that are applied
to the Environment inherited from PC/2 either before or after the selected Environment
Spaces.
This control is filled depending on the setting of the :hp9.Leading:ehp9. and
:hp9.Trailing:ehp9. radiobutton.
:p.These settings are merged into the application's Environment immediately before the
application gets started, giving you a chance to override environment variables
inherited from PC/2's Environment.
:dt.:hp9.Leading&colon.:ehp9.
:dd.When selecting this radiobutton, the additions and modifications made in the
:hp9.Environment Modifications:ehp9. multiline entryfield for merging with the Environment
inherited from PC/2 before any Environment Space is merged into, is displayed.
You may use this setting to clear environment variables inherited from PC/2, for example
you may initialize that the HELP environment variable does not include OS/2 help
located at [d]&colon.\OS2\HELP;[d]&colon.\OS2\HELP\TUTORIAL;[d]&colon.\MMOS2\HELP (where
[d] is the drive you installed OS/2 onto).
:dt.:hp9.Trailing&colon.:ehp9.
:dd.When selecting this radiobutton, the additions and modifications made in the
:hp9.Environment Modifications:ehp9. multiline entryfield for merging with the Environment
inherited from PC/2 after any Environment Space is merged into, is displayed.
You may use this setting to accept most, but replace a few, of the environment variables
inherited from the Environment Spaces.
:p.The Environment setting from PC/2 version 1.90- are migrated into the
Trailing Environment.
:edl.
:p.For OS/2 versions 3.00 (WARP) and above, you may also specify the settings
:hp4.BEGINLIBPATH:ehp4. and :hp4.ENDLIBPATH:ehp4., which are not part of the process's
environment, but are actually the process's information area, to be technically exact.
Even if both are not part of the environment, PC/2 and the OS/2 command
processor :hp4.CMD.EXE:ehp4. allow you to treat both settings like environment
variables, because this is more consistent for the user.
:warning text='Note:'.
The :hp4.BEGINLIBPATH:ehp4. and :hp4.ENDLIBPATH:ehp4. settings are not available
for OS/2 2.x, because the Operating System doesn't support it, and will be ignored
for these versions.
:ewarning.
:p.PC/2 allows you to include the Menuentries'
:link reftype=hd res=1800.<Icons>:elink. in the Popup Menu.
To locate the executable to load the icon from, PC/2 will also try to find the
executable in the PATH defined in the Menuentry's Environment Settings.
This removes the need to specify a fully qualified path and a working directory,
if the executable can be found in a directory pointed to by the PATH environment
variable specific for this session (that is, the executable's working directory
does not need to be included in the PATH entry in your CONFIG.SYS, but the
executable's icon gets loaded even then!).
:warning.
You can't replace the :hp4.LIBPATH:ehp4. variable, which, although it's treated like a
environment variable in :hp9.CONFIG.SYS:ehp9., but does not belong to a process's
environment, but to the process itself.
The :hp4.LIBPATH:ehp4. is a systemwide resource.
:ewarning.
:warning.
Although PC/2 allows you to specify environment variables in any case, PC/2 will
match one environment variable with another environment variable when they only
differ in the case of individual letters.
This makes sense as the OS/2 command processor CMD.EXE (that runs OS/2 windowed and
fullscreen sessions) does convert all environment variables to uppercase.
:ewarning.
:warning.
PC/2 allows you to specify environment entries of any length, the OS/2 command
processor CMD.EXE only supports environment entries up to about 1000 characters.
I think there is also an OS/2 limitation of 1024 characters, so ensure that you
don't overcome these limits.
:ewarning.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Program installation Scheduler notebook page.        *
.*--------------------------------------------------------------------------------------*
:h2 res=1600.Scheduler Settings
:p.The PC/2 Scheduler allows you to select one or more regular events, when PC/2 should start
the Menuentry automatically.
Depending on the type of the event you have selected, PC/2 will display listboxes where
you can select from all valid choices.
:warning text='Note!'.
When you invoke the Program Installation dialog, PC/2 ensures that each of the
listboxes have at least 1 selected item.
You can change that selection by clicking onto a different item with mouse
button 1, if you want to select (or deselect) multiple items press and hold
the :hp4.CTRL:ehp4.-key while clicking onto the items with mouse button 1.
However you can deselect all items in a listbox, in that case the Scheduler will
never match the current time with your data and the application will not be
scheduled.
:ewarning.
:dl tsize=12 break=fit.
:dthd.:hp4.Event&colon.
:ddhd.Resulting function&colon.:ehp4.
:dt.:hp9.Disabled&colon.:ehp9.
:dd.Selecting the Disabled pushbutton, disables the scheduling of the selected events.
The selected events will not be cleared until you save the Menuentry's settings and
exit and relaunch PC/2.
Therefore this option allows you to temporarily suspend the scheduling without
losing your selections in the listboxes.
:dt.:hp9.Hourly&colon.:ehp9.
:dd.The Hourly pushbutton is used to define hourly scheduling events.
In the :hp9.Minute:ehp9. listbox, you can select the minute(s) the program should
be scheduled every hour.
:dt.:hp9.Daily&colon.:ehp9.
:dd.The Daily pushbutton is used to define daily scheduling events.
In the :hp9.Minute:ehp9. and :hp9.Hour:ehp9. listboxes, you can select the minute(s)
and hour(s) the program should be scheduled every day.
:dt.:hp9.Weekly&colon.:ehp9.
:dd.The Weekly pushbutton is used to define weekly scheduling events.
In the :hp9.Minute:ehp9., :hp9.Hour:ehp9. and :hp9.Weekday:ehp9. listboxes, you can
select the minute(s), hour(s) and weekday(s) the program should be scheduled every week.
:dt.:hp9.Monthly&colon.:ehp9.
:dd.The Monthly pushbutton is used to define monthly scheduling events.
In the :hp9.Minute:ehp9., :hp9.Hour:ehp9. and :hp9.Day:ehp9. listboxes, you can
select the minute(s), hour(s) and day(s) the program should be every month.
:dt.:hp9.Once&colon.:ehp9.
:dd.The Once pushbutton is used to define yearly scheduling events.
In the :hp9.Minute:ehp9., :hp9.Hour:ehp9., :hp9.Day:ehp9. and :hp9.Month:ehp9.
listboxes, you can select the minute(s), hour(s), day(s) and month(s) the program
should be year by year.
:dt.:hp9.Save anyway&colon.:ehp9.
:dd.When you have selected the Disable radiobutton, the scheduling events you
have defined will not be saved and therefore lost when you save the Popup Menu,
unless you have also checked the Save anyway checkbox.
This setting helps you not to loose your configuration settings, even if you
have disabled scheduling of the Menuentry until future requirements.
:dt.:hp9.Minute&colon.:ehp9.
:dd.Select one or more minutes.
:dt.:hp9.Hour&colon.:ehp9.
:dd.Select one or more hours.
:dt.:hp9.Weekday&colon.:ehp9.
:dd.Select one or more weekdays.
:dt.:hp9.Day&colon.:ehp9.
:dd.Select one or more days.
:dt.:hp9.Month&colon.:ehp9.
:dd.Select one or more months.
:edl.
:p.In the listboxes you can select one or more (regular) scheduling events.
This allows you to define multiple regular events, e.g. you can define that the OS/2
format program FORMAT.EXE is started at 0 and 12 o'clock daily to clean up
your TEMP partition, or you can define that your mailing program is looking for
mail at 0, 15, 30 and 45 minutes after each full hour.
:p.You can not define multiple non regular event for a single Menuentry, e.g. you
can not define that a program is launched on 1st of April, 4th of July and
24th of December.
If you have such a requirement, a workaround would be to define multiple Menuentries
for the same session, each having defined different scheduling events.
:p.For each selected listbox, you have to select at least one entry, otherwise
no scheduling will occur for this Menuentry.
:p.Press :hp4.Ok:ehp4. to accept, or press :hp4.Cancel:ehp4. to ignore the data
you entered in all notebook pages.
:p.:link reftype=hd res=1200.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected examples from the Program installation PC/2 help.                  *
.*--------------------------------------------------------------------------------------*
:h2 res=4050.Program installation example 1
:p.Assume you want to add Lotus 123/G:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.Lotus 123/G:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.c&colon.\123g\123g.exe:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.c&colon.\123g:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.(none)
:edl.
:p.Lotus 123/G is a Presentation Manager application so you would select
:hp4.Presentation Manager:ehp4. as program type. The program style may be
:hp4.Maximized:ehp4., because more information is visible. The priority
may be left at the default selection :hp4.No Change:ehp4..
:p.Assume you want to add the AntiVirus utility from your LAN Server:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.AntiVirus (LAN):ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.\\DOMAIN1\Utilities\AntiVirus\AV.exe:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.(none)
:dt.:hp9.Parameters&colon.:ehp9.
:dd.(none)
:edl.
:p.Let's assume AntiVirus is of the same type as Lotus 123/G above,
so just use the same program styles.
:p.Assume you want to run the MultiMedia Player from the partition OS/2
was booted from as you have multiple versions of OS/2 installed on
different partitions:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.MultiMedia Player:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.%BOOTDRIVE%\MMOS2\MPPM.EXE:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.%BOOTDRIVE%\MMOS2\MOVIES:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.:hp4.-L %CDDRIVE%\MMOS2\VIDEOS\New\ThinkPad.AVI:ehp4.
:dt.:hp9.Icon File&colon.:ehp9.
:dd.:hp4.%PC2%\Icons\Intro.ico:ehp4.
:edl.
:p.As the example shows, you can use the macro %BOOTDRIVE% whereever
you want to have the driver letter of the partition OS/2 was booted
from instead, even in multiple entryfields (e.g. most likely useful in the
Parameters or Workdirectory entryfield).
You can also use the %PC2% macro whereever you want to have the path
PC/2 was lanched from instead, again in other entryfields too.
Or you can use the %CDDRIVE% macro whereever you want the drive letter
of the first CD-ROM drive, again in other entryfields too.
:p.Assume that you want to start StarOffice in a setup where StarOffice
is installed as a network installation (e.g. Setup /NET) on a Server
and you have your personal settings on your local workstation.
For that, an Environment Space is defined where the Environment Variable
:hp9.SONET=T&colon.\SO51-EN:ehp9. points to the Server and
:hp9.SOLOCAL=d&colon.\StarOffice:ehp9. points to your local customization.
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.StarOffice:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.%SONET%\SOFFICE.EXE:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.%SOLOCAL%\DATA:ehp4.
:edl.
:p.That example would ensure that your working directory is set to
your local customization of StarOffice while the executable is started
via the Server (e.g. residing on a write-protected drive).
:p.Assume you want to add the WPS Object System Clock (located in the
folder :hp4.OS/2 System&rarrow.System Setup:ehp4.):
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.System Clock:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.<WP_CLOCK>:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.(n/a)
:dt.:hp9.Parameters&colon.:ehp9.
:dd.(n/a)
:edl.
:p.The System Clock is a WPS Object, so :hp4.WPS Object:ehp4. is
selected as the program type.
You may have to look into the file :hp4.OS2_20.RC:ehp4. to get the defined
name of preinstalled WPS Objects.
Most settings are not available for WPS Objects.
:p.Assume you want to add the WPS clock object with the ObjectID :hp4.<WP_CLOCK>:ehp4..
First you must extract the ObjectID by using WPS 2 PC/2.
Then drag the extracted object from the WPS 2 PC/2 window onto
PC/2's Setup dialog, to create a Menuentry within the Popup Menu.
After you have created the clock Menuentry, you can view its settings by
editing the Menuentry:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.System Clock:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.<WP_CLOCK>:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.(n/a)
:dt.:hp9.Parameters&colon.:ehp9.
:dd.(n/a)
:edl.
:p.The System Clock is a WPS Object, so :hp4.WPS Object:ehp4.
is used as the program type.
Assuming that you extracted the ObjectID as outlined
before, then the Object ID is :hp4.<WP_CLOCK>:ehp4..
:p.Sorry, this may be a little complicated, but it is the only way
to get a known Object ID.
:p.Assume you want to add the WIN-OS2 Program Manager.
Open the Program Installation dialog, and either enter the path information
(e.g c&colon.\os2\mdos\winos2\progman.exe) or drag :hp4.PROGMAN.EXE:ehp4.
into.
If you have entered the data manually, you also must check the
seamless WIN-OS2 session.
Further assume that you want in the same WIN-OS2 session to also start the
WIN-OS2 File Manager :hp4.WINFILE.EXE:ehp4. -  then you'd add it to the
Parameters entryfield:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.Prog &amp. File Manager:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.c&colon.\os2\mdos\winos2\progman.exe:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.c&colon.\os2\mdos\winos2:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.:hp4.winfile.exe:ehp4.
:edl.
:p.You can only add a single Parameter to the WIN-OS2 Program Manager.
:p.Assume you want to detach :hp4.TCPSTART.CMD:ehp4.. You likely know that Rexx batch jobs
are run by a command processor, that is :hp4.CMD.EXE:ehp4., and as command processors
know that they run such batch jobs there are even two ways to achive our goal.
The first one is:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.Detached TCP/IP startup:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.c&colon.\os2\cmd.exe:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.c&colon.\mptn\bin:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.:hp4./C TCPSTART.CMD:ehp4.
:edl.
:p.Alternatively you could have skipped one temporary command processor by using
the second method:
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.Detached TCP/IP startup:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.TCPSTART.CMD:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.c&colon.\mptn\bin:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.(none)
:edl.
:p.Now assume you want to detach the :hp4.RC5DES:ehp4. cracking client (e.g. to hide it in
a way unexperienced users can't detect it ;-):
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.You would enter&colon.:ehp4.
:dt.:hp9.Program Title&colon.:ehp9.
:dd.:hp4.RC5DES Client:ehp4.
:dt.:hp9.Executable&colon.:ehp9.
:dd.:hp4.RC5Des.exe:ehp4.
:dt.:hp9.Workdirectory&colon.:ehp9.
:dd.:hp4.c&colon.\RC5Des:ehp4.
:dt.:hp9.Parameters&colon.:ehp9.
:dd.:hp4.-ini OS2Proxy.ini:ehp4.
:edl.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Control Addition PC/2 dialog box.                    *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=2900.Help for PC/2 Control Addition dialog
:p.This PC/2 Control Addition dialog allows you to add Menucontrol items to PC/2's Popup Menu.
:artwork name='Source\Resource\ControlAdditionDialog.bmp' align=left.
:p.Except for :hp9.Break Separator:ehp9. and :hp9.Menu Separator:ehp9., you may only add
1 instance of the Menucontrol to the Popup Menu.
Instead of activating PC/2's features from the Popup Menu, you may invoke them
from PC/2's :link reftype=hd res=4013.<Smarticons>:elink. instead.
:p.The Menucontrol :hp9.Configure Popup Menu Settings:ehp9. must be added
to the Popup Menu in order that PC/2 displays the Popup Menu after mouse button 1
or mouse button 3 clicks, otherwise the
:link reftype=hd res=301.<Setup dialog>:elink. will
be invoked after each mouse click until you have added this Menucontrol. PC/2
supports the following Menucontrols:
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Configure Popup Menu Settings:ehp9.
:dd.This button is used to add the Menucontrol to be able to configure the Popup Menu
via the Setup dialog.
:dt.:hp9.Configure Desktop Settings:ehp9.
:dd.This button is used to add the Menucontrol to configure the Virtual Desktop feature.
:dt.:hp9.About PC/2:ehp9.
:dd.This button adds the About PC/2 Menucontrol.
:dt.:hp9.ShutDown OS/2:ehp9.
:dd.This button adds a Menucontrol, which allows you to shut down OS/2.
In the :link reftype=hd res=2050.<ShutDown Options>:elink. you may select
if you want PC/2 to confirm your request and if the WPS method or PC/2's :hp9.Fast
ShutDown:ehp9. method will be used which even may include a reboot of your PC.
This Menucontrol may automatically scheduled by the
:link reftype=hd res=1600.<Scheduler>:elink..
:dt.:hp9.PowerDown OS/2:ehp9.
:dd.This button adds a Menucontrol, which allows you to suspend or power off your
workstation.
In the :link reftype=hd res=2050.<PowerDown Options>:elink. you may select
if you want PC/2 to confirm your request and if your PC should either be suspended
or powered off.
This Menucontrol may automatically scheduled by the
:link reftype=hd res=1600.<Scheduler>:elink..
Without needing to add this Menucontrol, you can enable the
:link reftype=hd res=2050.<Automatic PowerDown Timer>:elink. to suspend or
power off your workstation after a specified amount of no user activity (regarding
mouse and keyboard).
:dt.:hp9.Help:ehp9.
:dd.This button adds the online help Menucontrol item.
:dt.:hp9.Exit:ehp9.
:dd.This button is used to add the Menucontrol to be able to exit PC/2 via a
message box by just clicking on :hp4.Ok:ehp4. or pressing the :hp4.Enter:ehp4. key.
:p.When running PC/2 as the WPS process, this Menuitem will be disabled, because
a WPS process should not terminate.
This Menucontrol may automatically scheduled by
:link reftype=hd res=1600.<Scheduler>:elink..
:dt.:hp9.PC/2 Spooler:ehp9.
:dd.This button is used to activate the PC/2's :hp9.Spooler Control Window:ehp9. from
the Popup Menu.
Please refer to the :link reftype=hd res=2000.<Physical Desktop Settings>:elink.
for more information about PC/2's Spooler.
:dt.:hp9.Lockup OS/2:ehp9.
:dd.This button is used to activate PC/2's :hp9.Lockup feature:ehp9. from
the Popup Menu.
Please refer to the :link reftype=hd res=2150.<Security Settings>:elink. for
more information about PC/2's Lockup feature.
This Menucontrol may automatically scheduled by
:link reftype=hd res=1600.<Scheduler>:elink..
:dt.:hp9.Break Separator:ehp9.
:dd.This button is used to add one break separator Menucontrol. You may add this
Menucontrol to convert a large single-column menu to a smaller multi-column menu.
This Control is a Menucontrol that you may add as often you prefer. I suggest to
use this Control only to break Menuitems not being a Submenu.
:dt.:hp9.Menu Separator:ehp9.
:dd.This button is used to add one separator Menucontrol. You may add this Menucontrol
to enhance the readability of the Popup Menu. This Control is a Menucontrol that
you may add as often you prefer.
:edl.
:p.Press :hp4.Ok:ehp4. to accept or press :hp4.Cancel:ehp4. to ignore your modifications.
:p.:link reftype=hd res=1200.<Backward>:elink.
                                :link reftype=hd res=1650.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for the Control Configuration PC/2 dialog box.                *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=1650.Help for PC/2 Control Configuration dialog
:p.For the PC/2 Lockup, Exit PC/2, PowerDown OS/2 and ShutDown OS/2 Menucontrols, you can use the PC/2
Scheduler to automatically select these Menucontrols for you (as shown for PowerDown OS/2).
:artwork name='Source\Resource\ControlConfigurationDialog.bmp' align=left.
:p.For more information about the notebook pages, select:
:ul compact.
:li.:link reftype=hd res=1700.<Current Control>:elink.
:li.:link reftype=hd res=1600.<Scheduler Settings>:elink.
:eul.
:p.Press :hp4.Ok:ehp4. to accept or press :hp4.Cancel:ehp4. to ignore your modifications.
:p.:link reftype=hd res=2900.<Backward>:elink.
                                :link reftype=hd res=2600.<Forward>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Control Configuration Control notebook page.         *
.*--------------------------------------------------------------------------------------*
:h2 res=1700.Current Control
:p.This notebook page displays the current Menucontrol for which you are currently
edition the settings.
However, you can't change the type of the Menucontrol from this notebook, you have
to make such changes from within the
:link reftype=hd res=301.<PC/2 Setup>:elink. dialog


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Resort PC/2 dialog box.                              *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=2600.Help for PC/2 Resort dialog
:p.This dialog box contains two listboxes, the left one that displays the currently
available Menuentries and the right one that displays the currently resorted
Menuentries.
:artwork name='Source\Resource\ResortDialog.bmp' align=left.
Select one or more Menuentries from the left listbox and press the
:hp4.Reorder:ehp4. button, to resort it to the right listbox.
For the remaining Menuentry(ies) repeat this procedure, until the left listbox is empty.
Instead of selecting a Menuentry and then clicking on the :hp4.Reorder:ehp4.
button, you may doubleclick on a Menuentry.
:p.You can cancel the resort before you have done any sort, but you have to resort
the complete Submenu once you made one resort operation.
:p.All sorted Menuentries in the right listbox can't be started with PC/2 until
you complete the resort by pressing :hp4.OK:ehp4..
:warning.
You have to press :hp4.OK:ehp4. in the :link reftype=hd
res=301.<Setup Dialog>:elink. to write the resorted Menuentries
structure to the configuration file.
:p.Because a Break Separator is a style of the preceding Menuentry, you can't
resort a Break Separator standalone, you can only resort its :hp2.owning:ehp2. Menuentry
which automatically resorts the Break Separator too.
:ewarning.
:p.:link reftype=hd res=1650.<Backward>:elink.
                                :link reftype=hd res=3200.<Forward>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Startup Parameters dialog box.                       *
.*--------------------------------------------------------------------------------------*

:h1 res=3200.Help for PC/2 Startup Environment Spaces dialog
:p.The PC/2 Startup Environment Spaces dialog allows you to (optionally) modify the
Environment Spaces hou have selected for the executable to be launched.
:artwork name='Source\Resource\StartupEnvironmentsDialog.bmp' align=left.
:p.PC/2 allows you to replace the Environment Spaces defined for the application in
the :link reftype=hd res=1550.<Program Installation>:elink. dialog
completely or partly.
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Startup Environment Spaces:ehp9.
:dd.This listbox contains all available Envrionment Spaces.
After the initial display of this dialog, the listbox has selected all that
Envrionment Spaces you have selected for the program in the Program
Installation dialog.
If you select :hp9.Default:ehp9. exactly these Environment Spaces will be applied
for the launch of the program.
:p.You can modify the Environment Spaces PC/2 will apply to the to be launched
program by selecting additional and deselecting already included Environment Spaces.
To apply your modifications afterwards just select the :hp9.Selected:ehp9. pushbutton,
selecting the :hp9.Default:ehp9. pushbutton will ignore any changes you have made.
:dt.:hp9.Selected:ehp9.
:dd.Press :hp9.Selected:ehp9. to launch the program with the Environment Spaces you
have selected in the :hp9.Startup Environment Spaces:ehp9. listbox.
:dt.:hp9.Default:ehp9.
:dd.Press :hp9.Default:ehp9. to launch the program with the Environment Spaces you
have defined for that program in the Program Installation dialog.
This option launches the program as if the Startup Environment Spaces
dialog has never been invoked.
:edl.
:warning text='Note:'.
If you don't make changes in the :hp9.Startup Environment
Spaces:ehp9. listbox, the pushbuttons :hp9.Selected:ehp9. and :hp9.Default:ehp9.
will launch the selected application the same way.
:ewarning.
:p.:link reftype=hd res=2600.<Backward>:elink.
                                :link reftype=hd res=3100.<Forward>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Startup Parameters dialog box.                       *
.*--------------------------------------------------------------------------------------*

:h1 res=3100.Help for PC/2 Startup Parameters dialog
:p.The PC/2 Startup Parameters dialog allows you to (optionally) pass commandline
arguments to the executable you have selected to be launched.
:artwork name='Source\Resource\StartupParametersDialog.bmp' align=left.
:p.PC/2 allows you to replace the commandline parameters of your application
completely or partly.
.******
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.User added parameters:ehp9.
:dd.This entryfield is used to enter the user supplied portion of the commandline
parameters passed to the application to be launched.
:dt.:hp9.Find File(s):ehp9.
:dd.Sometimes, e.g. for your editor, you may want to pass one or more filenames
as commandline arguments.
You can either enter them in the entryfield, or press the :hp9.Find File(s):ehp9.
pushbutton to select them using the standard OS/2 file dialog.
The selected filenames in the file dialog will be appended to the entryfield.
If you want to add filenames from more than one directory, just invoke the
file dialog again.
:dt.:hp9.Accept:ehp9.
:dd.Press :hp9.Accept:ehp9. to accept the parameters you entered and add them
to the commandline parameters passed to the to be launched session.
If you want to add not parameter, just press the pushbutton too.
:dt.:hp9.Cancel:ehp9.
:dd.Select :hp9.Cancel:ehp9. to abort your request to launch the session.
:edl.
.******
:p.For example, assume you have a TCP/IP communications package installed on your
system. You also have added an Menuitem that starts a file transfer program named
:hp4.FTP:ehp4.. :hp4.FTP:ehp4. requires the name of the host
you want to connect to, as a parameter.
To prompt you for this parameter immediately before :hp4.FTP:ehp4. is started,
you would modify the parameters entryfield of the PC/2 Program Installation
dialog like :hp4.[Please enter host address]:ehp4.
:p.Further assume that you selected the Menuitem :hp4.FTP:ehp4. from the
Popup Menu. Just before :hp4.FTP:ehp4. is started, a dialog box is displayed,
requesting the host address from you. You would type something like
:hp4.ftp.cdrom.com:ehp4., then the file transfer program will be invoked
as if you typed :hp4.FTP ftp.cdrom.com:ehp4. from a commandline.
:p.Generally, the syntax for the commandline parameters is:
:p.:hp4.Parameter(s) 1 [Text] Parameter(s) 2:ehp4.
:p.where Parameter(s) 1 and Parameter(s) 2 are optional, but can not be
changed at runtime once supplied in the Program Installation dialog.
:p.Assume you typed :hp4.FirstFile.doc:ehp4. as the first, and
:hp4.ThirdFile.doc:ehp4. as the second parameter.
You would be asked by a dialog box what to replace :hp4.[Text]:ehp4. with.
Assume you entered :hp4.MyFile.doc:ehp4. then the program will be invoked
with the commandline parameters :hp4.FirstFile.doc MyFile.doc ThirdFile.doc:ehp4..
The string :hp4.[Text]:ehp4. would have been replaced by :hp4.MyFile.doc:ehp4..
Of course both parameters may be omitted, both may contain more than one parameter
or only one of them may present. Omitting :hp4.[Text]:ehp4. would start the
application without asking you for a commandline parameter prior.
:p.You may also use this dialog to add a second WIN-OS2 application to a
WIN-OS2 session, if the first application is the WIN-OS2 Program Manager.
:p.PC/2 allows you to edit commandline parameters similar to the WPS (open
the Settings notebook of any application,
position the cursor into the Parameters entryfield and
press F1 or Help to get further information).
:p.:link reftype=hd res=3200.<Backward>:elink.
                                :link reftype=hd res=302.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration PC/2 dialog box.               *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=302.Help for PC/2 Desktop Configuration dialog
:p.The PC/2 Desktop Configuration dialog is the entry to configure the behaviour
of PC/2.
:artwork name='Source\Resource\DesktopConfigurationDialog.bmp' align=left.
That dialog is used to configure what PC/2 features are enabled, how PC/2 changes the
default OS/2 behaviour and the interfaces between PC/2 and PM (Presentation Manager).
It is also where you configure the layout of
:link reftype=hd res=4013.<PC/2's Overview Window>:elink., to understand
the terminology used within this dialog I recommend you to take at the Overview
window first.
:p.The dialog consists of the following pages, for more information just click on the
hyperlinks:
:ul compact.
:li.:link reftype=hd res=1800.<Virtual Desktop Settings Page 1>:elink.
:li.:link reftype=hd res=1850.<Virtual Desktop Settings Page 2>:elink.
:li.:link reftype=hd res=1900.<Overview Window Settings>:elink.
:li.:link reftype=hd res=1950.<SessionBar Settings>:elink.
:li.:link reftype=hd res=2000.<Physical Desktop Settings>:elink.
:li.:link reftype=hd res=2050.<General PC/2 Settings>:elink.
:li.:link reftype=hd res=2100.<SmartIcons Settings>:elink.
:li.:link reftype=hd res=2150.<Security Settings>:elink.
:li.:link reftype=hd res=2200.<Environment Spaces Settings Page 1>:elink.
:li.:link reftype=hd res=2250.<Environment Spaces Settings Page 2>:elink.
:li.:link reftype=hd res=2300.<Registration Settings>:elink.
:li.:link reftype=hd res=2350.<Spy Window Settings>:elink.
:eul.
:p.Once you have configured the notebook pages, just press
:hp4.Save:ehp4. to bring them into effect and save them to the profile, :hp2.or:ehp2.
:hp4.Cancel:ehp4. to make them temporarily effective, but without saving them
into the profiles :hp9.PC2.INI:ehp9. and :hp9.PC2.CFG:ehp9. to disk.
If your :hp9.PC2.INI:ehp9. file is corrupted, PC/2 assumes default values, which
you then can edit and save to a new INI file, if :hp9.PC2.CFG:ehp9. is corrupted,
you may lose the defined Popup Menu.
To prevent possible data loss (unlikely that this will happen, but no software really
is perfect ;-) you should regularily backup your configuration by selecting the
:hp4.Backup:ehp4. pushbutton, which will backup your :hp9.PC2.INI:ehp9. to
:hp9.PC2-INI.BAK:ehp9., :hp9.PC2.CFG:ehp9. to :hp9.PC2-CFG.BAK:ehp9. and
:hp9.PC2.ENV:ehp9. to :hp9.PC2-ENV.BAK:ehp9..
:p.:link reftype=hd res=3100.<Backward>:elink.
                                :link reftype=hd res=4013.<Forward>:elink.


.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Virtual Desktop Settings       *
.* notebook page 1.                                                                     *
.*--------------------------------------------------------------------------------------*
:h2 res=1800.Virtual Desktop Settings Page 1
:p.This page is used to control the Virtual Desktop feature of PC/2.
Click :link reftype=hd res=1850.<here>:elink. to switch to the second page.
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Virtual Desktop:ehp9.
:dd.Select this checkbox to enable the :hp4.Virtual Desktop:ehp4. feature of
PC/2.
:dt.:hp9.Count:ehp9.
:dd.These two spinbuttons enable you to define the number of Virtual Desktops in
horizontal direction and vertical direction, both in the range of 1 to 9.
The first spinbutton determines the number of Virtual Desktops in the horizontal
direction, the second spinbutton the number of Virtual Desktops in the vertical
direction.
:dt.:hp9.Overview Window:ehp9.
:dd.You may also want an Overview Window, that contains the current
layout of the Virtual Desktop.
Select this checkbox to enable this feature.
You may doubleclick with mouse button 1 on any of the up to 81 :hp4.Virtual
Desktop:ehp4.s to immediately switch to this Desktop.
commandline option, to move the Physical Desktop.
:dt.:hp9.Follow focus:ehp9.
:dd.When this option is enabled, PC/2 prevents its windows receiving the input
focus, just because OS/2 always switches the input focus to the window on top
of Z-Order if one window loses the input focus (e.g. when being closed).
PC/2 will not try to find the next lower window in the Z-Order and switch to
it if this window is visible, restored and completely positioned withing the
current Virtual Desktop.
If that fails PC/2 ensures as a last resort that the window below the mouse pointer
gets the input focus next, even if this means that PC/2 does change the current
Virtual Desktop.
The goal of that adjustment is to ensure to switch to that Virtual Desktop where
the window getting the focus is best visible (e.g. think of a window that is to
80 percent on the next Virtual Desktop, then PC/2 will switch away from the
current Virtual Desktop where only 20 percent are visible).
Admittedly, sometimes that may cause some "peculiar" (or seamingly random) switching
away from the Virtual Desktop you thought that will be switch to, but then you can just
deactivate this option.
:warning text="Note!".
OS/2 PM (Presentation Manager) does not understand Virtual Desktops natively, nor
does it expect that the input focus changing will not depend on the Z-Order of
windows, thus this feature can't be perfect in any situtation.
:ewarning.
:dt.:hp9.Click to Move:ehp9.
:dd.If you prefer to click to a surrounding row or column, before the Physical
Desktop is moved, select this option, otherwise you can change to another
Virtual Desktop by just moving the mouse pointer on a surrounding row or
column of your Display.
:dt.:hp9.Autoscroll:ehp9.
:dd.When this option is selected, moving the cursor to a surrounding row and/or
column of your Display start autoscrolling in the requested direction.
Autoscrolling stops only when you move the mouse pointer away from the
surrounding row or column, or the outermost Virtual Desktop is reached.
As modern display hardware is very fast, but not nearly as fast as the update of
the datastructures behind the Virtual Desktop features, removing the mouse
pointer from the surrounding rows and/or columns may not stop autoscrolling
immediately, as the display updates are always lagging behind.
:p.If this option is not selected, PC/2 ensures that you scroll only one
unit in the requested direction, by changing the pointer position away from
the surrounding row and/or column, once the scroll has been performed.
:dt.:hp9.Scrolling Delay:ehp9.
:dd.The :hp9.Scrolling Delay:ehp9. lets you adjust how long the mouse pointer must
be positioned on a surrounding row and/or column before PC/2 will start to scroll
or autoscroll to the adjacent Virtual Desktop.
Use this scrollbar to customize that delay between 0 and 2500 ms (milliseconds)
in steps of 100 ms (to be exact, your selection will be rounded up to the next
multiple of the OS/2 timer tick, which has a granularity of 32 ms for all OS/2
versions available currently).
Dpending on the system load, the selected delay may be extended additionally.
:p.
For example, if you set that delay to 1000 ms, you would have time up to 1 second to
drag e.g. a sizing border that is located at a surrounding row and/or column
before PC/2 recognizes from the mouse pointer position that it should switch to
the next Virtual Desktop.
:dt.:hp9.Move WPS:ehp9.
:dd.This checkbox allows you to move the Desktop within the Virtual Desktop
like any other window. The Desktop is another name for the WPS (WorkPlace-Shell)
shipped with OS/2.
You may check this option if you don't need the services provided by the WPS
on all Virtual Desktops.
:dt.:hp9.Expand WPS:ehp9.
:dd.When selecting this feature, the WPS is expanded to the size of the
Virtual Desktop. You may then distribute WPS Objects to any of the Virtual
Desktops. If you have a scroll bar on your WPS, this feature removes the
scrollbar when your WPS completely fits into the Virtual Desktop.
:dt.:hp9.Desktop Scrolling:ehp9.
:dd.The scrollbar :hp9.Desktop Scrolling:ehp9. enables you to define the
scrolling factor that is applied to your Virtual Desktop, when switching to
the next Virtual Desktop, by clicking on a surrounding row or column.
You can select between 0 and 100 % with 5% increments, with 100 % being the
default one.
:dt.:hp9.Button 1:ehp9.
:dd.Select this option if you want to activate the Popup Menu or
switch to another Virtual Desktop by clicking with mouse button 1.
:p.Alternatively you may also activate the Popup Menu from the keyboard
, for example by pressing the
:hp4.LEFT SHIFT:ehp4. and the :hp4.RIGHT SHIFT:ehp4.-Keys simultaneously.
:dt.:hp9.Button 3:ehp9.
:dd.Select this option if you want to activate the Popup Menu or
switch to another Virtual Desktop by clicking with mouse button 3.
This of course requires a 3 button mouse and a mouse device driver,
that supports a 3 button mouse.
:dt.:hp9.Over any window:ehp9.
:dd. When checking this checkbox, the Popup Menu gets activated when
clicking with the mouse, regardless what window is below the current
pointer position.
:p.This makes only sense, when you have selected to activate the Popup Menu with
mouse button 3, because most applications requires mouse button 1 for
themselves.
Selecting this option concurrently with mouse button 1 would degrade the
usablity of OS/2 considerably.
:warning text='Note:'.
As I don't have a 3-button mouse I could not test button 3 functionality, but
I have reports that selecting this option together with :hp9.Button 3:ehp9.
causes a hang when being used over PMMAIL windows, chances are that other
applications will have that problem too.
:ewarning.
:dt.:hp9.Single Click:ehp9.
:dd.Select this option if you want to activate the Popup Menu or
switch to another Virtual Desktop with a single click with mouse button 1
or mouse button 3.
:dt.:hp9.Double Click:ehp9.
:dd.Select this option if you want to activate the Popup Menu or
switch to another Virtual Desktop with a double click with mouse button 1
or mouse button 3.
:dt.:hp9.Draw Icons:ehp9.
:dd.Select this checkbox to draw the icons of the applications described
by the Menuentries in addition to the applications' titles.
Displaying an application's icon, either full or half sized, increases
your productivity due to the fact graphics are more kognitive than text,
for the cost of a slight delay during drawing the Popup Menu.
:p.Unfortunately it requires some time to load the icons for all Menuentries
(Menuentries that are OS/2 PM and WIN-OS2 executables to be exact),
as finding the file to load the icon from requires some smart work, as PC/2
tries to load the icon from:
:ol.
:li.the fully qualified path you entered in the
:link reftype=hd res=1250.<Executable entryfield>:elink., or
:li.the fully qualified path comprising ot the
:link reftype=hd res=1250.<Workdirectory and the Executable>:elink., or
:li.each directory specified in the PATH environment variable for a Menuentry's
:link reftype=hd res=1550.<Session Environment>:elink., or
:li.a cache, that caches the result of the last time the icon was loaded.
:eol.
:p.As you may imagine, the time required to locate the icon greatly increases
from step 1 to 3.
:p.Unfortunately step 3 is the most powerful one (much more powerful than the
WPS) as you just have to specify the name of the executable (without a fully
qualified path information and without a working directory) and the Session
Environment, if the Session's Environment contains a PATH environment variable
pointing to the directory the executable can be found at.
You may select more information about setting the
:link reftype=hd res=1550.<Session's Environment>:elink. and about
:link reftype=hd res=2200.<Environment Spaces>:elink., if
you are not sure how this will work.
:p.Fortunately, PC/2 is able to use step 4. to load the icon, which is simply
a cache that saves the results from the last time performing steps 2. and 3..
As much work is replaced by using this cache, much time can by saved the next
time the icons need to be loaded.
:p.The cache will be used as long as PC/2 is running, to use the cache the next
time you launch PC/2 the cache needs to be saved into your configuration file
too.
To save your configuration file, just activate the Setup
dialog and select the Save pushbutton - of course all other changes you made
to the Popup Menu will be saved too!
:dt.:hp9.Full Size:ehp9.
:dd.Selecting this radiobutton displays the applications' icons at their full
size in addition to the titles.
The full size of and icon is usually 32 * 32 pixels for VGA resolution, and
40 * 40 for SVGA resolutions.
:dt.:hp9.Half Size:ehp9.
:dd.Selecting this radiobutton displays the application's icons at their half
size, that is 16 * 16 for VGA, and 20 * 20 for SVGA resolutions.
:dt.:hp9.Default Selection:ehp9.
:dd.Select this item if you don't want to automatically select any of the
Menuentries.
The Popup Menu's lower left corner is then positioned at the pointer position,
unless the Popup Menu can't be displayed completely there.
:dt.:hp9.Preselect First Item:ehp9.
:dd.Select this item if you want PC/2 to automatically select the first entry
of your Popup Menu for you. If the first item is a Submenu, it will be opened
automatically.
:dt.:hp9.Select Menu Center:ehp9.
:dd.Select this item to automatically center the Popup Menu around your
pointer, unless the Popup Menu can't be displayed completely there.
:dt.:hp9.Left+Right SHIFT:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by pressing
the :hp4.Left and Right SHIFT:ehp4. keys together.
:dt.:hp9.Left CTRL:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Left CTRL:ehp4. key.
:p.This option (and the following ones) depend on the fact that pressing a key
on the keyboard generates 2 different codes, the make and the break code.
The make code is generated when the key is pressed and the break code is
generated when the key is depressed.
Most keys generate the make code multiple times a second, when the key is
pressed and kept down (which can be controlled in the Keyboard Settings
notebook of the WPS).
PC/2 displays the Popup Menu only, when a single make code is followed by a
single break code, however this requires you to immediately depress the key
after pressing it to allow PC/2 to recognize the key press.
This allows you to use the keys for other applications, by
just keeping the key down as long as at least one another make code follows
the first make code.
:warning text='Note:'.
If want to use the CTRL options, please note that there
is a problem when activating the Window List with CTRL+ESC.
If you press the CTRL key fast enough that the single make code is immediately
followed by the single make code even when pressing ESC simultaneously,
the Window List will popup for a moment immediately followed by PC/2's
Popup Menu.
You can only avoid this problem when keeping the CTRL key pressed a little
longer, so that at least two make codes are generated, or explained in more
technical terms, CTRL+ESC is a key combination where the ESC key is filtered
by a device driver so that PC/2 can not detect it, however it can detect the
CTRL key's make and break code as the device driver doesn't filter them (because
it can't know that the user wants to press the ESC key and when it detects and
filters the ESC key, the CTRL key was already pressed).
:dt.:hp9.Right CTRL:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Right CTRL:ehp4. key.
:dt.:hp9.Left WIN95:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Left WIN95:ehp4. key.
This key is only available on WIN95 keyboards, which have been introduced by
many vendors after WIN95 was released.
:dt.:hp9.Right WIN95:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Right WIN95:ehp4. key.
:dt.:hp9.WIN95 Menu:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.WIN95 Menu:ehp4. key.
:warning text='Note:'.
Without allowing PC/2 to use this key to activate
its Popup Menu, this key will do the same as the :hp4.Prt Scrn:ehp4. key
using the default OS/2 keyboard driver.
:ewarning.
:dt.:hp9.Caps Lock:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Caps Lock:ehp4. key.
:dt.:hp9.Scroll Lock:ehp9.
:dd.When checking this option, you can display PC/2's Popup Menu by a single
click of the :hp4.Scroll Lock:ehp4. key.
:edl.
:p.Press :hp4.Save:ehp4. to accept the configuration and save it into :hp9.PC2.INI:ehp9.,
or press :hp4.Cancel:ehp4. to ignore any changes you made.
If your INI file is corrupted, PC/2 assumes default values, which
you then can edit and save to a new INI file.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Virtual Desktop Settings       *
.* notebook page 2.                                                                     *
.*--------------------------------------------------------------------------------------*
:h2 res=1850.Virtual Desktop Settings Page 2
:p.This page is used to control the Virtual Desktop feature of PC/2.
Click :link reftype=hd res=1800.<here>:elink. to switch to the first page.
:dl tsize=12 break=fit.
:dthd.:hp4.Fieldname&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Sliding Focus:ehp9.
:dd.Select this checkbox to enable the :hp9.Sliding Focus:ehp9. feature of
PC/2.
:warning text='Note:'.
Even if this setting is enabled, you can temporarily prevent the
focus from moving by holding down the :hp4.SHIFT:ehp4.-key when moving the
mouse.
:ewarning.
:warning text='Caution:'.
The :hp2.Sliding Focus does not work very well for seamless WIN-OS2:ehp2. sessions.
It seems that the link between the WIN-OS2 "drawing area" (that is where the seamless
WIN-OS2 application draws) and the OS/2 PM windows backing it (seamless WIN-OS2 support
is implemented with "hidden" PM windows, as PM natively doesn't provide WINDOWS APIs)
gets lost.
Symptoms are that windows "shine through" and mouse and keyboard input gets routed
to "hidden" windows.
:ewarning.
:warning text='Caution:'.
The :hp2.Sliding Focus does not work very well with Lotus Notes:ehp2. also (well, I find
that the focus handling of Notes is brain-damaged anyway).
When moving the pointer over a horizontal sizing border a loop alternating the
focus between the Lotus Notes Desktop window and the topmost open window (e.g.
Mail edit window) occurs.
:ewarning.
:dt.:hp9.Preserve Z-Order:ehp9.
:dd.To preserve the Z-Order of the windows, that is, the order in which a window in
the foreground overlaps a window behind, also check the :hp9.Preserve Z-Order:ehp9.
checkbox.
:p.
When having checked this setting, you might have to click onto the controls
(e.g. entryfield, radiobutton,...) you want to put the input focus at, because
though the window has been activated by the :hp9.Sliding Focus:ehp9., it may
still be covered partly by another window.
Even if you don't have checked :hp9.Activate Windows:ehp9., that is the control
below the mouse pointer gets the input focus automatically, you may have to
click onto the window to make controls covered by other windows visible.
Due to the dependencies of the current window with others, clicking onto the
current window sometimes may not bring the whole window into the foreground.
:p.
Even if this setting is enabled, you can temporarily prevent changes
of the Z-Order by holding down the :hp4.SHIFT:ehp4.-key when moving the
mouse.
:dt.:hp9.Activate Windows:ehp9.
:dd.Selecting this checkbox causes the sliding focus to be applied on frame
windows only, without setting the input focus to the control (e.g. entryfield,
radiobutton,...) directly below the mouse pointer.
Instead the control that had the focus just before the frame window lost the
input focus will be selected again.
You may alternatively press the :hp4.ALT:ehp4. key while moving the pointer to
temporarliy activate this feature, without actually checking the checkbox.
:dt.:hp9.Activation Delay:ehp9.
:dd.You may select that the focus does not change immediately after the mouse
was positioned onto a different window but after a configurable delay.
Use this scrollbar to customize that delay between 0 and 2500 ms (milliseconds)
in steps of 100 ms (to be exact, your selection will be rounded up to the next
multiple of the OS/2 timer tick, which has a granularity of 32 ms for all OS/2
versions available currently).
Dpending on the system load, the selected delay may be extended additionally.
:warning text='Note!'.
Due to PM not supporting a :hp9.Sliding Focus:ehp9. natively, you may experience
that the input focus and/or the activation of a window is not set automatically
and you are required to click onto that window, or that a window looses the focus
immediately after you activated it (e.g. when using popup menus).
This are limitations of the tricks used to fool PM to implement the :hp9.Sliding
Focus:ehp9..
:ewarning.
:dt.:hp9.Restrict Frame Size:ehp9.
:dd.This option allows you to limit the size of all frame windows (where the WPS
is the only exception) to a certain size.
This may be very useful when using a hardware and a video driver that supports
Hardware assisted Panning (e.g. on the LCDs of IBM ThinkPads).
In short words, Hardware assisted Panning is the hardware assisted moving of
a, e.g. VGA sized, display within the larger, e.g. XGA sized, video memory.
Please refer to :link reftype=hd res=4005.<PC/2's Frequently Asked Questions>:elink.
(question 72.) for a more detailed description of Hardware Panning and to
the vendors of your PC and video adapter if you don't know if your hardware
supports Hardware Panning.
To summarize, Hardware assisted Panning is a hardware (read fast) implementation
of a Virtual Desktop.
PC/2 allows you to prevent sizeable windows from being sized larger than your
phyiscal display which is can be panned (moved) anywhere within the bounds of
the Virtual Desktop.
:p.You also might find it useful to reduce the icon and default font size, because
for resolutions higher than 800 * 600 OS/2 and WIN-OS2 automatically change to
a larger icon and font size to compensate for the increased resolution.
This is done because the icons and font size should be resolution independent,
so if you increase the resolution (that is DPI - dots per inch), you must also
increase the icon and font size in order to prevent them becoming smaller.
On a Hardware Panning system, this has the bad effect that less text fits on
your display, because the (LCD's) e.g. VGA size doesn't change, it just becomes
a window into a larger sized e.g. XGA sized GUI (graphical user interface).
Please refer to :link reftype=hd res=4005.<PC/2's Frequently Asked Questions>:elink.
(questions 73. and 74.) too, for a detailed description on how to change the
icon and font size of OS/2 and WIN-OS2.
:warning text='Hint!'.
To adjust the icon and font size I would recommend you to get Jason Shannon's
SMALLF13.ZIP, which should be available on all major OS/2 archives (e.g. hobbes),
which provides a excellent automatization of the required patches.
If you are a really lucky guy, you may even get support for the default font
and icon sizes by the display driver itself (e.g. the latest S3 Corp. drivers
allow to adjust them via the Capabilities pushbutton on the Screen page of the
System notebook).
:ewarning.
:dt.:hp9.640 * 480:ehp9.
:dd.The frame windows will be limited to a size of 640 by 480 pixels.
:dt.:hp9.800 * 600:ehp9.
:dd.The frame windows will be limited to a size of 800 by 600 pixels.
:dt.:hp9.1024 * 768:ehp9.
:dd.The frame windows will be limited to a size of 1024 by 768 pixels.
:dt.:hp9.1280 * 1024:ehp9.
:dd.The frame windows will be limited to a size of 1280 by 1024 pixels.
:dt.:hp9.Exception List:ehp9.
:dd.You can add the name of executables whose windows you don't want to be restricted to
the selected size, or that dislike being restricted by PC/2
(e.g. IBM's Communication Manager/2).
:p.Just specify unqualified filenames, for example :hp4.FDISKPM:ehp4. to exclude
the executable x&colon.\OS2\FDISKPM.EXE.
:warning text='Note:'.
OS/2 supports case sensitive filenames, though HPFS currently
is only case preserving.
Therefore, PC/2 requires you to enter the executable's filename with respect to
filename case sensitivity.
If you are not sure about the exact case sensitive spelling of your executable,
invoke a OS/2 :hp4.DIR:ehp4. command against the executable.
As PC/2 performs an exact match, the longer the Exception List, the more comparisms
are needed, so please keep your list as small as possible.
:ewarning.
:warning text='Hint!'.
To find out the executable that created a window most easily,
just use the :link reftype=hd res=2350.<Spy>:elink. notebook page.
:ewarning.
:warning text='Important!'.
For processes running in the context of the Presentation
Manager itself (that is the windowed command processors CMD.EXE and COMMAND.COM and
all processes running under these command processors and seamless WIN-OS2 sessions),
please specify :hp4.CMD:ehp4. for command processors and :hp4.SEAMLESS:ehp4. for
seamless WIN-OS2 sessions, as the executable's filename.
When using the :link reftype=hd res=2350.<Spy Window Settings>:elink. of the
Desktop dialog, you will notice that the process that runs command processors and
seamless WIN-OS2 sessions is PMSHELL.EXE, however as the Workplace Shell (WPS) is
also run by PMSHELL.EXE one can't differentiate between VIO, WIN-OS2 and WPS windows.
Therefore PC/2 translates VIO windows to match with :hp4.CMD:ehp4. and seamless
WIN-OS2 windows to match with :hp4.SEAMLESS:ehp4. instead of :hp4.PMSHELL:ehp4..
:ewarning.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration OverView Window Settings       *
.* notebook page.                                                                       *
.*--------------------------------------------------------------------------------------*
:h2 res=1900.Overview Window Settings
:p.PC/2's :link reftype=hd res=4013.<Overview Window>:elink. is the main
entrypoint of Program Commender/2.
This notebook page allows you to customize the appearance according to your preferences.
:p.Just a few words about the :link reftype=hd res=4013.<Overview Window>:elink..
The Overview Window is a scaled down representation of the Virtual Desktop of PC/2 and
the application windows residing on the Virtual Desktop.
The Virtual Desktop is represented by the client area of the Overview Window, individual
Virtual Desktops are drawn as a grid, applications windows, which may be on the current or
any Virtual Desktop, are represented by rectangles drawn onto the Overview Window.
The relation of such rectangles with real application windows can be found by the rectangle's
relative size and position or preferably by the text, which is shown when :hp9.Window Titles:ehp9.
are enabled.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Edge Activation:ehp9.
:dd.Select this option if you want to be able to unhide the Overview window when you
move the mouse pointer to an edge of the screen.
The :hp9.Delay:ehp9. allows you to optionally set a delay which must expire before the
Overview window is unhidden, if you remove the mouse pointer from the egde the timer
is reset.
Select the edge of your screen (symbolized by the rectangle) which should be sensitive
for unhiding the Overview window with the 4 buttons at the edges of the rectangle.
:p.
The sensitive area (hotspot) has the layout of a triangle that is fitted into the edges, and is
a few pixels wide, so you don't need to move the pointer exactly into the edges.
The length of the triangle's sides is calculated as a fixed percentage of 3 percent
of the horizontal resolution you are running your display driver in (e.g. that would
give for a resolution of 1280*1024 a length of 38 pixels).
The sides of the triangle that are located on the row and column surrounding the display
are not part of the sensitive area because they are used to switch between Virtual
Desktops (if they were it would be confusing as this could lead to 2 effects at the
same time, that is unhiding the Overview window and changing to the adjacent Virtual
Desktop).
:p.
When the Overview window gets unhidden and receives the input focus due to Edge Activation,
it will ensure that it hides itself again when losing the input focus (e.g. by the user
clicking onto another window to give it the input focus).
When having activated the :hp9.Sliding Focus:ehp9. this may cause some unwanted effects,
as moving the pointer without having to click onto a window will also change the input
focus, possibly causing the Overview window to hide itself.
:warning text='Note!'.
As PC/2 tries not to confuse the user, the Overview window will only be unhidden and
hidden again when losing the focus, when the Overview window was already hidden at the
time the pointer was moved into the sensitive area.
The idea behind that approach is, that if the Overview window is visible, the user
has requested that and PC/2 should not change that.
:p.
To hide the Overview window you can select between clicking on the titlebar's hide icon
or by deselecting the :hp9.Virtual Desktop:ehp9. checkbox on the
:link reftype=hd res=1800.<Virtual Desktop Settings>:elink. page 1.
:ewarning.
:dt.:hp9.Delay:ehp9.
:dd.This entryfield, which can by adjusted by the scrollbar below it, defined the time
the mouse pointer must be kept at the edge for the Overview window to be unhidden.
You may use this delay to prevent unwanted unhiding of the Overview window if you just
need to mode to an edge for a moment to e.g. resize a window or select an icon.
:dt.:hp9.Keep on Z-order top:ehp9.
:dd.Select this option if you want the Overview window to always stay on
the top of the Desktop compared to all other windows (except the ones PC/2
created itself), even if another window has the focus.
:warning text='Note:'.
To prevent possible display corruption, seamless WIN-OS2 sessions are excluded
from this logic, and may overlap the Overview Window.
Also, screen savers may not be able to draw the whole Desktop then.
:ewarning.
:warning text='Caution!'.
To to a bug in an earlier version of the PostScript printer driver which was
shipped as a coding sample some windows (most likely printer drivers) behave
peculiar when one of PC/2's window ensure itself to stay on top of Z-Order.
In technical terms, PC/2's window becomes the owner of such windows even if they
are running in a different process context (e.g. WPS).
If you would close PC/2 during this situation you can be sure that you will hang
PM and OS/2 and have to reboot soon.
As this is highly reproduceable, I've opened an APAR against OS/2, the change
team in Austin could reproduce the problem and adviced to upgrade to the latest
printer driver (in my case the PostScript driver which corrected the problem
then).
:ewarning.
:dt.:hp9.Hide Titlebar:ehp9.
:dd.To hide the titlebar of PC/2's Overview Window select this option. This increases
the space available to paint a reduced image of the Virtual Desktops.
:dt.:hp9.Hide Smarticons:ehp9.
:dd.To hide the smarticon bar of PC/2's Overview Window select this option. This
increases the space available to paint a reduced image of the Virtual Desktops too,
at the cost of not being able to manipulate (other than the currently selected
manipulation, usually :hp9.Move window:ehp9.) PM windows on the Overview Window.
:dt.:hp9.Target Cursor:ehp9.
:dd.Select this option if you want the Overview window to display a cursor depending
on the currently selected manipulation (e.g. :hp9.Move window:ehp9.) instead of the
default OS/2 pointer cursor.
As only monochrome cursors are supported by the hardware cursor function of typical
graphic cards (at least all cards I've worked with), the :hp9.Target Cursor:ehp9.,
the default OS/2 cursors may be updated faster than colorized cursors, which are
drawn by the CPU.
:warning text='Note:'.
This does not just apply to PC/2, as far as I've seen, all cursors utilizing more
than 2 colors are not drawn hardware assisted and thus slower, this includes also
the optional cursor types supplied by Warp 4.
:ewarning.
:dt.:hp9.Single Click:ehp9.
:dd.You can also switch between Virtual Desktops by clicking onto the Overview window.
By default this requires a doubleclick with mouse button 1, however by selecting this
option you can change this to a singleclick.
:dt.:hp9.3D Border:ehp9.
:dd.When having selected this option, the rectangles drawn onto the Overview window
which represent you application windows will have a three-dimensional border,
causing a 3D look and feel.
Not having selected this option will draw a black border around the rectangles
represention application windows except for the one currently having the input
focus which will be framed in red.
:dt.:hp9.Window Titles:ehp9.
:dd.Having selected this option will draw the title of the window, which is
represented by a rectangle onto the rectangle too.
This allows you to easily identify which application window a rectangle
represents, as the title is either the Titlebar of the window, or when the
Titlebar is not available the window's Window List entry.
:dt.:hp9.Color Optimization:ehp9.
:dd.Select this option to allow PC/2 to adjust the color of the :hp9.Window Titles:ehp9.
so that the contrast is a maximum, however the customized color will not be
taken then.
:dt.:hp9.Icons:ehp9.
:dd.Selecting this option will draw the application window's icon onto the
rectangle representing the application window.
Unfortunately some windows don't supply their icons to other programs, in that
case a colored rectangle is drawn.
:dt.:hp9.Gradient:ehp9.
:dd.Selecting this option will coordinate the colors used to draw the rectangles
representing the application windows in a way that the color changes in steps
from the window lowest in Z-Order to the window highest in Z-Order.
:dt.:hp9.Random:ehp9.
:dd.Selecting this option will draw the rectangles representing the application
windows in colors selected from a fixed table, where the window's Z-Order is
used as an index into that table.
:p.As the table (consisting of about 7 entries) is internal to PC/2 and one
usually can't anticipate the Z-Order of windows it looks like the color is
selected and changes randomly (e.g. window creation, destruction and even
just clicking onto a window will change while Z-Order).
This is not the case, the colors do depend on the Z-Order of the windows!
:warning text='Note:'.
The user of the Z-Order to select a window's color is the only way to get
a ordinal sequence for equal distribution of the colors defined in the table -
and the Z-Order changes (even if can not notice any visible change on your
screen) permanently, the window colors seem to change randomly.
If the CPU requirements weren't so high, I would also support to represent
a window by its downscaled image additionally.
:ewarning.
:dt.:hp9.Monochrome:ehp9.
:dd.Select this option is you want all rectangles representing application
windows to be drawn in the same color.
You still can differentiate between them by their overlapping and by their
title if the option :hp9.Window Titles:ehp9. is checked.
:dt.:hp9.Colorization:ehp9.
:dd.PC/2 allows you the customize the Overview window according to your preferences.
This includes both, color and font customization, which can be done in two ways:
:ol.
:li.You may drag colors and fonts from the color and font palette directly onto
the Overview Window.
:p.To change the rectangles (representing the application windows) colors just drag
a color from the color palette, to change the :hp9.Window Titles:ehp9. text color drag
a color while holding down the :hp4.CTRL:ehp4.-Key, and to change the background
of the Overview window itself drag a color while holding down the :hp4.SHIFT:ehp4.-Key.
:p.To change the font used to draw the window's name into the rectangle, just drag
a font from the font palette onto the Overview window.
:li.You may alternatively also drag colors onto the color areas, marked by a surrounding
box, of the :hp9.Colorization:ehp9. section.
Beside the color areas, a text descibes what color setting one color represent,
e.g. :hp9.Overview Color:ehp9. means that the color is used to draw the background
of the Overview Window.
:p.Depending on the selection of :hp9.Icons:ehp9., :hp9.Gradient:ehp9., :hp9.Random:ehp9.
or :hp9.Monochrome:ehp9. the colors in the color areas and the describing texts
change, some color areas and texts may even be removed if they are not applicable.
:eol.
:dt.:hp9.Sticky Windows:ehp9.
:dd.This multiline entryfield can be used to define some windows to be :hp9.Sticky
Windows:ehp9., that is those windows will always be kept on the currently active
Virtual Desktop.
This may be useful if you do want PC/2 to keep applications, like e.g. a Swapper
Monitor, visibly on the screen regardles which Virtual Desktops you switch between.
:p.
To allow PC/2 to know which window you are referring to, you have to enter
either the application's name from its titlebar, the application's name in the Window List
pr the application window's class name (though you will only be able to get a window's
class name by using PC/2's :link reftype=hd res=2350.<Spy>:elink. notebook
page or utilities like PMTREE or TWCP).
:p.
PC/2 adds all entries for all windows marked as :hp9.Sticky Window (Keep on active Desktop):ehp9.
at :link reftype=hd res=1450.<Hotkey>:elink. notebook pages.
Even if you remove one of those entries here, the next time PC/2 lanches they will
be added again.
The :hp9.Sticky Windows:ehp9. option here is used to get an overview about all defined
sticky windows and a way to define windows not appearing in PC/2's Popup Menu as
sticky too (in other words, just to have a window sticky, you don't need to create
a (dummy) entry for it in the Popup Menu).
:warning text='Note:'.
PC/2 will match the lines in the :hp9.Sticky Windows:ehp9. with the first window
containing that (sub)string, even if it has not been launched by PC/2 or multiple
instances of that window exist.
If some windows are kept on every Virtual Desktop you can't explain why, most likely
these windows matches with Menuentries where this checkbox has been selected.
:ewarning.
:warning text='Warning!'.
As said, since version 2.10 the :hp9.Sticky Window:ehp9. entryfield on this page will
included the windows added into the :hp9.Sticky Windows:ehp9. feature at the
:link reftype=hd res=1450.<Hotkey Settings>:elink. of an Menuentry during initialization
of PC/2.
If you delete the entry there, but do not unselect the checkbox there, the deletion will
only be effective while running PC/2, the next time you launch PC/2 it will be added again
as long as the :hp9.Sticky Window:ehp9. checkbox stays checked at the programs Menuentry.
:p.
While I admit this may be confusing, it is a liability of the design changes that had to
be introduced in the more than 6 years history of PC/2.
:ewarning.
:warning text='Note!'.
Sticky means sticky relative to the visible area of the display, that is if the
:hp9.Sticky Window:ehp9. is visible it will be visible on all Virtual Desktops you
switch to.
If the :hp9.Sticky Window:ehp9. is only partly visible, it is partly visible on all
Virtual Desktops you switch to, if it is invisible due to being just outside it will
be invisible on all Virtual Desktops you switch to (which of course means that you
must be sure what you are doing when having :hp9.Sticky Windows:ehp9. outside of
the visible display area, however you could move it to the visible area via the
Overview window or selecting Tile or Cascade from the Window List).
:ewarning.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration SessionBar Settings notebook   *
.* page.                                                                                *
.*--------------------------------------------------------------------------------------*
:h2 res=1950.SessionBar Settings
:p.The :link reftype=hd res=263.SessionBar:elink. is window that allows
you to manage your application windows easily, and in contrast to similar toos
(e.g. the Window List, WarpCenter) also takes the Virtual Desktops into account.
The :link reftype=hd res=263.SessionBar:elink.
SessionBar also supports color changes by dragging colors from the WPS Color Palette.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Show SessionBar:ehp9.
:dd.This checkbox allows you to activate PC/2's
:link reftype=hd res=263.<SessionBar>:elink., which gives you an
enhanced control to manipulate your programs' windows on the Virtual Desktops.
:dt.:hp9.Large:ehp9. Icon
:dd.Selecting this radiobutton displays the application's icons at their full
size.
:dt.:hp9.Small:ehp9. Icon
:dd.Selecting this radiobutton displays the application's icons at their half size,
that is 16 * 16 for VGA, and 20 * 20 for SVGA resolutions.
:dt.:hp9.Keep on Z-Order top:ehp9.
:dd.Select this option if you want the SessionBar window to always stay on
the top of the Desktop compared to all other windows regardless of the SessionBar's
position (except for other windows PC/2 created itself), even if another window
has the focus.
:p.The SessionBar by default will prevent other windows to cover it, however when this option
is not selected, the SessionBar allows other window to cover it if this is a result
of maximizing the window (except for seamless WIN-OS2 windows).
In other words, unless a window is maximized, the SessionBar tries to ensure that
it does not get overlayed by operations that causes the window to be moved or resized.
:dt.:hp9.BubbleHelp:ehp9.
:dd.Selecting this checkbox causes the SessionBar to display the status text that
gets displays in the SessionBar's Statusarea also to display in a popup BubbleHelp
window.
This may be helpful if you chose to activate :hp9.Hide Statusarea:ehp9. to allow the
SessionBar to display more session icons.
:dt.:hp9.Hide Statusarea:ehp9.
:dd.The Status area will not be used to display information (like e.g.
to display the :hp9.Default Date/Time:ehp9.), however an empty rectangle
will be displayed.
This is caused that the other parts of the SessionBar are at fixed sizes which
may not add up exactly to the width of the screen, and the empty Status area
will be used to fill up that excess space.
:dt.:hp9.Default Date/Time:ehp9.
:dd.Select this option to display the current date/time in the default format
provided by PC/2 in the Status area.
The default format will be for example :hp4.Mon. 1.Oct.1997 08&colon.00&colon.00:ehp4..
When this option is not checked, the format used will be :hp4.date time:ehp4.
where :hp4.date:ehp4. is the current date and :hp4.time:ehp4. is the current
time in the format selected by the current settings in the Country WPS Object.
:p.
For example, assume that in the Country object you have selected the date to
be displayed in format :hp4.Year-Month-Day:ehp4. with :hp4./:ehp4. being the
separator and the time in format :hp4.24 hours:ehp4. with :hp4..:ehp4. being
the separator, the current date/time will look like :hp4.1997/10/01 08&colon.00&colon.00:ehp4..
With the Country WPS Object you can easily change this to
:hp4.01.10.1997 08&colon.00&colon.00 AM:ehp4..
:p.
As PC/2 takes the current settings from :hp4.OS2.INI:ehp4. the last settings of the
Country WPS Object will be taken even if the Workplace Shell (PMSHELL.EXE) is
not running.
However you can't change the Country settings without PMSHELL running (though
you can edit the settings under the :hp4.PM_National:ehp4. application in OS2.INI
yourself, if you are sure what you are doing).
:p.
When you change the settings in the Country WPS Objects it may take a few seconds
until these changes are also reflected on the SessionBar.
:dt.:hp9.Hide Date/Time:ehp9.
:dd.Select this option if you are one of those people who prefer to use another
way to display the clock and do not want to have multiple applications doing the
same.
This causes the current date/time not to be displayed at all, but the Status area
is still present to display, e.g. the Session names.
:dt.:hp9.Format:ehp9.
:dd.If this entryfield is not empty, the SessionBar will display (length limited) the
Date/Time according to the data entered there (by using the C-library function strftime()).
The formatting characters are (as described in the C-library documentation, thus all
may not be useful under all circumstances):
:dl tsize=5 break=fit.
:dthd.:hp4.Code&colon.
:ddhd.Description.:ehp4.

:dt.:hp9.%a:ehp9.
:dd.Replace with abbreviated weekday name of locale.
:dt.:hp9.%A:ehp9.
:dd.Replace with full weekday name of locale.
:dt.:hp9.%b:ehp9.
:dd.Replace with abbreviated month name of locale.
:dt.:hp9.%B:ehp9.
:dd.Replace with full month name of locale.
:dt.:hp9.%c:ehp9.
:dd.Replace with date and time of locale.
:dt.:hp9.%C:ehp9.
:dd.Replace with locale's century number (year divided by 100 and truncated)
:dt.:hp9.%d:ehp9.
:dd.Replace with day of the month (01-31).
:dt.:hp9.%D:ehp9.
:dd.Insert date in mm/dd/yy form, regardless of locale.
:dt.:hp9.%e:ehp9.
:dd.Insert month of the year as a decimal number ("01"-"12"). Under POSIX, it's a
2-character, right-justified, blank-filled field.
:dt.:hp9.%E[cCxXyY] :ehp9.
:dd.If the alternate date/time format is not available, the %E descriptors are
mapped to their unextended counterparts.  For example, %EC is mapped to %C.
:dt.:hp9.%Ec:ehp9.
:dd.Replace with the locale's alternative date and time representation.
:dt.:hp9.%EC:ehp9.
:dd.Replace with the name of the base year (period) in the locale's alternate representation.
:dt.:hp9.%Ex:ehp9.
:dd.Replace with the locale's alternative date representation.
:dt.:hp9.%EX:ehp9.
:dd.Replace with the locale's alternative time representation.
:dt.:hp9.%Ey:ehp9.
:dd.Replace with the offset from %EC (year only) in the locale's alternate representation.
:dt.:hp9.%EY:ehp9.
:dd.Replace with the full alternative year representation.
:dt.:hp9.%h:ehp9.
:dd.Replace with locale's abbreviated month name. This is the same as %b.
:dt.:hp9.%H:ehp9.
:dd.Replace with hour (24-hour clock) as a decimal number (00-23).
:dt.:hp9.%I:ehp9.
:dd.Replace with hour (12-hour clock) as a decimal number (01-12).
:dt.:hp9.%j:ehp9.
:dd.Replace with day of the year (001-366).
:dt.:hp9.%m:ehp9.
:dd.Replace with month (01-12).
:dt.:hp9.%M:ehp9.
:dd.Replace with minute (00-59).
:dt.:hp9.%n:ehp9.
:dd.Replace with a new line.
:dt.:hp9.%O[deHImMSUwWy]:ehp9.
:dd.If the alternative date/time format is not available, the %O descriptors are
mapped to their unextended counter-parts.  For example, %Od is mapped to %d.
:dt.:hp9.%Od:ehp9.
:dd.Replace with the day of month, using the locale's alternative numeric symbols,
filled as needed with leading zeroes if there is any alternative symbol for zero;
otherwise fill with leading spaces.
:dt.:hp9.%Oe:ehp9.
:dd.Replace with the day of the month, using the locale's alternative numeric
symbols, filled as needed with leading spaces.
:dt.:hp9.%OH:ehp9.
:dd.Replace with the hour (24-hour clock), using the locale's alternative numeric symbols.
:dt.:hp9.%OI:ehp9.
:dd.Replace with the hour (12-hour clock), using the locale's alternative numeric symbols.
:dt.:hp9.%Om:ehp9.
:dd.Replace with the month, using the locale's alternative numeric symbols.
:dt.:hp9.%OM:ehp9.
:dd.Replace with the minutes, using the locale's alternative numeric symbols.
:dt.:hp9.%OS:ehp9.
:dd.Replace with the seconds, using the locale's alternative numeric symbols.
:dt.:hp9.%Ou:ehp9.
:dd.Replace with the weekday as a decimal number (1 to 7), with 1 representing
Monday, using the locale's alternative numeric symbols.
:dt.:hp9.%OU:ehp9.
:dd.Replace with the week number of the year (00-53), where Sunday is the first
day of the week, using the locale's alternative numeric symbols.
:dt.:hp9.&amp.OV:ehp9.
:dd.Replace with week number of the year (01-53), where Monday is the first day
of the week, using the locale's alternative numeric symbols.
:dt.:hp9.%Ow:ehp9.
:dd.Replace with the weekday (Sunday=0), using the locale's alternative numeric
symbols.
:dt.:hp9.%OW:ehp9.
:dd.Replace with the week number of the year (01-53), where Monday is the first
day of the week, using the locale's alternative numeric symbols.
:dt.:hp9.%Oy:ehp9.
:dd.Replace with the year (offset from %C) in the locale's alternative representation,
using the locale's alternative numeric symbols.
:dt.:hp9.%p:ehp9.
:dd.Replace with the locale's equivalent of AM or PM.
:dt.:hp9.%r:ehp9.
:dd.Replace with a string equivalent to %I&colon.%M&colon.%S %p; or use "t_fmt_ampm" from
LC_TIME, if present.
:dt.:hp9.%R:ehp9.
:dd.Replace with time in 24 hour notation (%H&colon.%M)
:dt.:hp9.%S:ehp9.
:dd.Replace with second (00-61).
:dt.:hp9.%t:ehp9.
:dd.Replace with a tab.
:dt.:hp9.%T:ehp9.
:dd.Replace with a string equivalent to %H&colon.%M&colon.%S.
:dt.:hp9.%u:ehp9.
:dd.Replace with the weekday as a decimal number (1 to 7), with 1 representing
Monday.
:dt.:hp9.%U:ehp9.
:dd.Replace with week number of the year (00-53), where Sunday is the first day
of the week.
:dt.:hp9.%V:ehp9.
:dd.Replace with week number of the year (01-53), where Monday is the first day
of the week.
:dt.:hp9.%w:ehp9.
:dd.Replace with weekday (0-6), where Sunday is 0.
:dt.:hp9.%W:ehp9.
:dd.Replace with week number of the year (00-53), where Monday is the first day
of the week.
:dt.:hp9.%x:ehp9.
:dd.Replace with date representation of locale.
:dt.:hp9.%X:ehp9.
:dd.Replace with time representation of locale.
:dt.:hp9.%y:ehp9.
:dd.Replace with year without the century (00-99).
:dt.:hp9.%Y:ehp9.
:dd.Replace with year including the century.
:dt.:hp9.%Z:ehp9.
:dd.Replace with name of time zone, or no characters if time zone is not available.
:dt.:hp9.%%:ehp9.
:dd.Replace with %.
:edl.
:p.For example, specifying :hp4.%A, %d.%m.%Y %T %p:ehp4. would result in a Date/Time
display of :hp4.Sunday, 16.Jul 2000 12&colon.19&colon.10 P:ehp4. (where :hp4.PM:ehp4. is
cut to :hp4.P:ehp4. due to the intended field length limit, and you may notice that
:hp4.,:ehp4. is another characters meaning that you can use characters not specified
in above table also).
:dt.:hp9.Dynamic Activation:ehp9.
:dd.The :hp9.Static:ehp9. SessionBar, static in the sense that the SessionBar is a window on
your screen which stays at its position unless you move it to a different one,
gets replaced by a :hp9.Dynamic:ehp9. SessionBar by selecting this option.
It allows you to slide the SessionBar dynamically onto the screen by just
moving the mouse pointer onto the area that would have been covered if the static
SessionBar had been visible on top or bottom of your screen (which you can
adjust with the :hp9.HotSpot Height:ehp9. spinbutton).
If the static SessionBar had been above the upper half of your screen, the sensitve
area is located on top of the screen, otherwise at the bottom of the screen.
:p.
This option is useful if you don't want to waste space on screen for the SessionBar,
but have its functionality available when you need it.
Of course, this feature may be too demanding for your hardware, from my experience I
if you have at least a Pentium based PC it will work perfectly.
:dt.:hp9.Activation Delay:ehp9.
:dd.In order to allow you to move the mouse pointer into the sensitive area without
the SessionBar sliding in immediately, you can select a delay between 0 and 2500 ms
(milliseconds) in steps of 100 ms (to be exact, your selection will be rounded up to
the next multiple of the OS/2 timer tick, which has a granularity of 32 ms for all
OS/2 versions available currently).
They :hp9.Dynamic:ehp9. SessionBar will slide in only if the specified delay has
expired and the mouse pointer is still within the sensitive area.
:p.
For example, assume you have selected 1000 ms.
This means you can leave the mouse pointer up to 1 second within the sensitive area
without the SessionBar sliding in, allowing you e.g. to press a pushbutton, select
some text, or resize a window with its sizing border.
:p.
The lower the value of the :hp9.Activation delay:ehp9., the more likely the SessionBar
will slide in unwantedly, the higher the value the longer it takes to get access to the
SessionBar's functionality.
In other words, you have to find a compromise, which likely will be in the range from
500 to 1000 ms from my experience.
:dt.:hp9.HotSpot Height:ehp9.
:dd.This options allows you to define the height of the area where the
:hp9.Dynamic Activation:ehp9. will be triggered by moving the mouse pointer
into it.
By default, this :hp9.HotSpot Height:ehp9. is initialized to the height of the
SessionBar, that is the sensitive area is the area covered by the SessionBar if it
is visible.
As :hp9.Small Icons:ehp9. allows you to reduce the height of the SessionBar to half
height, the selected :hp9.HotSpot Height:ehp9. is automatically reduce by half too.
For example, if you selected 10 pixel as the :hp9.HotSpot Height:ehp9. and also
:hp9.Small Icons:ehp9., then the effective height will only be 5 pixels.
:dt.:hp9.Performance Chart:ehp9.
:dd.This option allows you to request that a CPU Performance graphics is displayed,
though as OS/2 support is required it will not be available on all OS/2 versions,
however at least Warp 4 or Warp Server Advanced SMP will work.
This graphics draws per available CPU a chart of the CPU utilization for a selectable
period of time, the reporting intervall.
The :link reftype=hd res=263.<SessionBar>:elink. shows you how
this graphics looks like, including the popup menu to select the reporting intervall
from and a detailed description how the CPU Performance graphics works.
:dt.:hp9.Fill Mode:ehp9.
:dd.The CPU Performance graphics shows the CPU utilization divided into Interrupt,
Busy and Idle time.
Only the Interrupt and Busy times are drawn, the remaining height represents the
Idle time then.
Having selected :hp9.Fill Mode:ehp9., the utilization is drawn as a vertical line,
otherwise it is drawn just as a pixel where the pixel represents the utilization
value.
:dt.:hp9.No Grid:ehp9.
:dd.This option allows you to prevent that the grid is drawn.
However, as the grid is related to the time in a way that the its vertical lines
represent timestamps (e.g. if you selected to report the CPU Performance of the last
hour, a vertical line represents the beginning of another 10 minutes period aligned
to the time, that is the vertical lines appear exactly at 0, 10, 20, ... minutes after
a full hour) some information is lost then.
:dt.:hp9.Exception List:ehp9.
:dd.You can add the name of application windows not to be shown in the SessionBar.
To specify that a certain window of an application should be hidden, you have to add
case sensitive (at least a part of) the window's Window List entry after a :hp4.&colon.:ehp4..
For example, when you launch the sample Ultimotion video copied to your Movies folder
during OS/2 Installation, the Window List contains 2 entries, :hp4.Media Player - MACAW.AVI:ehp4.
for the player controls and :hp4.IBM Ultimotion(TM):ehp4..
:p.
In order to just hide the Ultimotion video but not the player controls and e.g. and Intel
Indeo video, you would add :hp4.Ultimo:ehp4. into the :hp9.Exception List:ehp9.,
to also hide any Intel Indeo video you would add another line containing
:hp4.Indeo:ehp4..
:p.
The specification of an window by :hp4.Windowname:ehp4. means that :hp4.Windowname:ehp4. is
a case sensitive substring of the to be hidden window's Window List entry, e.g. :hp4.Ultimo:ehp4.
matches :hp4.IBM Ultimotion(TM):ehp4., :hp4.Ultimotion:ehp4., ... but not e.g. :hp4.ULTIMO:ehp4..
:dt.:hp9.Show QuickSwitch:ehp9.
:dd.This checkbox allows you to activate PC/2's
:link reftype=hd res=262.<QuickSwitch>:elink. window, which allows you
to select and switch to sessions by pressing and holding the :hp4.CTRL:ehp4.-Key while using the
:hp4.TAB:ehp4. key for the next (and :hp4.SHIFT:ehp4. for the previous) session.
In contrast to PM's builtin :hp4.ALT+TAB:ehp4. keys, PC/2 will also switch between Virtual
Desktops if required.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Physical Desktop Settings      *
.* notebook page.                                                                       *
.*--------------------------------------------------------------------------------------*
:h2 res=2000.Physical Desktop Settings
:p.This page is used to control the Physical Desktop from PC/2.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.OS/2's Desktop Name:ehp9.
:dd.PC/2 has to know the name of the WPS process (that is the name of the window
PMSHELL.EXE creates, which is the Desktop OS/2 installs by default).
Under an English version of OS/2 2.0 it's usually named
:hp4.OS/2 2.0 Desktop:ehp4., but this changed under OS/2 2.1 to :hp4.Desktop:ehp4.,
if you haven't installed it over an existing OS/2 2.0 system.
You may rename the Desktop by activating the Window List, and clicking with mouse
button 1 on the Desktop's entry while holding the :hp4.ALT:ehp4.-Key.
Changing the title can be used to replace :hp4.OS/2 2.0 Desktop:ehp4.
by :hp4.Desktop:ehp4..
:warning text='Important!:'.
If you use a non English OS/2 version, you have to translate the Desktop
window's name to your national language, for example you would enter
"Arbeitsoberfl„che" when running a German OS/2.
You may extract the exact spelling from either the Window List
(where the Desktop is one of the running programs) or from OS/2's User's Guide.
:p.PC/2 requires the exact spelling of the Desktop window's name to locate
the WPS process under all running processes.
Having not entered the Desktop's name correctly causes some functions of PC/2
to fail or causes unexpected results!
:ewarning.
:warning text='Hint!'.
To find out the Desktop's name most easily,
just switch to the :link reftype=hd res=2350.<Spy>:elink. notebook page,
select the :hp9.Enable spying:ehp9. checkbox, and move the mouse pointer over
any area covered by the WorkPlace Shell (WPS).
:ewarning.
:dt.:hp9.Window List Name:ehp9.
:dd.PC/2 also must know the name of the Window List.
The Window List is usually named :hp4.Window List:ehp4. when running an
English version of OS/2.
:warning text='Important!'.
If you use a non English OS/2 version, you have to translate the Window
List's name to your national language, for example you would enter
"Fensterliste" when running a German OS/2.
You may extract the extract exact spelling from either the Window List
itself or from OS/2's User's Guide.
:p.PC/2 requires the exact spelling of the Window List's name to locate the
Window List window under all windows on your Desktop.
Having not entered the Window List's name correctly causes some functions of PC/2
to fail or causes unexpected results!
:ewarning.
:warning text='Hint!'.
To find out the Window Lists's name most easily,
just switch to the :link reftype=hd res=2350.<Spy>:elink. notebook page,
select the :hp9.Enable spying:ehp9. checkbox, and move the mouse pointer over
any area covered by the Window List after having it activated (e.g. by pressing
CTRL+ESC).
:ewarning.
:dt.:hp9.Button 2 Titlebar click lowers Z-order:ehp9.
:dd.Select this checkbox if you want to click down mouse button 2 on any window's
titlebar to set this window to the bottom of all windows. This moves the window
you clicked on behind all other windows, lowering its Z-Order to the minimum.
The window under the mouse pointer is moved to the foreground and activated, if
it is not the same you clicked on.
:p.By holding the :hp4.CTRL:ehp4.-Key while clicking with mouse button 2 on
a window's titlebar, the window under the mouse pointer gets not moved to the
foreground and activated. This allows you to kick a window into the background
without affecting the Z-order of other windows.
:p.By holding the :hp4.Alt:ehp4.-key while clicking with mouse button 2 on
a window's titlebar, the titlebar acts as if you haven't checked PC/2's
option :hp9.Button 2 titlebar click lowers Z-order:ehp9.. This allows you to move
the window or activate any special features the window supports.
:dt.:hp9.Button 2 Border click moves window:ehp9.
:dd.Select this checkbox if you want to be able to move a window by simply
clicking down mouse button 2 on the window's border, and then dragging the window
to its new position.
You may try to move a window if it has no border at all (e.g. usually found
on dialog windows) as PC/2 tries to handle that window as if it had a
sizing border.
:dt.:hp9.Use Spooler Control Window:ehp9.
:dd.PC/2 provides a Spooler Control Window, which allows you to manage the
printer queues and jobs of your local workstation.
This feature is primarily to provide printer management, when PC/2 is
running as a WPS replacement, because the standard OS/2 Spooler management
is a SOM application, and requires the WPS to be running.
:p.By default PC/2 starts the Spooler Control Window only, when PC/2 is
running as a WPS replacement.
When selecting this checkbox, the PC/2 Spooler Control Window is also
available when you run PC/2 on top of the WPS (which increases PC/2's
memory requirements slightly).
Changes of this setting aren't available immediately, but the next time
you start PC/2.
:dt.:hp9.Background Bitmap:ehp9.
:dd.Check this option to add the background Bitmap support of PC/2. PC/2
then allows you to define a bitmap, which is displayed on PM background.
PM background means all the background of OS/2 that is not covered by the
WPS. For example if you have 3 Virtual Desktops, having the WPS as the
centered one, then the 2 Virtual Desktops beside the WPS will display the
background bitmap.
:dt.:hp9.Normal:ehp9.
:dd.This option displays the bitmap unsized. The bitmap is centered.
:dt.:hp9.Scaled:ehp9.
:dd.This option scales the bitmap so that is covers the complete screen.
:dt.:hp9.Tiled:ehp9.
:dd.The bitmap is tiled covering the complete screen with the tile count
selected in the :hp9.Tile count:ehp9. spinbutton. You can select a tile
count of 1 to 99 although OS/2 seems to limit this to about 20.
:p.:warning text='Caution!'.
Under OS/2 3.00 Warp this seems to hang OS/2
completely, requiring a soft reset.
A defect (PMR 4X472) has been opened against OS/2 Warp, so this may be fixed by a
future CSD (Corrective Service Diskette).
:p.This bug is not a problem of PC/2, since it works correctly under
OS/2 2.x.
:ewarning.
:dt.:hp9.Find Bitmap...:ehp9.
:dd.Select this pushbutton to use the standard OS/2 file dialog to locate
the bitmap file you want to display.
:dt.:hp9.Path to Bitmap:ehp9.
:dd.Either fill this entryfield by selecting a bitmap with the file
dialog by clicking on :hp9.Find Bitmap...:ehp9. or enter the fully qualified
path to  the bitmap manually.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration General PC/2 Settings notebook *
.* page.                                                                                *
.*--------------------------------------------------------------------------------------*
:h2 res=2050.General PC/2 Settings
:p.This page is used to control PC/2 settings that have a general character.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Ignore Autostart:ehp9.
:dd.Check this item to ignore the :hp9.Autostart:ehp9. style of applications
in PC/2's Popup Menu, without having to remove this style
from all applications individually.
You may find this feature useful when temporarily exiting PC/2
and reloading PC/2 without launching the applications having the
:hp9.Autostart:ehp9. style set a second time.
:p.You may also specify the commandline option :hp4.-NoAutostart:ehp4., as
described in :link reftype=hd res=4008.<General Help for PC/2>:elink.,
to get the same behaviour.
:p.
If this option was checked but the profile did contain items that included the
Autostart option, unchecking this option causes the Autostart cycle to begin
(e.g. if you deactivate this option 30 minutes after you have launched PC/2,
the Autostart delay will start counting from 0 at that moment).
:dt.:hp9.Ignore Scheduler:ehp9.
:dd.Check this item to ignore scheduling requests from applications
in PC/2's Popup Menu, without having to remove the scheduler settings
from all applications individually.
:p.You may also specify the commandline option :hp4.-NoScheduler:ehp4., as
described in :link reftype=hd res=4008.<General Help for PC/2>:elink.,
to get the same behaviour.
:dt.:hp9.No systemwide Hotkeys:ehp9.
:dd.Check this item to allow PC/2 Hotkeys to be detected only, if
either PC/2, the Window List, PM or the WPS is active window when pressing a
Hotkey.
This prevents PC/2 from stealing Hotkeys from your application
software you are working with.
:p.For example, if you have defined the Hotkey :hp4.CTRL+U:ehp4. to
start a host upload program, you usually don't want to invoke this upload,
when pressing :hp4.CTRL+U:ehp4. in :hp4.EPM:ehp4. to undo a typing mistake.
:dt.:hp9.Diagnostics:ehp9.
:dd.When this option has been checked, PC/2 will activate advance diagnostic
facilities inside OS/2 that are equivalent to having added the :hp4.DUMPPROCESS:ehp4.
and :hp4.SUPPRESSPOPUPS:ehp4. statements to the :hp4.CONFIG.SYS:ehp4. (please
see the online OS/2 command reference for a detailed description.
:warning text='Caution!'.
Please see the detailed information in the section
:link reftype=hd res=4015.<PC/2 trap/bug diagnostics>:elink.,
as once these settings have been activated, your system will behave differently!
:ewarning.
:dt.:hp9.Normal ShutDown (WPS method):ehp9.
:dd.Check this item if you want PC/2 to perform a Shutdown of OS/2
the way the WPS does, when selecting ShutDown from its context menu.
When this item is checked, and you select :hp9.ShutDown:ehp9. from PC/2's
Popup Menu or SmartIcon, OS/2 informs all application that OS/2 is going to
shut down, giving them a chance to write unsaved changes to disk.
Some applications, e.g. OS/2 command processors, even ask if you are sure
to close them.
:p.Without selecting this item, PC/2 will Shutdown OS/2 in a way only the
filesystem buffers are cleared, called :hp9.Fast ShutDown:ehp9..
:dt.:hp9.No verify ShutDown:ehp9.
:dd.Selecting this option disables the message box, that verifies that the
user really has requested to ShutDown OS/2 having either selected the Fast
ShutDown method or the method used by the WPS.
:dt.:hp9.Reboot:ehp9.
:dd.If you want to reboot your workstation automatically after a Fast
ShutDown, check this option. This may be useful when you want to ShutDown
and reboot in a single step.
As you may also schedule a Shutdown of OS/2 with PC/2's
:link reftype=hd res=1600.<Scheduler>:elink.,
you can automatically reboot your workstations afterwards by selecting
this option.
This feature will be useful to reboot for example your server regularly at
midnight without manual intervention.
:warning text='Note:'.
To reboot OS/2, PC/2 requires the :hp9.DOS.SYS:ehp9. device driver to be installed.
Ensure that for OS/2 2.11 the line DEVICE=x&colon.\OS2\DOS.SYS, and for
OS/2 WARP the line DEVICE=x&colon.\OS2\BOOT\DOS.SYS is contained in your
CONFIG.SYS, where x is the the drive letter you installed OS/2 at, e.g. E).
This line may only be missing for non standard OS/2 Installations, e.g.
maintenance partitions created by
:link reftype=hd res=4014.<BOOTOS2>:elink..
:ewarning.
:dt.:hp9.Support APM:ehp9.
:dd.Select this option to allow PC/2 using anything from APM (Advanced Power
Management).
Under Warp Advanced Server SMP (on a dual Pentium-Pro powered IBM Intellistation Z Pro) I
got reproducable kernel hangs whenever PC/2 touched the APM.SYS device driver.
I have no idea why that happened, because from my commandline based :hp2.APM/2:ehp2.
program (available from my homepage) APM did not hang under that OS/2 version,
however the number of threads, APIs called, program's size, ... differ and that
may cause that difference in behaviour.
:warning text='Note:'.
To service APM requests, PC/2 requires the :hp9.APM.SYS:ehp9. device driver to be
installed and APM enabled from the Power WPS Object.
Ensure that for OS/2 2.11 the line DEVICE=x&colon.\OS2\APM.SYS, and for
OS/2 WARP the line DEVICE=x&colon.\OS2\BOOT\APM.SYS is contained in your
CONFIG.SYS, where x is the the drive letter you installed OS/2 at, e.g. E).
This line may be missing if you didn't request APM support during installation
of OS/2 (just start the OS/2 installation to selectively install APM support)
or your PC doesn't support APM (which should be described in the User's Guide
of your PC).
Even if APM is installed, APM requests may fail as there can be different
implementations of APM (you may try to update your BIOS) and there is no easy
way to query the implementation details.
:ewarning.
:warning text='Note:'.
Even if APM requests seem to work, it may hang OS/2 (e.g. as suspend requests
do on my desktop PC, where my ThinkPad has no problem).
I suspect this mainly depends on the PC hardware and the BIOS implementation.
:ewarning.
:warning text='Warning!'.
Only the APM device drivers beginning with Warp 4 support power off requests,
previous versions also power off your workstaation, though no ShutDown of OS/2
is performed and :hp9.CHKDSK:ehp9. will be run during the next boot.
Do not copy Warp 4's APM device drivers to previous versions of OS/2, as there
are also dependencies upon other system files including the OS/2 kernel!
:ewarning.
:warning text='Hint!'.
Today's workstation allow you to define a date and time when your system powers
on automatically and boots by booting into your PC's setup.
This is supported by APM 1.2 specification, so you may want to look into
your workstation's User's Guide for more information.
OS/2 currently supports only the APM 1.1 specification, but support for 1.2
is available from the DDKPACK Web-site or beginning with FP35 respectively
FP6.
PC/2 may support to query and set the APM date and time settings from its
dialogs in a future version as :hp2.APM/2:ehp2. (available from my
homepage) does.
:ewarning.
:warning text='Shameless Plug!'.
If you want much more control about APM you may want to look into my APM/2
utility V1.40 which is available at
:hp4.http&colon.//www.geocities.com/SiliconValley/Pines/7885/:ehp4. or
the major OS/2 anonymous-ftp servers.
This utility is a commandline base applications that allows you to execute almost
all requests to OS/2's APM subsystem that the APM.SYS 1.2 driver supports.
As always, your mileage may vary regarding which features actually work!
:ewarning.
:dt.:hp9.Enable APM:ehp9.
:dd.This option is equivalent to the Power Management options :hp9.On:ehp9.
and :hp9.Off:ehp9. of the Power WPS Object.
When running PC/2 as the WPS, or you have not selected APM support during your
initial OS/2 installation (thus not having the Power WPS Object) but added
APM.SYS later on, you can therefore control the state of the APM too.
:warning text='Note:'.
However if you run PC/2 under the WPS the settings of the Power WPS Object
and from PC/2 are a compromise, the one invoked later (usually PC/2 as
the WPS gets loaded previously) defines the actual state of APM.
For example, if the Power WPS Object is in APM disabled state, but PC/2
is in APM enabled state, PC/2 will enable APM when being invoked later
than the WPS.
:p.I said this is a compromise, because PC/2 and the WPS can't communicate
about APM support.
If you want to disable APM support for sure, disable APM from the Power WPS
Object and from PC/2.
You may even uncomment the APM.SYS from your CONFIG.SYS or adjust APM
support in your BIOS setup to totally disable APM with the next reboot.
:p.By the way, adjusting APM support in your BIOS is the best solution to
prevent unwanted APM events for e.g. servers that run around the clock).
Adjusting does not necessarily mean turning off APM completely, for
example you may want that the CPU is kept a full speed but the display
is put into a power-saving mode when no user activity is detected.
For a server you may want to prevent the harddisks from spinning down
(which is unlikely anyway as mostly only (E)IDE drives support spinning
down and it is a nonsense to have a server with (E)IDE drives, you'll
get decent disk performance only with SCSI or SSA drives) but for a
workstation you may allow the harddisk spinning down even when the CPU
is kept on running at full rate (to spend its cycles in e.g. RC5 cracking
efforts or similar ;-).
:ewarning.
:dt.:hp9.Suspend:ehp9.
:dd.This option allows you to suspend your PC or LapTop via APM (Advanced
Power Management).
:dt.:hp9.Pwr Off:ehp9.
:dd.This option allows you to power off your PC or LapTop via APM (Advanced
Power Management).
Note that Warp 4 is required in order to avoid a CHKDSK during the next
reboot.
:warning text='Warning!'.
APM Power Off may not work on all systems despite they are supporting APM and
APM.SYS works correctly.
Symptoms are (thanks to Frank Schroeder from the OS/2 I/O Subsystem/Device
Driver Development, who owns the OS/2 APM driver, for his support to give me
some explanations about possible causes):
:ul compact.
:li.APM power off does power off your PC, but even under Warp 4 a CHKDSK is
performed due to the next reboot.
This may be related to the combination of a too fast processor and a too slow
harddisk, because the request to the disk subsystem didn't finish in time
before the power is dropped (this may corrupt partitions).
:li.Your PC is powered off, but immediately powers on again.
This may be related that the APM BIOS thinks that is should power up again,
which can be caused by "Wakeup on LAN" adapters or that OS/2's APM support
polls APM BIOS which confuses APM BIOS.
:li.APM power off does not work under OS/2 but does under Windows.
As OS/2 (as a protected mode operating system) may use different entrypoints
into APM BIOS as Windows, there are some chances that there are BIOS bugs.
Even when the same BIOS code is used, timing issues ad differences in the
frequency of polling, the order of APIs called, the method of APM event
notifications,... can explain differences.
:eul.
:ewarning.
:dt.:hp9.Don't verify:ehp9.
:dd.Selecting this option disables the message box, that verifies that the
user really has requested to PowerDown OS/2.
:dt.:hp9.Enable PowerDown Timer:ehp9.
:dd.Checking this item enables the :hp9.Automatic PowerDown Timer:ehp9..
Once activated, the PowerDown Timer detects no user activity for a certain period
and suspends or powers down your workstation after the period has expired
automatically.
:dt.:hp9.Automatic PowerDown Timer:ehp9.
:dd.This spinbutton allows you to defines the period after which your workstation
is suspended or powered down automatically when PC/2 does not detect any user
input for this period.
:warning text='Note:'.
PC/2 only detects activity for the Presentation Manager (PM) screen group,
switching to any fullscreen session is cause PC/2 to clear and suspend the PowerDown
Timer causing it never to expire.
This is a security concern you should take into account when switching to fullscreen
sessions.
:ewarning.
:dt.:hp9.Adv. Marking:ehp9.
:dd.Select this option to enable the Advanced Marking feature of PC/2.
Advanced Marking allows you to copy to or paste from the OS/2 clipboard
by simple mouse clicks.
Advanced Marking is supported for entryfields, multiline entryfields, AVIO
(OS/2 and DOS windowed sessions) windows, E editor, EPM, IBM Personal
Communications/3270, Advantis PassPort/2, IBM Communications Manager/2,
NetScape Navigator/2 for both version 2.02 and 4.04
(additionally PC/2 fixes a bug/design flaw for
NetScape 2.02 in that it does not empty the clipboard before copying data
into it, which can cause that data is in the clipboard which comes from
different applications/incompatible formats) and Lotus Notes 4.52 windows.
Additionally, you can copy the text from button and static text controls
(but of course you can't paste into those controls), by
dragging the mouse while having pressed mouse button 1 (e.g. useful to
copy a URL in NetScape's download dialog that is too large to be visible
completely and can't be taken from the web-page directly due to being the
result of a http-redirect request or cgi-bin applet).
:p.Select the text to copy into the clipboard by dragging the mouse pointer
while having mouse button 1 pressed, around the text you want to select
When you release mouse button 1, the selected text is automatically copied
into OS/2's clipboard.
:p.If you want to temporarily disable Advanced Marking, then ensure that you
press and hold one of the :hp4.SHIFT:ehp4.-Keys while performing the copy or
paste operation.
:p.If you want to select the text without automatically pasting your selection
into OS/2's clipboard, just press and hold the :hp4.ALT:ehp4.-Key while marking the text
(except for IBM Communcations Manager/2, which does not allow any key to be
pressed while marking text).
:p.To paste text from OS/2's clipboard simply doubleclick with mouse button
2. The text gets automatically pasted from the clipboard then.
Unfortunately multiline entryfields already select the current line on
dobuleclicks with mouse button 1, so Advanced Marking has only mouse button
2 available.
:p.If you paste text from the OS/2 clipboard into a OS/2 or DOS windowed
command prompt, trailing \r and/or \n (CRLF) characters are removed in
order to prevent the immediate execution of the pasted text as a command.
This may help you to create a command line from more than 1 paste operation.
However, a single \r\n character sequence is added, if you press and hold
the :hp4.CTRL:ehp4.-Key while pasting the text.
:p.You'll never have to select Mark, Copy or Paste from a DOS
or OS/2 window's system menu, from the OS/2 E and EPM editors, or from a
window of IBM's Personal Communications/3270, Advantis PassPort/2,
IBM's Communications Manager/2, NetScape Navigator/2 and Communicator/2
or Lotus Notes again!
Of course you may operate Advanced Marking cooperatively with any
other application that copies to or pastes text from the OS/2 clipboard,
for example your favourite word processor.
:warning text='Note:'.
Under WARP 4 you should turn off VIO marking from
the System notebook when using PC/2's Advanced Marking.
:p.If you are not sure what and when something gets copied into the OS/2
clipboard, just open the clipboard (e.g. by opening its object or invoking
CLIPOS2.EXE).
:ewarning.
:warning text='Note:'.
Can the original mouse functionality, which has been overlayed by PC/2's
Advanced Marking, be activated?
Unfortunately the answer is no (as there is no consistant key/mouse
event combination that would work for all supported applications).
:ewarning.
:dt.Button :hp9.2:ehp9.
:dd.Advanced Marking uses mouse button 2 as described above.
:dt.Button :hp9.3:ehp9.
:dd.Advanced Marking replaces the mouse 2 as described above with mouse button 3.
As applications may have defined other operations with mouse 2, which get
hidden when this button is used for Advanced Marking, using button 3 on a
3-button mouse may be a useful alternative, as this button is
very likely not widely used by applications (simply because many mice don't
have that button, like the "standard" IBM PS/2 mouse, e.g. IBM's Intellistation
workstation mice do have 3 buttons).
:dt.:hp9.Dyn. MenuSelection:ehp9.
:dd.Select this option to enable the Dynamic MenuSelection feature of PC/2.
Dynamic MenuSelection automatically selects the Menuentry below the mouse pointer,
when moving the mouse pointer over a menu's various Menuentries.
Dynamic MenuSelection gets activated when you select a Menuentry of a
menu until the menu is dismissed.
:warning text='Note:'.
The Dynamic MenuSelection is based that Menuentries within a menu are unique,
that is they can be identified by a unique identifier.
Unfortunately this may not be the case for all menus, e.g. the Bookmarks submenu
of NetScape Communicator/2 4.04 contains Menuentries all having the same
non-unique identifier of 0, which is completely valid for PM but unusuable for
PC/2.
:p.
In those cases the Dynamic MenuSelection will not work (in detail it will only
select the first Menuentry having the non-unique identifier).
A workaround is to select Menuentries while having mouse button 1 pressed, which
of course will not activate the Dynamic MenuSelection, but it prevents PC/2 from
selecting always the first Menuentry with the non-unique identifier.
:ewarning.
:dt.:hp9.Cascade Submenus:ehp9.
:dd.Select this option to request PC/2 to recursively open Submenus within
the currently selected Submenu.
:warning text='Note:'.
This option will have an effect only when a menu is operated by the mouse pointer.
If you are operating the menu with the keyboard and have selected :hp9.Keyboard:ehp9.
to enable the Dynamic MenuSelection for the keyboard too, this option will have
no effect (due to the way PM menus work).
:ewarning.
:p.
In other words, this option will recursively open the Menuentry :hp4.FileManagers:ehp4.
when selecting the Menuentry :hp4.Desktop:ehp4. from a menu of the structure
:hp4.Desktop&rarrow.OS/2&rarrow.Apps&rarrow.FileManagers:ehp4. (assuming that all
menuentries are submenus).
Not having selected this option, only the Submenu :hp4.OS/2:ehp4. would have been
opened.
:dt.:hp9.Keyboard:ehp9.
:dd.Select this option to request PC/2 to perform the Dynamic MenuSelection also
when operating a menu with the keyboard.
Due to the way menus are implemented by PM, selected submenus may recursively
cascade open partly, ignoring the state of the :hp9.Cascade Submenus:ehp9. settings.
This is not as perfect as I want it and is subject of future improvements (technically,
this is a very complicated stuff, just see the source to verify that, and also very
difficult to debug, one can't use PM debuggers to debug the SIQ and adding traces often
does either flood with information or is not conclusive, I'm confident that I'll be
able to fix that using ICAT as some point).
In case of problems just disable the Keyboard support of Dynamic MenuSelection.
:warning text='Note:'.
As the mouse pointer is not moved but the mouse generates messages regularily, those
messages will cause another menuentry to be selected in contrast to what you will be
selecting with the keyboard (because there is no way in PM to differentiate if a selection
of a menuentry is caused by the mouse or keyboard).
:p.
Also as the keyboard operation works directly on menuentries, the :hp9.Activation Delay:ehp9.
is ignored for menuentries that are directly selected by the keyboard (in other words, when
you press the cursor down key to select the next menuentry downwards, the entry is selected
immediately, because you requested that by the cursor down key, however if the menuentry is
a submenu, the recursive cascading will observe the requested delay).
:p.
To avoid that in general, ensure that the mouse pointer is not covered by any menuentry
of the menu and for PC/2's Popup Menu I recommend to disable the :hp9.Preselect First Item:ehp9.
or :hp9.Select Menu Center:ehp9. options.
Also, it is not possible to close a submenu with the :hp9.ESC:ehp9.-key when the pointer
is positioned over the "parent" menuentry.
:p.
I have also seen that this setting may cause a menu to permanentely flip between menuentries
(e.g. in ICSDEBUG), you may be able to recover from that by :hp9.CTRL+ESC:ehp9. only.
:ewarning.
:dt.:hp9.Activation Delay:ehp9.
:dd.You may select that the menuentry below the mouse pointer does not get selected
immediately but after a configurable delay.
This may be useful when having icons within a menu (especially PC/2's Popup Menu)
as that may avoid some unnecessary drawing of menuentries and submenus.
Use this scrollbar to customize that delay between 0 and 2500 ms (milliseconds)
in steps of 100 ms (to be exact, your selection will be rounded up to the next
multiple of the OS/2 timer tick, which has a granularity of 32 ms for all OS/2
versions available currently).
Dpending on the system load, the selected delay may be extended additionally.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.


.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration SmartIcons Settings notebook   *
.* page.                                                                                *
.*--------------------------------------------------------------------------------------*
:h2 res=2100.SmartIcons Settings
:p.This page is used to control PC/2 settings that add SmartIcons to all windows
(unless you excluded some processes) titlebars to add functionality such as a Close
or RollUp window.
:warning text='Warning!'.
Due to the way Titlebar Smarticons are implemented in PC/2, PM may hang when you exit
PC/2 (requireing a reboot to regain control over PM).
I thus do recommend not to use that feature if you plan to restart PC/2 frequently,
and to use some other Titlebar enhancers (until I find the time to redo the implementation
in a better way).
:ewarning.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Show Icons:ehp9.
:dd.Select this option to enable the Titlebar Smarticon feature of PC/2.
PC/2 will then add the selected Smarticons into the titlebar of all windows
having a titlebar.
These Smarticons provide a point and click access to functions not accessible
immediately, or not available at all, to increase your productivity.
:p.Depending on the version of OS/2 you are using, you will get the Titlebar
Smarticons in either the left WARP 4 (aka "Merlin") or right WARP 3 layout:
:dl tsize=6 break=all.
:dt.:artwork name='Source\Resource\Merlin\CloseWindow.Up.bga' runin.
:artwork name='Source\Resource\Warp\CloseWindow.Up.bga' runin.:hp9.Close Window:ehp9.
:dd.This option will close your window, but will only be added to windows
that don't have a close button yet under Warp 4.
This feature will try to close a window by simulating user action,
depending on the existence of:
:ol compact.
:li.a Ok pushbutton (usually the leftmost pushbutton on dialogs and
message boxes),
:li.a Yes pushbutton (usually the leftmost pushbutton on message boxes),
:li.a Enter pushbutton (usually the default selection on message boxes),
:li.a Cancel pushbutton (usually the second pushbutton on dialogs and
message boxes),
:li.the Close Menuentry in the system menu of the window (when available),
:li.the Close Menuentry in the system menu (by posting directly to the
client area, if the system menu of the window does not contain the Close
Menuentry).
:eol.
:dt.:artwork name='Source\Resource\Merlin\Rollup.Up.bga' runin.
:artwork name='Source\Resource\Warp\Rollup.Up.bga' runin.:hp9.Rollup Window:ehp9.
:dd.This option will "roll up" a window to the height of the titlebar.
The smarticon will then change to the :hp9.Rolldown Window:ehp9., the next
click on it will "roll down" the window to its previous size.
This feature as an alternative to minimizing a window will help you to clean
up a crowded Desktop.
:p.Unfortunately all windows not be restored correctly, or even worse, when
you close a window while being rolled up, it may remember its "tiny" size.
Because of this, the Rollup button has been removed between Merlin Beta and
Merlin GA, however as this has proven a useful feature, PC/2 implements it.
:dt.:artwork name='Source\Resource\Merlin\ContextHelp.Up.bga' runin.
:artwork name='Source\Resource\Warp\ContextHelp.Up.bga' runin.:hp9.Context Help:ehp9.
:dd.When clicking onto the Context Help smarticon, PC/2 tries to activate
the context sensitive help for the control (e.g. entryfield, pushbutton,...)
having the input focus.
If activating the Context Help fails, PC/2 will try to activate the
General Help.
:p.Unfortunately OS/2 2.x behaves differently compared to OS/2 WARP and does
not work very well at all.
:dt.:artwork name='Source\Resource\Merlin\MoveVirtualDesktop.Up.bga' runin.
:artwork name='Source\Resource\Warp\MoveVirtualDesktop.Up.bga' runin.:hp9.Move to Desktop:ehp9.
:dd.When clicking onto this smarticon, PC/2 moves the window onto the
next adjacent Virtual Desktop, if there is one.
Depending where on the smarticon you clicked on, PC/2 will move the window
to the next Desktop above, to the right, below or to the left of the
current Physical Desktop.
:p.PC/2 detects the mouse clicks by applying an invisible grid, one axis
going from the upper left to the lower right edge, and one axis going
from the lower left to the upper right edge.
Depending on which of the 4 quadrants you have clicked on, the corresponding
Virtual Desktop will be selected.
:dt.:artwork name='Source\Resource\Merlin\MoveEdge.Up.bga' runin.
:artwork name='Source\Resource\Warp\MoveEdge.Up.bga' runin.:hp9.Move to Edge:ehp9.
:dd.When clicking onto this smarticon, PC/2 moves the window into a edge of
the current Physical Desktop.
Depending where on the smarticon you clicked on, PC/2 will move the window
to the upper left, upper right, lower right or lower left edge.
:p.PC/2 detects the mouse clicks by applying an invisible grid, its
a Cartesian grid going through the smarticons's center.
Depending on which of the 4 quadrants you have clicked on, the corresponding
edge will be selected.
:edl.
:warning text='Caution:'.
Adding Titlebar Smarticons has severe impacts to how
OS/2 processes, if you experience hangs/slowdowns you may want to disable this
feature or at least add the failing applications into the Exception List.
The problems arise from the way subclassing windows is implemented in OS/2
and Windows, too (technically, when undoing subclassing, pointers to window
procedures that are no longer valid may be passed to PM, causing a access
violation the next time a message is routed to that invalid address).
There is also a systemwide limit of 128 windows PC/2 can add Smarticons into.
:p.You may very likely get into troubles when adding PC/2's smarticons under
OS/2 2.x or if you use other programs that add icons to all titlebars
(e.g. VoiceType Dictation, NPSWPS, XIT, NETGUI, ...).
:ewarning.
:warning text='Implementation details'.
In more technical terms, to add
Smarticons into the titlebars of all windows available systemwide PC/2 does:
:ol compact.
:li.Install an systemwide Input and SendMsg hook (and as a systemwide hook must
reside in a DLL, PC/2 can get access to code and data shared in this DLL between
all processes).
:li.Broadcast a message to all frame windows.
This message is then processed in the Input hook in the process context of the
receiving window (yes, we're running our code in a different address space!).
The window is registered in a array (counting against the mentioned limit of 128
window) and the frame window gets subclassed by a window procedure also residing
in the DLL,
and a message gets sent to update the frame controls.
:li.The message sent to update the frame controls is then processed in the subclassed
window procedure (which is shared between all subclassed window, we find the
context in the array by lookup with the window handle).
The bitmaps of the smarticons get loaded, and ownerdrawn static text controls
are created and added to the list of frame controls.
:li.When the user interacts with the Titlebar Smarticons, a paint event gets
sent and PC/2 repaints the smarticons accordingly.
:li.To undo the subclassing a message is posted again.
In the subclassed frame window procedure the message is processed by undoing the
subclassing, destroying the resources and posting a message to the original
frame window procedure to update its frame controls.
:eol.
:p.I'm not sure why PC/2 has problems under OS/2 2.x.
The reason why one should not use more than one program that add smarticons
to all windows titlebars is that there seems to be no way to break the LIFO
(last in, first out) principle when subclassing the same window procedure
multiple times (and when using multiple programs there is no guarantee
that the LIFO principle is kept).
:ewarning.
:dt.:hp9.Exception List:ehp9.
:dd.You can add the name of executables that make problems/don't work correctly,
when PC/2 adds Smarticons to the Titlebars of their windows, into this
multiline entryfield.
:p.Just specify unqualified filenames, for example :hp4.FDISKPM:ehp4. to exclude
the executable x&colon.\OS2\FDISKPM.EXE.
:warning text='Note:'.
OS/2 supports case sensitive filenames, though HPFS currently
is only case preserving.
Therefore, PC/2 requires you to enter the executable's filename with respect to
filename case sensitivity.
If you are not sure about the exact case sensitive spelling of your executable,
invoke a OS/2 :hp4.DIR:ehp4. command against the executable.
As PC/2 performs an exact match, the longer the Exception List, the more comparisms
are needed, so please keep your list as small as possible.
:ewarning.
:ewarning.
:warning text='Hint!'.
To find out the executable that created a window most easily,
just use the :link reftype=hd res=2350.<Spy>:elink. notebook page.
:ewarning.
:warning text='Important!'.
For processes running in the context of the Presentation
Manager itself (that is the windowed command processors CMD.EXE and COMMAND.COM and
all processes running under these command processors), please specify :hp4.CMD:ehp4.
as the executable's filename.
When using the :link reftype=hd res=2350.<Spy Window Settings>:elink. of the
Desktop dialog, you will notice that the process that runs these command processors
is PMSHELL.EXE, however as the Workplace Shell (WPS) is also run by PMSHELL.EXE one
can't differentiate between VIO and WPS windows.
Therefore PC/2 translates VIO windows to match with :hp4.CMD:ehp4. instead of
:hp4.PMSHELL:ehp4..
:ewarning.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.


.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Security Settings notebook     *
.* page                                                                                 *
.*--------------------------------------------------------------------------------------*
:h2 res=2150.PC/2 Security Settings
:p.This page is used to control the PC/2 Security feature. This feature allows you to
protect your workstation from possible intrusion:
:ul.
:li.You may prevent access to your workstation by activating the
:link reftype=hd res=1100.<Lockup dialog>:elink.
from the :hp9.Lockup OS/2:ehp9. Menuentry.
Once activated - either manually or after a specified time of no user input activity -
you can only get back access to your system by entering the Lockup Password.
Even a reboot via :hp4.ALT+CTRL+DEL:ehp4. is disabled then.
:warning text='Caution!'.
Please take care of the following limitations of the Lockup dialog.
First, if you have configured the OS/2 Lockup dialog to be activated automatically,
this dialog will take precedence over PC/2's lockup dialog.
As the OS/2 Lockup dialog is part of the WPS (PMSHELL), this problem does vanish if
you are not running the WPS, that is e.g. using PC/2 as the WPS replacement.
Second, if another program opens a systemmodal window (that is a window that prevents
all other windows to receive mouse and keyboard input) while PC/2's Lockup dialog is
active, this dialog takes precedence, however also the opposite is true.
In summary, if you concurrently use programs that use systemmodal windows, security
may suffer (all but one such window will no longer by systemmodal).
:ewarning.
:li.You may protect the most critical Menuentries in PC/2's Popup Menu,
that is the :link reftype=hd res=302.<Desktop Configuration
dialog>:elink., the :link reftype=hd res=301.<Setup dialog>:elink. and the
:link reftype=hd res=4013.<Exit PC/2>:elink. function.
Prior to getting access to these Menuentries, you have to enter the Access Password
in the :link reftype=hd res=1101.<Lockup dialog>:elink. first.
Just define an :hp9.Access Password:ehp9. and select the option :hp9.Protect Popup Menu:ehp9..
:p.
You can also protect launching of executables by checking the :hp9.Password
Protected:ehp9. option on the
:link reftype=hd res=1400.<Style>:elink. notebook page (of the Program
Installation dialog).
:eul.
:p.The Security feature allows you to define passwords of up to 32 alphanumeric characters,
preserving case sensitivity (that is a password entered in lowercase does not match the
same password entered in uppercase). The password is entered and saved in an encrypted
form, so its not trivial to hack, but provides by no means absolute security (because
PC/2 includes its source code in the package).
:p.Any encrypted entryfield you are requested to enter a password in does not support
all processing you may know from normal entryfields, e.g. you can't paste into or
the DELETE character clears all characters entered.
:p.You may define that the Lockup feature gets activated after a customable period of
user inactivity and immediately after a boot, which does not prevent any Autostart
program to be started (e.g. allowing you to start a peer service).
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Access Password:ehp9.
:dd.Enter the password you want to use for PC/2's Access protection feature into this entryfield.
You may enter up to 32 alphanumeric characters with case sensitivity.
Once the :link reftype=hd res=1101.Password Dialog:elink. is
displayed enter exactly the same character string to dismiss that dialog.
:warning text='Hint:'.
You may also use an empty string as the Access Password,
however its your decision if this is safe enough.
:dt.:hp9.Verification:ehp9.
:dd.You are required to enter the password entered in the :hp9.Access Password:ehp9.
entryfield twice to verify you didn't make any misspells.
:dt.:hp9.Protect Popup Menu:ehp9.
:dd.Having checked this item protects the access to the
:link reftype=hd res=302.<Desktop Configuration dialog>:elink.,
the :link reftype=hd res=301.<Setup dialog>:elink. and the
:link reftype=hd res=4013.<Exit PC/2>:elink. with the :hp9.Security
Password:ehp9.
You have to enter the Password in the :hp9.Password Dialog:ehp9. correctly prior
getting access to these functions.
:dt.:hp9.Lockup Password:ehp9.
:dd.Enter the password you want to use for PC/2's Lockup feature into this entryfield.
You may enter up to 32 alphanumeric characters with case sensitivity. Once the
:hp9.Lockup Dialog:ehp9. is displayed enter exactly the
same character string to dismiss that dialog.
:warning text='Hint:'.
You may also use an empty string as the Lockup Password,
however its your decision if this is safe enough.
:dt.:hp9.Verification:ehp9.
:dd.You are required to enter the password entered in the :hp9.Lockup Password:ehp9.
entryfield twice to verify you didn't make any misspells.
:dt.:hp9.Enable Lockup Timer:ehp9.
:dd.Checking this item enables the :hp9.Automatic Lockup Timer:ehp9.. Once activated,
the Lockup Timer detects no user activity for a certain period and displays the
:hp9.Lockup Dialog:ehp9. after the period has expired automatically.
:dt.:hp9.Lockup after boot:ehp9.
:dd.Checking this item activates the Lockup Dialog immediately after OS/2 finished
the boot process. This option of course only makes sense if PC/2 is loaded during
the boot process, either because being the workplace process, being added to the
autostart folder, being a restartable WPS object or being started via
:hp9.STARTUP.CMD:ehp9..
:dt.:hp9.Automatic Lockup Timer:ehp9.
:dd.This spinbutton allows you to define the period the Lockup Dialog is activated
automatically when PC/2 does not detect any user input for this period.
You may select between 1 and 60 minutes.
You have to activate the checkbox :hp9.Enable Lockup Timer:ehp9. before you are able
to make any changes.
:warning text='Note:'.
PC/2 only detects activity for the Presentation Manager (PM) screen group,
switching to any fullscreen session is cause PC/2 to clear and suspend the Lockup
Timer causing it never to expire.
This is a security concern you should take into account when switching to fullscreen
sessions.
:ewarning.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Environment Space Settings     *
.* notebook page 1                                                                      *
.*--------------------------------------------------------------------------------------*
:h2 res=2200.Managing Environment Spaces
:p.This and the :link reftype=hd res=2250.<next page>:elink. allow you to
define :hp9.Environment Spaces:ehp9., which are useful to manage the environments
of your applications.
Some more information about an applications environment can be found at the
:link reftype=hd res=1550.<Environment Settings>:elink. notebook page,
including information about the predefined environment variables
:hp4.%PC2%:ehp4., :hp4.%BOOTDRIVE%:ehp4. or :hp4.%CDDRIVE%:ehp4..
:p.
A Environment Space is a collection of definitions of Environment Variables.
A Environment Variable is a pair of the form :hp9.EnvironmentVariable=Value:ehp9., which
is used to define a session's Environment.
You may have already noticed, that OS/2's CONFIG.SYS contains many definitions of
environment variables, for example the PATH, HELP or DELDIR variables.
:p.Most products modify your CONFIG.SYS during installation to add or modify environment
variables.
Unfortunately this may cause your system to malfunction, because the entries of some
products may conflict.
Or even worse, some programs are not able to modify lines in your CONFIG.SYS correctly,
once they exceed a line length of 255 characters.
Every developer knows that many problems arise when having installed for example IBM C Set++
and IBM VisualAge C++ on the same machine concurrently.
A very uncomfortable solution would be to have a certain CONFIG.SYS for each product and
reboot to take another CONFIG.SYS into effect.
:p.PC/2 offers a solution to this problem.
PC/2 lets you define a Environment Space for a certain product which contains all environment
variables necessary to successfully run this product.
For all conflicting products you may create Environment Spaces, for example a Environment
Space for IBM C Set++ one for IBM VisualAge C++ one for the IBM Developer's Toolkit 2.1
and one for the Warp Toolkit.
:p.For a program's Menuentry, you may then select one or more Environment Spaces on the
:link reftype=hd res=1550.<Environment Settings>:elink. page.
When launching the program later on, PC/2 merges its Environment with the Environment
Spaces and Environment defined for the program.
For example you may create a OS/2 window inheriting the Environment Spaces of
the both, the Warp Toolkit and VisualAge C++.
When launching this OS/2 window, you will have an Environment to compile programs
with VisualAge C++ under the Warp Toolkit.
Similar you may create a OS/2 window inheriting the 2.1 Toolkit and C Set++
Environment Spaces.
:p.With PC/2 you will be able to select and :hp2.switch between conflicting environments without
changing your CONFIG.SYS and without rebooting:ehp2. your workstation.
As you can inherit a Environment Space to more than one Menuentry, you can create
a standard Environment for all executables of a product without needing to define
the environment settings for each executable once.
Just modify a Environment Space and all Popup Menu entries using this Environment
will now using the updated Environment - its really that easy.
:p.:warning text='Note:'.
As OS/2 2.x does not allow you to change the LIBPATH at
runtime, I strongly suggest you to upgrade to OS/2 WARP.
:p.Some applications not only change the Environment but also add device drivers
into CONFIG.SYS.
PC/2 allows you to undo the changes in the Environment, but device drivers have
to be kept in CONFIG.SYS.
:ewarning.
:p.The following table describes how to create, remove or modify Environment Spaces.
You may enter the details of an Environment Space on the
:link reftype=hd res=2250.<next notebook page>:elink..
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Predefined Environment Spaces:ehp9.
:dd.This listbox contains the name of the currently available Environment Spaces.
To operate on a certain Environment Space just select it with your mouse, then
select one of the pushbuttons to perform the operation.
:p.If you want to view the contents of a certain Environment Space, simply doubleclick
on it, and the second Environments notebook page will automatically displayed.
Of course you can do this manually too by selecting a Environment Space and then click
on the right arrow pushbutton to turn to the next notebook page.
:p.Some operations on this notebook page (e.g. Export) can be applied to multiple
Environment Spaces simultaneously.
In this case you may want to select multiple Environment Spaces by holding down the
:hp4.CTRL:ehp4.-Key while selecting from the list with mouse clicks.
:dt.:hp9.Add:ehp9.
:dd.Select this pushbutton to add a new Environment Space.
:dt.:hp9.Change:ehp9.
:dd.Select this pushbutton to modify the selected Environment Space.
:dt.:hp9.Remove:ehp9.
:dd.Select this pushbutton to delete the selected Environment Space from the
list of all available Environment Spaces.
:dt.:hp9.As Batch:ehp9.
:dd.Select this pushbutton to save all selected Environment Spaces into a
batch file (*.CMD) so that you can create the Environment also by just running
that batch file under an OS/2 command processor (e.g. CMD.EXE).
The standard OS/2 file dialog is displayed, where you can enter the filename
to write the data as a batch file into.
:dt.:hp9.Export:ehp9.
:dd.Select this pushbutton to save all selected Environment Spaces into a file.
The standard OS/2 file dialog is displayed, where you can enter the filename to
write the data into.
:dt.:hp9.Import:ehp9.
:dd.Select this pushbutton to load Environment Spaces from a file.
The standard OS/2 file dialog is displayed, where you can select or enter the
filename to read the data from.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Environment Space Settings     *
.* notebook page 2                                                                      *
.*--------------------------------------------------------------------------------------*
:h2 res=2250.Defining an Environment Space
:p.This page allows you to edit the details of an Environment Space.
Click :link reftype=hd res=2200.<here>:elink. to switch to the first page.
A Environment Space is differentiated by its name and has one or more environment
variables attached:
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Name:ehp9.
:dd.This entryfield contains the name of the Environment Space you are editing.
When adding Environment Spaces for a program's Menuentry you may then select this
name from a listbox containing all available Environment Spaces.
:dt.:hp9.Variables:ehp9.
:dd.Use this multiline entryfield to define the environment variables contained in
this Environment Space.
An entry in the Environment specifies the value of a environment variable, and a entry
has therefore the following form:
:p.:hp4.ENVIRONMENTVARIABLE=[[VALUE][%ENVIRONMENTVARIABLE%][VALUE]]:ehp4.
:p.where :hp4.ENVIRONMENTVARIABLE:ehp4. is the name of a environment variable,
:hp4.VALUE:ehp4. is the value it gets assigned and :hp4.[...]:ehp4. specifies,
that this item may be specified 0...n times.
You may replace :hp4.VALUE:ehp4. by anything you want, but ensure that you use
the correct spelling.
:p.If you need to include the token % itself into a Environment, just replace
it with the token %%.
PC/2 will not treat the %% token as part of an environment variable's name, and
will keep a single % token in the Environment.
:edl.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Registration Settings notebook *
.* page                                                                                 *
.*--------------------------------------------------------------------------------------*
:h2 res=2300.Online Registration (Automatic Update capability)
:p.First, :hp2.thank you very much for your support of PC/2 by registering as a PC/2 user:ehp2.!
:p.Second, why do I want you to register as a user of PC/2?
:ul.
:li.As PC/2 is and ever will be Freeware (but of course donations are welcome ;-) and
even includes the source code (at least for the official versions, for betas I will make
it availale on requests), if you want to look at some hacks ;-) I have no real figures
how many users PC/2 has.
From mail I received and the number of hits on the PC/2 homepage I'm quite confident the
number is at least a few thousand, but due to many anonymous-ftp sites mirroring PC/2 also
I don't have real numbers.
:p.
I still do have ideas how to enhance PC/2 and still am interested in feedback from you, but
the more users PC/2 has the more motivation I get to spend time on it.
Thus, I really would appreciate your registration (ok, it's not really a "registration", because
neither prevents no regisitration you from using PC/2 nor will registration activate hidden
features as there are none).
:p.If you hopefully want PC/2 to register you as a user, please fill out some data about you in
the fields :hp9.First Name:ehp9., :hp9.Last Name:ehp9., :hp9.E-Mail:ehp9. and
check the option :hp9.Send User Registration:ehp9..
If you really don't want PC/2 registering you as a PC/2 user, just ensure
that :hp9.Send User Registration:ehp9. is not selected, however the entryfields
must be filled in or a message box will ask you to do so each time you load PC/2
(I ask you to use your real name and e-mail address, but remember that PC/2 can't
and won't check that).
:p.Please to not misuse this feature!
As the standard HTTP protocol is used it is easy to start attacks, as you can see from
the :hp9.Url:ehp9. specified, my pages are located at Geocities (a provider for free
homepages), so attacks will only hurt Geocities but under no circumstances IBM which
I work for!
:p.You may have to specify a :hp9.Userid:ehp9. and :hp9.Password:ehp9. to authenticate
with the Web-Server (unlikely that will change for Geocities) and/or your
:hp9.Proxy:ehp9.-Server (much more likely).
In both cases you would enter the same Userid and Password as you would when Netscape
asks you to authenticate while trying to load the specified :hp9.Url:ehp9. (you could copy
and paste it into Netscape to see what PC/2 retrieves), and the same Proxy settings
as you have defined in Netscape (just that the :hp9.Proxy:ehp9. Hostname or IP-Address
and the Port are written in URL-formatting, e.g. I would have to enter
:hp4.proxy.vienna.at.ibm.com&colon.8080:ehp4..
:p.If you keep :hp9.Send User Registration:ehp9. enabled, I will receive mail about your
registration having the following contents (most important to me is the version
data in the Subject line):
:xmp.
Please respond to No.Return.Email@geocities.com (Geocities WebMonitor mail)
To: Roman Stangl/Austria/IBM@IBMAT
cc:
Subject: FORM results

Sent from Mail Form posted at:

(Subject)  PC/2 V2.20 (08,2000) Online Automatic Update
(FirstName)  Roman
(LastName)  Stangl
(Email)  Roman_Stangl@at.ibm.com
(TCP/IP Address)  9.244.7.113
(Hostname)  stangl.vienna.at.ibm.com

Remote IP: 195.212.29.18
:exmp.
:li.Registration also has a bonus for you, which is that I know you and can keep you informed
about the status of PC/2 (without requireing you to regularily look into my web pages),
this feature is called the :hp2.Online Automatic Update:ehp2. capability.
:p.Registration allows me to inform you online about updates to PC/2, bug fixes,
possible problems and to download corrections and additions to the PC/2 package.
By default, PC/2 uses the information on this page to regularily contacts a webpage
maintained by me as the primary source for news about PC/2.
This webpage will be downloaded and analyzed and you will be informed by message boxes
if updates have been detected.
In short words, an :hp2.Automatic Update:ehp2. capability.
:p.
If you really don't want PC/2 trying to retrieve mentioned information online, just ensure
that :hp9.Online Program Updates:ehp9. is not selected.
:li.I also want to confirm if time has come and the technological base is available
to allow programs like PC/2 to use the Internet as a media to exchange information.
:p.
For example, StarOffice 5 allows free downloads but requires you to register to
prevent the 30-day timeout and offers to do so via Internet, doing so makes it
easy for users to access and test the software (IMHO a very efficient and elegant
distribution channel).
On the other hand, the rumours that the next version of MicroSoft products will cease
to function if you don't register them regularily is a dangerous development.
I do understand that this may reduce piracy (though not stop it, as with kernel debuggers,
ICE's (In Circuit Emulators) it's just a matter of time to patch out the registration
code), but the great danger is surveillance and increased cost due to time or usage based
fees - the Microsoft tax on every new PC is enough!
:p.
This is another reason that I believe in OSS (Open Source Software), Software should be
free (not necessarily in the sense it does not cost anything, but basically gratis with
support, which hopefully isn't required too often as Software should be designed in a
way to allow even novice users to understand what's going on, obtainable on a pay-per-call
basis).
:p.
By the way, you may ask why PC/2 and it's source are Freeware but not accompanied by the
GNU Public License.
Well, first as a non native English speaker I simply have problem to understand all its
consequences and second I do not allow PC/2 in parts or completely ported to any Windows
platform and IMHO this would violate the GNU License.
:p.
I'm quite sure that an Internet connection nowadays is pretty standard, and PC/2's
online Registration will allow me to prove that, to see if it makes sense that future
programs from me will use this technology from the beginning.
:p.
The online registration process will only work if you are connected to the Internet
via TCP/IP either directly or though a proxy, though you don't need a permanent connection (for
dial-up connections you can request PC/2 to update while connected by pressing the
:hp9.Update Now:ehp9. button).
:li.Finally, there is no secret going on with the Registration.
The information downloaded from my webpage is mirrored locally in a file named
:hp9.PC2.NET:ehp9., which you can browse (but please don't modify) with any editor.
:p.
And absolutely no data is collected from your PC to be sent to me, just what you
have entered in the Registration page and the TCP/IP address and hostname of the
workstation you are running PC/2 at (I added that because Geocities automatically
also adds the host it got connected from, but first only in dotted-decimal form
and not the real address when using proxies).
You can verify that easily by using the :hp4.IPTRACE:ehp4. and :hp4.IPFORMAT:ehp4.
(by the way programs you really should know) tools!.
:eul.
:p.
:p.Assuming that you are (hopefully ;-) interested in Registration, what does that imply in
detail?
:ol.
:li.PC/2 tries to detect if you have installed TCP/IP, the protocol used for the
Internet.
If it isn't installed properly, :hp9.PC2NET.DLL:ehp9. will not even be loaded (and
this DLL is where the Registration functionality resides in) saving memory.
:li.If TCP/IP is found it tries to connect at regular intervalls
to the author's Automatic Update Homepage specified in the :hp9.Url:ehp9. entryfield,
to download it and parse it for any news (you can use any browser if you are really
interested in its contents ;-).
:p.
As said above, the information retrieved online is stored in the file :hp9.PC2.NET:ehp9., you
can browse it (please don't modify it) or delete it to get back to status when PC/2 was
installed the first time (causing all news and updates to be downloaded again, which might
be useful if some files got currupted).
Once your User Registration has been sent, this is added to :hp9.PC2.NET:ehp9. to prevent
it being sent multiple times.
:li.For connection, PC/2 first tries to directly connect to the :hp9.Url:ehp9. specified,
which will only work if you are directly connected to the Internet.
If specified, PC/2 tries to connect to the :hp9.Url:ehp9. by using the :hp9.Proxy Address:ehp9.
specified, which should work from PCs connected to the internet via a firewall.
:li.The information is gathered then via :hp4.HTTP:ehp4. requests, similar to what WWW-Browsers
do (you can use :hp4.IPTRACE:ehp4. and :hp4.IPFORMAT:ehp4. to verify), for the document
specified at :hp9.Url:ehp9..
:p.
The downloaded document is then parsed for information regarding PC/2 about updates, bug fixes,
possible problems or other information that may be of interrest for you.
Depending if you have not yet seen that information, that information will be presented
to you.
:li.Having selected :hp9.Send User Information:ehp9.,
PC/2 registers you online as an PC/2 user again via an :hp4.HTTP:ehp4. request.
Again, you don't need to register and it doesn't cause you any troubles (e.g. spamming
from me as I don't distribute :hp2.any:ehp2. I get), but I would
really appreciate it knowing you as a PC/2 user!
:li.When looking into :hp9.PC2.NET:ehp9., in the tag :hp4.Status:ehp4. you will find
the date when PC/2 last successfully could connect to the server specified in the
:hp9.Url:ehp9..
Even after successful connects, PC/2 retries to connect again every few days to check
for news, after unsuccessful PC/2 retries every hour.
:p.
The tag :hp4.Registration:ehp4. gives you the date PC/2 could successfully transfer
the :hp9.Send User Registration:ehp9. information, of course only if you have enabled
it by checking :hp9.Send User Registration:ehp9..
:eol.
:p.The following fields are required to allow PC/2 automatically check for updates.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Online Program Update:ehp9.
:dd.Having selected that option allows PC/2 to automatically try to gather information about
PC/2 updates, bug fixes and problems or other information that may be of interrest for you
from the document specified at :hp9.Url:ehp9..
:dt.:hp9.Send User Information:ehp9.
:dd.Having selected this option, your user information will be sent to my homepage, and
will then allow me to register you as a PC/2 user.
Again, if you don't want PC/2 to send that information, keep that option unselected.
Please note that the fields here have to be filled in, even if :hp9.Send User Registration:ehp9.
is not selected, or a message box will remind you every time you lanch PC/2.
:dt.:hp9.First Name:ehp9.
:dd.Please enter your first name here.
:dt.:hp9.Last Name:ehp9.
:dd.Please enter your last name here.
:dt.:hp9.E-Mail:ehp9.
:dd.Please enter the Internet E-Mail account you want information about PC/2 to be sent to (I will
not distribute any user data and will solely use it, to e-mail you only when e.g. some important
news/warnings about severe problems/... are available.
:dt.:hp9.Url:ehp9.
:dd.This read-only entryfield specifies the Url of the document that will be loaded and
presented to you if news are recognized.
You can not modify the contents of that field, but you may copy that Url into your browser
to see the document if you don't want PC/2 trying to access it automatically.
:dt.:hp9.Proxy:ehp9.
:dd.If you are connected to the Internet only via a firewall, you need to fill in the
:hp9.Proxyaddress:ehp9. field with the address and port of your Proxy.
For example, I have to enter :hp4.proxy.vienna.at.ibm.com&colon.8080:ehp4. here to
use the Proxy :hp4.proxy.vienna.at.ibm.com:ehp4. at the Proxyport :hp4.8080:ehp4. (by the
way ports :hp4.80:ehp4. and :hp4.8080:ehp4. are widely used Proxy ports.
If you are not sure what to enter here, just open your browser (e.g. Netscape) and copy the
Proxy settings from there.
:warning text="Note!".
If you don't have a Proxy in your network or you didn't adjust the sample configuration
shipped with PC/2, you very likely will experience TCP/IP 10022 errors (which means that
the DNS (Domain Name Server) Lookup can't find the Proxy.
Thus if you don't have a Proxy empty out the :hp9.Proxy:ehp9. entryfield, if you have one
ensure that you have adjusted the sample data correctly.
:ewarning.
:dt.:hp9.Userid:ehp9.
:dd.In case the Web-Server serving the :hp9.Url:ehp9. (unlikely) or the :hp9.Proxy:ehp9.
(much more likely) you are using requires authorization, you have to enter your
:hp9.Userid:ehp9. here.
In both cases you would enter the same :hp9.Userid:ehp9. and :hp9.Password:ehp9. as you would when
Netscape asks you to authenticate while trying to load the specified :hp9.Url:ehp9.,
so using Netscape to display the :hp9.Url:ehp9. is a easy way to verify what you have
to enter exactly.
:dt.:hp9.Password:ehp9.
:dd.In case the Web-Server serving the :hp9.Url:ehp9. (unlikely) or the :hp9.Proxy:ehp9.
you are using requires authorization, you have to enter your :hp9.Password:ehp9. here.
:dt.:hp9.Update Now:ehp9.
:dd.Pressing this button requests PC/2 to try to retrieve news from :hp9.URL:ehp9. page.
Of course, this will only work if TCP/IP is installed and a Internet connection is
active.
If PC/2 could not detect TCP/IP support installed, this button will by not available.
:dt.:hp9.Show Status Window:ehp9.
:dd.Select this checkbox to allow PC/2 to display a small Status Window whenever PC/2
tries to establish a Internet connection.
:dt.Status text
:dd.If PC/2 will display the status of what is going on in the online Registration process.
:edl.
:p.Both the :hp9.Proxy Address:ehp9. and :hp9.Url:ehp9. can be entered in the form URL
(Uniform Resource Locators) are usually entered, e.g. it does not make a difference
if you write :hp4.http&colon.//proxy.vienna.at.ibm.com&colon.8080/:ehp4. or
:hp4.proxy.vienna.at.ibm.com&colon.8080:ehp4. or just :hp4.proxy.vienna.at.ibm.com:ehp4.
(which causes the default Proxy port of :hp4.80:ehp4. to be used).
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.


.*--------------------------------------------------------------------------------------*
.* The user selected help from the Desktop Configuration Spy Window Settings notebook   *
.* page                                                                                 *
.*--------------------------------------------------------------------------------------*
:h2 res=2350.Spying a window's details
:p.This notebook page allows you to get information about the programs and windows of
the applications you are running, which would not be trivial to get otherwise.
:p.For example, in the :link reftype=hd res=1450.<Hotkey Settings>:elink. dialog
of the Program Installation, you are required to enter an application window's titlebar
name, or its name in the Window List, or its class name in order to use the
:hp9.Sticky Windows:ehp9. and :hp9.Sticky Window (Keep on active Desktop):ehp9. feature.
:p.You may also need the information displayed in this window to find out the exectable
a window was created from, to fill in the :hp9.Exception Lists:ehp9. when using the support
for :link reftype=hd res=1850.<Hardware Panning>:elink. and for
:link reftype=hd res=1400.<Titlebar Smarticons>:elink..
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Enable spying:ehp9.
:dd.Use this checkbox to activate or deactivate spying windows.
This setting will not be save when you close this dialog, as it does not make sense
to do all the processing required to fill this window when it is not visible.
:dt.:hp9.Title:ehp9.
:dd.This field displays the window's title, which is read from the titlebar.
:dt.:hp9.Executable:ehp9.
:dd.This field displays the executable which created the window.
:dt.:hp9.Window List:ehp9.
:dd.This field displays the the window's entry in the Window List.
:dt.:hp9.Class:ehp9.
:dd.This field displays the window's class name.
:dt.:hp9.Process:ehp9.
:dd.This field displays the process identifier.
:dt.:hp9.Thread:ehp9.
:dd.This field displays the thread identifier.
:dt.:hp9.Position:ehp9.
:dd.This field displays the windows horizontal and vertical position.
:dt.:hp9.Size:ehp9.
:dd.This field displays the windows horizontal and vertical size.
:dt.:hp9.Frame Handle:ehp9.
:dd.This field displays frame window's window handle.
:edl.
:warning text="Note!".
When you have enabled spying, you can suspend spying by pressing the :hp4.ALT:ehp4. key
over the window whose data you want to have frozen in this notebook page.
This may be useful when you want to copy and paste the information gathered for a certain frame
window to e.g. one of PC/2's Exception Lists.
:ewarning.
:p.Press :hp4.Save:ehp4. to accept and save changes into the profiles, or press
:hp4.Cancel:ehp4. to accept them without saving them to disk.
:p.:link reftype=hd res=302.<Return to dialog help>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for PC/2's Overview Window.                                   *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4013.Help for PC/2 Overview Window
:p.PC/2's Desktop Overview Window displays a sized image of all windows available on
all Virtual Desktops and allows direct manipulation.
Of course PC/2's Overview Window must be enabled (which is the default after installing
it for the first time) before it is displayed.
:p.To change the font or colors of the Overview Window just drag a font from the Font
Palette or color from the Color Palette onto the Overview Window.
To change the rectangles' (representing the windows) color just drag a color, to
change the text color drag a color while holding down the :hp4.CTRL:ehp4.-Key,
and to change the background of the Overview window itself drag a color while
holding down the :hp4.SHIFT:ehp4.-Key.
Alternatively you may have to use the
:link reftype=hd res=1900.<Overview Window Settings>:elink. to customize
the colors, as depending on the :hp9.Window Representation:ehp9. used, some
colors can't be changed by dropping colors onto the Overview Window (because
there are more colors involved than there are key and mouse events available
to specify which color to change).
:p.To change the font used to draw the window's name into the rectangle, just drag
a font from the font palette onto the Overview window.
Changes of the font and color will be saved the next time you exit PC/2, however
the Overview window must be in :link reftype=hd res=1800.Uncolored:elink.
mode for color changes to be come effective.
:p.At first an overview about key features of PC/2.
:ul.
:li.A :hp9.Virtual Desktop:ehp9. is a Desktop enhancement that expands the size of
your display and controls which part of the virtual display is covered by your
physical screen.
:artwork name='Source\Resource\Pc2Win.bmp' align=left.
For example assume that you are running OS/2 on an XGA-2 at a resolution of 1024 * 768
pixels.
Your display has then 1024 columns and 768 rows of pixels, giving you a
Desktop where application windows can be seen.
We will call this the :hp9.Physical Desktop:ehp9..
Of course you can start windows outside this Desktop, on one of the :hp9.Virtual
Desktop:ehp9.s, but you won't be able to see them unless you switch to this Desktop.
:p.PC/2 also supports to switch between Virtual Desktops using they keyboard.
Just press and hold the :hp4.CTRL+ALT+SHIFT:ehp4. keys and then press any cursor key
to switch to the Virtual Desktop next in the direction the cursor key points to
(of course the overall Virtual Desktop size limits still apply).
Both, the cursor keys from the cursor block and the cursor keys of the numeric
keypad are supported (even diagonal, that is e.g. right and upwards, movements
are supported by using the keys between the cursor keys, e.g. the page up
key will move right and upwards).
:p.To summarize the graphics, assume you are using an XGA-2 display adapter at a
resolution of 1024 * 768 points:
:ol.
:li.The Physical Desktop, that is the screen you are currently looking
into is located at the center of all :hp9.Virtual Desktop:ehp9.s, after invoking PC/2.
The lower left point (:hp4.1:ehp4. in our graphics) is then the origin in the
:hp9.Virtual Desktop:ehp9. with coordinates of (0, 0).
All coordinates on the :hp9.Virtual Desktop:ehp9. are then counted relative to this
absolute origin, counting positive on the right and upper direction and negative to
the lower and bottom direction.
:li.The :hp9.Virtual Desktop:ehp9. to the left to your Display after starting PC/2
has negative horizontal coordinates.
In our example, this point (:hp4.2:ehp4.) would be (-1024, 0).
:li.The :hp9.Virtual Desktop:ehp9. right to your Display after starting PC/2
has the absolute coordinates of (1024, 0) (point :hp4.3:ehp4.) in our example.
:p.On a VGA screen at a resolution of 640 * 480 the coordinates would be (640, 0), as
you have hopefully assumed.
:li.When you are using OS/2, you have usually a Physical Desktop in front
of you, where the WPS (WorkPlace Shell, usually called Desktop, point :hp4.4:ehp4.
in our graphics) is running.
After invoking PC/2, the WPS is drawn on the :hp9.Overview Window:ehp9.
with the name :hp4.Desktop:ehp4..
Behind the WPS, which is a Presentation Manager application just covering the
whole screen, is the image of the Physical Desktop, named :hp4.Display:ehp4.,
but it is covered by the WPS, so you can't see it.
:p.Now assume you move the Physical Desktop onto the :hp9.Virtual Desktop:ehp9.
where the IBM Internet Connection Server, (named IBM Internet Con in our graphics), runs,
that is moving the lower left edge of your display from :hp4.1:ehp4. to :hp4.2:ehp4.
in our graphics.
The Physical Desktop will then be at :hp4.2:ehp4. in our graphics, and you
would be able to see its name :hp9.Display:ehp9. then, because it is not longer covered
by the WPS.
:eol.
:p.Simply speaking, imagine the :hp9.Virtual Desktop:ehp9. as an up to 9 * 9 array of Desktops,
with the Physical Desktop, that is the display, just covering 1 of all
:hp9.Virtual Desktop:ehp9.s.
You just navigate your display around within the :hp9.Virtual Desktop:ehp9.s as you
would do on a map from sector to sector.
Distributing applications on the :hp9.Virtual Desktop:ehp9.s simply helps you to
avoid having all application's windows on your display.
:p.Now assume that you move the Physical Desktop left, all windows you have
started will move right, so some shift out on the right side and some shift in
on the left side of your display.
:p.Controlling the windows is exactly that what PC/2 does for you.
If you move your mouse pointer on the leftmost column of your display
(column 0), all windows move rightward, as the Physical Desktop moves
leftward within the :hp9.Virtual Desktop:ehp9..
The same principle is used on the rightmost columns and the top and
bottom rows.
If you move your mouse pointer into a corner of your display, the Physical Desktop
even will move in two directions, leading to a horizontal and a
vertical movement.
:p.Of course you can only move your Physical Desktop in one direction,
as long as this movement keeps it within the :hp9.Virtual Desktop:ehp9., whose
size you can select from 1 * 1 up to 9 * 9.
:li.The Overview Window is controlled by smarticons, located at the top of the window.
The currently active smarticon is framed and pressed.
If mouse button 2 is pressed on a window, the action corresponding to the active
smarticon is performed, except for the window rectangle corresponding to the Desktop
(WPS) where you also need to press and hold the :hp4.ALT:ehp4.-Key while performing
the selected action.
:p.To perform actions on the windows displayed in PC/2's Desktop Overview Window, the
following smarticons may be used:
:dl tsize=12 break=fit.
:dthd.:hp4.Icon&colon.
:ddhd.Description&colon.:ehp4.
:dt.:artwork name='Source\Resource\Exit.bga' runin align=left.:hp9.Exit PC/2:ehp9.
:dd.Click on this smarticon to exit PC/2.
:p.When running PC/2 as the WPS process, this smarticon will be disabled,
because the WPS (or WPS replacement) process should not terminate.
:dt.:artwork name='Source\Resource\Move.bga' runin align=left.:hp9.Move window:ehp9.
:dd.Selecting this icon changes the pointer to a window pointer cursor, allowing you
to select and move a window by clicking and holding mouse button 2.
:dt.:artwork name='Source\Resource\Hide.bga' runin align=left.:hp9.Hide window:ehp9.
:dd.Selecting this icon changes the pointer to a window pointer cursor, allowing you
to select and hide a window by clicking mouse button 2 on it.
The window can be made visible again by selecting it from the Window List.
:dt.:artwork name='Source\Resource\Top.bga' runin align=left.:hp9.Set to top:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
move a window on the top of the Desktop by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\Bottom.bga' runin align=left.:hp9.Set to bottom:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
move a window to the bottom of the Desktop by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\Close.bga' runin align=left.:hp9.Kill a session:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
terminate a window by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\Maximize.bga' runin align=left.:hp9.Maximize window:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
maximize a window by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\Minimize.bga' runin align=left.:hp9.Minimize window:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
minimize a window by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\Restore.bga' runin align=left.:hp9.Restore window:ehp9.
:dd.Selecting this icon changes the pointer to a cross wires cursor, allowing you to
restore a window by clicking mouse button 2 on it.
:dt.:artwork name='Source\Resource\ShutDown.bga' runin align=left.:hp9.ShutDown OS/2:ehp9.
:dd.Selecting this icon changes the pointer to a window pointer cursor and displays
an optional message box asking to continue ShutDown of OS/2.
PC/2 then requests OS/2 to close all file buffers and suspends all
disk accesses, depending on your selections in the
:link reftype=hd res=2050.<General Settings>:elink. notebook page.
A ShutDown of OS/2 may take a few seconds to complete.
:dt.:artwork name='Source\Resource\PowerDown.bga' runin align=left.:hp9.PowerDown OS/2:ehp9.
:dd.Selecting this icon changes the pointer to a window pointer cursor and displays
an optional message box asking to continue PowerDown of OS/2.
OS/2 may then either be suspended or shut down and
your PC will be powered off, depending on your selections in the
:link reftype=hd res=2050.<General Settings>:elink. notebook page via APM
requests.
A PowerDown of OS/2 may take a few seconds to complete.
:dt.:artwork name='Source\Resource\Lockup.bga' runin align=left.:hp9.Lockup OS/2:ehp9.
:dd.Selecting this icon immediately activates the Lockup feature of PC/2.
This may help you to prevent penetration of your system while leaving your
system.
Select the :link reftype=hd res=2150.<Lockup PC/2 Settings>:elink. dialog
to modify the Lockup feature settings of PC/2.
:dt.:artwork name='Source\Resource\Help.bga' runin align=left.:hp9.PC/2 Help:ehp9.
:dd.Selecting this icon changes the pointer to a window pointer cursor and displays
PC/2's online help you are reading now.
:edl.
:p.If you press the :hp4.CTRL:ehp4.-key while clicking with mouse button 1 onto the
Overview window, PC/2 displays a popup menu, which allows you to
configure which SmartIcons should be included in the Overview window's menubar
and to perform a PC/2 dump:
:dl tsize=12 break=fit.
:dthd.:hp4.Submenu&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.SmartIcons:ehp9.
:dd.Use this submenu to configure the Overview window's SmartIcons.
:ul.
:li.Exit
:li.Move
:li.Hide
:li.Z-Order Top
:li.Z-Order Bottom
:li.Close
:li.Maximize
:li.Minimize
:li.Restore
:li.ShutDown
:li.PowerDown
:li.Lockup
:li.Help
:eul.
:dt.:hp9.Debug:ehp9.
:dd.Use this submenu to request PC/2's debugging support.
You may only want to select these options when the author of PC/2 has requested
you to do so.
Though you can't do any harm to your PC when selecting these options, the data
generated by that options may not be useful for you.
:ul.
:li.Dump PC/2 process
:p.When selecting this option, PC/2 dumps its address space to disk, that is
into the next available :hp9.PDUMP.nnn:ehp9. file in the root directory of your
boot drive.
This option performs the same dump as described in
:link reftype=hd res=4015.<PC/2 hang/trap/bug diagnostics>:elink.
with the only difference that PC/2 will be still running after the dump
(though your PC will be unresponsive to user input during the about 10 secunds
required to write that dump).
:li.Dump W-Thread
:p.Use this option to dump the thread status of PC/2's Working thread to
PC/2's dump file :hp9.PC2.TRP:ehp9..
This thread is responsible for all heavy duty work, most visibly recalculating
and redrawing the Overview window.
:li.Dump I-Thread
:p.Use this option to dump the thread status of PC/2's I/O thread to PC/2's
dump file :hp9.PC2.TRP:ehp9..
This lower priority thread is responsible for reading and writing PC/2's Popup
Menu to file.
:eul.
:edl.
:li.PC/2 supports :hp9.Sliding Focus:ehp9. and :hp9.Virtual
Desktops:ehp9..
The :hp9.Sliding Focus:ehp9. gives you a behavior you may know from X-Windows -
you don't have to click on a window to activate it and to switch the input focus
to it - the window below the mouse pointer is automatically activated.
Just move the mouse pointer from one window to another to switch the input focus.
You'll notice that the window below the mouse pointer always has a highlighted
frame, which OS/2 uses to signal the active window as having the input focus.
You may use the :hp4.SHIFT:ehp4.-key to temporarily suspend the :hp9.Sliding Focus:ehp9.,
which you may find useful for some programs, e.g. online help displayed with
:hp4.VIEW.EXE:ehp4., because OS/2 doesn't expect mouse movements to switch
the active window.
:eul.
:p.All settings are saved and will be used when PC/2 is loaded unless you have
specified a different behavior with the
:link reftype=hd res=3100.<Commandline Parameters>:elink..
:warning text='Caution:'.
You may want to disable the :hp9.Sliding Focus:ehp9.
on a machine used for development. When using a debugger (e.g. ICSDEBUG) your
machine is likely to lock up with the :hp9.Sliding Focus:ehp9. enabled.
:p.You can use the :hp9.Virtual Desktop:ehp9. on a development machine, just
avoid switching between Desktops when a debugger is active.
:p.Trap popup screens also make changes in the focus order so I would suggest
you use the keyboard to dismiss these dialogs.
:ewarning.
:p.:link reftype=hd res=302.<Backward>:elink.
                                :link reftype=hd res=258.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for the Spooler window.                                       *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=258.PC/2's Spooler Control Window
:p.PC/2 supports a Spooler Control Window, which is simply a container filled
with the available printer queue and job information available on the local
machine PC/2 is running.
:artwork name='Source\Resource\SpoolerControlWindow.bmp' runin align=left.
:p.To change the font of the Spooler Control Window, just drag a font
from the Font Palette to the Spooler Control Window container. This
font will be saved the next time you exit PC/2.
The PC/2 Spooler was primarily added to be able to manage print jobs on systems
PC/2 is running as a WPS replacement. Any WPS replacement has to start the
OS/2 spooler and provide a mechanism to manage print jobs.
:p.To be able to display PC/2's Spooler Control Window, you must select the
Menuitem :hp9.PC/2 Spooler:ehp9. from PC/2's Popup Menu (of course you have to
add this Menuitem via the
:link reftype=hd res=2900.<Control Addition>:elink. dialog.
:p.If the container is empty, PC/2 can't find any printer queue on your local
workstation.
:dl tsize=12 break=fit.
:dthd.:hp4.Menuentry&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Refresh:ehp9.
:dd.Select this item to refresh the Spooler window container immediately from
the real-time printer queue status of the local workstation. When you don't select
this item, the Spooler window container gets refreshed regularly.
:dt.:hp9.Hold job:ehp9.
:dd.Select this item to set the currently selected job(s) to hold status.
:dt.:hp9.Release job:ehp9.
:dd.Select this item to set the currently selected job(s) to normal status
(this request will be ignored for jobs being in the normal status).
:dt.:hp9.Delete job:ehp9.
:dd.Select this item to delete the currently selected job(s). The jobs will be
removed from the printer queue by the OS/2 spooler in a few seconds.
:dt.:hp9.Hold queue:ehp9.
:dd.Select this item to suspend the currently selected queue(s). All jobs running
in these queues will suspended from printing, too.
:dt.:hp9.Release queue:ehp9.
:dd.Select this item to set the currently selected queue(s) to the normal status
(this request will be ignored for queues being in the normal status).
:dt.:hp9.Help:ehp9.
:dd.Selecting this item just displays the online help panel you are currently
reading.
:edl.
:p.:link reftype=hd res=4013.<Backward>:elink.
                                :link reftype=hd res=262.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for the QuickSwitch window.                                   *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************
:h1 res=262.PC/2's QuickSwitch Window
:p.When :link reftype=hd res=1950.<enabled>:elink.,
PC/2 supports the QuickSwitch window, which allows you to switch to the next/previous
session(s), which of course switches to the Virtual Desktop the session is running in.
:artwork name='Source\Resource\QuickSwitch.bmp' align=left.
The QuickSwitch window displays the icon and the Window List entry of the session
you may select to switch to in a window centered on your screen,
You may know such a feature from WIN-OS2 fullscreen.
:p.To select the next session, press and hold the :hp4.CTRL:ehp4.-Key while using the
:hp4.TAB:ehp4. key to select the session you want to switch to.
To select the previous session, press and hold the :hp4.SHIFT:ehp4. key while holding
the :hp4.CTRL:ehp4.-Key while using the :hp4.TAB:ehp4. key the select the session you want
to switch to.
All sessions running on your system are in a kind of circular buffer, there is no
explicit beginning and no explicit end.
In contrast to PM's builtin :hp4.ALT+TAB:ehp4. keys, PC/2 will also switch between Virtual
Desktops if required.
:p.Once you have selected the session you want to switch to, just release the CTRL key
to switch to this session.
PC/2 will switch to the Virtual Desktop this session is running in and activates the
session's window.
:p.:link reftype=hd res=258.<Backward>:elink.
                                :link reftype=hd res=263.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected help for the SessionBar window.                                    *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************
:h1 res=263.PC/2's SessionBar Window
:p.PC/2 supports the :link reftype=hd res=1850.<SessionBar>:elink. window, which
gives you an enhanced control of your windows running on the Virtual Desktops.
:artwork name='Source\Resource\SessionBar.bmp' align=left.
:p.
The SessionBar consists of (above screencapture shows the default colors and edited
to show multiple Popup Menus):
:ul compact.
:li.The Warp PC/2 pushbutton, which is an alternative way to invoke PC/2's Popup Menu.
:p.
Clicking with mouse button 1 activates PC/2's Popup Menu.
:p.
:li.The IconBar, which displays the titlebar icons of all sessions registered in
the Window List (when available, otherwise a replacement image is drawn).
The active window of the currently active session (which may consist of more than one window),
is drawn with an inversed border.
:p.
Clicking with mouse button 2 activates a Popup Menu to customize the display.
:p.
:li.The scrollbuttons, which allow you to scroll the IconBar.
:p.
:li.The (optional) APM battery status area for computers that can run from line or battery power.
:p.
:li.The (optional) Processor (CPU) Performance status area for OS/2 versions that support the
performance counters, of course supporting SMP systems.
:p.
Clicking with mouse button 2 activates a Popup Menu to customize the display.
:p.
:li.The (optional) Status area, which either displays the current date/time, the APM power status
(while the mouse pointer is positioned over the APM battery status area) or the name of
the session below the mouse pointer (while the mouse pointer is positioned over a
session's icon of the IconBar).
If the Status area is requested to be hidden, a small empty frame will be drawn anyway to allow better
alignment of other parts of the SessionBar.
:p.
The format of the current date/time displayed depends on the settings of the date and
time format defined in the Country WPS Object (unless you have selected the :hp9.Default
Date/Time:ehp9. format, in which case PC/2's format will be used).
:p.
By default, the date/time is display right justified, but you can set your preferences from
the Popup Menu you can display by clicking with mouse button 2.
:eul.
:p.You can drag the SessionBar with mouse button 2 to your most favourable position
(which will be remembered for subsequent invocations of PC/2), by clicking anywhere
except on the windows icons and dragging it to a new position.
PC/2 ensures that the SessionBar stays automatically on top of other windows (top
of Z-Order) when requested with the SessionBar's settings on the
:link reftype=hd res=1850.<Virtual Desktop Settings Page 2>:elink.
notebook page.
:p.You can customize the colors used to draw the SessionBar in general and of the
CPU Performance Status area in detail by dragging a color from
the WPS Color palette onto the Overview window.
:p.
In general for the SessionBar, to change the background color just drag a color, to
change the (foreground) text color drag a color while holding down the :hp4.CTRL:ehp4.-Key.
To change the font of the status area, drag a font from the Font Palette onto
the SessionBar.
Of course, all those changes are persistent, that is they are saved into :hp4.PC2.INI:ehp4..
:p.
For the CPU Performance Status area in detail, drag a color from the WPS Color palette
onto the CPU Performance Status area:
:ol compact.
:li.while holding down mouse button 2, to change the color representing the Interrupt time
(defaults to red)
:li.while holding down the :hp4.CTRL:ehp4.-Key together with mouse button 2, to change the
color representing the Busy time (defaults to green)
:li.while holding down the :hp4.SHIFT+CTRL:ehp4.-Key together with mouse button 2, to change
the grid color (defaults to blue)
:li.while holding down the :hp4.SHIFT:ehp4.-Key together with mouse button 2, to change
the color of the background area (defaults to dialog background color)
:eol.
:p.When having selected the SessionBar to be kept on top of Z-Order, PC/2
tries to ensure that no other window hides the SessionBar, by adjusting the
Z-Order of these windows.
Additionally, if the SessionBar is moved to the top or bottom of the screen
PC/2 tries to ensure that the SessionBar and other windows don't overlap, by
adjusting the size and/or position of these windows (e.g. maximized windows reduced to
cover all of the display but the area used by the SessionBar, instead of the whole
screen as one would usually expect, except for seamless WIN-OS2 sessions).
:p.When moving the mouse pointer over a session's icon at the IconBar, the Status
area displays the name of the session, that is the session's Window List entry
:p.The APM battery status area displays the current battery capacity compared to
being loaded fully 100 percent.
Battery capacity is displayed as a green bar graph, except when the battery capacity
is only 10 percent or left, which caues the graph to be displayed in red.
When moving the mouse pointer over the APM battery status area, the Status area
displays the exact battery capacity in percent including the soure your PC is powered
from, that is either line or battery power.
:warning text='Note:'.
To prevent possible display corruption, seamless WIN-OS2 sessions are excluded
partly from this logic, and may overlap the SessionBar.
:ewarning.
:p.The CPU Performance status area displays per processor a graph of
the recent history of the CPU utilization divided by the CPU Idle, Busy and Interrupt
time.
However, as support for reporting utilization data by OS/2 is required this graphics
will not be available on all OS/2 versions, however at least Warp 4 or Warp Server
Advanced SMP will work.
Actually the Interrupt and Busy time are drawn upwards from the bottom of the graphics,
what is left is the Idle time.
In other words, from the bottom first the Interrupt time is drawn (in red) and from
that point the Busy time is drawn (in green), from that point to the top of the graphics
the Idle time is represented.
Usually, the Interrupt time is very low, so it may not be visible at all, and if
additionally the CPU if flat out, you may end up seeing just 100 percent Busy time.
:p.
Depending on your selection of the :hp9.Fill Mode:ehp9. at the
:link reftype=hd res=1950.<SessionBar Settings>:elink. page, the current
Interrupt and Busy time may be represented by either filled vertical lines or by just
drawing a single pixel.
For the former method, the CPU utilization is drawn first and then a grid, for the
later method the grid is drawn first and then the CPU utilization.
The grid consists of horizontal lines representing the utilization ranging from 0 to
100 percent (in either 25 or 50 percent steps depending on the height of the graphics)
and vertical lines representing milestones (e.g. every 10 seconds when having selected
that the graphics represents 1 minute).
The vertical lines are drawn for "even" milestones, that is having selected e.g. a
reporting intervall of 1 minute, the milestones are seconds 0, 10, 20,... even if that
reporting intervall is selected at an "uneven" seconds like e.g. 23.
This allows you to realize the milestones in the graphics even for historic data.
When changing between the reporting intervalls, it may take up to 1 minute before the
graphics is redisplayed.
This is directly related to the granularity of the timer, e.g. having selected 1 hour,
the graphics is updated every minute, and as "even" milestones are awaited this will
cause that the beginning of the next minute is awaited (and if that intervall is
selected as second 1, 59 seconds will be awaited).
Not doing that this way would prevent that the vertical lines represent "even"
milestones.
:p.
To change the reporting intervall, just activate the CPU Performance graphics popup menu,
by clicking with mouse button 2 anywhere at the area of the graphics:
:dl tsize=12 break=fit.
:dthd.:hp4.Menuitem&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Settings:ehp9.
:dd.The :hp9.Settings:ehp9. submenu allows you to select the reporting intervall that
represents the CPU Performance graphics area.
You may select between 1 minute, 5 minutes, 30 minutes and 1 hour, the vertical lines
representing milestones are drawn every 10 seconds, 1 minute, 5 minutes and 10 minutes
respectively (remember, that they are drawn on "even" milestones).
:dt.:hp9.CPU 1 online:ehp9.
:dd.For CPU 1, which is either the single CPU in a UP (UniProcessor) environment or
the first CPU in a MP (MultiProcessor) environment, you can't make adjustments.
In other words, CPU 1 can't be set offline and will always be running, thus the selection
is even disabled.
Note, in an MP environment this means for the current PC architecture this CPU can't
be replaced (in the sense of hot-swapping like for disk drives) even if some additional
hardware would allow one to do a physical power off of the CPU socket after setting
the CPU offline.
:p.The menuitem corresponding to a certain CPU will have a check mark beside if that CPU
is online, if the CPU is offline the check mark will be removed.
:dt.:hp9.CPU n online:ehp9.
:dd.For all additional CPUs in an MP environment, one line is added.
This allows you to toggle these CPUs between online and offline (in which case the
CPU Performance graphics displays :hp4.Offline:ehp4. instead of the utilization data),
and with additional hardware it would even be possible to replace the CPU then
(though I don't know any PC hardware supporting this yet, and CPUs usually don't go
bad).
The actual number of CPUs displayed here depends on how many are detected by OS/2 SMP
during boot, as a poor freeware author I only have 2 CPUs on my IBM Intellistation Z ;-).
:edl.
Under a OS/2 SMP version, you can set any CPU but CPU 1 to either :hp9.Online:ehp9. (which
means that OS/2 uses that CPU as a resource where threads can be dispatched to) or to
:hp9.Offline:ehp9. (which means that OS/2 no longer does dispatch threads to that CPU).
The more CPUs are online the more processing power you have available.
:warning text='Note:'.
You may notice that the Interrupt time (drawn in red) isn't visible at all (though
you may "force" it to appear by starting at least 2 OS/2 windows and run :hp4.CHKDSK /F&colon.2:ehp4.
against partitions on several disks).
This is due to the facts that the Interrupt time is very low in general (and as user
applications don't get dispatched then this is good news ;-), it may be overlayed by
the grid (especially when having selected :hp9.Fill Mode:ehp9.) and that CPUs other
than the first one don't seem to do interrupt processing at all (at least under the
current OS/2 versions as a consequence as how the APIC 1.x specification is implemented
in both software and hardware, though as the term SMP (Symmetric MultiProcessing) is
used it to characterize such PCs, it isn't completely symmetrical at least in my
definition of SMP then).
:ewarning.
:p.
As PC/2 uses the hardware based performance counters built-in into OS/2 (Warp 4+ and
Warp Server Advanced SMP+, for OS/2 versions lacking that support that area is just
not available), the graphics reflects the reality in contrast to e.g.
:hp9.PULSE:ehp9. which only shows an approximation.
:warning text='Caution!'.
Under OS/2 2.11 and Warp 3 but not under Warp 4 or when using the latest driver, the Job
Properties dialog of the PostScript WPS object selects PC/2's Overview or SessionBar
window as the owner (you will notice this, that the Job Properties dialog is positioned
at the lower left edge of PC/2's window, and PC/2's window gets disabled, causing
beeps when clicking on it.
In technical terms, PC/2's window becomes the owner of the Job Property window,
even if it is running in a different process context (e.g. WPS).
If you would close PC/2 during this situation you can be sure that you will hang
PM and OS/2 and have to reboot soon.)
:p.This defect has been reproduced by the OS/2 change team in Austin as APAR PMR 11867,
and the suggested fix is to install the latest OS/2 PostScript printer driver
(the Warp 4 driver doesn't have this defect, you can even install this one on
earlier releases of OS/2).
Unfortunately the defective PostScript printer driver was shipped as a sample how to
write printer drivers, therefore other printer drivers may experience that defect too.
You may also looking for the latest version of that printer driver too.
:ewarning.
:p.You can imagine that adjusting other windows is a difficult task, so some windows
may ignore such requests.
However, clicking onto the titlebar of such windows should realign them.
:p.Clicking on a session's icon in the IconBar with mouse button 1 not only switches
to the previously active window of that session, but even switches to the Virtual
Desktop this window resides on.
:p.Clicking on a session's icon in the IconBar with mouse button 2 displays a popup
menu, from where you can request the window or the application this window belongs to,
to be closed:
:dl tsize=12 break=fit.
:dthd.:hp4.Menuitem&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Show all Icons:ehp9.
:dd.When selecting this item, PC/2 shows all windows icons.
This option is useful to unhide windows you have hidden with the option
:hp9.Hide Icon:ehp9..
:dt.:hp9.Hide Icon:ehp9.
:dd.When selecting this item, PC/2 hides the selected window, that is though PC/2
will know it, it won't be drawn onto the SessionBar.
PC/2 will not save your selections persistently, that is you have to repeat this
step whenever a new instance of the SessionBar is created (usually by restarting
PC/2, but also if you disable and then enable the SessionBar).
:dt.:hp9.Close Application:ehp9.
:dd.When selecting this item, PC/2 tries to close the application this window belongs
to (by posting a WM_QUIT message).
Some applications however, refuse to close down or ask for verification.
:dt.:hp9.Close Window:ehp9.
:dd.When selecting this item, PC/2 tries to close the window (by posting a WM_SYSCOMMAND
SC_CLOSE message).
Again, some windows may decide to ignore this request.
:edl.
:p.If you press the :hp4.CTRL:ehp4.-key while clicking with mouse button 1 onto the
SessionBar window, PC/2 displays a popup menu, which allows you to perform a PC/2 dump:
:dl tsize=12 break=fit.
:dthd.:hp4.Submenu&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Debug:ehp9.
:dd.Use this submenu to request
:link reftype=hd res=4013.<PC/2 debugging support>:elink..
:ul compact.
:li.Dump PC/2 process
:li.Dump W-Thread
:li.Dump I-Thread
:eul.
:edl.
:p.:link reftype=hd res=262.<Backward>:elink.
                                :link reftype=hd res=4014.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected general help from the PC/2 Lockup dialog.                          *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=1100.Lockup OS/2 Help
:p.PC/2 can be configured protect access to your workstation by a
:link reftype=hd res=2150.<Lockup Password>:elink..
When the Lockup OS/2 feature is active, either after invoking manually or by the
configurable Lockup timer, this dialog is displayed and prevents access to your
system as long the correct Password has not been entered.
For complete protection, reboot of your workstation with :hp4.ALT+CTRL+DEL:ehp4. is
also disabled.
:p.This dialog allows other users to leave you messages while you are absent.
They just have to type in her names and messages and select the :hp9.Post It!:ehp9.
button to add them to the message log.
Pressing :hp9.Post It!:ehp9. will also clear the message windows to make room
for another message.
When a user has left you a message the Lockup OS/2 dialog will not appear after you
have correctly entered the Password, but the message windows will be replaced by
the message log, where you can see what the users had to tell you.
You can of course copy the messages into the clipboard.
:dl tsize=12 break=fit.
:dthd.:hp4.Control&colon.
:ddhd.Description&colon.:ehp4.
:dt.:hp9.Message from:ehp9.
:dd.Anyone can identify him/herself and leave a message for you to read.
:dt.:hp9.Post It!:ehp9.
:dd.The message is added to a message log and gets cleared.
:dt.:hp9.Password to unlock OS/2:ehp9.
:dd.In order to dismiss the Lockup OS/2 dialog and to view the messages users have
left for you, you have to type the Lockup Password correctly and to press
:hp4.Ok:ehp4..
If messages have been left and the message log gets displayed by pressing
:hp4.Ok:ehp4. you have to press :hp4.Ok:ehp4. a second time to dismiss the
dialog.
:edl.
:p.You have to enter the Password correctly and press :hp4.Ok:ehp4. to get access.
If you have accidentally mistyped the Password, you can make a unlimited number of retries.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected general help from the PC/2 Access Password dialog.                 *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=1101.Security Password Help
:p.PC/2 can be configured to proctect access to critical Menucontrols of its Popup Menu,
that is the :link reftype=hd res=302.<Desktop Configuration dialog>:elink.,
the :link reftype=hd res=301.<Setup dialog>:elink. and the
:link reftype=hd res=4013.<Exit PC/2>:elink. function, and Menuentries
selected by the user.
PC/2 also allows you to separate this Access protection from the Lockup feature,
allowing you to support multiple users that can prevent their work with a known Lockup
Password but can't launch Menucontrols and Menuentries protected by the Access Password
(e.g. when multiple users operate the same applications in around the clock shifts).
:p.
When having configured an Access Password and checked the option :hp9.Protect Popup Menu:ehp9.
in the Desktop Configuration dialog, you will be prompted for the Access Password
prior getting access to the Menucontrols.
Without having checked the :hp9.Protect Popup Menu:ehp9. option, only those Menuentries
are protected that have checked the :hp9.Password Protected:ehp9. option on the
:link reftype=hd res=1400.<Style>:elink. notebook page.
:p.You have to enter the Password correctly and press :hp4.Ok:ehp4. to get access to
the protected resources and to unlock the Lockup dialog or you may press :hp4.Cancel:ehp4.
to dismiss the dialog without getting access.
If you have accidentally mistyped the Password, you can make a unlimited number retries.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected compatibility information                                          *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4014.PC/2's Compatibility with other applications
:p.This section provides information about incompatibilities and compatibility with some
(but certainly not all) useful packages.
(EWS is the abbreviation for Employee Written Software, a program of IBM to promote
programs written by its employees outside of IBM.
OS2TOOLS is the OS/2 utilities archive on the IBM internal use only IBMPC conference
disk.)
:p.Compatibility information:
:dl tsize=12 break=fit.
:dthd.:hp4.Program&colon.
:ddhd.Compatibility information.:ehp4.
:dt.:hp9.XIT, NPSWPS, Object Desktop, Voicetype Dictation:ehp9.
:dd.All programs mentioned above have similarities to PC/2, that is they are not normal
application software, but system utilities that hook deeply into the system.
Partly they even implement similar features in a similar way, which can cause compatibility
problems between that applications (technically this is caused by the hooks and subclassing
these applications install).
:p.
If you encounter problems and you are running two or more of that utilities concurrently,
you can try to isolate the problem by running them selectively in parallel or by selectively
disabling overlapping features.
:edl.
:p.Useful packages you might find at ftp&colon.//ftp.pc.ibm.com/pub/pccbbs/os2_ews
or by using http&colon.//www.ftpsearch.lycos.com/:
:dl tsize=12 break=fit.
:dthd.:hp4.Program&colon.
:ddhd.Description:ehp4.
:dt.:hp9.BOOTOS2:ehp9.
:dd.This package, available as EWS (or from OS2TOOLS), allows you to create a bootable
system to either diskette or harddisk.
The bootable system can range from a single command prompt, over a PM to
a complete WPS setup. This is useful for creating a bootable maintenance partition on
a logical FAT (because HPFS requires a CHKDSK when not shut down correctly) drive
(requiring under 10 MB disc space), allowing you to recover a
unbootable production partition (e.g. changes in CONFIG.SYS which made it unbootable)
or to remove files, which would have been locked when booting from your production partition.
:p.After creating a PM setup, the standard OS/2 command processor
CMD.EXE is your Workplace Shell process.
:p.By replacing the line :hp4.SET RUNWORKPLACE=\OS2\CMD.EXE:ehp4., which
:hp4.BOOTOS2:ehp4. creates in your :hp9.CONFIG.SYS:ehp9., by
:hp4.SET RUNWORKPLACE=\PC2\PC2.EXE:ehp4., assuming you have installed PC/2 into the
directory :hp4.\PC2:ehp4. on the boot drive, PC/2 will be a more comfortable
Workplace Shell process. You can of course copy your existing :hp9.PC2.CFG:ehp9. file
to this directory, to continue fast access to your favorite programs.
:p.If you want PC/2 (or :hp9.SETBOOT.EXE:ehp9.) being able to automatically reboot OS/2
after a Shutdown, ensure that for OS/2 2.11 the line :hp4.DEVICE=x&colon.\OS2\DOS.SYS:ehp4., and for
OS/2 WARP the line :hp4.DEVICE=x&colon.\OS2\BOOT\DOS.SYS:ehp4. is added in your
:hp9.CONFIG.SYS:ehp9., where x is the the drive letter you installed OS/2 at, e.g. E).
:dt.:hp9.IPFCPREP:ehp9.
:dd.This package, available as EWS (or from OS2TOOLS) is a great help for writing
online help panels (that is compiling *.IPF files into *.HLP files).
This package contains a preprocessor that adds conditional compiling, macros and
some more things, which the IPFC (Information Presentation Facility Compiler) is
definitely missing.
:dt.:hp9.PMPRINTF:ehp9.
:dd.This package, available as EWS (or from OS2TOOLS) is a great help for debugging
PM programs.
This package contains an object named :hp4.PRINTF.OBJ:ehp4., which can be linked in
into an executable to replace the standard C-function printf(), which writes to
stdout, by a printf() that forwards the output into the window of a PM server
application.
This allows real-time traces of stdout output, which you can get otherwise for PM
programs only by redirecting stdout to a file, and analysing the file afterwards.
:dt.:hp9.PMSPY:ehp9.
:dd.This package, available from OS2TOOLS is a great help for debugging
PM messaging.
This package consists of a spy, which allows you the see (of course you may define
filters to see only some of) all PM messages sent within a process and between processes.
:dt.:hp9.PMTREE:ehp9.
:dd.This package, available as EWS (or from OS2TOOLS) or is a great help for analysis
of the hierarchy of all PM windows and within PM windows.
It allows you to see which controls a window consist of, and which styles, resources,
process IDs, window text, position and size,... are assigned to a window.
:dt.:hp9.TVFS:ehp9.
:dd.TVFS, the Toronto Virtual File System, available as EWS (or from OS2TOOLS) is a
must to manage multiple versions of development environments (e.g. IBM Developer's
Toolkit 2.1 and 3.0, IBM C Set++ and IBM VisualAge C++) on a single workstation not
connected to a LAN (using TVFS on a LAN of course allows you to use UNC-names).
:p.For example, say you want to have C Set++ and VisualAge C++ installed. During
installation (in different directories of course) both products add entries to the
CONFIG.SYS. Unfortunately both products require files having the same name. When such
a file (e.g. a DLL) is requested, OS/2 searches the environment (e.g. LIBPATH)
for the first occurence. If this occurence matches a file in the directory of the
version of the product you are not running currently, it may be time for the BRS
(Big Red Switch).
:p.With TVFS you may create a virtual drive, say K&colon.. Then you may link the
requested version of the product onto this virtual drive, for example you may link
F&colon.\IBMCPP30\ as K&colon.\IBMCPP\. In you CONFIG.SYS you have only included
references to the virtual drive K&colon., e.g. the LIBPATH contains only references
to K&colon.\IBMCPP\. If you then want to switch to another version of the product,
just unlink F&colon.\IBMCPP30\ from K&colon. and link F&colon.\IBMCPP21\ as
K&colon.\IBMCPP\ instead.
:p.Without any change to your CONFIG.SYS and without a reboot you have switched
your environment on the fly - what a lightening! Of course using PC/2 to create a Menuentry
that automatically links a certain version and sets up the complete environment
(e.g. environment variables that differ between both versions) is just as easy.
:dt.:hp9.WatchCat:ehp9.
:dd.WatchCat, available from all major OS/2 archives, adds what is definitely
missing in OS/2 - the possibility to recover from processes that CTRL+ESC is not
able to kill.
:p.Just press a hotkey (or activate a small piece of hardware) to invoke WatchCat
when you can't recover from an erroneous process. A fullscreen session allows you
to kill virtually all processes, or to shutdown and reboot your workstation as
a last resort, avoiding a CHKDSK during the next boot.
Once installed you don't want to miss it!
:dt.:hp9.SRVIFS:ehp9.
:dd.SRVIFS can be best characterized with "Mini Peer-Services".
Together with a minimal LAN adapter protocol support (LAPS - Lan Adapter Protocol
Support, or MPTS - Multiple Protocol Transport Services as it is called now) SRVIFS
is a Netbios protocol based Server and Requester.
:p.
You may know WARP's Remote Installation support, which is based on SRVIFS (it just
has replaced the commandline programs by a nice GUI and additionally it allows you
to create the remote installation diskettes).
When you look at the remote installation diskettes, you will find on the third
diskette that SRVIFS is used to link the remote CD-ROM (you are going to install
WARP from) as a network drive, similar to what the LAN Requester or Peer Services do.
:p.
You may now ask what this tool is useful for.
Well, on one hand you can have a LAN requester network support on WARP boot diskettes
or a maintenance partition, on the other hand if you run SRVIFS's SERVICE.EXE instead
of WARP's Remote Installation support, you can have a remote installation Server that
allows to service installation of different OS/2 versions (if you have n CD-ROM
drives (or copied the CD-ROMs onto a server), you can run n instances of SERVICE.EXE
allowing concurrent remote installation of different OS/2 versions (could be even 2.x!).
:dt.:hp9.ICAT:ehp9.
:dd.ICAT is a enhancement to the Kernel debugger that adds symbolic debugging for
device drivers, filesystems,...
Similar to the Kernel debugger, the ICAT user interface runs on a remote PC that
is connected to the MUT (machine under test) via a serial connection.
:p.
Of course you can also use it to debug PM programs where normal PM debuggers are
lost (e.g. debugging PM hooks or PM programs hanging due to a PM resource interlock
problem caused by PM's single input queue).
The only drawback is that ICAT requires you to install the debug kernel (which is
no problem so far), but the connection to the remote PC can only be done over the
serial port - an that's really slow (OS/2 for PowerPC had support for a LAN adapter
protocol stacks in the Kernel, so you could run the Kernel debugger via a fast
Token-Ring LAN instead of the serial port, but once again OS/2 for PowerPC was added
to the list of technically superior products that were scrapped (as e.g. MicroChannel,
PowerPC, OpenDoc, (D)SOM, OS/2 itself,... &colon.-(.
:dt.:hp9.IDBUG:ehp9.
:dd.This package (part of recent compilers of the IBM VisualAge family) is a remote
debugger, which means that in contrast to ICSDEBUG/IPMD this debugger allows you to
run the debuggers user interface on a different machine than the debuggee program.
The 2 PCs are connected via a LAN running the TCP/IP protocol.
:p.
Again, you can also use it to debug PM programs where normal PM debuggers are
lost (e.g. debugging PM hooks or PM programs hanging due to a PM resource interlock
problem caused by PM's single input queue).
When debugging complex multithreaded PM applications, this debugger definitely is a
must have.
:dt.:hp9.OS/2 2.x:ehp9.
:dd.PC/2 was successfully tested with OS/2 2.10 and 2.11 US. It may also be compatible
with OS/2 2.00, but this was not tested.
Some applications running under OS/2 2.x may not be compatible with PC/2's Titlebar
Smarticon enhancement.
:dt.:hp9.OS/2 WARP 3:ehp9.
:dd.PC/2 is fully compatible with OS/2 WARP 3. Using previous versions of PC/2
(1.70 and before) may cause problems when using the :hp4.Virtual Desktop:ehp4..
Of course, you can continue to use the Launchpad together with PC/2.
:dt.:hp9.OS/2 WARP 4:ehp9.
:dd.PC/2 is fully compatible with OS/2 WARP 4.
Of course, you can continue to use the Launchpad and WarpCenter together with PC/2.
:dt.:hp9.OS/2 WARP Server Advanced SMP:ehp9.
:dd.PC/2 is fully compatible with OS/2 WARP Srv. Adv. SMP, except that using PC/2's APM
support caused kernel hangs on my PC.
Of course, you can continue to use the Launchpad together with PC/2.
:dt.:hp9.OS/2 WARP Server for e-Business (Aurora):ehp9.
:dd.PC/2 is fully compatible with OS/2 WARP Server for e-Business, both running the
UNI and the SMP kernels, except that the CPU Performance graphics does not work
on my installations.
Of course, you can continue to use the Launchpad and WarpCenter together with PC/2.
:edl.
:p.:link reftype=hd res=262.<Backward>:elink.
                                :link reftype=hd res=4018.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected bugs/problems information                                          *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4018.PC/2's known problems
:p.This section provides information about known bugs and problems of PC/2.
You may also see some more information about the :link reftype=hd res=4016.<technical
background>:elink. of potential problems.
:ul.
:li.Traps and hangs when using Titlebar Smarticons
:p.All programs adding controls (e.g. buttons to titlebars) to the frame windows of all
applications running in the system suffer from the same design limitation of OS/2 (by the
way Windows has the same limitation).
This problem may cause during processing of a message, a call to pointers to window procedure
that is invalid may be made, and the applications likely terminates due to an access violation
then.
Some applications are not prepared to get additional controls added to the frame window,
and terminate due to access violations, because running out of resources (e.g. stack space).
:p.To avoid such problems you may:
:ul compact.
:li.either include the failing applications in the Exception List, or disable the Titlebar Smarticons,
:li.ensure that only one application adding controls to all frame windows is running
concurrently (for example, VoiceType Dictation, XIT, NPSWPS, the LAN Server GUI and PC/2 are
such applications),
:li.ensure that if you are running multiple applications that controls to all frame windows,
you don't close them as long frame windows contain controls added by them, or at least don't
close them in a different sequence than the inverse sequence you launched them,
:li.upgrade to Warp 4 as later releases of OS/2 seems to be more stable.
:eul.
:li.Slow notebook closing under OS/2 2.x
:p.When running PC/2 under OS/2 2.x, you may notice, that when you close PC/2's notebooks,
PC/2 scrolls through all notebook pages automagically.
I had to add this behaviour to prevent a bug of the Notebook control in relation to
MLEs (MultiLine Entryfields), which causes the MLEs to return only 1 character per CRLF
terminated line when the notebook pages containing the MLEs were never the topmost
pages.
:p.I know of at least another instance of that problem (with an equivalent workaround).
I'm pretty sure that this is a PM bug, as under Warp 3 and Warp 4 this bug has been
fixed (and I don't want to open an APAR just against OS/2 2.x as it is going out of
service soon).
:li.PC/2 windows become owner of other processes windows
:p.There exists a bug in earlier versions of the OS/2 PostScript printer driver, which causes
the owner chain to break when PC/2 ensures the Overview and SessionBar windows to be kept on
top of Z-Order.
I (and the OS/2 change team in Austin which handled this bug as APAR PMR 11867) could reproduce
this bug with the Job Properties dialog of the WPS PostScript printer drivers shipped with
OS/2 2.x and Warp 3.
I could not reproduce this problem under Warp 4 or when using the latest PostScript printer
drivers under OS/2 2.x and Warp 3 and the recommended fix is to update to the latest PostScript
printer driver.
:p.However, as the defective PostScript printer driver was shipped by IBM as an example how
to write printer drivers (as far as I know), other printer drivers (e.g. I know of the
Canon LBP-8III printer driver also having this defect) may also have copied that defect, so
please try to install the latest printer driver for other printers too.
:p.As a workaround, just be sure that you don't close PC/2 while one of its windows is the
owner of a window in another process (you can easily verify a window being the owner by
clicking on that window, if the system beeps and you can't interact with that window anymore it
has become the owner window of another window).
Otherwise you can be sure you have to reboot OS/2 soon.
:eul.
:p.:link reftype=hd res=4014.<Backward>:elink.
                                :link reftype=hd res=4011.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected error help.                                                        *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4011.Help for PC/2 Errors
:p.PC/2 can't verify all user input for validity, so some user input may cause
some PC/2 functions to fail. In this case, a message box is displayed containing
the module and the sourcecode linenumber that failed. Also, an error message is
displayed, containing which error occurred and what should be done to resolve the
problem.
:p.E.g. you might've configured to start the program :hp4.VIEW.EXE:ehp4. by
writing :hp4.c&colon.\os3\view.exe:ehp4. in the entryfield :hp9.Executable:ehp9.
in the Program Installation dialog. This of course leads to an error, because when
OS/2 tries to start this item, it can't find the directory :hp4.\os3\:ehp4. simply
because it should read :hp4.\os2\:ehp4.. A message box is displayed saying an
error occurred, and that the user input should be corrected.
:p.Some messages aren't even errors, just warnings (e.g. the message that informs
you that the file PC2.INI does not exist or must be updated from a previous version
of PC/2).
:p.All messages contain some information that may help to isolate the problem:
:ul compact.
:li.The first part contains a description of the problem, or what may have
caused the problem.
For example it may inform you that a file is missing, or a program could not
be started.
:li.The name of the source code module where the error was detected.
:li.The line number in the source code where the error message was displayed.
:li.The last lines may report the error code of the failing OS/2 call and
some hints how to correct the problem.
For example when changing into a non existing directory, the hints tell you that
the a bad user input for disk data caused the problem.
:eul.
:p.If you experience a problem you have no idea on how to solve or circumvent,
you are welcome to ask for support by sending some
:link reftype=hd res=303.<E-Mail>:elink. to the author.
:p.:link reftype=hd res=4018.<Backward>:elink.
                                :link reftype=hd res=4003.<Forward>:elink.

:h2 res=4100.Help for resource problem
:p.PC/2 detected an unexpected resource problem during its internal
processing. One ore more resources couldn't be allocated. Either
PC/2 or OS/2 has run out of resources.
:p.Please report this problem together with a description how this problem
occurred. You may also try to free up some resources by closing some
windows.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4101.Help for running PC/2
:p.PC/2 detected that one instance of PC/2 is already running
in your system. PC/2 can only be run once concurrently, this instance
will therefore shutdown itself.
:p.Please ensure that only one instance of PC/2 gets started concurrently,
to avoid this error message.
This limitation also applies when running two different versions of PC/2,
which also may trap all instances of PC/2 due to version incompatibility.
:p.You may experience this error if you have just exited PC/2 and restarted
it immediately afterwards.
This is due to the fact, that PC/2 performs some cleanup processing in
the background, after it gets invisible for the user, which may take some
time.
This is a normal behaviour, just wait a few more seconds (the faster your
PC, the less time PC/2 requires for its cleanup processing) between
exiting and restarting PC/2.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4102.Help for loading DLLs
:p.PC/2 requires access to the mandatory DLLs :hp4.PC2HOOK.DLL:ehp4. and
:hp4.PC2?RES.DLL:ehp4. (where ? is either V for VGA or B for SVGA) and
to the optional DLLs :hp4.PC2SPOOL.DLL:ehp4. and :hp4.PC2NET.DLL:ehp4..
:p.
There are mainly 2 reasons why PC/2 can't load those DLLs:
:ol compact.
:li.The DLL could not be located by the OS/2 loader or a function within that
DLL couldn't be loaded successfully.
This can and should not happen for :hp4.PC2HOOK.DLL:ehp4. and  :hp4.PC2?RES.DLL:ehp4.,
however in addition to having installed PM (Presentation Manager),
:hp4.PC2SPOOL.DLL:ehp4. requires the OS/2 Print Spooler and
:hp4.PC2NET.DLL:ehp4. requires TCP/IP installed additionally.
:li.The DLL could not be loaded because of a signature mismatch (which is
a kind of version check to ensure that PC/2 does not load wrong DLLs that
may result from an incorrect installation of PC/2 over an existing installation).
In that case you likely installed PC/2 incorrectly over a previous version, a
new clean install is recommended.
:eol.
:p.
Please ensure that PC/2 is installed correctly and verify that the mentioned
DLL can be found in the directory PC/2 was installed into.
You should also check the version of PC/2 and the DLLs using :hp4.BLDLEVEL:ehp4.
utility provided by OS/2.
Please refer to the :link reftype=hd res=4015.<PC/2 diagnostics>:elink.
section for more information about the :hp4.BLDLEVEL:ehp4. command.
:p.
You may need to reinstall PC/2 with the installation program by first deinstalling
it, rebooting and installing PC/2 again.
Though PC/2 will preserve your settings I strongly recommend you to backup
:hp9.PC2.CFG:ehp9., :hp9.PC2.INI:ehp9. and :hp9.PC2.ENV:ehp9..
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4103.Help for accessing PC2.INI
:p.A problem occurred accessing the profile :hp9.PC2.INI:ehp9..
You will usually get this message when :hp9.PC2.INI:ehp9. can't be accessed,
either because PC/2 can't read from it, or write to it.
:p.When you are running PC/2 for the first time, this message just
informs you that an empty or non-existent profile was found.
Once you have entered and saved some
:link reftype=hd res=302.<Desktop>:elink. configuration data,
PC/2 will create this file and use it for future invocations.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4107.Help for accessing PC2.ENV
:p.A problem occurred accessing the profile :hp9.PC2.ENV:ehp9., or the
Environment Spaces configuration profile you specified using the
:hp4.-Environment:ehp4. commandline parameter.
You will usually get this message when :hp9.PC2.ENV:ehp9. can't be accessed,
either because PC/2 can't read from it, or write to it.
:p.When you are running PC/2 for the first time, this message just
informs you that an empty or non-existent profile was found.
Once you have defined and saved some
:link reftype=hd res=2200.<Environment Spaces>:elink., PC/2 will create
this file and use it for future invocations.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4104.Help for loading PC2SPOOL.DLL
:p.PC/2 requires access to the DLL :hp8.PC2SPOOL.DLL:ehp8..
OS/2 reported that this DLL or a function within this DLL couldn't be loaded
successfully.
:p.Please ensure that :hp8.PC2SPOOL.DLL:ehp8. is in the directory PC/2
was started from.
You should also ensure that you don't use the DLL from another version of PC/2,
because they are likely incompatible.
You can check the version by comparing the file dates, which should be
identical.
:p.This message is only informational - you may continue using PC/2,
however the :hp9.Spooler:ehp9. interface is disabled.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4105.Help for creating working thread
:p.To improve overall system performance, OS/2 was designed to support multithreading
for applications.
PC/2 also uses this feature, to perform time-intensive tasks with a
second working thread, to avoid unresponsiveness of PM.
:p.During creation of this working thread, an error occurred.
PC/2 will not be able to continue.
:p.You may have run out of resources - close some windows or applications
and retry.
You may have run out of threads; if you are using applications that take
full power using multiple threads, increase the :hp4.THREADS:ehp4. count
in your :hp9.CONFIG.SYS:ehp9. then.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4106.Help for accessing PC2.CFG
:p.A problem occurred accessing the profile :hp9.PC2.CFG:ehp9., or the profile you specified
using the :hp4.-Profile:ehp4. commandline parameter.
You will usually get this message when :hp9.PC2.CFG:ehp9. can't be accessed,
either because PC/2 can't read from or write to.
:p.When you are running PC/2 for the first time, this message just
informs you that an empty or non-existent profile was found.
Once you enter some configuration data, PC/2 will create this file,
and use it for future invocations.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4108.Help for PC/2's hooks
:p.PC/2 requires to be able to hook into three hooks provided by OS/2 - (the
Input Hook, the Pre-Accelerator Hook and the Sendmessage Hook), in order to
intercept mouse movements, key presses and window positioning.
:p.If you experience this error during initialization of PC/2, you
can't continue - most likely you have run out of resources or OS/2 is
corrupted - retry after a Shutdown and reboot of OS/2.
Ensure that the correct version of :hp4.PC2HOOK.DLL:ehp4. is accessible in
the directory you installed PC/2 into, and rerun PC/2 again.
:p.If you experience this error during shutdown of PC/2 it is
recommended to shut down OS/2 to avoid possible data loss.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4109.Help for dialog creation
:p.You selected an option that required further user intervention.
PC/2 tried to load a dialog window to get the necessary user
input, but loading the dialog failed.
:p.You may have run out of resources.  Close some windows and applications and
retry again.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4110.Help for changing directory
:p.PC/2 tried to change the current working directory on a drive.
OS/2 reported that a call accessing the directories was unsuccessful, however you may
ignore this message and continue your work.
:p.You may notice that a selected session didn't get started, or the working
directory on a drive wasn't changed correctly.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4111.Help for changing priority
:p.PC/2 allows you to specify the priority for some session types,
which required PC/2 to change its priority, because the current
priority gets inherited to sessions started by PC/2
:p.You may ignore this message and continue your work, however a shut down is
suggested to avoid potential data loss.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4112.Help for starting a session
:p.An error occurred when PC/2 tried to launch the selected session.
PC/2 differentiates between executable programs, which PC/2 launches itself,
and WPS objects, which are launched by the WPS (WorkPlace Shell) on behalf
of PC/2.
Launching WPS object therefore requires the WorkPlace Shell process, that is
the executable PMSHELL.EXE, running.
:p.For executable programs you may verify the parameters you entered for the
session and retry the operation.
If a module name is given, that is a DLL of the product to be started,
you may have to add this DLL to a path pointed to by the LIBPATH environment
variable, or alternatively specify the BEGINLIBPATH or ENDLIBPATH settings
under OS/2 Warp.
:p.You may also check the following items:
:ul compact.
:li.Is the executable's name correctly spelled?
:li.If you don't have specified a working directory, does the executable's name
include full path information?
:li.Is the working directory spelled correctly?
:li.Are all DLLs required by the application accessible for the OS/2 loader, either
via the LIBPATH (configurable in your CONFIG.SYS only) or BEGINLIBPATH/ENDLIBPATH
(definable in PC/2's Program Installation dialog for every application)?
(Refer to your application's documentation on how to install the product correctly.
Did you reboot your system after finishing the installation to get the changes into
effect?)
:li.Is the application's environment specified correctly?
:li.Are the selected Environment Spaces configured correctly?
:li.Is the application's session type specified correctly?
:li.Have you tried to start the application through a command processor (CMD.EXE
for OS/2 programs, or COMMAND.COM for DOS programs)?
:li.Have you tried to start the application using the same parameters as entered in
the application's Program Installation notebook in a command processor window?
:eul.
:p.If the failure persists, you may have run out of resources, close some
windows and applications and retry again.
You may have run out of threads.  If you are using applications that take
full power using multiple threads, increase the :hp4.THREADS:ehp4. count
in your :hp9.CONFIG.SYS:ehp9. then.
:p.For WPS Objects ensure that the WPS is running. Either wait a few seconds if
you have just started the WorkPlace Shell process and retry the operation, or
start the WorkPlace Shell process (e.g. by typing START PMSHELL from an OS/2
commmand line).
You may verify that the WorkPlace Shell process is running by activating the
Window List and searching for an entry names :hp8.Desktop-Icon View:ehp8..
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4113.Help for accessing media
:p.PC/2 tried to access removable or non-removable media to
determine the media's parameters.
OS/2 reported that a call accessing the media was unsuccessful, however you may
ignore this message and continue your work.
:p.You may notice that a selected session didn't get started, or the working
directory on a drive wasn't changed correctly.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4114.Help for PC/2 as WorkPlace process
:p.When running PC/2 as the WorkPlace Shell (WPS) process (that is having a line
:hp4.SET RUNWORKPLACE=x&colon.\path\PC2.EXE:ehp4. in your :hp9.CONFIG.SYS:ehp9.),
you will not be able to close, and therefore terminate, PC/2.
PC/2 will ignore any requests to close from the Window List and the Menuentry
and Smarticon to close PC/2 are disabled during PC/2's initialization.
:p.This is not a limitation of PC/2, OS/2 always restarts the application
configured as the WorkPlace process, when this process terminates for some reason.
You may have noticed that the WPS itself gets restarted when it traps due to
an error caused by itself or its registered WPS Objects.
:p.If you no longer want to use PC/2 as the WorkPlace process, please
refer to :link reftype=hd res=4012.<Installing PC/2>:elink. on how
to undo the changes you made to use the PC/2 as the WorkPlace process.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4115.Help for PC/2 as PROTSHELL process
:p.When running PC/2 as the proteced mode shell (PROTSHELL) process (that is having a line
:hp4.PROTSHELL=x&colon.\path\PC2.EXE:ehp4. in your :hp9.CONFIG.SYS:ehp9.),
you will not be able to close, and therefore terminate, PC/2.
PC/2 will ignore any requests to close from the Window List and the Menuentry
and Smarticon to close PC/2 are disabled during PC/2's initialization.
:p.This is not a limitation of PC/2, but an OS/2 requirement, as OS/2 adds threads to the
process running as the PROTSHELL (e.g. to display the Window List).
Exiting the PROTSHELL process would cause a unrecoverable (except for a reboot) PM hang,
unfortunately any program failure of the PROTSHELL process would have the same
consequences.
:p.If you no longer want to use PC/2 as the PROTSHELL process, please
refer to :link reftype=hd res=4012.<Installing PC/2>:elink. on how
to undo the changes you made to use the PC/2 as the PROTSHELL process.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4116.Help for PC/2's Security features
:p.Because PC/2 displays the Access and Lockup Password (which are likely to be different)
only in encrypted form, PC/2 requires that the Passwords are entered twice.
You have to enter the Access and Lockup Passwords in the corresponding entryfields
and verify them in the corresponding Verification entryfields.
:p.This will ensure that you really know what Passwords you have entered, because
once the Lockup Dialog is active you can dismiss it only by using exactly the correct
password, whereas for the Access Password you can cancel your request.
The Lockup Dialog can't even be dismissed by shutting down OS/2 via
:hp4.CTRL+ALT+DEL:ehp4., so PC/2 forces you to enter the requested password twice.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4117.Help for automatic shutdown and reboot
:p.The request to reboot your workstation failed.
PC/2 requires the :hp9.DOS.SYS:ehp9. device driver to be installed to
reboot your workstation, please verify at the
:link reftype=hd res=2050.<General Settings>:elink.
help how to install this device driver.
:p.After the file system has benn shut down completely (that is when the hard disk
activity LED stopps blinking and a message is displayed that you can reboot now),
please press :hp4.ALT+CTRL+DEL:ehp4. manually to reboot.
:p.If the driver is installed, this failure is not expected to occur, so please
send any information how you experienced this problem to the author, thanks.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4118.Help for PC/2's internal timer
:p.The request to configure an OS/2 PM timer failed.
PC/2 requires a timer to refresh the PC/2 spooler control window and to lockup
your workstation with PC/2's lockup feature when the defined period of user
activity has timed out.
This failure is not expected to occur, but you may continue with PC/2 except for
an automatic refresh the PC/2 spooler control window and with manual activation of
the PC/2 lockup feature.
:p.Usually this problem occurs if you are run out of resources, so you may want to
close some windows and applications and retry.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4119.Help for caching icons for PC/2's Popup Menu
:p.If you have selected that PC/2's Popup Menu should contain the Menuentries icons
besides the icon texts, PC/2 has to do some quite smart work, to locate the
Menuentries icons. As you may imagine, this may quite take a while for a larger
Popup Menu.
:p.PC/2 therefore tries to cache the paths to access the Menuentries' icons
to reduce load time. In order to use this cache for the next time you launch PC/2
you have to save the Popup Menu to file - which of course saves all other unsaved
changes you've made to the Popup Menu.
You may of course ignore the request to save the Popup Menu, which just requires
PC/2 to fill the cache by some smart work the next time you launch PC/2 again
(instead of reusing the work done by the last time PC/2 ran, which would reduce
the load time of the Menuentries' icons).
:p.:link reftype=hd res=1800.<Click here>:elink. to select some more
information about using icons in the Popup Menu.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4120.Help for updating PC/2's Popup Menu
:p.PC/2 has either tried to adjust the Popup Menu or you have just tried to add
some Menuentries, extracted by WPS 2 PC/2, to the Popup Menu.
For some unforeseeable reasons PC/2 could adjust its internal structure that
controls the Popup Menu, but could not reflect these adjustments into the
structures OS/2 PM uses to display the Popup Menu.
:p.The adjustments made will not be reflected in the Popup Menu displayed by
PM, but they are reflected in PC/2's internal structures, so you may either
continue your work with an inaccurate Popup Menu, or you may want to save
the adjustments you made to your configuration file, then close and reload PC/2.
:p.To be on the safe side on protecting your data, you may want to make a copy
of your configuration file, which is usually named :hp8.PC2.CFG:ehp8. (unless
you specify the :hp4.Profile:ehp4.
:link reftype=hd res=4008.<Commandline option>:elink.) before saving the
adjustments you made to your configuration file.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4121.Help for creating or modifying PC/2's SessionBar
:p.PC/2 has either tried to create or to modify the SessionBar, but this
operation failed.
Most likely you have run out of resources, so close some windows or reboot.
:p.You may continue, though the SessionBar will not be available or the requested
modifications to the SessionBar will not be applied.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4122.Help for creating PC/2's QuickSwitch window
:p.PC/2 has either tried to create or to modify the QuickSwitch window, but this
operation failed.
Most likely you have run out of resources, so close some windows or reboot.
:p.You may continue, though the QuickSwitch window will not be available.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4123.Help for APM suspend/power off
:p.The request to suspend or power off your workstation failed.
PC/2 requires the :hp9.APM.SYS:ehp9. device driver to be installed to suspend
or power off your workstation, please
verify at the :link reftype=hd res=2050.<General Settings>:elink.
help how to install this device driver.
:ul compact.
:li.If you have selected to suspend, PC/2 will ask APM (Advanced Power Management)
to suspend your system.
Depending on your hardware and BIOS, your workstation will write its status onto
the harddisk an turn itself off (usually on Laptops), or put itself into a sleep
mode from where a mouseevent, keystroke, LAN access,... is required to recover.
:li.If you have selected to power off your workstation, P/2 will ask APM to
perform this operation.
APM will then perform a ShutDown of OS/2 and turn off your workstation.
:warning text='Warning!'.
Only the APM device drivers beginning with Warp 4 support power off requests,
previous versions also power off your workstaation, though no ShutDown of OS/2
is performed and :hp9.CHKDSK:ehp9. will be run during the next boot.
Do not copy Warp 4's APM device drivers to previous versions of OS/2, as there
are also dependencies upon other system files including the OS/2 kernel!
:ewarning.
:eul.
:p.If the driver is installed, this failure is not expected to occur, so please
send any information how you experienced this problem to the author, thanks.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4124.Help for cleanup processing
:p.A problem was encountered during termination processing of PC/2.
This problem is not expected to occur, though there may be situations I could
not anticipate during development and testing.
:p.As PC/2 is integrated very deeply into OS/2 PM, the only choices are trying
to kill PC/2 or to reboot as a final measure (after having saved your work,
which should be possible as PC/2 detected the problem before OS/2 becomes
inoperable).
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4125.Help for backing up PC/2 configuration
:p.PC/2 allows you to request that backups of your currently loaded configuration, that
is :hp9.PC2.CFG:ehp9., :hp9.PC2.INI:ehp9. and :hp9.PC2.ENV:ehp9. or their replacements
that have been specified by :link reftype=hd res=4008.<commandline options>:elink..
Additionally, PC/2 tries to count changes you have made to your configuration and
suggests you to do a backup if a threshold gets exceeded.
:p.Your configuration will be backed up into files named :hp9.PC2-CFG.BAK:ehp9.,
:hp9.PC2-INI.BAK:ehp9. and :hp9.PC2-ENV.BAK:ehp9.
If for some reason (you know software can't be perfect ;-) have to restore from the backup
files made, you can either copy them to different files which I strongly recommend or
request them to be used by commandline options (in which case further backups will fail,
as the "original" and "backup" will have the same filenames!).
:p.The backup will likely work, some reasons why it may fail are, that the files used
for the backup:
:ul compact.
:li.are locked (e.g. opened by another application)
:li.are set to read-only (e.g. by the :hp4.ATTRIB:ehp4. command)
:li.are located on a drive that is read-only (e.g. a LAN-drive, a TVFS-drive, a CD-ROM)
:li.contain invalid filename characters for the filesystem used (though I don't know
any where this could happen)
:eul.
The message box displayed when an error occurrs may be of further help, as it displays
the error in OS/2 terminology.
:p.In general, my advice is to backup critical files, and with critical files I mean files
that can't be easily restored by reinstalling an application or from a different
installation, regularily.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4126.Help for PC/2 thread problems
:p.PC/2 is a multithreaded application, where a main thread is responsible for the GUI
stuff handling and other threads supporting it for lengthy processing (in short exactly
the way OS/2 applications should be designed to prevent them blocking the SIQ (single
input queue), which is quite easy to do but not even IBM applications sometimes do).
:p.Unfortunately, the thread mentioned in the message seems to have a problem which
PC/2 has detected, as it does no longer respond to messages posted to it.
Likely this is due to the thread blocked/hanging/looping, but most likely it is blocked
in some OS/2 call, which from my experience can happen if other applications do block
the SIQ (e.g. running Lotus Notes and NetScape increases the likelyhood of such a
problem) or crash.
I definitely know that if PC/2 tries to query the window text of a frame window that
is just being destroyed, the OS/2 API WinQueryWindowText() does not return to the
caller PC/2 but is blocking.
I consider that a bug or design flaw of PM, however I don't expect that being changed
ever.
:p.Since V2.20, PC/2 now has a built-in :hp2.Hang Protection:ehp2. which will allow
PC/2 to continue in such situations after a few seconds or minutes.
The message you're currently looking at is the result of PC/2 detecting a thread
blocking for too long.
If you press :hp4.Ok:ehp4., PC/2 will use its unique :hp2.Hang Protection:ehp2. to
resume execution.
In most (in my test cases all) cases you will not have to do anything additional,
if the SessionBar and Overview Window is not updating then select the Overview
window and press :hp4.CTRL+SYSREQ:ehp4. to force PC/2 to sync between the threads
again, if that still does not help you have to (force) exit PC/2 by selecting exit
or pressing ALT+CTRL+SHIFT+END at the OverView window and then killing it with a
process killer if it does not terminate in a reasonable amount of time.
:p.Technically what is the :hp2.Hang Protection:ehp2.?
PC/2's main thread does regularily increament a counter for each thread, which the
thread resets for each message processed.
If a thread does not reset the counter in a reasonable amount of time, PC/2's main
thread detects the counter overflow, displays the message you are looking at and
kills the thread with DosKillThread().
For each thread during its startup an Exception Handler is installed that is called
for every exception that happens and in most cases just passes that exception on to
OS/2's default exception handler.
However, if the exception handler detects that the exception was raised by DosKillThread()
it uses the C-statements setjmp() and longjmp() to return the thread to a known state
again (that is the current message is not processed anymore and thread execution returns
to the message processing loop).
In all my test scenarios that resolved the problem of PC/2's Working Thread being blocked
in PM APIs, so it seems to work very well, one known problem is that some memory might
not be deallocated correctly as the thread will not be continued exactly where it
had been blocking but at the message loop.
If I had known all what I have learned from PC/2 back in 1992 when I started PC/2, and IBM
had had a C++ compiler, than some things could have been done more elegant or with more
fine-tuning, but you see from such things how powerful OS/2 really is.
:p.Please report from the message the thread and the last message being processed by it
to the author to help finding ways to diagnose such problems in more detail (including
if the Hang Protection has worked for you), thanks!
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4127.Help for PC/2 Online Automatic Update status
:p.PC/2 has tried to access the TCP/IP protocol stack or the WEB-Server, but one or
more (though only a few of them will show up simultaneously to prevent running out
of resources) unexpected errors occurred.
The error may either be caused by a local problem, e.g. that PC/2 can't correctly interface
with the local TCP/IP configuration, or by a remote problem, e.g. that PC/2 can't connect
to, read from, post to its webpage, PC/2 can't connect to your Proxy,
or that the WEB-Server the weppage is located at didn't
like the request (which likely is just a temporary problem with GeoCities).
:p.To correct the problem, you may try to connect to PC/2's homepage at
:hp4.http&colon.//www.geocities.com/SiliconValley/Pines/7885/AutoUpdate.html:ehp4.
with a Web-browser, e.g. NetScape.
If a page is displayed then, likely PC/2 or its setup caused the problem.
One thing you may check then if you have correctly entered the Proxy setup (it should
contain the same data as your browser), or if your Proxy (possible) or the
WEB-Server (very unlikely that GeoCities or I will need that in future) requires Authorization.
If the problem persists, please contact the author (possibly including a trace done with
:hp4.IPTRACE:ehp4. and :hp4.IPFORMAT:ehp4.)!
:p.Some of the most common error codes are:
:dl tsize=12 break=fit.
:dthd.:hp4.Error&colon.
:ddhd.Description.:ehp4.
:dt.:hp9.1:ehp9.
:dd.Invalid function. PC/2 could download its Automatic Update webppage but not correctly
parse the information within it. This may be a temporary problem (e.g. when the page
is currently updated by the author of PC/2), so you may retry with increasing intervalls
later and check the author's web pages with your browser for more details.
:dt.:hp9.5:ehp9.
:dd.Access denied. PC/2 failed to open :hp9.PC2.NET:ehp9. for writing.
Possible reasons are the file being read-only, opened exclusively by another
process or missing access rights.
You may try to rewrite :hp9.PC2.NET:ehp9. with an editor to determine the problem more
exactly.
:dt.:hp9.4xx:ehp9.
:dd.The :hp9.4xx:ehp9. HTTP Status Codes come from either your Proxy or the WEB-Server
PC/2 has connected to (so you know a connection was made and just needs to be fine-tuned).
The following HTTP Status Codes are defined (please find more details in the :hp4.Hypertext
Transfer Protocol -- HTTP/1.1:ehp4. which you should easily find on the Internet):
:dl tsize=5 break=fit.
:dthd.:hp4.Code&colon.
:ddhd.Description.:ehp4.
:dt.:hp9.400:ehp9.
:dd.Bad Request
:dt.:hp9.401:ehp9.
:dd.Unauthorized. This error means that the WEB-Server (Geocities where PC/2's Registration
page is located at) requires you to authenticate.
You would have to enter a :hp9.Userid:ehp9. and :hp9.Password:ehp9. to allow authentication,
however as currently neither Geocities nor me do plan to require authentication this problem
very likely is an error, so please drop me a e-mail to get some help.
:p.
If you have already supplied a :hp9.Userid:ehp9. and :hp9.Password:ehp9. and the error still
occurs please ensure that you have entered them correctly.
:dt.:hp9.402:ehp9.
:dd.Payment Required
:dt.:hp9.403:ehp9.
:dd.Forbidden
:dt.:hp9.404:ehp9.
:dd.Not Found
:dt.:hp9.405:ehp9.
:dd.Method Not Allowed
:dt.:hp9.406:ehp9.
:dd.Not Accessible
:dt.:hp9.407:ehp9.
:dd.Proxy Authentication Required.
This error means that the Proxy your are using requires you to authenticate.
You would have to enter a :hp9.Userid:ehp9. and :hp9.Password:ehp9. to allow authentication,
which is the same :hp9.Userid:ehp9. and :hp9.Password:ehp9. you would enter when
using NetScape to access PC/2's Registration page.
Your ISP (Internet Service Provider) should give you the required data, however if that
still does not work your Proxy may require a kind of authorization that PC/2 currently
does not support (only Basic authorization is supported at the moment, and I know only
of 1 user that requires Proxy Authorization at all).
:p.
If you have already supplied a :hp9.Userid:ehp9. and :hp9.Password:ehp9. and the error still
occurs please ensure that you have entered them correctly.
:dt.:hp9.408:ehp9.
:dd.Request Timeout
:dt.:hp9.409:ehp9.
:dd.Conflict
:dt.:hp9.410:ehp9.
:dd.Gone
:dt.:hp9.411:ehp9.
:dd.Length Required
:dt.:hp9.412:ehp9.
:dd.Precondition Failed
:dt.:hp9.413:ehp9.
:dd.Request Entity Too Large
:dt.:hp9.414:ehp9.
:dd.Request-URI Too Long
:dt.:hp9.415:ehp9.
:dd.Unsupported Media Type
:edl.
:dt.:hp9.5xx:ehp9.
:dd.The :hp9.5xx:ehp9. HTTP Status Codes come from either your Proxy or the WEB-Server (more likely)
PC/2 has connected to (so you know a connection was made and just needs to be fine-tuned).
The following HTTP Status Codes are defined (please find more details in the :hp4.Hypertext
Transfer Protocol -- HTTP/1.1:ehp4. which you should easily find on the Internet):
:dl tsize=5 break=fit.
:dthd.:hp4.Code&colon.
:ddhd.Description.:ehp4.
:dt.:hp9.500:ehp9.
:dd.Internal Server Error
:dt.:hp9.501:ehp9.
:dd.Not Implemented
:dt.:hp9.502:ehp9.
:dd.Bad Gateway
:dt.:hp9.503:ehp9.
:dd.Service Unavailable
:dt.:hp9.504:ehp9.
:dd.Gateway Timeout
:dt.:hp9.505:ehp9.
:dd.HTTP Version Not Supported
:edl.
:dt.:hp9.10022:ehp9.
:dd.Invalid argument. PC/2 tried to do a DNS (Domain Name Server) lookup, but the DNS
could not resolv PC/2's Automatic Update webpage. This may be a temporary problem
(e.g. due to network problems), so please retry later.
:dt.:hp9.10051:ehp9.
:dd.The network is unreachable. PC/2 could resolv the the address of its Automatic Update
webpage, but could not connect. This may be a temporary problem (e.g. due to network
problems), so please retry later.
:dt.:hp9.10054:ehp9.
:dd.Connection reset by peer. PC/2 could send the request to the server, but the
response was not retrieved in time.
This may be cause by a network problem, the server closing the connection prematurely
(or if you're taking too much time between sending and receiving while stepping
though PC2NET.DLL with the debugger).
:dt.:hp9.10057:ehp9.
:dd.The socket is not connected.
:dt.:hp9.10060:ehp9.
:dd.Connection timed out. PC/2 could resolv the the address of its Automatic Update
webpage, but could not connect. This may be a temporary problem (e.g. due to server
problems like overload or shutdown), so please retry later.
:dt.:hp9.10061:ehp9.
:dd.Connection refused. PC/2 did connect to a host, but this host refused to service
HTTP requests. This may be a temporary problem (e.g. due to server problems like overload
or shutdown), so please retry
later.
:edl.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4128.Help for PC/2 Automatic Update news retrieval
:p.PC/2 has downloaded some news about PC/2.
As the time between PC/2 versions more likely is in the range of more than 12 months (and
I won't change that as I do think too frequent updates are a pain, even neglecting
that it's a major effort for me to package, test, announce and distribute a new version,
I just don't have all time on earth ;-), this allows me to inform you about things
that might be interesting for you painlessly.
:p.
There's not much I can say about the news here, but they'll likely cover things like
announcements, problem workarounds, information about other useful programs,
clarification to the documentation shipped in the original PC/2 package,...
If you have missed the message box displaying the news, just check :hp9.PC2.NET:ehp9.,
which will contain the most recent news.
:p.
Finally some words why I added Online Automatic Update capabilities to PC/2.
The major reasons are to allow me easily to ship updates without distributing a complete
package, allow me to inform you about updates so you don't need to check my homepage
regularily (though you are welcome of course ;-) and to see if time has come to use
such a feature (time passes in Web years know and I think it's time for such a technology ;-).
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4129.Help for PC/2 Automatic Update file updates
:p.PC/2 has downloaded updates for PC/2, though likely manual intervention from your
side is required to make them effective (I don't think doing hidden updates is a good
idea in general).
There are two major types of updates, that is:
:ol compact.
:li.Replacements, which means a file part of the original PC/2 package will be replaced
by a more recent version.
This is usually caused to implement bug fixes or feature enhancements.
:li.Additions, which means a file not part of the original PC/2 package.
This is useful to make some minor feature enhancements available without releasing a new
version of PC/2.
As the time between PC/2 versions more likely is in the range of more than 12 months (and
I won't change that as I do think too frequent updates are a pain, even neglecting
that it's a major effort for me to package, test, announce and distribute a new version,
I just don't have all time on earth ;-), this allows me to release minor things painlessly.
:eol.
:p.
As I said, manuall intervention is usually required, and instructions will be part of
the update though I don't think it is so complicated you'll need them (you may want to
check :hp9.PC2.NET:ehp9. if you have missed the message box displaying the instructions).
In general, the following approach will most likely be part of the instructions:
:ol compact.
:li.For Replacements, in general you have to at least stop PC/2 if not to boot into
maintenance mode.
If the file can't be replaced (you can eaily verify that after stopping PC/2 and
trying to rename the to be updated file to the same name. If you then get an OS/2
SYS0032 error stating that the file is used by another process, then you will have
to reboot) then you have to boot into maintenance mode, usually by
pressing F1 during the boot blob and then selecting a command processor (F2 under Warp 4
and C under Warp 3).
Experienced users may just reboot and ensure that PC/2 does not get started automatically
during reboot (e.g. launched from :hp4.STARTUP.CMD:ehp4. or Startup folder) before the
file has been replaced.
:p.
Assume e.g. that the file :hp4.PC2Hook.DLL:ehp4. should be replaced.
PC/2 will have downloaded the replacement file, either within an archive where instructions
are included or just as a single file with a similar name, e.g. :hp4.PC2Hook.DL_:ehp4..
You just have to make a backup of the original :hp4.PC2Hook.DLL:ehp4. and then replace
it by :hp4.PC2Hook.DL_:ehp4. (e.g. by copying or renaming).
:li.For Additions, you don't need to stop PC/2.
Depending on the file you may just need to read it (e.g. for documentation enhancements),
load it (e.g. for updates to the Popup Menu) or import it (e.g. for Environment Spaces).
:eol.
:p.
Finally some words why I added the Online Automatic Update capability to PC/2.
The major reasons are to allow me easily to ship updates without distributing a complete
package, allow me to inform you about updates so you don't need to check my homepage
regularily (though you are welcome of course ;-) and to see if time has come to use
such a feature (time passes in Web years know and I think it's time for such a technology ;-).
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4131.Help for PC2NET.DLL
:p.PC2NET.DLL contains the support for the Online Automatic Update capability of PC/2.
PC/2 tried to load PC2NET.DLL during its initialization, however it could not be loaded
correctly.
Most likely one of the following problems occurred:
:ul compact.
:li.The version signature of the loaded PC2NET.DLL is incompatible.
:p.This may happen when you install a newer version of PC/2 over or in addition to an
older version of PC/2.
PC2NET.DLL was not replaced successfully or still gets loaded from the path containing
an older version.
Be sure that the installation finished successfully and that an older version of PC/2
is not accessible (e.g. by renaming its directory to something you are sure you have
not included in :hp4.PATH:ehp4., :hp4.LIBPATH:ehp4.,...).
:li.The contents loaded from PC2NET.DLL are invalid.
:p.This should not happen, because it means you are running the correct version of
PC2NET.DLL but its contents are still unusuable.
I can only think of that this can happen when OS/2 is running out of resources
or the PC2NET.DLL module is physically corrupted in which case you may reinstall it
from the original PC/2 package.
:eul.
side is required to make them effective (I don't think doing hidden updates is a good
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4132.Help for creating PC/2's BubbleHelp window
:p.PC/2 has either tried to create or to modify the BubbleHelp window, but this
operation failed.
Most likely you have run out of resources, so close some windows or reboot.
:p.You may continue, though the BubbleHelp window will not be available.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

:h2 res=4152.Help for PC/2 problem determination
:p.PC/2 has detected that :hp9.PC2.TRP:ehp9. has been updated recently (or to be exact since
the last time PC/2 has read it).
This file is used for :link reftype=hd res=4015.<PC/2 hang/trap/bug diagnostics>:elink..
:p.Usually there are 2 reasons why this file gets updated (please browse :hp9.PC2.TRP:ehp9.
to verify what your problem is, note that the most recent entries are at the bottom
of the file):
:ul compact.
:li.An exception (e.g. access violation) which causes PC/2 to terminate abnormally,
that is it closes unintented without being able to save any data.
PC2.TRP logs a hang starting with the line: "Exception Dump Facility".
I'm certainly interested about hearing about such problems as they
are bugs within PC/2 that really should be fixed. I might ask you to
download some stuff that allows me do diagnose the problem in more
detail.
:li.A hang of one or more threads within PC/2.
PC2.TRP logs a hang starting with the line: "Thread Dump Facility".
Most likely, scrolling some lines downwards shows the entrypoint
nearest to the hang with "Nearest    : Ordinal xxxx", where xxxx
is the entrypoint. For me, xxxx usually is 5120 in PMMERGE.DLL,
from reports 5352 seems to be popular too. If you find one of those
2 entrypoints then it is a already :hp2.known problem:ehp2. for
which you don't need to send me your :hp9.PC2.TRP:ehp9. (though I still
don't know what PM is doing there, as both entrypoints are not documented).
:eul.
:p.While the first reason is definitely a programming mistake (bug) inside PC/2, the second
reason seems to be usually caused by some PM APIs that do not return to the caller
(=PC/2) as expected (often caused by applications that hang PM's SIQ (Single Input Queue))..
:p.Anyway, I'm certainly interested in trying to fix as many bugs as possible, but I need
your help, first for telling me the problem and second helping to diagnose the problem
and possibly third to test fixes provided by me.
:p.
So, if you got the message box about reading this help and contacting me I would appreciate
it you doing the following steps to determine the problem:
:ol compact.
:li.Display PC/2's :link reftype=hd res=303.<About Dialog>:elink..
or use the :link reftype=hd res=4015.<BLDLEVEL>:elink.. command to find out
the version of PC/2 you are running.
For example that may tell you that you are running version 2.20.
:li.Load the file :hp9.PC2.TRP:ehp9. into an editor and find the most recent problem
logged (at the bottom of the file) by PC/2.
Most easily this can be done by scrolling to the end of the file and then scrolling upwards
until you find (beginning with 2.19e) a line beginning with :hp4.Exception Dump Facility:ehp4.
(for exceptions) or :hp4.Thread Dump Facility:ehp4. (for thread hangs) which is then
followed by a line :hp4.Version    : PC/2 - Program Commander/2 2.19e (09, 2000):ehp4..
That line gives you the version of the last problem logged.
:li.If the version of PC/2 and the version of the last problem logged match, please
contact me, and if your problem is reproducable (which you can find out by comparing
the problems logged with each other and and find the one where most numbers match)
then :hp2.please:ehp2. contact me.
If the version information does not match, PC/2 has detected a problem from an older
version of PC/2, I would then ask you to wait for the problem to reoccur, however it might
have even be fixed by a later version.
:li.You can find information about contacting me
:link reftype=hd res=303.<here>:elink..
:li.Please do not send me files like :hp9.PC2.TRP:ehp9., :hp9.POPUPLOG.OS2:ehp9. or
:hp9.PDUMP.xxx:ehp9. without asking me prior, in many cases I got those files the
problem was not PC/2 itself but some other applications!
When you have a problem I will tell you what to check to be sure it is a PC/2
problem!
:eol.
:p.
:p.Please refer also to the :link reftype=hd res=4011.<general hints>:elink.
about error messages.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected FAQ help.                                                          *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4003.Help Questions &amp. Answers (part 1)
:p.The following questions and answers is the online help version of
the :hp4.PC2-*.FAQ:ehp4. files.
Because of the size, I had to split it into multiple files.
:p.
:font facename=Courier size=9x5.
:xmp.
******************************************************************************
                        Frequently Asked Questions Part 1/3

                   PC/2 - Program Commander/2 Version 2.20 for
     IBM OS/2 2.x, OS/2 WARP, WARP Srv. SMP, Aurora Presentation Manager
                  Copyright (C) by Roman Stangl December, 2000

                               Version 2.20 Draft
******************************************************************************

Contents:

 1.) How can I configure PC/2 to display the Popup Menu on the Desktop, using
     a Mouse Button 1 or Mouse Button 3 singleclick instead of the default
     doubleclick?
 2.) PC/2 assumes that OS/2 is installed on drive C: , but I use drive H: !
 3.) How do I add DOS Settings correctly?
 4.) How can I add batch files?
 5.) Is HPFS supported?
 6.) How do I start a full-screen WIN-OS2 session?
 7.) How do I control what mode and session type a Windows application runs
     in?
 8.) How do I start a VDM from an image?
 9.) How do I start a VDM that boots from drive A: ?
10.) Why has PC/2 an option to shut down OS/2?
11.) How do I add commandline parameters at runtime?
12.) How do I install PC/2 as a replacement for the WPS?
13.) How do I add WPS Objects to start them via PC/2?
14.) How do I use Drag'n Drop?
15.) How can I add WPS Objects, and what WPS Object classes are allowed?
16.) Does PC/2 change to the root of all drives except for the working dir?
17.) Which versions of OS/2 are supported?
18.) How do I enable the Virtual Desktop and/or Sliding Focus?
19.) How does the sliding Focus work?
20.) How do I use the enabled Virtual Desktop?
21.) How does the Virtual Desktop work?
22.) How do I start applications on the Virtual Desktop?
23.) What's the name of the Desktop?
24.) What's the name of the Window List?
25.) What about seamless WIN-OS2 support, without a corresponding WPS object?
26.) How many applications can I start within a seamless WIN-OS2 session?
27.) How do I define a program's priority?
28.) How do I define a Hotkey to switch to a session?
29.) How does the Hotkey feature work?
30.) Why should I use the Hotkey feature?
31.) I have defined some Hotkeys - now these keys are not available on other
32.) I switched to an application - now some keys don't work as when not 
     running PC/2!
33.) I defined a Hotkey but the wrong window is activated!
34.) How to I define a Hotkey for a certain window (e.g. Drive C object)
     even if it wasn't started from PC/2?
35.) Do I need to completely fill in the Program Installation dialog to
     define a new Hotkey?
36.) I have 2 OS/2 windows with the same name. Will the Hotkey work?
37.) How do I access the Window List on PM?
38.) What is the feature "Button 2 Titlebar click lowers Z-order" useful for?
39.) Can a Hotkey defined for PC/2 itself?
40.) When switching to a Virtual Desktop with a Hotkey, how is the window
     aligned?

Questions, Answers:

 1.) How can I configure PC/2 to display the Popup Menu on the Desktop, using
     a Mouse Button 1 or Mouse Button 3 singleclick instead of the default
     doubleclick?

     First, you may select the Configure Desktop dialog on the Popup Menu to
     select the type of default mouse click action.  This information is
     written to the PC2.INI profile.
     Second, use the commandline option "-SingleClick" or "/SingleClick" to
     overwrite the default behavior read from PC2.INI. If you run PC/2
     as a WPS replacement, you can't use any commandline option, since
     OS/2 seems to dislike commandline options on programs run as the WPS.
     However, this doesn't prevent you from setting the default behavior using
     the Configure Desktop dialog.

 2.) PC/2 assumes that OS/2 is installed on drive C: , but I use drive H: !

     Either start with a blank Menu and add your programs, or use an editor
     such as EPM.EXE to change all " C: " occurrences to " H: " in the
     profile.
     The profile is named PC2.CFG by default.

 3.) How do I add DOS Settings correctly?

     If you wish to do this using your text editor, ensure you use the
     exact same spelling as found in the settings of a typical DOS
     program. They have usually ON/OFF or numbers as possible
     selections. For ON/OFF, you add =1 or =0 to the setting, e.g.
     HW_TIMER=1. For numbers, add =xxxx, e.g. EMS_MEMORY_LIMIT=1024 or
     DOS_VERSION=DOSPROG.EXE,5,0,255. But the really easy way to do this
     is to use the standard DOS Settings dialog, which is displayed when
     you click on the DOS Settings pushbutton.

 4.) How can I add batch files?

     Batch files aren't executable files, so they need a command
     processor to interpret them. Therefore, you have to select a
     command processor, (e.g. CMD.EXE) and add /C filename.bat or /C
     filename.cmd to the parameters entry field. You may wish to look at
     the command reference of CMD.EXE and COMMAND.COM for more details.
     If you use the standard file dialog to find the batch file you want
     to add, PC/2 automatically add this batch file as an argument to the
     command processor.

 5.) Is HPFS supported?

     Yes, PC/2 fully supports HPFS. However, if a filename contains spaces,
     you have to place it in double-quotes, e.g. at a command prompt you
     would type DIR "[d]\OS!2 2.0 Desktop\*" /s to list all files of the
     Desktop directory. The same is true for PC/2, e.g. if you want to start
     a batchfile that conforms to HPFS-filename conventions, you may create
     an entry like this : (Note: Replace [d] with a drive letter, e.g. E: )
        Program Title:      HPFS-Test
        Path and Filename:  [d]\OS!2 2.0 Desktop\Prompts\Command Processor.exe
        Working Directory:  [d]\OS!2 User Applications
        Parameters:         /C ""[d]\OS!2 2.0 Desktop\Run Me.cmd""
     Note: Passing HPFS-filenames to command processors may require two
     double-quotes (e.g. CMD.EXE does). Also take a look at the help pages
     for CMD.EXE to get more details. Open up an OS/2 command line prompt, and
     enter: help cmd.exe - this should get you to the right section.

 6.) How do I start a full-screen WIN-OS2 session?

     The easiest way is to select the Session type of Fullscreen WIN-OS2 and
     specifying the WIN-OS2 executable on the Application notebook page in the
     Program Installation dialog.

     Alternatively create a full-screen DOS command prompt that uses 
     WINOS2.COM as the executable's name. (e.g. [d]\OS2\MDOS\WINOS2\WINOS2.COM
     where [d] is the drive letter you installed WIN-OS2 onto, e.g. C: ). 

     Then add the required DOS Settings (e.g. the following settings greatly 
     improve performance on a 8514/A adapter and probably on other adapters 
     too (thanks to Timothy Sipples for them!):
        VIDEO_8514_XGA_IOTRAP=0
        VIDEO_RETRACE_EMULATION=0
        VIDEO_SWITCH_NOTIFICATION=1
        XMS=64
        EMS=0
        DPMI=4 (or more than 4, if you run a lot of Win apps)
        HW_ROM_TO_RAM=1
        IDLE_SECONDS=2
        IDLE_SENSITIVITY=100
        SESSION_PRIORITY=32 (for WARP only)
     However, modern hardware should be fast enough so that I suggest you to
     start with the following values instead:
        IDLE_SECONDS=1
        IDLE_SENSITIVITY=95
        SESSION_PRIORITY=1 (for WARP only)

 7.) How do I control what mode and session type a Windows application runs
     in?

     Just enter the application's data and select the mode the seamless
     WIN-OS2 session should run. This includes realmode, standard or
     enhanced 386 mode, both in separate sessions or in a common VDM.
     Of course you may select a fullscreen WIN-OS2 session.

     Caution! Sliding Focus does not work well, when running seamless
              WIN-OS2 sessions. To temporarily suspend Sliding, just hold 
              down the SHIFT key while moving the mouse.

 8.) How do I start a VDM from an image?

     First create the image, then create an entry of a fullscreen or windowed
     DOS session. Then add to the DOS settings:
        DOS_STARTUP_DRIVE=[path]DISK.IMG
     where [path]DISK.IMG is the path and filename of the image you want to
     start. Caution! On OS/2 2.0 (2.0 GA) it doesn't work; I don't know why.

 9.) How do I start a VDM that boots from drive A: ?

     Same as item 8.) says, only adjust the DOS settings to:
        DOS_STARTUP_DRIVE=A:
     Caution! On OS/2 2.0 (2.0 GA) the system traps, I don't know why.

10.) Why has PC/2 an option to shut down OS/2?

     Some people use PC/2 as a replacement for the WorkPlace Shell (this saves
     up to 4 MB memory!) and they can't click with button 2 to get the WPS
     Popup Menu, from where shutdown is usually selected. So, it is provided
     on PC/2's Popup Menu. PC/2 requests OS/2 to clear all file buffers. After
     all disk activity has been stopped, it is safe to power off your system.
     PC/2 don't close the programs, but after the disk activity has stopped,
     all processes are frozen (you can not switch to or select programs
     anymore).

     If you set up "Normal Shutdown" in the PC/2 Desktop Menu, when you click
     Shutdown OS/2 on the PC/2 Popup Menu, PC/2 will act just as the WPS does,
     when doing a normal shutdown.  In other words, apps are notified they are
     about to be shut down, and, depending on the application, you may get
     several questions asking something to the effect "Do you really want to
     do this?"

11.) How do I add commandline parameters at runtime?

     E.g. assume you created a Menuentry that starts the editor EPM.EXE.
     After you selected EPM, just before it is started, you can request
     a dialog box from PC/2, where you can enter any commandline parameter
     by adding a [...] construct in the parameters entryfield of the Program
     Installation dialog. This work similar to the same WPS feature.

12.) How do I install PC/2 as a replacement for the WPS?

     First, install PC/2 in the normal way. Second, edit your CONFIG.SYS to
     replace the statement (Note: substitute your drive name instead of
     [d], e.g. C: ):
        SET RUNWORKPLACE=[d]\OS2\PMSHELL.EXE
     to
        REM * SET RUNWORKPLACE=[d]\OS2\PMSHELL.EXE
        SET RUNWORKPLACE=[d]\path\PC2.EXE
     or
        REM * SET RUNWORKPLACE=[d]\OS2\PMSHELL.EXE
        SET RUNWORKPLACE=\path\PC2.EXE

     Where [d]\path is the drive and path (e.g. \PMAPPS\PC2 on drive C: )
     where you installed PC/2. (The second method can be used to add
     PC/2 as the Workplace process of a maintenance partition, e.g.
     created by the BOOTOS2 package). Second, ensure that all PC/2
     *.DLLs are in the directory PC/2 is running from, or in a path
     pointed to by the LIBPATH environment variable in your CONFIG.SYS.

     The next time you boot, the WPS will not be loaded (you can verify this
     by a blank screen displayed instead of the normal Desktop and objects.

     Not using the WPS saves memory and reduces boot time! However, you will
     not be able to change your system settings you are able when running the
     WPS. Also, if you install applications that create WPS object, these
     objects won't be installed and the installation may fail.

     Of course, you can also start the WPS later, either by starting
     PMSHELL.EXE from a command prompt or from a PMSHELL Menuitem of PC/2.
     You can follow the WPS initialization by heavy disk access and buildup
     of your Desktop with objects. Once the WPS is running again, you can
     change your system settings and installation programs will be able 
     to create WPS objects.
 
     To allow you to use the Popup Menu on the newly (re)started WPS, PC/2
     will detect this change immediately if you have correctly specified the
     name of the WPS (e.g. Desktop, Arbeitsoberfl„che,...). PC/2 displays the 
     Window List on PM additionally, just as the WPS does by clicking mouse 
     buttons 1 and 2 together.

13.) How do I add WPS Objects to start them via PC/2?

     The important thing is, that you have to know the WPS Object ID
     (Identifier) to be able to add them to PC/2. For preinstalled Objects,
     you can find the ID in the file [d]\OS2\OS2_20.RC (where [d] is the
     drive you installed OS/2 onto, e.g. C: ).
     Alternatively, you may use WPS 2 PC/2 to extract the Object ID from
     WPS Objects not being Folders or Program Objects, when available.
     The ID of Objects, created as outlined above, may than be added in the
     entryfield Path and Filename in the Program Installation dialog, with
     the Program Type WPS Object.
     Note that this function is not supported when PC/2 is used as the
     replacement for the WPS, as it is the WPS that services starting
     the Object ID's.

14.) How do I use Drag'n Drop?

     In addition to the standard file dialog, you may use Drag'n Drop to
     add applications in the Program Installation dialog. E.g. you may
     use a drive object, select one (only 1!) application, and then drag
     it into the dialog and drop it onto any area that accepts it (any area
     that isn't covered by text, buttons, entryfields,...).
     You can also drag one WPS Folder Object onto the first page of the
     Program Installation dialog, to open it when selecting it from the
     Popup Menu.
     PC/2 will try to determine the type of the application you added
     automatically (programs or folders), but you can still change it.

15.) How can I add WPS Objects, and what WPS Object classes are allowed?

     The simplest way is to use WPS 2 PC/2. Just drag the WPS Objects you
     want to add to PC/2's Popup Menu, onto the WPS *icon* of WPS 2 PC/2
     (Not the open WPS 2 PC/2 application).
     Then open the WPS 2 PC/2 application window by clicking on the icon
     mentioned before. Now drag the status bitmap (the full suitcase) from
     WPS 2 PC/2 onto the Setup dialog of PC/2, and voila, for each WPS
     Object extracted, a new Menuentry is created within PC/2's Popup Menu.
     WPS 2 PC/2 can extract data from WPS Objects of classes WPProgram,
     WPFolder, any WPS Object that has an Object ID defined, or WPShadow
     Objects of the classes mentioned previously.
     E.g. you can't extract your WPS Printer Object, or a Drive Object,
     unless an Object ID is assigned to these Objects.
     Note: When running PC/2 as a replacement shell for the WPS, these
     functions are inoperative, as they require the WPS to service them.

16.) Does PC/2 change to the root of all drives except for the working dir?

     Yes, PC/2 changes to the root of all non-removable drives before the
     selected application is started.

17.) Which versions of OS/2 are supported?

     PC/2 was developed under OS/2 2.1 GA, 2.11 GA, and WARP GA, but I would
     expect it to work under OS/2 2.0 and any Service Level too.
     Under OS/2 WARP for Windows, using a tiled background bitmap hangs OS/2
     solidly, requiring a reboot.
     Under OS/2 WARP Fullpack, using a tiled background bitmap just hangs
     PC/2, so in this case, just use CTRL+ESC to terminate PC/2. A defect
     (PMR 4X472) has been opened against OS/2 WARP, so a future CSD
     (Corrective Service Diskette) will hopefully repair this defect.
     This bug is not a PC/2 problem, since selecting a tiled background
     bitmap works correctly under OS/2 2.x.

     Unfortunately this bug has not been removed for OS/2 WARP 4 also.

18.) How do I enable the Virtual Desktop and/or Sliding Focus?

     Start PC/2, click on the Desktop and select SETUP Desktop. A dialog box
     will appear, where you can enable these features. Just press help for
     further information.
     To temporarily suspend the Sliding feature, just hold down the SHIFT key
     while moving the mouse.

     Caution! Sliding Focus does not work well, when running seamless
              WIN-OS2 sessions. To temporarily suspend Sliding, just hold 
              down the SHIFT key while moving the mouse.

19.) How does the sliding Focus work?

     For all pointer movements, OS/2 generates messages that contain the
     window where the message originated. PC/2 hooks into these messages
     and determines the active window. If the active window is not the
     window that originated the message, it makes the message originating
     window active.
     Of course, PC/2's window, the Desktop and the window list are treated
     differently. To preserve the Z-order of the windows, also check this
     option.
     Caution! Sliding Focus does not work well, when running seamless
              WIN-OS2 sessions. To temporarily suspend Sliding, just hold 
              down the SHIFT key while moving the mouse.

20.) How do I use the enabled Virtual Desktop?

     You may know that a VGA-display has usually 640 columns and 480
     rows (numbered from 0...639 and 0...479). If you position your mouse
     pointer over the surrounding rows and/or columns of your display
     (here columns 0 or 639 and/or rows 0 or 479) PC/2 SHIFTs all windows
     that exist on your system (with some exceptions like window list) in
     the opposite direction. Some windows may slide out, some may slide in
     from the other direction.
     You may click on any Virtual Desktop on the overview Window to
     immediately switch to that Virtual Desktop.

21.) How does the Virtual Desktop work?

     First, for all windows, OS/2 tracks their coordinates in data structures.
     PC/2 queries and adjusts them according to the mouse movements the
     user makes. For instance, if the mouse pointer is over column 0 on the
     screen, PC/2 adds to the x-position of all windows for example 50 % of
     the x-dimension of the screen. The windows appear to move rightward (out
     of the display) because the display SHIFTs leftward within the Virtual
     Desktop. Because windows are usually aligned on byte-boundaries, they
     may not appear on their original place if you SHIFT one unit in one
     direction and one unit in the opposite direction.

22.) How do I start applications on the Virtual Desktop?

     In the Program Installation dialog, you can define the size and
     position that the application should use. For instance, you can enter
     a position of -200 in X, which would position the application to the left
     of your Physical Desktop.  All coordinates are in relation to your
     Physical Desktop.  In other words, an x setting of -1 would position your
     application 1 pixel (out of a possible 640 in VGA) to the left off of your
     Physical Desktop.

     PC/2 forces the window to the position you entered, however the
     application may determine its size itself. To allow PC/2 to find the
     window, ensure that you enter the applications Window List entry or
     titlebar text to the Window Name entryfield on the Hotkey notebook page
     of the Program Installation dialog. You don't need to enter the complete
     Window List or titlebar text or window class name, just any substring that 
     matches with the text. Use the Spy notebook page in the Desktop dialog to
     determine this data easily.

     *However*, you must ensure that this substring matches *only* with the
     application you want. Results are unpredictable if multiple applications
     contain the same substring. For example you may find that a window is not
     kept on every Virtual Desktop if you would like to, or a window may be 
     kept on every Virtual Desktop while you can't explain why.

     Some applications are "stubborn", that is, no matter what you do to
     specify your xy coordinate position for the application, it refuses to
     obey, and starts where it wishes.  For instance, a OS/2 or DOS windowed 
     session that changes from 80x25 line mode to 80x28 line mode during 
     startup causes just such a condition (even if they would honor the 
     requested position when no mode change is done).

23.) What's the name of the Desktop?

     OS/2 2.0 introduced the name "OS/2 2.0 Desktop" for the Desktop window,
     which normally is the WPS. OS/2 2.1 and OS/2 WARP  use the name
     "Desktop" instead, if you install it over a newly formatted system.

     OS/2 2.1 and WARP may use a different name, if you install it over a
     previous OS/2 system, so please read the README file provided with your
     version of OS/2 carefully.

     If you use a non English OS/2 version, you have to translate the Desktop
     window's name to your national language, for example you would enter
     "Arbeitsoberfl„che" when running a German OS/2. You may extract the
     exact spelling from either the Window List (where the Desktop is one of
     the running programs) or from OS/2's User's Guide.
     PC/2 requires the exact spelling of the Desktop window's name to locate
     the WPS process under all running processes.
     
     Most easily, just use the Spy notebook page to find out the names of the
     Desktop (WPS) and Window List!

24.) What's the name of the Window List?

     The Window List (known as Task List in OS/2 1.x) is just named
     "Window List".

     If you use a non English OS/2 version, you have to translate the Window
     List's name to your national language, for example you would enter
     "Fensterliste" when running a German OS/2. You may extract the extract
     exact spelling from either the Window List itself or from OS/2's User's
     Guide.
     PC/2 requires the exact spelling of the Window List's name to locate the
     Window List under all applications.
     
     Most easily, just use the Spy notebook page to find out the names of the
     Desktop (WPS) and Window List!

25.) What about seamless WIN-OS2 support, without a corresponding WPS object?

     PC/2 allows you to start seamless WIN-OS2 sessions, as separate sessions
     or in a common VDM. This means, if you start a Windows application in a
     separate session, a complete copy of the WIN-OS2 environment will come up.
     This wastes memory, so I suggest to start Windows applications into a
     common VDM whenever possible.

     This seamless WIN-OS2 support works also without the WPS installed,
     under OS/2 2.1 GA with no problems, but OS/2 2.0 GA hangs during load.
     So under OS/2 2.0 GA, only start seamless WIN-OS2 sessions if the WPS is
     running.

     Under OS/2 2.0 GA you can only successfully start WIN-OS2 sessions in
     standard mode. 386 enhanced mode is not supported by OS/2, and real mode
     starts to load, but doesn't complete successfully for unknown reasons.
     Under OS/2 2.1 and WARP, real mode is not supported, so you can only start
     Standard mode and 386 Enhanced mode sessions.

26.) How many applications can I start within a seamless WIN-OS2 session?

     For a separate session usually only 1, but you may start the Windows
     Program Manager as the first, and any Windows applications as a
     commandline parameter, which the Program Manager will start for you.
     If you start the Program Manager, you can of course start any Windows
     application from it, but make sure that you have enough storage defined
     in the DOS Settings (usually DPMI_MEMORY_LIMIT=16 or so).

     For separate sessions, the limit is determined by your system
     configuration (e.g. how much memory you have in your kingdom. <smile> ).

27.) How do I define a program's priority?

     Just select the last notebook page from the Program Installation dialog.
     Warning - changing a program's priority may have severe impacts on your
     system's performance, giving all CPU time to the application.

     You may set a tape backup or image conversion program to a lower priority
     or a real-time data acquisition to a higher priority.

     For DOS sessions (and WIN-OS2 sessions are DOS sessions), the priority
     can't be changed, because this would lock PM for unknown reasons.

     You can't change the priority of WPS Object either, because the WPS
     actually starts them after a request from PC/2.

     On OS/2 Warp, you can adjust the priority of DOS sessions via the DOS
     Settings dialog, in the Settings Notebook.

28.) How do I define a Hotkey to switch to a session?

     Just select notebook page 4 from the Program Installation dialog.
     You may define any key from the pool keys 0 to 9 and A to Z combined with
     either with the RCTRL (right CTRL) or the CTRL (both the left and right 
     one) or the ALT or CTRL+ALT or the left WIN95 key. Once a Hotkey is defined 
     for an application, it is no more available for other sessions. You may 
     define Hotkeys for 180 different sessions as the maximum.

     In the Desktop Configuration notebook you may define the Hotkey to be
     detected systemwide (even if the focus window has an accelerator key
     defined for the Hotkey) or only when PC/2, the WPS or the Window List has
     the focus.

     If PC/2 detects that the user pressed a Hotkey and the application the
     Hotkey is defined for is not running, PC/2 launches the application,
     otherwise, PC/2 will jump to the application already running.

     You may temporarily suspend the Hotkey feature by pressing and holding
     the SHIFT key while pressing the Hotkey. PC/2 will then ignore the
     Hotkey and pass it over to the application that currently has the focus.

29.) How does the Hotkey feature work?

     PC/2 knows which Hotkeys are used if you have defined them. However, PC/2
     also needs to know at least a part of the application's titlebar text or
     Window List entry to switch to, so you have to fill in the entryfield
     "Window Name: " (on the Hotkey notebook page) with the session's titlebar
     text or Window List entry.

     For a session's Window Name either use part of the string that appears
     in the session's titlebar, or part of the string that appears in the
     Window List for this session.  This is most important that you follow
     this procedure, otherwise results become unpredictable.

     If you press the Hotkey for a session, PC/2 searches all titlebars
     and the Window List for the text from the "Window Name" entry and
     switches to that session when a match was found.
     PC/2 not only switches to the session the Hotkey is defined for, but
     also switches to the Virtual Desktop the session resides on.
     If neither the Window Name can be found on any window's titlebar nor
     in the Window List, PC/2 assumes the application is not running and
     launches it. This is why it is important that you ensure your entry in
     the "Window Name" entryfield accurately relates to the titlebar name of
     the running session's Window List entry.

     You may temporarily suspend Hotkey detection, if you press the SHIFT key
     simultaneously with the Hotkey. PC/2 then will *not* steal the Hotkey,
     but passes it on the the window that had the focus while the keypress.

30.) Why should I use the Hotkey feature?

     Well, you don't need to use it, but you may find it very useful to
     quickly switch to a certain session, for example a host terminal
     emulator and than switch back to a compiling environment residing
     on different Virtual Desktops.

31.) I have defined some Hotkeys - now these keys are not available on other
     sessions any more!

     Once you have defined a Hotkey, this Hotkey will be "seen" by PC/2
     when it is pressed, because PC/2 "steals" them, before the application,
     the key was pressed in, gets the notification for that key. Hotkeys are
     detected systemwide, unless you check the option "No systemwide Hotkeys".

     You may allow an application "see" the Hotkey, if you have pressed the
     SHIFT key simultaneously with the Hotkey.

     For example, the EPM editor has defined an accelerator key for CTRL+U,
     to invoke its undo function. You can define CTRL+U as a Hotkey for your
     terminal emulator, but EPM will no longer see this key then (and not 
     showing the undo window). To allow EPM to see CTRL+U then, you have to 
     press and hold the SHIFT key in addition to the CTRL key while pressing
     the U key.

32.) I switched to an application - now some keys don't work as when not 
     running PC/2!

     You have probably defined a systemwide Hotkey in PC/2 for that key. The
     Hotkey will not be "seen" by your application, unless you press the 
     SHIFT key while pressing the Hotkey, as described in 31.).

     A nice trick is to define the Hotkey together with keys usually not used
     in applications (like e.g. the Left WIN-Key) or a combination that can
     be invoked by 2 different keys (like e.g. the CTRL-Key where you have a
     left and a right one).

     For example, under Netscape CTRL+N opens a new window, so If you had
     defined a Hotkey CTRL+N under PC/2 that key would have been not available
     to Netscape. However if you had defined RCTRL+N as a PC/2 Hotkey then
     you would still be able to use CTRL+N within Netscape as long as you were
     using just the left CTRL-Key. That allows you to use CTRL+N for both,
     Netscape (with the left CTRL-Key) and for PC/2 (with the right CTRL-Key).

33.) I defined a Hotkey but the wrong window is activated!

     You have probably defined a Hotkey for an application and have
     inadvertantly started more than one copy of this same application. PC/2
     only switches to the first application window, and ignores all other
     sessions containing copies of the same hotkeyed application.

     Be sure that you don't start more than one instance of a session a
     Hotkey is defined for. A session is a PM, windowed OS/2 or DOS
     application that displays at least one window on PM (Presentation
     Manager).

34.) How to I define a Hotkey for a certain window (e.g. Drive C object)
     even if it wasn't started from PC/2?

     Assume that you have opened the Drive C - Tree View object from the
     OS/2 System->Drives object of the WPS and you want to define ALT+C
     as its Hotkey.
     Create a new Menuentry and skip to the Hotkey notebook page of the
     Program Installation dialog. Enable the Hotkey feature and select
     ALT+C as the Hotkey.
     Then select the entry of the Drive C object from the Current Window
     List MLE and doubleclick on it. The selected entry will also be
     filled into the entryfield Window Name. Now save the changes and
     voila, you can now use the newly defined Hotkey.

     As the Drive objects are WPS Objects too, Drive objects can only be
     opened while the WPS is running.

35.) Do I need to completely fill in the Program Installation dialog to
     define a new Hotkey?

     No, you just need to fill in the Hotkey labeled notebook page. PC/2
     uses the text entered in the entryfield Window Name to compare with
     all windows' titlebars and Window List entries. If the text matches
     any of these entries, (or subset of the entry) the window the entry is
     defined for is activated.

     To use the Hotkey feature, just ensure that the text entered in the
     entryfield Window Name is part of any window's titlebar or Window List
     entry.

36.) I have 2 OS/2 windows with the same name. Will the Hotkey work?

     Yes, but only for the first window whose titlebar or Window List entry
     matches to the Menuitem Window Name. You have to ensure a unique title
     for a window to allow PC/2 to exactly identify a window and activate it
     through the Hotkey function.

37.) How do I access the Window List on PM?

     If you are positioned over the PM with the mouse pointer, just press
     and hold down a mouse button and then click the other while not moving
     the mouse pointer. The Window List will appear with the mouse pointer
     in center (when the complete Window List can displayed then).
     The PM is the bottommost window which you can see as your Desktop's
     background, if either you don't run the WPS or you moved the WPS to
     an other Virtual Desktop.

     Of course pressing CTRL+ESC works too.

38.) What is the feature "Button 2 Titlebar click lowers Z-order" useful for?

     Even if you don't use the Virtual Desktop feature, you may find that 
     sometimes you like to switch to a window which is under the currently 
     active one, but you don't want to move or minimize the active one to get
     the other window to the top. If this option is on, it enables you to 
     just click mouse button 2 on the active window's titlebar to push this 
     window to the bottom. The window previously behind the active window now
     becomes the topmost window. Note that with this function enabled, you 
     can't activate window movement with mouse button 2 anymore - however you
     can still accomplish this using mouse button 1.

     Pressing the CTRL key while clicking on the titlebar only moves the
     window to bottom; pressing the Alt key activates the default action
     of the titlebar (usually activating the move window tracking frame).

     Therefore, the CTRL and Alt key may be temporarily used to suspend
     the default PC/2 behavior.

39.) Can a Hotkey defined for PC/2 itself?

     Yes, just enter PC/2 as the string to match on the Hotkey notebook
     page in the Program Installation dialog and assign a Hotkey e.g.
     ALT+P.

40.) When switching to a Virtual Desktop with a Hotkey, how is the window
     aligned?

     PC/2 moves the Desktop on the Virtual Desktop at the rate you defined
     with the Virtual Desktop Scrolling scrollbar on Desktop Setup dialog.
     The movement stops either when the middle point of a window becomes
     visible on the screen the first time, or a surrounding edge was hit.
     If you set the Virtual Desktop Scrolling to 100 % you will probably see
     the best results.

--->>>  --->>>  --->>> Please also read the second part <<<---  <<<---  <<<---

:exmp.
:font facename=default size=0x0.
:p.:link reftype=hd res=4011.<Backward>:elink.
                                :link reftype=hd res=4004.<Forward>:elink.

:h1 res=4004.Help Questions &amp. Answers (part 2)
:p.
:font facename=Courier size=9x5.
:xmp.
******************************************************************************
                        Frequently Asked Questions Part 2/3

                   PC/2 - Program Commander/2 Version 2.20 for
     IBM OS/2 2.x, OS/2 WARP, WARP Srv. SMP, Aurora Presentation Manager
                  Copyright (C) by Roman Stangl December, 2000

                               Version 2.20 Draft
******************************************************************************

Contents:

41.) How do I activate and use the PC/2 Spooler Control Window?
42.) Why does the Overview Window sometimes not correspond to the real Desktop
     application mix?
43.) I have configured a Popup Menu, but always get the Setup dialog when
     clicking on the Desktop.
44.) How can I change the default fonts used by PC/2?
45.) What is the addon WPS 2 PC/2 used for?
46.) How can I open WPS Folder Objects from PC/2's Popup Menu?
47.) What is the difference between the "slow" and "fast" version of PC/2.
48.) Why does an installation update of PC/2 not work with active CM/2?
49.) Why does selection of a tiled background bitmap hang OS/2 Warp 3.0?
50.) Do I have to change anything in CONFIG.SYS to run PC/2?
51.) I have defined a Hotkey, but now my favorite program doesn't get this
     key!
52.) What is referred as an *accelerator* key?
53.) Which sequence is used by PC/2 when autostarting programs?
54.) Are duplicate Menuentries allowed?
55.) What's the difference between the Program Title and Window Name?
56.) How can I use a Hotkey that starts a program, each time the Hotkey is
     pressed instead of switching to the running instance?
57.) Can Mouse Button 3 be used instead of Mouse Button 1 to activate the
     Popup Menu or to switch between Virtual Desktops?
58.) What does "Advanced Marking" mean?
59.) If running PC/2 as the WorkPlace Shell process, how can I launch WPS
     Objects or change system settings (as e.g. the background color)?
60.) I've activated Advanced Marking, now both mouse buttons 2 seem no longer
     to work as expected for IBM Personal Communications/3270, IBM
     Communications Manager/2, Advantis PassPort/2 or EPM.
61.) Does PC/2 also support OS/2 half-packs (OS/2 running on top of
     MS Windows)?
62.) I'm running PC/2 as the WorkPlace Shell process, how can I change PM
     settings (my system configuration), e.g. the PM background color?
63.) Advanced Marking doesn't paste at the correct location sometimes, what
     am I doing wrong?
64.) Are UNC Names supported?
65.) I have multiple partitions with (different versions of) OS/2
66.) I have enabled the feature "Button 2 Border click moves window", but I
     can't move some windows, what am I doing wrong?
67.) PC/2 used to work perfectly, but now it hangs OS/2 when starting!
68.) Can PC/2 help my server to recycle at midnight for example to get rid of
     resource leaks or hanging programs?
69.) When running PC/2 the focus change when closing windows behaves peculiar!
70.) I'm experiencing unexpected hangups/traps now that I've enabled the
     Titlebar Smarticons.

Questions, Answers:

41.) How do I activate and use the PC/2 Spooler Control Window?

     To be able to select the PC/2 Spooler, just add a the PC/2 Spooler via
     the Control Addition dialog to your Popup Menu. Then select the newly
     created item to show the PC/2 Spooler.
     The PC/2 Spooler is a container where you can manipulate the printer
     queues and print jobs of your local machine by pressing mouse button 2
     on any row you want to apply an action. A Popup Menu occurs, which you
     can use to select actions.

     Note that when running PC/2 as the WPS replacement, the PC/2 Spooler
     Control Window is the only way you can control your queues and jobs.
     Even more, without the PC/2 Spooler Control Window, the OS/2 spooler
     would not even be started!

42.) Why does the Overview Window sometimes not correspond to the real Desktop
     application mix?

     The Overview Window does its updating by event triggering.

     This means that an event (creating, destroying, sizing or moving a
     window) is the prerequisite for all windows available on the Desktop to
     get shown on the Overview Window.

     For example, because many events are generated when a Window is created,
     PC/2 ignores all events during working on the first event.
     So when PC/2 is drawing a window on the Overview Window as a result of a
     window creation, and another window is created (but doesn't trigger any
     action by PC/2 because PC/2 is already busy with the previous event) it
     will not be displayed on the Overview Window.

     To compensate for this, PC/2 redraws all windows again, when another
     event is triggered. However, even then, there is a low possibility that
     a window doesn't get recognized by PC/2.

     When the system is idle, no event triggers PC/2 actions and PC/2 is
     therefore idle too.

     PC/2 versions previous to 1.80 took another approach. They always drew
     the windows available, loading the CPU to a permanent 15 % even when
     the system was doing nothing. I am happy to say, this is no longer the
     case.

43.) I have configured a Popup Menu, but always get the Setup dialog when
     clicking on the Desktop.

     To display the configured Popup Menu, the Menu *must* contain the entry
     "Configure Desktop", otherwise, the Setup dialog will be displayed.

     To add the Menuentry "Configure Desktop", select the pushbutton
     "Add Control" from the Setup dialog. Then select "Configure Popup Menu
     Settings" to add the entry "Configure Desktop" to your Popup Menu.
     Now you will get the Popup Menu when clicking mouse button 1 or mouse
     button 3 on the Desktop.

44.) How can I change the default fonts used by PC/2?

     You can change the font of the Popup Menu, the Spooler Window and the
     Overview window by simply dragging a font from the font palette:

     +) to change the Popup Menu's font, drop the font onto the Smarticonbar
        on PC/2's Overview Window
     +) to change the Spooler Window Container's font, drop the font onto the
        container
     +) to change the Overview Window's font, drop the font onto the Overview
        Window (all fonts will be accepted, though some fonts produce not
        exactly the requested result).
     +) to change the SessionBar Window's font, drop the font onto the 
        SessionBar.
     +) to change the QuickSwitch Window's font, activate the QuickSwitch
        window drop the font onto it while it is still displayed. 

45.) What is the addon WPS 2 PC/2 used for?

     WPS 2 PC/2 is a WPS Object, residing in WPS2PC2.DLL. This DLL must reside
     in a directory that is accessible through the LIBPATH environment
     variable in your CONFIG.SYS.

     When WPS 2 PC/2 is installed and registered to the WPS, you can extract
     settings from WPS Program and Folder objects to be used within PC/2's
     Popup Menu.

     To extract the data from WPS Objects, just select the and drag them
     over to WPS 2 PC/2's closed icon on your Desktop, and drop them to let
     WPS 2 PC/2 extract their contents.

     To add extracted WPS Object data to PC/2's Popup Menu, open the
     WPS 2 PC/2 Object and PC/2's Setup dialog, and drag the Status bitmap
     (Full suitcase) from WPS 2 PC/2's window onto the Submenu/Program
     Configuration listbox on PC/2's Setup dialog.

     Menuitems corresponding to the settings of the WPS Objects you extracted
     will be added at the bottom of the current level of the Popup Menu (you
     probably will notice that the listbox gets filled with the top-level
     WPS Objects converted to Popup Menu entries).
     From now on, you can use exactly the same settings of your WPS Objects
     from PC/2's Popup Menu without entering them twice (even DOS settings
     will be extracted).

46.) How can I open WPS Folder Objects from PC/2's Popup Menu?

     Just create a Program Menuitem by selecting Add Program from PC/2's
     Setup dialog. Then drag one WPS Folder Object on PC/2's Program
     Installation dialog (and replace the Program Title field for better
     readability) - that's all.

47.) What is the difference between the "slow" and "fast" version of PC/2.

     With version 1.90, PC/2 slightly changed the way how the Overview window
     is updated.

     Up to version 1.80, PC/2 used to updated the Overview window only on
     the screen, causing some flicker. This approach is called "slow" because
     it gives the best performance for video chipsets, that have a small
     performance of moving data from main memory to video memory (e.g. the
     8514/A adapter because of its IO-mapped design). This version is called
     PC2SLOW.EXE (in PC/2 1.90) and can either be renamed to PC2.EXE or you 
     change the settings of PC/2's WPS Object.

     Beginning with version 1.90, PC/2 defaults to draw most changes of
     the Overview window into memory first, and just copies it onto the
     screen, when the drawing has finished, eliminating most flicker.
     This approach is called "fast" because for modern video chipsets
     (e.g. XGA, S3) there is no performance impact of moving data from
     main memory to video memory. This version requires a slightly more
     memory). Beginning with version 2.00, only the "fast" method is included,
     as this fits best to modern video hardware. Hovever, you can compile PC/2
     if you need the "slow" method.

48.) Why does an installation update of PC/2 not work with active CM/2?

     IBM Communication Manager/2 locks the DLL PC2HOOK.DLL when running,
     therefore you have to stop and shut down CM/2 prior to run an update of
     PC/2.
     This behavior of CM/2 is not expected and I can't explain it -
     but there are potentially more programs out having this behavior. So
     I suggest you shut down or not start applications prior to running an
     installation update of PC/2, if you experience problems installing PC/2.

49.) Why does selection of a tiled background bitmap hang OS/2 Warp 3.0?

     OS/2 WARP has a bug, when requesting display of a tiled background
     bitmap. It even behaves a little different between OS/2 WARP and OS/2
     WARP Fullpack. A defect (PMR 4X472) has been opened against OS/2 WARP,
     so a future CSD (Corrective Service Diskette) will hopefully repair this
     defect.
     This bug is not a PC/2 problem, since selecting a tiled background
     bitmap works correctly under OS/2 2.x.

     During beta tests of OS/2 WARP 3 and 4, I informed OS/2 development a few
     times about this bug (on an IBM internal forum, and via a note), but I
     have not yet heard back yet, in time for the cutoff date of this release.

50.) Do I have to change anything in CONFIG.SYS to run PC/2?

     No, PC/2 is so smart that it can locate all its files (the DLLs, the
     online HLP and configuration files) in the directory PC/2 was started
     from! This also holds true then using PC/2 as the WPS replacement.

     In contrast, many other programs require modifications in your CONFIG.SYS
     or that you copy some files in certain directories.

     Just WPS 2 PC/2 requires its DLL and HLP file in the directory \OS2\DLL
     and \OS2\HELP, because the WPS requires them in a path pointed to by the
     LIBPATH and HELP environment variables. Both files will be copied by the
     installation automatically.

     You may experience an error during startup of PC/2 that the online help
     can't be initialized. Most probably you are simply missing to include the
     current directory (.;) in the path pointed to by the HELP environment
     variable.

51.) I have defined a Hotkey, but now my favorite program doesn't get this
     key!

     When you have defined a Hotkey, PC/2 "sees" that Hotkey when any PM
     window (PM programs, OS/2 and DOS windows) has the focus unless:

     *) you have selected the checkbox "No systemwide Hotkeys" on the Desktop
        dialog notebook - using this option, PC/2 detects Hotkeys only when
        either PC/2, the Window List, or the WPS are the active tasks when
        pressing the Hotkey

     or

     *) the key is captured by OS/2 or a device driver (e.g. CTRL+ESC)

     or

     *) you have launched another program (after PC/2) that also detects keys
        systemwide.

52.) What is referred as an *accelerator* key?

     An accelerator is a special OS/2 PM (Presentation Manager) resource, that,
     when pressed, informs the window having the focus not by the key's ASCII
     character code, but by a special message.

     If a key is not captured by OS/2 (or a device driver) itself, PC/2 will
     "see" that accelerator key instead of an application when a systemwide
     Hotkey is defined for that key within PC/2, unless the SHIFT key is
     pressed simultaneously.

     For example, the EPM editor has defined an accelerator key for ALT+S,
     to invoke its split line function. You can define ALT+S as a systemwide
     Hotkey to launch PMSEEK, then EPM will no longer see this key, unless you
     have pressed the SHIFT key while pressing the Hotkey.

53.) Which sequence is used by PC/2 when autostarting programs?

     PC/2 uses a recursive depth-first search, when searching the Popup Menu
     for applications marked to be autostarted. For example, assume your Popup
     Menu looks like the following graphics (tree view):

                           +----+     Px ... x'th Program Menuentry, not
                        +--| A2 |                 not having autostart flag
                        |  +----+                 set
                        |  | P1 |
                +----+  |  +----+     Sy ... y'th Submenu Menuentry
             +--| A1 |  |  | A3 |
             |  +----+  |  +----+     Az ... z'th Program Menuentry, having
             |  | S2 |--+                         autostart flag set
     +----+  |  +----+     +----+
     | S1 !--+  | S3 |-----| P2 |
     +----+     +----+     +----+
     | A5 |                | P3 |
     +----+     +----+     +----+
     | S4 |-----| P4 |     | A4 |
     +----+     +----+     +----+
                | P5 |
                +----+
                | A6 |
                +----+

     \----/     \----/     \----/
      root      first      second
     level     Submenu    Submenu
                level      level

     Having above Popup Menu, invoking PC/2 would autostart applications in
     the following order:
     A1 --> A2 --> A3 --> A4 --> A5 --> A6

     To define a certain sequence for your applications to autostart, you have
     to position their Menuentries according to above graphics.

54.) Are duplicate Menuentries allowed?

     It is possible to have multiple Menuentries for the same applications,
     but I *strongly* recommend creation of just *one* Menuentry for a single
     application.
     PC/2 currently does *not* support a concept of "shadow Menuentries",
     behaving similar to WPS Shadowobjects.

55.) What's the difference between the Program Title and Window Name?

     The Program Title is the text displayed for an application's Menuentry in
     PC/2's Popup Menu. Some applications (e.g. OS/2 and DOS windows) even use
     this text in their titlebar, but mode applications (e.g. EPM) choose to
     set their titlebar text according to their own preferences.

     The Window Name is the text used by PC/2 to identify one certain PM
     window among all windows on PM. To identify a certain window, PC/2 uses
     the Window Name text and matches it with all windows on PM. A window is
     identified when the Menuentry's Window Name matches either the window's
     titlebar *or* the window's Window List entry. The first window of all
     windows on PM which matches with the Menuentry's Window Name is assumed
     as the window launched by this Menuentry by PC/2.

     If you choose not to fill the Window Name entryfield, PC/2 just copies
     the contents of the Program Title entryfield into Window Name.

     Most easily, just use the Spy notebook page to find out the names of the
     Desktop (WPS) and Window List or any other window you are interested in!

56.) How can I use a Hotkey that starts a program, each time the Hotkey is
     pressed instead of switching to the running instance?

     Just ensure that the program's Window Name entryfield contains a text,
     that never occurs on any other window's titlebar *or* Window List entry
     *or* window class name.

     For example I prefer CTRL+O to start an OS/2 window each time I press
     CTRL+O. I do so by entering the following data for this OS/2 window:
        Program Title:      OS/2 Window
        Window Name:        HK OS/2 Window
     Under the assumption that now other window on PM contains the string
     "HK OS/2 Window", CTRL+O will always start a new OS/2 window, but switch
     to the running one (if none is running, one is started of course).
     However, the Hotkey will not switch to a window names "OS/2 Window",
     because the string "HK OS/2 Window" is no substring of "OS/2 Window".

     Of course, if I for example would invoke EPM "HK OS/2 Window", then my
     OS/2 Window's Window Name "HK OS/2 Window" would be successfully matched
     with this EPM session when pressing CTRL+O, and the Hotkey would switch
     to this EPM session instead of starting another OS/2 window.

     Just ensure that it is very unlikely that any PM window contains the
     Window Name in its titlebar *or* Window List entry *or* window class, to
     start a new instances of the program each time the Hotkey is pressed.

     To find out a window's class name, you may use the Spy notebook page of
     the Desktop Dialog.

57.) Can Mouse Button 3 be used instead of Mouse Button 1 to activate the
     Popup Menu or to switch between Virtual Desktops?

     Yes, you may use the Desktop Configuration dialog to enable the
     activation of the Popup Menu and to switch between Virtual Desktops by
     clicking on surrounding rows or columns for clicking with mouse button 3.

     You may even select that the Popup Menu appears when clicking anywhere
     on the screen. This makes of course only sense when using mouse button 3,
     because mouse button 1 is used by OS/2 applications themselves.

58.) What does "Advanced Marking" mean?

     Advanced Marking is a feature of PC/2, that enables you to mark text,
     copy it to and paste it from the OS/2 keyboard just by mouse clicks.

     Advanced Marking works for static text fields, buttons, entryfields, 
     multiline entryfields, for OS/2 and DOS windows, E editor, EPM, IBM 
     Personal Communications/3270, Advantis PassPort/2, IBM Communications 
     Manager/2, Netscape Navigator/2 and Lotus Notes windows. 
     When enabling Advanced Marking you will never have to select Mark, Copy 
     or Paste from a DOS or OS/2 window's system menu again!

     To activate Advanced Marking, just mark the text you want to paste into
     OS/2's clipboard by pressing and holding mouse button 1 while selecting
     the text (you may already know this from (multiline)entryfields).
     Release mouse button 1 whenever you have selected the text you want to
     mark and copy into the clipboard. PC/2's Advanced Marking feature will
     automatically paste the selected text into OS/2's clipboard.
     To paste text from the OS/2 clipboard just doubleclick with mouse button
     2 (mouse button 1 is already reserved by OS/2) - that's all.

     If you want to temporarily suspend Advance Marking, just press and hold
     one of the SHIFT keys while copying or pasting.

     If you want to mark text without PC/2 automatically copying into the
     clipboard, just press and hold the ALT key while selecting the text
     (Note: IBM Communications Manager/2 will not allow you to move the mouse
     pointer while pressing a key).

     If you paste text from the OS/2 clipboard into a OS/2 or DOS windowed
     command prompt, trailing \r and/or \n (CRLF) characters are removed in
     order to prevent the immediate execution of the pasted text as a command.
     However, a single \r\n character sequence is added, if you press and hold
     the CTRL key while pasting the text.

     Of course you may use Advanced Marking cooperatively with an application
     that copies text to or pastes text from the OS/2 clipboard (e.g. your
     favourite word processor).

     For 3-button mice, you can select that PC/2 uses mouse button 3 instead 
     of mouse button 2. PC/2 then does not overlay the functionality of
     button 2 assigned by the applications.

     Note: Advanced Marking was tested with the E edtior from 2.10 to Merlin
           (OS/2 WARP V4), EPM 5.51, EPM 6.03a, EPM 6.03b, IBM Personal
           Communications/3270 V4.00, IBM Communications Manager/2 1.11 and
           Advantis PassPort/2 1.31D and 2.1D, NetScape Navigator/2 2.02, 
           Lotus Notes 4.52, other versions may be incompatible.

     Note: You should turn off VIO marking in the System notebook under WARP 4
           when using PC/2's Advanced Marking.

     Note: Though users have requested to make Advanced Marking to optionally
           require a key pressed simultaneously (e.g. CTRL or ALT), I have yet
           found not way for a consistant implementation, as IBM's CM/2 does
           not allow this, and at least CTRL is already used to paste with a
           CRLF added.

     Hint: If you are not sure what and when something gets copied into the 
           OS/2 clipboard, just open the clipboard (e.g. from either its WPS
           Object or CLIPOS2.EXE from the commandline).

59.) If running PC/2 as the WorkPlace Shell process, how can I launch WPS
     Objects or change system settings (as e.g. the background color)?

     When the WPS (PMSHELL.EXE) is not running, PC/2 displays an error message
     when you try to launch WPS objects from PC/2's Popup Menu. As WPS Objects
     are part of PMSHELL.EXE, these objects can't live without running
     PMSHELL.EXE.

     Also if the WPS (PMSHELL.EXE) is not running, you can't change your 
     system settings. To be exact, you still can do this with an INI editor
     and editing OS/2's OS2*.INI files directly, but I would strongly 
     discourage you doing so, unless you really know what you are doing (you
     can easily prevent PM or OS/2 from booting otherwise).

     As a sample, the REXX batch file PMColor.cmd is shipped with PC/2, which
     allows you to set the PM background color that is displayed after the 
     next reboot.

     If you want to launch a WPS Object, and the programs to interface with 
     the system settings are also WPS Objects, while running PC/2 as the 
     WorkPlace Shell process, ensure that PMSHELL.EXE is started before. Just 
     create a Menuentry by locating PMSHELL.EXE which is usually installed on 
     your boot drive [d] at:
        [d]\os2\PmShell.exe
     and launch PMSHELL.EXE from this item before invoking WPS Objects.

     Unfortunately PMSHELL.EXE can't be exited once it is running, so you have
     to find a process killer (e.g. KILLEM.EXE or PSPM2) to force PMSHELL.EXE
     to terminate it when no longer required. Warning! Terminating PMSHELL.EXE
     also terminates all WPS Objects too, and the changes you have made to 
     your system configuration may be lost (keep your system idle for at least
     a few minutes before killing PMSHELL.EXE, as the WPS usually will have
     updated the OS2*.INI files then).

60.) I've activated Advanced Marking, now both mouse buttons 2 seem no longer
     to work as expected for IBM Personal Communications/3270, IBM
     Communications Manager/2, Advantis PassPort/2, EPM, NetScape/2 and Lotus
     Notes!

     This is a restriction when using Advanced Marking. In order to implement
     Advanced Marking, PC/2 changes the functionality of mouse button 2. You
     may either disable Advanced Marking, or you may invoke the functions from
     the menubar (these functions are not used often anyway and are very
     likely also selectable from the menubar).

     For 3-button mice, you can select that PC/2 uses mouse button 3 instead 
     of mouse button 2. PC/2 then does not overlay the functionality of
     button 2 assigned by the applications.

61.) Does PC/2 also support OS/2 half-packs (OS/2 running on top of
     MS Windows)?

     Yes, PC/2 supports OS/2 running on top of MS Windows similar to OS/2
     full-pack (OS/2 including WIN-OS2). For WIN-OS2, the demonstrational
     configuration files assume that WIN-OS2 is used from a path similar to:
        [d]\os2\mdos\winos2
     where [d] is the drive (e.g. C: ) WIN-OS2 was installed into.
     To use PC/2 with MS Windows, replace these references with references
     to the path you installed MS Windows similar to:
        [d]\Windows
     where [d] is the drive (e.g. D: ) MS Window was installed into.

62.) I'm running PC/2 as the WorkPlace Shell process, how can I change PM
     settings (my system configuration), e.g. the PM background color?

     Most PM settings are adjusted through user-friendly dialogs provided
     by the WPS, but these dialogs just hide the low-level interface of the
     OS/2 INI files (OS2.INI and OS2SYS.INI), where PM gets its settings
     from.

     As a sample, the REXX batch file PMColor.cmd is shipped with PC/2, which
     allows you to set the PM background color that is displayed after the 
     next reboot.

     To adjust the PM settings you may temporarily invoke the WPS (start
     the executable PMSHELL.EXE), or if you are really sure what you are
     doing, use an INI editor to directly edit OS/2's OS2*.INI files or 
     use small REXX scripts (e.g. like SmallFnt.cmd).

     Unfortunately PMSHELL.EXE can't be exited once it is running, so you have
     to find a process killer (e.g. KILLEM.EXE or PSPM2) to force PMSHELL.EXE
     to terminate it when no longer required. Warning! Terminating PMSHELL.EXE
     also terminates all WPS Objects too, and the changes you have made to 
     your system configuration may be lost (keep your system idle for at least
     a few minutes before killing PMSHELL.EXE, as the WPS usually will have
     updated the OS2*.INI files then).

63.) Advanced Marking doesn't paste at the correct location sometimes, what
     am I doing wrong?

     Advanced Marking manipulates the default processing of mouse messages for
     the applications it supports, however there are limitations that can't be
     removed (e.g. the timely expiration of the message processing).

     Such limitations may cause text pasted to the wrong position when you
     paste by mouse button 2 doubleclicks to a window not having the focus
     (e.g. IBM Communications Manager/2). In this case the text may not got
     pasted to the position you doubleclicked on, but to the position the
     cursor was before the window lost the focus.

     You may either move the focus to this window before pasting into it, or
     you may increase the delay between both mouseclicks of the doubleclick.

64.) Are UNC Names supported?

     Yes, PC/2 supports UNC Names as most programs do, as UNC Name support is
     built into OS/2 natively. For example to launch a program from a OS/2
     LAN Server or from the OS/2 File and Print Client you may enter: (Note:
     Replace [d] with a drive letter, e.g. E: )
        Program Title:      VirusCheck LAN
        Path and Filename:  \\DOMAIN1\Utilities\AntiVirus\AV.exe
        Working Directory:  [d]
        Parameters:         -CheckSum

     Note: The working directory must not be an UNC name (which doesn't work
           for OS/2 command windows either, e.g. you can't enter the command
           CD \\DOMAIN1\Utilities

     You may of course also specify UNC Names in the session's Environment
     Settings, e.g. to allow above sample to load DLLs via UNC Names you may
     set into the Environment:
        ENDLIBPATH=\\DOMAIN\Utilities\AntiVirus\DLL;%ENDLIBPATH%

65.) I have multiple partitions with (different versions of) OS/2
     Installations, what can I do to setup a single installation of PC/2
     that can run OS/2 version dependent applications, e.g. the Multimedia
     Player?

     You can simply use the macro %BOOTDRIVE% (which is not case dependent)
     instead of a fixed drive letter. When launching the session this macro
     gets replaced by d: , where d is the drive letter of the drive you OS/2
     was booted from.

     You can also use the macro %PC2% which gets replaced by the fully 
     qualified path PC/2's executable was invoked from.

     You can also use the macro %CDDRIVE% which gets replaced by d: , where 
     d is the drive letter of the first CD-ROM drive.

     For example to run the OS/2 Multimedia Player from the partition OS/2
     booted from you enter:
        Program Title:      MultiMedia Player
        Path and Filename:  %BOOTDRIVE%\MMOS2\MPPM.EXE
        Working Directory:  %BOOTDRIVE%\MMOS2
        Parameters:         -L %CDDRIVE%\MMOS2\MOVIES\MACAW.AVI
        IconFile:           %PC2%\MMDemo.ico
     In the Parameters entryfield you may even want to enter e.g. the 
     %BOOTDRIVE% macro multiple times.

66.) I have enabled the feature "Button 2 Border click moves window", but I
     can't move some windows, what am I doing wrong?

     Most likely you haven't positioned the mouse pointer correctly on the
     border of that window. Just ensure that the mouse pointer is positioned
     on the frame window's border, press and hold mouse button 2 and try drag
     the window to its new position again. Even if a frame window has no
     border at all (e.g. usually for dialog frame windows), you can move that
     window, just imagine that it would have a sizeable border as PC/2 handles
     that window as if it had a sizing border.

67.) PC/2 used to work perfectly, but now it hangs OS/2 when starting!

     There is one known problem when upgrading from OS/2 2.x to OS/2 Warp. If
     you have configured a tiled background bitmap under OS/2 2.x you will
     experience an OS/2 hang when running PC/2 under OS/2 Warp (either by
     upgrading OS/2 or be booting from a different partition) due to an OS/2
     bug.

     To recover from such a hang, you may take one of the following measures:

        *) Delete PC2.INI, before PC/2 launches the next time, for example by
           booting a OS/2 command window. PC2.INI may be hidden and read-only,
           but will be found in the directory the executable PC2.EXE resides.
           Unfortunately you have to reconfigure PC/2's Desktop Configuration
           the next time you launch PC/2.

        *) Rename or delete the bitmap you have configured as the Desktop
           background bitmap of PC/2.

        *) Use an INI editor to remove the entries of PC/2's Desktop
           background bitmap - this is of course the toughest method! Be
           sure you know what you are doing!

68.) Can PC/2 help my server to recycle at midnight for example to get rid of
     resource leaks or hanging programs?

     Yes, PC/2 can reboot your server at a predetermined time automatically
     with the help of the integrated Scheduler. The Scheduler allows you to
     automatically launch Menuentries hourly, daily, weekly, monthly or once
     a year as if you had selected them from the Popup Menu. As Shutdown OS/2
     (beside PowerDown OS/2, Lockup OS/2 and Exit PC/2) is one of the 
     Menucontrols also being supported by the Scheduler, you can configure 
     PC/2 to automatically shut down and reboot OS/2 at midnight.
     Just remember that the clocks in PCs aren't very accurate at all! You
     must also have PC/2 configured that selecting the ShutDown OS/2 Menuentry
     automatically (that is no manual intervention required) shuts down and
     reboots your workstation!

     Note: To reboot OS/2, PC/2 requires the DOS.SYS device driver of OS/2 to
           be installed. Ensure that for OS/2 2.11 the line
           DEVICE=[x]\OS2\DOS.SYS, and for OS/2 WARP the line
           DEVICE=[x]\OS2\BOOT\DOS.SYS is contained in your CONFIG.SYS, where
           [d] is the the drive letter you installed OS/2 at, e.g. E: ).

           This line may only be missing for non standard OS/2 Installations,
           e.g. maintenance partitions created by BOOTOS2.

69.) When running PC/2 the focus change when closing windows behaves peculiar!

     First, most likely you have checked the option "Follow Focus". By 
     unchecking that option, this seamingly random switch between Virtual
     Desktops and/or windows will revert back to normal PM behaviour.

     Second a short explanation of the terminology of OS/2: You surely have
     heard the term "(input) focus". The control having the input focus will
     receive the user input, e.g. if you have a window with 5 entryfields
     controls, your keyboard input will be routed to that single entryfield
     that just has the input focus. Usually the titlebar of the window the
     control having the input focus is in will be marked active - so you can
     call it "focus window".

     Now assume that the focus window gets closed, PM has then to find another
     window becoming the focus window and inside this window a control
     receiving the input focus. PM finds the new focus window by examining the
     Z-Order. The window a user is working with (our focus window) is usually
     on top of Z-Order causing it to cover all other windows. So when this
     window gets destroyed, the new window getting on top of Z-Order will
     become the new focus window (unless it is minimized).

     So what has this to do with PC/2? Well, PC/2 ensures that its QuickSwitch,
     its SessionBar and its Overview (only when having selected "Keep PC/2 on
     Z-order top") window are kept on top of Z-Order when visible - causing to
     cover all other windows. Now when the focus window gets destroyed, one of
     PC/2's windows will become the focus window as it is on top of Z-Order.
     As this is very inconvenient from the usability standpoint, PC/2 ensures
     that its windows stay on top of Z-Order, but tries to refuse becoming the
     focus window passing the input focus on the the next window not part of
     PC/2 (which may also cause a switch to another Virtual Desktop).

     If the window is unfortunately on another Virtual Desktop, OS/2 doesn't 
     care as it doesn't know the concept of Virtual Desktops. OS/2 just 
     activates it, and if it is an e.g. editor, keystrokes will be reflected
     in the loaded file, however you won't see that window unless you switch 
     to the Virtual Desktop it is running inn. PC/2 just helps you by 
     switching to the Virtual Desktop the active window is located at.

     You may find it annoying that PC/2 switches Desktops, but otherwise a 
     window might get the focus you can't see (and PC/2's intension of the
     "Follow Focus" option is to prevent that)!
     
70.) I'm experiencing unexpected hangups/traps now that I've enabled the
     Titlebar Smarticons.

     Adding smarticons to all titlebars is very complicated as there is not
     native support for this in OS/2 (and they way subclassing is implemented
     in OS/2 and Windows too, it's easy to run into troubles), so there will 
     be problems and the only save way to remove them is to disable the 
     Titlebar Smarticons (at least for the failing applications by adding them
     to the exception .

     I know there are problems when having enabled Titlebar Smarticons under
     OS/2 2.x, or when running more than one program that adds smarticons into
     titlebars (e.g. WARP 4 VoiceType Dictation, NPSWPS, XIT,...) or when
     using a PM debugger. Unfortunately I have found no way yet to make this
     feature perfectly stable (mostly due to problems in PC/2 and partly due
     to lack of native OS/2 support for Titlebar Smarticons).

     You can exclude the name of executables, which appear to fail when using
     Titlebar Smarticons, by adding their case sensitive, unqualified 
     executable name without the extension in the Exception List on the page 
     you activated the Titlebar Smarticons. Adding ACS3EINI, VIEW3820, IPMD, 
     ICSDEBUG and NOTES (without the path and extension!) are applications I 
     have experienced problems with.

     Additionally, the Context Help Smarticon does not work very well under
     OS/2 2.x at all, because it behaves differently compared to OS/2 WARP.

     Most easily, just use the Spy notebook page to find out the names of the
     windows you want to add to the Exception List.

--->>>  --->>>  --->>> Please also read the third part  <<<---  <<<---  <<<---

:exmp.
:font facename=default size=0x0.
:p.:link reftype=hd res=4003.<Backward>:elink.
                                :link reftype=hd res=4005.<Forward>:elink.

:h1 res=4005.Help Questions &amp. Answers (part 3)
:p.
:font facename=Courier size=9x5.
:xmp.
****************************************************************************** 
                        Frequently Asked Questions Part 3/3

                   PC/2 - Program Commander/2 Version 2.20 for
     IBM OS/2 2.x, OS/2 WARP, WARP Srv. SMP, Aurora Presentation Manager
                  Copyright (C) by Roman Stangl December, 2000

                               Version 2.20 Draft
******************************************************************************

Contents:

71.) I pressed CTRL+ESC to activate the Window List, the Window List pops up
     but immediately disappears being replaced by PC/2's Popup Menu.
72.) What is Hardware assisted Panning (also called Hardware Virtual Desktop)?
73.) How can the icon and default font size be changed under OS/2 PM?
74.) How can the icon and default font size be changed under WIN-OS2?
75.) I want to include OS/2 and DOS command processor and seamless WIN-OS2
     sessions in an Exception List, however I can't seem to get it working 
     correctly!
76.) Some windows of PC/2 are always visible, even when e.g. my screen saver
     is running.
77.) I'm using the SessionBar and/or selected to keep PC/2's Overview window
     on top of Z-Order, now other applications behave differently!
78.) I'm using the SessionBar and/or selected to keep PC/2's Overview window
     on top of Z-Order, now my system is very unresponsive or hangs!
79.) Since I have/had PC/2 running, now other applications disappear silently,
     what's wrong with my system.
80.) Since running PC/2, files named PDUMP.xxx, POPUPLOG.OS2 are always 
     created and PC2.TRP grows!
91.) Since running PC/2 some applications disappear with heavy disk activity!
92.) Can I customize the bitmaps PC/2 uses?
93.) What adjustment to the workstations power management supports PC/2?
94.) PC/2 seems to prevent screen savers from working, can this be corrected?
95.) How can I view the source code?
96.) Is the data displayed in the SessionBar's CPU Performance graphics 
     correct?
97.) I think I'm running no CPU intensive application, but the SessionBar's
     CPU Performance graphics always shows a flat out system, what't that?
98.) What causes the message that "PC/2 has detected a not responding thread 
     and restarting PC/2 is recommened"?
99.) Is the Registration notebook page supposed to let me register PC/2?
100.) How does the Registration (Automatic Update) feature work?
101.) I'm confused with the Sticky Windows. I can't seem to figure out when
      windows will be kept on the active Desktop and when they won't?
102.) PC/2 always grabs the input focus even when starting some programs that
      can easily run in the background. Any way to prevent that?
103.) PC/2 supports Macros, what's that?
104.) PC/2's Online Automatic Update does not work!
105.) Can I delete PC2.TRP?
106.) PC/2, the WPS (PMSHELL.EXE) and WarpCenter, it behaves very peculiar!

Questions, Answers:

71.) I pressed CTRL+ESC to activate the Window List, the Window List pops up
     but immediately disappears being replaced by PC/2's Popup Menu.

     You have most likely selected that the Popup Menu should be displayed
     with the left and/or right CTRL key. Unfortunately you have to hold the
     CTRL key a little bit longer before pressing the ESC key or after
     depressing the ESC key in order to avoid this problem.

     The technical explanation is: Every key generates a make code when it
     get pressed and a break code when it gets depressed. If you press and
     hold a key, the make key gets repeated after a short delay at a certain
     rate (which can be configured in OS/2's System Setup folder with the
     Keyboard Object). PC/2 detects that the user wants the Popup Menu to be
     displayed with the CTRL key, when the first make code is followed by
     the break code of the same key, keeping the key pressed so that multiple
     make codes are generated or pressing a different key would tell PC/2
     that the Popup Menu should not be activated.

     However in the case of CTRL+ESC, PC/2 does not see that ESC follows the
     CTRL key, all it sees is that the CTRL key's make code gets immediately
     followed by the CTRL key's break code and displays the Popup Menu, which
     causes the Window List to disappear again. CTRL+ESC is one of the Hotkeys
     OS/2 filters via a device driver so that PC/2 (not being a device driver)
     can't see the ESC key of this combination. OS/2 does not filter the
     CTRL key, because it can't know that the user wants to press ESC next
     (the user could also press CTRL+Q instead), and as a result, PC/2 detects
     only the CTRL key toggled. If you hold the CTRL key a little longer,
     another make code follows the first make code and PC/2 knows that you
     don't want the Popup Menu to be displayed.

     As OS/2 displays the Window List after detecting the break code of the ESC
     key, but filters the CTRL make codes after detecting the make code of the
     ESC key there is no chance for a reliable workaround of this problem
     (unless using a device driver).

72.) What is Hardware assisted Panning (also called Hardware Virtual Desktop)?

     Some hardware and video drivers (e.g. IBM ThinkPads) support a feature
     that is (in technical terms) called Hardware Panning. For example assume
     that your video adapter has 1 MB video memory which supports a resolution
     of XGA, that is 1024 by 768 pixels with 256 colors. When you attach a
     CRT (display) its no problem to run that resolution. However assume that
     your video adapter is inside a Laptop, which has a LCD display that can
     display resolutions up to VGA, that is 640 by 480 pixels, only.
     With 1 MB video memory you can display a VGA resolutions with up to 64k
     colors, however if you only use 256 colors, more than half of the video
     memory is not used.

     So, vendors, like IBM, decided that if the VGA LCD runs only with 256 
     colors, the full videom memory can be used, if the VGA screen is used
     as a window within the XGA resolution. The hardware and video driver then
     support Hardware Panning, where this VGA sized window is moved by 
     hardware assistance within the XGA sized video memory. Due to the
     hardware assistance, this operation is much faster than any software
     solution. You can compare this with PC/2's Virtual Desktop, where the
     Physical Desktop covers only a part of the whole Virtual Desktop.

     Unfortunately this approach has a drawback. As the physical video memory
     organization is XGA, OS/2 PM also assumes a XGA-sized display, and windows
     are created with a XGA sized display in mind. Expressed differently, most
     windows are larger than the VGA display, which is very annoying from the
     point of usability.
   
     PC/2 now lets you limit the size of windows, for example to VGA. Using 
     this option while having activated the Hardware assisted Panning running 
     on XGA resolution, ensures that windows are not larger than your VGA 
     sized LCD display. Of course the windows will be displayed anywhere 
     within the XGA sized area, but you can move the windows or use the
     Hardware assisted Panning to move your VGA sized LCD display to cover
     that windows. However, this applies only for sizeable windows (that is
     windows having a sizeable border), because tests have shown that reducing
     non sizeable windows can cause them to be unoperable by the user.

     Most easily, just use the Spy notebook page to find out the names of the
     processes that created the windows you want to add to the Exception List. 
     Please also see item 75.) how to add OS/2 and DOS command windows and 
     seamless WIN-OS2 windows, as they are all run by PMSHELL.EXE.      

     Note: The VGA sized LCD panel within a XGA sized video memory is only an
           example. SVGA and XGA sized LCD panels already exist (e.g. the IBM
           ThinkPad 760 series), so the requirement for Hardware assisted 
           Panning may disappear in the near future, unless the supported
           video memory is not increased too (e.g. the IBM ThinkPad 760 series
           contain 2 MB video memory which would allow resolutions up to 1600 
           by 1200 pixels with 256 colors... (not necessarily on a CRT display 
           and/or the OS/2 video driver, but from the video memory 
           organization supported by the hardware)).

73.) How can the icon and default font size be changed under OS/2 PM?

     Unfortunately, changing the icon and default font size under OS/2 is a
     somewhat tricky and difficult task. I would recommend you to get Jason
     Shannon's SMALLF13.ZIP, which should be available on all major OS/2
     archives, which provides a excellent automatization of the required
     patches. If you are lucky, you may even get support for the default font
     and icon sizes by the display driver itself, e.g. the latest S3 Corp.
     drivers allow to adjust them via the Capabilities pushbutton on the 
     Screen page of the System notebook.

     How is the icon and default font size determined under OS/2 PM?
     Well, the icon and font sizes are dependent on the resolution you are
     running, and what resolution causes which icon and font sizes is
     unfortunately hardcoded in the display driver. For resolutions of
     VGA (640 * 480) and SVGA (800 * 600), the icon size is (usually) 32 * 32
     pixels and the font size is designed for 96 DPI (dots per inch). For
     resolutions of XGA (1024 * 768) and above, the icon size is (usually)
     40 * 40 pixels and the font size is designed for 120 DPI. Fortunately
     you can patch the display driver to use different icon and font sizes.

     Patching is a difficult and dangerous task, you can easily currupt your 
     display driver and prevent PM (Presentation Manager) from booting, 
     however you can always boot to a command line. The following procedure
     outlines what you have to do:

     1) You have to find the DLL that implements your display driver. This
        DLL is usually named after your video chipset, e.g. IBMS332.DLL for
        S3 or IBMCNT32.DLL for Chip's and Technologies. This DLL is located
        in [d]\OS2\DLL, where [d] is the drive you installed OS/2 onto, and
        also on the diskette (possibly in one of the archives) you installed 
        your display driver from.

     2) Make a backup copy of the DLL you are going to patch! Don't skip this
        step, this allows you to undo the patches you made! As the DLL is
        locked while PM is running, you have to boot to a OS/2 command window
        from either the Recovery Choices Screen or from another OS/2 partition
        or from the OS/2 boot diskettes.

     3) Locate the DPI definition in the DLL to change it from 120 (0x78) to
        96 (0x60). Either use SMALLF13 or search for a byte sequence of 0x78
        0x00 0x00 0x00, and patch 0x78 to 0x60 by using SMALLF13 or the OS/2
        PATCH command.

        Note: There may be multiple occurances of this byte sequence. You may
              have to try out them sequentially, if the system boots after the
              patch you changed the correct one, otherwise you will most 
              likely experience traps while booting PM.

     4) Locate the default icon size definition in the DLL to change it from
        40 (0x28) to 32 (0x20). Either use SMALLF13 again, or search for a 
        byte sequence of 0x28 0x00 0x28 0x00, and patch both 0x28 occurances
        to 0x20 by using SMALLF13 or the OS/2 PATCH command.

        Note: There may be multiple occurances of this byte sequence. You may
              have to try out them sequentially, if the system boots after the
              patch you changed the correct one, otherwise you will most 
              likely experience traps while booting PM.

     5) Put the patched DLL into effect. As the DLL is locked, you have to 
        boot to a OS/2 command window from either the Recovery Choices Screen
        or from another OS/2 partition or from the OS/2 boot diskettes.

     6) Reboot your system with the patched DLL in place and hopefully you
        will get more information onto your display!

     Note: Changing the icon size may be useful for systems (e.g. like most
           IBM ThinkPads) that support Hardware Panning. Changing the default
           font size may be useful for any resolution to display more 
           information on your display.

74.) How can the icon and default font size be changed under WIN-OS2?

     First, check that you have installed the font support for VGA, that is
     VGAFIX.FON, VGAOEM.FON and VGASYS.FON, which you most likely will find in
     the directory [d]\OS2\MDOS\WINOS2\SYSTEM, where [d] is the drive you
     installed WIN-OS2 into. If you have installed OS/2 VGA support before
     you installed your OS/2 video driver, these files will have been 
     installed, otherwise its very likely that they have been installed too.
     If you can't locate these files, either get them from another WIN-OS2 or
     Windows installation, or you have to find the archive from your OS/2
     installation medium containing these files and unpack them (the location
     of this archive is OS/2 version dependent, but you can easily display
     the contents of an archive by typing at an OS/2 command prompt:
         unpack archive /show
     where archive is the name of an archive.

     Second, make a backup copy of the file SYSTEM.INI, which you will find in
     the directory [d]\OS2\MDOS\WINOS2.

     At last, use your favourite editor to change the following statements in
     your SYSTEM.INI from:
         [boot]
         oemfonts.fon=xgaoem.fon
         fixedfon.fon=xgafix.fon
         fonts.fon=xgasys.fon
       
         [display]                
         dpi=120
     to:
         [boot]
         oemfonts.fon=vgaoem.fon
         fixedfon.fon=vgafix.fon
         fonts.fon=vgasys.fon
       
         [display]                
         dpi=96

     Restart WIN-OS2 and be pleased about the reduced font size.

     Note: As WIN-OS2 and Windows only support one icon size (32 * 32), you
           can't and don't need to change the icon size.

           Changing the default font size may be useful not only for systems
           supporting Hardware Panning (e.g. most IBM ThinkPads), but also
           may be useful to display more information on your display.
 
           The procedure outlined above will also work under Windows.

75.) I want to include OS/2 and DOS command processor and seamless WIN-OS2
     sessions in an Exception List, however I can't seem to get it working 
     correctly!

     To include OS/2 and DOS command processors simply enter CMD, to include
     seamless WIN-OS2 sessions simply enter SEAMLESS in the multiline 
     entryfield of the Titlebar Smarticons and/or Hardware Panning features.

     Note: OS/2 and DOS windowed command processors (and all processes 
           running under or as a windowed command processor) and seamless
           WIN-OS2 sessions run their windows in the context (address space) 
           of PM (Presentation Manager) itself. The executable of PM is the 
           executable PMSHELL.EXE, so you would have to specify PMSHELL.

           However the WPS (that is all your WPS windows) are also run by
           PMSHELL.EXE, so adding PMSHELL in the Exception List would also
           disable the features for WPS windows. Fortunately PC/2 is smart
           enough to allow you to specify CMD if you just want to include OS/2 
           and DOS windows or to specify SEAMLESS if you just want to include
           seamless WIN-OS2 sessions in the Exception List, while keeping WPS 
           windows. 

     Note: How does it fit together that OS/2 and/or DOS window and seamless
           WIN-OS2 sessions are run by PMSHELL but you are sure you've invoked 
           CMD.EXE, COMMAND.COM or WIN.COM?

           Well, for OS/2 and DOS command processors the program that really 
           runs really is CMD.EXE (or a kernel process for DOS windows) which 
           you can easily verify by using PSTAT.
           However OS/2 support for OS/2 and DOS windows in PMSHELL.EXE 
           displays the command processor's output in a PM window and passes
           input from that PM window back to the command processor. You can
           do that yourselves too, just start a command processor from your
           PM window and redirect its standard output and input.

           And for seamless WIN-OS2 sessions, the program that really runs is
           a kernel process which you can als verify by using PSTAT.
           However seamless WIN-OS2 support in PMSHELL.EXE is twofold, on one
           hand to integrate WIN-OS2 programs into PM, each seamless WIN-OS2 
           window is covered by a "shadow" PM window (e.g. to support focus 
           change), on the other hand the WIN-OS2 display driver is a (lower
           than PM) interface to the GRE (GRraphics Engine, the engine that is
           responsible for all drawing). Because of this "split" design, 
           screen corruption can occur when the "shadow" PM window gets out of
           sync with the "lower" layer GRE drawing - which is the primarily
           reason, PC/2's Overview and/or SessionBar will prevent all other PM 
           windows to get on top of Z-Order but don't prevent seamless WIN-OS2
           windows (because PC/2 can prevent the "shadow" PM window from 
           getting onto top of Z-Order and getting the user input, but the 
           "lower" layer GRE drawing would draw on PC/2's windows, you will
           then see the drawing of the seamless WIN-OS2 session, but the 
           WIN-OS2 session would never get user input - your GUI will become
           unusuable).

76.) Some windows of PC/2 are always visible, even when e.g. my screen saver
     is running.

     PC/2 ensures that its SessionBar (when having selected it to stay on top 
     of Z-Order), its Overview Window (when having selected it too in the 
     Desktop dialog) and the QuickSwitch window stay on top of Z-Order that is
     above all other windows to prevent them from being overlapped by other 
     windows.
     As there is no way to tell that a window, e.g. a screen saver, wants to
     stay on top of Z-Order anyway, the funny effect may occur that most of
     the display is covered by that window, but PC/2 windows are shining 
     through.
     For your screen saver, you can deselect that PC/2 keeps its window on 
     top of Z-Order, but you must also ensure that the SessionBar is not at
     absolute top or bottom of the screen, because even it the SessionBar
     may not be on top of Z-Order it will reposition other windows then to
     avoid overlapping.

     Note: I tried to avoid this behaviour by testing for systemmodal windows,
           and then prevent PC/2 windows to set themselves on top of Z-Order,
           but unfortunately the API WinQuerySysModalWindow() returned a 
           window sometimes when absolutely no systemmodal window was active.
           So I had to left out such a test.

77.) I'm using the SessionBar and/or selected to keep PC/2's Overview window
     on top of Z-Order, now other applications behave differently!

     You are most likely running OS/2 2.x or WARP 3 (under WARP 4 I couldn't
     reproduce that behaviour). To to a bug, PC/2's SessionBar and Overview 
     window can become part of the owner chain of other applications (e.g. 
     the Job Properties dialogs of printer drivers seem to show this problem 
     most likely, as a sample printer driver shipped by IBM had a bug, please
     upgrade to the latest driver level which should fix that problem). The 
     easiest way to find out if a window is the owner of another window is 
     to click on that window, if you can hear a beep and won't be able to 
     set the input focus to that window then you have found an owner.

     The problem you may experiencing is, that PC/2's windows become the 
     owner of other applications windows (even in other process contexts),
     causing problems with that applications. I've written a small sample
     program that shows that problem and made it available to OS/2 
     development, where they could reproduce the problem and advised to
     use the latest printer driver (which in my case worked for the PostScript
     driver).

     However, you can circumvent that problem easily. Disable that the 
     SessionBar is kept on top of Z-Order (at least temporarily). Also 
     uncheck the option to keep PC/2's Overview window on top of Z-Order. 
     Most easily, the problem usually can be avoided by changing the focus to
     other windows than PC/2's before invoking the windows you experienced 
     problems with. 
     
     As this behaviour is highly reproducable, I've opened the APAR 11867 and
     the change team advised to upgrade to the latest printer driver (and in
     my case for the PostScript driver, the problem was disappeared this way).

78.) I'm using the SessionBar and/or selected to keep PC/2's Overview window
     on top of Z-Order, now my system is very unresponsive or hangs!

     Due to the design of OS/2 PM, it is a problem to keep a window on top of
     Z-Order (compared to Windows). If PC/2 and another applications try to
     keep their windows on top of Z-Order in a similar way, it is likely that
     your system becomes unresponsive due to looping in processing the Z-Order
     chain or hang at all.

     The only ways to solve that problem is to only run one application
     concurrently, or change the behaviour of PC/2 not to keep windows on top
     of Z-Order as shown in question 77.).

79.) Since I have/had PC/2 running, now other applications disappear silently,
     what's wrong with my system.

     In a single word nothing, your system is still ok. In order to improve
     problem diagnostics, PC/2 may have activated OS/2 settings that are 
     equivalent to having added the DUMPPROCESS and SUPPRESSPOPUPS statements 
     (please see the online OS/2 command reference for a detailed 
     description) in your CONFIG.SYS, unless you have explicitely disabled 
     PC/2 diagnostics. I said may, because some beta version had activated
     that setting by default in contrast to the shipped version.

     These settings will write files named PDUMP.xxx (where xxx is unique) and
     POPUPLOG.OS2 into the root of your boot drive when a process dump is
     taken or a fatal unrecoverable error occurs. You can force a PC/2 process 
     dump by clicking with mouse button 1 onto PC/2's Overview or SessionBar
     window while pressing and holding the ALT and CTRL keys.

     Unfortunately there is no way to query and to activate these settings 
     just for PC/2's process. These settings will therefore be activated for
     all processes and stay activated even when PC/2 terminates. However, this
     does no harm to your system, an applications that terminates because of
     a fatal unrecoverable error does this silently (and the dumps will also
     written) without the system error popup you may expect.

     If these files contain the string PC2, they contain data of a fatal PC/2
     problem, please contact the author then. You may safely delete the 
     PDUMP.xxx and POPUPLOG.OS2 files to recover some (a few kB) diskspace 
     anytime.

     Note: You can request PC/2 that it doesn't activate the diagnostic
           settings inside the General notebook page. The settings as defined
           in your CONFIG.SYS will become active after the next reboot.

     Note: You can explicitely request a PC/2 dump by pressing the CTRL-key
           while clicking mouse button 1 onto the Overview or SessionBar
           window and selecting Debug->Dump PC/2 from the popup menu.

80.) Since running PC/2, files named PDUMP.xxx, POPUPLOG.OS2 are always 
     created and PC2.TRP grows!

     The files PDUMP.xxx (where xxx is unique) and POPUPLOG.OS2 contain 
     process dump data when a process was terminated due to a fatal 
     unrecoverable error. PC/2 activates two OS/2 RAS tracing settings that 
     are equivalent of having the statements DUMPPROCESS and SUPPRESSPOPUPS 
     in your CONFIG.SYS.

     Once activated, these settings will be active systemwide, that they will
     not be limited to PC/2's process only. They will also stay active even
     when PC/2 has already been terminated.

     For a default installation, the system error popup appears when a 
     application is terminated due to a fatal unrecoverable error. PC/2 will
     remove this popup and the information displayed will be appended to
     POPUPLOG.OS2 instead. One positive side effect is, that you get a log
     of processes terminated unexpectedly and you may help the vendor of
     the failing application by sending them this log together with the
     PDUMP.xxx file (to find the correct one, just search for the one 
     containing the uppercase name of the application's executable).

     If these files contain the string PC2, they contain data of a fatal PC/2
     problem, please contact the author then, but do *not* mail then without
     asking previously (as reading them depends on tools that must correspond
     to the CSD level of the system they were written at). You may safely 
     delete the PDUMP.xxx and POPUPLOG.OS2 files to recover some (a few kB) 
     diskspace anytime.

     Note: You can request PC/2 that it doesn't activate the diagnostic
           settings inside the General notebook page. The settings as defined
           in your CONFIG.SYS will become active after the next reboot.

     PC/2 always does append internal unrecoverable error into the file 
     PC2.TRP. This file is independent of the OS/2 CSD level, feel free to
     mail (MIME-64 encoded) them the author to help removing the remaining
     bugs. 

91.) Since running PC/2 some applications disappear with heavy disk activity!

     Most likely your application has terminated due to a fatal unrecoverable
     error. PC/2 activates settings, that instruct OS/2 to dump the data,
     that is displayed in the system error popups for default installation,
     into files.

     Check the root of your boot drive for files named PDUMP.xxx (where xxx is
     unique) and POPUPLOG.OS2. You may load both files into an editor to see
     which application terminated unexpectedly (POPUPLOG.OS2 is even human
     readable). The vendor of the failing application may be interested in 
     that data, at least I'm, as the author of PC/2.
    
     If these files contain the string PC2, they contain data of a fatal PC/2
     problem, please contact the author then. You may safely delete the 
     PDUMP.xxx and POPUPLOG.OS2 files to recover some (a few kB) diskspace 
     anytime.

     Note: You can request PC/2 that it doesn't activate the diagnostic
           settings inside the General notebook page. The settings as defined
           in your CONFIG.SYS will become active after the next reboot.

92.) Can I customize the bitmaps PC/2 uses?

     Yes, if you have installed the OS/2 Developers Toolkit you can customize 
     the bitmaps that are included in the resource files PC2xRES.RC, where x 
     is V for VGA and B for BGA screen resolutions. The required steps are 
     somewhat user-unfriendly, but this guide should make them fairly easily. 

     Note: If there are enough requests to allow customization without needing
           to have the OS/2 Developers Toolkit installed, I will remove this
           requirement.
    
     PC/2 includes the full source code in an archive named SOURCE.ZIP which
     the installation has copied into the directory you installed PC/2 into
     (unless you have installed only the required files), just unzip that
     archive.

     The following subdirectories will be created (and the SOURCE directory
     should be your working directory afterwards):
     \SOURCE\                ... C source files
     \SOURCE\RESOURCE\       ... Resource files (OS/2 version dependent)
     \SOURCE\RESOURCE\MERLIN ... Resource files (for Warp 4 and above)
     \SOURCE\RESOURCE\WARP   ... Resource files (for Warp 3 and before)
     To customize the bitmaps for Warp 4 copy the MERLIN subdirectory, for 
     Warp 3 copy the WARP directory, into the RESOURCE directory.
         
     Edit the PC2xRES.RC file to see which bitmaps you can customize, and the 
     Iconeditor to load and customize them. Bitmap files have the extension 
     xGA, where x is V for VGA and B for BGA screen resolutions again. You can
     customize the Titlebar Smarticons, PC/2's Overview window Smarticons, the 
     SessionBar's Launch button and others, just be sure that you don't modify 
     PC2xRES.RC.

     Once you have customized (and backuped) the bitmaps, invoke the resource
     compiler RC.EXE from the subdirectory SOURCE by typing (where x again is 
     V or B):
     RC -R PC2xRES.RC
     This will create a file named PC2xRES.RES, which you will have to add to
     PC2xRES.DLL by typing (where x again is V or B):
     RC PC2xRES.RES ..\PC2xRES.DLL  
  
     You can attach the resources to PC2xRES.DLL as often as you like, because
     the resource compiler will replace the previous resources attached to the
     DLL with the current resources you supplied to RC.EXE as the commandline
     argument.

     Note: I stronly recommend not to change the sizes of the bitmaps you
           customize. PC/2 will not to cease to function otherwise, but the
           bitmaps will look something wired.

93.) What adjustment to the workstations power management supports PC/2?

     Upon a requests from a Overview window's SmartIcon, a menuentry within
     the Popup Menu or with the Scheduler, you can either suspend or power off 
     your workstation. PC/2 request suspend and power off events to be 
     performed by APM (Advanced Power Management).  

     Note: To suspend or power off your workstation via APM, PC/2 requires the 
           APM.SYS device driver of OS/2 to be installed. Ensure that for OS/2 
           2.11 the line
           DEVICE=[x]\OS2\APM.SYS, and for OS/2 WARP the line
           DEVICE=[x]\OS2\BOOT\APM.SYS is contained in your CONFIG.SYS, where
           [d] is the the drive letter you installed OS/2 at, e.g. E: ).

           This line be missing is you haven't selected APM support during 
           OS/2 installation (which you can add now by selective install) or if
           your PC doesn't support APM (which you can verify in your PC's 
           User's Guide).

           WARP 4 supports APM specification 1.1, but a 1.2 compliant driver 
           is in development and will be released likely on the DDPAK site.

     Warning! For power off requests to work, you must be moving at least Warp 
              4. Previous OS/2 versions may power off your PC without 
              perfoming a ShutDown of OS/2, causing a CHKDSK during the next
              reboot.
            
              Do not try to run the Warp 4 APM device driver on prior versions
              of OS/2, as there are dependencies upon other system files 
              including the OS/2 kernel. Thanks to Frank Schroeder from OS/2
              I/O Subsystem/Device Driver Development for technical help  
              coding APM support.

              Depending on your hardware and BIOS (you may update to the 
              latest level), APM may not support suspend or power off requests
              or just hang OS/2. Unfortunately there is no easy way to find
              out what doesn't work, you have to experiment yourself.           
    
    Warning! APM Power Off may not work on all systems despite they are 
             supporting APM and APM.SYS works correctly. Symptoms are (thanks 
             to Frank Schroeder from the OS/2 I/O Subsystem/Device Driver 
             Development, who owns the OS/2 APM driver, for his support to 
             give me some explanations about possible causes):

             *) APM power off does power off your PC, but even under Warp 4 a
                CHKDSK is performed due to the next reboot. This may be 
                related to the combination of a too fast processor and a too 
                slow harddisk, because the request to the disk subsystem didn't 
                finish in time before the power is dropped (this may corrupt 
                partitions!). 
             *) Your PC is powered off, but immediately powers on again. This 
                may be related that the APM BIOS thinks that is should power up 
                again, which can be caused by "Wakeup on LAN" adapters or that 
                OS/2's APM support polls APM BIOS which confuses APM BIOS.
             *) APM power off does not work under OS/2 but does under Windows. 
                As OS/2 (as a protected mode operating system) may use 
                different entrypoints into APM BIOS as Windows, there are some 
                chances that there are BIOS bugs. Even when the same BIOS code 
                is used, timing issues and differences in the frequency of 
                polling, the order of APIs called, the method of APM event
                notifications,... can explain differences.

94.) PC/2 seems to prevent screen savers from working, can this be corrected?

     Well it depends. If you have selected that PC/2 keeps its Overview and
     SessionBar windows on top or Z-Order, then these window will cover any
     screen saver window. If the SessionBar window is a top or bottom of the
     screen, then the screen saver window won't be covered but adjusted in its
     position so that the SessionBar window is still visible. In that case you
     can remove the SessionBar from absolute top or bottom of the screen.
     
     However, even if PC/2's windows may overlap or may adjust the screen 
     saver's window the screen saver will still protect access to your PC,
     as it won't allow you to switch the input focus to any window other
     than the screen saver. If your screen saver is used to blank out your
     screen (to protect your monitor), then the suggested solution would be
     to use the APM (Advanced Power Management) functions builtin your PC to
     suspend or power off your display.

95.) How can I view the source code?
    
     To prevent using the source code on any of the Windows platforms, the
     source code archive has been encrypted by a program that is only 
     available on the OS/2 platform. As no Windows can run 32-bit OS/2
     applications you need to have OS/2 installed to run the decryption
     program.
 
     The encrypted source SOURCE.ZIE and the decryption program PROTECT.EXE
     are used to get the unencrypted source SOURCE.ZIP, which can be 
     uncompressed with UNZIP.EXE then.
     Just run from an OS/2 window the command: PROTECT pc2v200 SOURCE.ZIE
     and SOURCE.ZIP will be created.

     Note: The password pc2v200 (spelling is case sensitive!) is no secret,
           because the algorithm used to decode SOURCE.ZIE is proprietary
           and only available on OS/2. No OS/2 installed, no way to view
           my source.

           I had to use my own proprietary encryption, because the password
           feature of InfoZip's ZIP/UNZIP is also available on the Windows
           platforms and even if the source code is publicly available, the
           export regulations of the US would be violated if my proprietary
           version (so that decryption again would be bound to the OS/2 
           platform) of UNZIP.EXE were re-exported again. 

96.) Is the data displayed in the SessionBar's CPU Performance graphics 
     correct?

     Yes, as I'm using an OS/2 call available on Warp 4+ and Warp Server
     Advanced SMP+ and not just some loops checking free idle time (like
     PULSE does) that data is correct.

     However, to prevent excessive CPU usage PC/2 does not run itself with a
     higher priority than normal applications, which on the other hand may
     prevent a smooth updating of the graphics if the load is high (or the
     SIQ Single Input Queue is blocked).

97.) I think I'm running no CPU intensive application, but the SessionBar's
     CPU Performance graphics always shows a flat out system, what't that?

     Likely you are still running an application that ensures by a low 
     priority that applications using normal priority will get the CPU when 
     required but do consume the CPU when no other application requires it
     anyway. Examples are PULSE, the RC5 cracking client or the NetFinity 
     client.

     As OS/2 reports low priority usage also as CPU usage, and PC/2 gets this
     data from an OS/2 call, it is correct that the CPU utilization is shown
     as 100 %. The only way to avoid that is to stop the mentioned 
     applications.
 
98.) What causes the message that "PC/2 has detected a not responding thread 
     and restarting PC/2 is recommened"?

     PC/2 is a multithreaded application, and uses threads to perform functions
     like finding all windows and determining their size, position and name.
     To determine those data OS/2 APIs are uses, which sometimes send messages
     to the window in question.

     When an application's window now doesn't answer such questions, other
     applications sending message will have a problem too due to OS/2's design
     of the SIQ - Single Input Queue. If one program blocks the SIQ, all other
     programs won't react on user input (by keyboard and mouse) too, the only
     way to recover is that the program starts to process messages again or to
     exit it from the dialog presented after CTRL+ESC is pressed.

     Another side effect of that is IMHO that sometimes not the SIQ is blocked
     but calling some OS/2 APIs blocks the caller until the called program
     answers (and sometimes they don't or it may take a long time). In other
     words, PC/2 calls OS/2 APIs and those APIs never return, thus PC/2 is not
     able to continue processing. This happens seldomly from my experience, 
     but running Lotus Notes and NetScape together increases the likelyhood.

     PC/2 tries to recover automatically from such a hang, however if it 
     detects recovery is required, the user is prompted to confirm that.

99.) Is the Registration notebook page supposed to let me register PC/2?

     No! I did use the term "Registration" because it's a kind of 
     registration, but the term "Online Automatic Updates" which would 
     describe this feature much better simply doesn't fit onto the tab.
     PC/2 is and will be Freeware (though dontations are still welcome ;-),
     the Registrations just gives you and me the following advantages.

     If you register as a PC/2 user I will know you (and have better numbers
     of how many PC/2 users are out there, which of course gives me motivation
     to keep improving it). And of course, I will not pass on or make available
     that data anywhere.

     If you fill out the Registration notebook page completely (and correctly)
     PC/2 will then be able to automatically retrieve news/updates/fixes/...
     regarding PC/2 from the Internet (from my Automatic Update homepage to be
     exact) regularily. This does help you by not having to check my homepages
     for news regularily (though you are indeed welcome), and does help me by
     having a tool to "push" important news to PC/2 users easily (e.g. if a
     new version is coming out I will modify my Automatic Update homepage and
     PC/2 will display those news to you the next day (if the Registration
     page has been filled out correctly).

     The Automatic Update (aka Registration) feature is implemented in 
     PC2NET.DLL. PC/2 will only be able to load that DLL during startup when
     TCP/IP is installed correctly. Assuming this is the case, PC/2 will
     request you to fill out the Registration notebook page and use that
     information to contact my Automatic Update homepage directly or via a
     proxy. You can be connected to the Internet either permanently or via a
     dial-up connection.

     The information retrieved from the Automatic Update homepage is saved 
     into the file PC2.NET. If you have missed a message box (which is
     displayed when PC/2 has found some news), you can browse into that file
     (as your registration date is stored here too, please don't delete or
     modify this file unless you are really sure what you do e.g. in case of
     a corrupted file).

100.) How does the Registration (Automatic Update) feature work?

     The Automatic Update (aka Registration) feature tries to connect to my
     Automatic Update homepage (located at the URL specified at the 
     Registration notebook page, so you can use a browser to display it too).

     What PC/2 does is to retrieve documents and fills out a form with your
     registration data via the standard HTTP protocol. You can use IPTRACE
     and IPFORMAT to verify that there is really no magic.

     To register you as a user, a URL is posted to my Automatic Update 
     homepage which looks like as if a form had been filled out containing
     your registration data (this URL then causes my homepage to send me an
     e-mail containing your registration data, and that's all I ask you to
     do to allow me to know you as a PC/2 user).

     To inform you about news/updates/fixes/... PC/2 downloads a document
     from my Automatic Update homepage. This document is then parsed and
     mirrored locally into the file PC2.NET (to allow PC/2 to detect news
     which haven't been shown to you yet avoiding to show you known news
     multiple times). If news are detected, you will be informed by a
     message box, if updates are found they will be downloaded and you 
     will be presented instructions how to put that update into effect
     (e.g. stop PC/2, replace a DLL and reboot). The term update is used
     here for enhancements, bug fixes and additions.

     PC/2 tries to connect to the Automatic Update homepage daily, that is
     if the connection succeeded, it will wait 24 hours before connecting
     again (the last successful connection time is also saved into PC2.NET).
     If a connection is scheduled, PC/2 tries to connect to the homepage
     through a proxy (when specified) or directly. If the connection fails,
     it is retried a few times per hour unless a dial-up interface is
     detected (which is checked once a minute), because if a dial-up 
     interface is up then likely a connection to the Automatic Update
     homepage will work (whereas when a direct e.g. LAN connection fails,
     it is unlikely that this is just a problem for a few minutes). A Status
     window will inform you when a connection has been established and
     information is downloaded from my Automatic Update homepage.

     Again, the registration makes life easier, so I really would appreciate
     knowing you as a registered PC/2 user (but not doing so does *not*
     prevent you from using any PC/2 feature)!

101.) I'm confused with the Sticky Windows. I can't seem to figure out when
      windows will be kept on the active Desktop and when they won't?

     First, I admit that this really may be confusing due to the liabilities
     introduced with the necessary design changes over the more than 6 year
     history of PC/2 (the main reason was that a window not necessarily has
     a 1 to 1 relationship to an application which the concept of Menuentries
     assume and that it seems to be easier for users to configure such a
     feature at a central place).
     
     Up to version 2.00, you had to select the option "Keep on active Desktop"
     of an application Menuentry's Hotkey notebook page to keep a window always
     on the active Desktop.
     
     Beginning with version 2.10, this option has been renamed to "Sticky 
     Window" and additionally the Overview notebook page of the Desktop Dialog
     also contains an entryfield for Sticky Windows. Of course, uniqueness will
     be ensured.
     
     This new entryfield will be expanded by all Sticky Windows found in 
     Menuentries during loading of the Popup Menu in addition to what you enter
     here. However, when you delete an entry that was migrated from from a
     Menuentry, this will affect only the current instance of PC/2, because the
     next time you launch PC/2 the entry will be migrated again from the 
     Menuentry - unless you deselect the Sticky Window option there too.
     
     Even when you delete an entry that was migrated from an Menuentry, it will
     only disappear the the list in the Desktop Dialog's Overview notebook 
     page, but will still work according to the Menuentry's settings on the
     Hotkey page (because to make a window sticky, it must appear *either* at 
     the Overview *or* Hotkey page). This is true for at least version 2.10, as
     the removal of the Sticky Window option at the Hotkey page in a future 
     PC/2 version will set and end to that confusion.

102.) PC/2 always grabs the input focus even when starting some programs that
      can easily run in the background. Any way to prevent that?

     Yes, just select the Program Style "Background" on the Program 
     Installation's Style notebook page. PC/2 will then start the Program
     without first moving the input focus to itself.
     
     The rationale behind that is, that only sessions being in the foreground
     (and foreground is equivalent of having the input focus) can start other
     sessions into the foreground. And when you have not checked the 
     "Background" style, PC/2 assumes you want to launch the program into the
     foreground.
     
     This way, PC/2 has been working for years so I would break existing
     installations if I would change it, though now I admit that assuming that
     the session should be started into the background by default unless a
     "Foreground" option is selected would have been a better design.

103.) PC/2 supports Macros, what's that?
    
     You may already know that you can specify the Environment that will be
     attached to sessions you launch from the Popup-Menu. The Environment
     is a collection of sentences of the form:
         ENVIRONMENTVARIABLE=VALUE

     You may then specify that the VALUE of a ENVIRONMENTVARIABLE is 
     substituted instead of a certain text, and that text is a Macro. A
     Macro thus has the exact same literal as the ENVIRONMENTVARIABLE but
     pre- and postfixed by a percentage sign (%-sign).

     For example, assume you have defined in the Program Installation dialog:
        Program Title:      Macro-Test
        Path and Filename:  %INSTDIR%\BIN\MyApp.exe
        Working Directory:  %INSDDIR%\
        Parameters:         -Install %INSTDIR% -Temp %WORKDIR%
        Environment:        INSTDIR=[d]\APPS\TEST
                            WORKDIR=[d]\TEMP
    When launching that session what actually is presented to OS/2 to launch
    is:
        Program Title:      Macro-Test
        Path and Filename:  [d]\APPS\TEST\BIN\MyApp.exe
        Working Directory:  [d]\APPS\TEST\
        Parameters:         -Install [d]\APPS\TEST -Temp [d]\TEMP
        Environment:        INSTDIR=[d]\APPS\TEST
                            WORKDIR=[d]\TEMP
    which is indeed a very unique and powerful feature.

104.) PC/2's Online Automatic Update does not work!

    Most likely, your Proxy settings on the Desktop Dialog's Registration
    notebook page are not correct. You should use the same setting that work
    e.g. in Netscape, just the Proxy hostname and its Port are written in
    URL form (for examples please see the online help).

    PC/2 ships with default settings that might be better then not specifying
    Proxy settings at all, however in most cases you have to modify the
    default settings.

105.) Can I delete PC2.TRP?

    In one word, yes, it is not vital for PC/2 to function. More, you might
    even do that regularily as it may grow over time. More exactly that
    file contains the traces of 2 events critical to PC/2 (allowing me the
    author to improve debugging):

    *) Traps (access violations) within PC/2. Those traps are the result of
       a severe bug and you're welcome to report their occurance to the
       author.

       PC2.TRP logs a hang starting with the line: "Exception Dump Facility".
       I'm certainly interested about hearing about such problems as they
       are bugs within PC/2 that should be fixed. I might ask you to
       download some stuff that allows me do diagnose the problem in more
       detail.

    *) Thread hangs within PC/2. PC/2 has a kind of watchdog feature that
       allows it to detect hanging threads, which for reasons not clear to
       me sometimes can happen (to be exacty, some PM APIs under certain
       conditions just hang a thread by never returning to PC/2 again).

       PC2.TRP logs a hang starting with the line: "Thread Dump Facility".
       Most likely, scrolling some lines downwards shows the entrypoint
       nearest to the hang with "Nearest    : Ordinal xxxx", where xxxx
       is the entrypoint. For me, xxxx usually is 5120 in PMMERGE.DLL,
       from reports 5352 seems to be popular too. If you find one of those
       2 entrypoints then it is a already known problem (though I still
       don't know what PM is doing there, as both are not documented).

106.) PC/2, the WPS (PMSHELL.EXE) and WarpCenter, it behaves very peculiar!

    The WPS (PMSHELL.EXE) is just a window like many others, it even has a
    Titlebar which is per design not visible (but if you display the Window
    List and doubleclick with mouse button 1 on "Desktop" while holding down
    the CTRL-key, the WPS will resize it so that the Titlebar is visible).
    Normally the display looks like:

    ************ ... Invisible Titlebar of WPS 
    +----------+ ... The physical display (e.g. sized 1280x1024 pixels)
    !          !  
    !          !  
    !          !
    *----------+  

    Now when you open the WarpCenter at the top of screen, you will notice a
    short flash of the screen and then the WarpCenter is located on top of
    the screen:

    ************ ... WarpCenter covering the Titlebar of the resized WPS
    !          !  
    !          !  
    !          !
    *----------+  
 
    WarpCenter resizes the WPS so that the WPS's Titlebar would be visible
    (exactly as if you had shown it yourself with CTRL+MB1 doubleclick in
    the Window List), but only would as it is immediately covered by the
    WarpCenter itself. I said short flash, because parts of the screen
    redraw.

    If you now close the WarpCenter it restores the WPS back again so that
    the WPS Titlebar is just above the visible screen:
    
    ************
    +----------+
    !          !  
    !          !  
    !          !
    *----------+  

    And finally we are where we have to be carefull. The WarpCenter adjusts 
    the size of the WPS in such a way that it just covers the physical screen 
    with it's Titlebar just above the visible area (WarpCenter closed) or
    just covered by the WarpCenter itself (WarpCenter showing) *regardless* 
    of what size the WPS had been before!

    If you request PC/2 to resize the WPS over all Virtual Desktops with
    "Expand WPS" PC/2 honors that requests, however WarpCenter does not care
    and resizes the WPS again whenever you open or close it. In other words
    the changes done to the WPS by PC/2 can easily be undone by opening or
    closing the WarpCenter.

    Sorry, that's not the optimal behaviour on the side of the WarpCenter,
    but is the way it currently works.

Also, please read the help panels.

Enjoy! Roman
:exmp.
:font facename=default size=0x0.
:p.:link reftype=hd res=4004.<Backward>:elink.
                                :link reftype=hd res=4015.<Forward>:elink.


.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected PC/2 trapinfo help.                                                *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4015.PC/2 hang/trap/bug diagnostics
This chapter explains how you can assist me to locate and fix problems, that
might have been undetected so far.
Even if you don't completely understand every detail here, it shows you which
data I need and how interpretation of this data can show me the reason of a problem.
You may also see some more information about the :link reftype=hd res=4016.<technical
background>:elink. of potential problems.
:p.PC/2 supports 3 independent ways of reporting unrecoverable errors.
The first method it to request a dump of the complete PC/2 address space
(equivalent to :hp4.DUMPPROCESS:ehp4.) or just of
its threads (useful to diagnose recoverable errors) as described for the
:link reftype=hd res=4013.<Overview Window>:elink. Popup Menu,
the second is that all unrecoverable errors are appended to a file :hp9.PC2.TRP:ehp9.,
the third is to dump the registers and process context to disk when an
unrecoverable error occurred, by activation of OS/2 diagnostic support via the
:hp4.DUMPPROCESS:ehp4. and :hp4.SUPPRESSPOPUPS:ehp4. settings.
The OS/2 diagnostic support :hp4.DUMPPROCESS:ehp4. and :hp4.SUPPRESSPOPUPS:ehp4.
is valid systemwide once activated and can only be
deactivated after the next reboot again, but are a kind of last resort finding a problem.
:p.To help diagnosing hangs of PC/2 (which due to the complexity of PC/2 I can't
prevent for 100 %), I've added keys which will allow you to help me finding out
what went wrong.
If you experience problems with PC/2, you are welcome to e-mail me so I can assist you!
:dl tsize=12 break=fit.
:dthd.:hp4.Keys&colon.
:ddhd.Function&colon.:ehp4.
:dt.:hp9.ALT+SYSREQ:ehp9.
:dd.The key combination :hp9.ALT+SYSREQ:ehp9. (you will also have to press the
:hp9.ALT GR:ehp9.-key to access the SYSREQ function of the PRT-SCRN key) toggles
between the 4 debugging modes.
The actual debugging mode is also displayed in the Overview window's titlebar.
Debug levels are 0 to 3, where 0 means no debugging at all.
:p.The debug information activates the beeper for certain events and puts debug
information into a queue, which can be read by using :hp9.PMPRINTF.EXE:ehp9. from
the very useful :link reftype=hd res=4014.<PMPRINTF>:elink. IBM EWS
package.
:dt.:hp9.CTRL+SYSREQ:ehp9.
:dd.The key combination :hp9.CTRL+SYSREQ:ehp9. (you will also have to press the
:hp9.ALT GR:ehp9.-key to access the SYSREQ function of the PRT-SCRN key) will
generate message that may help to synchronize the threads within PC/2.
If messages get lost, and that may happen sometimes (as there are limits in the
number of retries and the sizes of queues), PC/2's main and the working threads
may get out of sync, which for example would prevent PC/2 from switching between
different Virtual Desktops.
:edl.
:p.PC/2 will append to a file named :hp9.PC2.TRP:ehp9., which is located in the directory
:hp9.PC2.EXE:ehp9. is located, whenever it detects an unrecoverable error (that is an
access violation exception or commonly called a "trap"). This file will not
only contain some information that would be also logged by :hp4.SUPPRESSPOPUPS:ehp4.
(which you can disable), but also a dump of the stack and the
sequence of functions called by walking up the chain of stack frames. PC/2
will terminate eventually after this file has been appended.
:p.Whenever you detect the :hp9.PC2.TRP:ehp9. has been created or is growing, I would
appreciate it very much if you can ZIP and E-mail me that archive MIME-64
encoded together with information which version of PC/2 you were running.
This file should allow me to find most if not all unrecoverable errors.
:p.Despite of the :hp4.DUMPPROCESS:ehp4. and :hp4.SUPPRESSPOPUPS:ehp4. settings in your
:hp9.CONFIG.SYS:ehp9., which wouldn't record and dump information anyway for a default
installation, PC/2 by default overwrites these settings (unless you explicitely disable
it) in a way dumps will be always
taken when PC/2 and other processes experience problems or you explicitely request it.
These dumps contain information that will hopefully help me and other vendors to diagnose
and fix problems, that have not shown up during the extensive tests.
I said hopefully, because this is not a trivial task (a detailed problem description
will therefore be appreciated too), and I have not much experience in that area yet.
:p.If you want to enable these settings by PC/2, you can
check the :hp9.Diagnostics:ehp9. checkbox at the Desktop Configuration dialog's
:link reftype=hd res=2050.<General PC/2 Settings>:elink. notebook page.
Only if that option is unchecked the settings as defined in your :hp9.CONFIG.SYS:ehp9.
(off by a default installation) will become active (all changes require a reboot
anyway).
:p.However, let's be realistic and assume that unrecoverable errors occur as shown in
the following sample:
:ul.
:li.Assume an application has terminated unexpectedly and the :hp4.SUPPRESSPOPUPS:ehp4.
setting is enabled.
To verify what has happened you may want to take a look into the files
:hp9.PC2.TRP:ehp9. located in the directory you installed PC/2 into, and
:hp9.POPUPLOG.OS2:ehp9. located in the root of your boot drive.
These files contain entries like the following:
:font facename=Courier size=9x5.
:cgraphic.
07-16-1997  20&colon.02&colon.25  SYS3175  PID 00ba
I&colon.\PROGRAMMING\PC2\PC2.EXE
c0000005
00027e31
P1=00000000  P2=ffffffff  P3=XXXXXXXX  P4=XXXXXXXX
EAX=ffaaffaa  EBX=00000000  ECX=0007208a  EDX=0007208a
ESI=00000000  EDI=00000000
DS=0053  DSACC=d0f3  DSLIM=1bffffff
ES=0053  ESACC=d0f3  ESLIM=1bffffff
FS=150b  FSACC=00f2  FSLIM=00000030
GS=0000  GSACC=****  GSLIM=********
CS&colon.EIP=005b&colon.00027e31  CSACC=d0df  CSLIM=1bffffff
SS&colon.ESP=0053&colon.000a9904  SSACC=d0f3  SSLIM=1bffffff
EBP=000a9968  FLG=00092246

PC2.EXE 0001&colon.00017e31
:ecgraphic.
:font facename=default size=0x0.
:p.Let's look into the data in more detail to see what it can tell us about the
unexcpected application termination (if you have installed the OS/2 Warp
Developer's Toolkit, you may look into the header BSEXCPT.H):
:font facename=Courier size=9x5.
:cgraphic.
Date of exception
    ³
    ³  Time of exception
    ³          ³
    ³          ³ Type of exception,
    ³          ³ type HELP SYS3175
    ³          ³         ³
    ³          ³         ³    Process ID
    ³          ³         ³        ³
07-16-1997  20&colon.02&colon.25  SYS3175  PID 00ba
I&colon.\PROGRAMMING\PC2\PC2.EXE ÄÄÄ Application that failed (PC2.EXE)
c0000005 ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Type of exception (XCPT_ACCESS_VIOLATION)
00027e31 ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Address that caused exception

Cause of exception (XCPT_UNKNOWN)
      ³
      ³   -1 (XCPT_LIMIT_ACCESS)
      ³            ³
P1=00000000  P2=ffffffff  P3=XXXXXXXX  P4=XXXXXXXX

Registers at the time of the exception (EAX contains data which
is suspect, as interpreted as an address (FFAAFFAA it is well
above the limits of the data (DSLIM) and extra (ESLIM) selectors
(being 1BFFFFFF in both cases).
                          ³
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
EAX=ffaaffaa  EBX=00000000  ECX=0007208a  EDX=0007208a
ESI=00000000  EDI=00000000
DS=0053  DSACC=d0f3  DSLIM=1bffffff
ES=0053  ESACC=d0f3  ESLIM=1bffffff
FS=150b  FSACC=00f2  FSLIM=00000030
GS=0000  GSACC=****  GSLIM=********
CS&colon.EIP=005b&colon.00027e31  CSACC=d0df  CSLIM=1bffffff
SS&colon.ESP=0053&colon.000a9904  SSACC=d0f3  SSLIM=1bffffff
EBP=000a9968  FLG=00092246

PC2.EXE 0001&colon.00017e31 ÄÄÄ Instruction where exception
                                (XCPT_LIMIT_ACCESS) occurred (same
                                value as address that caused exception!)
:ecgraphic.
:font facename=default size=0x0.
:p.From above information the exception occurred at the address 00017E31
of object 1 at an instruction that is like :hp9.MOV [EAX], EBX:ehp9.
(move the contents of EBX into the memory pointed to by the contents of EAX,
and as EAX points outside the data selector limit, this statement will
cause an exception).
However, you now want to know which part of source code corresponds to
that failing statement.
As shipped products usually don't contain debug information, this may be
somewhat tricky, which becomes very tricky if the exception did not occur
in your code, but e.g. an OS/2 system DLL.
:p.If the exception occurred in your code, one approach would be to load
the shipping code into the debugger, and set a breakpoint at the instruction
that failed.
Then scroll or run to the breakpoint and save the disassembled instruction
mnemonics around that breakpoint.
Then run the compiler with the same options you produced the shipping code again,
but this time including the option to generate an Assembler listing (e.g. IBM's
VisualAge C++ compiler supports this), and try to find the assembler instructions
that correspond to the instruction mnemonics from the disassembled code.
As a starting point where to search you may want to look into the MAP file.
:p.Another approach would be to compare the address of the failing instruction with
the MAP file you have (hopefully) saved for your shipped code.
For our example here, the MAP file contains (at the section where public symbols
are sorted by address):
:font facename=Courier size=9x5.
:cgraphic.
 0001&colon.00017C94       WinCloseDown
 0001&colon.00017D00       CreateEnvironment
 0001&colon.00018490       ConvertEnvironment
 0001&colon.000187D8       SessionEnvironment
 0001&colon.00018990       StartSession
 0001&colon.00019AD4       LoadMenuData2SessionData
:ecgraphic.
:font facename=default size=0x0.
:p.The failing instruction at 0001&colon.00017E31 in this case is between the beginning
addresses of the CreateEnvironment() and ConvertEnvironment() functions,
so something in CreateEnvironment() failed.
Now we know what part of the source code failed, however this code may be called
from multiple other parts of the source code.
To find out which other part of the code has called the failing function, we
need to look into the sequence of function calls.
PC/2 therefore logs additional information into :hp9.PC2.TRP:ehp9., in our case we
are interested in the stack dump, the stack frame pointer and the stack frames:
:font facename=Courier size=9x5.
:cgraphic.
Stack frame: (Up to OS call)
             0002896F PC2&colon.0
             00028E96 PC2&colon.0
             000AA684 PC2&colon.5
             1BD03893 PMMERGE&colon.3
             00011771 PC2&colon.0
             000AB41C PC2&colon.5
             0003F16F PC2&colon.0
:ecgraphic.
:font facename=default size=0x0.
:p.The stack frame pointer EBP has the value 000A9968.
Looking up the stack dump beginning with the stack frame pointer, PC/2 has
allready logged the sequence of function calls by walking up the chain of
stack frames.
The first instruction of the calling function that would have been executed if
the current function CreateEnvironment() wouldn't have faild, would have been
at the address 0001&colon.0001896F.
Looking again into the MAP file, we find that the function SessionEnvironment()
was the calling function.
The next caller 0001&colon.00018E96 would be StartSession() and so on.
In summary the calling sequence was: PM -> Window Procedure -> StartSession() ->
SessionEnvironment() -> CreateEnvironment().
With some luck (I say luck, as a problem may also happen in old 16-bit code
or in kernel code, which once again increases the difficultiness to find the
problem) the diagnostic data saved into :hp9.PC2.TRP:ehp9. shows
what happened and why it happened.
:p.For exceptions in products other than PC/2, the information in :hp9.POPUPLOG.OS2:ehp9.
helps you to find the product the failing executable belongs to, and finally
the vendor of that product.
You mail that information to the vendor of the failing application, as he/she
might be interested in that data, as this data is very independent of the OS/2
version you are running.
:li.Assume an application has terminated unexpectedly and the :hp4.DUMPPROCESS:ehp4.
setting is also enabled.
The verify what has happened you may want to take a look into the files
:hp9.PDUMP.xxx:ehp9., where :hp9.xxx:ehp9. is a unique number, located in the root
of your boot drive.
This file should allow one to locate virtually all unrecoverable errors, but
as the information in these files is OS/2 version dependent, you may contact the
author of the failing application first.
:p.To find out which application the executable, that has terminated unexpectedly,
belongs to, you may either:
:ul.
:li.load :hp9.PDUMP.xxx:ehp9. into an editor, for example :hp9.E.EXE:ehp9. and
search for the string :hp4..EXE:ehp4..
:li.use a :hp9.GREP:ehp9. utility and search for :hp4..EXE:ehp4..
:eul.
:p.Both methods should match with the executable that failed, and you should then be able
to find the product this executable belongs to, and finally the vendor of that product.
For example, if you find the string :hp4.PC2.EXE:ehp4. I may be interested in
that file, as PC2.EXE is one of PC/2's executables.
:p.You should also look for the string :hp4.Internal revision:ehp4. and save the data that
immediately follows that string, which is the internal OS/2 revision the dump was
created with.
This will have the layout of :hp4.x.yyy:ehp4., e.g. 8.200 for Warp 3 without any fixpack
installed, and the vendor will be interested in that as different revisions require
different revision of tools to read that dump.
:p.You may ask the vendor of the failing application if he/she is interested in that
dump by specifying the OS/2 internal revision is was taken at, accurately.
:eul.
:warning text='Note:'.
The following paragraphs are also contained in the document
:hp4.TrapInfo.doc:ehp4.,
available in the directory you installed PC/2 into, because you might need to read this
information while PC/2 is not running.
:ewarning.
PC/2 allows you to take a process dump (which does not terminate PC/2), which may
help me to find bugs and especially hangs.
A hang is a situation where user input is not (currectly) processed for a single
application or the whole system.
:p.The following steps will guide you on taking a PC/2 dump (which you should
only do on request or if you have really great difficulties running PC/2):
:ol.
:li.To take a dump, just press and hold the :hp4.CTRL:ehp4.-Key
while clicking with mouse button 1 onto PC/2's Overview or SessionBar windows
and select the menuentry :hp9.Debug->Dump PC/2:ehp9..
:warning text='Note:'.
Taking a process dump may take a few seconds.
While the dump is taken, your system will be unresponsive to user input.
Once a dump has been requested, OS/2 will start dumping any process that traps,
regardless of the previous value of the :hp4.DUMPPROCESS:ehp4. setting in
your :hp9.CONFIG.SYS:ehp9..
:ewarning.
:li.OS/2 will then write a unique file named :hp9.PDUMP.nnn:ehp9. into the root
directory of your boot drive, where :hp9.nnn:ehp9. is an index that is incremented each
time a new process dump is created.
:li.If there are multiple :hp9.PDUMP.nnn:ehp9. files, the one with the highest index
will be the one you have just taken.
You can verify this by searching for the string :hp9.PC2:ehp9. in the dump files.
:li.Please archive the dump file, preferably with InfoZip's ZIP utility, and encode it
in MIME-64 format (only this format please), if you are going to send me the dump via E-mail.
:li.As the tool to read the dumps is dependent on the level of OS/2 the dump was created
with, please drop me a note so that I can verify I can read the dump.
Most likely we will then agree that you send me the dump via E-mail.
:eol.
:p.In addition to take a PC/2 process dump manually, such dumps are also generated by
OS/2 when a fatal unrecoverable error occurs within PC/2 (OS/2 does not take such dumps
by default unless you have specified the :hp4.DUMPPROCESS:ehp4. statement in your
:hp9.CONFIG.SYS:ehp9., PC/2 will request such a dump by default, unless you have
explicitely disabled PC/2's diagnostics).
:p.If a fatal unrecoverable error (e.g. a access violation) occurs, in addition
to the PC/2 process dump, the system error popup will be suppressed and the
information will be added to the file :hp9.POPUPLOG.OS2:ehp9. instead (OS/2 will
display a system error popup unless you have specified the :hp4.SUPPRESSPOPUPS.:ehp4.
statement in your :hp9.CONFIG.SYS:ehp9., PC/2 will request to replace the
system error popup by logging to this file, unless you have explicitely disabled
PC/2's diagnostics):
:ol.
:li.Due to a fatal unrecoverable error, OS/2 will terminate PC/2 without displaying a
message (OS/2 will display a popup message unless you have specified the
:hp4.SUPPRESSPOPUPS.:ehp4. statment in your :hp9.CONFIG.SYS:ehp9., PC/2 will request
to replace the popup message by logging to a file anyway).
If you note that PC/2 disappears without you having selected it to exit, most likely
such an error occurred.
:li.OS/2 will then append the trap information to the file named :hp9.POPUPLOG.OS2:ehp9.
located in the root of your boot drive.
This file is human readable, therefore you don't have to archive or encode it when
sending it vial E-mail (though it does not harm if you prefer to do so).
:li.Please send me this file via E-mail.
:eol.
:p.:hp2.In addition to any dump, I need to know exactly which version of the PC/2 modules
you are using:ehp2..
PC/2 modules contain the same signature as OS/2 modules, which can be read by running
the OS/2 :hp4.BLDLEVEL:ehp4. utility against these modules.
From an OS/2 commandline please capture the output of the :hp4.BLDLEVEL:ehp4. runs
(to capture the output by redirection to a file named :hp9.BuildLvl:ehp9., you would
enter):
:ul compact.
:li.BLDLEVEL PC2.EXE >BuildLvl
:li.BLDLEVEL PC2Hook.dll >>BuildLvl
:li.BLDLEVEL PC2BRes.dll >>BuildLvl
:li.BLDLEVEL PC2VRes.dll >>BuildLvl
:li.BLDLEVEL PC2Spool.dll >>BuildLvl
:li.BLDLEVEL Wps2PC2.dll >>BuildLvl
:eul.
:p.Please add this information if you report a problem too.
:warning text='Note:'.
Both, the files :hp9.POPUPLOG.OS2:ehp9. and :hp9.PDUMP.nnn:ehp9.
will not be cleared or removed automatically.
In order to reduce disk space (which is only a few kB anyway), you may want to delete
these files from time to time.
:ewarning.
:warning text='Note:'.
Unfortunately once PC/2 activates that dumps and fatal unrecoverable
errors will be appended to :hp9.PDUMP.nnn:ehp9. and :hp9.POPUPLOG.OS2:ehp9., these
settings will stay active even after PC/2 has terminated.
This will have the side effect, that once PC/2 has activated these setting, all dumps
will be saved into :hp9.PDUMP.nnn:ehp9. and fatal unrecoverable errors will be appended
to :hp9.POPUPLOG.OS2:ehp9. and no longer displayed with a system error popup, the
applications terminates silently.
There is now way to limit the activation of these settings to PC/2's process only.
:ewarning.
:warning text='Note:'.
Under Warp 4, the file :hp9.POPUPLOG.OS2:ehp9. will be written by default, regardless
of the setting :hp4.SUPPRESSPOPUPS:ehp4. (if this setting is active, the system error
popup just won't be displayed).
:ewarning.
.* You may also fill in the :link reftype=hd res=4017.<PC/2 Survey>:elink. to give
.* me some more general feedback.
:p.:link reftype=hd res=4005.<Backward>:elink.
                                :link reftype=hd res=4016.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected PC/2 trapinfo help.                                                *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

:h1 res=4016.PC/2 bug background information
It should be no excuse for problems PC/2 causes in your environment but some more
detailed information about the reasons why PC/2 may not work under all circumstances.
PC/2 not working may cause slowdowns, traps or hangs on your system.
If you carefully read the following lines you may be able to adjust your system to
allow better operation of PC/2.
:p.
:ul.
:li.Obtaining information about all windows
:p.In order to draw the Overview window, to switch to windows or between Virtual Desktops,
PC/2 uses APIs like WinBeginEnumWindows()/WinEndEnumWindows(), WinQuerySwitchList() and
WinQueryWindowText().
Between calling these APIs, the state of a window may have already changed, e.g. the
window's owner may change to HWND_OBJECT or the window is during its destruction processing,
in that cases WinQueryWindowText() may not return and the thread doing that will hang
(that is will be blocked).
The main symptom is that the Overview windows doesn't get redrawn anymore.
If this problem (admittedly seldomly) occurs, some functions of PC/2 no longer
work and the only save way to recover is to reboot (though your system is still operational
outside of PC/2).
:li.Stack usage for hook processing
:p.PC/2 installs hooks which allow to take part and even change the way events are processed
(and as most GUIs are event driven you can imagine that hooks are very powerful).
Unfortunately adding these hooks and data required to process them takes up a little more
stack as usual.
For applications (very few though) even that little more stack usage may be enough to cause
malfunctions.
If this problem occurs, random traps or hangs of that application or even PM may occur,
at a last resort you may have to stop using PC/2 while using such applications.
:li.Code and data added to other processes
:p.PC/2 adds code and data to other PM processes, on one hand through the hooks and on the
other hand by modifying the frame window procedures of frame windows (when adding the
Titlebar Smarticons).
If that code has a problem this may cause random traps or hangs of applications, you may
try to isolate that problem by running applications with and without running PC/2
concurrently.
Even if the code works perfectly there are situations where PC/2 can't close down, because
not being able to remove that code because e.g. the window has changed its owner to
HWND_OBJECT and no longer processes messages (including the ones required to allow PC/2 to
remove the code it has added).
You may selectively add applications to the Exception List for the Titlebar Smarticons
to find out which application is incompatible, or even disable that feature, however a
reboot may be required to get back to a clean state.
:li.PC/2 incompatible with other applications
:p.PC/2 may be incompatible with other utilities that make similar modifications to the
system by installing hooks or adding code and data.
Utilities like XIT, NPSWPS, ObjectDesktop, WarpEnhancer, CandyBar and even VoiceType
Dictation are the most prominent examples.
Though I don't say they are incompatible at all, if you have problems like random traps
and hangs this would be a starting point to look at.
:p.The main problem is what I would call a design limitation of PM.
When subclassing a window procedure, you receive the address of the original window
procedure and the new window procedure will be called instead.
When undoing subclassing you have to pass the orignal window procedure and PM then
knows that your new one should be removed.
However, when another application does also a subclassing after PC/2, but PC/2 undoes
the subclassing before the other application, PM will become confused not calling
required window procedures at all.
You can imagine that this likely can cause random traps or hangs.
:p.
PM expects subclassing to be used with the FIFO (first in, first out) principle, however
applications from different vendors have no way to ensure that as there is no standard
way how to implement subclassing cooperatively.
To my knowledge, only a change in PM to replace the FIFO behaviour with a linked list
can change this.
:eul.
:p.:link reftype=hd res=4015.<Backward>:elink.
                                :link reftype=hd res=306.<Forward>:elink.
.*                                 :link reftype=hd res=4017.<Forward>:elink.

.****************************************************************************************
.*--------------------------------------------------------------------------------------*
.* The user selected PC/2 Survey help.                                                  *
.*--------------------------------------------------------------------------------------*
.****************************************************************************************

.* :h1 res=4017.PC/2 Survey
.* :p.PC/2 is Freeware, but I would like to hear from you to get some feedback about
.* the goodies and badies of PC/2, so please E-mail me the PC/2 Survey, which can
.* be found in the directory you installed PC/2 into.
.* :p.Please load the PC/2 Survey :hp4.Survey.doc:ehp4. into your favourite editor, fill
.* in the questionaire and E-mail it back to me.
.* :p.:link reftype=hd res=4016.<Backward>:elink.
.*                                 :link reftype=hd res=306.<Forward>:elink.


:euserdoc.

