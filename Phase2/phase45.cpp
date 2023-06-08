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
#include <cstdlib>
#include <utility>
#include <map>
#include <vector>
#include <stdio.h>
#include <thread>
#include <pthread.h>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <set>
#include <chrono>
#include <mutex>
#include <sstream>

using namespace std;

#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

//enum e_com {READ, PC, HELP, QUIT};
enum e_com {READ, PC, HELP, QUIT, LEV, LOGICSIM, RFL, DFS, PFS, RTG};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

class threeValue {
public:
   char value;

    threeValue(){
      this->value = 'X';
   }
    threeValue(char newVal){
      this->value = newVal;
   }

   threeValue operator|(const threeValue& target){
      threeValue result;
      if (this->value == '1' || target.value == '1'){
         result.value =  '1';
      }
      else if (this->value == 'X' || target.value == 'X'){
         result.value = 'X';
      }
      else {
         result.value = '0';
      }
      return result;
   } 

   threeValue operator^(threeValue target){
      threeValue result;
      if (this->value == 'X' || target.value == 'X'){
         result.value =  'X';
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
      else if (this->value == 'X' || target.value == 'X'){
         result.value = 'X';
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
     if (this->value == 'X'){
         result.value = 'X';
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

   // basic_ostream& operator << (){
   //    os << this->value;
      
   // }


};

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
   threeValue val;
   //vector<string> fault;
} NSTRUC;
map<unsigned, vector<string>> fault;

#define NUMFUNCS 10
int cread(char *), pc(char *), help(char *cp), quit(char *cp), lev(char *), logicsim(char*), rfl(char*), dfs(char*), pfs(char*), rtg(char *);

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
int procWidth; //processor bit width
vector<vector<threeValue> > allFaultVal;
vector<int> inputPorts;
vector<string> allFaultList;

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
   {"DFS", dfs, CKTLD},
   {"PFS", pfs, CKTLD},
   {"RTG", rtg, CKTLD}
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

      // read all the possible faults

      string fault0 = to_string(np->num) + "@" + "0";
      string fault1 = to_string(np->num) + "@" + "1";
      allFaultList.push_back(fault0);
      allFaultList.push_back(fault1);

      if(tp == PI){
         Pinput[ni++] = np;
         // input ports added
         inputPorts.push_back(np->num);
      }
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
   sort(inputPorts.begin(), inputPorts.end());

   cout << "==> OK\n";

    return 1;
}



void lev_simulation(){
   FILE *fp;
   int assignedCounter = 0;
   int ready;
   int gateCount;
   int i, j;
   int maxInLevel;
   NSTRUC *np, *currNp;   
   
   
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
}

int lev (char * cp) {
   FILE *fp;
   char fileName[MAXLINE];
   int assignedCounter = 0;
   int ready;
   int gateCount;
   int i, j;
   int maxInLevel;
   NSTRUC *np, *currNp;

   sscanf(cp, "%s", fileName);
   if((fp = fopen(fileName,"w")) == NULL) {
      cout << "OUT File "<<fileName<<" does not exist!\n";
      return 0;
   }

   lev_simulation();

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



map<int, vector<char> > read_test_vector (char* fileName, vector<int> & tempInputPorts){

   map<int, vector<char> > allInput;
   vector<string> lines;
   string line;

   ifstream input_file(fileName);
   if (!input_file.is_open()) {
      cerr << "Could not open the file - '"<< fileName << "'" << endl;
      return allInput;
   }
    // get input net ID
   getline(input_file, line);

   char charArrayLine [line.size() + 1];
   strcpy(charArrayLine, line.c_str());
   //cout << charArrayLine << endl;
   char *token = strtok(charArrayLine, ",");
   
   while (token != NULL)
   {
      //printf("%s\n", token);
      vector<char> newVec;
      string keyStr (token);
      stringstream ss (keyStr);
      
      int newKey = 0;
      ss >> newKey;
      //cout << "newKey: " << newKey << " " << keyStr << endl;
      tempInputPorts.push_back(newKey);
      allInput.insert(pair<int, vector<char> >(newKey, newVec));
      token = strtok(NULL, ",");
   }



    // rest of the file
    while (getline(input_file, line)){
      char charArrayLine_temp [line.size() + 1];
      strcpy(charArrayLine_temp, line.c_str());
      //cout << charArrayLine_temp << endl;
      char *token1 = strtok(charArrayLine_temp, ",");
   
      int index = 0;
       while (token1 != NULL)
       {
           //printf("---%s\n", token1);
           allInput[tempInputPorts.at(index)].push_back(*token1);
           token1 = strtok(NULL, ",");
           index += 1;
       }
       
       //lines.push_back(line);
    }

    // for (int i = 0; i < lines.size(); i++)
    //     cout << lines.at(i) << endl;

//    map<int, vector<char> >::iterator itr;
//     for(itr=allInput.begin();itr!=allInput.end();itr++)
//     {
//         cout<<itr->first<<" vector: ";
//         for (int i = 0; i < itr->second.size(); i ++){
//          cout<<itr->second.at(i)<<" ";
//         }
//         cout<<endl;
//     }
    input_file.close();


    return allInput;
}


// mode = 0 logic simulation
// mode = 1 fault simulation
mutex myMutex;

void logic_simulation_evaluation (map<int, char> testVector, string fault, vector<char>& currentOutputVector, int faultValIndex, int mode){
   NSTRUC *np;
   int i, j ,k;
   threeValue temp;

   // if (mode == 1){
   //    NSTRUC * Node = copy_all_node ();
   // }

   unsigned faultNum;
   char faultValue;
   if (mode == 1){
      size_t pos = fault.find('@');
      if (pos == string::npos){         // if no dot
         cout << "no @ in fault\n";
      }
      else{

         faultNum = stoi(fault.substr(0, pos));
         faultValue = fault.substr(pos, string::npos) [1];
      }

      //cout << "faultNum: " << faultNum << " faultValue: " << faultValue << endl;
   }
      //cout<<"in "<<endl;

      // initial port assignment
      int levelNum = 0;
      map<int, char>::iterator itr1;
      for(itr1 = testVector.begin(); itr1 != testVector.end(); itr1++){
      
         //cout<<itr1->first<<" vector: ";
         
         // loop every node for assignment

         for (int b = 0; b < Nnodes; b ++){
            np = &Node[b];
            //lock_guard<mutex> g(myMutex);
            //cout << "Node " << np->num << " size: "<< np->faultVal.size() << endl;
            if(np->num == itr1->first) {
               //cout << "faultValIndex: "<< faultValIndex<< " np->faultVal.size(): " << np->faultVal.size() << endl;
               if (mode == 1){
                  allFaultVal.at(np->indx).at(faultValIndex) = testVector[itr1->first];
               }
               else{
                  np->val = testVector[itr1->first];
               }
            }
            if(np->level > levelNum) levelNum = np->level;

         }
      }
      //cout << "======================" << faultValue << endl;
      // start logic sim
      
      
      lock_guard<mutex> g(myMutex);
      threeValue finalValue;
      threeValue nextValue;
      //cout << "levelNum: " << levelNum << endl;
      for(i = 1; i <= levelNum; i++){
         for(j = 0; j < Nnodes; j++){
            np = &Node[j];
            
            
            if(np->level == i) {
               // assign fault value

               //cout << "np num: " << np->num <<endl;
               if (mode == 0){
                  finalValue = np->unodes[0]->val;
               }
               else if (mode == 1){
                  //cout << "mode:  " << mode<<endl;
                  if (np->unodes[0]->num == unsigned(faultNum)){
                     //cout << "fault applied" <<endl;
                     finalValue = faultValue;
                  }
                  else{
                     
                     finalValue = allFaultVal.at(np->unodes[0]->indx).at(faultValIndex);
                  }
               }
               //cout << "old value: " << np->unodes[0]->val.value << endl;
               //cout << "new value: " << finalValue.value << endl;

               switch(np->type) {
                  //case 0: //PI
                  case 1: //branch
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }
                     
                     break;
                  case 2: //XOR
                     
                     for(k = 1; k < np->fin; k++){
                        if (mode == 0){
                           nextValue = np->unodes[k]->val;
                        }
                        else if (mode == 1){
                           //cout << "mode:  " << mode<<endl;
                           if (np->unodes[k]->num == unsigned(faultNum)){
                              //cout << "fault applied" <<endl;
                              nextValue = faultValue;
                           }
                           else{
                              nextValue = allFaultVal.at(np->unodes[k]->indx).at(faultValIndex);
                           }
                        }
                        finalValue = finalValue ^ nextValue;
                     }
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }
                     break;
                  case 3: //OR
                     
                     for(k = 1; k < np->fin; k++){
                        if (mode == 0){
                           nextValue = np->unodes[k]->val;
                        }
                        else if (mode == 1){
                           //cout << "mode:  " << mode<<endl;
                           if (np->unodes[k]->num == unsigned(faultNum)){
                              //cout << "fault applied" <<endl;
                              nextValue = faultValue;
                           }
                           else{
                              nextValue = allFaultVal.at(np->unodes[k]->indx).at(faultValIndex);
                           }
                        }
                        finalValue = finalValue| nextValue;
                     }
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }
                     break;
                  case 4: //NOR
                  

                     for(k = 1; k < np->fin; k++){
                        if (mode == 0){
                           nextValue = np->unodes[k]->val;
                        }
                        else if (mode == 1){
                           //cout << "mode:  " << mode<<endl;
                           if (np->unodes[k]->num == unsigned(faultNum)){
                              //cout << "fault applied" <<endl;
                              nextValue = faultValue;
                           }
                           else{
                              nextValue = allFaultVal.at(np->unodes[k]->indx).at(faultValIndex);
                           }
                        }
                        finalValue = finalValue | nextValue;
                     }
                     finalValue = !finalValue;
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }

                     break;
                  case 5: //NOT
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = !finalValue;
                     }else{
                        np->val = !finalValue;
                     }
                  
                          break;
                  case 6: //NAND
                     for(k = 1; k < np->fin; k++){
                        if (mode == 0){
                           nextValue = np->unodes[k]->val;
                        }
                        else if (mode == 1){
                           //cout << "mode:  " << mode<<endl;
                           if (np->unodes[k]->num == unsigned(faultNum)){
                              //cout << "fault applied" <<endl;
                              nextValue = faultValue;
                           }
                           else{
                              nextValue = allFaultVal.at(np->unodes[k]->indx).at(faultValIndex);
                           }
                        }
                        finalValue = finalValue & nextValue;
                     }
                     finalValue = !finalValue;
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }

                     break;
                  case 7: //AND
                     for(k = 1; k < np->fin; k++){
                        if (mode == 0){
                           nextValue = np->unodes[k]->val;
                        }
                        else if (mode == 1){
                           //cout << "mode:  " << mode<<endl;
                           if (np->unodes[k]->num == unsigned(faultNum)){
                              //cout << "fault applied" <<endl;
                              nextValue = faultValue;
                           }
                           else{
                              nextValue = allFaultVal.at(np->unodes[k]->indx).at(faultValIndex);
                           }
                        }
                        finalValue = finalValue & nextValue;
                     }
                     if (mode == 1){
                        allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
                     }else{
                        np->val = finalValue;
                     }
                     break;
                   default:
                       
                       break;
               }
            }
         }
      }
      // if (mode == 0){
      //    cout << "GOLDEN out:" << Poutput[0]->val.value  << " " << Poutput[1]->val.value<< endl;
      // }
      // else{
      //    cout << "FAULT "<< fault<<" out:" << Poutput[0]->val.value  << " " << Poutput[1]->val.value<< endl;
      // }  
      //cout <<"Npo " << Npo << endl;

      for(i = 0; i < Npo; i++){
         if (mode == 1){
            if (Poutput[i]->num == faultNum){
               currentOutputVector.push_back(faultValue);
            }
            else{
               currentOutputVector.push_back(allFaultVal.at(Poutput[i]->indx).at(faultValIndex).value);
            }
         }
         else{
            currentOutputVector.push_back(Poutput[i]->val.value);
         }
      }
}


