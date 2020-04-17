/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização e Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 743542
 * Aluno: Giulia Silva Fazzi
 * ========================================================================== */

/*
	Para o desenvolvimento desse trabalho, contou-se com ajuda do aluno
	Luiz Felipe Guimarães, a fim de discutir soluções lógicas
	e especificidades da linguagem C. Ainda, o monitor Vitor Novaes colaborou
	bastante para a correção de erros e otimização do código.
	
	Referência usada para os percursos de listagem das árvores:
	https://www.geeksforgeeks.org/tree-traversals-inorder-preorder-and-postorder/
*/

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 31
#define TAM_GENERO 2
#define TAM_NASCIMENTO 11
#define TAM_CELULAR 16
#define TAM_VEICULO 31
#define TAM_PLACA 9
#define TAM_DATA 9
#define TAM_HORA 6
#define TAM_TRAJETO 121
#define TAM_VALOR 7
#define TAM_VAGAS 2
#define TAM_STRING_INDICE (30 + 6 + 4 + 1)

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS "Busca por %s.\nNos percorridos:\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
	char celular[TAM_CELULAR];
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_ip *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_is *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de Caronas  */
void criar_iride(Indice *iride);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, int ip);

/* Aloca dinamicamente os vetores de chaves e descendentes */
void *criar_no(char ip);

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho);

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada. */
char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho);

/* Realiza percurso em-ordem imprimindo todas as caronas na ordem lexicografica do destino e data/hora e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_is(int noderrn, int nivel);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

char *gerarString(Carona *novo);

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn);
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn);

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_ip(int noderrn, int nivel);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *iride);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice iride);

void listar(Indice iprimary, Indice iride);

/*******************************************************/

void libera_no(void *node, char ip);

/*Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j);

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip chave);
void insere_chave_is(Indice *iride, Chave_is chave);

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
int insere_aux_ip(int noderrn, Chave_ip *chave);
int insere_aux_is(int noderrn, Chave_is *chave);

/* VOCÊS NÃO NECESSARIAMENTE PRECISAM USAR TODAS ESSAS FUNÇÕES, É MAIS PARA TEREM UMA BASE MESMO, 
 * PODEM CRIAR SUAS PRÓPRIAS FUNÇÕES SE PREFERIREM */

