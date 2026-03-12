#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string.h>

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




int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Texture texture;
    if (argc == 2){
        load_image(argv[1]);
    }

    while (true)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Texture texture;
        texture.loadFromImage(image);
        sf::Sprite sprite;
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        //window.draw(shape);
        window.display();


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
            printf("help\nexit\n");

        }else if(!strcmp(operation, "load")){
            load_image(operand);
        }else if(!strcmp(operation, "redraw")){
            
        }else{
            printf("Invalid command: %s\n", operation);
            printf("Write help for a list of commands.\n");
        }


    }

}