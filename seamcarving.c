#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "c_img.h"

void calc_energy(struct rgb_img *im, struct rgb_img **grad) {
    create_img(grad, im->height, im->width);

    //going through every pixel
    for (int i = 0; i < im->height; i++) {
        for (int j= 0; j < im->width; j++) {
            //getting the rgb of every pixel (for the y coordinate)
            int r_y, g_y, b_y, r_x, g_x, b_x;
            if (i == 0) {
                //edge cases (wrap around image)
                r_y = get_pixel(im, 1, j, 0) - get_pixel(im, im -> height - 1, j, 0);
                g_y = get_pixel(im, 1, j, 1) - get_pixel(im, im->height - 1, j, 1);
                b_y = get_pixel(im, 1, j, 2) - get_pixel(im, im -> height - 1, j, 2);
            } else if (i == im ->height - 1) {
                r_y = get_pixel(im, 0, j, 0) - get_pixel(im, im ->height - 2, j, 0);
                g_y = get_pixel(im, 0, j, 1) - get_pixel(im, im -> height - 2, j, 1);
                b_y = get_pixel(im, 0, j, 2) - get_pixel(im, im->height- 2 ,j ,2);
            } else {
                r_y = get_pixel(im, i+1, j, 0) - get_pixel(im, i - 1, j, 0);
                g_y = get_pixel(im, i+1, j, 1) - get_pixel(im, i - 1, j, 1);
                b_y = get_pixel(im, i+1, j, 2) - get_pixel(im, i - 1, j, 2);
            }

            //getting the rgb values for the x coordinates
            if (j == 0) {
                r_x = get_pixel(im, i, 1, 0) - get_pixel(im, i, im -> width - 1, 0);
                g_x = get_pixel(im, i, 1, 1) - get_pixel(im, i, im-> width - 1, 1);
                b_x = get_pixel(im, i, 1, 2) - get_pixel(im, i, im -> width - 1, 2);
            } else if (j == im -> width - 1) {
                r_x = get_pixel(im, i, 0, 0) - get_pixel(im, i, im -> width - 2, 0);
                g_x = get_pixel(im, i, 0, 1) - get_pixel(im, i, im -> width - 2, 1);
                b_x = get_pixel(im, i, 0, 2) - get_pixel(im, i, im-> width - 2 ,2);
            } else {
                r_x = get_pixel(im, i, j+1, 0) - get_pixel(im, i, j-1, 0);
                g_x = get_pixel(im, i, j+1, 1) - get_pixel(im, i, j-1, 1);
                b_x = get_pixel(im, i, j+1, 2) - get_pixel(im, i, j-1, 2);
            }

            float grad_energy = sqrtf(r_y*r_y + g_y*g_y + b_y*b_y + r_x*r_x + g_x*g_x + b_x*b_x);
            uint8_t dual_grad_ener = (uint8_t)(grad_energy/10);

            //set the dual gradient energy
            set_pixel(*grad, i, j, dual_grad_ener, dual_grad_ener, dual_grad_ener);
            
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr) {
    *best_arr = malloc(sizeof(double)* (grad->height) * (grad->width));

    for (int i = 0; i < grad->width; i++) {
        (*best_arr)[i] = get_pixel(grad, 0 , i ,0);
    }

    double temp1, temp2, temp3, mintemp;
    for (int i = 1; i < grad -> height; i++) {
        for (int j = 0; j < grad ->width; j++) {
            temp1 = (*best_arr)[(i-1)*grad->width + j];

            if (j == 0) {
                temp2 = temp1;     
            } else {
                temp2 = (*best_arr)[(i-1)*grad->width + j - 1];
            }

            if (j == grad->width -1) {
                temp3 = temp1;
            } else {
                temp3 = (*best_arr)[(i-1)*grad->width + j + 1];
            }

            if (temp1 < temp2) {
                if (temp1 < temp3) {
                    mintemp = temp1;
                } else {
                    mintemp = temp3;
                }
            } else {
                if (temp2 < temp3) {
                    mintemp = temp2;
                } else {
                    mintemp = temp3;
                }
            }

            (*best_arr)[i*(grad->width) + j] = get_pixel(grad, i, j, 0) + mintemp;

        }
    }
    
}

void recover_path(double *best, int height, int width, int **path) {
    *path = (int *)malloc(height * sizeof(int));

    int min_col = 0;
    double min_val = best[(height - 1) * width];

    for (int j = 1; j < width; j++) {
        double val = best[(height - 1) * width + j];
        if (val < min_val) {
            min_val = val;
            min_col = j;
        }
    }

    (*path)[height - 1] = min_col;

    for (int i = height - 2; i >= 0; i--) {
        int prev_col = (*path)[i + 1];
        int best_col = prev_col;
        double best_val = best[i * width + prev_col];

        if (prev_col > 0 && best[i * width + prev_col - 1] < best_val) {
            best_val = best[i * width + prev_col - 1];
            best_col = prev_col - 1;
        }

        if (prev_col < width - 1 && best[i * width + prev_col + 1] < best_val) {
            best_val = best[i * width + prev_col + 1];
            best_col = prev_col + 1;
        }

        (*path)[i] = best_col;
    }
}


void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path) {
    create_img(dest, src ->height, (src->width) - 1);

    for (int i = 0; i < src ->height; i++) {
        for (int j = 0; j < src ->width; j++) {
            if (j > path[i]) {
                set_pixel(*dest, i, j - 1, get_pixel(src, i, j, 0), get_pixel(src, i, j, 1), get_pixel(src, i, j ,2));
            } else if(j < path[i]) {
                set_pixel(*dest, i, j, get_pixel(src, i, j, 0), get_pixel(src, i, j, 1), get_pixel(src, i, j ,2));
            }
        }
    }
}

