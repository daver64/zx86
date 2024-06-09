#include <cpp/libcpp.hpp>

char *getline(FILE *fp)
{
    const int buffersize = 256;
    char *buffer = new char[buffersize];
    int index = 0;
    char c = 0;
    while (index < buffersize && c != '\n' && !feof(fp))
    {
        c = fgetc(fp);
        if (c != '\n' && !feof(fp))
        {
            buffer[index] = c;
        }
        else
        {
            buffer[index] = 0;
        }
        index++;
    }
    return buffer;
}

list<char *> string_to_list(char *line)
{
    char **args;
    int argc;
    argc = string_split(line, ' ', &args);
    list<char *> clist;
    for (int i = 0; i < argc; i++)
    {
        char *l = new char[256];
        strcat(l, *(args + i));
        clist.push_back(l);
        free(*(args + i));
    }
    free(args);
    return clist;
}
void print_list(list<char *> &l)
{
    node<char *> *n = l.gethead();
    while (n != NULL)
    {
        printf("<%s>\n", n->data);
        n = n->next;
    }
}
void destroy_list(list<char *> &l)
{
    node<char *> *n = l.gethead();
    while (n != NULL)
    {
        delete[] n->data;
        n->data = NULL;
        node<char *> *on = n;
        n = n->next;
        delete on;
    }
}


int max(int a, int b)
{
    return (a > b) ? a : b;
}

void do_tree()
{
  avl_node<int> *root = NULL;
  root = avl_insert_node(root, 33);
  root = avl_insert_node(root, 13);
  root = avl_insert_node(root, 53);
  root = avl_insert_node(root, 9);
  root = avl_insert_node(root, 21);
  root = avl_insert_node(root, 61);
  root = avl_insert_node(root, 8);
  root = avl_insert_node(root, 11);
  char *indent=new char[128];
  memset(indent,0,128);
  avl_print_tree<int>(root, indent, true);
  root = avl_delete_node(root, 13);
  //cout << "After deleting " << endl;
  memset(indent,0,128);
  avl_print_tree<int>(root, indent, true);
}
