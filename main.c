#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct{
	int byte_o;
	int id;
}indice;

indice indice_primario[55];

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
			getLine(string, arq);	// le uma linha do arquivo
			
			if(i == 0){			// pega o byte offset e os valores para o indice primario
				indice_primario[aux].id = atoi(string);
				indice_primario[aux].byte_o = byte_offset;
				aux ++;
			}
			strcat(buffer, string);			// concatena o lido com um buffer para armazenar o registro inteiro
			
		}
		i = strlen(buffer);		// calcula o tamanho do registro
		byte_offset = byte_offset + i + 4;		// calcula o byte offset do proximo registro
		fwrite(&i, sizeof(int), 1, destino);		// escreve o tamanho do registro 
		fwrite(buffer, sizeof(char), i, destino);		// escreve o registro
		buffer[0] = '\0';							// limpa o buffer
									
	}
	fclose(destino);
}

void menu(){
	int opcao = 1;
	char nome_arq [50];
	FILE *individuos;
	while(opcao > 0 && opcao < 4){

		printf("Trabalho Cadastro/Busca de Cães\n");
		printf("Opções: \n1)Importar Arquivo \n2)Buscar um cão \n3)Buscar todos os cães de uma raça \nDigite sua opção: ");
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
				}
				break;
			
			case 2:
				// buscar cão
				break;
				
			case 3:
				// buscar por raça
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
	int i;
	for(i = 0; i<55; i++){
		printf("ID: %d \t Byte Offset: %d\n", indice_primario[i].id, indice_primario[i].byte_o);
	}
}
