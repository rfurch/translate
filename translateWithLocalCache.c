/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include <ctype.h>
#include <curl/curl.h>
#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "translate.h"

// this code takes advantage of FREE google translationservices
// presented via URI:
// https://translate.googleapis.com/translate_a/single?client=gtx&sl=es&tl=en&dt=t&q=esot%es%una%prueba";
// altough the basic routines are the same that we can use in google business platform (we need to pay for 
// the service) in which a KEY is assigned. 
// the parsing procedures are similar (URI encode and JSON coding) 

#define BASIC_GOOGLE_TRANSLATE_URI "https://translate.googleapis.com/translate_a/single?client=gtx&"

// simple structure to hold queries and responses
typedef struct translationUserData
	{
		char	*URIString;	   // full URI string foor Google API
		char	*original;
		char 	*translated;
		char 	*to;
		char 	*from;		
	}translationUserData;

char 	*tt=NULL;
int 	_verbose=0;

//---------------------------------------------------------------

// this is CURL write handler as describred in:
//
// https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
//
// and we need to pay attention to:
//
// https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
//
// receives CURL data, received after a  call to curl_easy_perform
size_t  writeHandler(void *ptr, size_t size, size_t nmemb, void *stream){
	
	struct json_object 		*jobj=NULL;
	struct json_object 		*element1=NULL, *element1_1=NULL, *element1_1_1=NULL;

	char 					*str = calloc( (size * nmemb) + 10, sizeof(char));
	translationUserData		*tud = (translationUserData *)stream;
	
	memcpy(str, (char *) ptr, size * nmemb);
	str[size * nmemb] = 0;

	// get into ARRAY returned by google (array into array into array)   
	// we need  ((a[0])[0])[0]
	// example: [[["this is a very simple test","esto es una prueba muy simple",null,null,3]],null,"es"]	
    jobj = json_tokener_parse(str);
	//printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
	element1 = json_object_array_get_idx(jobj, 0);

	// get sub elements	
	element1_1 = json_object_array_get_idx(element1, 0);
	element1_1_1 = json_object_array_get_idx(element1_1, 0);
	tud->translated = strdup( json_object_to_json_string_ext(element1_1_1, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) );

    free (str);
	json_object_put(jobj);
    
    return( size * nmemb );
}

//---------------------------------------------------------------

int buildURIString(CURL *curl, translationUserData *tud)
{
char *escapedStr = NULL;

// this replaces special characters  encoding into URI ( eg:  ' '  is %20 )
escapedStr = curl_easy_escape(curl, tud->original, strlen(tud->original));

if ( (tud->URIString = (char *) calloc(sizeof(char), strlen(BASIC_GOOGLE_TRANSLATE_URI) + strlen(escapedStr) + 30)) == NULL) {
	printf("\n\n CALLOC ERROR!  \n\n");
	return(-1);			
	}

//https://translate.googleapis.com/translate_a/single?client=gtx&sl=es&tl=en&dt=t&q=esot%es%una%prueba";
//https://translate.googleapis.com/translate_a/single?client=gtx&"

strcpy(tud->URIString, BASIC_GOOGLE_TRANSLATE_URI);
strcat(tud->URIString, "sl=");			// source language
strcat(tud->URIString, tud->from);		// source language
strcat(tud->URIString, "&tl=");			// 'to' language
strcat(tud->URIString, tud->to);		// source language
strcat(tud->URIString, "&dt=t&q="); 	// aditional items
strcat(tud->URIString, escapedStr);		// original text in URI format

if (_verbose > 1)  {
	printf("\n Encoded TEXT: |%s| ", escapedStr);
	printf("\n URI String: |%s| \n\n", tud->URIString);
	}

free(escapedStr); 

return(0);
}

//---------------------------------------------------------------

// main translation function,  using CURL and JSON-C libraries
// receives translationUserData and returns 0 if OK 

int googleFreeTranslate(translationUserData *tud)
{
	CURL 		*curl=NULL;
	CURLcode 	res=0;
	int 		retCode=0;

	if (_verbose > 1)
		printf("\n Text to Translate from '%s' to '%s': |%s| \n", tud->from, tud->to, tud->original);


	// CURL Basic init
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	// compose complete URI string with text to translate, from and to languages
	if ( (retCode = buildURIString(curl, tud)) < 0) {
		printf("\n ERROR (%i) Building URI String !!!", retCode);
		return(retCode);
		}
	
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, tud->URIString);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeHandler);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)tud);

	#ifdef SKIP_PEER_VERIFICATION
		/*
		 * If you want to connect to a site who isn't using a certificate that is
		 * signed by one of the certs in the CA bundle you have, you can skip the
		 * verification of the server's certificate. This makes the connection
		 * A LOT LESS SECURE.
		 *
		 * If you have a CA cert for the server stored someplace else than in the
		 * default bundle, then the CURLOPT_CAPATH option might come handy for
		 * you.
		 */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	#endif

	#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		 * If the site you're connecting to uses a different host name that what
		 * they have mentioned in their server certificate's commonName (or
		 * subjectAltName) fields, libcurl will refuse to connect. You can skip
		 * this check, but this will make the connection less secure.
		 */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	#endif

		// Perform the request, res will get the return code 
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		// always cleanup 
		curl_easy_cleanup(curl);
	  }

	  curl_global_cleanup();	
	
