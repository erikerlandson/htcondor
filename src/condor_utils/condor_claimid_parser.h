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


#ifndef _CONDOR_CLAIMID_PARSER_H
#define _CONDOR_CLAIMID_PARSER_H

// ClaimIdParser knows how to extract parts of a claim id string.
// It expects the following format (generated by the starter's claim.C)
// "<ip:port>#...#secret_cookie"

class ClaimIdParser {
 public:
	ClaimIdParser():
		m_suppress_session(false) {
	}
	ClaimIdParser(char const *claim_id):
		m_claim_id(claim_id),
		m_suppress_session(false) {
	}
	ClaimIdParser(char const *session_id,char const *session_info,char const *session_key):
		m_suppress_session(false)
	{
		m_claim_id.sprintf("%s#%s%s",
						   session_id ? session_id : "",
						   session_info ? session_info : "",
						   session_key ? session_key : "");
		ASSERT( !session_info || !strchr(session_info,'#') );
		ASSERT( !session_key || !strchr(session_key,'#') );
	}
	void setClaimId(char const *claim_id) {
		m_claim_id = claim_id;
		m_sinful_part = "";
		m_public_part = "";
	}
	char const *claimId() {
		return m_claim_id.Value();
	}
	char const *startdSinfulAddr() {
		if( m_sinful_part.IsEmpty() ) {
			char const *str = m_claim_id.Value();
			char const *end = strchr(str,'#');
			int length = end ? end - str : 0;
			m_sinful_part.sprintf("%.*s",length,str);
		}
		return m_sinful_part.Value();
	}
	char const *publicClaimId() {
		if( m_public_part.IsEmpty() ) {
			char const *str = m_claim_id.Value();
			char const *end = strrchr(str,'#');
			int length = end ? end - str : 0;
			m_public_part.sprintf("%.*s#...",length,str);
		}
		return m_public_part.Value();
	}

	char const *secSessionId(bool ignore_session_info=false) {
			// This must return the same thing for both the schedd and
			// the startd, so be careful making any version-incompatible
			// changes.
		if( m_suppress_session ) {
			return NULL;
		}
		if( !ignore_session_info && !secSessionInfo() ) {
				// There is no session info, so no security session
				// was created.  Returning NULL here simplifies
				// the call sites that pass the session id to
				// startCommand().
			return NULL;
		}
		if( m_session_id.IsEmpty() ) {
			char const *str = m_claim_id.Value();
			char const *end = strrchr(str,'#');
			int length = end ? end - str : 0;
			m_session_id.sprintf("%.*s",length,str);
		}
		return m_session_id.Value();
	}
	char const *secSessionKey() {
			// expected format: blah#blah#...#[session_info]SESSION_KEY
		char const *str = m_claim_id.Value();
		char const *ptr = strrchr(str,'#');
		if(ptr) {
			ptr+=1;
		}
		char const *ptr2 = strrchr(str,']');
		if(ptr2) {
			ptr2+=1;
		}
		if( ptr2 > ptr ) {
				// skip past the session info
			ptr = ptr2;
		}
		return ptr;
	}
	char const *secSessionInfo() {
			// expected format: blah#blah#...#[session_info]SESSION_KEY
		if( m_session_info.IsEmpty() ) {
			char const *str = m_claim_id.Value();
			char const *ptr = strrchr(str,'#');
			char const *endptr;
			if( !ptr ) {
				return NULL;
			}
			ptr+=1;
			if( *ptr != '[' ) {
				return NULL;
			}
			endptr = strrchr(str,']');
			if(!endptr || endptr < ptr) {
				return NULL;
			}
			m_session_info.sprintf("%.*s",(int)(endptr+1-ptr),ptr);
		}

		if( m_session_info.IsEmpty() ) {
				// returning NULL here is a convenience for call sites
			return NULL;
		}
		return m_session_info.Value();
	}
	void setSecSessionInfo(char const *session_info) {
		if(session_info) {
			ASSERT(session_info[0] == '[');
			ASSERT(session_info[strlen(session_info)-1] == ']');
			ASSERT(strchr(session_info,'#') == NULL);
		}

		MyString new_claim_id = secSessionId(true);
		char const *session_key = secSessionKey();
		new_claim_id.sprintf_cat("#%s%s",
			session_info ? session_info : "",
			session_key ? session_key : "");

			// reset everything using the new claim id
		*this = ClaimIdParser(new_claim_id.Value());
	}
	void suppressSecSession( bool toggle ) {
		m_suppress_session = toggle;
	}

 private:
	MyString m_claim_id;
	MyString m_public_part;
	MyString m_sinful_part;

		// The following fields are for SEC_ENABLE_MATCH_PASSWORD_AUTHENTICATION
	bool m_suppress_session; // if true, secSessionId() always returns NULL
	MyString m_session_id;
	MyString m_session_key;
	MyString m_session_info;
};

#endif