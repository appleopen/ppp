/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * pppd.h - PPP daemon global declarations.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Id: pppd.h,v 1.16 2003/09/21 03:05:21 callie Exp $
 */

/*
 * TODO:
 */

#ifndef __PPPD_H__
#define __PPPD_H__

#include <stdio.h>		/* for FILE */
#include <limits.h>		/* for NGROUPS_MAX */
#include <sys/param.h>		/* for MAXPATHLEN and BSD4_4, if defined */
#include <sys/types.h>		/* for u_int32_t, if defined */
#include <sys/time.h>		/* for struct timeval */
#ifdef __APPLE__
#include <ppp_defs.h>
#else
#include <net/ppp_defs.h>
#endif
#include "patchlevel.h"

#if defined(__STDC__)
#include <stdarg.h>
#define __V(x)	x
#else
#include <varargs.h>
#define __V(x)	(va_alist) va_dcl
#define const
#define volatile
#endif

#ifdef INET6
#include "eui64.h"
#endif

/*
 * Limits.
 */

#define NUM_PPP		1	/* One PPP interface supported (per process) */
#define MAXWORDLEN	1024	/* max length of word in file (incl null) */
#define MAXARGS		1	/* max # args to a command */
#define MAXNAMELEN	256	/* max length of hostname or name for auth */
#define MAXSECRETLEN	256	/* max length of password or secret */

/*
 * Option descriptor structure.
 */
 
#ifdef __APPLE__
// bool is defined in headers as 4 bytes
#undef bool
#endif

typedef unsigned char	bool;

enum opt_type {
	o_special_noarg = 0,
	o_special = 1,
	o_bool,
	o_int,
	o_uint32,
	o_string,
	o_wild
};

typedef struct {
	char	*name;		/* name of the option */
	enum opt_type type;
	void	*addr;
	char	*description;
	unsigned int flags;
	void	*addr2;
	int	upper_limit;
	int	lower_limit;
	const char *source;
	short int priority;
	short int winner;
	void	*addr3;
} option_t;

/* Values for flags */
#define OPT_VALUE	0xff	/* mask for presupplied value */
#define OPT_HEX		0x100	/* int option is in hex */
#define OPT_NOARG	0x200	/* option doesn't take argument */
#define OPT_OR		0x400	/* OR in argument to value */
#define OPT_INC		0x800	/* increment value */
#define OPT_A2OR	0x800	/* for o_bool, OR arg to *(u_char *)addr2 */
#define OPT_PRIV	0x1000	/* privileged option */
#define OPT_STATIC	0x2000	/* string option goes into static array */
#define OPT_LLIMIT	0x4000	/* check value against lower limit */
#define OPT_ULIMIT	0x8000	/* check value against upper limit */
#define OPT_LIMITS	(OPT_LLIMIT|OPT_ULIMIT)
#define OPT_ZEROOK	0x10000	/* 0 value is OK even if not within limits */
#define OPT_HIDE	0x10000	/* for o_string, print value as ?????? */
#define OPT_A2LIST	0x10000 /* for o_special, keep list of values */
#define OPT_A2CLRB	0x10000 /* o_bool, clr val bits in *(u_char *)addr2 */
#define OPT_NOINCR	0x20000	/* value mustn't be increased */
#define OPT_ZEROINF	0x40000	/* with OPT_NOINCR, 0 == infinity */
#define OPT_PRIO	0x80000	/* process option priorities for this option */
#define OPT_PRIOSUB	0x100000 /* subsidiary member of priority group */
#define OPT_ALIAS	0x200000 /* option is alias for previous option */
#define OPT_A2COPY	0x400000 /* addr2 -> second location to rcv value */
#define OPT_ENABLE	0x800000 /* use *addr2 as enable for option */
#define OPT_A2CLR	0x1000000 /* clear *(bool *)addr2 */
#define OPT_PRIVFIX	0x2000000 /* user can't override if set by root */
#define OPT_INITONLY	0x4000000 /* option can only be set in init phase */
#define OPT_DEVEQUIV	0x8000000 /* equiv to device name */
#define OPT_DEVNAM	(OPT_INITONLY | OPT_DEVEQUIV)
#define OPT_A2PRINTER	0x10000000 /* *addr2 is a fn for printing option */
#define OPT_A2STRVAL	0x20000000 /* *addr2 points to current string value */
#define OPT_NOPRINT	0x40000000 /* don't print this option at all */

#define OPT_VAL(x)	((x) & OPT_VALUE)

/* Values for priority */
#define OPRIO_DEFAULT	0	/* a default value */
#define OPRIO_CFGFILE	1	/* value from a configuration file */
#define OPRIO_CMDLINE	2	/* value from the command line */
#define OPRIO_SECFILE	3	/* value from options in a secrets file */
#define OPRIO_ROOT	100	/* added to priority if OPT_PRIVFIX && root */

#ifndef GIDSET_TYPE
#define GIDSET_TYPE	gid_t
#endif

/* Structure representing a list of permitted IP addresses. */
struct permitted_ip {
    int		permit;		/* 1 = permit, 0 = forbid */
    u_int32_t	base;		/* match if (addr & mask) == base */
    u_int32_t	mask;		/* base and mask are in network byte order */
};

