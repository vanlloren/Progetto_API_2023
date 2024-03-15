#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NEWGAMEBUFFER 18

typedef struct nodo{
    char *word;
    int valid;
    struct nodo *father;
    struct nodo *left;
    struct nodo *right;
    /*char *color;*/
} nodo_t;

typedef nodo_t *Ptr_nodo;

typedef struct pos_list{
    int position;
    struct pos_list *next;
} pos_list_t;

typedef pos_list_t *Ptr_pos_list;

typedef struct letter{
    int belong;
    struct pos_list *valid_pos;
    struct pos_list *invalid_pos;
    int min;
    int ex;
    int exact;
}letter_t;

Ptr_pos_list distruggiLista(Ptr_pos_list l){
    Ptr_pos_list tmp;

    while(l!=NULL){
        tmp=l;
        l=l->next;
        free(tmp);
    }

    return NULL;
}

Ptr_nodo BST_Insert(Ptr_nodo Tree, Ptr_nodo z){		/*Tree è di tipo Ptr_nodo */
    Ptr_nodo y,x;

    y=NULL;
    x=Tree;

    while(x!=NULL){
        y=x;
        if(strcmp(z->word, x->word)<0){
            x=x->left;
        }else{
            x=x->right;
        }
    }
    z->father=y;
    if(y==NULL){
        Tree=z;
    }else if(strcmp(z->word, y->word)<0){
        y->left=z;
    }else{
        y->right=z;
    }

    z->left=NULL;
    z->right=NULL;

    return Tree;
}

void Inorder_Tree_Walk_fix(Ptr_nodo x){
    if(x!=NULL){
        Inorder_Tree_Walk_fix(x->left);
        if(x->valid==0){
            x->valid=1;
        }
        Inorder_Tree_Walk_fix(x->right);
    }
}

Ptr_nodo setup_list_mod(Ptr_nodo Tree, int k){
    char *word;
    Ptr_nodo z;



    word=malloc(sizeof(char)*(k+1));
    if(scanf("%[^\n]%*c", word)==0){
        return NULL;
    }

    do{
        z=malloc(sizeof(nodo_t));
        if(z!=NULL){
            z->word=malloc(sizeof(char)*(k+1));

            strcpy(z->word, word);
            z->valid=1;
            z->father=NULL;
            z->left=NULL;
            z->right=NULL;

        }
        Tree=BST_Insert(Tree, z);

        if(scanf("%[^\n]%*c", word)==0){
            return NULL;
        }
    }while(word[0]!='+');

    free(word);

    return Tree;
}

Ptr_nodo setup_list_17_mod(Ptr_nodo Tree, int k){
    char word[NEWGAMEBUFFER];
    Ptr_nodo z;



    if(scanf("%[^\n]%*c", word)==0){
        return NULL;
    }

    do{
        z=malloc(sizeof(nodo_t));
        if(z!=NULL){
            z->word=malloc(sizeof(char)*NEWGAMEBUFFER);
            strcpy(z->word, word);
            z->valid=1;
            z->father=NULL;
            z->left=NULL;
            z->right=NULL;

        }
        Tree=BST_Insert(Tree, z);

        if(scanf("%[^\n]%*c", word)==0){
            return NULL;
        }
    }while(word[0]!='+');


    return Tree;
}

void free_all(Ptr_nodo x){
    if(x!=NULL){
        free_all(x->left);
        free_all(x->right);
        free(x);
    }
}

void Inorder_Tree_Walk(Ptr_nodo x){
    if(x!=NULL){
        Inorder_Tree_Walk(x->left);
        printf("%s\n", x->word);
        Inorder_Tree_Walk(x->right);
    }
}

void Inorder_Tree_Walk_free_word(Ptr_nodo x){
    if(x!=NULL){
        Inorder_Tree_Walk_free_word(x->left);
        free(x->word);
        Inorder_Tree_Walk_free_word(x->right);
    }
}

Ptr_nodo setup_list_14(Ptr_nodo Tree, int k){
    char *word;
    Ptr_nodo z;


    word=malloc(sizeof(char)*(k+1));
    if(scanf("%[^\n]%*c", word)==0){
        return NULL;
    }

    while(word[0]!='+'){
        z=malloc(sizeof(nodo_t));
        if(z!=NULL){
            z->word=malloc(sizeof(char)*(k+1));
            if(z->word!=NULL){
                strcpy(z->word, word);
                z->valid=1;
                z->father=NULL;
                z->left=NULL;
                z->right=NULL;

            }
        }
        Tree=BST_Insert(Tree, z);
        if(scanf("%[^\n]%*c", word)==0){
            return NULL;
        }
    }

    free(word);

    return Tree;
}

