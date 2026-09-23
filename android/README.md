# Neverball for Meta Quest

Standalone OpenXR builds of Neverball and Neverputt for Meta Quest 3,
packaged as two Android APKs. They are separate applications because the
Horizon launcher lists applications and not the games inside them; the port
itself is shared, and lives entirely in `share/`.

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
| Right thumbstick | turn the camera left or right, one step per push; up and down do nothing (Neverball) |
| Trigger | click what the ray is on, or take the highlight if it is on nothing |
| A | take the highlight, wherever the ray is pointing |
| B | back |
| X | pause |
| Y | options |
| Grip | turn the camera left or right |

Setting `vr_control` to 1 tilts the floor by turning the wrist that holds
the right controller instead of with the thumbstick. The stick still moves
the menu highlight either way.

Neverputt aims and putts with the same buttons and has no floor to tilt, so
`vr_tilt_visual` and `vr_control` do nothing there. It has no camera
separate from the aim either, so the right stick steps the whole view round
by `vr_snap_turn` degrees while a stroke is being lined up, and the fine
aiming a shot actually needs stays on the left stick.

Recentring is the system's own: hold the Meta button. The interface panel is
placed relative to the reference space origin, so it follows.

## Comfort

A chase camera at life size is the uncomfortable case, so these are settings
rather than decisions. They are on the **VR Comfort** screen under Options,
Graphics, and in `neverballrc` as:

    vr_tilt_visual   30   percent of the world roll to actually show
    vr_vignette       1   close a black ring in from the edges while moving
    vr_snap_turn     45   degrees per step of manual turning; 0 turns smoothly
    vr_render_scale 100   percent of the runtime's recommended per-eye size

`vr_tilt_visual` only changes the picture. `game_tilt_grav()` on the server
is untouched, so the physics, the difficulty and the times are identical to
the flat game at any setting.

`vr_render_scale` is read once, when the swapchains are built, so it only
takes effect on a restart -- which is why it is not on the settings screen.
The runtime recommends 2800x2933 per eye on a Quest 3 and the game has the
budget to take it; the dial is there for a level that turns out to disagree.

The graphics options that describe a window -- display, resolution,
fullscreen, v-sync, antialiasing -- are hidden in a headset, where the
compositor owns the display and the window is never presented to.

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

## Pico

The same games build for Pico headsets. Each APK comes in a `quest` and a
`pico` flavour, which differ only in the manifest keys the two launchers
read; the code is the same, and binds the Pico controllers through
`XR_BD_controller_interaction` when the runtime offers it. `build.sh` builds
both flavours, and `./gradlew assemblePicoRelease` builds only the Pico ones.

## Building

Prerequisites: Android SDK with NDK 27.2, platform 34 and CMake 3.22.1; a
full JDK (not just a JRE -- the Android Gradle plugin needs jlink). A host
toolchain with libpng, libjpeg and zip is needed to compile and package the
level data.

    android/build.sh

That fetches and builds the third-party sources, compiles the `.map` files
to `.sol`, packages the game data and builds both APKs. Each step is skipped
if it has already been done, so it is also the quick way to rebuild after
editing the game. The steps by hand, if you would rather:

    android/deps/fetch-deps.sh      # pinned third-party sources
    android/deps/build-gl4es.sh
    android/deps/build-deps.sh
    make -j sols                    # 429 .map files, on the host
    android/make-assets.sh          # the data that ships in the APKs
    cd android && ./gradlew assembleDebug

The `.sol` compilation must run on the host: `mapc` is a build tool, and the
root Makefile has no host/target split. This mirrors what CI already does for
the web build (`.github/workflows/web-deploy.yml`).

## Releases

    android/make-keystore.sh        # once, then keep the key
    android/build.sh release

Android will not install an unsigned APK, so a release build needs a signing
key even to be sideloaded. `make-keystore.sh` creates one and writes
`keystore.properties` beside it; both are ignored by git, because a signing
key is a credential. Keep them somewhere safe -- losing the key means the
next release cannot be installed as an update over this one.

Without a key, `assembleRelease` still runs and leaves an unsigned APK,
which is useful for inspecting a build but cannot be installed.

The version comes from `scripts/version.sh`, so a build cannot claim to be
something it is not.

## Installing

    adb install -r android/ball/build/outputs/apk/quest/debug/ball-quest-debug.apk
    adb install -r android/putt/build/outputs/apk/quest/debug/putt-quest-debug.apk

On a Pico, install the `pico` APKs from the matching `pico` directories
instead.

    adb shell am start -n org.neverball/.NeverballActivity
    adb shell am start -n org.neverputt/.NeverputtActivity
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

    build.sh         one command, from a clean checkout to two APKs
    make-keystore.sh creates the release signing key, once
    deps/            fetch and build scripts for the third-party sources, all pinned
    make-assets.sh   packages the game data that ships in the APKs
    ball/, putt/     one Gradle module and one APK each; their
                     CMakeLists.txt keep their own source lists, in the
                     spirit of emscripten/ball.mk, rather than including
                     the root Makefile
    spike/           throwaway M0 spike proving OpenXR + gl4es + SDL coexist