/*
 * Unfortunately, the linux kernel driver uses a different structure
 * for statistics from the rest of the ports.
 * This structure serves as a common representation for the bits
 * pppd needs.
 */
struct pppd_stats {
    unsigned int	bytes_in;
    unsigned int	bytes_out;
    unsigned int	pkts_in;
    unsigned int	pkts_out;
};

/* Used for storing a sequence of words.  Usually malloced. */
struct wordlist {
    struct wordlist	*next;
    char		*word;
};

/* An endpoint discriminator, used with multilink. */
#define MAX_ENDP_LEN	20	/* maximum length of discriminator value */
struct epdisc {
    unsigned char	class;
    unsigned char	length;
    unsigned char	value[MAX_ENDP_LEN];
};

/* values for epdisc.class */
#define EPD_NULL	0	/* null discriminator, no data */
#define EPD_LOCAL	1
#define EPD_IP		2
#define EPD_MAC		3
#define EPD_MAGIC	4
#define EPD_PHONENUM	5

typedef void (*notify_func) __P((void *, int));

struct notifier {
    struct notifier *next;
    notify_func	    func;
    void	    *arg;
};

/*
 * Global variables.
 */

extern int	hungup;		/* Physical layer has disconnected */
extern int	ifunit;		/* Interface unit number */
extern char	ifname[];	/* Interface name */
extern char	hostname[];	/* Our hostname */
extern u_char	outpacket_buf[]; /* Buffer for outgoing packets */
extern int	phase;		/* Current state of link - see values below */
extern int	baud_rate;	/* Current link speed in bits/sec */
extern char	*progname;	/* Name of this program */
extern int	redirect_stderr;/* Connector's stderr should go to file */
extern char	peer_authname[];/* Authenticated name of peer */
extern int	auth_done[NUM_PPP]; /* Methods actually used for auth */
extern int	privileged;	/* We were run by real-uid root */
extern int	need_holdoff;	/* Need holdoff period after link terminates */
extern char	**script_env;	/* Environment variables for scripts */
extern int	detached;	/* Have detached from controlling tty */
extern GIDSET_TYPE groups[NGROUPS_MAX];	/* groups the user is in */
extern int	ngroups;	/* How many groups valid in groups */
extern struct pppd_stats link_stats; /* byte/packet counts etc. for link */
extern int	link_stats_valid; /* set if link_stats is valid */
extern int	link_connect_time; /* time the link was up for */
extern int	using_pty;	/* using pty as device (notty or pty opt.) */
extern int	log_to_fd;	/* logging to this fd as well as syslog */
extern bool	log_default;	/* log_to_fd is default (stdout) */
extern char	*no_ppp_msg;	/* message to print if ppp not in kernel */
extern volatile int status;	/* exit status for pppd */
#ifdef __APPLE__
extern int 	optionsfd;	/* parameters are given via pipe */
extern volatile int devstatus;	/* exit device status for pppd */
extern char	username[MAXNAMELEN];/* Our name for authenticating ourselves */
#endif
extern bool	devnam_fixed;	/* can no longer change devnam */
extern int	unsuccess;	/* # unsuccessful connection attempts */
extern int	do_callback;	/* set if we want to do callback next */
extern int	doing_callback;	/* set if this is a callback */
extern char	ppp_devnam[MAXPATHLEN];
extern char     remote_number[MAXNAMELEN]; /* Remote telephone number, if avail. */
extern int      ppp_session_number; /* Session number (eg PPPoE session) */

extern int	listen_time;	/* time to listen first (ms) */
extern struct notifier *pidchange;   /* for notifications of pid changing */
extern struct notifier *phasechange; /* for notifications of phase changes */
extern struct notifier *exitnotify;  /* for notification that we're exiting */
extern struct notifier *sigreceived; /* notification of received signal */
extern struct notifier *ip_up_notifier; /* IPCP has come up */
extern struct notifier *ip_down_notifier; /* IPCP has gone down */
extern struct notifier *auth_up_notifier; /* peer has authenticated */
extern struct notifier *link_down_notifier; /* link has gone down */

#ifdef __APPLE__
extern u_char inpacket_buf[PPP_MRU+PPP_HDRLEN]; /* buffer for incoming packet */
extern char	*terminal_script;/* Script to etablish connection once modem is connected */
extern char	*altconnect_script;/* alternate script to establish physical link */
extern int 	pty_delay;	/* timeout to wait for the pty command */
extern char 	*device;	 /* device we are using (can be use as a generic device container) */
extern char 	*remoteaddress; /* remoteaddress we are connecting to (can be use as a generic address container) */
extern char 	*altremoteaddress; /* alternate remoteaddress we are connecting to */
extern int 	redialcount;	/* number of time to redial */
extern int 	redialtimer;	/* delay in seconds to wait before to redial */
extern bool 	redialalternate; /* do we redial alternate number */
extern int  	redialingcount;  /* current redialing count */
extern bool  	redialingalternate;  /* currently redialing main or alternate number */
extern int 	busycode;	/* busy error code that triggers the redial */
extern int 	cancelcode;	/* cancel error code for connectors*/
extern int (*start_link_hook) __P((void));
extern int (*link_up_hook) __P((void));
extern bool link_up_done;
extern void (*wait_input_hook) __P((void));

extern int (*terminal_window_hook) __P((char *, int, int));

