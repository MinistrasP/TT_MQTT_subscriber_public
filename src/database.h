int createDb(void);
int insertData(char *inputLevel, char *data);
int printData(void);
int deleteAllData(void);
int callback(void *NotUsed, int argc, char **argv, char **azColName);
int openDatabase(void);
void closeDatabase(void);