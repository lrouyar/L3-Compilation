//
// Created by clovis on 30/03/17.
//

#include "Pascal.h"

struct Variable* Pascal_run( struct Stack* stack, struct Env* env, struct FuncList* functions, struct Ast* ast, struct Env* local)
{
    if(ast == 0)
        return 0;
    if(TypeSystem_isInit() == false)
        TypeSystem_init();
    char nodeType = ast->nodetype;

    if(nodeType == 'C') // commandes
    {
        int ope = *(int*)ast->value;
        int tmp;
        struct Variable* tmp1;
        struct Variable* tmp2;
        struct Type* tmp3;
        struct Func* tmp4;
        struct Env* tmp5;
        struct Env* tmp7;
        struct Ast* tmp6;
        switch(ope) {
            case Se:
                Pascal_run(stack, env, functions, ast->left, local);
                Pascal_run(stack, env, functions, ast->right, local);
                Collector_clean(stack);
                break;
            case AfInd:
                tmp1 = Pascal_run(stack, env, functions, ast->left->left, local); // Tableau à modifier
                tmp2 = Pascal_run(stack, env, functions, ast->left->right, local); //Index a modifier
                tmp = Variable_arrayGet(tmp2, stack, 0);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local); //valeur a placer
                //TODO: check qu'on déborde pas
                if(tmp2->type->desc != ARRAY) {
                    Stack_setValue(stack, stack->adr[tmp1->value] + tmp, Variable_arrayGet(tmp2, stack, 0));
#ifdef DEBUG
                    printf("AfInd: %d <- %d (tmp: %d adr: %d)\n", stack->adr[tmp1->value]+tmp, Variable_arrayGet(tmp2, stack, 0), tmp, stack->adr[tmp1->value]);
#endif
                }
                else
                {
                    Stack_setValue(stack, stack->adr[tmp1->value] + tmp, tmp2->value);
#ifdef DEBUG
                    printf("AfInd (array): %d <- %d (tmp: %d adr: %d)\n", stack->adr[tmp1->value]+tmp, tmp2->value, tmp, stack->adr[tmp1->value]);
#endif
                }
                break;
            case Af:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                if(tmp1->type->desc != ARRAY) {
#ifdef DEBUG
                    printf("%s <- %d\n", (char*)ast->left->value, Variable_arrayGet(tmp2, stack, 0));
#endif
                    Variable_arraySet(tmp1, stack,0, Variable_arrayGet(tmp2, stack, 0));
                }
                else {
                    Variable_arrayCopy(stack, tmp1, tmp2);
                }
                Collector_clean(stack);
                break;
            case Sk:
                Collector_clean(stack);
                break;
            case If:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                if(Variable_arrayGet(tmp1, stack, 0) == true)
                {
                    Pascal_run(stack, env, functions, ast->right->left, local);
                } else
                {
                    Pascal_run(stack, env, functions, ast->right->right, local);
                }
                Collector_clean(stack);
                break;
            case Wh:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                while(Variable_arrayGet(tmp1, stack, 0) == true)
                {
                    Pascal_run(stack, env, functions, ast->right, local);
                    tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                }
                Collector_clean(stack);
                break;
            case CallFUNC:
                printf("Searching to call %s\n", (char*)ast->left->value);
                tmp4 = FuncList_search(functions, ((char*)ast->left->value));
                // création de l'environnement local
                // Ajout des variables arguments
                tmp5 = Env_concat(tmp4->disclaimer->args, tmp4->vars);
                tmp6 = ast->right;
                // Copie des valeurs des arguments dans le nouvel environnement
                for(int i = 0; i < tmp4->disclaimer->args->length; i++)
                {
                    tmp1 = Pascal_run(stack, env, functions, tmp6->right,local);
                    Env_set_value_index(tmp5, i, tmp1);
                    tmp6 = tmp6->left;
                }
                // execution de la fonction
                Pascal_run(stack, env, functions, tmp4->ast, tmp5);
                free(tmp5);
                Collector_clean(stack);
                break;
        }
        return 0;
    }
    else if(nodeType == 'E') // expressions
    {
        int ope = *(int*)ast->value;
        int tmp;
        struct Variable* tmp1;
        struct Variable* tmp2;
        struct Type* tmp3;
        struct Func* tmp4;
        struct Env* tmp5;
        struct Env* tmp7;
        struct Ast* tmp6;
        switch(ope)
        {
            case Pl:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = Variable_arrayGet(tmp1, stack, 0) + Variable_arrayGet(tmp2, stack, 0);
                tmp1 = Variable_arrayInit(stack, Type_INT,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Mo:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = Variable_arrayGet(tmp1, stack, 0) - Variable_arrayGet(tmp2, stack, 0);
                tmp1 = Variable_arrayInit(stack, Type_INT,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Mu:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = Variable_arrayGet(tmp1, stack, 0) * Variable_arrayGet(tmp2, stack, 0);
                tmp1 = Variable_arrayInit(stack, Type_INT,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Or:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = false;
                if(Variable_arrayGet(tmp1, stack, 0) == true || Variable_arrayGet(tmp2, stack, 0) == true)
                    tmp = true;
                tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Lt:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = false;
                if(Variable_arrayGet(tmp1, stack, 0) < Variable_arrayGet(tmp2, stack, 0))
                    tmp = true;
                tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Eq:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = false;
                if(Variable_arrayGet(tmp1, stack, 0) == Variable_arrayGet(tmp2, stack, 0))
                    tmp = true;
                tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case And:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp = false;
                if(Variable_arrayGet(tmp1, stack, 0) == true && Variable_arrayGet(tmp2, stack, 0) == true)
                    tmp = true;
                tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case Not:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp = !Variable_arrayGet(tmp1, stack, 0);
                tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
                Variable_arraySet(tmp1, stack, 0, tmp);
                tmp1->refs = 0;
                return tmp1;
            case NewAr:
                tmp3 = (struct Type*)ast->left->value;
                tmp = Variable_arrayGet(Pascal_run(stack, env, functions, ast->right, local), stack, 0);
                printf("New Array %d\n", tmp);
                tmp1 = Variable_arrayInit(stack, tmp3, tmp);
                tmp1->refs = 0;
                Stack_print(stack);
                return tmp1;
            case GetARR:
                tmp1 = Pascal_run(stack, env, functions, ast->left, local);
                tmp2 = Pascal_run(stack, env, functions, ast->right, local);
                tmp2 = Variable_arrayInit(stack, tmp1->type->child,1);
                tmp = Variable_arrayGet(tmp1, stack, Variable_arrayGet(tmp2, stack, 0));
                if(tmp1->type->child->desc != ARRAY) {
                    Variable_arraySet(tmp2, stack, 0, tmp);
                }
                else
                {
                    tmp2->value = tmp;
                    tmp2->array_set = false;
                }

                tmp2->refs = 0;
                return tmp2;
            case CallFUNC:
                printf("Searching to call %s\n", (char*)ast->left->value);
                tmp4 = FuncList_search(functions, ((char*)ast->left->value));
                if(tmp4 == 0)
                {
                    printf("Function not declared.\n");
                    exit(-1);
                }
                // création de l'environnement local
                // Ajout des variables arguments
                tmp5 = Env_concat(tmp4->disclaimer->args, tmp4->vars);
                // TODO: on vérifie que l'environnement global et local ne rentrent pas en collision
                // TODO: on vérfie que la variable réservée portant le nom de la fonction n'est pas redéclarée par l'user
                tmp6 = ast->right;
                // Copie des valeurs des arguments dans le nouvel environnement
                for(int i = 0; i < tmp4->disclaimer->args->length; i++)
                {
                    if(tmp6->right == 0)
                    {
                        printf("Too few arguments.\n");
                        exit(-1);
                    }
                    tmp1 = Pascal_run(stack, env, functions, tmp6->right, local);
                    if(Type_check(tmp1->type, Env_get_value_index(tmp4->disclaimer->args, i)->type) == false)
                    {
                        printf("Type mismatch.\n");
                        exit(-1);
                    }
                    Env_set_value_index(tmp5, i, tmp1);
                    tmp6 = tmp6->left;
                }
                // Ajout d'une variable portant le nom de la fonction faisant office de return
                Env_add_value(tmp5, (char*)ast->left->value, Variable_arrayInit(stack, tmp4->disclaimer->type, 0));
                // execution de la fonction
                printf("Calling... %p\n", tmp4->ast);
                Pascal_run(stack, env, functions, tmp4->ast, tmp5);
                printf("Done\n");
                // Récupération de la variable faisant office de return
                tmp1 = Env_get_value(tmp5, (char*)ast->left->value);
                printf("Returning value %d\n", Variable_arrayGet(tmp1, stack, 0));
                free(tmp5);
                tmp1->refs = 0;
                return tmp1;



        }
    }
    else if(nodeType == 'I') // constantes
    {
        int tmp = *(int*)ast->value;
        struct Variable* tmp1 = Variable_arrayInit(stack, Type_INT,1);
        Variable_arraySet(tmp1, stack, 0, tmp);
        tmp1->refs = 0;
        return tmp1;
    }
    else if(nodeType == 'B') // constantes
    {
        int tmp = *(int*)ast->value;
        struct Variable* tmp1 = Variable_arrayInit(stack, Type_BOOL,1);
        Variable_arraySet(tmp1, stack, 0, tmp);
        tmp1->refs = 0;
        return tmp1;
    }
    else if(nodeType == 'V')
    {
        struct Variable* res = 0;
        if(local != 0)
            res = Env_get_value(local, (char*)ast->value);
        if(res == 0)
            res = Env_get_value(env, (char*)ast->value);
        Variable_print(res, stack);
        return res;
    }
}

