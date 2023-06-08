//
//  main.cpp
//  phase1
//
//  Created by Wei Deng on 11/7/22.
//

#include <iostream>
#include <cstdio>
#include <string.h>
#include <cstdlib>
using namespace std;

#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

//enum e_com {READ, PC, HELP, QUIT};
enum e_com {READ, PC, HELP, QUIT, LEV, LOGICSIM};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

struct cmdstruc {
   char name[MAXNAME];        /* command syntax */
   int (*fptr)(char *);             /* function pointer of the commands */
    //char * funcArg;
   enum e_state state;        /* execution state sequence */
};

typedef struct  n_struc {
   unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
   unsigned num;              /* line number(May be different from indx */
   enum e_gtype type;         /* gate type */
   unsigned fin;              /* number of fanins */
   unsigned fout;             /* number of fanouts */
   struct n_struc **unodes;   /* pointer to array of up nodes */
   struct n_struc **dnodes;   /* pointer to array of down nodes */
   int level;                 /* level of the gate output */
   int val;
} NSTRUC;

#define NUMFUNCS 6
int cread(char *), pc(char *), help(char *cp), quit(char *cp), lev(char *), logicsim(char*);

enum e_state Gstate = EXEC;     /* global exectution sequence */
NSTRUC *Node;                   /* dynamic array of nodes */
NSTRUC **Pinput;                /* pointer to array of primary inputs */
NSTRUC **Poutput;               /* pointer to array of primary outputs */
int Nnodes;                     /* number of nodes */
int Npi;                        /* number of primary inputs */
int Npo;                        /* number of primary outputs */
int Done = 0;                   /* status bit to terminate program */
char *FILENAME;
int level_num;

e_com& operator++(e_com& e_comID){
  return e_comID = static_cast<e_com>(static_cast<int>(e_comID)+1 );
}

struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread,EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   /**/{"LEV", lev, CKTLD},
   /**/{"LOGICSIM", logicsim, CKTLD},
};

int main(int argc, const char * argv[]) {
    
    enum e_com com;
       char cline[MAXLINE], wstr[MAXLINE], *cp;

       while(!Done) {
          cout << "\nCommand>";
          fgets(cline, MAXLINE, stdin);
          if(sscanf(cline, "%s", wstr) != 1) continue;
          cp = wstr;
          while(*cp){
        *cp= Upcase(*cp);
        cp++;
          }
          cp = cline + strlen(wstr);
          com = READ;
           while(com < NUMFUNCS && strcmp(wstr, command[com].name)){
               com = static_cast<e_com>(static_cast<int>(com)+1 );
               
           }
          if(com < NUMFUNCS) {
             if(command[com].state <= Gstate) (*command[com].fptr)(cp);
             else cout << "Execution out of sequence!\n";
          }
          else system(cline);
       }
    return 0;
}

/*-----------------------------------------------------------------------
input: circuit description file name
output: nothing
called by: main
description:
  This routine reads in the circuit description file and set up all the
  required data structure. It first checks if the file exists, then it
  sets up a mapping table, determines the number of nodes, PI's and PO's,
  allocates dynamic data arrays, and fills in the structural information
  of the circuit. In the ISCAS circuit description format, only upstream
  nodes are specified. Downstream nodes are implied. However, to facilitate
  forward implication, they are also built up in the data structure.
  To have the maximal flexibility, three passes through the circuit file
  are required: the first pass to determine the size of the mapping table
  , the second to fill in the mapping table, and the third to actually
  set up the circuit information. These procedures may be simplified in
  the future.
-----------------------------------------------------------------------*/
int clear();
int allocate();
int help(char *cp);


