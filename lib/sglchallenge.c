/*
 *			s g l c h a l l e n g e . c
 * $Revision: 167 $
 * $Author: eckertb $
 *
 * RMS Gateway
 *
 * Copyright (c) 2004-2014 Hans-J. Barthen - DL5DI
 * Copyright (c) 2008-2014 Brian R. Eckert - W3SG
 *
 * This module is derived from visual basic module provided by
 * the winlink team as a reference implementation.
 *
 * Questions or problems regarding this program can be emailed
 * to linux-rmsgw@w3sg.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef lint
static char svnid[] = "$Id: sglchallenge.c 167 2014-09-30 10:27:26Z eckertb $";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <endian.h>
#include <syslog.h>

#include "md5.h"

static char respStr[11]; /* buffer for response string return value */

/*
 * CallengeCode union facilitates handling endian-ness properly
 */
typedef union
{
     uint32_t	intVal;
     md5_byte_t	byteArr[4];
} ChallengeCode;

/*
 * winlink salt array (as defined by winlink team)
 */
static unsigned char salt[] = {
      77, 197, 101, 206, 190, 249,
      93, 200,  51, 243,  93, 237,
      71,  94, 239, 138,  68, 108,
      70, 185, 225, 137, 217,  16,
      51, 122, 193,  48, 194, 195,
     198, 175, 172, 169,  70,  84,
      61,  62, 104, 186, 114,  52,
      61, 168,  66, 129, 192, 208,
     187, 249, 232, 193,  41, 113,
      41,  45, 240,  16,  29, 228,
     208, 228,  61,  20
};

/***
 *  ChallengedPassword()
 *
 *  Calculate the challenge password response as follows:
 *  - Concatenate the challenge phrase, password, and supplied secret value (i.e. the salt)
 *  - Generate an MD5 hash of the result
 * - Convert the first 4 bytes of the hash to an integer (big endian) and return it
 */
static unsigned int ChallengedPassword(char *challengePhrase, char *password, unsigned char *slt)
{
     int i;
     int m = strlen(challengePhrase) + strlen(password);
     md5_byte_t tmpCP[m + 64 + 1]; // 64 is the length of the salt array, +1 for terminator
     md5_state_t state;
     md5_byte_t digest[16]; /* MD5 digest is always 16 bytes */
     ChallengeCode cc;

     /*
      * probably not important, but initialize challenge code to 0
      * the use of a four byte hex value is only to make clear that we are dealing with
      * a four byte value in the union--there is nothing special about this initialization value...
      * it's just 0
      */
     cc.intVal = 0x00000000;

     strcpy(tmpCP, challengePhrase);
     strcat(tmpCP, password);
     strncat(tmpCP, slt, 64);

     //printf("challenge string = '%s'", tmpCP);

     /*
      * compute hash
      */
     md5_init(&state);
     md5_append(&state, (const md5_byte_t *)tmpCP, strlen(tmpCP));
     md5_finish(&state, digest); // get the md5 hash (left in digest)

     /*
      * wipe buffer now that we have the digest
      */
     memset(tmpCP, '\0', sizeof(tmpCP));

     /*
      * Create a positive integer return value from the hash bytes (using first four bytes)
      * Winlink assumes (I'm assuming) machines are big endian.
      * I believe the little endian code matches the intent of the winlink implemenation if on
      * a little endian architecture
      */
#ifdef __BIG_ENDIAN
     cc.byteArr[3] = digest[3] & 0x3f; // strip sign bit from what is to be the high byte
     for (i = 2; i >=0; i--) {
	  cc.byteArr[i] = digest[i];
     }
#else // assuming little endian here
     cc.byteArr[0] = digest[3] & 0x3f; // strip sign bit from high byte
     for (i = 2; i >=0; i--) {
	  cc.byteArr[3 - i] = digest[i];
     }
#endif
     return(cc.intVal);
}


/***
 *  sgl_challenge_response()
 *
 *  given a CMS challenge code and the gateway password,
 *  compute the proper response code
 *
 *  returns: pointer to response code string
 */
char *sgl_challenge_response(char *challenge, char *pass)
{
     unsigned int retVal = 0;
     int i;

     syslog(LOG_DEBUG, "sgl_challenge_response(challenge = %s, pass = %s)",
	    challenge, pass);

     retVal = ChallengedPassword(challenge, pass, salt);
     syslog(LOG_DEBUG, "ChallengedPassword() returned: %u", retVal);

     /*
      * convert response (10 digits) to a string
      */
     sprintf(respStr, "%010d", retVal);

     /*
      * return only the last eight characters
      * of the computed response string
      */
     syslog(LOG_DEBUG, "sgl_challenge_response() returning with: %s", respStr + 2);
     return(respStr + 2);
}
