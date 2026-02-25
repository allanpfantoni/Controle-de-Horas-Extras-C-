#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FUNC 100
#define MAX_HORAS 500
#define MAX_NOME 50
#define MAX_CARGO 50

typedef struct {
    int id;
    char nome[MAX_NOME];
    char cargo[MAX_CARGO];
    int nivel; 
    int supervisorId;
    int ativo;
} Funcionario;

typedef struct {
    int id;
    int funcId;
    char data[11];
    float horas;
    char descricao[200];
    int status; 
} RegistroHoras;

Funcionario funcs[MAX_FUNC];
RegistroHoras horasExtra[MAX_HORAS];
int contFunc = 0, contHoras = 0;
int proxFuncId = 1, proxHoraId = 1;

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausa() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}

Funcionario* acharFunc(int id) {
    for (int i = 0; i < contFunc; i++)
        if (funcs[i].id == id && funcs[i].ativo)
            return &funcs[i];
    return NULL;
}

void cadastrarFunc() {
    limparTela();
    printf("=== CADASTRO DE FUNCIONARIO ===\n\n");

    if (contFunc >= MAX_FUNC) {
        printf("Nao da para cadastrar mais funcionarios.\n");
        pausa();
        return;
    }

    Funcionario f;
    f.id = proxFuncId++;
    f.ativo = 1;

    printf("Nome: ");
    getchar();
    fgets(f.nome, MAX_NOME, stdin);
    f.nome[strcspn(f.nome, "\n")] = 0;

    printf("Cargo: ");
    fgets(f.cargo, MAX_CARGO, stdin);
    f.cargo[strcspn(f.cargo, "\n")] = 0;

    printf("Nivel (1-Funcionario 2-Supervisor 3-Gestor): ");
    scanf("%d", &f.nivel);

    if (f.nivel == 1) {
        printf("ID do supervisor (0 se nao tiver): ");
        scanf("%d", &f.supervisorId);
    } else {
        f.supervisorId = 0;
    }

    funcs[contFunc++] = f;
    printf("\nFuncionario cadastrado! ID=%d\n", f.id);
    pausa();
}

void listarFuncs() {
    limparTela();
    printf("=== LISTA DE FUNCIONARIOS ===\n\n");
    printf("ID   Nome                       Cargo               Nivel\n");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < contFunc; i++) {
        if (funcs[i].ativo) {
            char* nivelStr = funcs[i].nivel == 1 ? "Funcionario" : funcs[i].nivel == 2 ? "Supervisor" : "Gestor";
            printf("%-4d %-25s %-20s %-10s\n", funcs[i].id, funcs[i].nome, funcs[i].cargo, nivelStr);
        }
    }
    pausa();
}

void registrarHoras() {
    limparTela();
    printf("=== REGISTRO DE HORAS EXTRAS ===\n\n");

    if (contHoras >= MAX_HORAS) {
        printf("Nao da para registrar mais horas.\n");
        pausa();
        return;
    }

    RegistroHoras r;
    r.id = proxHoraId++;
    r.status = 0;

    printf("ID do funcionario: ");
    scanf("%d", &r.funcId);
    if (!acharFunc(r.funcId)) {
        printf("Funcionario nao encontrado!\n");
        pausa();
        return;
    }

    printf("Data (DD/MM/AAAA): ");
    scanf("%s", r.data);
    printf("Horas trabalhadas: ");
    scanf("%f", &r.horas);

    printf("Descricao da atividade: ");
    getchar();
    fgets(r.descricao, 200, stdin);
    r.descricao[strcspn(r.descricao, "\n")] = 0;

    horasExtra[contHoras++] = r;
    printf("\nHoras extras registradas! ID=%d\n", r.id);
    pausa();
}

void aprovarSup() {
    limparTela();
    printf("=== APROVACAO SUPERVISOR ===\n");
    printf("Digite seu ID de supervisor: ");
    int supId;
    scanf("%d", &supId);

    Funcionario* sup = acharFunc(supId);
    if (!sup || sup->nivel < 2) {
        printf("Acesso negado!\n");
        pausa();
        return;
    }

    int achou = 0;
    for (int i = 0; i < contHoras; i++) {
        if (horasExtra[i].status == 0) { // pendente
            Funcionario* f = acharFunc(horasExtra[i].funcId);
            if (f) { 
                printf("ID:%d Funcionario:%s Data:%s Horas:%.2f\n", horasExtra[i].id, f->nome, horasExtra[i].data, horasExtra[i].horas);
                printf("1-Aprovar 2-Rejeitar: ");
                int o;
                scanf("%d", &o);
                if (o == 1) horasExtra[i].status = 1;
                else horasExtra[i].status = 2;
                achou = 1;
            }
        }
    }

    if (!achou) printf("Nenhum registro pendente.\n");
    pausa();
}

void aprovarGestor() {
    limparTela();
    printf("=== APROVACAO GESTOR ===\n");
    printf("Digite seu ID de gestor: ");
    int gId;
    scanf("%d", &gId);

    Funcionario* g = acharFunc(gId);
    if (!g || g->nivel != 3) {
        printf("Acesso negado!\n");
        pausa();
        return;
    }

    int achou = 0;
    for (int i = 0; i < contHoras; i++) {
        if (horasExtra[i].status == 1) { // aprovadas pelo supervisor
            Funcionario* f = acharFunc(horasExtra[i].funcId);
            if (f) {
                printf("ID:%d Funcionario:%s Data:%s Horas:%.2f\n", horasExtra[i].id, f->nome, horasExtra[i].data, horasExtra[i].horas);
                printf("1-Aprovar 2-Rejeitar: ");
                int o;
                scanf("%d", &o);
                if (o == 1) horasExtra[i].status = 1;
                else horasExtra[i].status = 2;
                achou = 1;
            }
        }
    }

    if (!achou) printf("Nenhum registro pendente.\n");
    pausa();
}

void relatorio() {
    limparTela();
    printf("=== RELATORIO DE HORAS EXTRAS ===\n\n");

    float total = 0;
    for (int i = 0; i < contHoras; i++) {
        Funcionario* f = acharFunc(horasExtra[i].funcId);
        if (f) {
            char* statusStr = horasExtra[i].status == 0 ? "Pendente" : horasExtra[i].status == 1 ? "Aprovado" : "Rejeitado";
            printf("ID:%d Funcionario:%s Data:%s Horas:%.2f Status:%s\n",
                   horasExtra[i].id, f->nome, horasExtra[i].data, horasExtra[i].horas, statusStr);
            if (horasExtra[i].status == 1) total += horasExtra[i].horas;
        }
    }
    printf("\nTotal de horas aprovadas: %.2f\n", total);
    pausa();
}

int main() {
    int op;
    do {
        limparTela();
        printf("=== SISTEMA DE HORAS EXTRAS ===\n");
        printf("1-Cadastrar Funcionario\n");
        printf("2-Listar Funcionarios\n");
        printf("3-Registrar Horas Extras\n");
        printf("4-Aprovar Supervisor\n");
        printf("5-Aprovar Gestor\n");
        printf("6-Relatorio\n");
        printf("0-Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        switch (op) {
            case 1: cadastrarFunc(); break;
            case 2: listarFuncs(); break;
            case 3: registrarHoras(); break;
            case 4: aprovarSup(); break;
            case 5: aprovarGestor(); break;
            case 6: relatorio(); break;
            case 0: printf("Saindo do sistema...\n"); break;
            default: printf("Opcao invalida!\n"); pausa();
        }
    } while (op != 0);
    return 0;
}