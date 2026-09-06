# Neverball for Meta Quest

A standalone OpenXR build of Neverball for Meta Quest 3, packaged as an
Android APK.

The level is presented as a tabletop diorama: a miniature roughly a metre
across, anchored in the room in front of you. Your viewpoint never moves on
its own, which is what makes a tilt game comfortable in a headset.

## How it fits together

Neverball's renderer is fixed-function OpenGL 1.x. Rather than rewrite it,
this build links [gl4es](https://github.com/ptitSeb/gl4es), which translates
OpenGL 1.x to OpenGL ES 2.0 — the same approach the web build already uses
(`share/video.c:279-285`).

Stereo rendering reuses the HMD abstraction that has been in the tree since
2013 (`share/hmd.h`): `share/state.c` already paints the scene once per eye,
and projection and head pose have exactly one injection point each. The
OpenXR backend slots in beside `hmd_null.c`.

SDL2 supplies the Android lifecycle, JNI, audio and font handling. OpenXR
owns the display; SDL's window exists but is never presented to.

## Building

Prerequisites: Android SDK with NDK 27.2, platform 34 and CMake 3.22.1;
JDK 17+; Gradle 8.2+. A host toolchain with libpng and libjpeg is needed to
compile the level data.

    # One-time: fetch and build third-party sources.
    android/deps/fetch-deps.sh
    android/deps/build-gl4es.sh

    # Compile the 429 .map files to .sol using the host mapc.
    make -j sols

    # Build the APK.
    cd android && gradle :app:assembleDebug

The `.sol` compilation must run on the host: `mapc` is a build tool, and the
root Makefile has no host/target split. This mirrors what CI already does for
the web build (`.github/workflows/web-deploy.yml`).

## Installing

    adb install -r android/app/build/outputs/apk/debug/app-debug.apk
    adb shell am start -n org.neverball/.NeverballActivity
    adb logcat -s Neverball:V NeverballSpike:V SDL:V

## Layout

    deps/            fetch and build scripts for SDL2 and gl4es, all pinned
    app/CMakeLists.txt   standalone build description, in the spirit of
                         emscripten/ball.mk — it does not include the root Makefile
    spike/           throwaway M0 spike proving OpenXR + gl4es + SDL coexist
