/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#ifndef DEFS_HEADER_DEFINED
#define	DEFS_HEADER_DEFINED
#include "std_includes.h"
#include "llist.h"
#include "io.h"
#include "shttpd.h"
#include "md5.h"
#include "my_json.h"
#include "disk_manage.h"

#define EXPIRE_PER_SEC (5)
#define	DFLT_IO_SIZ	16384		/* Default max request size	*/
#define	DELIM_CHARS	" ,"		/* Separators for lists		*/
#define	URI_MAX		32768		/* Maximum URI size		*/
#define LOCAL_ADDR "127.0.0.1"

#define	CONFIG		"/usr/mips/conf/dm_router.conf"	/* Configuration file		*/				
#define AIRDISK_DM_ROUTER_VERSION "airdisk_dm_router_version"
#define AIRDISK_PRO_DB_VERSION "airdisk_pro_db_version"
#define AIRDISK_PRO_FW_VERSION "airdisk_pro_fw_version"
#define PRODUCT_MODEL "product_model"
#define FILE_LISTENING_PORT "file_listen_ports"
#define ROUTER_LISTENING_PORT "router_listen_ports"
#define INIT_LISTENING_PORT "init_listen_ports"

#define DATABASE_NAME "database_name"
#define DISK_UUID_NAME "disk_uuid_name"

#define POWER_ACCESS "power_access"
#define WIFI_ACCESS "wifi_access"
#define REMOTEAP_ACCESS "remoteap_access"
#define FILE_TYPE_ACCESS "file_type_access"
#define BACKUP_ACCESS "backup_access"
#define COPY_TO_ACCESS "copy_to_access"
#define FILE_LIST_ACCESS "file_list_access"
/*
 * Darwin prior to 7.0 and Win32 do not have socklen_t
 */
#ifdef NO_SOCKLEN_T
typedef int socklen_t;
#endif /* NO_SOCKLEN_T */

/*
 * For parsing. This guy represents a substring.
 */
struct vec {
	const char	*ptr;
	int		len;
};
enum {E_FATAL = 1, E_LOG = 2};		/* Flags for elog() function	*/
typedef unsigned long big_int_t;	/* Type for Content-Length	*/
	
/*
 * Unified socket address
 */
struct usa {
	socklen_t len;
	union {
		struct sockaddr	sa;
		struct sockaddr_in sin;
	} u;
};

/*
 * This thing is aimed to hold values of any type.
 * Used to store parsed headers' values.
 */
union variant {
	char		*v_str;
	int		v_int;
	big_int_t	v_big_int;
	time_t		v_time;
	void		(*v_func)(void);
	void		*v_void;
	struct vec	v_vec;
};


struct mime_type {
	const char	*ext;
	int		ext_len;
	const char	*mime;
	int file_type;
};

/*
 * This guy holds parsed HTTP headers
 */
struct headers {
	union variant	cl;		/* Content-Length:		*/
	union variant	ct;		/* Content-Type:		*/
	union variant	connection;	/* Connection:			*/
};

/* Must go after union variant definition */
//#include "ssl.h"

/*
 * The communication channel
 */
union channel {
	int		fd;		/* Regular static file		*/
	int		sock;		/* Connected socket		*/		/* SSL-ed socket		*/
	struct {
		DIR	*dirp;
		char	*path;
	} dir;				/* Opened directory		*/
	struct {
		void		*state;	/* For keeping state		*/
		union variant	func;	/* User callback function	*/
		void		*data;	/* User defined parameters	*/
	} emb;				/* Embedded, user callback	*/
};

struct stream;

/*
 * IO class descriptor (file, directory, socket, SSL, CGI, etc)
 * These classes are defined in io_*.c files.
 */
struct io_class {
	const char *name;
	int (*read)(struct stream *, void *buf, size_t len);
	int (*write)(struct stream *, const void *buf, size_t len);
	void (*close)(struct stream *);
};

/*
 * Data exchange stream. It is backed by some communication channel:
 * opened file, socket, etc. The 'read' and 'write' methods are
 * determined by a communication channel.
 */
struct stream {
	struct conn		*conn;
	union channel		chan;		/* Descriptor		*/
	struct io		io;		/* IO buffer		*/
	const struct io_class	*io_class;	/* IO class		*/
	int			nread_last;	/* Bytes last read	*/
	int			headers_len;
	big_int_t		content_len;
	unsigned int		flags;
#define	FLAG_HEADERS_PARSED	1
#define	FLAG_SSL_ACCEPTED	2
#define	FLAG_R			4		/* Can read in general	*/
#define	FLAG_W			8		/* Can write in general	*/
#define	FLAG_CLOSED		16
#define	FLAG_DONT_CLOSE		32
#define	FLAG_ALWAYS_READY	64		/* File, dir, user_func	*/
};