int main()
{
	char *p; /* # */
			 /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 10);

	/* Índice primário */
	Indice iprimary;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Caronas */
	Indice iride;
	criar_iride(&iride);

	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1: /* Cadastrar uma nova Carona */
			cadastrar(&iprimary, &iride);
			break;
		case 2: /* Alterar a qtd de vagas de uma Carona */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar uma Carona */
			printf(INICIO_BUSCA);
			buscar(iprimary, iride);
			break;
		case 4: /* Listar todos as Caronas */
			printf(INICIO_LISTAGEM);
			listar(iprimary, iride);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if (!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
				{
					fwrite(p, 1, tamanho_registro_ip, stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if (!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
				{
					fwrite(p, 1, tamanho_registro_is, stdout);
					puts("");
				}
			//printf("%s\n", ARQUIVO_IS);
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
	if (rrn < 0)
		return 0;

	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.genero);
	printf("%s\n", j.nascimento);
	printf("%s\n", j.celular);
	printf("%s\n", j.veiculo);
	printf("%s\n", j.placa);
	printf("%s\n", j.data);
	printf("%s\n", j.hora);
	printf("%s\n", j.valor);
	printf("%s\n", j.vagas);

	strcpy(trajeto, j.trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL)
	{
		printf("%s", traj);
		traj = strtok(NULL, "|");
		if (traj != NULL)
		{
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn)
{
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[256] = '\0';
	Carona j;

	p = strtok(temp, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.genero, p);
	p = strtok(NULL, "@");
	strcpy(j.nascimento, p);
	p = strtok(NULL, "@");
	strcpy(j.celular, p);
	p = strtok(NULL, "@");
	strcpy(j.veiculo, p);
	p = strtok(NULL, "@");
	strcpy(j.placa, p);
	p = strtok(NULL, "@");
	strcpy(j.trajeto, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.hora, p);
	p = strtok(NULL, "@");
	strcpy(j.valor, p);
	p = strtok(NULL, "@");
	strcpy(j.vagas, p);

	gerarChave(&j);

	return j;
}

/* Gera chave primária */
void gerarChave(Carona *novo)
{
	char aux[TAM_PRIMARY_KEY];
	aux[0] = toupper(novo->nome[0]);  // primeira letra do nome
	aux[1] = toupper(novo->placa[0]); // primeira letra da placa
	aux[2] = toupper(novo->placa[1]); // segunda letra da placa
	aux[3] = toupper(novo->placa[2]); // terceira letra da placa
	aux[4] = toupper(novo->data[0]);  // primeiro dígito dia
	aux[5] = toupper(novo->data[1]);  // segundo dígito dia
	aux[6] = toupper(novo->data[3]);  // primeiro dígito mês
	aux[7] = toupper(novo->data[4]);  // segundo dígito mês
	aux[8] = toupper(novo->hora[0]);  // primeiro dígito hora
	aux[9] = toupper(novo->hora[1]);  // segundo dígito hora
	aux[10] = '\0';

	strcpy(novo->pk, aux);
}

char *gerarString(Carona *novo)
{
	char data[TAM_DATA - 2];
	char hora[TAM_HORA - 1];

	// data
	data[0] = novo->data[6];
	data[1] = novo->data[7];
	data[2] = novo->data[3];
	data[3] = novo->data[4];
	data[4] = novo->data[0];
	data[5] = novo->data[1];
	data[6] = '\0';

	// hora
	hora[0] = novo->hora[0];
	hora[1] = novo->hora[1];
	hora[2] = novo->hora[3];
	hora[3] = novo->hora[4];
	hora[4] = '\0';

	char *aux, trj[TAM_TRAJETO];
	// primeiro trajeto
	aux = strtok(novo->trajeto, "|");

	while (aux)
	{
		strcpy(trj, aux);
		aux = strtok(NULL, "|");
	}

	char *rtrn = (char *)malloc(TAM_STRING_INDICE);
	sprintf(rtrn, "%s$%s%s", trj, data, hora);

	return rtrn;
}

/* Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo)
{
	scanf("%[^\n]%*c", novo->nome);
	scanf("%[^\n]%*c", novo->genero);
	scanf("%[^\n]%*c", novo->nascimento);
	scanf("%[^\n]%*c", novo->celular);
	scanf("%[^\n]%*c", novo->veiculo);
	scanf("%[^\n]%*c", novo->placa);
	scanf("%[^\n]%*c", novo->trajeto);
	scanf("%[^\n]%*c", novo->data);
	scanf("%[^\n]%*c", novo->hora);
	scanf("%[^\n]%*c", novo->valor);
	scanf("%[^\n]%*c", novo->vagas);

	sprintf(registro, "%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@", novo->nome, novo->genero, novo->nascimento, novo->celular, novo->veiculo, novo->placa, novo->trajeto, novo->data, novo->hora, novo->valor, novo->vagas);
}

/* ==========================================================================
 * ================================ ÍNDICES =================================
 * ========================================================================== */

/* Aloca dinamicamente os vetores de chaves e descendentes */
void *criar_no(char ip)
{
	// índice primário
	if (ip == 'p')
	{
		node_Btree_ip *no = (node_Btree_ip *)malloc(sizeof(node_Btree_ip));

		// no máximo, m - 1 registros
		no->chave = (Chave_ip *)malloc(sizeof(Chave_ip) * (ordem_ip - 1));

		// no máximo, m descendentes
		no->desc = (int *)malloc(sizeof(int) * ordem_ip);

		no->num_chaves = 0;

		// inicializar descendentes como null
		for (int i = 0; i < ordem_ip; i++)
		{
			no->desc[i] = -1;
		}

		return no;
	}

	// índice secundário
	else
	{
		node_Btree_is *no = (node_Btree_is *)malloc(sizeof(node_Btree_is));

		// no máximo, m - 1 registros
		no->chave = (Chave_is *)malloc(sizeof(Chave_is) * (ordem_is - 1));

		// no máximo, m descendentes
		no->desc = (int *)malloc(sizeof(int) * ordem_is);

		// inicializar descendentes como null
		for (int i = 0; i < ordem_is; i++)
		{
			no->desc[i] = -1;
		}

		return no;
	}
}

void libera_no(void *node, char ip)
{
	if (ip == 'p')
	{
		// converte void node para node_btree_ip
		node_Btree_ip *no = (node_Btree_ip *)node;

		free(no->chave);
		free(no->desc);

		free(no);
	}

	else
	{
		// converte void node para node_btree_is
		node_Btree_is *no = (node_Btree_is *)node;

		free(no->chave);
		free(no->desc);

		//no->chave = NULL;
		//no->desc = NULL;

		free(no);
		//no = NULL;
	}
}

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip)
{
	// índice primário
	if (ip == 'p')
	{
		node_Btree_ip *aux = (node_Btree_ip *)salvar;
		char registro[tamanho_registro_ip];
		registro[0] = '\0';

		// 3 bytes quantidade de chaves
		char str[tamanho_registro_ip];
		sprintf(str, "%03d", aux->num_chaves);
		strcat(registro, str);

		for (int i = 0; i < aux->num_chaves; i++)
		{
			// 10 bytes pk
			sprintf(str, "%s", aux->chave[i].pk);
			strcat(registro, str);

			// 4 bytes rrn
			sprintf(str, "%04d", aux->chave[i].rrn);
			strcat(registro, str);
		}

		// completar com #
		while (strlen(registro) < (((ordem_ip - 1) * 14) + 3))
		{
			strcat(registro, "##############");
		}

		// 1 byte folha
		sprintf(str, "%c", aux->folha);
		strcat(registro, str);

		for (int y = 0; y < ordem_ip; y++)
		{
			// descendente nulo
			if (aux->desc[y] != -1 && y < aux->num_chaves + 1)
			{
				sprintf(str, "%03d", aux->desc[y]);
				strcat(registro, str);
			}
			else
			{
				strcat(registro, "***");
			}
		}

		registro[tamanho_registro_ip] = '\0';
		strncpy(ARQUIVO_IP + tamanho_registro_ip * rrn, registro, tamanho_registro_ip);
	}

	// índice secundário
	else
	{
		node_Btree_is *aux = (node_Btree_is *)salvar;
		char registro[tamanho_registro_is];
		registro[0] = '\0';

		// 3 bytes quantidade de chaves
		char str[tamanho_registro_is];
		sprintf(str, "%03d", aux->num_chaves);
		strcat(registro, str);

		for (int i = 0; i < aux->num_chaves; i++)
		{
			// 10 bytes pk
			sprintf(str, "%s", aux->chave[i].pk);
			strcat(registro, str);

			// 41 bytes string
			sprintf(str, "%s", aux->chave[i].string);
			//sscanf(str, "%[^#]", aux->chave[i].string);

			// completar com #
			while (strlen(str) < 41)
			{
				strcat(str, "#");
			}
			strcat(registro, str);
		}

		// completar com #
		while (strlen(registro) < (((ordem_is - 1) * 51) + 3))
		{
			strcat(registro, "#");
		}

		// 1 byte folha
		sprintf(str, "%c", aux->folha);
		strcat(registro, str);

		for (int i = 0; i < ordem_is; i++)
		{
			// descendente nulo
			if (aux->desc[i] != -1 && i < aux->num_chaves + 1)
			{
				sprintf(str, "%03d", aux->desc[i]);
				strcat(registro, str);
			}
			else
			{
				strcat(registro, "***");
			}
		}

		registro[tamanho_registro_is] = '\0';
		strncpy(ARQUIVO_IS + tamanho_registro_is * rrn, registro, tamanho_registro_is);
	}
}

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, int ip)
{
	// índice primário
	if (ip)
	{
		//char temp[tamanho_registro_ip];
		char *temp = (char *)malloc(tamanho_registro_ip + 1);
		char n[tamanho_registro_ip];
		n[0] = '\0';

		// pegar registro no arquivo do índice primário
		strncpy(temp, ARQUIVO_IP + rrn * tamanho_registro_ip, tamanho_registro_ip);
		temp[tamanho_registro_ip] = '\0';

		// criar nó auxiliar
		node_Btree_ip *aux = (node_Btree_ip *)criar_no('p');

		char chvs[4], rrn[5], pk[TAM_PRIMARY_KEY];

		// número de chaves
		strncpy(chvs, temp, 3);
		chvs[3] = '\0';
		aux->num_chaves = atoi(chvs);

		// percorrer temp
		temp += 3;

		for (int x = 0; x < aux->num_chaves; x++)
		{
			// pk
			strncpy(pk, temp, 10);
			pk[10] = '\0';
			strncpy(aux->chave[x].pk, pk, 10);
			aux->chave[x].pk[10] = '\0';

			temp += 10;

			// rrn
			strncpy(rrn, temp, 4);
			rrn[4] = '\0';
			aux->chave[x].rrn = atoi(rrn);

			temp += 4;
		}

		// percorrer temp para chaves não usadas
		temp += (ordem_ip - 1 - aux->num_chaves) * 14;

		// folha
		aux->folha = temp[0];
		temp += 1;

		for (int i = 0; i < ordem_ip; i++)
		{
			if (temp[0] != '*')
			{
				// descendentes
				strncpy(n, temp, 3);
				n[3] = '\0';
				aux->desc[i] = atoi(n);
			}

			else
			{
				aux->desc[i] = -1;
			}

			temp += 3;
		}

		return aux;
	}

	// índice secundário
	else
	{
		//char temp[tamanho_registro_ip];
		char *temp = (char *)malloc(tamanho_registro_is + 1);
		char n[tamanho_registro_is];
		n[0] = '\0';

		// pegar registro no arquivo do índice primário
		strncpy(temp, ARQUIVO_IS + rrn * tamanho_registro_is, tamanho_registro_is);
		temp[tamanho_registro_is] = '\0';

		// criar nó auxiliar
		node_Btree_is *aux = (node_Btree_is *)criar_no('s');

		char chvs[4], str[42], pk[TAM_PRIMARY_KEY];

		// número de chaves
		strncpy(chvs, temp, 3);
		chvs[3] = '\0';
		aux->num_chaves = atoi(chvs);

		// percorrer temp
		temp += 3;

		for (int x = 0; x < aux->num_chaves; x++)
		{
			int cont = 0;

			// pk
			strncpy(pk, temp, 10);
			pk[10] = '\0';
			strncpy(aux->chave[x].pk, pk, 10);
			aux->chave[x].pk[10] = '\0';

			temp += 10;

			while (temp[cont] != '#')
			{
				cont++;
			}

			// string
			strncpy(str, temp, 41);
			str[41] = '\0';
			strncpy(aux->chave[x].string, str, 41);
			aux->chave[x].string[cont] = '\0';

			temp += 41;
		}

		// percorrer temp para chaves não usadas
		temp += (ordem_is - 1 - aux->num_chaves) * 51;

		// folha
		aux->folha = temp[0];
		temp += 1;

		for (int i = 0; i < ordem_is; i++)
		{
			if (temp[0] != '*')
			{
				// descendentes
				strncpy(n, temp, 3);
				n[3] = '\0';
				aux->desc[i] = atoi(n);
			}

			else
			{
				aux->desc[i] = -1;
			}

			temp += 3;
		}

		return aux;
	}
}

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j)
{
	Chave_ip chave_ip;
	strcpy(chave_ip.pk, j.pk);
	chave_ip.rrn = nregistros;

	Chave_is chave_is;
	strcpy(chave_is.pk, j.pk);
	strcpy(chave_is.string, gerarString(&j));

	insere_chave_ip(iprimary, chave_ip);
	insere_chave_is(iride, chave_is);
}

/* ==========================================================================
 * ============================ ÍNDICE PRIMÁRIO =============================
 * ========================================================================== */

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary)
{
	Carona aux;
	Chave_ip c;

	iprimary->raiz = -1;

	// percorrer arquivo
	for (int i = 0; i < nregistros; i++)
	{
		aux = recuperar_registro(i);

		// pk
		strcpy(c.pk, aux.pk);

		// rrn
		c.rrn = i;

		insere_chave_ip(iprimary, c);
	}
}

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho)
{
	if (noderrn != -1)
	{
		node_Btree_ip *no = read_btree(noderrn, 1);
		int i = 0, j = 0, f = 0, r;

		// percorre as chaves e, se solicitado, imprime as que foram percorridas
		while (j < no->num_chaves)
		{
			if (strcmp(pk, no->chave[i].pk) > 0)
			{
				i++;
			}

			if (exibir_caminho)
			{
				if (j == no->num_chaves - 1)
				{
					printf("%s\n", no->chave[j].pk);
				}
				else
				{
					printf("%s, ", no->chave[j].pk);
				}
			}
			j++;
		}

		// achou a chave
		if (i < no->num_chaves && !strcmp(pk, no->chave[i].pk))
		{
			if (exibir_caminho)
			{
				printf("\n");
				exibir_registro(no->chave[i].rrn);
			}

			r = no->chave[i].rrn;

			libera_no(no, 'p');

			return r;
		}

		// chegou na folha e não achou a chave
		if (no->folha == 'T')
		{
			libera_no(no, 'p');

			return -1;
		}

		// chama a função para descendente
		else
		{
			r = no->desc[i];

			libera_no(no, 'p');

			return buscar_chave_ip(r, pk, exibir_caminho);
		}
	}

	else
	{
		return -1;
	}
}

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn)
{
	node_Btree_ip *no = read_btree(rrnesq, 1);
	int i = no->num_chaves - 1;
	int f = 0; // flag

	node_Btree_ip *aux = (node_Btree_ip *)criar_no('p'); // alocar nó do índice primário
	nregistrosip++;

	aux->folha = no->folha;
	aux->num_chaves = (ordem_ip - 1) / 2;

	// percorre as posições do aux
	for (int j = aux->num_chaves - 1; j >= 0; j--)
	{
		// a chave ainda não foi alocada e é maior que o elemento i do nó
		if (!f && strcmp(chave->pk, no->chave[i].pk) > 0)
		{
			// copiar a chave para o elemento j do aux
			strcpy(aux->chave[j].pk, chave->pk);
			aux->chave[j].rrn = chave->rrn;
			aux->desc[j + 1] = desc_dir_rrn;
			f = 1; // chave alocada
		}

		else
		{
			// copiar o elemento i do nó para a posição j do aux
			strcpy(aux->chave[j].pk, no->chave[i].pk);
			aux->chave[j].rrn = no->chave[i].rrn;
			aux->desc[j + 1] = no->desc[i + 1];
			i--;
		}
	}

	if (!f)
	{
		// "empurrar" chaves
		while (i >= 0 && strcmp(chave->pk, no->chave[i].pk) < 0)
		{
			no->chave[i + 1] = no->chave[i];
			no->desc[i + 2] = no->desc[i + 1];
			i--;
		}

		no->chave[i + 1] = *chave;
		no->desc[i + 2] = desc_dir_rrn;
	}

	*chave = no->chave[(ordem_ip / 2)]; // chave promovida
	aux->desc[0] = no->desc[(ordem_ip / 2) + 1];
	no->num_chaves = ordem_ip / 2;

	write_btree(aux, (nregistrosip - 1), 'p');
	write_btree(no, rrnesq, 'p');

	libera_no(aux, 'p');
	libera_no(no, 'p');

	return nregistrosip - 1;
}

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
int insere_aux_ip(int noderrn, Chave_ip *chave)
{
	node_Btree_ip *aux = read_btree(noderrn, 1);

	int i = aux->num_chaves - 1;
	int direito = -1;

	// verifica se o nó é folha
	if (aux->folha == 'T')
	{
		// verificar se o nó está cheio
		if (aux->num_chaves < ordem_ip - 1)
		{
			// percorrer nó para achar posição da inserção
			while (i >= 0 && strcmp(chave->pk, aux->chave[i].pk) < 0)
			{
				aux->chave[i + 1] = aux->chave[i];
				i--;
			}

			aux->chave[i + 1] = *chave;
			aux->num_chaves++;

			write_btree(aux, noderrn, 'p');

			libera_no(aux, 'p');

			chave = NULL;
			return -1; // não ocorreu overflow
		}

		else
		{
			libera_no(aux, 'p');

			// dividir nó cheio
			return divide_no_ip(noderrn, chave, direito);
		}
	}

	// nó não é folha
	else
	{
		// percorrer nó para achar posição
		while (i >= 0 && strcmp(chave->pk, aux->chave[i].pk) < 0)
		{
			i--;
		}

		i++;

		direito = insere_aux_ip(aux->desc[i], chave);

		if (direito != -1)
		{
			if (aux->num_chaves < ordem_ip - 1)
			{
				// mudei -1
				i = aux->num_chaves - 1;

				while (i >= 0 && strcmp(chave->pk, aux->chave[i].pk) < 0)
				{
					aux->chave[i + 1] = aux->chave[i];
					aux->desc[i + 2] = aux->desc[i + 1];
					i--;
				}

				aux->chave[i + 1] = *chave;
				aux->desc[i + 2] = direito;
				aux->num_chaves++;

				write_btree(aux, noderrn, 'p');

				libera_no(aux, 'p');

				chave = NULL;
				return -1;
			}

			else
			{
				libera_no(aux, 'p');

				return divide_no_ip(noderrn, chave, direito);
			}
		}

		else
		{
			libera_no(aux, 'p');

			chave = NULL;
			return -1;
		}
	}
}

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip chave)
{
	node_Btree_ip *aux = (node_Btree_ip *)criar_no('p');

	// nenhum registro
	if (iprimary->raiz == -1)
	{
		aux->folha = 'T';
		aux->num_chaves = 1;
		aux->chave[0] = chave;

		iprimary->raiz = 0;

		write_btree(aux, 0, 'p');

		nregistrosip++;
	}

	else
	{
		int direito = insere_aux_ip(iprimary->raiz, &chave);

		// se a chave foi promovida
		if (direito != -1)
		{
			aux->folha = 'F';
			aux->num_chaves = 1;
			aux->chave[0] = chave;

			aux->desc[0] = iprimary->raiz;
			aux->desc[1] = direito;

			iprimary->raiz = nregistrosip;

			write_btree(aux, nregistrosip, 'p');

			nregistrosip++;
		}
	}

	libera_no(aux, 'p');
}

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_ip(int noderrn, int nivel)
{
	if (noderrn != -1)
	{
		node_Btree_ip *no = read_btree(noderrn, 1);

		printf("%d - ", nivel);

		for (int i = 0; i < no->num_chaves; i++)
		{
			if (i == no->num_chaves - 1)
			{
				printf("%s\n", no->chave[i].pk);
			}
			else
			{
				printf("%s, ", no->chave[i].pk);
			}
		}

		for (int i = 0; i < no->num_chaves + 1; i++)
		{
			imprimir_arvore_ip(no->desc[i], nivel + 1);
		}

		libera_no(no, 'p');

		return 1;
	}

	else
	{
		return 0;
	}
}

