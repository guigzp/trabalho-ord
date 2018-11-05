// Trabalho 1 ORD 2018 - Profa. Valeria
// Alunos:
// Guilherme Zamberlam Pomini		RA: 99345
// Alisson Lopes					RA: 99687
// Alan Lopes						RA: 99659



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


// Estrutura para abstrair o Indice Primario, Secundario e a Lista invertida
// 4 bytes para o byte offset e 4 bytes para o id

// A mesma estrutura vai ser utilizada para Indice Primario, Secundario e a Lista invertida
// no caso do Indice Secundario e da Lista invertida, o campo byte_of simboliza o rrn

typedef struct{
	int byte_o;
	int id;
}indice;

// Trabalho comporta at� 250 registros

indice indice_primario[250];

indice indice_secundario[18];

indice lista_invertida[250];

int tamanho = -1;


// Bubble sort para ordenar o indice primario e secund�rio
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

// Le uma linha do arquivo passado e adiciona um pipe no final
int getLine (char *str, FILE *arq){
	int i = 0;
	fgets(str, 20, arq);
	while (str[i] != '\n' && str[i] != '\0') i++;
	str[i] = '|';		// adiciona o pipe
	str[i + 1] = '\0';	// adiciona o fim de string
	return i;
}

// Fun��o para ler um arquivo e o formatar para um arquivo com o tamanho antes de cada registro com os campos separados por pipes
int importar(FILE *arq){
	int i, count = 0, byte_offset = 0, aux = 0;
	char buffer[100];								// buffer para armazenar os dados do registro
	buffer[0] = '\0';								// inicia ele vazio
	char string[20];								// auxiliar para ler uma linha do arquivo inicial
	FILE *individuos = fopen("individuos.txt", "w");		// arquivo onde ser� escrito os registros
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
		fwrite(&i, sizeof(int), 1, individuos);		// escreve o tamanho do registro
		fwrite(buffer, sizeof(char), i, individuos);	// escreve o registro
		buffer[0] = '\0';							// limpa o buffer
		count ++;

	}
	bubble_sort(indice_primario, count);				// ordena o indice primario por id
	fclose(individuos);
	return count;
}


// Le um registro no offset para o buffer passado
void ler_registro(int offset, char* buffer){
	int tamanho;
	FILE* individuos = fopen("individuos.txt", "r");
	fseek(individuos, offset, SEEK_SET);
	fread(&tamanho, sizeof(int), 1, individuos);
	fread(buffer, sizeof(char), tamanho, individuos);
	buffer[tamanho] = '\0';
	fclose(individuos);
}

// Devolve o id do c�o a partir de um offset
int recupera_id(int offset){
	int i;
	if (offset == -1){
		return -1;
	}
	for(i = 0; i < tamanho; i++){
		if(indice_primario[i].byte_o == offset){
			return indice_primario[i].id;
		}
	}
}

// Devolve o id da ra�a de um registro
// Le o registro no offset indicado e retorna somente o id-r em formato de int
int recupera_id_raca(int offset){
	char string [50];
	char* aux;
	ler_registro(offset, string);
	aux = strtok(string, "|");
	aux = strtok(NULL, "|");
	return atoi(aux);
}

// Constroi o Indice Secundario
void constroi_indice_secundario(){
	int i, j, k;
	for(i = 0; i < 18; i++){
		indice_secundario[i].id = i +1;
		indice_secundario[i].byte_o = -1;				// � iniciado com -1 caso no arquivo n�o tenha um c�o com a ra�a, diferente de -1 � por ter no minimo 1
		for(j = 0; j < tamanho; j ++){
			if( recupera_id_raca(indice_primario[j].byte_o) == i + 1 ){		// encontra a primeira ocorrencia da ra�a de acordo com o indice primario
				// procura o RRN da lista invertida que possui o id da primeira ocorrencia da ra�a
				for(k = 0; k < tamanho; k++){
					if(lista_invertida[k].id == indice_primario[j].id){
						indice_secundario[i].byte_o = k;
						break;
					}
				}
				break;			// encerra o for
			}
		}
	}
	bubble_sort(indice_secundario, 18);
}

// retorna o rrn da lista invertida que o id indicado est�
int procura_rrn(int id){
	int i;
	for(i = 0; i < tamanho; i++){
		if(lista_invertida[i].id == id){
			return i;
		}
	}
}

// Percorre o indice primario vendo as proximas ra�as conforme o id passado e o rrn da primeira ocorrencia dessa ra�a fornecido pelo indice secundario
// Para cada indice correspondente a ra�a � feita a escrita na lista invertida seguindo a ordem
void procura_indice_primario(int idRaca, int rrn){
	int i;
	for(i = 0; i < tamanho; i++){							// Encontra o id da primeira ocorrencia da ra�a de acordo com o byte offset do indice secundario
		if( indice_primario[i].id == lista_invertida[rrn].id ){
			break;
		}
	}
	
	for(i+1 ; i < tamanho; i++){								// encontra as proximas ocorrencias da ra�a e vai escrevendo na lista invertida em ordem
		if(idRaca == recupera_id_raca(indice_primario[i].byte_o)){
			lista_invertida[rrn].byte_o = procura_rrn(indice_primario[i].id);
			rrn = procura_rrn(indice_primario[i].id);
		}
	}
	lista_invertida[rrn].byte_o = -1;	 // escreve -1 para indicar que n�o existem mais ocorrencias dessa ra�a a partir desse ponto
	
}

