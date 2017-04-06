//
// Created by clovis on 30/03/17.
//

#include "Pascal.h"

struct Ast* Pascal_Ast_init_leaf(int nodetype, int value)
{
    struct Ast* ast = malloc(sizeof(struct Ast));
    //printf("%p Leaf %c: ", ast, nodetype);
    ast->nodetype = nodetype;
    ast->left = 0;
    ast->right = 0;
    int* tmp = malloc(sizeof(int));
    *tmp = value;
    ast->value=tmp;
    //printf("\n");
    return ast;
}


struct Variable* Pascal_run( struct Stack* stack, struct Env* env, struct FuncList* functions, struct Ast* ast)
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
                Pascal_run(stack, env, functions, ast->left);
                Pascal_run(stack, env, functions, ast->right);
                break;
            case Af:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);

                if(Type_check(tmp1->type, tmp2->type) == false)
                {
                    printf("Cant affect non-identical types.\n");
                    exit(-1);
                }
                printf("Done running\n");
                if(tmp1->type->type != ARRAY) {
                    printf("%s <- %d\n", (char*)ast->left->value,  Variable_get(tmp2));
                    Variable_set(tmp1, Variable_get(tmp2));
                }
                else {
                    printf("Affecting array\n");
                    Variable_arrayCopy(stack, tmp1, tmp2);
                }
                break;
            case Sk:
                break;
            case If:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                if(tmp1->type->type == ARRAY)
                {
                    printf("Array variable cant be checked as boolean.\n");
                    exit(-1);
                }
                if(Variable_get(tmp1) == true)
                {
                    Pascal_run(stack, env, functions, ast->right->left);
                } else
                {
                    Pascal_run(stack, env, functions, ast->right->right);
                }
                break;
            case Wh:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                if(tmp1->type->type == ARRAY)
                {
                    printf("Array variable cant be checked as boolean.\n");
                    exit(-1);
                }
                while(Variable_get(tmp1) == true)
                {
                    Pascal_run(stack, env, functions, ast->right);
                    tmp1 = Pascal_run(stack, env, functions, ast->left);
                    if(tmp1->type->type == ARRAY)
                    {
                        printf("Array variable cant be checked as boolean.\n");
                        exit(-1);
                    }
                }
                break;
            case CallFUNC:
                tmp4 = FuncList_search(functions, ((char*)ast->left->value));
                if(tmp4 == 0)
                {
                    printf("Function not declared.\n");
                    exit(-1);
                }
                // création de l'environnement local
                tmp7 = Env_concat(env, 0);
                tmp5 = Env_concat(tmp7, tmp4->disclaimer->args);
                tmp6 = ast->right;
                for(int i = 0; i < tmp4->disclaimer->args->length; i++)
                {
                    if(tmp6->right == 0)
                    {
                        printf("Too few arguments.\n");
                        exit(-1);
                    }
                    tmp1 = Pascal_run(stack, env, functions, tmp6->right);
                    if(Type_check(tmp1->type, Env_get_value_index(tmp4->disclaimer->args, i)->type) == false)
                    {
                        printf("Type mismatch.\n");
                        exit(-1);
                    }
                    Env_set_value_index(tmp5, env->length + i, tmp1);
                    tmp6 = tmp6->left;
                }
                // execution de la fonction
                Pascal_run(stack, tmp5, functions, tmp4->ast);
                // copie des variables globales de l'environnement locale
                for(int i = 0; i < env->length; i++)
                {
                    Env_set_value_index(env, i, Env_get_value_index(tmp5, i));
                }
                free(tmp5);
                free(tmp7);
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
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = Variable_get(tmp1) + Variable_get(tmp2);
                tmp1 = Variable_init(Type_INT);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Mo:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = Variable_get(tmp1) - Variable_get(tmp2);
                tmp1 = Variable_init(Type_INT);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Mu:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = Variable_get(tmp1) * Variable_get(tmp2);
                tmp1 = Variable_init(Type_INT);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Or:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = false;
                if(Variable_get(tmp1) == true || Variable_get(tmp2) == true)
                    tmp = true;
                tmp1 = Variable_init(Type_BOOL);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Lt:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = false;
                if(Variable_get(tmp1) < Variable_get(tmp2))
                    tmp = true;
                tmp1 = Variable_init(Type_BOOL);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Eq:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = false;
                if(Variable_get(tmp1) == Variable_get(tmp2))
                    tmp = true;
                tmp1 = Variable_init(Type_BOOL);
                Variable_set(tmp1, tmp);
                return tmp1;
            case And:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                tmp = false;
                if(Variable_get(tmp1) == true && Variable_get(tmp2) == true)
                    tmp = true;
                tmp1 = Variable_init(Type_BOOL);
                Variable_set(tmp1, tmp);
                return tmp1;
            case Not:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                tmp = !Variable_get(tmp1);
                tmp1 = Variable_init(Type_BOOL);
                Variable_set(tmp1, tmp);
                return tmp1;
            case NewAr:
                tmp3 = (struct Type*)ast->left->value;
                if(tmp3 < 0)
                {
                    printf("Cant create array of negative size.\n");
                    exit(-1);
                }
                return Variable_arrayInit(tmp3, stack, Variable_get(Pascal_run(stack, env, functions, ast->right)));
            case GetARR:
                tmp1 = Pascal_run(stack, env, functions, ast->left);
                if(tmp1->type->type != ARRAY)
                {
                    printf("Invalid operation on non-array variable.\n");
                    exit(-1);
                }
                tmp2 = Pascal_run(stack, env, functions, ast->right);
                if(tmp2->type->type == ARRAY)
                {
                    printf("Index array must be numeric.\n");
                    exit(-1);
                }
                return Variable_arrayGet(tmp1, stack, Variable_get(tmp2));
            case CallFUNC:
                tmp4 = FuncList_search(functions, ((char*)ast->left->value));
                if(tmp4 == 0)
                {
                    printf("Function not declared.\n");
                    exit(-1);
                }
                // création de l'environnement local
                tmp7 = Env_concat(env, 0);
                tmp5 = Env_concat(tmp7, tmp4->disclaimer->args);
                tmp6 = ast->right;
                for(int i = 0; i < tmp4->disclaimer->args->length; i++)
                {
                    if(tmp6->right == 0)
                    {
                        printf("Too few arguments.\n");
                        exit(-1);
                    }
                    tmp1 = Pascal_run(stack, env, functions, tmp6->right);
                    if(Type_check(tmp1->type, Env_get_value_index(tmp4->disclaimer->args, i)->type) == false)
                    {
                        printf("Type mismatch.\n");
                        exit(-1);
                    }
                    Env_set_value_index(tmp5, env->length + i, tmp1);
                    tmp6 = tmp6->left;
                }
                // execution de la fonction
                tmp1 = Pascal_run(stack, tmp5, functions, tmp4->ast);
                if(Type_check(tmp1->type, tmp4->disclaimer->type) == false)
                {
                    printf("Wrong return type.\n");
                    exit(-1);
                }
                // copie des variables globales de l'environnement locale
                for(int i = 0; i < env->length; i++)
                {
                    Env_set_value_index(env, i, Env_get_value_index(tmp5, i));
                }
                free(tmp5);
                free(tmp7);
                break;



        }
    }
    else if(nodeType == 'I') // constantes
    {
        int tmp = *(int*)ast->value;
        struct Variable* tmp1 = Variable_init(Type_INT);
        Variable_set(tmp1, tmp);
        return tmp1;
    }
    else if(nodeType == 'B') // constantes
    {
        int tmp = *(int*)ast->value;
        struct Variable* tmp1 = Variable_init(Type_BOOL);
        Variable_set(tmp1, tmp);
        return tmp1;
    }
    else if(nodeType == 'V')
    {
        return Env_get_value(env, (char*)ast->value);
    }
}