int cread(char *cp)
{
   char buf[MAXLINE];
   int ntbl, *tbl, i, j, k, nd, tp, fo, fi, ni = 0, no = 0;
   FILE *fd;
   NSTRUC *np;

   sscanf(cp, "%s", buf);
   if((fd = fopen(buf,"r")) == NULL) {
      cout << "File %s does not exist!\n";
      return -1;
   }
   FILENAME = (char*)malloc(sizeof(char)*100);
   i = 9;
   while (buf[i] != '.'){
      FILENAME[i-9] = buf[i];
      i = i + 1;
      
   }
   if(Gstate >= CKTLD) clear();
   Nnodes = Npi = Npo = ntbl = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) {
         if(ntbl < nd) ntbl = nd;
         Nnodes ++;
         if(tp == PI) Npi++;
         else if(tp == PO) Npo++;
      }
   }
   tbl = (int *) malloc(++ntbl * sizeof(int));

   fseek(fd, 0L, 0);
   i = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) tbl[nd] = i++;
   }
   allocate();

   fseek(fd, 0L, 0);
   while(fscanf(fd, "%d %d", &tp, &nd) != EOF) {
      np = &Node[tbl[nd]];
      np->num = nd;
      if(tp == PI) Pinput[ni++] = np;
      else if(tp == PO) Poutput[no++] = np;
      switch(tp) {
         case PI:
         case PO:
         case GATE:
            fscanf(fd, "%d %d %d", &np->type, &np->fout, &np->fin);
            break;
         
         case FB:
            np->fout = np->fin = 1;
            fscanf(fd, "%d", &np->type);
            break;

         default:
            cout << "Unknown node type!\n";
            exit(-1);
         }
      np->unodes = (NSTRUC **) malloc(np->fin * sizeof(NSTRUC *));
      np->dnodes = (NSTRUC **) malloc(np->fout * sizeof(NSTRUC *));
      for(i = 0; i < np->fin; i++) {
         fscanf(fd, "%d", &nd);
         np->unodes[i] = &Node[tbl[nd]];
         }
      for(i = 0; i < np->fout; np->dnodes[i++] = NULL);
      }

   for(i = 0; i < Nnodes; i++) {
      for(j = 0; j < Node[i].fin; j++) {
         np = Node[i].unodes[j];
         k = 0;
         while(np->dnodes[k] != NULL) k++;
         np->dnodes[k] = &Node[i];
         }
      }
   fclose(fd);
   Gstate = CKTLD;
   cout << "==> OK\n";
    return 1;
}



int lev (char * cp) {
   FILE *fp;
   char fileName[MAXLINE];
   int currLev = 0;
   int assignedCounter = 0;
   int dnodeLen;
   int ready;
   int gateCount;
   int i, j, k;
   int maxInLevel;
   NSTRUC *np, *currNp;

   sscanf(cp, "%s", fileName);
   if((fp = fopen(fileName,"w")) == NULL) {
      cout << "OUT File "<<fileName<<" does not exist!\n";
      return 0;
   }

   gateCount = 0;
   // initialize all the levels
   for (i = 0; i < Nnodes; i ++){
      if (Node[i].type == IPT){
          Node[i].level = 0;
          assignedCounter = assignedCounter + 1;
      }
      else{
          Node[i].level = -1;
      }
      if (Node[i].type != IPT && Node[i].type != BRCH){
         gateCount = gateCount + 1;
      }
     
   }

   while (assignedCounter < Nnodes) {
      for (i = 0; i < Nnodes; i ++){
         // if not assigned level
         if (Node[i].level == -1){
            currNp = &Node[i];
            ready = 1;
            // branch
            if (currNp->type == BRCH){
               ready = (currNp->unodes[0]->level != -1);
               if (ready){
                  currNp->level = currNp->unodes[0]->level + 1;
                  assignedCounter = assignedCounter + 1;
               }
            }
            // gates
            else{
               maxInLevel = 0;
               for (j = 0; j < currNp->fin; j++){
                  if (currNp->unodes[j]->level > maxInLevel){
                     maxInLevel = currNp->unodes[j]->level;
                  }
                  if (currNp->unodes[j]->level == -1){
                     ready = 0;
                     break;
                  }
               }
               if (ready){
                  currNp->level = maxInLevel + 1;
                  assignedCounter = assignedCounter + 1;
               }

            }
            
           
         
         }
         
      }
    
   }
   fprintf(fp,"%s\n", FILENAME);
   fprintf(fp,"#PI: %d\n", Npi);
   fprintf(fp,"#PO: %d\n", Npo);
   fprintf(fp,"#Nodes: %d\n", Nnodes);
   fprintf(fp,"#Gates: %d\n",gateCount);

   for(i = 0; i<Nnodes; i++) {
      np = &Node[i];
      fprintf(fp, "%d %d\n", np->num, np->level);
      
   }
   fclose(fp);
    return 1;
}

