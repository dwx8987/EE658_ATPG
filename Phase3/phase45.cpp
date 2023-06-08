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
#include <stdexcept>
#include <time.h>

using namespace std;

#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */

#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

//enum e_com {READ, PC, HELP, QUIT};
enum e_com {READ, PC, HELP, QUIT, LEV, LOGICSIM, RFL, DFS, PFS, RTG, DALG, PODEM, ATPG_DET, ATPG, LP};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

class threeValue {
public:
   char value;

   char getValue(){
      return this->value;
   }

    threeValue(){
      this->value = 'X';
   }
    threeValue(char newVal){
      this->value = newVal;
   }
   threeValue(int newVal){
      if (newVal == 1){
         this->value = '1';
      }else if (newVal == 0){
         this->value = '0';
      }
      else{
         //cout << "constructor ERROR newVal = " << newVal << "\n";
         //throw invalid_argument("ERROR ");
      }
      
   }

   threeValue operator|(threeValue& target){
      threeValue result('X');
      if (this->value == '1' || target.value == '1'){
         result.value =  '1';
      }
      else if (this->value == 'X' || target.value == 'X'){
         result.value = 'X';
      }
      else if (this->value == 'D' || target.value == 'D'){
         if ((this->value == 'D' && target.value == 'E') || (this->value == 'E' && target.value == 'D')){
            result.value = '1';
         }
         else{
            result.value = 'D';
         }
      }
      else if (this->value == 'E' || target.value == 'E'){
         //cout << "CORRECT\n";
         result.value = 'E';
      }
      else {
         result.value = '0';
      }
      return result;
   } 

   threeValue operator^(threeValue target){
      threeValue result('X');
      if (this->value == 'X' || target.value == 'X'){
         result.value =  'X';
      }
      else if (this->value ==  target.value){
         result.value = '0';
      }

      else if((this->value == 'D' && target.value == '0') || (this->value == '0' && target.value == 'D')){
         result.value = 'D';
      }
       else if((this->value == 'E' && target.value == '0') || (this->value == '0' && target.value == 'E')){
         result.value = 'E';
      }
      else if((this->value == 'D' && target.value == '1') || (this->value == '1' && target.value == 'D')){
         result.value = 'E';
      }
       else if((this->value == 'E' && target.value == '1') || (this->value == '1' && target.value == 'E')){
         result.value = 'D';
      }
      
      else {
         result.value = '1';
      }
      return result;
   } 

   threeValue operator&(threeValue target){
      threeValue result('X');
      if (this->value == '0' || target.value == '0'){
         result.value =  '0';
      }
      else if (this->value == 'X' || target.value == 'X'){
         result.value = 'X';
      }
      else if (this->value == 'D' || target.value == 'D'){
         if ((this->value == 'D' && target.value == 'E') || (this->value == 'E' && target.value == 'D')){
            result.value = '0';
         }
         else{
            result.value = 'D';
         }
      }
      else if (this->value == 'E' || target.value == 'E'){
         result.value = 'E';
      }
      else {
         result.value = '1';
      }
      return result;
   }  


   bool operator==(int target){
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
      if (this->value == target){
         return true;
      }
      else {
         return false;
      }
   } 

   bool operator==(threeValue target){
      if (this->value == target.value){
         return true;
      }
      else {
         return false;
      }
   } 
  

   bool operator!=(int target){
      cout << "operator != (int) detected. Need to complete operator!= first\n";
      if (int(this->value -'0') != target){
         return true;
      }
      else {
         return false;
      }
   } 

   bool operator!=(threeValue target){
      if (this->value != target.value){
         return true;
      }
      else {
         return false;
      }
   } 

   bool operator!=(char target){
      if (this->value != target){
         return true;
      }
      else {
         return false;
      }
   } 



   threeValue operator!(){
      threeValue result('X');
     if (this->value == 'X'){
         result.value = 'X';
     }
     else if (this->value == '1'){
         result.value = '0';
     }
     else if (this->value == 'D'){
         result.value = 'E';
     }
     else if (this->value == 'E'){
         result.value = 'D';
     }
     else{
         result.value = '1';
     }
     return result;
   }  
   threeValue operator~(){
      threeValue result('X');
     if (this->value == 'X'){
         result.value = 'X';
     }
     else if (this->value == '1'){
         result.value = '0';
     }
     else if (this->value == 'D'){
         result.value = 'E';
     }
     else if (this->value == 'E'){
         result.value = 'D';
     }
     else{
         result.value = '1';
     }
     return result;
   } 

   void operator=(threeValue target){
      //cout << "target.value = " << target.value << endl;
      this->value = target.value;
   }
   void operator=(int target){
      if (target == 0){
         this->value = '0' ;
      }else{
         this->value = '1' ;
      }
      
   }
   void operator=(char target){
      this->value = target;
   }

