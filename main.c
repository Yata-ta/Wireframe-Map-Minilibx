#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <mlx.h>
#include <fcntl.h>
#include "get_next_line/get_next_line.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 720

#define MLX_ERROR 1

#define RED_PIXEL 0xFF0000
#define GREEN_PIXEL 0xFF00
#define WHITE_PIXEL 0xFFFFFF


typedef struct {
    int elevation;
    int color;
} Point;






typedef struct s_img{
    void *mlx_img;
    char *addr;
    int bpp; /* bits per pixel */
    int line_len;
    int endian;
} t_img;

typedef struct s_data{
    void *mlx_ptr;
    void *win_ptr;
    t_img img;
    int cur_img;
} t_data;

typedef struct s_rect{
    int x;
    int y;
    int width;
    int height;
    int color;
} t_rect;



// Aux Functions
/***********************************************************************************************************/

int hex_to_int(char *hex) {
    int result = 0;
    while (*hex) {
        char c = *hex++;
        int value = 0;
        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            value = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            value = 10 + (c - 'A');
        }
        result = (result << 4) | value;
    }
    return result;
}

int	ft_count_words(const char *str, char sep){
    int	count;

    count = 0;
    if (!*str)
        return (count);
    while (*str == sep)
        str++;
    if (*str)
        count = 1;
    while (*str)
    {
        if (*str == sep && *(str + 1) != 0 && *(str + 1) != sep)
            count++;
        str++;
    }

    return (count);
}




// MLX Base Functions
/***********************************************************************************************************/

void img_pix_put(t_img *img, int x, int y, int color){
    char *pixel;
    int i;

    i = img->bpp - 8;
    pixel = img->addr + (y * img->line_len + x * (img->bpp / 8));
    while (i >= 0)
    {
        /* big endian, MSB is the leftmost bit */
        if (img->endian != 0)
            *pixel++ = (color >> i) & 0xFF;
        /* little endian, LSB is the leftmost bit */
        else
            *pixel++ = (color >> (img->bpp - 8 - i)) & 0xFF;
        i -= 8;
    }
}


int get_points() {

    int file = open("test_maps/elem-col.fdf", O_RDONLY);
    if (file == -1) {
        printf("Unable to open file");
        return EXIT_FAILURE;
    }

    int buffer_size = 1024;
    char buffer[buffer_size];
    ssize_t bytes_read;
    int rows = 0, cols = 0;
    int temp_cols = 0;
    int is_first_line = 1;
    int in_word = 0; // Flag to check if we are in a word

    // Read file and determine the number of rows and columns
    while ((bytes_read = read(file, buffer, buffer_size)) > 0) {
        int i = 0;
        while (i < bytes_read) {
            if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n') {
                if (!in_word) {
                    temp_cols++;
                    in_word = 1;
                }
            } else {
                if (buffer[i] == '\n') {
                    rows++;
                    if (is_first_line) {
                        cols = temp_cols;
                        is_first_line = 0;
                    }
                    temp_cols = 0;
                }
                in_word = 0;
            }
            i++;
        }
    }

    if (bytes_read == -1) {
        printf("Failed to read data");
    }


    printf("Rows: %d, Cols: %d\n", rows, cols);




    // Allocate memory for the 2D array
    Point **array = malloc(rows * sizeof(Point *));
    if (!array) {
        printf("Failed to allocate memory");
    }
    for (int i = 0; i < rows; i++) {
        array[i] = malloc(cols * sizeof(Point));
        if (!array[i]) {
            printf("Failed to allocate memory");
        }
    }

    if (lseek(file, 0, SEEK_SET) == -1) {
        printf("Failed to rewind file");
    }

    // Read the file data into the array
    int row = 0, col = 0;
    in_word = 0;
    char number_buffer[20];
    int num_idx = 0;
    char color_buffer[20];
    int color_idx = 0;
    int color;

    while ((bytes_read = read(file, buffer, BUFFER_SIZE)) > 0) {
        int i = 0;
        while (i < bytes_read) {
            
            if (buffer[i] != ' ' && buffer[i] != '\n') {

                if (buffer[i] == ',') {
                    number_buffer[num_idx] = '\0';
                    array[row][col].elevation = atoi(number_buffer);
                    num_idx = 0;
                    in_word = 0;

                    i++; 
                    while (i < bytes_read && buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n') {
                        color_buffer[color_idx++] = buffer[i++];
                    }

                    color_buffer[color_idx] = '\0';
                    color = hex_to_int(color_buffer);

                    array[row][col].color = color;
                    color_idx = 0;
                } else {
                    number_buffer[num_idx++] = buffer[i];
                    in_word = 1;
                }
            } else {
                if (in_word) {
                    number_buffer[num_idx] = '\0';
                    array[row][col].elevation = atoi(number_buffer);
                    array[row][col].color = -1; // Default color value
                    num_idx = 0;
                    col++;
                }
                if (buffer[i] == '\n') {
                    row++;
                    col = 0;
                }
                in_word = 0;
            }
            i++;
        }
    }


    
    if (bytes_read == -1) {
        printf("Failed to read data");
    }


    if (close(file) == -1) {
        printf("Failed to close file");
    }



    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (array[i][j].color == -1) {
                printf("%d ", array[i][j].elevation);
            } else {
                printf("%d,0x%06X ", array[i][j].elevation, array[i][j].color);
            }
        }
        printf("\n");
    }




    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);


    return 0;
}




