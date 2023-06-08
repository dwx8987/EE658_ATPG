//
//  main.cpp
//  phase1
//
//  Created by Wei Deng on 11/7/22.
//

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <utility>
#include <map>
#include <vector>
#include <stdio.h>
#include <set>
#include <algorithm>

using namespace std;

#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

//enum e_com {READ, PC, HELP, QUIT};
enum e_com {READ, PC, HELP, QUIT, LEV, LOGICSIM, RFL, DFS};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

class threeValue {
public:
   char value;

    threeValue(){
      this->value = 'x';
   }
    threeValue(char newVal){
      this->value = newVal;
   }

   threeValue operator|(const threeValue& target){
      threeValue result;
      if (this->value == '1' || target.value == '1'){
         result.value =  '1';
      }
      else if (this->value == 'x' || target.value == 'x'){
         result.value = 'x';
      }
      else {
         result.value = '0';
      }
      return result;
   } 

   threeValue operator^(threeValue target){
      threeValue result;
      if (this->value == 'x' || target.value == 'x'){
         result.value =  'x';
      }
      else if (this->value ==  target.value){
         result.value = '0';
      }
      else {
         result.value = '1';
      }
      return result;
   } 

   threeValue operator&(threeValue target){
      threeValue result;
      if (this->value == '0' || target.value == '0'){
         result.value =  '0';
      }
      else if (this->value == 'x' || target.value == 'x'){
         result.value = 'x';
      }
      else {
         result.value = '1';
      }
      return result;
   }  


   bool operator==(int target){
      threeValue result;
      if (this->value == '0' && target == 0){
         return true;
      }
      else if (this->value == '1' && target == 1){
         return true;
      }
      else {
         return false;
      }
   } 
   bool operator==(char target){
      threeValue result;
      if (this->value == target){
         return true;
      }
      else {
         return false;
      }
   } 



   threeValue operator!(){
      threeValue result;
     if (this->value == 'x'){
         result.value = 'x';
     }
     else if (this->value == '1'){
         result.value = '0';
     }
     else{
         result.value = '1';
     }
     return result;
   }  

   void operator=(threeValue target){
      this->value = target.value;
   }
   void operator=(int target){
      this->value = '0' + target;
   }
   void operator=(char target){
      this->value = target;
   }


   void operator=(bool target){
      if (target){
         this->value = '1';
      }
      else{
         this->value = '0';
      }
      
   }


};

struct cmdstruc {
   char name[MAXNAME];        /* command syntax */
   int (*fptr)(char *);             /* function pointer of the commands */
    //char * funcArg;
   enum e_state state;        /* execution state sequence */
};

typedef struct  n_struc {
   unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
   int num;              /* line number(May be different from indx */
   enum e_gtype type;         /* gate type */
   unsigned fin;              /* number of fanins */
   unsigned fout;             /* number of fanouts */
   struct n_struc **unodes;   /* pointer to array of up nodes */
   struct n_struc **dnodes;   /* pointer to array of down nodes */
   int level;                 /* level of the gate output */
   threeValue val;
   vector<string> fault;
} NSTRUC;

