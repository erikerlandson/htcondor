/*
 *   This file was automatically generated by cextract version 1.2.
 *   Manual editing not recommended.
 *
 *   Created: Fri Oct 16 12:47:29 1992
 */

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __CEXTRACT__
#if defined(HAS_PROTO) || defined(__STDC__) || defined(__cplusplus)

int blankline ( char *str );
char * gen_ckpt_name ( char *directory, int cluster, int proc, int subproc );
char * gen_exec_name ( int cluster, int proc, int subproc );
void init_config ( void );
void config ( char *a_out_name, CONTEXT *context );
char * param ( char *name );
char * macro_expand ( char *name );
int boolean ( char *parameter, char *pattern );
void lower_case ( register char *str );
int hash ( register char *string, register int size );
void insert ( char *name, char *value, BUCKET *table[], int table_size );
int read_config ( char *tilde, char *config_name, CONTEXT *context, BUCKET *table[], int table_size, int expand_flag );
char * getline ( FILE *fp );
char * expand_macro ( char *value, BUCKET *table[], int table_size );
char * lookup_macro ( char *name, BUCKET *table[], int table_size );
void schedule_event ( int month, int day, int hour, int minute, int second, void (*func)() );
void event_mgr ( void );
void StartRecording ( void );
void CompleteRecording ( long numberOfBytes );
void ProcessLogging ( int request, int extraInteger );
void detach ( void );
int do_connect ( char *host, char *service, u_int port );
int udp_connect ( char *host, u_int port );
void dprintf_init ( int fd );
void dprintf ( int flags, char *fmt, ... );
FILE * debug_lock ( void );
void debug_unlock ( void );
void _EXCEPT_ ( char *fmt, ... );
EXPR * scan ( char *line );
EXPR * create_expr ( void );
void add_elem ( ELEM *elem, EXPR *expr );
void free_expr ( EXPR *expr );
void display_elem ( ELEM *elem, FILE *log_fp );
ELEM * eval ( char *name, CONTEXT *cont1, CONTEXT *cont2 );
void free_elem ( ELEM *elem );
ELEM * create_elem ( void );
int store_stmt ( EXPR *expr, CONTEXT *context );
CONTEXT * create_context ( void );
void free_context ( CONTEXT *context );
void display_context ( CONTEXT *context );
EXPR * search_expr ( char *name, CONTEXT *cont1, CONTEXT *cont2 );
EXPR * build_expr ( char *name, ELEM *val );
int evaluate_bool ( char *name, int *answer, CONTEXT *context1, CONTEXT *context2 );
int evaluate_float ( char *name, float *answer, CONTEXT *context1, CONTEXT *context2 );
int evaluate_int ( char *name, int *answer, CONTEXT *context1, CONTEXT *context2 );
int evaluate_string ( char *name, char **answer, CONTEXT *context1, CONTEXT *context2 );
int flock ( int fd, int op );
int getdtablesize ( void );
int getmnt ( int *start, struct fs_data buf[], unsigned bufsize, int mode, char *path );

/* int getpagesize ( void ); */
/* long unsigned int getpagesize( void ); */

char * getwd ( char *path );
XDR * OpenHistory ( char *file, XDR *xdrs, int *fd );
XDR * xdr_Init( int *sock, XDR *xdrs );
void CloseHistory ( XDR *H );
void AppendHistory ( XDR *H, PROC *p );
int ScanHistory ( XDR *H, void (*func)(PROC *) );
int LockHistory ( XDR *H, int op );
int insque ( struct qelem *elem, struct qelem *pred );
int remque ( struct qelem *elem );
DBM * OpenJobQueue ( char *path, int flags, int mode );
int CloseJobQueue ( DBM *Q );
int LockJobQueue ( DBM *Q, int op );
int CreateCluster ( DBM *Q );
int StoreProc ( DBM *Q, PROC *proc );
int FetchProc ( DBM *Q, PROC *proc );

#if defined(__cplusplus)
int ScanJobQueue ( DBM *Q, void (*func)(PROC *) );
int ScanCluster ( DBM *Q, int cluster, void (*func)() );
#else
int ScanJobQueue ( DBM *Q, int (*func)() );
int ScanCluster ( DBM *Q, int cluster, int (*func)() );
#endif