// Handles
/***********************************************************************************************************/

int handle_destroy_notify(t_data *data){
    mlx_destroy_window(data->mlx_ptr, data->win_ptr);
    data->win_ptr = NULL;
    return (0);
}


int handle_keypress(int keysym, t_data *data){
    if (keysym == XK_Escape)
    {
        mlx_destroy_window(data->mlx_ptr, data->win_ptr);
        data->win_ptr = NULL;
    }
    return (0);
}




// Renders
/***********************************************************************************************************/

void draw_line(t_img *img, int x1, int y1, int x2, int y2, int color){
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1){
        img_pix_put(img, x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y1 += sy;
        }
    }
}

int render_rect(t_img *img, t_rect rect){
    int i;
    int j;

    i = rect.y;
    while (i < rect.y + rect.height)
    {
        j = rect.x;
        while (j < rect.x + rect.width)
            img_pix_put(img, j++, i, rect.color);
        ++i;
    }
    return (0);
}

void render_background(t_img *img, int color){
    int i;
    int j;

    i = 0;
    while (i < WINDOW_HEIGHT)
    {
        j = 0;
        while (j < WINDOW_WIDTH)
        {
            img_pix_put(img, j++, i, color);
        }
        ++i;
    }
}


int render(t_data *data){
    if (data->win_ptr == NULL)
        return (1);

       

    render_background(&data->img, WHITE_PIXEL);
    //render_rect(&data->img, (t_rect){WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, 100, 100, GREEN_PIXEL});
    //render_rect(&data->img, (t_rect){0, 0, 100, 100, RED_PIXEL});

    draw_line(&data->img, 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, 0x0000FF); // Blue line

    mlx_put_image_to_window(data->mlx_ptr, data->win_ptr, data->img.mlx_img, 0, 0);

    return (0);
}





int main(void){

    get_points();

    t_data data;

    data.mlx_ptr = mlx_init();
    if (data.mlx_ptr == NULL)
        return (MLX_ERROR);

    
    data.win_ptr = mlx_new_window(data.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "my window");
    
    if (data.win_ptr == NULL){
        // free(data.win_ptr); 
        return (MLX_ERROR);
    }

    /* Setup hooks */
    data.img.mlx_img = mlx_new_image(data.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT);

    data.img.addr = mlx_get_data_addr(data.img.mlx_img, &data.img.bpp,
                                      &data.img.line_len, &data.img.endian);

    mlx_hook(data.win_ptr, DestroyNotify, StructureNotifyMask, &handle_destroy_notify, &data);
    mlx_loop_hook(data.mlx_ptr, &render, &data);
    mlx_hook(data.win_ptr, KeyPress, KeyPressMask, &handle_keypress, &data);

    mlx_loop(data.mlx_ptr);

    /* we will exit the loop if there's no window left, and execute this code */
    //mlx_destroy_image(data.mlx_ptr, data.img.mlx_img);
    //mlx_destroy_display(data.mlx_ptr);
    // free(data.mlx_ptr); // Remove this line
}