   int toInt(){
      if (this->value == '0'){
         return 0;
      }
      else{
         return 1;
      }
     
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

   friend inline std::ostream& operator<<(std::ostream& os, const threeValue& vec) {
        //os << (vec.m_format == Format::EVALUATION ? "EVAL: " : "COEF: ") << vec.GetValues();
         os << vec.value << endl;
        return os;
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

#define NUMFUNCS 15
int cread(char *), pc(char *), help(char *cp), quit(char *cp), lev(char *), lp(char *), logicsim(char*), rfl(char*), dfs(char*), pfs(char*), rtg(char *), dalg(char *), podem(char*), atpg_det(char *), atpg (char *);

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

// D-ALG
int dalgNodeNumber = -1;
threeValue dalgFaultValue = -1;

int podemNodeNumber;
threeValue podemFaultValue;

int maxLevel;

e_com& operator++(e_com& e_comID){
  return e_comID = static_cast<e_com>(static_cast<int>(e_comID)+1 );
}

struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread,EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   /**/{"LEV", lev, CKTLD},
   {"LP", lp, CKTLD},
   /**/{"LOGICSIM", logicsim, CKTLD},
   /**/{"RFL", rfl, CKTLD},
   {"DFS", dfs, CKTLD},
   {"PFS", pfs, CKTLD},
   {"RTG", rtg, CKTLD},
   {"DALG", dalg, CKTLD},
   {"PODEM", podem, CKTLD},
   {"ATPG_DET", atpg_det, EXEC},
   {"ATPG", atpg, EXEC}
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


map<int, int> numToIndx;
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
   int startIndx = 0;
   int endIndx = 0;
   cout << "buf = " << buf << endl;
   while (buf[endIndx] != '.'){
      endIndx = endIndx + 1;
      
   }
   startIndx = endIndx;
   cout << "endIndx: " << endIndx << endl;

   while ((buf[startIndx] != '/') && startIndx != 0){
      startIndx = startIndx - 1;
   }
   if (buf[startIndx] == '/'){
      startIndx += 1;
   }
   
   cout << "startIndx: " << startIndx << endl;

   for (int u = 0; u < (endIndx - startIndx) ; u ++){
      FILENAME[u] = buf[startIndx + u];
   }
   FILENAME[endIndx - startIndx] = '\0';


   cout << "FILENAME: " << FILENAME << endl;

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
      numToIndx[nd] = np->indx;
      

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
   
   maxLevel = 0;
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
            if (maxLevel < currNp->level){
               maxLevel = currNp->level;
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


int find_control_value(int gateIndx){
   //cout << "in find_control_value() type: " <<Node[gateIndx].type<<endl;
   switch(Node[gateIndx].type) {
      case 0: //PI
         return 3;
      case 1: //branch
         return 3;
      case 2: //XOR
         return 0;
      case 3: //OR
         return 1;
      case 4: //NOR
         return 1;
      case 5: //NOT
         return 3;
      case 6: //NAND
         return 0;
      case 7: //AND
         return 0;

      default: return 3;
   }

}


void print_all_node(vector<threeValue> allNodes, vector<int> DFrontier, vector<int> JFrontier){
   //lp();
   // cout << "print all node:\n";
   // for (int i = 0; i < Nnodes; i ++){
   //    cout << "num: " << Node[i].num << " val = " << allNodes.at(i);
   // }
   cout << "J_Frontier: ";
   for (int i = 0; i < JFrontier.size(); i ++){
      cout << JFrontier.at(i) << "("<<Node[JFrontier.at(i)].num <<") ";
   }
   cout << endl;

   cout << "D_Frontier: ";
   for (int i = 0; i < DFrontier.size(); i ++){
      cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
   }
   cout << endl;
}

void print_all_nodes(vector<threeValue> allNodes, vector<int> DFrontier, vector<int> JFrontier){
   //lp();
   cout << "print all node:\n";
   for (int i = 0; i < Nnodes; i ++){
      cout << "num: " << Node[i].num << " val = " << allNodes.at(i);
   }
   cout << "J_Frontier: ";
   for (int i = 0; i < JFrontier.size(); i ++){
      cout << JFrontier.at(i) << "("<<Node[JFrontier.at(i)].num <<") ";
   }
   cout << endl;

   cout << "D_Frontier: ";
   for (int i = 0; i < DFrontier.size(); i ++){
      cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
   }
   cout << endl;
}

// mode 0 - logicsim
// mode 1 - pfs
// mode 2 - forward implication
// mode 3 - imply only
bool single_node_logicsim (vector<threeValue> & dalgNodes, vector<int> & DFrontier, bool &assignOccurF, int faultNum, char faultValue, int faultValIndex, int nodeIndx, int mode){
   //cout << "np num: " << np->num <<endl;
   
   threeValue finalValue;
   threeValue nextValue;
   NSTRUC *np;
   int k;
   bool XvalueDetected = false;
   bool errorDetected = false;
   
   np = &Node[nodeIndx];

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
   else if (mode == 2 ||mode == 3){
      finalValue = dalgNodes.at(np->unodes[0]->indx);
      

      if (finalValue == 'X'){
         XvalueDetected = true;
      }
      else if (finalValue == 'D' || finalValue == 'E'){
         errorDetected = true;
      }
   }
   
 
   //cout << "old value: " << np->unodes[0]->val.value << endl;
   //cout << "new value: " << finalValue.value << endl;

   
   switch(np->type) {
      //case 0: //PI
      case 1: //branch
         if (finalValue == 'X'){
            XvalueDetected = true;
         }
         else if (finalValue == 'D' || finalValue == 'E'){
            errorDetected = true;
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
            else if (mode == 2 ){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
               if (nextValue == 'X'){
                  XvalueDetected = true;
               }
               else if (nextValue == 'D' || nextValue == 'E'){
                  errorDetected = true;
               }
            }
            else if(mode == 3){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
            }
            // cout << ":" << finalValue << endl;
            // cout << "-:" << nextValue << endl;
            finalValue = finalValue ^ nextValue;
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
            else if (mode == 2){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
               if (nextValue == 'X'){
                  XvalueDetected = true;
                 
               }
               else if (nextValue == 'D' || nextValue == 'E'){
                  errorDetected = true;
               }
            }
            else if(mode == 3){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
            }
            // cout << ":" << finalValue << endl;
            // cout << "-:" << nextValue << endl;
            finalValue = finalValue| nextValue;
            // cout << "+:" << finalValue << endl;
            // cout << "+:" << (finalValue| nextValue) << endl;
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
            else if (mode == 2){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
               if (nextValue == 'X'){
                  XvalueDetected = true;
                  
               }
               else if (nextValue == 'D' || nextValue == 'E'){
                  errorDetected = true;
               }
            }
            else if(mode == 3){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
            }
            finalValue = finalValue | nextValue;
         }

         finalValue = !finalValue;


         break;
      case 5: //NOT
         finalValue = !finalValue;
         if (finalValue == 'X'){
            XvalueDetected = true;
         }
         else if (finalValue == 'D' || finalValue == 'E'){
            errorDetected = true;
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
            else if (mode == 2){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
               if (nextValue == 'X'){
                  XvalueDetected = true;
                  
               }
               else if (nextValue == 'D' || nextValue == 'E'){
                  errorDetected = true;
               }
            }
            else if(mode == 3){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
            }
            finalValue = finalValue & nextValue;
         }
         finalValue = !finalValue;



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
            else if (mode == 2 ){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
               if (nextValue == 'X'){
                  XvalueDetected = true;
                  
               }
               else if (nextValue == 'D' || nextValue == 'E'){
                  errorDetected = true;
               }
            }
            else if(mode == 3){
               nextValue = dalgNodes.at(np->unodes[k]->indx);
            }

            finalValue = finalValue & nextValue;
         }
         break;

         default:
            
            break;
   }

   // update value to node j
   //cout << "DEMO np->num: " << np->num << " finalValue = " << finalValue << endl;
   //cout << "DEMO np->num: " << np->num << " dalgNodes.at(np->indx) = " <<dalgNodes.at(np->indx)<<" finalValue = " << finalValue << endl;
   if (mode == 1){
      allFaultVal.at(np->indx).at(faultValIndex) = finalValue;
      
   }
   else if(mode == 3){
      assignOccurF = true;
      dalgNodes.at(np->indx)  = finalValue;
      if (finalValue != 'X' && podemNodeNumber == np->num){
         //cout << "applied D\\E \n" << endl;
         if (finalValue == 0){
            dalgNodes.at(np->indx)  = 'E';
         }else{
            dalgNodes.at(np->indx)  = 'D';
         }
      }

      if (finalValue != 'X'){
         vector<int>::iterator it;
         it = find(DFrontier.begin(), DFrontier.end(), np->indx);

         if (it != DFrontier.end()){
            //cout << "\t\tDFrontier DELETE num: " <<np->num << "\n";
            DFrontier.erase(it);
         }
      }
      

      if (np->fout != 0){
         // cout << "\t\t in level 1 finalValue = " << finalValue << " np->dnodes[0]->num: " << np->dnodes[0]->num  << " np->dnodes[0]->indx: " << np->dnodes[0]->indx<<"\n";
         // cout << "\t\t dalgNodes.at(np->dnodes[0]->indx) = " << dalgNodes.at(np->dnodes[0]->indx) << "\n";
         if (dalgNodes.at(np->dnodes[0]->indx) == 'X' && (finalValue == 'D' || finalValue == 'E')){
            // cout << "\t\t in level 2\n";
            // cout << "old curr DFrontier: ";
            //    for (int i = 0; i < DFrontier.size(); i ++){
            //       cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
            //    }
            // cout << endl;
            vector<int>::iterator it0;
            it0 = find(DFrontier.begin(), DFrontier.end(), np->dnodes[0]->indx);
            if (it0 == DFrontier.end()){
               //cout << "\t\t DFrontier ADD--------------------------------- num: " << np->dnodes[0]->num << "\n";
               DFrontier.push_back(np->dnodes[0]->indx);
               // cout << "curr DFrontier: ";
               // for (int i = 0; i < DFrontier.size(); i ++){
               //    cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
               // }
               //cout << endl;
            }
         }
         
      }
      
      

   }
   else if (mode == 2 && finalValue!= 'X'){
     
      if (dalgNodes.at(np->indx) != 'X' ){
         if (dalgNodes.at(np->indx) != finalValue){
            if (dalgNodes.at(np->indx) == 'D' && finalValue == 1 && dalgNodeNumber == np->num){
               return true;
            }
            else if (dalgNodes.at(np->indx) == 'E' && finalValue == 0 && dalgNodeNumber == np->num){
               return true;
            }
            else{
               cout << "FI INCONSIST current num: "<< np->num << " finalValue: " << finalValue << " dalgNodes.at(np->indx): " << dalgNodes.at(np->indx)<< endl;
               return false;

            }
         }else{
            return true;
            //cout << "STAY np->num: " << np->num << " finalValue = " << finalValue << endl;
         }
         
      }
      else{
         //cout << "\tSUCCESS np->num: " << np->num << " type: " << np->type << " fin: " << np->fin << " finalValue = " << finalValue << endl;
         //cout << "DOwn: " << np->dnodes[0]->num << endl;
         if (finalValue == 'X'){
            cout << "\t\tinput0: " << dalgNodes.at(np->unodes[0]->indx) << " input1: " << dalgNodes.at(np->unodes[1]->indx) << " Xtrue: " << XvalueDetected << endl;
            throw std::invalid_argument("AddPositiveIntegers arguments must be positive");
          
         }
         
         assignOccurF = true;
      
         dalgNodes.at(np->indx) = finalValue;


         vector<int>::iterator it;
         it = find(DFrontier.begin(), DFrontier.end(), np->indx);

         if (it != DFrontier.end()){
            //cout << "\t\tDFrontier DELETE num: " <<np->num << "\n";
            DFrontier.erase(it);
         }
       
         //cout<< "\t\tnp->fout: " << np->fout << "\n";
         if (np->fout != 0){
            // cout << "\t\t in level 1 finalValue = " << finalValue << " np->dnodes[0]->num: " << np->dnodes[0]->num  << " np->dnodes[0]->indx: " << np->dnodes[0]->indx<<"\n";
            // cout << "\t\t dalgNodes.at(np->dnodes[0]->indx) = " << dalgNodes.at(np->dnodes[0]->indx) << "\n";
            if (dalgNodes.at(np->dnodes[0]->indx) == 'X' && (finalValue == 'D' || finalValue == 'E')){
               // cout << "\t\t in level 2\n";
               // cout << "old curr DFrontier: ";
               //    for (int i = 0; i < DFrontier.size(); i ++){
               //       cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
               //    }
               // cout << endl;
               vector<int>::iterator it0;
               it0 = find(DFrontier.begin(), DFrontier.end(), np->dnodes[0]->indx);
               if (it0 == DFrontier.end()){
                  //cout << "\t\t DFrontier ADD--------------------------------- num: " << np->dnodes[0]->num << "\n";
                  DFrontier.push_back(np->dnodes[0]->indx);
                  //cout << "curr DFrontier: ";
                  for (int i = 0; i < DFrontier.size(); i ++){
                     cout << DFrontier.at(i) << "("<<Node[DFrontier.at(i)].num <<") ";
                  }
                  cout << endl;
               }
            }
            
         }
         
        
         return true;
      }
      
   }
   else if (mode == 0){
      np->val = finalValue;
   }
   return true;
   
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
      
      
      // lock_guard<mutex> g(myMutex);
      // threeValue finalValue;
      // threeValue nextValue;
      //cout << "levelNum: " << levelNum << endl;
      for(i = 1; i <= levelNum; i++){
         for(j = 0; j < Nnodes; j++){
            if(Node[j].level == i) {
               vector<threeValue> placeHolder;
               vector<int> placeHolder2;
               bool placeHolder3;
            single_node_logicsim (placeHolder, placeHolder2, placeHolder3,  faultNum, faultValue, faultValIndex, j, mode);
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


   //cout << "--------------@--------------- i = " << i << endl;
      for(int k = 0; k<thread_width; k++){
         t[k].join();
      }
    //cout << "----------------------------- i = " << i  << endl;
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

void rtg_subroutine (string outFileName0,  string outFileName1, int nTot, int nTFCR, vector<string> & detectedFault, vector<vector<char> >& allTestVectors, int mode){

   double FC;
   fstream ofp0;
   fstream ofp1;
   if (mode == 0){
      ofp0.open(outFileName0, ios::out);
      ofp1.open(outFileName1, ios::out);
   }
   
   int currentTestVecNum = 0;
   
   if (mode == 0){
      for (int i = 0; i < inputPorts.size(); i ++){
         if (i == 0){
            ofp0 << inputPorts.at(i);
         }
         else{
            ofp0 << "," << inputPorts.at(i);
         }
      }
      ofp0 << endl;
   }
   
    // for (char a : newVec1){
    //   cout << a << " ";
    // }
    // cout <<endl;

//    cout << "allFaultList.size(): " << allFaultList.size() << endl;
//    cout << "detectedFault.size(): " << detectedFault.size() << endl;
   while (1){

      if (mode == 0 && currentTestVecNum >= nTot){
         break;
      }
      if (mode == 1 && FC > 70){
         break;
      }

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

      allTestVectors = randomTestVectors;

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


      if (mode == 0){
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
     

   }

   ofp0.close();
   ofp1.close();

}


int rtg(char *cp){
   
   lev_simulation();
   detect_processor();
   string outFileName0 ;
   string outFileName1 ;

   int nTot;
   int nTFCR;
   double FC;

   stringstream ss;
   ss << cp;

   string nTotS;
   string nTFCRS;
   ss >> nTotS;
   ss >> nTFCRS;
   ss >> outFileName0;
   ss >> outFileName1;
   nTot = stoi(nTotS);
   nTFCR = stoi(nTFCRS);
   cout << outFileName0 << endl;
   cout << outFileName1 << endl;;

   // cout << "nTot " << nTot << endl;
   // cout << "nTFCR " << nTFCR << endl;
   
   vector<string> detectedFault;
   for (int j = 0; j < Nnodes*2; j++){

      string newFault = "PlaceHolder";
      detectedFault.push_back(newFault);
   }

   vector<vector<char> > allTestVectors;
   rtg_subroutine (outFileName0, outFileName1, nTot, nTFCR, detectedFault, allTestVectors, 0);
   
   
   // cout << "allFault detectedFault:"<< endl;
   // for (int i = 0; i < detectedFault.size(); i ++){
   //    cout << detectedFault.at(i) << endl;
   // }


   return 0;

}





int lp(char *)
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
   allFaultList.clear();
   allFaultVal.clear();
   inputPorts.clear();

   
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
   maxLevel = 0;
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


//============================================================================================================================================================
// D-ALG

class dalgFaultValueFault{};

bool errorAtOutput = false;


int dalg_initialize_nodes(vector <threeValue> &allNodes, int nodeNumber, int faultValue, vector<int> & DFrontier){
   int indx = -1;
   allNodes.clear();
   for (int i = 0; i < Nnodes; i ++){
      if (Node[i].num == dalgNodeNumber){
         indx = Node[i].indx;
      }
   }
   if (indx == -1){
      cout << "ERROR dlag_initialize_nodes Fails: indx == -1\n";
      return 0;
   }
   for (int i = 0; i < Nnodes; i ++){
      threeValue newNode('X');
      if (i == indx){
         cout << "fault node indx = " << i << endl;
         if (Node[indx].fout > 0){
            if (Node[indx].dnodes[0]->type != 1){
               DFrontier.push_back(Node[indx].dnodes[0]->indx);
            }
         }
         
         try {
            if (dalgFaultValue == 1){
               newNode = 'E';
            }
            else if (dalgFaultValue == 0){
               newNode = 'D';
            }
            else{
               throw dalgFaultValueFault();
            }
         }
         catch (dalgFaultValueFault a){
            cout << "Error in dalg_initialize_nodes(), \"dalgFaultValue\" unknown.\n";
         }
         
      }
      allNodes.push_back(newNode);
   }
   cout << "dalg_initialize_nodes DONE\n";
   return 1;
}



bool checkErrorAtPO(vector<threeValue> delagNodes, int &num){
   for(int i = 0; i < Npo; i++){
      if (delagNodes.at(Poutput[i]->indx) == 'D' || delagNodes.at(Poutput[i]->indx) == 'E'){
         num = Poutput[i]->num;
         return true;
      }
   }
   return false;
}


// mode 0 - logicsim
// mode 1 - pfs
// mode 2 - forward implication

bool backward_implication (vector<threeValue> & dalgNodes,vector<int> & JFrontier, int nodeIndx, bool &assignOccurB){
   //cout << "np num: " << np->num <<endl;
   threeValue finalValue;
   threeValue nextValue;
   NSTRUC *np;
   int k;
   np = &Node[nodeIndx];

  
   threeValue currentValue = dalgNodes.at(np->indx);
   if (currentValue == 'D' && dalgNodeNumber == np->num){
      currentValue = 1;
   }
   if (currentValue == 'E' && dalgNodeNumber == np->num){
      currentValue = 0;
   }
   //cout << "old value: " << np->unodes[0]->val.value << endl;
   //cout << "new value: " << finalValue.value << endl;
   if (currentValue == 'D' || currentValue == 'E' ||currentValue == 'X'){
      return true;
   }
  

   bool inconsistDetected = false;
   bool JFrontierAdd = false;
   bool assignOccur = false;

   int Xcounter = 0;
   int kValue = -1;
   
   switch(np->type) {
      //case 0: //PI
      case 1: //branch
         if (dalgNodes.at(np->unodes[0]->indx) == 'X'){
            dalgNodes.at(np->unodes[0]->indx) = currentValue;
            //cout << "Branch Backward assigned Node num: " << np->num << "\n";
            assignOccur = true;
         }
         else{
            if (dalgNodes.at(np->unodes[0]->indx) != currentValue){
               cout << "BI INCONSIST BRANCH current num: "<< np->num << " dalgNodes.at(np->unodes[0]->indx): " << dalgNodes.at(np->unodes[0]->indx) << " currentValue: " << currentValue<< endl;
               inconsistDetected = true;

               break;
            }
            
         }

         break;
      case 2: //XOR

         finalValue = 'X';
         finalValue = dalgNodes.at(np->unodes[0]->indx);
         for(k = 0; k < np->fin; k++){
            if (dalgNodes.at(np->unodes[k]->indx) != 'X'){
               if (finalValue == 'X'){
                  finalValue = dalgNodes.at(np->unodes[k]->indx);
               }
               else{
                  nextValue = dalgNodes.at(np->unodes[k]->indx);
                  finalValue = finalValue ^ nextValue;
               }
            }
            else{
               Xcounter ++;
               kValue = k;
            }
         }
         if (Xcounter == 0){
            if (currentValue != finalValue){
               cout << "BI INCONSIST XOR current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
               inconsistDetected = true;
            }
            
         }
         else if (Xcounter == 1){
            assignOccur = true;
            dalgNodes.at(np->unodes[kValue]->indx) = currentValue ^ finalValue;
         }
         else{
            //j_frontier
            JFrontierAdd = true;
         }

         break;
      case 3: //OR---------------------------------------------------
     
         
         if (currentValue == '0'){
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) == 'X'){
                  dalgNodes.at(np->unodes[k]->indx) = '0'; 
                  assignOccur = true;
                 
               }
               else{
                  if (dalgNodes.at(np->unodes[k]->indx) != '0'){
                     cout << "BI INCONSIST OR current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                     inconsistDetected = true;
                     break;
                  }
               }
            }
            
         }
         else{
            //OR---------------------------------------------------
            finalValue = 'X';
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) != 'X'){
                  if (finalValue == 'X'){
                     finalValue = dalgNodes.at(np->unodes[k]->indx);
                  }
                  else{
                     nextValue = dalgNodes.at(np->unodes[k]->indx);
                     finalValue = finalValue | nextValue;
                  }
               }
               else{
                  Xcounter ++;
                  kValue = k;
               }
            }
            if (Xcounter == 0){
               if (currentValue != finalValue){
                  cout << "BI INCONSIST OR NO-X current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                  inconsistDetected = true;
               }
            }
            else if (Xcounter == 1){
               if (currentValue == 1 && (finalValue == 0 || finalValue == 'D' || finalValue == 'E'|| finalValue == 'X')){
                  assignOccur = true;
                  dalgNodes.at(np->unodes[kValue]->indx) = '1';
               }
               else if (currentValue == 1 && (finalValue == 1 )){
                  // j_frontier
                  //JFrontierAdd = true;
               }
               
            }
            else{
               if (finalValue == '0' || finalValue == 'X'){
                  JFrontierAdd = true;
               }
                  //j_frontier
                  //JFrontierAdd = true;
               
            }
         }

         break;

      case 4:  //NOR---------------------------------------------------
         if (currentValue == '1'){
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) == 'X'){
                  dalgNodes.at(np->unodes[k]->indx) = '0'; 
                  assignOccur = true;
               }
               else{
                  if (dalgNodes.at(np->unodes[k]->indx) != '0'){
                     cout << "BI INCONSIST NOR current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                     inconsistDetected = true;
                     break;
                  }
               }
            }
            
         }
         else{
            //NOR---------------------------------------------------
            finalValue = 'X';
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) != 'X'){
                  if (finalValue == 'X'){
                     finalValue = dalgNodes.at(np->unodes[k]->indx);
                  }
                  else{
                     nextValue = dalgNodes.at(np->unodes[k]->indx);
                     finalValue = finalValue | nextValue;
                  }
               }
               else{
                  Xcounter ++;
                  kValue = k;
               }
            }
            finalValue = !finalValue;
            if (Xcounter == 0){
               if (currentValue!=finalValue){
                  cout << "BI INCONSIST NOR NO-X current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                  inconsistDetected = true;
               }
            }
            else if (Xcounter == 1){
               if (currentValue == 0 && (finalValue == 0 || finalValue == 'D' || finalValue == 'E'|| finalValue == 'X')){
                  assignOccur = true;
                  dalgNodes.at(np->unodes[kValue]->indx) = '1';
               }
               else if (currentValue == 0 &&  (finalValue == 1 )){
                  // j_frontier
                  //JFrontierAdd = true;
               }
               
            }
            else{
                  if (finalValue == '1' || finalValue == 'X'){
                     JFrontierAdd = true;
                  }
                  //j_frontier
                  //JFrontierAdd = true;
               
            }
         }
         


         break;
      case 5: //NOT
         currentValue = !currentValue;
         if (dalgNodes.at(np->unodes[0]->indx) == 'X'){
            dalgNodes.at(np->unodes[0]->indx) = currentValue;
            assignOccur = true;
         }
         else{
            if (dalgNodes.at(np->unodes[0]->indx) != currentValue){
               cout << "BI INCONSIST NOT current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
               inconsistDetected = true;
            }
         }
         break;

      case 6: //NAND---------------------------------------------------
         if (currentValue == '0'){
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) == 'X'){
                  dalgNodes.at(np->unodes[k]->indx) = '1'; 
                  assignOccur = true;
               }
               else{
                  if (dalgNodes.at(np->unodes[k]->indx) != '1'){
                     cout << "BI INCONSIST NAND current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                     inconsistDetected = true;
                     break;
                  }
               }
            }
         }
         else{ 
            //NAND---------------------------------------------------
            finalValue = 'X';
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) != 'X'){
                  if (finalValue == 'X'){
                     finalValue = dalgNodes.at(np->unodes[k]->indx);
                  }
                  else{
                     nextValue = dalgNodes.at(np->unodes[k]->indx);
                     finalValue = finalValue & nextValue;
                  }
               }
               else{
                  Xcounter ++;
                  kValue = k;
               }
            }
            finalValue = !finalValue;
            //cout << "Xcounter = " << Xcounter << endl;
            if (Xcounter == 0){
               if (currentValue != finalValue){
                  cout << "BI INCONSIST NAND NO-X current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                  inconsistDetected = true;
               }
            
            }
            else if (Xcounter == 1){
               if (currentValue == '1' && (finalValue == '0'|| finalValue == 'D' || finalValue == 'E'|| finalValue == 'X')){
                  assignOccur = true;
                  dalgNodes.at(np->unodes[kValue]->indx) = '0';
               }
               else if (currentValue == '1' &&  (finalValue == '1')){
                  // j_frontier
                  //JFrontierAdd = true;
               }
            }
            else{
                  if (finalValue == '0' || finalValue == 'X'){
                     JFrontierAdd = true;
                  }
                  //j_frontier
                  //JFrontierAdd = true;
            }

         }
         
         break;
      case 7: //AND---------------------------------------------------
         if (currentValue == '1'){
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) == 'X'){
                  dalgNodes.at(np->unodes[k]->indx) = '1'; 
                  assignOccur = true;
               }
               else{
                  if (dalgNodes.at(np->unodes[k]->indx) != '1'){
                     cout << "BI INCONSIST AND current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                     inconsistDetected = true;
                     break;
                  }
               }
            }
            
         }
         else{
            //AND---------------------------------------------------
            finalValue = 'X';
            for(k = 0; k < np->fin; k++){
               if (dalgNodes.at(np->unodes[k]->indx) != 'X'){
                  if (finalValue == 'X'){
                     finalValue = dalgNodes.at(np->unodes[k]->indx);
                  }
                  else{
                     nextValue = dalgNodes.at(np->unodes[k]->indx);
                     finalValue = finalValue & nextValue;
                  }
               }
               else{
                  Xcounter ++;
                  kValue = k;
               }
            }
      
            if (Xcounter == 0){
               if (currentValue != finalValue){
                  cout << "BI INCONSIST AND NO-X current num: "<< np->num << " finalValue: " << finalValue << " currentValue: " << currentValue<< endl;
                  inconsistDetected = true;
               }
            
            }
            else if (Xcounter == 1){
               if (currentValue == '0' && (finalValue == '1' || finalValue == 'D' || finalValue == 'E' || finalValue == 'X')){
                  assignOccur = true;
                  dalgNodes.at(np->unodes[kValue]->indx) = '0';
               }
               else if (currentValue == '0' && finalValue == '0'){
                  // j_frontier
                  //JFrontierAdd = true;
               }
               
               
            }
            else{
               if (finalValue == '1' || finalValue == 'X'){
                  JFrontierAdd = true;
               }
                  //j_frontier
                  //JFrontierAdd = true;
               
            }
         }
         break;

         default: break;
   }

   //cout << "BACKWARD num = " <<  np->num << " assignOccur = " << assignOccur << "\n";
   if (inconsistDetected){
   //cout << "BACKWARD FAILED\n";
      return false;
   }
   assignOccurB = assignOccur;
   if (assignOccur){
      vector<int>::iterator it;
      it = find(JFrontier.begin(), JFrontier.end(), np->indx);
      if (it != JFrontier.end()){
         JFrontier.erase(it);
      }
   }

   
   if (JFrontierAdd && find(JFrontier.begin(), JFrontier.end(), np->indx) == JFrontier.end() && currentValue != 'D'&& currentValue != 'E' && currentValue != 'X'){
      cout << "JFrontier added num: " << np->num << "indx: " << np->indx<< endl;
      cout << "\tself: "<<  dalgNodes.at(np->indx) << endl;
      cout << "\t fin: " << Node[np->indx].fin << endl;
      for (int i = 0; i < Node[np->indx].fin; i ++){
         cout << " input" << i << ": " << dalgNodes.at(np->unodes[i]->indx);
      }
      cout<< endl;
      
      JFrontier.push_back(np->indx);
   }
   

   return true;
   
}
bool check(vector<threeValue> & allNodes , vector<int> & JFrontier, vector<int> & DFrontier){

}

