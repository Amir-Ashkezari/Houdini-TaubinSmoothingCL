# Taubin Smoothing CL

A custom houdini operator, do a very fast neighbours lookup and smooths out polygons (using opencl) while preserving the main details.

### Build and the Installation

#### 1. Install prerequisites

- Required:
    - C++ compiler:
        - gcc
        - Xcode
        - Microsoft Visual Studio
    - CMake
    - SideFX Houdini

#### 2. Download the source code

You can download source code archives from [GitHub](https://www.github.com/Amir-Ashkezari/Houdini-TaubinSmoothingCL) or use ```git``` to clone the repository.

```
> git clone https://github.com/Amir-Ashkezari/Houdini-TaubinSmoothingCL
```

#### 3. Building the source code

[Getting Started with the HDK](https://www.sidefx.com/tutorials/quick-tip-getting-started-with-the-hdk/)

#### 4. Icon and helpcard

Copy and paste "config" and "help" folders in the houdini preferences folder.

Default paths of preferences folder:
  * linux: /home/username/houdini*.*
  * mac: /Users/username/Library/Preferences/houdini/*.*
  * windows: C:\Users\username\Documents\houdini*.*

### Bugs/Issues

If you found any bugs or issues, leave a comment on the "Issues" tab
