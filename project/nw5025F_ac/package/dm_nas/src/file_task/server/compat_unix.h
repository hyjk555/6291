/*
 * Copyright (c) 2004-2007 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#include <pwd.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#define	DIRSEP				'/'
#define	IS_DIRSEP_CHAR(c)		((c) == '/')
#define	O_BINARY			0
#define	closesocket(a)			close(a)
#define	ERRNO				errno
#define	NO_GUI

#define	InitializeCriticalSection(x)	pthread_mutex_init(x,NULL)/* FIXME UNIX version is not MT safe */
#define	EnterCriticalSection(x)			pthread_mutex_lock(x)
#define	LeaveCriticalSection(x)			pthread_mutex_unlock(x)
#define DestroyCriticalSection(x)		pthread_mutex_destroy(x)