extern struct notifier *auth_start_notify;
extern struct notifier *auth_withpeer_fail_notify;
extern struct notifier *auth_withpeer_success_notify;
extern struct notifier *auth_peer_success_notify;
/* struct send with auth_peer_success_notify */
struct auth_peer_success_info {
    int protocol;
    int protocol_flavor;
    char *name;
    int namelen;
};

extern struct notifier *lcp_up_notify;
extern struct notifier *lcp_down_notify;
extern struct notifier *lcp_lowerup_notify;
extern struct notifier *lcp_lowerdown_notify;

extern struct notifier *lcp_timeremaining_notify;
/* struct send with lcp_timeremaining_notify */
struct lcp_timeremaining_info {
    int	time;	/* time remaining in seconds */
    char *text;	/* optional text sent by the server */
    int textlen;/* len of the text */
};

extern struct notifier *ip_up_notify;
extern struct notifier *ip_down_notify;

extern struct notifier *initscript_started_notify;
extern struct notifier *initscript_finished_notify;
extern struct notifier *connectscript_started_notify;
extern struct notifier *connectscript_finished_notify;
extern struct notifier *terminalscript_started_notify;
extern struct notifier *terminalscript_finished_notify;

extern struct notifier *connect_started_notify;
extern struct notifier *connect_success_notify;
extern struct notifier *connect_fail_notify;
extern struct notifier *disconnect_started_notify;
extern struct notifier *disconnect_done_notify;

extern struct notifier *stop_notify;
extern struct notifier *cont_notify;
#endif

/* Values for do_callback and doing_callback */
#define CALLBACK_DIALIN		1	/* we are expecting the call back */
#define CALLBACK_DIALOUT	2	/* we are dialling out to call back */

/*
 * Variables set by command-line options.
 */

extern int	debug;		/* Debug flag */
extern int	kdebugflag;	/* Tell kernel to print debug messages */
extern int	default_device;	/* Using /dev/tty or equivalent */
extern char	devnam[MAXPATHLEN];	/* Device name */
extern int	crtscts;	/* Use hardware flow control */
extern bool	modem;		/* Use modem control lines */
extern int	inspeed;	/* Input/Output speed requested */
extern u_int32_t netmask;	/* IP netmask to set on interface */
extern bool	lockflag;	/* Create lock file to lock the serial dev */
extern bool	nodetach;	/* Don't detach from controlling tty */
extern bool	updetach;	/* Detach from controlling tty when link up */
extern char	*initializer;	/* Script to initialize physical link */
extern char	*connect_script; /* Script to establish physical link */
extern char	*disconnect_script; /* Script to disestablish physical link */
extern char	*welcomer;	/* Script to welcome client after connection */
extern char	*ptycommand;	/* Command to run on other side of pty */
extern int	maxconnect;	/* Maximum connect time (seconds) */
extern char	user[MAXNAMELEN];/* Our name for authenticating ourselves */
extern char	passwd[MAXSECRETLEN];	/* Password for PAP or CHAP */
extern bool	auth_required;	/* Peer is required to authenticate */
extern bool	persist;	/* Reopen link after it goes down */
extern bool	uselogin;	/* Use /etc/passwd for checking PAP */
extern char	our_name[MAXNAMELEN];/* Our name for authentication purposes */
extern char	remote_name[MAXNAMELEN]; /* Peer's name for authentication */
extern bool	explicit_remote;/* remote_name specified with remotename opt */
extern bool	demand;		/* Do dial-on-demand */
extern char	*ipparam;	/* Extra parameter for ip up/down scripts */
extern bool	cryptpap;	/* Others' PAP passwords are encrypted */
extern int	idle_time_limit;/* Shut down link if idle for this long */
extern bool    	noidlerecv;     /* Disconnect if idle only for outgoing traffic */
extern int	holdoff;	/* Dead time before restarting */
extern bool	holdoff_specified; /* true if user gave a holdoff value */
extern bool	notty;		/* Stdin/out is not a tty */
extern char	*pty_socket;	/* Socket to connect to pty */
extern char	*record_file;	/* File to record chars sent/received */
extern bool	sync_serial;	/* Device is synchronous serial device */
extern int	maxfail;	/* Max # of unsuccessful connection attempts */
extern char	linkname[MAXPATHLEN]; /* logical name for link */
extern bool	tune_kernel;	/* May alter kernel settings as necessary */
extern int	connect_delay;	/* Time to delay after connect script */
extern int	max_data_rate;	/* max bytes/sec through charshunt */
extern int	req_unit;	/* interface unit number to use */
extern bool	multilink;	/* enable multilink operation */
extern bool	noendpoint;	/* don't send or accept endpt. discrim. */
extern char	*bundle_name;	/* bundle name for multilink */
extern bool	dump_options;	/* print out option values */
extern bool	dryrun;		/* check everything, print options, exit */

#ifdef MAXOCTETS
extern unsigned int maxoctets;	     /* Maximum octetes per session (in bytes) */
extern int       maxoctets_dir;      /* Direction :
				      0 - in+out (default)
				      1 - in 
				      2 - out
				      3 - max(in,out) */
