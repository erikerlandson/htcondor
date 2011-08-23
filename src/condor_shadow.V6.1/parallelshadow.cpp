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



#include "condor_common.h"
#include "parallelshadow.h"
#include "condor_daemon_core.h"
#include "condor_qmgr.h"         // need to talk to schedd's qmgr
#include "condor_attributes.h"   // for ATTR_ ClassAd stuff
#include "condor_email.h"        // for email.
#include "list.h"                // List class
#include "internet.h"            // sinful->hostname stuff
#include "daemon.h"
#include "env.h"
#include "condor_config.h"
#include "spooled_job_files.h"
#include "condor_claimid_parser.h"

RemoteResource *parallelMasterResource = NULL;

ParallelShadow::ParallelShadow() {
    nextResourceToStart = 0;
	numNodes = 0;
    shutDownMode = FALSE;
    ResourceList.fill(NULL);
    ResourceList.truncate(-1);
	actualExitReason = -1;
	info_tid = -1;
	is_reconnect = false;
	shutdownPolicy = ParallelShadow::WAIT_FOR_NODE0;
}

ParallelShadow::~ParallelShadow() {
        // Walk through list of Remote Resources.  Delete all.
    for ( int i=0 ; i<=ResourceList.getlast() ; i++ ) {
        delete ResourceList[i];
    }
	daemonCore->Cancel_Command( SHADOW_UPDATEINFO );
}

void 
ParallelShadow::init( ClassAd* job_ad, const char* schedd_addr, const char *xfer_queue_contact_info )
{

	char buf[256];

    if( ! job_ad ) {
        EXCEPT( "No job_ad defined!" );
    }

        // BaseShadow::baseInit - basic init stuff...
    baseInit( job_ad, schedd_addr, xfer_queue_contact_info );

		// Register command which gets updates from the starter
		// on the job's image size, cpu usage, etc.  Each kind of
		// shadow implements it's own version of this to deal w/ it
		// properly depending on parallel vs. serial jobs, etc. 
	daemonCore->
		Register_Command( SHADOW_UPDATEINFO, "SHADOW_UPDATEINFO", 
						  (CommandHandlercpp)&ParallelShadow::updateFromStarter,
						  "ParallelShadow::updateFromStarter", this, DAEMON ); 


        // make first remote resource the "master".  Put it first in list.
    MpiResource *rr = new MpiResource( this );
	parallelMasterResource = rr;

	char buffer[1024];
	char *lspool = param("SPOOL");
	char *dir = gen_ckpt_name(lspool, getCluster(), 0, 0);
	free(lspool);
	job_ad->Assign(ATTR_REMOTE_SPOOL_DIR,dir);
	free(dir); dir = NULL;
	job_ad->Insert(buffer);

    snprintf( buf, 256, "%s = %s", ATTR_MPI_IS_MASTER, "TRUE" );
    if( !job_ad->Insert(buf) ) {
        dprintf( D_ALWAYS, "Failed to insert %s into jobAd.\n", buf );
        shutDown( JOB_NOT_STARTED );
    }

	replaceNode( job_ad, 0 );
	rr->setNode( 0 );
	snprintf( buf, 256, "%s = 0", ATTR_NODE );
	job_ad->InsertOrUpdate( buf );
    rr->setJobAd( job_ad );

	rr->setStartdInfo( job_ad );

	job_ad->Assign( ATTR_JOB_STATUS, RUNNING );

    ResourceList[ResourceList.getlast()+1] = rr;

	shutdownPolicy = ParallelShadow::WAIT_FOR_NODE0;

	MyString policy;
	job_ad->LookupString(ATTR_PARALLEL_SHUTDOWN_POLICY, policy);
	if (policy == "WAIT_FOR_ALL") {
		dprintf(D_ALWAYS, "Setting parallel shutdown policy to WAIT_FOR_ALL\n");
		shutdownPolicy= WAIT_FOR_ALL;
	}
}


bool
ParallelShadow::shouldAttemptReconnect(RemoteResource *r) {
	if (shutdownPolicy == WAIT_FOR_ALL) {
		return true;
	}

	if ((shutdownPolicy == WAIT_FOR_NODE0) && (r != ResourceList[0])) {
		return false;
	}

	return true;
}