#define NUMFUNCS 8
int cread(char *), pc(char *), help(char *cp), quit(char *cp), lev(char *), logicsim(char*), rfl(char*), dfs(char*);

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
   /**/{"RFL", rfl, CKTLD},
   /**/{"DFS", dfs, CKTLD},
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
   char val;
   int i, j, k;
   level_num = 0;
   sscanf(cp, "%s %s", buf, outf);


   map<int, vector<char> > allInput;
   vector<string> lines;
   string line;

   ifstream input_file(buf);
   if (!input_file.is_open()) {
      cerr << "Could not open the file - '"
             << buf << "'" << endl;
      return 0;
   }

    // get input net ID
    getline(input_file, line);

   char charArrayLine [line.size() + 1];
   strcpy(charArrayLine, line.c_str());
   cout << charArrayLine << endl;
    char *token = strtok(charArrayLine, ",");
   
   vector <int> inputPorts;
    while (token != NULL)
    {
        //printf("%s\n", token);
        vector<char> newVec;
        int newKey = int(*token - '0');
        inputPorts.push_back(newKey);
        allInput.insert(pair<int, vector<char> >(newKey, newVec));
        token = strtok(NULL, ",");
    }



    // rest of the file
    while (getline(input_file, line)){
      char charArrayLine_temp [line.size() + 1];
      strcpy(charArrayLine_temp, line.c_str());
      cout << charArrayLine_temp << endl;
      char *token1 = strtok(charArrayLine_temp, ",");
   
      int index = 0;
       while (token1 != NULL)
       {
           //printf("---%s\n", token1);
           allInput[inputPorts.at(index)].push_back(*token1);
           token1 = strtok(NULL, ",");
           index += 1;
       }
       
       
       lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i++)
        cout << lines.at(i) << endl;

   map<int, vector<char> >::iterator itr;
    for(itr=allInput.begin();itr!=allInput.end();itr++)
    {
        cout<<itr->first<<" vector: ";
        for (int i = 0; i < itr->second.size(); i ++){
         cout<<itr->second.at(i)<<" ";
        }
        cout<<endl;
    }
    input_file.close();


   // while(fscanf(fd, "%d,%c", &nd, &val) != EOF){
   //    for(i = 0; i < Nnodes; i++){
   //      np = &Node[i];
   //      if(np->num == nd) np->val = val;
   //      if(np->level > level_num) level_num = np->level;
   //    }
   // }

    // output file section
    FILE *out_file;
    out_file = fopen(outf,"w");
    for(i = 0; i < Npo; i++){
      if (i == 0){
         fprintf(out_file,"%d",Poutput[i]->num);

      }
      else{
         fprintf(out_file,",%d",Poutput[i]->num);
      }
   }
   fprintf(out_file,"\n", NULL);

   // main procedure
   threeValue temp;

   // Loop each input vector
   for (int a = 0; a < allInput[inputPorts.at(0)].size(); a ++){
      level_num = 0;
      cout<<"in "<<endl;

      // initial port assignment
      map<int, vector<char> >::iterator itr1;
      for(itr1=allInput.begin();itr1!=allInput.end();itr1++)
      {
         cout<<itr1->first<<" vector: ";
         
         // loop every node for assignment
         for (int b = 0; b < Nnodes; b ++){
            np = &Node[b];
            if(np->num == itr1->first) np->val = allInput[itr1->first].at(a);
            if(np->level > level_num) level_num = np->level;

         }
      }


      // start logic sim
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
                     temp = np->unodes[0]->val;

                     for(k = 1; k < np->fin; k++){
                        temp = temp | np->unodes[k]->val;
                     }
                     temp = !temp;
                     np->val = temp;

                     break;
                  case 5: np->val = !np->unodes[0]->val;//NOT
                          break;
                  case 6: //NAND
                     // np->val = np->unodes[0]->val;
                     // for(k = 1; k < np->fin; k++){
                     //    np->val = np->val & np->unodes[k]->val;
                     // }

                     // np->val = np->val == 0 ? 1 : 0;
                    
                     temp = np->unodes[0]->val;

                     for(k = 1; k < np->fin; k++){
                        temp = temp & np->unodes[k]->val;
                     }
                     temp = !temp;
                     np->val = temp;

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

      for(i = 0; i < Npo; i++){
         if (i == 0){
            fprintf(out_file,"%c", Poutput[i]->val.value);
         }
         else{
            fprintf(out_file,",%c", Poutput[i]->val.value);
         }


      }
   fprintf(out_file,"\n", NULL);
   }
   cout << "done" <<endl;

    fclose(out_file);
    /*for(i = 0; i < Nnodes; i++){
      np = &Node[i];
      printf("output %d %d\n", np->num, np->val);
    } */
    return 1;
}


int rfl (char * cp) {
   FILE *fp;
   char fileName[MAXLINE];
   int i;
   NSTRUC *np;

   sscanf(cp, "%s", fileName);
   if((fp = fopen(fileName,"w")) == NULL) {
      cout << "OUT File "<<fileName<<" does not exist!\n";
      return 0;
   }

   for(i = 0; i < Nnodes; i++){
      np = &Node[i];
      if(np->type == IPT || np->type == BRCH){
         fprintf(fp,"%d@0\n", np->num);
         fprintf(fp,"%d@1\n", np->num);
      }
   }
   fclose(fp);
   return 1;
}