bool dalg_imply_and_check(vector<threeValue> & allNodes, vector<int> & DFrontier, vector<int> & JFrontier, bool *errorAtOutput){
   NSTRUC *np;
   // forward implication

   //allNodes.at(14) = '1';
   //print_all_node(allNodes, DFrontier , JFrontier);
   bool assignedOccurF = true;
   bool assignedOccurB = true;
   while (assignedOccurF || assignedOccurB){
      assignedOccurF = false;
      assignedOccurB = false;
      //cout <<maxLevel << endl;
      cout << "forward START\n";
      bool singleAssigOccF = false;
      bool singleAssigOccB = false;
      for (int i = 1; i <= maxLevel; i ++){
         for (int j = 0; j < Nnodes; j ++){
            if (Node[j].level == i){
               if (!single_node_logicsim(allNodes, DFrontier, singleAssigOccF, dalgNodeNumber, dalgFaultValue.value, -1, j, 2)){
                  return false;
               }
               if (singleAssigOccF){
                  assignedOccurF = true;
               }
            }
         }
      }
   cout << "forward DONE\n";
   print_all_node(allNodes, DFrontier , JFrontier);

      // backward implication
      for (int i = maxLevel; i >= 1; i --){
         for (int j = 0; j < Nnodes; j ++){
            if (Node[j].level == i && allNodes.at(j) != 'X'){
               if (!backward_implication (allNodes, JFrontier, j, singleAssigOccB)){
                  return false;
               }
               if (singleAssigOccB){
                  assignedOccurB = true;
               }
            }
         }
      }
      cout << "backward DONE: F_status: " <<assignedOccurF << "  B_status: "<< assignedOccurB << "\n";
      
      
   }
   for (int p = 0; p < Nnodes; p ++){
      bool error = false;
      bool inputXs = false;
      // update D_frontier
      
         
      for (int o = 0; o < Node[p].fin; o++){
         if (allNodes.at(Node[p].unodes[o]->indx) == 'D' || allNodes.at(Node[p].unodes[o]->indx) == 'E'){
            error = true;
         }
         else if (allNodes.at(Node[p].unodes[o]->indx) == 'X'){
            inputXs = true;
         }
      }
      
      vector<int>::iterator it0;
      it0 = find(JFrontier.begin(), JFrontier.end(), Node[p].indx);
      // found remove
      if (it0 != JFrontier.end() && !inputXs){
         JFrontier.erase(it0);
      }
      // else if(it0 == JFrontier.end() && (allNodes.at(Node[p].indx) == '1' || allNodes.at(Node[p].indx) == '0') && inputXs && !error ){
      //    JFrontier.push_back(Node[p].indx);
      // }

   }
   
   print_all_node(allNodes, DFrontier , JFrontier);

   // check

   return true;
}