void
ParallelShadow::reconnect( void )
{

	dprintf( D_FULLDEBUG, "ParallelShadow::reconnect\n");
	is_reconnect = true;
	spawn();
}


bool 
ParallelShadow::supportsReconnect( void )
{
		// Iff all remote resources support reconect,
		// then this shadow does.  If any do not, we do not
    for ( int i=0 ; i<=ResourceList.getlast() ; i++ ) {
        if (! ResourceList[i]->supportsReconnect()) {
			return false;
		}
    }
	
	return true;
}


void
ParallelShadow::spawn( void )
{
		/*
		  This is lame.  We should really do a better job of dealing
		  with the multiple ClassAds for MPI universe via the classad
		  file mechanism (pipe to STDIN, usually), instead of this
		  whole mess, and spawn() should really just call
		  "startMaster()".  however, in the race to get disconnected
		  operation working for vanilla, we cut a few corners and
		  leave this as it is.  whenever we're seriously looking at
		  MPI support again, we should fix this, too.
		*/
		/*
		  Finally, register a timer to call getResources(), which
		  sends a command to the schedd to get all the job classads,
		  startd sinful strings, and ClaimIds for all the matches
		  for our computation.  
		  In the future this will just be a backup way to get the
		  info, since the schedd will start to push all this info to
		  our UDP command port.  For now, this is the only way we get
		  the info.
		*/
	info_tid = daemonCore->
		Register_Timer( 1, 0,
						(TimerHandlercpp)&ParallelShadow::getResources,
						"getResources", this );
	if( info_tid < 0 ) {
		EXCEPT( "Can't register DC timer!" );
	}
}


int 
ParallelShadow::getResources( void )
{
    dprintf ( D_FULLDEBUG, "Getting machines from schedd now...\n" );

    char *host = NULL;
    char *claim_id = NULL;
    MpiResource *rr;
	int job_cluster;
	char buf[128];

    int numProcs=0;    // the # of procs to come
    int numInProc=0;   // the # in a particular proc.
	ClassAd *job_ad = NULL;
	int nodenum = 1;
	ReliSock* sock;

	job_cluster = getCluster();
    rr = ResourceList[0];
	rr->getClaimId( claim_id );

		// First, contact the schedd and send the command, the
		// cluster, and the ClaimId
	Daemon my_schedd (DT_SCHEDD, getScheddAddr(), NULL);

	if(!(sock = (ReliSock*)my_schedd.startCommand(GIVE_MATCHES))) {
		EXCEPT( "Can't connect to schedd at %s", getScheddAddr() );
	}
		
	sock->encode();
	if( ! sock->code(job_cluster) ) {
		EXCEPT( "Can't send cluster (%d) to schedd\n", job_cluster );
	}
	if( ! sock->code(claim_id) ) {
		EXCEPT( "Can't send ClaimId to schedd\n" );
	}

		// Now that we sent this, free the memory that was allocated
		// with getClaimId() above
	delete [] claim_id;
	claim_id = NULL;

	if( ! sock->end_of_message() ) {
		EXCEPT( "Can't send EOM to schedd\n" );
	}
	
		// Ok, that's all we need to send, now we can read the data
		// back off the wire
	sock->decode();

        // We first get the number of proc classes we'll be getting.
    if ( !sock->code( numProcs ) ) {
        EXCEPT( "Failed to get number of procs" );
    }

        /* Now, do stuff for each proc: */
    for ( int i=0 ; i<numProcs ; i++ ) {
        if( !sock->code( numInProc ) ) {
            EXCEPT( "Failed to get number of matches in proc %d", i );
        }

        dprintf ( D_FULLDEBUG, "Got %d matches for proc # %d\n",
				  numInProc, i );

        for ( int j=0 ; j<numInProc ; j++ ) {
            if ( !sock->code( host ) ||
                 !sock->code( claim_id ) ) {
                EXCEPT( "Problem getting resource %d, %d", i, j );
            }
			ClaimIdParser idp( claim_id );
            dprintf( D_FULLDEBUG, "Got host: %s id: %s\n", host, idp.publicClaimId() );
            
			job_ad = new ClassAd();

			if( !job_ad->initFromStream(*sock)  ) {
				EXCEPT( "Failed to get job classad for proc %d", i );
			}

            if ( i==0 && j==0 ) {
					/* 
					   TODO: once this is just backup for if the
					   schedd doesn't push it, we need to NOT ignore
					   the first match, since we don't already have
					   it, really.
					*/
                    /* first host passed on command line...we already 
                       have it!  We ignore it here.... */

                free( host );
                free( claim_id );
                host = NULL;
                claim_id = NULL;
				delete job_ad;
                continue;
            }

            rr = new MpiResource( this );
            rr->setStartdInfo( host, claim_id );
 				// for now, set this to the sinful string.  when the
 				// starter spawns, it'll do an RSC to register a real
				// hostname... 
			rr->setMachineName( host );

			replaceNode ( job_ad, nodenum );
			rr->setNode( nodenum );
			snprintf( buf, 128, "%s = %d", ATTR_NODE, nodenum );
			job_ad->InsertOrUpdate( buf );
			snprintf( buf, 128, "%s = \"%s\"", ATTR_MY_ADDRESS,
					 daemonCore->InfoCommandSinfulString() );
			job_ad->InsertOrUpdate( buf );

			char buffer[1024];
			char *lspool = param("SPOOL");
			char *dir = gen_ckpt_name(lspool, job_cluster, 0, 0);
			free(lspool);
			job_ad->Assign(ATTR_REMOTE_SPOOL_DIR, dir);
			free(dir); dir = NULL;
			job_ad->Insert(buffer);

				// Put the correct claim id into this ad's ClaimId attribute.
				// Otherwise, it is the claim id of the master proc.
				// This is how the starter finds out about the claim id.
			job_ad->Assign(ATTR_CLAIM_ID,claim_id);

			rr->setJobAd( job_ad );
			nodenum++;

            ResourceList[ResourceList.getlast()+1] = rr;

                /* free stuff so next code() works correctly */
            free( host );
            free( claim_id );
            host = NULL;
            claim_id = NULL;

        } // end of for loop for this proc

    } // end of for loop on all procs...

    sock->end_of_message();

	numNodes = nodenum;  // for later use...

    dprintf ( D_PROTOCOL, "#1 - Shadow started; %d machines gotten.\n", 
			  numNodes );

    startMaster();

    return TRUE;
}


