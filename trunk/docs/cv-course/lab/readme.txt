The folder contains source code and supplementary files
for simple projects using OpenCV.

Files description
------------------

skeleton/* - source code and project files of the skeleton OpenCV-based program
             that reads images and displays them.
squares/* - source code and project files of the squares program - much more
            advanced variant of skeleton program. It finds squares in the loaded images
lab.dsw - workspace for Microsoft Developer Studio 6.0

pic*.bmp - sample images used in demos


Build Instructions for Win32:
-----------------------------

    + Install OpenCV beta2.1 or later version
    + Copy lab folder to opencv\ (root OpenCV folder)
    + Open lab.dsw
    
    + Select skeleton project as active
      (right click on the project name in the workspace pane -> "Set as Active Project")
    + Build it (F7)
    + Run it (F5 or Ctrl-F5)
    
    + Select square project as active
      (right click on the project name in the workspace pane -> "Set as Active Project")
    + Build it (F7)
    + Run it (F5 or Ctrl-F5)

    + You may also select active project and build configuration (Debug or Release)
      via menu: "Build"->"Set Active Configuration..."

To create your own OpenCV-based project in Developer Studio
from scratch do the following:

    + Within Developer Studio create new application:
      - select from menu "File"->"New..."->"Projects" tab.
        Choose "Win32 Application" or "Win32 console application" - the latter is
        the easier variant and the both sample projects have this type.

      - type the project name and choose location
      - you may create own workspace for the project ("Create new workspace")
        or include the new project into the currently loaded workspace
        ("Add to current workspace").
      - click "next" button
      - choose "An empty project", click "Finish", "OK".

      After the above steps done Developer Studio will create the project
      folder (by default it has the same name as the project),
      <project name>.dsp file and, optionally, <project name>.dsw,.ncb ... files
      if you create own workspace.
    
    + Add a file to the project:
      - select from menu "File"->"New..."->"Files" tab.
      - choose "C++ Source File", type file name and press "OK"
      - add OpenCV-related #include directives:
        
        #include "cv.h"
        /* #inlcude "cvaux.h" // experimental stuff (if need) */
        #include "highgui.h"

      Or, you may copy some existing file (say, skeleton.c) to the project folder,
      open it and add to the project
      (right click in editor view -> "Insert File into Project" -> <your project name> ).
      
    + Customize project settings:
      - Activate project setting dialog by choosing menu item
        "Project"->"Settings...".
      - Select your project in the right pane.
      - Tune settings, common to both Release and Debug configurations:
        * Select "Settings For:"->"All Configurations"
        * Choose "C/C++" tab -> "Preprocessor" category -> "Additional Include Directories:".
          Add comma-separated relative (to the project folder) or absolute paths
          to opencv\cv\include, opencv\otherlibs\highgui and, optionally,
          opencv\cvaux\include.
          E.g. for lab\skeleton project relative path to
          opencv\cv\include would be ..\..\cv\include if the lab folder is placed in
          the root opencv folder.
        * Choose "Link" tab -> "Input" category -> "Additional library path:".
          Add the paths to all neccessary import libraries
          (..\..\lib in case of lab\skeleton project).
          If you plan to use IPL, add path to opencv\otherlibs\_ipl\lib\msvc as well
          (..\..\otherlibs\_ipl\lib\msvc for lab\skeleton).
       - Tune settings for "Debug" configuration
        * Select "Settings For:"->"Win32 Debug". 
        * Choose "Link" tab -> "General" category -> "Object/library modules".
          Add space-separated cvd.lib, highguid.lib, cvauxd.lib (optionally),
          ipl.lib (optionally)
        * You may also want to change location and name of output file. For example,
          if you want the output .exe file to be put into the project folder, rather
          than Debug/ subfolder, you may type ./<exe-name>d.exe in
          "Link" tab -> "General" category -> "Output file name:".
       - Tune settings for "Release" configuration
        * Select "Settings For:"->"Win32 Release". 
        * Choose "Link" tab -> "General" category -> "Object/library modules".
          Add space-separated cv.lib, highgui.lib, cvaux.lib (optionally),
          ipl.lib (optionally)
        * Optionally, you may change name of the .exe file:
          type ./<exe-name>.exe in "Link" tab -> "General" category -> "Output file name:".
     
    + Add dependency projects into workspace:
       - Choose from menu: "Project" -> "Insert project into workspace".
       - Select opencv\cv\make\cv.dsp.
       - Do the same for opencv\cvaux\make\cvaux.dsp, opencv\otherlibs\highgui\highgui.dsp.
       - Set dependencies:
        * Choose from menu: "Project" -> "Dependencies..."
        * For "cvaux" choose "cv",
          for "highgui" choose "cv",
          for your project choose all: "cv", "cvaux", "highgui".

       The dependencies customization allows to automatically build debug versions
       of opencv libraries and rebuild the binaries if the sources are changed somehow.
          
       That's it.

Build instructions for Linux:
-------------------------------

    + Install and build opencv-0.9.3 or higher
      (unpack, in opencv-0.9.3 type
        $ ./configure
        $ make
        $ su
        # make install
        # ldconfig)
      By default, opencv is installed into /usr/local, so, make sure that
      /usr/local/lib is in /etc/ld.so.conf.
    + Unpack the course folder
    + in lab folder type
        $ make
        $ cd skeleton
        $ ./skeleton
        $ cd ../squares
        $ ./squares

-----------------------------------------------------------------------
-----------------------------------------------------------------------

>>TODO:<<
    - how to create OpenCV-based project under linux
    - how to build demos using Borland compiler
    - how to get video capture (cvcam - adopt code from StereoDemo?)





