/****************************************************************************\
 *                                  Syndi™                                  *
 *                                                                          *
 *                         Copyright © 2021 Aquefir                         *
 *                 Released under Artisan Software Licence.                 *
\****************************************************************************/

#include "ircmsg.h"

#include <stdio.h>
#include <stdlib.h>
#include <uni/err.h>
#include <uni/memory.h>
#include <uni/str.h>

#define FMT "PRIVMSG %s :%s\r\n"
#define FMT_SZ sizeof( "PRIVMSG  :\r\n" )

void syndi_privmsg_make( char * chan, char * text, char * out, ptri * out_sz )
{
	if( chan == NULL || text == NULL )
	{
		uni_die( );
	}

	{
		const ptri chan_sz = uni_strlen( chan ),
			   text_sz = uni_strlen( text );
		const ptri buf_sz  = chan_sz + text_sz + FMT_SZ + 1;
		char * buf         = alloca( buf_sz );

		uni_memset( buf, 0, buf_sz );

		/* no C99, so the bounds are ensured in the calculations above
		 */
		sprintf( FMT, chan, text );

		if( out != NULL )
		{
			uni_memcpy( out, (const char *)buf, buf_sz );
		}

		if( out_sz != NULL )
		{
			*out_sz = buf_sz;
		}
	}
}
