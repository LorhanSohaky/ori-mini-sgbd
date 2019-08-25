#include "commands.h"

FILE *tables_index = NULL; // Variável global utilizada para manipular arquivos
int qt_tables = 0;      // Quantidade de tabelas

// Cria tabela
// table: Struct com as informações para crira uma tabela
void createTable(TableWType *table) {
    tables_index = safe_fopen(TABLES_INDEX, "rb+");
    qt_tables = read_qt_tables(tables_index);

    if (qt_tables) {
        TableName *names = read_tables_names(tables_index, qt_tables);

        if (tableNameExists(names, table->name, qt_tables)) {
            raiseError(CT_TABLE_EXISTS);
        }

        free(names);
    }

    TableWRep tableData;

    // Converte a tabela
    if(!convertToRep(&tableData, table)) {
        raiseError(CT_WRONG_TYPE);
    }

    // Escreve a tabela no arquivo
    write_table_metadata(tables_index, &tableData, qt_tables);

    qt_tables++;

    // Salva o novo número de tabelas
    write_qt_tables(tables_index, qt_tables);

    fclose(tables_index);

    printf("Criando tabela %s.\n\n", table->name);
}

// Remove tabela
// table_name: Nome da tabela
// OBS: remove o nome da lista de index e salva o último no lugar, a menos que seja o último esteja sendo removido
void removeTable(TableName table_name) {
    tables_index = safe_fopen(TABLES_INDEX, "rb+");

    qt_tables = read_qt_tables(tables_index);

    if (!qt_tables) {
        printf("Nenhuma tabela no sistema.\n");
        fclose(tables_index);
        return;
    }

    TableName *names = read_tables_names(tables_index, qt_tables);

    int i = 0;
    while (i < qt_tables) {
        if (!strcmp(names[i], table_name)) {
            break;
        }
        i++;
    }

    if (i == qt_tables) {
        raiseError(RT_CANT_FIND_TABLE);
    }

    printf("Removendo a tabela %s.\n", table_name);

    qt_tables--;

    if (i != qt_tables) {
        strncpy(names[i], names[qt_tables], sizeof(TableName));
        write_tables_names(tables_index, names, qt_tables);
    }

    for (int i = 0; i < qt_tables; i++) {
        printf(">>%s\n", names[i]);
    }

    write_qt_tables(tables_index, qt_tables);

    TablePath path = "";

    safe_strcat(path, TABLES_DIR);
    safe_strcat(path, table_name);
    safe_strcat(path, TABLE_EXTENSION);

    safe_remove(path);

    free(names);

    fclose(tables_index);
}

// Apresenta tabela tabela
// table_name: Nome da tabela
void apTable(TableName table_name) {
    TableWRep *tableData = read_table_metadata(table_name);

    if (!tableData) {
        raiseError(AT_CANT_FIND_TABLE);
    }

    TableWType *table = safe_malloc(sizeof(TableWType));

    convertToType(table, tableData);

    printf("Mostrando a tabela %s.\n\n", table_name);

    printf("%s\n", table->name);

    for (int j = 0; j < table->cols; j++) {
        printf("- %s:%s\n", table->types[j], table->fields[j]);
    }

    free(tableData);
    free(table);
}

// Lista tabelas
void listTables() {
    tables_index = safe_fopen(TABLES_INDEX, "rb+");

    qt_tables = read_qt_tables(tables_index);

    if (!qt_tables) {
        printf("Nenhuma tabela no sistema.\n");
        fclose(tables_index);
        return;
    }

    printf("Listando %d tabelas.\n", qt_tables);

    TableName *names = read_tables_names(tables_index, qt_tables);

    for (int i = 0; i < qt_tables; i++) {
        printf("- %s\n", names[i]);
    }

    free(names);

    fclose(tables_index);

    printf("\n");
}

