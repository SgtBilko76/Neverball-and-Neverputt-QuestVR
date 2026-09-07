# Make a Neverputt data package. SOLs must already be compiled.
#
# Neverputt shares almost everything with Neverball but reads a different
# slice of it: courses rather than level sets, and the map directories those
# courses name. It ships as one package because there is no download system
# behind it here -- see android/make-assets.sh.

DATA_DIR := ./data
OUTPUT_DIR ?= $(shell pwd)/packages

# Shared with Neverball.

ASSET_PATHS := \
	back \
	ball/basic-ball \
	bgm \
	courses.txt \
	geom \
	gui \
	icon \
	item \
	lang \
	lights.txt \
	map-back \
	png \
	snd \
	textures \
	ttf

# The courses themselves, and the map directories they name.

ASSET_PATHS += \
	holes-abc.txt \
	holes-iCourse.txt \
	holes-kk.txt \
	holes-paxed.txt \
	holes-paxed2.txt \
	holes-paxed3.txt \
	holes-putt.txt \
	holes-slippi.txt \
	holes-vidski.txt \
	map-ckk \
	map-iCourse \
	map-paxed \
	map-paxed2 \
	map-paxed3 \
	map-putt \
	map-slippi \
	map-vidski \
	shot-putt

.PHONY: package-only
package-only:
	$(MAKE) -f mk/package.mk PACKAGE_ID=base-neverputt DATA_DIR=$(DATA_DIR) ASSET_PATHS="$(ASSET_PATHS)" package

GNUMAKEFLAGS = --no-print-directory
