# Neverball for Meta Quest

A standalone OpenXR build of Neverball for Meta Quest 3, packaged as an
Android APK.

The level is presented at life size -- one world unit is one metre, so the
ball is a 50 cm sphere -- and you ride the game's own chase camera, floating
a couple of metres behind it. The comfort work that a moving viewpoint needs
is a layer on top of that rather than a change of presentation.

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
owns the display; SDL's window exists but is never presented to. The EGL
display, config and context handed to `xrCreateSession` are recovered from
the context SDL created, which is what keeps gl4es initialised the way the
web build already proved works.

## Controls

| | |
|---|---|
| Left thumbstick | tilt the floor, and move the menu highlight |
| Right thumbstick | turn the camera left or right, one step per push; up and down do nothing |
| Point and trigger | click whatever the ray is on |
| A | select |
| B | back |
| X | pause |
| Y | options |
| Grip | turn the camera left or right |

Setting `vr_control` to 1 tilts the floor by turning the wrist that holds
the right controller instead of with the thumbstick. The stick still moves
the menu highlight either way.

Recentring is the system's own: hold the Meta button. The interface panel is
placed relative to the reference space origin, so it follows.

## Comfort

A chase camera at life size is the uncomfortable case, so the settings that
matter are in `neverballrc` rather than compiled in:

    vr_tilt_visual   30   percent of the world roll to actually show
    vr_vignette       1   close a black ring in from the edges while moving
    vr_snap_turn     45   degrees per step of manual turning; 0 turns smoothly

`vr_tilt_visual` only changes the picture. `game_tilt_grav()` on the server
is untouched, so the physics, the difficulty and the times are identical to
the flat game at any setting.

## gl4es and the swapchain images

gl4es virtualizes texture names. `gl4es_getTexture()` looks a name up in its
own table and, for one it did not create, quietly generates a fresh real
texture and remaps to it. Handing it an OpenXR swapchain image therefore
produces a framebuffer that reports itself complete, that clears and draws
without raising an error, and that the compositor never sees -- a black
headset with a healthy-looking frame rate.

Framebuffer names are not virtualized, so `share/hmd_openxr.c` builds the
eye framebuffers with the driver's own entry points, taken by `dlsym` since
gl4es owns the ones the game links against. gl4es then does nothing with
them but bind them by name and draw.

Anything else that hands gl4es an object it did not create will need the
same treatment.

## Building

Prerequisites: Android SDK with NDK 27.2, platform 34 and CMake 3.22.1; a
full JDK (not just a JRE -- the Android Gradle plugin needs jlink). A host
toolchain with libpng, libjpeg and zip is needed to compile and package the
level data.

    # One-time: fetch and build third-party sources.
    android/deps/fetch-deps.sh
    android/deps/build-gl4es.sh
    android/deps/build-deps.sh

    # Compile the 429 .map files to .sol using the host mapc.
    make -j sols

    # Package the game data that ships inside the APK.
    android/make-assets.sh

    # Build the APK.
    cd android && ./gradlew :app:assembleDebug

The `.sol` compilation must run on the host: `mapc` is a build tool, and the
root Makefile has no host/target split. This mirrors what CI already does for
the web build (`.github/workflows/web-deploy.yml`).

## Installing

    adb install -r android/app/build/outputs/apk/debug/app-debug.apk
    adb shell am start -n org.neverball/.NeverballActivity
    adb logcat -s Neverball:V NeverballSpike:V SDL:V

The game also writes its own log to `files/neverball.log` in the app's
private storage, which survives logcat's ring buffer rolling over:

    adb shell "run-as org.neverball cat files/neverball.log"

Horizon OS puts the headset to sleep when it is not being worn, and the
session then leaves the rendering states, which is exactly when the game
stops stepping. To exercise a build without wearing it:

    adb shell am broadcast -a com.oculus.vrpowermanager.prox_close

Horizon OS also refuses to launch the app outright once the controllers have
gone to sleep -- look for `RequiresControllersLaunchInterceptor` in logcat.
Nothing over adb wakes them; press a controller button or pick one up.

## Layout

    deps/            fetch and build scripts for the third-party sources, all pinned
    make-assets.sh   packages the game data that ships in the APK
    app/CMakeLists.txt   standalone build description, in the spirit of
                         emscripten/ball.mk — it does not include the root Makefile
    spike/           throwaway M0 spike proving OpenXR + gl4es + SDL coexist
