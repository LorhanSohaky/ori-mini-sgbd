#include "commandsDefines.h"

const char TABLES_INDEX[] = "tablesIndex.bin";
const char TABLES_DIR[] = "tables/";

const char CT[] = "CT";
const char RT[] = "RT";
const char AT[] = "AT";
const char LT[] = "LT";
const char IR[] = "IR";
const char BR[] = "BR";
const char AR[] = "AR";
const char RR[] = "RR";
const char CI[] = "CI";
const char RI[] = "RI";
const char GI[] = "GI";
const char EB[] = "EB";

const char U[] = "U";
const char N[] = "N";
const char A[] = "A";
const char H[] = "H";

const char *commands[] = {
    CT, RT, AT, LT, IR, BR, AR, RR, CI, RI, GI, EB
};

int qtTables = 0;

int CMD_LIMIT = 3;
int PARAMETER_LIMIT = 2;

FILE *tablesIndex = NULL;

ResultNode *resultTree = NULL;