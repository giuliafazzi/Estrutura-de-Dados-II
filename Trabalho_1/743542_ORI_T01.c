/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 743542
 * Aluno: Giulia Silva Fazzi
 * ========================================================================== */

/*
	Para o desenvolvimento desse trabalho, contou-se com ajuda dos alunos
	Luiz Felipe Guimarães e Bruno Bevilacqua Rizzi, a fim de discutir soluções lógicas
	e especificidades da linguagem C. Ainda, houve conversas em alto nível com o
	aluno João Victor Montefusco.
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
#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_TRAJETOS 30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!\n"
#define INICIO_BUSCA "**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM "**********************LISTAR**********************\n"
#define INICIO_ALTERACAO "**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO "**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO "**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO "*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"

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

/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
} Is;

/* Struct para índice secundário de data */
typedef struct secundary_index_of_date
{
	char pk[TAM_PRIMARY_KEY];
	char data[TAM_DATA];
} Isd;

/* Struct para índice secundário de hora */
typedef struct secundary_index_of_time
{
	char pk[TAM_PRIMARY_KEY];
	char hora[TAM_HORA];
} Ist;

/* Lista ligada usada no índice invertido*/
typedef struct linked_list
{
	char pk[TAM_PRIMARY_KEY];
	struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index
{
	char trajeto[TAM_TRAJETO];
	ll *lista;
} Ir;

/*GLOBAL:*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Carona */
Carona recuperar_registro(int rrn);

/* Altera a quantidade de vagas de uma carona */
int alterar(int *nregistros, Ip *iprimary);

/* Remove uma carona */
int remover(int *nregistros, Ip *iprimary);

/* Liberar espaço (retira registros excluídos) */
void liberar(int *nregistros, int *ntraj, Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute);

/* Liberar índice de trajetos */
void liberarTrajeto(int *nregistros, Ir *iroute);

/* Liberar memória ao finalizar o programa */
void liberarMemoria(Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *ntraj);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int *nregistros);

/* Insere um novo registro no índice primário */
void atualizar_iprimary(Ip *indice_primario, int *nregistros, char *pk);

/* (Re)faz os índices secundários */
void criar_isecundary(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *ntraj);

/* Cria o índice de trajetos */
void criar_iroute(Ir *iroute, Carona car, int *ntraj);

/* Insere um novo registro nos índices secundários */
void atualizar_isecundary(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, Carona car, int *ntraj);

/* Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Faz o cadastro de um novo registro de carona */
void cadastrarRegistro(int *nregistros, Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *ntraj);

/* Opções de busca */
void buscar(int *nregistros, int *ntraj, Ip *iprimary, Isd *idate, Ir *iroute);

/* Realiza busca por chave primária */
void buscarCodigo(Ip *iprimary, int *nregistros);

/* Realiza busca por data da carona */
void buscarData(Ip *iprimary, Isd *idate, int *nregistros);

/* Realiza busca por trajeto */
void buscarTrajeto(Ip *iprimary, Ir *iroute, int *nregistros, int *ntraj);

/* Realiza busca por trajeto e data */
void buscarTrajetoData(Ip *iprimary, Ir *iroute, Isd *idate, int *nregistros, int *ntraj);

/* Opções de listagem */
void listar(int *nregistros, int *ntraj, Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime);

/* Realiza listagem por código */
void listarCodigo(Ip *iprimary, int *nregistros);

/* Realiza listagem por nome do motorista */
void listarMotorista(Ip *iprimary, Is *idriver, int *nregistros);

/* Realiza listagem por trajeto */
void listarTrajeto(Ip *iprimary, Ir *iroute, int *nregistros, int *ntraj);

/* Realiza listagem por data e hora */
void listarDataHora(Ip *iprimary, Isd *idate, Ist *itime, int *nregistros);

/* Realiza listagem por trajeto e data e hora */
void listarTrajetoDataHora(Ip *iprimary, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *ntraj);

/* Compara trajetos para bsearch*/
int comparar_traj(const void *a, const void *b);

/* Compara ordem alfabética das chaves */
int comparar_iprimary(const void *a, const void *b);

/* Compara pk da hora */
int comparar_pktime(const void *a, const void *b);

/* Compara pk da data */
int comparar_pkdata(const void *a, const void *b);

