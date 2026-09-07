/*
 * Copyright (C) 2013 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#ifndef HMD_H
#define HMD_H

/*---------------------------------------------------------------------------*/

int  hmd_stat(void);

void hmd_init(void);
void hmd_free(void);
void hmd_poll(void);
void hmd_step(void);
void hmd_swap(void);

int  hmd_should_render(void);

/* Is the player looking at the game, rather than at a system overlay? */

int  hmd_focused(void);

/*
 * Vertical field of view of the eye being painted, in degrees. Zero when
 * there is no headset, and also for the older backends that pack both eyes
 * into one window and have no such thing.
 */

float hmd_fov(void);

void hmd_prep_left(void);
void hmd_prep_right(void);

void hmd_persp(float, float);
void hmd_ortho(void);

/*---------------------------------------------------------------------------*/

#endif