/* ==========================================================================
 * =========================== ÍNDICE SECUNDÁRIO ============================
 * ========================================================================== */

/* (Re)faz o índice de Caronas  */
void criar_iride(Indice *iride)
{
	Carona aux;
	Chave_is c;
	char data[TAM_DATA - 2];
	char hora[TAM_HORA - 1];

	iride->raiz = -1;

	// percorrer o arquivo
	for (int i = 0; i < nregistros; i++)
	{
		aux = recuperar_registro(i);

		// pk
		strcpy(c.pk, aux.pk);

		strcpy(c.string, gerarString(&aux));

		insere_chave_is(iride, c);
	}
}

char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho)
{
	if (noderrn != -1)
	{
		node_Btree_is *no = read_btree(noderrn, 0);
		int i = 0, j = 0, f = 0, r;

		// percorre as chaves e, se solicitado, imprime as que foram percorridas
		while (j < no->num_chaves)
		{
			if (strcmp(titulo, no->chave[i].string) > 0)
			{
				i++;
			}

			if (exibir_caminho)
			{
				if (j == no->num_chaves - 1)
				{
					printf("%s\n", no->chave[j].string);
				}
				else
				{
					printf("%s, ", no->chave[j].string);
				}
			}
			j++;
		}

		// achou a chave
		if (i < no->num_chaves && !strcmp(titulo, no->chave[i].string))
		{
			libera_no(no, 'p');

			return no->chave[i].pk;
		}

		// chegou na folha e não achou a chave
		if (no->folha == 'T')
		{
			libera_no(no, 'p');

			return "***";
		}

		// chama a função para descendente
		else
		{
			r = no->desc[i];

			libera_no(no, 'p');

			return buscar_chave_is(r, titulo, exibir_caminho);
		}
	}
	else
	{
		return "***";
	}
}

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn)
{
	node_Btree_is *no = read_btree(rrnesq, 0);
	int i = no->num_chaves - 1;
	int f = 0; // flag

	node_Btree_is *aux = (node_Btree_is *)criar_no('s'); // alocar nó do índice primário
	nregistrosis++;

	aux->folha = no->folha;
	aux->num_chaves = (ordem_is - 1) / 2;

	// percorre as posições do aux
	for (int j = aux->num_chaves - 1; j >= 0; j--)
	{
		// a chave ainda não foi alocada e é maior que o elemento i do nó
		if (!f && strcmp(chave->string, no->chave[i].string) > 0)
		{
			// copiar a chave para o elemento j do aux
			strcpy(aux->chave[j].pk, chave->pk);
			strcpy(aux->chave[j].string, chave->string);
			aux->desc[j + 1] = desc_dir_rrn;
			f = 1; // chave alocada
		}

		else
		{
			// copiar o elemento i do nó para a posição j do aux
			strcpy(aux->chave[j].pk, no->chave[i].pk);
			strcpy(aux->chave[j].string, no->chave[i].string);
			aux->desc[j + 1] = no->desc[i + 1];
			i--;
		}
	}

	if (!f)
	{
		// "empurrar" chaves
		while (i >= 0 && strcmp(chave->string, no->chave[i].string) < 0)
		{
			no->chave[i + 1] = no->chave[i];
			no->desc[i + 2] = no->desc[i + 1];
			i--;
		}

		no->chave[i + 1] = *chave;
		no->desc[i + 2] = desc_dir_rrn;
	}

	*chave = no->chave[(ordem_is / 2)]; // chave promovida
	aux->desc[0] = no->desc[(ordem_is / 2) + 1];
	no->num_chaves = ordem_is / 2;

	write_btree(aux, (nregistrosis - 1), 's');
	write_btree(no, rrnesq, 's');

	libera_no(aux, 's');
	libera_no(no, 's');

	return nregistrosis - 1;
}

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
int insere_aux_is(int noderrn, Chave_is *chave)
{
	node_Btree_is *aux = read_btree(noderrn, 0);

	int i = aux->num_chaves - 1;
	int direito = -1;

	// verifica se o nó é folha
	if (aux->folha == 'T')
	{
		// verificar se o nó está cheio
		if (aux->num_chaves < ordem_is - 1)
		{
			// percorrer nó para achar posição da inserção
			while (i >= 0 && strcmp(chave->string, aux->chave[i].string) < 0)
			{
				aux->chave[i + 1] = aux->chave[i];
				i--;
			}

			aux->chave[i + 1] = *chave;
			aux->num_chaves++;

			write_btree(aux, noderrn, 's');

			libera_no(aux, 's');

			chave = NULL;
			return -1; // não ocorreu overflow
		}

		else
		{
			libera_no(aux, 's');

			// dividir nó cheio
			return divide_no_is(noderrn, chave, direito);
		}
	}

	// nó não é folha
	else
	{
		// percorrer nó para achar posição
		while (i >= 0 && strcmp(chave->string, aux->chave[i].string) < 0)
		{
			i--;
		}

		i++;

		direito = insere_aux_is(aux->desc[i], chave);

		if (direito != -1)
		{
			if (aux->num_chaves < ordem_is - 1)
			{
				// mudei -1
				i = aux->num_chaves - 1;

				while (i >= 0 && strcmp(chave->string, aux->chave[i].string) < 0)
				{
					aux->chave[i + 1] = aux->chave[i];
					aux->desc[i + 2] = aux->desc[i + 1];
					i--;
				}

				aux->chave[i + 1] = *chave;
				aux->desc[i + 2] = direito;
				aux->num_chaves++;

				write_btree(aux, noderrn, 's');

				libera_no(aux, 's');

				chave = NULL;
				return -1;
			}

			else
			{
				libera_no(aux, 's');

				return divide_no_is(noderrn, chave, direito);
			}
		}

		else
		{
			libera_no(aux, 's');

			chave = NULL;
			return -1;
		}
	}
}