// typedef struct  n_struc {
//    unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
//    unsigned num;              /* line number(May be different from indx */
//    enum e_gtype type;         /* gate type */
//    unsigned fin;              /* number of fanins */
//    unsigned fout;             /* number of fanouts */
//    struct n_struc **unodes;   /* pointer to array of up nodes */
//    struct n_struc **dnodes;   /* pointer to array of down nodes */
//    int level;                 /* level of the gate output */
//    threeValue val;
//    //vector<string> fault;
// } NSTRUC;
bool dalg_wrapper(vector<threeValue>  &allNodes, vector<int> DFrontier, vector<int> JFrontier){
   cout << "dalg_wrapper() START -----------------------\n";
   vector<threeValue> newAllNodes;
   bool errorAtOutput = false;
   if (!dalg_imply_and_check(allNodes, DFrontier, JFrontier, &errorAtOutput)){
      //print_all_nodes(allNodes, DFrontier , JFrontier);
      cout << "dalg_imply_and_check() FALSE\n";
      return false;
   }
   newAllNodes = allNodes;
   //print_all_nodes(allNodes, DFrontier , JFrontier);
   //print_all_nodes(newAllNodes, DFrontier , JFrontier);
   // /* error NOT at PO---------------------------------------------------------------*/
   // if (checkErrorAtPO(newAllNodes)){
   //    //print_all_nodes(allNodes, DFrontier , JFrontier);
   //    throw std::invalid_argument("Error at PO");

   // }
   int POnum;
   if (!checkErrorAtPO(newAllNodes, POnum)){
      cout << "***Error not at PO\n";
      if (DFrontier.size() == 0){
         cout << "DFrontier is empty + Error not at PO\n";

         return false;
      }
      else {
         // loop all D_frontier

     
         for (int h = 0;h < DFrontier.size(); h++){

            //print_all_node(newAllNodes, DFrontier, JFrontier);
            //cout<< "s: " << s << endl;
            int selectedGateIndx = DFrontier.at(h);
            cout<< "D_frontier selected: " << selectedGateIndx << endl;
            //DFrontier.erase(DFrontier.begin());
        
            threeValue c;
            c = find_control_value(selectedGateIndx);
            if (c == '3'){
               cout << "c = 3\n" << endl;
               throw std::invalid_argument("Invalid controlling value");
               return false;
            }
            //cout << "\"c\" = " << c << endl;
            // XOR no controlling value
            if (Node[selectedGateIndx].type == 2){
               for (int p = 0; p < 2; p ++){
                  if (p == 1){
                     c = ~c;
                  }
                  vector<threeValue> oldValues = newAllNodes;
                  for (int i = 0; i < Node[selectedGateIndx].fin; i++){
                     int inputNodeIndx = Node[selectedGateIndx].unodes[i]->indx;
                     if (newAllNodes.at(inputNodeIndx).value != 'D' && newAllNodes.at(inputNodeIndx).value != 'E'){
                        //cout << "inputNodeIndx: " << inputNodeIndx << endl;
                        
                        newAllNodes.at(inputNodeIndx) = ~c;
                    
                     }
                  }
                  vector<int> newDFrontier;
                  if (dalg_wrapper(newAllNodes, newDFrontier, JFrontier)){
                     allNodes = newAllNodes;
                     return true;
                  }
                  newAllNodes = oldValues;
               }
            } 
            // regular operation
            else{
               vector<threeValue> oldValues = newAllNodes;
               for (int i = 0; i < Node[selectedGateIndx].fin; i++){
                  int inputNodeIndx = Node[selectedGateIndx].unodes[i]->indx;
                  if (newAllNodes.at(inputNodeIndx).value != 'D' && newAllNodes.at(inputNodeIndx).value != 'E'){
                     //cout << "inputNodeIndx: " << inputNodeIndx << endl;
                    
                     newAllNodes.at(inputNodeIndx) = ~c;
                  }
               }
               vector<int> newDFrontier;
               if (dalg_wrapper(newAllNodes, newDFrontier, JFrontier)){
                  allNodes = newAllNodes;
                  return true;
               }
               newAllNodes = oldValues;
               
            }
            //DFrontier.insert(DFrontier.begin()+s, selectedGateIndx);
            
         }
         //cout << "FAIL error at PO DFrontier out s = " << s<<"\n";
         //print_all_node(newAllNodes, DFrontier, JFrontier);

   
         return false;
         
      }
   }
   /* error at PO---------------------------------------------------------------*/
   else{
      cout << "\n\n Passed into Part 2 ============================================================== Error at: " << POnum << "\n";
      //return true;
      if (JFrontier.size() == 0){
         //print_all_nodes(newAllNodes, DFrontier , JFrontier);
         allNodes = newAllNodes;
         cout << "ALL DONE\n";
         return true;
      }else{
         
         while (JFrontier.size() != 0){
         //for (int j = JFrontier.size()-1; j >=0; j --){
            vector<threeValue> oldNodes = newAllNodes;
            // select a gate (G) from the J-frontier
            int selectedGateIndx = JFrontier.at(0);
            JFrontier.erase(JFrontier.begin());
            //JFrontier.erase(JFrontier.begin() + j);
            cout<< "===============================================================================\n";
            cout<< "J_frontier selected: " << selectedGateIndx << " type:  " << Node[selectedGateIndx].type << " num:  " << Node[selectedGateIndx].num <<" fin: " << Node[selectedGateIndx].fin << endl;
            // cout << " input0: "<<newAllNodes.at(Node[selectedGateIndx].unodes[0]->indx)<<endl;

            // c = controlling value of G
            threeValue c;
            c = find_control_value(selectedGateIndx);
            if (c == -1){
               //cout << "FALSE \"c\" = -1 in dalg_wrapper\n";
             
               cout << "c = -1\n" << endl;
               throw std::invalid_argument("AddPositiveIntegers arguments must be positive");
               
               

               return false;
            }
            //cout << " \"c\" = " << c << endl;
            // repeat
            // begin

            if (Node[selectedGateIndx].type == 2){
               for (int p = 0; p < 4; p ++){
                  if (p == 2){
                     c = ~c;
                  }
         
                  for (int i = 0; i < Node[selectedGateIndx].fin; i++){
                     int inputNodeIndx = Node[selectedGateIndx].unodes[i]->indx;
                     //cout << "Node[selectedGateIndx].unodes[i]->num: " << Node[selectedGateIndx].unodes[i]->num << endl;
                     if (newAllNodes.at(inputNodeIndx) == 'X'){
                        //cout << "inputNodeIndx: " << inputNodeIndx << endl;
                        //char oldValue = newAllNodes.at(inputNodeIndx).value;
                        newAllNodes.at(inputNodeIndx) = c;
                        vector<int> emptyDFrontier;
                        if (dalg_wrapper(newAllNodes, emptyDFrontier, JFrontier)){
                           allNodes = newAllNodes;
                           return true;
                        }
                        // assign cbar to j /* reverse decision */
                        if (p == 0 || p == 2){
                           newAllNodes.at(inputNodeIndx) = ~c;
                        }
                        
                        // if (dalg_wrapper(newAllNodes, DFrontier, JFrontier)){
                        //    return true;
                        // }
                     }
                     //newAllNodes = oldNodes;
                  }
               }
            }
            else{
               for (int i = 0; i < Node[selectedGateIndx].fin; i++){
                  int inputNodeIndx = Node[selectedGateIndx].unodes[i]->indx;
                  //cout << "Node[selectedGateIndx].unodes[i]->num: " << Node[selectedGateIndx].unodes[i]->num << endl;
                  if (newAllNodes.at(inputNodeIndx) == 'X'){
                     //cout << "inputNodeIndx: " << inputNodeIndx << endl;
                     //char oldValue = newAllNodes.at(inputNodeIndx).value;
                     newAllNodes.at(inputNodeIndx) = c;
                     vector<int> emptyDFrontier;
                     if (dalg_wrapper(newAllNodes, emptyDFrontier, JFrontier)){
                        allNodes = newAllNodes;
                        return true;
                     }
                     // assign cbar to j /* reverse decision */
                     newAllNodes.at(inputNodeIndx) = 'X';
                     // if (dalg_wrapper(newAllNodes, DFrontier, JFrontier)){
                     //    return true;
                     // }
                  }
                  //newAllNodes = oldNodes;
               }
            }
            

            //JFrontier.insert(JFrontier.begin()+j, selectedGateIndx);
         }
          cout<< "=================================================================FAIL error at PO num: "<< POnum <<"\n";
          //cout<< "J_frontier selected: " << selectedGateIndx << " type:  " << Node[selectedGateIndx].type << " num:  " << Node[selectedGateIndx].num <<" fin: " << Node[selectedGateIndx].fin << endl<<endl;
         
         //print_all_node(newAllNodes, DFrontier, JFrontier);
         
         return false;
         
         // select an input (j) of G with value x
         // assign c to j
         // if
         // D-alg() = SUCCESS then return SUCCESS
         
         // end
         // until all inputs of G are specified
         // return FAILURE
         // end

      }
   }



   return true;
}