/* Compara string */
int comparar_str(const void *a, const void *b);

/* Compara ordem alfabética dos nomes */
int comparar_idriver(const void *a, const void *b);

/* Compara ordem das datas e horas */
int comparar_idateitime(const void *a, const void *b);

/* Compara ordem das datas */
int comparar_idate(const void *a, const void *b);

/* Compara ordem das horas */
int comparar_itime(const void *a, const void *b);

/* Gera a chave para determinada struct de carona */
void gerarChave(Carona *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ntraj = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *)malloc(MAX_REGISTROS * sizeof(Ip));
	if (!iprimary)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/*ALOCAR E CRIAR ÍNDICES SECUNDÁRIOS*/
	Is *idriver = (Is *)malloc(MAX_REGISTROS * sizeof(Is));
	Ir *iroute = (Ir *)malloc(MAX_REGISTROS * sizeof(Ir));
	Isd *idate = (Isd *)malloc(MAX_REGISTROS * sizeof(Isd));
	Ist *itime = (Ist *)malloc(MAX_REGISTROS * sizeof(Ist));

	if (!idriver || !iroute || !idate || !itime)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	criar_isecundary(idriver, iroute, idate, itime, &nregistros, &ntraj);

	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		//printf("escolha:");
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1:
			/*cadastro*/
			cadastrarRegistro(&nregistros, iprimary, idriver, iroute, idate, itime, &ntraj);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			if (alterar(&nregistros, iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			/*excluir produto*/
			printf(INICIO_EXCLUSAO);
			if (remover(&nregistros, iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 4:
			printf(INICIO_BUSCA);
			buscar(&nregistros, &ntraj, iprimary, idate, iroute);
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			listar(&nregistros, &ntraj, iprimary, idriver, iroute, idate, itime);
			break;
		case 6:
			/*    libera espaço */
			liberar(&nregistros, &ntraj, iprimary, idriver, idate, itime, iroute);
			break;
		case 7:
			printf(INICIO_ARQUIVO);
			if (nregistros == 0)
				printf(ARQUIVO_VAZIO);
			else
				printf("%s\n", ARQUIVO);
			break;
		case 8:
			/*    imprime os índices secundários */
			imprimirSecundario(idriver, iroute, idate, itime, nregistros, ntraj);
			break;
		case 9:
			/*    Liberar memória e finalizar o programa */
			liberarMemoria(iprimary, idriver, iroute, idate, itime, &ntraj);
			return 0;
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
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

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Carona */
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

/* Altera a quantidade de vagas de uma carona */
int alterar(int *nregistros, Ip *iprimary)
{
	char par[TAM_PRIMARY_KEY] = "", vagas[TAM_VAGAS] = "";
	char temp[TAM_ARQUIVO] = "", *r = "";
	Ip *p, aux;
	int f = 0, cont = 0;

	// recebe pk
	scanf("%[^\n]%*c", par);

	// verificar se a pk já existe
	strcpy(aux.pk, par);
	p = (Ip *)bsearch(&aux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

	if (!p || p->rrn == -1)
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
			strncpy(temp, ARQUIVO + ((p->rrn) * TAM_REGISTRO), TAM_REGISTRO);

			r = strtok(temp, "@");
			cont += strlen(r) + 1;

			// pular campos
			for (int i = 0; i < 9; i++)
			{
				r = strtok(NULL, "@");
				cont += strlen(r) + 1;
			}

			// alterar valor da quantidade de vagas
			ARQUIVO[((p->rrn) * TAM_REGISTRO) + cont] = vagas[0];

			return 1;
		}
		else
		{
			printf(CAMPO_INVALIDO);
		}
	}

	return 0;
}

/* Remove uma carona */
int remover(int *nregistros, Ip *iprimary)
{
	char par[TAM_PRIMARY_KEY];
	Ip *p, aux;

	// recebe pk
	scanf("%[^\n]%*c", par);

	// verificar se a pk já existe
	strcpy(aux.pk, par);
	p = (Ip *)bsearch(&aux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

	if (!p || p->rrn == -1)
	{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	// alterar caracteres
	ARQUIVO[((p->rrn) * TAM_REGISTRO)] = '*';
	ARQUIVO[((p->rrn) * TAM_REGISTRO) + 1] = '|';

	// alterar rrn
	p->rrn = -1;

	return 1;
}

/* Liberar espaço (retira registros excluídos) */
void liberar(int *nregistros, int *ntraj, Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute)
{
	char temp[TAM_ARQUIVO] = "", aux[TAM_REGISTRO + 1] = "", rem[3] = "";
	int cont = 0;

	// percorrer o arquivo
	for (int i = 0; i < *nregistros; i++)
	{
		// pegar os dois primeiros caracteres
		rem[0] = ARQUIVO[(i * TAM_REGISTRO)];
		rem[1] = ARQUIVO[(i * TAM_REGISTRO) + 1];
		rem[2] = '\0';

		aux[TAM_REGISTRO] = '\0';

		// se não começar com *|, copia para o arquivo temporário
		if (strcmp("*|", rem) != 0)
		{
			strncpy(aux, ARQUIVO + (i * TAM_REGISTRO), TAM_REGISTRO);
			strcat(temp, aux);
			cont++;
		}
	}
	// alterar ARQUIVO
	strcpy(ARQUIVO, temp);

	// alterar número de registros
	*nregistros = cont;

	// liberar índices
	liberarTrajeto(ntraj, iroute);
	*ntraj = 0;

	// atualizar índices
	criar_iprimary(iprimary, nregistros);
	criar_isecundary(idriver, iroute, idate, itime, nregistros, ntraj);
}

/* Liberar índice de trajetos */
void liberarTrajeto(int *ntraj, Ir *iroute)
{
	ll *temp1, *temp2;

	// percorrer o índice de trajetos
	for (int i = 0; i < *ntraj; i++)
	{
		temp1 = iroute[i].lista;

		while (temp1)
		{
			temp2 = temp1->prox;
			free(temp1);
			temp1 = temp2;
		}

		strcpy(iroute[i].trajeto, "");
	}
}

/* Liberar memória ao finalizar o programa */
void liberarMemoria(Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *ntraj)
{
	// liberar índices
	free(itime);
	free(idate);

	liberarTrajeto(ntraj, iroute);
	free(iroute);

	free(idriver);
	free(iprimary);
}

/* Lê entradas do usuário e cadastra um nova carona */
void cadastrarRegistro(int *nregistros, Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *ntraj)
{
	// verificar se o arquivo já está cheio
	if (*nregistros < MAX_REGISTROS)
	{
		Carona car;
		char registro[TAM_REGISTRO + 1];
		Ip aux, *p;

		ler_entrada(registro, &car);

		// gerar pk
		gerarChave(&car);

		// verificar se a pk já existe
		strcpy(aux.pk, car.pk);
		p = (Ip *)bsearch(&aux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

		if (p && p->rrn != -1)
		{
			printf(ERRO_PK_REPETIDA, aux.pk);
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
		atualizar_iprimary(iprimary, nregistros, car.pk);

		// adicionar_iprimary
		atualizar_isecundary(idriver, iroute, idate, itime, nregistros, car, ntraj);

		// incrementar número de registros
		(*nregistros)++;
	}
}

/* Opções de busca */
void buscar(int *nregistros, int *ntraj, Ip *iprimary, Isd *idate, Ir *iroute)
{
	int op = 0;

	scanf("%d", &op);

	switch (op)
	{
	case 1:
		/* Busca por código */
		buscarCodigo(iprimary, nregistros);
		break;

	case 2:
		/* Busca por data */
		buscarData(iprimary, idate, nregistros);
		break;

	case 3:
		/* Busca por localidade */
		buscarTrajeto(iprimary, iroute, nregistros, ntraj);
		break;

	case 4:
		/* Busca por localidade e data */
		buscarTrajetoData(iprimary, iroute, idate, nregistros, ntraj);
		break;

	default:
		break;
	}
}

/* Realiza busca por chave primária */
void buscarCodigo(Ip *iprimary, int *nregistros)
{
	char par[TAM_PRIMARY_KEY];
	scanf("%s", par);

	Ip *i, iaux;

	// busca binária no índice primário
	strcpy(iaux.pk, par);
	i = (Ip *)bsearch(&iaux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

	if (!i || i->rrn == -1) // se não encontrar nada, exibe mensagem
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
	else
	{
		exibir_registro(i->rrn);
	}
}

/* Realiza busca por data da carona */
void buscarData(Ip *iprimary, Isd *idate, int *nregistros)
{
	char par[TAM_DATA];
	scanf("%s", par);
	par[TAM_DATA - 1] = '\0';

	int f = 0; // flag

	Isd *d, daux;
	Ip *p, paux;

	// percorrer índice de datas
	for (int i = 0; i < *nregistros; i++)
	{
		// procurar no índice primário
		strcpy(paux.pk, idate[i].pk);
		p = (Ip *)bsearch(&paux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

		// verificar se registro foi excluído
		if (!strcmp(idate[i].data, par) && p->rrn != -1)
		{
			if (f)
			{
				printf("\n");
			}
			exibir_registro(p->rrn);
			f = 1;
		}
	}

	if (!f) // se não encontrar nada, exibe mensagem
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

/* Realiza busca por trajeto */
void buscarTrajeto(Ip *iprimary, Ir *iroute, int *nregistros, int *ntraj)
{
	char par[TAM_TRAJETO];
	char lixo;
	Ir *r, raux;

	scanf("%c", &lixo);
	scanf("%[^\n]%*c", par);

	int f = 0; // flag

	// busca binária no índice de trajetos
	strcpy(raux.trajeto, par);
	r = (Ir *)bsearch(&raux, iroute, *ntraj, sizeof(Ir), comparar_traj);

	if (r)
	{
		Ip *p, paux;
		ll *lst = r->lista;

		// percorre a lista ligada
		while (lst)
		{
			// buscar a chave no índice primário
			strcpy(paux.pk, lst->pk);
			p = (Ip *)bsearch(&paux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

			// verificar se o registro não foi excluído
			if (p->rrn != -1)
			{
				if (f)
				{
					printf("\n");
				}
				exibir_registro(p->rrn);
				f = 1;
			}

			lst = lst->prox;
		}
	}

	if (!f) // se não encontrar nada, exibe mensagem
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

/* Realiza busca por trajeto e data */
void buscarTrajetoData(Ip *iprimary, Ir *iroute, Isd *idate, int *nregistros, int *ntraj)
{
	char par_traj[TAM_TRAJETO] = "", par_data[TAM_DATA] = "";
	Ir *r, raux;
	char lixo;
	int f = 0; // flag

	scanf("%c", &lixo);
	scanf("%[^\n]%*c", par_traj);
	scanf("%s", par_data);

	// ordenar índice por pk
	qsort(idate, *nregistros, sizeof(Isd), comparar_pkdata);

	// busca binária no índice de trajetos
	strcpy(raux.trajeto, par_traj);
	r = (Ir *)bsearch(&raux, iroute, *ntraj, sizeof(Ir), comparar_traj);

	if (r)
	{
		Isd *d, daux;
		Ip *p, paux;
		ll *lst = r->lista;

		// percorre a lista ligada
		while (lst)
		{
			// buscar a chave no índice de datas
			strcpy(daux.pk, lst->pk);
			d = (Isd *)bsearch(&daux, idate, *nregistros, sizeof(Isd), comparar_pkdata);

			if (d && !strcmp(d->data, par_data))
			{
				// buscar a chave no índice primário
				strcpy(paux.pk, lst->pk);
				p = (Ip *)bsearch(&paux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

				// verificar se o registro não foi excluído
				if (p->rrn != -1)
				{
					if (f)
					{
						printf("\n");
					}
					exibir_registro(p->rrn);
					f = 1;
				}
			}
			lst = lst->prox;
		}
	}

	if (!f) // se não encontrar nada, exibe mensagem
	{
		printf(REGISTRO_N_ENCONTRADO);
	}

	// ordenar índice por data
	qsort(idate, *nregistros, sizeof(Isd), comparar_idate);
}

/* Opções de listagem */
void listar(int *nregistros, int *ntraj, Ip *iprimary, Is *idriver, Ir *iroute, Isd *idate, Ist *itime)
{
	int op = 0;

	scanf("%d", &op);

	switch (op)
	{
	case 1:
		/* Listagem por código */
		listarCodigo(iprimary, nregistros);
		break;

	case 2:
		/* Listagem por trajeto */
		listarTrajeto(iprimary, iroute, nregistros, ntraj);
		break;

	case 3:
		/* Listagem por nome do motorista */
		listarMotorista(iprimary, idriver, nregistros);
		break;

	case 4:
		/* Listagem por data e hora */
		listarDataHora(iprimary, idate, itime, nregistros);
		break;

	case 5:
		/* Listagem por trajeto e data e hora*/
		listarTrajetoDataHora(iprimary, iroute, idate, itime, nregistros, ntraj);
		break;

	default:
		break;
	}
}

/* Realiza listagem por código */
void listarCodigo(Ip *iprimary, int *nregistros)
{
	int f = 0;

	// percorre índice primário
	for (int i = 0; i < *nregistros; i++)
	{
		if (iprimary[i].rrn != -1)
		{
			if (f)
			{
				printf("\n");
			}

			// se o registro não tiver sido, excluido, exibe
			exibir_registro(iprimary[i].rrn);
			f = 1;
		}
	}

	if (!f)
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

/* Realiza listagem por nome do motorista */
void listarMotorista(Ip *iprimary, Is *idriver, int *nregistros)
{
	int f = 0;
	Ip *p, paux;

	// percorre índice secundário
	for (int i = 0; i < *nregistros; i++)
	{
		// busca binária no índice primário
		strcpy(paux.pk, idriver[i].pk);
		p = (Ip *)bsearch(&paux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

		if (p->rrn != -1)
		{
			if (f)
			{
				printf("\n");
			}

			// se o registro não tiver sido, excluido, exibe
			exibir_registro(p->rrn);
			f = 1;
		}
	}

	if (!f)
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

/* Realiza listagem por trajeto */
void listarTrajeto(Ip *iprimary, Ir *iroute, int *nregistros, int *ntraj)
{
	int f = 0; // flag

	// percorre o índice de trajetos
	for (int i = 0; i < *ntraj; i++)
	{
		Ip *p, paux;
		ll *lst = (ll *)malloc(sizeof(ll));
		lst = iroute[i].lista;

		// percorre a lista ligada
		while (lst)
		{
			// buscar a chave no índice primário
			strcpy(paux.pk, lst->pk);
			p = (Ip *)bsearch(&paux, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

			// verificar se o registro não foi excluído
			if (p->rrn != -1)
			{
				if (f)
				{
					printf("\n");
				}
				exibir_registro(p->rrn);
				f = 1;
			}

			lst = lst->prox;
		}
	}

	if (!f)
	{
		printf(REGISTRO_N_ENCONTRADO);
	}
}

/* Realiza listagem por trajeto e data e hora */
void listarTrajetoDataHora(Ip *iprimary, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *ntraj)
{
	int cont = 0;
	int a = 0; // flag
	Ip *p, *paux, px;
	Isd *d, *daux, dx;
	Ist *t, *taux, tx;
	ll *lst = (ll *)malloc(sizeof(ll));

	// ordenar índices por pk
	qsort(idate, *nregistros, sizeof(Isd), comparar_pkdata);
	qsort(itime, *nregistros, sizeof(Ist), comparar_pktime);

	// percorrer o índice de trajetos
	for (int i = 0; i < *ntraj; i++)
	{
		lst = iroute[i].lista;

		while (lst)
		{
			cont++;
			lst = lst->prox;
		}

		p = (Ip *)malloc((cont) * sizeof(Ip));
		d = (Isd *)malloc((cont) * sizeof(Isd));
		t = (Ist *)malloc((cont) * sizeof(Ist));

		cont = 0;

		lst = iroute[i].lista;

		// percorre a lista ligada
		while (lst)
		{
			// buscar a data da pk
			strcpy(dx.pk, lst->pk);
			daux = (Isd *)bsearch(&dx, idate, *nregistros, sizeof(Isd), comparar_pkdata);

			// buscar a hora da pk
			strcpy(tx.pk, lst->pk);
			taux = (Ist *)bsearch(&tx, itime, *nregistros, sizeof(Ist), comparar_pktime);

			// buscar rrn da pk
			strcpy(px.pk, lst->pk);
			paux = (Ip *)bsearch(&px, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

			// inserir nos índices auxiliares
			strcpy(d[cont].pk, lst->pk);
			strcpy(d[cont].data, daux->data);
			strcpy(t[cont].pk, lst->pk);
			strcpy(t[cont].hora, taux->hora);
			strcpy(p[cont].pk, lst->pk);
			p[cont].rrn = paux->rrn;

			cont++;
			lst = lst->prox;
		}

		qsort(d, cont - 1, sizeof(Isd), comparar_idate); // ordenar idate auxiliar

		if (cont != 0)
		{
			if (a)
			{
				printf("\n");
			}
			listarDataHora(p, d, t, &cont);
			a = 1;
		}

		cont = 0;

		// liberar índices auxiliares
		free(t);
		free(d);
		free(p);
	}

	// ordenar índices por data e por hora
	qsort(idate, *nregistros, sizeof(Isd), comparar_idate);
	qsort(itime, *nregistros, sizeof(Ist), comparar_itime);
}

/* Realiza listagem por data e hora */
void listarDataHora(Ip *iprimary, Isd *idate, Ist *itime, int *nregistros)
{
	int f = 0;
	char aux[*nregistros][6 + TAM_HORA + TAM_PRIMARY_KEY - 1];
	Ist *haux, t;
	char data[TAM_DATA];
	char ano[3] = "", mes[3] = "", dia[3] = "";
	qsort(itime, *nregistros, sizeof(Ist), comparar_pktime);

	// percorrer índice de datas
	for (int i = 0; i < *nregistros; i++)
	{
		strcpy(data, idate[i].data);

		// separar data
		ano[0] = data[6];
		ano[1] = data[7];
		ano[2] = '\0';

		mes[0] = data[3];
		mes[1] = data[4];
		mes[2] = '\0';

		dia[0] = data[0];
		dia[1] = data[1];
		dia[2] = '\0';

		strcpy(aux[i], ano);
		strcat(aux[i], mes);
		strcat(aux[i], dia);

		// criar string com pk + data + hora
		strcpy(t.pk, idate[i].pk);
		haux = (Ist *)bsearch(&t, itime, *nregistros, sizeof(Ist), comparar_pktime);

		strcat(aux[i], haux->hora);
		strcat(aux[i], haux->pk);
		aux[i][6 + TAM_HORA + TAM_PRIMARY_KEY - 2] = '\0';
	}

	// ordenar vetor de strings
	qsort(aux, *nregistros, 6 + TAM_HORA + TAM_PRIMARY_KEY - 1, comparar_str);

	Ip *p, x;
	char pkaux[TAM_PRIMARY_KEY] = "";

	for (int i = 0; i < *nregistros; i++)
	{
		// cortar pk
		pkaux[0] = aux[i][11];
		pkaux[1] = aux[i][12];
		pkaux[2] = aux[i][13];
		pkaux[3] = aux[i][14];
		pkaux[4] = aux[i][15];
		pkaux[5] = aux[i][16];
		pkaux[6] = aux[i][17];
		pkaux[7] = aux[i][18];
		pkaux[8] = aux[i][19];
		pkaux[9] = aux[i][20];
		pkaux[10] = '\0';

		// buscar a chave no índice primário
		strcpy(x.pk, pkaux);
		p = (Ip *)bsearch(&x, iprimary, *nregistros, sizeof(Ip), comparar_iprimary);

		// verificar se o registro não foi excluído
		if (p->rrn != -1)
		{
			if (f)
			{
				printf("\n");
			}
			exibir_registro(p->rrn);
			f = 1;
		}
	}

	if (!f)
	{
		printf(REGISTRO_N_ENCONTRADO);
	}

	// ordenar iime por hora
	qsort(itime, *nregistros, sizeof(Ist), comparar_itime);
}

/* ==========================================================================
 * ============================= CRIAR ÍNDICES ==============================
 * ========================================================================== */

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int *nregistros)
{
	Carona aux;

	// percorrer todos os nregistros
	for (int i = 0; i < *nregistros; i++)
	{
		// chamar recuperar registro
		aux = recuperar_registro(i);

		// salvar pk e rrn
		strcpy(indice_primario[i].pk, aux.pk);
		indice_primario[i].rrn = i;

		// ver se o registro foi apagado
		if (strncmp(aux.nome, "*|", 2) == 0)
		{
			indice_primario[i].rrn = -1;
		}
		else
		{
			indice_primario[i].rrn = i;
		}
	}

	// ordenar
	qsort(indice_primario, *nregistros, sizeof(Ip), comparar_iprimary);
}

/* Insere um novo registro no índice primário */
void atualizar_iprimary(Ip *indice_primario, int *nregistros, char *pk)
{
	// inserir na última posição do índice primário
	strcpy(indice_primario[*nregistros].pk, pk);
	indice_primario[*nregistros].rrn = *nregistros;

	// ordenar índice por pk
	qsort(indice_primario, *nregistros + 1, sizeof(Ip), comparar_iprimary);
}

/* Insere um novo registro nos índices secundários */
void atualizar_isecundary(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, Carona car, int *ntraj)
{
	// inseir na última posição dos índices
	strcpy(idriver[*nregistros].pk, car.pk);
	strcpy(idate[*nregistros].pk, car.pk);
	strcpy(itime[*nregistros].pk, car.pk);

	strcpy(idriver[*nregistros].nome, car.nome);
	strcpy(idate[*nregistros].data, car.data);
	strcpy(itime[*nregistros].hora, car.hora);

	qsort(idriver, *nregistros + 1, sizeof(Is), comparar_idriver); // ordenar idriver
	qsort(idate, *nregistros + 1, sizeof(Isd), comparar_idate);	// ordenar idate
	qsort(itime, *nregistros + 1, sizeof(Ist), comparar_itime);	// ordenar itime

	criar_iroute(iroute, car, ntraj);
}

/* (Re)faz os índices secundários */
void criar_isecundary(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int *nregistros, int *ntraj)
{
	Carona aux;

	for (int i = 0; i < *nregistros; i++)
	{
		aux = recuperar_registro(i);

		// salvar pk
		strcpy(idriver[i].pk, aux.pk);
		strcpy(idate[i].pk, aux.pk);
		strcpy(itime[i].pk, aux.pk);

		// salvar dados
		strcpy(idriver[i].nome, aux.nome);
		strcpy(idate[i].data, aux.data);
		strcpy(itime[i].hora, aux.hora);

		criar_iroute(iroute, aux, ntraj);
	}

	qsort(idriver, *nregistros, sizeof(Is), comparar_idriver); // ordenar idriver
	qsort(idate, *nregistros, sizeof(Isd), comparar_idate);	// ordenar idate
	qsort(itime, *nregistros, sizeof(Ist), comparar_itime);	// ordenar itime
}

/* Cria o índice de trajetos */
void criar_iroute(Ir *iroute, Carona car, int *ntraj)
{
	char *aux;
	Ir *ptr, raux;

	// primeiro trajeto
	aux = strtok(car.trajeto, "|");

	while (aux) // repete até ser nulo
	{
		// busca para saber se trajeto já existe
		strcpy(raux.trajeto, aux);
		ptr = (Ir *)bsearch(&raux, iroute, *ntraj, sizeof(Ir), comparar_traj);

		if (!ptr)
		{
			// se não, insere na lista de trajetos
			strcpy(iroute[*ntraj].trajeto, aux);
			iroute[*ntraj].lista = (ll *)malloc(sizeof(ll));
			strcpy(iroute[*ntraj].lista->pk, car.pk);
			iroute[*ntraj].lista->prox = NULL;
			(*ntraj)++;
		}
		else
		{
			ll *novo = (ll *)malloc(sizeof(ll));
			strcpy(novo->pk, car.pk);
			novo->prox = NULL;

			// inserir na primeira posição
			if (strcmp(ptr->lista->pk, novo->pk) > 0)
			{
				novo->prox = ptr->lista;
				ptr->lista = novo;
			}
			else
			{
				ll *lst = ptr->lista;
				// procurar posição para inserção
				while (lst->prox && strcmp(lst->prox->pk, novo->pk) < 0)
				{
					lst = lst->prox;
				}

				if (strcmp(lst->pk, novo->pk) != 0)
				{
					// inserir na última posição
					if (!lst->prox)
					{
						lst->prox = novo;
						novo->prox = NULL;
					}

					// inserir no meio
					else
					{
						novo->prox = lst->prox;
						lst->prox = novo;
					}
				}
			}
		}

		// ordena trajetos
		qsort(iroute, *ntraj, sizeof(Ir), comparar_traj);

		aux = strtok(NULL, "|");
	}
}

/* Compara pk da hora */
int comparar_pktime(const void *a, const void *b)
{
	return strcmp(((Ist *)a)->pk, ((Ist *)b)->pk);
}

/* Compara pk da data */
int comparar_pkdata(const void *a, const void *b)
{
	return strcmp(((Isd *)a)->pk, ((Isd *)b)->pk);
}

/* Compara string */
int comparar_str(const void *a, const void *b)
{
	return strcmp((char *)a, (char *)b);
}

/* Compara trajetos para bsearch*/
int comparar_traj(const void *a, const void *b)
{
	return strcmp(((Ir *)a)->trajeto, ((Ir *)b)->trajeto);
}

/* Compara ordem alfabética das chaves */
int comparar_iprimary(const void *a, const void *b)
{
	return strcmp(((Ip *)a)->pk, ((Ip *)b)->pk);
}

/* Compara ordem alfabética dos nomes */
int comparar_idriver(const void *a, const void *b)
{
	// se os nomes forem iguais, ordenar por pk
	if (strcmp(((Is *)a)->nome, ((Is *)b)->nome) == 0)
	{
		return strcmp(((Is *)a)->pk, ((Is *)b)->pk);
	}
	else
	{
		return strcmp(((Is *)a)->nome, ((Is *)b)->nome);
	}
}

/* Compara ordem das datas */
int comparar_idate(const void *a, const void *b)
{
	char data_a[TAM_DATA], data_b[TAM_DATA];
	strcpy(data_a, ((Isd *)a)->data);
	strcpy(data_b, ((Isd *)b)->data);

	char ano_a[3] = "", mes_a[3] = "", dia_a[3] = "";
	char ano_b[3] = "", mes_b[3] = "", dia_b[3] = "";

	ano_a[0] = toupper(data_a[6]);
	ano_a[1] = toupper(data_a[7]);
	ano_a[2] = '\0';
	ano_b[0] = toupper(data_b[6]);
	ano_b[1] = toupper(data_b[7]);
	ano_b[2] = '\0';

	if (!strcmp(data_a, data_b)) // se as datas forem iguais, ordena por pk
	{
		return strcmp(((Isd *)a)->pk, ((Isd *)b)->pk);
	}
	else if (atoi(ano_a) == atoi(ano_b))
	{
		mes_a[0] = toupper(data_a[3]);
		mes_a[1] = toupper(data_a[4]);
		mes_a[2] = '\0';
		mes_b[0] = toupper(data_b[3]);
		mes_b[1] = toupper(data_b[4]);
		mes_b[2] = '\0';

		if (atoi(mes_a) == atoi(mes_b))
		{
			dia_a[0] = toupper(data_a[0]);
			dia_a[1] = toupper(data_a[1]);
			dia_a[2] = '\0';
			dia_b[0] = toupper(data_b[0]);
			dia_b[1] = toupper(data_b[1]);
			dia_b[2] = '\0';

			return (atoi(dia_a) > atoi(dia_b));
		}
		else
		{
			return (atoi(mes_a) > atoi(mes_b));
		}
	}
	else
	{
		return (atoi(ano_a) > atoi(ano_b));
	}
}

/* Compara ordem das horas */
int comparar_itime(const void *a, const void *b)
{
	// se as horas forem iguais, ordenar por pk
	if (strcmp(((Ist *)a)->hora, ((Ist *)b)->hora) == 0)
	{
		return strcmp(((Ist *)a)->pk, ((Ist *)b)->pk);
	}
	else
	{
		return strcmp(((Ist *)a)->hora, ((Ist *)b)->hora);
	}
}

/* Imprimir indices secundarios */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj)
{
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if (!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint)
	{
	case 1:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idriver[i].pk, idriver[i].nome);
		}
		break;
	case 2:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idate[i].pk, idate[i].data);
		}
		break;
	case 3:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", itime[i].pk, itime[i].hora);
		}
		break;
	case 4:
		for (int i = 0; i < ntraj; i++)
		{
			printf("%s", iroute[i].trajeto);
			aux = iroute[i].lista;
			while (aux != NULL)
			{
				printf(" %s", aux->pk);
				aux = aux->prox;
			}
			printf("\n");
		}
		break;
	default:
		printf(OPCAO_INVALIDA);
		break;
	}
}