/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organizacao e Recuperacao da Informacao
 * Prof. Tiago A. Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
 *
 * RA: 743542
 * Aluno: Giulia Silva Fazzi
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

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

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO "**********************EXCLUIR*********************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"

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

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct
{
	int tam;
	Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/*Recupera um registro do arquivo de dados*/
Carona recuperar_registro(int rrn);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a);

/*Função de Hash*/
short hash(const char *chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Funcoes do menu*/
void ler_entrada(char *registro, Carona *novo);
void cadastrar(Hashtable *tabela);
int alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int remover(Hashtable *tabela);
void imprimir_tabela(Hashtable tabela);
void liberar_tabela(Hashtable *tabela);

/*Funcoes auxiliares*/
void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable *tabela);

int buscar_pk(Hashtable tabela, char *par);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Arquivo */
	int carregarArquivo = nregistros = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	// if (carregarArquivo)
	// 	carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	// if (carregarArquivo)
	// 	carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while (opcao != 6)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1:
			cadastrar(&tabela);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			printf("%s", (alterar(tabela)) ? SUCESSO : FALHA);
			break;
		case 3:
			printf(INICIO_BUSCA);
			buscar(tabela);
			break;
		case 4:
			printf(INICIO_EXCLUSAO);
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA);
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;
		case 7:
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;
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

short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn)
{
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
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

/* Libera a Tabela Hash */
void liberar_tabela(Hashtable *tabela)
{
	Chave *aux1, *aux2, *aux3;
	short i;
	for (i = 0, aux1 = tabela->v[i]; i < tabela->tam; aux1 = tabela->v[++i])
		for (aux2 = aux1; aux2; aux3 = aux2, aux2 = aux2->prox, free(aux3), aux3 = NULL)
			;
	free(tabela->v);
}

short hash(const char *chave, int tam)
{
	int n = 0;

	for (int i = 1; i <= 8; i++)
	{
		n += i * f(chave[i - 1]);
	}

	return n % tam;
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

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a)
{
	// calcular número primo (maior ou igual a tam)
	int f; // flag
	int t = a;

	while (1)
	{
		f = 1;

		for (int i = 2; i < t && f; i++)
		{
			if (t % i == 0)
			{
				f = 0;
			}
		}

		// não teve nenhum divisor menor que t
		if (f)
		{
			break; // parar while
		}

		t++;
	}

	return t;
}

/* ==========================================================================
 * ========================== FUNÇÕES AUXILIARES ============================
 * ========================================================================== */
void criar_tabela(Hashtable *tabela, int tam)
{
	tabela->tam = tam;

	tabela->v = (Chave **)malloc(sizeof(Chave) * tam);

	for (int i = 0; i < tam; i++)
	{
		tabela->v[i] = (Chave *)malloc(sizeof(Chave));
		tabela->v[i]->rrn = -1;
		tabela->v[i]->prox = NULL;
	}
}

void inserir_tabela(Hashtable *tabela, Carona car)
{
	int i = hash(car.pk, tabela->tam);
	int c = 0;

	// novo nó
	Chave *aux = (Chave *)malloc(sizeof(Chave));
	strcpy(aux->pk, car.pk);
	aux->rrn = nregistros;

	Chave *atual = tabela->v[i];

	// achar posição da pk
	if (atual->prox == NULL) // primeira posição
	{
		aux->prox = NULL;
		tabela->v[i]->prox = aux;
	}
	else
	{
		// perccorer lista
		while (atual->prox != NULL && strcmp(atual->prox->pk, aux->pk) < 0)
		{
			atual = atual->prox;
		}

		aux->prox = atual->prox;
		atual->prox = aux;
	}

	printf(REGISTRO_INSERIDO, car.pk);

	// incrementar número de registros
	nregistros++;
}

/* ==========================================================================
 * ============================ FUNÇÕES DO MENU =============================
 * ========================================================================== */
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

void cadastrar(Hashtable *tabela)
{
	Carona car;
	char registro[TAM_REGISTRO + 1];

	ler_entrada(registro, &car);

	// gerar pk
	gerarChave(&car);

	// verificar se a pk já existe
	int b = buscar_pk(*tabela, car.pk);

	if (b != -1)
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

	// inserir no hash
	inserir_tabela(tabela, car);
}

int alterar(Hashtable tabela)
{
	char par[TAM_PRIMARY_KEY] = "", vagas[TAM_VAGAS] = "";
	char temp[TAM_ARQUIVO] = "", *r = "";
	int f = 0, cont = 0;

	// recebe pk
	scanf("%[^\n]%*c", par);

	// verificar se a pk já existe
	int b = buscar_pk(tabela, par);

	if (b == -1)
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
			strncpy(temp, ARQUIVO + (b * TAM_REGISTRO), TAM_REGISTRO);

			r = strtok(temp, "@");
			cont += strlen(r) + 1;

			// pular campos
			for (int i = 0; i < 9; i++)
			{
				r = strtok(NULL, "@");
				cont += strlen(r) + 1;
			}

			// alterar valor da quantidade de vagas
			ARQUIVO[(b * TAM_REGISTRO) + cont] = vagas[0];

			return 1;
		}
		else
		{
			printf(CAMPO_INVALIDO);
		}
	}

	return 0;
}

void buscar(Hashtable tabela)
{
	char par[TAM_PRIMARY_KEY];
	scanf("%s", par);

	int rrn = buscar_pk(tabela, par);

	if (rrn != -1)
	{
		exibir_registro(rrn);
	}
	else
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

int buscar_pk(Hashtable tabela, char *par)
{
	int f = 0; // flag
	int rrn;

	//buscar na tabela
	rrn = hash(par, tabela.tam);

	Chave *aux = tabela.v[rrn]->prox;

	while (aux != NULL)
	{
		if (!strcmp(aux->pk, par))
		{
			return aux->rrn;
		}

		aux = aux->prox;
	}

	return -1;
}

int remover(Hashtable *tabela)
{
	char par[TAM_PRIMARY_KEY];

	// recebe pk
	scanf("%[^\n]%*c", par);

	// verificar se a pk já existe
	int b = buscar_pk(*tabela, par);

	if (b == -1)
	{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	else
	{
		// alterar caracteres
		ARQUIVO[(b * TAM_REGISTRO)] = '*';
		ARQUIVO[(b * TAM_REGISTRO) + 1] = '|';

		int h = hash(par, tabela->tam);

		Chave *atual = tabela->v[h]->prox;

		if (!strcmp(atual->pk, par))
		{
			free(atual);
			atual = NULL;
		}
		else
		{
			Chave *aux;

			// perccorer lista
			while (atual->prox != NULL && strcmp(atual->prox->pk, par) < 0)
			{
				atual = atual->prox;
				aux = atual->prox;
			}

			atual->prox = atual->prox->prox;

			free(aux);
		}

		return 1;
	}
}

void imprimir_tabela(Hashtable tabela)
{
	for (int i = 0; i < tabela.tam; i++)
	{
		printf("[%d]", i);

		// percorrer lista
		Chave *aux = tabela.v[i]->prox;

		while (aux != NULL)
		{
			printf(" %s", aux->pk);

			aux = aux->prox;
		}
		printf("\n");
	}
}