Ptr_nodo setup_list(Ptr_nodo Tree, int k){ /* va modificata come setup_list_14*/
    char word1[NEWGAMEBUFFER];
    Ptr_nodo z=NULL;



    if(scanf("%[^\n]%*c", word1)==0){
        return NULL;
    }


    while(word1[0]!='+'){
        z=malloc(sizeof(nodo_t));
        if(z!=NULL){
            z->word=malloc(sizeof(char)*(NEWGAMEBUFFER));
            if(z->word!=NULL){
                strcpy(z->word, word1);
                z->valid=1;
                z->father=NULL;
                z->left=NULL;
                z->right=NULL;

            }
        }
        Tree=BST_Insert(Tree, z);

        if(scanf("%[^\n]%*c", word1)==0){
            return NULL;
        }
    }

    return Tree;
}

void Inorder_Tree_Walk_mod(Ptr_nodo x){
    if(x!=NULL){
        Inorder_Tree_Walk_mod(x->left);
        if(x->valid==1){
            printf("%s\n", x->word);
        }
        Inorder_Tree_Walk_mod(x->right);
    }
}

int ricerca_cella(char x){
    int i;

    if(x==45){
        i=0;
    }else if(47<x && x<58){
        i=x%47;
    }else if(64<x && x<91){
        i=x%=54;
    }else if(x==95){
        i=37;
    }else{
        i=x%59;
    }

    return i;
}

void fix_valid_words(letter_t criteria[], Ptr_nodo Tree, int len, char *known){
    int i, j, h, k, use, index, found;
    Ptr_pos_list tmp;

    if(Tree->valid==1){
        for(h=0;h<64;h++){
            if(criteria[h].belong==2){
                use=0;
                for(k=0;k<len;k++){
                    if(h==0){
                        if(Tree->word[k]==45){
                            use++;
                        }
                    }else if(1<=h && h<=10){
                        if(Tree->word[k]==47+h){
                            use++;
                        }
                    }else if(11<=h && h<=36){
                        if(Tree->word[k]==54+h){
                            use++;
                        }
                    }else if(h==37){
                        if(Tree->word[k]==95){
                            use++;
                        }
                    }else{
                        if(Tree->word[k]==59+h){
                            use++;
                        }
                    }
                }
                if(use==0){
                    Tree->valid=0;
                    return;
                }
            }
        }
        for(i=0;i<len;i++){
            if(known[i]!='?'){
                if(Tree->word[i]!=known[i]){
                    Tree->valid=0;
                    return;
                }
            }
        }
        for(i=0;i<len;i++){
            index=ricerca_cella(Tree->word[i]);
            if(criteria[index].belong==0){
                Tree->valid=0;
                return;
            }
            if(criteria[index].ex==1){
                found=0;
                for(j=0;j<len;j++){
                    if(Tree->word[j]==Tree->word[i]){
                        found++;
                    }
                }
                if(found!=criteria[index].exact){
                    Tree->valid=0;
                    return;
                }
            }else{
                found=0;
                for(j=0;j<len;j++){
                    if(Tree->word[j]==Tree->word[i]){
                        found++;
                    }
                }
                if(found<criteria[index].min){
                    Tree->valid=0;
                    return;
                }
            }
            for(tmp=criteria[index].valid_pos;tmp!=NULL && tmp->position<=i;tmp=tmp->next){
                if(Tree->word[tmp->position]!=Tree->word[i]){
                    Tree->valid=0;
                    return;
                }
            }
            for(tmp=criteria[index].invalid_pos;tmp!=NULL && tmp->position<=i;tmp=tmp->next){
                if(tmp->position==i){
                    Tree->valid=0;
                    return;
                }
            }
        }

    }

    return;

}

void Inorder_Tree_Walk_crit(Ptr_nodo x, letter_t criteria[], int k, char *known){

    if(x!=NULL){
        Inorder_Tree_Walk_crit(x->left, criteria, k, known);
        fix_valid_words(criteria, x, k, known);
        Inorder_Tree_Walk_crit(x->right, criteria, k, known);
    }
}

