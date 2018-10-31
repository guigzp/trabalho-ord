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

// Fun��o para ler um arquivo e o formatar para um arquivo com o tamanho antes de cada registro com os campos separados por pipes
void importar(FILE *arq){
	int i;
	int byte_offset = 0, aux = 0;
	char buffer[100];								// buffer para armazenar os dados do registro
	buffer[0] = '\0';								// inicia ele vazio	
	char string[20];								// auxiliar para ler uma linha do arquivo inicial
	FILE *destino = fopen("destino.txt", "w");		// arquivo onde ser� escrito os registros
	while( !feof(arq) ){
		for(i = 0; i < 4; i++){
			getLine(string, arq);					// le uma linha do arquivo
			
			if(i == 0){								// pega o byte offset e os valores para o indice primario
				indice_primario[aux].id = atoi(string);
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
	bubble_sort(indice_primario, 55);
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

// Devolve o id da ra�a de um registro
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

void busca_cao(int id){
	int i;
	for (i = 0; i < 55; i++){
		if (id == indice_primario[i].id){
			
		}
	}
}

void menu(){
	int opcao = 1;
	int i;
	char nome_arq [50];
	FILE *individuos;
	while(opcao > 0 && opcao < 6){

		printf("Trabalho Cadastro/Busca de C�es\n");
		printf("Op��es: \n1)Importar Arquivo \n2)Buscar um c�o \n3)Buscar todos os c�es de uma ra�a \n4)Mostrar Indice Prim�rio \n5)Mostrar Indice Secund�rio \nDigite sua op��o: ");
		scanf("%d", &opcao);

		switch(opcao){
			case 1:
				printf("Digite o nome do arquivo para importa��o: ");
				scanf("%s", nome_arq);
				individuos = fopen(nome_arq, "r");
				if(individuos == NULL){
					printf("Falha na abertura do arquivo fornecido!");
				}else{
					importar(individuos);
					fclose(individuos);
					constroi_indice_secundario();
				}
				
				break;
			
			case 2:
				// buscar c�o
				break;
				
			case 3:
				// buscar por ra�a
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
				
			default:
				printf("Encerrando o programa!\n");
		}
		//system("cls");
	}
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	menu();
}
