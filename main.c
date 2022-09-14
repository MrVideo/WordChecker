#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 64
#define RED 'r'
#define BLACK 'b'

struct node {
    struct node *next;
    char data[];
};

struct res_node {
    char c;
    int index;
    int correct;
    int count;
    int exact;
    struct res_node *next;
};

struct tnode {
    struct tnode *left, *right, *parent;
    char color;
    char data[];
};

typedef struct node *list;
typedef struct res_node *res_list;
typedef struct tnode *tree;

// List management functions
int list_size(list l);
list add_sort(list l, const char *new_data, int len);
list add(list l, const char *new_data, int len);
void print(list l);
res_list add_res(res_list r, char c, int index, int correct, int count, int exact);
list destroy(list l);
res_list res_destroy(res_list r);

// Red-Black Tree management functions
void print_tree(tree t);
tree rbt_search(tree t, const char data[]);
void rbt_insert(tree *t, const char *data, int len);
void left_rotate(tree *root, tree *node);
void right_rotate(tree *root, tree *node);
void rbt_insert_fixup(tree *t, tree *node_to_fix);
void add_to_list(tree *t, list *f, res_list *r, int len);

// Game functions
void add_new_words(tree *t, list *f, res_list *r, int len, int f_created);
void word_check(char *password, char *buffer, char *guide, int len, int *tp, short *pc, short *wc, list *f, res_list *r,
                int f_created);
void occurrences_check(list *f, char c, int count, int strict);
void new_words_check(res_list *r, list *f, char *new_word, int len);
void char_delete(list *f, char c, int correct);
void char_count(short *bc, const char *word, int len);
short char_check(short *bc, char c);
void delete_index(int i, char c, list *f, int correct);