typedef struct conn {
	struct llhead	link;		/* Connections chain		*/
	struct shttpd_ctx *ctx;		/* Context this conn belongs to */
	struct usa	sa;		/* Remote socket address	*/

	int		loc_port;	/* Local port			*/
	int		status;		/* Reply status code		*/
	char		*uri;		/* Decoded URI			*/
	char		*request;	/* Request line			*/
	char		*query;		/* QUERY_STRING part of the URI	*/

	struct headers	ch;		/* Parsed client headers	*/
	struct stream	loc;		/* Local stream			*/
	struct stream	rem;		/* Remote stream		*/
	unsigned long offset;
	unsigned long length;
	int cmd;
	int seq;
    unsigned copy_seq;
	unsigned del_seq;
	char session[32];
	int error;
	unsigned long fileSize;
	unsigned modifyTime;
	struct record_dnode *dn;
	char* cfg_path;
	char* tmp_path;
	//base interface
	char* src_path;
	char* des_path;
	char* disk_root;
	JObj *r_json; 
	struct timeval tstart;
	int pageNum;
	int fileNum;
	int fileType;
	unsigned totalCount;
	unsigned long totalSize;
	int totalPage;
	char uuid[32];
	char ver[32];
	char username[64];
	char password[64];
	uint8_t deviceTpye;
	char *deviceUuid;
	char *deviceName;

	char client_ip[32];

	int client_port;
	uint8_t statusFlag;

	unsigned cur_time;
	char *file_uuid;
	char *disk_name;
	char *dir_name;
	char *disk_uuid;
	file_uuid_list_t *flist;
	unsigned nfiles;
	time_t record_time;
};

#define THREAD_COUNT 1
#define DOCUMENT_ROOT "/tmp/mnt"
//#define BACKUP_FLODER "backup floder"
#define TMP_PATH_NAME ".dmt"
#define CFG_PATH_NAME ".dmt.cfg"

#define RECV_TIMEOUT        3000
#define HEART_BEAT_PORT 27212


/*
 * SHTTPD context
 */
struct shttpd_ctx {
	int		nactive;	/* # of connections now		*/
	uint64_t	in, out;	/* IN/OUT traffic counters	*/
	struct llhead	connections;	/* List of connections		*/
	char	*document_root;		/* Document root		*/
	char	*ports;			/* Listening ports		*/
	int	io_buf_size;		/* IO buffer size		*/
	pthread_mutex_t mutex;
};

/*
 * In SHTTPD, list of values are represented as comma or space separated
 * string. For example, list of CGI extensions can be represented as
 * ".cgi,.php,.pl", or ".cgi .php .pl". The macro that follows allows to
 * loop through the individual values in that list.
 * A "const char *" pointer and size_t variable must be passed to the macro.
 * Spaces or commas can be used as delimiters (macro DELIM_CHARS)
 */
#define	FOR_EACH_WORD_IN_LIST(s,len)	\
	for (; s != NULL && (len = strcspn(s, DELIM_CHARS)) != 0; s += len + 1)
extern int db_get_mime_type(const char *uri, int len);
extern void	stop_stream(struct stream *stream);
extern void	open_listening_ports(struct shttpd_ctx *ctx);

extern struct shttpd_ctx *init_from_argc_argv();
extern int	my_strncasecmp(register const char *,
		register const char *, size_t);
extern int	my_snprintf(char *buf, size_t buflen, const char *fmt, ...);

extern void	set_close_on_exec(int fd);
extern int	set_non_blocking_mode(int fd);
extern int	my_stat(const char *, struct stat *stp);
extern int	my_open(const char *, int flags, int mode);

extern const struct io_class	io_file;
extern const struct io_class	io_socket;

extern int file_json_to_string(struct conn *c,JObj* response_json);


#define FILENAME_MAX 4607

/*
 * io_*.c
 */
extern const struct io_class	io_file;
extern const struct io_class	io_socket;
extern const struct io_class	io_dir;
extern const struct io_class	io_type;


extern int	put_dir(const char *path);
extern void	get_dir(struct conn *c);
extern void	get_type(struct conn *c);

extern void	get_file(struct conn *c, struct stat *stp);


#endif /* DEFS_HEADER_DEFINED */