bool single_fault_dalg (vector <threeValue> &allNodes){

   cout << "nodeNumber = " << dalgNodeNumber << endl;
   cout << "faultValue = " << dalgFaultValue << endl;


   vector<int> DFrontier, JFrontier;
   if (!dalg_initialize_nodes(allNodes, dalgNodeNumber, dalgFaultValue.value, DFrontier)){
      throw std::invalid_argument("dalg_initialize_nodes() FAIL\n");
      return false;
   }

   return dalg_wrapper(allNodes, DFrontier, JFrontier);

}


void dalg_podem_output(vector<vector <char> > patterns, vector <threeValue> allNodes, int fault, int faultV, int mode){
   string outFileName = "";
   
   //  for (int i = 0; i < MAXLINE; i++) {
   //      outFileName = outFileName + FILENAME[i];
   //  }
    
   stringstream ss;
   ss << FILENAME;
   string circuit;
   ss >> circuit;

   if (mode == 0){
      outFileName = circuit + "_DALG_" + to_string(fault) + "@" + to_string(faultV-48) + ".txt";
   }
   else if (mode == 1){
      outFileName = circuit + "_PODEM_" + to_string(fault) + "@" + to_string(faultV-48) + ".txt";
   }
   else if (mode == 2){
      outFileName = circuit + "_DALG_ATPG_patterns.txt";
   }else if (mode == 3){
      outFileName = circuit + "_PODEM_ATPG_patterns.txt";
   }
   else if (mode == 4){
      outFileName = circuit + "_ATPG_patterns.txt";
   }
   cout << "\n" << outFileName;
   
   fstream ofp;
   ofp.open(outFileName, ios::out);


   for(int i = 0; i < Npi; i++){
      if (i == 0){
         ofp << inputPorts.at(i);
      }
      else{
         ofp << "," << inputPorts.at(i);
      }
   }
   ofp << "\n";

   if (mode == 0 || mode == 1){
      for(int i = 0; i < Npi; i++){
         if (i == 0){
            ofp << allNodes.at(Pinput[i]->indx).value;
         }
         else{
            ofp << "," << allNodes.at(Pinput[i]->indx).value;
         }
      }
   }
   else{
      for(int i = 0; i < patterns.size(); i++){
         for (int j = 0; j < Npi; j ++){
            if (j == 0){
               ofp << patterns.at(i).at(j);
            }
            else{
               ofp << "," << patterns.at(i).at(j);
            }
         }
         ofp << "\n";
         
      }
   }
   
}




