#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string.h>

#define HEIGHT 1000
#define WIDTH 1000

sf::Image image;
int image_size_x = 1;
int image_size_y = 1;


int load_image(char* filepath){
    if (!image.loadFromFile(filepath))
    {
        printf("Failed to load image. Usage: ./cmd filepath\n");
        return 100;
    }

    image_size_x = image.getSize().x;
    image_size_y = image.getSize().y;
    printf("Successfully loaded image with resolution %dx%dpx\n", image_size_x, image_size_y);

    return 0;
}

void set_sprite_scale(sf::Sprite* sprite){
    float h_ratio = (float)WIDTH/(float)image_size_x;
    float w_ratio = (float)HEIGHT/(float)image_size_y;
    float ratio = h_ratio < w_ratio ? h_ratio : w_ratio; // picks smaller ratio of the two
    sprite->setScale(sf::Vector2f(ratio, ratio));
}



int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");

    sf::Texture texture;
    if (argc == 2){
        load_image(argv[1]);
    }

    int target_x = 0;
    int target_y = 0;
    sf::CircleShape target(4.0f);
    target.setFillColor(sf::Color::Magenta);
    target.setOrigin(sf::Vector2f(2.0f, 2.0f));

    while (true){


        sf::Texture texture;
        texture.loadFromImage(image);
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
            printf("help\nexit\n\
            sx    Set target x\
            sy    Set target y\
            nx    Nudge target x\
            ny    Nudge target y\
            ");

        }else if(!strcmp(operation, "load")){
            load_image(operand);
        }else if(!strcmp(operation, "redraw")){
        
        }else if(!strcmp(operation, "sx")){
            target_x = atoi(operand);
            target.setPosition(sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "sy")){
            target_y = atoi(operand);
            target.setPosition(sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "nx")){
            target_x += atoi(operand);
            target.setPosition(sf::Vector2f(target_x, target_y));
        }else if(!strcmp(operation, "ny")){
            target_y += atoi(operand);
            target.setPosition(sf::Vector2f(target_x, target_y));
        
        }else{
            printf("Invalid command: %s\n", operation);
            printf("Write help for a list of commands.\n");
        }


    }

}