[![Build status](https://ci.appveyor.com/api/projects/status/github/Thracky/GloSC?svg=true)](https://ci.appveyor.com/project/Thracky/glosc) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) [![Github All Releases](https://img.shields.io/github/downloads/Alia5/GloSC/total.svg)]() [![Discord](https://img.shields.io/discord/368823110817808384.svg)](https://discord.gg/T9b4D5y)


![GloSC logo](https://github.com/Alia5/GloSC/blob/master/GloSC_Icon_small.png?raw=true "GloSC logo")

# GloSC

GloSC or Global SteamController is a tool that allows you to use your SteamController as a system wide XInput-controller alongside a system wide (borderless window) Steam overlay

All complete with **per application bindings and working rumble emulation.**

GloSC can, but isn't required to, launch any of your favorite games or applications and directly add them to Steam, be it Win32 or UWP!

It is **the tool** to enjoy any game that has trouble with Steam and/or *add extra functionality* to your SteamController

*UWP*, *Reshade / SweetFX*, *Origin* and *Uplay* and *more* with **no hassle**

---

**How does it work? / What does it do?**

GloSC creates and adds a (or multiple) non-Steam shortcuts to Steam. When one of those is launched, a transparent, borderless window appears in which you can use the Steam-overlay. You also get access to touch- and radial-menus and other functionality normally only present in Games

In addition to that, XInput-inputs are redirected to the whole operating system, so that they will work with any game or application

This brings full SteamController functionality to the desktop and any other application the SteamController might not have worked before

---

**Cleaning up misconceptions**

GloSC doesn't hook into any of your games, launched programs or any system-component, except into Steam itself to keep the controller from switching to the desktop-config (if not disabled)

Games do not need to be launched using GloSC.  

If the "Start Application" option does not work, launch any GloSC-Shortcut from Steam, followed by a game or application.


```
```

## FAQ / Troubleshooting

* **Q: The overlay isn't showing up!**

  A: The overlay only works for windowed or borderless windowed mode applications.
     This is even true for UWP-Games! While it's true that UWP doesn't run exclusive fullscreen, some Windows bullshit prevents unsigned apps from drawing over them.
     
     Try running your games in borderless window mode

* **Q: GloSC always creates four controllers, resulting in doubled inputs**

  A: Enable Xbox360 controller rebinding in Steam! It's in the "Controller" section in the Steam Settings. 

* **Q: Hitting the Steam Button + RT to take a screenshot doesn't work**

  A: Another limitation that most likely won't be fixed, even if Steam would take a screenshot, you'd most likely get just a blank image

* **Q: The controller doesn't use my config**

    A: In some cases the privileges of the launched application are higher than Steams, switching the controller to "Lizard-Mode"
    Try running Steam **as Admin**
    
* **Q: GloSC overlay causes the screen to be black**

  A: If running on mobile, open up NVidia control panel (right click your Desktop and select it). Manage the 3D-settings and have it auto select your graphics processor.

  Alternatively there is the _`bEnableOverlayOnlyConfig`_ setting in GloSC's .ini files.
  If this flag is set, the overlay is only shown if it is opened.
  This will, unfortunately, also prevent you from using touch- and radial-menus.

  Instructions for AMD GPUs should be similar, in case any issues occur.        
     
```
```

Join the GloSC discord here: https://discord.gg/T9b4D5y

---

**GloSC consists of:**
 
 * A main application ("GloSC") handling shortcut ("SteamTarget") creation and their addition to Steam.
 * The "SteamTarget" which does most of the magic - Showing the overlay to the user as well as talking to the ViGEm-driver for system wide Controller emulation
 * A GloSC Watchdog making sure Steam get's unhooked once a "SteamTarget" is closed.
 * EnforceBindingDLL that gets injected into Steam, preventing the controller switching to the desktop-config

---

Like my stuff? Hit me up [on twitter](https://twitter.com/Flatspotpics) or consider donating to my [PayPal](https://www.paypal.me/Flatspotpics)

GloSC got mentioned from Valve in the [Steam client beta change log on the 9. of January](https://twitter.com/flatspotpics/status/818697837055770624)

You can learn more about how GloSC works [here](https://behind.flatspot.pictures/third-party-steam-controller-software-part2-my-take-on-it/)

For Building / Manual installation refer to the [build guide](https://github.com/Alia5/GloSC/blob/master/Build_Install.md)

GloSC is not affiliated with Valve, Steam, or any of their partners.

---

GloSC is built using [Qt 5.X](https://www.qt.io/) and [SFML](http://www.sfml-dev.org/) for drawing the overlay

The system wide Xbox-Controller works via [ViGEm](https://github.com/nefarius/ViGEm)


## License

```
Copyright 2017-2018 Peter Repukat - FlatspotSoftware

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