int dalg(char *cp){
   lev_simulation();
   detect_processor();
//    lp();
//    threeValue a('X');
//    threeValue b('X');
//    threeValue c('X');
//    threeValue d('1');
//    threeValue e('0');
//    cout << (a & b & c & e) << endl;
//    cout << (a & b & c & d) << endl;
// cout << (a | b | c | d) << endl;
// cout << (a | b | c | e) << endl;
//    return 0;

   
   fstream ofp0;

   int faultV;

   sscanf(cp, "%d %d", &dalgNodeNumber, &faultV);
   dalgFaultValue = faultV;
   vector <threeValue> allNodes;

   single_fault_dalg(allNodes);
   dalg_podem_output(vector<vector <char> >(), allNodes, dalgNodeNumber, dalgFaultValue.value, 0);

   // if (nodeNumber == -1 || faultValue = -1){
   //    cout << "invalid input detected.\n";
   //    return 1;
   // }

   // cout << "nodeNumber = " << dalgNodeNumber << endl;
   // cout << "faultValue = " << dalgFaultValue << endl;

   // vector <threeValue> allNodes;
   // vector<int> DFrontier, JFrontier;
   // if (!dalg_initialize_nodes(allNodes, dalgNodeNumber, dalgFaultValue, DFrontier)){
   //    return 0;
   // }

  
   // dalg_wrapper(allNodes, DFrontier, JFrontier);
   

   // cout <<"allNodes.size() = " << allNodes.size() << endl;
   // cout << "allNodes.at().num = " << Node[7].num << endl;
   // cout << "allNodes.at().value = " << allNodes.at(7) << endl;

   return 0;
}

//============================================================================================================================================================
// PODEM

class objectRetVal{
   public:
      int faultN;
      int faultV;
      objectRetVal(){
         this->faultN = 0;
         this->faultV = -1;
      }
      objectRetVal(int a, int b){
         this->faultN = a;
         this->faultV = b;
      }

};





objectRetVal objective (vector<threeValue>  allNodes, vector<int> DFrontier, int faultIndx, int faultValue) {
   // fault has not been activated
   if (allNodes.at(faultIndx) == 'X'){
      //cout << "objective X on Node: " << Node[faultIndx].num << " val: " << faultValue << endl;
      objectRetVal rv (faultIndx, faultValue);
      return rv;
   }
   //propagate fault effect
   else{
      //cout << "objective propagate fault on Node: " << Node[faultIndx].num << " val: " << faultValue << endl;
      int newfaultIndx;
      int newfaultValue;
      
      //for (int i = 0; i < DFrontier.size(); i ++){
         int selectedGateIndx = DFrontier.at(0);

         threeValue c = find_control_value(selectedGateIndx);
         newfaultValue = (~c).toInt();
         for (int j = 0; j < Node[selectedGateIndx].fin; j++){
            int inputNodeIndx = Node[selectedGateIndx].unodes[j]->indx;
            if (allNodes.at(inputNodeIndx) == 'X' && inputNodeIndx == numToIndx[podemNodeNumber]){
               //cout<< "object ERROR Node\n";
               newfaultValue = podemFaultValue.toInt();
               break;
            }
            else if (allNodes.at(inputNodeIndx) == 'X'){
              // cout<< "object Node select: " << Node[inputNodeIndx].num<<"\n";
               newfaultIndx = inputNodeIndx;
               break;
            }

            
         }
         
      //}
      //newfaultIndx = selectedGateIndx;
      objectRetVal rv (newfaultIndx, newfaultValue);
      return rv;
   }

}

bool checkSettingAllInputs (int indx, int val){
   switch (Node[indx].type){
      case 0: 
         throw std::invalid_argument("PI in checkSettingAllInputs");
      case 1: 
         throw std::invalid_argument("BRANCH in checkSettingAllInputs");
      case 2: //XOR
         return true;
      break;
      case 3: // OR
         if (val == 0){
            return true;
         }else{
            return false;
         }
      case 4: //NOR
         if (val == 1){
            return true;
         }else{
            return false;
         }
      break;
      case 5: //NOT
         throw std::invalid_argument("NOT in checkSettingAllInputs");
      
      case 6: //NAND
         if (val == 0){
            return true;
         }else{
            return false;
         }
      break;
      case 7:  //AND
         if (val == 1){
            return true;
         }else{
            return false;
         }
      break;
   
   }
   return false;
}

objectRetVal backtrace (vector<threeValue>  allNodes, int faultIndx, int faultValue) {
   threeValue v(faultValue);
 
   int n;
   if (Node[faultIndx].type == 0) {
      objectRetVal rv(faultIndx,faultValue);
      return rv;
   }
   
   if (Node[faultIndx].type == 4 || Node[faultIndx].type == 5 ||Node[faultIndx].type == 6){
      // cout << "inversed Node[faultIndx] = " << Node[faultIndx].num << " type = " << Node[faultIndx].type <<"\n";
      // cout << "v = " << v << endl;
      v = ~v;
      //cout << " v = " << v << endl;
   }

   //if (checkSettingAllInputs(faultIndx, faultValue)){
      for (int i = 0; i < Node[faultIndx].fin; i ++){
         int inputNodeIndx = Node[faultIndx].unodes[i]->indx;
         if (allNodes.at(inputNodeIndx) == 'X'){
            n = inputNodeIndx;
            break;
         }
      }
   //}
   objectRetVal newV;
   newV = backtrace (allNodes, n, v.toInt());
   n = newV.faultN;
   v = newV.faultV;

   
   objectRetVal rv(n,v.toInt());
   return rv;
}