void
ParallelShadow::startMaster()
{
    MpiResource *rr;
    dprintf ( D_FULLDEBUG, "In ParallelShadow::startMaster()\n" );

	rr = ResourceList[0];

	spawnNode( rr );

	spawnAllComrades();

}

void
ParallelShadow::spawnAllComrades( void )
{
		/* 
		   If this function is being called, we've already spawned the
		   root node and gotten its ip/port from our special pseudo
		   syscall.  So, all we have to do is loop over our remote
		   resource list, modify each resource's job ad with the
		   appropriate info, and spawn our node on each resource.
		*/

    MpiResource *rr;
	int last = ResourceList.getlast();
	while( nextResourceToStart <= last ) {
        rr = ResourceList[nextResourceToStart];
		spawnNode( rr );  // This increments nextResourceToStart 
    }
	ASSERT( nextResourceToStart == numNodes );
}


void 
ParallelShadow::spawnNode( MpiResource* rr )
{
	if (is_reconnect) {
		dprintf(D_FULLDEBUG, "reconnecting to the following remote resource:\n");
		rr->dprintfSelf(D_FULLDEBUG);
		rr->reconnect();
	} else {
			// First, contact the startd to spawn the job
		if( rr->activateClaim() != OK ) {
			shutDown( JOB_NOT_STARTED );
			
		}
	}

    dprintf ( D_PROTOCOL, "Just requested resource for node %d\n",
			  nextResourceToStart );

	nextResourceToStart++;
}


void 
ParallelShadow::cleanUp( void )
{
	// kill all the starters
	MpiResource *r;
	int i;
    for( i=0 ; i<=ResourceList.getlast() ; i++ ) {
		r = ResourceList[i];
		r->killStarter();
	}		
}

int ParallelShadow::JobSuspend(int sig)
{
	int iRet = 0;
	MpiResource *r;
	int i;
    for( i=0 ; i<=ResourceList.getlast() ; i++ ) {
		r = ResourceList[i];
		if (!r->suspend())
		{
			iRet = 1;
			dprintf ( D_ALWAYS, "ParallelShadow::JobSuspend() sig %d FAILED\n", sig );
		}
	}		
	
	return iRet;
	
}

