/* Simple C program that connects to MySQL Database server*/
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//-------------------------------------------------------------------
//-------------------------------------------------------------------

// init and connect to DB.  Return instance fo further usage 
MYSQL	*localDBInit(char *server, char *user, char *pass, char *db)
{
MYSQL       *conn=NULL;

// init instance
if ( (conn = mysql_init(NULL)) == NULL)
	return(NULL);			// strange error, memory maybe...
	
// Connect to database 
if (!mysql_real_connect(conn, server, user, pass, db, 0, NULL, 0))
    {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(NULL);
    }

return(conn);
}

//-------------------------------------------------------------------

// terminate DB connection
int localDBClose(MYSQL *conn)
{

if (conn)
	mysql_close(conn);

return(0);
}

//-------------------------------------------------------------------

// this function searches in local DB (MySQL) for a translation 
// for 'original'  sentence FROM ENGLISH (hardcoded so far...) into 'toLanguageAlias'
// returns ALLOCATED translation, so
// IT IS MANDATORY TO FREE RETURNED STRING AFTER USE
// in case of no translation found or error returns NULL

char *localDBSearch(MYSQL *conn, char *original, char *toLanguageAlias)
{
MYSQL_RES   *res=NULL;
MYSQL_ROW   row;
char 		*myQueryString=NULL;
char 		*localTranslation=NULL;
char 		*originalEscaped=NULL;

if (conn == NULL)	{
	fprintf(stderr, "\n localTranslateSearch ERROR:  NULL DB handler!!! ");
	return(NULL);
	}

if (original == NULL)	{
	fprintf(stderr, "\n localTranslateSearch ERROR:  NULL sentence !!! ");
	return(NULL);
	}

if (toLanguageAlias == NULL)	{
	fprintf(stderr, "\n localTranslateSearch ERROR:  NULL toLanguageAlias !!! ");
	return(NULL);
	}	

if ( (myQueryString = calloc(300 + strlen(original) + strlen(toLanguageAlias), sizeof(char))) == NULL )	{
	fprintf(stderr, "\n localTranslateSearch ERROR:  allocation !!! ");
	return(NULL);
	}	

if ( ( originalEscaped = calloc(10 + 2 * strlen(original), sizeof(char))) == NULL )  	{
	fprintf(stderr, "\n localTranslateSearch ERROR: allocation !!! ");
	return(NULL);
	}	

mysql_real_escape_string(conn, originalEscaped, original, strlen(original));

sprintf(myQueryString, "SELECT t.translation FROM translations t \
 LEFT JOIN languages l ON t.to_language_id=l.id \
 LEFT JOIN expressions_cache e ON t.expression_id=e.id \
 WHERE l.alias='%s' AND e.english_expression='%s';", toLanguageAlias, originalEscaped);
 
 // send SQL query 
if (mysql_query(conn, myQueryString))
    {
	fprintf(stderr, "\n localTranslateSearch ERROR: %s",  mysql_error(conn));
	return(NULL);
    }
    
if ( (res = mysql_store_result(conn)) == NULL )
	{
	fprintf(stderr, "\n localTranslateSearch ERROR: %s",  mysql_error(conn));
	return(NULL);
	}

if (mysql_num_rows(res) > 0)
	if  ((row = mysql_fetch_row(res))) 
		localTranslation = strdup(row[0]);
    
mysql_free_result(res);
if (myQueryString)
	free(myQueryString);
if (originalEscaped)
	free(originalEscaped);	
	
return(localTranslation);
}

//-------------------------------------------------------------------

// this function inserts in local DB (MySQL) an spanish translation 
// for original text (in english)

int localDBInsertSpanishTranslation(MYSQL *conn, char *englishText, char *spanishText)
{
MYSQL_RES   	*res=NULL;
MYSQL_ROW   	row;
char 		*myQueryString=NULL;
int 		englishExpressionID=0;

if (conn == NULL)	{
	fprintf(stderr, "\n localDBInsertSpanishTranslation ERROR:  NULL DB handler!!! ");
	return(-1);
	}

if (englishText == NULL)	{
	fprintf(stderr, "\n localDBInsertSpanishTranslation ERROR:  NULL sentence !!! ");
	return(-1);
	}

if (spanishText == NULL)	{
	fprintf(stderr, "\n localDBInsertSpanishTranslation ERROR:  NULL toLanguageAlias !!! ");
	return(-1);
	}	

if ( (myQueryString = calloc(300 + strlen(spanishText) + strlen(englishText), sizeof(char))) == NULL )	{
	fprintf(stderr, "\n localTranslateSearch ERROR:  allocation !!! ");
	return(-1);
	}	
	
// check if english expression is already present, just in case
sprintf(myQueryString, "SELECT id FROM wms_translation.expressions_cache WHERE english_expression='%s';", englishText);
if (mysql_query(conn, myQueryString))
    {
	fprintf(stderr, "\n localTranslateSearch ERROR: %s",  mysql_error(conn));
	return(-1);
    }
if ( (res = mysql_store_result(conn)) == NULL )
	{
	fprintf(stderr, "\n localTranslateSearch ERROR: %s",  mysql_error(conn));
	return(-1);
	}

englishExpressionID = -1;
if (mysql_num_rows(res) > 0)
    if  ((row = mysql_fetch_row(res))) 
	englishExpressionID = atoi(row[0]);
    
if ( englishExpressionID < 0 )
    {
    printf ("\n inserting NEW expression: %s", englishText);

    sprintf(myQueryString, "INSERT INTO wms_translation.expressions_cache (english_expression) VALUES ('%s');", englishText);
    if (mysql_query(conn, myQueryString))
	{
	    fprintf(stderr, "\n localTranslateSearch ERROR: %s",  mysql_error(conn));
	    return(-1);
	}




    }   
    
mysql_free_result(res);
if (myQueryString)
	free(myQueryString);
	
return(0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------



