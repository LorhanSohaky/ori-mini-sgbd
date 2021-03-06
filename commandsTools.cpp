#include "commandsTools.hpp"

// Verifica se existe um índice hash para a tabela
int tem_index_hash(TableName tableName, Field field) {
    char * hashFilename = glueString(5, "tables_index/", tableName, "_", field, "_hash.index");
    return fileExist(hashFilename);
}

// Verifica se existe um índice tree para a tabela
int tem_index_tree(TableName tableName, Field field) {
    char * treeFilename = glueString(5, "tables_index/", tableName, "_", field, "_tree.index");
    return fileExist(treeFilename);
}

// Antes de usar essa função, garanta que o campo exista na tablea utilizando a função fieldExistInTable(...)
Type getFieldType(TableName tableName, Field field) {
    // Path do arquivo da tabela
    char *path = glueString(2, TABLES_DIR, tableName);
    // Abre o arquivo da tabela
    FILE *tableFile = fopenSafe(path, "rb+");
    // Lê os metadados
    Table table;
    fread(&table, sizeof(Table), 1, tableFile);
    // Laço de repetição para encontrar a posição do campo desejado
    int i;
    for(i = 0; i < table.cols && (strcmp(table.fields[i], field)); i++);
    // Retorna o tipo do campo desejado baseado na sua posição
    return table.types[i];
}

// Lê os metadados de uma tabela e os retorna
Table readTable(TableName name) {
    // Path do arquivo da tabela
    char *path = glueString(2, TABLES_DIR, name);
    // Abre o arquivo da tabela
    FILE *tableFile = fopenSafe(path, "rb+");
    // Lê os metadados
    Table table;
    fread(&table, sizeof(Table), 1, tableFile);
    // Variável que indicará se o campo existe na tabela
    return table;
}

int fieldExistInTable(char *name, Field field) {
    // Path do arquivo da tabela
    char *path = glueString(2, TABLES_DIR, name);
    // Abre o arquivo da tabela
    FILE *tableFile = fopenSafe(path, "rb+");
    // Lê os metadados
    Table table;
    fread(&table, sizeof(Table), 1, tableFile);
    // Variável que indicará se o campo existe na tabela
    int exist = 0;
    for(int i = 0; i < table.cols && !exist; i++) {
        exist = (strcmp(table.fields[i], field) == 0);
    }
    return exist;
}

int tableExists(int qtTables, char *name) {
    // Flag que representa a existência da tabela
    int exists = 0;

    // Pula para o começo dos nomes
    fseek(tablesIndex, sizeof(int), SEEK_SET);

    int i = 0;
    // Para cada tabela, enquanto é diferente
    while (exists == 0 && i < qtTables) {
        // Número de blocos
        int blocks = 0;

        // Lê o número de blocos
        fread(&blocks, sizeof(int), 1, tablesIndex);

        // Se o espaço possuí informações válidas
        if (blocks > 0) {
            // Tamanho real do nome
            int size = blocks*BLOCK_SIZE;
            char *buf = (char *)mallocSafe(size);

            // Lê o nome
            fread(buf, size, 1, tablesIndex);

            // Compara com o nome existente
            exists = (strcmp(buf, name) == 0);

            free(buf);

            // Incrementa só se a tabela é válida
            i++;
        } else {
            // Pula o espaço no caso de informações inválidas
            blocks *= -1;
            fseek(tablesIndex, blocks*BLOCK_SIZE, SEEK_CUR);
        }
    }

    return exists;
}

int tableNameIsUnique(int qtTables, char *name, long int *marker) {
    // Flag, novo nome é diferente
    int diff = 1;

    // Pula para o começo dos nomes
    fseek(tablesIndex, sizeof(int), SEEK_SET);

    int i = 0;
    // Para cada tabela, enquanto é diferente
    while (diff != 0 && i < qtTables) {
        // Número de blocos
        int blocks = 0;

        // Marcador da posição do igual ou última
        if (marker) {
            *marker = ftell(tablesIndex);
        }

        // Lê o número de blocos
        fread(&blocks, sizeof(int), 1, tablesIndex);

        // Se o espaço possuí informações válidas
        if (blocks > 0) {
            // Tamanho real do nome
            int size = blocks*BLOCK_SIZE;
            char *buf = (char *)mallocSafe(size);

            // Lê o nome
            fread(buf, size, 1, tablesIndex);

            // Compara com o novo nome
            diff = strcmp(buf, name);

            free(buf);

            // Incrementa só se a tabela é válida
            i++;
        } else {
            // Pula o espaço no caso de informações inválidas
            blocks *= -1;
            fseek(tablesIndex, blocks*BLOCK_SIZE, SEEK_CUR);
        }
    }

    return diff;
}

void addTableName(int qtTables, char *name) {
    qtTables++;

    // Pula para o começo do arquivo
    fseek(tablesIndex, 0, SEEK_SET);
    // Escreve o novo valor
    fwrite(&qtTables, sizeof(int), 1, tablesIndex);

    // Pula para o fim do arquivo
    fseek(tablesIndex, 0, SEEK_END);

    // Tamanho da string
    int length = strlen(name);
    // Quantidade de blocos
    int blocks = length/BLOCK_SIZE + (length%BLOCK_SIZE != 0 ? 1 : 0);

    // Escreve a quantidade de blocos
    fwrite(&blocks, sizeof(int), 1, tablesIndex);
    // Escreve a string
    fwrite(name, blocks*BLOCK_SIZE, 1, tablesIndex);
}