int ParallelShadow::JobResume(int sig)
{
	int iRet = 0;
	MpiResource *r;
	int i;
    for( i=0 ; i<=ResourceList.getlast() ; i++ ) {
		r = ResourceList[i];
		if (!r->resume())
		{
			iRet = 1;
			dprintf ( D_ALWAYS, "ParallelShadow::JobResume() sig %d FAILED\n", sig );
		}
	}
	
	return iRet;
}


bool
ParallelShadow::claimIsClosing( void )
{
	return false;
}

void 
ParallelShadow::gracefulShutDown( void )
{
	cleanUp();
}


void
ParallelShadow::emailTerminateEvent( int exitReason, update_style_t kind )
{
	int i;
	FILE* mailer;
	Email msg;
	MyString str;
	char *s;

	mailer = msg.open_stream( jobAd, exitReason );
	if( ! mailer ) {
			// nothing to do
		return;
	}

	fprintf( mailer, "Your Condor-MPI job %d.%d has completed.\n", 
			 getCluster(), getProc() );

	fprintf( mailer, "\nHere are the machines that ran your MPI job.\n");

	if (kind == US_TERMINATE_PENDING) {
		fprintf( mailer, "    Machine Name         \n" );
		fprintf( mailer, " ------------------------\n" );

		// Don't print out things like the exit codes since they have
		// been lost and it is a little too much work to add them into
		// the jobad for the amount of time I have to get this working.
		// This should be a more rare case in any event.

		jobAd->LookupString(ATTR_REMOTE_HOSTS, str);
		StringList slist(str.Value());

		slist.rewind();
		while((s = slist.next()) != NULL)
		{
			fprintf( mailer, "%s\n", s);
		}

		fprintf( mailer, "\nExit codes are currently unavailable.\n\n");
		fprintf( mailer, "\nHave a nice day.\n" );

		return;
	}

	fprintf( mailer, "They are listed in the order they were started\n" );
	fprintf( mailer, "in, which is the same as MPI_Comm_rank.\n\n" );
	fprintf( mailer, "    Machine Name               Result\n" );
	fprintf( mailer, " ------------------------    -----------\n" );

	// This is the normal case of US_NORMAL. In this mode we can print
	// out a bunch of interesting things about the job which the remote
	// resources know about.

	int allexitsone = TRUE;
	int exit_code;
	for ( i=0 ; i<=ResourceList.getlast() ; i++ ) {
		(ResourceList[i])->printExit( mailer );
		exit_code = (ResourceList[i])->exitCode();
		if( exit_code != 1 ) {
			allexitsone = FALSE;
		}
	}

	if ( allexitsone ) {
		fprintf ( mailer, "\nCondor has noticed that all of the " );
		fprintf ( mailer, "processes in this job \nhave an exit status " );
		fprintf ( mailer, "of 1.  This *might* be the result of a core\n");
		fprintf ( mailer, "dump.  Condor can\'t tell for sure - the " );
		fprintf ( mailer, "MPICH library catches\nSIGSEGV and exits" );
		fprintf ( mailer, "with a status of one.\n" );
	}

	msg.writeCustom(jobAd);

	fprintf( mailer, "\nHave a nice day.\n" );
}


void 
ParallelShadow::shutDown( int exitReason )
{
	if (exitReason != JOB_NOT_STARTED) {
		if (shutdownPolicy == WAIT_FOR_ALL) {
			for ( int i=0 ; i<=ResourceList.getlast() ; i++ ) {
				RemoteResource *r = ResourceList[i];
				// If the policy is wait for all nodes to exit
				// see if any are still running.  If so,
				// just return, and wait for them all to go
				if (r->getResourceState() != RR_FINISHED) {
					return;
				}
			}
			
		}
			// If node0 is still running, don't really shut down
		RemoteResource *r =  ResourceList[0];
		if (r->getResourceState() != RR_FINISHED) {
			return;
		}
	}
		/* With many resources, we have to figure out if all of
		   them are done, and we have to figure out if we need
		   to kill others.... */

		// This code waits for all nodes to shut down before 
		// we exit.  We may want an option for user jobs to
		// specify which shutdown policy they want
/*
	if( !shutDownLogic( exitReason ) ) {
		return;  // leave if we're not *really* ready to shut down.
	}
*/

	handleJobRemoval(0);

		/* if we're still here, we can call BaseShadow::shutDown() to
		   do the real work, which is shared among all kinds of
		   shadows.  the shutDown process will call other virtual
		   functions to get universe-specific goodness right. */
	BaseShadow::shutDown( exitReason );
}


