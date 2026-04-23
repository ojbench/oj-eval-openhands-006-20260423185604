#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <cstring>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Custom global variables for client
char** current_map;  // Current state of the map as seen by the client
int visited_grids;   // Count of visited grids
int marked_mines;    // Count of marked mines

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize global variables
  current_map = new char*[rows];
  for (int i = 0; i < rows; i++) {
    current_map[i] = new char[columns];
    for (int j = 0; j < columns; j++) {
      current_map[i][j] = '?';
    }
  }
  visited_grids = 0;
  marked_mines = 0;
  
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  for (int i = 0; i < rows; i++) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; j++) {
      current_map[i][j] = line[j];
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  
  // Strategy 1: Auto-explore grids where all mines are marked
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (current_map[i][j] >= '0' && current_map[i][j] <= '8') {
        int mine_count = current_map[i][j] - '0';
        int marked_count = 0;
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && current_map[ni][nj] == '@') {
            marked_count++;
          }
        }
        
        if (marked_count == mine_count) {
          Execute(i, j, 2);  // Auto-explore
          return;
        }
      }
    }
  }
  
  // Strategy 2: Mark obvious mines (if a visited grid has k unknown neighbors and its number is k)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (current_map[i][j] >= '0' && current_map[i][j] <= '8') {
        int mine_count = current_map[i][j] - '0';
        int marked_count = 0;
        int unknown_count = 0;
        std::vector<std::pair<int, int>> unknown_cells;
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (current_map[ni][nj] == '@') {
              marked_count++;
            } else if (current_map[ni][nj] == '?') {
              unknown_count++;
              unknown_cells.push_back({ni, nj});
            }
          }
        }
        
        // If remaining unknown cells equal remaining mines, mark them all
        if (unknown_count > 0 && marked_count + unknown_count == mine_count) {
          Execute(unknown_cells[0].first, unknown_cells[0].second, 1);  // Mark mine
          return;
        }
      }
    }
  }
  
  // Strategy 3: Visit safe cells (if all mines around a cell are already marked)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (current_map[i][j] >= '0' && current_map[i][j] <= '8') {
        int mine_count = current_map[i][j] - '0';
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_cells;
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (current_map[ni][nj] == '@') {
              marked_count++;
            } else if (current_map[ni][nj] == '?') {
              unknown_cells.push_back({ni, nj});
            }
          }
        }
        
        // If all mines are marked, visit unknown cells
        if (marked_count == mine_count && !unknown_cells.empty()) {
          Execute(unknown_cells[0].first, unknown_cells[0].second, 0);  // Visit
          return;
        }
      }
    }
  }
  
  // Strategy 4: Advanced reasoning - find cells that must be safe
  // Look for patterns where we can deduce safe cells
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (current_map[i][j] >= '0' && current_map[i][j] <= '8') {
        int mine_count = current_map[i][j] - '0';
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_cells;
        
        for (int k = 0; k < 8; k++) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (current_map[ni][nj] == '@') {
              marked_count++;
            } else if (current_map[ni][nj] == '?') {
              unknown_cells.push_back({ni, nj});
            }
          }
        }
        
        // Check each unknown cell to see if it's safe
        for (auto& cell : unknown_cells) {
          bool is_safe = true;
          
          // Check all neighbors of this unknown cell
          for (int k = 0; k < 8; k++) {
            int ni = cell.first + dx[k];
            int nj = cell.second + dy[k];
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && 
                current_map[ni][nj] >= '0' && current_map[ni][nj] <= '8') {
              int neighbor_mine_count = current_map[ni][nj] - '0';
              int neighbor_marked = 0;
              int neighbor_unknown = 0;
              
              for (int m = 0; m < 8; m++) {
                int nni = ni + dx[m];
                int nnj = nj + dy[m];
                if (nni >= 0 && nni < rows && nnj >= 0 && nnj < columns) {
                  if (current_map[nni][nnj] == '@') {
                    neighbor_marked++;
                  } else if (current_map[nni][nnj] == '?') {
                    neighbor_unknown++;
                  }
                }
              }
              
              // If this cell being a mine would exceed the neighbor's mine count, it's safe
              if (neighbor_marked >= neighbor_mine_count) {
                continue;  // This neighbor says it's safe
              }
              
              // If this is the only unknown cell and one more mine is needed, it's a mine
              if (neighbor_unknown == 1 && neighbor_marked + 1 == neighbor_mine_count) {
                is_safe = false;
                break;
              }
            }
          }
          
          if (is_safe) {
            Execute(cell.first, cell.second, 0);  // Visit safe cell
            return;
          }
        }
      }
    }
  }
  
  // Strategy 5: If no safe move found, pick the first unknown cell (fallback)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (current_map[i][j] == '?') {
        Execute(i, j, 0);  // Visit
        return;
      }
    }
  }
}

#endif