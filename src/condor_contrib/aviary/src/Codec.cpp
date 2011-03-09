/***************************************************************
 *
 * Copyright (C) 2009-2011 Red Hat, Inc.
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

// condor includes
#include "condor_common.h"
#ifndef WIN32
    #include "stdint.h"
#endif
#include "condor_qmgr.h"

// local includes
#include "Codec.h"
#include "AviaryUtils.h"

using namespace compat_classad;
using namespace aviary::codec;
using namespace aviary::util;

template <class Codec>
Codec*
DefaultCodecFactory<Codec>::createCodec() {
    if (!m_codec) {
        m_codec = new Codec;
    }
    return m_codec;
}

bool
Codec::addAttributeToMap (ClassAd& ad, const char* name, AttributeMapType& _map)
{
    ExprTree *expr;

    // All these extra lookups are horrible. They have to
    // be there because the ClassAd may have multiple
    // copies of the same attribute name! This means that
    // the last attribute with a given name will set the
    // value, but the last attribute tends to be the
    // attribute with the oldest (wrong) value. How
    // annoying is that!
    if (!(expr = ad.Lookup(name))) {
        dprintf(D_FULLDEBUG, "Warning: failed to lookup attribute '%s' from ad\n", name);
        return false;
    }

    classad::Value value;
    ad.EvaluateExpr(expr,value);
    switch (value.GetType()) {
        // TODO: does this cover expressions also?
        case classad::Value::BOOLEAN_VALUE:
            {
            _map[name] = new Attribute(Attribute::EXPR_TYPE,trimQuotes(ExprTreeToString(expr)).c_str());
            }
            break;
        case classad::Value::INTEGER_VALUE:
            _map[name] = new Attribute(Attribute::INTEGER_TYPE,ExprTreeToString(expr));;
            break;
        case classad::Value::REAL_VALUE:
            _map[name] = new Attribute(Attribute::FLOAT_TYPE,ExprTreeToString(expr));
            break;
        case classad::Value::STRING_VALUE:
        default:
            _map[name] = new Attribute(Attribute::STRING_TYPE,trimQuotes(ExprTreeToString(expr)).c_str());
    }

    return true;
}

bool
Codec::classAdToMap(ClassAd& ad, AttributeMapType& _map)
{
    ClassAd::iterator iter;

    ad.ResetExpr();
    _map.clear();
    iter = ad.begin();
    while (iter != ad.end()) {
            string name = iter->first;
            if (!addAttributeToMap(ad, name.c_str(), _map)) {
                    return false;
            }
            iter++;
    }

    // TODO: debug
    //  if (DebugFlags & D_FULLDEBUG) {
    //      ad.dPrint(D_FULLDEBUG|D_NOHEADER);
    //  }

    return true;
}


bool
Codec::mapToClassAd(AttributeMapType& _map, ClassAd& ad)
{

    for (AttributeMapIterator entry = _map.begin(); _map.end() != entry; entry++) {
        const char* name = entry->first;
        Attribute* value = entry->second;

        switch (value->getType()) {
            case Attribute::INTEGER_TYPE:
                ad.Assign(name, atoi(value->getValue()));
                break;
            case Attribute::FLOAT_TYPE:
                ad.Assign(name, atof(value->getValue()));
                break;
            case Attribute::EXPR_TYPE:
            case Attribute::STRING_TYPE:
                ad.Assign(name, value->getValue());
                break;
            default:
                dprintf(D_FULLDEBUG, "Warning: Unknown/unsupported type in map for attribute '%s'\n", name);
        }
    }

    // TODO: debug
    //  if (DebugFlags & D_FULLDEBUG) {
    //      ad.dPrint(D_FULLDEBUG|D_NOHEADER);
    //  }

    return true;
}

bool
Codec::procIdToMap(int clusterId, int procId, AttributeMapType& _map)
{
    ClassAd *ad;

    if (NULL == (ad = ::GetJobAd(clusterId, procId, false))) {
        dprintf(D_ALWAYS,
                "::GetJobAd method called on %d.%d, which does not exist\n",
                clusterId, procId);
        return false;
    }

    return mapToClassAd(_map, *ad);
}