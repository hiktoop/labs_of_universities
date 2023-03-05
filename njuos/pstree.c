/*
 pstree - display a tree of processes
 this is a reduce implementation of pstree
 support three arguements:

   -V  show the infomantion about pstree
   -P  show tree with Pid
   -n  show tree in order
*/

#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define PATH "/proc" // in linux system

/* the input args' map:
 * --1 V
 * -1- P
 * 1-- n
 */ 
int status; 

// node of a tree
struct node {
  char name[100];
  int pid;
  int ppid;
  // struct node *ppid;
  struct node *sons[100];
  int sonum; // the number of son
};
// node to record name, pid and ppid
struct node tree[1000];
int nodeCnt = 0;
const int root = 1, nodeMax = 999;


// print version infomation(ok)
void printV() { 
  printf("pstree 0.1\n");
  printf("the argvs you can input:\n");
  printf("pstree[...]\n");
  printf("  -n : sort by pid\n");
  printf("  -p : show pid\n");
  printf("  -V : show version infomation\n");
}


/* 
 * decode the input args
 * the return value maps:
 * -1 --- irlegal
 *  0 --- legal
 */
int decode(char *s) {
  for (int i = 0; s[i] != 0; i++) {
    if(s[i] != '-' && s[i] != 'n' && s[i] != 'p' && s[i] != 'V')
      return -1; // Ilifgal
    if(s[i] == '-') continue;
    if(s[i] == 'V') status |= 1;        // --1
    if(s[i] == 'p') status |= (1 << 1); // -1-
    if(s[i] == 'n') status |= (1 << 2); // 1--
  }
  return  0;
}


void printSon(int fa, int dep) {
  for(int i=0; i < nodeCnt; i++) {
    if(tree[i].ppid == fa) {
      for(int i=0; i<dep; i++)
        printf("\t");
      printf("%s", tree[i].name);
      if(status & (1<<1)) printf("(%d)\n", tree[i].pid);
      else printf("\n");
      printSon(tree[i].pid, dep + 1);
    }
  }
}
// print the pid by tree
void printree() { 
  for(int i=0; i < nodeCnt; i++) {
    if(tree[i].ppid == 0) {
      printf("%s", tree[i].name);
      if(status & (1<<1)) printf("(%d)\n", tree[i].pid);
      else printf("\n");
      printSon(tree[i].pid, 1);
    }
  }

}


//return 1 if it's a digit
int Isdigit(const char *s) { 
  int i = 0;
  while(s[i] != 0) {
    if(!isdigit(s[i]))
      return 0;
    i++;
  }
  return 1;
}




// put name, pid and ppid to node
int analyinfo(char * s) {
  if(s[0] == 'N' && s[1] == 'a' && s[2] == 'm' && s[3] == 'e' && s[4] == ':') {
    strcpy(tree[nodeCnt].name, s+6);
    tree[nodeCnt].name[strlen(tree[nodeCnt].name)-1] = 0;
    return 1;
  }
  if(s[0] == 'P' && s[1] == 'i' && s[2] == 'd' && s[3] == ':') {
    tree[nodeCnt].pid = atoi(s+5);
    return 1;
  }
  if(s[0] == 'P' && s[1] == 'P' && s[2] == 'i' && s[3] =='d' && s[4] == ':') {
    tree[nodeCnt].ppid = atoi(s+6);
    return 1;
  }
  return 0;
}

// read infomation about name, pid and ppid (167)
void readinfo(char *pathname) {
  FILE *fp = fopen(pathname, "r");
  if(fp == NULL) {
    printf("%s open fail!\n", pathname);
    return;
  }
  char s[100];
  int cnt = 0;
  while(fgets(s, 99, fp)) {
    cnt += analyinfo(s);
    // printf("name = %s\npid = %d\nppid = %d\n", tree[nodeCnt].name, tree[nodeCnt].pid, tree[nodeCnt].ppid);
    // printf("cnt = %d\n\n", cnt);
    if(cnt == 3) break; // all had been read
  }
  nodeCnt++;
  if(nodeCnt > nodeMax)
    printf("---node number is too many so there is a error!---\n");
  fclose(fp);
}


// begin of the code
int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    assert(argv[i]);
    if(decode(argv[i]) == -1) {
      printf("Irlegal input!\n");
      return 0;
    }
  }
  assert(!argv[argc]);
  if(status & 1) { // print version info
    printV();
    return 0;
  }

  DIR *dirp;
  struct dirent *dp;
  
  dirp = opendir(PATH);
  while(1) {
    dp = readdir(dirp);
    if(dp == NULL) break;
    // skip ., .. and value not digit
    if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || !Isdigit(dp->d_name))
      continue;
    char name[100] = {0, };
    strcat(name, PATH);
    strcat(name, "/");
    strcat(name, dp->d_name);
    strcat(name, "/status");
    readinfo(name);
    // printf("%s\n", dp->d_name);
  }
  printree(); // print tree
  return 0;
}