extern int       maxoctets_timeout;  /* Timeout for check of octets limit */
#define PPP_OCTETS_DIRECTION_SUM        0
#define PPP_OCTETS_DIRECTION_IN         1
#define PPP_OCTETS_DIRECTION_OUT        2
#define PPP_OCTETS_DIRECTION_MAXOVERAL  3
/* same as previos, but little different on RADIUS side */
#define PPP_OCTETS_DIRECTION_MAXSESSION 4	
#endif

#ifdef PPP_FILTER
extern struct	bpf_program pass_filter;   /* Filter for pkts to pass */
extern struct	bpf_program active_filter; /* Filter for link-active pkts */
#endif

#ifdef MSLANMAN
extern bool	ms_lanman;	/* Use LanMan password instead of NT */
				/* Has meaning only with MS-CHAP challenges */
#endif

/* Values for auth_pending, auth_done */
#define PAP_WITHPEER	0x1
#define PAP_PEER	0x2
#define CHAP_WITHPEER	0x4
#define CHAP_PEER	0x8
#ifdef EAP
#define EAP_WITHPEER	0x10
#define EAP_PEER	0x20
#endif
/* Values for auth_done only */
#define CHAP_MD5_WITHPEER	0x10
#define CHAP_MD5_PEER		0x20
#ifdef CHAPMS
#define CHAP_MS_SHIFT		6	/* LSB position for MS auths */
#define CHAP_MS_WITHPEER	0x40
#define CHAP_MS_PEER		0x80
#define CHAP_MS2_WITHPEER	0x100
#define CHAP_MS2_PEER		0x200
#endif

extern char *current_option;	/* the name of the option being parsed */
extern int  privileged_option;	/* set iff the current option came from root */
extern char *option_source;	/* string saying where the option came from */
extern int  option_priority;	/* priority of current options */

/*
 * Values for phase.
 */
#define PHASE_DEAD		0
#define PHASE_INITIALIZE	1
#define PHASE_SERIALCONN	2
#define PHASE_DORMANT		3
#define PHASE_ESTABLISH		4
#define PHASE_AUTHENTICATE	5
#define PHASE_CALLBACK		6
#define PHASE_NETWORK		7
#define PHASE_RUNNING		8
#define PHASE_TERMINATE		9
#define PHASE_DISCONNECT	10
#define PHASE_HOLDOFF		11
#define PHASE_ONHOLD		12
#define PHASE_WAITONBUSY	13

/*
 * The following struct gives the addresses of procedures to call
 * for a particular protocol.
 */
struct protent {
    u_short protocol;		/* PPP protocol number */
    /* Initialization procedure */
    void (*init) __P((int unit));
    /* Process a received packet */
    void (*input) __P((int unit, u_char *pkt, int len));
    /* Process a received protocol-reject */
    void (*protrej) __P((int unit));
    /* Lower layer has come up */
    void (*lowerup) __P((int unit));
    /* Lower layer has gone down */
    void (*lowerdown) __P((int unit));
    /* Open the protocol */
    void (*open) __P((int unit));
    /* Close the protocol */
    void (*close) __P((int unit, char *reason));
    /* Print a packet in readable form */
    int  (*printpkt) __P((u_char *pkt, int len,
			  void (*printer) __P((void *, char *, ...)),
			  void *arg));
    /* Process a received data packet */
    void (*datainput) __P((int unit, u_char *pkt, int len));
    bool enabled_flag;		/* 0 iff protocol is disabled */
    char *name;			/* Text name of protocol */
    char *data_name;		/* Text name of corresponding data protocol */
    option_t *options;		/* List of command-line options */
    /* Check requested options, assign defaults */
    void (*check_options) __P((void));
    /* Configure interface for demand-dial */
    int  (*demand_conf) __P((int unit));
    /* Say whether to bring up link for this pkt */
    int  (*active_pkt) __P((u_char *pkt, int len));
#ifdef __APPLE__
    /* connection is on hold */
    void  (*hold) __P((int unit));
    /* connection resumes */
    void  (*cont) __P((int unit)); 
    int (*state) __P((int unit)); 
#endif
};

/* Table of pointers to supported protocols */
extern struct protent *protocols[];

/*
 * This struct contains pointers to a set of procedures for
 * doing operations on a "channel".  A channel provides a way
 * to send and receive PPP packets - the canonical example is
 * a serial port device in PPP line discipline (or equivalently
 * with PPP STREAMS modules pushed onto it).
 */
struct channel {
	/* set of options for this channel */
	option_t *options;
	/* find and process a per-channel options file */
	void (*process_extra_options) __P((void));
	/* check all the options that have been given */
	void (*check_options) __P((void));
	/* get the channel ready to do PPP, return a file descriptor */
#ifdef __APPLE__
	int  (*connect) __P((int *));
#else
	int  (*connect) __P((void));
#endif
	/* we're finished with the channel */
	void (*disconnect) __P((void));
	/* put the channel into PPP `mode' */
	int  (*establish_ppp) __P((int));
	/* take the channel out of PPP `mode', restore loopback if demand */
	void (*disestablish_ppp) __P((int));
	/* set the transmit-side PPP parameters of the channel */
	void (*send_config) __P((int, u_int32_t, int, int));
	/* set the receive-side PPP parameters of the channel */
	void (*recv_config) __P((int, u_int32_t, int, int));
	/* cleanup on error or normal exit */
	void (*cleanup) __P((void));
	/* close the device, called in children after fork */
	void (*close) __P((void));
#ifdef __APPLE__
	void (*wait_input) __P((void));
#endif
};

