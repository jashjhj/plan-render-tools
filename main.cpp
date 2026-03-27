#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string.h>

#include "formulae.cpp"
#include "render.h"

#define HEIGHT 1000
#define WIDTH 1000
#define PI 3.14156

sf::Image image;
int image_size_x = 1;
int image_size_y = 1;

Color colour(255, 126, 100);

sf::Image prev_image;
char last_load[128];

unsigned char colour_threshold = 10;
///Measured in the increment in rads fromn one line to the next.
float draw_density = 0.0002f;
float start_angle = 0;
float end_angle = 2*PI;

float* distance_map;

int load_image(char* filepath){
    if (!image.loadFromFile(filepath))
    {
        return 100;
    }

    image_size_x = image.getSize().x;
    image_size_y = image.getSize().y;
    printf("Successfully loaded image with resolution %dx%dpx\n", image_size_x, image_size_y);

    strcpy(last_load, filepath);
    return 0;
}

float image_scale;
void set_sprite_scale(sf::Sprite* sprite){
    float h_ratio = (float)WIDTH/(float)image_size_x;
    float w_ratio = (float)HEIGHT/(float)image_size_y;
    float ratio = h_ratio < w_ratio ? h_ratio : w_ratio; // picks smaller ratio of the two
    image_scale = ratio;
    sprite->setScale(sf::Vector2f(ratio, ratio));
}



int main(int argc, char** argv){

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Scanline Computer");

    sf::Texture texture;
    if (argc == 2){
        load_image(argv[1]);
    }

    int target_x = 0;
    int target_y = 0;
    sf::CircleShape target(4.0f);
    target.setFillColor(colour);
    target.setOrigin(sf::Vector2f(2.0f, 2.0f));
    distance_map = (float*)malloc(sizeof(float));

    while (true){


        
        
        sf::Texture texture;
        
        if(!texture.loadFromImage(image)){
            printf("Load image using `load <filepath>`\n");
        };
        sf::Sprite sprite;
        sprite.setTexture(texture);
        set_sprite_scale(&sprite);

        window.clear();
        window.draw(sprite);
        window.draw(target);
        window.display();

        //////////////////////////////////////  PARSE INPUT  /////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////

        char input[128]; // parse input
        char operation[128];
        char operand[128];
        scanf(" %126[^\n]", input);
        char stage = 0; // stage is 0 until it finds where second part (operand) begins
        for(int i = 0; i<128; i++){
            if(stage == 0){
                if (input[i] == '\00'){
                    operation[i] = '\00';
                    operand[0] = '\00';
                    break;
                }else if (input[i] == ' '){
                    operation[i] = '\00';
                    stage = i+1;
                }else{operation[i] = input[i];}; // copy opearation
            }
            else{
                if (input[i] == '\00' || input[i] == ' '){
                    operand[i-stage] = '\00';
                    break;
                }else{operand[i-stage] = input[i];}
            }

        }
        
        if(!strcmp(operation, "exit")){
            window.close();
            return 0;
        }else if(!strcmp(operation, "help")){
            printf("\
load            <filepath>      Loads from given filepath. Accepts bmp, png, tga, jpg, gif, psd, hdr, pic and pnm. Jpg without arithmetic encoding.\n\
reload                          Reloads from last successfully loaded file, good for resetting.\n\
save            <filepath>      Saves to given filepath.\n\
saveperspective <filepath>  Saves a perspective template to given filepath.\n\
\n\
sx              <int>           Set target x\n\
sy              <int>           Set target y\n\
nx              <int>           Nudge target x\n\
ny              <int>           Nudge target y\n\
getpos                          Returns current target pos\n\
density         <float>         of drawing in scans per degree\n\
\n\
setstart        <angle>         Sets start angle. In Degrees, as a bearing from North.\n\
setend          <angle>         Sets end angle. Likewise.\n\
threshold       <int>           Sets low pass threshold that cancels the line. 0 is unstoppable, 255 is max. Default 10.\n\
\n\
r               <int>           Sets colour of drawing. Stored out of 255.\n\
g               <int>           Likewise\n\
b               <int>           Likewise\n\
getcolour                       Returns current colour\n\
\n\
draw                            Draw!\n\
sketch                          Draw at a low density without having to change programmed density.\n\
\n\
redraw                          Redraws window.\n\
exit\n\
\n");

        }else if(!strcmp(operation, "load")){
            
            load_image(operand);
        }else if(!strcmp(operation, "reload")){
            if(last_load[0] == '\00'){printf("No last load to load from. Please use load <filepath>\n");}
            else{load_image(last_load);}
        
        }else if(!strcmp(operation, "save")){
            printf("...");
            if( image.saveToFile(operand) ){
                printf("Successfully saved image to `%s`.\n", operand);
            }else{
                printf("Failed to save image.\n");
            }
        }else if(!strcmp(operation, "saveperspective")){
            printf("...");
            Image drawing = render::draw(distance_map, abs(end_angle-start_angle));
            if( drawing.saveToFile(operand) ){
                printf("Successfully saved perspective sketch to `%s`.\n", operand);
            }else{
                printf("Failed to save Perspective Sketch.\n");
            }

        
        }else if(!strcmp(operation, "undo")){

            image = prev_image;

        }else if(!strcmp(operation, "redraw")){
        
        }else if(!strcmp(operation, "draw")){
            prev_image = image; // prepare for undo;
            free(distance_map);
            distance_map = draw_sight(&image, target_x, target_y, start_angle, end_angle, draw_density, colour, colour_threshold);
        }else if(!strcmp(operation, "sketch")){
            prev_image = image;
            free(distance_map);
            distance_map = draw_sight(&image, target_x, target_y, start_angle, end_angle, 0.002, colour, colour_threshold);
        
        }else if(!strcmp(operation, "setstart")){
            start_angle = PI / 180.0f * atof(operand);
        }else if(!strcmp(operation, "setend")){
            end_angle = PI / 180.0f * atof(operand);
        
        }else if(!strcmp(operation, "threshold")){
            colour_threshold = atoi(operand);
        }else if(!strcmp(operation, "r")){
            colour.r = atoi(operand);
            target.setFillColor(colour);
            //printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        }else if(!strcmp(operation, "g")){
            colour.g = atoi(operand);
            target.setFillColor(colour);
            //printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        }else if(!strcmp(operation, "b")){
            colour.b = atoi(operand);
            target.setFillColor(colour);
            //printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        }else if(!strcmp(operation, "getcolour")){
            printf("Colour = %dr %dg %db.\n", colour.r, colour.g, colour.b);
        

        }else if(!strcmp(operation, "sx")){
            target_x = atoi(operand);
            target.setPosition(image_scale * sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "sy")){
            target_y = atoi(operand);
            target.setPosition(image_scale * sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "nx")){
            target_x += atoi(operand);
            target.setPosition(image_scale * sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "ny")){
            target_y += atoi(operand);
            target.setPosition(image_scale * sf::Vector2f(target_x, target_y));

        }else if(!strcmp(operation, "getpos")){
            printf("Target position: (%d, %d)\n", target_x, target_y);
        }else if(!strcmp(operation, "density")){
            draw_density = PI / (180 *float(atof(operand)));
            printf("Draw density set to %f scanlinesper degree.\n", atof(operand));

        }else{
            printf("Invalid command: %s\n", operation);
            printf("Write help for a list of commands.\n");
        }


    }

}