int main() {
    // Variable declarations
    char *buffer = calloc(BUFSIZE, sizeof(char));
    char *password = NULL, *guide = NULL;
    int word_length, cmd_chk = 0, restart, ok = 0, tries = 0, f_created = 0;
    short pc[64] = {0}, wc[64] = {0};
    tree wordlist = NULL;
    list filtered = NULL;
    res_list restrictions = NULL;

    // Declaration of pointers to tree and lists
    tree *tw = &wordlist;
    list *lf = &filtered;
    res_list *lr = &restrictions;

    // -- Game setup --
    // This is only run once.

    // Saves the word length decided by the user.
    word_length = (int)strtol(fgets(buffer, BUFSIZE, stdin), NULL, 10);

    // Starts saving new words to the main word list.
    if(word_length > 0) {
        password = malloc(sizeof(char) * (word_length + 1));
        guide = malloc(sizeof(char) * (word_length + 1));
        while(!cmd_chk) {
            if(scanf("%s", buffer) != 0) {
                if(strcmp(buffer, "+nuova_partita") == 0)
                    cmd_chk = 1;
                else if(strcmp(buffer, "+inserisci_inizio") == 0)
                    add_new_words(tw, lf, lr, word_length, f_created);
                else rbt_insert(tw, buffer, word_length);
                memset(buffer, 0, BUFSIZE);
            }
        }
    }

    // If the word has been input before, the password is saved.
    if(scanf("%s", buffer) != 0) {
        strncpy(password, buffer, word_length + 1);
        char_count(pc, password, word_length);
    }

    memset(buffer, 0, BUFSIZE);

    // Asks the user for the number of tries in the game.
    if(scanf("%d", &tries) != 0) {

        // -- Main game loop --
        // This is run until EOF is read from stdin.

        while(1) {
            // Resets the restart variable for future restarts.
            restart = 0;
            // Checks for EOF to terminate program
            if(scanf("%s", buffer) == EOF)
                break;
            else {
                // If the "+inserisci_inizio" command is passed, new words are added to the wordlist through
                // the "add_new_words" function.
                if(strcmp(buffer, "+inserisci_inizio") == 0) {
                    add_new_words(tw, lf, lr, word_length, f_created);
                    memset(buffer, 0, BUFSIZE);
                    continue;
                }
                // If "+stampa_filtrate" is called, the filtered list is printed on the screen.
                else if(strcmp(buffer, "+stampa_filtrate") == 0) {
                    if(!f_created) print_tree(wordlist);
                    else print(filtered);
                    memset(buffer, 0, BUFSIZE);
                    continue;
                }
                // If the word is not of the correct length, or it does not appear in the wordlist,
                // no tries are consumed and the user can retry.
                else if(strlen(buffer) != word_length || rbt_search(wordlist, buffer) == NULL) {
                    printf("not_exists\n");
                    memset(buffer, 0, BUFSIZE);
                    continue;
                }
                // If none of the checks before this get triggered, the word is checked for correctness.
                else {
                    if(strcmp(password, buffer) == 0) ok = 1;
                    else if(tries >= 0) {
                        memset(wc, 0, BUFSIZE * sizeof(short));
                        char_count(wc,buffer, word_length);
                        word_check(password, buffer, guide, word_length, &tries, pc, wc, lf, lr, f_created);
                        if(!f_created) {
                            add_to_list(tw, lf, lr, word_length);
                            f_created = 1;
                        }
                        memset(buffer, 0, BUFSIZE);
                        for(int i = 0; i < word_length; ++i)
                            printf("%c", guide[i]);
                        printf("\n%d\n", list_size(filtered));
                        if(tries != 0) continue;
                    }
                }
            }

            // Prints the outcome and resets the game.
            if(ok || tries == 0) {
                if(ok) printf("ok\n");
                if(!tries) printf("ko\n");
                ok = 0;
                memset(buffer, 0, BUFSIZE);
                restrictions = res_destroy(restrictions);
                filtered = destroy(filtered);
                f_created = 0;
                // The user can manage settings while in this loop, until they decide to start a new game.
                while(!restart) {
                    if(scanf("%s", buffer) == EOF) break;
                    else {
                        if(strcmp(buffer, "+inserisci_inizio") == 0)
                            add_new_words(tw, lf, lr, word_length, f_created);
                        if(strcmp(buffer, "+nuova_partita") == 0) {
                            memset(buffer, 0, BUFSIZE);
                            if(scanf("%s", buffer) != 0) {
                                strncpy(password, buffer, word_length + 1);
                                memset(pc, 0, BUFSIZE * sizeof(short));
                                char_count(pc, password, word_length);
                            }
                            if(scanf("%d", &tries) != 0)
                                restart = 1;
                        }
                    }
                }
            }
        }
    }
    // Terminates the program
    free(buffer);
    free(password);
    free(guide);
    return 0;
}

// List function definitions

int list_size(list l) {
    list curr = l;
    int i = 0;
    while(curr != NULL) {
        ++i;
        curr = curr -> next;
    }
    return i;
}

list add_sort(list l, const char *new_data, int len) {
    list prev = NULL, curr = l;

    //If the list is empty, add the element on top.
    if(l == NULL) {
        list tmp = malloc(sizeof(struct node) + sizeof(char) * (len + 1));
        strcpy(tmp->data, new_data);
        tmp -> next = l;
        return tmp;
    } else {
        //If the list is not empty
        while(curr != NULL) {
            //If new_data comes before the current node's data then add it before curr
            if(strcmp(new_data, curr -> data) < 0) {
                list tmp = malloc(sizeof(struct node) + sizeof(char) * (len + 1));
                strcpy(tmp->data, new_data);
                tmp -> next = curr;
                //If the item goes in the first position, do not try to connect the previous node to it
                //Otherwise, do it
                if(prev != NULL) {
                    prev -> next = tmp;
                    return l;
                } else return tmp;
                //If the strings are the same, add it in the next node (it keeps the algorithm stable)
            } else {
                prev = curr;
                curr = curr -> next;
            }
        }
        //If the list is over and nothing was added to it, the item goes in last
        if(curr == NULL && prev != NULL) {
            list tmp = malloc(sizeof(struct node) + sizeof(char) * (len + 1));
            strcpy(tmp->data, new_data);
            tmp -> next = curr;
            prev -> next = tmp;
            return l;
        }
    }
    //This gets executed in case of an error
    return NULL;
}

