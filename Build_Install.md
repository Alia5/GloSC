# Building GloSC

**Build requirements:**
- M$ Visual Studio (2019, Community is fine)
- [Qt 5.13.2](http://info.qt.io/download-qt-for-application-development) (x86)
- [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools)
- [ViGEm](https://github.com/nefarius/ViGEm)
- [Windows 10 SDK version 10.0.18362.0](https://go.microsoft.com/fwlink/?linkid=2083338)

ViGEms user mode library as well as [SFML](http://sfml-dev.org) are bundled in the repo.
For ViGEms installation refer to the Readme-files from the relevant sub-projects, or use provided install script (in redist)

--

Set the correct Qt version for "*GloSC*", and "*SteamTarget*" depending on your build.

Rebuild the solution and you should be good to go.

# Installation

Copy all **files** out of the relevant build directories, as well as, "*TargetConfig.ini*" and the relevant SFML and QT .dll-files in a separate directory.

Afterwards create your shortcuts with GloSC and you are ready to go.


