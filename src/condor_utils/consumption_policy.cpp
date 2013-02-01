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


bool cp_supports_policy(ClassAd& resource) {
    // currently, only p-slots can support a meaningful consumption policy
    bool part = false;
    if (!resource.LookupBool(ATTR_SLOT_PARTITIONABLE, part)) part = false;
    if (!part) return false;
    
    // must support MachineResources attribute
    string mrv;
    if (!resource.LookupString(ATTR_MACHINE_RESOURCES, mrv)) return false;

    // must define ConsumptionXxx for all resources Xxx (including extensible resources)
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


void cp_compute_consumption(ClassAd& job, ClassAd& resource, map<string, double>& consumption) {
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
            // this case is intended to be operative when a scheduler has set 
            // such values and sent them on to the startd that owns this resource
            // (e.g. I'd not expect this case to arise elsewhere, like the negotiator)
            formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
            if (!job.LookupString(ra.c_str(), rsave)) {
                EXCEPT("Resource ad missing %s attribute", ra.c_str());
            }
            job.Assign(ra.c_str(), rv);
        }

        // get the requested asset value
        formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
        ClassAd::iterator f(job.find(ra));
        if (f == resource.end()) {
            // a RequestXxx attribute not present on job ad is OK - default it to zero
            rv = 0;
        } else if (!job.EvalFloat(ra.c_str(), NULL, rv)) {
            // if RequestXxx attribute exists, I insist that it evaluates to a float
            EXCEPT("Job ad attribute %s failed to evaluate\n", ra.c_str());
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


bool cp_sufficient_assets(ClassAd& job, ClassAd& resource) {
    map<string, double> consumption;
    cp_compute_consumption(job, resource, consumption);
    return cp_sufficient_assets(resource, consumption);
}


bool cp_sufficient_assets(ClassAd& resource, const map<string, double>& consumption) {
    for (map<string, double>::const_iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        dprintf(D_ALWAYS, "EJE: asset %s...\n", asset);
        double av=0;
        if (!resource.LookupFloat(asset, av)) {
            EXCEPT("Missing %s resource asset", asset);
        }
        dprintf(D_ALWAYS, "EJE: consumption(%s)= %g, available= %g\n", asset, j->second, av);
        if (av < j->second) {
            dprintf(D_ALWAYS, "EJE: insufficient quantity of asset %s\n", asset);
            return false;
        }
    }
    return true;
}


double cp_deduct_assets(ClassAd& job, ClassAd& resource, bool test) {
    map<string, double> consumption;
    cp_compute_consumption(job, resource, consumption);

    // slot weight before asset deductions
    double w0 = 0;
    if (!resource.EvalFloat(ATTR_SLOT_WEIGHT, NULL, w0)) {
        EXCEPT("Failed to evaluate %s", ATTR_SLOT_WEIGHT);
    }

    // deduct consumption from the resource assets
    for (map<string, double>::iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        double av=0;
        if (!resource.LookupFloat(asset, av)) {
            EXCEPT("Missing %s resource asset", asset);
        }
        resource.Assign(asset, av - j->second);
    }

    // slot weight after deductions
    double w1 = 0;
    if (!resource.EvalFloat(ATTR_SLOT_WEIGHT, NULL, w1)) {
        EXCEPT("Failed to evaluate %s", ATTR_SLOT_WEIGHT);
    }

    // define cost as difference in slot weight before and after asset deduction
    double cost = w0 - w1;

    // if we are not in testing mode, then keep the asset deductions
    if (!test) return cost;

    // The Dude just wants his assets back
    for (map<string, double>::iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        double av=0;
        resource.LookupFloat(asset, av);
        resource.Assign(asset, av + j->second);
    }

    return cost;
}


void cp_override_requested(ClassAd& job, ClassAd& resource, map<string, double>& consumption) {
    cp_compute_consumption(job, resource, consumption);

    for (map<string, double>::iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        string ra;
        string oa;
        formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
        formatstr(oa, "_orig_%s%s", ATTR_REQUEST_PREFIX, asset);
        job.CopyAttribute(oa.c_str(), ra.c_str());
    }
}


void cp_restore_requested(ClassAd& job, const map<string, double>& consumption) {
    for (map<string, double>::const_iterator j(consumption.begin());  j != consumption.end();  ++j) {
        const char* asset = j->first.c_str();
        string ra;
        string oa;
        formatstr(ra, "%s%s", ATTR_REQUEST_PREFIX, asset);
        formatstr(oa, "_orig_%s%s", ATTR_REQUEST_PREFIX, asset);
        job.CopyAttribute(ra.c_str(), oa.c_str());
        job.Delete(oa);
    }    
}