int dfs(char *cp)
{
   char buf[MAXLINE];
   char outf[MAXLINE];
   FILE *fd;
   NSTRUC *np;
   int nd;
   char val;
   int i, j, k;
   level_num = 0;
   sscanf(cp, "%s %s", buf, outf);
   threeValue parity;
   char fault_val;
   string f;//to store the fault value
   string num_str;
   map<int, vector<char> > allInput;
   vector<string> lines;
   string line;

   ifstream input_file(buf);
   if (!input_file.is_open()) {
      cerr << "Could not open the file - '"
             << buf << "'" << endl;
      return 0;
   }

    // get input net ID
    getline(input_file, line);

   char charArrayLine [line.size() + 1];
   strcpy(charArrayLine, line.c_str());
   cout << charArrayLine << endl;
    char *token = strtok(charArrayLine, ",");
   
   vector <int> inputPorts;
    while (token != NULL)
    {
        //printf("%s\n", token);
        vector<char> newVec;
        int newKey = int(*token - '0');
        inputPorts.push_back(newKey);
        allInput.insert(pair<int, vector<char> >(newKey, newVec));
        token = strtok(NULL, ",");
    }



    // rest of the file
    while (getline(input_file, line)){
      char charArrayLine_temp [line.size() + 1];
      strcpy(charArrayLine_temp, line.c_str());
      cout << charArrayLine_temp << endl;
      char *token1 = strtok(charArrayLine_temp, ",");
   
      int index = 0;
       while (token1 != NULL)
       {
           //printf("---%s\n", token1);
           allInput[inputPorts.at(index)].push_back(*token1);
           token1 = strtok(NULL, ",");
           index += 1;
       }
       
       
       lines.push_back(line);
    }
   cout << endl;
    for (int i = 0; i < lines.size(); i++)
        cout << lines.at(i) << endl;

   map<int, vector<char> >::iterator itr;
    for(itr=allInput.begin();itr!=allInput.end();itr++)
    {
        cout<<itr->first<<" vector: ";
        for (int i = 0; i < itr->second.size(); i ++){
         cout<<itr->second.at(i)<<" ";
        }
        cout<<endl;
    }
    input_file.close();


   // output file section
   FILE *out_file;
   out_file = fopen(outf,"w");

   // main procedure
   threeValue temp;

   // Loop each input vector
   for (int a = 0; a < allInput[inputPorts.at(0)].size(); a++){
      level_num = 0;
      cout<<endl<<"in ";

      // initial port assignment
      map<int, vector<char> >::iterator itr1;
      for(itr1=allInput.begin();itr1!=allInput.end();itr1++)
      {
         cout<<itr1->first<<" vector: ";
         
         // loop every node for assignment
         for (int b = 0; b < Nnodes; b ++){
            np = &Node[b];
            if(np->num == itr1->first) {
               np->val = allInput[itr1->first].at(a);
               f = "";
               fault_val = (np->val.value == '1') ? '0' : '1';
               f.insert(f.begin(), fault_val);
               num_str = to_string(np->num);
               np->fault.push_back(num_str + '@' + f);
            }
            if(np->level > level_num) level_num = np->level;
         }
      }

      // start logic sim
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
                     temp = np->unodes[0]->val;

                     for(k = 1; k < np->fin; k++){
                        temp = temp | np->unodes[k]->val;
                     }
                     temp = !temp;
                     np->val = temp;

                     break;
                  case 5: np->val = !np->unodes[0]->val;//NOT
                          break;
                  case 6: //NAND
                     temp = np->unodes[0]->val;

                     for(k = 1; k < np->fin; k++){
                        temp = temp & np->unodes[k]->val;
                     }
                     temp = !temp;
                     np->val = temp;

                     break;
                  case 7: //AND
                     np->val = np->unodes[0]->val;
                     for(k = 1; k < np->fin; k++){
                        np->val = np->val & np->unodes[k]->val;
                     }
                     break;
                     default: break;
               }
            }
         }
      }
      //generate a fault list
      vector<string> fault_list;
      for(i = 0; i < Nnodes; i++){
         f = "";
         fault_val = (np->val.value == '1') ? '0' : '1';
         f.insert(f.begin(), fault_val);
         num_str = to_string(np->num);
         fault_list.push_back(num_str + '@' + f);
      }

      
      // start fault dectective
      for(i = 1; i <= level_num; i++){
         for(j = 0; j < Nnodes; j++){
            np = &Node[j];
            if(np->level == i) {
               switch(np->type) {
                  //case 0: //PI
                  case 1: //branch
                     np->fault.insert(np->fault.end(),np->unodes[0]->fault.begin(),np->unodes[0]->fault.end());
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     np->fault.push_back(num_str + '@' + f);
                     break;
                  case 2: //XOR
                     for(k = 0; k < np->fin; k++){
                        np->fault.insert(np->fault.end(),np->unodes[k]->fault.begin(),np->unodes[k]->fault.end());
                     }
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     np->fault.push_back(num_str + '@' + f);
                     break;
                  case 3: //OR
                  case 4: //NOR
                     parity = '1';
                     for(k = 0; k < np->fin; k++){
                        parity = parity & np->unodes[k]->val;
                     }
                     if(parity == '1'){ 
                        for(k = 0; k < np->fin; k++){
                           np->fault.insert(np->fault.end(),np->unodes[k]->fault.begin(),np->unodes[k]->fault.end());
                        }
                     }
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     np->fault.push_back(num_str + '@' + f);
                     break;
                  case 5: //NOT
                     np->fault.insert(np->fault.end(),np->unodes[0]->fault.begin(),np->unodes[0]->fault.end());
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     np->fault.push_back(num_str + '@' + f);
                     break;
                  case 6: //NAND
                  case 7: //AND
                     parity = '1';
                     for(k = 0; k < np->fin; k++){
                        parity = parity & np->unodes[k]->val;
                     }

                     if(parity == '1'){ 
                        for(k = 0; k < np->fin; k++){
                           np->fault.insert(np->fault.end(),np->unodes[k]->fault.begin(),np->unodes[k]->fault.end());
                        }
                     }
                     else{
                        parity == '0';
                        for(k = 0; k < np->fin; k++){
                           parity = parity | np->unodes[k]->val;
                        }
                        if (parity == '0'){
                           vector<string>::iterator ptr;
                           for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                              int intersection = 0;
                              for(k = 0; k < np->fin; k++){
                                 if(find(np->unodes[k]->fault.begin(), np->unodes[k]->fault.end(), *ptr) != np->unodes[k]->fault.end()){
                                    intersection++;
                                 }
                              }
                              if(intersection == np->fin){
                                 np->fault.push_back(*ptr);
                              }
                           }
                        }
                        else{
                           for(k = 0; k < np->fin; k++){
                              if(np->unodes[k]->val.value == '0'){
                                 np->fault.insert(np->fault.end(),np->unodes[k]->fault.begin(),np->unodes[k]->fault.end());
                              }
                           }
                        }
                     }

                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     np->fault.push_back(num_str + '@' + f);
                     break;
                  default: break;
               }
            }
         }
      }

      vector<string> summary;
      for(i = 0; i < Npo; i++){
         summary.insert(summary.end(), Poutput[i]->fault.begin(), Poutput[i]->fault.end());   
      }
      set<string>s(summary.begin(), summary.end());
      summary.assign(s.begin(),s.end());
      for(i = 0; i < summary.size(); i++){
         const char* summary_str = summary[i].c_str();
         fprintf(out_file,"%s\n", summary_str);
      }

   fprintf(out_file,"\n", NULL);

   cout<<"check the fault"<<endl;
   for(i = 0; i < Nnodes; i++){
      np = &Node[i];
      cout << "the node" << np->num<<" ";
      for(j = 0; j < np->fault.size(); j++){
         cout<< np->fault[j] << " ";
      }
      cout<<endl;
   }
   cout <<endl<< "done" <<endl;

    fclose(out_file);
    return 1;
   }
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