int 
ParallelShadow::shutDownLogic( int& exitReason ) {

		/* What sucks for us here is that we know we want to shut 
		   down, but we don't know *why* we are shutting down.
		   We have to look through the set of MpiResources
		   and figure out which have exited, how they exited, 
		   and if we should kill them all... Basically, the only
		   time we *don't* remove everything is when all the 
		   resources have exited normally.  */

	dprintf( D_FULLDEBUG, "Entering shutDownLogic(r=%d)\n", 
			 exitReason );

		/* if we have a 'pre-startup' exit reason, we can just
		   dupe that to all resources and exit right away. */
	if ( exitReason == JOB_NOT_STARTED  ||
		 exitReason == JOB_SHADOW_USAGE ) {
		for ( int i=0 ; i<ResourceList.getlast() ; i++ ) {
			(ResourceList[i])->setExitReason( exitReason );
		}
		return TRUE;
	}

		/* Now we know that *something* started... */
	
	int normal_exit = FALSE;

		/* If the job on the resource has exited normally, then
		   we don't want to remove everyone else... */
	if( (exitReason == JOB_EXITED) && !(exitedBySignal()) ) {
		dprintf( D_FULLDEBUG, "Normal exit\n" );
		normal_exit = TRUE;
	}

	if ( (!normal_exit) && (!shutDownMode) ) {
		/* We get here and try to shut everyone down.  Don't worry;
		   this function will only fire once. */
		handleJobRemoval( 666 );

		actualExitReason = exitReason;
		shutDownMode = TRUE;
	}

		/* We now have to figure out if everyone has finished... */
	
	int alldone = TRUE;
	MpiResource *r;

    for ( int i=0 ; i<=ResourceList.getlast() ; i++ ) {
		r = ResourceList[i];
		char *res = NULL;
		r->getMachineName( res );
		dprintf( D_FULLDEBUG, "Resource %s...%13s %d\n", res,
				 rrStateToString(r->getResourceState()), 
				 r->getExitReason() );
		delete [] res;
		switch ( r->getResourceState() )
		{
			case RR_PENDING_DEATH:
				alldone = FALSE;  // wait for results to come in, and
			case RR_FINISHED:
				break;            // move on...
			case RR_PRE: {
					// what the heck is going on? - shouldn't happen.
				r->setExitReason( JOB_NOT_STARTED );
				break;
			}
		    case RR_STARTUP:
			case RR_EXECUTING: {
				if ( !normal_exit ) {
					r->killStarter();
				}
				alldone = FALSE;
				break;
			}
			default: {
				dprintf ( D_ALWAYS, "ERROR: Don't know state %d\n", 
						  r->getResourceState() );
			}
		} // switch()
	} // for()

	if ( (!normal_exit) && shutDownMode ) {
		/* We want the exit reason  to be set to the exit
		   reason of the job that caused us to shut down.
		   Therefore, we set this here: */
		exitReason = actualExitReason;
	}

	if ( alldone ) {
			// everyone has reported in their exit status...
		dprintf( D_FULLDEBUG, "All nodes have finished, ready to exit\n" );
		return TRUE;
	}

	return FALSE;
}

int 
ParallelShadow::handleJobRemoval( int sig ) {

    dprintf ( D_FULLDEBUG, "In handleJobRemoval, sig %d\n", sig );
	remove_requested = true;

	ResourceState s;

    for ( int i=0 ; i<=ResourceList.getlast() ; i++ ) {
		s = ResourceList[i]->getResourceState();
		if( s == RR_EXECUTING || s == RR_STARTUP ) {
			ResourceList[i]->setExitReason( JOB_KILLED );
			ResourceList[i]->killStarter();
		}
    }

	return 0;
}

/* This is basically a search-and-replace "#MpInOdE#" with a number 
   for that node...so we can address each mpi node in the submit file. */