extern struct channel *the_channel;

#define ppp_send_config(unit, mtu, accm, pc, acc)			 \
do {									 \
	if (the_channel->send_config)					 \
		(*the_channel->send_config)((mtu), (accm), (pc), (acc)); \
} while (0)

#define ppp_recv_config(unit, mtu, accm, pc, acc)			 \
do {									 \
	if (the_channel->send_config)					 \
		(*the_channel->recv_config)((mtu), (accm), (pc), (acc)); \
} while (0)

/*
 * Prototypes.
 */

/* Procedures exported from main.c. */
void set_ifunit __P((int));	/* set stuff that depends on ifunit */
void detach __P((void));	/* Detach from controlling tty */
void die __P((int));		/* Cleanup and exit */
void quit __P((void));		/* like die(1) */
void novm __P((char *));	/* Say we ran out of memory, and die */
void timeout __P((void (*func)(void *), void *arg, int s, int us));
                                /* Call func(arg) after s.us seconds */
void untimeout __P((void (*func)(void *), void *arg));
				/* Cancel call to func(arg) */
void record_child __P((int, char *, void (*) (void *), void *));
int  device_script __P((char *cmd, int in, int out, int dont_wait));
				/* Run `cmd' with given stdin and stdout */
pid_t run_program __P((char *prog, char **args, int must_exist,
		       void (*done)(void *), void *arg));
				/* Run program prog with args in child */
void reopen_log __P((void));	/* (re)open the connection to syslog */
void update_link_stats __P((int)); /* Get stats at link termination */
void script_setenv __P((char *, char *, int));	/* set script env var */
void script_unsetenv __P((char *));		/* unset script env var */
void new_phase __P((int));	/* signal start of new phase */
void add_notifier __P((struct notifier **, notify_func, void *));
void remove_notifier __P((struct notifier **, notify_func, void *));
void notify __P((struct notifier *, int));

/* Procedures exported from tty.c. */
void tty_init __P((void));

/* Procedures exported from utils.c. */
void log_packet __P((u_char *, int, char *, int));
				/* Format a packet and log it with syslog */
void print_string __P((char *, int,  void (*) (void *, char *, ...),
		void *));	/* Format a string for output */
int slprintf __P((char *, int, char *, ...));		/* sprintf++ */
int vslprintf __P((char *, int, char *, va_list));	/* vsprintf++ */
size_t strlcpy __P((char *, const char *, size_t));	/* safe strcpy */
size_t strlcat __P((char *, const char *, size_t));	/* safe strncpy */
void dbglog __P((char *, ...));	/* log a debug message */
void info __P((char *, ...));	/* log an informational message */
void notice __P((char *, ...));	/* log a notice-level message */
void warning __P((char *, ...));	/* log a warning message */
void error __P((char *, ...));	/* log an error message */
void fatal __P((char *, ...));	/* log an error message and die(1) */
void init_pr_log __P((char *, int));	/* initialize for using pr_log */
void pr_log __P((void *, char *, ...));	/* printer fn, output to syslog */
void end_pr_log __P((void));	/* finish up after using pr_log */
void dump_packet __P((const char *, u_char *, int));
				/* dump packet to debug log if interesting */

/* Procedures exported from auth.c */
void link_required __P((int));	  /* we are starting to use the link */
void link_terminated __P((int));  /* we are finished with the link */
void link_down __P((int));	  /* the LCP layer has left the Opened state */
void link_established __P((int)); /* the link is up; authenticate now */
void start_networks __P((int));   /* start all the network control protos */
void continue_networks __P((int)); /* start network [ip, etc] control protos */
void np_up __P((int, int));	  /* a network protocol has come up */
void np_down __P((int, int));	  /* a network protocol has gone down */
void np_finished __P((int, int)); /* a network protocol no longer needs link */
void auth_peer_fail __P((int, int));
				/* peer failed to authenticate itself */
void auth_peer_success __P((int, int, int, char *, int));
				/* peer successfully authenticated itself */
void auth_withpeer_fail __P((int, int));
				/* we failed to authenticate ourselves */
#ifdef __APPLE__
void auth_withpeer_cancelled __P((int, int));
				/* authentication cancelled by user */
#endif
void auth_withpeer_success __P((int, int, int));
				/* we successfully authenticated ourselves */
void auth_check_options __P((void));
				/* check authentication options supplied */
void auth_reset __P((int));	/* check what secrets we have */
int  check_passwd __P((int, char *, int, char *, int, char **));
				/* Check peer-supplied username/password */
int  get_secret __P((int, char *, char *, char *, int *, int));
				/* get "secret" for chap */
int  auth_ip_addr __P((int, u_int32_t));
				/* check if IP address is authorized */
int  bad_ip_adrs __P((u_int32_t));
				/* check if IP address is unreasonable */
#ifdef __APPLE__
void check_idle __P((void *));
#endif

/* Procedures exported from demand.c */
void demand_conf __P((void));	/* config interface(s) for demand-dial */
void demand_block __P((void));	/* set all NPs to queue up packets */
void demand_unblock __P((void)); /* set all NPs to pass packets */
void demand_discard __P((void)); /* set all NPs to discard packets */
void demand_rexmit __P((int));	/* retransmit saved frames for an NP */
int  loop_chars __P((unsigned char *, int)); /* process chars from loopback */
int  loop_frame __P((unsigned char *, int)); /* should we bring link up? */

