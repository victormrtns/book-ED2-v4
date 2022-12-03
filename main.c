#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Livro
{
    char ISBN[14];
    char titulo[50];
    char autor[50];
    char ano[5];
} REG;

typedef struct Hash
{
    char ISBN[14];
    int rrn;
} Hash;
int hash_func(char *isbn);
void inserir();
REG pegar_registro();
void escrever_arquivo(REG registro);
void criar_hash();
void remover();
void buscar();
void pegar_isbn_pos(int pos);
int main() {
    FILE *out;
    FILE *hash;

    REG registro;
    int contador=0;
    int opcao;
    printf("-------MENU-------\n");
    do{
        printf("1.Inserir\n");
        printf("2.Remover\n");
        printf("3. Buscar\n");
        printf("0. Sair do programa\n");
        printf("Digite a opcao: ");
        scanf("%d",&opcao);

        switch (opcao) {
            case 0:
                break;
            case 1:
                printf("Entrando na funcao inserir:\n");
                inserir();
                break;
            case 2:
                printf("Entrando na funcao remover:\n");
                remover();
                break;
            case 3:
                printf("Entrando na funcao buscar:\n");
                buscar();
                break;
        }
    }while(opcao!=0);

    return 0;
}
REG pegar_registro() {

    FILE *insere,*hash;
    int resto;
    char aux[14],ISBN[14];
    REG registro;
    insere = fopen("insere.bin", "rb+");
    int contador = 0;
    if((hash = fopen("hash.bin","r+b")) == NULL ){
        printf("Criar Arquivo hash.bin\n");
        criar_hash();
    }
    // le no insere.bin registro a inserir e escreve no final do arq main.bin string formatada com todos os dados
    while (fread(&registro, sizeof(REG), 1, insere)) {
        sprintf(aux, "%s", registro.ISBN);

        // quando acha isbn q ainda nao leu recupera os dados
        if (aux[0] != '/') {
            resto = hash_func(aux);
            fseek(hash,((resto*18)),SEEK_SET);
            fread(&ISBN,sizeof(char),14,hash);
            fclose(hash);
            if(strcmp(ISBN,aux)==0){
                aux[0] = '/';
                fseek(insere,sizeof(REG)*contador,SEEK_SET);
                fwrite(aux,sizeof(aux),1,insere);
                fclose(insere);
                return registro;
            }
            escrever_arquivo(registro);
            // copia isbn, volta no comeco para posicionar no arquivo do comeco do registro pra pegar todos os dados
            fseek(insere,sizeof(REG)*contador,SEEK_SET);
            // aux na primeira posicao recebe / e escreve no arquivo insere / no inicio do isbn
            aux[0] = '/';
            fwrite(aux,sizeof(aux),1,insere);
            fclose(insere);
            return registro;
        }
        contador++;
    }
    printf("Nao retorna mais nada\n");
}

void escrever_arquivo(REG registro){
    FILE *out;
    out = fopen("main.bin", "r+b");
    if(out == NULL){
        out = fopen("main.bin", "w+b");
    }
    fseek(out, 0, SEEK_END);
    fwrite(&registro.ISBN, sizeof(registro.ISBN), 1, out);
    fwrite(&registro.titulo, sizeof(registro.titulo), 1, out);
    fwrite(&registro.autor, sizeof(registro.autor), 1, out);
    fwrite(&registro.ano, sizeof(registro.ano), 1, out);
    fclose(out);
}

