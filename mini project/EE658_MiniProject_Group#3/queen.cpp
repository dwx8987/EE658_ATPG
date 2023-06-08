#include <iostream>
#include <fstream>
//using namespace std;
int ChessBoard[10][10];

/*void PrintBoard(int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            std::cout<<ChessBoard[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
    std::cout<<std::endl;
}*/
int solution_count = 1;
std::ofstream myfile;
void PrintBoard(int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(ChessBoard[i][j] == 1){
                //std::cout<<"("<<i<<","<<j<<")"<<" ";
                myfile<<"("<<i<<","<<j<<")";
                if (i < 7) myfile << ",";
            }
        }
    }
    //std::cout<<std::endl;
    myfile<<"\n";
}

bool isSafe(int col, int row, int n){
    for(int i = 0; i < row; i++){
        if(ChessBoard[i][col]) return false;
    }
    for(int i = row, j = col; i>=0 && j>=0; i--, j--){
        if(ChessBoard[i][j]) return false;
    }
    for(int i = row, j = col; i>=0 && j<n; i--, j++){
        if(ChessBoard[i][j]) return false;
    }
    return true;
}

bool Solution(int n, int row){
    if(n == row){
        //std::cout<<"Solution "<<solution_count<<": ";
        myfile<<"Solution "<<solution_count<<": ";
        PrintBoard(n);
        solution_count++;
        return true;
    }

    bool result = false;
    for(int i = 0; i < n; i++){
        if(isSafe(i,row,n)){
            ChessBoard[row][i] = 1;
            result = Solution(n, row+1) || result;
            ChessBoard[row][i] = 0;
        }
    }
    return result;
}

/*int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int n;
    std::cout<<"input"<<std::endl;
    std::cin>>n;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            ChessBoard[i][j] = 0;
        }
    }
    bool result = Solution(n, 0);
    if(result == false) std::cout<<"the solution does not exist"<<std::endl;
    else std::cout<<std::endl;
    return 0;
}*/
int main(){
    myfile.open("queen.txt");
    myfile << "Total solution:    ";
    std::ios_base::sync_with_stdio(false);
    int n = 8;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            ChessBoard[i][j] = 0;
        }
    }
    bool result = Solution(n, 0);
    myfile.seekp(std::ios::beg);
    myfile << "Total solution: " << solution_count - 1<< "\n";
    myfile.close();
    if(result == false) std::cout<<"the solution does not exist"<<std::endl;
    else std::cout<<std::endl;
    return 0;
}