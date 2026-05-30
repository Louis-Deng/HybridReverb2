cmake_minimum_required(VERSION 3.22)

project(hybridrevjo VERSION 2.21)
set(HYBRIDREVJO_OUTPUT_NAME "Rebirberator2")

set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE" CACHE PATH "Path to JUCE")
if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
    set(JUCE_PATH "/Users/louisdeng/Documents/JUCE" CACHE PATH "Path to JUCE" FORCE)
endif()

add_subdirectory("${JUCE_PATH}" JUCE)

# Uncomment and set path when building AAX:
# juce_set_aax_sdk_path(/path/to/AAX_SDK)

juce_add_plugin(hybridrevjo
    COMPANY_NAME "Louis DSP"
    COMPANY_WEBSITE "https://github.com/Louis-Deng"
    BUNDLE_ID "com.louisdsp.hybridrevjo"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    PLUGIN_MANUFACTURER_CODE LDSP
    PLUGIN_CODE Hrb2
    PLUGIN_NAME "${HYBRIDREVJO_OUTPUT_NAME}"
    PRODUCT_NAME "${HYBRIDREVJO_OUTPUT_NAME}"

    FORMATS AU VST3 Standalone
    AU_MAIN_TYPE kAudioUnitType_Effect
    VST3_CATEGORIES Fx Reverb Stereo
    AAX_CATEGORY Reverb Effect

    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(hybridrevjo)

juce_add_binary_data(hybridrevjoBinaryData
    SOURCES
        Source/IR/aula.wav
        Source/IR/lecture.wav
        Source/IR/meeting.wav
        Source/IR/office.wav
        Source/IR/stair.wav
)

target_sources(hybridrevjo
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/ParameterManager.cpp
)

target_compile_definitions(hybridrevjo
    PUBLIC
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(hybridrevjo
    PRIVATE
        hybridrevjoBinaryData
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
