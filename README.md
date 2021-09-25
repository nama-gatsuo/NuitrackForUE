# Nuitrack for Unreal Engine

![](./Docs/trail-example.gif)

Unreal Engine plugin to bridge [Nuitrack](https://nuitrack.com/). Nuitrack is a middleware to provide 3d skeleton tracking solution using a depth sensor. It has many PROs: AI-based, relatively fast and stable, coverage of depth sensor brands. Nuitrack has [an official plugin](https://github.com/3DiVi/nuitrack-sdk/tree/master/UnrealEngine) which still only provides native C++ interface. This plugin aims to encapsulate Nuitrack's C++ functions and align with Unreal Engine manner. 

## Prerequisites

* [Nuitrack](https://nuitrack.com/) is installed.
    * Free Trial version will work but timeout within 3 minutes.
* Replace the TBB library in your Unreal Editor with the library from Nuitrack
    * follow [this](https://github.com/3DiVi/nuitrack-sdk/blob/master/doc/Troubleshooting.md#unreal-engine)
* Unreal Engine 4.27~
    * Only tested with 4.27. May work with lower.
* Locate ["NuitrackModule" directory](https://github.com/3DiVi/nuitrack-sdk/tree/master/UnrealEngine/NuitrackPlugin/Source/NuitrackModule) from SDK repo into this plugin's `/Source/` 

## Features

### In-Editor activation

* Color buffer into `RenderTarget2D`s. 

![](./Docs/editor-activation.gif)

### Blueprint activation

![](./Docs/bp-activation.png)

## TODO
* Avatar controll (Skeletal mesh controll via AnimGraph)