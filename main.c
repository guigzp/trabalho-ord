#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct{
	int byte_o;
	int id;
}indice;

indice indice_primario[55];

indice indice_secundario[18];

indice lista_invertida[55];


// Ordenar o indice primario
void bubble_sort (indice vetor[], int n) {
    int k, j;
    indice aux;
    for (k = 1; k < n; k++) {
        for (j = 0; j < n - 1; j++) {
            if (vetor[j].id > vetor[j + 1].id) {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

int getLine (char *str, FILE *arq){
	int i = 0;
	fgets(str, 20, arq);
	while (str[i] != '\n' && str[i] != '\0') i++;
	str[i] = '|';		// adiciona o pipe
	str[i + 1] = '\0';	// adiciona o fim de string
	return i;
}

// Função para ler um arquivo e o formatar para um arquivo com o tamanho antes de cada registro com os campos separados por pipes
void importar(FILE *arq){
	int i;
	int byte_offset = 0, aux = 0;
	char buffer[100];								// buffer para armazenar os dados do registro
	buffer[0] = '\0';								// inicia ele vazio
	char string[20];								// auxiliar para ler uma linha do arquivo inicial
	FILE *destino = fopen("destino.txt", "w");		// arquivo onde será escrito os registros
	while( !feof(arq) ){
		for(i = 0; i < 4; i++){
			getLine(string, arq);					// le uma linha do arquivo

			if(i == 0){								// pega o byte offset e os valores para o indice primario e a a lista invertida
				indice_primario[aux].id = atoi(string);
				lista_invertida[aux].id = indice_primario[aux].id;
				indice_primario[aux].byte_o = byte_offset;
				aux ++;
			}
			strcat(buffer, string);					// concatena o lido com um buffer para armazenar o registro inteiro

		}
		i = strlen(buffer);							// calcula o tamanho do registro
		byte_offset = byte_offset + i + 4;			// calcula o byte offset do proximo registro
		fwrite(&i, sizeof(int), 1, destino);		// escreve o tamanho do registro
		fwrite(buffer, sizeof(char), i, destino);	// escreve o registro
		buffer[0] = '\0';							// limpa o buffer

	}
	bubble_sort(indice_primario, 55);				// ordena o indice primario por id
	fclose(destino);
}


// Le um registro no offset para o buffer passado
void ler_registro(int offset, char* buffer){
	int tamanho;
	FILE* destino = fopen("destino.txt", "r");
	fseek(destino, offset, SEEK_SET);
	fread(&tamanho, sizeof(int), 1, destino);
	fread(buffer, sizeof(char), tamanho, destino);
	buffer[tamanho] = '\0';
	fclose(destino);
}

// Devolve o id da raça de um registro
int recupera_id(int offset){
	char string [50];
	char* aux;
	ler_registro(offset, string);
	aux = strtok(string, "|");
	aux = strtok(NULL, "|");
	return atoi(aux);
}

// Constroi o Indice Secundario
void constroi_indice_secundario(){
	int i, j;
	for(i = 0; i < 18; i++){
		indice_secundario[i].id = i +1;
		indice_secundario[i].byte_o = -1;
		for(j = 0; j < 55; j ++){
			if( recupera_id(indice_primario[j].byte_o) == i + 1 ){
				indice_secundario[i].byte_o = indice_primario[j].byte_o;
				break;			// encerra o for
			}
		}
	}
}

void escreve_lista_invertida(int id, int byte_o){
	int i;
	for(i = 0; i < 55; i++){
		if( lista_invertida[i].id == id ){
			lista_invertida[i].byte_o = byte_o;
			break;
		}
	}
}

void procura_indice_primario(int idRaca, int byte_o){
	int id_anterior, i,j;
	for(i = 0; i < 55; i++){
		if( indice_primario[i].byte_o == byte_o ){
			id_anterior = indice_primario[i].id;
			break;
		}
	}
	
	for(i+1 ; i < 55; i++){
		if(idRaca == recupera_id(indice_primario[i].byte_o)){
			escreve_lista_invertida(id_anterior, indice_primario[i].byte_o);
			id_anterior = indice_primario[i].id;
		}
	}
	escreve_lista_invertida(id_anterior, -1);
	
}

//Constroi a Lista Invertida
void constroi_lista_invertida(){
    int i, j;
	for(i = 0; i < 18; i++){
       procura_indice_primario(indice_secundario[i].id, indice_secundario[i].byte_o);
	}
}


// Busca um cão a partir de um ID
void busca_cao(int id){
	int i;
	char string [50];
	char* aux;
	int achou = 0;
	for (i = 0; i < 55; i++){
		if (id == indice_primario[i].id){
			ler_registro(indice_primario[i].byte_o, string);
			achou = 1;
			break;
		}
	}
	if(achou == 0){
		printf("Não existe um cão com o ID informado cadastrado!\n");
	}else{
		aux = strtok(string, "|");
		printf("ID: %s\n", aux);
		aux = strtok(NULL, "|");
		printf("ID-Raça: %s\n", aux);
		aux = strtok(NULL, "|");
		printf("Nome: %s\n", aux);
		aux = strtok(NULL, "|");
		printf("Sexo: %s\n", aux);
	}
}

void escreve_indices(){
	FILE* primario = fopen("indicePrimario.txt", "w");
	FILE* secundario = fopen("indiceSecundario.txt", "w");
	int i;
	char delimitador = '|';

	for(i = 0; i < 55; i++){
		fwrite(&indice_primario[i].id, sizeof(int), 1, primario);
		fwrite(&delimitador, sizeof(char), 1, primario);
		fwrite(&indice_primario[i].byte_o, sizeof(int), 1, primario);
		fwrite(&delimitador, sizeof(char), 1, primario);
	}

	for(i = 0; i < 18; i++){
		fwrite(&indice_secundario[i].id, sizeof(int), 1, secundario);
		fwrite(&delimitador, sizeof(char), 1, secundario);
		fwrite(&indice_secundario[i].byte_o, sizeof(int), 1, secundario);
		fwrite(&delimitador, sizeof(char), 1, secundario);
	}

	fclose(primario);
	fclose(secundario);
}

void menu(){
	int opcao = 1;
	int i;
	char nome_arq [50];
	FILE *individuos;
	while(opcao > 0 && opcao < 7){

		printf("Trabalho Cadastro/Busca de Cães\n");
		printf("Opções: \n1)Importar Arquivo \n2)Buscar um cão \n3)Buscar todos os cães de uma raça \n4)Mostrar Indice Primário \n5)Mostrar Indice Secundário \n6)Mostrar lista Invertida \nDigite sua opção: ");
		scanf("%d", &opcao);

		switch(opcao){
			case 1:
				printf("Digite o nome do arquivo para importação: ");
				scanf("%s", nome_arq);
				individuos = fopen(nome_arq, "r");
				if(individuos == NULL){
					printf("Falha na abertura do arquivo fornecido!");
				}else{
					importar(individuos);
					fclose(individuos);
					constroi_indice_secundario();
					constroi_lista_invertida();
				}

				break;

			case 2:
				printf("Digite o ID do cão a ser buscado: ");
				scanf("%d", &i);
				busca_cao(i);
				break;

			case 3:
				printf("Digite o ID da raça a ser buscada: ");
				scanf("%d", &i);
				break;

			case 4:
				for(i = 0; i<55; i++){
					printf("ID: %d \t Byte Offset: %d\n", indice_primario[i].id, indice_primario[i].byte_o);
				}
				break;

			case 5:
				for(i = 0; i < 18; i++){
					printf("ID: %d \t Byte Offset: %d \n", indice_secundario[i].id, indice_secundario[i].byte_o);
				}
				break;
            case 6:
				for(i = 0; i < 55; i++){
					printf("ID-I: %d \t Prox-raça: %d \n", lista_invertida[i].id, lista_invertida[i].byte_o);
				}
				break;

			default:
				printf("Encerrando o programa!\n");
		}
		//system("cls");
	}
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	menu();
	escreve_indices();
}