void insere_chave_is(Indice *iride, Chave_is chave)
{
	node_Btree_is *aux = (node_Btree_is *)criar_no('s');

	// nenhum registro
	if (iride->raiz == -1)
	{
		aux->folha = 'T';
		aux->num_chaves = 1;
		aux->chave[0] = chave;

		iride->raiz = 0;

		write_btree(aux, 0, 's');

		nregistrosis++;
	}

	else
	{
		int direito = insere_aux_is(iride->raiz, &chave);

		// se a chave foi promovida
		if (direito != -1)
		{
			aux->folha = 'F';
			aux->num_chaves = 1;
			aux->chave[0] = chave;

			aux->desc[0] = iride->raiz;
			aux->desc[1] = direito;

			iride->raiz = nregistrosis;

			write_btree(aux, nregistrosis, 's');

			nregistrosis++;
		}
	}
}

int imprimir_arvore_is(int noderrn, int nivel)
{
	int i = 0;

	if (noderrn != -1)
	{
		node_Btree_is *no = read_btree(noderrn, 0);
		char *temp = (char *)malloc(49);
		char n[49];
		n[0] = '\0';

		imprimir_arvore_is(no->desc[i], nivel + 1);

		for (i = 0; i < no->num_chaves; i++)
		{
			// destino
			char *aux;
			strcpy(temp, no->chave[i].string);
			temp[49] = '\0';

			aux = strtok(temp, "$");
			printf("%s", aux);

			for (int j = strlen(aux); j < 30; j++)
			{
				printf("-");
			}

			aux = strtok(NULL, "\0");

			printf(" ");

			// data
			printf("%c%c/%c%c/%c%c", aux[4], aux[5], aux[2], aux[3], aux[0], aux[1]);

			aux += 6;

			printf(" - ");

			// hora
			printf("%c%c:%c%c", aux[0], aux[1], aux[2], aux[3]);

			printf("-\n");

			imprimir_arvore_is(no->desc[i + 1], nivel + 1);
		}

		libera_no(no, 'p');

		return 1;
	}

	else
	{
		return 0;
	}
}

