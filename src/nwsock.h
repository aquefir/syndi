/****************************************************************************\
 *                                  Syndi™                                  *
 *                                                                          *
 *                         Copyright © 2021 Aquefir                         *
 *                 Released under Artisan Software Licence.                 *
\****************************************************************************/

#ifndef INC__SYNDI_NWSOCK_H
#define INC__SYNDI_NWSOCK_H

#include <glib.h>
#include <unistd.h>

#include "common.h"

/**
 * NETWORK SOCKET MODULE
 * Structures and functions for state management of IRC network connections
 *
 * These use TCP/IPv4 socket connections and the ASCII-based IRCv3 protocol.
 * The implementation uses POSIX-based sockets only.
 * Networks are connected to with one socket each, and there is a lock-like
 * object used in message-passing functions to transiently pass them around.
 *
 * This module is only responsible for:
 *  1. Initialisation and finalisation of socket structs et cetera
 *  2. Initialisation and finalisation of concurrency structs et cetera
 *  3. The sending of messages from one network-socket to the rest
 *  4. The receiving of messages from one network-socket to each
 */

struct syndi_nwsock_synchro;
struct syndi_nwsock_sock;
struct syndi_nwsock_callback_data
{
	char msg[SYNDI_MAX_MSGLEN + 1];
	char from[SYNDI_MAX_NICKLEN + 1];
	uint64_t timestamp;
};

struct syndi_nwsock_sock_initopts
{
	unsigned is_hostname : 1;
	unsigned use_sasl : 1;
	unsigned use_tls : 1;
	unsigned allow_selfsign_tls : 1;
	union
	{
		char hostname[_POSIX_HOST_NAME_MAX + 1];
		uint32_t ipv4_addr;
	};
	uint16_t port;
	char nick[SYNDI_MAX_NICKLEN + 1];
	char sasl_pass[SYNDI_MAX_SASL_PASSWDLEN + 1];
};

typedef int ( *syndi_nwsock_callback )( struct syndi_nwsock_callback_data );

gboolean syndi_nwsock_sock_init( struct syndi_nwsock_sock_initopts,
	struct syndi_nwsock_sock **,
	GError ** );
void syndi_nwsock_sock_fini( struct syndi_nwsock_sock * );

gboolean syndi_nwsock_synchro_init( struct syndi_nwsock_sock **,
	unsigned,
	struct syndi_nwsock_synchro **,
	GError ** );
void syndi_nwsock_synchro_fini( struct syndi_nwsock_synchro * );
gboolean syndi_nwsock_synchro_add(
	struct syndi_nwsock_synchro *, struct syndi_nwsock_sock *, GError ** );
gboolean syndi_nwsock_synchro_del(
	struct syndi_nwsock_synchro *, struct syndi_nwsock_sock *, GError ** );

#endif /* INC__SYNDI_NWSOCK_H */
