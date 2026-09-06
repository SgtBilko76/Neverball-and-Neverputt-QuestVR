/*
 * Copyright (C) 2021-2025 Jānis Rūcis
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

#include "fetch.h"

/*
 * Never equal to a registered SDL event type, so the dispatch in the main
 * loop simply never matches.
 */

unsigned long FETCH_EVENT = (unsigned long) -1;

void fetch_enable(int enable)
{
}

void fetch_init(void)
{
}

void fetch_handle_event(void *data)
{
}

void fetch_quit(void)
{
}

unsigned int fetch_file(const char *url, const char *dst, struct fetch_callback callback)
{
    return 0;
}