int TerminateCluster ( DBM *Q, int cluster, int status );
int TerminateProc ( DBM *Q, PROC_ID *pid, int status );
CLUSTER_LIST * fetch_cluster_list ( DBM *Q );
int data_too_big ( int size );
#if 0
#if defined(LINUX)
DBM * dbm_open ( const char *file, int flags, int mode );
#else
DBM * dbm_open ( char *file, int flags, int mode );
#endif
int dbm_store ( DBM *Q, datum key, datum data, int flags );
datum dbm_fetch ( DBM *Q, datum key );
int dbm_delete ( DBM *Q, datum key );
#endif
char * ltrunc ( register char *str );
int set_machine_status ( int status );
int get_machine_status ( void );
int mkargv ( int *argc, char *argv[], char *line );
int display_proc_short ( PROC *proc );
char * format_time ( float fp_secs );
int display_proc_long ( PROC *proc );
int display_v2_proc_long ( PROC *proc );
int setegid ( int egid );
int seteuid ( int euid );

#if 0
int setlinebuf ( FILE *fp );
#endif

int setregid ( int rgid, int egid );
int setreuid ( int ruid, int euid );
int setrgid ( int rgid );
int display_status_line ( STATUS_LINE *line, FILE *fp );
char * shorten ( char *state );
int free_status_line ( STATUS_LINE *line );
int print_header ( FILE *fp );
char * format_seconds ( int t_sec );
#if 0
char * strdup ( const char *s );
char * strdup ( char *s );
#endif
int stricmp ( register char *s1, register char *s2 );
int strincmp ( register char *s1, register char *s2, register n );
char * substr ( char *string, char *pattern );
int init_condor_uid ( void );
int set_condor_euid ( void );
int set_condor_ruid ( void );
int set_root_euid ( void );
int update_rusage ( register struct rusage *ru1, register struct rusage *ru2 );
int calc_virt_memory ( void );
int free_fs_blocks(char *filename);
int calc_disk_needed( PROC * proc );
char *GetEnvParam( char * param, char * env_string );
PROC *ConstructProc( int, PROC *);

#else /* HAS_PROTO */

int blankline ();
char * gen_ckpt_name ();
char * gen_exec_name ();
void init_config ();
void config ();
char * param ();
int boolean ();
void lower_case ();
int hash ();
void insert ();
int read_config ();
char * getline ();
char * expand_macro ();
char * lookup_macro ();
void schedule_event ();
void event_mgr ();
void StartRecording ();
void CompleteRecording ();
void ProcessLogging ();
void detach ();
int do_connect ();
int udp_connect ();
void dprintf_init ();
void dprintf ();
FILE * debug_lock ();
void debug_unlock ();
void _EXCEPT_ ();
EXPR * scan ();
EXPR * create_expr ();
void add_elem ();
void free_expr ();
void display_elem ();
ELEM * eval ();
void free_elem ();
ELEM * create_elem ();
int store_stmt ();
CONTEXT * create_context ();
void free_context ();
void display_context ();
EXPR * search_expr ();
EXPR * build_expr ();
int evaluate_bool ();
int evaluate_float ();
int evaluate_int ();
int evaluate_string ();
int flock ();
int getdtablesize ();
int getmnt ();
/* int getpagesize (); */
char * getwd ();
XDR * OpenHistory ();
XDR * xdr_Init ();
void CloseHistory ();
void AppendHistory ();
int ScanHistory ();
int LockHistory ();
int insque ();
int remque ();
DBM * OpenJobQueue ();
int CloseJobQueue ();
int LockJobQueue ();
int CreateCluster ();
int StoreProc ();
int FetchProc ();
int ScanJobQueue ();
int ScanCluster ();
int TerminateCluster ();
int TerminateProc ();
CLUSTER_LIST * fetch_cluster_list ();
int data_too_big ();
DBM * dbm_open ();
int dbm_store ();
datum dbm_fetch ();
int dbm_delete ();
char * ltrunc ();
int set_machine_status ();
int get_machine_status ();
int mkargv ();
int display_proc_short ();
char * format_time ();
int display_proc_long ();
int display_v2_proc_long ();
int setegid ();
int seteuid ();
int setregid ();
int setreuid ();
int setrgid ();
int display_status_line ();
char * shorten ();
int free_status_line ();
int print_header ();
char * format_seconds ();
char * strdup ();
int stricmp ();
int strincmp ();
char * substr ();
int init_condor_uid ();
int set_condor_euid ();
int set_condor_ruid ();
int set_root_euid ();
int update_rusage ();
int calc_virt_memory ();
int free_fs_blocks();
int calc_disk_needed();
char *GetEnvParam();
PROC *ConstructProc();

#endif /* HAS_PROTO */
#endif /* __CEXTRACT__ */

#if defined(__cplusplus)
}		/* End of extern "C" declaration */
#endif