void
ParallelShadow::replaceNode ( ClassAd *ad, int nodenum ) {

	ExprTree *tree = NULL;
	char node[9];
	const char *lhstr, *rhstr;

	snprintf( node, 9, "%d", nodenum );

	ad->ResetExpr();
	while( ad->NextExpr(lhstr, tree) ) {
		rhstr = ExprTreeToString(tree);
		if( !lhstr || !rhstr ) {
			dprintf( D_ALWAYS, "Could not replace $(NODE) in ad!\n" );
			return;
		}

		MyString strRh(rhstr);
		if (strRh.replaceString("#pArAlLeLnOdE#", node))
		{
			ad->AssignExpr( lhstr, strRh.Value() );
			dprintf( D_FULLDEBUG, "Replaced $(NODE), now using: %s = %s\n", 
					 lhstr, strRh.Value() );
		}
	}	
}


int
ParallelShadow::updateFromStarter(int  /*command*/, Stream *s)
{
	ClassAd update_ad;
	s->decode();
	update_ad.initFromStream(*s);
	s->end_of_message();
	return updateFromStarterClassAd(&update_ad);
}

int
ParallelShadow::updateFromStarterClassAd(ClassAd* update_ad)
{
	ClassAd *job_ad = getJobAd();
	MpiResource* mpi_res = NULL;
	int mpi_node = -1;

		// First, figure out what remote resource this info belongs to. 
	if( ! update_ad->LookupInteger(ATTR_NODE, mpi_node) ) {
			// No ATTR_NODE in the update ad!
		dprintf( D_ALWAYS, "ERROR in ParallelShadow::updateFromStarter: "
				 "no %s defined in update ad, can't process!\n",
				 ATTR_NODE );
		return FALSE;
	}
	if( ! (mpi_res = findResource(mpi_node)) ) {
		dprintf( D_ALWAYS, "ERROR in ParallelShadow::updateFromStarter: "
				 "can't find remote resource for node %d, "
				 "can't process!\n", mpi_node );
		return FALSE;
	}

	int prev_disk = getDiskUsage();
	struct rusage prev_rusage = getRUsage();


		// Now, we're in good shape.  Grab all the info we care about
		// and put it in the appropriate place.
	mpi_res->updateFromStarter(update_ad);

		// XXX TODO: Do we want to update our local job ad?  Do we
		// want to store the maximum in there?  Seperate stuff for
		// each node?  

    int cur_disk = getDiskUsage();
    if( cur_disk > prev_disk ) {
		job_ad->Assign(ATTR_DISK_USAGE, cur_disk);
    }

    struct rusage cur_rusage = getRUsage();
    if( cur_rusage.ru_stime.tv_sec > prev_rusage.ru_stime.tv_sec ) {
        job_ad->Assign(ATTR_JOB_REMOTE_SYS_CPU,
					   (float)cur_rusage.ru_stime.tv_sec);
    }
    if( cur_rusage.ru_utime.tv_sec > prev_rusage.ru_utime.tv_sec ) {
        job_ad->Assign(ATTR_JOB_REMOTE_USER_CPU,
					   (float)cur_rusage.ru_utime.tv_sec);
    }
	return TRUE;
}


MpiResource*
ParallelShadow::findResource( int node )
{
	MpiResource* mpi_res;
	int i;
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		if( node == mpi_res->getNode() ) {
			return mpi_res;
		}
	}
	return NULL;
}


struct rusage
ParallelShadow::getRUsage( void ) 
{
	MpiResource* mpi_res;
	struct rusage total;
	struct rusage cur;
	int i;
	memset( &total, 0, sizeof(struct rusage) );
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		cur = mpi_res->getRUsage();
		total.ru_stime.tv_sec += cur.ru_stime.tv_sec;
		total.ru_utime.tv_sec += cur.ru_utime.tv_sec;
	}
	return total;
}


int
ParallelShadow::getImageSize( void )
{
	MpiResource* mpi_res;
	int i, max = 0, val;
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		val = mpi_res->getImageSize();
		if( val > max ) {
			max = val;
		}
	}
	return max;
}


int
ParallelShadow::getDiskUsage( void )
{
	MpiResource* mpi_res;
	int i, max = 0, val;
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		val = mpi_res->getDiskUsage();
		if( val > max ) {
			max = val;
		}
	}
	return max;
}


float
ParallelShadow::bytesSent( void )
{
	MpiResource* mpi_res;
	int i;
	float total = 0;
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		total += mpi_res->bytesSent();
	}
	return total;
}


