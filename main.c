#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

int getline (char *str, FILE *arq){
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
	char buffer[100];
	buffer[0] = '\0';
	char string[20];
	FILE *destino = fopen("destino.txt", "w");
	while( !feof(arq) ){
		for(i = 0; i < 4; i++){
			getline(string, arq);
			strcat(buffer, string);
		}
		i = strlen(buffer);
		fwrite(&i, sizeof(int), 1, destino);
		fwrite(buffer, sizeof(char), i, destino);
		buffer[0] = '\0';
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
			
			case 2:
				// buscar cão
				
			case 3:
				// buscar por raça
			
			default:
				printf("Encerrando o programa!");
		}
		system("cls");
	}
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	menu();
}
