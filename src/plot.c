#include <tgmath.h>
#include <string.h>

#include "nvtop/plot.h"

static inline int data_level(double rows, double data, double increment) {
  return (int)(rows - floor(data / increment));
}

void nvtop_line_plot(WINDOW *win, size_t num_data, const double *data,
                     double min, double max, unsigned num_plots,
                     const char *legend[]) {
  if (num_data == 0)
    return;
  int rows, cols;
  getmaxyx(win, rows, cols);
  rows -= 1; // Maximum drawable character
  double increment = (max - min) / (double)(rows);
  int low_data_level = data_level(0, min, increment);
  for (size_t k = 0; k < num_plots; ++k) {
    int level_previous = data_level(rows, data[k], increment) - low_data_level;
    int level_next, level_current;
    wattron(win, COLOR_PAIR(1 + k % 5));
    for (size_t i = k; i < num_data || i < (size_t)cols; i += num_plots) {
      level_next = i + num_plots >= num_data
                       ? level_next
                       : data_level(rows, data[i + num_plots], increment) -
                             low_data_level;
      level_current = data_level(rows, data[i], increment) - low_data_level;
      int top, bottom;
      if (level_current == level_previous) {
        mvwaddch(win, level_current, i, ACS_HLINE);
        top = bottom = level_current;
      } else {
        if (level_current < level_previous) {
          top = level_previous;
          bottom = level_current;
        } else {
          top = level_current;
          bottom = level_previous;
        }
        for (int j = bottom + 1; j < top; j++) {
          mvwaddch(win, j, i, ACS_VLINE);
        }
        if (level_current > level_previous) {
          mvwaddch(win, level_current, i, ACS_LLCORNER);
          mvwaddch(win, level_previous, i, ACS_URCORNER);
        } else {
          mvwaddch(win, level_current, i, ACS_ULCORNER);
          mvwaddch(win, level_previous, i, ACS_LRCORNER);
        }
      }
      for (unsigned j = 0; j < num_plots; ++j) {
        if (j == k)
          continue;
        int cross_level = -low_data_level;
        if (j < k || i + j < num_plots + k)
          cross_level += data_level(rows, data[i - k + j], increment);
        else
          cross_level +=
              data_level(rows, data[i - k + j - num_plots], increment);
        if (cross_level == top && top == bottom)
          continue;

        if (cross_level > bottom && cross_level < top) {
          mvwaddch(win, cross_level, i, ACS_PLUS);
        } else {
          if (cross_level == top) {
            mvwaddch(win, cross_level, i, ACS_BTEE);
          } else {
            if (cross_level == bottom) {
              mvwaddch(win, cross_level, i, ACS_TTEE);
            } else {
              wattroff(win, COLOR_PAIR(1 + k % 5));
              wattron(win, COLOR_PAIR(1 + j % 5));
              mvwaddch(win, cross_level, i, ACS_HLINE);
              wattroff(win, COLOR_PAIR(1 + j % 5));
              wattron(win, COLOR_PAIR(1 + k % 5));
            }
          }
        }
      }
      level_previous = level_current;
    }
    wattroff(win, COLOR_PAIR(1 + k % 5));
  }
  int plot_y_position = 0;
  for (unsigned i = 0; i < num_plots && plot_y_position < rows; ++i) {
    if (legend[i]) {
      size_t length = strlen(legend[i]);
      wattron(win, COLOR_PAIR(1 + i % 5));
      if (length < (size_t)cols) {
        mvwprintw(win, plot_y_position, cols-length, "%s", legend[i]);
      } else {
        wmove(win, plot_y_position, 0);
        for (int j = 0; j < cols; ++j) {
          waddch(win, legend[i][j]);
        }
      }
      wattroff(win, COLOR_PAIR(1 + i % 5));
      plot_y_position++;
    }
  }
}

void nvtop_bar_plot(WINDOW *win, size_t num_data, const double *data,
                    double min, double max) {
  if (num_data == 0)
    return;
  int rows, cols;
  getmaxyx(win, rows, cols);
  rows -= 1;
  double increment = (max - min) / (double)(rows);
  int low_data_level = data_level(0, min, increment);
  for (size_t i = 0; i < num_data || i < (size_t)cols; ++i) {
    for (int j = data_level(rows, data[i], increment) - low_data_level;
         j <= rows; j++) {
      mvwaddch(win, j, i, ACS_CKBOARD);
    }
  }
}

void draw_rectangle(WINDOW *win, unsigned startX, unsigned startY, unsigned sizeX, unsigned sizeY) {
  for (unsigned i = 0; i < sizeX - 2; ++i) {
    mvwaddch(win, startY, startX + 1 + i, ACS_HLINE);
    mvwaddch(win, startY + sizeY - 1, startX + 1 + i, ACS_HLINE);
  }
  for (unsigned i = 0; i < sizeY - 2; ++i) {
    mvwaddch(win, startY + 1 + i, startX, ACS_VLINE);
    mvwaddch(win, startY + 1 + i, startX + sizeX - 1, ACS_VLINE);
  }
  mvwaddch(win, startY, startX, ACS_ULCORNER);
  mvwaddch(win, startY, startX + sizeX - 1, ACS_URCORNER);
  mvwaddch(win, startY + sizeY - 1, startX, ACS_LLCORNER);
  mvwaddch(win, startY + sizeY - 1, startX + sizeX - 1, ACS_LRCORNER);
}
