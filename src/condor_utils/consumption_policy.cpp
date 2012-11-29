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


#include "condor_common.h"
#include "MyString.h"
#include "condor_attributes.h"
#include "string_list.h"

#include "consumption_policy.h"


bool supports_consumption_policy(ClassAd& resource) {
    string mrv;
    if (!resource.LookupString(ATTR_MACHINE_RESOURCES, mrv)) return false;

    StringList alist(mrv.c_str());
    alist.rewind();
    while (char* asset = alist.next()) {
        if (0 == strcmp(asset, "swap")) continue;
        string ca;
        formatstr(ca, "%s%s", ATTR_CONSUMPTION_PREFIX, asset);
        ClassAd::iterator f(resource.find(ca));
        if (f == resource.end()) return false;
    }

    return true;
}


void compute_asset_consumption(ClassAd& job, ClassAd& resource, map<string, double>& consumption) {
    consumption.clear();

    string mrv;
    if (!resource.LookupString(ATTR_MACHINE_RESOURCES, mrv)) {
        EXCEPT("Resource ad missing %s attribute", ATTR_MACHINE_RESOURCES);
    }

    StringList alist(mrv.c_str());
    alist.rewind();
    while (char* asset = alist.next()) {
        if (0 == strcmp(asset, "swap")) continue;
        dprintf(D_ALWAYS, "EJE: asset %s...\n", asset);

        string rsave;
        string ra;
        formatstr(ra, "_condor_%s%s", ATTR_REQUEST_PREFIX, asset);
        double rv=0;
        if (job.EvalFloat(ra.c_str(), NULL, rv)) {
            // Allow _condor_RequestedXXX to override RequestedXXX
            formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
            if (!job.LookupString(ra.c_str(), rsave)) {
                EXCEPT("Resource ad missing %s attribute", ra.c_str());
            }
            job.Assign(ra.c_str(), rv);
        }

        // get the requested asset value
        formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
        if (!job.EvalFloat(ra.c_str(), NULL, rv)) {
            EXCEPT("Resource ad missing %s attribute", ra.c_str());
        }
        dprintf(D_ALWAYS, "EJE: requested(%s) = %g\n", asset, rv);

        // compute the consumed value for the asset
        string ca;
        formatstr(ca, "%s%s", ATTR_CONSUMPTION_PREFIX, asset);
        double cv = 0;
        if (!resource.EvalFloat(ca.c_str(), &job, cv)) {
            EXCEPT("Evaluation of %s attribute failed", ca.c_str());
        }

        if (rsave != "") {
            // restore saved value for RequestedXXX if it was overridden by _condor_RequestedXXX
            formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
            job.Assign(ra.c_str(), rsave);
        }

        dprintf(D_ALWAYS, "EJE: consumption(%s) = %g\n", asset, cv);

        consumption[asset] = cv;
    }
}


bool consume_resource_assets(ClassAd& job, ClassAd& resource, bool test) {
    map<string, double> consumption;
    compute_asset_consumption(job, resource, consumption);
    for (map<string, double>::iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        dprintf(D_ALWAYS, "EJE: asset %s...\n", asset);
        double av=0;
        if (!resource.LookupFloat(asset, av)) {
            EXCEPT("Missing %s attribute", asset);
        }
        dprintf(D_ALWAYS, "EJE: consumption(%s)= %g, available= %g\n", asset, j->second, av);
        if (av < j->second) {
            dprintf(D_ALWAYS, "EJE: insufficient quantity of asset %s\n", asset);
            return false;
        }
    }

    // if we're in test mode, we skip actual consumption
    if (test) return true;

    // we can satisfy the requested assets, so do the deed:
    for (map<string, double>::iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        double av=0;
        resource.LookupFloat(asset, av);
        resource.Assign(asset, av - j->second);
    }
    return true;
}
