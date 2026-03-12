#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string.h>

#include "formulae.cpp"

#define HEIGHT 1000
#define WIDTH 1000
#define PI 3.14156

sf::Image image;
int image_size_x = 1;
int image_size_y = 1;

Color colour(255, 126, 100);

sf::Image prev_image;
char last_load[128];

float draw_density = 0.001f;


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



int main(int argc, char** argv)
{
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
help\n\
\n\
sx          Set target x\n\
sy          Set target y\n\
nx          Nudge target x\n\
ny          Nudge target y\n\
density     in scans per degree\n\
exit\n\
");

        }else if(!strcmp(operation, "load")){
            load_image(operand);
        }else if(!strcmp(operation, "reload")){
            if(last_load[0] == '\00'){printf("No last load to load from. Please use load <filepath>\n");}
            else{load_image(last_load);}
        
        }else if(!strcmp(operation, "save")){
            if( image.saveToFile(operand) ){
                printf("Successfully saved image to `%s`.\n", operand);
            }

        
        }else if(!strcmp(operation, "undo")){

            image = prev_image;

            printf("Undone!\n");

        }else if(!strcmp(operation, "redraw")){
        
        }else if(!strcmp(operation, "draw")){
            prev_image = image; // prepare for undo;
            draw_sight(&image, target_x, target_y, 0, PI*2.0f, draw_density, colour);
        
        }else if(!strcmp(operation, "r")){
            colour.r = atoi(operand);
            target.setFillColor(colour);
            printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        }else if(!strcmp(operation, "g")){
            colour.g = atoi(operand);
            target.setFillColor(colour);
            printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        }else if(!strcmp(operation, "b")){
            colour.b = atoi(operand);
            target.setFillColor(colour);
            printf("New colour set to rgb %d %d %d.\n", colour.r, colour.g, colour.b);

        

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
        }else if(!strcmp(operation, "density")){
            draw_density = PI / (180 *float(atof(operand)));
            printf("Draw density set to %f scanlinesper degree.\n", atof(operand));

        }else{
            printf("Invalid command: %s\n", operation);
            printf("Write help for a list of commands.\n");
        }


    }

}