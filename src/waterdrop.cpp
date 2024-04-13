#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include<vector>
#include<random>
#include<cmath>
#include<limits>
#include<utility>

namespace py = pybind11;
using namespace std;

 /************************************************************************
 * 加水滴
 *   (x,y)为欲加入水滴的栅格的坐标
 *   k: 水滴破裂时所含有的水滴数量
 *   m: 坛子里的水滴数量
 *   direction为水滴运动的方向
 *      0:表示点击该栅格
 *      1:释放的水滴向左运动
 *      2:释放的水滴向右运动
 *      3:释放的水滴向上运动
 *      4:释放的水滴向下运动
 ************************************************************************/

const int EXPANSION_THRESHOLD = 10;  // 节点访问10次后才考虑扩展

 void  drop_(std::vector<std::vector<int>> &board, int x, int y, int k, int&  m, int direction, int & bonus)
 {
    if(x<0 || y<0 || x>=board.size() || y>board.size()) return;

    if (direction == 0){
        m--;  // 坛子里的水滴
        if (board[x][y]<k) board[x][y]++;
    }
    else {
        if (board[x][y]==0)        //该水滴按照原来的方向继续移动
        {
            if(direction==1) drop_(board,x-1,y,k,m,1,bonus);    //go on moving to left
            else if(direction==2) drop_(board,x+1,y,k,m,2,bonus);    //go on moving to right
            else if(direction==3) drop_(board,x,y-1,k,m,3,bonus);    //go on moving to up
            else if(direction==4) drop_(board,x,y+1,k,m,4,bonus);    //go on moving to down
        }
        else if(board[x][y]<k) board[x][y]++;
    }

    if(board[x][y]==k)
    {
        board[x][y]=0;    //水滴破裂
        bonus++;
        drop_(board,x-1,y,k,m,1,bonus);    //release a drop moving to left
        drop_(board,x+1,y,k,m,2,bonus);    //release a drop moving to right
        drop_(board,x,y-1,k,m,3,bonus);    //release a drop moving to up
        drop_(board,x,y+1,k,m,4,bonus);    //release a drop moving to down
    }
}

std::tuple<vector<vector<int>>, int, bool> drop(const vector<vector<int>>& original_board, int x, int y, int k, int m, int direction) {
    int bonus = 0;
    vector<vector<int>> board_copy = original_board;
    drop_(board_copy, x, y, k, m, direction, bonus);
    m += bonus / 3;

    for (auto & row : board_copy)
    for (auto & item : row)
    if (item > 0)return make_tuple(board_copy, m, false || m <= 0);
    return make_tuple(board_copy, m, true);
}



std::pair<int, int> select_random_position(const std::vector<std::vector<int>>& board) {
    std::vector<std::pair<int, int>> available_positions;
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j] > 0)
                available_positions.emplace_back(i, j);
        }
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, available_positions.size() - 1);
    return available_positions[distrib(gen)];
}


std::pair<std::vector<std::pair<int, int>>, int> rollout(const vector<vector<int>>& original_board, int m, int k) {
    vector<pair<int, int>> path;
    vector<vector<int>> board = original_board;
    bool game_over = false;
    while (!game_over) {
        auto [x, y] = select_random_position(board);
        auto [new_board, new_m, new_game_over] = drop(board, x, y, k, m, 0); // Assuming direction 0 for adding a droplet
        m = new_m;
        board = new_board;
        game_over = new_game_over;
        path.push_back(make_pair(x, y));
    }

    return make_pair(path, m);
}


std::pair<std::vector<std::pair<int, int>>, int> random_search(const vector<vector<int>> original_board, int m, int k, int max_rollouts){
    int max_m = -1;
    vector<pair<int, int>> best_path;
    for (int i = 0; i < max_rollouts; i++){
        auto [path, final_m] = rollout(original_board, m, k);
        if (final_m > max_m){
            max_m = final_m;
            best_path = path;
        }
    }
    return make_pair(best_path, max_m);
}

PYBIND11_MODULE(waterdrop, m) {
    m.def("drop", &drop, "A function that adds a water drop to the board",
          py::arg("original_board"), py::arg("x"), py::arg("y"), py::arg("k"), py::arg("m"), py::arg("direction"));
    m.def("search", &random_search, "A function to search for a solution path",
          py::arg("board"), py::arg("m"), py::arg("k"), py::arg("max_rollouts"));
}

