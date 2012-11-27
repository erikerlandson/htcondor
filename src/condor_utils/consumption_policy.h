/***************************************************************
 *
 * Copyright (C) 1990-2012, Condor Team, Computer Sciences Department,
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
#ifndef CONSUMPTION_POLICY_H
#define CONSUMPTION_POLICY_H

#include "compat_classad.h"
#include <map>

bool supports_consumption_policy(ClassAd& resource);
void compute_asset_consumption(ClassAd& job, ClassAd& resource, std::map<string, double>& consumption);
bool consume_resource_assets(ClassAd& job, ClassAd& resource);

#endif // CONSUMPTION_POLICY_H