return(0);	
}

//---------------------------------------------------------------
// attempt to translate sentence from local database
// Returns 0 (OK)   or -1 in case of error

int localTranslate(translationUserData *tud)
{
MYSQL	*conn=NULL;
int 	err=0;

if ( (conn = localDBInit("192.168.254.20", "translation", "ndif4ctnE6", "wms_translation")) == NULL )
	{
	printf("\n localDBInit ERROR!");
	err = -1;
	}
else if ( (tud->translated = localDBSearch(conn, tud->original, tud->to)) == NULL )
	{
	err = -1;
	}
	
localDBClose(conn);
mysql_library_end();
return(err);
}

//---------------------------------------------------------------

//  insert an english expression and its translation into local cache (DB)

int localInsert(translationUserData *tud)
{
MYSQL	*conn=NULL;
int 	err=0;

if ( (conn = localDBInit("192.168.254.20", "translation", "ndif4ctnE6", "wms_translation")) == NULL )
	{
	printf("\n localDBInit ERROR!");
	err = -1;
	}
else {
    if (localDBInsertSpanishTranslation(conn, tud->original, tud->translated) < 0)
	{
	printf("\n LocalInsert ERROR!");
	err = -1;
	}
    }	 
	 
localDBClose(conn);
mysql_library_end();
return(err);
}
	
//---------------------------------------------------------------

// main translation function,  using local cache as first option
// if it fails or no local translatino found, the routine uses 
// free google translate API

int translate(translationUserData *tud)
{
int 	err=0;

// first we try on local cache...
if ( (err = localTranslate(tud)) >= 0 )
	printf("\n Translation: LOCAL");
else
	{  // not found on local cache,  go to google...
	if ( (err = googleFreeTranslate(tud)) >= 0 )
		{    
		printf("\n Translation: GOOGLE");	
		localInsert(tud);
		}
	}

return(err);	
}

//---------------------------------------------------------------

// basic help
int usage(char *programName)
{
	printf("\n\n Usage:  %s -i \"input text to translate from spanish into english\"  -f [FROM] -t [TO] \n\n", programName);
	printf("\n  Some languages (TO and FROM) are:  'auto': 'Automatic', 'da': 'Danish',  'nl': 'Dutch' ");
	printf("\n  'en': 'English', 'fr': 'French', 'de': 'German', 'it': 'Italian', 'es': 'Spanish' \n\n");

	printf("\n\n NOTE:  -i is mandatory (text to translate!) ");
	printf("\n\n DEFAULT:  FROM:  es (spanish)  TO:  en (english) \n\n");
	
	return(0);
}

//---------------------------------------------------------------
//---------------------------------------------------------------

// simple wrapper to test funtionality!

int main(int narg, char *argv[])
{
int 					opt=0;
int 					translationError=0;
translationUserData		tud;		// this struscture contains all data to translate

if (narg<2) {
	usage(argv[0]);
	exit(0);
}

// secure init (0's)
memset(&tud, 0, sizeof(translationUserData));

// command line options and argument parser
while ((opt = getopt (narg, argv, "i:f:t:v")) != -1)
	switch (opt)
		{
		case 'i':
			tud.original = strdup(optarg);
		break;
		
		case 't':
			tud.to = strdup(optarg);
		break;
		
		case 'f':
			tud.from = strdup(optarg);
		break;
		
		case 'v':
			_verbose++;
		break;
	
		default:
			usage(argv[0]);
			exit(0);
		}

// text to translate is mandatory
if ( tud.original == NULL ) {
	usage(argv[0]);
	exit(0);
	}
	
// some defaults	
if ( tud.from == NULL ) 
	tud.from = strdup("es");

if ( tud.to == NULL ) 
	tud.to = strdup("en");
	
if ( (translationError = translate(&tud)) < 0 )
	printf("\n\n ERROR [%i]Translating text.... \n\n", translationError);
else 
	{
	printf("\n text: |%s| ", tud.original);
	printf("\n translated from '%s' to '%s' as: ", tud.from, tud.to);
	printf("\n text: |%s| \n\n", tud.translated);
	free(tud.translated);
	free(tud.original);
	free(tud.to);
	free(tud.from);
	free(tud.URIString);
	}

return 0;
}


//---------------------------------------------------------------
//---------------------------------------------------------------