/* ==========================================================================
 * ============================ FUNÇÕES DO MENU =============================
 * ========================================================================== */

/* Cadastra uma nova carona */
void cadastrar(Indice *iprimary, Indice *iride)
{
	// verificar se o arquivo já está cheio
	if (nregistros < MAX_REGISTROS)
	{
		Carona car;
		char registro[TAM_REGISTRO + 1];
		int b;

		// scanfs
		ler_entrada(registro, &car);

		// gerar pk
		gerarChave(&car);

		// verificar se a pk já existe
		b = buscar_chave_ip(iprimary->raiz, car.pk, 0);

		if (b != -1) // se a chave for encontrada, exibe mensagem
		{
			printf(ERRO_PK_REPETIDA, car.pk);
			return;
		}

		// completar com #
		while (strlen(registro) < TAM_REGISTRO)
		{
			strcat(registro, "#");
		}

		registro[TAM_REGISTRO] = '\0';

		// armazenar registro no arquivo
		strcat(ARQUIVO, registro);

		// atualizar índices
		inserir_registro_indices(iprimary, iride, car);

		// incrementar número de registros
		nregistros++;
	}
	else
	{
		printf(MEMORIA_INSUFICIENTE);
	}
}

int alterar(Indice iprimary)
{
	char par[TAM_PRIMARY_KEY] = "", vagas[TAM_VAGAS] = "";
	char temp[TAM_ARQUIVO] = "", *r = "";

	Indice *p, aux;
	int f = 0, cont = 0, rrn = 0;

	// recebe pk
	scanf("%[^\n]%*c", par);

	// verificar se a pk já existe
	rrn = buscar_chave_ip(iprimary.raiz, par, 0);

	if (rrn == -1)
	{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	// repetir até informar um valor certo
	while (!f)
	{
		// receber número de vagas
		scanf("%[^\n]%*c", vagas);
		vagas[1] = '\0';

		// verificar se o valor informado é válido
		if (isdigit(vagas[0]) && vagas[1] == '\0')
		{
			f = 1;

			// pegar registro
			strncpy(temp, ARQUIVO + (rrn * TAM_REGISTRO), TAM_REGISTRO);

			r = strtok(temp, "@");
			cont += strlen(r) + 1;

			// pular campos
			for (int i = 0; i < 9; i++)
			{
				r = strtok(NULL, "@");
				cont += strlen(r) + 1;
			}

			// alterar valor da quantidade de vagas
			ARQUIVO[(rrn * TAM_REGISTRO) + cont] = vagas[0];

			return 1;
		}
		else
		{
			printf(CAMPO_INVALIDO);
		}
	}

	return 0;
}

/* Buscar uma carona */
void buscar(Indice iprimary, Indice iride)
{
	int op = 0;
	char par[TAM_PRIMARY_KEY], dest[TAM_TRAJETO], dt[TAM_DATA], hr[TAM_HORA];
	char lixo;

	scanf("%d", &op);

	switch (op)
	{
	case 1:
		/* Busca por código */
		scanf("%s", par); // ler pk

		printf(NOS_PERCORRIDOS_IP, par);

		if (buscar_chave_ip(iprimary.raiz, par, 1) == -1)
		{
			printf("\n%s", REGISTRO_N_ENCONTRADO);
		}
		break;

	case 2:
		/* Busca por destino, data e hora */
		scanf("%c", &lixo);
		scanf("%[^\n]", dest); // ler destino
		scanf("%s", dt);	   // ler data
		scanf("%s", hr);	   // ler hora

		Carona c;
		strcpy(c.trajeto, dest);
		strcpy(c.data, dt);
		strcpy(c.hora, hr);

		char tit[TAM_STRING_INDICE], p[TAM_PRIMARY_KEY];
		strcpy(tit, gerarString(&c));

		printf(NOS_PERCORRIDOS_IS, tit);

		strcpy(p, buscar_chave_is(iride.raiz, tit, 1));

		if (strcmp(p, "***") != 0)
		{
			printf("\n");
			exibir_registro(buscar_chave_ip(iprimary.raiz, p, 0));
		}
		else
		{
			printf("\n%s", REGISTRO_N_ENCONTRADO);
		}
		break;
	}
}

/* Imprimir os índices */
void listar(Indice iprimary, Indice iride)
{
	int op = 0;
	char *par;

	scanf("%d", &op);

	switch (op)
	{
	case 1:
		if (!imprimir_arvore_ip(iprimary.raiz, 1))
		{
			printf(REGISTRO_N_ENCONTRADO);
		}
		break;
	case 2:
		if (!imprimir_arvore_is(iride.raiz, 1))
		{
			printf(REGISTRO_N_ENCONTRADO);
		}
		break;
	}
}