void inserir(){
    REG registro;
    int resto=0,resto1=0;
    int tentativa = 0;
    int pos;
    char ISBN[14];
    FILE *hash;
    FILE *main;
    int contador = 0;
    FILE *out;
    registro = pegar_registro();
    out = fopen("main.bin", "r+b");
    int c = getc(out);
    while (c != EOF) {
        c = getc(out);
        contador++;
    }
    contador = contador/119;
    contador --;
    fclose(out);

   main= fopen("main.bin","r+b");
    if((hash = fopen("hash.bin","r+b")) == NULL ){
        printf("Criar Arquivo hash.bin\n");
        criar_hash();
    }
    hash = fopen("hash.bin","r+b");
    resto= hash_func(registro.ISBN);
    resto1 = resto;
    while(resto<31){
        if(tentativa>30){
            printf("Nao e possivel inserir mais no arquivo hash\n");
            fclose(hash);
            fclose(main);
            return;
        }
        fseek(hash,((resto*18)),SEEK_SET);
        fread(&ISBN,sizeof(char),14,hash);
        if(strcmp(ISBN,registro.ISBN)==0){
            printf("Ja existe o que deseja inserir!\n");
            return;
        }
        if(resto ==30){
            fseek(hash,((resto*18)+14),SEEK_SET);
            fread(&pos,sizeof(int),1,hash);
            if(pos>=0){
                resto =0;
                resto1 = 300;
                tentativa = 1;
            }
        }
        fseek(hash,((resto*18)+14),SEEK_SET);
        fread(&pos,sizeof(int),1,hash);
        if(((pos ==-1) || (pos==-2)) && (resto == resto1)){
            fseek(hash,-18,SEEK_CUR);
            fwrite(registro.ISBN, sizeof(char), 14, hash);
            fwrite(&contador, sizeof(int), 1, hash);
            fclose(hash);
            fclose(main);
            printf("ISBN: %s \n",registro.ISBN);
            printf("Endereco:%d \n",resto);
            printf("Chave %s inserida com sucesso: \n",registro.ISBN);
            return;
        }
        if(((pos ==-1) || (pos==-2)) && (resto != resto1)){
            fseek(hash,-18,SEEK_CUR);
            fwrite(registro.ISBN, sizeof(char), 14, hash);
            fwrite(&contador, sizeof(int), 1, hash);
            fclose(hash);
            fclose(main);
            printf("ISBN: %s \n",registro.ISBN);
            printf("Endereco:%d \n",resto);
            printf("Colisao\n");
            printf("Tentativa:%d \n ",tentativa);
            printf("Chave %s inserida com sucesso \n",registro.ISBN);
            return;
        }
        tentativa++;
        resto++;
    }
}



void criar_hash(){
    FILE *hash_file;
    const int tam = 31;
    hash_file = fopen("hash.bin","w+b");
        Hash hash[tam];
        int i, j;

        for(i = 0; i < 31; i++) {
            for (j = 0; j < 13; j++){
                hash[i].ISBN[j] = '|';
            }
            hash[i].ISBN[13] = '\0';
            hash[i].rrn = -1;
            fwrite(hash->ISBN, sizeof(char), 14, hash_file);
            fwrite(&hash->rrn, sizeof(int), 1, hash_file);
        }
    printf("Arquivo hash criado com sucesso\n");
    fclose(hash_file);
}

int hash_func(char *isbn){
    int soma;
    printf("ISBN que passara pela func hash: %s\n",isbn);

    soma = (int)(atoll(isbn) % 31);
    return soma;

}

void remover(){
    FILE *out,*remove,*hash;
    char aux[14],ISBN[14],aux1[14];
    int contador = 0;
    int contador1;
    int resto;
    int pos;
    out = fopen("main.bin", "r+b");
    hash = fopen("hash.bin", "r+b");
    remove = fopen("remove.bin", "r+b");
    if(out ==NULL || hash==NULL || remove==NULL){
        printf("Algum arquivo essencial nao foi criado\n");
    }
    while (fread(&aux, sizeof(aux), 1, remove)) {
        // quando acha isbn q ainda nao leu recupera os dados
        if (aux[0] != '/') {
            strcpy(aux1,aux);
            contador1=contador;
            resto = hash_func(aux);

            while(resto<31) {
                if(resto ==30){
                    fseek(hash,((resto*18)),SEEK_SET);
                    fread(&ISBN,sizeof(char),14,hash);
                    if(strcmp(ISBN,aux)!=0){
                        resto =0;
                    }
                }
                fseek(hash, ((resto * 18) + 14), SEEK_SET);
                fread(&pos, sizeof(int), 1, hash);
                if (pos>=0) {
                    fseek(hash, -18, SEEK_CUR);
                    fread(&ISBN, sizeof(char), 14, hash);
                    if(strcmp(ISBN,aux)==0){
                        fseek(hash, ((resto * 18) + 14), SEEK_SET);
                        resto = -2;
                        fwrite(&resto, sizeof(int), 1, hash);
                        aux[0] = '/';

                        fseek(remove,14*contador,SEEK_SET);
                        fwrite(aux,sizeof(aux),1,remove);
                        fclose(remove);
                        fclose(out);
                        fclose(hash);
                        return;
                    }
                }
                resto++;
                if(pos==-1){
                    aux1[0] = '/';
                    fseek(remove,14*contador1,SEEK_SET);
                    fwrite(aux1,sizeof(aux),1,remove);
                    printf("Nao Encontra ISBN para remover\n");
                    fclose(remove);
                    fclose(out);
                    fclose(hash);
                    return;
                }

            }
            aux1[0] = '/';
            fseek(remove,14*contador1,SEEK_SET);
            fwrite(aux1,sizeof(aux),1,remove);
            printf("Nao Encontra ISBN para remover\n");
            fclose(remove);
            fclose(out);
            fclose(hash);
        }
        contador++;
    }

}

