/*
 * File:   main.cpp
 * Desenvolvido e testado em sistema Linux
 *
 */
#include <map>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// remove pontuacao de uma palavra
void removePontuacao (char *palavra) {
    int length = strlen(palavra);
    if (
        (palavra[length-1] == '.') || (palavra[length-1] == ',') || (palavra[length-1] == ';') ||
        (palavra[length-1] == ':') || (palavra[length-1] == '?') || (palavra[length-1] == '!')
       )
        palavra[length-1] = '\0';
}

void removeEspacos(char * palavra){
    char * temp = new char[strlen(palavra)];
    int j=0;

    for (j=0; j<strlen(palavra); j++){
        for(int i=0; i<strlen(palavra); i++){
            if(palavra[i] == ' ') continue;
            temp[j] = palavra[i];
        }
    }
    temp[j] = '\0';
    palavra = temp;
}

// imprime linha do arquivo com base no offset da palavra
void imprimeLinha(int offset,FILE *f) {
    int pos = ftell(f);
    char linha[2048];
    while (pos < offset) {
        fgets(linha,2047,f);
        pos = ftell(f);
    }
    printf("%s",linha);
}

// retorna a palavra formada pelos "tamanho" primeiros caracteres da "linha" obtida do arquivo
char * obterPalavra(char *linha, int tamanho){
    if(strlen(linha) <= tamanho) return linha;
    char *palavra = (char *) malloc(sizeof(char) * tamanho);
    for(int i=0; i<tamanho; i++){
        palavra[i] = linha[i];
    }
    palavra[tamanho] = '\0';
    return palavra;
}

char * extractStringBetweenTags(char * stringToSearch, char * startTag, char * endTag){
    bool inserir = false;
    int count = 0;
    char * retorno = (char *) malloc(sizeof(char)*strlen(stringToSearch));

    for (int i=0; i<strlen(stringToSearch); i++){
        if(stringToSearch[i] == startTag[0]){
            inserir = true;
            continue;
        }
        if(stringToSearch[i] == endTag[0]){
            inserir = false;
            continue;
        }
        if(inserir){
            retorno[count++] = stringToSearch[i];
        }
    }
    retorno[count] = '\0';
    return retorno;
}

int * separarVirgulas(char * offset){
    char * str = offset;

    char * pch;
    printf ("Splitting string \"%s\" into tokens:\n",str);
    pch = strtok (str,",");
    vector<int> numbers;

    while (pch != NULL){
        printf ("%s\n",pch);
        numbers.push_back(stoi(pch));
        pch = strtok (NULL, ",");
    }
    int * ret;
    std::copy(numbers.begin(), numbers.end(), ret);
    // int * ret = &numbers[0];
    return ret;
}

int * obterOffsets(char * linha){
    char * offset = extractStringBetweenTags(linha, "{", "}");
    std::cout << "offset: " << offset << "\n";
    return separarVirgulas(offset);
}

// classe que implementa a lista invertida
class listaInvertida {
public:
    // construtor
    listaInvertida() {
        this->lista = fopen("lista_invertida.txt", "w+");
     }
    // destrutor
    ~listaInvertida() {
        fclose(this->lista);
    }

    map<string, vector<int>> listaTemp;

    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) { 
        string palavraString(palavra);
        vector<int> vetor = listaTemp[palavraString];
        vetor.push_back(offset);
        listaTemp[palavraString] = vetor;
    }

    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        
        // voltar ao inicio do arquivo lista
        fseek(this->lista, 0, SEEK_SET);
        char linha[2048];
        bool palavraTavaSalva = false;
        
        // verificar se a palavra existe no arquivo lista
        while(! feof(this->lista)){
            fgets(linha, 2047, this->lista);
            char* word = obterPalavra(linha, strlen(palavra));
            if(strcmp(word, palavra) == 0){
                palavraTavaSalva = true;
                break;
            }
        }

        // substituir pelo resultado da busca na lista invertida
        quantidade[0] = 0;
        int i = 0;
        int *offsets = new int[1];

        // palavra nao encontrada
        if( ! palavraTavaSalva){
            return offsets;
        }

        offsets = obterOffsets(linha);
        quantidade[0] = sizeof(offsets)/sizeof(offsets[0]);

12 e perguntou-lhe: Amigo, como entraste aqui, sem teres veste nupcial? Ele, por�m, emudeceu.

        // exemplo: retornar os primeiros 10 offsets da palavra "terra"
        // offsets[i++] = 58;
        // offsets[i++] = 69;
        // offsets[i++] = 846;
        // offsets[i++] = 943;
        // offsets[i++] = 1083;
        // offsets[i++] = 1109;
        // offsets[i++] = 1569;
        // offsets[i++] = 1792;
        // offsets[i++] = 2041;
        // offsets[i++] = 2431;
        return offsets;
    }
private:
    FILE *lista;
};

// programa principal
int main(int argc, char** argv) {
    char *str = "Breno";
    string strCpp(str);
    map<string, vector<int>> listaInvertida; 
    listaInvertida[strCpp] = {1, 2, 3};

    // abrir arquivo
    ifstream in("biblia.txt");
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n");
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;
        listaInvertida lista;
        // ler palavras
        while (!in.eof()) {
            // ler palavra
            in >> palavra;
            // pegar offset
            offset = in.tellg();
            // remover pontuacao
            removePontuacao(palavra);
            // desconsiderar palavras que sao marcadores do arquivo
            if (!((palavra[0] == '#') || (palavra[0] == '[') || ((palavra[0] >= '0') && (palavra[0] <= '9')))) {
                //printf("%d %s\n", offset,palavra); fflush(stdout); // debug :-)
                lista.adiciona(palavra, offset);
                contadorDePalavras++;
                if (contadorDePalavras % 1000 == 0) { printf(".");  fflush(stdout); }
            }
        }
        in.close();

        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
                // busca na lista invertida
                int *offsets = lista.busca(palavra,&quantidade);
                printf("qtd: %d", quantidade);
                // com vetor de offsets, recuperar as linhas que contem a palavra desejada
                if (quantidade > 0) {
                    FILE *f = fopen("biblia.txt","rt");
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i],f);
                    fclose(f);
                }
                else
                    printf("nao encontrou %s\n",palavra);
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}



