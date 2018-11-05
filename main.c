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

// Trabalho comporta até 250 registros

indice indice_primario[250];

indice indice_secundario[18];

indice lista_invertida[250];

int tamanho = -1;


// Bubble sort para ordenar o indice primario e secundário
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

// Função para ler um arquivo e o formatar para um arquivo com o tamanho antes de cada registro com os campos separados por pipes
int importar(FILE *arq){
	int i, count = 0, byte_offset = 0, aux = 0;
	char buffer[100];								// buffer para armazenar os dados do registro
	buffer[0] = '\0';								// inicia ele vazio
	char string[20];								// auxiliar para ler uma linha do arquivo inicial
	FILE *individuos = fopen("individuos.txt", "w");		// arquivo onde será escrito os registros
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

// Devolve o id do cão a partir de um offset
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

// Devolve o id da raça de um registro
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
		indice_secundario[i].byte_o = -1;				// é iniciado com -1 caso no arquivo não tenha um cão com a raça, diferente de -1 é por ter no minimo 1
		for(j = 0; j < tamanho; j ++){
			if( recupera_id_raca(indice_primario[j].byte_o) == i + 1 ){		// encontra a primeira ocorrencia da raça de acordo com o indice primario
				// procura o RRN da lista invertida que possui o id da primeira ocorrencia da raça
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

// retorna o rrn da lista invertida que o id indicado está
int procura_rrn(int id){
	int i;
	for(i = 0; i < tamanho; i++){
		if(lista_invertida[i].id == id){
			return i;
		}
	}
}

// Percorre o indice primario vendo as proximas raças conforme o id passado e o rrn da primeira ocorrencia dessa raça fornecido pelo indice secundario
// Para cada indice correspondente a raça é feita a escrita na lista invertida seguindo a ordem
void procura_indice_primario(int idRaca, int rrn){
	int i;
	for(i = 0; i < tamanho; i++){							// Encontra o id da primeira ocorrencia da raça de acordo com o byte offset do indice secundario
		if( indice_primario[i].id == lista_invertida[rrn].id ){
			break;
		}
	}
	
	for(i+1 ; i < tamanho; i++){								// encontra as proximas ocorrencias da raça e vai escrevendo na lista invertida em ordem
		if(idRaca == recupera_id_raca(indice_primario[i].byte_o)){
			lista_invertida[rrn].byte_o = procura_rrn(indice_primario[i].id);
			rrn = procura_rrn(indice_primario[i].id);
		}
	}
	lista_invertida[rrn].byte_o = -1;	 // escreve -1 para indicar que não existem mais ocorrencias dessa raça a partir desse ponto
	
}

//Constroi a Lista Invertida
void constroi_lista_invertida(){
    int i, j;
	for(i = 0; i < 18; i++){
		if(indice_secundario[i].byte_o != -1){				// verifica se existe pelo menos 1 ocorrencia da raça cadastrado
			procura_indice_primario(indice_secundario[i].id, indice_secundario[i].byte_o);
		}
	}
}

// Printa o nome da raça de acordo com o id-r passado
void printa_raca(int idRaca){
	if(idRaca == 1){
		printf("Raça: BASSET HOUND\n");
	}else if(idRaca == 2){
		printf("Raça: BEAGLE\n");
	}else if(idRaca == 3){
		printf("Raça: BORDER COLLIE\n");
	}else if(idRaca == 4){
		printf("Raça: BOUVIER DE FLANDRES\n");
	}else if(idRaca == 5){
		printf("Raça: BOXER\n");
	}else if(idRaca == 6){
		printf("Raça: COCKER SPANIEL INGLES\n");
	}else if(idRaca == 7){
		printf("Raça: DALMATA\n");
	}else if(idRaca == 8){
		printf("Raça: DOBERMANN\n");
	}else if(idRaca == 9){
		printf("Raça: FOX TERRIER\n");
	}else if(idRaca == 10){
		printf("Raça: GOLDEN RETRIEVER\n");
	}else if(idRaca == 11){
		printf("Raça: LABRADOR\n");
	}else if(idRaca == 12){
		printf("Raça: PASTOR ALEMAO\n");
	}else if(idRaca == 13){
		printf("Raça: POINTER INGLES\n");
	}else if(idRaca == 14){
		printf("Raça: ROTTWEILER\n");
	}else if(idRaca == 15){
		printf("Raça: SETTER INGLES\n");
	}else if(idRaca == 16){
		printf("Raça: SETTER IRLANDES\n");
	}else if(idRaca == 17){
		printf("Raça: TERRIER BRASILEIRO\n");
	}else if(idRaca == 18){
		printf("Raça: YORKSHIRE TERRIER\n");
	}
}

// Função para printar os dados de um cão
void printa_cao(char string[]){
	char* aux;
	int raca;
	aux = strtok(string, "|");
	printf ("\n ------------------- \n");
	printf("ID: %s\n", aux);
	aux = strtok(NULL, "|");
	printf("ID-Raça: %s\n", aux);
	raca = atoi(aux);
	aux = strtok(NULL, "|");
	printf("Nome: %s\n", aux);
	printa_raca(raca);
	aux = strtok(NULL, "|");
	printf("Sexo: %s\n", aux);
	printf (" ------------------- \n");
}


// Busca um cão a partir de um ID
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
		printf("Não existe um cão com o ID informado cadastrado!\n");
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

// Busca todos os cães de uma raça a partir do id utilizando o indice primario, secundario e a lista invertida
void busca_raca(int id){
	int i, proximo;
	char string[50];
	for(i = 0; i < 18; i++){					// encontra a primeira ocorrencia da raça fornecida pelo indice secundario
		if( indice_secundario[i].id == id ){
			proximo = indice_secundario[i].byte_o;
			break;
		}
	}
	if(indice_secundario[i].byte_o != -1){					// verifica se o id-r fornecido está cadastrado
		ler_registro(recupera_offset(lista_invertida[proximo].id), string);
		printa_cao(string);											// printa o primeiro de acordo com o indice secundario
		proximo = lista_invertida[proximo].byte_o;
		while(proximo != -1){								// printa os cães na ordem da lista invertida
			ler_registro(recupera_offset(lista_invertida[proximo].id), string);
			printa_cao(string);
			proximo = lista_invertida[proximo].byte_o;
		}
	}else{
		printf("Não existe um cão com a raça informada cadastrado no sistema!\n");
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
	
	
	// Escrita da Lista Invertida (poderia ser junto do indice primario mas está separado para melhor visualização)
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
	printf("\t Trabalho Cadastro/Busca de Cães\n");
	printf("-------------------------------------------------\n");
	printf("|\t Opções:                                |\n");
	printf("|                                               |");
	printf("\n|\t 1) Importar Arquivo                    |");
	printf("\n|\t 2) Buscar um cão                       |");
	printf("\n|\t 3) Buscar todos os cães de uma raça    |");
	printf("\n|\t 4) Mostrar Indice Primário             |");
	printf("\n|\t 5) Mostrar Indice Secundário           |");
	printf("\n|\t 6) Mostrar lista Invertida             |");
	printf("\n|\t 0) Encerrar                            |");
	printf("\n-------------------------------------------------");
	printf("\nDigite sua opção: ");
}

// Menu para as opções
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
				printf("Digite o nome do arquivo para importação: ");
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
					printf("Necessário importar um arquivo primeiro!\n");
				}else{
					printf("Digite o ID do cão a ser buscado: ");
					scanf("%d", &i);
					busca_cao(i);
				}
				system("PAUSE");
				break;

			case 3:
				system("cls");
				if(tamanho == -1){
					printf("Necessário importar um arquivo primeiro!\n");
				}else{
					printf("Digite o ID da raça a ser buscada: ");
					scanf("%d", &i);
					busca_raca(i);
				}
				system("PAUSE");
				break;

			case 4:
				system("cls");
				if(tamanho == -1){
					printf("Necessário importar um arquivo primeiro!\n");
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
					printf("Necessário importar um arquivo primeiro!\n");
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
            		printf("Necessário importar um arquivo primeiro!\n");
				}else{
					printf("Lista Invertida: \n");
					for(i = 0; i < tamanho; i++){
						printf("RRN: %d \t ID-I: %d \t RRN Prox-raça: %d \n", i, lista_invertida[i].id, lista_invertida[i].byte_o);
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
