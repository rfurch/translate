#include <mysql/mysql.h>

//-------------------------------------------------------------------
//-------------------------------------------------------------------

// init and connect to DB.  Return instance fo further usage 
MYSQL	*localDBInit(char *server, char *user, char *pass, char *db);
int localDBClose(MYSQL *conn);
char *localDBSearch(MYSQL *conn, char *original, char *toLanguageAlias);
int localDBInsertSpanishTranslation(MYSQL *conn, char *englishText, char *spanishText);
