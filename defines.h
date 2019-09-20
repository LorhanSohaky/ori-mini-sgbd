#ifndef DEFINES_H
#define DEFINES_H

#define HISTORY_FILE "./.history"
#define PROMPT "\x1b[1;32mSGBD\x1b[0m> "

#define NUMBER_COLUMNS_LIMIT 32

typedef char *TableName;
typedef char Type;
typedef Type TypeArr[NUMBER_COLUMNS_LIMIT];
typedef char *Field;
typedef Field FieldArr[NUMBER_COLUMNS_LIMIT];
typedef void *Value;
typedef Value ValueArr[NUMBER_COLUMNS_LIMIT];

typedef struct Table {
    int cols;
    int rows;
    TableName name;
    TypeArr types;
    FieldArr fields;
} Table;

typedef struct Row {
    int cols;
    TableName tableName;
    ValueArr values;
} Row;

typedef struct Selection {
    char parameter;
    TableName tableName;
    Field field;
    Value value;
} Selection;

typedef struct {
    char *command;
    union {
        Table table;
        Row row;
        Selection selection;
    } data;
} ParsedData;

extern const char INT[];
extern const char STR[];
extern const char FLT[];
extern const char BIN[];

extern int TYPE_LIMIT;

#endif /* DEFINES_H */