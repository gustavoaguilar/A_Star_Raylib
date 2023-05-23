#include "raylib.h"
#include "list.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define TILE_SIZE (32)

#define WINDOW_TITLE "A* - raylib"

typedef struct t_A_Start_Node{
    struct t_A_Start_Node* parent;

    float x;
    float y;

    int g;
    int h;
    int f;
}t_A_Start_Node;

int map[25][14] = {0};

t_A_Start_Node* astar_create_node(float x, float y, int g, int h){
    t_A_Start_Node* node = (t_A_Start_Node*) malloc(sizeof(t_A_Start_Node));
    
    if(node == NULL){
        return NULL;
    }

    node->x = x;
    node->y = y;
    node->g = g;
    node->h = h;
    node->f = 8*g + 10*h;

    return node;
}
// Check if an A_Star content is in list, return the index
int astar_check_if_in_list(t_List* list, float x, float y){
    for(int i = 0; i < list->size; i++){

        t_A_Start_Node* temp_node = (t_A_Start_Node*) list_get(list, i);
        if(temp_node == NULL){
            return 0;
        }
        if(temp_node->x == x && temp_node->y == y){
            return i;
        }
    }

    return 0;
}

// Distance between two points, as we do not use diagonals just use the coord diff
float astar_distance_between(float start_x, float start_y, float end_x, float end_y){
    return (abs(start_x - end_x) + abs(start_y - end_y));
}

// Compare the coordinates from the node to the map, if a wall, return 1
int check_if_wall(float x, float y){
    int x_to_int = (int) x;
    int y_to_int = (int) y;

    return map[x_to_int][y_to_int];
}

// Run the A* algorithm
t_List* astar(Vector2 start, Vector2 target){
    // Create the lists
    t_List* open_list = list_create();
    t_List* closed_list = list_create();
    t_List* path_list = list_create();
    
    // Debug variable to see the time to run A*
    double run_time = GetTime();

    // Create the start and end nodes
    t_A_Start_Node* start_node = astar_create_node(start.x, start.y, 0, 0);
    t_A_Start_Node* target_node = astar_create_node(target.x, target.y, 0, 0);

    // Add the start node to the open list
    list_push(open_list, start_node);

    // While open list with nodes, loop
    while(open_list->size > 0){

        // Get the node with the least score
        int index = 0;
        int min_score = __INT_MAX__;

        for(int i = 0; i < open_list->size; i++){
            t_A_Start_Node* temp_node = (t_A_Start_Node*) list_get(open_list, i);

            if(temp_node->f <= min_score){
                min_score = temp_node->f;
                index = i;
            }
        }

        //Node to be analised
        t_A_Start_Node* current_node = (t_A_Start_Node*) list_get(open_list, index);
        // Remove from open and push to closed, so we don't analyze this node again
        list_remove(open_list, current_node);
        list_push(closed_list, current_node);

        // Check if the current node is the target
        if(current_node->x == target.x && current_node->y == target.y){
            // Put the target on the closed list, so we can analyze it backwards and find the path
            target_node->parent = current_node;
            list_push(closed_list, target_node);
            
            // Iterate the list, going from the nodes parents until we find the start node
            t_A_Start_Node* node;
            node = target_node;
            for(int i = 0; i < closed_list->size; i++){
                
                // find the next parent node
                int next_index = list_find(closed_list, node->parent);
                node = (t_A_Start_Node*) list_get(closed_list, next_index);

                // Add the node to the path
                t_A_Start_Node* path_node = astar_create_node(node->x, node->y, node->g, node->h);
                list_push(path_list, path_node);

                // if the distance to the start node is zero, we finalized our path
                if(!astar_distance_between(node->x, node->y, start_node->x, start_node->y)){
                    // clear the lists and return the path list
                    list_erase(open_list);
                    list_erase(closed_list);

                    run_time = GetTime() - run_time;
                    TraceLog(LOG_INFO, TextFormat("Elapsed time: %f seconds", run_time));

                    return path_list;
                }
            }
        }

        //Generate children! We are not using diagonals in this example
        Vector2 next_position[4] = {{current_node->x+1, current_node->y}, {current_node->x-1, current_node->y}, {current_node->x, current_node->y+1}, {current_node->x, current_node->y-1}};
        
        for(int i = 0; i < 4; i++){
            // check if this node is already analyzed of if its a wall
            if(!astar_check_if_in_list(closed_list, next_position[i].x, next_position[i].y) && !check_if_wall(next_position[i].x, next_position[i].y)){
                // Create the node
                t_A_Start_Node* child = astar_create_node(next_position[i].x, next_position[i].y, 0, 0);
                child->g = astar_distance_between(child->x, child->y, start.x, start.y);
                child->h = astar_distance_between(child->x, child->y, target.x, target.y);
                child->f = 8*child->g + 10*child->h;

                // Check if the node exist in the open list
                int index = astar_check_if_in_list(open_list, child->x, child->y);
                if(index > 0){
                    // if yes, update it's score and remove the created child (because they are in the same position)
                    t_A_Start_Node* old_node = (t_A_Start_Node*) list_get(open_list, index);
                    if(old_node != NULL){
                        if(child->g <= old_node->g){
                            old_node->g = child->g;
                            old_node->parent = current_node;
                            free(child);
                        }
                    }
                }else{
                    // If not, just add the new child to the open list
                    child->parent = current_node;
                    list_push(open_list, child);
                }
            }
        }
    }
    // If we got in here, the A* couldn't fin the path, clear everything
    list_erase(open_list);
    list_erase(closed_list);
    list_erase(path_list);
    return NULL;
}