list add(list l, const char *new_data, int len) {
    list tmp = malloc(sizeof(struct node) + sizeof(char) * (len + 1));
    strcpy(tmp->data, new_data);
    tmp->next = l;
    return tmp;
}

void print(list l) {
    list curr = l;
    while(curr != NULL) {
        printf("%s\n", curr -> data);
        curr = curr -> next;
    }
}

res_list add_res(res_list r, char c, int index, int correct, int count, int exact) {
    res_list tmp = malloc(sizeof(struct res_node));
    tmp->c = c;
    tmp->index = index;
    tmp->correct = correct;
    tmp->count = count;
    tmp->exact = exact;
    tmp->next = r;
    return tmp;
}

list destroy(list l) {
    list curr = l, tmp = NULL;
    while(curr != NULL) {
        tmp = curr -> next;
        free(curr);
        curr = tmp;
    }
    return NULL;
}

res_list res_destroy(res_list r) {
    res_list curr = r, tmp = NULL;
    while(curr != NULL) {
        tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    return NULL;
}

// Red-Black Tree function definitions

void print_tree(tree t) {
    // When the bottom of the branch is reached, the function returns.
    if(t == NULL) return;
    // Recursive call to print the left child of a given node.
    print_tree(t->left);
    printf("%s\n", t->data);
    // Recursive call to print the right child of a given node.
    print_tree(t->right);
}

tree rbt_search(tree t, const char *data) {
    // This variable saves the result of strcmp to make the code easier to read.
    int search_result;
    // If the tree is empty or the word was not found anywhere else, the function returns NULL.
    if(t == NULL) return t;
    else {
        search_result = strcmp(t->data, data);
        // If the string in the tree node and the one passed as an argument are the same, the node is returned.
        if(!search_result) return t;
        // Otherwise, if the string found in the node comes before the one to be found, the function keeps searching
        // to the right.
        else if(search_result < 0) return rbt_search(t->right, data);
        // If the string in the node comes after instead, the function keeps searching to the left.
        else return rbt_search(t->left, data);
    }
}

void rbt_insert(tree *t, const char *data, int len) {
    // A new portion of memory is reserved for the node and the string passed as an argument is copied in it.
    tree new_node = malloc(sizeof(struct tnode) + sizeof(char) * (len + 1));
    strcpy(new_node->data, data);
    tree tmp_node = NULL, curr_node = *t;
    // This loop finds the correct location for the new node.
    while(curr_node != NULL) {
        // The tree is searched starting from the root.
        tmp_node = curr_node;
        // If the data to be added comes before the current node, then we proceed to the left branch.
        if(strcmp(new_node->data, curr_node->data) < 0)
            curr_node = curr_node->left;
        // Otherwise, we proceed to the right branch.
        else curr_node = curr_node->right;
    }
    // When we arrive to an empty subtree (or leaf), we add the new node, connecting it to its parent, found in the
    // previous loop.
    new_node->parent = tmp_node;
    // If the node found beforehand is empty, the new node becomes the root of the tree.
    if(tmp_node == NULL) *t = new_node;
    // This section of code decides whether the new node will become the left child or right child of the parent node.
    else if(strcmp(new_node->data, tmp_node->data) < 0)
        tmp_node->left = new_node;
    else tmp_node->right = new_node;
    // The new node is initialized: its children are set as NULL and its color is set to red.
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->color = RED;
    // This function then fixes possible violations of RBT rules.
    rbt_insert_fixup(t, &new_node);
}

void left_rotate(tree *root, tree *node) {
    // To rotate the node passed as an argument, we need a temporary node as a buffer.
    // The left subtree of the node 'tmp' becomes the right subtree of the node passed to the function.
    tree tmp = (*node)->right;
    (*node)->right = tmp->left;
    // If the new right subtree of 'node' is not empty (or a leaf), then it is reattached by reassigning its parent
    // property.
    if(tmp->left != NULL)
        tmp->left->parent = *node;
    // The parent of the node has to become tmp's parent for them to rotate.
    tmp->parent = (*node)->parent;
    // If the node is the root of the tree, then the root is updated with tmp, which is the new root.
    if((*node)->parent == NULL)
        *root = tmp;
    // Otherwise, the node's new parent is reassigned according to whether node was a left or a right child.
    else if(*node == (*node)->parent->left)
        (*node)->parent->left = tmp;
    else (*node)->parent->right = tmp;
    // The node becomes tmp's left child and tmp becomes node's parent.
    tmp->left = *node;
    (*node)->parent = tmp;
}

void right_rotate(tree *root, tree *node) {
    // This function is equivalent to the one above, but with left and right exchanged.
    tree tmp = (*node)->left;
    (*node)->left = tmp->right;
    if(tmp->right != NULL)
        tmp->right->parent = (*node);
    tmp->parent = (*node)->parent;
    if((*node)->parent == NULL)
        *root = tmp;
    else if((*node) == (*node)->parent->left)
        (*node)->parent->left = tmp;
    else (*node)->parent->right = tmp;
    tmp->right = (*node);
    (*node)->parent = tmp;
}

void rbt_insert_fixup(tree *t, tree *node_to_fix) {
    // These two nodes are declared for convenience reasons. They will become a node's parent and grandparent.
    tree parent = NULL, grandparent = NULL;
    // The loop is exited if:
    // - The node that has to be fixed is the root: in that case, recoloring the node black fixes the tree.
    // - The color of the node to be fixed is not black: the fix-up needs to be done on red nodes only.
    // - The color of the parent of the node to be fixed is not red: restrictions on the color of children nodes in RBTs
    //   is only valid for red nodes, not black ones.
    while(((*node_to_fix) != *t) && ((*node_to_fix)->color != BLACK) && ((*node_to_fix)->parent->color == RED)) {
        // These assignments are done for convenience, to make the code easier to read.
        parent = (*node_to_fix)->parent;
        grandparent = (*node_to_fix)->parent->parent;
        // This part of the code fixes nodes when the parent of the node to be fixed is a left child.
        if(parent == grandparent->left) {
            // This assignment is done for convenience.
            tree uncle = grandparent->right;
            // If the node's uncle is red (and not empty), then the nodes are recolored to balance the tree.
            // This is generally defined as 'case 1'.
            if(uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                // The check continues on the node's grandparent.
                *node_to_fix = grandparent;
            } else {
                // These two cases (namely 'case 2' and 'case 3') are defined when the node's uncle is black.
                // 'Case 2': the node's uncle is black and the node to be fixed is a right child.
                if((*node_to_fix) == parent->right) {
                    // The node's parent is rotated left. We are now in 'case 3'.
                    left_rotate(t, &parent);
                    *node_to_fix = parent;
                    parent = (*node_to_fix)->parent;
                }
                // 'Case 3': the node's uncle is black and the node to be fixed is a left child.
                // In this case, the node's grandparent is rotated right and, with some recoloring, the fixup is done.
                right_rotate(t, &grandparent);
                parent->color = BLACK;
                grandparent->color = RED;
                *node_to_fix = parent;
            }
        } else {
            // This part of the code fixes nodes when the parent of the node to be fixed is a right child instead.
            // The cases are exactly the same, but with 'left' and 'right' reversed.
            tree uncle = grandparent->left;
            if(uncle != NULL && uncle->color == RED) {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                *node_to_fix = grandparent;
            } else {
                if((*node_to_fix) == parent->left) {
                    right_rotate(t, &parent);
                    (*node_to_fix) = parent;
                    parent = (*node_to_fix)->parent;
                }
                left_rotate(t, &grandparent);
                parent->color = BLACK;
                grandparent->color = RED;
                (*node_to_fix) = parent;
            }
        }
    }
    // This is the base case: if the node passed to the function is the root or the fixup has finished,
    // the root is colored black.
    (*t)->color = BLACK;
}

void add_to_list(tree *t, list *f, res_list *r, int len) {
    // This function makes an in order tree walk in reverse to add filtered elements of the tree to the filtered list.
    // The walk is done from right to left instead of the usual left to right because it's faster to add a node
    // to a list when it's added to the top of the list.
    if(*t == NULL) return;
    add_to_list(&(*t)->right, f, r, len);
    res_list curr = *r;
    int to_add = 1;
    while(curr != NULL) {
        if(curr->index != -1) {
            if(curr->correct && (*t)->data[curr->index] != curr->c) {
                to_add = 0;
                break;
            }
            if(!curr->correct && (*t)->data[curr->index] == curr->c) {
                to_add = 0;
                break;
            }
        }
        if(curr->count != -1) {
            if (curr->count == 0) {
                if (strchr((*t)->data, curr->c) != NULL) {
                    to_add = 0;
                    break;
                }
            }
            else {
                char *w = (*t)->data;
                int count = 0;
                while(w != NULL) {
                    w = strchr(w, curr->c);
                    if(w != NULL) {
                        ++count;
                        ++w;
                    }
                }
                if(curr->exact && count != curr->count) {
                    to_add = 0;
                    break;
                }
                if(!curr->exact && count < curr->count) {
                    to_add = 0;
                    break;
                }
            }
        }
        curr = curr->next;
    }
    if(to_add) *f = add(*f, (*t)->data, len);
    add_to_list(&(*t)->left, f, r, len);
}

// Game function definitions

void add_new_words(tree *t, list *f, res_list *r, int len, int f_created) {
    char *buffer = calloc(BUFSIZE, sizeof(char));
    int exit = 0;
    while(!exit) {
        if(scanf("%s", buffer) != 0) {
            if(strcmp(buffer, "+inserisci_fine") == 0)
                exit = 1;
            else {
                rbt_insert(t, buffer, len);
                if (f_created && r != NULL)
                    new_words_check(r, f, buffer, len);
            }
        }
        memset(buffer, 0, BUFSIZE);
    }
    free(buffer);
}

void word_check(char *password, char *buffer, char *guide, int len, int *tp, short *pc, short *wc, list *f, res_list *r,
                int f_created) {
    // Empty guide buffer
    memset(guide, 0, len);
    // Wrong word: one try is subtracted to the counter.
    --*tp;
    short pwd, count;
    char tmp;
    for (int i = 0; i < len; ++i) {
        if(guide[i] != '+' && guide[i] != '|' && guide[i] != '/') {
            tmp = buffer[i];
            pwd = char_check(pc, tmp);
            count = char_check(wc, tmp);
            if(pwd != -1 && count != -1) {
                char *b = buffer, *p = password;
                long index, p_index;
                // If these two values are the same or there are more 'tmp' characters in the password, the symbols
                // must be either '+' or '|'.
                if(pwd >= count) {
                    // While there are more 'tmp' characters in both strings, check whether they are in the same index
                    // position. If so, mark '+', else mark '|'.
                    while(b != NULL) {
                        b = strchr(b, tmp);
                        if (b != NULL) {
                            p = password;
                            int found = 0;
                            index = b - buffer;
                            if (index < len) {
                                while (p != NULL && !found) {
                                    p = strchr(p, tmp);
                                    if (p != NULL) {
                                        p_index = p - password;
                                        if (p_index < len) {
                                            if (index == p_index) {
                                                guide[index] = '+';
                                                if(f_created) delete_index((int) index, tmp, f, 1);
                                                *r = add_res(*r, tmp, (int) index, 1, -1, -1);
                                                found = 1;
                                            }
                                        }
                                        ++p;
                                    }
                                }
                                if (!found) {
                                    guide[index] = '|';
                                    if(f_created) delete_index((int) index, tmp, f, 0);
                                    *r = add_res(*r, tmp, (int) index, 0, -1, -1);
                                }
                            }
                            ++b;
                        }
                    }
                    if(f_created) occurrences_check(f, tmp, count, 0);
                    *r = add_res(*r, tmp, -1, -1, count, 0);
                }
                // If pwd < count, then some characters in the guide will be '/'.
                else {
                    // If the password does not contain that character then every instance of it will be '/'.
                    if(pwd == 0) {
                        while (b != NULL) {
                            b = strchr(b, tmp);
                            if (b != NULL) {
                                index = b - buffer;
                                if (index < len)
                                    guide[index] = '/';
                                ++b;
                            }
                        }
                        // This function deletes all the words that contain characters that do not appear in the
                        // password anywhere.
                        if(f_created) char_delete(f, tmp, 0);
                        *r = add_res(*r, tmp, -1, -1, 0, -1);
                    } else {
                        // While there are more 'tmp' characters in the password, check whether they are in the
                        // correct position in the buffer.
                        int pwd_tmp = pwd, count_tmp = count;
                        // This loop checks for correct characters in the correct places by "syncing" the two char
                        // pointers. This goes on until there are no more occurrences of a given character or until
                        // all of the characters in the password are found to be correct.
                        while(b != NULL && p != NULL && pwd_tmp > 0) {
                            b = strchr(b, tmp);
                            p = strchr(p, tmp);
                            if(p == NULL || b == NULL)
                                break;
                            index = b - buffer;
                            p_index = p - password;
                            if(index < len && p_index < len) {
                                if(index == p_index) {
                                    guide[index] = '+';
                                    if(f_created) delete_index((int)index, tmp, f, 1);
                                    *r = add_res(*r, tmp, (int)index, 1, -1, -1);
                                    --pwd_tmp;
                                    --count_tmp;
                                    ++b;
                                    ++p;
                                } else {
                                    if(index < p_index)
                                        ++b;
                                    if(index > p_index)
                                        ++p;
                                }
                            }
                        }
                        // If pwd_temp is not 0 yet, then there are more characters that are correct but not
                        // in the right place.
                        // The b pointer is reset and the check starts from the beginning.
                        b = buffer;
                        while(b != NULL && pwd_tmp > 0) {
                            b = strchr(b, tmp);
                            if(b == NULL) break;
                            index = b - buffer;
                            if(index < len) {
                                if (guide[index] != '+') {
                                    guide[index] = '|';
                                    if(f_created) delete_index((int) index, tmp, f, 0);
                                    *r = add_res(*r, tmp, (int) index, 0, -1, -1);
                                    --pwd_tmp;
                                }
                                ++b;
                            }
                        }
                        // If pwd_temp is 0 here, then all that are left are wrong characters.
                        // The b pointer is reset if the loop above run.
                        if(b != buffer) b = buffer;
                        while(b != NULL && count_tmp > 0) {
                            b = strchr(b, tmp);
                            if(b == NULL) break;
                            index = b - buffer;
                            if(index < len) {
                                if(guide[index] != '+' && guide[index] != '|') {
                                    guide[index] = '/';
                                    if(f_created) delete_index((int) index, tmp, f, 0);
                                    *r = add_res(*r, tmp, (int) index, 0, -1, -1);
                                    --count_tmp;
                                }
                                ++b;
                            }
                        }
                        if(f_created) occurrences_check(f, tmp, pwd, 1);
                        *r = add_res(*r, tmp, -1, -1, pwd, 1);
                    }
                }
            }
        }
    }
}

void occurrences_check(list *f, char c, int count, int strict) {
    list curr = *f, tmp = NULL, prev = NULL;
    char *w;
    int curr_count, del;
    while(curr != NULL) {
        del = 0;
        w = curr->data;
        curr_count = 0;
        // This loop counts the number of occurrences of the character passed to occurrences_check in the current
        // word in the filtered list.
        while(w != NULL && curr_count <= count) {
            w = strchr(w, c);
            if(w != NULL) {
                ++curr_count;
                ++w;
            }
        }
        // If the check must be strict, if the current word in filtered has a different number of characters than
        // the one passed to the function, it is deleted.
        if(strict) {
            if(curr_count != count) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
        }
        // If the check is not strict, if the current word does not have at least 'count' characters, it is deleted.
        else {
            if(curr_count < count) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
        }
        // If no deletion occurred, the curr pointer is updated
        if(!del) {
            prev = curr;
            curr = curr->next;
        }
    }
}

void new_words_check(res_list *r, list *f, char *new_word, int len) {
    res_list curr = *r;
    while(curr != NULL) {
        if(curr->index != -1) {
            if(curr->correct && new_word[curr->index] != curr->c) return;
            if(!curr->correct && new_word[curr->index] == curr->c) return;
        }
        if(curr->count != -1) {
            if (curr->count == 0) {
                if (strchr(new_word, curr->c) != NULL) return;
            }
            else {
                char *w = new_word;
                int count = 0;
                while(w != NULL) {
                    w = strchr(w, curr->c);
                    if(w != NULL) {
                        ++count;
                        ++w;
                    }
                }
                if(curr->exact && count != curr->count) return;
                if(!curr->exact && count < curr->count) return;
            }
        }
        curr = curr->next;
    }
    // If the function did not return, the word can be added.
    *f = add_sort(*f, new_word, len);
}

void char_delete(list *f, char c, int correct) {
    list curr = *f, tmp = NULL, prev = NULL;
    int del;    // Checks whether there was a deletion or not and skips the reassignment of 'curr' if it happened.
    if(correct) {
        while(curr != NULL) {
            del = 0;
            if(strchr(curr->data, c) == NULL) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
            if(!del) {
                prev = curr;
                curr = curr->next;
            }
        }
    } else {
        while(curr != NULL) {
            del = 0;
            if(strchr(curr->data, c) != NULL) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
            if(!del) {
                prev = curr;
                curr = curr->next;
            }
        }
    }
}

void char_count(short *bc, const char *word, int len) {
    for(int i = 0; i < len; ++i) {
        if(word[i] == '-')
            ++bc[0];
        if(word[i] >= '0' && word[i] <= '9')
            ++bc[word[i] - 47];
        if(word[i] >= 'A' && word[i] <= 'Z')
            ++bc[word[i] - 54];
        if(word[i] == '_')
            ++bc[37];
        if(word[i] >= 'a' && word[i] <= 'z')
            ++bc[word[i] - 59];
    }
}

short char_check(short *bc, char c) {
    if(c == '-') return bc[0];
    if(c >= '0' && c <= '9') return bc[c - 47];
    if(c >= 'A' && c <= 'Z') return bc[c - 54];
    if(c == '_') return bc[37];
    if(c >= 'a' && c <= 'z') return bc[c - 59];
    return -1;
}

void delete_index(int i, char c, list *f, int correct) {
    list curr = *f, tmp = NULL, prev = NULL;
    int del;
    while(curr != NULL) {
        del = 0;
        if(correct) {
            if(curr->data[i] != c) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
        } else {
            if(curr->data[i] == c) {
                tmp = curr;
                curr = curr->next;
                if(prev != NULL) prev->next = curr;
                else *f = curr;
                free(tmp);
                del = 1;
            }
        }
        if(!del) {
            prev = curr;
            curr = curr->next;
        }
    }
}