/* Procedures exported from multilink.c */
void mp_check_options __P((void)); /* Check multilink-related options */
int  mp_join_bundle __P((void));  /* join our link to an appropriate bundle */
char *epdisc_to_str __P((struct epdisc *)); /* string from endpoint discrim. */
int  str_to_epdisc __P((struct epdisc *, char *)); /* endpt disc. from str */

/* Procedures exported from sys-*.c */
void sys_init __P((void));	/* Do system-dependent initialization */
void sys_cleanup __P((void));	/* Restore system state before exiting */
int  sys_check_options __P((void)); /* Check options specified */
void sys_close __P((void));	/* Clean up in a child before execing */
int  ppp_available __P((void));	/* Test whether ppp kernel support exists */
int  get_pty __P((int *, int *, char *, int));	/* Get pty master/slave */
int  open_ppp_loopback __P((void)); /* Open loopback for demand-dialling */
int  tty_establish_ppp __P((int));  /* Turn serial port into a ppp interface */
void tty_disestablish_ppp __P((int)); /* Restore port to normal operation */
void generic_disestablish_ppp __P((int dev_fd)); /* Restore device setting */
int  generic_establish_ppp __P((int dev_fd)); /* Make a ppp interface */
void make_new_bundle __P((int, int, int, int)); /* Create new bundle */
int  bundle_attach __P((int));	/* Attach link to existing bundle */
void cfg_bundle __P((int, int, int, int)); /* Configure existing bundle */
void clean_check __P((void));	/* Check if line was 8-bit clean */
void set_up_tty __P((int, int)); /* Set up port's speed, parameters, etc. */
void restore_tty __P((int));	/* Restore port's original parameters */
void setdtr __P((int, int));	/* Raise or lower port's DTR line */
void output __P((int, u_char *, int)); /* Output a PPP packet */
void wait_input __P((struct timeval *));
				/* Wait for input, with timeout */
void add_fd __P((int));		/* Add fd to set to wait for */
void remove_fd __P((int));	/* Remove fd from set to wait for */
#ifdef __APPLE__
void sys_reinit();			/* reinit after pid has changed */
void sys_install_options(void);		/* install system specific options, before sys_init */
int sys_loadplugin(char *arg);
int sys_getconsoleuser(uid_t *uid);	/* get the current console user */
void sys_new_event(u_long m);
void sys_publish_status(u_long status, u_long devstatus);
void sys_publish_remoteaddress(char *addr);
int getabsolutetime(struct timeval *timenow);
bool is_ready_fd(int fd);	/* check if fd is ready (out of select) */
void set_up_tty_local __P((int, int)); /* Set up port's 'local' parameters only. */
void ppp_hold __P((int unit));	/* stop ppp traffic on this link */
void ppp_cont __P((int unit));	/* resume ppp traffic on this link */
void auth_hold(int unit);
void auth_cont(int unit);
int wait_input_fd(int fd, int delay);
#ifdef INET6
int ether_to_eui64(eui64_t *p_eui64);
#endif
void generic_send_config __P((int, u_int32_t, int, int));
				/* Configure i/f transmit parameters */
void generic_recv_config __P((int, u_int32_t, int, int));
				/* Configure i/f receive parameters */
#endif
int  read_packet __P((u_char *)); /* Read PPP packet */
int  get_loop_output __P((void)); /* Read pkts from loopback */
void tty_send_config __P((int, u_int32_t, int, int));
				/* Configure i/f transmit parameters */
void tty_set_xaccm __P((ext_accm));
				/* Set extended transmit ACCM */
void tty_recv_config __P((int, u_int32_t, int, int));
				/* Configure i/f receive parameters */
int  ccp_test __P((int, u_char *, int, int));
				/* Test support for compression scheme */
void ccp_flags_set __P((int, int, int));
				/* Set kernel CCP state */
int  ccp_fatal_error __P((int)); /* Test for fatal decomp error in kernel */
int  get_idle_time __P((int, struct ppp_idle *));
				/* Find out how long link has been idle */
int  get_ppp_stats __P((int, struct pppd_stats *));
				/* Return link statistics */
void netif_set_mtu __P((int, int)); /* Set PPP interface MTU */
int  netif_get_mtu __P((int));      /* Get PPP interface MTU */
int  sifvjcomp __P((int, int, int, int));
				/* Configure VJ TCP header compression */
int  sifup __P((int));		/* Configure i/f up for one protocol */
int  sifnpmode __P((int u, int proto, enum NPmode mode));
				/* Set mode for handling packets for proto */
int  sifdown __P((int));	/* Configure i/f down for one protocol */
int  sifaddr __P((int, u_int32_t, u_int32_t, u_int32_t));
				/* Configure IPv4 addresses for i/f */
int  cifaddr __P((int, u_int32_t, u_int32_t));
				/* Reset i/f IP addresses */
#ifdef INET6
int  sif6addr __P((int, eui64_t, eui64_t));
				/* Configure IPv6 addresses for i/f */
int  cif6addr __P((int, eui64_t, eui64_t));
				/* Remove an IPv6 address from i/f */
