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

/*---------------------------------------------------------------------------*/

int  hmd_stat(void) { return 0; }

void hmd_init(void) { }
void hmd_free(void) { }
void hmd_poll(void) { }
void hmd_step(void) { }
void hmd_swap(void) { }

int  hmd_should_render(void) { return 1; }
int  hmd_focused(void)       { return 1; }
float hmd_fov(void)          { return 0.0f; }

void hmd_prep_left(void)  { }
void hmd_prep_right(void) { }

void hmd_persp(float n, float f) { }
void hmd_ortho(void) { }

/*---------------------------------------------------------------------------*/