int logicsim(char *cp)
{
   char buf[MAXLINE];
   char outf[MAXLINE];
   FILE *fd;
   NSTRUC *np;
   int nd;
   int val;
   int i, j, k;
   level_num = 0;
   sscanf(cp, "%s %s", buf, outf);
   if((fd = fopen(buf,"r")) == NULL){
      cout << "PI_File %s does not exit!\n";
      return 0 ;
   }

   fseek(fd, 0L, 0);
   while(fscanf(fd, "%d,%d", &nd, &val) != EOF){
      for(i = 0; i < Nnodes; i++){
        np = &Node[i];
        if(np->num == nd) np->val = val;
        if(np->level > level_num) level_num = np->level;
      }
   }
   fclose(fd);


   for(i = 1; i <= level_num; i++){
      for(j = 0; j < Nnodes; j++){
         np = &Node[j];
         if(np->level == i) {
            switch(np->type) {
               //case 0: //PI
               case 1: np->val = np->unodes[0]->val;//branch
                       break;
               case 2: //XOR
                  np->val = np->unodes[0]->val;
                  for(k = 1; k < np->fin; k++){
                     np->val = np->val ^ np->unodes[k]->val;
                  }
                  break;
               case 3: //OR
                  np->val = np->unodes[0]->val;
                  for(k = 1; k < np->fin; k++){
                     np->val = np->val | np->unodes[k]->val;
                  }
                  break;
               case 4: //NOR
                  np->val = np->unodes[0]->val;
                  for(k = 1; k < np->fin; k++){
                     np->val = np->val | np->unodes[k]->val;
                  }
                  np->val = np->val == 0 ? 1 : 0;
                  break;
               case 5: np->val = !np->unodes[0]->val;//NOT
                       break;
               case 6: //NAND
                  np->val = np->unodes[0]->val;
                  for(k = 1; k < np->fin; k++){
                     np->val = np->val & np->unodes[k]->val;
                  }
                  np->val = np->val == 0 ? 1 : 0;
                  break;
               case 7: //AND
                  np->val = np->unodes[0]->val;
                  for(k = 1; k < np->fin; k++){
                     np->val = np->val & np->unodes[k]->val;
                  }
                  break;
                default:
                    
                    break;
            }
         }
      }
   }
    
    FILE *out_file;
    out_file = fopen(outf,"w");
    for(i = 0; i < Npo; i++){
     fprintf(out_file,"%d,%d\n",Poutput[i]->num, Poutput[i]->val);
    }
    fclose(out_file);
    /*for(i = 0; i < Nnodes; i++){
      np = &Node[i];
      printf("output %d %d\n", np->num, np->val);
    } */
    return 1;
}

int lp(char *cp)
{
   int i, j;
   NSTRUC *np;
   char *gname(int);
   
    cout << " Node   Type \tIn     \t\t\tOut    \nLevel  \n";
    cout << "------ ------\t-------\t\t\t-------\t-------\n";
   for(i = 0; i<Nnodes; i++) {
      np = &Node[i];
       cout << "\t\t\t\t\t";
      for(j = 0; j<np->fout; j++) cout << np->dnodes[j]->num <<" ";
       cout << "\t " << np->level;
       cout.precision(5);
       
       cout << "\r" << np->num<< gname(np->type)<<"\t";
      for(j = 0; j<np->fin; j++) cout << np->unodes[j]->num <<" ";
       cout <<"\n";
   }
    cout <<"Primary inputs:  ";
   for(i = 0; i<Npi; i++) cout <<Pinput[i]->num<<" ";
   cout << "\n";
    cout <<"Primary outputs: ";
   for(i = 0; i<Npo; i++) cout <<Poutput[i]->num << " ";
   cout << "\n\n";
    cout << "Number of nodes = " << Nnodes << endl;
    cout << "Number of primary inputs = "<<Npi<<endl;
    cout << "Number of primary outputs = "<< Npo << endl;
    return 1;
}




