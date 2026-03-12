#include <SFML/Graphics.hpp>
#include <thread>
#include <math.h>

using namespace sf;

float colour_threshold = 0.1;
Color colour(0.5, 1.0, 1.0);
char* map;
unsigned int map_size_x;
unsigned int map_size_y;

void do_line(Image *image, Vector2u from, float angle ){

    Vector2f driving_vector(sin(angle), cos(angle));

    Vector2u current_pos = from;
    Vector2f rel_pos(0.5, 0.5);

    for(int i = 0; i < 80; i++){
        Color current_colour = image->getPixel(current_pos.x, current_pos.y);
        float current_brightness = (current_colour.r + current_colour.g + current_colour.b) / 3.0f;
        if (current_brightness < colour_threshold){
            return; // if hit end of track.
        }
        
        map[current_pos.x + current_pos.y * map_size_x] = 1; // update map with new good pixel. and then calculate next picel to check
        
        float distance_x = driving_vector.x >= 0 ? 1.0f - rel_pos.x : rel_pos.x;
        float proportion_x = distance_x/driving_vector.x;

        float distance_y = driving_vector.y >= 0 ? 1.0f - rel_pos.y : rel_pos.y;
        float proportion_y = distance_y/driving_vector.y;
        
        if(proportion_x < proportion_y){ // will get to x border sooner
            rel_pos += driving_vector * proportion_x * 1.001f;
        }else{
            rel_pos += driving_vector * proportion_y * 1.001f;
        }
        if (rel_pos.x > 1.0f){current_pos.x += 1; rel_pos.x -= 1.0f;} // loop around relative positions
        else if (rel_pos.x < 0.0f){current_pos.x -= 1; rel_pos.x += 1.0f;}
        if (rel_pos.y > 1.0f){current_pos.y += 1; rel_pos.y -= 1.0f;}
        else if (rel_pos.y < 0.0f){current_pos.y -= 1; rel_pos.y += 1.0f;}
        
        if(current_pos.x < 0){return;} // finsih if hit edge
        else if (current_pos.x >= map_size_x){return;}
        if(current_pos.y < 0){return;}
        else if (current_pos.y >= map_size_y){return;}
    }

    return;
}

void do_lines(Image *image, Vector2u from, float angle_from, float angle_to, float increment){
    unsigned long int lines_to_draw = (int)(abs(angle_to - angle_from) / increment);
    for (unsigned long int i = 0; i<lines_to_draw; i++){
        do_line(image, from, angle_from + (float)i*increment);

        printf("%4f %%Done!\n", 100.0f * (float)i / (float)lines_to_draw);
    }
}

void paint_image(Image *image){
    for(unsigned int y = 0; y < map_size_y; y++){
        for(unsigned int x = 0; x<map_size_x; x++){
            if(map[map_size_x*y + x] != 0){
                Color local_colour = image->getPixel(x, y);
                Color new_colour = Color(local_colour.r * colour.r, local_colour.g * colour.g, local_colour.b * colour.b);
                image->setPixel(x,y, new_colour); // paint if map is set!
            }
        }
    }
}

void draw_sight(Image *image, int from_x, int from_y, float angle_from, float angle_to){
    map_size_x = image->getSize().x;
    map_size_y = image->getSize().y;
    map = (char*)malloc(map_size_x * map_size_y);
    memset(map, 0, map_size_x* map_size_y);

    do_lines(image, Vector2u(from_x, from_y), angle_from, angle_to, 0.1);
    paint_image(image);

    free(map);
}