//Constroi a Lista Invertida
void constroi_lista_invertida(){
    int i, j;
	for(i = 0; i < 18; i++){
		if(indice_secundario[i].byte_o != -1){				// verifica se existe pelo menos 1 ocorrencia da ra�a cadastrado
			procura_indice_primario(indice_secundario[i].id, indice_secundario[i].byte_o);
		}
	}
}

// Printa o nome da ra�a de acordo com o id-r passado
void printa_raca(int idRaca){
	if(idRaca == 1){
		printf("Ra�a: BASSET HOUND\n");
	}else if(idRaca == 2){
		printf("Ra�a: BEAGLE\n");
	}else if(idRaca == 3){
		printf("Ra�a: BORDER COLLIE\n");
	}else if(idRaca == 4){
		printf("Ra�a: BOUVIER DE FLANDRES\n");
	}else if(idRaca == 5){
		printf("Ra�a: BOXER\n");
	}else if(idRaca == 6){
		printf("Ra�a: COCKER SPANIEL INGLES\n");
	}else if(idRaca == 7){
		printf("Ra�a: DALMATA\n");
	}else if(idRaca == 8){
		printf("Ra�a: DOBERMANN\n");
	}else if(idRaca == 9){
		printf("Ra�a: FOX TERRIER\n");
	}else if(idRaca == 10){
		printf("Ra�a: GOLDEN RETRIEVER\n");
	}else if(idRaca == 11){
		printf("Ra�a: LABRADOR\n");
	}else if(idRaca == 12){
		printf("Ra�a: PASTOR ALEMAO\n");
	}else if(idRaca == 13){
		printf("Ra�a: POINTER INGLES\n");
	}else if(idRaca == 14){
		printf("Ra�a: ROTTWEILER\n");
	}else if(idRaca == 15){
		printf("Ra�a: SETTER INGLES\n");
	}else if(idRaca == 16){
		printf("Ra�a: SETTER IRLANDES\n");
	}else if(idRaca == 17){
		printf("Ra�a: TERRIER BRASILEIRO\n");
	}else if(idRaca == 18){
		printf("Ra�a: YORKSHIRE TERRIER\n");
	}
}

// Fun��o para printar os dados de um c�o
void printa_cao(char string[]){
	char* aux;
	int raca;
	aux = strtok(string, "|");
	printf ("\n ------------------- \n");
	printf("ID: %s\n", aux);
	aux = strtok(NULL, "|");
	printf("ID-Ra�a: %s\n", aux);
	raca = atoi(aux);
	aux = strtok(NULL, "|");
	printf("Nome: %s\n", aux);
	printa_raca(raca);
	aux = strtok(NULL, "|");
	printf("Sexo: %s\n", aux);
	printf (" ------------------- \n");
}


// Busca um c�o a partir de um ID
void busca_cao(int id){
	int i;
	char string [50];
	int achou = 0;
	for (i = 0; i < tamanho; i++){
		if (id == indice_primario[i].id){
			ler_registro(indice_primario[i].byte_o, string);
			achou = 1;
			break;
		}
	}
	if(achou == 0){
		printf("N�o existe um c�o com o ID informado cadastrado!\n");
	}else{
		printa_cao(string);
	}
}

// Recupera o offset de um ID-I
int recupera_offset(int id){
	int i;
	for(i = 0; i < tamanho; i++){
		if(indice_primario[i].id == id){
			return indice_primario[i].byte_o;
		}
	}
}

// Busca todos os c�es de uma ra�a a partir do id utilizando o indice primario, secundario e a lista invertida
void busca_raca(int id){
	int i, proximo;
	char string[50];
	for(i = 0; i < 18; i++){					// encontra a primeira ocorrencia da ra�a fornecida pelo indice secundario
		if( indice_secundario[i].id == id ){
			proximo = indice_secundario[i].byte_o;
			break;
		}
	}
	if(indice_secundario[i].byte_o != -1){					// verifica se o id-r fornecido est� cadastrado
		ler_registro(recupera_offset(lista_invertida[proximo].id), string);
		printa_cao(string);											// printa o primeiro de acordo com o indice secundario
		proximo = lista_invertida[proximo].byte_o;
		while(proximo != -1){								// printa os c�es na ordem da lista invertida
			ler_registro(recupera_offset(lista_invertida[proximo].id), string);
			printa_cao(string);
			proximo = lista_invertida[proximo].byte_o;
		}
	}else{
		printf("N�o existe um c�o com a ra�a informada cadastrado no sistema!\n");
	}
}

