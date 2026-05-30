# HybridReverb2
A Hybrid Reverberation using JUCE Framework
product as an AU/VST/AAX plugin to use within DAW

due to certificate problems, you'd have to build the plugin on your local computer. 



# Source
Built using JUCE 8.0.1 and Xcode 15.4
To build for yourself, see JUCE documentation on how to use Projucer. 


# Debug and Build using Cursor and CMake 2.21c
Debug (development)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target hybridrevjo_AU -j
cmake --build build --target hybridrevjo_VST3 -j
cmake --build build --target hybridrevjo_Standalone -j

Output: build/hybridrevjo_artefacts/Debug/...
AU install (with COPY_PLUGIN_AFTER_BUILD): ~/Library/Audio/Plug-Ins/Components/Rebirberator 2.component

Validate:
auval -v aufx Hrb2 LDSP

Release (shipping / performance testing)
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target hybridrevjo_AU -j
cmake --build build-release --target hybridrevjo_VST3 -j
cmake --build build-release --target hybridrevjo_Standalone -j

Output: build-release/hybridrevjo_artefacts/Release/...
Release AU is also copied to ~/Library/Audio/Plug-Ins/Components/ on build.

Cursor tasks
Build AU (Debug) — default build task
Build AU (Release)
Build Standalone (Debug) / (Release)
Validate AU — runs auval after a debug AU build

# Version 2.06
stripped unnecessary code, prefiltering button removed for further refinement (does not affect audio)


# Version 2.05
AU effect plugin
supports mono->mono, mono->stereo, stereo->stereo channel configuration
supports native arm64, intel64 buildable
2026: added ParameterManager support to initialize parameter without UI
