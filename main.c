#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(){
	FILE *arq = fopen("individuos.txt", "r");
	if(arq == NULL){
		printf("Erro");
	}
	importar(arq);
	fclose(arq);
}