int main(void){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Vector2 start_point = {3.0, 3.0};
    Vector2 target_point = {15.0, 7.0};
    t_List* path_list = astar(start_point, target_point);// = astar(start_point, target_point);

    while (!WindowShouldClose()){

        if(IsKeyPressed(KEY_UP)){
            target_point.y-=1;
        }
        if(IsKeyPressed(KEY_DOWN)){
            target_point.y+=1;
        }
        if(IsKeyPressed(KEY_RIGHT)){
            target_point.x+=1;
        }
        if(IsKeyPressed(KEY_LEFT)){
            target_point.x-=1;
        }
        if(IsKeyPressed(KEY_SPACE)){
            if(path_list != NULL){
                free(path_list);
                path_list = NULL;
            }
            path_list = astar(start_point, target_point);
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            int x = GetMouseX()/32;
            int y = GetMouseY()/32;
            map[x][y] = 1;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            int x = GetMouseX()/32;
            int y = GetMouseY()/32;
            map[x][y] = 0;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);
            for(int i = 0; i*TILE_SIZE < SCREEN_HEIGHT; i++){
                DrawLine(0,i*TILE_SIZE,SCREEN_WIDTH,i*TILE_SIZE,BLACK);
            }
            for(int i = 0; i*TILE_SIZE < SCREEN_WIDTH; i++){
                DrawLine(i*TILE_SIZE, 0, i*TILE_SIZE, SCREEN_HEIGHT,BLACK);
            }

            if(path_list != NULL){
                for(int i = 0; i < path_list->size; i++){
                    t_A_Start_Node* node = (t_A_Start_Node*) list_get(path_list, i);
                    DrawRectangle(node->x*32, node->y*32, TILE_SIZE, TILE_SIZE, YELLOW);
                }
            }
            
            DrawRectangle(start_point.x*TILE_SIZE, start_point.y*TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
            DrawRectangle(target_point.x*TILE_SIZE, target_point.y*TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
            
            for(int i = 0; i < 25; i++){
                for(int j = 0; j < 14; j++){
                    switch(map[i][j]){
                    case 0:
                        // DrawRectangle(i*32, j*32, 32, 32, GRAY);
                        break;
                    case 1:
                        DrawRectangle(i*32, j*32, 32, 32, BLACK);
                        break;
                    }
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
