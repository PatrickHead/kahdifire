#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>

enum test_enum
{
  ONE,
  TWO,
  N = 99
};

struct a_thing
{
  char *str;
  union
  {
    int i;
    double d;
  };
  struct
  {
    char c;
    unsigned int u;
  } s;
  int arr1[3][10];
  int arr2[5];
  int *arr3[42];
  double **values;
  //int **arr4;
  //int arr4[][8];
};

struct my_struct
{
  int a;
  struct a_thing *thing;
  struct a_thing real_thing;
  struct a_thing arr_thing[5];
  struct a_thing *parr_thing[5];
  //struct stat st;
  //struct stat *st;
};

typedef union a_union a_union;

union a_union
{
  char *blisfit;
  long l;
};

typedef struct
{
  int i;
  char c;
} struct_t_2;

int main(int argc, char **argv)
{
  int a = 10;
  union a_union u;
  enum test_enum e = N;
  //struct a_thing at;

  u.l = 12;
  //at.i = 1;

  printf("a=%d\n", a);
  //printf("at.i=%d\n", at.i);
  printf("u.l=%ld\n", u.l);
  printf("e=%d\n", e);

  return 0;
}