void buscar(){
    FILE *out,*busca,*hash;
    char aux[14],ISBN[14],aux1[14];
    int contador = 0;
    int acesso =0;
    int contador1;
    int resto;
    int pos;
    out = fopen("main.bin", "r+b");
    hash = fopen("hash.bin", "r+b");
    busca = fopen("busca.bin", "r+b");
    if(out ==NULL || hash==NULL || busca==NULL){
        printf("Algum arquivo essencial nao foi criado\n");
    }
    while (fread(&aux, sizeof(aux), 1, busca)) {
        // quando acha isbn q ainda nao leu recupera os dados
        if (aux[0] != '/') {
            contador1 = contador;
            strcpy(aux1,aux);
            resto = hash_func(aux);
            while(resto<31) {
                if(resto ==30){
                    fseek(hash,((resto*18)),SEEK_SET);
                    fread(&ISBN,sizeof(char),14,hash);
                    if(strcmp(ISBN,aux)!=0){
                        resto =0;
                        acesso = 1;
                    }
                }
                fseek(hash, ((resto * 18) + 14), SEEK_SET);
                fread(&pos, sizeof(int), 1, hash);
                if(pos ==-1){
                    printf("Chave %s nao encontrada\n",aux1);
                    aux1[0] = '/';
                    fseek(busca,14*contador1,SEEK_SET);
                    fwrite(aux1,sizeof(aux),1,busca);
                    fclose(busca);
                    fclose(out);
                    fclose(hash);
                    return;
                }
                if (pos>=0) {
                    fseek(hash, -18, SEEK_CUR);
                    fread(&ISBN, sizeof(char), 14, hash);
                    if(strcmp(ISBN,aux)==0){
                        if(pos ==-2){
                            printf("Chave %s nao encontrada\n",aux1);
                            aux1[0] = '/';
                            fseek(busca,14*contador1,SEEK_SET);
                            fwrite(aux1,sizeof(aux),1,busca);
                            fclose(busca);
                            fclose(out);
                            fclose(hash);
                            return;
                        }
                        printf("Chave %s encontrada, endereco %d, %d acesso\n",ISBN,resto,acesso);
                        printf("POS: %d \n",pos);
                        pegar_isbn_pos(pos);
                        aux[0] = '/';
                        fseek(busca,14*contador,SEEK_SET);
                        fwrite(aux,sizeof(aux),1,busca);
                        fclose(busca);
                        fclose(out);
                        fclose(hash);
                        return;
                    }
                }
                resto++;
                acesso++;
            }
            printf("Chave %s nao encontrada\n",aux1);
            aux1[0] = '/';
            fseek(busca,14*contador1,SEEK_SET);
            fwrite(aux1,sizeof(aux),1,busca);
            fclose(busca);
            fclose(out);
            fclose(hash);
            return;
        }
        contador++;
    }

}

void pegar_isbn_pos(int pos){
    FILE *out;
    REG registro;
    out = fopen("main.bin", "r+b");
    if(out ==NULL){
        printf("Algum arquivo essencial nao foi criado\n");
    }
    fseek(out,119*pos,SEEK_SET);
    fread(&registro, sizeof(REG), 1, out);

    printf("ISBN: %s\n",registro.ISBN);
    printf("ano: %s\n",registro.ano);
    printf("autor: %s\n",registro.autor);
    printf("titulo: %s\n",registro.titulo);
    fclose(out);

}