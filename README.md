# Implementação de Lista Invertida com Índice Primário e Secundário em C

O presente trabalho faz parte da disciplina de Organização e Recuperação de Dados na Universidade Estadual de Maringá no segundo semestre de 2018.

## Autor

* **Guilherme Zamberlam Pomini** - Contato: guizamberlam@hotmail.com

## Descrição do Trabalho

Este trabalho visa implementar os conceitos de Lista Invertida, Índice Primário e Índice Secundário para manutenção de registros em arquivos. Tais conceitos foram aplicados em um sistema de cadastro e busca de cães, permitindo a importação de arquivos e busca.

## Funcionalidades

* Importação de um arquivo com ID, ID-raca, Nome e Sexo do cão com 1 campo por linha. Suporta até 250 registros.

* Transformação do arquivo de entrada em um arquivo de registros de tamanho variável com identificador de tamanho e campos separados por pipes

* Construção de um Índice Primário tendo como chave o ID do cão

* Construção de um Índice Secundário tendo como chave o ID-Raça

* Construção de uma Lista Invertida de ID-Raça

* Busca de um cão a partir de um ID

* Busca de todos os cães de uma raça a partir de um ID-Raça

## Programas Utilizados

IDE Dev C++

## Como Rodar

Compilar o arquivo main.c, executar e fornecer um arquivo de entrada no formato esperado.


