# Nuitrack for Unreal Engine

![](./Docs/trail-example.gif)

Unreal Engine plugin to bridge [Nuitrack](https://nuitrack.com/). Nuitrack is a middleware to provide 3d skeleton tracking solution using a depth sensor. It has many PROs: AI-based, relatively fast and stable, coverage of depth sensor brands. Nuitrack has [an official plugin](https://github.com/3DiVi/nuitrack-sdk/tree/master/UnrealEngine) for Unreal Engine which still only provides native C++ interface. This plugin aims to encapsulate Nuitrack's C++ functions and align with Unreal Engine manner. 

## Prerequisites

* [Nuitrack](https://nuitrack.com/) is installed.
    * Free Trial version will work but timeout within 3 minutes.
* Replace the TBB library in your Unreal Editor with the library from Nuitrack
    * follow [this](https://github.com/3DiVi/nuitrack-sdk/blob/master/doc/Troubleshooting.md#unreal-engine)
* Locate ["NuitrackModule" directory from original SDK repo](https://github.com/3DiVi/nuitrack-sdk/tree/master/UnrealEngine/NuitrackPlugin/Source/NuitrackModule)  into this plugin's `/Source/` directory.
* Unreal Engine 4.27~
    * Only tested with 4.27. May work with lower.
## Features

Almost all implementations are provided via `NuitrackIO` UObject.

### Data acquisition

* Color / Depth / User buffer can be written into `RenderTarget2D`.
    * Depthe data are stored RenderTarget2D into standard 8bit RGBA texture.  
    R: first 8bit as uint8 of original uint16 sample  
    G: last 8bit as uint8 of original uint16 sample  
    B: 0x00 or 0xFF (if depth sample is invalid)  
    A: 0xFF (Constant value)
* Skeleton tracking, Hand tracking, Gesture recognizer are accessible through Blueprint.

![](./Docs/bp-activation.png)

![](./Docs/bp-sample.png)

### In-Editor activation

* `NuitrackIO` and `NuitrackVisualizer` Actor will work in editor time

![](./Docs/editor-activation.gif)

### Avatar

* Skeletal mesh will work via Anim blueprint
* See `ABP_NuitrackToHumanoid` which is compatible with Manequine in ThirdPerson sample.

![](./Docs/avatar.gif)

# License
## MIT License
Copyright 2021 Ayumu Nagamtsu

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