#endif
#ifdef __APPLE__
int  sifnpafmode __P((int u, int proto, enum NPAFmode mode));
				/* Set mode for filtering addresses for proto */
int sifdns(u_int32_t dns1, u_int32_t dns2);
#endif
int  sifdefaultroute __P((int, u_int32_t, u_int32_t));
				/* Create default route through i/f */
int  cifdefaultroute __P((int, u_int32_t, u_int32_t));
				/* Delete default route through i/f */
int  sifproxyarp __P((int, u_int32_t));
				/* Add proxy ARP entry for peer */
int  cifproxyarp __P((int, u_int32_t));
				/* Delete proxy ARP entry for peer */
u_int32_t GetMask __P((u_int32_t)); /* Get appropriate netmask for address */
int  lock __P((char *));	/* Create lock file for device */
int  relock __P((int));		/* Rewrite lock file with new pid */
void unlock __P((void));	/* Delete previously-created lock file */
void logwtmp __P((const char *, const char *, const char *));
				/* Write entry to wtmp file */
int  get_host_seed __P((void));	/* Get host-dependent random number seed */
int  have_route_to __P((u_int32_t)); /* Check if route to addr exists */
#ifdef PPP_FILTER
int  set_filters __P((struct bpf_program *pass, struct bpf_program *active));
				/* Set filter programs in kernel */
#endif
#ifdef IPX_CHANGE
int  sipxfaddr __P((int, unsigned long, unsigned char *));
int  cipxfaddr __P((int));
#endif
int  get_if_hwaddr __P((u_char *addr, char *name));
char *get_first_ethernet __P((void));

/* Procedures exported from options.c */
int setipaddr __P((char *, char **, int)); /* Set local/remote ip addresses */
int  parse_args __P((int argc, char **argv));
				/* Parse options from arguments given */
#ifdef __APPLE__
int options_from_fd __P((int));
#endif
int  options_from_file __P((char *filename, int must_exist, int check_prot,
			    int privileged));
				/* Parse options from an options file */
int  options_from_user __P((void)); /* Parse options from user's .ppprc */
int  options_for_tty __P((void)); /* Parse options from /etc/ppp/options.tty */
int  options_from_list __P((struct wordlist *, int privileged));
				/* Parse options from a wordlist */
int  getword __P((FILE *f, char *word, int *newlinep, char *filename));
				/* Read a word from a file */
void option_error __P((char *fmt, ...));
				/* Print an error message about an option */
int int_option __P((char *, int *));
				/* Simplified number_option for decimal ints */
void add_options __P((option_t *)); /* Add extra options */
void check_options __P((void));	/* check values after all options parsed */
int  override_value __P((const char *, int, const char *));
				/* override value if permitted by priority */
void print_options __P((void (*) __P((void *, char *, ...)), void *));
				/* print out values of all options */

int parse_dotted_ip __P((char *, u_int32_t *));

/*
 * Hooks to enable plugins to change various things.
 */
extern int (*new_phase_hook) __P((int));
extern int (*idle_time_hook) __P((struct ppp_idle *));
extern int (*holdoff_hook) __P((void));
extern int (*pap_check_hook) __P((void));
extern int (*pap_auth_hook) __P((char *user, char *passwd, char **msgp,
				 struct wordlist **paddrs,
				 struct wordlist **popts));
extern void (*pap_logout_hook) __P((void));
extern int (*pap_passwd_hook) __P((char *user, char *passwd));
extern int (*allowed_address_hook) __P((u_int32_t addr));
extern void (*ip_up_hook) __P((void));
extern void (*ip_down_hook) __P((void));
extern void (*ip_choose_hook) __P((u_int32_t *));

extern int (*chap_check_hook) __P((void));
extern int (*chap_passwd_hook) __P((char *user, char *passwd));

/* Let a plugin snoop sent and received packets.  Useful for L2TP */
extern void (*snoop_recv_hook) __P((unsigned char *p, int len));
extern void (*snoop_send_hook) __P((unsigned char *p, int len));

#ifdef __APPLE__
/* Hook for access control list to verify if user should have access */
extern int (*acl_hook) __P((char *user, int len));
#endif


/*
 * Inline versions of get/put char/short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be u_char *.
 */
#define GETCHAR(c, cp) { \
	(c) = *(cp)++; \
}
#define PUTCHAR(c, cp) { \
	*(cp)++ = (u_char) (c); \
}


#define GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}
#define PUTSHORT(s, cp) { \
	*(cp)++ = (u_char) ((s) >> 8); \
	*(cp)++ = (u_char) (s); \
}

#define GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}
#define PUTLONG(l, cp) { \
	*(cp)++ = (u_char) ((l) >> 24); \
	*(cp)++ = (u_char) ((l) >> 16); \
	*(cp)++ = (u_char) ((l) >> 8); \
	*(cp)++ = (u_char) (l); \
}

#define INCPTR(n, cp)	((cp) += (n))
#define DECPTR(n, cp)	((cp) -= (n))

/*
 * System dependent definitions for user-level 4.3BSD UNIX implementation.
 */

#define TIMEOUT(r, f, t)	timeout((r), (f), (t), 0)
#define UNTIMEOUT(r, f)		untimeout((r), (f))

#define BCOPY(s, d, l)		memcpy(d, s, l)
#define BZERO(s, n)		memset(s, 0, n)