/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  The routine prints out the circuit description from previous READ command.
-----------------------------------------------------------------------*/
int pc(char *cp)
{
   int i, j;
   NSTRUC *np;
   char *gname(int);
   
   cout << " Node   Type \tIn     \t\t\tOut    \n";
    cout << "------ ------\t-------\t\t\t-------\n";
    
    for(i = 0; i<Nnodes; i++) {
       np = &Node[i];
        cout << "\t\t\t\t\t";
       for(j = 0; j<np->fout; j++) cout << np->dnodes[j]->num <<" ";
        cout.precision(5);
        
        cout << "\r" << np->num<< gname(np->type)<<"\t";
       for(j = 0; j<np->fin; j++) cout << np->unodes[j]->num <<" ";
        cout <<"\n";
    }
    cout <<"Primary inputs:  ";
   for(i = 0; i<Npi; i++) cout <<Pinput[i]->num<<" ";
   cout << "\n";
    cout <<"Primary outputs: ";
   for(i = 0; i<Npo; i++) cout <<Poutput[i]->num << " ";
   cout << "\n\n";
    cout << "Number of nodes = " << Nnodes << endl;
    cout << "Number of primary inputs = "<<Npi<<endl;
    cout << "Number of primary outputs = "<< Npo << endl;
    return 1;
    
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  The routine prints ot help inormation for each command.
-----------------------------------------------------------------------*/
int help(char *cp)
{
    cout << "READ filename - ";
    cout << "read in circuit file and creat all data structures\n";
    cout << "PC - ";
    cout << "print circuit information\n";
    cout << "HELP - ";
    cout << "print this help information\n";
    cout << "QUIT - ";
    cout << "stop and exit\n";
    return 1;
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  Set Done to 1 which will terminates the program.
-----------------------------------------------------------------------*/
int quit(char *cp)
{
   Done = 1;
    return 1;
}

/*======================================================================*/

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine clears the memory space occupied by the previous circuit
  before reading in new one. It frees up the dynamic arrays Node.unodes,
  Node.dnodes, Node.flist, Node, Pinput, Poutput, and Tap.
-----------------------------------------------------------------------*/
int clear()
{
   int i;

   for(i = 0; i<Nnodes; i++) {
      free(Node[i].unodes);
      free(Node[i].dnodes);
   }
   free(Node);
   free(Pinput);
   free(Poutput);
   Gstate = EXEC;
    return 1;
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine allocatess the memory space required by the circuit
  description data structure. It allocates the dynamic arrays Node,
  Node.flist, Node, Pinput, Poutput, and Tap. It also set the default
  tap selection and the fanin and fanout to 0.
-----------------------------------------------------------------------*/
int allocate()
{
   int i;

   Node = (NSTRUC *) malloc(Nnodes * sizeof(NSTRUC));
   Pinput = (NSTRUC **) malloc(Npi * sizeof(NSTRUC *));
   Poutput = (NSTRUC **) malloc(Npo * sizeof(NSTRUC *));
   for(i = 0; i<Nnodes; i++) {
      Node[i].indx = i;
      Node[i].fin = Node[i].fout = 0;
   }
    return 1;
}

/*-----------------------------------------------------------------------
input: gate type
output: string of the gate type
called by: pc
description:
  The routine receive an integer gate type and return the gate type in
  character string.
-----------------------------------------------------------------------*/
char *gname(int tp)
{
   switch(tp) {
      case 0: return((char *)"PI");
      case 1: return((char *)"BRANCH");
      case 2: return((char *)"XOR");
      case 3: return((char *)"OR");
      case 4: return((char *)"NOR");
      case 5: return((char *)"NOT");
      case 6: return((char *)"NAND");
      case 7: return((char *)"AND");
   }
    return (char *)0;
}
