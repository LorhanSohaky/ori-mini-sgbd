#include "parser.h"

// Erro global de interpretação de comandos
int CMD_ERROR_CODE = NONE;

// Marca a posição corrente no buffer de comando
char *parsing;

// Buffers: Nome da tabela, nome do campo(chave) e valor
// Usados em comando com único parâmetro
TableName table_name;
Field field_name;
Value value;

// Buffers: Tipos, nomes do campo(chave) e valores
// Usados em comando com mutiplos parâmetros
TypeArr type_name_arr;
FieldArr field_name_arr;
ValueArr value_arr;

// Index dos vetores a cima
int index_arr = 0;

// Identifica o comando
// command: String com a linha de comando em questão
// Lógica básica: Procura o comando e pula o ponteiro para após o mesmo, lê o nome da tabela, outro parâmetro até o ':' e outro até o ';' ou fim, repete
void parser(char *command) {
    index_arr = 0;

    if (parsing = findl(command, CT, 0)) {
        if(sscanf(parsing, "%s %[^:^;]%*c%[^;^\n]", table_name, type_name_arr[index_arr], field_name_arr[index_arr]) == 3) {
            toUpperCase(table_name);
            toUpperCase(type_name_arr[index_arr]);
            index_arr++;

            while (parsing = find(parsing, ";")) {
                if(sscanf(parsing, "%[^:^;]%*c%[^;^\n]", type_name_arr[index_arr], field_name_arr[index_arr]) == 2) {
                    toUpperCase(type_name_arr[index_arr]);
                    index_arr++;
                } else {
                    CMD_ERROR_CODE = CT_WS_USC; return;
                }
            }

            createTable(table_name, type_name_arr, field_name_arr, index_arr);
        } else {
            CMD_ERROR_CODE = CT_WS; return;
        }
    } else if (parsing = findl(command, RT, 0)) {
        if (sscanf(parsing, "%s", table_name) == 1) {
            toUpperCase(table_name);
            removeTable(table_name);
        } else {
            CMD_ERROR_CODE = RT_WS; return;
        }
    } else if (parsing = findl(command, AT, 0)) {
        if (sscanf(parsing, "%s", table_name) == 1) {
            toUpperCase(table_name);
            apTable(table_name);
        } else {
            CMD_ERROR_CODE = AT_WS; return;
        }
    } else if (parsing = findl(command, LT, 0)) {
        listTables();
    } else if (parsing = findl(command, IR, 0)) {
        if (sscanf(parsing, "%s %[^;^\n]", table_name, value_arr[index_arr]) == 2) {
            toUpperCase(table_name);

            index_arr++;

            while (parsing = find(parsing, ";")) {
                if (sscanf(parsing, "%[^;^\n]", value_arr[index_arr]) == 1) {
                    index_arr++;
                } else {
                    CMD_ERROR_CODE = IR_USC; return;
                }
            }

            includeReg(table_name, value_arr, index_arr);
        } else {
            CMD_ERROR_CODE = IR_WS; return;
        }
    } else if (parsing = findl(command, BR, 0)) {
        char *temp = parsing;
        char *aux_type = 0;

        if (temp = findl(stripStart(parsing), U, 1)) {
            aux_type = U;
        } else if (temp = findl(stripStart(parsing), N, 1)) {
            aux_type = N;
        } else {
            CMD_ERROR_CODE = BR_MP; return;
        }

        if (sscanf(temp, "%s %[^:]%*c%s", table_name, field_name, value) == 3) {
            toUpperCase(table_name);
            
            if (aux_type == U) {
                busRegU(table_name, field_name, value);
            } else if (aux_type == N) {
                busRegN(table_name, field_name, value);
            } else {
                CMD_ERROR_CODE = IN_ERROR;  return;
            }
        } else {
            CMD_ERROR_CODE = BR_WS; return;
        }
    } else if (parsing = findl(command, AR, 0)) {
        if (sscanf(parsing, "%s", table_name) == 1) {
            toUpperCase(table_name);
            apReg(table_name);
        } else {
            CMD_ERROR_CODE = AR_WS; return;
        }
    } else if (parsing = findl(command, RR, 0)) {
        if (sscanf(parsing, "%s", table_name) == 1) {
            toUpperCase(table_name);

            removeReg(table_name);
        } else {
            CMD_ERROR_CODE = RR_WS; return;
        }
    } else if (parsing = findl(command, CI, 0)) {
        char *temp = parsing;
        char *aux_type = 0;

        if (temp = findl(stripStart(parsing), A, 1)) {
            aux_type = A;
        } else if (temp = findl(stripStart(parsing), H, 1)) {
            aux_type = H;
        } else {
            CMD_ERROR_CODE = CI_MP; return;
        }

        if (sscanf(temp, "%s %s", table_name, field_name) == 2) {
            toUpperCase(table_name);
            
            if (aux_type == A) {
                createIndexA(table_name, field_name);
            } else if (aux_type == H) {
                createIndexH(table_name, field_name);
            } else {
                CMD_ERROR_CODE = IN_ERROR;  return;
            }
        } else {
            CMD_ERROR_CODE = CI_WS;  return;              
        }
    } else if (parsing = findl(command, RI, 0)) {
        if (sscanf(parsing, "%s %s", table_name, field_name) == 2) {
            toUpperCase(table_name);
            removeIndex(table_name, field_name);
        } else {
            CMD_ERROR_CODE = RI_WS; return;
        }
    } else if (parsing = findl(command, GI, 0)) {
        if (sscanf(parsing, "%s %s", table_name, field_name) == 2) {
            toUpperCase(table_name);
            genIndex(table_name, field_name);
        } else {
            CMD_ERROR_CODE = GI_WS; return;
        }
    } else if (parsing = findl(command, EB, 0)) {
        CMD_ERROR_CODE = EXIT; return;
    } else {
        CMD_ERROR_CODE = NO_CMD; return;
    }

    CMD_ERROR_CODE = NONE; return;
}