// Escreve em arquivos o Indice Primario, Secundario e a Lista Invertida
void escreve_indices(){
	FILE* primario = fopen("indicePrimario.txt", "w");
	FILE* secundario = fopen("indiceSecundario.txt", "w");
	FILE* invertida = fopen("listaInvertida.txt", "w");
	int i;
	char delimitador = '|';

	// Escrita do indice primario
	for(i = 0; i < tamanho; i++){
		fwrite(&indice_primario[i].id, sizeof(int), 1, primario);
		fwrite(&delimitador, sizeof(char), 1, primario);
		fwrite(&indice_primario[i].byte_o, sizeof(int), 1, primario);
		fwrite(&delimitador, sizeof(char), 1, primario);
	}

	// Escrita do Indice Secundario
	for(i = 0; i < 18; i++){
		if(indice_secundario[i].byte_o != -1){
			fwrite(&indice_secundario[i].id, sizeof(int), 1, secundario);
			fwrite(&delimitador, sizeof(char), 1, secundario);
			fwrite(&indice_secundario[i].byte_o, sizeof(int), 1, secundario);
			fwrite(&delimitador, sizeof(char), 1, secundario);
		}
	}
	
	
	// Escrita da Lista Invertida (poderia ser junto do indice primario mas est� separado para melhor visualiza��o)
	for(i = 0; i < tamanho; i++){
		fwrite(&lista_invertida[i].id, sizeof(int), 1, invertida);
		fwrite(&delimitador, sizeof(char), 1, invertida);
		fwrite(&lista_invertida[i].byte_o, sizeof(int), 1, invertida);
		fwrite(&delimitador, sizeof(char), 1, invertida);
	}

	fclose(primario);
	fclose(secundario);
	fclose(invertida);
}

void printa_menu(){
	system("cls");
	printf("\t Trabalho Cadastro/Busca de C�es\n");
	printf("-------------------------------------------------\n");
	printf("|\t Op��es:                                |\n");
	printf("|                                               |");
	printf("\n|\t 1) Importar Arquivo                    |");
	printf("\n|\t 2) Buscar um c�o                       |");
	printf("\n|\t 3) Buscar todos os c�es de uma ra�a    |");
	printf("\n|\t 4) Mostrar Indice Prim�rio             |");
	printf("\n|\t 5) Mostrar Indice Secund�rio           |");
	printf("\n|\t 6) Mostrar lista Invertida             |");
	printf("\n|\t 0) Encerrar                            |");
	printf("\n-------------------------------------------------");
	printf("\nDigite sua op��o: ");
}

// Menu para as op��es
void menu(){
	int opcao = 1, i;
	char nome_arq [50];
	FILE *individuos;
	while(opcao > 0 && opcao < 7){
		
		printa_menu();
		scanf("%d", &opcao);
		
		switch(opcao){
			case 1:
				system("cls");
				printf("Digite o nome do arquivo para importa��o: ");
				scanf("%s", nome_arq);
				individuos = fopen(nome_arq, "r");
				if(individuos == NULL){
					printf("Falha na abertura do arquivo fornecido!");
				}else{
					tamanho = importar(individuos);		// tamanho recebe a quantidade de registros
					fclose(individuos);
					constroi_indice_secundario();
					constroi_lista_invertida();
					printf("Arquivo Importado com sucesso!\n");
					system("PAUSE");
				}

				break;

			case 2:
				system("cls");
				if(tamanho == -1){
					printf("Necess�rio importar um arquivo primeiro!\n");
				}else{
					printf("Digite o ID do c�o a ser buscado: ");
					scanf("%d", &i);
					busca_cao(i);
				}
				system("PAUSE");
				break;

			case 3:
				system("cls");
				if(tamanho == -1){
					printf("Necess�rio importar um arquivo primeiro!\n");
				}else{
					printf("Digite o ID da ra�a a ser buscada: ");
					scanf("%d", &i);
					busca_raca(i);
				}
				system("PAUSE");
				break;

			case 4:
				system("cls");
				if(tamanho == -1){
					printf("Necess�rio importar um arquivo primeiro!\n");
				}else{
					printf("Indice Primario: \n");
					for(i = 0; i < tamanho; i++){
						printf("ID-I: %d \t Byte Offset: %d\n", indice_primario[i].id, indice_primario[i].byte_o);
					}
				}
				system("PAUSE");
				break;

			case 5:
				system("cls");
				if(tamanho == -1){
					printf("Necess�rio importar um arquivo primeiro!\n");
				}else{
					printf("Indice Secundario: \n");
					for(i = 0; i < 18; i++){
						if(indice_secundario[i].byte_o != -1)
						printf("ID-R: %d \t RRN: %d \n", indice_secundario[i].id, indice_secundario[i].byte_o);
					}
				}
				system("PAUSE");
				break;
				
            case 6:
            	system("cls");
            	if(tamanho == -1){
            		printf("Necess�rio importar um arquivo primeiro!\n");
				}else{
					printf("Lista Invertida: \n");
					for(i = 0; i < tamanho; i++){
						printf("RRN: %d \t ID-I: %d \t RRN Prox-ra�a: %d \n", i, lista_invertida[i].id, lista_invertida[i].byte_o);
					}
				}
				system("PAUSE");
				break;

			default:
				printf("Encerrando o programa!\n");
		}
	}
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	menu();
	escreve_indices();
}
