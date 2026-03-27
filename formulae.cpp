#include <SFML/Graphics.hpp>
#include <thread>
#include <math.h>

using namespace sf;

char* map;
unsigned int map_size_x;
unsigned int map_size_y;
unsigned char colour_threshold_local = 10;

float do_line(Image *image, Vector2u from, float angle ){

    Vector2f driving_vector(sin(angle), -cos(angle)); // normalised
    float cum_dist = 0.0f;
    //printf("Vector: %3f, %3f.", driving_vector.x, driving_vector.y);

    Vector2u current_pos = from;
    Vector2f rel_pos(0.5, 0.5);

    for(unsigned int i = 0; i < 100000; i++){
        Color current_colour = image->getPixel(current_pos.x, current_pos.y);
        float current_brightness = current_colour.r/3 + current_colour.g/3 + current_colour.b / 3;
        if (current_brightness < colour_threshold_local){
            return cum_dist; // if hit end of track.
        }
        
        map[current_pos.x + current_pos.y * map_size_x] = 1; // update map with new good pixel. and then calculate next picel to check
        
        float distance_x = driving_vector.x >= 0 ? 1.0f - rel_pos.x : -rel_pos.x;
        float proportion_x = distance_x/driving_vector.x;

        float distance_y = driving_vector.y >= 0 ? 1.0f - rel_pos.y : -rel_pos.y;
        float proportion_y = distance_y/driving_vector.y;
        
        if(proportion_x < proportion_y){ // will get to x border sooner
            rel_pos += driving_vector * proportion_x * 1.0f;
            cum_dist += proportion_x;
        }else{
            rel_pos += driving_vector * proportion_y * 1.0f;
            cum_dist += proportion_y;
        }
        if (rel_pos.x >= 1.0f){current_pos.x += 1; rel_pos.x -= 1.0f;} // loop around relative positions
        else if (rel_pos.x <= 0.0f){current_pos.x -= 1; rel_pos.x += 1.0f;}
        if (rel_pos.y >= 1.0f){current_pos.y += 1; rel_pos.y -= 1.0f;}
        else if (rel_pos.y <= 0.0f){current_pos.y -= 1; rel_pos.y += 1.0f;}
        
        if(current_pos.x < 0){return cum_dist;} // finsih if hit edge
        else if (current_pos.x >= map_size_x){return cum_dist;}
        if(current_pos.y < 0){return cum_dist;}
        else if (current_pos.y >= map_size_y){return cum_dist;}
    }
    return -0.0f; // Should never reach here.
}

void do_lines(Image *image, Vector2u from, float angle_from, float angle_to, float increment, float* distances){
    unsigned long int lines_to_draw = (int)(abs(angle_to - angle_from) / increment);
    float iterating_sign = angle_from < angle_to ? 1.0f : -1.0f;
    unsigned long int i = 0;
    for (i = 0; i<lines_to_draw; i++){
        distances[i] = do_line(image, from, angle_from + (float)i*increment * iterating_sign);

        printf("%.1f%% Done! Current Line length = %.1f\r", 100.0f * (float)i / (float)lines_to_draw, distances[i]); // progress
    }
    distances[lines_to_draw] = -100.0f; // End marker
}

void paint_image(Image *image, Color colour){
    for(unsigned int y = 0; y < map_size_y; y++){
        for(unsigned int x = 0; x<map_size_x; x++){
            if(map[map_size_x*y + x] != 0){
                Color local_colour = image->getPixel(x, y);
                Color new_colour(
                    (char)((float)local_colour.r * (float)colour.r  * (1.0f/(256.0f))),
                    (char)((float)local_colour.g * (float)colour.g  * (1.0f/(256.0f))),
                    (char)((float)local_colour.b * (float)colour.b  * (1.0f/(256.0f)))
                );
                
                image->setPixel(x,y, new_colour); // paint if map is set!
            }
        }
    }
}

float* draw_sight(Image *image, int from_x, int from_y, float angle_from, float angle_to, float density, Color colour, unsigned char colour_thresh){
    map_size_x = image->getSize().x;
    map_size_y = image->getSize().y;
    map = (char*)malloc(map_size_x * map_size_y);
    memset(map, 0, map_size_x* map_size_y);

    int number_distances = (int)(abs(angle_to - angle_from) / density);
    float* distances = (float*)malloc((number_distances+2) * sizeof(float));

    colour_threshold_local = colour_thresh;

    do_lines(image, Vector2u(from_x, from_y), angle_from, angle_to, density, distances);
    printf("\n..."); // no \n in loading code.
    paint_image(image, colour);

    free(map);

    printf("Finished!\n");
    return distances;
}