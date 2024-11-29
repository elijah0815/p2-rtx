<h1 align="center">Portal 2 RTX Remix Compatibility Mod</h1>

<div align="center" markdown="1"> 

This client modification is specifically made for nvidia's [rtx-remix](https://github.com/NVIDIAGameWorks/rtx-remix).  
How does a shader based game work with remix? By manually reimplementing fixed function rendering :) 

<br>

__WIP__ & __Please Note:__  
RTX Remix was never intented to support Portal 2.  
If you encounter crashes, broken things or similar, it's either due to incompatibility, the limits of fixed function rendering (not being able to get the same result that shaders can achieve)  
or due to bugs in the compatibility mod itself.  

<br>

### This is not trying to be a remaster.
It simply makes the game compatible with RTX Remix.

Please keep that in mind.

</div>

<div align="center" markdown="1">

<img src="img/logo.png" alt="Description" width="80%">
</div>

<div align="center" markdown="1">

### __[ Remix Compatibility Features ]__   
🔹Most things are rendered using the fixed-function pipeline🔹  
🔹Remix friendly culling and the ability to manually override culling per visleaf🔹  
🔹Per map loading of remix config files to set remix variables🔹  
🔹Ability to animate remix variables on events🔹  
🔹Spawning of unique anchor meshes🔹  
🔹Per map fog settings🔹  

<br>
<br>

If you want to support my work, consider buying me some coffee:  

[![ko-fi](https://xoxor4d.github.io/assets/img/social/kofi.png)](https://ko-fi.com/xoxor4d)
</div>

<br>
<br>

<div align="center" markdown="1">

![img](img/01.png)
![img](img/04.png)
</div>

<br>

# Usage / Installation
- Download the latest [release](https://github.com/xoxor4d/p2-rtx/releases) and follow instructions found there.
- The `.zip` with the commit hash contains the compatibility mod and the `_remix_mod.zip` contains a base remix-mod required to get portals to work (and a few emissive textures etc.)
- To avoid rebuilding the sound cache on first startup, copy `_master.cache` from  
`root/portal2/maps/soundcache` to `root/portal2_dlc3/maps/soundcache`
- Start the game by executing `_start_portal2_rtx.bat`
- The window title should change to `Portal 2 - RTX` followed by the GitHub commit number if successful

- [Set graphic options](#graphic-settings) ⚠️ 
> - Shader Detail: Low  
> - Effect Detail: Low  
> - Model /Texture Detail: High  
> - Paged Pool Memory: High  

<br>

#### ⚠️ Info: 
- See the [Wiki](https://github.com/xoxor4d/p2-rtx/wiki/Compatibility-Mod-Feature-Guide) for in-depth guides on features that come with the compatibility mod 🍓
- Current releases ship with a [custom build of the remix-dxvk runtime](https://github.com/xoxor4d/dxvk-remix/tree/combine/pairs_mask_rs) which includes necessary changes  
for Portal 2 (`bin/.trex/d3d9.dll`)  
- Releases also include [Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases) (`bin/winmm.dll`)  

<br>

#### ⚠️ Troubleshooting:
- Unable to load `_rtx.asi`:  
Download and install [DirectX End-User Runtimes (June 2010)](https://www.microsoft.com/en-ie/download/details.aspx?id=8109)

- Portals not showing up or looking incorrect:  
Make sure that you installed the `_remix_mod.zip`. See [Issue #13](https://github.com/xoxor4d/p2-rtx/issues/13)

- Game is too dark, sunlight is leaking on certain maps, textures look flat:  
This is not an issue with the compatibility mod itself. It's up to the people modding the game using the remix toolset to place proper lights and overhaul textures and meshes.

- Look at [Closed Issues](https://github.com/xoxor4d/p2-rtx/issues?q=is%3Aissue+is%3Aclosed) to see if people had similar but now resolved issues.

<br>

#  Credits
- [Nvidia - RTX Remix](https://github.com/NVIDIAGameWorks/rtx-remix)
- [People of the showcase discord](https://discord.gg/j6sh7JD3v9) - especially the nvidia engineers ✌️
- All early access people for testing/bug reporting and for covering my electricity bill ⚡
- [Wolƒe Strider Shoσter](https://github.com/wolfestridershooter) - for all the high quality bug reports! 
- Yosuke Nathan - Portal 2 Remix Logo
- [Minhook](https://github.com/TsudaKageyu/minhook)
- [Toml11](https://github.com/ToruNiina/toml11)

<br>

<div align="center" markdown="1">

![img](img/02.png)
![img](img/03.png)
![img](img/05.png)
</div>


<br>
<br>


# Graphic-Settings

<div align="center" markdown="1">

<img src="img/settings.png" alt="Description" width="70%">
</div>
