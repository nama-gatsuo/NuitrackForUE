# Nuitrack for Unreal Engine

![](./Docs/trail-example.gif)

Unreal Engine plugin to bridge [Nuitrack](https://nuitrack.com/). Nuitrack is a middleware to provide 3d skeleton tracking solution using a depth sensor. It has many PROs: AI-based, relatively fast and stable, coverage of depth sensor brands. Nuitrack has [an official plugin](https://github.com/3DiVi/nuitrack-sdk/tree/master/UnrealEngine) which still only provides native C++ interface. This plugin aims to encapsulate Nuitrack's C++ functions and align with Unreal Engine manner. 

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

### In-Editor activation

* Color buffer can be written into `RenderTarget2D`.
* `NuitrackVisualizer` Actor also works in editor time.

![](./Docs/editor-activation.gif)

### Blueprint activation

![](./Docs/bp-activation.png)

* Data accessor such as 
    * Skeleton tracking results
    * Hand tracking results

## TODO
* Avatar controll (Skeletal mesh controll via AnimGraph)