#define PRINTMSG(m, l)		{ info("Remote message: %0.*v", l, m); }

/*
 * MAKEHEADER - Add Header fields to a packet.
 */
#define MAKEHEADER(p, t) { \
    PUTCHAR(PPP_ALLSTATIONS, p); \
    PUTCHAR(PPP_UI, p); \
    PUTSHORT(t, p); }

/*
 * Exit status values.
 */
#define EXIT_OK			0
#define EXIT_FATAL_ERROR	1
#define EXIT_OPTION_ERROR	2
#define EXIT_NOT_ROOT		3
#define EXIT_NO_KERNEL_SUPPORT	4
#define EXIT_USER_REQUEST	5
#define EXIT_LOCK_FAILED	6
#define EXIT_OPEN_FAILED	7
#define EXIT_CONNECT_FAILED	8
#define EXIT_PTYCMD_FAILED	9
#define EXIT_NEGOTIATION_FAILED	10
#define EXIT_PEER_AUTH_FAILED	11
#define EXIT_IDLE_TIMEOUT	12
#define EXIT_CONNECT_TIME	13
#define EXIT_CALLBACK		14
#define EXIT_PEER_DEAD		15
#define EXIT_HANGUP		16
#define EXIT_LOOPBACK		17
#define EXIT_INIT_FAILED	18
#define EXIT_AUTH_TOPEER_FAILED	19
#ifdef __APPLE__
#define EXIT_TERMINAL_FAILED	20
#define EXIT_DEVICE_ERROR	21
#endif
#ifdef MAXOCTETS
#ifdef __APPLE__
#define EXIT_TRAFFIC_LIMIT	22
#else
#define EXIT_TRAFFIC_LIMIT	20
#endif
#endif
#ifdef __APPLE__
#define EXIT_PEER_NOT_AUTHORIZED	23
#endif

/*
 * Debug macros.  Slightly useful for finding bugs in pppd, not particularly
 * useful for finding out why your connection isn't being established.
 */
#ifdef DEBUGALL
#define DEBUGMAIN	1
#define DEBUGFSM	1
#define DEBUGLCP	1
#define DEBUGIPCP	1
#define DEBUGACSCP	1
#define DEBUGUPAP	1
#define DEBUGIPV6CP	1
#define DEBUGCHAP	1
#define DEBUGEAP	1
#endif

#ifndef LOG_PPP			/* we use LOG_LOCAL2 for syslog by default */
#if defined(DEBUGMAIN) || defined(DEBUGFSM) || defined(DEBUGSYS) \
  || defined(DEBUGLCP) || defined(DEBUGIPCP) || defined(DEBUGUPAP) \
  || defined(DEBUGCHAP) || defined(DEBUG) || defined(DEBUGIPV6CP) \
  || defined(DEBUGEAP) || defined(DEBUGACSCP)
#define LOG_PPP LOG_LOCAL2
#else
#define LOG_PPP LOG_DAEMON
#endif
#endif /* LOG_PPP */

#ifdef DEBUGMAIN
#define MAINDEBUG(x)	if (debug) dbglog x
#else
#define MAINDEBUG(x)
#endif

#ifdef DEBUGSYS
#define SYSDEBUG(x)	if (debug) dbglog x
#else
#define SYSDEBUG(x)
#endif

#ifdef DEBUGFSM
#define FSMDEBUG(x)	if (debug) dbglog x
#else
#define FSMDEBUG(x)
#endif

#ifdef DEBUGLCP
#define LCPDEBUG(x)	if (debug) dbglog x
#else
#define LCPDEBUG(x)
#endif

#ifdef DEBUGIPCP
#define IPCPDEBUG(x)	if (debug) dbglog x
#else
#define IPCPDEBUG(x)
#endif

#ifdef DEBUGACSCP
#define ACSCPDEBUG(x)	if (debug) dbglog x
#else
#define ACSCPDEBUG(x)
#endif

#ifdef DEBUGIPV6CP
#define IPV6CPDEBUG(x)  if (debug) dbglog x
#else
#define IPV6CPDEBUG(x)
#endif

#ifdef DEBUGUPAP
#define UPAPDEBUG(x)	if (debug) dbglog x
#else
#define UPAPDEBUG(x)
#endif

#ifdef DEBUGCHAP
#define CHAPDEBUG(x)	if (debug) dbglog x
#else
#define CHAPDEBUG(x)
#endif

#ifdef DEBUGEAP
#define EAPDEBUG(x)	if (debug) dbglog x
#else
#define EAPDEBUG(x)
#endif

#ifdef DEBUGIPXCP
#define IPXCPDEBUG(x)	if (debug) dbglog x
#else
#define IPXCPDEBUG(x)
#endif

#ifndef SIGTYPE
#if defined(sun) || defined(SYSV) || defined(POSIX_SOURCE)
#define SIGTYPE void
#else
#define SIGTYPE int
#endif /* defined(sun) || defined(SYSV) || defined(POSIX_SOURCE) */
#endif /* SIGTYPE */

#ifndef MIN
#define MIN(a, b)	((a) < (b)? (a): (b))
#endif
#ifndef MAX
#define MAX(a, b)	((a) > (b)? (a): (b))
#endif

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#endif /* __PPP_H__ */