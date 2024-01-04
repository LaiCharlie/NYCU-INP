#include <math.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/un.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int N = 30;
int board[50][50];
char buf[32768];

void printBoard(){
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++){
            if(board[i][j])
                cout << 'Q' << " ";
            else
                cout << '.' << " ";
        }
        cout << '\n';
    }
}

bool isValid(int row, int col) {
    for (int i = 0; i < N; i++)
        if (board[row][i])
            return false;

    for (int i = 0; i < N; i++)
        if (board[i][col])
            return false;

    for (int i=row, j=col; i>=0 && j>=0; i--, j--)
        if (board[i][j])
            return false;
    for (int i=row, j=col; i<N && j<N;   i++, j++)
        if (board[i][j])
            return false;
    for (int i=row, j=col; j>=0 && i<N;  i++, j--)
        if (board[i][j])
            return false;
    for (int i=row, j=col; i>=0 && j<N;  i--, j++)
        if (board[i][j])
            return false;
    return true;
}

bool solveNQueen(int col) {
    if (col >= N)
        return true;

    for (int i = 0; i < N; i++) {
        if(board[i][col])
            return solveNQueen(col + 1);

        else if (isValid(i, col) ) {
            board[i][col] = 1;
            if(solveNQueen(col + 1))
                return true;
            board[i][col] = 0;
        }
    }
    return false;
}

bool checkSolution() {
    if(solveNQueen(0) == false) {
        cout << "Solution does not exist";
        return false;
    }

    return true;
}

static const char* socket_path = "/queen.sock";

int main(){
    memset(board, 0, sizeof(board));

	int sock = 0;
	if((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		cout << "Client: Error on socket() call\n";
		return 1;
	}

	struct sockaddr_un remote;
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, socket_path);
	int data_len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if(connect(sock, (struct sockaddr*)&remote, data_len) == -1){
		cout << "Client: Error on connect call\n";
		return 1;
	}

    string mes = "P\n";
    cout << "send mes : " << mes << "\n";
	send(sock, mes.c_str(), strlen(mes.c_str()), 0);

	memset(buf, '\0', sizeof(buf));
	if((data_len = recv(sock, buf, sizeof(buf), 0)) > 0){
        string temp = "";
        int col = 0;
		for(int i=0; i<data_len; i++){
            if(buf[i] == '\n'){
                for(int j=0, idx = 0; j<temp.length(); j+=2, idx++){
                    board[col][idx] = (temp[j] == '.') ? 0 : 1;
                }
                col++;
                temp = "";
            }
            else
                temp += buf[i];
        }
	}

    if(checkSolution() == false){
        cout << "SOLVER ERR: INCOMPLETE!\n";
    }

    vector<pair<int, int> > temp;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            if(board[i][j]){
                temp.push_back(make_pair(i, j));
            }
        }
    }

    cout << "Queen number = " << temp.size() << '\n';
    for(int j=0;j<temp.size();j++){
        string mes = "m " + to_string(temp[j].first) + " " + to_string(temp[j].second) + "\n";
        cout << "send mes : " << mes;
		send(sock, mes.c_str(), strlen(mes.c_str()), 0);

		memset(buf, '\0', sizeof(buf));
	    data_len = recv(sock, buf, sizeof(buf), 0);
        cout << buf << '\n';
    }

    mes = "P\n";
    cout << "send mes : " << mes;
	send(sock, mes.c_str(), strlen(mes.c_str()), 0);
	memset(buf, '\0', sizeof(buf));
	data_len = recv(sock, buf, sizeof(buf), 0);
    cout << buf << '\n';

    mes = "C\n";
    cout << "send mes : " << mes;
	send(sock, mes.c_str(), strlen(mes.c_str()), 0);
	memset(buf, '\0', sizeof(buf));
	data_len = recv(sock, buf, sizeof(buf), 0);
    cout << buf << '\n';

    close(sock);
	return 0;
}

// 0 : Available commands:
// 1 : - H: Show this help.
// 2 : - P: Print the current puzzle.
// 3 : - M: Mark Qeeun on a cell. Usage: M [row] [col]
// 4 : - C: Check the placement of the queens.
// 5 : - S: Serialize the current board.
// 6 : - Q: Quit.