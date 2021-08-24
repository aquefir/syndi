/****************************************************************************\
 *                                  Syndi™                                  *
 *                                                                          *
 *                         Copyright © 2021 Aquefir                         *
 *                 Released under Artisan Software Licence.                 *
\****************************************************************************/

#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "ircmsg.h"

enum
{
	BUF_SZ    = 512,
	LOG_OUT   = 0,
	LOG_IN    = 1,
	LOG_ERR   = 2,
	NET_COUNT = 11
};

int conn;
char sbuf[512];

static int logging;

static const char * const nicks[NET_COUNT] = { "Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia",
	"Zenobia" };

static const char * const chans[NET_COUNT] = { "#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk",
	"#fhtk" };

static const char * const hosts[NET_COUNT] = { "irc.juggler.jp",
	"irc.data.lt",
	"irc.deltaanime.net",
	"irc.choopa.net",
	"irc.esper.net",
	"irc.hackint.org",
	"irc.librairc.net",
	"irc.oftc.net",
	"irc.rizon.net",
	"tardis.swiftirc.net",
	"hades.de.eu.synirc.net" };

static const char * const ports[NET_COUNT] = { "6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667",
	"6667" };

void syndi_log( unsigned flags, const char * fmt, ... )
{
	char buf[BUF_SZ];
	va_list ap;

	if( !logging )
	{
		return;
	}

	va_start( ap, fmt );
	vsnprintf( buf, BUF_SZ, fmt, ap );
	va_end( ap );

	fprintf( flags == LOG_ERR ? stderr : stdout,
		flags == LOG_IN ? ">> %s" : "<< %s",
		buf );
}

void syndi_write( int c, const char * fmt, ... )
{
	char buf[BUF_SZ];
	va_list ap;

	va_start( ap, fmt );
	vsnprintf( buf, BUF_SZ, fmt, ap );
	va_end( ap );

	syndi_log( LOG_OUT, (const char *)buf );
	write( c, (const char *)buf, strlen( buf ) );
}

int main( int ac, char * av[] )
{
	char buf[BUF_SZ + 1];
	int conns[NET_COUNT];
	unsigned i;
	const struct addrinfo hints = {
		0, AF_INET, SOCK_STREAM, 0, 0, NULL, NULL, NULL };

	memset( conns, 0, sizeof( int ) * NET_COUNT );

	for( i = 0; i < NET_COUNT; ++i )
	{
		struct addrinfo * res;

		getaddrinfo( hosts[i], ports[i], &hints, &res );

		conns[i] = socket(
			res->ai_family, res->ai_socktype, res->ai_protocol );
		connect( conn, res->ai_addr, res->ai_addrlen );

		syndi_write(
			conns[i], "USER %s 0 0 :%s\r\n", nicks[i], nicks[i] );
		syndi_write( conns[i], "NICK %s\r\n", nicks[i] );
	}

	for( ;; )
	{
		/* this variable tracks whether any server connections are
		 * still going */
		unsigned waiting = 0;

		for( i = 0; i < NET_COUNT; ++i )
		{
			ssize_t sl;

			if( conns[i] == 0 )
			{
				continue;
			}

			memset( buf, 0, BUF_SZ + 1 );
			sl = read( conns[i], buf, BUF_SZ );

			if( sl == 0 )
			{
				conns[i] = 0;

				continue;
			}

			syndi_log( LOG_IN, "%s\n", buf );
			waiting = 1;

			if( !strncmp( "PING", buf, 4 ) )
			{
				buf[1] = 'O';
				syndi_write( conns[i], "%s", buf );
			}
		}

		if( !waiting )
		{
			break;
		}
	}

	syndi_log( LOG_OUT, "All connections closed. Exiting...\n" );

	return 0;
}

#if 0
void raw( char * fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	vsnprintf( sbuf, 512, fmt, ap );
	va_end( ap );
	printf( "<< %s", sbuf );
	write( conn, sbuf, strlen( sbuf ) );
}

int main( int ac, char ** av )
{
	char * nick    = "Zenobia";
	char * channel = "#fhtk";
	char * host    = "catastrophe.esper.net";
	char * port    = "6667";

	char *user, *command, *where, *message, *sep, *target;
	int i, j, l, sl, o = -1, start, wordcount;
	char buf[513];
	struct addrinfo hints, *res;

	memset( &hints, 0, sizeof hints );
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo( host, port, &hints, &res );
	conn = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
	connect( conn, res->ai_addr, res->ai_addrlen );

	raw( "USER %s 0 0 :%s\r\n", nick, nick );
	raw( "NICK %s\r\n", nick );

	while( ( sl = read( conn, sbuf, 512 ) ) )
	{
		for( i = 0; i < sl; i++ )
		{
			o++;
			buf[o] = sbuf[i];
			if( ( i > 0 && sbuf[i] == '\n' &&
				    sbuf[i - 1] == '\r' ) ||
				o == 512 )
			{
				buf[o + 1] = '\0';
				l          = o;
				o          = -1;

				printf( ">> %s", buf );

				if( !strncmp( buf, "PING", 4 ) )
				{
					buf[1] = 'O';
					raw( buf );
				}
				else if( buf[0] == ':' )
				{
					wordcount = 0;
					user = command = where = message =
						NULL;
					for( j = 1; j < l; j++ )
					{
						if( buf[j] == ' ' )
						{
							buf[j] = '\0';
							wordcount++;
							switch( wordcount )
							{
							case 1:
								user = buf + 1;
								break;
							case 2:
								command = buf +
									start;
								break;
							case 3:
								where = buf +
									start;
								break;
							}
							if( j == l - 1 )
								continue;
							start = j + 1;
						}
						else if( buf[j] == ':' &&
							wordcount == 3 )
						{
							if( j < l - 1 )
								message = buf +
									j + 1;
							break;
						}
					}

					if( wordcount < 2 )
						continue;

					if( !strncmp( command, "001", 3 ) &&
						channel != NULL )
					{
						raw( "JOIN %s\r\n", channel );
					}
					else if( !strncmp( command,
							 "PRIVMSG",
							 7 ) ||
						!strncmp( command,
							"NOTICE",
							6 ) )
					{
						if( where == NULL ||
							message == NULL )
							continue;
						if( ( sep = strchr( user,
							      '!' ) ) != NULL )
							user[sep - user] =
								'\0';
						if( where[0] == '#' ||
							where[0] == '&' ||
							where[0] == '+' ||
							where[0] == '!' )
							target = where;
						else
							target = user;
						printf( "[from: %s] [reply-with: %s] [where: %s] [reply-to: %s] %s",
							user,
							command,
							where,
							target,
							message );
						raw( "%s %s :%s",
							command,
							target,
							message );
					}
				}
			}
		}
	}

	return 0;
}
#endif /* 0 */
