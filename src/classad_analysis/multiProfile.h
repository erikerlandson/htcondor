/***************************Copyright-DO-NOT-REMOVE-THIS-LINE**
  *
  * Condor Software Copyright Notice
  * Copyright (C) 1990-2006, Condor Team, Computer Sciences Department,
  * University of Wisconsin-Madison, WI.
  *
  * This source code is covered by the Condor Public License, which can
  * be found in the accompanying LICENSE.TXT file, or online at
  * www.condorproject.org.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * AND THE UNIVERSITY OF WISCONSIN-MADISON "AS IS" AND ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY, OF SATISFACTORY QUALITY, AND FITNESS
  * FOR A PARTICULAR PURPOSE OR USE ARE DISCLAIMED. THE COPYRIGHT
  * HOLDERS AND CONTRIBUTORS AND THE UNIVERSITY OF WISCONSIN-MADISON
  * MAKE NO MAKE NO REPRESENTATION THAT THE SOFTWARE, MODIFICATIONS,
  * ENHANCEMENTS OR DERIVATIVE WORKS THEREOF, WILL NOT INFRINGE ANY
  * PATENT, COPYRIGHT, TRADEMARK, TRADE SECRET OR OTHER PROPRIETARY
  * RIGHT.
  *
  ****************************Copyright-DO-NOT-REMOVE-THIS-LINE**/

#ifndef __MULTIPROFILE_H__
#define __MULTIPROFILE_H__

#include "boolExpr.h"
#include "list.h"

/** A BoolExpr in Disjunctive Profile Form, which is a disjunction of Profiles.
	Currently the only way to initialize a MultiProfile is with the
	ExprToMultiProfile method in the BoolExpr class.
	@see BoolExpr
*/
class MultiProfile : public BoolExpr
{
	friend class BoolExpr;
 public:

		/** Default Constructor */
	MultiProfile( );

		/** Destructor */
	~MultiProfile( );

		/** Determines if the MultiProfile represents a literal value
			@return true if MulitProfile is a literal value, false otherwise.
		*/
	bool IsLiteral( );

		/** Gets the BoolValue if the MultiProfile represents a literal value
			@param result the BoolValue represented by the MultiProfile
			@return true on success, false on failure or if MultiProfile is not
			a literal value.
		*/
	bool GetLiteralValue( BoolValue &result );

		/** Gets the number of profiles contained in the MultiProfile
			@param result The number of profiles.
			@return true on success, false on failure.
		*/
	bool GetNumberOfProfiles( int &result );
		
		/** Rewinds the list of Profiles in the MultiProfile
			@return true on success, false on failure
		*/
	bool Rewind( );

		/** Gets the next Profile in the MultiProfile
			@param result the next profile in the MultiProfile
			@return true on success, false on failure
		*/
	bool NextProfile( Profile *&result );

		/** Generate a string representation of the MultiProfile.
			@param buffer A string to print the result to.
			@return true on success, false on failure.
		*/ 
	bool ToString( string &buffer );


		// unsupported methods
	bool AddProfile( Profile & );
	bool RemoveProfile( Profile & );
	bool RemoveAllProfiles( );

		/** A repository for annotations for the MultiProfile.
			@see MultiProfileExplain
		*/
	MultiProfileExplain explain;

    MultiProfile & operator=(const MultiProfile& copy) { return *this;}
 protected:
	bool InitVal( classad::Value & );
 private:
	bool isLiteral;
	BoolValue literalValue;
	bool AppendProfile( Profile * );
	List<Profile> profiles;
};	

#endif // __MULTIPROFILE_H__