int logicsim(char *cp)
{
   char buf[MAXLINE];
   char outf[MAXLINE];
   FILE *fd;
   //NSTRUC *np;
   int nd;
   char val;
   int i, j, k;
   sscanf(cp, "%s %s", buf, outf);
   lev_simulation();

   map<int, vector<char> > allInput;
   vector <int> tempInputPorts;
   allInput = read_test_vector (buf, tempInputPorts);

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


   // Loop each input vector
   for (int a = 0; a < allInput[tempInputPorts.at(0)].size(); a ++){
      map<int, vector<char> >::iterator itr1;
      map<int, char> newTestVector;
      for(itr1=allInput.begin();itr1!=allInput.end();itr1++)
      {
         newTestVector.insert(pair<int, char> (itr1->first, itr1->second.at(a)));
      }

      vector<char> tempVec;
      //cout <<"running\n";
      logic_simulation_evaluation(newTestVector,"", tempVec,0,0);
      //cout <<"end running\n";
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



int pfs_single(map<int, char> testPatternSingle, string fault, int faultValIndex, vector<vector<char> >& allOutputVector, int index){
   vector <char> outputVector;
   //cout << "in pfs_single: fault " << fault << endl;
   logic_simulation_evaluation(testPatternSingle, fault, outputVector, faultValIndex,1);
   for(int i = 0; i < Npo; i++){
          allOutputVector.at(index).push_back(outputVector.at(i));
         
   }
   // cout << "output vector:" << endl;
   // for (char one : outputVector){
   //    cout << one << ", ";
   // }



   return 0;
}

int pfs_golden(map<int, char> testPatternSingle, vector<vector<char> >& allOutputVector){
   vector <char> outputVector;
   logic_simulation_evaluation(testPatternSingle, "", outputVector,0,0);
   for(int i = 0; i < Npo; i++){
          allOutputVector.at(0).push_back(outputVector.at(i));
         
   }
   // cout << "GOLDEN output vector:" << endl;
   // for (char one : outputVector){
   //    cout << one << ", ";
   // }
   // cout <<endl;
   return 0;
}

void parallel_fault_simulation_wrapper (map<int, vector<char> > & testPattern, vector<string> & faultList, vector<string> & detectedFault, int testVectorNum){

   int singleVec_total_threads;
   if (faultList.size() > procWidth -1){
      int rem = (faultList.size() % (procWidth-1) > 0) ? 1:0;
      singleVec_total_threads = (faultList.size()+faultList.size()/(procWidth-1) + rem);
   }
   else {
      singleVec_total_threads = (faultList.size()+1);
   }
// fault output thread
   
   //for each test vector

   int totalRun = 0;
   int single_totalRun = 0;
      
   int i = 0;
   int j = 0;
   int runTime = 0;
   // << "testVectorNum: " << testVectorNum << endl;
   while (i<testVectorNum){
      int counter = 0;
      int thread_width;
      // update thread number
    //   cout << "single_totalRun " << single_totalRun << endl;
    //   cout << "singleVec_total_threads " << singleVec_total_threads << endl;
      if (singleVec_total_threads-single_totalRun < procWidth){
         thread_width = singleVec_total_threads-single_totalRun;
      }
      else{
         thread_width = procWidth;
      }
    //  cout << "thread_width " << thread_width << endl;
    //   cout << "totalRun " << totalRun << endl;
      std::thread t[thread_width];
      vector<vector<char> > outputThread;

      for (int p = 0; p < thread_width; p ++){
         vector<char> newVec;
         outputThread.push_back(newVec);

      }

      // golden output thread
      map<int, char> newTestVector;
      map<int, vector<char> >::iterator itr1;
      for(itr1=testPattern.begin();itr1!=testPattern.end();itr1++)
      {
         newTestVector.insert(pair<int, char> (itr1->first, itr1->second.at(i)));
      }


      t[0] = std::thread(pfs_golden, newTestVector, std::ref(outputThread));
      counter += 1;
      totalRun += 1;
      single_totalRun += 1;

      // fault output thread
      int indention =  j;
      while(j < faultList.size()){
         
         t[counter]= std::thread(pfs_single, newTestVector, faultList[j], j, std::ref(outputThread), counter);
         counter+= 1;
         totalRun += 1;
         single_totalRun += 1;
         j += 1;
         if (counter == 64){
            break;
         }
      }


    // cout << "--------------@---------------" << endl;
      for(int k = 0; k<thread_width; k++){
         t[k].join();
      }
    //  cout << "-----------------------------" << endl;
      for (int p = 1; p < thread_width; p ++){


         // cout << "GOLD " << outputThread.at(0).at(0) << " " << outputThread.at(0).at(1) << " " << outputThread.at(0).at(2)
         // << " "<< faultList.at(p-1+indention) <<" "<<outputThread.at(p).at(0)
         //  << " " << outputThread.at(p).at(1) << " " << outputThread.at(p).at(2) << endl;
          bool notEqual = false;
          for (int s = 0; s < Npo; s++){
            if (outputThread.at(p).at(s) != outputThread.at(0).at(s)){
               notEqual = true;
               break;
            }
          }
         if (notEqual){
               
            //    cout <<"j: "<<j<< endl;
            //    cout << " fault detected: index: " << p-1+indention << endl;
               
               //cout <<"== p-1+indention: " << p-1+indention << endl;
               // if (!foudString(faultList.at(p-1+indention), detectedFault)){

               //    detectedFault.push_back(faultList.at(p-1+indention));
               // }

              
                  detectedFault.at(p-1+indention) = (faultList.at(p-1+indention));
               
               
               
         }
      }

      // next test vector
      if (j == faultList.size()){
         i += 1;
         j = 0; 
         single_totalRun = 0;   
      }
      runTime ++ ;
      //cout << "-----------------------------" << runTime<< endl;
   }
   cout <<"PFS totalRun : " << totalRun <<endl;

}


void detect_processor(){
   // detect processor bit width
   switch (sizeof(void*)){
      case 4: procWidth = 32;
         //cout << "32" << endl;
      break;
      case 8: procWidth = 64;
         //cout << "64" << endl;
      break;
   default:procWidth = 0;
         //cout << "error processor width" << endl;
      break;
   }
}


int pfs(char * cp){
   lev_simulation();
   detect_processor();
   //NSTRUC * AllNode = copy_all_node ();

   char inFileName0 [MAXLINE]; 
   char inFileName1 [MAXLINE];
   char outFileName [MAXLINE];

   string fileData;
   string line;
   vector<string> faultList;
   vector <string> detectedFault;
   map<int, vector<char> > testPattern;
   vector <int> tempInputPorts;

   sscanf(cp, "%s %s %s", inFileName0, inFileName1, outFileName);


   // read in fault list 
   fstream fp;
   fp.open(inFileName0, ios::in);

   while (getline(fp, line)){
      faultList.push_back(line);
   }
   for (int j = 0; j < Nnodes*2; j++){

         string newFault = "PlaceHolder";
         detectedFault.push_back(newFault);
   }
   // for (string line : faultList){
   //    cout << line << endl;
   // }

   fp.close();

   // read in test vector
   fp.open(inFileName1, ios::in);
   testPattern = read_test_vector(inFileName1 , tempInputPorts);

   fp.close();


   // thread operation
   
   parallel_fault_simulation_wrapper (testPattern, faultList, detectedFault, testPattern[tempInputPorts.at(0)].size());

   fstream ofp;
   ofp.open(outFileName,ios::out);
   for (int p = 0; p < detectedFault.size(); p ++){
         if (detectedFault.at(p).compare("PlaceHolder") != 0){
            ofp << detectedFault.at(p)  << endl;
         }
   }
   // Wait for threads, which are running concurrently, to finish

   ofp.close();
   cout << "DONE" <<endl;
   //cout <<"totalRun : " << totalRun <<endl;

  return (0);
}



void generate_random_test_vector (vector<char> & temp ){
   for (int i = 0; i < Npi; i ++){
      int number = rand() % 2;
      temp.push_back(char(number+'0'));
   }

}

void rtg_rand_vec_generate_wrapper(int nTFCR, vector<vector<char> > & outputTestVector){


   int totalRun = 0;
      
   int j = 0;

   while (totalRun < nTFCR){
      //cout << "in nTFCR" << nTFCR << endl;
      int counter = 0;
      int thread_width;
      // update thread number
      if (nTFCR - totalRun < procWidth){
         thread_width = nTFCR - totalRun;
      }
      else{
         thread_width = procWidth;
      }
      // cout << "thread_width " << thread_width << endl;
      // cout << "totalRun " << totalRun << endl;
      std::thread t[thread_width];
      for (int i = 0; i < thread_width; i ++){
            t[i]= std::thread(generate_random_test_vector,std::ref(outputTestVector.at(totalRun)));
            totalRun += 1;
      }
     
      
      for(int k = 0; k<thread_width; k++){
         t[k].join();
      }
   }


   // cout <<"RANDOM totalRun : " << totalRun <<endl;
   // cout << "RANDOM outputTestVector:"<< endl;
   // for (int i = 0; i < nTFCR; i ++){
   //    for (int j = 0; j < Npi; j ++){
   //       cout << outputTestVector.at(i).at(j) <<" ";
   //    }
   //    cout << endl;
   // }
   // cout << endl;

}


int rtg(char *cp){
   
   lev_simulation();
   detect_processor();
   char outFileName0 [MAXLINE]; 
   char outFileName1 [MAXLINE];
   fstream ofp0;
   fstream ofp1;

   int nTot;
   int nTFCR;
   int currentTestVecNum = 0;
   double FC;

   sscanf(cp, "%d %d %s %s", &nTot, &nTFCR, outFileName0, outFileName1);
   // cout << "nTot " << nTot << endl;
   // cout << "nTFCR " << nTFCR << endl;
   ofp0.open(outFileName0, ios::out);
   ofp1.open(outFileName1, ios::out);

   for (int i = 0; i < inputPorts.size(); i ++){
      if (i == 0){
         ofp0 << inputPorts.at(i);
      }
      else{
         ofp0 << "," << inputPorts.at(i);
      }
   }
   ofp0 << endl;

    vector<char> newVec1;
    generate_random_test_vector(newVec1);
    // for (char a : newVec1){
    //   cout << a << " ";
    // }
    // cout <<endl;

   vector<string> detectedFault;
for (int j = 0; j < Nnodes*2; j++){

         string newFault = "PlaceHolder";
         detectedFault.push_back(newFault);
   }
//    cout << "allFaultList.size(): " << allFaultList.size() << endl;
//    cout << "detectedFault.size(): " << detectedFault.size() << endl;
   while (currentTestVecNum < nTot){

      //cout << "currentTestVecNum" << endl;
      vector<vector<char> >randomTestVectors;
      for (int p = 0; p < nTFCR; p ++){
            vector<char> newVec;
            randomTestVectors.push_back(newVec);

      }
      rtg_rand_vec_generate_wrapper(nTFCR, randomTestVectors);

      // cout << "RANDOM outputTestVector:"<< endl;
      // for (int i = 0; i < nTFCR; i ++){
      //    for (int j = 0; j < Npi; j ++){
      //       cout << randomTestVectors.at(i).at(j) <<" ";
      //    }
      //    cout << endl;
      // }
      // cout << endl;
      map<int, vector<char> >randomTestVectorsMap;


      for (int i = 0; i < randomTestVectors.size(); i ++){
         for (int j = 0; j < Npi; j ++){
            randomTestVectorsMap[inputPorts.at(j)].push_back(randomTestVectors.at(i).at(j));
         }
      }

       // map<int, vector<char> >::iterator itr;
       // for(itr=randomTestVectorsMap.begin();itr!=randomTestVectorsMap.end();itr++)
       // {
       //     cout<<itr->first<<" vector: ";
       //     for (int i = 0; i < itr->second.size(); i ++){
       //      cout<<itr->second.at(i)<<" ";
       //     }
       //     cout<<endl;
       // }
       //produce all fault list
      //  for (int i = 0; i< allFaultList.size(); i ++){
      //     cout << allFaultList.at(i) << endl;
      //  }
      parallel_fault_simulation_wrapper (randomTestVectorsMap, allFaultList, detectedFault, nTFCR);
      int detectedCount = 0;
      for (int j = 0; j < detectedFault.size(); j ++){
         
         if (detectedFault.at(j).compare("PlaceHolder") != 0){
            detectedCount += 1;
         }
         
      }
      FC = (double) detectedCount / allFaultList.size()*100.0;

      //cout << "FC: " << FC << endl;
      

      currentTestVecNum += nTFCR;


      // Test Vectors output to file
      for (int j = 0; j < nTFCR; j ++){
         for (int i = 0; i < inputPorts.size(); i ++){
            if (i == 0){
               ofp0 << randomTestVectorsMap[inputPorts.at(i)].at(j);
            }
            else{
               ofp0 << "," << randomTestVectorsMap[inputPorts.at(i)].at(j);
            }
         }
         ofp0 << endl;
      }

      // FC coverage output to file
  
      ofp1 << std::fixed << std::setprecision(2)<< FC << endl;

   }

   ofp0.close();
   ofp1.close();
   
   // cout << "allFault detectedFault:"<< endl;
   // for (int i = 0; i < detectedFault.size(); i ++){
   //    cout << detectedFault.at(i) << endl;
   // }


   return 0;

}





int lp(char *cp)
{
   lev_simulation();
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
      vector<threeValue> oneFaultVal;
      for (int j = 0; j < Nnodes*2; j++){
         threeValue newVal;
         oneFaultVal.push_back(newVal);
      }
      allFaultVal.push_back(oneFaultVal);
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


   lev_simulation(); 

   //map<int, vector<char> > allInput;
   vector <int> tempInputPorts;
   allInput = read_test_vector (buf, tempInputPorts);
   FILE *out_file;
   out_file = fopen(outf,"w");

   // main procedure
   threeValue temp;
   vector<string> summary;
   // Loop each input vector
      // Loop each input vector
   for (int a = 0; a < allInput[tempInputPorts.at(0)].size(); a ++){
      map<int, vector<char> >::iterator itr1;
      cout<< allInput[tempInputPorts.at(0)].size();
      for(itr1=allInput.begin();itr1!=allInput.end();itr1++)
      {
         //cout<<itr1->first<<" vector: ";
         
         // loop every node for assignment
         for (int b = 0; b < Nnodes; b ++){
            np = &Node[b];
            if(np->num == itr1->first) {
               np->val = allInput[itr1->first].at(a);
            }
            if(np->level > level_num) level_num = np->level;
         }
      }
      map<int, char> newTestVector;
      for(itr1=allInput.begin();itr1!=allInput.end();itr1++)
      {
         newTestVector.insert(pair<int, char> (itr1->first, itr1->second.at(a)));
      }

      vector<char> tempVec;
      //cout <<"running\n";
      logic_simulation_evaluation(newTestVector,"", tempVec,0,0);

      //generate a fault list
      vector<string> fault_list;
      for(i = 0; i < Nnodes; i++){
         np = &Node[i];
         f = "";
         fault_val = (np->val.value == '1') ? '0' : '1';
         f.insert(f.begin(), fault_val);
         num_str = to_string(np->num);
         fault_list.push_back(num_str + '@' + f);
         //cout<<"fault_list is: "<<num_str<<"@"<<f<<"  ";
      }

      
      // start fault dectective
      vector<string>::iterator ptr;
      for(i = 0; i <= level_num; i++){
         for(j = 0; j < Nnodes; j++){
            np = &Node[j];
            if(np->level == i) {
               switch(np->type) {
                  case 0: //PI
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  case 1: //branch
                     fault[np->num].insert(fault[np->num].end(),fault[np->unodes[0]->num].begin(),fault[np->unodes[0]->num].end());
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  case 2: //XOR
                     //vector<string>::iterator ptr;
                     for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                        int intersection = 0;
                        for(k = 0; k < np->fin; k++){
                           if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) != fault[np->unodes[k]->num].end()){
                              intersection++;
                           }
                        }
                        if(intersection == 1){
                           fault[np->num].push_back(*ptr);
                        }
                     }
                     /*for(k = 0; k < np->fin; k++){
                        fault[np->num].insert(fault[np->num].end(),fault[np->unodes[k]->num].begin(),fault[np->unodes[k]->num].end());
                     }*/
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  case 3: //OR
                  case 4: //NOR
                     parity = '0';
                     for(k = 0; k < np->fin; k++){
                        parity = parity | np->unodes[k]->val;
                     }
                     if(parity == '0'){ 
                        for(k = 0; k < np->fin; k++){
                           fault[np->num].insert(fault[np->num].end(),fault[np->unodes[k]->num].begin(),fault[np->unodes[k]->num].end());
                        }
                     }
                     else{
                        parity == '1';
                        for(k = 0; k < np->fin; k++){
                           parity = parity & np->unodes[k]->val;
                        }
                        if (parity == '1'){
                           for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                              int intersection = 0;
                              for(k = 0; k < np->fin; k++){
                                 if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) != fault[np->unodes[k]->num].end()){
                                    intersection++;
                                 }
                              }
                              if(intersection == np->fin){
                                 fault[np->num].push_back(*ptr);
                              }
                           }
                           /*for(k = 0; k < np->fin; k++){
                              if(np->unodes[k]->val.value == '0'){
                                 fault[np->num].insert(np->fault.end(),fault[np->unodes[k]->num].begin(),fault[np->unodes[k]->num].end());
                              }
                           }*/
                        }
                        else{
                           for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                              //cout<<*ptr<<" "<<endl;
                              int intersection = 0;
                              for(k = 0; k < np->fin; k++){
                                 if(np->unodes[k]->val.value == '1'){
                                    if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) != fault[np->unodes[k]->num].end()){
                                       intersection++;
                                    }
                                 }
                                 else if (np->unodes[k]->val.value == '0'){
                                    if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) == fault[np->unodes[k]->num].end()){
                                       intersection++;
                                    }
                                 }
                              }
                              if(intersection == np->fin){
                                 fault[np->num].push_back(*ptr);
                              }
                           }
                        }
                     }
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  case 5: //NOT
                     fault[np->num].insert(fault[np->num].end(),fault[np->unodes[0]->num].begin(),fault[np->unodes[0]->num].end());
                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  case 6: //NAND
                  case 7: //AND
                     parity = '1';
                     for(k = 0; k < np->fin; k++){
                        parity = parity & np->unodes[k]->val;
                     }

                     if(parity == '1'){ 
                        for(k = 0; k < np->fin; k++){
                           fault[np->num].insert(fault[np->num].end(),fault[np->unodes[k]->num].begin(),fault[np->unodes[k]->num].end());
                        }
                     }
                     else{
                        parity == '0';
                        for(k = 0; k < np->fin; k++){
                           parity = parity | np->unodes[k]->val;
                        }
                        if (parity == '0'){
                           //vector<string>::iterator ptr;
                           //cout<<"wufei_test "<<np->num<<endl;
                           for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                              //cout<<*ptr<<" "<<endl;
                              int intersection = 0;
                              for(k = 0; k < np->fin; k++){
                                 if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) != fault[np->unodes[k]->num].end()){
                                    intersection++;
                                 }
                              }
                              if(intersection == np->fin){
                                 //cout<<"wufei"<<*ptr <<endl;
                                 fault[np->num].push_back(*ptr);
                              }
                           }
                        }
                        else{
                           for(ptr = fault_list.begin(); ptr < fault_list.end(); ptr++){
                              //cout<<*ptr<<" "<<endl;
                              int intersection = 0;
                              for(k = 0; k < np->fin; k++){
                                 if(np->unodes[k]->val.value == '0'){
                                    if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) != fault[np->unodes[k]->num].end()){
                                       intersection++;
                                    }
                                 }
                                 else if (np->unodes[k]->val.value == '1'){
                                    if(find(fault[np->unodes[k]->num].begin(), fault[np->unodes[k]->num].end(), *ptr) == fault[np->unodes[k]->num].end()){
                                       intersection++;
                                    }
                                 }
                              }
                              if(intersection == np->fin){
                                 fault[np->num].push_back(*ptr);
                              }
                           }
                           /*for(k = 0; k < np->fin; k++){
                              if(np->unodes[k]->val.value == '0'){
                                 fault[np->num].insert(fault[np->num].end(),fault[np->unodes[k]->num].begin(),fault[np->unodes[k]->num].end());
                              }
                           }*/
                        }
                     }

                     f = "";
                     fault_val = (np->val.value == '1') ? '0' : '1';
                     f.insert(f.begin(), fault_val);
                     num_str = to_string(np->num);
                     fault[np->num].push_back(num_str + '@' + f);
                     break;
                  default: break;
               }
            }
         }
      }


      for(i = 0; i < Npo; i++){
         summary.insert(summary.end(), fault[Poutput[i]->num].begin(), fault[Poutput[i]->num].end());   
      }
      set<string>s(summary.begin(), summary.end());
      summary.assign(s.begin(),s.end());
      /*for(i = 0; i < summary.size(); i++){
         const char* summary_str = summary[i].c_str();
         fprintf(out_file,"%s\n", summary_str);
      }*/


   cout<<"check the fault"<<endl;
   /*for(i = 0; i < Nnodes; i++){
      np = &Node[i];
      cout << "the node" << np->num<<" ";
      for(j = 0; j < fault[np->num].size(); j++){
         cout<< fault[np->num][j] << " ";
      }
      cout<<endl;
   }*/
   cout <<endl<< "done" <<endl;
   }
    for(i = 0; i < summary.size(); i++){
         const char* summary_str = summary[i].c_str();
         fprintf(out_file,"%s\n", summary_str);
    }
    fclose(out_file);
}