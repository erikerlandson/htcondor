 ###############################################################
 # 
 # Copyright (C) 2009-2011, Red Hat, Inc.
 # 
 # Licensed under the Apache License, Version 2.0 (the "License"); you 
 # may not use this file except in compliance with the License.  You may 
 # obtain a copy of the License at 
 # 
 #    http://www.apache.org/licenses/LICENSE-2.0 
 # 
 # Unless required by applicable law or agreed to in writing, software 
 # distributed under the License is distributed on an "AS IS" BASIS, 
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and 
 # limitations under the License. 
 # 
 ###############################################################

find_multiple( "mongoclient" MONGODB_FOUND )
find_multiple( "boost_system" BOOSTDEP_FOUND )

if (MONGODB_FOUND AND BOOSTDEP_FOUND)
	set (MONGODB_FOUND "${MONGODB_FOUND};${BOOSTDEP_FOUND}")
	message (STATUS "plumage deps found (${MONGODB_FOUND}) and will build")

	append_var (MONGODB_INCLUDES "/usr/include/mongo")

	############################
	# file target references.
	############################

	file( GLOB ETL_COMMON_SRCS ODSMongodbOps.cpp ODSPoolUtils.cpp)

	file( GLOB ETL_SERVER_SRCS ods_job_daemon_main.cpp 
						ODSJobLogConsumer.cpp
						ODSHistoryFile.cpp ODSHistoryUtils.cpp 
						${ETL_COMMON_SRCS} )

	############################
	# daemon emitter plugins
	############################
	#src_target_ref( condor_collector "hashkey" COLLECTOR_REF )
	#src_target_ref( utils "set_user_priv_from_ad" UTILS_REF )

	file( GLOB CollectorSrcs ODSCollectorPlugin.cpp ODSAccountant.cpp ODSProcessors.cpp ${ETL_COMMON_SRCS})
	condor_plugin( ODSCollectorPlugin-plugin "${CollectorSrcs}" "${C_LIBEXEC}" "${MONGODB_FOUND}" ON)

	############################
	# job etl daemon
	############################
	condor_exe( ods_job_etl_server "${ETL_SERVER_SRCS}"
	  			"${C_SBIN}" "${CONDOR_LIBS};${PCRE_FOUND};${OPENSSL_FOUND};${COREDUMPER_FOUND};${KRB5_FOUND};${GLOBUS_FOUND};${MONGODB_FOUND}" OFF )

	############################
	# history loader tool
	############################
	set(TOOL_LINK_LIBS "${CONDOR_TOOL_LIBS};${PCRE_FOUND};${OPENSSL_FOUND};${KRB5_FOUND}")
	condor_exe(ods_job_etl_tool "ods_history_tool_main.cpp" ${C_BIN} "${TOOL_LINK_LIBS};${MONGODB_FOUND}" OFF)

	############################
	# install targets
	############################
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../etc/62plumage.config DESTINATION "${C_ETC_EXAMPLES}")
	file(GLOB PLUMAGE_STATS ${CMAKE_CURRENT_SOURCE_DIR}/../test/plumage_stats )
	install(FILES ${PLUMAGE_STATS} DESTINATION ${C_BIN} PERMISSIONS ${CONDOR_SCRIPT_PERMS})
	file(GLOB TEST_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/../test/plumage_*)
	list(REMOVE_ITEM TEST_SCRIPTS ${PLUMAGE_STATS})
	
	install(FILES ${TEST_SCRIPTS} DESTINATION ${C_LIB}/plumage PERMISSIONS ${CONDOR_SCRIPT_PERMS})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README DESTINATION ${C_LIB}/plumage)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../SCHEMA DESTINATION ${C_LIB}/plumage)
else()
	message (STATUS "plumage deps *not found*")
endif()