void fix_criteria(char *result, letter_t criteria[], char *word, int len, char *known){
    int i, j, index, test, minor, bar;
    Ptr_pos_list tmp, prec, sup;



    for(i=0;i<len;i++){
        index=ricerca_cella(word[i]);
        if(criteria[index].belong!=0){
            test=0;
            minor=0;
            bar=0;
            for(j=0;j<len;j++){
                if(word[j]==word[i] && result[j]!='/'){
                    minor++;
                }
                if(word[j]==word[i] && result[j]!='/'){
                    test=1;
                }
                if(word[j]==word[i] && result[j]=='/'){
                    bar=1;
                }
            }
            if(test!=0){
                criteria[index].belong=2;
                if(result[i]=='+'){
                    known[i]=word[i];
                    tmp=malloc(sizeof(pos_list_t));
                    if(tmp!=NULL){
                        tmp->position=i;
                        tmp->next=NULL;
                        if(criteria[index].valid_pos==NULL){
                            criteria[index].valid_pos=tmp;
                        }else{
                            prec=criteria[index].valid_pos;
                            while(((prec->position)<(tmp->position))&& prec->next!=NULL){
                                prec=prec->next;
                            }
                            if(prec->next==NULL){
                                prec->next=tmp;
                            }else{
                                sup=prec;
                                prec=tmp;
                                tmp->next=sup;
                            }
                        }
                    }
                    /*
                    for(h=0;h<64;h++){
                        if(h!=index){
                            if(criteria[h].belong!=0){
                                criteria[h].invalid_pos=Insert(criteria[h].invalid_pos, i);
                            }
                        }
                    }
                    */
                }else if(result[i]=='|'){
                    tmp=malloc(sizeof(pos_list_t));
                    if(tmp!=NULL){
                        tmp->position=i;
                        tmp->next=NULL;
                        if(criteria[index].invalid_pos==NULL){
                            criteria[index].invalid_pos=tmp;
                        }else{
                            prec=criteria[index].invalid_pos;
                            while(((prec->position)<(tmp->position))&& prec->next!=NULL){
                                prec=prec->next;
                            }
                            if(prec->next==NULL){
                                prec->next=tmp;
                            }else{
                                sup=prec;
                                prec=tmp;
                                tmp->next=sup;
                            }
                        }
                    }
                }
                if(bar==0){
                    if(minor>criteria[index].min){
                        criteria[index].min=minor;
                    }
                }else{
                    criteria[index].exact=minor;
                    criteria[index].ex=1;
                }
            }
            else{
                criteria[index].belong=0;

            }
        }
    }
}

void compare(char *target, char *word, char *result, int k){

/*	idea più semplice: copia di target e word, primo controllo segno i '+' poi segno le '|' modificando le stringhe */

    char *t_copy, *w_copy;
    int i, j, found;

    for(i=0;i<k;i++){
        result[i]='/';
    }

    t_copy=malloc(sizeof(char)*(k+1));
    w_copy=malloc(sizeof(char)*(k+1));

    strcpy(t_copy, target);
    strcpy(w_copy, word);

    for(i=0;i<k;i++){
        if(t_copy[i]==w_copy[i]){
            result[i]='+';
            t_copy[i]='?';
            w_copy[i]='?';
        }
    }

    for(i=0;i<k;i++){
        if(result[i]!='+'){
            found=0;
            for(j=0;j<k && found==0;j++){
                if(t_copy[j]==w_copy[i]){
                    found=1;
                    result[i]='|';
                    t_copy[j]='?';
                    w_copy[i]='?';
                }
            }
        }
    }

    free(t_copy);
    free(w_copy);
}

void remaining(Ptr_nodo Tree, int *i){
    if(Tree!=NULL){
        remaining(Tree->left, i);
        if(Tree->valid==1){
            (*i)=(*i)+1;
        }
        remaining(Tree->right, i);
    }
}

int confronta_parole(char *target, char *word, Ptr_nodo Tree, letter_t criteria[], char *known){
    char *result;
    int alt, i, k;
    int	*j;

    k=strlen(word);
    result=malloc(sizeof(char)*(k+1));


    compare(target, word, result, k);

    alt=0;
    for(i=0;i<k;i++){
        if(result[i]!='+'){
            alt=1;
        }
    }

    if(alt==0){
        free(result);
        return 1;
    }

    for(i=0;i<k;i++){
        printf("%c", result[i]);
    }

    fix_criteria(result, criteria, word, k, known);
    Inorder_Tree_Walk_crit(Tree, criteria, k, known);

    j=malloc(sizeof(int));
    if(j!=NULL){
        *j=0;
        remaining(Tree, j);
        printf("\n");
        printf("%d\n", *j);
    }

    free(j);
    free(result);

    return 0;
}

int is_valid(char *word, Ptr_nodo Tree){
    if(Tree==NULL){
        return 0;
    }
    if(strcmp(word, Tree->word)<0){
        return is_valid(word, Tree->left);
    }else if(strcmp(word, Tree->word)>0){
        return is_valid(word, Tree->right);
    }

    return 1;
}