int podemActualFaultNum;
int podemActualFaultValue;


bool PODEM_wrapper (vector<threeValue> & allNodes, vector<int> &DFrontier, int faultIndx, int faultValue) {
   //   cout << "==========================================================================\n";
   //   cout<< "PODEM_wrapper START: num= " << Node[faultIndx].num << "val = " <<faultValue << endl;
   int numERR;
   if (checkErrorAtPO(allNodes, numERR)){
      //cout << "ERROR at PO++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      return true;
   }
   if (DFrontier.size() == 0){
      //cout << "DFrontier size 0 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      return false;
   }
   objectRetVal objRV, bkRV;
   objRV = objective(allNodes, DFrontier, faultIndx, faultValue);
    //cout << "objective() returned: num = " << Node[objRV.faultN].num << " val = " << objRV.faultV<< endl;
   bkRV = backtrace(allNodes, objRV.faultN, objRV.faultV);
   threeValue bkV = bkRV.faultV;
    //cout << "backtrace() returned: num = " << Node[bkRV.faultN].num << " val = " << bkRV.faultV << endl;
    //cout << "FIRST forward START PI = "  << Node[bkRV.faultN].num<< " val = " << bkRV.faultV << "=======================\n";
   bool singleAssigOccF = false;
   if (Node[bkRV.faultN].num == podemNodeNumber){
      if (bkRV.faultV == 0){
         allNodes.at(bkRV.faultN) = 'E';
      }
      else{
         allNodes.at(bkRV.faultN) = 'D';
      }
   }
   else{
      if (bkRV.faultV == -1){
         throw invalid_argument("ERROR");
      }
      allNodes.at(bkRV.faultN) = bkRV.faultV;
   }
   
   for (int i = 1; i <= maxLevel; i ++){
      for (int j = 0; j < Nnodes; j ++){
         if (Node[j].level == i){
            single_node_logicsim(allNodes, DFrontier, singleAssigOccF, podemNodeNumber, podemFaultValue.toInt(), -1, j, 3);
         }
      }
   }
   //print_all_nodes(allNodes, DFrontier,  vector<int>());
   //cout << "FIRST forward DONE PI = " << Node[bkRV.faultN].num<< " val =" << bkRV.faultV << "=======================\n";
   return true;
   if (PODEM_wrapper(allNodes, DFrontier, faultIndx, faultValue)){
      //print_all_nodes(allNodes, DFrontier,  vector<int>());
      return true;
   }
   
   //cout << "SECOND forward START PI = "  << Node[bkRV.faultN].num<< " val =" << ~bkV << "=======================\n";

   int newRV;
   if (bkRV.faultV == 0){
      newRV = 1;
   }
   else{
      newRV = 0;
   }
   if (Node[bkRV.faultN].num== podemNodeNumber){
      
      if (newRV == 0){
         allNodes.at(bkRV.faultN) = 'E';
      }
      else{
         allNodes.at(bkRV.faultN) = 'D';
      }
   }
   else{
      allNodes.at(bkRV.faultN) = newRV;
   }
   
   for (int i = 1; i <= maxLevel; i ++){
      for (int j = 0; j < Nnodes; j ++){
         if (Node[j].level == i){
            single_node_logicsim(allNodes, DFrontier, singleAssigOccF, podemNodeNumber, podemFaultValue.toInt(), -1, j, 3);
         }
      }
   }
   //cout << "SECOND forward DONE PI = "  << Node[bkRV.faultN].num<< " val =" << ~bkV << "=======================\n";

   if (PODEM_wrapper(allNodes, DFrontier, faultIndx, faultValue)){
      //print_all_nodes(allNodes, DFrontier,  vector<int>());
      return true;
   }

   //cout << "THRID forward START PI = "  << Node[bkRV.faultN].num<< " val =" << 'X' << "=======================\n";
   
   allNodes.at(bkRV.faultN) = 'X';
   for (int i = 1; i <= maxLevel; i ++){
      for (int j = 0; j < Nnodes; j ++){
         if (Node[j].level == i){
            single_node_logicsim(allNodes, DFrontier, singleAssigOccF, podemNodeNumber, podemFaultValue.toInt(), -1, j, 3);
         }
      }
   }
   //cout << "THRID forward DONE PI = "  << Node[bkRV.faultN].num<< " val =" << 'X' << "=======================\n";


   return false;
}


int podem_initialize_nodes(vector <threeValue> &allNodes, int nodeNumber, int faultValue, vector<int> & DFrontier){
   int indx = -1;
   allNodes.clear();
   indx = numToIndx[nodeNumber];
   if (indx == -1){
      //cout << "ERROR dlag_initialize_nodes Fails: indx == -1\n";
      return 0;
   }
   for (int i = 0; i < Nnodes; i ++){
      threeValue newNode('X');
      if (i == indx){
         //cout << "fault node indx = " << i << endl;
         if (Node[indx].fout > 0){
            NSTRUC* np;
            np = &Node[indx];
            np = np->dnodes[0];
            while (np->type == 1){
               np = np->dnodes[0];
            }
            DFrontier.push_back(np->indx);
         }
      }
      allNodes.push_back(newNode);
   }
   //print_all_nodes(allNodes, DFrontier, vector<int> ());
   //cout << "podem_initialize_nodes DONE\n";
   return 1;
}

int single_fault_podem(vector<threeValue> & allNodes){
   vector<int> DFrontier, JFrontier;
   podem_initialize_nodes(allNodes, podemNodeNumber, podemFaultValue.toInt(), DFrontier);
   //print_all_nodes(allNodes, DFrontier,  vector<int>());
   podemFaultValue = ~podemFaultValue;
   if (PODEM_wrapper(allNodes, DFrontier, numToIndx[podemNodeNumber], (podemFaultValue.toInt()))){
      //print_all_nodes(allNodes, DFrontier,  vector<int>());
   }
   return 1;
}

int podem(char *cp){
   lev_simulation();
   detect_processor();

   
   fstream ofp0;

   int num;
   int val;
   sscanf(cp, "%d %d", &num , &val);
   // if (nodeNumber == -1 || faultValue = -1){
   //    cout << "invalid input detected.\n";
   //    return 1;
   // }
   podemNodeNumber = num;
   podemFaultValue = val;
   cout << "nodeNumber = " << podemNodeNumber << endl;
   cout << "faultValue = " << podemFaultValue << endl;

   vector <threeValue> allNodes;
   single_fault_podem(allNodes);

   // vector<int> DFrontier, JFrontier;
   // podem_initialize_nodes(allNodes, podemNodeNumber, podemFaultValue.toInt(), DFrontier);
   // print_all_nodes(allNodes, DFrontier,  vector<int>());
   // podemFaultValue = ~podemFaultValue;
   // if (PODEM_wrapper(allNodes, DFrontier, numToIndx[podemNodeNumber], (podemFaultValue.toInt()))){
   //    print_all_nodes(allNodes, DFrontier,  vector<int>());
   // }
   dalg_podem_output(vector<vector <char> >(), allNodes, podemNodeNumber, (~podemFaultValue).toInt(), 1);
   

   // cout <<"allNodes.size() = " << allNodes.size() << endl;
   // cout << "allNodes.at().num = " << Node[7].num << endl;
   // cout << "allNodes.at().value = " << allNodes.at(7) << endl;

   return 0;
}