float
ParallelShadow::bytesReceived( void )
{
	MpiResource* mpi_res;
	int i;
	float total = 0;
	for( i=0; i<=ResourceList.getlast() ; i++ ) {
		mpi_res = ResourceList[i];
		total += mpi_res->bytesReceived();
	}
	return total;
}

int
ParallelShadow::getExitReason( void )
{
	if( ResourceList[0] ) {
		return ResourceList[0]->getExitReason();
	}
	return -1;
}


bool
ParallelShadow::setMpiMasterInfo( char*   /*str*/ )
{
	return false;
}


bool
ParallelShadow::exitedBySignal( void )
{
	if( ResourceList[0] ) {
		return ResourceList[0]->exitedBySignal();
	}
	return false;
}


int
ParallelShadow::exitSignal( void )
{
	if( ResourceList[0] ) {
		return ResourceList[0]->exitSignal();
	}
	return -1;
}


int
ParallelShadow::exitCode( void )
{
	if( ResourceList[0] ) {
		return ResourceList[0]->exitCode();
	}
	return -1;
}


void
ParallelShadow::resourceBeganExecution( RemoteResource* rr )
{
	bool all_executing = true;

	int i;
	for( i=0; i<=ResourceList.getlast() && all_executing ; i++ ) {
		if( ResourceList[i]->getResourceState() != RR_EXECUTING ) {
			all_executing = false;
		}
	}

	if( all_executing ) {
			// All nodes in this computation are now running, so we 
			// can finally log the execute event.
		ExecuteEvent event;
		event.setExecuteHost( "MPI_job" );
		if ( !uLog.writeEvent( &event, jobAd )) {
			dprintf ( D_ALWAYS, "Unable to log EXECUTE event." );
		}
		
			// Now that everything is started, we can finally invoke
			// the base copy of this function to handle shared code.
		BaseShadow::resourceBeganExecution(rr);
	}
}


void
ParallelShadow::resourceReconnected( RemoteResource*  /*rr*/ )
{
		//EXCEPT( "impossible: MPIShadow doesn't support reconnect" );
}


void
ParallelShadow::logDisconnectedEvent( const char* reason )
{
	JobDisconnectedEvent event;
	event.setDisconnectReason( reason );

/*
	DCStartd* dc_startd = remRes->getDCStartd();
	if( ! dc_startd ) {
		EXCEPT( "impossible: remRes::getDCStartd() returned NULL" );
	}
	event.setStartdAddr( dc_startd->addr() );
	event.setStartdName( dc_startd->name() );

	if( !uLog.writeEvent(&event,jobAd) ) {
		dprintf( D_ALWAYS, "Unable to log ULOG_JOB_DISCONNECTED event\n" );
	}
*/
}


void
ParallelShadow::logReconnectedEvent( void )
{
	JobReconnectedEvent event;

/*
	DCStartd* dc_startd = remRes->getDCStartd();
	if( ! dc_startd ) {
		EXCEPT( "impossible: remRes::getDCStartd() returned NULL" );
	}
	event.setStartdAddr( dc_startd->addr() );
	event.setStartdName( dc_startd->name() );

	char* starter = NULL;
	remRes->getStarterAddress( starter );
	event.setStarterAddr( starter );
	delete [] starter;
	starter = NULL;

*/
	if( !uLog.writeEvent(&event,jobAd) ) {
		dprintf( D_ALWAYS, "Unable to log ULOG_JOB_RECONNECTED event\n" );
	}

}


void
ParallelShadow::logReconnectFailedEvent( const char* reason )
{
	JobReconnectFailedEvent event;

	event.setReason( reason );

/*
	DCStartd* dc_startd = remRes->getDCStartd();
	if( ! dc_startd ) {
		EXCEPT( "impossible: remRes::getDCStartd() returned NULL" );
	}
	event.setStartdName( dc_startd->name() );
*/

	if( !uLog.writeEvent(&event,jobAd) ) {
		dprintf( D_ALWAYS, "Unable to log ULOG_JOB_RECONNECT_FAILED event\n" );
	}
		//EXCEPT( "impossible: MPIShadow doesn't support reconnect" );
}

bool
ParallelShadow::updateJobAttr( const char *name, const char *expr, bool log )
{
	return job_updater->updateAttr( name, expr, true, log );
}

bool
ParallelShadow::updateJobAttr( const char *name, int value, bool log )
{
	return job_updater->updateAttr( name, value, true, log );
}

