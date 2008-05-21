/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/

#ifndef _GIDUSE_H
#define _GIDUSE_H

// this module handles probing the OS regarding what GIDs are in
// use

#include <stdbool.h>
#include <sys/types.h>

// check which of a range of GIDs are currently in use by one or more
// processes on the system. returns 0 on success, -1 on failure. the
// range that is checked is [first, first + count - 1]. the used
// parameter must point to an array of integers. on success, each array
// element will be set to the number of processes using the corresponding
// GID
//
int giduse_probe(gid_t first, int count, int* used);

#endif