void new_game(int k, Ptr_nodo valid_words){
    char *target, *word, *known;
    int i, n, win, val;
    letter_t criteria[64];


    target=malloc(sizeof(char)*(k+1));
    if(target!=NULL){

        if(scanf("%[^\n]%*c", target)==0){
            return;
        }
    }


    if(scanf("%d%*c", &n)==0){
        return;
    }

    for(i=0;i<64;i++){
        criteria[i].belong=1;
        criteria[i].min=0;
        criteria[i].ex=0;
        criteria[i].valid_pos=NULL;
        criteria[i].invalid_pos=NULL;
    }

    win=0;
    known=malloc(sizeof(char)*(k+1));
    for(i=0;i<k;i++){
        known[i]='?';
    }

    while(n>0){
        if(k<17){
            word=malloc(sizeof(char)*NEWGAMEBUFFER);
        }else{
            word=malloc(sizeof(char)*(k+1));
        }

        if(scanf("%[^\n]%*c", word)==0){
            return;
        }
        if(word[0]!='+'){
            val=is_valid(word, valid_words);
            if(val==0){
                printf("not_exists\n");
            }else{
                win=confronta_parole(target, word, valid_words, criteria, known);
                if(win==1){
                    printf("ok\n");
                    n=0;
                }else{
                    n=n-1;
                }
            }
        }else{
            if(word[1]=='i'){
                valid_words=setup_list_17_mod(valid_words, k);
            }else{
                Inorder_Tree_Walk_mod(valid_words);
            }
        }
        if(k<17){
            for(i=0;i<NEWGAMEBUFFER;i++){
                word[i]='e';
            }
        }else{
            for(i=0;i<k;i++){
                word[i]='e';
            }
        }
        free(word);
    }
    if(win!=1){
        printf("ko\n");
    }

    for(i=0;i<64;i++){
        distruggiLista(criteria[i].valid_pos);
        distruggiLista(criteria[i].invalid_pos);
    }
    free(target);
    free(known);
}

int main(){
    int k=0, i;
    char *start, *word;
    Ptr_nodo valid_words;



    if(scanf("%d%*c", &k)==0){
        return 0;
    }

    word=malloc(sizeof(char)*(k+1));

    if(scanf("%[^\n]%*c", word)==0){
        return 0;
    }

    valid_words=malloc(sizeof(nodo_t));
    if(valid_words!=NULL){
        valid_words->word=malloc(sizeof(char)*(k+1));
        if(valid_words->word!=NULL){
            strcpy(valid_words->word, word);
            valid_words->valid=1;
            valid_words->father=NULL;
            valid_words->left=NULL;
            valid_words->right=NULL;

        }
    }

    if(k>13){								/*se ho parole più corte di 14 caratteri necessito di un array da 15 celle minimo per contenere '+nuova_partita'*/
        valid_words=setup_list_14(valid_words, k);
    }else{
        valid_words=setup_list(valid_words, k);
    }




    if(k<17){
        start=malloc(sizeof(char)*NEWGAMEBUFFER);
        do{
            free(start);
            new_game(k, valid_words);
            Inorder_Tree_Walk_fix(valid_words);
            start=malloc(sizeof(char)*NEWGAMEBUFFER);

            if(scanf("%[^\n]%*c", start)!=0){
                if(start[0]=='+' && start[1]=='i'){
                    valid_words=setup_list_17_mod(valid_words, k);

                    if(scanf("%[^\n]%*c", start)==0){
                        for(i=0;i<NEWGAMEBUFFER;i++){
                            start[i]='e';
                        }
                    }
                }
            }else{
                for(i=0;i<NEWGAMEBUFFER;i++){
                    start[i]='e';
                }
            }
        }while(start[0]=='+' && start[1]=='n');
    }else{
        start=malloc(sizeof(char)*(k+1));
        do{
            free(start);
            new_game(k, valid_words);
            Inorder_Tree_Walk_fix(valid_words);
            start=malloc(sizeof(char)*(k+1));

            if(scanf("%[^\n]%*c", start)!=0){
                if(start[0]=='+' && start[1]=='i'){
                    valid_words=setup_list_17_mod(valid_words, k);
                    if(scanf("%[^\n]%*c", start)==0){
                        for(i=0;i<k;i++){
                            start[i]='e';
                        }
                    }
                }
            }else{
                for(i=0;i<k;i++){
                    start[i]='e';
                }
            }
        }while(start[0]=='+' && start[1]=='n');
    }



    free(word);
    Inorder_Tree_Walk_free_word(valid_words);
    free_all(valid_words);
    free(start);

    return 0;
}
