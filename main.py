import sys
sys.path.append('./build/lib.linux-x86_64-cpython-311')

import waterdrop
import numpy as np


class WaterDrop:
    def __init__(self, board, m=10):
        self.board = board
        self.m = m  # 初始水滴
        self.k = 5
        
    def place(self, x, y):
        self.board, self.m, empty = waterdrop.drop(self.board, x, y, self.k, self.m, direction=0)
        
    def search(self):
        path, m = waterdrop.search(self.board, self.m, self.k, max_rollouts=10000)
        return path, m
        

if __name__ == "__main__":
    board = np.array([[3, 2, 3, 2, 1, 1],
                      [0, 4, 2, 2, 1, 3],
                      [3, 0, 2, 2, 0, 2],
                      [2, 3, 4, 1, 1, 1],
                      [4, 3, 3, 3, 4, 3],
                      [0, 0, 2, 0, 3, 0]])
    
    game = WaterDrop(board, m=10)
    path, m = game.search()
    print(path)
    print(m)
    