// Inclui registro na tabela
// row: Struct com os valores de um registro
void includeReg(Row *row) {
    printf("Novo registro na tabela %s.\n", row->table_name);

    TableWRep *meta = read_table_metadata(row->table_name);

    TablePath path = "";

    safe_strcat(path, TABLES_DIR);
    safe_strcat(path, row->table_name);
    safe_strcat(path, TABLE_EXTENSION);

    FILE *table_file = safe_fopen(path, "rb+");
    fseek(table_file, 0, SEEK_END);

    if (meta->cols != row->size) {
        raiseError(IR_DIFF_PARAM_NUMB);
    }

    int temp = 0;

    for (int i = 0; i < row->size; i++) {
        if (meta->types[i] == STR_REP) {
            char str[STR_SIZE] = "";
            if (sscanf(row->values[i], "%[^\n]", str) == 1) {        
                fwrite(str, STR_SIZE * sizeof(char), 1, table_file);

                temp+=256;
            } else {
                raiseError(IR_WRONG_VALUE);
            }
        } else if (meta->types[i] == INT_REP) {
            int i = 0;
            if (sscanf(row->values[i], "%d", &i) == 1) {        
                fwrite(&i, sizeof(int), 1, table_file);
           
                temp+=4;
            } else {
                raiseError(IR_WRONG_VALUE);
            }
        } else if (meta->types[i] == FLT_REP) {
            float f = 0.0;
            if (sscanf(row->values[i], "%f", &f) == 1) {        
                fwrite(&f, sizeof(float), 1, table_file);
             
                temp+=4;
            } else {
                raiseError(IR_WRONG_VALUE);
            }
        } else if (meta->types[i] == BIN_REP) {
            char bin[BIN_SIZE] = "";
            if (sscanf(row->values[i], "%[^\n]", bin) == 1) {        
                fwrite(bin, BIN_SIZE * sizeof(char), 1, table_file);
               
                temp+=256;
            } else {
                raiseError(IR_WRONG_VALUE);
            }
        }
    }

    printf("%d\n", temp);

    free(meta);

    fclose(table_file);
}

// Busca registros na tabela, único
// table_name: Nome da tabela
// field_name: Nome do campo(chave)
// value: Nome do valor
void busRegU(TableName table_name, Field field_name, Value value) {
    printf("Buscando %s igual à %s em %s.\n", field_name, value, table_name);

    raiseError(TODO);
}

// Busca registros na tabela, todos
// table_name: Nome da tabela
// field_name: Nome do campo(chave)
// value: Nome do valor
void busRegN(TableName table_name, Field field_name, Value value) {
    printf("Buscando todos os %s igual à %s em %s.\n", table_name, field_name, value);

    raiseError(TODO);
}

// Apresenta registros pesquisados da tabela
// table_name: Nome da tabela
void apReg(TableName table_name) {
    printf("Mostrando a pesquisa de %s.\n", table_name);

    raiseError(TODO);
}

// Remove registro da tabela
// table_name: Nome da tabela
void removeReg(TableName table_name) {
    printf("Registros removidos de %s.\n", table_name);
    
    raiseError(TODO);
}

// Cria index da tabela, árvore
// table_name: Nome da tabela
// field_name: Nome da campo(chave) a ser usado
void createIndexA(TableName table_name, Field field_name) {
    printf("Criado um índice com árvore para %s usando a coluna %s.\n", table_name, field_name);

    raiseError(TODO);
}

// Cria index da tabela, heap
// table_name: Nome da tabela
// field_name: Nome da campo(chave) a ser usado
void createIndexH(TableName table_name, Field field_name) {
    printf("Criado um índice com heap para %s usando a coluna %s.\n", table_name, field_name);

    raiseError(TODO);
}

// Remove index da tabela
// table_name: Nome da tabela
// field_name: Nome da campo(chave) a ser usado
void removeIndex(TableName table_name, Field field_name) {
    printf("Removendo o índice de %s referente a chave %s.\n", table_name, field_name);

    raiseError(TODO);
}

// Gera index
// table_name: Nome da tabela
// field_name: Nome da campo(chave) a ser usado
void genIndex(TableName table_name, Field field_name) {
    printf("Gerando um índice com %s para %s usando a coluna %s.\n", "-recuperar-", table_name, field_name);

    raiseError(TODO);
}