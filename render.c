#include "render.h"
#include "linear.h"

double edge_function(const Matrix* a, const Matrix* b, const Matrix* c) {
    return ((matrix_get(b, 0, 0) - matrix_get(a, 0, 0))
        * (matrix_get(c, 1, 0) - matrix_get(a, 1, 0)))
        - ((matrix_get(b, 1, 0) - matrix_get(a, 1, 0))
        * (matrix_get(c, 0, 0) - matrix_get(a, 0, 0)));
}

void draw_triangle(SDL_Renderer* renderer, Triangle* tri) { //  add z buffer? and somehow incorporate lighting..
    const Matrix* a = tri->vertices[0];
    const Matrix* b = tri->vertices[1];
    const Matrix* c = tri->vertices[2];
    
    for (int i = 0; i < 3; i++) {
        if (tri->colors[i] == NULL || 
            tri->colors[i]->rows != 3 || 
            tri->colors[i]->cols != 1) {
            fprintf(stderr, "Error: Invalid color matrix for triangle\n");
            return;
        }
    }
    
    const double abc = edge_function(a, b, c);
    
    if (abc > 0) {
        fprintf(stderr, "Warning: Triangle has counter-clockwise winding, skipping\n");
        return;
    }
    
    double minx = matrix_get(a, 0, 0);
    if (matrix_get(b, 0, 0) < minx) minx = matrix_get(b, 0, 0);
    if (matrix_get(c, 0, 0) < minx) minx = matrix_get(c, 0, 0);
    
    double maxx = matrix_get(a, 0, 0);
    if (matrix_get(b, 0, 0) > maxx) maxx = matrix_get(b, 0, 0);
    if (matrix_get(c, 0, 0) > maxx) maxx = matrix_get(c, 0, 0);
    
    double miny = matrix_get(a, 1, 0);
    if (matrix_get(b, 1, 0) < miny) miny = matrix_get(b, 1, 0);
    if (matrix_get(c, 1, 0) < miny) miny = matrix_get(c, 1, 0);
    
    double maxy = matrix_get(a, 1, 0);
    if (matrix_get(b, 1, 0) > maxy) maxy = matrix_get(b, 1, 0);
    if (matrix_get(c, 1, 0) > maxy) maxy = matrix_get(c, 1, 0);
    
    Uint8 oldr, oldg, oldb, olda;
    SDL_GetRenderDrawColor(renderer, &oldr, &oldg, &oldb, &olda);
    
    Matrix* p = matrix_new(2, 1);
    
    for (int y = miny; y < maxy; y++) {
        matrix_set(p, 1, 0, y);
        
        for (int x = minx; x < maxx; x++) {
            matrix_set(p, 0, 0, x);
            
            const double abp = edge_function(a, b, p);
            const double bcp = edge_function(b, c, p);
            const double cap = edge_function(c, a, p);
            
            if (abp <= 0 && bcp <= 0 && cap <= 0) {
                // barycentric coordinates
                const double bc_a = bcp / abc;
                const double bc_b = cap / abc;
                const double bc_c = abp / abc;
                
                const Uint8 r = (Uint8)(matrix_get(tri->colors[0], 0, 0) * bc_a
                    + matrix_get(tri->colors[1], 0, 0) * bc_b
                    + matrix_get(tri->colors[2], 0, 0) * bc_c);
                const Uint8 g = (Uint8)(matrix_get(tri->colors[0], 1, 0) * bc_a
                    + matrix_get(tri->colors[1], 1, 0) * bc_b
                    + matrix_get(tri->colors[2], 1, 0) * bc_c);
                const Uint8 b = (Uint8)(matrix_get(tri->colors[0], 2, 0) * bc_a
                    + matrix_get(tri->colors[1], 2, 0) * bc_b
                    + matrix_get(tri->colors[2], 2, 0) * bc_c);
                
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
    
    SDL_SetRenderDrawColor(renderer, oldr, oldg, oldb, olda);
    free_matrix(p);
}