double atpg_det_subroutine(vector<string> oneFaultList, vector<string>& detectedFault, vector<vector <char> > & allTestVectors, int mode){
   
   for(string fault : oneFaultList){
      //cout << "fault: " << fault <<endl;
      vector <threeValue> allNodes;
      unsigned faultNum;
      char faultValue;

      size_t pos = fault.find('@');
      if (pos == string::npos){         // if no dot
         cout << "no @ in fault\n";
         throw std::invalid_argument("ERROR");
      }
      else{
         if (mode == 0){
            dalgNodeNumber = stoi(fault.substr(0, pos));
            dalgFaultValue = fault.substr(pos, string::npos) [1];
         }
         else{
            podemNodeNumber = stoi(fault.substr(0, pos));
            podemFaultValue = fault.substr(pos, string::npos) [1];
         }
      }
      bool status = false;
      if (mode == 0){
         status = single_fault_dalg(allNodes);
      }
      else{
         //cout << "fault " << fault << endl;
         
         status = single_fault_podem(allNodes);
      }
      // if (fault.compare("6@1") == 0){
      //    print_all_nodes(allNodes , vector<int>(), vector<int>());
      // }
      if (status){
         vector<char> testVector;
         // cout << "Pinput : ";
         // for (int j = 0; j < Npi; j ++){
         //    //cout  << Pinput[j]->indx << " ";
         //    cout << to_string(allNodes.at(Pinput[j]->indx).getValue()) + " ";
            
         // }
         // cout << endl;
         for (int j = 0; j < Npi; j ++){
            char value = allNodes.at(Pinput[j]->indx).getValue();
            if (value == 'X'){
               int val = rand() % 2;
               if (val == 1){
                  value = '1';
               }
               else{
                  value = '0';
               }
               
            }
            else if (value == 'D'){
               value = '1';
            }
            else if (value == 'E'){
               value = '0';
            }
            testVector.push_back(value);
            // if (value != '0' && value != '1'){
            //    cout << "ERROR value fault:" << fault <<"\n";
            // }
            
            
         }
         allTestVectors.push_back(testVector);
         // cout << "+: ";
         // for (int p = 0; p < testVector.size(); p++){
         //    cout << testVector.at(p) << " ";
         // }
         // cout << endl;

         //cout << "allTestVectors pushed back vector size = " << testVector.size() << endl;

      }
      else{
         cout << "FAILED: " << fault << endl;
      }
      
   }
   //cout << "TEST vector simulated allTestVectors.size() = " << allTestVectors.size() << "\n";

   map<int, vector<char> >allTestVectorsMap;

   for (int i = 0; i < allTestVectors.size(); i ++){
      for (int j = 0; j < Npi; j ++){
         allTestVectorsMap[inputPorts.at(j)].push_back(allTestVectors.at(i).at(j));
      }
   }
   // for (int j = 0; j < Npi; j ++){
   //       cout << "size() = " << allTestVectorsMap[inputPorts.at(j)].size() << "\n";
   // }
   //    map<int, vector<char> >::iterator itr;
   //  for(itr=allTestVectorsMap.begin();itr!=allTestVectorsMap.end();itr++)
   //  {
   //      cout<<itr->first<<" vector: ";
   //      for (int i = 0; i < itr->second.size(); i ++){
   //       cout<<itr->second.at(i)<<" ";
   //      }
   //      cout<<endl;
   //  }


   parallel_fault_simulation_wrapper (allTestVectorsMap, oneFaultList, detectedFault, allTestVectors.size());
   cout << "PFS DONE\n";
   
   
   

   return 1;
}

int atpg_det(char * cp){
   clock_t tStart = clock();
   stringstream ss;
   ss << cp;

   string cicruitName;
   string method;
   ss >> cicruitName;
   ss >> method;
   cout << cicruitName << endl;
   cout << method << endl;;

   std::transform(method.begin(), method.end(), method.begin(),
    [](unsigned char c){ return std::toupper(c); });

   fstream ifp;
   string inputFileName = cicruitName + ".ckt";
   ifp.open(inputFileName, ios::in);

   int n = inputFileName.length();
 
    // declaring character array
   char char_array[n + 1];
   strcpy(char_array, inputFileName.c_str());

   cread(char_array);
   lev_simulation();
   detect_processor();

   //lp();

   string reportOutFile;
   double FC;

   fstream ofp1;

   vector<string> detectedFault;
   vector<vector<char> > allTestVectors;

   for (int j = 0; j < allFaultList.size(); j++){

      string newFault = "PlaceHolder";
      detectedFault.push_back(newFault);
   }

   // D-algorithm===========================================
   if (method.compare("DALG") == 0){
      atpg_det_subroutine(allFaultList, detectedFault, allTestVectors, 0);
      reportOutFile = cicruitName + "_DALG_ATPG_report.txt";
       dalg_podem_output(allTestVectors, vector<threeValue>(), 0, 0, 0+2);
   }
   // PODEM=================================================
   else if (method.compare("PODEM") == 0){
      atpg_det_subroutine(allFaultList,detectedFault, allTestVectors,  1);
      reportOutFile = cicruitName + "_PODEM_ATPG_report.txt";
      dalg_podem_output(allTestVectors, vector<threeValue>(), 0, 0, 1+2);

   }
   // ERROR=================================================
   else{
      throw std::invalid_argument("\ninvalid Method\n");
   }

   int detectedCount = 0;
   for (int j = 0; j < detectedFault.size(); j ++){
      //cout << "-: " << detectedFault.at(j) << endl;
      if (detectedFault.at(j).compare("PlaceHolder") != 0){
         detectedCount += 1;
      }
      
   }
   FC = (double) detectedCount / allFaultList.size()*100.0;

   cout << "FC: " << FC << endl;

   printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  

   ofp1.open(reportOutFile, ios::out);
   
   ofp1 << "Algorithm: " << method << "\n" << "Circuit: " << cicruitName << "\n" << std::fixed << std::setprecision(2)<< "Fault Coverage: "<< FC << "%";
   ofp1 << "\nTime: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << " seconds" <<endl;


}


int atpg (char * cp){
   clock_t tStart = clock();
   stringstream ss;
   ss << cp;

   string cicruitName;
   ss >> cicruitName;
   cout << cicruitName << endl;


   fstream ifp;
   string inputFileName = cicruitName + ".ckt";
   ifp.open(inputFileName, ios::in);

   int n = inputFileName.length();
 
    // declaring character array
   char char_array[n + 1];
   strcpy(char_array, inputFileName.c_str());

   cread(char_array);
   lev_simulation();
   detect_processor();


   vector<string> detectedFault;
   for (int j = 0; j < Nnodes*2; j++){
      string newFault = "PlaceHolder";
      detectedFault.push_back(newFault);
   }

   vector<vector<char> > allTestVectors;
   int testNum = 10;
   if (Nnodes > 100){
      testNum = 30;
   }
   rtg_subroutine (" ", " ", testNum, testNum, detectedFault, allTestVectors, 1);

   vector<string> restFaultList;
   int detectedCount = 0;
   for (int j = 0; j < detectedFault.size(); j ++){
      //cout << "-: " << detectedFault.at(j) << endl;
      if (detectedFault.at(j).compare("PlaceHolder") != 0){
         detectedCount += 1;
      }
      else{
         restFaultList.push_back(allFaultList.at(j));
      }
      
   }

   cout << "Faults Num = " << allFaultList.size() << " "<< "detectedCount =  "<< detectedCount << endl;
   cout << "FC = " << (double) detectedCount/allFaultList.size()*100.0 << endl;

   // cout << "restFaultList:======================== \n";
   // for (int j = 0; j < restFaultList.size(); j++){

   //    cout << restFaultList.at(j) << endl;
   // }

   cout << "detectedCount: " << detectedCount;

   vector<string> restDetectedFault;
   for (int j = 0; j < restFaultList.size(); j++){

      string newFault = "PlaceHolder";
      restDetectedFault.push_back(newFault);
   }




 //-----------------------------------------------------------------------

   fstream ofp1;
   double FC;
   vector<vector <char> > podemTestVectors;
   atpg_det_subroutine(restFaultList, restDetectedFault, podemTestVectors, 2);



   int podemDetectedCount = 0;
   vector<string> restFaultList2;
   for (int j = 0; j < restDetectedFault.size(); j++){

      if (restDetectedFault.at(j).compare("PlaceHolder") != 0){
         podemDetectedCount += 1;
      }
      else{
         restFaultList2.push_back(restFaultList.at(j));
         //cout << restFaultList.at(j) << endl;
      }
   }

   cout << "Faults Num = " << restFaultList.size() << " "<< "podemDetectedCount =  "<< podemDetectedCount << endl;
   cout << "FC = " << (double) podemDetectedCount/restFaultList.size()*100.0 << endl;

//  //-----------------------------------------------------------------------
//    vector<string> restDetectedFault2;
//    for (int j = 0; j < restFaultList2.size(); j++){

//       string newFault = "PlaceHolder";
//       restDetectedFault2.push_back(newFault);
//    }

//    vector<vector <char> > podemTestVectors2;
//    atpg_det_subroutine(restFaultList2, restDetectedFault2, podemTestVectors2, 2);


//    int podemDetectedCount1 = 0;
//    for (int j = 0; j < restDetectedFault2.size(); j++){

//       if (restDetectedFault2.at(j).compare("PlaceHolder") != 0){
//          podemDetectedCount1 += 1;
//       }
      
//    }
  
//    cout << "Faults Num = " << restFaultList2.size() << " "<< "podemDetectedCount1 =  "<< podemDetectedCount1 << endl;
//    cout << "FC = " << (double) podemDetectedCount1/restFaultList2.size()*100.0 << endl;



 //-----------------------------------------------------------------------

 double faultCount = podemDetectedCount + detectedCount;

   double actualFC = faultCount/allFaultList.size() *100.0;

   cout << "current FC = " << FC << endl;
   cout << std::fixed << setprecision(2) <<"faultCount: "<< faultCount << endl;
   cout << "detectedCount: " << detectedCount + faultCount << endl;
   cout << "actual FC = " << actualFC << endl;

   string reportOutFile = cicruitName + "_ATPG_report.txt";

   ofp1.open(reportOutFile, ios::out);
   
   ofp1 << "Algorithm: ATPG\n" << "Circuit: " << cicruitName << "\n" << std::fixed << std::setprecision(2)<< "Fault Coverage: "<< actualFC << "%";
   ofp1 << "\nTime: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << " seconds" <<endl;

//-----------------------------------------------------------------------

   for (int i = 0; i < podemTestVectors.size(); i ++){
      vector <char> newVal;
      newVal = podemTestVectors.at(i);
      allTestVectors.push_back(newVal);
   }

   dalg_podem_output(allTestVectors, vector <threeValue>(), 0, 0, 4);


}