long int addToExFile(char *str, FILE *dataFile, FILE *emptyFile) {
    // Posição que a string foi escrita
    long int pos;
    // Tamanho da string
    int size = strlen(str);

    // Auxiliar para o block vazio
    EmptyBlock emptyBlock;
    emptyBlock.size = 0;
    emptyBlock.pos = -1;

    // Lê o maior bloco do arquivo
    fseek(emptyFile, 0, SEEK_SET);
    fread(&emptyBlock, sizeof(EmptyBlock), 1, emptyFile);

    // Se o maior elemento é maior que a string
    if (emptyBlock.size >= size) {
        // Remove o maior bloco
        removeEmptyBlock(emptyFile);

        // Pula para posição vazia
        fseek(dataFile, emptyBlock.pos, SEEK_SET);
        // Salva a posição da string
        pos = ftell(dataFile);
        // Escreve o tamanho da string
        fwrite(&size, sizeof(int), 1, dataFile);
        // Escreve a string
        fwrite(str, size, 1, dataFile);

        // Atualiza o tamanho do bloco remanescente
        emptyBlock.size -= size;

        // Se o bloco ainda possuí espaço
        if (emptyBlock.size) {
            // Atualiza a posição (tamanho da string, tamanho do tamanho da string)
            emptyBlock.pos += size + sizeof(int);
            // Grava o novo tamanho, logo após a última string
            fwrite(&(emptyBlock.size), sizeof(int), 1, dataFile);
            // Coloca o bloco atualizado no arquivo
            insertEmptyBlock(emptyFile, &emptyBlock);
        }
    // Se a lista está vazia ou o maior bloco não é suficiente
    } else {
        // Pula para o fim
        fseek(dataFile, 0, SEEK_END);
        // Salva a posição da string
        pos = ftell(dataFile);
        // Escreve o tamanho da string
        fwrite(&size, sizeof(int), 1, dataFile);
        // Escreve a string
        fwrite(str, size, 1, dataFile);
    }

    return pos;
}

void removeFromExFile(long int pos, FILE *dataFile, FILE *emptyFile) {
    // Tamanho do bloco
    int size;
    // Pula para o bloco que será removida
    fseek(dataFile, pos, SEEK_SET);
    // Lê o tamanho do bloco
    fread(&size, sizeof(int), 1, dataFile);

    // Novo bloco
    EmptyBlock empty;
    empty.size = size;
    empty.pos = pos;

    insertEmptyBlock(emptyFile, &empty);
}

// Remove o primeiro elemento do arquivo
void removeEmptyBlock(FILE *emptyFile) {
    EmptyBlock empty;

    // Posição corrente
    long pos;

    // Vai para o começo do arquivo
    fseek(emptyFile, 0, SEEK_SET);

    // Lê a posição do maior elemento
    fread(&pos, sizeof(long), 1, emptyFile);

    // Pula para o maior elemento
    fseek(emptyFile, pos, SEEK_SET);

    // Lê o maior bloco
    fread(&empty, sizeof(EmptyBlock), 1, emptyFile);

    // Pula para o começo
    fseek(emptyFile, 0, SEEK_SET);

    // Grava a posição do próximo elemento
    fread(&(empty.next), sizeof(long), 1, emptyFile);
}

// Coloca um elemento na lista do arquivo
void insertEmptyBlock(FILE *emptyFile, EmptyBlock *emptyBlock) {
    EmptyBlock empty;

    // Posição corrente
    long pos = -1;
    long pastPos = -1;

    // Vai para o começo do arquivo
    fseek(emptyFile, 0, SEEK_SET);

    // Lê a posição do maior elemento
    fread(&pos, sizeof(long), 1, emptyFile);

    // Pula para o maior elemento
    fseek(emptyFile, pos, SEEK_SET);

    // Lê o maior bloco
    fread(&empty, sizeof(EmptyBlock), 1, emptyFile);

    // Enquanto o novo bloco é menor
    while (emptyBlock->size < empty.size) {
        // Atualiza o pos
        pastPos = pos;
        pos = empty.next;

        // Pula para a próximo elemento
        fseek(emptyFile, pos, SEEK_SET);

        // Lê o bloco
        fread(&empty, sizeof(EmptyBlock), 1, emptyFile);
    }

    // O novo bloco é maior ou igual a um encontrado

    // Atualiza o bloco com a posição do próximo bloco
    emptyBlock->next = pos;

    // Vai para o fim do arquivo
    fseek(emptyFile, 0, SEEK_END);

    // Salva a posição
    pos = ftell(emptyFile);

    // Grava o novo bloco
    fwrite(&empty, sizeof(EmptyBlock), 1, emptyFile);

    // Se pos == -1, então é o maior bloco
    if (pastPos == -1) {
        // Vai para o começo do arquivo
        fseek(emptyFile, 0, SEEK_SET);

        // Grava a posição do novo maior elemento
        fwrite(&pos, sizeof(long), 1, emptyFile);
    
    // Se não é um bloco no meio da lista
    } else {
        // Vai para o bloco que vai ser atualizado
        fseek(emptyFile, pastPos, SEEK_SET);

        // Lê o bloco
        fread(&empty, sizeof(EmptyBlock), 1, emptyFile);

        // Atualiza a posição do próximo elemento
        empty.next = pos;

        // Grava o bloco atualizado
        fwrite(&empty, sizeof(EmptyBlock), 1, emptyFile);
    }
}
