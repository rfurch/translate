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

myQueryString = calloc(300 + strlen(original) + strlen(toLanguageAlias), sizeof(char));

sprintf(myQueryString, "SELECT t.translation FROM translations t \
 LEFT JOIN languages l ON t.toLanguageID=l.id \
 LEFT JOIN expressionsCache e ON t.expressionID=e.id \
 WHERE l.alias='%s' AND e.englishExpression='%s';", toLanguageAlias, original);
 
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
	
return(localTranslation);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------



